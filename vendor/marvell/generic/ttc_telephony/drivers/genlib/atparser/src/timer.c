/*****************************************************************************
* Utility Library
*
* Timer Utilities
*
* DESCRIPTION
*    The duration to timeout and the number of timeouts a timer performs is
*    configurable on a per-timer basis.  Upon timeout, an application
*    supplied call-back function is called.
*
*    This implementation converts the single UNIX SIGALRM timer into multiple
*    application timers.  This allows an application program to run more than
*    one timer at a time.
*
*    Each timer node is a single independent timer.  Timer nodes are
*    maintained in a static ordered singly linked list.  The timers that
*    will timeout the soonest are at the head of the list, the timers that
*    will timeout the latest are at the tail of the list:
*
*         .---------.
*         |tail_p---+----------------------------------.
*         |head_p---+----.                             |
*         |count= 3 |    V                             V
*         `---------'  .-----------.  .-----------.  .-----------.
*                      |  next_p---+->|  next_p---+->|  next_p---+->NULL
*                      |    :      |  |    :      |  |    :      |
*	                `-----------'  `-----------'  `-----------'
*
*    New timers are inserted into the appropriate place in the above list
*    by the function '_utlAddTimer()'.
*
*    'period' specifies how long it will take the timer to time out.
*    'num_cycles' specifies how may times the timer will cycle before
*    it stops running.  A timer can be configured to run forever by setting
*    'num_cycles' to utlTIMER_INFINITY.  A timer can be configured to
*    run only once by setting 'num_cycles' to '1'.
*
*    The function `utlTimerTimeoutHandler()' is invoked by `utlEventLoop()'
*    whenever a SIGALRM signal occurs.  This function determines which timers
*    has/have expired and then causes the the appropriate timeout functions
*    to be called
*
*    We detect host O/S time changes by comparing the timeout time stored
*    in the timer pointed to by "timers.head_p" to the current time when the
*    SIGALRM signal occurs.  If these times differ by more than
*    utlTIMER_CORRECTION_THRESHOLD, we correct the timeout times
*    of all running periodic timers to account for the changed host O/S time.
*    This implementation assumes that when the host O/S time is changed, that
*    the running setitimer() continues to run until completion as if the
*    host O/S time had not been changed.  This is currently true on
*    Mandrake Linux 7.2/8.0/9.0/9.1, HP-UX 10.2, Solaris 8.0, and
*    FreeBSD 4.4.
*****************************************************************************/

#include <errno.h>
#include <math.h>
#include <signal.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include <utlTypes.h>

#include <utlError.h>
#include <utlEventHandler.h>
#include <utlLinkedList.h>
#include <utlMalloc.h>
#include <utlSemaphore.h>
#include <utlTime.h>
#include <utlTrace.h>

#include <utlTimer.h>

/*-- Configuration ----------------------------------------------------------*/

/*--- maximum permitted setitimer timeout error (microseconds) ---*/
#define utlSETITIMER_MAX_ERR 1000

/*--- maximum timeout error (microseconds) before corrective action is taken
      to compensate for a system time change */
#define utlTIMER_CORRECTION_THRESHOLD ((long)500000)

/*--- maximum call-back function time (microseconds) before corrective action
      is taken to compensate for slow-to-return call-back functions */
#define utlTIMER_CALL_BACK_TIME_THRESHOLD ((long)1000000)

/*--- minimum time period (microseconds) ---*/
#define utlTIMER_MINIMUM_PERIOD ((utlNanosecond_T)1000)

/*--- for tracing timers ---*/
#define utlTRACE_TIMER "timers"

#define UNUSEDPARAM(param) (void)param;
/*-- Data Types -------------------------------------------------------------*/

typedef struct utlTimerNode_S *utlTimerNode_P;
typedef struct utlTimerNode_S {
	utlTimerNode_P next_p;
	utlTimerId_T id;                /* timer ID                     */
	utlRelativeTime_T period;       /* period between time outs     */
	struct timeval expiry;          /* time of next time out        */
	bool absolute;                  /* this an absolute timer?      */
	utlTimeOutCount_T num_cycles;   /* total # of time out periods  */
	utlTimeOutCount_T cycles_left;  /* current # of time-outs left  */
	bool marked_flag;               /* house keeping during timeout */
	struct {
		utlTimerFunction_P function_p;
		void               *arg_p;
	} call_back;
} utlTimerNode_T;
typedef const utlTimerNode_T *utlTimerNode_P2c;


/*-- Local Data ------------------------------------------------------------*/

static utlSemaphore_T timer_semaphore = utlNULL_SEMAPHORE;  /* protection for static timer data */

static bool initialized            = false;
static utlLinkedList_T timers                 = utlEMPTY_LINKED_LIST;
static utlTimerNode_P doing_timer_p          = NULL;            /* the timer we are currently processing    */
static utlTimerId_T next_possible_timer_id = 1;                 /* next unused timer ID                     */

static unsigned long setitimer_compensation = 0;         /* A POSIX conformant setitimer() implementation
							    never times out before the specified
							    interval: it always times out some short
							    time after the specified interval has
							    elapsed, with the timeout-error being
							    influenced by the system tick rate and
							    loading.  On some Linux implementations
							    setitimer() will occasionally time-out
							    early (violation!).  Compensation for
							    this is stored in this variable. */


/*---------------------------------------------------------------------------*
* FUNCTION
*	_utlAddTimer(node_p)
* INPUT
*	node_p == the timer node to add
* OUTPUT
*	none
* RETURNS
*	nothing
* DESCRIPTION
*	Adds one timer node to the running timer list.  An insertion sort is
*	performed to keep the timers ordered from next-to-time-out to
*      last-to-time-out.
*---------------------------------------------------------------------------*/
static void _utlAddTimer(utlTimerNode_P node_p)
{
	utlTimerNode_P curr_p;
	utlTimerNode_P prev_p;

	assert(node_p != NULL);

	/*--- find where to insert 'node' into the running timer list ---*/
	prev_p = NULL;
	curr_p = (utlTimerNode_P)timers.head_p;
	while (curr_p != NULL)
	{
		if (timercmp(&(curr_p->expiry), &(node_p->expiry), <))
		{
			prev_p = curr_p;
			curr_p = curr_p->next_p;
		}
		else
			break;
	}

	/*--- link timer instance into running timers list ---*/
	(void)utlPutNode(&timers, utlTimerNode_T, prev_p, node_p);
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	_utlRemoveTimer(node_p)
* INPUT
*	node_p == the timer node to remove
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS if the timer was removed, utlFAILED if the timer was not found
* DESCRIPTION
*	Removes one timer node from the running timer list.  First we must
*	search for the timer that we wish to remove and the timer that
*	preceeds the one we wish to remove.  Knowing this we can remove the
*	desired timer from the linked list of running timers.
*---------------------------------------------------------------------------*/
static utlReturnCode_T _utlRemoveTimer(utlTimerNode_P node_p)
{
	utlTimerNode_P curr_p;
	utlTimerNode_P prev_p;

	assert(node_p != NULL);

	/*--- search for the timer pointed to by `node_p' ---*/
	prev_p = NULL;
	curr_p = (utlTimerNode_P)timers.head_p;
	while (curr_p != NULL)
	{

		/*--- we found it, so unlink timer from running timer list ---*/
		if (curr_p == node_p)
		{
			if (utlGetNode(&timers, utlTimerNode_T, prev_p, curr_p) != node_p)
			{
				utlError("Timer list corruption");
				return utlFAILED;
			}

			return utlSUCCESS;
		}

		prev_p = curr_p;
		curr_p = curr_p->next_p;
	}

	return utlFAILED;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	_utlTimerTimeoutHandler(handler_type, event_type, fd, period, arg_p)
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
* DESCRIPTION
*	Handles one or more time outs.  This function is only invoked by
*      receipt of a SIGALRM signal.
*---------------------------------------------------------------------------*/
static utlReturnCode_T _utlTimerTimeoutHandler(const utlEventHandlerType_T handler_type,
					       const utlEventHandlerType_T event_type,
					       const int fd,
					       const utlRelativeTime_P2c period_p,
					       void                  *arg_p)
{
	UNUSEDPARAM(fd)
	UNUSEDPARAM(period_p)

	utlReturnCode_T rc;
	struct timeval curr_time;
	struct itimerval interval_timer;
	long call_back_time_threshold;

	assert(handler_type == utlEVENT_HANDLER_TYPE_SIGALRM);
	assert(event_type   == SIGALRM);
	assert(arg_p        == NULL);

	/*--- acquire exclusive access ---*/
	if (utlAcquireExclusiveAccess(&timer_semaphore) != utlSUCCESS)
		return utlFAILED;

	/*--- ensure that there is something to time out!  There is a possible
	      race condition between timers timing out and stopping timers.
	      Thus if there are no timers running, don't complain, just return */
	if (timers.node_count == (utlNodeCount_T)0)
	{

		/*--- release exclusive access ---*/
		if (utlReleaseExclusiveAccess(&timer_semaphore) != utlSUCCESS)
			return utlFAILED;

		return utlSUCCESS;
	}

	/*--- fetch current time (since epoch) ---*/
	for (;; )
	{
		if (gettimeofday(&curr_time, NULL) == 0)
			break;

		if (errno != EINTR)
		{
			utlError("Call to %s() failed, %s", "gettimeofday", strerror(errno));

			/*--- clean ---*/
			(void)utlReleaseExclusiveAccess(&timer_semaphore);

			return utlFAILED;
		}
	}

	/*--- initialize all "marked" flags ---*/
	{
		utlTimerNode_P node_p;

		for (node_p = (utlTimerNode_P)timers.head_p; node_p != NULL; node_p = node_p->next_p)
			node_p->marked_flag = false;
	}

	/*--- compensate if host date/time changed while timer was running...

	      The time-out time of the first node should match the system time when
	      the setitimer() time-out occurs.

	      Detection:

	      - When the system date/time is advanced, the running setitimer() carries on
		as usual, but upon timeout we find the system date/time is unexpectedly
		way advanced.

	      - When the system date/time is retarded, the running setitimer() carries on
		as usual, but upon timeout we find the system date/time is unexpectedly
		way in the past. */
	{
		utlTimerNode_P node_p;
		struct timeval time_diff;
		long time_err;

		/*--- the head node contains the expected timeout time of the setitimer() signal ---*/
		node_p = (utlTimerNode_P)timers.head_p;

		/*--- we'll compare the current time to the expected timeout
		      time.  If they differ by more than a threshold, we'll
		      fix up the time-out times of all running timers */
		time_diff.tv_sec  = curr_time.tv_sec  - node_p->expiry.tv_sec;
		time_diff.tv_usec = curr_time.tv_usec - node_p->expiry.tv_usec;

		/*--- compute time error (in usec) ---*/
		time_err =  (time_diff.tv_sec * 1000000) + time_diff.tv_usec;

		/*--- is host's setitimer() implementation not POSIX compliant? ---*/
		if ((time_err < 0) && (-time_err < utlSETITIMER_MAX_ERR))
		{
			/*--- update setitimer() compensation ---*/
			if (setitimer_compensation <(unsigned long) (-time_err))
				setitimer_compensation = (setitimer_compensation - time_err) / 2;
		}

		/*--- if there's too much error ---*/
		if (labs(time_err) > utlTIMER_CORRECTION_THRESHOLD)
		{

			/*--- adjust the timeout-times of all pending relative timers ---*/
			for (node_p = (utlTimerNode_P)timers.head_p; node_p != NULL; node_p = node_p->next_p)
			{
				if (node_p->absolute == false)
				{

					/*--- adjust expiry time ---*/
					node_p->expiry.tv_sec  += time_diff.tv_sec;
					node_p->expiry.tv_usec += time_diff.tv_usec;

					/*--- normalize expiry time ---*/
					if (node_p->expiry.tv_usec > 999999)
					{
						node_p->expiry.tv_sec  +=  node_p->expiry.tv_usec / 1000000;
						node_p->expiry.tv_usec  =  node_p->expiry.tv_usec % 1000000;
					}
					if (node_p->expiry.tv_usec <      0)
					{
						node_p->expiry.tv_sec  -=          ((-node_p->expiry.tv_usec - 1) / 1000000) + 1;
						node_p->expiry.tv_usec  = 999999 - ((-node_p->expiry.tv_usec - 1) % 1000000);
					}
				}
			}

			/*--- re-insert pending absolute timers ---*/
			{
				utlLinkedList_T absolute_timers;

				/*--- first initialize linked list of absolute timers ---*/
				utlInitLinkedList(&absolute_timers);

				/*--- remove all absolute timers from running timers list ---*/
				for (node_p = (utlTimerNode_P)timers.head_p; node_p != NULL; )
				{
					utlTimerNode_P next_node_p;

					next_node_p = node_p->next_p;

					if (node_p->absolute == true)
					{
						(void)_utlRemoveTimer(node_p);
						utlPutTailNode(&absolute_timers, utlTimerNode_T, node_p);
					}

					node_p = next_node_p;
				}

				/*--- re-insert all absolute timers into the running timers list ---*/
				while ((node_p = utlGetHeadNode(&absolute_timers, utlTimerNode_T)) != NULL)
				{
					/*--- re-compute period to timeout ---*/
					if (node_p->expiry.tv_usec >= curr_time.tv_usec)
					{
						node_p->period.seconds     =  node_p->expiry.tv_sec  - curr_time.tv_sec;
						node_p->period.nanoseconds = (node_p->expiry.tv_usec - curr_time.tv_usec) * 1000;

					}
					else if (node_p->expiry.tv_sec > curr_time.tv_sec)
					{
						node_p->period.seconds     =            node_p->expiry.tv_sec  - curr_time.tv_sec - 1;
						node_p->period.nanoseconds = (1000000 + node_p->expiry.tv_usec - curr_time.tv_usec) * 1000;

					}
					else
					{
						node_p->period.seconds     = 0;
						node_p->period.nanoseconds = 0;
					}

					_utlAddTimer(node_p);
				}
			}
		}
	}

	/*--- handle all timed out timers... ---*/
	for (rc = utlSUCCESS;; )
	{

		call_back_time_threshold = 0;

		while (timers.node_count != (utlNodeCount_T)0)
		{
			utlTimerNode_P node_p;

			node_p = (utlTimerNode_P)timers.head_p;

			/*--- has this timer not timed out yet? ---*/
			if (timercmp(&(node_p->expiry), &curr_time, >))
				break;

			/*--- unlink timed-out timer from running timers list ---*/
			node_p = utlGetHeadNode(&timers, utlTimerNode_T);

			/*--- perform time-out action ---*/
			{
				utlAbsoluteTime_T abs_time;

				doing_timer_p = node_p;

				if (node_p->call_back.function_p != NULL)
				{
					abs_time.seconds     = curr_time.tv_sec;
					abs_time.nanoseconds = curr_time.tv_usec * 1000;

					utlTrace(utlTRACE_TIMER,
						 char buf[40];

						 (void)utlFormatDateTime(buf, sizeof(buf), &abs_time);

						 utlPrintTrace("timeout %lu of 0x%x for timer %lu at %s\n",
							       node_p->num_cycles - node_p->cycles_left + 1,
							       (unsigned int)node_p->call_back.function_p,
							       node_p->id,
							       buf);
						 );

					if ((*(node_p->call_back.function_p))(node_p->id,
									      node_p->num_cycles - node_p->cycles_left + 1,
									      node_p->call_back.arg_p,
									      &abs_time) != utlSUCCESS)
					{
						/*--- clean ---*/
						doing_timer_p = NULL;

						utlError("Callback function 0x%08x failed", node_p->call_back.function_p);

						/*--- terminate this timer and note failure ---*/
						node_p->cycles_left = 0;
						rc = utlFAILED;
					}

					call_back_time_threshold += utlTIMER_CALL_BACK_TIME_THRESHOLD;
				}

				doing_timer_p = NULL;
			}

			/*--- reschedule this timer? ---*/
			if (node_p->cycles_left > (utlTimeOutCount_T)1)
			{

				/*--- mark as having been processed ---*/
				node_p->marked_flag = true;

				/*--- Advance expiry time.  If the time out is less than 30 seconds
				      then schedule the next time out to be relative to the current time,
				      else it will be relative to when it should have timed out */
				if ((node_p->period.seconds     < (utlSecond_T)30) &&
				    (node_p->period.nanoseconds < (utlNanosecond_T)300000000))
					node_p->expiry = curr_time;

				node_p->expiry.tv_sec  += node_p->period.seconds;
				node_p->expiry.tv_usec += node_p->period.nanoseconds / 1000;

				/*--- normalize expiry time ---*/
				if (node_p->expiry.tv_usec > 999999)
				{
					node_p->expiry.tv_sec  +=  node_p->expiry.tv_usec / 1000000;
					node_p->expiry.tv_usec  =  node_p->expiry.tv_usec % 1000000;
				}
				if (node_p->expiry.tv_usec <      0)
				{
					node_p->expiry.tv_sec  -=          ((-node_p->expiry.tv_usec - 1) / 1000000) + 1;
					node_p->expiry.tv_usec  = 999999 - ((-node_p->expiry.tv_usec - 1) % 1000000);
				}

				if (node_p->num_cycles != utlTIMER_INFINITY)
					node_p->cycles_left--;

				_utlAddTimer(node_p);

			}
			else
			{
				utlFree(node_p);
			}
		}

		/* no more timers running? */
		if (timers.node_count == (utlNodeCount_T)0)
			break;

		/*--- the following deals with the consequences of one or more just-called
		      timeout handlers taking a long time.  We'll pass through the list of
		      running timers again and again until we either hit a timer node we've
		      already processed, or no more timer nodes have timed out */
		{
			utlTimerNode_P node_p;
			struct timeval curr_time2;

			node_p = (utlTimerNode_P)timers.head_p;

			/*--- if this timer node has already been processed on this SIGALRM event ---*/
			if (node_p->marked_flag == true)
				break;

			/*--- re-fetch current time (since epoch) ---*/
			for (;; )
			{
				if (gettimeofday(&curr_time2, NULL) == 0)
					break;

				if (errno != EINTR)
				{
					utlError("Call to %s() failed, %s", "gettimeofday", strerror(errno));

					/*--- clean ---*/
					(void)utlReleaseExclusiveAccess(&timer_semaphore);

					return utlFAILED;
				}
			}

			/*--- did host date/time change? ---*/
			{
				struct timeval elapsed_time;

				/*--- is re-fetched current time older than previously fetched current time? ---*/
				if (timercmp(&curr_time2, &curr_time, <))
					break;

				/*--- compute elapsed time for previous round of call-back invocations ---*/
				elapsed_time.tv_sec = curr_time2.tv_sec - curr_time.tv_sec;

				if (curr_time2.tv_usec >= curr_time.tv_usec)
					elapsed_time.tv_usec = curr_time2.tv_usec - curr_time.tv_usec;
				else if (elapsed_time.tv_sec > 0)
				{
					elapsed_time.tv_sec -= 1;
					elapsed_time.tv_usec = curr_time2.tv_usec + 1000000 - curr_time.tv_usec;
				}
				else
					break;

				/*--- is re-fetched current time newer than previously fetched current time + call-back time threshold? ---*/
				if (((elapsed_time.tv_sec * 1000000) + elapsed_time.tv_usec) > call_back_time_threshold)
					break;
			}

			/* update current time */
			curr_time = curr_time2;

			/*--- has this timer not timed out yet? ---*/
			if (timercmp(&(node_p->expiry), &curr_time, >))
				break;
		}
	}

	/*--- if no more timers are now running... ---*/
	if (timers.node_count == (utlNodeCount_T)0)
	{

		/*--- disable interval timer ---*/
		timerclear(&interval_timer.it_interval);
		timerclear(&interval_timer.it_value);

		if (setitimer(ITIMER_REAL, &interval_timer, NULL) != 0)
		{
			utlError("Call to %s() failed, %s", "setitimer", strerror(errno));

			/*--- clean ---*/
			(void)utlReleaseExclusiveAccess(&timer_semaphore);

			return utlFAILED;
		}

	}
	else
	{
		utlTimerNode_P node_p;

		node_p = (utlTimerNode_P)timers.head_p;

		/*--- don't allow interval timer to restart upon time out ---*/
		timerclear(&interval_timer.it_interval);

		/*--- if the next timer has not yet timed out... ---*/
		if (timercmp(&(node_p->expiry), &curr_time, >))
		{

			/*--- configure next time out ---*/
			interval_timer.it_value.tv_sec = node_p->expiry.tv_sec - curr_time.tv_sec;
			if ((node_p->expiry.tv_usec + (long)setitimer_compensation) < curr_time.tv_usec)
			{
				interval_timer.it_value.tv_sec  -= 1;
				interval_timer.it_value.tv_usec = (node_p->expiry.tv_usec + setitimer_compensation) + 1000000 - curr_time.tv_usec;
			}
			else
				interval_timer.it_value.tv_usec = (node_p->expiry.tv_usec + setitimer_compensation)           - curr_time.tv_usec;

		}
		else
		{
			interval_timer.it_value.tv_sec  =  0;
			interval_timer.it_value.tv_usec = (1 + setitimer_compensation);
		}

		/*--- reschedule timer ---*/
		if (setitimer(ITIMER_REAL, &interval_timer, NULL) != 0)
		{
			utlError("Call to %s() failed, %s", "setitimer", strerror(errno));

			/*--- clean ---*/
			(void)utlReleaseExclusiveAccess(&timer_semaphore);

			return utlFAILED;
		}
	}

	/*--- release exclusive access ---*/
	if (utlReleaseExclusiveAccess(&timer_semaphore) != utlSUCCESS)
		return utlFAILED;

	return rc;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlInitTimers()
* INPUT
*	none
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*	Initializes the timer facilities.
*---------------------------------------------------------------------------*/
utlReturnCode_T utlInitTimers(void)
{
	/*--- if timers are already initialized, bail out ---*/
	if (initialized != false)
		return utlSUCCESS;

	/*--- mutex initialization ---*/
	if (!utlIsSemaphoreInitialized(&timer_semaphore))
	{
		if (utlInitSemaphore(&timer_semaphore, utlSEMAPHORE_ATTR_SHARING_ENABLE |
				     utlSEMAPHORE_ATTR_NESTING_ENABLE) != utlSUCCESS)
			return utlFAILED;
	}

	/*--- arrange for event loop to call our time-out function whenever SIGALRM occurs ---*/
	if (utlSetSignalEventHandler(SIGALRM, utlEVENT_HANDLER_PRIORITY_HIGH, _utlTimerTimeoutHandler, NULL) == (utlEventHandlerId_T)utlFAILED)
		return utlFAILED;

	initialized = true;

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	_utlStartTimer(curr_time_p, node_p, function_p, arg_p)
* INPUT
*       curr_time_p == pointer to the current time
*	      node_p == pointer to a filled timer node
*        function_p == pointer to the time-out call-back function
*             arg_p == user-defined function argument
* OUTPUT
*	none
* RETURNS
*	The ID of the timer that was started for success, utlFAILED for failure
*      (and free's `node_p')
* DESCRIPTION
*	Starts a timer instance.  A timer is started by figuring out when it
*      will expire, putting it into the running timer list and then updating
*	the timer if required.
*---------------------------------------------------------------------------*/
static utlTimerId_T _utlStartTimer(const struct timeval     *curr_time_p,
				   utlTimerNode_P node_p,
				   const utlTimerFunction_P function_p,
				   void               *arg_p)
{
	UNUSEDPARAM(arg_p)

	utlAssert(curr_time_p != NULL);
	utlAssert(node_p      != NULL);
	utlAssert(function_p  != NULL);

	/*--- normalize expiry time ---*/
	if (node_p->expiry.tv_usec > 999999)
	{
		node_p->expiry.tv_sec  +=  node_p->expiry.tv_usec / 1000000;
		node_p->expiry.tv_usec  =  node_p->expiry.tv_usec % 1000000;
	}
	if (node_p->expiry.tv_usec <      0)
	{
		node_p->expiry.tv_sec  -=          ((-node_p->expiry.tv_usec - 1) / 1000000) + 1;
		node_p->expiry.tv_usec  = 999999 - ((-node_p->expiry.tv_usec - 1) % 1000000);
	}

	/*--- add timer to list of running timers ---*/
	_utlAddTimer(node_p);

	/*--- if the newly added timer will time out before any other timer in the list,
	      or if it is the very first timer to be added to the list, configure interval
	      timer... */
	if (node_p == (utlTimerNode_P)timers.head_p)
	{
		struct itimerval interval_timer;

		/*--- don't allow interval timer to restart upon time out ---*/
		timerclear(&interval_timer.it_interval);

		/*--- configure next time out ---*/
		interval_timer.it_value.tv_sec = node_p->expiry.tv_sec - curr_time_p->tv_sec;
		if ((node_p->expiry.tv_usec + (long)setitimer_compensation) < curr_time_p->tv_usec)
		{
			interval_timer.it_value.tv_sec  -= 1;
			interval_timer.it_value.tv_usec = (node_p->expiry.tv_usec + setitimer_compensation) + 1000000 - curr_time_p->tv_usec;
		}
		else
			interval_timer.it_value.tv_usec = (node_p->expiry.tv_usec + setitimer_compensation)           - curr_time_p->tv_usec;

		if (setitimer(ITIMER_REAL, &interval_timer, NULL) != 0)
		{
			utlError("Call to %s() failed, %s", "setitimer", strerror(errno));

			/*--- clean ---*/
			if (_utlRemoveTimer(node_p) == utlSUCCESS)
				utlFree(node_p);

			return utlFAILED;
		}
	}

	utlTrace(utlTRACE_TIMER,
		 utlPrintTrace("start %s timer %lu, handler = 0x%x, period = %d.%09d secs, cycles = %lu\n",
			       (node_p->absolute == true) ? "absolute" : "periodic",
			       node_p->id,
			       (unsigned int)node_p->call_back.function_p,
			       node_p->period.seconds,
			       node_p->period.nanoseconds,
			       node_p->num_cycles);
		 );

	return node_p->id;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlStartAbsoluteTimer(timeout_time_p, function_p, arg_p)
* INPUT
*	timeout_time_p == a time-out time
*          function_p == pointer to the time-out call-back function
*               arg_p == user-defined function argument
* OUTPUT
*	none
* RETURNS
*	The ID of the timer that was started for success, utlFAILED for failure
* DESCRIPTION
*	Starts a timer instance that will timeout at the specified time-out
*      time.
*---------------------------------------------------------------------------*/
utlTimerId_T utlStartAbsoluteTimer(const utlAbsoluteTime_P2c timeout_time_p,
				   const utlTimerFunction_P function_p,
				   void                *arg_p)
{
	utlTimerId_T rv;
	struct timeval curr_time;
	utlTimerNode_P node_p;

	utlAssert(timeout_time_p != NULL);
	utlAssert(function_p     != NULL);

	/*--- initialize timers (if necessary) ---*/
	if (initialized != true)
		if (utlInitTimers() != utlSUCCESS)
			return utlFAILED;

	/*--- acquire exclusive access ---*/
	if (utlAcquireExclusiveAccess(&timer_semaphore) != utlSUCCESS)
		return utlFAILED;

	/*--- fetch a fresh timer node ---*/
	if ((node_p = (utlTimerNode_P)utlMalloc(sizeof(utlTimerNode_T))) == NULL)
	{

		/*--- clean ---*/
		(void)utlReleaseExclusiveAccess(&timer_semaphore);

		return utlFAILED;
	}

	/*--- find an unused timer ID ---*/
	for (;; )
	{
		utlTimerNode_P curr_p;

		for (curr_p = (utlTimerNode_P)timers.head_p; curr_p != NULL; curr_p = curr_p->next_p)
			if (curr_p->id == next_possible_timer_id)
				break;

		if ((curr_p == NULL) && (next_possible_timer_id != (utlTimerId_T)0) &&
		    (next_possible_timer_id != (utlTimerId_T)utlFAILED))
			break;

		next_possible_timer_id++;
	}

	/*--- fetch current time (since epoch) ---*/
	for (;; )
	{
		if (gettimeofday(&curr_time, NULL) == 0)
			break;

		if (errno != EINTR)
		{
			utlError("Call to %s() failed, %s", "gettimeofday", strerror(errno));

			/*--- clean ---*/
			utlFree(node_p);
			(void)utlReleaseExclusiveAccess(&timer_semaphore);

			return utlFAILED;
		}
	}

	/*--- initialize timer node ---*/
	node_p->next_p               = NULL;
	node_p->id                   = next_possible_timer_id++;
	node_p->call_back.function_p = function_p;
	node_p->call_back.arg_p      = arg_p;

	{
		/*--- initialize timer node (completion) ---*/
		node_p->expiry.tv_sec  = timeout_time_p->seconds;
		node_p->expiry.tv_usec = timeout_time_p->nanoseconds / 1000;

		node_p->period.seconds     =  node_p->expiry.tv_sec  - curr_time.tv_sec;
		node_p->period.nanoseconds = (node_p->expiry.tv_usec - curr_time.tv_usec) * 1000;

		/*--- normalize time ---*/
		if (node_p->period.seconds     > 999999999)
		{
			node_p->period.seconds     +=  node_p->period.nanoseconds / 1000000000;
			node_p->period.nanoseconds  =  node_p->period.nanoseconds % 1000000000;
		}
		if (node_p->period.nanoseconds <         0)
		{
			node_p->period.seconds     -=             ((-node_p->period.nanoseconds - 1) / 1000000000) + 1;
			node_p->period.nanoseconds  = 999999999 - ((-node_p->period.nanoseconds - 1) % 1000000000);
		}

		node_p->absolute    = true;
		node_p->num_cycles  = 1;
		node_p->cycles_left = 1;
		node_p->marked_flag = false;
	}

	rv = _utlStartTimer(&curr_time, node_p, function_p, arg_p);

	/*--- release exclusive access ---*/
	if (utlReleaseExclusiveAccess(&timer_semaphore) != utlSUCCESS)
		return utlFAILED;

	return rv;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlStartTimer(period_p, num_cycles, function_p, arg_p)
* INPUT
*	  period_p == time-out period
*      num_cycles == number of time-out cycles (>= 1, utlTIMER_INFINITY)
*      function_p == pointer to the time-out call-back function
*           arg_p == user-defined function argument
* OUTPUT
*	none
* RETURNS
*	The ID of the timer that was started for success, utlFAILED for failure
* DESCRIPTION
*	Starts a timer instance.  A timer is started by figuring out when it
*      will expire, putting it into the running timer list and then updating
*	the timer if required.
*
*      Note that if an absolute time-out time is supplied, the `num_cycles'
*      parameter is ignored.
*---------------------------------------------------------------------------*/
utlTimerId_T utlStartTimer(const utlRelativeTime_P2c period_p,
			   const utlTimeOutCount_T num_cycles,
			   const utlTimerFunction_P function_p,
			   void                *arg_p)
{
	utlTimerId_T rv;
	struct timeval curr_time;
	utlTimerNode_P node_p;

	/*--- check parameters ---*/
	utlAssert(period_p   != NULL);
	utlAssert(num_cycles >= (utlTimeOutCount_T)1);
	utlAssert(function_p != NULL);

	/*--- initialize timers (if necessary) ---*/
	if (initialized != true)
		if (utlInitTimers() != utlSUCCESS)
			return utlFAILED;

	/*--- acquire exclusive access ---*/
	if (utlAcquireExclusiveAccess(&timer_semaphore) != utlSUCCESS)
		return utlFAILED;

	/*--- fetch a fresh timer node ---*/
	if ((node_p = (utlTimerNode_P)utlMalloc(sizeof(utlTimerNode_T))) == NULL)
	{

		/*--- clean ---*/
		(void)utlReleaseExclusiveAccess(&timer_semaphore);

		return utlFAILED;
	}

	/*--- find an unused timer ID ---*/
	for (;; )
	{
		utlTimerNode_P curr_p;

		for (curr_p = (utlTimerNode_P)timers.head_p; curr_p != NULL; curr_p = curr_p->next_p)
			if (curr_p->id == next_possible_timer_id)
				break;

		if ((curr_p == NULL) && (next_possible_timer_id != (utlTimerId_T)0) &&
		    (next_possible_timer_id != (utlTimerId_T)utlFAILED))
			break;

		next_possible_timer_id++;
	}

	/*--- fetch current time (since epoch) ---*/
	for (;; )
	{
		if (gettimeofday(&curr_time, NULL) == 0)
			break;

		if (errno != EINTR)
		{
			utlError("Call to %s() failed, %s", "gettimeofday", strerror(errno));

			/*--- clean ---*/
			utlFree(node_p);
			(void)utlReleaseExclusiveAccess(&timer_semaphore);

			return utlFAILED;
		}
	}

	/*--- initialize timer node ---*/
	node_p->next_p               = NULL;
	node_p->id                   = next_possible_timer_id++;
	node_p->call_back.function_p = function_p;
	node_p->call_back.arg_p      = arg_p;

	{
		utlRelativeTime_T period;

		/*--- enforce minimum period ---*/
		period = *period_p;
		if ((period.seconds == (utlSecond_T)0) && (period.nanoseconds < utlTIMER_MINIMUM_PERIOD))
			period.nanoseconds = utlTIMER_MINIMUM_PERIOD;

		node_p->period         = period;
		node_p->expiry.tv_sec  = curr_time.tv_sec  + period.seconds;
		node_p->expiry.tv_usec = curr_time.tv_usec + period.nanoseconds / 1000;

		node_p->absolute       = false;
		node_p->num_cycles     = num_cycles;
		node_p->cycles_left    = num_cycles;
		node_p->marked_flag    = false;
	}

	rv = _utlStartTimer(&curr_time, node_p, function_p, arg_p);

	/*--- release exclusive access ---*/
	if (utlReleaseExclusiveAccess(&timer_semaphore) != utlSUCCESS)
		return utlFAILED;

	return rv;

}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlStopTimer(timer_id)
* INPUT
*	 timer_id == which timer to stop.
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure
* DESCRIPTION
*	Stops the specified timer.  Stopping a timer consists of finding it
*	in the running timer list, removing it from this list, and then
*	updating the timer if required.  There is a race condition between
*	a timer timing out and stop timer requests; thus if we could not
*	find the timer in the timer list, we'll pretend that the request
*	was honored and just return with success (since the timer is not
*      running anyway).
*---------------------------------------------------------------------------*/
utlReturnCode_T utlStopTimer(const utlTimerId_T timer_id)
{
	utlTimerNode_P node_p;

	/* it means the timer has been stopped */
	if (timer_id == 0)
		return utlFAILED;

	/*--- initialize timers (if necessary) ---*/
	if (initialized != true)
		if (utlInitTimers() != utlSUCCESS)
			return utlFAILED;

	/*--- acquire exclusive access ---*/
	if (utlAcquireExclusiveAccess(&timer_semaphore) != utlSUCCESS)
		return utlFAILED;

	/*--- if we are trying to stop the same timer than is currently timing out... ---*/
	if ((doing_timer_p != NULL) && (timer_id == doing_timer_p->id))
	{
		node_p = doing_timer_p;

		node_p->cycles_left = 0; /* force timer to stop once time-out handling is done */

		utlTrace(utlTRACE_TIMER,
			 utlPrintTrace("stop %s timer %lu, handler = 0x%x, period = %d.%09d secs, cycles = %lu\n",
				       (node_p->absolute == true) ? "absolute" : "periodic",
				       node_p->id,
				       (unsigned int)node_p->call_back.function_p,
				       node_p->period.seconds,
				       node_p->period.nanoseconds,
				       node_p->num_cycles);
			 );

		/*--- release exclusive access ---*/
		if (utlReleaseExclusiveAccess(&timer_semaphore) != utlSUCCESS)
			return utlFAILED;

		return utlSUCCESS;
	}

	/*--- find the requested timer ---*/
	for (node_p = (utlTimerNode_P)timers.head_p; node_p != NULL; node_p = node_p->next_p)
		if (node_p->id == timer_id)
			break;

	/*--- if timer is not running, we'll return under the assumption that the timer just timed out ---*/
	if (node_p == NULL)
	{

		/*--- release exclusive access ---*/
		if (utlReleaseExclusiveAccess(&timer_semaphore) != utlSUCCESS)
			return utlFAILED;

		return utlSUCCESS;
	}

	{
		bool is_first_node;
		struct itimerval interval_timer;

		/*--- note if `node_p' is the first timer in the list ---*/
		if (node_p == (utlTimerNode_P)timers.head_p) is_first_node = true;
		else is_first_node = false;

		/*--- remove 'node_p' ---*/
		if (_utlRemoveTimer(node_p) != utlSUCCESS)
		{

			/*--- release exclusive access ---*/
			if (utlReleaseExclusiveAccess(&timer_semaphore) != utlSUCCESS)
				return utlFAILED;

			return utlSUCCESS;
		}

		utlTrace(utlTRACE_TIMER,
			 utlPrintTrace("stop %s timer %lu, handler = 0x%x, period = %d.%09d secs, cycles = %lu\n",
				       (node_p->absolute == true) ? "absolute" : "periodic",
				       node_p->id,
				       (unsigned int)node_p->call_back.function_p,
				       node_p->period.seconds,
				       node_p->period.nanoseconds,
				       node_p->num_cycles);
			 );

		utlFree(node_p);

		/*--- if no more timers are now running... ---*/
		if (timers.node_count == (utlNodeCount_T)0)
		{

			/*--- don't allow interval timer to restart upon time out ---*/
			timerclear(&interval_timer.it_interval);

			/*--- disable interval timer ---*/
			timerclear(&interval_timer.it_value);

			if (setitimer(ITIMER_REAL, &interval_timer, NULL) != 0)
			{
				utlError("Call to %s() failed, %s", "setitimer", strerror(errno));

				/*--- clean ---*/
				utlReleaseExclusiveAccess(&timer_semaphore);

				return utlFAILED;
			}

			/*--- else if 'node_p' was the first timer in the list... ---*/
		}
		else if (is_first_node == true)
		{
			struct timeval curr_time;

			/*--- fetch current time (since epoch) ---*/
			for (;; )
			{
				if (gettimeofday(&curr_time, NULL) == 0)
					break;

				if (errno != EINTR)
				{
					utlError("Call to %s() failed, %s", "gettimeofday", strerror(errno));

					/*--- clean ---*/
					utlReleaseExclusiveAccess(&timer_semaphore);

					return utlFAILED;
				}
			}

			/*--- fixup interval timer for the new timer node ---*/
			node_p = (utlTimerNode_P)timers.head_p;

			/*--- don't allow interval timer to restart upon time out ---*/
			timerclear(&interval_timer.it_interval);

			/*--- if the next timer has not yet timed out... ---*/
			if (timercmp(&(node_p->expiry), &curr_time, >))
			{

				/*--- configure next time out ---*/
				interval_timer.it_value.tv_sec = node_p->expiry.tv_sec - curr_time.tv_sec;
				if ((node_p->expiry.tv_usec + (long)setitimer_compensation) < curr_time.tv_usec)
				{
					interval_timer.it_value.tv_sec -= 1;
					interval_timer.it_value.tv_usec = (node_p->expiry.tv_usec + setitimer_compensation) + 1000000 - curr_time.tv_usec;
				}
				else
					interval_timer.it_value.tv_usec = (node_p->expiry.tv_usec + setitimer_compensation)           - curr_time.tv_usec;

			}
			else
			{
				interval_timer.it_value.tv_sec  =  0;
				interval_timer.it_value.tv_usec = (1 + setitimer_compensation);
			}

			if (setitimer(ITIMER_REAL, &interval_timer, NULL) != 0)
			{
				utlError("Call to %s() failed, %s", "setitimer", strerror(errno));

				/*--- clean ---*/
				utlReleaseExclusiveAccess(&timer_semaphore);

				return utlFAILED;
			}
		}
	}

	/*--- release exclusive access ---*/
	if (utlReleaseExclusiveAccess(&timer_semaphore) != utlSUCCESS)
		return utlFAILED;

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlIsTimerRunning(timer_id)
* INPUT
*	 timer_id == which timer to query.
* OUTPUT
*	none
* RETURNS
*	true if the timer is running, else false
* DESCRIPTION
*	Determines if the specified timer is currently running.
*---------------------------------------------------------------------------*/
bool utlIsTimerRunning(const utlTimerId_T timer_id)
{
	utlTimerNode_P node_p;

	if (timer_id == 0)
		return false;

	/*--- initialize timers (if necessary) ---*/
	if (initialized != true)
		return false;

	/*--- acquire shared access ---*/
	if (utlAcquireSharedAccess(&timer_semaphore) != utlSUCCESS)
		return false;

	/*--- if we are trying to query the same timer than is currently timing out... ---*/
	if ((doing_timer_p != NULL) && (timer_id == doing_timer_p->id))
	{

		/*--- release shared access ---*/
		if (utlReleaseSharedAccess(&timer_semaphore) != utlSUCCESS)
			return false;

		return true;
	}

	/*--- else search for the requested timer ---*/
	for (node_p = (utlTimerNode_P)timers.head_p; node_p != NULL; node_p = node_p->next_p)
		if (node_p->id == timer_id)
		{

			/*--- release shared access ---*/
			if (utlReleaseSharedAccess(&timer_semaphore) != utlSUCCESS)
				return false;

			return true;
		}

	/*--- clean ---*/
	(void)utlReleaseSharedAccess(&timer_semaphore);

	return false;
}

/*---------------------------------------------------------------------------*
 * FUNCTION
 *      utlQueryTimer(timer_id, what, ...)
 *      utlQueryTimer(timer_id, utlTIMER_QUERY_IS_RUNNING,      is_running_p)
 *      utlQueryTimer(timer_id, utlTIMER_QUERY_IS_ABSLUTE,      is_absolute_p)
 *      utlQueryTimer(timer_id, utlTIMER_QUERY_NUM_CYCLES,      num_cycles_p)
 *      utlQueryTimer(timer_id, utlTIMER_QUERY_CYCLES_LEFT,     cycles_left_p)
 *      utlQueryTimer(timer_id, utlTIMER_QUERY_ELAPSED_TIME,    time_elapsed_p)
 *      utlQueryTimer(timer_id, utlTIMER_QUERY_TIME_TO_TIMEOUT, time_left_p)
 * INPUT
 *           timer_id == which timer to query.
 *       is_running_p == points to where to place the is-running flag
 *      is_absolute_p == points to where to place the is-absolute flag
 *       num_cycles_p == points to where to place the total number of cycles
 *      cycles_left_p == points to where to place the number of cycles left
 *        time_left_p == points to where to place the elapsed time
 *        time_left_p == points to where to place the time until the next timeout
 * OUTPUT
 *      *is_running_p == is the timer running?
 *       *time_left_p == elapsed time
 *       *time_left_p == time until the next timeout
 * RETURNS
 *	utlSUCCESS for success, utlFAILED for failure
 * DESCRIPTION
 *      Queries the specified timer.
 *----------------------------------------------------------------------*/
utlReturnCode_T utlQueryTimer(const utlTimerId_T timer_id,
			      const utlTimerQuery_T what,
			      ...)
{
	struct timeval curr_time;
	utlTimerNode_P node_p;
	va_list va_arg_p;
	bool              *is_running_p;
	bool              *is_absolute_p;
	utlTimeOutCount_P num_cycles_p;
	utlTimeOutCount_P cycles_left_p;
	utlRelativeTime_P elapsed_time_p;
	utlRelativeTime_T time_left;
	utlRelativeTime_P time_left_p;

	if (timer_id == 0)
		return utlFAILED;

	/*--- initialize timers (if necessary) ---*/
	if (initialized != true)
		if (utlInitTimers() != utlSUCCESS)
			return utlFAILED;

	/*--- acquire shared access ---*/
	if (utlAcquireSharedAccess(&timer_semaphore) != utlSUCCESS)
		return utlFAILED;

	/*--- if we are trying to query the same timer that is currently timing out ---*/
	if ((doing_timer_p != NULL) && (timer_id == doing_timer_p->id))
	{
		switch (what)
		{
		case utlTIMER_QUERY_IS_RUNNING:
			va_start(va_arg_p, what);
			is_running_p = va_arg(va_arg_p, bool *);
			va_end(va_arg_p);

			*is_running_p = false;
			break;

		case utlTIMER_QUERY_IS_ABSOLUTE:
			va_start(va_arg_p, what);
			is_absolute_p = va_arg(va_arg_p, bool *);
			va_end(va_arg_p);

			*is_absolute_p = doing_timer_p->absolute;
			break;

		case utlTIMER_QUERY_NUM_CYCLES:
			va_start(va_arg_p, what);
			num_cycles_p = va_arg(va_arg_p, utlTimeOutCount_P);
			va_end(va_arg_p);

			*num_cycles_p = doing_timer_p->num_cycles;
			break;

		case utlTIMER_QUERY_CYCLES_LEFT:
			va_start(va_arg_p, what);
			cycles_left_p = va_arg(va_arg_p, utlTimeOutCount_P);
			va_end(va_arg_p);

			if (doing_timer_p->num_cycles == utlTIMER_INFINITY) *cycles_left_p = doing_timer_p->cycles_left;
			else *cycles_left_p = doing_timer_p->cycles_left - 1;
			break;

		case utlTIMER_QUERY_ELAPSED_TIME:
			va_start(va_arg_p, what);
			elapsed_time_p = va_arg(va_arg_p, utlRelativeTime_P);
			va_end(va_arg_p);

			*elapsed_time_p = doing_timer_p->period;
			break;

		case utlTIMER_QUERY_TIME_TO_TIMEOUT:
			va_start(va_arg_p, what);
			time_left_p = va_arg(va_arg_p, utlRelativeTime_P);
			va_end(va_arg_p);

			time_left_p->seconds     = 0;
			time_left_p->nanoseconds = 0;
			break;

		default:
			utlError("Default case reached with %d", what);

			/*--- clean ---*/
			(void)utlReleaseSharedAccess(&timer_semaphore);

			return utlFAILED;
		}

		/*--- release shared access ---*/
		if (utlReleaseSharedAccess(&timer_semaphore) != utlSUCCESS)
			return utlFAILED;

		return utlSUCCESS;
	}

	/*--- find the requested timer ---*/
	for (node_p = (utlTimerNode_P)timers.head_p; node_p != NULL; node_p = node_p->next_p)
		if (node_p->id == timer_id)
			break;

	switch (what)
	{
	case utlTIMER_QUERY_IS_RUNNING:
		va_start(va_arg_p, what);
		is_running_p = va_arg(va_arg_p, bool *);
		va_end(va_arg_p);

		if (node_p == NULL) *is_running_p = false;
		else *is_running_p = true;
		break;

	case utlTIMER_QUERY_IS_ABSOLUTE:
		va_start(va_arg_p, what);
		is_absolute_p = va_arg(va_arg_p, bool *);
		va_end(va_arg_p);

		if (node_p == NULL) *is_absolute_p = false;
		else *is_absolute_p = node_p->absolute;
		break;

	case utlTIMER_QUERY_NUM_CYCLES:
		va_start(va_arg_p, what);
		num_cycles_p = va_arg(va_arg_p, utlTimeOutCount_P);
		va_end(va_arg_p);

		if (node_p == NULL) *num_cycles_p = 0;
		else *num_cycles_p = node_p->num_cycles;
		break;

	case utlTIMER_QUERY_CYCLES_LEFT:
		va_start(va_arg_p, what);
		cycles_left_p = va_arg(va_arg_p, utlTimeOutCount_P);
		va_end(va_arg_p);

		if (node_p == NULL) *cycles_left_p = 0;
		else *cycles_left_p = node_p->cycles_left;
		break;

	case utlTIMER_QUERY_ELAPSED_TIME:
		va_start(va_arg_p, what);
		elapsed_time_p = va_arg(va_arg_p, utlRelativeTime_P);
		va_end(va_arg_p);

		if (node_p == NULL)
		{
			elapsed_time_p->seconds     = 0;
			elapsed_time_p->nanoseconds = 0;

			/*--- release shared access ---*/
			if (utlReleaseSharedAccess(&timer_semaphore) != utlSUCCESS)
				return utlFAILED;

			return utlSUCCESS;
		}

		/*--- fetch current time (since epoch) ---*/
		for (;; )
		{
			if (gettimeofday(&curr_time, NULL) == 0)
				break;

			if (errno != EINTR)
			{
				utlError("Call to %s() failed, %s", "gettimeofday", strerror(errno));

				/*--- clean ---*/
				(void)utlReleaseSharedAccess(&timer_semaphore);

				return utlFAILED;
			}
		}

		/*--- compute time until expiry (expiry_time - current_time) ---*/
		if (node_p->expiry.tv_sec >= curr_time.tv_sec)
		{
			time_left.seconds = node_p->expiry.tv_sec - curr_time.tv_sec;

			if (node_p->expiry.tv_usec >= curr_time.tv_usec)
				time_left.nanoseconds = (node_p->expiry.tv_usec - curr_time.tv_usec) * 1000;
			else if (time_left.seconds > 0)
			{
				time_left.seconds    -= 1;
				time_left.nanoseconds = (node_p->expiry.tv_usec + 1000000 - curr_time.tv_usec) * 1000;
			}
			else
			{
				time_left.seconds     = 0;
				time_left.nanoseconds = 0;
			}
		}
		else
		{
			time_left.seconds     = 0;
			time_left.nanoseconds = 0;
		}

		/*--- compute elapsed time (period - time_until_expiry) ---*/
		if (node_p->period.seconds >= time_left.seconds)
		{
			elapsed_time_p->seconds = node_p->period.seconds - time_left.seconds;

			if (node_p->period.nanoseconds >= time_left.nanoseconds)
				elapsed_time_p->nanoseconds = node_p->period.nanoseconds - time_left.nanoseconds;
			else if (elapsed_time_p->nanoseconds > 0)
			{
				elapsed_time_p->seconds    -= 1;
				elapsed_time_p->nanoseconds = node_p->period.nanoseconds + 1000000000 - time_left.nanoseconds;
			}
			else
				*elapsed_time_p = node_p->period;
		}
		else
			*elapsed_time_p = node_p->period;
		break;

	case utlTIMER_QUERY_TIME_TO_TIMEOUT:
		va_start(va_arg_p, what);
		time_left_p = va_arg(va_arg_p, utlRelativeTime_P);
		va_end(va_arg_p);

		if (node_p == NULL)
		{
			time_left_p->seconds     = 0;
			time_left_p->nanoseconds = 0;

			/*--- release shared access ---*/
			if (utlReleaseSharedAccess(&timer_semaphore) != utlSUCCESS)
				return utlFAILED;

			return utlSUCCESS;
		}

		/*--- fetch current time (since epoch) ---*/
		for (;; )
		{
			if (gettimeofday(&curr_time, NULL) == 0)
				break;

			if (errno != EINTR)
			{
				utlError("Call to %s() failed, %s", "gettimeofday", strerror(errno));

				/*--- clean ---*/
				(void)utlReleaseSharedAccess(&timer_semaphore);

				return utlFAILED;
			}
		}

		/*--- compute time until expiry ---*/
		if (node_p->expiry.tv_sec >= curr_time.tv_sec)
		{
			time_left_p->seconds = node_p->expiry.tv_sec - curr_time.tv_sec;

			if (node_p->expiry.tv_usec >= curr_time.tv_usec)
				time_left_p->nanoseconds = (node_p->expiry.tv_usec - curr_time.tv_usec) * 1000;
			else if (time_left_p->seconds > 0)
			{
				time_left_p->seconds    -= 1;
				time_left_p->nanoseconds = (node_p->expiry.tv_usec + 1000000 - curr_time.tv_usec) * 1000;
			}
			else
			{
				time_left_p->seconds     = 0;
				time_left_p->nanoseconds = 0;
			}
		}
		else
		{
			time_left_p->seconds     = 0;
			time_left_p->nanoseconds = 0;
		}
		break;

	default:
		utlError("Default case reached with %d", what);

		/*--- clean ---*/
		(void)utlReleaseSharedAccess(&timer_semaphore);
		return utlFAILED;
	}

	/*--- release shared access ---*/
	if (utlReleaseSharedAccess(&timer_semaphore) != utlSUCCESS)
		return utlFAILED;

	return utlSUCCESS;
}
#if defined(utlDEBUG) || defined(utlTEST)

/*---------------------------------------------------------------------------*
 * FUNCTION
 *	utlVStringDumpTimers(v_string_p, prefix_p)
 * INPUT
 *	v_string_p == pointer to an initialized utlVString_T structure
 *        prefix_p == pointer to a prefix string
 * OUTPUT
 *	*v_string_p == the updated utlVString_T structure
 * RETURNS
 *	nothing
 * DESCRIPTION
 *	Dumps the timer list to `v_string_p'.
 *--------------------------------------------------------------------------*/
void utlVStringDumpTimers(const utlVString_P v_string_p,
			  const char         *prefix_p)
{
	utlTimerNode_P node_p;
	char buf[75];

	utlAssert(v_string_p != NULL);
	utlAssert(prefix_p   != NULL);

	/*--- acquire shared access ---*/
	if (utlAcquireSharedAccess(&timer_semaphore) != utlSUCCESS)
		return;

	(void)utlVStringPrintF(v_string_p, "%s:\n", prefix_p);

	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

	/*--- is setitimer() implementation not POSIX compliant? ---*/
	if (setitimer_compensation != 0)
	{
		(void)utlVStringPrintF(v_string_p, "%s: Non-POSIX compliance compensation=%d.%06d seconds\n",
				       prefix_p,
				       setitimer_compensation / 1000000,
				       setitimer_compensation % 1000000);

		(void)utlVStringPrintF(v_string_p, "%s:\n", prefix_p);
	}

	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

	(void)utlVStringPrintF(v_string_p, "%s: Running Timers:\n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s:          |                   |                                |          |                 |           |   Call Back Function\n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s:     ID   | Period (seconds)  |          Next Expiry           |   Type   |     Cycles      | Timer HID |  Function  |  Argument\n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s: ---------+-------------------+--------------------------------+----------+-----------------+-----------+------------+------------\n", prefix_p);

	/*--- is a timer currently in the middle of timing out? ---*/
	if (doing_timer_p != NULL)
	{

		(void)utlVStringPrintF(v_string_p, "%s: %8d |", prefix_p, doing_timer_p->id);

		(void)utlVStringPrintF(v_string_p, " %8ld.%09d |", doing_timer_p->period.seconds, doing_timer_p->period.nanoseconds);
		(void)utlVStringPrintF(v_string_p, " Now (time-out in progress)     |");

		(void)utlVStringPrintF(v_string_p, " %s |", (doing_timer_p->absolute == true) ? "absolute" : "periodic");

		sprintf(buf, "%lu of %lu", doing_timer_p->num_cycles - (doing_timer_p->cycles_left - 1), doing_timer_p->num_cycles);
		(void)utlVStringPrintF(v_string_p, " %15s |", buf);

		(void)utlVStringPrintF(v_string_p, " %9d |", doing_timer_p->id);

		if (doing_timer_p->call_back.function_p != NULL) (void)utlVStringPrintF(v_string_p, " 0x%08x |", doing_timer_p->call_back.function_p);
		else (void)utlVStringPrintF(v_string_p, "   <NULL>   |");
		if (doing_timer_p->call_back.arg_p      != NULL) (void)utlVStringPrintF(v_string_p, " 0x%08x\n", doing_timer_p->call_back.arg_p);
		else (void)utlVStringPrintF(v_string_p, "   <NULL>\n");
	}

	for (node_p = (utlTimerNode_P)timers.head_p; node_p != NULL; node_p = node_p->next_p)
	{
		time_t time;
		struct tm *tm_p;

		(void)utlVStringPrintF(v_string_p, "%s: %8d |", prefix_p, node_p->id);

		(void)utlVStringPrintF(v_string_p, "%8ld.%09d |", node_p->period.seconds, node_p->period.nanoseconds);

		time = node_p->expiry.tv_sec;
		tm_p = gmtime(&time);
		(void)utlVStringPrintF(v_string_p, " %04d-%02d-%02d %02d:%02d:%02d.%06d UTC |",
				       tm_p->tm_year + 1900,
				       tm_p->tm_mon  + 1,
				       tm_p->tm_mday,
				       tm_p->tm_hour,
				       tm_p->tm_min,
				       tm_p->tm_sec,
				       node_p->expiry.tv_usec);

		(void)utlVStringPrintF(v_string_p, " %s |", (node_p->absolute == true) ? "absolute" : "periodic");

		sprintf(buf, "%lu of %lu", node_p->num_cycles - node_p->cycles_left, node_p->num_cycles);
		(void)utlVStringPrintF(v_string_p, " %15s |", buf);

		(void)utlVStringPrintF(v_string_p, " %9d |", node_p->id);

		if (node_p->call_back.function_p != NULL) (void)utlVStringPrintF(v_string_p, " 0x%08x |", node_p->call_back.function_p);
		else (void)utlVStringPrintF(v_string_p, "   <NULL>   |");
		if (node_p->call_back.arg_p      != NULL) (void)utlVStringPrintF(v_string_p, " 0x%08x\n", node_p->call_back.arg_p);
		else (void)utlVStringPrintF(v_string_p, "   <NULL>\n");
	}

	/*--- release shared access ---*/
	(void)utlReleaseSharedAccess(&timer_semaphore);
}

/*---------------------------------------------------------------------------*
 * FUNCTION
 *	utlDumpTimers(file_p)
 * INPUT
 *	file_p == pointer to an open file
 * OUTPUT
 *	none
 * RETURNS
 *	nothing
 * DESCRIPTION
 *	Dumps the timer list to `file_p'.
 *--------------------------------------------------------------------------*/
void utlDumpTimers(FILE *file_p)
{
	utlVString_T v_string;

	utlAssert(file_p != NULL);

	utlInitVString(&v_string);

	utlVStringDumpTimers(&v_string, "Timers");

	utlVStringPuts(&v_string, file_p);

	utlVStringFree(&v_string);
}
#endif /* defined(utlDEBUG) || defined(utlTEST) */

#ifdef utlTEST
/*--------------------------------------------------------------------------*/
typedef struct {
	enum {
		utlMODE_NULL,
		utlMODE_EXIT_EVENT_HANDLER,
		utlMODE_STOP_TIMER,
	} mode;
	utlTimeOutCount_T num_cycles;
	utlTimeOutCount_T curr_cycle;
	double last;
	double period;
} testInfo_T;


/*--------------------------------------------------------------------------*
* FUNCTION
*	testFunc(id, timeout_count, arg_p, curr_time_p)
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
*--------------------------------------------------------------------------*/
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

	delta =              curr_time_p->seconds + (curr_time_p->nanoseconds / 1000000000.0) - test_info_p->last;
	test_info_p->last =  curr_time_p->seconds + (curr_time_p->nanoseconds / 1000000000.0);

	/*--- time-out time OK? ---*/
	if ((test_info_p->curr_cycle > (unsigned long)1) && (fabs(test_info_p->period - delta) > 0.15))
		fprintf(stderr, "timerTest: warning, timing error = %f\n", fabs(test_info_p->period - delta));
	if (test_info_p->curr_cycle != timeout_count)
	{
		fprintf(stderr, "timerTest: warning, cycle count error, (expected %lu, got %lu)\n",
			test_info_p->curr_cycle, timeout_count);
		return utlFAILED;
	}

	if (test_info_p->curr_cycle == test_info_p->num_cycles)
	{
		switch (test_info_p->mode)
		{
		case utlMODE_NULL:
			break;

		case utlMODE_EXIT_EVENT_HANDLER:
			utlExitEventLoop();
			break;

		case utlMODE_STOP_TIMER:
			utlStopTimer(id);
			break;
		}
	}

	return utlSUCCESS;
}

/*--------------------------------------------------------------------------*
* FUNCTION
*	timerTest()
* INPUT
*      none
* OUTPUT
*	none
* RETURNS
*	"true" for pass, "false" for failure
*--------------------------------------------------------------------------*/
bool timerTest()
{
	utlRelativeTime_T period;

	if (utlInitTimers() != utlSUCCESS)
	{
		fprintf(stderr, "timerTest: _utlTimerInit()\n");
		return false;
	}

	/*--- test that timer starts, times out properly, and cycles 3 times ---*/
	{
		utlTimerId_T exit_timer_id;
		testInfo_T exit_test_info;
		utlTimerId_T timer_id;
		testInfo_T test_info;

		/* exit timer */
		period.seconds     = 7;
		period.nanoseconds = 0;

		exit_test_info.mode = utlMODE_EXIT_EVENT_HANDLER;
		exit_test_info.num_cycles = 1;
		exit_test_info.curr_cycle = 0;
		exit_test_info.last       = 0.0;
		exit_test_info.period     =  period.seconds +
					    (period.nanoseconds / 1000000000.0);

		if ((exit_timer_id = utlStartTimer(&period, exit_test_info.num_cycles, &testFunc, &exit_test_info)) == utlFAILED)
		{
			fprintf(stderr, "timerTest: utlStartTimer(1.0)\n");
			return false;
		}

		/* timer being tested */
		period.seconds     = 1;
		period.nanoseconds = 0;

		test_info.mode = utlMODE_NULL;
		test_info.num_cycles = 3;
		test_info.curr_cycle = 0;
		test_info.last       = 0.0;
		test_info.period     =  period.seconds +
				       (period.nanoseconds / 1000000000.0);

		if ((timer_id = utlStartTimer(&period, test_info.num_cycles, &testFunc, &test_info)) == utlFAILED)
		{
			fprintf(stderr, "timerTest: utlStartTimer(1.2)\n");
			return false;
		}

		if (utlEventLoop(true) != utlSUCCESS)
		{
			fprintf(stderr, "timerTest: utlEventLoop(1.3)\n");
			return false;
		}

		if ((test_info.num_cycles != 3) ||
		    (test_info.curr_cycle != 3))
		{
			fprintf(stderr, "timerTest: invalid timer data(1.4)\n");
			return false;
		}
	}

	/*--- test that timer never starts if we stop it before entering the event loop ---*/
	{
		utlTimerId_T exit_timer_id;
		testInfo_T exit_test_info;
		utlTimerId_T timer_id;
		testInfo_T test_info;

		/* exit timer */
		period.seconds     = 7;
		period.nanoseconds = 0;

		exit_test_info.mode = utlMODE_EXIT_EVENT_HANDLER;
		exit_test_info.num_cycles = 1;
		exit_test_info.curr_cycle = 0;
		exit_test_info.last       = 0.0;
		exit_test_info.period     =  period.seconds +
					    (period.nanoseconds / 1000000000.0);

		if ((exit_timer_id = utlStartTimer(&period, exit_test_info.num_cycles, &testFunc, &exit_test_info)) == utlFAILED)
		{
			fprintf(stderr, "timerTest: utlStartTimer(2.0)\n");
			return false;
		}

		/* timer being tested */
		period.seconds     = 1;
		period.nanoseconds = 0;

		test_info.mode = utlMODE_STOP_TIMER;
		test_info.num_cycles = 3;
		test_info.curr_cycle = 0;
		test_info.last       = 0.0;
		test_info.period     =  period.seconds +
				       (period.nanoseconds / 1000000000.0);

		if ((timer_id = utlStartTimer(&period, 1000, &testFunc, &test_info)) == utlFAILED)
		{
			fprintf(stderr, "timerTest: utlStartTimer(2.1)\n");
			return false;
		}

		if (utlStopTimer(timer_id) != utlSUCCESS)
		{
			fprintf(stderr, "timerTest: utlStopTimer(2.2)\n");
			return false;
		}

		if (utlEventLoop(true) != utlSUCCESS)
		{
			fprintf(stderr, "timerTest: utlEventLoop(2.3)\n");
			return false;
		}

		if ((test_info.num_cycles != 3) ||
		    (test_info.curr_cycle != 0))
		{
			fprintf(stderr, "timerTest: invalid timer data(2.4)\n");
			return false;
		}
	}

	/*--- test that timer starts, times out properly, and stops when we tell it to ---*/
	{
		utlTimerId_T exit_timer_id;
		testInfo_T exit_test_info;
		utlTimerId_T timer_id;
		testInfo_T test_info;

		/* exit timer */
		period.seconds     = 7;
		period.nanoseconds = 0;

		exit_test_info.mode = utlMODE_EXIT_EVENT_HANDLER;
		exit_test_info.num_cycles = 1;
		exit_test_info.curr_cycle = 0;
		exit_test_info.last       = 0.0;
		exit_test_info.period     =  period.seconds +
					    (period.nanoseconds / 1000000000.0);

		if ((exit_timer_id = utlStartTimer(&period, exit_test_info.num_cycles, &testFunc, &exit_test_info)) == utlFAILED)
		{
			fprintf(stderr, "timerTest: utlStartTimer(3.0)\n");
			return false;
		}

		/* timer being tested */
		period.seconds     = 1;
		period.nanoseconds = 0;

		test_info.mode = utlMODE_STOP_TIMER;
		test_info.num_cycles = 3;
		test_info.curr_cycle = 0;
		test_info.last       = 0.0;
		test_info.period     =  period.seconds +
				       (period.nanoseconds / 1000000000.0);

		if ((timer_id = utlStartTimer(&period, 1000, &testFunc, &test_info)) == utlFAILED)
		{
			fprintf(stderr, "timerTest: utlStartTimer(3.1)\n");
			return false;
		}

		if (utlEventLoop(true) != utlSUCCESS)
		{
			fprintf(stderr, "timerTest: utlEventLoop(3.2)\n");
			return false;
		}

		if ((test_info.num_cycles != 3) ||
		    (test_info.curr_cycle != 3))
		{
			fprintf(stderr, "timerTest: invalid timer data(3.3)\n");
			return false;
		}
	}


	/*--- many timers test ---*/
	{
		utlTimerId_T exit_timer_id;
		testInfo_T exit_test_info;
		utlTimerId_T timer_id[10];
		testInfo_T test_info[10];
		int i;

		/* exit timer */
		period.seconds     = 7;
		period.nanoseconds = 0;

		exit_test_info.mode = utlMODE_EXIT_EVENT_HANDLER;
		exit_test_info.num_cycles = 1;
		exit_test_info.curr_cycle = 0;
		exit_test_info.last       = 0.0;
		exit_test_info.period     =  period.seconds +
					    (period.nanoseconds / 1000000000.0);

		if ((exit_timer_id = utlStartTimer(&period, exit_test_info.num_cycles, &testFunc, &exit_test_info)) == utlFAILED)
		{
			fprintf(stderr, "timerTest: utlStartTimer(4.0)\n");
			return false;
		}

		/* timers being tested */
		for (i = 0; i < 10; i++)
		{
			switch (i)
			{
			case 0: period.seconds = 0; period.nanoseconds = 200000000; break;
			case 1: period.seconds = 0; period.nanoseconds = 250000000; break;
			case 2: period.seconds = 0; period.nanoseconds = 300000000; break;
			case 3: period.seconds = 0; period.nanoseconds = 330000000; break;
			case 4: period.seconds = 0; period.nanoseconds = 370000000; break;
			case 5: period.seconds = 1; period.nanoseconds = 0;         break;
			case 6: period.seconds = 1; period.nanoseconds = 100000000; break;
			case 7: period.seconds = 1; period.nanoseconds = 200000000; break;
			case 8: period.seconds = 1; period.nanoseconds = 300000000; break;
			case 9: period.seconds = 1; period.nanoseconds = 400000000; break;
			}

			test_info[i].mode = utlMODE_NULL;
			test_info[i].num_cycles = 4;
			test_info[i].curr_cycle = 0;
			test_info[i].last       = 0.0;
			test_info[i].period     =  period.seconds +
						  (period.nanoseconds / 1000000000.0);

			if ((timer_id[i] = utlStartTimer(&period, test_info[i].num_cycles, &testFunc, test_info + i)) == utlFAILED)
			{
				fprintf(stderr, "timerTest: utlStartTimer(4.1)\n");
				return false;
			}
		}

		utlDumpTimers(stdout);

		if (utlEventLoop(true) != utlSUCCESS)
		{
			fprintf(stderr, "timerTest: utlEventLoop(4.2)\n");
			return false;
		}

		for (i = 0; i < 10; i++)
		{
			if ((test_info[i].num_cycles != 4) ||
			    (test_info[i].curr_cycle != 4))
			{
				fprintf(stderr, "timerTest: invalid timer data(4.3)\n");
				return false;
			}
		}
	}

	/*--- test an absolute timer */
	{
		utlTimerId_T exit_timer_id;
		testInfo_T exit_test_info;
		utlTimerId_T timer_id;
		testInfo_T test_info;
		utlAbsoluteTime_T timeout_time;
		struct timeval curr_time;

		/* exit timer */
		period.seconds     = 7;
		period.nanoseconds = 0;

		exit_test_info.mode = utlMODE_EXIT_EVENT_HANDLER;
		exit_test_info.num_cycles = 1;
		exit_test_info.curr_cycle = 0;
		exit_test_info.last       = 0.0;
		exit_test_info.period     =  period.seconds +
					    (period.nanoseconds / 1000000000.0);

		if ((exit_timer_id = utlStartTimer(&period, exit_test_info.num_cycles, &testFunc, &exit_test_info)) == utlFAILED)
		{
			fprintf(stderr, "timerTest: utlStartTimer(5.0)\n");
			return false;
		}

		gettimeofday(&curr_time, NULL);

		/* timer being tested */
		timeout_time.seconds     = curr_time.tv_sec  + 2;
		timeout_time.nanoseconds = curr_time.tv_usec * 1000;

		test_info.mode = utlMODE_NULL;
		test_info.num_cycles = 1;
		test_info.curr_cycle = 0;
		test_info.last       = 0.0;
		test_info.period     =  2.0;

		if ((timer_id = utlStartAbsoluteTimer(&timeout_time, &testFunc, &test_info)) == utlFAILED)
		{
			fprintf(stderr, "timerTest: utlStartTimer(5.2)\n");
			return false;
		}

		if (utlEventLoop(true) != utlSUCCESS)
		{
			fprintf(stderr, "timerTest: utlEventLoop(5.3)\n");
			return false;
		}

		if ((test_info.num_cycles != 1) ||
		    (test_info.curr_cycle != 1))
		{
			fprintf(stderr, "timerTest: invalid timer data(5.4)\n");
			return false;
		}
	}

	return true;
}
#endif /* utlTEST */

