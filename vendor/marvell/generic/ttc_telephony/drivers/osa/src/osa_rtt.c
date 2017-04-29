/* ===========================================================================
   File        : osa_rtt.c
   Description : Linux Real Time Threads operating system specific code
	      for the OS Abstraction Layer API.

   Notes       :

   Copyright (c) 2001 Intel CCD. All Rights Reserved
   =========================================================================== */

#ifdef OSA_RTT

/************************************************************************
 * Project header files
 ***********************************************************************/
#include "osa.h"

/************************************************************************
 * External Interfaces
 ***********************************************************************/


/************************************************************************
 * External Variables
 ***********************************************************************/
extern OSA_ControlBlock* OSA_ControlBlockPtr;
static char *OSA_EmptyName = "";

INT32 OSA_SavedInterrupt = OS_ENABLED;

OS_Sema_t OSA_RttTickSemaphore;
UINT32 OSA_RttRtcCounter;


/*********************************************************************
 * OSA Private Methods
 ********************************************************************/
UINT8 OSA_RttTimerStack[OSA_RTT_TIMER_STACK];
void OSA_RttTimerTask(void*);

#ifdef OSA_SIMULATE_TICK
UINT8 OSA_RttTickStack[OSA_RTT_TICK_STACK];
void OSA_RttTickTask(void*);
#endif


/***********************************************************************
*
* Name:        OS_SpecificInit()
*
* Description: Initialize OS specific stuff.
*
* Parameters:  None
*
* Returns:     nothing
*
* Notes:
*
***********************************************************************/
void OSA_RttInit(void)
{
	OS_Task_t task;
	OSA_STATUS rt_val;

	/*
	** Initialize RTT specific components
	*/

	/* Create the semaphore for the timer task */
	OSA_SEMA_CREATE(&OSA_RttTickSemaphore, 1, OS_PRIORITY, rt_val);

	/* Create & start the timer task */
	OSA_TASK_CREATE(&task, (void *)OSA_RttTimerStack, OSA_RTT_TIMER_STACK, OSA_RTT_TIMER_PRIORITY,
			"OSATimer", OSA_RttTimerTask, (void *)OS_NULL, rt_val);

#ifdef OSA_SIMULATE_TICK
	/* Create & start the simulated tick task */
	OSA_TASK_CREATE(&task, (void *)OSA_RttTickStack, OSA_RTT_TICK_STACK, OSA_RTT_TICK_PRIORITY,
			"OSATick", OSA_RttTickTask, (void *)OS_NULL, rt_val);
#endif

	/* Clear the internal OS Real-Time Tick counter */
	OSA_RttRtcCounter = 0L;
}

#if (OSA_TASKS)
/***********************************************************************
*
* Name:        OSA_RttTaskCreate()
*
* Description: Get a OSTask data structure from the task pool
*              and initialize it.
*
* Parameters:
*  void*    stackPtr          - pointer to start of task stack area
*  UINT16   stackSize         - number of bytes in task stack area
*  UINT8    priority          - priority of task
*  void    (*taskStart)(void) - task entry point
*  void     *argv             - argument to be passed into task function
*
* Returns:
*  OS_Task_t *task            - reference to task or zero if create failed
*
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSA_RttTaskCreate( OS_Task_t *task, void* stackPtr,
			      UINT32 stackSize, UINT8 priority, CHAR *taskName,
			      void (*taskStart)(void*), void *argv )
{
	RTT_RTN_STATUS status;
	RttSchAttr attr;

	/* RTT does not return much so do some error checking */
	if (taskStart == NULL)
		return (OS_INVALID_PTR);
	if (stackPtr == 0L)
		return (OS_INVALID_MEMORY);
	if (stackSize < OSA_MIN_STACK_SIZE)
		return (OS_INVALID_SIZE);
	/* RTT does not allow 31 */
	if (priority >= 31)
		return(OS_INVALID_PRIORITY);


	/* Create the RTT task */
	attr.startingtime = RTTZEROTIME;
	attr.priority = priority;
	attr.deadline = RTTNODEADLINE;

	status = RttCreate(task, taskStart, (int)stackSize, taskName,
			   (void *)argv, attr, RTTUSR);

	/* Do error code translation to generic OSA errors */
	return( (status == RTTOK) ? OS_SUCCESS : OS_FAIL);
}

/***********************************************************************
*
* Name:        OSA_RttTaskDelete()
*
* Description: Delete an OSTask data structure from the task pool.
*
* Parameters:
*  OS_Task_t *task            - reference to task
*
* Returns:
*
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSA_RttTaskDelete( OS_Task_t *task )
{
	RTT_RTN_STATUS status;

	/* Delete the RTT task */
	status = RttKill(*task);

	/* Do error code translation to generic OSA errors */
	return( (status == RTTOK) ? OS_SUCCESS : OS_FAIL);
}

/***********************************************************************
*
* Name:        OSA_RttTaskSuspend()
*
* Description: Suspend a task.
*
* Parameters:
*  OS_Task_t *task            - reference to task
*
* Returns:
*
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSA_RttTaskSuspend( OS_Task_t *task )
{
	RTT_RTN_STATUS status;

	/* Suspend the RTT task */
	status = RttSuspend();

	/* Do error code translation to generic OSA errors */
	return( (status == RTTOK) ? OS_SUCCESS : OS_FAIL);
}

/***********************************************************************
*
* Name:        OSA_RttTaskResume()
*
* Description: Resume a task.
*
* Parameters:
*  OS_Task_t *task            - reference to task
*
* Returns:
*
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSA_RttTaskResume( OS_Task_t *task )
{
	RTT_RTN_STATUS status;

	/* Resume the RTT task */
	status = RttResume(*task);

	/* Do error code translation to generic OSA errors */
	return( (status == RTTOK) ? OS_SUCCESS : OS_FAIL);
}

/***********************************************************************
*
* Name:        OSA_RttTaskSleep()
*
* Description: Put the current task to sleep.
*
* Parameters:
*  UINT32    time             - time in SECONDS not ticks
*
* Returns:
*
*
* Notes:  RTT sleep time resolutions available are seconds or useconds
*         and not ticks. As the time parameter is only a UINT16, this
*         would only allow for 65ms max. Seconds was then chosen as
*         least divergent from the OS specification.
*
*
***********************************************************************/
void OSA_RttTaskSleep( UINT32 ticks )
{
	UINT32 usTime = 0;
	UINT32 sTime = 0;
	RttTimeValue sleepTime;

	/* Convert the provided ticks to time for RTT */
	usTime = OSA_TICK_FREQ_IN_MILLISEC * ticks * 1000;

	if (usTime > 4294967296)
	{
		sTime = (ticks * OSA_TICK_FREQ_IN_MILLISEC) / 1000;
		if ( (ticks * OSA_TICK_FREQ_IN_MILLISEC) % 1000)
		{
			usTime = ( usTime - (sTime * 1000000));
		}
	}

	sleepTime.seconds = sTime;
	sleepTime.microseconds = usTime;

	/* Put the current RTT task to sleep */
	RttSleepFor(sleepTime);
}

/***********************************************************************
*
* Name:        OSA_RttTaskChangePriority()
*
* Description: Change the priority of a task. (NOT SUPPORTED BY RTT!)
*
* Parameters:
*  OS_Task_t *task            - reference to task
*  UINT32    newPriority      - new task priority
*
* Returns:
*  UINT32    *oldPriority     - old task priority
*
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSA_RttTaskChangePriority( OS_Task_t task, UINT8 newPriority,
				      UINT8 *oldPriority )
{
	RTT_RTN_STATUS status;
	RttSchAttr sched;

	/* RTT does not allow 31 */
	if (newPriority >= 31)
		return(OS_INVALID_PRIORITY);

	/* Get the current scheduler attributes of the task */
	status = RttGetThreadSchedAttr(task, &sched);

	if (status == RTTOK)
	{
		/* Store the old priority */
		*oldPriority = (UINT8)sched.priority;

		/* Set the new priority */
		sched.priority = newPriority;
		status = RttSetThreadSchedAttr(task, sched);

	}
	else
		status = OS_FAIL;

	/* Do error code translation to generic OSA errors */
	return( (status == RTTOK) ? OS_SUCCESS : OS_FAIL);
}

/***********************************************************************
*
* Name:        OSA_RttTaskGetPriority()
*
* Description: Get the priority of a task. (NOT SUPPORTED BY RTT!)
*
* Parameters:
*  OS_Task_t *task            - reference to task
*
* Returns:
*  UINT8     *priority        - priority of the task
*
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSA_RttTaskGetPriority( OS_Task_t task, UINT8 *priority )
{
	RTT_RTN_STATUS status;
	RttSchAttr sched;

	/* Get the current scheduler attributes of the task */
	status = RttGetThreadSchedAttr(task, &sched);

	/* RTT priorities are in the range 0 - 30, 0 highest, 30 lowest.
	 * The OS Shell allows 0 - 255 so convert the priority to an OS value.
	 */
	*priority = (UINT8)sched.priority;

	/* Do error code translation to generic OSA errors */
	return( (status == RTTOK) ? OS_SUCCESS : OS_FAIL);
}
#endif

#if (OSA_MSG_QUEUES)
/***********************************************************************
*
* Name:        OSA_RttQueueCreate()
*
* Description: Create a message queue.
*
* Parameters:
*  UINT32    maxSize          - max message size
*  UINT32    maxNumber        - max number of messages in the queue
*  UINT32    waitingMode      - scheduling of waiting tasks
*                               OS_FIFO, OS_PRIORITY
*
* Returns:
*  OS_MsgQ_t *msgQRef         - reference to the message queue
*
* Notes:
*
*
***********************************************************************/
OSA_STATUS OSA_RttQueueCreate(OS_MsgQ_t *msgQRef, UINT32 maxSize, UINT32 maxNumber,
			      UINT32 waitingMode)
{
	RTT_RTN_STATUS rtn;

	/* RTT does not return much so do some error checking */
	if (maxSize == 0L)
		return (OS_INVALID_SIZE);
	if (maxNumber == 0L)
		return (OS_NO_RESOURCES);
	if ( (waitingMode != OS_FIFO) && (waitingMode != OS_PRIORITY) )
		return (OS_INVALID_MODE);

	/* Create an RTT queue that is non-blocking */
	rtn = RttNewQueue(msgQRef, maxSize * maxNumber, RTTQDEFAULT);
	return((rtn == RTTQOK) ? OS_SUCCESS : OS_FAIL);
}

/***********************************************************************
*
* Name:        OSA_RttQueueDelete()
*
* Description: Delete a message queue.
*
* Parameters:
*  OS_MsgQ_t msgQRef         - reference to the message queue
*
*
* Returns:
*
* Notes:
*
*
***********************************************************************/
OSA_STATUS OSA_RttQueueDelete(OS_MsgQ_t *msgQRef)
{
	RTT_RTN_STATUS rtn;

	rtn = RttReapQueue(*msgQRef, OS_NULL);
	return((rtn == RTTQOK) ? OS_SUCCESS : OS_FAIL);
}

/***********************************************************************
*
* Name:        OSA_RttQueueSend()
*
* Description: Send a message to a queue.
*
* Parameters:
*  OS_MsgQ_t msgQRef         - reference to the message queue
*  UINT32    size            - message size
*  UINT8     *msg            - message to be sent
*  UINT32    timeout         - not used in RTT
*
*
* Returns:
*
* Notes:
*
*
***********************************************************************/
OSA_STATUS OSA_RttQueueSend(OS_MsgQ_t *msgQRef, UINT32 size, UINT8 *msg,
			    UINT32 timeout)
{
	OSA_STATUS status;
	RTT_RTN_STATUS rtn;
	int count;
	void            *temp;

	/* RTT does not return much so do some error checking */
	if (msg == NULL)
		return (OS_INVALID_POINTER);
	/* RTT does not allow timeouts */
	OSA_FULL_ASSERT( (timeout == OSA_SUSPEND) || (timeout == OSA_NO_SUSPEND));

	/* Get some memory for the data as RTT just passes the pointer
	 * to the data on the queue.
	 */
	OSA_MEM_ALLOC(OSA_ControlBlockPtr->heapPool, OSA_VARIABLE, size, &temp, OSA_SUSPEND, status);

	if (temp)
	{
		memcpy(temp, (void *)msg, size);

		rtn = RttEnqueue(*msgQRef, temp, &count);

		switch (rtn)
		{
		case RTTQFULL:
			return(OS_QUEUE_FULL);
		case RTTQOK:
			return(OS_SUCCESS);
		case RTTQFAILED:
			return(OS_FAIL);
		default:
			return(OS_FAIL);
		}
	}
	else
		return (OS_FAIL);
}

/***********************************************************************
*
* Name:        OSA_RttQueueRecv()
*
* Description: Receive a message from a queue.
*
* Parameters:
*  OS_MsgQ_t msgQRef         - reference to the message queue
*  UINT32    size            - max message size to copy to user's buffer
*  UINT8     *msg            - pointer to buffer to store message
*
* Returns:
*
* Notes:
*
*
***********************************************************************/
OSA_STATUS OSA_RttQueueRecv(OS_MsgQ_t *msgQRef, UINT8 *msg, UINT32 size,
			    UINT32 timeout)
{
	OSA_STATUS status;
	RTT_RTN_STATUS rtn;
	UINT8           *temp = NULL;

	/* RTT does not return much so do some error checking */
	if (msg == NULL)
		return (OS_INVALID_POINTER);
	/* RTT does not allow timeouts */
	OSA_FULL_ASSERT( (timeout == OSA_SUSPEND) || (timeout == OSA_NO_SUSPEND));

	/* Check for messages on the queue */
	rtn = RttDequeue(*msgQRef, (void **)&temp);

	/* If we got a message, copy it and free the space */
	if (temp)
	{
		memcpy((void*)msg, (void*)temp, size);
		OSA_MEM_FREE(OSA_ControlBlockPtr->heapPool, OSA_VARIABLE, temp, status);

	}

	switch (rtn)
	{
	case RTTQEMPTY:
		return(OS_QUEUE_EMPTY);
	case RTTQFULL:
		return(OS_QUEUE_FULL);
	case RTTQOK:
		return(OS_SUCCESS);
	case RTTQFAILED:
		return(OS_FAIL);
	default:
		return(OS_FAIL);
	}
}
#endif

#if (OSA_FLAGS)
/***********************************************************************
*
* Name:        OSA_RttFlagCreate()
*
* Description: Create event flag.
*
* Parameters:
*
*  OSFlagRef flagRef - event flag reference
*
* Returns:
*
*   OS_SUCCESS
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSA_RttFlagCreate( OS_Flag_t *flag )
{
	OSA_STATUS status;

	OSA_SEMA_CREATE(&flag->semaphore, 0, OS_FIFO, status);
	OSA_ASSERT(status == OS_SUCCESS);

	if (status == OS_SUCCESS)
		flag->flag = 0L;

	return (status);
}

/***********************************************************************
*
* Name:        OSA_RttFlagDelete()
*
* Description: Delete event flag.
*
* Parameters:
*
*  OSFlagRef flagRef - event flag reference
*
* Returns:
*
*   OS_SUCCESS
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSA_RttFlagDelete( OS_Flag_t *flag )
{
	OSA_STATUS status;

	OSA_SEMA_DELETE(flag->semaphore, status);
	if (status == OS_SUCCESS)
		flag->flag = 0L;

	return (status);
}

/***********************************************************************
*
* Name:        OSA_RttFlagSetBits()
*
* Description: Set event flag in target event register
*
* Parameters:
*
*  OS_Flag_t flagRef - event flag reference who's event flags will be set
*  UINT32    value   - flags to set
*
* Returns:
*
*   OS_INVALID_REF      - if flag reference is zero
*   OS_SUCCESS          - if the event flag is set properly
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSA_RttFlagSetBits( OS_Flag_t *flag, UINT32 mask, UINT32 operation )
{
	OSA_STATUS status;

	if (operation == OSA_FLAG_AND)
	{
		flag->flag &= mask;
	}
	else if (operation == OSA_FLAG_OR)
	{
		flag->flag |= mask;
	}
	else
	{
		return(OS_INVALID_MODE);
	}

	OSA_SEMA_RELEASE(flag->semaphore, status);

	return(OS_SUCCESS);
}

/***********************************************************************
*
* Name:        OSA_RttFlagWait()
*
* Description: Block until one or more of the requested event flags
*              is set.
*
* Parameters:
*
*   OSFlagRef flagRef   - event flag to be monitored
*   UINT32    mask      - bit mask of bits to be monitored in event register
*   UINT32*   flags     - all event flags that were set in the register
*
* Returns:
*
*
* Notes:
*
*    This function returns all set events, but only clears those that were
*    requested so that events will not be lost if they are not acted upon
*    immediately.
*
***********************************************************************/
OSA_STATUS OSA_RttFlagWait( OS_Flag_t *flag, UINT32 mask, UINT32 operation,
			    UINT32* flags, UINT32 timeout )
{
	OSA_STATUS status = OS_FAIL;
	OSA_STATUS rtn;
	UINT8 noMatch = TRUE;
	UINT32 compare;

	/* Convert the operation */
	if ( (operation != OSA_FLAG_AND) &&
	     (operation != OSA_FLAG_AND_CLEAR) &&
	     (operation != OSA_FLAG_OR) &&
	     (operation != OSA_FLAG_OR_CLEAR) )
	{
		return(OS_INVALID_MODE);
	}
	if (flags == 0)
		return(OS_INVALID_POINTER);
	/* RTT does not allow timeouts */
	OSA_FULL_ASSERT( (timeout == OSA_SUSPEND) || (timeout == OSA_NO_SUSPEND));

	/* Continue to wait for a match in the requested event mask */
	while (noMatch)
	{
		/* Wait forever for events to be set */
		if (timeout == OSA_SUSPEND)
		{
			OSA_SEMA_ACQUIRE(flag->semaphore, OSA_SUSPEND, rtn);
		}
		else if (timeout == OSA_NO_SUSPEND)
		{
			OSA_SEMA_ACQUIRE(flag->semaphore, OSA_NO_SUSPEND, rtn);
		}

		if (rtn == OS_SUCCESS)
		{
			compare = flag->flag & mask;

			/* All events must be present. Check the compare value. */
			if ((operation == OSA_FLAG_AND) || (operation == OSA_FLAG_AND_CLEAR) )
			{
				compare = (compare == mask);
			}

			/* Necessary event flags are present */
			if (compare)
			{
				/* Store which event flags were set */
				*flags = flag->flag;

				if ( (operation == OSA_FLAG_AND_CLEAR) || (operation == OSA_FLAG_OR_CLEAR) )
				{
					/* Clear waited for event flags */
					flag->flag &= ~mask;
					noMatch = FALSE;
					status = OS_SUCCESS;
				}
				else
				{
					noMatch = FALSE;
					status = OS_SUCCESS;
				}
			}
			else /* No Match in the event flags */
			{
				if (timeout == OSA_SUSPEND)
				{
					noMatch = TRUE;
					continue;
				}
				else
				{
					/* Store which event flags were set */
					*flags = flag->flag;
					OSA_SEMA_RELEASE(flag->semaphore, rtn);
					OSA_ASSERT(rtn == OS_SUCCESS);
					noMatch = FALSE;
					status = OS_FLAG_NOT_PRESENT;
				}
			}
		} /* End of getting flag semaphore */
		else
		{
			/* Semaphore was not available so we won't wait for it */
			if (rtn == OS_UNAVAILABLE)
			{
				/* Store which event flags were set */
				*flags = flag->flag;
				status = OS_FLAG_NOT_PRESENT;
				break;
			}
			else /* Semaphore acquire failed */
			{
				status = OS_FAIL;
				break;
			}
		}

	} /* End of while waiting for events to match */

	return(status);
}
#endif

#if (OSA_TIMERS)
/***********************************************************************
*
* Name:        OSA_RttTimerCreate()
*
* Description: Initialize a OS_Timer data structure from the timer pool.
*
* Parameters:
*  OS_Timer_t* timer      - pointer to timer structure to initialize
*
* Returns:
*
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSA_RttTimerCreate( OS_Timer_t *timer )
{
	/* RTT does not return much so do some error checking */
	if (timer == NULL)
		return (OS_INVALID_REF);

	/* Initialize the RTT specific data */
	timer->ticksLeft    = 0;
	timer->reschedTicks = 0;
	timer->expirations  = 0;
	timer->handler      = OS_NULL;
	timer->timerArgc    = 0;

	return(OS_SUCCESS);
}

/***********************************************************************
*
* Name:        OSA_RttTimerDelete()
*
* Description: Re-initialize timer variables.
*
* Parameters:
*  OS_Timer_t* timer      - pointer to timer structure to initialize
*
* Returns:
*
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSA_RttTimerDelete( OS_Timer_t *timer )
{
	/* RTT does not return much so do some error checking */
	if (timer == NULL)
		return (OS_INVALID_REF);

	/* Initialize the RTT specific data */
	timer->ticksLeft    = 0;
	timer->reschedTicks = 0;
	timer->handler      = OS_NULL;
	timer->timerArgc    = 0;

	return(OS_SUCCESS);
}

/***********************************************************************
*
* Name:        OSA_RttTimerStart()
*
* Description: Start the timer.
*
* Parameters:
*  OS_Timer_t* timer          - pointer to timer structure to initialize
*
* Returns:
*
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSA_RttTimerStart( OS_Timer_t *timer, UINT32 time, UINT32 resched,
			      void (*handler)(UINT32), UINT32 argc )
{
	/* RTT does not return much so do some error checking */
	if (timer == NULL)
		return (OS_INVALID_REF);

	/* Update the RTT timer data */
	timer->state        = OS_TIMER_ACTIVE;
	timer->ticksLeft    = time;
	timer->reschedTicks = resched;
	timer->handler      = handler;
	timer->timerArgc    = argc;
	timer->expirations  = 0L;

	return(OS_SUCCESS);
}

/***********************************************************************
*
* Name:        OSA_RttTimerStop()
*
* Description: Stop the timer.
*
* Parameters:
*  OS_Timer_t* timer          - pointer to timer structure to initialize
*
* Returns:
*
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSA_RttTimerStop( OS_Timer_t *timer )
{
	/* RTT does not return much so do some error checking */
	if (timer == NULL)
		return (OS_INVALID_REF);

	/* Set the timer to inactive */
	timer->state = OS_TIMER_INACTIVE;

	/* Do not need to do anything */
	return(OS_SUCCESS);
}

/***********************************************************************
*
* Name:        OSA_RttTimerTask
*
* Description: This task manually manages the timers in the system by
*              checking each one for a change in status.
*              If a timer has expired the installed callback routine
*              is called. After this operation is over the task
*              suspends itself. This task will be periodically
*              resumed by using a system timer.
*
*
* Parameters:
*
* Returns:
*
* Notes:
*
***********************************************************************/
void OSA_RttTimerTask(void *argv)
{
	OS_TimerPool *timerPool = &OSA_ControlBlockPtr->timerPool;
	OSA_STATUS rt_val;
	OS_Timer    **activeListHead = OS_NULL;
	OS_Timer    *currentNode = OS_NULL;

	activeListHead = &timerPool->head;

	while (1)
	{
		/* Wait on the sem to get the next tick */
		OSA_SEMA_ACQUIRE(OSA_RttTickSemaphore, OSA_SUSPEND, rt_val);

		currentNode = *activeListHead;

		/* Check for any expired timers on the active list */
		while (currentNode != OS_NULL )
		{
			/* If the timer is active update it */
			if (currentNode->os_ref.state == OS_TIMER_ACTIVE)
			{
				currentNode->os_ref.ticksLeft--;

				/* Timer has expired */
				if (currentNode->os_ref.ticksLeft == 0)
				{
					/* Handle if timer is to be rescheduled */
					if (currentNode->os_ref.reschedTicks)
					{
						currentNode->os_ref.ticksLeft = currentNode->os_ref.reschedTicks;
						currentNode->os_ref.expirations++;

						/* Invoke the handler */
						currentNode->os_ref.handler(currentNode->os_ref.timerArgc);
					}
					else /* Set the timer to inactive and delete it */
					{
						currentNode->os_ref.state = OS_TIMER_INACTIVE;
						currentNode->state = OS_TIMER_INACTIVE;
						currentNode->os_ref.expirations++;

						/* Invoke the handler */
						currentNode->os_ref.handler(currentNode->os_ref.timerArgc);
					}

				}       /* End timer expired */

			}               /* Check if timer is active */

			/* Go on to next timer */
			currentNode = currentNode->next;

		}       /* active list scanned */

	}               /* Eternal timer loop  */

	return;
}
#endif

#ifdef OSA_SIMULATE_TICK
/***********************************************************************
*
* Name:        OSA_RttTickTask
*
* Description: This task provides a simulated 1 second tick for the
*              RTT timer implementation. It sleeps for one second & then
*              releases the timer tick semaphore to indicate to the timer
*              task that the base tick was received.
*
* Parameters:
*
* Returns:
*
* Notes:
*
***********************************************************************/
void OSA_RttTickTask(void *argv)
{
	UINT32 rt_val;

	while (1)
	{
		/* Sleep for 10000 us or 10 ms */
		RttUSleep(OSA_TICK_FREQ_IN_MILLISEC * 1000);
		OSA_CLOCK_TICK;
	}

	return;
}
#endif

/***********************************************************************
*
* Name:        OSA_RttClockTick
*
* Description: This function provides the tick stimulus for the timer
*              task. The timer task uses this stimulus to scan for
*              expired timers on the list. It also updates the internal
*              clock ticks.
*
* Parameters:
*
* Returns:
*
* Notes:
*
***********************************************************************/
void OSA_RttClockTick(void)
{
	UINT32 rt_val;

	OSA_RttRtcCounter++;
	OSA_SEMA_RELEASE(OSA_RttTickSemaphore, rt_val);
}

#if (OSA_SEMAPHORES)
/***********************************************************************
*
* Name:        OSA_RttSemaCreate()
*
* Description: Get a OS_Sema data structure from the semaphore pool
*              and initialize it.
*
* Parameters:
*  UINT32    initialCount     - initial count of the semaphore
*  UINT32    waitingMode      - scheduling of waiting tasks
*                               OS_FIFO, OS_PRIORITY
*
* Returns:
*  OS_Sema_t *sema            - OS assigned semaphore reference
*
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSA_RttSemaCreate( OS_Sema_t *sema, UINT32 initialCount, UINT32 waitingMode )
{
	RTT_RTN_STATUS rtn;

	if ((waitingMode != OS_FIFO) && (waitingMode != OS_PRIORITY))
		return (OS_INVALID_MODE);


	/* Create the RTT Counting Semaphore */
	rtn = RttAllocSem(sema, (int)initialCount, waitingMode);

	/* Do error code translation to generic OSA errors */
	return( (rtn == RTTOK) ? OS_SUCCESS : OS_FAIL);
}

/***********************************************************************
*
* Name:        OSA_RttSemaDelete()
*
* Description: Delete a semaphore.
*
* Parameters:
*  OS_Sema_t sema            - OS semaphore reference
*
* Returns:
*
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSA_RttSemaDelete( OS_Sema_t *sema )
{
	RTT_RTN_STATUS rtn;

	/* RTT does not return much so do some error checking */
	if (sema == NULL)
		return (OS_INVALID_REF);

	/* Delete the Semaphore */
	rtn = RttFreeSem(*sema);

	/* Do error code translation to generic OSA errors */
	return( (rtn == RTTOK) ? OS_SUCCESS : OS_FAIL);
}

/***********************************************************************
*
* Name:        OSA_RttSemaAcquire()
*
* Description: Acquire a semaphore.
*
* Parameters:
*  OS_Sema_t sema            - OS semaphore reference
*
* Returns:
*
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSA_RttSemaAcquire( OS_Sema_t *sema, UINT32 timeout )
{
	int value;
	RTT_RTN_STATUS rtn;

	/* RTT does not return much so do some error checking */
	if (sema == NULL)
		return (OS_INVALID_REF);
	/* RTT does not allow timeouts */
	OSA_FULL_ASSERT( (timeout == OSA_SUSPEND) || (timeout == OSA_NO_SUSPEND));

	/* If we are blocking don't bother checking the semaphore value */
	if (timeout == OSA_SUSPEND)
	{
		/* Acquire the Semaphore */
		rtn = RttP(*sema);
		OSA_ASSERT(rtn == RTTOK);

		/* Do error code translation to generic OSA errors */
		return( (rtn == RTTOK) ? OS_SUCCESS : OS_FAIL);
	}
	else
	{
		/* Check the Semaphore value */
		rtn = RttSemValue(*sema, &value);

		/* Semaphore is available */
		if (value > 0)
		{
			/* Acquire the Semaphore */
			rtn = RttP(*sema);
			OSA_ASSERT(rtn == RTTOK);

			/* Do error code translation to generic OSA errors */
			return( (rtn == RTTOK) ? OS_SUCCESS : OS_FAIL);
		}
		else /* Semaphore not available */
		{
			return(OS_UNAVAILABLE);
		}
	}
}

/***********************************************************************
*
* Name:        OSA_RttSemaRelease()
*
* Description: Release a semaphore.
*
* Parameters:
*  OS_Sema_t sema            - OS semaphore reference
*
* Returns:
*
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSA_RttSemaRelease( OS_Sema_t *sema )
{
	RTT_RTN_STATUS status;

	/* RTT does not return much so do some error checking */
	if (sema == NULL)
		return (OS_INVALID_REF);

	/* Release the Semaphore */
	status = RttV(*sema);
	OSA_ASSERT(status == RTTOK);

	/* Do error code translation to generic OSA errors */
	return( (status == RTTOK) ? OS_SUCCESS : OS_FAIL);
}
#endif

#if (OSA_MEM_POOLS)
/***********************************************************************
*
* Name:        OSA_RttMemAlloc
*
* Description: Allocate block of memory from OS memory pool
*
* Parameters:
*  OSPoolRef   poolRef     - memory pool reference
*  UINT32      size        - size of memory block in bytes
*
* Returns:     void** mem  - non-zero pointer to allocated memory block
*                            or zero if allocation failed.
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSA_RttMemAlloc(UINT32 size, void **mem, UINT32 timeout)
{
	/* RTT does not allow timeouts */
	OSA_FULL_ASSERT( (timeout == OSA_SUSPEND) || (timeout == OSA_NO_SUSPEND));

	/* Allocate memory for the OS task pool. */
	*mem = RttMalloc(size);

	if (*mem != NULL)
	{
		return (OS_SUCCESS);
	}
	else
	{
		return (OS_NO_MEMORY);
	}
}
#endif /* OSA_MEM_POOLS */


#endif /* OSA_RTT */

