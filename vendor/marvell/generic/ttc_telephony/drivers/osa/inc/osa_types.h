/* ===========================================================================
   File        : osa_types.h
   Description : Data types file for the os/osa package

   Notes       :

   Copyright (c) 2001 Intel CCD. All Rights Reserved
   =========================================================================== */

#if !defined(_OSA_TYPES_H_)
#define _OSA_TYPES_H_

#include "osa_config.h" /* needed to be included first since OSA_INTERRUPTS is defined here*/

#ifdef OSA_SYMBIAN_USER_MODE
 #include "osa_symbian_user.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Use RTT specific stuff */
#ifdef OSA_RTT
 #include "osa_rtt.h"
#endif

/* Use Nucleus specific stuff */
#ifdef OSA_NUCLEUS
 #include "osa_nucleus.h"
#endif

/* Use WinCE specific stuff */
#ifdef OSA_WINCE
 #include "osa_wince.h"
#endif

/* Use Symbian specific stuff */
#ifdef OSA_SYMBIAN
 #include "osa_symbian.h"
#endif

/* Use POSIX specific stuff */
#ifdef OSA_LINUX
 #include "osa_linux.h"
#endif

/* Use Win32 specific stuff */
#ifdef OSA_WIN32
 #include "osa_win32.h"
#endif


/* For Symbian, we include these definitions elsewhere */
#ifndef OSA_SYMBIAN_USER_MODE

 #define  OSA_ISRS_MAX                  32  /* Max number of ISR's (fixed)     */
 #define  OSA_MAX_MBOX_DATA_SIZE        4
 #define  OSA_MEMORY_ALIGN              (0x3)
 #define  OSA_VAR_MEM_OVERHEAD          4  /* Variable queue memory overhead per message */
 #define  OSA_TASK_DELETED              0
 #define  OSA_TASK_RUNNING              1
 #define  OSA_TASK_SUSPENDED            2

 #define  OSA_EVENT_HISR_MAX_ENTRIES    256
 #define  OSA_EVENT_HISR_STACK_SIZE     (OSA_MIN_STACK_SIZE)
 #define  OSA_EVENT_HISR_PRIORITY       0
 #define  OSA_NAME_SIZE                 8
 #define  OSA_MAX_ATTACHED_PROCESSES    8

/*=======================================================================
*  OS Memory Pool Structure
*======================================================================*/
#if (OSA_MEM_POOLS)
typedef struct _OS_Mem
{
	union
	{
		OS_MemPool_t vPoolRef;          /* Variable pool ref */
		OS_PartitionPool_t fPoolRef;    /* Fixed pool ref */
	}u;
	UINT32 type;                            /* Pool type OSA_FIXED/OSA_VARIABLE */
	void            *refCheck;
	UINT8           *poolBase;
	UINT32 poolSize;
	UINT32 partitionSize;
	struct _OS_Mem  *next;
	struct _OS_Mem  *prev;
} OS_Mem;

typedef struct
{
	UINT32 numCreated;                              /* # created memory pools */
	OS_Sema_t poolSemaphore;                        /* pool semaphore */
	OS_Mem memPools[OSA_MEM_POOLS_MAX];             /* Memory pools */
	OS_Mem          *head;                          /* Active list */
	OS_Mem          *free;                          /* Free list */
} OS_MemPool;
#endif

/*=======================================================================
 *  OS Task Structures
 *=====================================================================*/
#if (OSA_TASKS)
typedef struct _OS_Task
{
	OS_Task_t os_ref;
	void              *refCheck;
	struct _OS_Task*  next;
	struct _OS_Task*  prev;
	UINT8 state;
	char name[OSA_NAME_SIZE];               /* Service name */
	void              *tlsPtr;              /* pointer to the thread local storage (TLS) */
	UINT8 padding[3];
} OS_Task;

typedef struct
{
	UINT32 numCreated;
	UINT32 totalNumCreated;
	OS_Sema_t poolSemaphore;
#ifndef OSA_USE_DYNAMIC_REFS
	OS_Task task[OSA_TASKS_MAX];
#endif
	OS_Task         *free;                  /* Free list */
	OS_Task         *head;                  /* Active list */
} OS_TaskPool;
#endif

/*=======================================================================
*  Event Flag Structures
*======================================================================*/
#if (OSA_FLAGS)
typedef struct _OS_Flag
{
	OS_Flag_t os_ref;
	void            *refCheck;
	UINT32 eventMask;
	struct _OS_Flag *next;
	struct _OS_Flag *prev;
} OS_Flag;

typedef struct
{
	UINT32 numCreated;
	OS_Sema_t poolSemaphore;
	UINT32 counter;
#ifndef OSA_USE_DYNAMIC_REFS
	OS_Flag eventGroups[OSA_FLAGS_MAX];
#endif
	OS_Flag         *head;                  /* Active queue list */
	OS_Flag         *free;                  /* Free queue list */
#ifndef OSA_WINCE
	OS_Hisr_t os_HISR;
	UINT32 getIndex;
	UINT8 stack[OSA_EVENT_HISR_STACK_SIZE];
	UINT32 count;
#endif
} OS_FlagPool;
#endif

/*========================================================================
 *  OS Message Structures
 *========================================================================*/
#if (OSA_MSG_QUEUES)
typedef struct _OS_MsgQueue
{
	OS_MsgQ_t os_ref;                     /* OS queue reference */
	void                *refCheck;
	struct _OS_MsgQueue *next;
	struct _OS_MsgQueue *prev;
	UINT32 msgCount;
#ifdef OSA_DEBUG
	UINT32 maxNumber;
	UINT32 maxMsgSize;
	UINT32 totalMsgReceived;              /* used for statistics */
	UINT32 totalBytesReceived;
	UINT32 maxMsgInQueue;
	struct _OSA_ControlBlock    *controlBlock;
	OS_Sema_t           *poolSema;
#endif
} OS_MsgQueue;

typedef struct
{
	UINT32 numCreated;
	OS_Sema_t poolSemaphore;
#ifndef OSA_USE_DYNAMIC_REFS
	OS_MsgQueue queues[OSA_MSG_QUEUES_MAX];
#endif
	OS_MsgQueue     *head;                  /* Active queue list */
	OS_MsgQueue     *free;                  /* Free queue list */
} OS_MsgQPool;
#endif

/*=======================================================================
 *  OS Mailbox Structures
 *=====================================================================*/
#if (OSA_MBOX_QUEUES)
typedef struct _OS_Mbox
{
	OS_Mbox_t os_ref;
	void            *refCheck;
	struct _OS_Mbox *next;
	struct _OS_Mbox *prev;
	UINT32 msgCount;
#ifdef OSA_DEBUG
	UINT32 maxNumber;
	UINT32 totalMsgReceived;          /* used for statistics */
	UINT32 maxMsgInQueue;
	struct _OSA_ControlBlock    *controlBlock;
	OS_Sema_t       *poolSema;
#endif
} OS_Mbox;

typedef struct
{
	UINT32 numCreated;                              /* # created mailboxes */
	OS_Sema_t poolSemaphore;                        /* timer pool semaphore */
#ifndef OSA_USE_DYNAMIC_REFS
	OS_Mbox mboxes[OSA_MBOX_QUEUES_MAX];            /* Mailboxes */
#endif
	OS_Mbox         *head;                          /* Active mailbox list */
	OS_Mbox         *free;                          /* Free mailbox list */
} OS_MboxPool;
#endif

/*=======================================================================
 *  OS Timer Structures
 *=====================================================================*/
#if (OSA_TIMERS)
typedef struct _OS_Timer
{
	OS_Timer_t os_ref;
	void             *refCheck;
	UINT32 state;
	struct _OS_Timer *next;
	struct _OS_Timer *prev;
} OS_Timer;

typedef struct
{
	UINT32 numCreated;                      /* # created timers */
	OS_Sema_t poolSemaphore;                /* timer pool semaphore */
#ifndef OSA_USE_DYNAMIC_REFS
	OS_Timer timers[OSA_TIMERS_MAX];        /* Timers */
#endif
	OS_Timer        *head;                  /* Active timers list */
	OS_Timer        *free;                  /* Free timers list */
} OS_TimerPool;
#endif

/*=======================================================================
 *  Semaphore
 *=====================================================================*/
#if (OSA_SEMAPHORES)
typedef struct _OS_Sema
{
	OS_Sema_t os_ref;
	void             *refCheck;
	struct _OS_Sema  *next;
	struct _OS_Sema  *prev;
} OS_Sema;


typedef struct
{
	UINT32 numCreated;
	UINT32 counter;
	OS_Sema_t poolSemaphore;
#ifndef OSA_USE_DYNAMIC_REFS
	OS_Sema semaphores[OSA_SEMAPHORES_MAX];
#endif
	OS_Sema          *head;
	OS_Sema          *free;
} OS_SemaPool;
#endif

/*=======================================================================
 *  Mutex
 *=====================================================================*/
#if (OSA_MUTEXES)
typedef struct _OS_Mutex
{
	OS_Mutex_t os_ref;
	void              *refCheck;
	OS_Task_t         *owner;
	struct _OS_Mutex  *next;
	struct _OS_Mutex  *prev;
	UINT8 basePriority;             /* Orig priority for priority inheritance */
	UINT8 currentPriority;          /* Highest priority of waiting tasks      */
	UINT8 highestWaiting;           /* Max priority of tasks waiting          */
	UINT8 numTasks;                 /* # tasks in mutex Q including owner     */
} OS_Mutex;


typedef struct
{
	UINT32 numCreated;
	UINT32 counter;
	OS_Sema_t poolSemaphore;
#ifndef OSA_USE_DYNAMIC_REFS
	OS_Mutex mutexes[OSA_MUTEXES_MAX];
#endif
	OS_Mutex        *head;
	OS_Mutex        *free;
} OS_MutexPool;
#endif

#if (OSA_INTERRUPTS)
/*=======================================================================
*  Interrupts
*======================================================================*/
typedef struct _OS_Isr
{
	OS_Hisr_t os_ref;
	void            *refCheck;
	UINT32 eventMask;
	UINT32 isrNum;
} OS_Isr;

typedef struct
{
	UINT32 numCreated;
#ifndef OSA_USE_DYNAMIC_REFS
	OS_Isr isrs[OSA_INTERRUPTS];
	UINT8 stack[OSA_SISR_STACK_SIZE * OSA_INTERRUPTS];
#endif
} OS_IsrPool;
#endif

/*=======================================================================
*  OS Statistics
*======================================================================*/

typedef struct
{
	UINT32 sysMsgCount;                             /* System msg counter */
	UINT32 sysMaxMsgAllowed;                        /* Max msgs in system */
	UINT32 sysTotalMsgReceived;                     /* Sys tot msg received */
	UINT32 sysMaxMsgs;                              /* Max # msgs in sys queue */
	UINT32 sysMboxCount;
	UINT32 sysMaxMboxAllowed;
	UINT32 sysTotalMboxReceived;
	UINT32 sysMaxMboxMsgs;
	UINT32 sysMsgQMemRequired;
	OSSemaRef msgQStatsSema4;

} OS_SystemStats;

/*=======================================================================
*  OS Process Control
*======================================================================*/

typedef struct
{
	OS_Proc_t pid;
	UINT32 numAttached;
} OS_ProcId;

typedef struct
{
	UINT32 numProcAttached;
	OS_Sema_t sema;
	OS_ProcId proc[OSA_MAX_ATTACHED_PROCESSES];
} OS_ProcessCntrl;


/*=======================================================================
*  OS Control Block
*======================================================================*/

typedef struct _OSA_ControlBlock
{
	OS_Sema_t csOSAContextLock;                     //Tianbo: global context lock for user
	OS_Sema_t csOSAContextLockInternal;             // For internal OSA usage
	OS_Sema_t OSA_TickSemaphore;
	UINT32 OSA_RtcCounter;
	OS_Task_t OSATimerTask;
	OS_Task_t OSATickTask;
	void                 *globalChunk;
	OS_ProcessCntrl attach;
#if (OSA_MSG_QUEUES)
	OS_MsgQPool msgQPool;
#endif
#if (OSA_MBOX_QUEUES)
	OS_MboxPool mboxPool;
#endif
#if (OSA_TASKS)
	OS_TaskPool taskPool;
#endif
#if (OSA_TIMERS)
	OS_TimerPool timerPool;
#endif
#if (OSA_SEMAPHORES)
	OS_SemaPool semaPool;
#endif
#if (OSA_MUTEXES)
	OS_MutexPool mutexPool;
#endif
#if (OSA_FLAGS)
	OS_FlagPool eventPool;
#endif
#if (OSA_INTERRUPTS)
	OS_IsrPool isrPool;
#endif
#ifdef OSA_DEBUG
	OS_SystemStats stats;
#endif
#if (OSA_MBOX_QUEUES && OSA_MEM_POOLS)
	OS_MemPool_t mboxPoolMem;
#endif
#if (OSA_MSG_QUEUES && OSA_MEM_POOLS)
	OS_MemPool_t heapPool;
#endif
#if (OSA_MEM_POOLS)
	OS_MemPool memPool;
#endif

} OSA_ControlBlock;


/*=======================================================================
*  Macros
*======================================================================*/

/* Checks reference for NULL and validates with current stored value */
/*   ref   - api reference tested for NULL                           */
/*   osRef - tested for valid stored pointer to internal reference   */
#ifndef OSA_DEBUG
#define OSA_REF_CHECK(ref, osRef) do{ }while(0)
#else
#define OSA_REF_CHECK(ref, osRef) \
	{ \
		if (ref == NULL)			\
			return (OS_INVALID_REF);				    \
		if (osRef->refCheck != &osRef->os_ref)						    \
			return (OS_INVALID_REF);				    \
	}
#endif

/* Linked List management macros. Structure must have *next & *prev. */

/* Add the item to the front of the list */
#define OSA_ADD_TO_ACTIVE_LIST(head, item)  { \
		item->prev = OS_NULL; \
		if (head == OS_NULL) { \
			head = item; \
			item->next = OS_NULL; }	\
		else { \
			head->prev = item; \
			item->next = head; \
			head = item; } }

/* Delete the item from the active list */
#define OSA_DELETE_FROM_ACTIVE_LIST(head, item) { \
		if (item->prev != OS_NULL) { \
			(item->prev)->next = item->next; } \
		else { \
			head = item->next; } \
		if (item->next != OS_NULL) { \
			(item->next)->prev = item->prev; } \
		item->prev = OS_NULL; \
		item->next = OS_NULL; }

/* Get the first item from the free list */
#define OSA_GET_FROM_FREE_LIST(head, item) { \
		if (head != OS_NULL) { \
			item = head; \
			head = head->next; \
			item->next = OS_NULL; }	\
		else { \
			item = OS_NULL; } }

/* Add to the front of the free list */
#define OSA_ADD_TO_FREE_LIST(head, item) { \
		item->next = OS_NULL; \
		item->next = head; \
		head = item; }

#endif /* OSA_SYMBIAN_USER_MODE */

#ifdef __cplusplus
}
#endif

#endif /* _OSA_TYPES_H_ */


/*                      end of osa_types.h
   --------------------------------------------------------------------------- */




