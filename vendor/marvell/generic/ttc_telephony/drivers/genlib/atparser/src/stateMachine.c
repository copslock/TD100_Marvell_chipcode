/*****************************************************************************
* Utility Library
*
* DESCRIPTION
*    State machine utility.  A state machine consists of two or more states.
*    The reception of events causes the state machine to transition from one
*    state to another.  This state machine implementation is table driven,
*    with (for a given state machine) one table containing all defined states,
*    and an event table for each state.  Callback functions (if specified)
*    are called each time an event occurs, each time a state is entered, and
*    each time a state is exited.  A timer can be automatically run whenever
*    a state is entered, and will be automatically stopped whenever a state
*    is exited.  When a timer times out, RETRY, GIVE_UP, or TIMEOUT events are
*    generated (as appropriate).  TIMEOUT events are generated if no
*    retry-count has been specified, otherwise RETRY events are generated until
*    the retry-count reaches it's maximum, at which point a single GIVE_UP
*    event is generated.
*
*    This utility is thread-safe.
*
* EXAMPLE USAGE
*
*      --- The sample state machine ---
*
*                     BOOT
*                      |
*                      |reset
*                      v
*                   STOPPED<--------------.
*                      |                  |
*                      |start             |
*                      v          stop    |
*       .----------->ACTIVE-------------->|
*       |              |                  |
*       |    .--------.|poll timeout      |
*       |    |        ||                  |
*       |    |retry   vv          stop    |
*       |    `--------POLL--------------->|
*       |             ||                  |
*       |        reply||                  |
*       `-------------'|give-up           |
*                      |                  |
*                      v         stop     |
*                   FAILED----------------'
*
*      --- Event Handlers ---
*      utlEVENT_HANDLER(myBootReset, arg_p, va_arg_p)
*      {
*          <react to reception of a BOOT event here>
*
*          return utlSET_STATE(utlSTATE_ID_STOPPED);
*      }
*
*      utlEVENT_HANDLER(myStoppedStart, arg_p, va_arg_p)
*      {
*          <react to reception of a START event here>
*
*          return utlSET_STATE(utlSTATE_ID_ACTIVE);
*      }
*
*      utlEVENT_HANDLER(myActiveStop, arg_p, va_arg_p)
*      {
*          <react to reception of a STOP event here>
*
*          return utlSET_STATE(utlSTATE_ID_STOPPED);
*      }
*
*      utlEVENT_HANDLER(myActiveTimeout, arg_p, va_arg_p)
*      {
*          <react to reception of a poll TIMEOUT event here>
*
*          return utlPUSH_STATE(utlSTATE_POLL);
*      }
*
*      utlEVENT_HANDLER(myPollStop, arg_p, va_arg_p)
*      {
*          <react to reception of a STOP event here>
*
*          if (utlRESCHEDULE_STATE(utlSTATE_ID_STOPPED) != utlSUCCESS)
*              <handle error here>
*
*          return utlPOP_STATE();
*      }
*
*      utlEVENT_HANDLER(myPollReply, arg_p, va_arg_p)
*      {
*          <react to reception of a poll REPLY event here>
*
*          return utlPOP_STATE();
*      }
*
*      utlEVENT_HANDLER(myPollRetry, arg_p, va_arg_p)
*      {
*          <react to reception of a RETRY poll event here>
*
*         return utlSET_STATE(utlSTATE_ID_POLL);
*      }
*
*      utlEVENT_HANDLER(myPollGiveUp, arg_p, va_arg_p)
*      {
*          <react to reception of a GIVE-UP event here>
*
*          if (utlRESCHEDULE_STATE(utlSTATE_FAILED) != utlSUCCESS)
*              <handle error here>
*
*          return utlPOP_STATE();
*      }
*
*      utlEVENT_HANDLER(myFailedStop, arg_p, va_arg_p)
*      {
*          <react to reception of a STOP event here>
*
*          return utlSET_STATE(utlSTATE_ID_STOPPED);
*      }
*
*      --- State-change Handlers ---
*      utlSTATE_CHANGE_HANDLER(myEnterBoot, arg_p)
*      {
*          <do stuff when entering BOOT state here>
*
*          return utlSUCCESS;
*      }
*
*      utlSTATE_CHANGE_HANDLER(myEnterStopped, arg_p)
*      {
*          <do stuff when entering STOPPED state here>
*
*          return utlSUCCESS;
*      }
*
*      utlSTATE_CHANGE_HANDLER(myExitStopped, arg_p)
*      {
*          <do stuff when leaving STOPPED state here>
*
*          return utlSUCCESS;
*      }
*
*      utlSTATE_CHANGE_HANDLER(myEnterActive, arg_p)
*      {
*          <do stuff when entering ACTIVE state here>
*
*          return utlSUCCESS;
*      }
*
*      utlSTATE_CHANGE_HANDLER(myExitActive, arg_p)
*      {
*          <do stuff when leaving ACTIVE state here>
*
*          return utlSUCCESS;
*      }
*
*      utlSTATE_CHANGE_HANDLER(myEnterPoll, arg_p)
*      {
*          <do stuff when entering POLL state here (like send out a poll message)>
*
*          return utlSUCCESS;
*      }
*
*      --- State-machine tables ---
*      static utlStateMachineEvent_T    boot_state[] = {utlDEFINE_EVENT_HANDLER_RESET(myBootReset),
*                                                utlDEFINE_EVENT_HANDLER_NULL()};
*
*      static utlStateMachineEvent_T stopped_state[] = {utlDEFINE_EVENT_HANDLER_START(myStoppedStart),
*                                                utlDEFINE_EVENT_HANDLER_NULL()};
*
*      static utlStateMachineEvent_T  active_state[] = {utlDEFINE_EVENT_HANDLER_STOP(myActiveStop),
*                                                utlDEFINE_EVENT_HANDLER_TIMEOUT(myActiveTimeout),
*                                                utlDEFINE_EVENT_HANDLER_NULL()};
*
*      static utlStateMachineEvent_T    poll_state[] = {utlDEFINE_EVENT_HANDLER_STOP(myPollStop),
*                                                utlDEFINE_EVENT_HANDLER_REPLY(myPollReply),
*                                                utlDEFINE_EVENT_HANDLER_RETRY(myPollRetry),
*                                                utlDEFINE_EVENT_HANDLER_GIVE_UP(myPollGiveUp),
*                                                utlDEFINE_EVENT_HANDLER_NULL()};
*
*      static utlStateMachineEvent_T  failed_state[] = {utlDEFINE_EVENT_HANDLER_STOP(myFailedStop),
*                                                utlDEFINE_EVENT_HANDLER_NULL()};
*
*      static utlRelativeTime_T poll_period;
*      static utlRelativeTime_T request_timeout;
*      static unsigned int      request_retries;
*
*      static utlStateMachineState_T my_states[] = {utlDEFINE_STATE_BOOT(      boot_state, NULL,             NULL,             myEnterBoot,    NULL),
*                                            utlDEFINE_STATE_STOPPED(stopped_state, NULL,             NULL,             myEnterStopped, myExitStopped),
*                                            utlDEFINE_STATE_ACTIVE(  active_state, &poll_period,     NULL,             myEnterActive,  myExitActive),
*                                            utlDEFINE_STATE_POLL(      poll_state, &request_timeout, &request_retries, myEnterPoll,    NULL),
*                                            utlDEFINE_STATE_FAILED(  failed_state, NULL,             NULL,             NULL,           NULL),
*                                            utlDEFINE_STATE_NULL()};
*
*      main()
*      {
*      utlStateMachine_P state_machine_p;
*
*           poll_period.seconds     = 60;
*           poll_period.nanoseconds = 0;
*
*           request_timeout.seconds     = 5;
*           request_timeout.nanoseconds = 0;
*
*           request_retries = 3;
*
*          if ((state_machine_p = utlOpenStateMachine("my state machine", 0, my_states, NULL)) == NULL)
*              <handle error here>
*
*          if (utlEventLoop(true) != utlSUCCESS)
*              <handle error here>
*
*          utlDumpStateMachine(stderr, state_machine_p);
*          utlDumpStateMachineHistory(stderr, state_machine_p);
*
*          if (utlCloseStateMachine(state_machine_p) != utlSUCCESS)
*              <handle error here>
*
*          return 0;
*      }
*
*****************************************************************************/

#include <errno.h>
#include <string.h>
#include <stdarg.h>

#include <utlTypes.h>
#include <utlTimer.h>
#include <utlError.h>
#include <utlMalloc.h>
#include <utlSemaphore.h>
#include <utlTrace.h>

#ifdef utlTEST
#   include <utlEventHandler.h>
#endif

#include <utlStateMachine.h>


/*--- Configuration ---------------------------------------------------------*/

/*--- maximum number of states in any given state machine ---*/
#define utlSTATE_MACHINE_MAX_STATES ((size_t)48)

/*--- maximum number of events in any given state ---*/
#define utlSTATE_MACHINE_MAX_EVENTS ((size_t)32)

/*--- for tracing state machines ---*/
#define utlTRACE_STATE_MACHINE "state machines"


/*--- Local Data ------------------------------------------------------------*/

static utlSemaphore_T state_machines_semaphore = utlNULL_SEMAPHORE;
static utlLinkedList_T state_machines           = utlEMPTY_LINKED_LIST;


#if defined(utlDEBUG) || defined(utlTEST)

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlClearStateMachineHistory(state_machine_p)
* INPUT
*      state_machine_p == pointer to an initialized utlStateMachine_T structure
* OUTPUT
*	none
* RETURNS
*	nothing
* DESCRIPTION
*	Clears the state-machine state transition history.
*---------------------------------------------------------------------------*/
void utlClearStateMachineHistory(const utlStateMachine_P state_machine_p)
{
	size_t index;

	utlAssert(state_machine_p != NULL);

	/*--- clear event occurrences history ---*/
	state_machine_p->history.num_events = 0;
	for (index = 0; index < (utlSTATE_MACHINE_MAX_EVENT_HISTORY + 1); index++)
	{
		state_machine_p->history.events[index].time_stamp.seconds     = 0;
		state_machine_p->history.events[index].time_stamp.nanoseconds = 0;
		state_machine_p->history.events[index].state_id = utlSTATE_ID_NULL;
		state_machine_p->history.events[index].event_id = utlSTATE_EVENT_ID_NULL;
	}

	/*--- clear state-transitions history ---*/
	state_machine_p->history.num_states = 0;
	for (index = 0; index < (utlSTATE_MACHINE_MAX_STATE_HISTORY + 1); index++)
	{
		state_machine_p->history.events[index].time_stamp.seconds     = 0;
		state_machine_p->history.events[index].time_stamp.nanoseconds = 0;
		state_machine_p->history.states[index].op            = utlSTATE_MACHINE_OP_UNKNOWN;
		state_machine_p->history.states[index].prev_state_id = utlSTATE_ID_NULL;
		state_machine_p->history.states[index].state_id      = utlSTATE_ID_NULL;
	}
}
#endif

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlOpenStateMachine(name_p, user_flags, states_p, arg_p)
* INPUT
*          name_p == pointer to a null-terminated character string
*      user_flags == user flags
*        states_p == pointer to a filled array of utlStateMachine_T structures
*	     arg_p == pointer to user-defined data
* OUTPUT
*	none
* RETURNS
*	A pointer to an initialized state machine structure for success, a NULL
*      pointer for failure.
* DESCRIPTION
*	Opens and initializes a new state machine.
*---------------------------------------------------------------------------*/
utlStateMachine_P utlOpenStateMachine(const char                   *name_p,
				      const utlStateMachineFlags_T user_flags,
				      const utlStateMachineState_P states_p,
				      void                   *arg_p)
{
	utlStateMachine_P state_machine_p;

	utlAssert(name_p   != NULL);
	utlAssert(states_p != NULL);

	/*--- check passed-in state-machine data structures ---*/
	{
		utlStateMachineState_P state_p;
		size_t num_states;

		num_states = 0;

		/*--- check for missing NULL states and events ---*/
		for (state_p = states_p; state_p->state_id != utlSTATE_ID_NULL; state_p++)
		{
			utlStateMachineEvent_P event_p;

			/*--- ensure there are not too many states ---*/
			num_states++;
			utlAssert(num_states <= utlSTATE_MACHINE_MAX_STATES);

			{
				size_t num_events;

				num_events = 0;

				for (event_p = state_p->events_p; event_p->event_id != utlSTATE_EVENT_ID_NULL; event_p++)
				{

					/*--- ensure there are not too many events ---*/
					num_events++;
					utlAssert(num_events <= utlSTATE_MACHINE_MAX_EVENTS);
				}
				if ((event_p->name_p == NULL) || (strcmp(event_p->name_p, "Null") != 0))
				{
					utlError("No NULL event in state machine `%s', state `%s'", name_p, state_p->name_p);
					return NULL;
				}
			}
		}
		if ((state_p->name_p == NULL) || (strcmp(state_p->name_p, "Null") != 0))
		{
			utlError("No NULL state in state machine `%s'", name_p);
			return NULL;
		}

		/*--- for each state... ---*/
		for (state_p = states_p; state_p->state_id != utlSTATE_ID_NULL; state_p++)
		{
			utlStateMachineEvent_P event_p;

			/*--- check for unnamed states/events ---*/
			{
				utlAssert(state_p->name_p != NULL);

				for (event_p = state_p->events_p; event_p->event_id != utlSTATE_EVENT_ID_NULL; event_p++)
					utlAssert(event_p->name_p != NULL);
			}

			/*--- check for states with duplicate ID's or names ---*/
			{
				utlStateMachineState_P item_p;

				for (item_p = states_p; item_p != state_p; item_p++)
				{
					if ((       item_p->state_id == state_p->state_id) ||
					    (strcmp(item_p->name_p,     state_p->name_p) == 0))
					{
						utlError("Duplicate states in state machine `%s', state `%s'", name_p, state_p->name_p);
						return NULL;
					}
				}
			}

			/*--- if a timeout is specified, check that a timeout event-handler is defined ---*/
			if (state_p->timeout_p != NULL)
			{
				utlAssert(state_p->events_p != NULL);

				/*--- if no retries were specified ---*/
				if ((state_p->max_retries_p != NULL) && (*(state_p->max_retries_p) > (unsigned int)0))
				{
					for (event_p = state_p->events_p; event_p->event_id != utlSTATE_EVENT_ID_NULL; event_p++)
						if ((event_p->event_id == utlSTATE_EVENT_ID_RETRY) ||
						    (event_p->event_id == utlSTATE_EVENT_ID_GIVE_UP))
							break;
				}
				else
				{
					for (event_p = state_p->events_p; event_p->event_id != utlSTATE_EVENT_ID_NULL; event_p++)
						if (event_p->event_id == utlSTATE_EVENT_ID_TIMEOUT)
							break;
				}
				utlAssert(event_p->event_id != utlSTATE_EVENT_ID_NULL);
			}

			/*--- if max-retries is specified, check that a retry event-handler is defined ---*/
			if ((state_p->max_retries_p != NULL) && (*(state_p->max_retries_p) > (unsigned int)0))
			{
				utlAssert(state_p->events_p != NULL);

				for (event_p = state_p->events_p; event_p->event_id != utlSTATE_EVENT_ID_NULL; event_p++)
					if (event_p->event_id == utlSTATE_EVENT_ID_RETRY)
						break;
				utlAssert(event_p->event_id != utlSTATE_EVENT_ID_NULL);
			}

			/*--- if any events are defined for this state ---*/
			if (state_p->events_p != NULL)
			{
				/*--- for each event accepted in this state... ---*/
				for (event_p = state_p->events_p; event_p->event_id != utlSTATE_EVENT_ID_NULL; event_p++)
				{
					utlStateMachineEvent_P item_p;

					/*--- check for events with duplicate IDs or names ---*/
					for (item_p = state_p->events_p; item_p != event_p; item_p++)
						if ((       item_p->event_id == event_p->event_id) ||
						    (strcmp(item_p->name_p,     event_p->name_p) == 0))
						{
							utlError("Duplicate events in state machine `%s', state `%s', event `%s'", name_p, state_p->name_p, event_p->name_p);
							return NULL;
						}
				}
			}
		}
	}

	/*--- semaphore initialization ---*/
	if (!utlIsSemaphoreInitialized(&state_machines_semaphore))
	{
		if (utlInitSemaphore(&state_machines_semaphore,
				     utlSEMAPHORE_ATTR_SHARING_ENABLE |
				     utlSEMAPHORE_ATTR_NESTING_ENABLE |
				     utlSEMAPHORE_ATTR_SCOPE_PROCESS_PRIVATE) != utlSUCCESS)

			return NULL;
	}

	/*--- allocate space for state machine structure ---*/
	if ((state_machine_p = utlMalloc(sizeof(utlStateMachine_T))) == NULL)
		return NULL;

	/*--- initialize state machine structure ---*/
	{
		memset(state_machine_p, 0, sizeof(utlStateMachine_T));

		state_machine_p->next_p = NULL;

		/*--- save state-machine's name ---*/
		strncpy(state_machine_p->name, name_p, sizeof(state_machine_p->name) - 1);
		state_machine_p->name[         sizeof(state_machine_p->name) - 1] = '\0';

		state_machine_p->user_flags = user_flags;

		if (utlInitSemaphore(&(state_machine_p->state_machine_semaphore),
				     utlSEMAPHORE_ATTR_SHARING_ENABLE |
				     utlSEMAPHORE_ATTR_NESTING_ENABLE |
				     utlSEMAPHORE_ATTR_SCOPE_PROCESS_PRIVATE) != utlSUCCESS)
		{

			/*--- clean ---*/
			utlFree(state_machine_p);

			return NULL;
		}

		state_machine_p->call_back.arg_p = arg_p;
		state_machine_p->states_p        = states_p;
		state_machine_p->next_seq_num    = 0;
		state_machine_p->curr_seq_num    = 0; /* `0' means no current event */

#       if defined(utlDEBUG) || defined(utlTEST)
		utlClearStateMachineHistory(state_machine_p);
#       endif   /* defined(utlDEBUG) || defined(utlTEST) */
	}

	/*--- initialize state-machine context ---*/
	{
		utlStateMachineState_P state_p;

		/*--- for each state... ---*/
		for (state_p = state_machine_p->states_p; state_p->state_id != utlSTATE_ID_NULL; state_p++)
		{
			state_p->context.timer_id    = 0;
			state_p->context.num_retries = 0;
		}
	}

	/*--- initialize state-machine statistics ---*/
	{
		utlStateMachineState_P state_p;

		/*--- for each state... ---*/
		for (state_p = state_machine_p->states_p; state_p->state_id != utlSTATE_ID_NULL; state_p++)
		{
			utlStateMachineEvent_P event_p;

			/*--- for each event accepted in this state... ---*/
			if (state_p->events_p != NULL)
				for (event_p = state_p->events_p; event_p->event_id != utlSTATE_EVENT_ID_NULL; event_p++)
					event_p->statistics.events = 0;

			state_p->statistics.enters = 0;
			state_p->statistics.exits  = 0;
		}
	}

	/*--- acquire write-access ---*/
	if (utlAcquireExclusiveAccess(&state_machines_semaphore) != utlSUCCESS)
	{

		/*--- clean ---*/
		utlFree(state_machine_p);

		return NULL;
	}

	(void)utlPutTailNode(&state_machines, utlStateMachine_T, state_machine_p);

	utlTrace(utlTRACE_STATE_MACHINE,
		 utlPrintTrace("open state-machine `%s'\n", state_machine_p->name);
		 );

	/*--- enter BOOT state... ---*/
	{
		utlStateMachineState_P state_p;
		utlAbsoluteTime_T curr_time;

		/*--- fetch current time ---*/
		if (utlCurrentTime(&curr_time) != utlSUCCESS)
		{

			/*--- clean ---*/
			(void)utlGetNode(&state_machines, utlStateMachine_T, NULL, state_machine_p);
			(void)utlReleaseExclusiveAccess(&state_machines_semaphore);
			utlFree(state_machine_p);

			return NULL;
		}

		/*--- search for boot-state ---*/
		for (state_p = state_machine_p->states_p; state_p->state_id != utlSTATE_ID_NULL; state_p++)
			if (state_p->state_id == utlSTATE_ID_BOOT)
				break;

		/*--- ensure boot state was found ---*/
		utlAssert(state_p->state_id == utlSTATE_ID_BOOT);

		/*--- set current state ---*/
		state_machine_p->stack.curr_level      = 0;
		state_machine_p->stack.state_p[0] = state_p;

		/*--- update statistics ---*/
		state_p->statistics.enters++;

#       if defined(utlDEBUG) || defined(utlTEST)
		/*--- update state-transitions history ---*/
		{
			size_t index;

			index = state_machine_p->history.num_states % (utlSTATE_MACHINE_MAX_EVENT_HISTORY + 1);

			state_machine_p->history.states[index].time_stamp    = curr_time;
			state_machine_p->history.states[index].op            = utlSTATE_MACHINE_OP_SET;
			state_machine_p->history.states[index].prev_state_id = utlSTATE_ID_NULL;
			state_machine_p->history.states[index].state_id      = utlSTATE_ID_BOOT;
			state_machine_p->history.num_states++;
		}
#       endif   /* defined(utlDEBUG) || defined(utlTEST) */

		/*--- invoke enter-boot-state call-back (if any) ---*/
		if (state_p->call_back.enter_function_p != NULL)
		{
			if ((*(state_p->call_back.enter_function_p))(state_machine_p,
								     utlSTATE_ID_BOOT,
								     &curr_time,
								     state_machine_p->call_back.arg_p) != utlSUCCESS)
			{

				utlError("Callback function 0x%08x returned failure", state_p->call_back.enter_function_p);

				/*--- clean ---*/
				(void)utlGetNode(&state_machines, utlStateMachine_T, NULL, state_machine_p);
				(void)utlReleaseExclusiveAccess(&state_machines_semaphore);
				utlFree(state_machine_p);

				return NULL;
			}
		}

		/*--- we don't support timers in the boot state ---*/
		utlAssert(state_p->timeout_p     == NULL);
		utlAssert(state_p->max_retries_p == NULL);
	}

	/*--- release write-access ---*/
	if (utlReleaseExclusiveAccess(&state_machines_semaphore) != utlSUCCESS)
	{

		/*--- clean ---*/
		(void)utlGetNode(&state_machines, utlStateMachine_T, NULL, state_machine_p);
		(void)utlReleaseExclusiveAccess(&state_machines_semaphore);
		utlFree(state_machine_p);

		return NULL;
	}

	return state_machine_p;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlCloseStateMachine(state_machine_p)
* INPUT
*      state_machine_p == pointer to an open utlStateMachine_T structure
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, otherwise utlFAILED
* DESCRIPTION
*	Closes the state machine pointed to by `state_machine_p', freeing
*      all allocated resources associated with this state machine.
*---------------------------------------------------------------------------*/
utlReturnCode_T utlCloseStateMachine(const utlStateMachine_P state_machine_p)
{
	utlStateMachineState_P state_p;

	utlAssert(state_machine_p != NULL);

	/*--- acquire write-access ---*/
	if (utlAcquireExclusiveAccess(&(state_machine_p->state_machine_semaphore)) != utlSUCCESS)
		return utlFAILED;

	/*--- stop all running timers ---*/
	for (state_p = state_machine_p->states_p; state_p->state_id != utlSTATE_ID_NULL; state_p++)
		if (state_p->context.timer_id != 0)
			if (utlStopTimer(state_p->context.timer_id) != utlSUCCESS)
			{

				/*--- clean ---*/
				(void)utlReleaseExclusiveAccess(&(state_machine_p->state_machine_semaphore));

				return utlFAILED;
			}

	utlTrace(utlTRACE_STATE_MACHINE,
		 utlPrintTrace("close state-machine `%s'\n", state_machine_p->name);
		 );

	/*--- acquire write-access ---*/
	if (utlAcquireExclusiveAccess(&state_machines_semaphore) != utlSUCCESS)
	{

		/*--- clean ---*/
		(void)utlReleaseExclusiveAccess(&(state_machine_p->state_machine_semaphore));

		return utlFAILED;
	}

	(void)utlGetNode(&state_machines, utlStateMachine_T, NULL, state_machine_p);
	utlFreeConst(state_machine_p);

	/*--- release write-access ---*/
	if (utlReleaseExclusiveAccess(&state_machines_semaphore) != utlSUCCESS)
	{

		/*--- clean ---*/
		(void)utlReleaseExclusiveAccess(&(state_machine_p->state_machine_semaphore));

		return utlFAILED;
	}

	/*--- release write-access ---*/
	if (utlReleaseExclusiveAccess(&(state_machine_p->state_machine_semaphore)) != utlSUCCESS)
		return utlFAILED;

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	_utlVStateMachineEvent(state_machine_p, event_id, curr_time_p, va_arg_p)
* INPUT
*      state_machine_p == pointer to an open utlStateMachine_T structure
*             event_id == which event occurred
*          curr_time_p == pointer to the current time
*             va_arg_p == argument pointer
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, otherwise utlFAILED
* DESCRIPTION
*	Notifies the state machine of the occurrence of the specified `event_id',
*      which causes it to invoke the associated event call-back functions
*      (if any).
*---------------------------------------------------------------------------*/
static utlReturnCode_T _utlVStateMachineEvent(const utlStateMachine_P state_machine_p,
					      const utlStateMachineEventId_T event_id,
					      const utlAbsoluteTime_P2c curr_time_p,
					      va_list va_arg_p)
{
	utlAbsoluteTime_T curr_time;
	utlAbsoluteTime_P2c time_p;
	utlStateMachineEvent_P event_p;
	utlStateMachineState_P state_p;

	utlAssert(state_machine_p != NULL);

	utlAssert(state_machine_p->stack.state_p[state_machine_p->stack.curr_level] != NULL);
	state_p = state_machine_p->stack.state_p[state_machine_p->stack.curr_level];

	/*--- if current time was not supplied ---*/
	if (curr_time_p == NULL)
	{

		/*--- fetch current time ---*/
		if (utlCurrentTime(&curr_time) != utlSUCCESS)
			return utlFAILED;

		time_p = &curr_time;
	}
	else
		time_p = curr_time_p;

	/*--- search for event... ---*/
	for (event_p = state_p->events_p; event_p->event_id != utlSTATE_EVENT_ID_NULL; event_p++)
		if (event_p->event_id == event_id)
			break;

	/*--- if corresponding event was not found, look up default event... ---*/
	if (event_p->event_id == utlSTATE_EVENT_ID_NULL)
	{

		for (event_p = state_p->events_p; event_p->event_id != utlSTATE_EVENT_ID_NULL; event_p++)
			if (event_p->event_id == utlSTATE_EVENT_ID_DEFAULT)
				break;

		/*--- if no default event, assert ---*/
		utlAssert(event_p->event_id != utlSTATE_EVENT_ID_NULL);
	}

	utlTrace(utlTRACE_STATE_MACHINE,
		 utlPrintTrace("event `%s' (%d)\n",
			       event_p->name_p,
			       event_p->event_id);
		 );

	/*--- advance sequence number ---*/
	while (++(state_machine_p->next_seq_num) == (unsigned long)0)
		/* do nothing */;
	state_machine_p->curr_seq_num = state_machine_p->next_seq_num;

	/*--- update statistics ---*/
	event_p->statistics.events++;

#   if defined(utlDEBUG) || defined(utlTEST)
	/*--- update event-occurrences history ---*/
	{
		size_t index;

		index = state_machine_p->history.num_events % (utlSTATE_MACHINE_MAX_EVENT_HISTORY + 1);

		state_machine_p->history.events[index].time_stamp = *time_p;
		state_machine_p->history.events[index].seq_num    = state_machine_p->curr_seq_num;
		state_machine_p->history.events[index].state_id   = state_p->state_id;
		state_machine_p->history.events[index].event_id   = event_id;
		state_machine_p->history.num_events++;
	}
#   endif /* defined(utlDEBUG) || defined(utlTEST) */


	/*--- invoke event call-back (if any) ---*/
	if ((event_p->call_back.function_p != NULL))
	{
		utlReturnCode_T rc;

		rc = (*(event_p->call_back.function_p))(state_machine_p,
							state_p->state_id,
							event_id,
							time_p,
							state_machine_p->call_back.arg_p,
							va_arg_p);

		if (rc != utlSUCCESS)
		{
			utlError("Callback function 0x%08x returned failure", state_p->call_back.enter_function_p);

			/*--- clean ---*/
			state_machine_p->curr_seq_num = 0; /* no current event */

			return utlFAILED;
		}
	}

	state_machine_p->curr_seq_num = 0; /* no current event */

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlVStateMachineEvent(state_machine_p, event_id, curr_time_p, va_arg_p)
* INPUT
*      state_machine_p == pointer to an open utlStateMachine_T structure
*             event_id == which event occurred
*          curr_time_p == pointer to the current time
*             va_arg_p == argument pointer
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, otherwise utlFAILED
* DESCRIPTION
*	Notifies the state machine of the occurrence of the specified `event_id',
*      which causes it to invoke the associated event call-back functions
*      (if any).
*---------------------------------------------------------------------------*/
utlReturnCode_T utlVStateMachineEvent(const utlStateMachine_P state_machine_p,
				      const utlStateMachineEventId_T event_id,
				      const utlAbsoluteTime_P2c curr_time_p,
				      va_list va_arg_p)
{
	utlReturnCode_T rc;

	utlAssert(state_machine_p != NULL);

	/*--- acquire write-access ---*/
	if (utlAcquireExclusiveAccess(&(state_machine_p->state_machine_semaphore)) != utlSUCCESS)
		return utlFAILED;

	rc = _utlVStateMachineEvent(state_machine_p, event_id, curr_time_p, va_arg_p);

	/*--- release write-access ---*/
	if (utlReleaseExclusiveAccess(&(state_machine_p->state_machine_semaphore)) != utlSUCCESS)
		return utlFAILED;

	return rc;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlStateMachineEvent(state_machine_p, event_id, curr_time_p, ...)
* INPUT
*      state_machine_p == pointer to an open utlStateMachine_T structure
*             event_id == which event occurred
*          curr_time_p == pointer to the current time or NULL
*                  ... == other user-defined function arguments
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, otherwise utlFAILED
* DESCRIPTION
*	Notifies the state machine of the occurrence of the specified `event_id',
*      which causes it to invoke the associated event call-back functions
*      (if any).
*---------------------------------------------------------------------------*/
utlReturnCode_T utlStateMachineEvent(const utlStateMachine_P state_machine_p,
				     const utlStateMachineEventId_T event_id,
				     const utlAbsoluteTime_P2c curr_time_p,
				     ...)
{
	utlReturnCode_T rc;
	va_list va_arg_p;

	utlAssert(state_machine_p != NULL);

	/*--- acquire write-access ---*/
	if (utlAcquireExclusiveAccess(&(state_machine_p->state_machine_semaphore)) != utlSUCCESS)
		return utlFAILED;

	va_start(va_arg_p, curr_time_p);
	rc = _utlVStateMachineEvent(state_machine_p, event_id, curr_time_p, va_arg_p);
	va_end(va_arg_p);

	/*--- release write-access ---*/
	if (utlReleaseExclusiveAccess(&(state_machine_p->state_machine_semaphore)) != utlSUCCESS)
		return utlFAILED;

	return rc;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlStateMachineBroadcast(user_flags, event_id, curr_time_p, ...)
* INPUT
*       user_flags == flags to specify which state machines to broadcast to
*         event_id == which event occurred
*      curr_time_p == pointer to the current time or NULL
*              ... == other user-defined function arguments
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, otherwise utlFAILED
* DESCRIPTION
*	Notifies all state machines matching "user_flags" of the occurrence
*      of the specified `event_id', which causes them to invoke the associated
*      event call-back functions (if any).  A state machine matches if the
*      result of "anding" `user_flags' with a given state machine's user
*      flags is non-zero.
*---------------------------------------------------------------------------*/
utlReturnCode_T utlStateMachineBroadcast(const utlStateMachineFlags_T user_flags,
					 const utlStateMachineEventId_T event_id,
					 const utlAbsoluteTime_P2c curr_time_p,
					 ...)
{
	utlReturnCode_T rc;
	utlStateMachine_P state_machine_p;
	va_list va_arg_p;

	rc = utlSUCCESS;

	/*--- for every state machine whose "set" user_flags intersects that of `user_flags' ---*/
	va_start(va_arg_p, curr_time_p);
	for (state_machine_p = (utlStateMachine_P)state_machines.head_p; state_machine_p != NULL; state_machine_p = state_machine_p->next_p)
		if ((state_machine_p->user_flags & user_flags) != (utlStateMachineFlags_T)0)
			if (utlVStateMachineEvent(state_machine_p, event_id, curr_time_p, va_arg_p) != utlSUCCESS)
				rc = utlFAILED;
	va_end(va_arg_p);

	return rc;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	_utlStateMachineStateTimeoutHandler(state_machine_p, curr_time_p)
* INPUT
*	      arg_p == pointer to user-defined data
*	curr_time_p == current time since the epoch
* OUTPUT
*	modifies the data pointed to by `arg_p'
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure.
* DESCRIPTION
*	Handles internal state-machine time-out events: issues a new state
*      machine event.  If we have not reached max-retries, we issue a RETRY
*      event, else we issue either a TIMEOUT or GIVE_UP event.
*---------------------------------------------------------------------------*/
static utlReturnCode_T _utlStateMachineStateTimeoutHandler(const utlStateMachine_P state_machine_p,
							   const utlAbsoluteTime_P2c curr_time_p)
{
	utlStateMachineState_P state_p;

	utlAssert(state_machine_p != NULL);

	utlAssert(state_machine_p->stack.state_p[state_machine_p->stack.curr_level] != NULL);
	state_p = state_machine_p->stack.state_p[state_machine_p->stack.curr_level];

	utlAssert(state_p->timeout_p != NULL);

	/*--- no retries configured ---*/
	if ((state_p->max_retries_p == NULL) || (*(state_p->max_retries_p) == 0))
		return utlVStateMachineEvent(state_machine_p, utlSTATE_EVENT_ID_TIMEOUT, curr_time_p, NULL);

	/*--- give up? ---*/
	if (++(state_p->context.num_retries) > *(state_p->max_retries_p))
		return utlVStateMachineEvent(state_machine_p, utlSTATE_EVENT_ID_GIVE_UP, curr_time_p, NULL);

	/*--- try again ---*/
	return utlVStateMachineEvent(state_machine_p, utlSTATE_EVENT_ID_RETRY, curr_time_p, NULL);
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	_utlStateMachineStateTimeoutHandler(id, timeout_count, arg_p, curr_time_p)
* INPUT
*                 id == timer ID
*      timeout_count == the time-out count
*	        arg_p == pointer to user-defined data
*	  curr_time_p == current time since the epoch
* OUTPUT
*	modifies the data pointed to by `arg_p'
* RETURNS
*	utlSUCCESS for success, utlFAILED for failure.
* DESCRIPTION
*	Handles internal state-machine time-out events.
*---------------------------------------------------------------------------*/
static utlReturnCode_T utlStateMachineStateTimeoutHandler(const utlTimerId_T id,
							  const unsigned long timeout_count,
							  void                *arg_p,
							  const utlAbsoluteTime_P2c curr_time_p)
{
	utlStateMachine_P state_machine_p;
	utlReturnCode_T rc;

	state_machine_p = (utlStateMachine_P)arg_p;

	utlAssert(state_machine_p != NULL);

	/*--- acquire write-access ---*/
	if (utlAcquireExclusiveAccess(&(state_machine_p->state_machine_semaphore)) != utlSUCCESS)
		return utlFAILED;

	rc = _utlStateMachineStateTimeoutHandler(state_machine_p, curr_time_p);

	/*--- release write-access ---*/
	if (utlReleaseExclusiveAccess(&(state_machine_p->state_machine_semaphore)) != utlSUCCESS)
		return utlFAILED;

	return rc;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlCurrentStateMachineLevel(state_machine_p)
* INPUT
*      state_machine_p == pointer to an open utlStateMachine_T structure
* OUTPUT
*	none
* RETURNS
*	The current level for success, otherwise utlFAILED
* DESCRIPTION
*	Fetches the current level on the state machine's stack.
*---------------------------------------------------------------------------*/
int utlCurrentStateMachineLevel(const utlStateMachine_P state_machine_p)
{
	utlAssert(state_machine_p != NULL);

	return state_machine_p->stack.curr_level;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlIsStateMachineStateActive(state_machine_p, state_id)
* INPUT
*      state_machine_p == pointer to an open utlStateMachine_T structure
*             state_id == the state to test for
* OUTPUT
*	none
* RETURNS
*	"true" if `state_id' is active, otherwise "false"
* DESCRIPTION
*	Determines if the specified `state' is currently active.
*---------------------------------------------------------------------------*/
bool utlIsStateMachineStateActive(const utlStateMachine_P state_machine_p,
				  const utlStateMachineStateId_T state_id)
{
	size_t level;

	utlAssert(state_machine_p != NULL);

	/*--- acquire read-access ---*/
	if (utlAcquireSharedAccess(&(state_machine_p->state_machine_semaphore)) != utlSUCCESS)
		return false;

	/*--- search stack for `state'... ---*/
	for (level = 0; level <= state_machine_p->stack.curr_level; level++)
		if ((state_machine_p->stack.state_p[level] != NULL) &&
		    (state_machine_p->stack.state_p[level]->state_id == state_id))
		{

			/*--- release read-access ---*/
			if (utlReleaseSharedAccess(&(state_machine_p->state_machine_semaphore)) != utlSUCCESS)
				return false;

			return true;
		}

	/*--- release read-access ---*/
	(void)utlReleaseSharedAccess(&(state_machine_p->state_machine_semaphore));

	return false;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlFindStateMachineStateInfo(state_machine_p, state_id)
* INPUT
*      state_machine_p == pointer to an open utlStateMachine_T structure
*             state_id == the new state
* OUTPUT
*	none
* RETURNS
*	a pointer to the found state for success, otherwise NULL
* DESCRIPTION
*	Finds the state machine state specified by `state_id'.
*---------------------------------------------------------------------------*/
utlStateMachineState_P utlFindStateMachineStateInfo(const utlStateMachine_P state_machine_p,
						    const utlStateMachineStateId_T state_id)
{
	utlStateMachineState_P state_p;

	utlAssert(state_machine_p != NULL);

	/*--- acquire read-access ---*/
	if (utlAcquireSharedAccess(&(state_machine_p->state_machine_semaphore)) != utlSUCCESS)
		return false;

	/*--- search for new state... ---*/
	for (state_p = state_machine_p->states_p; state_p->state_id != utlSTATE_ID_NULL; state_p++)
		if (state_p->state_id == state_id)
			break;
	if (state_p->state_id == utlSTATE_ID_NULL)
	{
		utlError("Invalid state %d for state machine `%s'", state_id, state_machine_p->name);

		/*--- clean ---*/
		(void)utlReleaseSharedAccess(&(state_machine_p->state_machine_semaphore));

		return NULL;
	}

	/*--- release read-access ---*/
	if (utlReleaseSharedAccess(&(state_machine_p->state_machine_semaphore)) != utlSUCCESS)
		return false;

	return state_p;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	_utlEnterStateMachineState(state_machine_p, state_id, curr_time_p, retry_flag)
* INPUT
*      state_machine_p == pointer to an open utlStateMachine_T structure
*             state_id == the new state
*          curr_time_p == pointer to the current time
*           retry_flag == do we want to retry `state'?
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, otherwise utlFAILED
* DESCRIPTION
*	Sets the state machine state to `state_id', which causes the associated
*      state call-back functions (if any) to be invoked.
*
*      If the retry flag is true, we won't clear the retries count if the
*      current state and `state_id' are different.
*---------------------------------------------------------------------------*/
static utlReturnCode_T _utlEnterStateMachineState(const utlStateMachine_P state_machine_p,
						  const utlStateMachineStateId_T state_id,
						  const utlAbsoluteTime_P2c curr_time_p,
						  const bool retry_flag)
{
	utlAbsoluteTime_T curr_time;
	utlAbsoluteTime_P2c time_p;
	utlStateMachineState_P enter_state_p;
	utlStateMachineState_P exit_state_p;
	unsigned long curr_seq_num;
	unsigned int orig_num_retries;
	utlReturnCode_T rc;

	utlAssert(state_machine_p != NULL);

	/*--- if current time was not supplied ---*/
	if (curr_time_p == NULL)
	{

		/*--- fetch current time ---*/
		if (utlCurrentTime(&curr_time) != utlSUCCESS)
			return utlFAILED;

		time_p = &curr_time;
	}
	else
		time_p = curr_time_p;

	/*--- check old state... ---*/
	utlAssert(     state_machine_p->stack.state_p[state_machine_p->stack.curr_level] != NULL);
	exit_state_p = state_machine_p->stack.state_p[state_machine_p->stack.curr_level];

	/*--- search for new state... ---*/
	if ((enter_state_p = utlFindStateMachineStateInfo(state_machine_p, state_id)) == NULL)
		return utlFAILED;

	/*--- if we wish to retry `state' ---*/
	orig_num_retries = enter_state_p->context.num_retries;
	if (retry_flag == true)
	{

		/*--- no retries configured? ---*/
		if ((enter_state_p->max_retries_p == NULL) || (*(enter_state_p->max_retries_p) == 0))
			return utlVStateMachineEvent(state_machine_p, utlSTATE_EVENT_ID_TIMEOUT, time_p, NULL);

		/*--- give up? ---*/
		if (++(enter_state_p->context.num_retries) > *(enter_state_p->max_retries_p))
			return utlVStateMachineEvent(state_machine_p, utlSTATE_EVENT_ID_GIVE_UP, time_p, NULL);

		/*--- else if we're entering a new state, reset retry count ---*/
	}
	else if (enter_state_p != exit_state_p)
		enter_state_p->context.num_retries = 0;

	/*--- note current sequence number ---*/
	curr_seq_num = state_machine_p->curr_seq_num;

	/*--- exit old state... ---*/
	{
		utlTrace(utlTRACE_STATE_MACHINE,
			 utlPrintTrace("exit state `%s' (%d)\n",
				       exit_state_p->name_p,
				       exit_state_p->state_id);
			 );

		/*--- invoke exit-state call-back (if any) ---*/
		if (       exit_state_p->call_back.exit_function_p != NULL)
		{
			if ((*(exit_state_p->call_back.exit_function_p))(state_machine_p,
									 exit_state_p->state_id,
									 time_p,
									 state_machine_p->call_back.arg_p) != utlSUCCESS)
			{
				utlError("Exit-state call-back function 0x%08x returned failure", exit_state_p->call_back.enter_function_p);

				/*--- clean ---*/
				state_machine_p->curr_seq_num      = curr_seq_num;
				enter_state_p->context.num_retries = orig_num_retries;

				return utlFAILED;
			}
		}

		/*--- stop timer (if running) ---*/
		if (exit_state_p->context.timer_id != 0)
			if (utlStopTimer(exit_state_p->context.timer_id) != utlSUCCESS)
			{

				/*--- clean ---*/
				state_machine_p->curr_seq_num      = curr_seq_num;
				enter_state_p->context.num_retries = orig_num_retries;

				return utlFAILED;
			}
	}

	/*--- restore current sequence num (in case state-entry handler modified it) ---*/
	state_machine_p->curr_seq_num = curr_seq_num;

	/*--- update statistics ---*/
	exit_state_p->statistics.exits++;
	enter_state_p->statistics.enters++;

#   if defined(utlDEBUG) || defined(utlTEST)
	/*--- update state-transitions history ---*/
	{
		size_t index;

		index = state_machine_p->history.num_states % (utlSTATE_MACHINE_MAX_EVENT_HISTORY + 1);

		state_machine_p->history.states[index].time_stamp    = *time_p;
		state_machine_p->history.states[index].seq_num       = curr_seq_num;
		state_machine_p->history.states[index].op            = utlSTATE_MACHINE_OP_SET;
		state_machine_p->history.states[index].prev_state_id =  exit_state_p->state_id;
		state_machine_p->history.states[index].state_id      = enter_state_p->state_id;
		state_machine_p->history.num_states++;
	}
#   endif /* defined(utlDEBUG) || defined(utlTEST) */


	rc = utlSUCCESS;

	/*--- enter new state... ---*/
	{
		state_machine_p->stack.state_p[state_machine_p->stack.curr_level] = enter_state_p;

		utlTrace(utlTRACE_STATE_MACHINE,
			 utlPrintTrace("enter state `%s' (%d)\n",
				       enter_state_p->name_p,
				       enter_state_p->state_id);
			 );

		/*--- invoke enter-state call-back (if any) ---*/
		if (       enter_state_p->call_back.enter_function_p != NULL)
		{
			if ((*(enter_state_p->call_back.enter_function_p))(state_machine_p,
									   enter_state_p->state_id,
									   time_p,
									   state_machine_p->call_back.arg_p) != utlSUCCESS)
			{
				/*--- fixup statistics ---*/
				enter_state_p->statistics.enters--;
				exit_state_p->statistics.enters++;

#               if defined(utlDEBUG) || defined(utlTEST)
				/*--- fixup state-transitions history ---*/
				{
					size_t index;

					index = (state_machine_p->history.num_states - 1) % (utlSTATE_MACHINE_MAX_EVENT_HISTORY + 1);

					state_machine_p->history.states[index].state_id = exit_state_p->state_id;
				}
#               endif           /* defined(utlDEBUG) || defined(utlTEST) */

				/*--- re-enter exit state ---*/
				enter_state_p = exit_state_p;
				state_machine_p->stack.state_p[state_machine_p->stack.curr_level] = enter_state_p;
				enter_state_p->context.num_retries = orig_num_retries;

				if (          enter_state_p->call_back.enter_function_p != NULL)
					(void)(*(enter_state_p->call_back.enter_function_p))(state_machine_p,
											     enter_state_p->state_id,
											     time_p,
											     state_machine_p->call_back.arg_p);

				utlError("Enter-state call-back function 0x%08x returned failure", enter_state_p->call_back.enter_function_p);
				rc = utlFAILED;
			}
		}

		/*--- start timer (if configured) ---*/
		if (enter_state_p->timeout_p != NULL)
			if ((enter_state_p->context.timer_id = utlStartTimer(enter_state_p->timeout_p,
									     1,
									     utlStateMachineStateTimeoutHandler,
									     state_machine_p)) == utlFAILED)
			{
				/*--- clean ---*/
				enter_state_p->context.num_retries = orig_num_retries;

				rc = utlFAILED;
			}
	}

	/*--- restore current sequence num ---*/
	state_machine_p->curr_seq_num = curr_seq_num;

	return rc;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlSetStateMachineState(state_machine_p, state_id, curr_time_p)
* INPUT
*      state_machine_p == pointer to an open utlStateMachine_T structure
*             state_id == the new state
*          curr_time_p == pointer to the current time
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, otherwise utlFAILED
* DESCRIPTION
*	Sets the state machine state to `state_id', which causes the associated
*      state call-back functions to be invoked (if any).
*---------------------------------------------------------------------------*/
utlReturnCode_T utlSetStateMachineState(const utlStateMachine_P state_machine_p,
					const utlStateMachineStateId_T state_id,
					const utlAbsoluteTime_P2c curr_time_p)
{
	utlReturnCode_T rc;

	/*--- acquire write-access ---*/
	if (utlAcquireExclusiveAccess(&(state_machine_p->state_machine_semaphore)) != utlSUCCESS)
		return utlFAILED;

	rc = _utlEnterStateMachineState(state_machine_p, state_id, curr_time_p, false);

	/*--- release write-access ---*/
	if (utlReleaseExclusiveAccess(&(state_machine_p->state_machine_semaphore)) != utlSUCCESS)
		return utlFAILED;

	return rc;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlRetryStateMachineState(state_machine_p, state_id, curr_time_p)
* INPUT
*      state_machine_p == pointer to an open utlStateMachine_T structure
*             state_id == the new state
*          curr_time_p == pointer to the current time
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, otherwise utlFAILED
* DESCRIPTION
*	Re-enters the state machine state `state_id', which causes the associated
*      state call-back functions to be invoked (if any).
*---------------------------------------------------------------------------*/
utlReturnCode_T utlRetryStateMachineState(const utlStateMachine_P state_machine_p,
					  const utlStateMachineStateId_T state_id,
					  const utlAbsoluteTime_P2c curr_time_p)
{
	utlReturnCode_T rc;

	/*--- acquire write-access ---*/
	if (utlAcquireExclusiveAccess(&(state_machine_p->state_machine_semaphore)) != utlSUCCESS)
		return utlFAILED;

	rc = _utlEnterStateMachineState(state_machine_p, state_id, curr_time_p, true);

	/*--- release write-access ---*/
	if (utlReleaseExclusiveAccess(&(state_machine_p->state_machine_semaphore)) != utlSUCCESS)
		return utlFAILED;

	return rc;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	_utlPushStateMachineState(state_machine_p, state_id, curr_time_p)
* INPUT
*      state_machine_p == pointer to an open utlStateMachine_T structure
*             state_id == the new state
*          curr_time_p == pointer to the current time
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, otherwise utlFAILED
* DESCRIPTION
*	Saves the current state, then sets the state machine state to
*      `state_id', which causes the associated state call-back functions
*      to be invoked (if any).
*---------------------------------------------------------------------------*/
static utlReturnCode_T _utlPushStateMachineState(const utlStateMachine_P state_machine_p,
						 const utlStateMachineStateId_T state_id,
						 const utlAbsoluteTime_P2c curr_time_p)
{
	utlAbsoluteTime_T curr_time;
	utlAbsoluteTime_P2c time_p;
	utlStateMachineState_P enter_state_p;
	utlStateMachineState_P exit_state_p;
	unsigned long curr_seq_num;
	unsigned int orig_num_retries;
	utlReturnCode_T rc;

	utlAssert(state_machine_p != NULL);

	/*--- if current time was not supplied ---*/
	if (curr_time_p == NULL)
	{

		/*--- fetch current time ---*/
		if (utlCurrentTime(&curr_time) != utlSUCCESS)
			return utlFAILED;

		time_p = &curr_time;
	}
	else
		time_p = curr_time_p;

	/*--- check for stack overflow ---*/
	if ((state_machine_p->stack.curr_level + 1) >= utlSTATE_MACHINE_MAX_LEVELS)
	{
		utlError("State machine `%s' stack overflow", state_machine_p->name);
		return utlFAILED;
	}

	/*--- check old state... ---*/
	utlAssert(     state_machine_p->stack.state_p[state_machine_p->stack.curr_level] != NULL);
	exit_state_p = state_machine_p->stack.state_p[state_machine_p->stack.curr_level];

	/*--- search for new state... ---*/
	if ((enter_state_p = utlFindStateMachineStateInfo(state_machine_p, state_id)) == NULL)
		return utlFAILED;

	/*--- if we're entering a new state, reset retry count ---*/
	orig_num_retries = enter_state_p->context.num_retries;
	if (enter_state_p != exit_state_p)
		enter_state_p->context.num_retries = 0;

	/*--- note current sequence number ---*/
	curr_seq_num = state_machine_p->curr_seq_num;

	/*--- exit old state... ---*/
	{
		utlTrace(utlTRACE_STATE_MACHINE,
			 utlPrintTrace("exit (push) state `%s' (%d)\n",
				       exit_state_p->name_p,
				       exit_state_p->state_id);
			 );

		/*--- invoke exit-state call-back (if any) ---*/
		if (       exit_state_p->call_back.exit_function_p != NULL)
		{
			if ((*(exit_state_p->call_back.exit_function_p))(state_machine_p,
									 exit_state_p->state_id,
									 time_p,
									 state_machine_p->call_back.arg_p) != utlSUCCESS)
			{
				utlError("Exit-state call-back function 0x%08x returned failure", exit_state_p->call_back.enter_function_p);

				/*--- clean ---*/
				state_machine_p->curr_seq_num      = curr_seq_num;
				enter_state_p->context.num_retries = orig_num_retries;

				return utlFAILED;
			}
		}

		/*--- stop timer (if running) ---*/
		if (exit_state_p->context.timer_id != 0)
			if (utlStopTimer(exit_state_p->context.timer_id) != utlSUCCESS)
			{

				/*--- clean ---*/
				state_machine_p->curr_seq_num      = curr_seq_num;
				enter_state_p->context.num_retries = orig_num_retries;

				return utlFAILED;
			}
	}

	/*--- restore current sequence num (in case state-entry handler modified it) ---*/
	state_machine_p->curr_seq_num = curr_seq_num;

	/*--- update statistics ---*/
	exit_state_p->statistics.exits++;
	enter_state_p->statistics.enters++;

#   if defined(utlDEBUG) || defined(utlTEST)
	/*--- update state-transitions history ---*/
	{
		size_t index;

		index = state_machine_p->history.num_states % (utlSTATE_MACHINE_MAX_EVENT_HISTORY + 1);

		state_machine_p->history.states[index].time_stamp    = *time_p;
		state_machine_p->history.states[index].seq_num       = curr_seq_num;
		state_machine_p->history.states[index].op            = utlSTATE_MACHINE_OP_PUSH;
		state_machine_p->history.states[index].prev_state_id =  exit_state_p->state_id;
		state_machine_p->history.states[index].state_id      = enter_state_p->state_id;
		state_machine_p->history.num_states++;
	}
#   endif /* defined(utlDEBUG) || defined(utlTEST) */

	rc = utlSUCCESS;

	/*--- push new state... ---*/
	{
		state_machine_p->stack.curr_level++;
		state_machine_p->stack.state_p[state_machine_p->stack.curr_level] = enter_state_p;

		utlTrace(utlTRACE_STATE_MACHINE,
			 utlPrintTrace("enter (push) state `%s' (%d)\n",
				       enter_state_p->name_p,
				       enter_state_p->state_id);
			 );

		/*--- invoke enter-state call-back (if any) ---*/
		if (       enter_state_p->call_back.enter_function_p != NULL)
		{
			if ((*(enter_state_p->call_back.enter_function_p))(state_machine_p,
									   enter_state_p->state_id,
									   time_p,
									   state_machine_p->call_back.arg_p) != utlSUCCESS)
			{

				/*--- fixup statistics ---*/
				enter_state_p->statistics.enters--;
				exit_state_p->statistics.enters++;

#               if defined(utlDEBUG) || defined(utlTEST)
				/*--- fixup state-transitions history ---*/
				{
					size_t index;

					index = (state_machine_p->history.num_states - 1) % (utlSTATE_MACHINE_MAX_EVENT_HISTORY + 1);

					state_machine_p->history.states[index].state_id = exit_state_p->state_id;
				}
#               endif           /* defined(utlDEBUG) || defined(utlTEST) */

				/*--- re-enter exit state ---*/
				enter_state_p = exit_state_p;
				state_machine_p->stack.curr_level--;
				enter_state_p->context.num_retries = orig_num_retries;

				if (          enter_state_p->call_back.enter_function_p != NULL)
					(void)(*(enter_state_p->call_back.enter_function_p))(state_machine_p,
											     enter_state_p->state_id,
											     time_p,
											     state_machine_p->call_back.arg_p);

				utlError("Enter-state call-back function 0x%08x returned failure", enter_state_p->call_back.enter_function_p);
				rc = utlFAILED;
			}
		}

		/*--- start timer (if configured) ---*/
		if (enter_state_p->timeout_p != NULL)
			if ((enter_state_p->context.timer_id = utlStartTimer(enter_state_p->timeout_p,
									     1,
									     utlStateMachineStateTimeoutHandler,
									     state_machine_p)) == utlFAILED)
			{
				/*--- clean ---*/
				enter_state_p->context.num_retries = orig_num_retries;

				rc = utlFAILED;
			}
	}

	/*--- restore current sequence num ---*/
	state_machine_p->curr_seq_num = curr_seq_num;

	return rc;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlPushStateMachineState(state_machine_p, state_id, curr_time_p)
* INPUT
*      state_machine_p == pointer to an open utlStateMachine_T structure
*             state_id == the new state
*          curr_time_p == pointer to the current time
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, otherwise utlFAILED
* DESCRIPTION
*	Pushes a new state-machine state.
*---------------------------------------------------------------------------*/
utlReturnCode_T utlPushStateMachineState(const utlStateMachine_P state_machine_p,
					 const utlStateMachineStateId_T state_id,
					 const utlAbsoluteTime_P2c curr_time_p)
{
	utlReturnCode_T rc;

	utlAssert(state_machine_p != NULL);

	/*--- acquire write-access ---*/
	if (utlAcquireExclusiveAccess(&(state_machine_p->state_machine_semaphore)) != utlSUCCESS)
		return utlFAILED;

	rc = _utlPushStateMachineState(state_machine_p, state_id, curr_time_p);

	/*--- release write-access ---*/
	if (utlReleaseExclusiveAccess(&(state_machine_p->state_machine_semaphore)) != utlSUCCESS)
		return utlFAILED;

	return rc;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	_utlScheduleStateMachineState(state_machine_p, state_id, curr_time_p)
* INPUT
*      state_machine_p == pointer to an open utlStateMachine_T structure
*             state_id == the new state
*          curr_time_p == pointer to the current time
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, otherwise utlFAILED
* DESCRIPTION
*	Modifies the state-machine stack such that we'll enter the state
*      machine state `state_id' when we pop from the current state.
*---------------------------------------------------------------------------*/
static utlReturnCode_T _utlScheduleStateMachineState(const utlStateMachine_P state_machine_p,
						     const utlStateMachineStateId_T state_id,
						     const utlAbsoluteTime_P2c curr_time_p)
{
	utlAbsoluteTime_T curr_time;
	utlAbsoluteTime_P2c time_p;
	utlStateMachineState_P curr_state_p;
	utlStateMachineState_P state_p;

	utlAssert(state_machine_p != NULL);

	/*--- if current time was not supplied ---*/
	if (curr_time_p == NULL)
	{

		/*--- fetch current time ---*/
		if (utlCurrentTime(&curr_time) != utlSUCCESS)
			return utlFAILED;

		time_p = &curr_time;
	}
	else
		time_p = curr_time_p;

	/*--- check for stack overflow (if we're pushing) ---*/
	if ((state_machine_p->stack.curr_level + 1) >= utlSTATE_MACHINE_MAX_LEVELS)
	{
		utlError("State machine `%s' stack overflow", state_machine_p->name);
		return utlFAILED;
	}

	/*--- check current state... ---*/
	utlAssert(     state_machine_p->stack.state_p[state_machine_p->stack.curr_level] != NULL);
	curr_state_p = state_machine_p->stack.state_p[state_machine_p->stack.curr_level];

	/*--- search for state to be scheduled... ---*/
	if ((state_p = utlFindStateMachineStateInfo(state_machine_p, state_id)) == NULL)
		return utlFAILED;

	/*--- insert state under current state ---*/
	{
		state_machine_p->stack.state_p[state_machine_p->stack.curr_level + 1] = state_machine_p->stack.state_p[state_machine_p->stack.curr_level];
		state_machine_p->stack.state_p[state_machine_p->stack.curr_level]     = state_p;
		state_machine_p->stack.curr_level++;

		utlTrace(utlTRACE_STATE_MACHINE,
			 utlPrintTrace("schedule state `%s' (%d)\n",
				       state_p->name_p,
				       state_p->state_id);
			 );
	}

#   if defined(utlDEBUG) || defined(utlTEST)
	/*--- update state-transitions history ---*/
	{
		size_t index;

		index = state_machine_p->history.num_states % (utlSTATE_MACHINE_MAX_EVENT_HISTORY + 1);

		state_machine_p->history.states[index].time_stamp    = *time_p;
		state_machine_p->history.states[index].seq_num       = state_machine_p->curr_seq_num;
		state_machine_p->history.states[index].op            = utlSTATE_MACHINE_OP_SCHEDULE;
		state_machine_p->history.states[index].prev_state_id = curr_state_p->state_id;
		state_machine_p->history.states[index].state_id      =      state_p->state_id;
		state_machine_p->history.num_states++;
	}
#   endif /* defined(utlDEBUG) || defined(utlTEST) */

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlScheduleStateMachineState(state_machine_p, state_id, curr_time_p)
* INPUT
*      state_machine_p == pointer to an open utlStateMachine_T structure
*             state_id == the new state
*          curr_time_p == pointer to the current time
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, otherwise utlFAILED
* DESCRIPTION
*	Schedules a new state-machine state.
* TYPICAL USE
*         :
*      utlScheduleStateMachineState(state_machine_p, MY_LOGOUT_STATE, curr_time_p)
*      utlSetStateMachineState(state_machine_p, MY_LOGIN_STATE, curr_time_p)
*         :
*      <do stuff while logged in here>
*         :
*      utlPopStateMachineState(state_machine_p, curr_time_p)
*         :
*---------------------------------------------------------------------------*/
utlReturnCode_T utlScheduleStateMachineState(const utlStateMachine_P state_machine_p,
					     const utlStateMachineStateId_T state_id,
					     const utlAbsoluteTime_P2c curr_time_p)
{
	utlReturnCode_T rc;

	utlAssert(state_machine_p != NULL);

	/*--- acquire write-access ---*/
	if (utlAcquireExclusiveAccess(&(state_machine_p->state_machine_semaphore)) != utlSUCCESS)
		return utlFAILED;

	rc = _utlScheduleStateMachineState(state_machine_p, state_id, curr_time_p);

	/*--- release write-access ---*/
	if (utlReleaseExclusiveAccess(&(state_machine_p->state_machine_semaphore)) != utlSUCCESS)
		return utlFAILED;

	return rc;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	_utlRescheduleStateMachineState(state_machine_p, state_id, curr_time_p)
* INPUT
*      state_machine_p == pointer to an open utlStateMachine_T structure
*             state_id == the new state
*          curr_time_p == pointer to the current time
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, otherwise utlFAILED
* DESCRIPTION
*	Modifies the state machine stack such we'll enter the state machine
*      state `state_id' (when we pop from the current state) instead of the
*      state we would have entered had this function not been called.
*---------------------------------------------------------------------------*/
static utlReturnCode_T _utlRescheduleStateMachineState(const utlStateMachine_P state_machine_p,
						       const utlStateMachineStateId_T state_id,
						       const utlAbsoluteTime_P2c curr_time_p)
{
	utlAbsoluteTime_T curr_time;
	utlAbsoluteTime_P2c time_p;
	utlStateMachineState_P curr_state_p;
	utlStateMachineState_P state_p;

	utlAssert(state_machine_p != NULL);

	/*--- if current time was not supplied ---*/
	if (curr_time_p == NULL)
	{

		/*--- fetch current time ---*/
		if (utlCurrentTime(&curr_time) != utlSUCCESS)
			return utlFAILED;

		time_p = &curr_time;
	}
	else
		time_p = curr_time_p;

	/*--- check that there's something to reschedule ---*/
	if (state_machine_p->stack.curr_level == 0)
	{
		utlError("State machine `%s' stack underflow", state_machine_p->name);
		return utlFAILED;
	}

	/*--- check current state... ---*/
	utlAssert(     state_machine_p->stack.state_p[state_machine_p->stack.curr_level] != NULL);
	curr_state_p = state_machine_p->stack.state_p[state_machine_p->stack.curr_level];

	/*--- check state to be replaced... ---*/
	utlAssert(state_machine_p->stack.state_p[state_machine_p->stack.curr_level - 1] != NULL);
	state_p = state_machine_p->stack.state_p[state_machine_p->stack.curr_level - 1];

	/*--- search for state to be the replacement... ---*/
	if ((state_p = utlFindStateMachineStateInfo(state_machine_p, state_id)) == NULL)
		return utlFAILED;

	/*--- replace state under current state ---*/
	{
		state_machine_p->stack.state_p[state_machine_p->stack.curr_level - 1] = state_p;

		utlTrace(utlTRACE_STATE_MACHINE,
			 utlPrintTrace("re-schedule state `%s' (%d)\n",
				       state_p->name_p,
				       state_p->state_id);
			 );
	}

#   if defined(utlDEBUG) || defined(utlTEST)
	/*--- update state-transitions history ---*/
	{
		size_t index;

		index = state_machine_p->history.num_states % (utlSTATE_MACHINE_MAX_EVENT_HISTORY + 1);

		state_machine_p->history.states[index].time_stamp    = *time_p;
		state_machine_p->history.states[index].seq_num       = state_machine_p->curr_seq_num;
		state_machine_p->history.states[index].op            = utlSTATE_MACHINE_OP_RESCHEDULE;
		state_machine_p->history.states[index].prev_state_id = curr_state_p->state_id;
		state_machine_p->history.states[index].state_id      =      state_p->state_id;
		state_machine_p->history.num_states++;
	}
#   endif /* defined(utlDEBUG) || defined(utlTEST) */

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlRescheduleStateMachineState(state_machine_p, state_id, curr_time_p)
* INPUT
*      state_machine_p == pointer to an open utlStateMachine_T structure
*             state_id == the new state
*          curr_time_p == pointer to the current time
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, otherwise utlFAILED
* DESCRIPTION
*	Reschedules a state-machine state.
* TYPICAL USE
*         :
*      utlScheduleStateMachineState(state_machine_p, MY_LOGOUT_STATE, curr_time_p)
*      utlRescheduleStateMachineState(state_machine_p, MY_OUT_OF_SERVICE_STATE, curr_time_p)
*      utlSetStateMachineState(state_machine_p, MY_LOGIN_STATE, curr_time_p)
*         :
*      <do stuff while logged in here>
*         :
*      utlPopStateMachineState(state_machine_p, curr_time_p)
*         :
*---------------------------------------------------------------------------*/
utlReturnCode_T utlRescheduleStateMachineState(const utlStateMachine_P state_machine_p,
					       const utlStateMachineStateId_T state_id,
					       const utlAbsoluteTime_P2c curr_time_p)
{
	utlReturnCode_T rc;

	utlAssert(state_machine_p != NULL);

	/*--- acquire write-access ---*/
	if (utlAcquireExclusiveAccess(&(state_machine_p->state_machine_semaphore)) != utlSUCCESS)
		return utlFAILED;

	rc = _utlRescheduleStateMachineState(state_machine_p, state_id, curr_time_p);

	/*--- release write-access ---*/
	if (utlReleaseExclusiveAccess(&(state_machine_p->state_machine_semaphore)) != utlSUCCESS)
		return utlFAILED;

	return rc;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	_utlUnscheduleStateMachineState(state_machine_p, curr_time_p)
* INPUT
*      state_machine_p == pointer to an open utlStateMachine_T structure
*          curr_time_p == pointer to the current time
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, otherwise utlFAILED
* DESCRIPTION
*	Unschedules a previously scheduled state machine state such that we
*      no longer enter this state when we pop from the current state.
*---------------------------------------------------------------------------*/
static utlReturnCode_T _utlUnscheduleStateMachineState(const utlStateMachine_P state_machine_p,
						       const utlAbsoluteTime_P2c curr_time_p)
{
	utlAbsoluteTime_T curr_time;
	utlAbsoluteTime_P2c time_p;
	utlStateMachineState_P curr_state_p;
	utlStateMachineState_P state_p;

	utlAssert(state_machine_p != NULL);

	/*--- if current time was not supplied ---*/
	if (curr_time_p == NULL)
	{

		/*--- fetch current time ---*/
		if (utlCurrentTime(&curr_time) != utlSUCCESS)
			return utlFAILED;

		time_p = &curr_time;
	}
	else
		time_p = curr_time_p;

	/*--- check that there's something to unschedule ---*/
	if (state_machine_p->stack.curr_level == 0)
	{
		utlError("State machine `%s' stack underflow", state_machine_p->name);
		return utlFAILED;
	}

	/*--- check current state... ---*/
	utlAssert(     state_machine_p->stack.state_p[state_machine_p->stack.curr_level] != NULL);
	curr_state_p = state_machine_p->stack.state_p[state_machine_p->stack.curr_level];

	/*--- check state to be removed... ---*/
	utlAssert(state_machine_p->stack.state_p[state_machine_p->stack.curr_level - 1] != NULL);
	state_p = state_machine_p->stack.state_p[state_machine_p->stack.curr_level - 1];

	/*--- remove state under current state ---*/
	{
		state_machine_p->stack.curr_level--;
		state_machine_p->stack.state_p[state_machine_p->stack.curr_level] = curr_state_p;

		utlTrace(utlTRACE_STATE_MACHINE,
			 utlPrintTrace("unschedule state `%s' (%d)\n",
				       state_p->name_p,
				       state_p->state_id);
			 );
	}

#   if defined(utlDEBUG) || defined(utlTEST)
	/*--- update state-transitions history ---*/
	{
		size_t index;

		index = state_machine_p->history.num_states % (utlSTATE_MACHINE_MAX_EVENT_HISTORY + 1);

		state_machine_p->history.states[index].time_stamp    = *time_p;
		state_machine_p->history.states[index].seq_num       = state_machine_p->curr_seq_num;
		state_machine_p->history.states[index].op            = utlSTATE_MACHINE_OP_UNSCHEDULE;
		state_machine_p->history.states[index].prev_state_id = curr_state_p->state_id;
		state_machine_p->history.states[index].state_id      =      state_p->state_id;
		state_machine_p->history.num_states++;
	}
#   endif /* defined(utlDEBUG) || defined(utlTEST) */

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlUnscheduleStateMachineState(state_machine_p, curr_time_p)
* INPUT
*      state_machine_p == pointer to an open utlStateMachine_T structure
*          curr_time_p == pointer to the current time
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, otherwise utlFAILED
* DESCRIPTION
*	Unschedules a previously scheduled state machine state such that we
*      no longer enter this state when we pop from the current state.
*---------------------------------------------------------------------------*/
utlReturnCode_T utlUnscheduleStateMachineState(const utlStateMachine_P state_machine_p,
					       const utlAbsoluteTime_P2c curr_time_p)
{
	utlReturnCode_T rc;

	utlAssert(state_machine_p != NULL);

	/*--- acquire write-access ---*/
	if (utlAcquireExclusiveAccess(&(state_machine_p->state_machine_semaphore)) != utlSUCCESS)
		return utlFAILED;

	rc = _utlUnscheduleStateMachineState(state_machine_p, curr_time_p);

	/*--- release write-access ---*/
	if (utlReleaseExclusiveAccess(&(state_machine_p->state_machine_semaphore)) != utlSUCCESS)
		return utlFAILED;

	return rc;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	_utlPopStateMachineState(state_machine_p, curr_time_p)
* INPUT
*      state_machine_p == pointer to an open utlStateMachine_T structure
*          curr_time_p == pointer to the current time
*             va_arg_p == argument pointer
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, otherwise utlFAILED
* DESCRIPTION
*	Pops the current state machine state, which causes the associated
*      state call-back functions to be invoked (if any).
*---------------------------------------------------------------------------*/
static utlReturnCode_T _utlPopStateMachineState(const utlStateMachine_P state_machine_p,
						const utlAbsoluteTime_P2c curr_time_p)
{
	utlAbsoluteTime_T curr_time;
	utlAbsoluteTime_P2c time_p;
	utlStateMachineState_P enter_state_p;
	utlStateMachineState_P exit_state_p;
	unsigned long curr_seq_num;
	unsigned int orig_num_retries;
	utlReturnCode_T rc;

	utlAssert(state_machine_p != NULL);

	/*--- if current time was not supplied ---*/
	if (curr_time_p == NULL)
	{

		/*--- fetch current time ---*/
		if (utlCurrentTime(&curr_time) != utlSUCCESS)
			return utlFAILED;

		time_p = &curr_time;
	}
	else
		time_p = curr_time_p;

	/*--- check old state... ---*/
	utlAssert(     state_machine_p->stack.state_p[state_machine_p->stack.curr_level] != NULL);
	exit_state_p = state_machine_p->stack.state_p[state_machine_p->stack.curr_level];

	/*--- fetch new state... ---*/
	{
		if (state_machine_p->stack.curr_level <= (size_t)0)
		{
			utlError("State machine `%s' stack underflow", state_machine_p->name);
			return utlFAILED;
		}

		enter_state_p = state_machine_p->stack.state_p[state_machine_p->stack.curr_level - 1];
	}

	/*--- if we're entering a new state, reset retry count ---*/
	orig_num_retries = enter_state_p->context.num_retries;
	if (enter_state_p != exit_state_p)
		enter_state_p->context.num_retries = 0;

	/*--- note current sequence number ---*/
	curr_seq_num = state_machine_p->curr_seq_num;

	/*--- exit old state... ---*/
	{
		utlTrace(utlTRACE_STATE_MACHINE,
			 utlPrintTrace("exit (pop) state `%s' (%d)\n",
				       exit_state_p->name_p,
				       exit_state_p->state_id);
			 );

		/*--- invoke exit-state call-back (if any) ---*/
		if (       exit_state_p->call_back.exit_function_p != NULL)
		{
			if ((*(exit_state_p->call_back.exit_function_p))(state_machine_p,
									 exit_state_p->state_id,
									 time_p,
									 state_machine_p->call_back.arg_p) != utlSUCCESS)
			{
				utlError("Exit-state call-back function 0x%08x returned failure", exit_state_p->call_back.enter_function_p);

				/*--- clean ---*/
				state_machine_p->curr_seq_num      = curr_seq_num;
				enter_state_p->context.num_retries = orig_num_retries;

				return utlFAILED;
			}
		}

		/*--- stop timer (if running) ---*/
		if (exit_state_p->context.timer_id != 0)
			if (utlStopTimer(exit_state_p->context.timer_id) != utlSUCCESS)
			{

				/*--- clean ---*/
				state_machine_p->curr_seq_num      = curr_seq_num;
				enter_state_p->context.num_retries = orig_num_retries;

				return utlFAILED;
			}
	}

	/*--- restore current sequence num (in case state-entry handler modified it) ---*/
	state_machine_p->curr_seq_num = curr_seq_num;

	/*--- update statistics ---*/
	exit_state_p->statistics.exits++;
	enter_state_p->statistics.enters++;

#   if defined(utlDEBUG) || defined(utlTEST)
	/*--- update state-transitions history ---*/
	{
		size_t index;

		index = state_machine_p->history.num_states % (utlSTATE_MACHINE_MAX_EVENT_HISTORY + 1);

		state_machine_p->history.states[index].time_stamp    = *time_p;
		state_machine_p->history.states[index].seq_num       = curr_seq_num;
		state_machine_p->history.states[index].op            = utlSTATE_MACHINE_OP_POP;
		state_machine_p->history.states[index].prev_state_id =  exit_state_p->state_id;
		state_machine_p->history.states[index].state_id      = enter_state_p->state_id;
		state_machine_p->history.num_states++;
	}
#   endif /* defined(utlDEBUG) || defined(utlTEST) */

	rc = utlSUCCESS;

	/*--- pop to new state... ---*/
	{
		state_machine_p->stack.state_p[state_machine_p->stack.curr_level] = NULL;
		state_machine_p->stack.curr_level--;

		utlTrace(utlTRACE_STATE_MACHINE,
			 utlPrintTrace("enter (pop) state `%s' (%d)\n",
				       enter_state_p->name_p,
				       enter_state_p->state_id);
			 );

		/*--- invoke enter-state call-back (if any) ---*/
		if (       enter_state_p->call_back.enter_function_p != NULL)
		{
			if ((*(enter_state_p->call_back.enter_function_p))(state_machine_p,
									   enter_state_p->state_id,
									   time_p,
									   state_machine_p->call_back.arg_p) != utlSUCCESS)
			{

				/*--- fixup statistics ---*/
				enter_state_p->statistics.enters--;
				exit_state_p->statistics.enters++;

#               if defined(utlDEBUG) || defined(utlTEST)
				/*--- fixup state-transitions history ---*/
				{
					size_t index;

					index = (state_machine_p->history.num_states - 1) % (utlSTATE_MACHINE_MAX_EVENT_HISTORY + 1);

					state_machine_p->history.states[index].state_id = exit_state_p->state_id;
				}
#               endif           /* defined(utlDEBUG) || defined(utlTEST) */

				/*--- re-enter exit state ---*/
				enter_state_p = exit_state_p;
				state_machine_p->stack.curr_level++;
				state_machine_p->stack.state_p[state_machine_p->stack.curr_level] = enter_state_p;
				enter_state_p->context.num_retries = orig_num_retries;

				if (          enter_state_p->call_back.enter_function_p != NULL)
					(void)(*(enter_state_p->call_back.enter_function_p))(state_machine_p,
											     enter_state_p->state_id,
											     time_p,
											     state_machine_p->call_back.arg_p);

				utlError("Enter-state call-back function 0x%08x returned failure", enter_state_p->call_back.enter_function_p);
				rc = utlFAILED;
			}
		}

		/*--- start timer (if configured) ---*/
		if (enter_state_p->timeout_p != NULL)
			if ((enter_state_p->context.timer_id = utlStartTimer(enter_state_p->timeout_p,
									     1,
									     utlStateMachineStateTimeoutHandler,
									     state_machine_p)) == utlFAILED)
			{
				/*--- clean ---*/
				enter_state_p->context.num_retries = orig_num_retries;

				rc = utlFAILED;
			}
	}

	/*--- restore current sequence num ---*/
	state_machine_p->curr_seq_num = curr_seq_num;

	return utlSUCCESS;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlPopStateMachineState(state_machine_p, curr_time_p)
* INPUT
*      state_machine_p == pointer to an open utlStateMachine_T structure
*          curr_time_p == pointer to the current time
* OUTPUT
*	none
* RETURNS
*	utlSUCCESS for success, otherwise utlFAILED
* DESCRIPTION
*	Pops the current state machine state.
*---------------------------------------------------------------------------*/
utlReturnCode_T utlPopStateMachineState(const utlStateMachine_P state_machine_p,
					const utlAbsoluteTime_P2c curr_time_p)
{
	utlReturnCode_T rc;

	utlAssert(state_machine_p != NULL);

	/*--- acquire write-access ---*/
	if (utlAcquireExclusiveAccess(&(state_machine_p->state_machine_semaphore)) != utlSUCCESS)
		return utlFAILED;

	rc = _utlPopStateMachineState(state_machine_p, curr_time_p);

	/*--- release write-access ---*/
	if (utlReleaseExclusiveAccess(&(state_machine_p->state_machine_semaphore)) != utlSUCCESS)
		return utlFAILED;

	return rc;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlStateMachineStateElapsedTime(state_machine_p)
* INPUT
*      state_machine_p == pointer to an open utlStateMachine_T structure
* OUTPUT
*      none
* RETURNS
*	the elapsed time
* DESCRIPTION
*	Fetches how long we've been in the current state.
*---------------------------------------------------------------------------*/
utlRelativeTime_T utlStateMachineStateElapsedTime(const utlStateMachine_P state_machine_p)
{
	utlRelativeTime_T rv;
	utlStateMachineState_P state_p;

	utlAssert(state_machine_p != NULL);

	/*--- acquire read-access ---*/
	if (utlAcquireSharedAccess(&(state_machine_p->state_machine_semaphore)) != utlSUCCESS)
	{
		rv.seconds     = 0;
		rv.nanoseconds = 0;
		return rv;
	}

	/*--- check old state... ---*/
	utlAssert(state_machine_p->stack.state_p[state_machine_p->stack.curr_level] != NULL);
	state_p = state_machine_p->stack.state_p[state_machine_p->stack.curr_level];

	if (state_p->context.timer_id == 0)
	{

		/*--- clean ---*/
		(void)utlReleaseSharedAccess(&(state_machine_p->state_machine_semaphore));

		rv.seconds     = 0;
		rv.nanoseconds = 0;
		return rv;
	}

	if (utlQueryTimer(state_p->context.timer_id, utlTIMER_QUERY_ELAPSED_TIME, &rv) != utlSUCCESS)
	{

		/*--- clean ---*/
		(void)utlReleaseSharedAccess(&(state_machine_p->state_machine_semaphore));

		rv.seconds     = 0;
		rv.nanoseconds = 0;
		return rv;
	}

	/*--- release read-access ---*/
	if (utlReleaseSharedAccess(&(state_machine_p->state_machine_semaphore)) != utlSUCCESS)
	{
		rv.seconds     = 0;
		rv.nanoseconds = 0;
		return rv;
	}

	return rv;
}
#if defined(utlDEBUG) || defined(utlTEST)

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlVStringDumpStateMachine(v_string_p, state_machine_p, prefix_p)
* INPUT
*	     v_string_p == pointer to an initialized utlVString_T structure
*      state_machine_p == pointer to an initialized utlStateMachine_T structure
*             prefix_p == pointer to a prefix string
* OUTPUT
*	*v_string_p == the updated utlVString_T structure
* RETURNS
*	nothing
* DESCRIPTION
*	Dumps state machine information to `v_string_p'.
*---------------------------------------------------------------------------*/
void utlVStringDumpStateMachine(const utlVString_P v_string_p,
				const utlStateMachine_P state_machine_p,
				const char              *prefix_p)
{
	size_t level;
	utlStateMachineState_P state_p;

	utlAssert(v_string_p      != NULL);
	utlAssert(state_machine_p != NULL);
	utlAssert(prefix_p        != NULL);

	/*--- acquire read-access ---*/
	if (utlAcquireSharedAccess(&(state_machine_p->state_machine_semaphore)) != utlSUCCESS)
		return;

	(void)utlVStringPrintF(v_string_p, "%s:\n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s: State Machine\n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s:                 Name: %s\n", prefix_p, state_machine_p->name);

	(void)utlVStringPrintF(v_string_p, "%s:     Current State(s): %s  (top)\n", prefix_p, state_machine_p->stack.state_p[state_machine_p->stack.curr_level]->name_p);
	if (state_machine_p->stack.curr_level > 0)
		for (level = state_machine_p->stack.curr_level - 1;; )
		{
			(void)utlVStringPrintF(v_string_p, "%s:                     %s\n", prefix_p, state_machine_p->stack.state_p[level]->name_p);

			if (level == (size_t)0)
				break;
			level--;
		}

	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

	(void)utlVStringPrintF(v_string_p, "%s:\n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s: Defined State Machine States:\n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s:   State ID  |         State          | Time Out (seconds) | Max Retries | # Enters  | # Exits\n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s: ------------+------------------------+--------------------+-------------+-----------+-----------\n", prefix_p);

	/*--- for each state... ---*/
	for (state_p = state_machine_p->states_p; state_p->state_id != utlSTATE_ID_NULL; state_p++)
	{

		(void)utlVStringPrintF(v_string_p, "%s:  %-10d |", prefix_p, state_p->state_id);

		if (state_p == state_machine_p->stack.state_p[state_machine_p->stack.curr_level]) (void)utlVStringPrintF(v_string_p, "*%-22s |", state_p->name_p);
		else (void)utlVStringPrintF(v_string_p, " %-22s |", state_p->name_p);

		if (state_p->timeout_p == NULL)
			(void)utlVStringPrintF(v_string_p, "                    |");
		else
			(void)utlVStringPrintF(v_string_p, " %8ld.%09d |",
					       state_p->timeout_p->seconds,
					       state_p->timeout_p->nanoseconds);


		if (state_p->max_retries_p == NULL) (void)utlVStringPrintF(v_string_p, "           0 |");
		else (void)utlVStringPrintF(v_string_p, " %11d |", *(state_p->max_retries_p));

		(void)utlVStringPrintF(v_string_p, " %9d | %9d\n", state_p->statistics.enters,
				       state_p->statistics.exits);
	}

	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

	(void)utlVStringPrintF(v_string_p, "%s:\n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s: Recognized State Machine Events:\n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s:          State          |  Event ID  |              Event             | # Events\n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s: ------------------------+------------+--------------------------------+----------\n", prefix_p);

	/*--- for each state... ---*/
	for (state_p = state_machine_p->states_p; state_p->state_id != utlSTATE_ID_NULL; state_p++)
	{
		utlStateMachineEvent_P event_p;

		/*--- if any events are defined for this state ---*/
		if (state_p->events_p != NULL)
		{
			if ((state_p != state_machine_p->states_p) &&
			    (state_p->events_p[0].event_id != utlSTATE_EVENT_ID_NULL))
				(void)utlVStringPrintF(v_string_p, "%s:                         |            |                                |\n", prefix_p);

			/*--- for each event accepted in this state... ---*/
			for (event_p = state_p->events_p; event_p->event_id != utlSTATE_EVENT_ID_NULL; event_p++)
			{

				(void)utlVStringPrintF(v_string_p, "%s:  %-22s | %-10d | %-30s |",
						       prefix_p,
						       state_p->name_p,
						       event_p->event_id,
						       event_p->name_p);

				(void)utlVStringPrintF(v_string_p, " %8d\n", event_p->statistics.events);
			}
		}
	}

	(void)utlReleaseSharedAccess(&(state_machine_p->state_machine_semaphore));
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlDumpStateMachine(file_p, state_machine_p)
* INPUT
*	         file_p == pointer to an open file
*      state_machine_p == pointer to an initialized utlStateMachine_T structure
* OUTPUT
*	none
* RETURNS
*	nothing
* DESCRIPTION
*	Dumps state machine information to `file_p'.
*---------------------------------------------------------------------------*/
void utlDumpStateMachine(      FILE              *file_p,
			       const utlStateMachine_P state_machine_p)
{
	utlVString_T v_string;

	utlAssert(file_p != NULL);

	utlInitVString(&v_string);

	utlVStringDumpStateMachine(&v_string, state_machine_p, "StateMachine");

	utlVStringPuts(&v_string, file_p);

	utlVStringFree(&v_string);
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlVStringDumpStateMachineHistory(v_string_p, state_machine_p, prefix_p)
* INPUT
*	     v_string_p == pointer to an initialized utlVString_T structure
*      state_machine_p == pointer to an initialized utlStateMachine_T structure
*             prefix_p == pointer to a prefix string

* OUTPUT
*	*v_string_p == the updated utlVString_T structure
* RETURNS
*	nothing
* DESCRIPTION
*	Dumps state machine historical information to `file_p'.
*---------------------------------------------------------------------------*/
void utlVStringDumpStateMachineHistory(const utlVString_P v_string_p,
				       const utlStateMachine_P state_machine_p,
				       const char              *prefix_p)
{
	size_t head_index;
	size_t index;
	size_t tail_index;

	utlAssert(v_string_p      != NULL);
	utlAssert(state_machine_p != NULL);
	utlAssert(prefix_p        != NULL);

	/*--- acquire read-access ---*/
	if (utlAcquireSharedAccess(&(state_machine_p->state_machine_semaphore)) != utlSUCCESS)
		return;

	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

	(void)utlVStringPrintF(v_string_p, "%s:\n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s: State Machine Name: %s\n", prefix_p, state_machine_p->name);

	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

	(void)utlVStringPrintF(v_string_p, "%s:\n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s: State Transitions History:\n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s:             Time Stamp             |     Current State      |    Current Event     | Operation  |  New State\n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s: -----------------------------------+------------------------+----------------------+------------+----------------------\n", prefix_p);

	if (state_machine_p->history.num_states < (utlSTATE_MACHINE_MAX_STATE_HISTORY + 1))
	{
		head_index = 0;
		tail_index = state_machine_p->history.num_states;
	}
	else
	{
		head_index = (state_machine_p->history.num_states + 1) % (utlSTATE_MACHINE_MAX_STATE_HISTORY + 1);
		tail_index =  state_machine_p->history.num_states      % (utlSTATE_MACHINE_MAX_STATE_HISTORY + 1);
	}

	/*--- for each state change... ---*/
	for (index = head_index; index != tail_index; )
	{
		utlStateMachineState_P state_p;
		char date_string[40];

		/*--- generate formatted date/time string ---*/
		if (utlFormatDateTime(date_string, sizeof(date_string), &(state_machine_p->history.states[index].time_stamp)) != utlSUCCESS)
			if (sprintf(date_string, "%d.%09d",
				    state_machine_p->history.states[index].time_stamp.seconds,
				    state_machine_p->history.states[index].time_stamp.nanoseconds) < 0)
				date_string[0] = '\0';

		(void)utlVStringPrintF(v_string_p, "%s:  %-33s |", prefix_p, date_string);

		for (state_p = state_machine_p->states_p; state_p->state_id != utlSTATE_ID_NULL; state_p++)
			if (state_p->state_id == state_machine_p->history.states[index].prev_state_id)
				break;
		if (state_p->state_id != utlSTATE_ID_NULL) (void)utlVStringPrintF(v_string_p, " %-22s |",       state_p->name_p);
		else if (index != head_index) (void)utlVStringPrintF(v_string_p, " state=%-16d |", state_machine_p->history.states[index].prev_state_id);
		else (void)utlVStringPrintF(v_string_p, " <undefined>            |");

		if (state_machine_p->history.states[index].seq_num != 0)
		{
			size_t i;
			size_t head_i;
			size_t tail_i;

			if (state_machine_p->history.num_events < (utlSTATE_MACHINE_MAX_EVENT_HISTORY + 1))
			{
				head_i = 0;
				tail_i = state_machine_p->history.num_events;
			}
			else
			{
				head_i = (state_machine_p->history.num_events + 1) % (utlSTATE_MACHINE_MAX_EVENT_HISTORY + 1);
				tail_i =  state_machine_p->history.num_events      % (utlSTATE_MACHINE_MAX_EVENT_HISTORY + 1);
			}

			for (i = head_i; i != tail_i; )
			{
				if (state_machine_p->history.events[i].seq_num == state_machine_p->history.states[index].seq_num)
					break;

				if (++i > utlSTATE_MACHINE_MAX_EVENT_HISTORY)
					i = 0;
			}

			if (i == tail_i)
				(void)utlVStringPrintF(v_string_p, " <event purged>       |");
			else
			{
				utlStateMachineEvent_P event_p;

				for (event_p = state_p->events_p; event_p->event_id != utlSTATE_EVENT_ID_NULL; event_p++)
					if (event_p->event_id == state_machine_p->history.events[i].event_id)
						break;
				if (event_p->event_id != utlSTATE_EVENT_ID_NULL) (void)utlVStringPrintF(v_string_p, " %-20s |",       event_p->name_p);
				else (void)utlVStringPrintF(v_string_p, " event=%-14d |", state_machine_p->history.events[i].event_id);
			}

		}
		else
			(void)utlVStringPrintF(v_string_p, "                      |");

		switch (state_machine_p->history.states[index].op)
		{
		case utlSTATE_MACHINE_OP_UNKNOWN:    (void)utlVStringPrintF(v_string_p, " <unknown>  |");  break;
		case utlSTATE_MACHINE_OP_SET:        (void)utlVStringPrintF(v_string_p, " Set-state  |");  break;
		case utlSTATE_MACHINE_OP_PUSH:       (void)utlVStringPrintF(v_string_p, " Push-state |");  break;
		case utlSTATE_MACHINE_OP_SCHEDULE:   (void)utlVStringPrintF(v_string_p, " Schedule   |");  break;
		case utlSTATE_MACHINE_OP_RESCHEDULE: (void)utlVStringPrintF(v_string_p, " Reschedule |");  break;
		case utlSTATE_MACHINE_OP_UNSCHEDULE: (void)utlVStringPrintF(v_string_p, " Unschedule |");  break;
		case utlSTATE_MACHINE_OP_POP:        (void)utlVStringPrintF(v_string_p, " Pop-state  |");  break;
		default:                             (void)utlVStringPrintF(v_string_p, " %-10d |", state_machine_p->history.states[index].op);  break;
		}

		for (state_p = state_machine_p->states_p; state_p->state_id != utlSTATE_ID_NULL; state_p++)
			if (state_p->state_id == state_machine_p->history.states[index].state_id)
				break;
		if (state_p->state_id != utlSTATE_ID_NULL) (void)utlVStringPrintF(v_string_p, " %-20s\n",       state_p->name_p);
		else (void)utlVStringPrintF(v_string_p, " state=%-14d\n", state_machine_p->history.states[index].prev_state_id);

		if (++index > utlSTATE_MACHINE_MAX_STATE_HISTORY)
			index = 0;
	}

	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

	(void)utlVStringPrintF(v_string_p, "%s:\n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s: State Machine Event History:\n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s:             Time Stamp             |     Current State      |     Event\n", prefix_p);
	(void)utlVStringPrintF(v_string_p, "%s: -----------------------------------+------------------------+------------------------\n", prefix_p);

	if (state_machine_p->history.num_events < (utlSTATE_MACHINE_MAX_EVENT_HISTORY + 1))
	{
		head_index = 0;
		tail_index = state_machine_p->history.num_events;
	}
	else
	{
		head_index = (state_machine_p->history.num_events + 1) % (utlSTATE_MACHINE_MAX_EVENT_HISTORY + 1);
		tail_index =  state_machine_p->history.num_events      % (utlSTATE_MACHINE_MAX_EVENT_HISTORY + 1);
	}

	for (index = head_index; index != tail_index; )
	{
		utlStateMachineState_P state_p;
		utlStateMachineEvent_P event_p;
		char date_string[40];

		/*--- generate formatted date/time string ---*/
		if (utlFormatDateTime(date_string, sizeof(date_string), &(state_machine_p->history.events[index].time_stamp)) != utlSUCCESS)
			if (sprintf(date_string, "%d.%09d",
				    state_machine_p->history.events[index].time_stamp.seconds,
				    state_machine_p->history.events[index].time_stamp.nanoseconds) < 0)
				date_string[0] = '\0';

		(void)utlVStringPrintF(v_string_p, "%s:  %-33s |", prefix_p, date_string);

		for (state_p = state_machine_p->states_p; state_p->state_id != utlSTATE_ID_NULL; state_p++)
			if (state_p->state_id == state_machine_p->history.events[index].state_id)
				break;
		if (state_p->state_id != utlSTATE_ID_NULL) (void)utlVStringPrintF(v_string_p, " %-22s |",       state_p->name_p);
		else (void)utlVStringPrintF(v_string_p, " state=%-16d |", state_machine_p->history.events[index].state_id);

		for (event_p = state_p->events_p; event_p->event_id != utlSTATE_EVENT_ID_NULL; event_p++)
			if (event_p->event_id == state_machine_p->history.events[index].event_id)
				break;
		if (event_p->event_id == utlSTATE_EVENT_ID_NULL)
		{
			/*--- event is not recognized in this state, so search other states... ---*/
			for (state_p = state_machine_p->states_p; state_p->state_id != utlSTATE_ID_NULL; state_p++)
				for (event_p = state_p->events_p; event_p->event_id != utlSTATE_EVENT_ID_NULL; event_p++)
					if (event_p->event_id == state_machine_p->history.events[index].event_id)
						break;
		}
		if (event_p->event_id != utlSTATE_EVENT_ID_NULL) (void)utlVStringPrintF(v_string_p, " %-22s\n",       event_p->name_p);
		else (void)utlVStringPrintF(v_string_p, " event=%-16d\n", state_machine_p->history.events[index].event_id);

		if (++index > utlSTATE_MACHINE_MAX_EVENT_HISTORY)
			index = 0;
	}

	(void)utlReleaseSharedAccess(&(state_machine_p->state_machine_semaphore));
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlDumpStateMachineHistory(file_p, state_machine_p)
* INPUT
*	         file_p == pointer to an open file
*      state_machine_p == pointer to an initialized utlStateMachine_T structure
* OUTPUT
*	none
* RETURNS
*	nothing
* DESCRIPTION
*	Dumps state machine historical information to `file_p'.
*---------------------------------------------------------------------------*/
void utlDumpStateMachineHistory(      FILE              *file_p,
				      const utlStateMachine_P state_machine_p)
{
	utlVString_T v_string;

	utlAssert(file_p != NULL);

	utlInitVString(&v_string);

	utlVStringDumpStateMachineHistory(&v_string, state_machine_p, "StateMachine");

	utlVStringPuts(&v_string, file_p);

	utlVStringFree(&v_string);
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlIsStateMachineStateSequence(state_machine_p, state_ids_p)
* INPUT
*      state_machine_p == pointer to an initialized utlStateMachine_T structure
*          state_ids_p == pointer to a list of state IDs
* OUTPUT
*	none
* RETURNS
*	true if the state-transision sequence occured, otherwise false
* DESCRIPTION
*	Checks `state_machine_p' to see if the state-transision sequence
*      specified by `state_ids_p' occurred.
*---------------------------------------------------------------------------*/
bool utlIsStateMachineStateSequence(const utlStateMachine_P state_machine_p,
				    const utlStateMachineStateId_P state_ids_p)
{
	size_t head_index;
	size_t index;
	size_t tail_index;

	utlAssert(state_machine_p != NULL);
	utlAssert(state_ids_p     != NULL);

	if (state_machine_p->history.num_states < (utlSTATE_MACHINE_MAX_STATE_HISTORY + 1))
	{
		head_index = 0;
		tail_index = state_machine_p->history.num_states;
	}
	else
	{
		head_index = (state_machine_p->history.num_states + 1) % (utlSTATE_MACHINE_MAX_STATE_HISTORY + 1);
		tail_index =  state_machine_p->history.num_states      % (utlSTATE_MACHINE_MAX_STATE_HISTORY + 1);
	}

	/*--- for each stored state transition... ---*/
	for (index = head_index; index != tail_index; )
	{

		/*--- found the start of a possible match? ---*/
		if (state_machine_p->history.states[index].state_id == state_ids_p[0])
		{
			size_t cmp_index;
			utlStateMachineStateId_P state_id_p;

			state_id_p = state_ids_p;

			/*--- check for perfect match ---*/
			for (cmp_index = index; cmp_index != tail_index; )
			{
				if (state_machine_p->history.states[cmp_index].state_id != *state_id_p)
					break;

				state_id_p++;
				if (*state_id_p == utlSTATE_ID_NULL)
					return true;
				if (++cmp_index > utlSTATE_MACHINE_MAX_STATE_HISTORY)
					cmp_index = 0;
			}
		}

		if (++index > utlSTATE_MACHINE_MAX_STATE_HISTORY)
			index = 0;
	}

	return false;
}
#endif /* defined(utlDEBUG) || defined(utlTEST) */

#ifdef utlTEST
/*----------------------------------------------------------------------*/

typedef struct {
	struct {
		struct {
			size_t boot;
			size_t stopped;
			size_t active;
			size_t login;
			size_t poll;
		} enter;
		struct {
			size_t boot;
			size_t stopped;
			size_t active;
			size_t login;
			size_t poll;
		} exit;
	} state;
	struct {
		size_t start;
		size_t stop;
		size_t retry;
		size_t give_up;
		size_t reply;
		size_t reset;
	} event;
} testInfo_T;

/*--- define states ---*/
#define myLOGIN_STATE utlDEFINE_STATE_ID(1)
#define myPOLL_STATE  utlDEFINE_STATE_ID(2)
#define myTMP_STATE   utlDEFINE_STATE_ID(3)
#define myTMP2_STATE  utlDEFINE_STATE_ID(4)

/*---------------------------------------------------------------------------*
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
*	Handles test termination.
*---------------------------------------------------------------------------*/
static utlReturnCode_T testFunc(const utlTimerId_T id,
				const unsigned long timeout_count,
				void                *arg_p,
				const utlAbsoluteTime_P2c curr_time_p)
{
	utlExitEventLoop();

	return utlSUCCESS;
}
/*---------------------------------------------------------------------------*/
utlEVENT_HANDLER(myBootReset, arg_p, va_arg_p)
{
	testInfo_T *test_info_p;

	test_info_p = (testInfo_T *)arg_p;

	test_info_p->event.reset++;

	return utlSET_STATE(utlSTATE_ID_STOPPED);
}
/*---------------------------------------------------------------------------*/
utlEVENT_HANDLER(myStoppedStart, arg_p, va_arg_p)
{
	testInfo_T *test_info_p;
	int a, b, c;

	test_info_p = (testInfo_T *)arg_p;

	test_info_p->event.start++;

	a = va_arg(va_arg_p, int);
	b = va_arg(va_arg_p, int);
	c = va_arg(va_arg_p, int);

	if ((a != 99) || (b != 88) || (c != 77))
		return utlFAILED;

	return utlSET_STATE(utlSTATE_ID_ACTIVE);
}
/*---------------------------------------------------------------------------*/
utlEVENT_HANDLER(myActiveStop, arg_p, va_arg_p)
{
	testInfo_T *test_info_p;

	test_info_p = (testInfo_T *)arg_p;

	test_info_p->event.stop++;

	return utlSUCCESS;
}
/*---------------------------------------------------------------------------*/
utlEVENT_HANDLER(myActiveRetry, arg_p, va_arg_p)
{
	testInfo_T *test_info_p;

	test_info_p = (testInfo_T *)arg_p;

	test_info_p->event.retry++;

	return utlSET_STATE(utlSTATE_ID_ACTIVE);
}
/*---------------------------------------------------------------------------*/
utlEVENT_HANDLER(myActiveGiveUp, arg_p, va_arg_p)
{
	testInfo_T *test_info_p;

	test_info_p = (testInfo_T *)arg_p;

	test_info_p->event.give_up++;

	if (utlSCHEDULE_STATE(myTMP2_STATE) != utlSUCCESS)
		return utlFAILED;

	if (utlRESCHEDULE_STATE(myPOLL_STATE) != utlSUCCESS)
		return utlFAILED;

	/* do an extra push/pop for testing purposes */
	{
		if (utlPUSH_STATE(myTMP_STATE) != utlSUCCESS)
			return utlFAILED;

		if (utlPOP_STATE() != utlSUCCESS)
			return utlFAILED;
	}

	return utlSET_STATE(myLOGIN_STATE);
}
/*---------------------------------------------------------------------------*/
utlEVENT_HANDLER(myLoginReply, arg_p, va_arg_p)
{
	testInfo_T *test_info_p;

	test_info_p = (testInfo_T *)arg_p;

	test_info_p->event.reply++;

	return utlPOP_STATE();
}
/*---------------------------------------------------------------------------*/
utlEVENT_HANDLER(myPollStop, arg_p, va_arg_p)
{
	testInfo_T *test_info_p;

	test_info_p = (testInfo_T *)arg_p;

	test_info_p->event.stop++;

	return utlSUCCESS;
}
/*---------------------------------------------------------------------------*/
utlSTATE_CHANGE_HANDLER(myEnterBoot, arg_p)
{
	testInfo_T *test_info_p;

	test_info_p = (testInfo_T *)arg_p;

	test_info_p->state.enter.boot++;

	return utlEVENT(utlSTATE_EVENT_ID_RESET);
}
/*---------------------------------------------------------------------------*/
utlSTATE_CHANGE_HANDLER(myExitBoot, arg_p)
{
	testInfo_T *test_info_p;

	test_info_p = (testInfo_T *)arg_p;

	test_info_p->state.exit.boot++;

	return utlSUCCESS;
}
/*---------------------------------------------------------------------------*/
utlSTATE_CHANGE_HANDLER(myEnterStopped, arg_p)
{
	testInfo_T *test_info_p;

	test_info_p = (testInfo_T *)arg_p;

	test_info_p->state.enter.stopped++;

	return utlStateMachineEvent(state_machine_p, utlSTATE_EVENT_ID_START, curr_time_p, 99, 88, 77);
}
/*---------------------------------------------------------------------------*/
utlSTATE_CHANGE_HANDLER(myExitStopped, arg_p)
{
	testInfo_T *test_info_p;

	test_info_p = (testInfo_T *)arg_p;

	test_info_p->state.exit.stopped++;

	return utlSUCCESS;
}
/*---------------------------------------------------------------------------*/
utlSTATE_CHANGE_HANDLER(myEnterActive, arg_p)
{
	testInfo_T *test_info_p;

	test_info_p = (testInfo_T *)arg_p;

	test_info_p->state.enter.active++;

	return utlSUCCESS;
}
/*---------------------------------------------------------------------------*/
utlSTATE_CHANGE_HANDLER(myExitActive, arg_p)
{
	testInfo_T *test_info_p;

	test_info_p = (testInfo_T *)arg_p;

	test_info_p->state.exit.active++;

	return utlSUCCESS;
}
/*---------------------------------------------------------------------------*/
utlSTATE_CHANGE_HANDLER(myEnterLogin, arg_p)
{
	testInfo_T *test_info_p;

	test_info_p = (testInfo_T *)arg_p;

	test_info_p->state.enter.login++;

	return utlEVENT(utlSTATE_EVENT_ID_REPLY);
}
/*---------------------------------------------------------------------------*/
utlSTATE_CHANGE_HANDLER(myExitLogin, arg_p)
{
	testInfo_T *test_info_p;

	test_info_p = (testInfo_T *)arg_p;

	test_info_p->state.exit.login++;

	return utlSUCCESS;
}
/*---------------------------------------------------------------------------*/
utlSTATE_CHANGE_HANDLER(myEnterPoll, arg_p)
{
	testInfo_T *test_info_p;

	test_info_p = (testInfo_T *)arg_p;

	test_info_p->state.enter.poll++;

	return utlSUCCESS;
}
/*---------------------------------------------------------------------------*/
utlSTATE_CHANGE_HANDLER(myExitPoll, arg_p)
{
	testInfo_T *test_info_p;

	test_info_p = (testInfo_T *)arg_p;

	test_info_p->state.exit.poll++;

	return utlSUCCESS;
}
/*---------------------------------------------------------------------------*/

/*--- assign event handlers to each state ---*/
static utlStateMachineEvent_T boot_state[] = { utlDEFINE_EVENT_HANDLER_RESET(myBootReset),
					       utlDEFINE_EVENT_HANDLER_NULL() };

static utlStateMachineEvent_T stopped_state[] = { utlDEFINE_EVENT_HANDLER_START(myStoppedStart),
						  utlDEFINE_EVENT_HANDLER_NULL() };

static utlStateMachineEvent_T active_state[] = { utlDEFINE_EVENT_HANDLER_STOP(myActiveStop),
						 utlDEFINE_EVENT_HANDLER_RETRY(myActiveRetry),
						 utlDEFINE_EVENT_HANDLER_GIVE_UP(myActiveGiveUp),
						 utlDEFINE_EVENT_HANDLER_NULL() };

static utlStateMachineEvent_T login_state[] = { utlDEFINE_EVENT_HANDLER_REPLY(myLoginReply),
						utlDEFINE_EVENT_HANDLER_NULL() };

static utlStateMachineEvent_T poll_state[] = { utlDEFINE_EVENT_HANDLER_STOP(myPollStop),
					       utlDEFINE_EVENT_HANDLER_NULL() };

static utlStateMachineEvent_T tmp_state[] = { utlDEFINE_EVENT_HANDLER_NULL() };

static utlStateMachineEvent_T tmp2_state[] = { utlDEFINE_EVENT_HANDLER_NULL() };

static utlRelativeTime_T active_timeout;
static unsigned int active_retries;

/*--- assign states to state machine ---*/
static utlStateMachineState_T my_states[] = { utlDEFINE_STATE_BOOT(                    boot_state, NULL,	    NULL,	     myEnterBoot,		myExitBoot),
					      utlDEFINE_STATE_STOPPED(              stopped_state, NULL,	    NULL,	     myEnterStopped,		myExitStopped),
					      utlDEFINE_STATE_ACTIVE(                active_state, &active_timeout, &active_retries, myEnterActive,		myExitActive),
					      utlDEFINE_STATE(myLOGIN_STATE,			   "Login",	    login_state,     NULL,			NULL,		myEnterLogin,		 myExitLogin),
					      utlDEFINE_STATE( myPOLL_STATE,			   "Poll",	    poll_state,	     NULL,			NULL,		myEnterPoll,		 myExitPoll),
					      utlDEFINE_STATE(  myTMP_STATE,			   "Tmp",	    tmp_state,	     NULL,			NULL,		NULL,			 NULL),
					      utlDEFINE_STATE( myTMP2_STATE,			   "Tmp2",	    tmp2_state,	     NULL,			NULL,		NULL,			 NULL),
					      utlDEFINE_STATE_NULL() };


/*---------------------------------------------------------------------------*
* FUNCTION
*	stateMachineTest()
* INPUT
*	none
* OUTPUT
*	none
* RETURNS
*	"true" for pass, "false" for failure
*---------------------------------------------------------------------------*/
bool stateMachineTest(void)
{
	utlStateMachine_P state_machine_p;
	testInfo_T test_info;

	active_timeout.seconds     =  1;
	active_timeout.nanoseconds =  0;

	active_retries = 1;

	test_info.state.enter.boot    = 0;
	test_info.state.enter.stopped = 0;
	test_info.state.enter.active  = 0;
	test_info.state.enter.login   = 0;
	test_info.state.enter.poll    = 0;
	test_info.state.exit.boot     = 0;
	test_info.state.exit.stopped  = 0;
	test_info.state.exit.active   = 0;
	test_info.state.exit.login    = 0;
	test_info.state.exit.poll     = 0;
	test_info.event.start         = 0;
	test_info.event.stop          = 0;
	test_info.event.retry         = 0;
	test_info.event.give_up       = 0;
	test_info.event.reply         = 0;
	test_info.event.reset         = 0;

	/*--- test utlOpenStateMachine() -----------------------------------------*/
	if ((state_machine_p = utlOpenStateMachine("my state machine", 0, my_states, &test_info)) == NULL)
	{
		fprintf(stderr, "stateMachineTest: utlOpenStateMachine() failed\n");
		return false;
	}

	{
		utlRelativeTime_T period;
		utlTimerId_T exit_timer_id;
		testInfo_T exit_test_info;

		/* exit timer */
		period.seconds     = 3;
		period.nanoseconds = 0;

		if ((exit_timer_id = utlStartTimer(&period, 1, &testFunc, &exit_test_info)) == utlFAILED)
		{
			fprintf(stderr, "stateMachineTest: utlStartTimer(1.0)\n");
			return false;
		}

		if (utlEventLoop(true) != utlSUCCESS)
		{
			fprintf(stderr, "stateMachineTest: utlEventLoop(1.3)\n");
			return false;
		}
	}

	utlDumpStateMachine(stderr, state_machine_p);
	utlDumpStateMachineHistory(stderr, state_machine_p);

	if ((test_info.state.enter.boot    != 1) ||
	    (test_info.state.enter.stopped != 1) ||
	    (test_info.state.enter.active  != 3) ||
	    (test_info.state.enter.login   != 1) ||
	    (test_info.state.enter.poll    != 1) ||
	    (test_info.state.exit.boot     != 1) ||
	    (test_info.state.exit.stopped  != 1) ||
	    (test_info.state.exit.active   != 3) ||
	    (test_info.state.exit.login    != 1) ||
	    (test_info.state.exit.poll     != 0) ||
	    (test_info.event.start         != 1) ||
	    (test_info.event.stop          != 0) ||
	    (test_info.event.retry         != 1) ||
	    (test_info.event.give_up       != 1) ||
	    (test_info.event.reply         != 1) ||
	    (test_info.event.reset         != 1))
	{
		fprintf(stderr, "stateMachineTest: peg counts failed\n");
		return false;
	}

	/*--- test utlCloseStateMachine() ----------------------------------------*/

	if (utlCloseStateMachine(state_machine_p) != utlSUCCESS)
		fprintf(stderr, "stateMachineTest: utlCloseStateMachine() failed\n");

	return true;
}
#endif /* utlTEST */

