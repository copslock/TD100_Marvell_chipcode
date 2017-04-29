/* ===========================================================================
   File        : osa_linux.h
   Description : Definition of OSA Software Layer data types specific to the
	      POSIX OS.

   Notes       :

   Copyright (c) 2005 Intel CHG. All Rights Reserved
   =========================================================================== */
#ifndef _OSA_LINUX_H
#define _OSA_LINUX_H


#ifdef OSA_LINUX

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <errno.h>
#include <string.h>
#include "gbl_types.h"
#include "osa_types.h"

/************************************************************************
 * External Interfaces
 ***********************************************************************/

extern UINT32 OSA_RtcCounter;
//extern CRITICAL_SECTION csOSAContextLock;

/*************************************************************************
* Constants
*************************************************************************/
#define OSA_PIPE_MEM_OVERHEAD   4

#define OSA_POOL_MEM_OVERHEAD         8                         /* Fixed pool overhead per partition */
#define OSA_FIXED_POOL_MEM_OVERHEAD   sizeof(OS_PartMemHdr)     /* Fixed pool overhead per partition */
#define OSA_VAR_POOL_MEM_OVERHEAD     12                        /* Variable pool overhead per partition */
#define OSA_MAX_TASK_NAME_SIZE          8
#define OSA_MAX_PRIORITY                31
#define OSA_MAX_NATIVE_PRIORITY         1000      /* Symbian priorities are 0 lowest to 1000 highest (opposite to OSA) */

/* Remain for backwards compatibility */
#define OS_SUSPEND              OSA_SUSPEND
#define OS_NO_SUSPEND           OSA_NO_SUSPEND
#define OS_FLAG_AND             OSA_FLAG_AND
#define OS_FLAG_AND_CLEAR       OSA_FLAG_AND_CLEAR
#define OS_FLAG_OR              OSA_FLAG_OR
#define OS_FLAG_OR_CLEAR        OSA_FLAG_OR_CLEAR
#define OS_FIXED                OSA_FIXED
#define OS_VARIABLE             OSA_VARIABLE
#define OS_FIFO                 OSA_FIFO
#define OS_PRIORITY             OSA_PRIORITY

#define OSA_TIMER_STACK      20000
#define OSA_TIMER_PRIORITY   20
#define OSA_TICK_STACK       20000
#define OSA_TICK_PRIORITY    25

/*************************************************************************
* POSIX Specific Types
*************************************************************************/
typedef UINT32 OS_Hisr_t;
typedef UINT32 OS_MemPool_t;
typedef pid_t OS_Proc_t;

typedef void (*funcPtrType)(void*);
typedef struct
{
	pthread_t task;
	pthread_attr_t attr;
	funcPtrType taskStartPtr;
	void            *argv;
	UINT8 priority;
	UINT8 filler[3];
} OS_Task_t;

/* Semaphore reference */
typedef struct
{
	sem_t os_sema;
	UINT32 count;
} OS_Sema_t;

/* Mutex reference */
typedef struct
{
	pthread_mutex_t os_mutex;
	pthread_mutexattr_t attr;
	UINT32 owner;
	UINT32 locked;
} OS_Mutex_t;

/* Flag reference */
typedef struct
{
	UINT32 flag;
	OS_Sema_t semaphore;
} OS_Flag_t;

/* Partition memory pool reference */
typedef struct
{
	struct _OS_PartMemHdr   *free;  /* Free partition list */
	OS_Mutex_t mutex;               /* Mutex to secure access */
	OS_Sema_t sema;                 /* Sema for blocking when no mem left */
	UINT32 numFree;                 /* Number of partitions free */
	void                    *chunk;
} OS_PartitionPool_t;

/* Partition memory pool header */
typedef struct _OS_PartMemHdr
{
	struct _OS_PartMemHdr   *next;
	OS_PartitionPool_t      *pool;
#ifdef OSA_MEM_CHECK
	UINT32 size;
	UINT32 partSize;
#endif
} OS_PartMemHdr;

#ifndef OSA_USE_INTERNAL_MESSAGING
/* Message Queue reference */
typedef struct
{
	int os_queue;
	UINT32 id;
	UINT32 maxSize;
	UINT32 maxNum;
	UINT32 msgCount;
} OS_MsgQ_t;

/* Linux Messaging requires a struct to be sent */
typedef struct
{
	UINT32 type;
	char        *msg;
} OS_Msg;

/* Mailbox Queue reference */
typedef struct
{
	int os_queue;
	UINT32 id;
	UINT32 maxSize;
	UINT32 maxNum;
	UINT32 msgCount;
} OS_Mbox_t;

#else
//
// Base Message Queue structure
// Created only ONCE for every message queue, includes the message queue as well
// Is created in the address space of invoking process/task,
// but then the pointer is mapped so that other process/task can access this as well
// in their own address space
// The memory for this structure and the message queue is shared across all processes/tasks
// that want to use the memory queue
//
#define MAX_NAME_LENGTH             128
#define OSA_MSG_QUEUE_HEADER_SIZE   sizeof(OSMsgQInfo)
typedef struct _MsgQInfo
{
	UINT8*      memPtr;                     // Original allocated memory pointer
	UINT8*      msgPtr;                     // Pointer to start of actual message queue
	UINT32*     msgLenPtr;                  // Pointer to message len array
	UINT32 maxNumMsgs;                      // Maximum number of messages in message queue
	UINT32 maxSizeOfMsg;                    // Maximum size of each message in the queue
	UINT32 msgCount;                        // Current message number in the queue
	UINT32 readCount;                       // Number of total messages read from queue
	UINT32 writeCount;                      // Number of total messages written into queue
	UINT8 queueName[MAX_NAME_LENGTH];       // Unique Queue Name, used to create a unique event for queue
} OSMsgQInfo, *POSMsgQInfo;

//
// Local message queue structure
// Created when the message queue is opened/initialized by each process/task that wants to use it
// This structure is created once in address space of every process/task that wants to use the
// message queue
// Includes a pointer to base memory queue
//
typedef struct _LocalMsgQInfo
{
	OS_Sema_t recvEventHandle;
	OS_Sema_t sendSemaphore;
	OS_Sema_t protectSemaphore;
	POSMsgQInfo pMsgQInfo;

} LocalMsgQInfo, *PLocalMsgQInfo;


typedef UINT32 BaseMsgQHandle;
typedef UINT32 MsgQHandle;



/* Message queue reference */
typedef struct
{
	PLocalMsgQInfo os_ref;
} OS_MsgQ_t;

/* Mailbox queue reference */
typedef struct
{
	LocalMsgQInfo os_ref;
} OS_Mbox_t;
#endif

/* timer information */
typedef void (*timerCBPtrType)(UINT32);
typedef struct
{
	UINT32 os_ref;
	UINT32 state;
	UINT32 ticksLeft;
	UINT32 reschedTicks;
	UINT32 expirations;
	timerCBPtrType callback;
	UINT32 timerArgc;
} OS_Timer_t;


/*
** OS System call prototypes
*/
void OSA_Initialize(void*);

OSA_STATUS OSA_TaskCreate( OS_Task_t *, UINT8 *, UINT8, UINT32, void (*)(void*), void *);
OSA_STATUS OSA_TaskDelete( OS_Task_t task );
OSA_STATUS OSA_TaskSuspend( OS_Task_t task );
OSA_STATUS OSA_TaskResume( OS_Task_t task );
OSA_STATUS OSA_TaskGetPriority( OS_Task_t *task, UINT8 *oldPriority);
OSA_STATUS OSA_TaskChangePriority( OS_Task_t *task, UINT8 newPriority, UINT8 *oldPriority);
void* OSA_TaskStartWrapper(void *argv);

OSA_STATUS OSA_QueueCreate( OS_MsgQ_t *msgQRef, char *queueName, UINT32 maxSize, UINT32 maxNumber, void *queueAddr, UINT8 waitingMode );
OSA_STATUS OSA_QueueDelete( OS_MsgQ_t *msgQRef );
OSA_STATUS OSA_QueueSend( OS_MsgQ_t *msgQRef, UINT32 size, UINT8 *msg, UINT32 timeout);
OSA_STATUS OSA_QueueRecv( OS_MsgQ_t *msgQRef, UINT8 *msg, UINT32 size, UINT32 timeout );
OSA_STATUS OSA_QueuePoll( OS_MsgQ_t *msgQRef, UINT32 *msgCount );

OSA_STATUS OSA_FlagCreate( OS_Flag_t *flagRef, char * );
OSA_STATUS OSA_FlagDelete( OS_Flag_t *flagRef);
OSA_STATUS OSA_FlagSetBits( OS_Flag_t *flagRef, UINT32 mask, UINT32 operation);
OSA_STATUS OSA_FlagWait( OS_Flag_t *flagRef, UINT32 mask, UINT32 operation, UINT32* flags, UINT32 timeout );

OSA_STATUS OSA_SemaCreate( OS_Sema_t *sema, char *, UINT32 initialCount, UINT8 waitingMode );
OSA_STATUS OSA_SemaDelete( OS_Sema_t *sema );
OSA_STATUS OSA_SemaAcquire( OS_Sema_t *sema, UINT32 timeout );
OSA_STATUS OSA_SemaRelease( OS_Sema_t *sema );
OSA_STATUS OSA_SemaPoll( OS_Sema_t *sema, UINT32* count );

OSA_STATUS OSA_TimerCreate( OS_Timer_t *timer );
OSA_STATUS OSA_TimerDelete( OS_Timer_t *timer );
OSA_STATUS OSA_TimerStart( OS_Timer_t * timer, UINT32 time, UINT32 resched, void (*handler)(UINT32), UINT32 argc );
OSA_STATUS OSA_TimerStop( OS_Timer_t *timer );
OSA_STATUS OSA_TimerGetStatus( OS_Timer_t *timer, OSATimerStatus* status );

void OSA_ClockTick(void *block);
//UINT32 OSA_GetTicks(void);

OSA_STATUS OSA_MutexCreate( OS_Mutex_t *mutex, char* name );
OSA_STATUS OSA_MutexDelete( OS_Mutex_t *mutex );
OSA_STATUS OSA_MutexLock( OS_Mutex_t *mutex, UINT32 timeout );
OSA_STATUS OSA_MutexUnlock( OS_Mutex_t *mutex );

OSA_STATUS OSA_MemPoolCreate( void *pool, char *name, UINT32 poolType, UINT8 *poolBase, UINT32 poolSize, UINT32 partitionSize);
OSA_STATUS OSA_MemPoolDelete( void *pool, UINT32 poolType );
OSA_STATUS OSA_MemAlloc( void *pool, UINT32 poolType, UINT32 size, UINT32 partSize, void **mem, UINT32 timeout );
OSA_STATUS OSA_MemFree( void *pool, UINT32 poolType, void* mem );

BOOL OSA_GetControlBlock(void**, size_t, BOOL );
//OSA_STATUS OSA_FIsrCreate  ( UINT32, void(*)(UINT8) );
OSA_STATUS OSA_SIsrCreate( UINT32, OS_Hisr_t *, void (*)(void), UINT8, UINT8 *, UINT32 );
OSA_STATUS OSA_SIsrDelete( OS_Hisr_t *);
OSA_STATUS OSA_SIsrNotify( UINT32 isrNum );
OSA_STATUS OSA_TranslateErrorCode(UINT32 osErrCode);
void OSA_Run(void);
void OSA_TaskSleep(UINT32 ticks);
void OSA_ContextLock(void);
void OSA_ContextUnlock(void);

/*
** OS System call macros
*/

#define OSA_INIT(a) OSA_Initialize(a)
#define OSA_RUN OSA_Run() /* nothing needed to be done in Linux */
#ifdef OSA_USE_DYNAMIC_REFS
#define OSA_GET_REF(pool, ref, size) \
	{ \
		if (!((void*)ref = malloc((UINT32)size)) ) { \
			      OSA_ASSERT(FALSE); } \
		      ref->refCheck = (void *)ref; \
		      }
#define OSA_DELETE_REF(pool, ref) \
	{ \
		ref->refCheck = NULL; \
		free(ref); \
	}
#else
#define OSA_GET_REF(pool, ref, size) \
	{ \
		OSA_STATUS rtn;	\
		OSA_SEMA_ACQUIRE(pool->poolSemaphore, OSA_SUSPEND, rtn); \
		OSA_GET_FROM_FREE_LIST(pool->free, ref); \
		if(ref == OS_NULL) { \
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
#endif

/* Thread Control */

#define OSA_TASK_CREATE(taskPtr, stackPtr, stackSize, priority, name, \
			taskStart, argv, rtn) \
	{ \
		rtn = OSA_TaskCreate( taskPtr, name, priority, stackSize, taskStart, argv); \
	}
#define OSA_TASK_DELETE(taskPtr, rtn)				    \
	{								\
		rtn = OSA_TaskDelete( taskPtr );			    \
	}
#define OSA_TASK_SUSPEND(taskPtr, rtn)				    \
	{								\
		rtn = OSA_TaskSuspend( taskPtr );			    \
	}
#define OSA_TASK_RESUME(taskPtr, rtn)				    \
	{								\
		rtn = OSA_TaskResume( taskPtr );			    \
	}
#define OSA_TASK_SLEEP(ticks)					    \
	{								\
		OSA_TaskSleep(ticks);						   \
	}

#define OSA_TASK_CHANGE_PRIORITY(taskPtr, newPriority, oldPriority, rtn) \
	{								\
		rtn = OSA_TaskChangePriority( taskPtr, newPriority, oldPriority );  \
	}
#define OSA_TASK_GET_PRIORITY(taskPtr, oldPriority, rtn)	    \
	{								\
		rtn = OSA_TaskGetPriority( taskPtr, oldPriority );	    \
	}
#define OSA_TASK_IDENTIFY(taskPtr)				    \
	{								\
		OSA_ControlBlock* block;				    \
		BOOL stat = OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE); \
		stat = stat; /*suppress compiler warning*/	     \
		pthread_t currentId;					    \
		OS_Task *task = block->taskPool.head;			    \
		if (NULL == task) OSA_ASSERT(FALSE);									 \
		currentId = pthread_self();				    \
		while (task != NULL) {					     \
			if (currentId == task->os_ref.task)			 \
			{							\
				taskPtr = (OS_Task_t *)&task->os_ref;		    \
			}							\
			task = task->next;					\
		}							    \
	}
#define OSA_TASK_YIELD usleep(1);
#define OSA_TASK_EQUAL(taskPtr, rtn)				    \
	{								\
		OS_Task_t *currTask;					    \
		currTask = pthread_self();				    \
		if (taskPtr == currTask)								     \
			rtn = TRUE;									\
		else									 \
			rtn = FALSE;									 \
	}

/* Messaging */
#define OSA_QUEUE_CREATE(qRef, name, size, num, addr, mode, rtn)    \
	{								\
		rtn = OSA_QueueCreate(qRef, name, size, num, addr, mode); \
	}
#define OSA_QUEUE_DELETE(qRef, rtn)				    \
	{								\
		rtn = OSA_QueueDelete(&qRef);				    \
	}
#define OSA_QUEUE_SEND(qRef, size, ptr, timeout, rtn)		    \
	{								\
		rtn = OSA_QueueSend(&qRef, size, ptr, timeout);		    \
	}
#define OSA_QUEUE_RECV(qRef, msg, size, timeout, rtn)		    \
	{								\
		rtn = OSA_QueueRecv(&qRef, msg, size, timeout);		    \
	}

#define OSA_QUEUE_POLL(qRef, msgCount, rtn)			    \
	{								\
		rtn = OS_UNSUPPORTED;					    \
	}

/* Mailboxes */
#define OSA_MAILBOX_CREATE(qRef, name, num, mode, rtn)		    \
	{								\
		rtn = OSA_QueueCreate((OS_MsgQ_t *)qRef, name, OSA_MAX_MBOX_DATA_SIZE, num, 0L, mode);	  \
	}
#define OSA_MAILBOX_DELETE(qRef, rtn)				    \
	{								\
		rtn = OSA_QueueDelete((OS_MsgQ_t *)&qRef);		    \
	}
#define OSA_MAILBOX_SEND(qRef, ptr, timeout, rtn)		    \
	{								\
		rtn = OSA_QueueSend((OS_MsgQ_t *)&qRef, OSA_MAX_MBOX_DATA_SIZE, ptr, timeout);	      \
	}
#define OSA_MAILBOX_RECV(qRef, msg, timeout, rtn)		    \
	{								\
		rtn = OSA_QueueRecv((OS_MsgQ_t *)&qRef, msg, OSA_MAX_MBOX_DATA_SIZE, timeout);	      \
	}
#define OSA_MAILBOX_POLL(qRef, msgCount, rtn)			    \
	{								\
		rtn = OS_UNSUPPORTED;					    \
	}

/* Events */
#define OSA_FLAG_CREATE(flagRef, name, rtn)			    \
	{								\
		rtn = OSA_FlagCreate(flagRef, name);			    \
	}
#define OSA_FLAG_DELETE(flagRef, rtn)				    \
	{								\
		rtn = OSA_FlagDelete(flagRef);				    \
	}

#define OSA_FLAG_SET_BITS(flagRef, mask, operation, rtn)	    \
	{								\
		rtn = OSA_FlagSetBits(flagRef, mask, operation);	    \
	}

#define OSA_FLAG_WAIT(flagRef, reqValue, mode, currValue, timeout, rtn)	\
	{								\
		rtn = OSA_FlagWait(flagRef, reqValue, mode, currValue, timeout);  \
	}
#define OSA_FLAG_PEEK(flagRef, value, rtn)			    \
	{								\
		*value = flagRef.flag;					    \
		rtn = OS_SUCCESS;					    \
	}


/* Timers */
#define OSA_TIMER_CREATE(timerRef, rtn)				    \
	{								\
		rtn = OSA_TimerCreate(timerRef);			    \
	}
#define OSA_TIMER_DELETE(timerRef, rtn)				    \
	{								\
		rtn = OSA_TimerDelete(&timerRef);			    \
	}

#define OSA_TIMER_START(timerRef, time, resched, handler,	    \
			argc, rtn)				     \
	{								\
		rtn = OSA_TimerStart(&timerRef, time, resched, handler, argc); \
	}
#define OSA_TIMER_STOP(timerRef, rtn)				    \
	{								\
		rtn = OSA_TimerStop(&timerRef);				    \
	}
#define OSA_TIMER_STATUS(timerRef, status, rtn)			    \
	{								\
		status->status        = timerRef.state;			    \
		status->expirations   = timerRef.expirations;		    \
		rtn = OS_SUCCESS;					    \
	}

#define OSA_MEM_ALLOC(pool, type, size, partSize, mem, timeout, rtn) \
	{								\
		rtn = OSA_MemAlloc( (void*)pool, type, size, partSize, mem, timeout ); \
	}
#define OSA_MEM_FREE(pool, type, mem, rtn)			    \
	{								\
		rtn = OSA_MemFree( (void*)pool, type, mem );		      \
	}
#define OSA_MEM_POOL_CREATE(pool, name, type, base, size, partition, mode, rtn)	\
	{								\
		rtn = OSA_MemPoolCreate(pool, name, type, base, size, partition);  \
	}
#define OSA_MEM_POOL_DELETE(pool, type, rtn)			    \
	{								\
		rtn = OSA_MemPoolDelete(pool, type);			    \
	}

/* Semaphores */
#define OSA_SEMA_CREATE(sema, name, initialCount, waitingMode, rtn ) \
	{								\
		rtn = OSA_SemaCreate(sema, name, initialCount, waitingMode); \
	}
#define OSA_SEMA_DELETE(sema, rtn)				    \
	{								\
		rtn = OSA_SemaDelete(&sema);				    \
	}

#define OSA_SEMA_ACQUIRE(sema, timeout, rtn)			    \
	{								\
		rtn = OSA_SemaAcquire(&sema, timeout);			    \
	}
#define OSA_SEMA_RELEASE(sema, rtn)				    \
	{								\
		rtn = OSA_SemaRelease(&sema);				    \
	}
#define OSA_SEMA_POLL(sema, count, rtn)				    \
	{								\
		rtn = OSA_SemaPoll(&sema, count);			    \
	}

/* Mutexes */
#define OSA_MUTEX_CREATE(mutex, name, waitingMode, rtn )	    \
	{								\
		rtn = OSA_MutexCreate(mutex, name);			    \
	}
#define OSA_MUTEX_DELETE(mutex, rtn)				    \
	{								\
		rtn = OSA_MutexDelete(&mutex);				    \
	}
#define OSA_MUTEX_LOCK(mutex, timeout, rtn)			    \
	{								\
		rtn = OSA_MutexLock(&mutex, timeout);			    \
	}

#define OSA_MUTEX_UNLOCK(mutex, rtn)				    \
	{								\
		rtn = OSA_MutexUnlock(&mutex);				    \
	}

/* Context lock */
#define OSA_CONTEXT_LOCK        OSA_ContextLock()

#define OSA_CONTEXT_UNLOCK      OSA_ContextUnlock()

/*#define OSA_ISR_CREATE_FISR(isrNum, func, rtn)                      \
    {                                                               \
	rtn = OSA_FIsrCreate(isrNum, func);                         \
    }*/
#define OSA_ISR_CREATE_SISR(isrNum, isr, func, priority, stack, size, rtn) \
	{								\
		rtn = OSA_SIsrCreate(isrNum, isr, func, priority, stack, size);	\
	}
#define OSA_ISR_DELETE_SISR(isr, rtn)				    \
	{								\
		rtn = OSA_SIsrDelete(isr);				    \
	}
#define OSA_ISR_NOTIFY_SISR(isr, rtn)				    \
	{								\
		rtn = OSA_SIsrNotify(isr);				    \
	}
#define OSA_INTERRUPT_ENABLE(number, rtn)			    \
	{								\
		rtn = OS_SUCCESS;					    \
	}
/*        INTERRUPTS_ON();                                            \*/
#define OSA_INTERRUPT_DISABLE(number, rtn)			    \
	{								\
		rtn = OS_SUCCESS;					    \
	}
/*        INTERRUPTS_OFF();                                           \*/

/* RTC services */
#define OSA_GET_TIME(time)					    \
	{								\
		OSA_ControlBlock* block;				    \
		BOOL stat = OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE); \
		stat = stat; /*suppress compiler warnning*/	       \
		time = block->OSA_RtcCounter;				    \
	}
#define OSA_GET_SYSTEM_TIME(secsPtr, milliSecsPtr, rtn)		    \
	{								\
		rtn = OS_SUCCESS;					    \
	}

#define OSA_CLOCK_TICK(a) OSA_ClockTick(a)

#define OSA_ERROR(err)						    \
	{								\
		DBGMSG("\n*** OSA_LINUX_ERROR -> %d *** \n File: %s\n Line: %d\n", err, __FILE__, __LINE__); \
		exit(1); \
	}                                                    /*changed %ld to %d due to compiler warning*/


#endif  /* OSA_LINUX */

#endif  /* _OSA_LINUX_H */

