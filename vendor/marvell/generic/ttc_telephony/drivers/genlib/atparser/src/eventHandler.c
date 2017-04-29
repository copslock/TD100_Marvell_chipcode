/*****************************************************************************
* Utility Library
*
* DESCRIPTION
*    An application's main event-dispatch loop.  Dispatches file-descriptor,
*    idle, and signal events.  All dispatched functions execute outside of
*    the context of raw UNIX/Linux signal handlers.
*
*    Signal handling is done via pipes.  The use of a pipe solves the problem
*    of re-entrency.  Signals are asynchronous and could thus occur at any
*    time.  If application signal handlers were called directly from the signal
*    handler, those handlers could not call functions that are not re-entrent
*    (i.e. `malloc()').  This is a serious restriction.  Careful signal blocking
*    and unblocking could work around this but is extremely error prone.  A pipe
*    allows an asynchronous signal to be turned into the synchronous arrival
*    of a character on a file descriptor.  Now application signal handlers can
*    call functions that are not re-entrant with no problems.  But this does
*    mean that a `read()' or `select()' must be hanging on the pipe to catch
*    signal characters, and that when such a character arrives that the signal
*    handler function is invoked.
*
* FUTURES
*   Q: How should one mix signals and threads in programs?
*   A: The less you mix them, the better. Notice that all pthread_* functions are
*      not async-signal safe, meaning that you should not call them from signal
*      handlers. This recommendation is not to be taken lightly: your program
*      can deadlock if you call a pthread_* function from a signal handler!
*
*      The only sensible things you can do from a signal handler is set a global
*      flag, or call sem_post on a semaphore, to record the delivery of the signal.
*      The remainder of the program can then either poll the global flag, or use
*      sem_wait() and sem_trywait() on the semaphore.
*
*      Another option is to do nothing in the signal handler, and dedicate one
*      thread (preferably the initial thread) to wait synchronously for signals,
*      using sigwait(), and send messages to the other threads accordingly.
*
*      Semaphores were added to POSIX not as part of the thread package but as a
*      separate set of interfaces. The interface allows easy use of semaphores
*      in different processes by creating named semaphore objects. At the same
*      time anonymous semaphores are available for the use in multi-threaded
*      applications.  Affected interfaces:
*          sem_close()
*          sem_destroy()
*          sem_getvalue()
*          sem_init()
*          sem_open()
*          sem_post()
*          sem_trywait()
*          sem_unlink()
*          sem_wait()
*
*   Q: When one thread is blocked in sigwait(), other threads no longer receive
*      the signals sigwait() is waiting for! What happens?
*   A: It's an unfortunate consequence of how LinuxThreads implements sigwait().
*      Basically, it installs signal handlers on all signals waited for, in
*      order to record which signal was received. Since signal handlers are
*      shared with the other threads, this temporarily deactivates any signal
*      handlers you might have previously installed on these signals.
*      Though surprising, this behavior actually seems to conform to the POSIX
*      standard. According to POSIX, sigwait() is guaranteed to work as
*      expected only if all other threads in the program block the signals
*      waited for (otherwise, the signals could be delivered to other threads
*      than the one doing sigwait(), which would make sigwait() useless). In
*      this particular case, the problem described in this question does not
*      appear.  One day, sigwait() will be implemented in the kernel, along
*      with other POSIX 1003.1b extensions, and sigwait() will have a more
*      natural behavior (as well as better performance).
*****************************************************************************/

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef utlTEST
#   include <math.h>
#endif

#include <utlTypes.h>

#include <utlError.h>
#include <utlLinkedList.h>
#include <utlMalloc.h>
#include <utlTime.h>
#include <utlTimer.h>
#include <utlSemaphore.h>
#include <utlTrace.h>

#include <utlEventHandler.h>


/*--- Configuration ---------------------------------------------------------*/

/*--- for tracing event handling ---*/
#define utlTRACE_EVENT_HANDLER "events"

#define UNUSEDPARAM(param) (void)param;
/*--- Data Types ------------------------------------------------------------*/

typedef struct utlEventHandlerNode_S *utlEventHandlerNode_P;
typedef struct utlEventHandlerNode_S {
	utlEventHandlerNode_P next_p;
	utlEventHandlerType_T type;             /* event handler type     */
	utlEventHandlerPriority_T priority;     /* event handler priority */
	bool enable;
	utlEventHandlerId_T handler_id;         /* event handler ID       */
	union {
		utlRelativeTime_T period;       /* only valid when type == utlEVENT_HANDLER_TYPE_IDLE                 */
		int fd;             /* only valid when type == utlEVENT_HANDLER_TYPE_READ
							    or utlEVENT_HANDLER_TYPE_WRITE
							    or utlEVENT_HANDLER_TYPE_READ_WRITE
							    or utlEVENT_HANDLER_TYPE_EXCEPTION
							    or utlEVENT_HANDLER_TYPE_EXCEPTION_READ
							    or utlEVENT_HANDLER_TYPE_EXCEPTION_WRITE
							    or utlEVENT_HANDLER_TYPE_EXCEPTION_READ_WRITE */
		int signal;                     /* only valid when type == utlEVENT_HANDLER_TYPE_SIG*                 */
		int exit;                       /* only valid when type == utlEVENT_HANDLER_TYPE_EXIT                 */
	} u;
	struct {
		utlEventHandlerFunction_P function_p;
		void                      *arg_p;
	} call_back;
}             utlEventHandlerNode_T;
typedef const utlEventHandlerNode_T *utlEventHandlerNode_P2c;


/*--- Local Data ------------------------------------------------------------*/

static utlSemaphore_T events_semaphore = utlNULL_SEMAPHORE;  /* protection for static event-handling data */

/*--- signal-event pipe ---*/
static struct {
	bool initialized;
	int read_fd;
	int write_fd;
} plumbing = { false, 0, 0 };

static utlLinkedList_T fd_handlers = utlEMPTY_LINKED_LIST;
static utlLinkedList_T idle_handlers = utlEMPTY_LINKED_LIST;
static utlLinkedList_T signal_handlers = utlEMPTY_LINKED_LIST;
static utlLinkedList_T exit_handlers = utlEMPTY_LINKED_LIST;

static enum {
	utlDELETED_NONE           = 0x00,
	utlDELETED_FD_HANDLER     = 0x01,
	utlDELETED_SIGNAL_HANDLER = 0x02,
	utlDELETED_IDLE_HANDLER   = 0x04,
	utlDELETED_EXIT_HANDLER   = 0x08
} deleted_pending = utlDELETED_NONE;

static bool running                  = false;
static utlEventHandlerId_T next_possible_handler_id = 1;  /* next unused handler ID */


/*---------------------------------------------------------------------------*
* FUNCTION
*	_utlFindUnusedHandlerId()
* INPUT
*      none
* OUTPUT
*	none
* RETURNS
*	An event handler ID for success, utlFAILED for failure
* DESCRIPTION
*	Finds the next unused event handler ID.
*---------------------------------------------------------------------------*/
static utlEventHandlerId_T _utlFindUnusedHandlerId(void)
{
	for (;; )
	{
		utlEventHandlerNode_P curr_p;

		for (curr_p = (utlEventHandlerNode_P)fd_handlers.head_p; curr_p != NULL; curr_p = curr_p->next_p)
			if (curr_p->handler_id == next_possible_handler_id)
				break;
		if (curr_p == NULL)
		{
			for (curr_p = (utlEventHandlerNode_P)idle_handlers.head_p; curr_p != NULL; curr_p = curr_p->next_p)
				if (curr_p->handler_id == next_possible_handler_id)
					break;
			if (curr_p == NULL)
			{
				for (curr_p = (utlEventHandlerNode_P)signal_handlers.head_p; curr_p != NULL; curr_p = curr_p->next_p)
					if (curr_p->handler_id == next_possible_handler_id)
						break;
				if (curr_p == NULL)
				{
					for (curr_p = (utlEventHandlerNode_P)exit_handlers.head_p; curr_p != NULL; curr_p = curr_p->next_p)
						if (curr_p->handler_id == next_possible_handler_id)
							break;
					if ((curr_p == NULL) && (next_possible_handler_id != (utlTimerId_T)0) &&
					    (next_possible_handler_id != (utlTimerId_T)utlFAILED))
						return next_possible_handler_id;
				}
			}
		}

		next_possible_handler_id++;
	}

	utlError("No more free event-handler IDs");
	return utlFAILED;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	_utlSignalToString(signal)
* INPUT
*	signal == a signal
* OUTPUT
*	none
* RETURNS
*	nothing
* DESCRIPTION
*	Converts a signal into a character string.
*---------------------------------------------------------------------------*/
static char *_utlSignalToString(const int signal)
{
	static char buf[20];

	switch (signal)
	{
	case SIGHUP:    return "SIGHUP";
	case SIGINT:    return "SIGINT";
	case SIGQUIT:   return "SIGQUIT";
	case SIGILL:    return "SIGILL";
	case SIGTRAP:   return "SIGTRAP";
	case SIGABRT:   return "SIGABRT";
	case SIGFPE:    return "SIGFPE";
	case SIGKILL:   return "SIGKILL";
	case SIGBUS:    return "SIGBUS";
	case SIGSEGV:   return "SIGSEGV";
	case SIGSYS:    return "SIGSYS";
	case SIGPIPE:   return "SIGPIPE";
	case SIGALRM:   return "SIGALRM";
	case SIGTERM:   return "SIGTERM";
	case SIGURG:    return "SIGURG";
	case SIGSTOP:   return "SIGSTOP";
	case SIGTSTP:   return "SIGTSTP";
	case SIGCONT:   return "SIGCONT";
	case SIGCHLD:   return "SIGCHLD";
	case SIGTTIN:   return "SIGTTIN";
	case SIGTTOU:   return "SIGTTOU";
	case SIGIO:     return "SIGIO";
	case SIGXCPU:   return "SIGXCPU";
	case SIGXFSZ:   return "SIGXFSZ";
	case SIGVTALRM: return "SIGVTALRM";
	case SIGPROF:   return "SIGPROF";
	case SIGWINCH:  return "SIGWINCH";
	case SIGUSR1:   return "SIGUSR1";
	case SIGUSR2:   return "SIGUSR2";
/*	case SIGPOLL:   return "SIGPOLL";   */
/*	case SIGIOT:    return "SIGIOT";    */
/*	case SIGSTKFLT: return "SIGSTKFLT"; */
/*	case SIGCLD:    return "SIGCLD";    */
/*	case SIGPWR:    return "SIGPWR";    */
/*	case SIGUNUSED: return "SIGUNUSED"; */

	default:
		(void)sprintf(buf, "<%d>", signal);
		return buf;
	}

	/*NOTREACHED*/
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	_utlEventTypeToString(event_type)
* INPUT
*	event_type == an event type
* OUTPUT
*	none
* RETURNS
*	nothing
* DESCRIPTION
*	Converts an event type into a character string.
*---------------------------------------------------------------------------*/
static char *_utlEventTypeToString(const utlEventHandlerType_T event_type)
{
	static char buf[20];

	switch (event_type)
	{
	case utlEVENT_HANDLER_TYPE_UNKNOWN:              return "unknown";

	case utlEVENT_HANDLER_TYPE_SIGHUP:               return "SIGHUP";
	case utlEVENT_HANDLER_TYPE_SIGINT:               return "SIGINT";
	case utlEVENT_HANDLER_TYPE_SIGQUIT:              return "SIGQUIT";
	case utlEVENT_HANDLER_TYPE_SIGILL:               return "SIGILL";
	case utlEVENT_HANDLER_TYPE_SIGTRAP:              return "SIGTRAP";
	case utlEVENT_HANDLER_TYPE_SIGABRT:              return "SIGABRT";
	case utlEVENT_HANDLER_TYPE_SIGFPE:               return "SIGFPE";
	case utlEVENT_HANDLER_TYPE_SIGKILL:              return "SIGKILL";
	case utlEVENT_HANDLER_TYPE_SIGBUS:               return "SIGBUS";
	case utlEVENT_HANDLER_TYPE_SIGSEGV:              return "SIGSEGV";
	case utlEVENT_HANDLER_TYPE_SIGSYS:               return "SIGSYS";
	case utlEVENT_HANDLER_TYPE_SIGPIPE:              return "SIGPIPE";
	case utlEVENT_HANDLER_TYPE_SIGALRM:              return "SIGALRM";
	case utlEVENT_HANDLER_TYPE_SIGTERM:              return "SIGTERM";
	case utlEVENT_HANDLER_TYPE_SIGURG:               return "SIGURG";
	case utlEVENT_HANDLER_TYPE_SIGSTOP:              return "SIGSTOP";
	case utlEVENT_HANDLER_TYPE_SIGTSTP:              return "SIGTSTP";
	case utlEVENT_HANDLER_TYPE_SIGCONT:              return "SIGCONT";
	case utlEVENT_HANDLER_TYPE_SIGCHLD:              return "SIGCHLD";
	case utlEVENT_HANDLER_TYPE_SIGTTIN:              return "SIGTTIN";
	case utlEVENT_HANDLER_TYPE_SIGTTOU:              return "SIGTTOU";
	case utlEVENT_HANDLER_TYPE_SIGIO:                return "SIGIO";
	case utlEVENT_HANDLER_TYPE_SIGXCPU:              return "SIGXCPU";
	case utlEVENT_HANDLER_TYPE_SIGXFSZ:              return "SIGXFSZ";
	case utlEVENT_HANDLER_TYPE_SIGVTALRM:            return "SIGVTALRM";
	case utlEVENT_HANDLER_TYPE_SIGPROF:              return "SIGPROF";
	case utlEVENT_HANDLER_TYPE_SIGWINCH:             return "SIGWINCH";
	case utlEVENT_HANDLER_TYPE_SIGUSR1:              return "SIGUSR1";
	case utlEVENT_HANDLER_TYPE_SIGUSR2:              return "SIGUSR2";
/*      case utlEVENT_HANDLER_TYPE_SIGPOLL:              return "SIGPOLL";   */
/*      case utlEVENT_HANDLER_TYPE_SIGIOT:               return "SIGIOT";    */
/*      case utlEVENT_HANDLER_TYPE_SIGSTKFLT:            return "SIGSTKFLT"; */
/*      case utlEVENT_HANDLER_TYPE_SIGCLD:               return "SIGCLD";    */
/*      case utlEVENT_HANDLER_TYPE_SIGPWR:               return "SIGPWR";    */
/*      case utlEVENT_HANDLER_TYPE_SIGUNUSED:            return "SIGUNUSED"; */
	case utlEVENT_HANDLER_TYPE_WRITE:                return "write";
	case utlEVENT_HANDLER_TYPE_READ:                 return "read";
	case utlEVENT_HANDLER_TYPE_READ_WRITE:           return "read-write";
	case utlEVENT_HANDLER_TYPE_EXCEPTION:            return "exception";
	case utlEVENT_HANDLER_TYPE_EXCEPTION_WRITE:      return "exception-write";
	case utlEVENT_HANDLER_TYPE_EXCEPTION_READ:       return "exception-read";
	case utlEVENT_HANDLER_TYPE_EXCEPTION_READ_WRITE: return "exception-read-write";
	case utlEVENT_HANDLER_TYPE_IDLE:                 return "idle";
	case utlEVENT_HANDLER_TYPE_EXIT:                 return "exit";
	case utlEVENT_HANDLER_TYPE_DIRECT_FUNC_CALL:     return "direct";
	case utlEVENT_HANDLER_TYPE_ZOMBIE:               return "zombie";

	default:
		(void)sprintf(buf, "<%u>", event_type);
		return buf;
	}

	/*NOTREACHED*/
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	_utlEventHandlerSignal(signal)
* INPUT
*	signal == which signal occurred
* OUTPUT
*	none
* RETURNS
*	nothing
* DESCRIPTION
*	Handles signal events: Sends one signal ID byte down the signal-event
*      pipe.  This function is only invoked by the operating system upon
*      receipt of a signal.
*---------------------------------------------------------------------------*/
static void _utlEventHandlerSignal(int signal)
{
	char buf[1];
	int n;

	/*--- write signal ID byte to signal-event pipe ---*/
	buf[0] = signal;
	for (n = 100; n > 0; n--)
	{
		ssize_t rv;

		/*--- if write succeeds, we're done ---*/
		if ((rv = write(plumbing.write_fd, buf, (size_t)1)) == (ssize_t)1)
			return;

		if (rv == (ssize_t)0)
			continue;

		/*--- if write didn't fail due to an interrupt, pause before trying again ---*/
		if (errno != EINTR)
		{
			int select_rv;

			for (;; )
			{
				struct timeval timeout;

				timeout.tv_sec  = 0;
				timeout.tv_usec = 100000;
				if ((select_rv = select(0, NULL, NULL, NULL, &timeout)) >= 0)
					break;

				if (errno != EINTR)
				{
					utlSigError(utlHERE, "Call to %s() failed, %s", "select", strerror(errno));
					break;
				}
			}
		}
	}

	/*--- we're inside a signal handler, so we must use utlSigError() for error reporting ---*/
	utlSigError(utlHERE, "write() to signal-event pipe failed after 100 attempts");
}
/*---------------------------------------------------------------------------*
* FUNCTION
*	_utlEventHandlerExit()
* INPUT
*	none
* OUTPUT
*	none
* RETURNS
*	nothing
* DESCRIPTION
*	Handles exit events: invokes all set exit-handler functions.
*---------------------------------------------------------------------------*/
static void _utlEventHandlerExit(void)
{
	utlEventHandlerPriority_T priority;

	utlTrace(utlTRACE_EVENT_HANDLER,
		 utlPrintTrace("exit event\n");
		 );

	/*--- acquire exclusive-access.  We're going to use an exclusive-lock even
	      though a shared-lock would do so that add/remove event handlers,
	      and enable/disable FD event handlers routines can be called without
	      resulting in deadlock (all these routines need to engage an
	      exclusive-lock) */
	if (utlAcquireExclusiveAccess(&events_semaphore) != utlSUCCESS)
		return;

	/*--- for each possible priority level... ---*/
	for (priority = utlEVENT_HANDLER_PRIORITY_INTERNAL; priority >= utlEVENT_HANDLER_PRIORITY_LOW; priority--)
	{
		utlEventHandlerNode_P node_p;

		/*--- for each exit handler... ---*/
		for (node_p = (utlEventHandlerNode_P)exit_handlers.head_p; node_p != NULL; node_p = node_p->next_p)
		{

			/*--- wrong priority? ---*/
			if (node_p->priority != priority)
				continue;

			/*--- perform exit-handler action ---*/
			if (node_p->call_back.function_p != NULL)
			{
				utlTrace(utlTRACE_EVENT_HANDLER,
					 utlPrintTrace("exit event for 0x%x\n", (unsigned int)node_p->call_back.function_p);
					 );

				if ((*(node_p->call_back.function_p))(node_p->type, utlEVENT_HANDLER_TYPE_EXIT, -1, NULL, node_p->call_back.arg_p) != utlSUCCESS)
				{
					utlError("Callback function 0x%08x failed", node_p->call_back.function_p);

					/*--- clean ---*/
					(void)utlReleaseExclusiveAccess(&events_semaphore);

					return;
				}
			}
		}
	}

	/*--- release exclusive-access ---*/
	(void)utlReleaseExclusiveAccess(&events_semaphore);
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	_utlEventHandlerSignalHandler(handler_type, event_type, fd, period, arg)
* INPUT
*        handler_type == handler type
*          event_type == event type
*                  fd == which file descriptor
*	      period_p == idle period
*	         arg_p == pointer to user-defined data
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure.
* DESCRIPTION
*	Handles one or more signal events.  This function is only invoked by
*      receipt of a signal ID byte on the signal-event pipe.
*---------------------------------------------------------------------------*/
static utlReturnCode_T _utlEventHandlerSignalHandler(const utlEventHandlerType_T handler_type,
						     const utlEventHandlerType_T event_type,
						     const int fd,
						     const utlRelativeTime_P2c period_p,
						     void                  *arg_p)
{
	UNUSEDPARAM(period_p)

	utlAssert(handler_type == utlEVENT_HANDLER_TYPE_READ);
	utlAssert(event_type   == utlEVENT_HANDLER_TYPE_READ);
	utlAssert(fd           == plumbing.read_fd);
	utlAssert(arg_p        == NULL);

	utlTrace(utlTRACE_EVENT_HANDLER,
		 utlPrintTrace("%s event\n", _utlEventTypeToString(event_type));
		 );

	/*--- acquire exclusive-access.  We're going to use an exclusive-lock even
	      though a shared-lock would do so that add/remove event handlers,
	      and enable/disable FD event handlers routines can be called without
	      resulting in deadlock (all these routines need to engage an
	      exclusive-lock) */
	if (utlAcquireExclusiveAccess(&events_semaphore) != utlSUCCESS)
		return utlFAILED;

	/*--- read all signal ID bytes queued in signal-event pipe ---*/
	{
		char buf[128];

		for (;; )
		{
			int n;
			int i;

			for (;; )
			{
				if ((n = read(plumbing.read_fd, buf, sizeof(buf))) >= 0)
					break;

				/*--- nothing to read? ---*/
				if (errno == EAGAIN)
				{
					n = 0;
					break;
				}

				if (errno != EINTR)
				{
					utlError("Call to %s() failed, %s", "read", strerror(errno));

					/*--- clean ---*/
					(void)utlReleaseExclusiveAccess(&events_semaphore);

					return utlFAILED;
				}
			}

			/*--- for each signal ID byte... ---*/
			for (i = 0; i < n; i++)
			{
				utlEventHandlerPriority_T priority;

				/*--- for each possible priority level... ---*/
				for (priority = utlEVENT_HANDLER_PRIORITY_INTERNAL; priority >= utlEVENT_HANDLER_PRIORITY_LOW; priority--)
				{
					utlEventHandlerNode_P node_p;

					/*--- for each signal handler... ---*/
					for (node_p = (utlEventHandlerNode_P)signal_handlers.head_p; node_p != NULL; node_p = node_p->next_p)
					{

						/*--- wrong priority? ---*/
						if (node_p->priority != priority)
							continue;

						/*--- found a(nother) handler for this signal? ---*/
						if (node_p->u.signal == buf[i])
						{

							/*--- perform signal-handler action ---*/
							{
								if (node_p->call_back.function_p != NULL)
								{
									utlTrace(utlTRACE_EVENT_HANDLER,
										 utlPrintTrace("signal %s event for 0x%x\n",
											       _utlSignalToString(node_p->u.signal),
											       (unsigned int)node_p->call_back.function_p);
										 );

									if ((*(node_p->call_back.function_p))(node_p->type, (utlEventHandlerType_T)node_p->u.signal, -1, NULL, node_p->call_back.arg_p) != utlSUCCESS)
									{
										utlError("Callback function 0x%08x failed", node_p->call_back.function_p);

										/*--- clean ---*/
										(void)utlReleaseExclusiveAccess(&events_semaphore);

										return utlFAILED;
									}
								}
							}
						}
					}
				}
			}

			/*--- if we've read everything ---*/
			if ((unsigned int)n < sizeof(buf))
				break;
		}
	}

	/*--- release exclusive-access ---*/
	if (utlReleaseExclusiveAccess(&events_semaphore)  != utlSUCCESS)
		return utlFAILED;

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlInitSignalPipe()
* INPUT
*	none
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*	Initializes the signal-event pipe.
*---------------------------------------------------------------------------*/
static utlReturnCode_T utlInitSignalPipe(void)
{
	utlEventHandlerId_T handler_id;
	utlEventHandlerNode_P node_p;

	/*--- if signal handling is already initialized, bail out ---*/
	if (plumbing.initialized != false)
		return utlSUCCESS;

	/*--- create signal-event pipe ---*/
	{
		int pipe_fds[2];

		if (pipe(pipe_fds) < 0)
		{
			utlError("Call to %s() failed, %s", "pipe", strerror(errno));
			return utlFAILED;
		}

		plumbing.read_fd  = pipe_fds[0];
		plumbing.write_fd = pipe_fds[1];
	}

	if ((handler_id = _utlFindUnusedHandlerId()) == (utlEventHandlerId_T)utlFAILED)
	{

		/*--- clean ---*/
		(void)close(plumbing.write_fd);
		(void)close(plumbing.read_fd);

		return utlFAILED;
	}

	if ((node_p = (utlEventHandlerNode_P)utlMalloc(sizeof(utlEventHandlerNode_T))) == NULL)
	{

		/*--- clean ---*/
		(void)close(plumbing.write_fd);
		(void)close(plumbing.read_fd);

		return utlFAILED;
	}

	/*--- initialize the new event handler node ---*/
	node_p->next_p               = NULL;
	node_p->type                 = utlEVENT_HANDLER_TYPE_READ;
	node_p->priority             = utlEVENT_HANDLER_PRIORITY_INTERNAL;
	node_p->enable               = true;
	node_p->handler_id           = handler_id;
	node_p->u.fd                 = plumbing.read_fd;
	node_p->call_back.function_p = _utlEventHandlerSignalHandler;
	node_p->call_back.arg_p      = NULL;

	/*--- add to list of known fd-event handlers ---*/
	(void)utlPutTailNode(&fd_handlers, utlEventHandlerNode_T, node_p);

	plumbing.initialized = true;

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlDeleteEventHandler(handler_id)
* INPUT
*	handler_id == which event handler to delete
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS success, utlFAILED for failure
* DESCRIPTION
*	Deletes the specified event handler.  This function actually only
*      marks handlers for deletion.  The actual deletion occurs the next
*      time the event handlers list is traversed by the main event loop.
*---------------------------------------------------------------------------*/
utlReturnCode_T utlDeleteEventHandler(const utlEventHandlerId_T handler_id)
{
	utlEventHandlerNode_P node_p;

	/*--- mutex initialization ---*/
	if (!utlIsSemaphoreInitialized(&events_semaphore))
	{
		if (utlInitSemaphore(&events_semaphore, utlSEMAPHORE_ATTR_SHARING_ENABLE |
				     utlSEMAPHORE_ATTR_NESTING_ENABLE) != utlSUCCESS)
			return utlFAILED;
	}

	/*--- acquire exclusive-access ---*/
	if (utlAcquireExclusiveAccess(&events_semaphore) != utlSUCCESS)
		return utlFAILED;

	/*--- mark fd-event handler we wish to delete... ---*/
	for (node_p = (utlEventHandlerNode_P)fd_handlers.head_p; node_p != NULL; node_p = node_p->next_p)
		if (node_p->handler_id == handler_id)
		{
			utlTrace(utlTRACE_EVENT_HANDLER,
				 char *what;

				 switch (node_p->type)
				 {
				 case utlEVENT_HANDLER_TYPE_READ:                 what = "read";                  break;
				 case utlEVENT_HANDLER_TYPE_WRITE:                what = "write";                 break;
				 case utlEVENT_HANDLER_TYPE_READ_WRITE:           what = "read/write";            break;
				 case utlEVENT_HANDLER_TYPE_EXCEPTION:            what = "exception";             break;
				 case utlEVENT_HANDLER_TYPE_EXCEPTION_READ:       what = "exception/read";        break;
				 case utlEVENT_HANDLER_TYPE_EXCEPTION_WRITE:      what = "exception/write";       break;
				 case utlEVENT_HANDLER_TYPE_EXCEPTION_READ_WRITE: what = "exception/read/write";  break;
				 default:                                         what = "<unknown>";             break;
				 }

				 utlPrintTrace("deleted `%s' event handler (id=%u) for fd %d\n",
					       what, (unsigned int)handler_id, node_p->u.fd);
				 );

			node_p->type     = utlEVENT_HANDLER_TYPE_ZOMBIE;
			deleted_pending |= utlDELETED_FD_HANDLER;

			/*--- release exclusive-access ---*/
			if (utlReleaseExclusiveAccess(&events_semaphore) != utlSUCCESS)
				return utlFAILED;

			return utlSUCCESS;
		}

	/*--- mark idle event handler we wish to delete... ---*/
	for (node_p = (utlEventHandlerNode_P)idle_handlers.head_p; node_p != NULL; node_p = node_p->next_p)
		if (node_p->handler_id == handler_id)
		{
			utlTrace(utlTRACE_EVENT_HANDLER,
				 utlPrintTrace("deleted `idle' event handler (HID=%u), called at %d.%09d secs\n",
					       (unsigned int)handler_id,
					       node_p->u.period.seconds,
					       node_p->u.period.nanoseconds);
				 );

			node_p->type     = utlEVENT_HANDLER_TYPE_ZOMBIE;
			deleted_pending |= utlDELETED_IDLE_HANDLER;

			/*--- release exclusive-access ---*/
			if (utlReleaseExclusiveAccess(&events_semaphore) != utlSUCCESS)
				return utlFAILED;

			return utlSUCCESS;
		}

	/*--- mark signal event handler we wish to delete... ---*/
	for (node_p = (utlEventHandlerNode_P)signal_handlers.head_p; node_p != NULL; node_p = node_p->next_p)
		if (node_p->handler_id == handler_id)
		{
			utlTrace(utlTRACE_EVENT_HANDLER,
				 utlPrintTrace("deleted `signal' event handler (HID=%u)\n",
					       (unsigned int)handler_id);
				 );

			node_p->type     = utlEVENT_HANDLER_TYPE_ZOMBIE;
			deleted_pending |= utlDELETED_SIGNAL_HANDLER;

			/*--- release exclusive-access ---*/
			if (utlReleaseExclusiveAccess(&events_semaphore) != utlSUCCESS)
				return utlFAILED;

			return utlSUCCESS;
		}

	/*--- mark exit event handler we wish to delete... ---*/
	for (node_p = (utlEventHandlerNode_P)exit_handlers.head_p; node_p != NULL; node_p = node_p->next_p)
	{
		if (node_p->handler_id == handler_id)
		{
			utlTrace(utlTRACE_EVENT_HANDLER,
				 utlPrintTrace("deleted `exit' event handler (HID=%u)\n",
					       (unsigned int)handler_id);
				 );

			node_p->type     = utlEVENT_HANDLER_TYPE_ZOMBIE;
			deleted_pending |= utlDELETED_EXIT_HANDLER;

			/*--- release exclusive-access ---*/
			if (utlReleaseExclusiveAccess(&events_semaphore) != utlSUCCESS)
				return utlFAILED;

			return utlSUCCESS;
		}
	}

	utlError("Event handler %d not found", handler_id);

	/*--- clean ---*/
	(void)utlReleaseExclusiveAccess(&events_semaphore);

	return utlFAILED;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlSetFdEventHandler(type, priority, fd, function, arg)
* INPUT
*	      type == the type of event to report:
*                      utlEVENT_HANDLER_TYPE_READ
*                      utlEVENT_HANDLER_TYPE_WRITE
*                      utlEVENT_HANDLER_TYPE_READ_WRITE
*                      utlEVENT_HANDLER_TYPE_EXCEPTION
*                      utlEVENT_HANDLER_TYPE_EXCEPTION_READ
*                      utlEVENT_HANDLER_TYPE_EXCEPTION_WRITE
*                      utlEVENT_HANDLER_TYPE_EXCEPTION_READ_WRITE
*        priority == the handler priority
*              fd == the file descriptor to watch
*      function_p == pointer to the event call-back function
*           arg_p == user-defined function argument
* OUTPUT
*	none
* RETURNS
*	An event handler ID for success, utlFAILED for failure
* DESCRIPTION
*	Arranges for `function_p' to be called when there is `type' activity
*      on `fd'.
*
*      Note that there can be zero or more call-back functions defined for
*      the same "type" and "fd".
*---------------------------------------------------------------------------*/
utlEventHandlerId_T utlSetFdEventHandler(const utlEventHandlerType_T type,
					 const utlEventHandlerPriority_T priority,
					 const int fd,
					 const utlEventHandlerFunction_P function_p,
					 void                      *arg_p)
{
	utlEventHandlerId_T handler_id;
	utlEventHandlerNode_P node_p;

	switch (type)
	{
	case utlEVENT_HANDLER_TYPE_READ:
	case utlEVENT_HANDLER_TYPE_WRITE:
	case utlEVENT_HANDLER_TYPE_READ_WRITE:
	case utlEVENT_HANDLER_TYPE_EXCEPTION:
	case utlEVENT_HANDLER_TYPE_EXCEPTION_READ:
	case utlEVENT_HANDLER_TYPE_EXCEPTION_WRITE:
	case utlEVENT_HANDLER_TYPE_EXCEPTION_READ_WRITE: break;

	default:
		return utlFAILED;
	}
	switch (priority)
	{
	case utlEVENT_HANDLER_PRIORITY_CRITICAL:
	case utlEVENT_HANDLER_PRIORITY_HIGH:
	case utlEVENT_HANDLER_PRIORITY_MEDIUM:
	case utlEVENT_HANDLER_PRIORITY_LOW:    break;

	default:
		return utlFAILED;
	}
	utlAssert(function_p != NULL);

	/*--- make all reads non-blocking for this file descriptor... ---*/
	{
		bool fd_valid;
		int oflags;

		fd_valid = false;

		/*--- fetch current flag settings ---*/
		for (;; )
		{
			if ((oflags = fcntl(fd, F_GETFL)) != -1)
			{
				fd_valid = true;
				break;
			}

			if (errno == EBADF)
				break;

			if (errno == EINTR)
			{
				utlError("Call to %s() failed, %s", "fcntl", strerror(errno));
				return utlFAILED;
			}
		}

		/*--- if file descriptor is valid ---*/
		if (fd_valid == true)
		{
			oflags |= O_NONBLOCK;

			/*--- apply new flag settings ---*/
			for (;; )
			{
				if (fcntl(fd, F_SETFL, oflags) == 0)
					break;

				if (errno == EINTR)
				{
					utlError("Call to %s() failed, %s", "fcntl", strerror(errno));
					return utlFAILED;
				}
			}
		}
	}

	/*--- mutex initialization ---*/
	if (!utlIsSemaphoreInitialized(&events_semaphore))
	{
		if (utlInitSemaphore(&events_semaphore, utlSEMAPHORE_ATTR_SHARING_ENABLE |
				     utlSEMAPHORE_ATTR_NESTING_ENABLE) != utlSUCCESS)
			return utlFAILED;
	}

	/*--- acquire exclusive-access ---*/
	if (utlAcquireExclusiveAccess(&events_semaphore) != utlSUCCESS)
		return utlFAILED;

	if ((handler_id = _utlFindUnusedHandlerId()) == (utlEventHandlerId_T)utlFAILED)
	{

		/*--- clean ---*/
		(void)utlReleaseExclusiveAccess(&events_semaphore);

		return utlFAILED;
	}

	if ((node_p = (utlEventHandlerNode_P)utlMalloc(sizeof(utlEventHandlerNode_T))) == NULL)
	{

		/*--- clean ---*/
		(void)utlReleaseExclusiveAccess(&events_semaphore);

		return utlFAILED;
	}

	/*--- initialize the new event handler node ---*/
	node_p->next_p               = NULL;
	node_p->type                 = type;
	node_p->priority             = priority;
	node_p->enable               = true;
	node_p->handler_id           = handler_id;
	node_p->u.fd                 = fd;
	node_p->call_back.function_p = function_p;
	node_p->call_back.arg_p      = arg_p;

	/*--- add to list of known fd-event handlers ---*/
	(void)utlPutTailNode(&fd_handlers, utlEventHandlerNode_T, node_p);

	utlTrace(utlTRACE_EVENT_HANDLER,
		 char *what;

		 switch (type)
		 {
		 case utlEVENT_HANDLER_TYPE_READ:                 what = "read";                  break;
		 case utlEVENT_HANDLER_TYPE_WRITE:                what = "write";                 break;
		 case utlEVENT_HANDLER_TYPE_READ_WRITE:           what = "read/write";            break;
		 case utlEVENT_HANDLER_TYPE_EXCEPTION:            what = "exception";             break;
		 case utlEVENT_HANDLER_TYPE_EXCEPTION_READ:       what = "exception/read";        break;
		 case utlEVENT_HANDLER_TYPE_EXCEPTION_WRITE:      what = "exception/write";       break;
		 case utlEVENT_HANDLER_TYPE_EXCEPTION_READ_WRITE: what = "exception/read/write";  break;
		 default:                                        what = "<unknown>";             break;
		 }

		 utlPrintTrace("added `%s' event handler (HID=%u) for fd %d\n",
			       what, (unsigned int)node_p->handler_id, node_p->u.fd);
		 );

	/*--- release exclusive-access ---*/
	if (utlReleaseExclusiveAccess(&events_semaphore) != utlSUCCESS)
		return utlFAILED;

	return handler_id;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	_utlSetFdEventDetection(type, fd)
* INPUT
*	type == the type of event to enable:
*                  utlEVENT_HANDLER_TYPE_READ
*                  utlEVENT_HANDLER_TYPE_WRITE
*                  utlEVENT_HANDLER_TYPE_READ_WRITE
*                  utlEVENT_HANDLER_TYPE_EXCEPTION
*                  utlEVENT_HANDLER_TYPE_EXCEPTION_READ
*                  utlEVENT_HANDLER_TYPE_EXCEPTION_WRITE
*                  utlEVENT_HANDLER_TYPE_EXCEPTION_READ_WRITE
*        fd == the file descriptor being watched
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*	Sets detection of events for the specified file descriptor.
*---------------------------------------------------------------------------*/
static utlReturnCode_T _utlSetFdEventDetection(const utlEventHandlerType_T type,
					       const int fd,
					       const bool value)
{
	utlEventHandlerNode_P node_p;

	/*--- mutex initialization ---*/
	if (!utlIsSemaphoreInitialized(&events_semaphore))
	{
		if (utlInitSemaphore(&events_semaphore, utlSEMAPHORE_ATTR_SHARING_ENABLE |
				     utlSEMAPHORE_ATTR_NESTING_ENABLE) != utlSUCCESS)
			return utlFAILED;
	}

	/*--- acquire exclusive-access ---*/
	if (utlAcquireExclusiveAccess(&events_semaphore) != utlSUCCESS)
		return utlFAILED;

	/*--- find fd-event handlers we wish to toggle... ---*/
	for (node_p = (utlEventHandlerNode_P)fd_handlers.head_p; node_p != NULL; node_p = node_p->next_p)
		if ((node_p->type == type) && (node_p->u.fd == fd))
		{
			node_p->enable = value;

			/*--- release exclusive-access ---*/
			if (utlReleaseExclusiveAccess(&events_semaphore) != utlSUCCESS)
				return utlFAILED;

			return utlSUCCESS;
		}

	utlError("fd %d not found", fd);

	/*--- clean ---*/
	(void)utlReleaseExclusiveAccess(&events_semaphore);

	return utlFAILED;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlEnableFdEventHandler(type, fd)
* INPUT
*	type == the type of event to enable:
*                  utlEVENT_HANDLER_TYPE_READ
*                  utlEVENT_HANDLER_TYPE_WRITE
*                  utlEVENT_HANDLER_TYPE_READ_WRITE
*                  utlEVENT_HANDLER_TYPE_EXCEPTION
*                  utlEVENT_HANDLER_TYPE_EXCEPTION_READ
*                  utlEVENT_HANDLER_TYPE_EXCEPTION_WRITE
*                  utlEVENT_HANDLER_TYPE_EXCEPTION_READ_WRITE
*        fd == the file descriptor being watched
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*	Enables detection of events for the specified file descriptor.
*---------------------------------------------------------------------------*/
utlReturnCode_T utlEnableFdEventHandler(const utlEventHandlerType_T type,
					const int fd)
{
	return _utlSetFdEventDetection(type, fd, true);
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlDisableFdEventHandler(type, fd)
* INPUT
*	type == the type of event to disable:
*                  utlEVENT_HANDLER_TYPE_READ
*                  utlEVENT_HANDLER_TYPE_WRITE
*                  utlEVENT_HANDLER_TYPE_READ_WRITE
*                  utlEVENT_HANDLER_TYPE_EXCEPTION
*                  utlEVENT_HANDLER_TYPE_EXCEPTION_READ
*                  utlEVENT_HANDLER_TYPE_EXCEPTION_WRITE
*                  utlEVENT_HANDLER_TYPE_EXCEPTION_READ_WRITE
*        fd == the file descriptor being watched
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*	Disables detection of events for the specified file descriptor.
*---------------------------------------------------------------------------*/
utlReturnCode_T utlDisableFdEventHandler(const utlEventHandlerType_T type,
					 const int fd)
{
	return _utlSetFdEventDetection(type, fd, false);
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlSetIdleEventHandler(period, priority, function, arg)
* INPUT
*        period_p == pointer to a filled utlRelativeTime_T structure
*        priority == the handler priority
*      function_p == pointer to the event call-back function
*           arg_p == user-defined function argument
* OUTPUT
*	none
* RETURNS
*	An event handler ID for success, utlFAILED for failure
* DESCRIPTION
*	Arranges for `function_p' to be called when there has been no event
*      activity for more than the time specified by `period_p'.  If `function_p'
*      is NULL, deletes all previously set call-back functions having the
*      same time-out period.  The actual deletion is deferred until the event
*      handler loop processes the next round of events.
*
*      Note that there can be zero or more idle call-back functions defined.
*---------------------------------------------------------------------------*/
utlEventHandlerId_T utlSetIdleEventHandler(const utlRelativeTime_P2c period_p,
					   const utlEventHandlerPriority_T priority,
					   const utlEventHandlerFunction_P function_p,
					   void                      *arg_p)
{
	utlEventHandlerId_T handler_id;
	utlEventHandlerNode_P node_p;

	utlAssert(period_p != NULL);

	switch (priority)
	{
	case utlEVENT_HANDLER_PRIORITY_CRITICAL:
	case utlEVENT_HANDLER_PRIORITY_HIGH:
	case utlEVENT_HANDLER_PRIORITY_MEDIUM:
	case utlEVENT_HANDLER_PRIORITY_LOW:    break;

	default:
		return utlFAILED;
	}
	utlAssert(function_p != NULL);

	/*--- mutex initialization ---*/
	if (!utlIsSemaphoreInitialized(&events_semaphore))
	{
		if (utlInitSemaphore(&events_semaphore, utlSEMAPHORE_ATTR_SHARING_ENABLE |
				     utlSEMAPHORE_ATTR_NESTING_ENABLE) != utlSUCCESS)
			return utlFAILED;
	}

	/*--- acquire exclusive-access ---*/
	if (utlAcquireExclusiveAccess(&events_semaphore) != utlSUCCESS)
		return utlFAILED;

	if ((handler_id = _utlFindUnusedHandlerId()) == (utlEventHandlerId_T)utlFAILED)
	{

		/*--- clean ---*/
		(void)utlReleaseExclusiveAccess(&events_semaphore);

		return utlFAILED;
	}

	if ((node_p = (utlEventHandlerNode_P)utlMalloc(sizeof(utlEventHandlerNode_T))) == NULL)
	{

		/*--- clean ---*/
		(void)utlReleaseExclusiveAccess(&events_semaphore);

		return utlFAILED;
	}

	/*--- initialize the new event handler node ---*/
	node_p->next_p               = NULL;
	node_p->type                 = utlEVENT_HANDLER_TYPE_IDLE;
	node_p->priority             = priority;
	node_p->enable               = true;
	node_p->handler_id           = handler_id;
	node_p->u.period             = *period_p;
	node_p->call_back.function_p = function_p;
	node_p->call_back.arg_p      = arg_p;

	/*--- add to list of known idle handlers (insertion-sort) ---*/
	{
		utlEventHandlerNode_P curr_p;
		utlEventHandlerNode_P prev_p;

		/*--- find where to insert 'node' into the idle-handlers list (order from smallest-to-largest
		      timeout period, and from highest-to-lowest priority for entries having the same timeout
		      period) */
		prev_p = NULL;
		curr_p = (utlEventHandlerNode_P)idle_handlers.head_p;
		while (curr_p != NULL)
		{
			if (                                                                   (curr_p->u.period.seconds     <  node_p->u.period.seconds)     ||
											       ((curr_p->u.period.seconds     == node_p->u.period.seconds)     && ((curr_p->u.period.nanoseconds <  node_p->u.period.nanoseconds) ||
												((curr_p->u.period.nanoseconds == node_p->u.period.nanoseconds) && (curr_p->priority             >= node_p->priority)))))
			{
				prev_p = curr_p;
				curr_p = curr_p->next_p;
			}
			else
				break;
		}

		/*--- link node into idle-handlers list ---*/
		(void)utlPutNode(&idle_handlers, utlEventHandlerNode_T, prev_p, node_p);
	}

	utlTrace(utlTRACE_EVENT_HANDLER,
		 utlPrintTrace("added `idle' event handler (HID=%u), called after %d.%09d secs\n",
			       (unsigned int)node_p->handler_id,
			       node_p->u.period.seconds,
			       node_p->u.period.nanoseconds);
		 );

	/*--- release exclusive-access ---*/
	if (utlReleaseExclusiveAccess(&events_semaphore) != utlSUCCESS)
		return utlFAILED;

	return handler_id;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	_utlEstablishSignaling(signal)
* INPUT
*	signal == the signal to establish a handler for.
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*	Sets up signal handling for the specified signal (if not already done).
*---------------------------------------------------------------------------*/
static utlReturnCode_T _utlEstablishSignaling(int signal)
{
	if (plumbing.initialized == false)
		if (utlInitSignalPipe() != utlSUCCESS)
			return utlFAILED;

	/*--- Check if signal is already established or is used for something else ---*/
	{
		struct sigaction curr_action;

		if (sigemptyset(&(curr_action.sa_mask)) != 0)
		{
			utlError("Call to %s() failed, %s", "sigemptyset", strerror(errno));
			return utlFAILED;
		}

		if (sigaction(signal, NULL, &curr_action) != 0)
		{
			utlError("Call to %s() failed, %s", "sigaction", strerror(errno));
			return utlFAILED;
		}

		/*--- if already established ---*/
		if (curr_action.sa_handler == _utlEventHandlerSignal)
			return utlSUCCESS;

		/*--- if used for something else ---*/
		if ((curr_action.sa_handler != SIG_IGN) &&
		    (curr_action.sa_handler != SIG_DFL))
		{
			utlError("Signal resource %s already in use", _utlSignalToString(signal));

			/*--- clean ---*/
			(void)close(plumbing.write_fd);
			(void)close(plumbing.read_fd);

			return utlFAILED;
		}
	}

	/*--- Establish the signal handler ---*/
	{
		struct sigaction action;

		(void)memset(&action, 0, sizeof(action));
		action.sa_handler = _utlEventHandlerSignal;
		action.sa_flags   = SA_RESTART;

		if (sigemptyset(&(action.sa_mask)) != 0)
		{
			utlError("Call to %s() failed, %s", "sigemptyset", strerror(errno));
			return utlFAILED;
		}

		if (sigaction(signal, &action, NULL) != 0)
		{
			utlError("Call to %s() failed, %s", "sigaction", strerror(errno));
			return utlFAILED;
		}
	}

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlSetSignalEventHandler(signal, priority, function, arg)
* INPUT
*          signal == the UNIX/Linux signal of interest
*        priority == the handler priority
*      function_p == pointer to the event call-back function
*           arg_p == user-defined function argument
* OUTPUT
*	none
* RETURNS
*	An event handler ID for success, utlFAILED for failure
* DESCRIPTION
*	Arranges for `function_p' to be called whenever the specified `signal'
*      occurs.  If `function_p' is NULL, deletes all previously set call-back
*      functions for `signal'.  The actual deletion is deferred until the
*      event handler loop processes the next round of events.
*
*      Note that there can be zero or more signal-event call-back functions
*      defined.
*---------------------------------------------------------------------------*/
utlEventHandlerId_T utlSetSignalEventHandler(const int signal,
					     const utlEventHandlerPriority_T priority,
					     const utlEventHandlerFunction_P function_p,
					     void                      *arg_p)
{
	utlEventHandlerId_T handler_id;
	utlEventHandlerNode_P node_p;
	utlEventHandlerType_T type;

	/*---  check parameters (and convert signal to a type) ---*/
	switch (signal)
	{
	case SIGHUP:    type = utlEVENT_HANDLER_TYPE_SIGHUP;    break;
	case SIGINT:    type = utlEVENT_HANDLER_TYPE_SIGINT;    break;
	case SIGQUIT:   type = utlEVENT_HANDLER_TYPE_SIGQUIT;   break;
	case SIGILL:    type = utlEVENT_HANDLER_TYPE_SIGILL;    break;
	case SIGTRAP:   type = utlEVENT_HANDLER_TYPE_SIGTRAP;   break;
	case SIGABRT:   type = utlEVENT_HANDLER_TYPE_SIGABRT;   break;
	case SIGFPE:    type = utlEVENT_HANDLER_TYPE_SIGFPE;    break;
	case SIGKILL:   type = utlEVENT_HANDLER_TYPE_SIGKILL;   break;
	case SIGBUS:    type = utlEVENT_HANDLER_TYPE_SIGBUS;    break;
	case SIGSEGV:   type = utlEVENT_HANDLER_TYPE_SIGSEGV;   break;
	case SIGSYS:    type = utlEVENT_HANDLER_TYPE_SIGSYS;    break;
	case SIGPIPE:   type = utlEVENT_HANDLER_TYPE_SIGPIPE;   break;
	case SIGALRM:   type = utlEVENT_HANDLER_TYPE_SIGALRM;   break;
	case SIGTERM:   type = utlEVENT_HANDLER_TYPE_SIGTERM;   break;
	case SIGURG:    type = utlEVENT_HANDLER_TYPE_SIGURG;    break;
	case SIGSTOP:   type = utlEVENT_HANDLER_TYPE_SIGSTOP;   break;
	case SIGTSTP:   type = utlEVENT_HANDLER_TYPE_SIGTSTP;   break;
	case SIGCONT:   type = utlEVENT_HANDLER_TYPE_SIGCONT;   break;
	case SIGCHLD:   type = utlEVENT_HANDLER_TYPE_SIGCHLD;   break;
	case SIGTTIN:   type = utlEVENT_HANDLER_TYPE_SIGTTIN;   break;
	case SIGTTOU:   type = utlEVENT_HANDLER_TYPE_SIGTTOU;   break;
	case SIGIO:     type = utlEVENT_HANDLER_TYPE_SIGIO;     break;
	case SIGXCPU:   type = utlEVENT_HANDLER_TYPE_SIGXCPU;   break;
	case SIGXFSZ:   type = utlEVENT_HANDLER_TYPE_SIGXFSZ;   break;
	case SIGVTALRM: type = utlEVENT_HANDLER_TYPE_SIGVTALRM; break;
	case SIGPROF:   type = utlEVENT_HANDLER_TYPE_SIGPROF;   break;
	case SIGWINCH:  type = utlEVENT_HANDLER_TYPE_SIGWINCH;  break;
	case SIGUSR1:   type = utlEVENT_HANDLER_TYPE_SIGUSR1;   break;
	case SIGUSR2:   type = utlEVENT_HANDLER_TYPE_SIGUSR2;   break;
/*	case SIGPOLL:   type = utlEVENT_HANDLER_TYPE_SIGPOLL;   break; */
/*	case SIGIOT:    type = utlEVENT_HANDLER_TYPE_SIGIOT;    break; */
/*	case SIGSTKFLT: type = utlEVENT_HANDLER_TYPE_SIGSTKFLT; break; */
/*	case SIGCLD:    type = utlEVENT_HANDLER_TYPE_SIGCLD;    break; */
/*	case SIGPWR:    type = utlEVENT_HANDLER_TYPE_SIGPWR;    break; */
/*	case SIGUNUSED: type = utlEVENT_HANDLER_TYPE_SIGUNUSED; break; */

	default:
		return utlFAILED;
	}
	switch (priority)
	{
	case utlEVENT_HANDLER_PRIORITY_CRITICAL:
	case utlEVENT_HANDLER_PRIORITY_HIGH:
	case utlEVENT_HANDLER_PRIORITY_MEDIUM:
	case utlEVENT_HANDLER_PRIORITY_LOW:    break;

	default:
		return utlFAILED;
	}
	utlAssert(function_p != NULL);

	/*--- mutex initialization ---*/
	if (!utlIsSemaphoreInitialized(&events_semaphore))
	{
		if (utlInitSemaphore(&events_semaphore, utlSEMAPHORE_ATTR_SHARING_ENABLE |
				     utlSEMAPHORE_ATTR_NESTING_ENABLE) != utlSUCCESS)
			return utlFAILED;
	}

	/*--- acquire exclusive-access ---*/
	if (utlAcquireExclusiveAccess(&events_semaphore) != utlSUCCESS)
		return utlFAILED;

	/*--- setup signaling ---*/
	if (_utlEstablishSignaling(signal) != utlSUCCESS)
	{

		/*--- clean ---*/
		(void)utlReleaseExclusiveAccess(&events_semaphore);

		return utlFAILED;
	}

	if ((handler_id = _utlFindUnusedHandlerId()) == (utlEventHandlerId_T)utlFAILED)
	{

		/*--- clean ---*/
		(void)utlReleaseExclusiveAccess(&events_semaphore);

		return utlFAILED;
	}

	if ((node_p = (utlEventHandlerNode_P)utlMalloc(sizeof(utlEventHandlerNode_T))) == NULL)
	{

		/*--- clean ---*/
		(void)utlReleaseExclusiveAccess(&events_semaphore);

		return utlFAILED;
	}

	/*--- initialize the new event handler node ---*/
	node_p->next_p               = NULL;
	node_p->type                 = type;
	node_p->priority             = priority;
	node_p->enable               = true;
	node_p->handler_id           = handler_id;
	node_p->u.signal             = signal;
	node_p->call_back.function_p = function_p;
	node_p->call_back.arg_p      = arg_p;

	/*--- add to list of known signal handlers ---*/
	(void)utlPutTailNode(&signal_handlers, utlEventHandlerNode_T, node_p);

	utlTrace(utlTRACE_EVENT_HANDLER,
		 utlPrintTrace("added `signal' event handler (HID=%u), for signal %s\n",
			       (unsigned int)node_p->handler_id,
			       _utlSignalToString(signal));
		 );

	/*--- release exclusive-access ---*/
	if (utlReleaseExclusiveAccess(&events_semaphore) != utlSUCCESS)
		return utlFAILED;

	return handler_id;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlSetExitEventHandler(priority, function, arg)
* INPUT
*        priority == the handler priority
*      function_p == pointer to the event call-back function
*           arg_p == user-defined function argument
* OUTPUT
*	none
* RETURNS
*	An event handler ID for success, utlFAILED for failure
* DESCRIPTION
*	Arranges for `function_p' to be called whenever the application exits.
*      If `function_p' is NULL, deletes all previously set call-back functions.
*      The actual deletion is deferred until the event handler loop processes
*      the next round of events.
*
*      Note that there can be zero or more exit-event call-back functions
*      defined.
*---------------------------------------------------------------------------*/
utlEventHandlerId_T utlSetExitEventHandler(const utlEventHandlerPriority_T priority,
					   const utlEventHandlerFunction_P function_p,
					   void                      *arg_p)
{
	static bool at_exit_initialized = false;

	utlEventHandlerId_T handler_id;
	utlEventHandlerNode_P node_p;

	switch (priority)
	{
	case utlEVENT_HANDLER_PRIORITY_CRITICAL:
	case utlEVENT_HANDLER_PRIORITY_HIGH:
	case utlEVENT_HANDLER_PRIORITY_MEDIUM:
	case utlEVENT_HANDLER_PRIORITY_LOW:    break;

	default:
		return utlFAILED;
	}
	utlAssert(function_p != NULL);

	/*--- mutex initialization ---*/
	if (!utlIsSemaphoreInitialized(&events_semaphore))
	{
		if (utlInitSemaphore(&events_semaphore, utlSEMAPHORE_ATTR_SHARING_ENABLE |
				     utlSEMAPHORE_ATTR_NESTING_ENABLE) != utlSUCCESS)
			return utlFAILED;
	}

	/*--- acquire exclusive-access ---*/
	if (utlAcquireExclusiveAccess(&events_semaphore) != utlSUCCESS)
		return utlFAILED;

	/*--- setup exit handling (if not yet setup) ---*/
	if (at_exit_initialized != true)
	{
		if (atexit(_utlEventHandlerExit) != 0)
		{
			utlError("Call to %s() failed, %s", "atexit", strerror(errno));

			/*--- clean ---*/
			(void)utlAcquireExclusiveAccess(&events_semaphore);

			return utlFAILED;
		}

		at_exit_initialized = true;
	}

	if ((handler_id = _utlFindUnusedHandlerId()) == (utlEventHandlerId_T)utlFAILED)
	{

		/*--- clean ---*/
		(void)utlAcquireExclusiveAccess(&events_semaphore);

		return utlFAILED;
	}

	if ((node_p = (utlEventHandlerNode_P)utlMalloc(sizeof(utlEventHandlerNode_T))) == NULL)
	{

		/*--- clean ---*/
		(void)utlAcquireExclusiveAccess(&events_semaphore);

		return utlFAILED;
	}

	/*--- initialize the new event handler node ---*/
	node_p->next_p               = NULL;
	node_p->type                 = utlEVENT_HANDLER_TYPE_EXIT;
	node_p->priority             = priority;
	node_p->enable               = true;
	node_p->handler_id           = handler_id;
	node_p->u.exit               = 0;
	node_p->call_back.function_p = function_p;
	node_p->call_back.arg_p      = arg_p;

	/*--- add to list of known exit handlers ---*/
	(void)utlPutTailNode(&exit_handlers, utlEventHandlerNode_T, node_p);

	utlTrace(utlTRACE_EVENT_HANDLER,
		 utlPrintTrace("added `exit' event handler (HID=%u)\n",
			       (unsigned int)node_p->handler_id);
		 );

	/*--- release exclusive-access ---*/
	if (utlReleaseExclusiveAccess(&events_semaphore) != utlSUCCESS)
		return utlFAILED;

	return handler_id;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlExitEventLoop()
* INPUT
*      none
* OUTPUT
*	none
* RETURNS
*	nothing
* DESCRIPTION
*	Arranges for the event handler loop to exit the next time it attempts
*      to process events.
*---------------------------------------------------------------------------*/
void utlExitEventLoop(void)
{
	running = false;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlEventLoop(block)
* INPUT
*      block == block, handling events? (vs only polling once, then returning)
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*	The main event handler loop.
*
*      Note that if an event handler function returns utlFAILED, this function
*      will exit and return utlFAILED.
*---------------------------------------------------------------------------*/
utlReturnCode_T utlEventLoop(const bool block)
{
	running = true;

	/*--- mutex initialization ---*/
	if (!utlIsSemaphoreInitialized(&events_semaphore))
	{
		if (utlInitSemaphore(&events_semaphore, utlSEMAPHORE_ATTR_SHARING_ENABLE |
				     utlSEMAPHORE_ATTR_NESTING_ENABLE) != utlSUCCESS)
			return utlFAILED;
	}

	for (;; )
	{
		fd_set read_fds;
		fd_set write_fds;
		fd_set except_fds;
		int highest_fd;
		utlEventHandlerNode_P node_p;
		int select_rv;
		utlEventHandlerType_T event_type;
		utlEventHandlerPriority_T priority;

		/*--- acquire exclusive-access ---*/
		if (utlAcquireExclusiveAccess(&events_semaphore) != utlSUCCESS)
			return utlFAILED;

		/*-- were any handlers deleted? ---*/
		if (deleted_pending != utlDELETED_NONE)
		{
			utlEventHandlerNode_P prev_p;

			/*--- clean-out deleted fd handlers ---*/
			prev_p = NULL;
			for (node_p = (utlEventHandlerNode_P)fd_handlers.head_p; node_p != NULL; )
			{

				/*--- delete node if it is a zombie ---*/
				if (node_p->type == utlEVENT_HANDLER_TYPE_ZOMBIE)
				{
					utlEventHandlerNode_P deleted_p;

					if ((deleted_p = utlGetNode(&fd_handlers, utlEventHandlerNode_T, prev_p, node_p)) != NULL)
					{
						utlFree(deleted_p);
					}

					/*--- advance to node following deleted node ---*/
					if (prev_p == NULL) node_p = (utlEventHandlerNode_P)fd_handlers.head_p;
					else node_p = prev_p->next_p;

					continue;
				}

				/*--- advance to next node ---*/
				prev_p = node_p;
				node_p = node_p->next_p;
			}

			/*--- clean-out deleted idle handlers ---*/
			prev_p = NULL;
			for (node_p = (utlEventHandlerNode_P)idle_handlers.head_p; node_p != NULL; )
			{

				/*--- delete node if it is a zombie ---*/
				if (node_p->type == utlEVENT_HANDLER_TYPE_ZOMBIE)
				{
					utlEventHandlerNode_P deleted_p;

					if ((deleted_p = utlGetNode(&idle_handlers, utlEventHandlerNode_T, prev_p, node_p)) != NULL)
					{
						utlFree(deleted_p);
					}

					/*--- advance to node following deleted node ---*/
					if (prev_p == NULL) node_p = (utlEventHandlerNode_P)idle_handlers.head_p;
					else node_p = prev_p->next_p;

					continue;
				}

				/*--- advance to next node ---*/
				prev_p = node_p;
				node_p = node_p->next_p;
			}

			/*--- clean-out deleted signal handlers ---*/
			prev_p = NULL;
			for (node_p = (utlEventHandlerNode_P)signal_handlers.head_p; node_p != NULL; )
			{

				/*--- delete node if it is a zombie ---*/
				if (node_p->type == utlEVENT_HANDLER_TYPE_ZOMBIE)
				{
					utlEventHandlerNode_P deleted_p;

					if ((deleted_p = utlGetNode(&signal_handlers, utlEventHandlerNode_T, prev_p, node_p)) != NULL)
					{
						utlFree(deleted_p);
					}

					/*--- advance to node following deleted node ---*/
					if (prev_p == NULL) node_p = (utlEventHandlerNode_P)signal_handlers.head_p;
					else node_p = prev_p->next_p;

					continue;
				}

				/*--- advance to next node ---*/
				prev_p = node_p;
				node_p = node_p->next_p;
			}

			/*--- clean-out deleted exit handlers ---*/
			prev_p = NULL;
			for (node_p = (utlEventHandlerNode_P)exit_handlers.head_p; node_p != NULL; )
			{

				/*--- delete node if it is a zombie ---*/
				if (node_p->type == utlEVENT_HANDLER_TYPE_ZOMBIE)
				{
					utlEventHandlerNode_P deleted_p;

					if ((deleted_p = utlGetNode(&exit_handlers, utlEventHandlerNode_T, prev_p, node_p)) != NULL)
					{
						utlFree(deleted_p);
					}

					/*--- advance to node following deleted node ---*/
					if (prev_p == NULL) node_p = (utlEventHandlerNode_P)exit_handlers.head_p;
					else node_p = prev_p->next_p;

					continue;
				}

				/*--- advance to next node ---*/
				prev_p = node_p;
				node_p = node_p->next_p;
			}

			deleted_pending = 0;
		}

		/*--- initialize fd sets ---*/
		highest_fd = 0;
		FD_ZERO(  &read_fds);
		FD_ZERO( &write_fds);
		FD_ZERO(&except_fds);
		for (node_p = (utlEventHandlerNode_P)fd_handlers.head_p; node_p != NULL; node_p = node_p->next_p)
		{
			if (node_p->enable != true)
				continue;

			if (highest_fd < node_p->u.fd)
				highest_fd = node_p->u.fd;
			switch (node_p->type)
			{
			case utlEVENT_HANDLER_TYPE_READ:                 FD_SET(node_p->u.fd,   &read_fds);  break;
			case utlEVENT_HANDLER_TYPE_WRITE:                FD_SET(node_p->u.fd,  &write_fds);  break;
			case utlEVENT_HANDLER_TYPE_READ_WRITE:           FD_SET(node_p->u.fd,   &read_fds);
				FD_SET(node_p->u.fd,  &write_fds);  break;
			case utlEVENT_HANDLER_TYPE_EXCEPTION:            FD_SET(node_p->u.fd, &except_fds);  break;
			case utlEVENT_HANDLER_TYPE_EXCEPTION_READ:       FD_SET(node_p->u.fd,   &read_fds);
				FD_SET(node_p->u.fd, &except_fds);  break;
			case utlEVENT_HANDLER_TYPE_EXCEPTION_WRITE:      FD_SET(node_p->u.fd,  &write_fds);
				FD_SET(node_p->u.fd, &except_fds);  break;
			case utlEVENT_HANDLER_TYPE_EXCEPTION_READ_WRITE: FD_SET(node_p->u.fd,   &read_fds);
				FD_SET(node_p->u.fd,  &write_fds);
				FD_SET(node_p->u.fd, &except_fds);  break;

			case utlEVENT_HANDLER_TYPE_ZOMBIE: break;

			default:
				utlError("Default case reached with %d", node_p->type);

				/*--- clean ---*/
				(void)utlReleaseExclusiveAccess(&events_semaphore);

				return utlFAILED;
			}
		}

		/*--- release exclusive-access ---*/
		if (utlReleaseExclusiveAccess(&events_semaphore) != utlSUCCESS)
			return utlFAILED;

		/*--- should we just poll for any events... ---*/
		if (block == false)
		{

			/*--- unblocked check for fd events ---*/
			for (;; )
			{
				struct timeval timeout;

				timeout.tv_sec  = 0;
				timeout.tv_usec = 0;
				if ((select_rv = select(highest_fd + 1, &read_fds, &write_fds, &except_fds, &timeout)) >= 0)
					break;

				if (errno != EINTR)
				{
					utlError("Call to %s() failed, %s", "select", strerror(errno));
					return utlFAILED;
				}
			}

			/*--- else handle only fd events (since no idle-time-out event handlers are defined)... ---*/
		}
		else if (utlIsListEmpty(idle_handlers))
		{

			/*--- block waiting for fd events ---*/
			for (;; )
			{
				if ((select_rv = select(highest_fd + 1, &read_fds, &write_fds, &except_fds, NULL)) >= 0)
					break;

				if (errno != EINTR)
				{
					utlError("Call to %s() failed, %s", "select", strerror(errno));
					return utlFAILED;
				}
			}

			/*--- else handle both fd and idle-time-out events... ---*/
		}
		else
		{
			struct timespec elapsed;

			elapsed.tv_sec  = 0;
			elapsed.tv_nsec = 0;

			select_rv = 0;

			/*--- acquire exclusive-access ---*/
			if (utlAcquireExclusiveAccess(&events_semaphore) != utlSUCCESS)
				return utlFAILED;

			/*--- for each idle handler... ---*/
			for (node_p = (utlEventHandlerNode_P)idle_handlers.head_p; node_p != NULL; node_p = node_p->next_p)
			{
				struct timespec period;
				struct timeval timeout;
				/*--- skip node if it is a zombie ---*/
				if (node_p->type == utlEVENT_HANDLER_TYPE_ZOMBIE)
					continue;

				utlAssert(node_p->type == utlEVENT_HANDLER_TYPE_IDLE);

				/*--- convert idle period to a timeval struct ---*/
				period.tv_sec  = node_p->u.period.seconds;
				period.tv_nsec = node_p->u.period.nanoseconds;

				/*--- compute next expiry time ---*/
				timeout.tv_sec = period.tv_sec - elapsed.tv_sec;
				if (period.tv_nsec < elapsed.tv_nsec)
				{
					timeout.tv_sec  -= 1;
					timeout.tv_usec = (period.tv_nsec + 1000000000 - elapsed.tv_nsec)/1000;
				}
				else
					timeout.tv_usec = (period.tv_nsec - elapsed.tv_nsec)/1000;

				/*--- advance elapsed idle-time ---*/
				elapsed = period;

				/*--- release exclusive-access, there is the possibility that new
				      idle handlers will get added to the list while we're blocked
				      on the following select(), but we'll just ignore them if
				      their timeoout is smaller than the current timeout, and
				      process them if the timeout is greater */
				if (utlReleaseExclusiveAccess(&events_semaphore) != utlSUCCESS)
					return utlFAILED;

				/*--- wait for either fd or idle-time-out events ---*/
				for (;; )
				{
					if ((select_rv = select(highest_fd + 1, &read_fds, &write_fds, &except_fds, &timeout)) >= 0)
						break;

					if (errno != EINTR)
					{
						utlError("Call to %s() failed, %s", "select", strerror(errno));

						/*--- clean ---*/
						(void)utlReleaseExclusiveAccess(&events_semaphore);

						return utlFAILED;
					}
				}

				/*--- re-acquire exclusive-access ---*/
				if (utlAcquireExclusiveAccess(&events_semaphore) != utlSUCCESS)
					return utlFAILED;

				/*--- if an fd-event occurred, exit this idle-time-out loop ---*/
				if (select_rv > 0)
					break;

				utlTrace(utlTRACE_EVENT_HANDLER,
					 utlPrintTrace("`idle' event after %d.%09d secs for handler 0x%x\n",
						       node_p->u.period.seconds,
						       node_p->u.period.nanoseconds,
						       (unsigned int)node_p->call_back.function_p);
					 );

				/*--- if we get to here, an idle-time-out event occurred, so invoke the idle handler ---*/
				if ((*(node_p->call_back.function_p))(node_p->type, utlEVENT_HANDLER_TYPE_IDLE, -1, &(node_p->u.period), node_p->call_back.arg_p) != utlSUCCESS)
				{
					utlError("Callback function 0x%08x failed", node_p->call_back.function_p);

					/*--- clean ---*/
					(void)utlReleaseExclusiveAccess(&events_semaphore);

					return utlFAILED;
				}

				/*--- if we were asked to exit the event loop ---*/
				if (running == false)
				{

					/*--- release exclusive-access ---*/
					if (utlReleaseExclusiveAccess(&events_semaphore) != utlSUCCESS)
						return utlFAILED;

					return utlSUCCESS;
				}

				/*--- re-initialize fd sets ---*/
				{
					utlEventHandlerNode_P node_p;

					highest_fd = 0;
					FD_ZERO(  &read_fds);
					FD_ZERO( &write_fds);
					FD_ZERO(&except_fds);
					for (node_p = (utlEventHandlerNode_P)fd_handlers.head_p; node_p != NULL; node_p = node_p->next_p)
					{
						if (node_p->enable != true)
							continue;

						if (highest_fd < node_p->u.fd)
							highest_fd = node_p->u.fd;

						switch (node_p->type)
						{
						case utlEVENT_HANDLER_TYPE_READ:                 FD_SET(node_p->u.fd,   &read_fds);  break;
						case utlEVENT_HANDLER_TYPE_WRITE:                FD_SET(node_p->u.fd,  &write_fds);  break;
						case utlEVENT_HANDLER_TYPE_READ_WRITE:           FD_SET(node_p->u.fd,   &read_fds);
							FD_SET(node_p->u.fd,  &write_fds);  break;
						case utlEVENT_HANDLER_TYPE_EXCEPTION:            FD_SET(node_p->u.fd, &except_fds);  break;
						case utlEVENT_HANDLER_TYPE_EXCEPTION_READ:       FD_SET(node_p->u.fd,   &read_fds);
							FD_SET(node_p->u.fd, &except_fds);  break;
						case utlEVENT_HANDLER_TYPE_EXCEPTION_WRITE:      FD_SET(node_p->u.fd,  &write_fds);
							FD_SET(node_p->u.fd, &except_fds);  break;
						case utlEVENT_HANDLER_TYPE_EXCEPTION_READ_WRITE: FD_SET(node_p->u.fd,   &read_fds);
							FD_SET(node_p->u.fd,  &write_fds);
							FD_SET(node_p->u.fd, &except_fds);  break;
						case utlEVENT_HANDLER_TYPE_ZOMBIE: break;

						default:
							utlError("Default case reached with %d", node_p->type);

							/*--- clean ---*/
							(void)utlReleaseExclusiveAccess(&events_semaphore);

							return utlFAILED;
						}
					}
				}
			}

			/*--- release exclusive-access ---*/
			if (utlReleaseExclusiveAccess(&events_semaphore) != utlSUCCESS)
				return utlFAILED;

			/*--- no more idle handlers left to process? ---*/
			if (select_rv == 0)
			{

				/*--- wait only for fd events ---*/
				for (;; )
				{
					if ((select_rv = select(highest_fd + 1, &read_fds, &write_fds, &except_fds, NULL)) >= 0)
						break;

					if (errno != EINTR)
					{
						utlError("Call to %s() failed, %s", "select", strerror(errno));
						return utlFAILED;
					}
				}
			}
		}

		/*--- acquire exclusive-access ---*/
		if (utlAcquireExclusiveAccess(&events_semaphore) != utlSUCCESS)
			return utlFAILED;

		/*--- implement round-robin fd event handling ---*/
		{
			utlEventHandlerNode_P node_p;

			if ((node_p = utlGetHeadNode(&fd_handlers, utlEventHandlerNode_T)) != NULL)
				(void)utlPutTailNode(&fd_handlers, utlEventHandlerNode_T, node_p);
		}

		utlTrace(utlTRACE_EVENT_HANDLER,
			 utlPrintTrace("fd event on %d fd(s)\n", select_rv);
			 );

		/*--- for each possible priority level... ---*/
		for (priority = utlEVENT_HANDLER_PRIORITY_INTERNAL; priority >= utlEVENT_HANDLER_PRIORITY_LOW; priority--)
		{

			/*--- for each fd handler... ---*/
			for (node_p = (utlEventHandlerNode_P)fd_handlers.head_p; node_p != NULL; node_p = node_p->next_p)
			{

				/*--- wrong priority? ---*/
				if (node_p->priority != priority)
					continue;

				/*--- skip node if it is a zombie ---*/
				if (node_p->type == utlEVENT_HANDLER_TYPE_ZOMBIE)
					continue;

				/*--- invoke call-back function if select knew about this 'fd' and there was activity on it... */
				switch (node_p->type)
				{
				case utlEVENT_HANDLER_TYPE_READ:
					if ((node_p->u.fd <= highest_fd) && (FD_ISSET(node_p->u.fd, &read_fds)))
					{

						utlTrace(utlTRACE_EVENT_HANDLER,
							 utlPrintTrace("`read' event on fd %d for handler 0x%x\n",
								       node_p->u.fd,
								       (unsigned int)node_p->call_back.function_p);
							 );

						event_type = utlEVENT_HANDLER_TYPE_READ;

						if ((*(node_p->call_back.function_p))(node_p->type, event_type, node_p->u.fd, NULL, node_p->call_back.arg_p) != utlSUCCESS)
						{
							utlError("Callback function 0x%08x failed", node_p->call_back.function_p);

							/*--- clean ---*/
							(void)utlReleaseExclusiveAccess(&events_semaphore);

							return utlFAILED;
						}

						select_rv--;
					}
					break;

				case utlEVENT_HANDLER_TYPE_WRITE:
					if ((node_p->u.fd <= highest_fd) && (FD_ISSET(node_p->u.fd, &write_fds)))
					{

						utlTrace(utlTRACE_EVENT_HANDLER,
							 utlPrintTrace("`write' event on fd %d for handler 0x%x\n",
								       node_p->u.fd,
								       (unsigned int)node_p->call_back.function_p);
							 );

						event_type = utlEVENT_HANDLER_TYPE_WRITE;

						if ((*(node_p->call_back.function_p))(node_p->type, event_type, node_p->u.fd, NULL, node_p->call_back.arg_p) != utlSUCCESS)
						{
							utlError("Callback function 0x%08x failed", node_p->call_back.function_p);

							/*--- clean ---*/
							(void)utlReleaseExclusiveAccess(&events_semaphore);

							return utlFAILED;
						}

						select_rv--;
					}
					break;

				case utlEVENT_HANDLER_TYPE_READ_WRITE:
					if ((node_p->u.fd <= highest_fd) && (FD_ISSET(node_p->u.fd,   &read_fds) ||
									     FD_ISSET(node_p->u.fd,  &write_fds)))
					{

						utlTrace(utlTRACE_EVENT_HANDLER,
							 utlPrintTrace("`read/write' event on fd %d for handler 0x%x\n",
								       node_p->u.fd,
								       (unsigned int)node_p->call_back.function_p);
							 );

						if (FD_ISSET(node_p->u.fd, &read_fds)) event_type = utlEVENT_HANDLER_TYPE_READ;
						else event_type = utlEVENT_HANDLER_TYPE_WRITE;

						if ((*(node_p->call_back.function_p))(node_p->type, event_type, node_p->u.fd, NULL, node_p->call_back.arg_p) != utlSUCCESS)
						{
							utlError("Callback function 0x%08x failed", node_p->call_back.function_p);

							/*--- clean ---*/
							(void)utlReleaseExclusiveAccess(&events_semaphore);

							return utlFAILED;
						}

						select_rv--;
					}
					break;

				case utlEVENT_HANDLER_TYPE_EXCEPTION:
					if ((node_p->u.fd <= highest_fd) && (FD_ISSET(node_p->u.fd, &except_fds)))
					{

						utlTrace(utlTRACE_EVENT_HANDLER,
							 utlPrintTrace("`exception' event on fd %d for handler 0x%x\n",
								       node_p->u.fd,
								       (unsigned int)node_p->call_back.function_p);
							 );

						event_type = utlEVENT_HANDLER_TYPE_EXCEPTION;

						if ((*(node_p->call_back.function_p))(node_p->type, event_type, node_p->u.fd, NULL, node_p->call_back.arg_p) != utlSUCCESS)
						{
							utlError("Callback function 0x%08x failed", node_p->call_back.function_p);

							/*--- clean ---*/
							(void)utlReleaseExclusiveAccess(&events_semaphore);

							return utlFAILED;
						}

						select_rv--;
					}
					break;

				case utlEVENT_HANDLER_TYPE_EXCEPTION_READ:
					if ((node_p->u.fd <= highest_fd) && (FD_ISSET(node_p->u.fd,   &read_fds) ||
									     FD_ISSET(node_p->u.fd, &except_fds)))
					{

						utlTrace(utlTRACE_EVENT_HANDLER,
							 utlPrintTrace("`exception/read' event on fd %d for handler 0x%x\n",
								       node_p->u.fd,
								       (unsigned int)node_p->call_back.function_p);
							 );

						if (FD_ISSET(node_p->u.fd, &read_fds)) event_type = utlEVENT_HANDLER_TYPE_READ;
						else event_type = utlEVENT_HANDLER_TYPE_EXCEPTION;

						if ((*(node_p->call_back.function_p))(node_p->type, event_type, node_p->u.fd, NULL, node_p->call_back.arg_p) != utlSUCCESS)
						{
							utlError("Callback function 0x%08x failed", node_p->call_back.function_p);

							/*--- clean ---*/
							(void)utlReleaseExclusiveAccess(&events_semaphore);

							return utlFAILED;
						}

						select_rv--;
					}
					break;

				case utlEVENT_HANDLER_TYPE_EXCEPTION_WRITE:
					if ((node_p->u.fd <= highest_fd) && (FD_ISSET(node_p->u.fd,  &write_fds) ||
									     FD_ISSET(node_p->u.fd, &except_fds)))
					{

						utlTrace(utlTRACE_EVENT_HANDLER,
							 utlPrintTrace("`exception/write' event on fd %d for handler 0x%x\n",
								       node_p->u.fd,
								       (unsigned int)node_p->call_back.function_p);
							 );

						if (FD_ISSET(node_p->u.fd, &write_fds)) event_type = utlEVENT_HANDLER_TYPE_WRITE;
						else event_type = utlEVENT_HANDLER_TYPE_EXCEPTION;

						if ((*(node_p->call_back.function_p))(node_p->type, event_type, node_p->u.fd, NULL, node_p->call_back.arg_p) != utlSUCCESS)
						{
							utlError("Callback function 0x%08x failed", node_p->call_back.function_p);

							/*--- clean ---*/
							(void)utlReleaseExclusiveAccess(&events_semaphore);

							return utlFAILED;
						}

						select_rv--;
					}
					break;

				case utlEVENT_HANDLER_TYPE_EXCEPTION_READ_WRITE:
					if ((node_p->u.fd <= highest_fd) && (FD_ISSET(node_p->u.fd,   &read_fds) ||
									     FD_ISSET(node_p->u.fd,  &write_fds) ||
									     FD_ISSET(node_p->u.fd, &except_fds)))
					{

						utlTrace(utlTRACE_EVENT_HANDLER,
							 utlPrintTrace("`exception/read/write' event on fd %d for handler 0x%x\n",
								       node_p->u.fd,
								       (unsigned int)node_p->call_back.function_p);
							 );

						if (FD_ISSET(node_p->u.fd,  &read_fds)) event_type = utlEVENT_HANDLER_TYPE_READ;
						else if (FD_ISSET(node_p->u.fd, &write_fds)) event_type = utlEVENT_HANDLER_TYPE_WRITE;
						else event_type = utlEVENT_HANDLER_TYPE_EXCEPTION;

						if ((*(node_p->call_back.function_p))(node_p->type, event_type, node_p->u.fd, NULL, node_p->call_back.arg_p) != utlSUCCESS)
						{
							utlError("Callback function 0x%08x failed", node_p->call_back.function_p);

							/*--- clean ---*/
							(void)utlReleaseExclusiveAccess(&events_semaphore);

							return utlFAILED;
						}

						select_rv--;
					}
					break;

				default:
					utlError("Default case reached with %d", node_p->type);

					/*--- clean ---*/
					(void)utlReleaseExclusiveAccess(&events_semaphore);

					return utlFAILED;
				}

				/*--- have all current 'select()' events been handled? ---*/
				if (select_rv == 0)
					break;
			}
		}

		/*--- release exclusive-access ---*/
		if (utlReleaseExclusiveAccess(&events_semaphore) != utlSUCCESS)
			return utlFAILED;

		/*--- if we were asked to exit the event loop ---*/
		if (running == false)
			return utlSUCCESS;

		/*--- if we were not asked to block, we're done! ---*/
		if (block == false)
			return utlSUCCESS;
	}

	/*NOTREACHED*/
	utlError("Not reached error");
	return utlFAILED;
}
#if defined(utlDEBUG) || defined(utlTEST)

/*---------------------------------------------------------------------------*
 * FUNCTION
 *	utlVStringDumpEventHandlers(v_string_p, prefix_p)
 * INPUT
 *	v_string_p == pointer to an initialized utlVString_T structure
 *        prefix_p == pointer to a prefix string
 * OUTPUT
 *	*v_string_p == the updated utlVString_T structure
 * RETURNS
 *	nothing
 * DESCRIPTION
 *	Dumps the event handlers to `v_string_p'.
 *----------------------------------------------------------------------*/
void utlVStringDumpEventHandlers(const utlVString_P v_string_p,
				 const char         *prefix_p)
{
	utlEventHandlerNode_P node_p;

	utlAssert(v_string_p != NULL);
	utlAssert(prefix_p   != NULL);

	(void)utlVStringPrintF(v_string_p, "%s: Registered Event Handlers:\n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s:            |      |                   |           |   Call Back Function\n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s:    Type    |  fd  | Period (seconds)  | Event HID |  Function  |  Argument\n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s: -----------+------+-------------------+-----------+------------+------------\n", prefix_p);

	for (node_p = (utlEventHandlerNode_P)fd_handlers.head_p; node_p != NULL; node_p = node_p->next_p)
	{
		switch (node_p->type)
		{
		case utlEVENT_HANDLER_TYPE_READ:                 (void)utlVStringPrintF(v_string_p, "%s:    Read    |", prefix_p);         break;
		case utlEVENT_HANDLER_TYPE_WRITE:                (void)utlVStringPrintF(v_string_p, "%s:    Write   |", prefix_p);         break;
		case utlEVENT_HANDLER_TYPE_READ_WRITE:           (void)utlVStringPrintF(v_string_p, "%s: Read/Write |", prefix_p);         break;
		case utlEVENT_HANDLER_TYPE_EXCEPTION:            (void)utlVStringPrintF(v_string_p, "%s: Execption  |", prefix_p);         break;
		case utlEVENT_HANDLER_TYPE_EXCEPTION_READ:       (void)utlVStringPrintF(v_string_p, "%s: Excep/Read |", prefix_p);         break;
		case utlEVENT_HANDLER_TYPE_EXCEPTION_WRITE:      (void)utlVStringPrintF(v_string_p, "%s: Excep/Write|", prefix_p);         break;
		case utlEVENT_HANDLER_TYPE_EXCEPTION_READ_WRITE: (void)utlVStringPrintF(v_string_p, "%s: Ex/Rea/Writ|", prefix_p);         break;
		case utlEVENT_HANDLER_TYPE_ZOMBIE:               (void)utlVStringPrintF(v_string_p, "%s:   Zombie   |", prefix_p);         break;
		default:                                         (void)utlVStringPrintF(v_string_p, "%s: %8d |", prefix_p, node_p->type);  break;
		}
		(void)utlVStringPrintF(v_string_p, " %4d |", node_p->u.fd);
		(void)utlVStringPrintF(v_string_p, "                   |");

		(void)utlVStringPrintF(v_string_p, " %9d |", node_p->handler_id);

		if (node_p->call_back.function_p != NULL) (void)utlVStringPrintF(v_string_p, " 0x%08x |", node_p->call_back.function_p);
		else (void)utlVStringPrintF(v_string_p, "   <NULL>   |");
		if (node_p->call_back.arg_p      != NULL) (void)utlVStringPrintF(v_string_p, " 0x%08x\n", node_p->call_back.arg_p);
		else (void)utlVStringPrintF(v_string_p, "   <NULL>\n");
	}

	for (node_p = (utlEventHandlerNode_P)idle_handlers.head_p; node_p != NULL; node_p = node_p->next_p)
	{
		switch (node_p->type)
		{
		case utlEVENT_HANDLER_TYPE_IDLE:   (void)utlVStringPrintF(v_string_p, "%s:    Idle    |", prefix_p);               break;
		case utlEVENT_HANDLER_TYPE_ZOMBIE: (void)utlVStringPrintF(v_string_p, "%s:   Zombie   |", prefix_p);               break;
		default:                           (void)utlVStringPrintF(v_string_p, "%s:    %4d    |", prefix_p, node_p->type);  break;
		}
		(void)utlVStringPrintF(v_string_p, "      |");
		(void)utlVStringPrintF(v_string_p, "%8ld.%09d |", node_p->u.period.seconds, node_p->u.period.nanoseconds);

		(void)utlVStringPrintF(v_string_p, " %9d |", node_p->handler_id);

		if (node_p->call_back.function_p != NULL) (void)utlVStringPrintF(v_string_p, " 0x%08x |", node_p->call_back.function_p);
		else (void)utlVStringPrintF(v_string_p, "   <NULL>   |");
		if (node_p->call_back.arg_p      != NULL) (void)utlVStringPrintF(v_string_p, " 0x%08x\n", node_p->call_back.arg_p);
		else (void)utlVStringPrintF(v_string_p, "   <NULL>\n");
	}

	for (node_p = (utlEventHandlerNode_P)signal_handlers.head_p; node_p != NULL; node_p = node_p->next_p)
	{
		switch (node_p->type)
		{
		case utlEVENT_HANDLER_TYPE_SIGHUP:    (void)utlVStringPrintF(v_string_p, "%s:   SIGHUP   |", prefix_p);               break;
		case utlEVENT_HANDLER_TYPE_SIGINT:    (void)utlVStringPrintF(v_string_p, "%s:   SIGINT   |", prefix_p);               break;
		case utlEVENT_HANDLER_TYPE_SIGQUIT:   (void)utlVStringPrintF(v_string_p, "%s:   SIGQUIT  |", prefix_p);               break;
		case utlEVENT_HANDLER_TYPE_SIGILL:    (void)utlVStringPrintF(v_string_p, "%s:   SIGILL   |", prefix_p);               break;
		case utlEVENT_HANDLER_TYPE_SIGTRAP:   (void)utlVStringPrintF(v_string_p, "%s:   SIGTRAP  |", prefix_p);               break;
		case utlEVENT_HANDLER_TYPE_SIGABRT:   (void)utlVStringPrintF(v_string_p, "%s:   SIGABRT  |", prefix_p);               break;
		case utlEVENT_HANDLER_TYPE_SIGFPE:    (void)utlVStringPrintF(v_string_p, "%s:   SIGFPE   |", prefix_p);               break;
		case utlEVENT_HANDLER_TYPE_SIGKILL:   (void)utlVStringPrintF(v_string_p, "%s:   SIGKILL  |", prefix_p);               break;
		case utlEVENT_HANDLER_TYPE_SIGBUS:    (void)utlVStringPrintF(v_string_p, "%s:   SIGBUS   |", prefix_p);               break;
		case utlEVENT_HANDLER_TYPE_SIGSEGV:   (void)utlVStringPrintF(v_string_p, "%s:   SIGSEGV  |", prefix_p);               break;
		case utlEVENT_HANDLER_TYPE_SIGSYS:    (void)utlVStringPrintF(v_string_p, "%s:   SIGSYS   |", prefix_p);               break;
		case utlEVENT_HANDLER_TYPE_SIGPIPE:   (void)utlVStringPrintF(v_string_p, "%s:   SIGPIPE  |", prefix_p);               break;
		case utlEVENT_HANDLER_TYPE_SIGALRM:   (void)utlVStringPrintF(v_string_p, "%s:   SIGALRM  |", prefix_p);               break;
		case utlEVENT_HANDLER_TYPE_SIGTERM:   (void)utlVStringPrintF(v_string_p, "%s:   SIGTERM  |", prefix_p);               break;
		case utlEVENT_HANDLER_TYPE_SIGURG:    (void)utlVStringPrintF(v_string_p, "%s:   SIGURG   |", prefix_p);               break;
		case utlEVENT_HANDLER_TYPE_SIGSTOP:   (void)utlVStringPrintF(v_string_p, "%s:   SIGSTOP  |", prefix_p);               break;
		case utlEVENT_HANDLER_TYPE_SIGTSTP:   (void)utlVStringPrintF(v_string_p, "%s:   SIGTSTP  |", prefix_p);               break;
		case utlEVENT_HANDLER_TYPE_SIGCONT:   (void)utlVStringPrintF(v_string_p, "%s:   SIGCONT  |", prefix_p);               break;
		case utlEVENT_HANDLER_TYPE_SIGCHLD:   (void)utlVStringPrintF(v_string_p, "%s:   SIGCHLD  |", prefix_p);               break;
		case utlEVENT_HANDLER_TYPE_SIGTTIN:   (void)utlVStringPrintF(v_string_p, "%s:   SIGTTIN  |", prefix_p);               break;
		case utlEVENT_HANDLER_TYPE_SIGTTOU:   (void)utlVStringPrintF(v_string_p, "%s:   SIGTTOU  |", prefix_p);               break;
		case utlEVENT_HANDLER_TYPE_SIGIO:     (void)utlVStringPrintF(v_string_p, "%s:   SIGIO    |", prefix_p);               break;
		case utlEVENT_HANDLER_TYPE_SIGXCPU:   (void)utlVStringPrintF(v_string_p, "%s:   SIGXCPU  |", prefix_p);               break;
		case utlEVENT_HANDLER_TYPE_SIGXFSZ:   (void)utlVStringPrintF(v_string_p, "%s:   SIGXFSZ  |", prefix_p);               break;
		case utlEVENT_HANDLER_TYPE_SIGVTALRM: (void)utlVStringPrintF(v_string_p, "%s:  SIGVTALRM |", prefix_p);               break;
		case utlEVENT_HANDLER_TYPE_SIGPROF:   (void)utlVStringPrintF(v_string_p, "%s:   SIGPROF  |", prefix_p);               break;
		case utlEVENT_HANDLER_TYPE_SIGWINCH:  (void)utlVStringPrintF(v_string_p, "%s:   SIGWINCH |", prefix_p);               break;
		case utlEVENT_HANDLER_TYPE_SIGUSR1:   (void)utlVStringPrintF(v_string_p, "%s:   SIGUSR1  |", prefix_p);               break;
		case utlEVENT_HANDLER_TYPE_SIGUSR2:   (void)utlVStringPrintF(v_string_p, "%s:   SIGUSR2  |", prefix_p);               break;
		case utlEVENT_HANDLER_TYPE_ZOMBIE:    (void)utlVStringPrintF(v_string_p, "%s:   Zombie   |", prefix_p);               break;

/*	    case utlEVENT_HANDLER_TYPE_SIGPOLL:   (void) utlVStringPrintF(v_string_p, "%s:   SIGPOLL  |", prefix_p);               break;*/
/*	    case utlEVENT_HANDLER_TYPE_SIGIOT:    (void) utlVStringPrintF(v_string_p, "%s:   SIGIOT   |", prefix_p);               break;*/
/*	    case utlEVENT_HANDLER_TYPE_SIGSTKFLT: (void) utlVStringPrintF(v_string_p, "%s:  SIGSTKFLT |", prefix_p);               break;*/
/*	    case utlEVENT_HANDLER_TYPE_SIGCLD:    (void) utlVStringPrintF(v_string_p, "%s:   SIGCLD   |", prefix_p);               break;*/
/*	    case utlEVENT_HANDLER_TYPE_SIGPWR:    (void) utlVStringPrintF(v_string_p, "%s:   SIGPWR   |", prefix_p);               break;*/
/*	    case utlEVENT_HANDLER_TYPE_SIGUNUSED: (void) utlVStringPrintF(v_string_p, "%s:  SIGUNUSED |", prefix_p);               break;*/
		default:                              (void)utlVStringPrintF(v_string_p, "%s:    %4d    |", prefix_p, node_p->type);  break;
		}
		(void)utlVStringPrintF(v_string_p, "      |");
		(void)utlVStringPrintF(v_string_p, "                   |");

		(void)utlVStringPrintF(v_string_p, " %9d |", node_p->handler_id);

		if (node_p->call_back.function_p != NULL) (void)utlVStringPrintF(v_string_p, " 0x%08x |", node_p->call_back.function_p);
		else (void)utlVStringPrintF(v_string_p, "   <NULL>   |");
		if (node_p->call_back.arg_p      != NULL) (void)utlVStringPrintF(v_string_p, " 0x%08x\n", node_p->call_back.arg_p);
		else (void)utlVStringPrintF(v_string_p, "   <NULL>\n");
	}

	for (node_p = (utlEventHandlerNode_P)exit_handlers.head_p; node_p != NULL; node_p = node_p->next_p)
	{
		switch (node_p->type)
		{
		case utlEVENT_HANDLER_TYPE_EXIT:             (void)utlVStringPrintF(v_string_p, "%s:    Exit    |", prefix_p);               break;
		case utlEVENT_HANDLER_TYPE_DIRECT_FUNC_CALL: (void)utlVStringPrintF(v_string_p, "%s:  Func Call |", prefix_p);               break;
		case utlEVENT_HANDLER_TYPE_ZOMBIE:           (void)utlVStringPrintF(v_string_p, "%s:   Zombie   |", prefix_p);               break;
		default:                                     (void)utlVStringPrintF(v_string_p, "%s:    %4d    |", prefix_p, node_p->type);  break;
		}
		(void)utlVStringPrintF(v_string_p, "      |");
		(void)utlVStringPrintF(v_string_p, "                   |");

		(void)utlVStringPrintF(v_string_p, " %9d |", node_p->handler_id);

		if (node_p->call_back.function_p != NULL) (void)utlVStringPrintF(v_string_p, " 0x%08x |", node_p->call_back.function_p);
		else (void)utlVStringPrintF(v_string_p, "   <NULL>\n");
		if (node_p->call_back.arg_p      != NULL) (void)utlVStringPrintF(v_string_p, " 0x%08x |", node_p->call_back.arg_p);
		else (void)utlVStringPrintF(v_string_p, "   <NULL>\n");
	}
}

/*---------------------------------------------------------------------------*
 * FUNCTION
 *	utlDumpEventHandlers(file_p)
 * INPUT
 *	file_p == pointer to an open file
 * OUTPUT
 *	none
 * RETURNS
 *	nothing
 * DESCRIPTION
 *	Dumps the event handlers to `file_p'.
 *----------------------------------------------------------------------*/
void utlDumpEventHandlers(FILE *file_p)
{
	utlVString_T v_string;

	utlAssert(file_p != NULL);

	utlInitVString(&v_string);

	utlVStringDumpEventHandlers(&v_string, "EventHandlers");

	utlVStringPuts(&v_string, file_p);

	utlVStringFree(&v_string);
}
#endif  /* defined(utlDEBUG) || defined(utlTEST) */

#ifdef utlTEST
/*----------------------------------------------------------------------*/
typedef struct {
	enum {
		utlMODE_NULL,
		utlMODE_EXIT_EVENT_HANDLER,
	} mode;
	size_t num_cycles;
	size_t curr_cycle;
	double last;
	double period;
} testInfo_T;

/*----------------------------------------------------------------------*
* FUNCTION
*	testFunc(id, timeout_count, arg, curr_time)
* INPUT
*                 id == timer ID
*      timeout_count == the time-out count
*	        arg_p == pointer to user-defined data
*	  curr_time_p == current time since the epoch
* OUTPUT
*	modifies the data pointed to by `arg_p'
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure.
* DESCIPTION
*	Handles timer time outs.
*----------------------------------------------------------------------*/
static utlReturnCode_T testFunc(const utlTimerId_T id,
				const unsigned long timeout_count,
				void                *arg_p,
				const utlAbsoluteTime_P2c curr_time_p)
{
	testInfo_T *test_info_p;
	double delta;

	utlAssert(arg_p       != NULL);
	utlAssert(curr_time_p != NULL);

	test_info_p = (testInfo_T *)arg_p;
	test_info_p->curr_cycle++;

	delta =             curr_time_p->seconds + (curr_time_p->nanoseconds / 1000000000.0) - test_info_p->last;
	test_info_p->last = curr_time_p->seconds + (curr_time_p->nanoseconds / 1000000000.0);

	/*--- time-out time OK? ---*/
	if ((test_info_p->curr_cycle > (size_t)1) && (fabs(test_info_p->period - delta) > 0.15))
		fprintf(stderr, "eventHandlerTest: warning, timing error = %f\n", fabs(test_info_p->period - delta));

	if (test_info_p->curr_cycle == test_info_p->num_cycles)
	{
		switch (test_info_p->mode)
		{
		case utlMODE_NULL:
			break;

		case utlMODE_EXIT_EVENT_HANDLER:
			utlExitEventLoop();
			break;
		}
	}

	return utlSUCCESS;
}

/*----------------------------------------------------------------------*
* FUNCTION
*	idleFunc(handler_type, event_type, fd, period, arg)
* INPUT
*      handler_type == handler type
*        event_type == event type
*                fd == which file descriptor
*	    period_p == idle period
*	       arg_p == pointer to user-defined data
* OUTPUT
*	modifies the data pointed to by `arg_p'
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure.
* DESCIPTION
*	Handles idle-time-out events.
*----------------------------------------------------------------------*/
static utlReturnCode_T idleFunc(const utlEventHandlerType_T handler_type,
				const utlEventHandlerType_T event_type,
				const int fd,
				const utlRelativeTime_P2c period_p,
				void                  *arg_p)
{
	testInfo_T *test_info_p;
	double n;

	utlAssert(arg_p    != NULL);
	utlAssert(fd       == -1);
	utlAssert(period_p != NULL);

	test_info_p = (testInfo_T *)arg_p;
	test_info_p->curr_cycle++;

	n =  period_p->seconds +
	    (period_p->nanoseconds / 1000000000.0);

	/*--- passed-in period OK? ---*/
	if (fabs(test_info_p->period - n) > 0.0001)
		return utlFAILED;

	return utlSUCCESS;
}

/*----------------------------------------------------------------------*
* FUNCTION
*	fdFunc(handler_type, event_type, fd, period, arg)
* INPUT
*      handler_type == handler type
*        event_type == event type
*                fd == which file descriptor
*	    period_p == idle period
*	       arg_p == pointer to user-defined data
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure.
* DESCIPTION
*	Currently does nothing.
*----------------------------------------------------------------------*/
static utlReturnCode_T fdFunc(const utlEventHandlerType_T handler_type,
			      const utlEventHandlerType_T event_type,
			      const int fd,
			      const utlRelativeTime_P2c period_p,
			      void                  *arg_p)
{
	return utlSUCCESS;
}

/*----------------------------------------------------------------------*
* FUNCTION
*	signalFunc(handler_type, event_type, fd, period_p, arg_p)
* INPUT
*      handler_type == handler type
*        event_type == event type
*                fd == which file descriptor
*	    period_p == idle period
*	       arg_p == pointer to user-defined data
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure.
* DESCIPTION
*	Prints a message.
*----------------------------------------------------------------------*/
static utlReturnCode_T signalFunc(const utlEventHandlerType_T handler_type,
				  const utlEventHandlerType_T event_type,
				  const int fd,
				  const utlRelativeTime_P2c period_p,
				  void                  *arg_p)
{
	(void)fprintf(stderr, "EventHandlerTest: signal %s event\n", _utlSignalToString(event_type));

	return utlSUCCESS;
}

/*----------------------------------------------------------------------*
* FUNCTION
*	eventHandlerTest()
* INPUT
*      none
* OUTPUT
*	none
* RETURNS
*	"true" for pass, "false" for failure
*----------------------------------------------------------------------*/
utlReturnCode_T eventHandlerTest()
{
	utlRelativeTime_T period;

	/*--- Hook up some signal handlers ---*/
	utlSetSignalEventHandler(SIGHUP,  utlEVENT_HANDLER_PRIORITY_MEDIUM, signalFunc, NULL);
	utlSetSignalEventHandler(SIGQUIT, utlEVENT_HANDLER_PRIORITY_MEDIUM, signalFunc, NULL);

	/*--- an idle event loop ---*/
	{
		testInfo_T exit_test_info;
		utlTimerId_T exit_timer_id;

		/* exit timer */
		period.seconds     = 7;
		period.nanoseconds = 0;

		exit_test_info.mode = utlMODE_EXIT_EVENT_HANDLER;
		exit_test_info.num_cycles = 1;
		exit_test_info.curr_cycle = 0;
		exit_test_info.last   = 0.0;
		exit_test_info.period =  period.seconds +
					(period.nanoseconds / 1000000000.0);

		if ((exit_timer_id = utlStartTimer(&period, exit_test_info.num_cycles, &testFunc, &exit_test_info)) == utlFAILED)
		{
			fprintf(stderr, "eventHandlerTest: utlStartTimer(1.0)\n");
			return false;
		}

		if (utlEventLoop(true) != utlSUCCESS)
		{
			fprintf(stderr, "eventHandlerTest: utlEventLoop(1.3)\n");
			return false;
		}
	}

	/*--- test "read" events using timers ---*/
	{
		testInfo_T exit_test_info;
		utlTimerId_T exit_timer_id;
		utlTimerId_T timer_id;
		testInfo_T test_info;

		/* exit timer */
		period.seconds     = 7;
		period.nanoseconds = 0;

		exit_test_info.mode = utlMODE_EXIT_EVENT_HANDLER;
		exit_test_info.num_cycles = 1;
		exit_test_info.curr_cycle = 0;
		exit_test_info.last   = 0.0;
		exit_test_info.period =  period.seconds +
					(period.nanoseconds / 1000000000.0);

		if ((exit_timer_id = utlStartTimer(&period, exit_test_info.num_cycles, &testFunc, &exit_test_info)) == utlFAILED)
		{
			fprintf(stderr, "eventHandlerTest: utlStartTimer(2.0)\n");
			return false;
		}

		/* timer to generate read events */
		period.seconds     = 1;
		period.nanoseconds = 0;

		test_info.mode = utlMODE_NULL;
		test_info.num_cycles = 3;
		test_info.curr_cycle = 0;
		test_info.last   = 0.0;
		test_info.period =  period.seconds +
				   (period.nanoseconds / 1000000000.0);

		if ((timer_id = utlStartTimer(&period, test_info.num_cycles, &testFunc, &test_info)) == utlFAILED)
		{
			fprintf(stderr, "eventHandlerTest: utlStartTimer(2.2)\n");
			return false;
		}

		if (utlEventLoop(true) != utlSUCCESS)
		{
			fprintf(stderr, "eventHandlerTest: utlEventLoop(2.3)\n");
			return false;
		}

		if ((test_info.num_cycles != 3) ||
		    (test_info.curr_cycle != 3))
		{
			fprintf(stderr, "eventHandlerTest: invalid timer data(2.4)\n");
			return false;
		}
	}

	/*--- test a single idle event ---*/
	{
		testInfo_T exit_test_info;
		utlTimerId_T exit_timer_id;
		utlEventHandlerId_T handler_id;
		testInfo_T test_info;

		/* exit timer */
		period.seconds     = 7;
		period.nanoseconds = 0;

		exit_test_info.mode = utlMODE_EXIT_EVENT_HANDLER;
		exit_test_info.num_cycles = 1;
		exit_test_info.curr_cycle = 0;
		exit_test_info.last   = 0.0;
		exit_test_info.period =  period.seconds +
					(period.nanoseconds / 1000000000.0);

		if ((exit_timer_id = utlStartTimer(&period, exit_test_info.num_cycles, &testFunc, &exit_test_info)) == utlFAILED)
		{
			fprintf(stderr, "eventHandlerTest: utlStartTimer(3.0)\n");
			return false;
		}

		/* idle handler */
		period.seconds     = 1;
		period.nanoseconds = 0;

		test_info.mode = utlMODE_NULL;
		test_info.num_cycles = 0;
		test_info.curr_cycle = 0;
		test_info.last   = 0.0;
		test_info.period =  period.seconds +
				   (period.nanoseconds / 1000000000.0);

		if ((handler_id = utlSetIdleEventHandler(&period, utlEVENT_HANDLER_PRIORITY_MEDIUM, idleFunc, &test_info)) == utlFAILED)
		{
			fprintf(stderr, "eventHandlerTest: utlSetIdleEventHandler(3.1)\n");
			return false;
		}

		if (utlEventLoop(true) != utlSUCCESS)
		{
			fprintf(stderr, "eventHandlerTest: utlEventLoop(3.2)\n");
			return false;
		}

		if ((test_info.num_cycles != 0) ||
		    (test_info.curr_cycle != 1))
		{
			fprintf(stderr, "eventHandlerTest: invalid timer data(3.3)\n");
			return false;
		}

		/* delete idle event handler */
		if (utlDeleteEventHandler(handler_id) != utlSUCCESS)
		{
			fprintf(stderr, "eventHandlerTest: utlSetIdleEventHandler(3.5)\n");
			return false;
		}
	}

	/*--- test multiple idle events ---*/
	{
		testInfo_T exit_test_info;
		utlTimerId_T exit_timer_id;
		utlEventHandlerId_T handler_id[10];
		size_t i;
		testInfo_T test_info[10];

		/* exit timer */
		period.seconds     = 7;
		period.nanoseconds = 0;

		exit_test_info.mode = utlMODE_EXIT_EVENT_HANDLER;
		exit_test_info.num_cycles = 1;
		exit_test_info.curr_cycle = 0;
		exit_test_info.last   = 0.0;
		exit_test_info.period =  period.seconds +
					(period.nanoseconds / 1000000000.0);

		if ((exit_timer_id = utlStartTimer(&period, exit_test_info.num_cycles, &testFunc, &exit_test_info)) == utlFAILED)
		{
			fprintf(stderr, "eventHandlerTest: utlStartTimer(4.0)\n");
			return false;
		}

		for (i = 0; i < 10; i++)
		{
			/* idle handlers */
			switch (i)
			{
			case 0: period.seconds = 0;  period.nanoseconds = 500000000;  break;
			case 1: period.seconds = 1;  period.nanoseconds = 0;          break;
			case 2: period.seconds = 1;  period.nanoseconds = 500000000;  break;
			case 3: period.seconds = 2;  period.nanoseconds = 0;          break;
			case 4: period.seconds = 2;  period.nanoseconds = 500000000;  break;
			case 5: period.seconds = 3;  period.nanoseconds = 0;          break;
			case 6: period.seconds = 3;  period.nanoseconds = 500000000;  break;
			case 7: period.seconds = 4;  period.nanoseconds = 0;          break;
			case 8: period.seconds = 4;  period.nanoseconds = 500000000;  break;
			case 9: period.seconds = 5;  period.nanoseconds = 0;          break;
			}

			test_info[i].mode = utlMODE_NULL;
			test_info[i].num_cycles = 0;
			test_info[i].curr_cycle = 0;
			test_info[i].last   = 0.0;
			test_info[i].period =  period.seconds +
					      (period.nanoseconds / 1000000000.0);

			if ((handler_id[i] = utlSetIdleEventHandler(&period, utlEVENT_HANDLER_PRIORITY_MEDIUM, idleFunc, test_info + i)) == utlFAILED)
			{
				fprintf(stderr, "eventHandlerTest: utlSetIdleEventHandler(4.1)\n");
				return false;
			}
		}

		if (utlEventLoop(true) != utlSUCCESS)
		{
			fprintf(stderr, "eventHandlerTest: utlEventLoop(4.2)\n");
			return false;
		}

		for (i = 0; i < 10; i++)
		{
			if ((test_info[i].num_cycles != 0) ||
			    (test_info[i].curr_cycle != 1))
			{
				fprintf(stderr, "eventHandlerTest: invalid timer data(4.3)\n");
				return false;
			}
		}

		/* delete idle event handlers */
		for (i = 0; i < 10; i++)
		{
			/* idle handlers */
			switch (i)
			{
			case 0: period.seconds = 0;  period.nanoseconds = 500000000;  break;
			case 1: period.seconds = 1;  period.nanoseconds = 0;          break;
			case 2: period.seconds = 1;  period.nanoseconds = 500000000;  break;
			case 3: period.seconds = 2;  period.nanoseconds = 0;          break;
			case 4: period.seconds = 2;  period.nanoseconds = 500000000;  break;
			case 5: period.seconds = 3;  period.nanoseconds = 0;          break;
			case 6: period.seconds = 3;  period.nanoseconds = 500000000;  break;
			case 7: period.seconds = 4;  period.nanoseconds = 0;          break;
			case 8: period.seconds = 4;  period.nanoseconds = 500000000;  break;
			case 9: period.seconds = 5;  period.nanoseconds = 0;          break;
			}
			if (utlDeleteEventHandler(handler_id[i]) != utlSUCCESS)
			{
				fprintf(stderr, "eventHandlerTest: utlSetIdleEventHandler(4.4)\n");
				return false;
			}
		}
	}

	/*--- test multiple mixed idle and read events ---*/
	{
		testInfo_T exit_test_info;
		utlTimerId_T exit_timer_id;
		utlEventHandlerId_T handler_id[10];
		size_t i;
		testInfo_T test_info[10];
		utlTimerId_T timer_id;

		/* exit timer */
		period.seconds     = 10;
		period.nanoseconds = 0;

		exit_test_info.mode = utlMODE_EXIT_EVENT_HANDLER;
		exit_test_info.num_cycles = 1;
		exit_test_info.curr_cycle = 0;
		exit_test_info.last   = 0.0;
		exit_test_info.period =  period.seconds +
					(period.nanoseconds / 1000000000.0);

		if ((exit_timer_id = utlStartTimer(&period, exit_test_info.num_cycles, &testFunc, &exit_test_info)) == utlFAILED)
		{
			fprintf(stderr, "eventHandlerTest: utlStartTimer(5.0)\n");
			return false;
		}

		/* timer to generate read events */
		period.seconds     = 2;
		period.nanoseconds = 0;

		test_info[0].mode = utlMODE_NULL;
		test_info[0].num_cycles = 3;
		test_info[0].curr_cycle = 0;
		test_info[0].last   = 0.0;
		test_info[0].period =  period.seconds +
				      (period.nanoseconds / 1000000000.0);

		if ((timer_id = utlStartTimer(&period, test_info[0].num_cycles, &testFunc, test_info + 0)) == utlFAILED)
		{
			fprintf(stderr, "eventHandlerTest: utlStartTimer(5.1)\n");
			return false;
		}

		for (i = 1; i < 4; i++)
		{
			/* idle handlers */
			switch (i)
			{
			case 1: period.seconds = 0;  period.nanoseconds = 500000000;  break;
			case 2: period.seconds = 1;  period.nanoseconds = 0;          break;
			case 3: period.seconds = 1;  period.nanoseconds = 500000000;  break;
			}

			test_info[i].mode = utlMODE_NULL;
			test_info[i].num_cycles = 0;
			test_info[i].curr_cycle = 0;
			test_info[i].last   = 0.0;
			test_info[i].period =  period.seconds +
					      (period.nanoseconds / 1000000000.0);

			if ((handler_id[i] = utlSetIdleEventHandler(&period, utlEVENT_HANDLER_PRIORITY_MEDIUM, idleFunc, test_info + i)) == utlFAILED)
			{
				fprintf(stderr, "eventHandlerTest: utlSetIdleEventHandler(5.2)\n");
				return false;
			}
		}

		utlDumpEventHandlers(stdout);

		if (utlEventLoop(true) != utlSUCCESS)
		{
			fprintf(stderr, "eventHandlerTest: utlEventLoop(5.3)\n");
			return false;
		}

		for (i = 1; i < 4; i++)
		{
			if ((test_info[i].num_cycles != 0) ||
			    (test_info[i].curr_cycle != 4))
			{
				fprintf(stderr, "eventHandlerTest: invalid timer data(5.4)\n");
				return false;
			}
		}

		/* delete idle event handlers */
		for (i = 1; i < 4; i++)
		{
			/* idle handlers */
			switch (i)
			{
			case 1: period.seconds = 0;  period.nanoseconds = 500000000;  break;
			case 2: period.seconds = 1;  period.nanoseconds = 0;          break;
			case 3: period.seconds = 1;  period.nanoseconds = 500000000;  break;
			}

			if (utlDeleteEventHandler(handler_id[i]) != utlSUCCESS)
			{
				fprintf(stderr, "eventHandlerTest: utlSetIdleEventHandler(5.5)\n");
				return false;
			}
		}
	}

	/*--- test setting up and deleting multiple event handlers ---*/
	{
		size_t i;
		utlEventHandlerId_T idle_handler_id[10];
		utlEventHandlerId_T read_handler_id[10];
		utlEventHandlerId_T write_handler_id[10];
		utlEventHandlerId_T exception_handler_id[10];

		/* setup handlers */
		for (i = 0; i < 10; i++)
		{

			/* idle handler */
			switch (i)
			{
			case 0: period.seconds = 0;  period.nanoseconds = 500000000;  break;
			case 1: period.seconds = 1;  period.nanoseconds = 0;          break;
			case 2: period.seconds = 1;  period.nanoseconds = 500000000;  break;
			case 3: period.seconds = 2;  period.nanoseconds = 0;          break;
			case 4: period.seconds = 2;  period.nanoseconds = 500000000;  break;
			case 5: period.seconds = 3;  period.nanoseconds = 0;          break;
			case 6: period.seconds = 3;  period.nanoseconds = 500000000;  break;
			case 7: period.seconds = 4;  period.nanoseconds = 0;          break;
			case 8: period.seconds = 4;  period.nanoseconds = 500000000;  break;
			case 9: period.seconds = 5;  period.nanoseconds = 0;          break;
			}

			if ((idle_handler_id[i] = utlSetIdleEventHandler(&period, utlEVENT_HANDLER_PRIORITY_MEDIUM, idleFunc, NULL)) == utlFAILED)
			{
				fprintf(stderr, "eventHandlerTest: utlSetIdleEventHandler(6.1)\n");
				return false;
			}

			/* read handler */
			if ((read_handler_id[i] = utlSetFdEventHandler(utlEVENT_HANDLER_TYPE_READ, utlEVENT_HANDLER_PRIORITY_MEDIUM, i + 10, fdFunc, NULL)) == utlFAILED)
			{
				fprintf(stderr, "eventHandlerTest: utlSetFdEventHandler(6.3)\n");
				return false;
			}

			/* write handler */
			if ((write_handler_id[i] = utlSetFdEventHandler(utlEVENT_HANDLER_TYPE_WRITE, utlEVENT_HANDLER_PRIORITY_MEDIUM, i + 10, fdFunc, NULL)) == utlFAILED)
			{
				fprintf(stderr, "eventHandlerTest: utlSetFdEventHandler(6.4)\n");
				return false;
			}

			/* exception handler */
			if ((exception_handler_id[i] = utlSetFdEventHandler(utlEVENT_HANDLER_TYPE_EXCEPTION, utlEVENT_HANDLER_PRIORITY_MEDIUM, i + 10, fdFunc, NULL)) == utlFAILED)
			{
				fprintf(stderr, "eventHandlerTest: utlSetFdEventHandler(6.5)\n");
				return false;
			}
		}

		/* delete handlers */
		for (i = 0; i < 10; i++)
		{

			/* idle handler */
			switch (i)
			{
			case 0: period.seconds = 0;  period.nanoseconds = 500000000;  break;
			case 1: period.seconds = 1;  period.nanoseconds = 0;          break;
			case 2: period.seconds = 1;  period.nanoseconds = 500000000;  break;
			case 3: period.seconds = 2;  period.nanoseconds = 0;          break;
			case 4: period.seconds = 2;  period.nanoseconds = 500000000;  break;
			case 5: period.seconds = 3;  period.nanoseconds = 0;          break;
			case 6: period.seconds = 3;  period.nanoseconds = 500000000;  break;
			case 7: period.seconds = 4;  period.nanoseconds = 0;          break;
			case 8: period.seconds = 4;  period.nanoseconds = 500000000;  break;
			case 9: period.seconds = 5;  period.nanoseconds = 0;          break;
			}

			if (utlDeleteEventHandler(idle_handler_id[i]) != utlSUCCESS)
			{
				fprintf(stderr, "eventHandlerTest: utlSetIdleEventHandler(6.6)\n");
				return false;
			}

			/* read handler */
			if (utlDeleteEventHandler(read_handler_id[i]) != utlSUCCESS)
			{
				fprintf(stderr, "eventHandlerTest: utlSetFdEventHandler(6.7)\n");
				return false;
			}

			/* write handler */
			if (utlDeleteEventHandler(write_handler_id[i]) != utlSUCCESS)
			{
				fprintf(stderr, "eventHandlerTest: utlSetFdEventHandler(6.8)\n");
				return false;
			}

			/* exception handler */
			if (utlDeleteEventHandler(exception_handler_id[i]) != utlSUCCESS)
			{
				fprintf(stderr, "eventHandlerTest: utlSetFdEventHandler(6.9)\n");
				return false;
			}
		}

		if (utlEventLoop(false) != utlSUCCESS)
		{
			fprintf(stderr, "eventHandlerTest: utlEventLoop(6.10)\n");
			return false;
		}
	}

	return true;
}
#endif /* utlTEST */

