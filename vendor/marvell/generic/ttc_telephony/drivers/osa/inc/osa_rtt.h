/* ===========================================================================
   File        : osa_rtt.h
   Description : Definition of OSA Software Layer data types specific to the
	      Linux RTT OS.

   Notes       :

   Copyright (c) 2001 Intel CCD. All Rights Reserved
   =========================================================================== */
#ifndef _OSA_RTT_H
#define _OSA_RTT_H

#include "gbl_types.h"
#include "rtthreads.h"
#include "RttQueue.h"
#include "RttMutex.h"
#include "RttCommon.h"

/************************************************************************
 * External Interfaces
 ***********************************************************************/
#ifdef OSA_PC_TEST
    #include <stdio.h>

/* These definitions are needed as we do not yet have the INTC
 * package yet from Israel.
 */
typedef UINT32 INTC_InterruptSources;
    #define INTC_RC_OK              0
    #define INTC_MAX_INTERRUPT_SOURCES   110

#else
    #ifdef OSA_NUCLEUS
	#include "intc.h"
    #endif
#endif

extern UINT32 OSA_RttRtcCounter;

/*************************************************************************
* Constants
*************************************************************************/
#define OSA_RTT_TIMER_STACK      20000
#define OSA_RTT_TIMER_PRIORITY   10
#define OSA_RTT_TICK_STACK       20000
#define OSA_RTT_TICK_PRIORITY    5
#define OSA_RTT_HIGHEST_PRIORITY 0
#define OSA_RTT_LOWEST_PRIORITY  30
#define OSA_MIN_STACK_SIZE       240
#define OSA_SUSPEND              0xFFFFFFFFUL
#define OSA_NO_SUSPEND           0
#define OSA_FLAG_AND             10
#define OSA_FLAG_AND_CLEAR       11
#define OSA_FLAG_OR              12
#define OSA_FLAG_OR_CLEAR        13
#define OSA_FIXED                4
#define OSA_VARIABLE             5
#define OSA_FIFO                 RTTFCFS
#define OSA_PRIORITY             RTTPRIORITY
#define OSA_POOL_MEM_OVERHEAD         8         /* Fixed pool overhead per partition */
#define OSA_FIXED_POOL_MEM_OVERHEAD   8         /* Fixed pool overhead per partition */
#define OSA_VAR_POOL_MEM_OVERHEAD     12        /* Variable pool overhead per partition */

/* Remain for backwards compatibility */
#define OS_SUSPEND               OSA_SUSPEND
#define OS_NO_SUSPEND            OSA_NO_SUSPEND
#define OS_FLAG_AND              OSA_FLAG_AND
#define OS_FLAG_AND_CLEAR        OSA_FLAG_AND_CLEAR
#define OS_FLAG_OR               OSA_FLAG_OR
#define OS_FLAG_OR_CLEAR         OSA_FLAG_OR_CLEAR
#define OS_FIXED                 OSA_FIXED
#define OS_VARIABLE              OSA_VARIABLE
#define OS_FIFO                  OSA_FIFO
#define OS_PRIORITY              OSA_PRIORITY

/*************************************************************************
* RTT Specific Types
*************************************************************************/
typedef void*               OSAFlagRef;
typedef RttThreadId OS_Task_t;
typedef RttQueue OS_MsgQ_t;
typedef RttQueue OS_Mbox_t;
typedef RttSem OS_Sema_t;
typedef RttSem OS_Mutex_t;

typedef UINT32 OS_EventGroup_t;
typedef UINT32 OS_Hisr_t;
typedef UINT32 OS_MemPool_t;
typedef UINT32 OS_PartitionPool_t;
typedef int RTT_RTN_STATUS;
typedef UINT8 OSA_STATUS;


/* Remain for backwards compatibility */
typedef UINT8 OS_STATUS;
typedef void*              OSFlagRef;

/* Flag reference */
typedef struct
{
	OS_EventGroup_t flag;
	OS_Sema_t semaphore;
} OS_Flag_t;

/* Timer status information structure */
typedef struct
{
	UINT32 status;          /* timer status OS_ENABLED, OS_DISABLED    */
	UINT32 expirations;     /* number of expirations for cyclic timers */

} OSATimerStatus;

/* RTT specific timer information */
typedef struct
{
	UINT32 state;
	UINT32 ticksLeft;
	UINT32 reschedTicks;
	UINT32 expirations;
	void (*handler)(UINT32);
	UINT32 timerArgc;
} OS_Timer_t;

/*
** OS System call prototypes
*/
void OSA_RttInit(void);

OSA_STATUS OSA_RttTaskCreate( OS_Task_t *, void *, UINT32, UINT8, CHAR *, void (*)(void*), void * );
OSA_STATUS OSA_RttTaskDelete( OS_Task_t *task );
OSA_STATUS OSA_RttTaskSuspend( OS_Task_t *task );
OSA_STATUS OSA_RttTaskResume( OS_Task_t *task );
void OSA_RttTaskSleep( UINT32 ticks );
OSA_STATUS OSA_RttTaskChangePriority( OS_Task_t task, UINT8 newPriority, UINT8 *oldPriority);
OSA_STATUS OSA_RttTaskGetPriority( OS_Task_t task, UINT8 *priority);

OSA_STATUS OSA_RttQueueCreate( OS_MsgQ_t *msgQRef, UINT32 maxSize, UINT32 maxNumber, UINT32 waitingMode );
OSA_STATUS OSA_RttQueueDelete( OS_MsgQ_t *msgQRef );
OSA_STATUS OSA_RttQueueSend( OS_MsgQ_t *msgQRef, UINT32 size, UINT8 *msg, UINT32 timeout );
OSA_STATUS OSA_RttQueueRecv( OS_MsgQ_t *msgQRef, UINT8 *msg, UINT32 size, UINT32 timeout );
OSA_STATUS OSA_RttQueuePoll( OS_MsgQ_t *msgQRef, UINT32 *msgCount );
OSA_STATUS OSA_RttMboxSend( OS_MsgQ_t *msgQRef, UINT32 size, void *msg, UINT32 timeout);
OSA_STATUS OSA_RttMboxRecv( OS_MsgQ_t *msgQRef, void *msg, UINT32 size, UINT32 timeout );
OSA_STATUS OSA_RttFlagCreate( OS_Flag_t *flag );
OSA_STATUS OSA_RttFlagDelete( OS_Flag_t *flag );
OSA_STATUS OSA_RttFlagSetBits( OS_Flag_t *flag, UINT32 mask, UINT32 operation );
OSA_STATUS OSA_RttFlagWait( OS_Flag_t *flag, UINT32 mask, UINT32 operation, UINT32* flags, UINT32 timeout );
OSA_STATUS OSA_RttFlagPeek( OS_Flag_t *flag, UINT32* flags );
OSA_STATUS OSA_RttTimerCreate( OS_Timer_t *timer );
OSA_STATUS OSA_RttTimerDelete( OS_Timer_t *timer );
OSA_STATUS OSA_RttTimerStart( OS_Timer_t * timer, UINT32 time, UINT32 resched, void (*handler)(UINT32), UINT32 argc );
OSA_STATUS OSA_RttTimerStop( OS_Timer_t *timer );
OSA_STATUS OSA_RttTimerGetStatus( OS_Timer_t *timer, OSATimerStatus* status );
void OSA_RttTimerExpire( UINT32 );
void OSA_RttClockTick(void);

OSA_STATUS OSA_RttSemaCreate( OS_Sema_t *sema, UINT32 initialCount, UINT32 waitingMode );
OSA_STATUS OSA_RttSemaDelete( OS_Sema_t *sema );
OSA_STATUS OSA_RttSemaAcquire( OS_Sema_t *sema, UINT32 timeout );
OSA_STATUS OSA_RttSemaRelease( OS_Sema_t *sema );
OSA_STATUS OSA_RttMemPoolCreate( void *pool, UINT32 poolType, UINT8 *poolBase, UINT32 poolSize, UINT32 partitionSize, UINT32 waitingMode );
OSA_STATUS OSA_RttMemPoolDelete( void *pool, UINT32 poolType );
OSA_STATUS OSA_RttMemAlloc( UINT32 size, void **mem, UINT32 timeout );
OSA_STATUS OSA_RttMemFree( void* mem);


/*
** OS System call macros
*/

#define OSA_INIT OSA_RttInit()
#define OSA_RUN /* Rtt does not need to do anything more before the scheduler runs */

#define OSA_GET_REF(pool, ref, size) \
	{ \
		OSA_STATUS rtn;	\
		OSA_SEMA_ACQUIRE(pool->poolSemaphore, OSA_SUSPEND, rtn); \
		OSA_GET_FROM_FREE_LIST(pool->free, ref); \
		if(ref == OS_NULL) {\
			OSA_SEMA_RELEASE(pool->poolSemaphore, rtn); } \
		else { \
			OSA_ADD_TO_ACTIVE_LIST((pool->head), ref); \
			pool->numCreated++; \
			OSA_SEMA_RELEASE(pool->poolSemaphore, rtn); \
			ref->refCheck = (void *)ref; } \
	}
#define OSA_DELETE_REF(pool, ref) \
	{ \
		OSA_STATUS rtn;	\
		OSA_SEMA_ACQUIRE(pool->poolSemaphore, OSA_SUSPEND, rtn); \
		OSA_DELETE_FROM_ACTIVE_LIST((pool->head), ref);	\
		pool->numCreated--; \
		OSA_ADD_TO_FREE_LIST((pool->free), ref); \
		OSA_SEMA_RELEASE(pool->poolSemaphore, rtn); \
		ref->refCheck = NULL; \
	}

/* Thread Control */
#define OSA_TASK_CREATE(taskPtr, stackPtr, stackSize, priority,	    \
			name, taskStart, argv, rtn)		    \
	{								\
		rtn = OSA_RttTaskCreate( taskPtr, stackPtr, stackSize, priority, name, taskStart, argv); \
	}
#define OSA_TASK_DELETE(taskPtr, rtn)				    \
	{								\
		rtn = OSA_RttTaskDelete( &taskPtr  );			    \
	}
#define OSA_TASK_SUSPEND(taskPtr, rtn)				    \
	{								\
		rtn = OSA_RttTaskSuspend( &taskPtr );			    \
	}
#define OSA_TASK_RESUME(taskPtr, rtn)				    \
	{								\
		rtn = OSA_RttTaskResume( &taskPtr  );			    \
	}
#define OSA_TASK_SLEEP(ticks)					    \
	{								\
		OSA_RttTaskSleep( ticks );				    \
	}
#define OSA_TASK_CHANGE_PRIORITY(taskPtr, newPriority, oldPriority, rtn) \
	{								\
		rtn = OSA_RttTaskChangePriority( *taskPtr, newPriority, oldPriority ); \
	}
#define OSA_TASK_GET_PRIORITY(taskPtr, oldPriority, rtn)	    \
	{								\
		rtn = OSA_RttTaskGetPriority( *taskPtr, oldPriority );	    \
	}
#define OSA_TASK_IDENTIFY(taskPtr)				    \
	{								\
		OS_Task_t currTask;				      \
		OS_TaskPool*    taskPoolPtr;				    \
		OS_Task*        task;					    \
		UINT8 i;					  \
								    \
		currTask = RttMyThreadId();				    \
		taskPoolPtr = &OSA_ControlBlockPtr->taskPool;		    \
		task = &taskPoolPtr->task[0];				    \
		for (i = 0; i < OSA_TASKS; i++)				     \
		{							    \
			if (currTask.lid == task->os_ref.lid)			 \
			{							\
				taskPtr = (OS_Task_t *)&task->os_ref;		    \
				break;						    \
			}							\
			task++;							\
		}							    \
	}
#define OSA_TASK_YIELD OSA_RttTaskSleep( 1 ) /*sched_yield()*/
#define OSA_TASK_EQUAL(taskPtr, rtn)				    \
	{								\
		OS_Task_t currTask;					    \
		if (taskPtr == NULL)					     \
		{							    \
			rtn = FALSE;						\
		}							    \
		else							    \
		{							    \
			currTask = RttMyThreadId();				\
			if (taskPtr->lid == currTask.lid)								      \
				rtn = TRUE;									\
			else									 \
				rtn = FALSE;									 \
		}							    \
	}

/* Messaging */
#define OS_QUEUE_CREATE(qRef, name, size, num, addr, mode, rtn)	    \
	{								\
		rtn = OSA_RttQueueCreate(qRef, size, num, mode);	    \
	}
#define OS_QUEUE_DELETE(qRef, rtn)				    \
	{								\
		rtn = OSA_RttQueueDelete(&qRef);			    \
	}
#define OS_QUEUE_SEND(qRef, size, ptr, timeout, rtn)		    \
	{								\
		rtn = OSA_RttQueueSend(&qRef, size, ptr, timeout);	    \
	}
#define OS_QUEUE_RECV(qRef, msg, size, timeout, rtn)		    \
	{								\
		rtn = OSA_RttQueueRecv(&qRef, msg, size, timeout);	    \
	}
#define OS_QUEUE_POLL(qRef, msgCount, rtn)			    \
	{								\
		rtn = OS_UNSUPPORTED;					    \
	}

/* Mailboxes */
#define OSA_MAILBOX_CREATE(qRef, name, num, mode, rtn)		    \
	{								\
		rtn = OSA_RttQueueCreate(qRef, OSA_MAX_MBOX_DATA_SIZE, num, mode); \
	}
#define OSA_MAILBOX_DELETE(qRef, rtn)				    \
	{								\
		rtn = OSA_RttQueueDelete(&qRef);			    \
	}
#define OSA_MAILBOX_SEND(qRef, ptr, timeout, rtn)		    \
	{								\
		rtn = OSA_RttQueueSend(&qRef, OSA_MAX_MBOX_DATA_SIZE, ptr, timeout);   \
	}
#define OSA_MAILBOX_RECV(qRef, msg, timeout, rtn)		    \
	{								\
		rtn = OSA_RttQueueRecv(&qRef, msg, OSA_MAX_MBOX_DATA_SIZE, timeout);   \
	}
#define OSA_MAILBOX_POLL(qRef, msgCount, rtn)			    \
	{								\
		rtn = OS_UNSUPPORTED;					    \
	}

/* Events */
#define OSA_FLAG_CREATE(flagRef, name, rtn)			    \
	{								\
		rtn = OSA_RttFlagCreate(flagRef);			    \
	}
#define OSA_FLAG_DELETE(flagRef, rtn)				    \
	{								\
		rtn = OSA_RttFlagDelete(flagRef);			    \
	}
#define OSA_FLAG_SET_BITS(flagRef, mask, operation, rtn)	    \
	{								\
		rtn = OSA_RttFlagSetBits(flagRef, mask, operation);	    \
	}
#define OSA_FLAG_WAIT(flagRef, reqValue, mode, currValue, timeout, rtn)	\
	{								\
		rtn = OSA_RttFlagWait(flagRef, reqValue, mode, currValue, timeout); \
	}
#define OSA_FLAG_PEEK(flagRef, value, rtn)			    \
	{								\
		*value = flagRef.flag;					    \
		rtn = OS_SUCCESS;					    \
	}

/* Timers */
#define OSA_TIMER_CREATE(timerRef, rtn)				    \
	{								\
		rtn = OSA_RttTimerCreate(timerRef);			    \
	}
#define OSA_TIMER_DELETE(timerRef, rtn)				    \
	{								\
		rtn = OSA_RttTimerDelete(&timerRef);			    \
	}
#define OSA_TIMER_START(timerRef, time, resched, handler,	    \
			argc, rtn)				     \
	{								\
		rtn = OSA_RttTimerStart(&timerRef, time, resched, handler, argc); \
	}
#define OSA_TIMER_STOP(timerRef, rtn)				    \
	{								\
		rtn = OSA_RttTimerStop(&timerRef);			    \
	}
#define OSA_TIMER_STATUS(timerRef, status, rtn)			    \
	{								\
		status->status        = timerRef.state;			    \
		status->expirations   = timerRef.expirations;		    \
		rtn = OS_SUCCESS;					    \
	}


#define OSA_MEM_ALLOC(pool, type, size, mem, timeout, rtn)	    \
	{								\
		rtn = OSA_RttMemAlloc( size, mem, timeout );		    \
	}
#define OSA_MEM_FREE(pool, type, mem, rtn)			    \
	{								\
		RttFree(mem);						    \
		rtn = OS_SUCCESS;					    \
	}
#define OSA_MEM_POOL_CREATE(pool, type, base, size, partition,	    \
			    mode, rtn)				     \
	{								\
		rtn = OS_SUCCESS;					    \
	}
#define OSA_MEM_POOL_DELETE(pool, type, rtn)			    \
	{								\
		rtn = OS_SUCCESS;					    \
	}


/* Semaphores */
#define OSA_SEMA_CREATE(sema, name, initialCount, waitingMode, rtn ) \
	{								\
		rtn = OSA_RttSemaCreate(sema, initialCount, waitingMode);   \
	}
#define OSA_SEMA_DELETE(sema, rtn)				    \
	{								\
		rtn = OSA_RttSemaDelete(&sema);				    \
	}
#define OSA_SEMA_ACQUIRE(sema, timeout, rtn)			    \
	{								\
		rtn = OSA_RttSemaAcquire(&sema, timeout);		    \
	}
#define OSA_SEMA_RELEASE(sema, rtn)				    \
	{								\
		rtn = OSA_RttSemaRelease(&sema);			    \
	}
#define OSA_SEMA_POLL(sema, count, rtn)				    \
	{								\
		int cnt;						    \
								    \
		rtn = RttSemValue(sema, &cnt);				    \
		*count = cnt;						    \
	}

/* Mutexes */
#define OSA_MUTEX_CREATE(mutex, name, waitingMode, rtn )	    \
	{								\
		rtn = OSA_RttSemaCreate(mutex, 1, waitingMode);		    \
	}
#define OSA_MUTEX_DELETE(mutex, rtn)				    \
	{								\
		rtn = OSA_RttSemaDelete(&mutex);			    \
	}
#define OSA_MUTEX_LOCK(mutex, timeout, rtn)			    \
	{								\
		rtn = OSA_RttSemaAcquire(&mutex, timeout);		    \
	}
#define OSA_MUTEX_UNLOCK(mutex, rtn)				    \
	{								\
		rtn = OSA_RttSemaRelease(&mutex);			    \
	}

/* Context lock */
#define OSA_CONTEXT_LOCK RttBeginCritical()
#define OSA_CONTEXT_UNLOCK RttEndCritical()

#define OSA_ISR_CREATE_FISR(isrNum, func, rtn)			    \
	{								\
		rtn = OS_SUCCESS;					    \
	}
#define OSA_ISR_CREATE_SISR(isrNum, isr, func, priority, stack, size, rtn) \
	{								\
		rtn = OS_SUCCESS;					    \
	}
#define OSA_ISR_DELETE_SISR(isr, rtn)				    \
	{								\
		rtn = OS_SUCCESS;					    \
	}
#define OSA_ISR_NOTIFY_SISR(isr, rtn)				    \
	{								\
		rtn = OS_SUCCESS;					    \
	}
#define OSA_INTERRUPT_ENABLE(number, rtn)			    \
	{								\
		rtn = OS_SUCCESS;					    \
	}
#define OSA_INTERRUPT_DISABLE(number, rtn)			    \
	{								\
		rtn = OS_SUCCESS;					    \
	}

/* RTC services */
#define OSA_GET_TIME(ticks)					    \
	{								\
		ticks = OSA_RttRtcCounter;				    \
	}

#define OSA_GET_SYSTEM_TIME(secsPtr, milliSecsPtr, rtn)		    \
	{								\
		UINT32 currentTime;					    \
								    \
		currentTime = OSA_RttRtcCounter * OSA_TICK_FREQ_IN_MILLISEC; \
								    \
		*secsPtr = currentTime / 1000;				   \
		*milliSecsPtr = (UINT16)				    \
				(( currentTime - (*secsPtr * 1000)) / 10) * 10;	\
		rtn = OS_SUCCESS;					    \
	}

#define OSA_CLOCK_TICK OSA_RttClockTick()


#endif /* _OSA_RTT_H */

