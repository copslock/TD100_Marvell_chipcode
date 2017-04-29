/* ===========================================================================
   File        : osa.h
   Description : This file defines the Intel CCD OS wrapper API and
	      definitions for external application use.

	      The OSA API allows applications to be developed independent of
	      the target microkernel/hardware environment. It provides the
	      facility to add support for multiple independent operating
	      systems from different vendors.

    The OSA API defines a set of interfaces that provide the following
    basic operating system services:

      OS Management     - OSAInitialize(), OSARun()
      Tasks             - OSATaskCreate(), OSATaskDelete(), OSATaskChangePriority(),
			- OSATaskGetPriority(), OSATaskSuspend(), OSATaskResume(),
			- OSATaskSleep(), OSATaskGetCurrentRef()
      Event Flags       - OSAFlagCreate(), OSAFlagDelete(), OSAFlagSet(),
			- OSAFlagWait(), OSAFlagPeek()
      Timers            - OSATimerCreate(), OSATimerDelete(), OSATimerStart(),
			- OSATimerStop(), OSATimerGetStatus()
      Messaging         - OSAMsgQCreate(), OSAMsgQDelete(),
			- OSAMsgQSend(), OSAMsgQRecv(), OSAMsgQPoll()
			- OSAMsgQCreateWithMem()
      Mailboxes         - OSAMailboxQCreate(), OSAMailboxQDelete(),
			- OSAMailboxQSend(), OSAMailboxQRecv(), OSAMailboxQPoll()
      Semaphores        - OSASemaphoreCreate(), OSASemaphoreDelete(),
			- OSASemaphoreAcquire(), OSASemaphoreRelease(),
			- OSASemaphorePoll()
      Mutexes           - OSAMutexCreate(), OSAMutexDelete(),
			- OSAMutexLock(), OSAMutexUnlock(),
      Interrupt Control - OSAIsrCreate(), OSAIsrDelete(),
			- OSAIsrEnable(), OSAIsrDisable(), OSAIsrNotify()
      Real-Time Clock   - OSAGetTicks(), OSAGetClockRate(), OSATick()
			- OSAGetSystemTime()
      Memory            - OSAMemPoolCreate(), OSAMemPoolDelete(),
			- OSAMemPoolAlloc(), OSAMemPoolFree(),
			- OSAMemPoolFixedFree()
      Thread local storage
			- OSATaskSetTls()
			- OSATaskGetTls()
      Symbian Memory    - OSAMemPoolCreateGlobal(), OSAGlobalAlloc(),
			- OSAGlobalRead(), OSAGlobalWrite()

   Copyright (c) 2001 Intel CCD. All Rights Reserved
   =========================================================================== */
#ifndef _OSA_H
#define _OSA_H

#ifndef IMPC
#define IMPC
#endif

#ifdef __cplusplus
  #ifdef ENV_SYMBIAN
    #include "e32def.h"
    #undef  IMPC
    #define IMPC  IMPORT_C
  #endif
extern "C" {
#endif

#include "gbl_types.h"
#include "osa_config.h" /*needed for OSA_NO_PRIORITY_CONVERSION*/

/*****************************************************************************
 * OSA Constants
 ****************************************************************************/
 #define OSA_API_VERSION         51  /* this is the OSA API document version*/
 #define OSA_NULL                NULL
 #define OSA_TIMER_DEAD          0
 #define OSA_TIMER_CREATED       1
 #define OSA_TIMER_ACTIVE        2
 #define OSA_TIMER_INACTIVE      3
 #define OSA_ENABLED             2
 #define OSA_DISABLED            3

 #define OSA_LOWEST_PRIORITY     255
 #define OSA_HIGHEST_PRIORITY    0

 #define OSA_MIN_STACK_SIZE      256
 #define OSA_ENABLE_INTERRUPTS   1
 #define OSA_DISABLE_INTERRUPTS  2
 #define OSA_SUSPEND             0xFFFFFFFF
 #define OSA_NO_SUSPEND          0
 #define OSA_FLAG_AND            5
 #define OSA_FLAG_AND_CLEAR      6
 #define OSA_FLAG_OR             7
 #define OSA_FLAG_OR_CLEAR       8
 #define OSA_FIXED               9
 #define OSA_VARIABLE            10
 #define OSA_FIFO                11
 #define OSA_PRIORITY            12
 #define OSA_GLOBAL              13

 #define OSA_SISR_PRIORITY       1

/* Remain for backwards compatibility */
 #define OS_NULL                OSA_NULL
 #define OS_TIMER_DEAD          OSA_TIMER_DEAD
 #define OS_TIMER_CREATED       OSA_TIMER_CREATED
 #define OS_TIMER_ACTIVE        OSA_TIMER_ACTIVE
 #define OS_TIMER_INACTIVE      OSA_TIMER_INACTIVE
 #define OS_ENABLED             OSA_ENABLED
 #define OS_DISABLED            OSA_DISABLED
 #define OS_SUSPEND             OSA_SUSPEND
 #define OS_NO_SUSPEND          OSA_NO_SUSPEND
 #define OS_FLAG_AND            OSA_FLAG_AND
 #define OS_FLAG_AND_CLEAR      OSA_FLAG_AND_CLEAR
 #define OS_FLAG_OR             OSA_FLAG_OR
 #define OS_FLAG_OR_CLEAR       OSA_FLAG_OR_CLEAR
 #define OS_LOWEST_PRIORITY     OSA_LOWEST_PRIORITY
 #define OS_HIGHEST_PRIORITY    OSA_HIGHEST_PRIORITY
 #define OS_POOL_MEM_OVERHEAD   OSA_POOL_MEM_OVERHEAD

 #define OS_FIXED               OSA_FIXED
 #define OS_VARIABLE            OSA_VARIABLE
 #define OS_FIFO                OSA_FIFO
 #define OS_PRIORITY            OSA_PRIORITY

#ifdef CODE_COVERAGE
void codeCoverageTag(unsigned int, unsigned char * );
    #define CODE_COVERAGE_TAG    codeCoverageTag(__LINE__, __FILE__);
#else
    #define CODE_COVERAGE_TAG
#endif

/*========================================================================
 *  OSA Return Error Codes
 *========================================================================*/

enum
{
	OS_SUCCESS = 0,         /* 0x0 -no errors                                        */
	OS_FAIL,                /* 0x1 -operation failed code                            */
	OS_TIMEOUT,             /* 0x2 -Timed out waiting for a resource                 */
	OS_NO_RESOURCES,        /* 0x3 -Internal OS resources expired                    */
	OS_INVALID_POINTER,     /* 0x4 -0 or out of range pointer value                  */
	OS_INVALID_REF,         /* 0x5 -invalid reference                                */
	OS_INVALID_DELETE,      /* 0x6 -deleting an unterminated task                    */
	OS_INVALID_PTR,         /* 0x7 -invalid memory pointer                           */
	OS_INVALID_MEMORY,      /* 0x8 -invalid memory pointer                           */
	OS_INVALID_SIZE,        /* 0x9 -out of range size argument                       */
	OS_INVALID_MODE,        /* 0xA, 10 -invalid mode                                 */
	OS_INVALID_PRIORITY,    /* 0xB, 11 -out of range task priority                   */
	OS_UNAVAILABLE,         /* 0xC, 12 -Service requested was unavailable or in use  */
	OS_POOL_EMPTY,          /* 0xD, 13 -no resources in resource pool                */
	OS_QUEUE_FULL,          /* 0xE, 14 -attempt to send to full messaging queue      */
	OS_QUEUE_EMPTY,         /* 0xF, 15 -no messages on the queue                     */
	OS_NO_MEMORY,           /* 0x10, 16 -no memory left                              */
	OS_DELETED,             /* 0x11, 17 -service was deleted                         */
	OS_SEM_DELETED,         /* 0x12, 18 -semaphore was deleted                       */
	OS_MUTEX_DELETED,       /* 0x13, 19 -mutex was deleted                           */
	OS_MSGQ_DELETED,        /* 0x14, 20 -msg Q was deleted                           */
	OS_MBOX_DELETED,        /* 0x15, 21 -mailbox Q was deleted                       */
	OS_FLAG_DELETED,        /* 0x16, 22 -flag was deleted                            */
	OS_INVALID_VECTOR,      /* 0x17, 23 -interrupt vector is invalid                 */
	OS_NO_TASKS,            /* 0x18, 24 -exceeded max # of tasks in the system       */
	OS_NO_FLAGS,            /* 0x19, 25 -exceeded max # of flags in the system       */
	OS_NO_SEMAPHORES,       /* 0x1A, 26 -exceeded max # of semaphores in the system  */
	OS_NO_MUTEXES,          /* 0x1B, 27 -exceeded max # of mutexes in the system     */
	OS_NO_QUEUES,           /* 0x1C, 28 -exceeded max # of msg queues in the system  */
	OS_NO_MBOXES,           /* 0x1D, 29 -exceeded max # of mbox queues in the system */
	OS_NO_TIMERS,           /* 0x1E, 30 -exceeded max # of timers in the system      */
	OS_NO_MEM_POOLS,        /* 0x1F, 31 -exceeded max # of mem pools in the system   */
	OS_NO_INTERRUPTS,       /* 0x20, 32 -exceeded max # of isr's in the system       */
	OS_FLAG_NOT_PRESENT,    /* 0x21, 33 -requested flag combination not present      */
	OS_UNSUPPORTED,         /* 0x22, 34 -service is not supported by the OS          */
	OS_NO_MEM_CELLS,        /* 0x23, 35 -no global memory cells                      */
	OS_DUPLICATE_NAME,      /* 0x24, 36 -duplicate global memory cell name           */
	OS_INVALID_PARM         /* 0x25, 37 -invalid parameter                           */
};


/*****************************************************************************
 * OSA Data Types
 ****************************************************************************/

typedef void*   OSATaskRef;
typedef void*   OSASemaRef;
typedef void*   OSAMutexRef;
typedef void*   OSAMsgQRef;
typedef void*   OSAMailboxQRef;
typedef void*   OSAPoolRef;
typedef void*   OSATimerRef;
typedef void*   OSAFlagRef;
typedef UINT8 OSA_STATUS;
/* Remain for backwards compatibility */
typedef void*   OSTaskRef;
typedef void*   OSSemaRef;
typedef void*   OSMutexRef;
typedef void*   OSMsgQRef;
typedef void*   OSMailboxQRef;
typedef void*   OSPoolRef;
typedef void*   OSTimerRef;
typedef void*   OSFlagRef;
typedef UINT8 OS_STATUS;

/*****************************************************************************
 * OSA Function Prototypes
 ****************************************************************************/

/*========================================================================
 *  OSA Initialization:
 *
 *  Initializes OSA internal structures, tables, and OS specific services.
 *
 *========================================================================*/
IMPC extern OSA_STATUS OSAInitialize(void);

/*========================================================================
 *  OSA Task Management:
 *========================================================================*/
IMPC extern OSA_STATUS OSATaskCreate(
	OSATaskRef * taskRef,           /* OS task reference                       */
	void*       stackPtr,           /* pointer to start of task stack area     */
	UINT32 stackSize,               /* number of bytes in task stack area      */
	UINT8 priority,                 /* task priority 0 - 31                    */
	CHAR        * taskName,         /* task name                               */
	void (*taskStart)(void*),       /* pointer to task entry point        */
	void*       argv                /* task entry argument pointer             */
	);

IMPC extern OSA_STATUS OSATaskDelete( OSATaskRef taskRef );
IMPC extern OSA_STATUS OSATaskSuspend( OSATaskRef taskRef );
IMPC extern OSA_STATUS OSATaskResume( OSATaskRef taskRef );
IMPC extern void OSATaskSleep( UINT32 ticks ); /* time to sleep in ticks        */

IMPC extern OSA_STATUS OSATaskChangePriority(
	OSATaskRef taskRef,             /* OS task reference                       */
	UINT8 newPriority,              /* new task priority                       */
	UINT8       *oldPriority        /* old task priority                       */
	);

IMPC extern OSA_STATUS OSATaskGetPriority(
	OSATaskRef taskRef,     /* OS task reference                       */
	UINT8       *priority   /* task priority                           */
	);

IMPC extern void OSATaskYield( void );                          /* Allow other ready tasks to run          */
IMPC OSA_STATUS OSATaskGetCurrentRef( OSATaskRef *taskRef );    /* Get current running task ref  */

IMPC extern void OSARun( void );

/*========================================================================
 *  OSA Message Passing
 *========================================================================*/
IMPC extern OSA_STATUS OSAMsgQCreate(
	OSMsgQRef   * msgQRef,  /* OS message queue reference              */
#ifdef OSA_QUEUE_NAMES
	char        *queueName, /* name of message queue                   */
#endif
	UINT32 maxSize,         /* max message size the queue supports     */
	UINT32 maxNumber,       /* max # of messages in the queue          */
	UINT8 waitingMode       /* mode of tasks waiting OS_FIFO, OS_PRIORITY */
	);

IMPC extern OSA_STATUS OSAMsgQCreateWithMem(
	OSMsgQRef   * msgQRef,  /* OS message queue reference              */
#ifdef OSA_QUEUE_NAMES
	char        *queueName, /* name of message queue                   */
#endif
	UINT32 maxSize,         /* max message size the queue supports     */
	UINT32 maxNumber,       /* max # of messages in the queue          */
	void        *qAddr,     /* start address of queue memory           */
	UINT8 waitingMode       /* mode of tasks waiting OS_FIFO, OS_PRIORITY */
	);

IMPC extern OSA_STATUS OSAMsgQDelete( OSMsgQRef msgQRef );

IMPC extern OSA_STATUS OSAMsgQSend(
	OSMsgQRef msgQRef,      /* message queue reference                 */
	UINT32 size,            /* size of the message                     */
	UINT8       *msgPtr,    /* start address of the data to be sent    */
	UINT32 timeout          /* OS_SUSPEND, OS_NO_SUSPEND, or timeout   */
	);

IMPC extern OSA_STATUS OSAMsgQRecv(
	OSMsgQRef msgQRef,      /* message queue reference                 */
	UINT8       *recvMsg,   /* pointer to the message received         */
	UINT32 size,            /* size of the message                     */
	UINT32 timeout          /* OS_SUSPEND, OS_NO_SUSPEND, or timeout   */
	);

IMPC extern OSA_STATUS OSAMsgQPoll(
	OSMsgQRef msgQRef,      /* message queue reference                 */
	UINT32      *msgCount   /* number of messages on the queue         */
	);

/*========================================================================
 *  OSA Mailboxes
 *========================================================================*/
IMPC extern OSA_STATUS OSAMailboxQCreate(
	OSMailboxQRef   * mboxQRef,     /* OS mailbox queue reference              */
#ifdef OSA_QUEUE_NAMES
	char            *queueName,     /* name of mailbox queue                   */
#endif
	UINT32 maxNumber,               /* max # of messages in the queue          */
	UINT8 waitingMode               /* mode of tasks waiting OS_FIFO, OS_PRIORITY */
	);

IMPC extern OSA_STATUS OSAMailboxQDelete( OSMailboxQRef mboxRef );

IMPC extern OSA_STATUS OSAMailboxQSend(
	OSMailboxQRef mboxQRef,         /* message queue reference                 */
	void            *msgPtr,        /* start address of the data to be sent    */
	UINT32 timeout                  /* OS_SUSPEND, OS_NO_SUSPEND               */
	);

IMPC extern OSA_STATUS OSAMailboxQRecv(
	OSMailboxQRef mboxQRef,         /* message queue reference                 */
	void            **recvMsg,      /* pointer to the message received         */
	UINT32 timeout                  /* OS_SUSPEND, OS_NO_SUSPEND, or timeout   */
	);

IMPC extern OSA_STATUS OSAMailboxQPoll(
	OSMailboxQRef mboxQRef,         /* message queue reference                 */
	UINT32          *msgCount       /* number of messages on the queue         */
	);


/*========================================================================
 *  OSA Event Management:
 *========================================================================*/
IMPC extern OSA_STATUS OSAFlagCreate(
	OSFlagRef   *flagRef   /* OS reference to the flag                */
	);

IMPC extern OSA_STATUS OSAFlagCreateGlobal(
	OSFlagRef   *flagRef,   /* OS reference to the flag                */
	char        *flagName   /* name of the event flag                  */
	);

IMPC extern OSA_STATUS OSAFlagDelete( OSFlagRef flagRef );

IMPC extern OSA_STATUS OSAFlagSet(
	OSFlagRef flagRef,      /* OS reference to the flag                */
	UINT32 mask,            /* flag mask                               */
	UINT32 operation        /* OSA_FLAG_AND, OSA_FLAG_OR               */
	);

IMPC extern OSA_STATUS OSAFlagWait(
	OSFlagRef flagRef,      /* OS reference to the flag                */
	UINT32 mask,            /* flag mask to wait for                   */
	UINT32 operation,       /* OSA_FLAG_AND, OSA_FLAG_AND_CLEAR,       */
				/* OSA_FLAG_OR, OSA_FLAG_OR_CLEAR          */
	UINT32      *flags,     /* Current value of all flags              */
	UINT32 timeout          /* OS_SUSPEND, OS_NO_SUSPEND, or timeout   */
	);

IMPC extern OSA_STATUS OSAFlagPeek(
	OSFlagRef flagRef,      /* OS reference to the flag                */
	UINT32      *flags      /* returned current value of the flag      */
	);


/*========================================================================
 *  OSA Timer Management:
 *========================================================================*/
/* Timer status information structure */
typedef struct
{
	UINT32 status;          /* timer status OS_ENABLED, OS_DISABLED    */
	UINT32 expirations;     /* number of expirations for cyclic timers */
} OSATimerStatus;

IMPC extern OSA_STATUS OSATimerStart(
	OSTimerRef timerRef,                    /* OS supplied timer reference             */
	UINT32 initialTime,                     /* initial expiration time in ms           */
	UINT32 rescheduleTime,                  /* timer period after initial expiration   */
	void (*callBackRoutine)(UINT32),        /* timer call-back routine     */
	UINT32 timerArgc                        /* argument to be passed to call-back on expiration */
	);

IMPC extern OSA_STATUS OSATimerCreate( OSTimerRef* timerRef );
IMPC extern OSA_STATUS OSATimerDelete( OSTimerRef timerRef );
IMPC extern OSA_STATUS OSATimerStop( OSTimerRef timerRef );

IMPC extern OSA_STATUS OSATimerGetStatus(
	OSTimerRef timerRef,            /* timer reference                     */
	OSATimerStatus   *timerStatus   /* timer status information            */
	);


/*========================================================================
 *  OSA Semaphore Management
 *========================================================================*/
IMPC extern OSA_STATUS OSASemaphoreCreate(
	OSSemaRef *semaRef,     /* OS semaphore reference                  */
	UINT32 initialCount,    /* initial count of the semaphore          */
	UINT8 waitingMode       /* mode of tasks waiting OS_FIFO, OS_PRIORITY */
	);

IMPC extern OSA_STATUS OSASemaphoreCreateGlobal(
	OSSemaRef   *semaRef,   /* OS semaphore reference                     */
	char        *semaName,  /* name of sema to create                     */
	UINT32 initialCount,    /* initial count of the semaphore             */
	UINT8 waitingMode       /* mode of tasks waiting OS_FIFO, OS_PRIORITY */
	);

IMPC extern OSA_STATUS OSASemaphoreDelete( OSSemaRef semaRef );

IMPC extern OSA_STATUS OSASemaphoreAcquire(
	OSSemaRef semaRef,      /* OS semaphore reference                   */
	UINT32 timeout          /* OS_SUSPEND, OS_NO_SUSPEND, or timeout    */
	);

IMPC extern OSA_STATUS OSASemaphoreRelease( OSSemaRef semaRef );

IMPC extern OSA_STATUS OSASemaphorePoll(
	OSSemaRef semaRef,      /* OS semaphore reference                   */
	UINT32    *count        /* Current semaphore count                  */
	);

/*========================================================================
 *  OSA Mutex Management
 *========================================================================*/
IMPC extern OSA_STATUS OSAMutexCreate(
	OSMutexRef  *mutexRef,          /* OS mutex reference                         */
	UINT8 waitingMode               /* mode of tasks waiting OS_FIFO, OS_PRIORITY */
	);

IMPC extern OSA_STATUS OSAMutexDelete( OSSemaRef semaRef );

IMPC extern OSA_STATUS OSAMutexLock(
	OSMutexRef mutexRef,    /* OS mutex reference                       */
	UINT32 timeout          /* OS_SUSPEND, OS_NO_SUSPEND, or timeout    */
	);

IMPC extern OSA_STATUS OSAMutexUnlock( OSMutexRef mutexRef );


/*========================================================================
 *  OSA Interrupt Control
 *========================================================================*/
IMPC extern OSA_STATUS OSAIsrCreate(
	UINT32 isrNum,                  /* interrupt to attach routine to          */
	void (*fisrRoutine)(UINT32), /* first level ISR routine to be called    */
	void (*sisrRoutine)(void)       /* second level ISR routine to be called   */
	);

IMPC extern OSA_STATUS OSAIsrDelete( UINT32 isrNum );
IMPC extern OSA_STATUS OSAIsrNotify( UINT32 isrNum );

extern UINT32 OSAIsrEnable( UINT32 mask );
extern UINT32 OSAIsrDisable( UINT32 mask );

IMPC extern OSA_STATUS OSAContextLock( void );
IMPC extern OSA_STATUS OSAContextUnlock( void );

/*========================================================================
 *  OSA Sys Context info
 *========================================================================*/
/* Previos Context information structure */
typedef struct
{
	UINT32 prev_task_context;
	UINT32 prev_interrupt_context;
} OSASysContext;


IMPC extern OSA_STATUS OSASysContextLock(OSASysContext *prevContext);
IMPC extern OSA_STATUS OSASysContextUnlock(void);
IMPC extern OSA_STATUS OSASysContextRestore(OSASysContext *prevContext);

/*===========================================================================
 *  OSA Real-Time Access:
 *=========================================================================*/
extern UINT32 OSAGetTicks( void );
IMPC extern UINT32 OSAGetClockRate( void );
extern void   OSATick( void );

IMPC extern OSA_STATUS OSAGetSystemTime(
	UINT32 *secsPtr,
	UINT16 *milliSecsPtr
	);

/*========================================================================
 *  OSA Memory Heap Access
 *
 *  Allocating Memory -
 *
 *  Deallocating Memory -
 *
 *========================================================================*/
IMPC extern OSA_STATUS OSAMemPoolCreate(
	OSPoolRef *poolRef,     /* OS assigned reference to the pool      */
	UINT32 poolType,        /* OSA_FIXED or OS_VARIABLE                */
	UINT8*    poolBase,     /* pointer to start of pool memory        */
	UINT32 poolSize,        /* number of bytes in the memory pool     */
	UINT32 partitionSize,   /* size of partitions in fixed pools      */
	UINT8 waitingMode       /* mode of tasks waiting OS_FIFO, OS_PRIORITY */
	);

IMPC extern OSA_STATUS OSAMemPoolDelete( OSPoolRef poolRef );

IMPC extern OSA_STATUS OSAMemPoolAlloc(
	OSPoolRef poolRef,      /* OS reference to the pool to be used    */
	UINT32 size,            /* number of bytes to be allocated        */
	void**    mem,          /* pointer to start of allocated memory   */
	UINT32 timeout          /* OS_SUSPEND, OS_NO_SUSPEND, or timeout  */
	);

IMPC extern OSA_STATUS OSAMemPoolFree(
	OSPoolRef poolRef,                                      /* OS reference to the pool to be used    */
	void*     mem                                           /* pointer to start of memory to be freed */
	);
IMPC extern OSA_STATUS OSAMemPoolFixedFree( void *mem );        /* Ptr to fixed pool memory */

IMPC extern OSA_STATUS OSAMemPoolCreateGlobal(
	OSAPoolRef *poolRef,    /* OS reference to the partition pool to be used    */
	char    *poolName,      /* name of global mem pool                */
	UINT8*  poolBase,       /* pointer to start of pool memory        */
	UINT32 poolSize,        /* size of the pool                       */
	UINT32 partitionSize    /* size of partitions in the pool         */
	);

IMPC extern OSA_STATUS OSATaskSetTls(void *arg);        /* set up the thread local storage (TLS) to the pointer arg*/
IMPC extern OSA_STATUS OSATaskGetTls(void **arg);       /* get the TLS pointer and assign it to arg */

#ifdef OSA_SYMBIAN_USER_MODE
/*========================================================================
 *  OSA Global Memory
 *========================================================================*/
IMPC extern OSA_STATUS OSAGlobalAlloc(
	const char* memName,    /* name of global memory chunk            */
	UINT32 memSize          /* number of bytes to be allocated        */
	);

IMPC extern OSA_STATUS OSAGlobalRead(
	const char* memName,    /* name of global memory chunk            */
	UINT32 pos,             /* offset from where data is to be read   */
	UINT32 length,          /* number of bytes to be read             */
	UINT8* data             /* location to store data that is read    */
	);

IMPC extern OSA_STATUS OSAGlobalWrite(
	const char* memName,    /* name of global memory chunk            */
	UINT32 pos,             /* offset to where data is to be written  */
	UINT32 length,          /* number of bytes to write               */
	const UINT8* data       /* data to write                          */
	);
#endif

/*****************************************************************************
 * OSA Internal Assert macro definition
 ****************************************************************************/
#ifndef OSA_SYMBIAN_USER_MODE
#ifdef OSA_USE_ASSERT

    #ifdef OSA_TRACEON

void OSA_AssertFail(const IsotelEngineID engineId, const UINT8 moduleId,
		    char *file, unsigned line);       /* Assert Fail function */
      #define OSA_ASSERT(f)   if ( !(f) ) OSA_AssertFail(MYENGINEID, MyNameID, __FILE__, __LINE__);

    #else

/* Tracing is disabled so no need for extra trace information */
void OSA_AssertFail(char *file, unsigned line);           /* Assert Fail function */
      #define OSA_ASSERT(f)   if ( !(f) ) OSA_AssertFail(__FILE__, __LINE__);

    #endif

#else

  #define OSA_ASSERT(f)
#endif
#endif

#ifdef OSA_RTT                                                  /* Assert for RTT unsupported code */

void OSA_AssertFail(char *file, unsigned line);                 /* Assert Fail function */
      #define OSA_FULL_ASSERT(f)   if ( !(f) ) OSA_AssertFail(__FILE__, __LINE__);

#endif

#define OSA_GET_MEM_POOL_STATS(memStats, block)				\
	{									\
		OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), TRUE); \
		OSASemaphoreAcquire(block->stats.msgQStatsSema4, OS_SUSPEND);	    \
		*memStats = block->stats.sysMsgQMemRequired;			    \
		OSASemaphoreRelease(block->stats.msgQStatsSema4);		    \
	}

/* Remap old function names to new ones to remain backwards compatibility with
 * old function calls.
 */
#define OSInitialize            OSAInitialize
#define OSRun                   OSARun
#define OSTaskCreate            OSATaskCreate
#define OSTaskDelete            OSATaskDelete
#define OSTaskPriority          OSATaskChangePriority
#define OSTaskGetPriority       OSATaskGetPriority
#define OSTaskSuspend           OSATaskSuspend
#define OSTaskResume            OSATaskResume
#define OSTaskSleep             OSATaskSleep
#define OSTaskYield             OSATaskYield
#define OSFlagCreate            OSAFlagCreate
#define OSFlagDelete            OSAFlagDelete
#define OSFlagSet               OSAFlagSet
#define OSFlagWait              OSAFlagWait
#define OSFlagPeek              OSAFlagPeek
#define OSTimerCreate           OSATimerCreate
#define OSTimerDelete           OSATimerDelete
#define OSTimerStart            OSATimerStart
#define OSTimerStop             OSATimerStop
#define OSTimerGetStatus        OSATimerGetStatus
#define OSMsgQCreate            OSAMsgQCreate
#define OSMsgQDelete            OSAMsgQDelete
#define OSMsgQSend              OSAMsgQSend
#define OSMsgQRecv              OSAMsgQRecv
#define OSMsgQPoll              OSAMsgQPoll
#define OSMailboxQCreate        OSAMailboxQCreate
#define OSMailboxQDelete        OSAMailboxQDelete
#define OSMailboxQSend          OSAMailboxQSend
#define OSMailboxQRecv          OSAMailboxQRecv
#define OSMailboxQPoll          OSAMailboxQPoll
#define OSSemaphoreCreate       OSASemaphoreCreate
#define OSSemaphoreDelete       OSASemaphoreDelete
#define OSSemaphoreAcquire      OSASemaphoreAcquire
#define OSSemaphoreRelease      OSASemaphoreRelease
#define OSSemaphorePoll         OSASemaphorePoll
#define OSMutexCreate           OSAMutexCreate
#define OSMutexDelete           OSAMutexDelete
#define OSMutexLock             OSAMutexLock
#define OSMutexUnlock           OSAMutexUnlock
#define OSIsrCreate             OSAIsrCreate
#define OSIsrDelete             OSAIsrDelete
#define OSIsrEnable             OSAIsrEnable
#define OSIsrDisable            OSAIsrDisable
#define OSIsrNotify             OSAIsrNotify
#define OSGetTicks              OSAGetTicks
#define OSGetClockRate          OSAGetClockRate
#define OSTick                  OSATick
#define OSMemPoolCreate         OSAMemPoolCreate
#define OSMemPoolDelete         OSAMemPoolDelete
#define OSMemPoolAlloc          OSAMemPoolAlloc
#define OSMemPoolFree           OSAMemPoolFree
#define OSContextLock           OSAContextLock
#define OSContextUnlock         OSAContextUnlock

#ifdef __cplusplus
}
#endif
#endif /* _OSA_H */

