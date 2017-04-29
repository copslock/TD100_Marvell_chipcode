/*****************************************************************************
* Utility Library
*
* State-machine utilities header file
*
*****************************************************************************/

#ifndef _UTL_STATE_MACHINE_INCLUDED
#define _UTL_STATE_MACHINE_INCLUDED

#include <stdio.h>

#include <utlTypes.h>
#include <utlTime.h>
#include <utlTimer.h>
#include <utlLinkedList.h>
#include <utlSemaphore.h>


/*--- Configuration ---------------------------------------------------------*/

/*--- maximum number of events to save in event-history buffer ---*/
#define utlSTATE_MACHINE_MAX_EVENT_HISTORY ((size_t)48)

/*--- maximum number of state-changes to save in state-history buffer ---*/
#define utlSTATE_MACHINE_MAX_STATE_HISTORY ((size_t)48)

/*--- size of state-machine state cache (entries) ---*/
#define utlSTATE_MACHINE_MAX_EVENT_CACHE ((size_t)5)

/*--- depth of state-machine stack (levels) ---*/
#define utlSTATE_MACHINE_MAX_LEVELS ((size_t)6)

/*--- maximum length of a state-machine name (characters) ---*/
#define utlMAX_STATE_MACHINE_NAME_LENGTH ((size_t)20)


/*--- Data Types ------------------------------------------------------------*/

/*--- basic state-machine states ---*/
#define utlSTATE_ID_NULL        0
#define utlSTATE_ID_BOOT        1
#define utlSTATE_ID_CONFIG      2
#define utlSTATE_ID_STOPPED     3
#define utlSTATE_ID_IDLING      4
#define utlSTATE_ID_ACTIVE      5
#define utlSTATE_ID_POLL        6
#define utlSTATE_ID_CALL_BACK   7
#define utlSTATE_ID_CONFIG_ERR  8
#define utlSTATE_ID_COMM_FAIL   9
#define utlSTATE_ID_FAILED     10
#define utlDEFINE_STATE_ID(n) (100 + n)

#define utlSTATE_NAME_NULL       "Null"
#define utlSTATE_NAME_BOOT       "Boot"
#define utlSTATE_NAME_CONFIG     "Config"
#define utlSTATE_NAME_STOPPED    "Stopped"
#define utlSTATE_NAME_IDLING     "Idling"
#define utlSTATE_NAME_ACTIVE     "Active"
#define utlSTATE_NAME_POLL       "Poll"
#define utlSTATE_NAME_CALL_BACK  "Call-back"
#define utlSTATE_NAME_CONFIG_ERR "Config-error"
#define utlSTATE_NAME_COMM_FAIL  "Comm-fail"
#define utlSTATE_NAME_FAILED     "Failed"

#define utlDEFINE_STATE_NULL()                                                                    { utlSTATE_ID_NULL,       utlSTATE_NAME_NULL,       NULL,     NULL,      NULL,          { NULL,                       NULL } }
#define utlDEFINE_STATE_BOOT(      events_p, timeout_p, max_retries_p, enter_prefix, exit_prefix) { utlSTATE_ID_BOOT,       utlSTATE_NAME_BOOT,       events_p, timeout_p, max_retries_p, { enter_prefix ## StateHandler, exit_prefix ## StateHandler } }
#define utlDEFINE_STATE_CONFIG(    events_p, timeout_p, max_retries_p, enter_prefix, exit_prefix) { utlSTATE_ID_CONFIG,     utlSTATE_NAME_CONFIG,     events_p, timeout_p, max_retries_p, { enter_prefix ## StateHandler, exit_prefix ## StateHandler } }
#define utlDEFINE_STATE_STOPPED(   events_p, timeout_p, max_retries_p, enter_prefix, exit_prefix) { utlSTATE_ID_STOPPED,    utlSTATE_NAME_STOPPED,    events_p, timeout_p, max_retries_p, { enter_prefix ## StateHandler, exit_prefix ## StateHandler } }
#define utlDEFINE_STATE_IDLING(    events_p, timeout_p, max_retries_p, enter_prefix, exit_prefix) { utlSTATE_ID_IDLING,     utlSTATE_NAME_IDLING,     events_p, timeout_p, max_retries_p, { enter_prefix ## StateHandler, exit_prefix ## StateHandler } }
#define utlDEFINE_STATE_ACTIVE(    events_p, timeout_p, max_retries_p, enter_prefix, exit_prefix) { utlSTATE_ID_ACTIVE,     utlSTATE_NAME_ACTIVE,     events_p, timeout_p, max_retries_p, { enter_prefix ## StateHandler, exit_prefix ## StateHandler } }
#define utlDEFINE_STATE_POLL(      events_p, timeout_p, max_retries_p, enter_prefix, exit_prefix) { utlSTATE_ID_POLL,       utlSTATE_NAME_POLL,       events_p, timeout_p, max_retries_p, { enter_prefix ## StateHandler, exit_prefix ## StateHandler } }
#define utlDEFINE_STATE_CALL_BACK( events_p, timeout_p, max_retries_p, enter_prefix, exit_prefix) { utlSTATE_ID_CALL_BACK,  utlSTATE_NAME_CALL_BACK,  events_p, timeout_p, max_retries_p, { enter_prefix ## StateHandler, exit_prefix ## StateHandler } }
#define utlDEFINE_STATE_CONFIG_ERR(events_p, timeout_p, max_retries_p, enter_prefix, exit_prefix) { utlSTATE_ID_CONFIG_ERR, utlSTATE_NAME_CONFIG_ERR, events_p, timeout_p, max_retries_p, { enter_prefix ## StateHandler, exit_prefix ## StateHandler } }
#define utlDEFINE_STATE_COMM_FAIL( events_p, timeout_p, max_retries_p, enter_prefix, exit_prefix) { utlSTATE_ID_COMM_FAIL,  utlSTATE_NAME_COMM_FAIL,  events_p, timeout_p, max_retries_p, { enter_prefix ## StateHandler, exit_prefix ## StateHandler } }
#define utlDEFINE_STATE_FAILED(    events_p, timeout_p, max_retries_p, enter_prefix, exit_prefix) { utlSTATE_ID_FAILED,     utlSTATE_NAME_FAILED,     events_p, timeout_p, max_retries_p, { enter_prefix ## StateHandler, exit_prefix ## StateHandler } }

#define NULLStateHandler NULL
#define utlDEFINE_STATE(state, name, events_p, timeout_p, max_retries_p,  enter_prefix,               exit_prefix) \
	{ state, name, events_p, timeout_p, max_retries_p, { enter_prefix ## StateHandler, exit_prefix ## StateHandler } }

/*--- basic state-machine events ---*/
#define utlSTATE_EVENT_ID_NULL         0
#define utlSTATE_EVENT_ID_START        1
#define utlSTATE_EVENT_ID_STOP         2
#define utlSTATE_EVENT_ID_RESET        3
#define utlSTATE_EVENT_ID_ENABLE       4
#define utlSTATE_EVENT_ID_DISABLE      5
#define utlSTATE_EVENT_ID_LOAD_CONFIG  6
#define utlSTATE_EVENT_ID_POLL         7
#define utlSTATE_EVENT_ID_REPLY        8
#define utlSTATE_EVENT_ID_SUCCESS      9
#define utlSTATE_EVENT_ID_FAILED      10
#define utlSTATE_EVENT_ID_BAD_REPLY   11
#define utlSTATE_EVENT_ID_RETRY       12
#define utlSTATE_EVENT_ID_GIVE_UP     13
#define utlSTATE_EVENT_ID_TIMEOUT     14
#define utlSTATE_EVENT_ID_CALL_BACK   15
#define utlSTATE_EVENT_ID_COMM_FAIL   16
#define utlSTATE_EVENT_ID_FAIL        17
#define utlSTATE_EVENT_ID_DEFAULT     18
#define utlDEFINE_STATE_EVENT_ID(n) (100 + n)

#define utlSTATE_EVENT_NAME_NULL        "Null"
#define utlSTATE_EVENT_NAME_START       "Start"
#define utlSTATE_EVENT_NAME_STOP        "Stop"
#define utlSTATE_EVENT_NAME_RESET       "Reset"
#define utlSTATE_EVENT_NAME_ENABLE      "Enable"
#define utlSTATE_EVENT_NAME_DISABLE     "Disable"
#define utlSTATE_EVENT_NAME_LOAD_CONFIG "Load-config"
#define utlSTATE_EVENT_NAME_POLL        "Poll"
#define utlSTATE_EVENT_NAME_REPLY       "Reply"
#define utlSTATE_EVENT_NAME_SUCCESS     "Success"
#define utlSTATE_EVENT_NAME_FAILED      "Failed"
#define utlSTATE_EVENT_NAME_BAD_REPLY   "Bad-Reply"
#define utlSTATE_EVENT_NAME_RETRY       "Retry"
#define utlSTATE_EVENT_NAME_GIVE_UP     "Give-up"
#define utlSTATE_EVENT_NAME_TIMEOUT     "Timeout"
#define utlSTATE_EVENT_NAME_CALL_BACK   "Call-back"
#define utlSTATE_EVENT_NAME_COMM_FAIL   "Comm-Fail"
#define utlSTATE_EVENT_NAME_FAIL        "Fail"
#define utlSTATE_EVENT_NAME_DEFAULT     "Default"

#define utlDEFINE_EVENT_HANDLER_NULL()                           { utlSTATE_EVENT_ID_NULL,        utlSTATE_EVENT_NAME_NULL,        { NULL } }
#define utlDEFINE_EVENT_HANDLER_START(      handler_name_prefix) { utlSTATE_EVENT_ID_START,       utlSTATE_EVENT_NAME_START,       { handler_name_prefix ## EventHandler } }
#define utlDEFINE_EVENT_HANDLER_STOP(       handler_name_prefix) { utlSTATE_EVENT_ID_STOP,        utlSTATE_EVENT_NAME_STOP,        { handler_name_prefix ## EventHandler } }
#define utlDEFINE_EVENT_HANDLER_RESET(      handler_name_prefix) { utlSTATE_EVENT_ID_RESET,       utlSTATE_EVENT_NAME_RESET,       { handler_name_prefix ## EventHandler } }
#define utlDEFINE_EVENT_HANDLER_ENABLE(     handler_name_prefix) { utlSTATE_EVENT_ID_ENABLE,      utlSTATE_EVENT_NAME_ENABLE,      { handler_name_prefix ## EventHandler } }
#define utlDEFINE_EVENT_HANDLER_DISABLE(    handler_name_prefix) { utlSTATE_EVENT_ID_DISABLE,     utlSTATE_EVENT_NAME_DISABLE,     { handler_name_prefix ## EventHandler } }
#define utlDEFINE_EVENT_HANDLER_LOAD_CONFIG(handler_name_prefix) { utlSTATE_EVENT_ID_LOAD_CONFIG, utlSTATE_EVENT_NAME_LOAD_CONFIG, { handler_name_prefix ## EventHandler } }
#define utlDEFINE_EVENT_HANDLER_POLL(       handler_name_prefix) { utlSTATE_EVENT_ID_POLL,        utlSTATE_EVENT_NAME_POLL,        { handler_name_prefix ## EventHandler } }
#define utlDEFINE_EVENT_HANDLER_REPLY(      handler_name_prefix) { utlSTATE_EVENT_ID_REPLY,       utlSTATE_EVENT_NAME_REPLY,       { handler_name_prefix ## EventHandler } }
#define utlDEFINE_EVENT_HANDLER_SUCCESS(    handler_name_prefix) { utlSTATE_EVENT_ID_SUCCESS,     utlSTATE_EVENT_NAME_SUCCESS,     { handler_name_prefix ## EventHandler } }
#define utlDEFINE_EVENT_HANDLER_FAILED(     handler_name_prefix) { utlSTATE_EVENT_ID_FAILED,      utlSTATE_EVENT_NAME_FAILED,      { handler_name_prefix ## EventHandler } }
#define utlDEFINE_EVENT_HANDLER_BAD_REPLY(  handler_name_prefix) { utlSTATE_EVENT_ID_BAD_REPLY,   utlSTATE_EVENT_NAME_BAD_REPLY,   { handler_name_prefix ## EventHandler } }
#define utlDEFINE_EVENT_HANDLER_RETRY(      handler_name_prefix) { utlSTATE_EVENT_ID_RETRY,       utlSTATE_EVENT_NAME_RETRY,       { handler_name_prefix ## EventHandler } }
#define utlDEFINE_EVENT_HANDLER_GIVE_UP(    handler_name_prefix) { utlSTATE_EVENT_ID_GIVE_UP,     utlSTATE_EVENT_NAME_GIVE_UP,     { handler_name_prefix ## EventHandler } }
#define utlDEFINE_EVENT_HANDLER_TIMEOUT(    handler_name_prefix) { utlSTATE_EVENT_ID_TIMEOUT,     utlSTATE_EVENT_NAME_TIMEOUT,     { handler_name_prefix ## EventHandler } }
#define utlDEFINE_EVENT_HANDLER_COMM_FAIL(  handler_name_prefix) { utlSTATE_EVENT_ID_COMM_FAIL,   utlSTATE_EVENT_NAME_COMM_FAIL,   { handler_name_prefix ## EventHandler } }
#define utlDEFINE_EVENT_HANDLER_CALL_BACK(  handler_name_prefix) { utlSTATE_EVENT_ID_CALL_BACK,   utlSTATE_EVENT_NAME_CALL_BACK,   { handler_name_prefix ## EventHandler } }
#define utlDEFINE_EVENT_HANDLER_FAIL(       handler_name_prefix) { utlSTATE_EVENT_ID_FAIL,        utlSTATE_EVENT_NAME_FAIL,        { handler_name_prefix ## EventHandler } }
#define utlDEFINE_EVENT_HANDLER_DEFAULT(    handler_name_prefix) { utlSTATE_EVENT_ID_DEFAULT,     utlSTATE_EVENT_NAME_DEFAULT,     { handler_name_prefix ## EventHandler } }

#define NULLEventHandler NULL
#define utlDEFINE_EVENT_HANDLER(event_id, event_name,  handler_name_prefix) \
	{ event_id, event_name, { handler_name_prefix ## EventHandler } }


typedef int utlStateMachineStateId_T, *utlStateMachineStateId_P;
typedef int utlStateMachineEventId_T, *utlStateMachineEventId_P;
typedef struct utlStateMachine_S *utlStateMachine_P;

typedef utlReturnCode_T (*utlStateMachineStateFunction_P)(const utlStateMachine_P state_machine_p,
							  const utlStateMachineStateId_T state,
							  const utlAbsoluteTime_P2c curr_time_p,
							  void                     *arg_p);

typedef utlReturnCode_T (*utlStateMachineEventFunction_P)(const utlStateMachine_P state_machine_p,
							  const utlStateMachineStateId_T state,
							  const utlStateMachineEventId_T event,
							  const utlAbsoluteTime_P2c curr_time_p,
							  void                     *arg_p,
							  va_list va_arg_p);

#define utlSTATE_CHANGE_HANDLER(handler_name_prefix, user_arg_p)					     \
	static utlReturnCode_T handler_name_prefix ## StateHandler(const utlStateMachine_P state_machine_p, \
								   const utlStateMachineStateId_T state,	    \
								   const utlAbsoluteTime_P2c curr_time_p,	\
								   void                     *user_arg_p)

#define utlEVENT_HANDLER(handler_name_prefix, user_arg_p, va_arg_p)					     \
	static utlReturnCode_T handler_name_prefix ## EventHandler(const utlStateMachine_P state_machine_p, \
								   const utlStateMachineStateId_T state,	    \
								   const utlStateMachineEventId_T event,	     \
								   const utlAbsoluteTime_P2c curr_time_p,	 \
								   void                     *user_arg_p,	  \
								   va_list va_arg_p)

#define utlSET_STATE(new_state)        utlSetStateMachineState(state_machine_p, new_state, curr_time_p)
#define utlPUSH_STATE(new_state)       utlPushStateMachineState(state_machine_p, new_state, curr_time_p)
#define utlPOP_STATE()                 utlPopStateMachineState(state_machine_p,            curr_time_p)
#define utlSCHEDULE_STATE(new_state)   utlScheduleStateMachineState(state_machine_p, new_state, curr_time_p)
#define utlRESCHEDULE_STATE(new_state) utlRescheduleStateMachineState(state_machine_p, new_state, curr_time_p)

#define utlEVENT(event)         utlStateMachineEvent(state_machine_p, event, curr_time_p)
#define utlVA_EVENT(event, ...) utlStateMachineEvent(state_machine_p, event, curr_time_p, __VA_ARGS__)

typedef struct utlStateMachineEvent_S {
	utlStateMachineEventId_T event_id;      /* id of this event   */
	const char               *name_p;       /* name of this event */

	struct {
		utlStateMachineEventFunction_P function_p;
	} call_back;

	struct {
		unsigned long events;
	} statistics;
}             utlStateMachineEvent_T, *utlStateMachineEvent_P;
typedef const utlStateMachineEvent_T  *utlStateMachineEvent_P2c;

typedef struct utlStateMachineState_S {
	utlStateMachineStateId_T state_id;              /* ID of current state   */
	const char               *name_p;               /* name of current state */

	utlStateMachineEvent_P events_p;                /* pointer to array of possible events */

	utlRelativeTime_P2c timeout_p;                  /* either NULL, or a pointer to a time-out period   */
	unsigned int             *max_retries_p;        /* either NULL, or a pointer to the max retry count */

	struct {
		utlStateMachineStateFunction_P enter_function_p;
		utlStateMachineStateFunction_P exit_function_p;
	} call_back;

	struct {
		utlTimerId_T timer_id;          /* ID of currently running timer */
		unsigned int num_retries;       /* current retry count           */
	} context;

	struct {
		unsigned long enters;
		unsigned long exits;
	} statistics;
}             utlStateMachineState_T, *utlStateMachineState_P;
typedef const utlStateMachineState_T  *utlStateMachineState_P2c;

typedef enum {
	utlSTATE_MACHINE_OP_UNKNOWN,
	utlSTATE_MACHINE_OP_SET,
	utlSTATE_MACHINE_OP_PUSH,
	utlSTATE_MACHINE_OP_SCHEDULE,
	utlSTATE_MACHINE_OP_RESCHEDULE,
	utlSTATE_MACHINE_OP_UNSCHEDULE,
	utlSTATE_MACHINE_OP_POP
} utlStateMachineOp_T;

typedef unsigned int utlStateMachineFlags_T;

typedef struct utlStateMachine_S {
	utlStateMachine_P next_p;

	char name[utlMAX_STATE_MACHINE_NAME_LENGTH]; /* name of this state machine */

	utlStateMachineFlags_T user_flags;

	utlSemaphore_T state_machine_semaphore; /* controls access to this state-machine */

	/* current state */
	struct {
		size_t curr_level;
		utlStateMachineState_P state_p[utlSTATE_MACHINE_MAX_LEVELS];
	} stack;

	struct {
		void *arg_p;
	} call_back;

	utlStateMachineState_P states_p;        /* pointer to array of possible states */

	unsigned long next_seq_num;             /* next unused sequence number */
	unsigned long curr_seq_num;             /* current     sequence number */

#   if defined(utlDEBUG) || defined(utlTEST)
	struct {
		size_t num_events;
		struct {
			utlAbsoluteTime_T time_stamp;
			unsigned long seq_num;                  /* event sequence number                        */
			utlStateMachineStateId_T state_id;      /* the state we were in when the event occurred */
			utlStateMachineEventId_T event_id;      /* the event that occurred while in `state'     */
		} events[utlSTATE_MACHINE_MAX_EVENT_HISTORY + 1];

		size_t num_states;
		struct {
			utlAbsoluteTime_T time_stamp;
			unsigned long seq_num;                  /* event sequence number */
			utlStateMachineOp_T op;
			utlStateMachineStateId_T prev_state_id; /* the previous state    */
			utlStateMachineStateId_T state_id;      /* the new state         */
		} states[utlSTATE_MACHINE_MAX_STATE_HISTORY + 1];
	} history;
#   endif /* defined(utlDEBUG) || defined(utlTEST) */

}             utlStateMachine_T;
typedef const utlStateMachine_T  *utlStateMachine_P2c;


/*--- Prototypes ------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

extern utlStateMachine_P      utlOpenStateMachine(const char                       *name_p,
						  const utlStateMachineFlags_T user_flags,
						  const utlStateMachineState_P states_p,
						  void                       *arg_p);
extern utlReturnCode_T        utlCloseStateMachine(const utlStateMachine_P state_machine_p);
extern utlReturnCode_T        utlVStateMachineEvent(const utlStateMachine_P state_machine_p,
						    const utlStateMachineEventId_T event_id,
						    const utlAbsoluteTime_P2c curr_time_p,
						    va_list va_arg_p);
extern utlReturnCode_T        utlStateMachineEvent(const utlStateMachine_P state_machine_p,
						   const utlStateMachineEventId_T event_id,
						   const utlAbsoluteTime_P2c curr_time_p,
						   ...);
extern int                    utlCurrentStateMachineLevel(const utlStateMachine_P state_machine_p);
extern bool                   utlIsStateMachineStateActive(const utlStateMachine_P state_machine_p,
							   const utlStateMachineStateId_T state_id);
extern utlStateMachineState_P utlFindStateMachineStateInfo(const utlStateMachine_P state_machine_p,
							   const utlStateMachineStateId_T state_id);
extern utlReturnCode_T        utlSetStateMachineState(const utlStateMachine_P state_machine_p,
						      const utlStateMachineStateId_T state_id,
						      const utlAbsoluteTime_P2c curr_time_p);
extern utlReturnCode_T        utlRetryStateMachineState(const utlStateMachine_P state_machine_p,
							const utlStateMachineStateId_T state_id,
							const utlAbsoluteTime_P2c curr_time_p);
extern utlReturnCode_T        utlPushStateMachineState(const utlStateMachine_P state_machine_p,
						       const utlStateMachineStateId_T state_id,
						       const utlAbsoluteTime_P2c curr_time_p);
extern utlReturnCode_T        utlScheduleStateMachineState(const utlStateMachine_P state_machine_p,
							   const utlStateMachineStateId_T state_id,
							   const utlAbsoluteTime_P2c curr_time_p);
extern utlReturnCode_T        utlRescheduleStateMachineState(const utlStateMachine_P state_machine_p,
							     const utlStateMachineStateId_T state_id,
							     const utlAbsoluteTime_P2c curr_time_p);
extern utlReturnCode_T        utlUnscheduleStateMachineState(const utlStateMachine_P state_machine_p,
							     const utlAbsoluteTime_P2c curr_time_p);
extern utlReturnCode_T        utlPopStateMachineState(const utlStateMachine_P state_machine_p,
						      const utlAbsoluteTime_P2c curr_time_p);
extern utlRelativeTime_T      utlStateMachineStateElapsedTime(const utlStateMachine_P state_machine_p);

extern utlReturnCode_T        utlSaveStateMachineEvent(const utlStateMachine_P state_machine_p,
						       const unsigned int priority,
						       const utlStateMachineEventId_T event_id,
						       void                       *user_data_p,
						       const utlAbsoluteTime_P2c curr_time_p);

#if defined(utlDEBUG) || defined(utlTEST)
extern void                   utlVStringDumpStateMachine(const utlVString_P v_string_p,
							 const utlStateMachine_P state_machine_p,
							 const char                       *prefix_p);
extern void                   utlDumpStateMachine(      FILE                       *file_p,
							const utlStateMachine_P state_machine_p);
extern void                   utlVStringDumpStateMachineHistory(const utlVString_P v_string_p,
								const utlStateMachine_P state_machine_p,
								const char                       *prefix_p);
extern void                   utlDumpStateMachineHistory(      FILE                       *file_p,
							       const utlStateMachine_P state_machine_p);
extern bool                   utlIsStateMachineStateSequence(const utlStateMachine_P state_machine_p,
							     const utlStateMachineStateId_P state_ids_p);
extern void                   utlClearStateMachineHistory(const utlStateMachine_P state_machine_p);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _UTL_STATE_MACHINE_INCLUDED */

