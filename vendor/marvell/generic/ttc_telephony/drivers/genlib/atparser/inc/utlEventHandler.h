/*****************************************************************************
* Utility Library
*
* Event handler utilities header file
*
*****************************************************************************/

#ifndef _UTL_EVENT_HANDLER_INCLUDED
#define _UTL_EVENT_HANDLER_INCLUDED

#include <stdio.h>
#include <signal.h>

#include <utlTypes.h>
#include <utlTime.h>
#include <utlVString.h>


/*--- Data Types ------------------------------------------------------------*/

typedef enum {
	utlEVENT_HANDLER_TYPE_UNKNOWN              = 0x000,

	utlEVENT_HANDLER_TYPE_SIGHUP               = SIGHUP,
	utlEVENT_HANDLER_TYPE_SIGINT               = SIGINT,
	utlEVENT_HANDLER_TYPE_SIGQUIT              = SIGQUIT,
	utlEVENT_HANDLER_TYPE_SIGILL               = SIGILL,
	utlEVENT_HANDLER_TYPE_SIGTRAP              = SIGTRAP,
	utlEVENT_HANDLER_TYPE_SIGABRT              = SIGABRT,
	utlEVENT_HANDLER_TYPE_SIGFPE               = SIGFPE,
	utlEVENT_HANDLER_TYPE_SIGKILL              = SIGKILL,
	utlEVENT_HANDLER_TYPE_SIGBUS               = SIGBUS,
	utlEVENT_HANDLER_TYPE_SIGSEGV              = SIGSEGV,
	utlEVENT_HANDLER_TYPE_SIGSYS               = SIGSYS,
	utlEVENT_HANDLER_TYPE_SIGPIPE              = SIGPIPE,
	utlEVENT_HANDLER_TYPE_SIGALRM              = SIGALRM,
	utlEVENT_HANDLER_TYPE_SIGTERM              = SIGTERM,
	utlEVENT_HANDLER_TYPE_SIGURG               = SIGURG,
	utlEVENT_HANDLER_TYPE_SIGSTOP              = SIGSTOP,
	utlEVENT_HANDLER_TYPE_SIGTSTP              = SIGTSTP,
	utlEVENT_HANDLER_TYPE_SIGCONT              = SIGCONT,
	utlEVENT_HANDLER_TYPE_SIGCHLD              = SIGCHLD,
	utlEVENT_HANDLER_TYPE_SIGTTIN              = SIGTTIN,
	utlEVENT_HANDLER_TYPE_SIGTTOU              = SIGTTOU,
	utlEVENT_HANDLER_TYPE_SIGIO                = SIGIO,
	utlEVENT_HANDLER_TYPE_SIGXCPU              = SIGXCPU,
	utlEVENT_HANDLER_TYPE_SIGXFSZ              = SIGXFSZ,
	utlEVENT_HANDLER_TYPE_SIGVTALRM            = SIGVTALRM,
	utlEVENT_HANDLER_TYPE_SIGPROF              = SIGPROF,
	utlEVENT_HANDLER_TYPE_SIGWINCH             = SIGWINCH,
	utlEVENT_HANDLER_TYPE_SIGUSR1              = SIGUSR1,
	utlEVENT_HANDLER_TYPE_SIGUSR2              = SIGUSR2,
/*oxo    utlEVENT_HANDLER_TYPE_SIGPOLL              = SIGPOLL,*/
	utlEVENT_HANDLER_TYPE_SIGIOT               = SIGIOT,
/*oxo    utlEVENT_HANDLER_TYPE_SIGSTKFLT            = SIGSTKFLT,*/
/*oxo    utlEVENT_HANDLER_TYPE_SIGCLD               = SIGCLD,*/
/*oxo    utlEVENT_HANDLER_TYPE_SIGPWR               = SIGPWR,*/
/*oxo    utlEVENT_HANDLER_TYPE_SIGUNUSED            = SIGUNUSED,*/

	utlEVENT_HANDLER_TYPE_WRITE                = 0x101,
	utlEVENT_HANDLER_TYPE_READ                 = 0x102,
	utlEVENT_HANDLER_TYPE_READ_WRITE           = 0x103,
	utlEVENT_HANDLER_TYPE_EXCEPTION            = 0x104,
	utlEVENT_HANDLER_TYPE_EXCEPTION_WRITE      = 0x105,
	utlEVENT_HANDLER_TYPE_EXCEPTION_READ       = 0x106,
	utlEVENT_HANDLER_TYPE_EXCEPTION_READ_WRITE = 0x107,

	utlEVENT_HANDLER_TYPE_IDLE                 = 0x201,     /* select-idle handlers */

	utlEVENT_HANDLER_TYPE_EXIT                 = 0x301,     /* program-exit handlers */

	utlEVENT_HANDLER_TYPE_DIRECT_FUNC_CALL     = 0x401,     /* directly called handlers */

	utlEVENT_HANDLER_TYPE_ZOMBIE               = 0x501      /* deleted handlers */
} utlEventHandlerType_T;

typedef enum {
	utlEVENT_HANDLER_PRIORITY_LOW      = 1,
	utlEVENT_HANDLER_PRIORITY_MEDIUM   = 2,
	utlEVENT_HANDLER_PRIORITY_HIGH     = 3,
	utlEVENT_HANDLER_PRIORITY_CRITICAL = 4,
	utlEVENT_HANDLER_PRIORITY_INTERNAL = 5
} utlEventHandlerPriority_T;

typedef unsigned long utlEventHandlerId_T;

typedef utlReturnCode_T (*utlEventHandlerFunction_P)(const utlEventHandlerType_T handler_type,
						     const utlEventHandlerType_T event_type,
						     const int fd,
						     const utlRelativeTime_P2c period_p,
						     void                  *arg_p);


/*--- Prototypes ------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

extern utlReturnCode_T     utlDeleteEventHandler(const utlEventHandlerId_T handler_id);
extern utlEventHandlerId_T utlSetFdEventHandler(const utlEventHandlerType_T type,
						const utlEventHandlerPriority_T priority,
						const int fd,
						const utlEventHandlerFunction_P function_p,
						void                      *arg_p);
extern utlReturnCode_T     utlEnableFdEventHandler(const utlEventHandlerType_T type,
						   const int fd);
extern utlReturnCode_T     utlDisableFdEventHandler(const utlEventHandlerType_T type,
						    const int fd);
extern utlEventHandlerId_T utlSetIdleEventHandler(const utlRelativeTime_P2c period_p,
						  const utlEventHandlerPriority_T priority,
						  const utlEventHandlerFunction_P function_p,
						  void                      *arg);
extern utlEventHandlerId_T utlSetSignalEventHandler(const int signal,
						    const utlEventHandlerPriority_T priority,
						    const utlEventHandlerFunction_P function_p,
						    void                      *arg);
extern utlEventHandlerId_T utlSetExitEventHandler(const utlEventHandlerPriority_T priority,
						  const utlEventHandlerFunction_P function_p,
						  void                      *arg);
extern void                utlExitEventLoop(void);
extern utlReturnCode_T     utlEventLoop(const bool block);

#if defined(utlDEBUG) || defined(utlTEST)
extern void            utlVStringDumpEventHandlers(const utlVString_P v_string_p,
						   const char                      *prefix_p);
extern void            utlDumpEventHandlers(      FILE                      *file_p);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _UTL_EVENT_HANDLER_INCLUDED */

