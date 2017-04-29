/* ===========================================================================
   File        : osa.c
   Description : Implementation of OSA wrapper API to be used with the
   following operating systems:

   - ATI Nucleus
   - WinCE 4.2 (Smartphone/PocketPC)
   - Symbian EPOC
   - Real Time Threads for Linux from UBC
   - POSIX for Linux MontaVista Kernel v2.6

   The OS API defines a set of interfaces that provide the following
   basic operating system services:

   OS Management     - OSAInitialize(), OSARun()
   Tasks             - OSATaskCreate(), OSATaskDelete(), OSATaskChangePriority(),
   - OSATaskGetPriority(), OSATaskSuspend(), OSATaskResume(),
   - OSATaskSleep(), OSATaskGetCurrentRef()
   Event Flags       - OSAFlagCreate(), OSAFlagDelete(), OSAFlagSet(),
   - OSAFlagWait(), OSAFlagPeek()
   Timers            - OSATimerCreate(), OSATimerDelete(), OSATimerStart(),
   - OSATimerStop(), OSATimerGetStatus()
   Messaging         - OSAMsgQCreate(), OSAMsgQDelete(), OSAMsgQCreateWithMem()
   - OSAMsgQSend(), OSAMsgQRecv(), OSAMsgQPoll()
   Mailboxes         - OSAMailboxQCreate(), OSAMailboxQDelete(),
   - OSAMailboxQSend(), OSAMailboxQRecv(), OSAMailboxQPoll()
   Semaphores        - OSASemaphoreCreate(), OSASemaphoreDelete(),
   - OSASemaphoreAcquire(), OSASemaphoreRelease(),
   - OSASemaphorePoll()
   Mutexes           - OSAMutexCreate(), OSAMutexDelete(),
   - OSAMutexLock(), OSAMutexUnlock(),
   Interrupt Control - OSAIsrCreate(), OSAIsrDelete(),
   - OSAIsrEnable(), OSAIsrDisable(), OSAIsrNotify()
   Real-Time Clock   - OSAGetTicks(), OSAGetClockRate(), OSATick(),
   - OSAGetSystemTime()
   Memory            - OSAMemPoolCreate(), OSAMemPoolDelete(),
   - OSAMemPoolAlloc(), OSAMemPoolFree(),
   - OSAMemPoolFixedFree()
   Thread local storage
   - OSATaskSetTls()
   - OSATaskGetTls()

   ---------------------------------------------------------------------
   This file is multiply reused: as osa.c and as include in the osai.c
   All OSA.C functions are separetad onto 2 service groups upon flags:
     - OSA_C_SERVICE_INIT     - code used at startup or slow code
     - OSA_C_SERVICE_RUN      - fast frequently used code
   This is usable for implementing them in separated *.o files in different memories.

   To "activate" this feuture the OSA_C_SERVICE_INIT_SEPARATE flag should be defined in a MAKEFILE.
   The default for no-flags is equal to the case
    BOTH (OSA_C_SERVICE_INIT and OSA_C_SERVICE_RUN) are implemented in the "osa.c"
   In case of separation another file OSAI.C is used with #include"osa.c" and OSA_C_SERVICE_INIT define.
   ================================================================================================*/


/************************************************************************
 * Project header files
 ***********************************************************************/
#include "osa.h"
#include "osa_types.h"


#if defined(OSA_NUCLEUS)
 #include "stdio.h"
 #include "string.h"
#endif
#if defined(OSA_NUCLEUS) && defined(OSA_STATS)
#include "diag.h"
#endif


#if defined (OSA_C_SERVICE_RUN)
#error Wrong flag or combination used
#endif
#if defined (OSA_C_SERVICE_INIT_SEPARATE)
  #if defined (OSA_C_SERVICE_INIT)
//This called from the file "osai.c" to implement INIT only
  #else
  #define OSA_C_SERVICE_RUN
  #endif
#else //OSA_C_SERVICE_INIT_SEPARATE
  #define OSA_C_SERVICE_INIT
  #define OSA_C_SERVICE_RUN
#endif //OSA_C_SERVICE_INIT_SEPARATE

#if defined (OSA_NU_SERVICE_INIT_SEPARATE) && !defined (OSA_C_SERVICE_INIT)
#define OSA_NU_SERVICE_RUN
#include "osa_nucleus.c"
#endif

/************************************************************************
 * OS resources are limited and defined once for a long period.
 * But at time of development the requirements may be changed.
 * Callers must check the OSA ret-code but not always do this.
 * Let's _EXTend the OSA_ASSERT checking with additional lines.
 * The OSA_ASSERT itself depends from the flag OSA_USE_ASSERT.
 *
 * For NUCLEUS use redefine asserts with a given utility instead of OSA-default
 ***********************************************************************/
#if defined(OSA_NUCLEUS) && defined (OSA_USE_ASSERT)
#include "utils.h"
#define USE_UTILS_ASSERT
#undef  OSA_ASSERT
#define OSA_ASSERT       ASSERT
#endif

#define OSA_ASSERT_EXT(COND)     OSA_ASSERT(COND)
#define OSA_ASSERT_EXT_TO(COND)  /*OSA_ASSERT(COND)*/

#ifdef WIN_TEST
#define ALIGN4
#elif defined(OSA_LINUX)
#define ALIGN4
#else
#define ALIGN4 __align(4)
#endif

#define UNUSEDPARAM(param) (void)param;


/************************************************************************
 * External Variables
 ***********************************************************************/
//extern INT32 OSA_SavedInterrupt;

#ifndef OSA_NU_TARGET
#define INTC_InterruptSources UINT32
UINT8 INTCBind(INTC_InterruptSources isrNum, void (*fisrRoutine)(UINT32))
{
	UNUSEDPARAM(isrNum)
	UNUSEDPARAM(fisrRoutine)
	return 0;
}
#else
#include "intc.h"
#endif

/************************************************************************
 * OS Resource Declarations
 ***********************************************************************/
extern char OSA_MessagePool[];
extern char OSA_MboxPool[];

#if defined (OSA_C_SERVICE_RUN)

OSA_ControlBlock*       OSA_ControlBlockPtr = NULL;
OSA_ControlBlock osa_ControlBlock;

#if (OSA_MSG_QUEUES)
#ifndef OSA_WINCE
char ALIGN4 OSA_MessagePool[OSA_MESSAGING_POOL_SIZE];
#endif
#endif
#endif //OSA_C_SERVICE_RUN

#if defined (OSA_C_SERVICE_INIT)
// The using MBOX to be obsolete for mmost of cases. So use "INIT" instead "RUN"

#if (OSA_MBOX_QUEUES)
#ifndef OSA_WINCE
char ALIGN4 OSA_MboxPool[OSA_MBOX_POOL_SIZE];
#endif
#endif
#endif //OSA_C_SERVICE_INIT


#if defined (OSA_C_SERVICE_INIT)
BOOL OSA_Initialized = FALSE;
#endif //OSA_C_SERVICE_INIT


#if defined (OSA_C_SERVICE_INIT)
//Debugging
#define OSA_MAX_ASSERT_FILENAME_LEN 128
static char OSA_assertFile[OSA_MAX_ASSERT_FILENAME_LEN + 1];
#if (OSA_PC_TEST || OSA_LINUX)
static UINT32 OSA_assertLine;
#endif

#endif //OSA_C_SERVICE_INIT


/*********************************************************************
 * OSB Private Methods
 ********************************************************************/
#if defined (OSA_C_SERVICE_INIT)
#if (OSA_TASKS)
static OSA_STATUS OS_TaskPoolInit(OSA_ControlBlock* block);
#endif
#if (OSA_MEM_POOLS)
static OSA_STATUS OS_MemPoolInit(OSA_ControlBlock* block);
#endif
#if (OSA_SEMAPHORES)
static OSA_STATUS OS_SemaPoolInit(OSA_ControlBlock* block);
#endif
#if (OSA_MUTEXES)
static OSA_STATUS OS_MutexPoolInit(OSA_ControlBlock* block);
#endif
#if (OSA_FLAGS)
static OSA_STATUS OS_FlagPoolInit(OSA_ControlBlock* block);
#endif
#if (OSA_TIMERS)
static OSA_STATUS OS_TimerPoolInit(OSA_ControlBlock* block);
#endif
#if (OSA_MSG_QUEUES)
static OSA_STATUS OS_MsgPoolInit(OSA_ControlBlock* block);
#endif
#if (OSA_MBOX_QUEUES)
static OSA_STATUS OS_MailboxPoolInit(OSA_ControlBlock* block);
#endif
#if (OSA_INTERRUPTS)
static void OS_IsrPoolInit(OSA_ControlBlock* block);
#endif
#endif //OSA_C_SERVICE_INIT


/*****************************************************************************
 * Debug/Trace support
 ****************************************************************************/

#if defined (OSA_C_SERVICE_INIT)
/*======================================================================
 *  OS Management:
 *
 *  Public:
 *
 *   OSAInitialize()
 *
 *====================================================================*/

/***********************************************************************
*
* Name:        OSAInitialize()
*
* Description: Initialize OS software data structues.
*
* Parameters:  None
*
* Returns:     nothing
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSAInitialize(void)
{
	OSA_STATUS rtn = OS_SUCCESS;
	OSA_ControlBlock *block = NULL;
	BOOL alreadyCreated = OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), TRUE);

#ifdef OSA_NUCLEUS
	void*       memoryPtr;
#endif

	if (!alreadyCreated)
	{
		CODE_COVERAGE_TAG;
		OSA_Initialized = TRUE;

		/* Initialize OS Specific stuff */
		OSA_INIT(block);

#if (OSA_MSG_QUEUES)
#ifdef OSA_NUCLEUS
		/* Create a memory pool for messaging */
		memoryPtr = (void *)OSA_MessagePool;
		memset(memoryPtr, 0xa5, (size_t)OSA_MESSAGING_POOL_SIZE);
		OSA_MEM_POOL_CREATE((void *)&block->heapPool, NULL, OSA_VARIABLE, \
				    memoryPtr, OSA_MESSAGING_POOL_SIZE, 4, OS_PRIORITY, rtn);

		if (rtn != OS_SUCCESS)
		{
			CODE_COVERAGE_TAG;
			ASSERT(0);
			return(OS_FAIL);
		}
#endif
#endif

#if (OSA_MBOX_QUEUES)
#ifdef OSA_NUCLEUS
		CODE_COVERAGE_TAG;
		/* Create a memory pool for mailboxes */
		memoryPtr = (void *)OSA_MboxPool;
		memset(memoryPtr, 0xa5, (size_t)OSA_MBOX_POOL_SIZE);
		OSA_MEM_POOL_CREATE((void *)&block->mboxPoolMem, NULL, OSA_VARIABLE, \
				    memoryPtr, OSA_MBOX_POOL_SIZE, 4, OS_PRIORITY, rtn);
		if (rtn != OS_SUCCESS)
		{
			CODE_COVERAGE_TAG;
			return(OS_FAIL);
		}
#endif
#endif

		/*
		 * Initialize OS components
		 */
#if (OSA_TASKS)
		if ((rtn = OS_TaskPoolInit(block)) != OS_SUCCESS)
		{
			CODE_COVERAGE_TAG;
			return(OS_FAIL);
		}
#endif
#if (OSA_SEMAPHORES)
		if ((rtn = OS_SemaPoolInit(block)) != OS_SUCCESS)
		{
			CODE_COVERAGE_TAG;
			return(OS_FAIL);
		}
#ifdef OSA_DEBUG
		CODE_COVERAGE_TAG;
		block->stats.sysMsgQMemRequired = 0;
		OSASemaphoreCreate(&block->stats.msgQStatsSema4, 0, OS_FIFO);
#endif
#endif
#if (OSA_MEM_POOLS)
		if ((rtn = OS_MemPoolInit(block)) != OS_SUCCESS)
		{
			CODE_COVERAGE_TAG;
			return(OS_FAIL);
		}
#endif
#if (OSA_FLAGS)
		if ((rtn = OS_FlagPoolInit(block)) != OS_SUCCESS)
		{
			CODE_COVERAGE_TAG;
			return(OS_FAIL);
		}
#endif
#if (OSA_TIMERS)
		if ((rtn = OS_TimerPoolInit(block)) != OS_SUCCESS)
		{
			CODE_COVERAGE_TAG;
			return(OS_FAIL);
		}
#endif
#if (OSA_MSG_QUEUES)
		if ((rtn = OS_MsgPoolInit(block)) != OS_SUCCESS)
		{
			CODE_COVERAGE_TAG;
			return(OS_FAIL);
		}
#endif
#if (OSA_MBOX_QUEUES)
		if ((rtn = OS_MailboxPoolInit(block)) != OS_SUCCESS)
		{
			CODE_COVERAGE_TAG;
			return(OS_FAIL);
		}
#endif
#if (OSA_MUTEXES)
		if ((rtn = OS_MutexPoolInit(block)) != OS_SUCCESS)
		{
			CODE_COVERAGE_TAG;
			return(OS_FAIL);
		}
#endif
#if (OSA_INTERRUPTS)
		CODE_COVERAGE_TAG;
		OS_IsrPoolInit(block);
#endif

	}
	return(OS_SUCCESS);
}


/***********************************************************************
*
* Name:        OSARun()
*
* Description: Enable the operating system's scheduler
*
* Parameters:  none
*
* Returns:     nothing
*
* Notes:
*
***********************************************************************/
void OSARun(void)
{
	CODE_COVERAGE_TAG;
	OSA_RUN;
}
#endif //OSA_C_SERVICE_INIT


#if (OSA_TASKS)
#if defined (OSA_C_SERVICE_INIT)
/*====================================================================
 *  OSA Task Management
 *
 *  Private:
 *
 *   OS_TaskPoolInit()
 *
 *  Public:
 *
 *   OSATaskCreate()     - create a task
 *   OSATaskDelete()     - delete a task
 *   OSATaskSuspend()    - suspend a task
 *   OSATaskResume()     - resume a task
 *   OSATaskPriority()   - get priority of a task
 *
 *====================================================================*/

/***********************************************************************
*
* Name:        OS_TaskPoolInit()
*
* Description: Initialize OS Task Pool
*
* Parameters:  None
*
* Returns:     Nothing
*
* Notes:
*
***********************************************************************/
static OSA_STATUS OS_TaskPoolInit(OSA_ControlBlock* block)
{
	OS_TaskPool     *poolPtr;
	OSA_STATUS status;

#ifndef OSA_USE_DYNAMIC_REFS
	OS_Task         *taskPtr;
	UINT8 index;
#endif

	CODE_COVERAGE_TAG;
	OSA_ASSERT(block != NULL);
	poolPtr = &block->taskPool;

	/* Create a semaphore for the task pool */
	OSA_SEMA_CREATE(&poolPtr->poolSemaphore, (char *)"TaskPool", 1, OS_FIFO, status);
	if (status != OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_FAIL);
	}

	/* initialize the list */
	poolPtr->head = NULL;

#ifndef OSA_USE_DYNAMIC_REFS
	CODE_COVERAGE_TAG;
	poolPtr->free = &(poolPtr->task[0]);
	taskPtr = poolPtr->free;

	/* Init task list */
	for ( index = 0; index < OSA_TASKS_MAX; index++)
	{
		CODE_COVERAGE_TAG;
		taskPtr->refCheck = NULL;
		taskPtr->next     = taskPtr + 1;
		taskPtr->state    = OSA_TASK_DELETED;
		memset(taskPtr->name, 0, (size_t)OSA_NAME_SIZE);
		memset(&taskPtr->os_ref, 0, sizeof(OS_Task_t));
		taskPtr++;
	}
	(taskPtr - 1)->next =  NULL;
#endif

	poolPtr->numCreated      = 0L;
	poolPtr->totalNumCreated = 0L;
	return(status);
}


/***********************************************************************
*
* Name:        OSATaskCreate()
*
* Description: Get a OSTask data structure from the task pool
*              and initialize it.
*
* Parameters:
*  OSTaskRef *taskRef         - reference to task
*  void*    stackPtr          - pointer to start of task stack area
*  UINT16   stackSize         - number of bytes in task stack area
*  UINT8    priority          - priority of task (0 - 31)
*  void    (*taskStart)(void*) - task entry point
*  void     *argv             - argument pointer to be passed into task
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*  OS_INVALID_PTR      - task entry is NULL
*  OS_INVALID_MEMORY   - stack pointer is NULL
*  OS_INVALID_SIZE     - stack size is insufficient
*  OS_INVALID_PRIORITY - invalid priority
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSATaskCreate( OSTaskRef *taskRef, void* stackPtr,
			  UINT32 stackSize, UINT8 priority, CHAR *taskName,
			  void (*taskStart)(void*), void *argv  )
{
	OS_TaskPool*    poolPtr;
	OS_Task*        taskPtr;
	OSA_STATUS status, rtn;
	OSA_ControlBlock* block;
#ifdef OSA_DEBUG
	char name[OSA_NAME_SIZE];
#else
	UNUSEDPARAM(stackPtr)
#endif


#ifdef OSA_NUCLEUS
	OS_Task         **freeList = OS_NULL;
	OS_Task         **activeListHead = OS_NULL;
	OS_Task_t        *currentTask;
#endif

	CODE_COVERAGE_TAG;
	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	poolPtr = &block->taskPool;

#ifdef OSA_DEBUG

	CODE_COVERAGE_TAG;
	memset((void*)name, 0, (size_t)OSA_NAME_SIZE);

	if (taskRef == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_INVALID_REF);
	}
	if (stackPtr == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_INVALID_MEMORY);
	}
	if (stackSize < OSA_MIN_STACK_SIZE)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_INVALID_SIZE);
	}
	if (taskStart == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_INVALID_PTR);
	}
	if (priority > OSA_LOWEST_PRIORITY)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_INVALID_PRIORITY);
	}
	/* Check the stack buffer pointer*/
	CODE_COVERAGE_TAG;
#ifndef OSA_LINUX
	OSA_ASSERT(( stackPtr != 0) && (((UINT32)stackPtr & 0x3) == 0));
	if (( stackPtr == 0) || (((UINT32)stackPtr & 0x3) != 0) )
	{
		CODE_COVERAGE_TAG;
		return(OS_FAIL);
	}
#endif
	/* fill the stack area for debugging purposes */
	/* do not define these values if this functionality is not needed */
#if (defined OSA_MEM_STACK_FILL_PATERN_SIZE && defined OSA_MEM_STACK_FILL_PATERN_VALUE)
	if (sizeof(stackSize) > (int)OSA_MEM_STACK_FILL_PATERN_SIZE)
	{
		CODE_COVERAGE_TAG;
		memset(stackPtr, (char)OSA_MEM_STACK_FILL_PATERN_VALUE, (size_t)OSA_MEM_STACK_FILL_PATERN_SIZE);
	}
	else
	{
		CODE_COVERAGE_TAG;
		memset(stackPtr, (char)OSA_MEM_STACK_FILL_PATERN_VALUE, stackSize);
	}
#endif

#endif  /* OSA_DEBUG */

#ifndef OSA_NUCLEUS
	CODE_COVERAGE_TAG;
	/* Get a free reference */
	OSA_GET_REF(poolPtr, taskPtr, sizeof(OS_Task));
	if(taskPtr == OS_NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_INVALID_PTR);
	}
#else
	/* Check if the task pool is full. */
	if (poolPtr->numCreated == OSA_TASKS_MAX)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_NO_TASKS);
	}

#ifdef OSA_NUCLEUS
	/* CQ00018938:
	 * If a high priority task ask for semaphore
	 * whilst another low priority task was in create progress and grabbed semaphore
	 * the OSA_NO_SUSPEND brings to dead lock -
	 *  "low" not running but has semaphore
	 *  "high" is running but has no semaphore.
	 *
	 * So do "suspend" but only if called from another task but not from initial-startup thread
	 */
	OSA_TASK_IDENTIFY( currentTask ); /* Get the reference for the running task */
	if (currentTask != 0)
	{
		OSA_SEMA_ACQUIRE(poolPtr->poolSemaphore, OSA_SUSPEND, rtn);
		OSA_ASSERT(rtn == OS_SUCCESS);
	}
	else
#endif
	{
		/* Get a free reference. We cannot suspend in the startup code so
		 * keep checking. */
		CODE_COVERAGE_TAG;
		rtn = OS_UNAVAILABLE;

		while (rtn == OS_UNAVAILABLE)
		{
			OSA_SEMA_ACQUIRE(poolPtr->poolSemaphore, OSA_NO_SUSPEND, rtn);
			if (rtn == OS_UNAVAILABLE)
			{
				CODE_COVERAGE_TAG;
				OSA_TASK_YIELD;
			}
		}
	}

	CODE_COVERAGE_TAG;
	freeList = &poolPtr->free;
	activeListHead = &poolPtr->head;
	OSA_GET_FROM_FREE_LIST((*freeList), taskPtr);
	OSA_SEMA_RELEASE(poolPtr->poolSemaphore, rtn);
#endif

	/* Call the OS specific function */
	OSA_TASK_CREATE(&taskPtr->os_ref, stackPtr, stackSize, priority, (UINT8 *)taskName,
			taskStart, argv, status);

	if (status == OS_SUCCESS)
	{
#ifdef OSA_NUCLEUS
		if (currentTask != 0)
		{
			OSA_SEMA_ACQUIRE(poolPtr->poolSemaphore, OSA_SUSPEND, rtn);
			OSA_ASSERT(rtn == OS_SUCCESS);
		}
		else
		{
			CODE_COVERAGE_TAG;
			/* We cannot suspend in the startup code so keep checking. */
			rtn = OS_UNAVAILABLE;

			while (rtn == OS_UNAVAILABLE)
			{
				OSA_SEMA_ACQUIRE(poolPtr->poolSemaphore, OSA_NO_SUSPEND, rtn);
				if (rtn == OS_UNAVAILABLE)
				{
					CODE_COVERAGE_TAG;
					OSA_TASK_YIELD;
				}
			}
		}

		OSA_ADD_TO_ACTIVE_LIST((*activeListHead), taskPtr);
		poolPtr->numCreated++;
		if (poolPtr->numCreated > poolPtr->totalNumCreated)
			poolPtr->totalNumCreated = poolPtr->numCreated;
		OSA_SEMA_RELEASE(poolPtr->poolSemaphore, rtn);
#endif
		CODE_COVERAGE_TAG;
		taskPtr->refCheck = (void *)&taskPtr->os_ref;
		strncpy(taskPtr->name, (const char*)taskName, OSA_NAME_SIZE);

		/* Store the task info into the taskRef */
		*taskRef = (OSTaskRef *)taskPtr;
		taskPtr->state = OSA_TASK_RUNNING;

		/* Start the OS Task */
		OSA_TASK_RESUME(taskPtr->os_ref, rtn);
	}
	else
	{
#ifndef OSA_NUCLEUS
		CODE_COVERAGE_TAG;
		OSA_DELETE_REF(poolPtr, taskPtr);
#else
		CODE_COVERAGE_TAG;
		/* We cannot suspend in the startup code so keep checking. */
		rtn = OS_UNAVAILABLE;

		while (rtn == OS_UNAVAILABLE)
		{
			OSA_SEMA_ACQUIRE(poolPtr->poolSemaphore, OSA_NO_SUSPEND, rtn);
			if (rtn == OS_UNAVAILABLE)
			{
				CODE_COVERAGE_TAG;
				OSA_TASK_YIELD;
			}
		}
		OSA_ADD_TO_FREE_LIST((*freeList), taskPtr);
		OSA_SEMA_RELEASE(poolPtr->poolSemaphore, rtn);
#endif
	}
	CODE_COVERAGE_TAG;
	return (status);
}

/***********************************************************************
*
* Name:        OSATaskDelete()
*
* Description: Delete an OSTask data structure from the task pool
*
* Parameters:
*  OSTaskRef *taskRef         - reference to task
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*  OS_FAIL             - OS specific error
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSATaskDelete( OSTaskRef taskRef )
{
	OS_TaskPool     *taskPool;
	OS_Task         *taskPtr = (OS_Task *)taskRef;
	OSA_STATUS status;
	OSA_ControlBlock* block;

#ifdef OSA_NUCLEUS
	OSA_STATUS rtn = OS_FAIL;
	OS_Task         **activeListHead = OS_NULL;
	OS_Task         **freeList = OS_NULL;
#endif
	CODE_COVERAGE_TAG;
	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);

	/* Validate the reference */
	OSA_REF_CHECK(taskRef, taskPtr);
	OSA_ASSERT(block != NULL);

	taskPool        = &block->taskPool;
#ifdef OSA_NUCLEUS
	CODE_COVERAGE_TAG;
	activeListHead  = &taskPool->head;
	freeList        = &taskPool->free;
#endif

	/* Call the OS specific function */
	OSA_TASK_DELETE(taskPtr->os_ref, status);

	if (status == OS_SUCCESS)
	{
#ifndef OSA_NUCLEUS
		CODE_COVERAGE_TAG;
		OSA_DELETE_REF(taskPool, taskPtr);
#else
		CODE_COVERAGE_TAG;
		/* We cannot suspend in the startup code so keep checking. */
		rtn = OS_UNAVAILABLE;

		while (rtn == OS_UNAVAILABLE)
		{
			OSA_SEMA_ACQUIRE(taskPool->poolSemaphore, OSA_NO_SUSPEND, rtn);
			if (rtn == OS_UNAVAILABLE)
			{
				CODE_COVERAGE_TAG;
				OSA_TASK_YIELD;
			}
		}
		OSA_DELETE_FROM_ACTIVE_LIST((*activeListHead), taskPtr);
		OSA_ADD_TO_FREE_LIST((*freeList), taskPtr);
		/* Decrease the number of created tasks */
		taskPool->numCreated--;
		OSA_SEMA_RELEASE(taskPool->poolSemaphore, rtn);
#endif
		taskPtr->refCheck = NULL;
		taskPtr->state    = OSA_TASK_DELETED;
		memset(taskPtr->name, 0, (size_t)OSA_NAME_SIZE);
	}
	return(status);
}


/***********************************************************************
*
* Name:        OSATaskSuspend()
*
* Description: Suspend the task.
*
* Parameters:  OSTaskRef taskRef     - reference of task to modify
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*  OS_FAIL             - OS specific error
*
* Notes:
*
*
***********************************************************************/
OSA_STATUS  OSATaskSuspend( OSTaskRef taskRef )
{
	OSA_STATUS return_val;
	OS_Task*    task = (OS_Task *)taskRef;

	CODE_COVERAGE_TAG;
	/* Validate the reference */
	OSA_REF_CHECK(taskRef, task);

	/* Set the state to suspended as we assume it worked */
	task->state = OSA_TASK_SUSPENDED;

	OSA_TASK_SUSPEND( task->os_ref, return_val );

	/* Suspend did not work so task must still be running */
	if (return_val != OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		task->state = OSA_TASK_RUNNING;
	}

	return (return_val);
}


/***********************************************************************
*
* Name:        OSATaskResume()
*
* Description: Resume the task.
*
* Parameters:  OSTaskRef taskRef     - reference of task to modify
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*  OS_FAIL             - OS specific error
*
* Notes:
*
*
***********************************************************************/
OSA_STATUS  OSATaskResume( OSTaskRef taskRef )
{
	OSA_STATUS return_val;
	OS_Task*     task = (OS_Task *)taskRef;

	CODE_COVERAGE_TAG;
	/* Validate the reference */
	OSA_REF_CHECK(taskRef, task);

	/* Set the state to running as we assume it worked */
	task->state = OSA_TASK_RUNNING;

	OSA_TASK_RESUME( task->os_ref, return_val );

	/* Resume did not work so task must still be suspended */
	if (return_val != OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		task->state = OSA_TASK_SUSPENDED;
	}

	return (return_val);
}


/***********************************************************************
*
* Name:        OSATaskSleep()
*
* Description: Put the task to sleep for a specified number of ticks.
*
* Parameters:  OSTaskRef taskRef     - reference of task to modify
*
* Returns:     None
*
* Notes:
*
*
***********************************************************************/
void  OSATaskSleep( UINT32 ticks )
{
	CODE_COVERAGE_TAG;
	if (ticks != 0)
	{
		CODE_COVERAGE_TAG;
		OSA_TASK_SLEEP( ticks );
	}
}


/***********************************************************************
*
* Name:        OSATaskChangePriority()
*
* Description: Change task priority and return value of old priority
*
* Parameters:  OSTaskRef taskRef     - reference of task to modify
*              UINT8     newPriority - new priority to assign
*              UINT8     oldPriority - old task priority
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*
* Notes:       For the OS valid Task priorities are in the range
*              0 (highest priority) to 31 (lowest priority).
*
***********************************************************************/
OSA_STATUS  OSATaskChangePriority( OSTaskRef taskRef, UINT8 newPriority, UINT8 *oldPriority)
{
	OSA_STATUS status;
	OS_Task*    task = (OS_Task *)taskRef;

#ifdef OSA_DEBUG
	CODE_COVERAGE_TAG;
	/* Validate the reference */
	OSA_REF_CHECK(taskRef, task);
	OSA_ASSERT(taskRef != NULL);

	if (newPriority > OSA_LOWEST_PRIORITY)
	{
		CODE_COVERAGE_TAG;
		return (OS_INVALID_PRIORITY);
	}
#endif

	CODE_COVERAGE_TAG;
	OSA_TASK_CHANGE_PRIORITY( &task->os_ref, newPriority, oldPriority, status );

	return (status);
}

/***********************************************************************
*
* Name:        OSATaskGetPriority()
*
* Description: Get the task priority.
*
* Parameters:  OSTaskRef taskRef     - reference of task
*              UINT8     *priority   - task priority
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*
* Notes:       For the OS valid Task priorities are in the range
*              0 (highest priority) to 31 (lowest priority).
*
***********************************************************************/
OSA_STATUS  OSATaskGetPriority( OSTaskRef taskRef, UINT8 *priority )
{
	OSA_STATUS status;
	OS_Task*    task = (OS_Task *)taskRef;

	CODE_COVERAGE_TAG;
	/* Validate the reference */
	OSA_REF_CHECK(taskRef, task);
	OSA_ASSERT(taskRef != NULL);

	OSA_TASK_GET_PRIORITY( &task->os_ref, priority, status );

	return (status);
}

/***********************************************************************
*
* Name:        OSATaskGetCurrentRef()
*
* Description: Get the current running task reference.
*
* Parameters:
*
* Returns:
*  OSTaskRef taskRef     - reference of task
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSATaskGetCurrentRef( OSATaskRef *taskRef )
{
	OS_Task_t  *currentTask;

	CODE_COVERAGE_TAG;
	/* Get the reference for the running task */
	OSA_TASK_IDENTIFY( currentTask );
	*taskRef = (OSTaskRef *)currentTask;

	return(OS_SUCCESS);
}
#endif //OSA_C_SERVICE_INIT

#if defined (OSA_C_SERVICE_RUN)
/***********************************************************************
*
* Name:        OSATaskYield()
*
* Description: Allow other tasks of same priority to run.
*
* Parameters:  None
*
* Returns:     None
*
* Notes:       As tasks are added to the ready list in FIFO order,
*              control will be transferred to the next ready task that
*              is at the same priority level. If no other tasks are
*              ready, the calling task will continue to execute.
*
***********************************************************************/
void  OSATaskYield( void )
{
	CODE_COVERAGE_TAG;
	OSA_TASK_YIELD;
}
#endif  //OSA_C_SERVICE_RUN
#endif  //OSA_TASKS


#if (OSA_MEM_POOLS)
#if defined (OSA_C_SERVICE_INIT)
/***********************************************************************
*
* Name:        OSMemPoolInit()
*
* Description: Initialize OS Memory Pools
*
* Parameters:  None
*
* Returns:     Nothing
*
* Notes:       All available memory pools in the system are created.
*
***********************************************************************/
static OSA_STATUS OS_MemPoolInit(OSA_ControlBlock* block)
{
	OS_MemPool*     memPoolPtr;
	OSA_STATUS status;

#ifndef OSA_USE_DYNAMIC_REFS
	UINT8 index;
	OS_Mem*         memPtr;
#endif

	CODE_COVERAGE_TAG;
	OSA_ASSERT(block != NULL);

	memPoolPtr = &(block->memPool);

	/*
	**  Create semaphore used to control access to the memory pool
	**  linked lists.
	*/
	OSA_SEMA_CREATE(&memPoolPtr->poolSemaphore, (char *)"MemPool", 1, OS_FIFO, status);

	if (status != OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_FAIL);
	}

	/* initialize the list */
	memPoolPtr->head = NULL;

#ifndef OSA_USE_DYNAMIC_REFS
	CODE_COVERAGE_TAG;
	memPoolPtr->free = &(memPoolPtr->memPools[0]);
	memPtr = memPoolPtr->free;

	for (index = 0; index < OSA_MEM_POOLS_MAX; index++)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(status == OS_SUCCESS);

		memPtr->refCheck = NULL;
		memPtr->next = memPtr + 1;
		memPtr++;
	}

	(memPtr - 1)->next = NULL;
#endif

	CODE_COVERAGE_TAG;
	memPoolPtr->numCreated = 0;
	return(status);
}


/*======================================================================
 *  OSA Memory Management
 *
 *   OSAMemPoolCreate() - Create a MemPool
 *   OSAMemPoolDelete() - Delete a MemPool
 *
 *   OSAMemPoolAlloc() - allocate block of memory from OS memory pool
 *   OSAMemPoolFree()  - free block of memory from OS memory pool
 *====================================================================*/

/***********************************************************************
*
* Name:        OSAMemPoolCreate()
*
* Description: Initialize a OS_MemPool data structure from the memory pool.
*
* Parameters:
*  OSPoolRef   *poolRef      - pointer to memory pool structure to fill
*  UINT32      poolType      - OSA_FIXED or OSA_VARIABLE
*  UINT8       *poolBase     - starting address of memory for the pool
*  UINT32      poolSize      - total # bytes for the memory pool
*  UINT32      partitionSize - memory partion size for Fixed pools
*  UINT8       waitingMode   - scheduling of waiting tasks
*                              OS_FIFO, OS_PRIORITY
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*  OS_INVALID_POINTER  - mem is NULL
*  OS_INVALID_SIZE     - size is NULL
*  OS_INVALID_MODE     - waiting mode is invalid
*  OS_NO_MEM_POOLS     - no available memory pools
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSAMemPoolCreate( OSPoolRef *poolRef, UINT32 poolType,
			     UINT8 *poolBase, UINT32 poolSize,
			     UINT32 partitionSize, UINT8 waitingMode )
{
	OSA_STATUS status;
	OS_Mem      *mem = 0;
	OS_MemPool  *memPool;
	OSA_ControlBlock* block;

#ifndef OSA_DEBUG
	UNUSEDPARAM(waitingMode)
#endif

	CODE_COVERAGE_TAG;
	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	memPool = &block->memPool;

#ifndef OSA_USE_DYNAMIC_REFS
	CODE_COVERAGE_TAG;
	/* Check if the pool is full. */
	if (memPool->numCreated == OSA_MEM_POOLS_MAX)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_NO_MEM_POOLS);
	}
#endif

#ifdef OSA_DEBUG
	CODE_COVERAGE_TAG;
	if (poolRef == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_REF);
	}
	if (poolBase == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_POINTER);
	}
	if ( (poolType == OSA_VARIABLE) && (poolSize == 0L) )
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_SIZE);
	}
	if ( (poolType == OSA_FIXED) && (partitionSize == 0L) )
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_SIZE);
	}
	if ( (poolType == OSA_FIXED) && (partitionSize > poolSize) )
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_SIZE);
	}
	if ( (waitingMode != OS_FIFO) && (waitingMode != OS_PRIORITY) )
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_MODE);
	}
#endif

	CODE_COVERAGE_TAG;
	/* Get a free reference */
	OSA_GET_REF(memPool, mem, sizeof(OS_Mem));
	if(mem == OS_NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_INVALID_PTR);
	}
	OSA_MEM_POOL_CREATE((OS_PartitionPool_t *)&mem->u, NULL, poolType, poolBase, poolSize, \
			    partitionSize, waitingMode, status);

	if (status == OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		mem->type     = poolType;
		mem->poolBase = poolBase;
		mem->poolSize = poolSize;
		mem->partitionSize = partitionSize;
		/* Store the filled queue into user's reference */
		*poolRef = (OSPoolRef *)mem;
	}
	else
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		OSA_DELETE_REF(memPool, mem);
	}

	return(status);
}


/***********************************************************************
*
* Name:        OSAMemPoolCreateGlobal()
*
* Description: Create or attach to a global named partition memory pool which
*              can be shared between multiple processes.
*
* Parameters:
*  OSPoolRef   *poolRef      - pointer to memory pool structure to fill
*  char        *poolName     - name of queue
*  UINT8       *poolBase     - starting address of memory for the pool
*  UINT32      poolSize      - total # bytes for the memory pool
*  UINT32      partitionSize - memory partion size for Fixed pools
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*  OS_INVALID_SIZE     - size is NULL
*  OS_NO_MEM_POOLS     - no available memory pools
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSAMemPoolCreateGlobal( OSPoolRef *poolRef, char *poolName,
				   UINT8 *poolBase, UINT32 poolSize, UINT32 partitionSize )
{
	OSA_STATUS status;
	OS_Mem      *mem = 0;
	OS_MemPool  *memPool;
	OSA_ControlBlock* block;

	CODE_COVERAGE_TAG;
	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	memPool = &block->memPool;

#ifdef OSA_DEBUG
	CODE_COVERAGE_TAG;
	if (poolRef == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_REF);
	}
#endif

	/* Use the provided shared memory region, otherwise get a free reference from the pool */
	if (poolBase)
	{
		CODE_COVERAGE_TAG;
		mem = (OS_Mem*)poolBase;
	}
	else
	{
		CODE_COVERAGE_TAG;
		OSA_GET_REF(memPool, mem, sizeof(OS_Mem));
		if(mem == OS_NULL)
		{
			CODE_COVERAGE_TAG;
			OSA_ASSERT_EXT(FALSE);
			return(OS_INVALID_PTR);
		}

	}

	OSA_MEM_POOL_CREATE((OS_PartitionPool_t *)&mem->u, poolName, OSA_GLOBAL, poolBase, \
			    poolSize, partitionSize, 0, status);

	if (status == OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		mem->type     = OSA_GLOBAL;
		mem->poolBase = poolBase;
		mem->poolSize = poolSize;
		mem->partitionSize = partitionSize;
		/* Store the filled queue into user's reference */
		if (poolBase)
		{
			CODE_COVERAGE_TAG;
			*poolRef = (OSPoolRef *)poolBase;
		}
		else
		{
			CODE_COVERAGE_TAG;
			*poolRef = (OSPoolRef *)mem;
		}

	}
	else
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		OSA_DELETE_REF(memPool, mem);
	}

	return(status);
}

/***********************************************************************
*
* Name:        OSAMemPoolDelete()
*
* Description: Delete a memory pool.
*
* Parameters:
*  OSPoolRef   poolRef     - reference to memory pool
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSAMemPoolDelete( OSPoolRef poolRef )
{
	OSA_STATUS status;
	OS_Mem      *mem = (OS_Mem *)poolRef;
	OS_MemPool  *memPool;
	OSA_ControlBlock* block;

	CODE_COVERAGE_TAG;
	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	memPool = &block->memPool;

#ifdef OSA_DEBUG
	CODE_COVERAGE_TAG;
	/* Validate the reference */
	if (poolRef == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_REF);
	}
	if (mem->refCheck != &mem->u)
	{
		CODE_COVERAGE_TAG;
		return(OS_INVALID_REF);
	}
#endif

	CODE_COVERAGE_TAG;
	OSA_ASSERT(poolRef != OS_NULL);

	OSA_MEM_POOL_DELETE((OS_PartitionPool_t *)&mem->u, mem->type, status);

	if (status == OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		OSA_DELETE_REF(memPool, mem);
	}

	return(status);
}

#endif //OSA_C_SERVICE_INIT

#if defined (OSA_C_SERVICE_RUN)
/***********************************************************************
*
* Name:        OSAMemPoolAlloc
*
* Description: Allocate block of memory from OS memory pool
*
* Parameters:
*  OSPoolRef   poolRef     - memory pool reference
*  UINT32      size        - size of memory block in bytes
*  void        **mem       - non-zero pointer to allocated memory block
*                            or zero if allocation failed.
*  UINT32      timeout     - OSA_SUSPEND, OSA_NO_SUSPEND
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*  OS_NO_MEMORY        - no memory left in the pool
*  OS_TIMEOUT          - timeout while suspended waiting for memory
*  OS_INVALID_POINTER  - mem is NULL
*  OS_INVALID_SIZE     - size exceeds memory pool partition size (fixed only)
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSAMemPoolAlloc(OSPoolRef poolRef, UINT32 size, void **mem, UINT32 timeout)
{
	OSA_STATUS return_val;
	OS_Mem      *pool = (OS_Mem *)poolRef;

	CODE_COVERAGE_TAG;
#ifdef OSA_DEBUG
	CODE_COVERAGE_TAG;
	/* Validate the reference */
	if (poolRef == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_REF);
	}
	if (pool->refCheck != &pool->u)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_INVALID_REF);
	}

	if ((pool->type == OSA_FIXED) && (size > pool->partitionSize))
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_SIZE);
	}
	if (size == 0L)
	{
		OSA_ASSERT(FALSE);
		return(OS_INVALID_SIZE);
	}
	if (mem == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_POINTER);
	}
#endif

#ifndef OSA_MEM_CHECK
	/* SCR CCD1456: Set size to configured size if partition pool */
	if (pool->type == OSA_FIXED || pool->type == OSA_GLOBAL)
	{
		CODE_COVERAGE_TAG;
		size = pool->partitionSize;
	}
#endif

	CODE_COVERAGE_TAG;
	/* Allocate memory for the OS task pool. */
	OSA_MEM_ALLOC(&pool->u, pool->type, size, pool->partitionSize, mem, timeout, return_val );
#ifdef OSA_DEBUG
	/* Clear the partition */
	if (return_val == OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		memset((void*)*mem, 0, (size_t)size);
	}
#endif

	return (return_val);
}


/***********************************************************************
*
* Name:        OSAMemPoolFree
*
* Description: Deallocate block of memory that was allocated by
*              OS_MemPoolAlloc().
*
* Parameters:
*  OSPoolRef   poolRef     - memory pool reference
*  void*       mem         - pointer to allocated block
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*  OS_INVALID_POINTER  - mem is NULL
*
* Notes:
*
***********************************************************************/
OSA_STATUS  OSAMemPoolFree(OSPoolRef poolRef, void* mem)
{
	OSA_STATUS return_val;
	OS_Mem      *pool = (OS_Mem *)poolRef;

	CODE_COVERAGE_TAG;
#ifdef OSA_DEBUG
	CODE_COVERAGE_TAG;
	/* Validate the reference */
	if (poolRef == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_REF);
	}
	if (pool->refCheck != &pool->u)
	{
		CODE_COVERAGE_TAG;
		return(OS_INVALID_REF);
	}
	if (mem == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_POINTER);
	}
#endif

	/* Free the memory. */
	OSA_MEM_FREE(&pool->u, pool->type, mem, return_val );

	return(return_val);
}
/***********************************************************************
*
* Name:        OSAMemPoolFixedFree
*
* Description: Deallocate a block of memory from a FIXED partition pool
*              that was allocated by OS_MemPoolAlloc().
*
* Parameters:
*  void*       mem         - pointer to allocated block from FIXED pool
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_POINTER  - mem is NULL
*
* Notes:
*
***********************************************************************/
OSA_STATUS  OSAMemPoolFixedFree(void* mem)
{
	OSA_STATUS return_val;

	CODE_COVERAGE_TAG;
#ifdef OSA_DEBUG
	CODE_COVERAGE_TAG;
	/* Validate the pointer */
	if (mem == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_POINTER);
	}
#endif

	/* Free the memory. */
	OSA_MEM_FREE(NULL, OSA_FIXED, mem, return_val );

	OSA_ASSERT(return_val == OS_SUCCESS);

	return(return_val);
}
#endif  //OSA_C_SERVICE_RUN
#endif  //OSA_MEM_POOLS


#if (OSA_MSG_QUEUES)
/*======================================================================
 *  OSA Message Passing Services:
 *
 *   OSAMsgQCreate()     - Create a message queue
 *   OSAMsgQDelete()     - Delete a message queue
 *   OSAMsgQPoll()       - Check for a message in a queue
 *   OSAMsgQSend()       - Send message to message queue
 *   OSAMsgQRecv()       - Receive message from message queue
 *====================================================================*/
#if defined (OSA_C_SERVICE_INIT)
/***********************************************************************
*
* Name:        OS_MsgPoolInit()
*
* Description: Initialize data structures for IPC.
*
* Parameters:
*
*
* Returns: Nothing
*
* Notes:
*
*
***********************************************************************/
static OSA_STATUS OS_MsgPoolInit(OSA_ControlBlock* block)
{
	OS_MsgQPool*    poolPtr;
	OSA_STATUS status;

#ifndef OSA_USE_DYNAMIC_REFS
	OS_MsgQueue     *theQueue;
	UINT8 index;

	CODE_COVERAGE_TAG;
	theQueue = &block->msgQPool.queues[0];
#endif
	CODE_COVERAGE_TAG;
	poolPtr = &(block->msgQPool);

	/*
	**  Create semaphore used to control access to the msg Pool
	**  linked lists.
	*/
	OSA_SEMA_CREATE(&poolPtr->poolSemaphore, (char *)"MsgPool", 1, OS_FIFO, status);

	if (status != OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_FAIL);
	}

	/* initialize the list */
	poolPtr->head = NULL;

#ifndef OSA_USE_DYNAMIC_REFS
	CODE_COVERAGE_TAG;
	poolPtr->free = &(poolPtr->queues[0]);
	theQueue = poolPtr->free;

	for (index = 0; index < OSA_MSG_QUEUES_MAX; index++)
	{
		CODE_COVERAGE_TAG;
		theQueue->msgCount            = 0;
#ifdef OSA_DEBUG
		CODE_COVERAGE_TAG;
		theQueue->maxNumber           = 0;
		theQueue->maxMsgSize          = 0;
		theQueue->totalMsgReceived    = 0;
		theQueue->totalBytesReceived  = 0;
		theQueue->maxMsgInQueue       = 0;
		theQueue->controlBlock        = block;
		theQueue->poolSema            = &poolPtr->poolSemaphore;
#endif
		theQueue->refCheck            = NULL;
		theQueue->next = theQueue + 1;
		theQueue++;
	}

	(theQueue - 1)->next = NULL;
#endif

	poolPtr->numCreated = 0;
	return(status);
}


/***********************************************************************
*
* Name:        OSAMsgQCreate()
*
* Description: Create or attach to a glogal named message queue using internal OSA
*              memory. The queue can be shared between processes.
*
* Parameters:
*  OSMsgQRef *msgQRef         - reference to the message queue
*  char      *queueName       - name of queue
*  UINT32    maxSize          - max size of data in queue
*  UINT32    maxNumber        - max number of messages on queue
*  UINT32    waitingMode      - scheduling of waiting tasks
*                               OS_FIFO, OS_PRIORITY
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*  OS_INVALID_MODE     - invalid waiting mode
*  OS_INVALID_SIZE     - max size is zero
*  OS_NO_QUEUES        - no message queues left in the system
*  OS_FAIL             - OS specific error
*
* Notes:
*
*
***********************************************************************/
OSA_STATUS OSAMsgQCreate(OSMsgQRef* msgQRef,
#ifdef OSA_QUEUE_NAMES
			 char *queueName,
#endif
			 UINT32 maxSize,
			 UINT32 maxNumber, UINT8 waitingMode)
{
	OS_MsgQPool*        poolPtr;
	OS_MsgQueue*        queuePtr = 0;
	OSA_STATUS status;
	OSA_ControlBlock*   block;

	CODE_COVERAGE_TAG;
	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	poolPtr = &block->msgQPool;

#ifndef OSA_QUEUE_NAMES
	CODE_COVERAGE_TAG;
	char *queueName = "OsaMgQ";
	sprintf(queueName, "%d", poolPtr->numCreated);
#endif

#ifndef OSA_USE_DYNAMIC_REFS
	CODE_COVERAGE_TAG;
	/* Check if the pool is full. */
	if (poolPtr->numCreated == OSA_MSG_QUEUES_MAX)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_NO_QUEUES);
	}
#endif

#ifdef OSA_DEBUG
	CODE_COVERAGE_TAG;
	if (msgQRef == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_REF);
	}
	if (maxSize == 0L)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_SIZE);
	}
	if (maxNumber == 0)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_NO_RESOURCES);
	}
	if ( (waitingMode != OSA_FIFO) && (waitingMode != OSA_PRIORITY) )
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_MODE);
	}
#endif

	/* Get a free reference */
	OSA_GET_REF(poolPtr, queuePtr, sizeof(OS_MsgQueue));
	if(queuePtr == OS_NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_INVALID_PTR);
	}
	/* Create the message queue */
	OSA_QUEUE_CREATE(&queuePtr->os_ref, queueName, maxSize, maxNumber, NULL, waitingMode, status);

	if (status == OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		queuePtr->msgCount            = 0;
#ifdef OSA_DEBUG
		CODE_COVERAGE_TAG;
		queuePtr->controlBlock        = block;
		queuePtr->maxNumber           = maxNumber;
		queuePtr->maxMsgSize          = maxSize;
		queuePtr->totalMsgReceived    = 0;
		queuePtr->totalBytesReceived  = 0;
		queuePtr->maxMsgInQueue       = 0;
#endif

		/* Store the filled queue into user's reference */
		*msgQRef = (OSMsgQRef *)queuePtr;
	}
	else
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		OSA_DELETE_REF(poolPtr, queuePtr);
	}

	return (status);
}

/***********************************************************************
*
* Name:        OSAMsgQCreateWithMem()
*
* Description: Create a message queue using user's memory for the queue.
*
* Parameters:
*  OSMsgQRef *msgQRef         - reference to the message queue
*  char      *queueName       - name of queue
*  UINT32    maxSize          - max size of data in queue
*  UINT32    maxNumber        - max number of messages on queue
*  void      *qAddr           - start address of queue memory
*  UINT32    waitingMode      - scheduling of waiting tasks
*                               OS_FIFO, OS_PRIORITY
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*  OS_INVALID_MODE     - invalid waiting mode
*  OS_INVALID_SIZE     - max size is zero
*  OS_NO_QUEUES        - no message queues left in the system
*  OS_FAIL             - OS specific error
*
* Notes:
*
*
***********************************************************************/
OSA_STATUS OSAMsgQCreateWithMem(OSMsgQRef* msgQRef,
#ifdef OSA_QUEUE_NAMES
				char *queueName,
#endif
				UINT32 maxSize,
				UINT32 maxNumber, void *qAddr, UINT8 waitingMode)
{
	OS_MsgQPool*    poolPtr;
	OS_MsgQueue*    queuePtr = 0;
	OSA_STATUS status;
	OSA_ControlBlock* block;

	CODE_COVERAGE_TAG;
	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	poolPtr = &block->msgQPool;

#ifndef OSA_QUEUE_NAMES
	char *queueName = "OsaMgQ";
	sprintf(queueName, "%d", poolPtr->numCreated);
#endif

#ifdef OSA_DEBUG
	CODE_COVERAGE_TAG;
	if (msgQRef == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_REF);
	}
	if (qAddr == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_POINTER);
	}
	if (queueName == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_POINTER);
	}

	if (maxSize == 0L)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_SIZE);
	}
	if (maxNumber == 0)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_NO_RESOURCES);
	}
	if ( (waitingMode != OSA_FIFO) && (waitingMode != OSA_PRIORITY) )
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_MODE);
	}
#endif


#ifndef OSA_USE_DYNAMIC_REFS
	CODE_COVERAGE_TAG;
	/* Check if the pool is full. */
	if (poolPtr->numCreated == OSA_MSG_QUEUES_MAX)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_NO_QUEUES);
	}
#endif

	/* Get a free reference */
	OSA_GET_REF(poolPtr, queuePtr, sizeof(OS_MsgQueue));
	if(queuePtr == OS_NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_INVALID_PTR);
	}
	/* Create the message queue */
	OSA_QUEUE_CREATE(&queuePtr->os_ref, queueName, maxSize, maxNumber, qAddr, waitingMode, status);

	if (status == OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		queuePtr->msgCount            = 0;
#ifdef OSA_DEBUG
		CODE_COVERAGE_TAG;
		queuePtr->controlBlock        = block;
		queuePtr->maxNumber           = maxNumber;
		queuePtr->maxMsgSize          = maxSize;
		queuePtr->totalMsgReceived    = 0;
		queuePtr->totalBytesReceived  = 0;
		queuePtr->maxMsgInQueue       = 0;
#endif

		/* Store the filled queue into user's reference */
		*msgQRef = (OSMsgQRef *)queuePtr;
	}
	else
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		OSA_DELETE_REF(poolPtr, queuePtr);
	}

	return (status);
}



/***********************************************************************
*
* Name:        OSAMsgQDelete()
*
* Description: Delete a message queue.
*
* Parameters:
*  OSMsgQRef msgQRef         - reference to the message queue
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*  OS_FAIL             - OS specific error
*
* Notes:
*
*
***********************************************************************/
OSA_STATUS OSAMsgQDelete(OSMsgQRef msgQRef)
{
	OS_MsgQPool     *poolPtr;
	OS_MsgQueue     *queuePtr = (OS_MsgQueue *)msgQRef;
	OSA_STATUS return_val = OS_FAIL;
	OSA_ControlBlock* block;

	CODE_COVERAGE_TAG;
	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	poolPtr = &block->msgQPool;

	/* Validate the reference */
	OSA_REF_CHECK(msgQRef, queuePtr);
	OSA_ASSERT(block != NULL);

	/* Call the OS specific function */
	OSA_QUEUE_DELETE(queuePtr->os_ref, return_val);

	OSA_ASSERT(return_val == OS_SUCCESS);

	if (return_val == OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		OSA_DELETE_REF(poolPtr, queuePtr);
	}

	return(return_val);
}
#endif //OSA_C_SERVICE_INIT

#if defined (OSA_C_SERVICE_RUN)
/***********************************************************************
*
* Name:        OSAMsgQSend()
*
* Description: Send message to message queue
*
* Parameters:
*
*   OSMsgQRef msgQRef - message queue to send message to
*   UINT32    size    - size of the message in bytes
*   UINT8*    msgPtr  - pointer to message
*   UINT32    timeout - OSA_SUSPEND, OSA_NO_SUSPEND, or timeout
*
* Returns:
*
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*  OS_INVALID_POINTER  - if msgPtr is zero
*  OS_QUEUE_FULL       - if there is no room for message on queue
*  OS_TIMEOUT          - timeout while suspended waiting for a queue
*  OS_INVALID_SIZE     - if message size exceeds configured queue size
*  OS_MSGQ_DELETED     - queue was deleted before service was completed
*  OS_FAIL             - OS specific error
*
* Notes:
*
*   Messages are copied to the back of the message queue and
*   removed from the front of the message queue.
*
***********************************************************************/
OSA_STATUS OSAMsgQSend(OSMsgQRef msgQRef, UINT32 size, UINT8* msgPtr, UINT32 timeout)
{
	OS_MsgQueue         *queuePtr = (OS_MsgQueue *)msgQRef;
	OSA_STATUS status;

	CODE_COVERAGE_TAG;
#ifdef OSA_DEBUG
	CODE_COVERAGE_TAG;
	/* Validate the reference */
	OSA_REF_CHECK(msgQRef, queuePtr);

	if ( (size > queuePtr->maxMsgSize) || (size == 0) )
	{
		CODE_COVERAGE_TAG;
		return(OS_INVALID_SIZE);
	}
	if (msgPtr == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_INVALID_POINTER);
	}
#endif

	OSA_QUEUE_SEND(queuePtr->os_ref, size, msgPtr, timeout, status);
	OSA_ASSERT_EXT_TO(status == OS_SUCCESS);

	if (status == OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		OSA_CONTEXT_LOCK;

		/* Increment Queue related statistics */
		queuePtr->msgCount++;
#ifdef OSA_DEBUG
		CODE_COVERAGE_TAG;
		/* Update the message statistics */
		queuePtr->totalMsgReceived++;
		queuePtr->totalBytesReceived += size;

		if (queuePtr->msgCount > queuePtr->maxMsgInQueue)
		{
			CODE_COVERAGE_TAG;
			queuePtr->maxMsgInQueue = queuePtr->msgCount;
		}

		OSA_ASSERT(queuePtr->controlBlock != NULL);

		/* Increment System related statistics */
		queuePtr->controlBlock->stats.sysMsgCount++;
		queuePtr->controlBlock->stats.sysTotalMsgReceived++;

		if (queuePtr->controlBlock->stats.sysMsgCount >
		    queuePtr->controlBlock->stats.sysMaxMsgs)
		{
			CODE_COVERAGE_TAG;
			queuePtr->controlBlock->stats.sysMaxMsgs =
				queuePtr->controlBlock->stats.sysMsgCount;
		}

#endif          /* OS Debug statistics */

		OSA_CONTEXT_UNLOCK;
	}
	if (status == OS_DELETED)
	{
		CODE_COVERAGE_TAG;
		return(OS_MSGQ_DELETED);
	}

	return(status);
}

/***********************************************************************
*
* Name:        OSAMsgQRecv()
*
* Description: Get pointer to message at the top of the message queue.
*
* Parameters:
*
*   OSMsgQRef msgQRef - reference to the queue that will be
*                       checked for data.
*   UINT8*    recvMsg - pointer to received message or NULL if there are no messages
*                       on the queue.
*   UINT32    size    - max size of message to receive
*   UINT32    timeout - OSA_SUSPEND, OSA_NO_SUSPEND, or timeout
*                       1 - 4,294,967,293
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*  OS_INVALID_POINTER  - if msgPtr is zero
*  OS_QUEUE_EMPTY      - message queue is empty
*  OS_TIMEOUT          - timeout while suspended waiting for a queue
*  OS_INVALID_SIZE     - if message size exceeds configured queue size
*  OS_MSGQ_DELETED     - queue was deleted before service was completed
*  OS_FAIL             - OS specific error
*
* Notes:
*
*   The received message pointer is valid until the next call to
*   OSMsgQRecv() using the same queue reference parameter.
*
*   A task may call this function at any time; it does not need to
*   wait for an event.
*
***********************************************************************/
OSA_STATUS OSAMsgQRecv(OSMsgQRef msgQRef, UINT8* recvMsg, UINT32 size,
		       UINT32 timeout )
{
	OS_MsgQueue      *queuePtr = (OS_MsgQueue *)msgQRef;
	OSA_STATUS status;

	CODE_COVERAGE_TAG;
#ifdef OSA_DEBUG
	CODE_COVERAGE_TAG;
	/* Validate the reference */
	OSA_REF_CHECK(msgQRef, queuePtr);

	if ( (size > queuePtr->maxMsgSize) || (size == 0L) )
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_SIZE);
	}
	if (recvMsg == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_POINTER);
	}
#endif

	OSA_CONTEXT_LOCK;
	if ( (timeout == OSA_NO_SUSPEND) && (queuePtr->msgCount == 0L) )
	{
		CODE_COVERAGE_TAG;
		OSA_CONTEXT_UNLOCK;
		return(OS_QUEUE_EMPTY);
	}

	OSA_CONTEXT_UNLOCK;

	OSA_QUEUE_RECV(queuePtr->os_ref, recvMsg, size, timeout, status);
	OSA_ASSERT_EXT_TO(status == OS_SUCCESS);

	if (status == OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		OSA_CONTEXT_LOCK;
		/* Decrement message count stat */
		queuePtr->msgCount--;

#ifdef OSA_DEBUG
		/* Decrement System related statistic */
		queuePtr->controlBlock->stats.sysMsgCount--;
#endif
		OSA_CONTEXT_UNLOCK;
	}

	if (status == OS_DELETED)
	{
		CODE_COVERAGE_TAG;
		return(OS_MSGQ_DELETED);
	}

	return (status);
}


/***********************************************************************
*
* Name:        OSAMsgQPoll()
*
* Description: Check for messages on the queue without taking any off
*              the queue.
*
* Parameters:
*
*   OSMsgQRef msgQRef  - reference to the queue that will be
*                        checked for data.
*   UINT32    msgCount - number of messages on the queue
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*  OS_INVALID_POINTER  - if msgCount pointer is NULL
*
* Notes:
*
*
***********************************************************************/
OSA_STATUS OSAMsgQPoll(OSMsgQRef msgQRef, UINT32 *msgCount)
{
	OS_MsgQueue      *queuePtr = (OS_MsgQueue *)msgQRef;
	OSA_STATUS status;

	CODE_COVERAGE_TAG;
#ifdef OSA_DEBUG
	CODE_COVERAGE_TAG;
	/* Validate the reference */
	OSA_REF_CHECK(msgQRef, queuePtr);

	if (msgCount == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_POINTER);
	}
#endif

	OSA_QUEUE_POLL(queuePtr->os_ref, msgCount, status);

	/* RTT does not support this so check internal counter */
	if (status == OS_UNSUPPORTED)
	{
		CODE_COVERAGE_TAG;
		*msgCount = queuePtr->msgCount;
		return (OS_SUCCESS);
	}

	return (status);
}
#endif  //OSA_C_SERVICE_RUN
#endif  //OSA_MSG_QUEUES


#if (OSA_MBOX_QUEUES)
/*======================================================================
 *  OS Mailbox Management
 *====================================================================*/
#if defined (OSA_C_SERVICE_INIT)
/***********************************************************************
*
* Name:        OS_MailboxPoolInit()
*
* Description: Initialize OS Mailbox Pool
*
* Parameters:  None
*
* Returns:     Nothing
*
* Notes:       All available mailboxes in the system are created in this function.
*
***********************************************************************/
static OSA_STATUS OS_MailboxPoolInit(OSA_ControlBlock* block)
{
	OS_MboxPool*    poolPtr;
	OSA_STATUS status;

#ifndef OSA_USE_DYNAMIC_REFS
	OS_Mbox*        mboxPtr;
	UINT8 index;
	CODE_COVERAGE_TAG;
#endif

	CODE_COVERAGE_TAG;
	OSA_ASSERT(block != NULL);

	poolPtr = &(block->mboxPool);

	/*
	**  Create semaphore used to control access to the mboxPool
	**  linked lists.
	*/
	OSA_SEMA_CREATE(&poolPtr->poolSemaphore, (char *)"MboxPool", 1, OS_FIFO, status);

	if (status != OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_FAIL);
	}

	/* initialize the list */
	poolPtr->head = NULL;

#ifndef OSA_USE_DYNAMIC_REFS
	CODE_COVERAGE_TAG;
	poolPtr->free = &(poolPtr->mboxes[0]);
	mboxPtr = poolPtr->free;

	for (index = 0; index < OSA_MBOX_QUEUES_MAX; index++)
	{
		CODE_COVERAGE_TAG;
		mboxPtr->next = mboxPtr + 1;
		mboxPtr->refCheck = NULL;
#ifdef OSA_DEBUG
		mboxPtr->controlBlock = block;
		mboxPtr->poolSema     = &poolPtr->poolSemaphore;
#endif
		mboxPtr++;
	}

	(mboxPtr - 1)->next = NULL;
#endif

	poolPtr->numCreated = 0;
	return(status);
}


/*======================================================================
 *  OSA Mailbox Services:
 *
 *   OSAMailboxQCreate() - Create a mailbox
 *   OSAMailboxQDelete() - Delete a mailbox
 *   OSAMailboxQRecv()   - Receive a message from a mailbox
 *   OSAMailboxQSend()   - Send a message to a mailbox
 *   OSAMailboxQPoll()   - Check for a message in a mailbox
 *====================================================================*/

/***********************************************************************
*
* Name:        OSAMailboxQCreate()
*
* Description: Initialize a OS_Mbox data structure from the mailbox pool.
*
* Parameters:
*  OSMailboxRef*   mboxQRef     - pointer to mailbox structure to fill
*  char            *queueName   - name of queue
*  UINT32          maxNumber    - number of bytes in task stack area
*  UINT8           waitingMode  - scheduling of waiting tasks
*                                 OS_FIFO, OS_PRIORITY
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*  OS_INVALID_MODE     - waiting mode is invalid
*  OS_NO_MBOXES        - no mailboxes left in the system
*  OS_FAIL             - OS specific error
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSAMailboxQCreate( OSMailboxQRef *mboxQRef,
#ifdef OSA_QUEUE_NAMES
			      char *queueName,
#endif
			      UINT32 maxNumber, UINT8 waitingMode )
{
	OSA_STATUS status;
	OS_Mbox         *queuePtr = (OS_Mbox *)mboxQRef;
	OS_MboxPool     *poolPtr;
	OSA_ControlBlock* block;

	CODE_COVERAGE_TAG;
	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	poolPtr = &block->mboxPool;

#ifndef OSA_QUEUE_NAMES
	CODE_COVERAGE_TAG;
	char *queueName = "OsaMbQ";
	sprintf(queueName, "%d", poolPtr->numCreated);
#endif

#ifndef OSA_USE_DYNAMIC_REFS
	CODE_COVERAGE_TAG;
	/* Check if the pool is full. */
	if (poolPtr->numCreated == OSA_MBOX_QUEUES_MAX)
	{
		OSA_ASSERT_EXT(FALSE);
		return(OS_NO_MBOXES);
	}
#endif

#ifdef OSA_DEBUG
	CODE_COVERAGE_TAG;
	if (mboxQRef == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_REF);
	}

	if (queueName == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_POINTER);
	}
	if (maxNumber == 0)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_NO_RESOURCES);
	}
	if ( (waitingMode != OSA_FIFO) && (waitingMode != OSA_PRIORITY) )
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_MODE);
	}
#endif

	/* Get a free reference */
	OSA_GET_REF(poolPtr, queuePtr, sizeof(OS_Mbox));
	if(queuePtr == OS_NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_INVALID_PTR);
	}
	/* Create the message queue */
	OSA_MAILBOX_CREATE(&queuePtr->os_ref, queueName, maxNumber, waitingMode, status);

	if (status == OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		queuePtr->msgCount            = 0;
#ifdef OSA_DEBUG
		CODE_COVERAGE_TAG;
		queuePtr->maxNumber           = maxNumber;
		queuePtr->controlBlock        = block;
		queuePtr->totalMsgReceived    = 0;
		queuePtr->maxMsgInQueue       = 0;
#endif

		/* Store the filled queue into user's reference */
		*mboxQRef = (OSMailboxQRef *)queuePtr;
	}
	else
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		OSA_DELETE_REF(poolPtr, queuePtr);
	}

	return(status);
}


/***********************************************************************
*
* Name:        OSAMailboxQDelete()
*
* Description: Delete a timer.
*
* Parameters:
*  OSMailboxRef    mailbox     - reference to mailbox
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*  OS_FAIL             - OS specific error
*
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSAMailboxQDelete( OSMailboxQRef mboxQRef )
{
	OSA_STATUS status;
	OS_Mbox         *mbox = (OS_Mbox *)mboxQRef;
	OS_MboxPool     *poolPtr;
	OSA_ControlBlock* block;

	CODE_COVERAGE_TAG;
	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	poolPtr = &block->mboxPool;

	/* Validate the reference */
	OSA_REF_CHECK(mboxQRef, mbox);

	OSA_MAILBOX_DELETE(mbox->os_ref, status);

	if (status == OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		OSA_DELETE_REF(poolPtr, mbox);
	}

	return(status);
}
#endif //OSA_C_SERVICE_INIT

#if defined (OSA_C_SERVICE_RUN)
/********************************************************************
 *
 * Name:        OSAMailboxQSend()
 *
 * Description: Send message to mailbox queue
 *
 * Parameters:
 *
 *   OSMailboxQRef  msgQRef  - message queue to send message to
 *   void           *msgPtr  - pointer to message
 *   UINT32         timeout  - OSA_SUSPEND, OSA_NO_SUSPEND
 *
 * Returns:
 *
 *  OS_SUCCESS          - service was completed successfully
 *  OS_INVALID_REF      - reference is NULL
 *  OS_INVALID_POINTER  - if msgPtr is zero
 *  OS_QUEUE_FULL       - if there is no room for messages on queue
 *  OS_MBOX_DELETED     - queue was deleted before service was completed
 *  OS_FAIL             - OS specific error
 *
 * Notes:
 *
 *   Messages are copied to the back of the message queue and
 *   removed from the front of the message queue.
 *
 ***********************************************************************/
OSA_STATUS OSAMailboxQSend(OSMailboxQRef mboxQRef, void* msgPtr, UINT32 timeout)
{
	OS_Mbox     *queuePtr = (OS_Mbox *)mboxQRef;
	OSA_STATUS status;

	CODE_COVERAGE_TAG;
#ifdef OSA_DEBUG
	CODE_COVERAGE_TAG;
	/* Validate the reference */
	OSA_REF_CHECK(mboxQRef, queuePtr);

	if ((timeout == OSA_NO_SUSPEND) && (queuePtr->msgCount == queuePtr->maxNumber))
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_QUEUE_FULL);
	}
	if (msgPtr == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_POINTER);
	}
#endif

	OSA_MAILBOX_SEND(queuePtr->os_ref, (UINT8 *)&msgPtr, timeout, status);
	OSA_ASSERT_EXT_TO(status == OS_SUCCESS);

	if (status == OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		OSA_CONTEXT_LOCK;

		/* Increment Queue related statistics */
		queuePtr->msgCount++;

#ifdef OSA_DEBUG
		/* Update the message statistics */
		queuePtr->totalMsgReceived++;

		if (queuePtr->msgCount > queuePtr->maxMsgInQueue)
		{
			CODE_COVERAGE_TAG;
			queuePtr->maxMsgInQueue = queuePtr->msgCount;
		}

		/* Increment System related statistics */
		queuePtr->controlBlock->stats.sysMboxCount++;
		queuePtr->controlBlock->stats.sysTotalMboxReceived++;

		if (queuePtr->controlBlock->stats.sysMboxCount >
		    queuePtr->controlBlock->stats.sysMaxMboxMsgs)
		{
			CODE_COVERAGE_TAG;
			queuePtr->controlBlock->stats.sysMaxMboxMsgs =
				queuePtr->controlBlock->stats.sysMboxCount;
		}

#endif          /* OS Debug statistics */

		OSA_CONTEXT_UNLOCK;
	}

	if (status == OS_DELETED)
	{
		CODE_COVERAGE_TAG;
		return(OS_MBOX_DELETED);
	}

	return(status);
}

/***********************************************************************
*
* Name:        OSAMailboxQRecv()
*
* Description: Get pointer to message at the top of the mailbox queue.
*
* Parameters:
*
*   OSMailboxQRef mboxRef   - reference to the queue that will be
*                             checked for data.
*   void**        recvMsg   - pointer to received message or NULL if
*                             there are no messages on the queue.
*   UINT32        timeout   - OSA_SUSPEND, OSA_NO_SUSPEND, or timeout
*                             1 - 4,294,967,293
*
* Returns:
*
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*  OS_INVALID_POINTER  - if msgPtr is zero
*  OS_QUEUE_EMPTY      - message queue is empty
*  OS_TIMEOUT          - timeout while suspended waiting for a queue
*  OS_MBOX_DELETED     - queue was deleted before service was completed
*  OS_FAIL             - OS specific error
*
* Notes:
*
*   A task may call this function at any time; it does not need to
*   wait for an event.
*
*
***********************************************************************/
OSA_STATUS OSAMailboxQRecv(OSMailboxQRef mboxQRef, void** recvMsg, UINT32 timeout)
{
	OS_Mbox          *queuePtr = (OS_Mbox *)mboxQRef;
	OSA_STATUS status;

	CODE_COVERAGE_TAG;
#ifdef OSA_DEBUG
	CODE_COVERAGE_TAG;
	/* Validate the reference */
	OSA_REF_CHECK(mboxQRef, queuePtr);

	if (recvMsg == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_INVALID_POINTER);
	}
#endif

	OSA_CONTEXT_LOCK;
	if ( (timeout == OSA_NO_SUSPEND) && (queuePtr->msgCount == 0L) )
	{
		CODE_COVERAGE_TAG;
		OSA_CONTEXT_UNLOCK;
		return(OS_QUEUE_EMPTY);
	}

	OSA_CONTEXT_UNLOCK;

	OSA_MAILBOX_RECV(queuePtr->os_ref, (UINT8 *)recvMsg, timeout, status);
	OSA_ASSERT_EXT_TO(status == OS_SUCCESS);

	if (status == OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		OSA_CONTEXT_LOCK;
		/* Decrement message count stat */
		queuePtr->msgCount--;

#ifdef OSA_DEBUG
		CODE_COVERAGE_TAG;
		/* Decrement System related statistic */
		queuePtr->controlBlock->stats.sysMboxCount--;
#endif
		OSA_CONTEXT_UNLOCK;
	}

	if (status == OS_DELETED)
	{
		CODE_COVERAGE_TAG;
		return(OS_MBOX_DELETED);
	}

	return (status);
}

/***********************************************************************
*
* Name:        OSAMailboxQPoll()
*
* Description: Check for messages on the queue without taking any off
*              the queue.
*
* Parameters:
*
*   OSMailboxQRef mboxqRef  - reference to the queue that will be
*                             checked for data.
*   UINT32        *msgCount - stored count of messages on the queue
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*  OS_INVALID_POINTER  - if msgPtr is zero
*
* Notes:
*
*
***********************************************************************/
OSA_STATUS OSAMailboxQPoll(OSMailboxQRef mboxQRef, UINT32 *msgCount)
{
	OS_Mbox          *queuePtr = (OS_Mbox *)mboxQRef;
	OSA_STATUS status;

	CODE_COVERAGE_TAG;
#ifdef OSA_DEBUG
	CODE_COVERAGE_TAG;
	/* Validate the reference */
	OSA_REF_CHECK(mboxQRef, queuePtr);

	if (msgCount == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_POINTER);
	}
#endif

	OSA_MAILBOX_POLL(queuePtr->os_ref, msgCount, status);

	/* RTT does not support this so check internal counter */
	if (status == OS_UNSUPPORTED)
	{
		CODE_COVERAGE_TAG;
		*msgCount = queuePtr->msgCount;
		return (OS_SUCCESS);
	}

	return (status);
}
#endif  //OSA_C_SERVICE_RUN
#endif  //OSA_MBOX_QUEUES


#if (OSA_FLAGS)
/*======================================================================
 *  OSA Flag Management
 *====================================================================*/
#if defined (OSA_C_SERVICE_INIT)
/***********************************************************************
*
* Name:        OS_FlagPoolInit()
*
* Description: Initialize data structures for event pools.
*
* Parameters:
*
*
* Returns: Nothing
*
* Notes:
*
*
***********************************************************************/
static OSA_STATUS OS_FlagPoolInit(OSA_ControlBlock* block)
{
	OS_FlagPool     *poolPtr;
	OSA_STATUS status;

#ifndef OSA_USE_DYNAMIC_REFS
	OS_Flag         *flag;
	UINT8 index;
#endif

	CODE_COVERAGE_TAG;
	OSA_ASSERT(block != NULL);
	poolPtr = &(block->eventPool);

	/*
	**  Create semaphore used to control access to the flag Pool
	**  linked lists.
	*/
	OSA_SEMA_CREATE(&poolPtr->poolSemaphore, (char *)"FlagPool", 1, OS_FIFO, status);

	if (status != OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_FAIL);
	}

	/* initialize the list */
	poolPtr->head = NULL;

#ifndef OSA_USE_DYNAMIC_REFS
	CODE_COVERAGE_TAG;
	poolPtr->free = &(poolPtr->eventGroups[0]);
	flag = poolPtr->free;

	for (index = 0; index < OSA_FLAGS_MAX; index++)
	{
		CODE_COVERAGE_TAG;
		flag->next = flag + 1;
		flag->refCheck = NULL;
		flag++;
	}

	(flag - 1)->next = NULL;
#endif

	poolPtr->numCreated = 0;

	/* Symbian needs a unique identifier for each flag name */
	poolPtr->counter = 0;
	return(status);
}


/*======================================================================
 *  OSA Event Flag Services
 *
 *   OSAFlagCreate() - Create event flag group
 *   OSAFlagCreateGlobal()  - Create global event flag group
 *   OSAFlagDelete() - Delete event flag group
 *   OSAFlagSet()    - Set event flag
 *   OSAFlagWait()   - Wait for event flags to be set
 *   OSAFlagPeek()   - Check event flag
 *====================================================================*/

/***********************************************************************
*
* Name:        OSAflagcreate()
*
* Description: Initialize data structures for IPC.
*
* Parameters:
*   OSFlagRef *flagRef         - reference to the flag created
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*  OS_NO_FLAGS         - no flags available in the system
*
* Notes:
*
*
***********************************************************************/
OSA_STATUS OSAFlagCreate(OSFlagRef* flagRef)
{
	OS_FlagPool *poolPtr;
	OS_Flag     *flag = 0;
	OSA_STATUS status;
	UINT8 name[OSA_NAME_SIZE + 1];
	OSA_ControlBlock* block;

	CODE_COVERAGE_TAG;
	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	poolPtr = &block->eventPool;

#ifndef OSA_USE_DYNAMIC_REFS
	CODE_COVERAGE_TAG;
	/* Check if the flag pool is full. */
	if (poolPtr->numCreated == OSA_FLAGS_MAX)
	{
		OSA_ASSERT_EXT(FALSE);
		return(OS_NO_FLAGS);
	}
#endif

#ifdef OSA_DEBUG
	CODE_COVERAGE_TAG;
	if (flagRef == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_REF);
	}

#endif

	/* Get a free reference */
	OSA_GET_REF(poolPtr, flag, sizeof(OS_Flag));
	if(flag == OS_NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_INVALID_PTR);
	}
	memset((void*)&flag->os_ref, 0, sizeof(OS_Flag_t));
	memset(name, 0, OSA_NAME_SIZE + 1);
	sprintf((char*)name, "OsFlg%d", poolPtr->counter++);

	OSA_FLAG_CREATE(&flag->os_ref, (char *)name, status);

	/* Check for flag creation error */
	if ( status == OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		/* Store the queue info into the queueRef */
		*flagRef = (OSFlagRef *)flag;
	}
	else
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		OSA_DELETE_REF(poolPtr, flag);
	}

	return (status);
}

/***********************************************************************
*
* Name:        OSAFlagCreateGlobal()
*
* Description: Create or attach to a global named event flag which is
*              sharable between processes.
*
* Parameters:
*   OSFlagRef *flagRef         - reference to the flag created
*   char      *flagName        - name of flag
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*  OS_NO_FLAGS         - no flags available in the system
*
* Notes:
*
*
***********************************************************************/
OSA_STATUS OSAFlagCreateGlobal(OSFlagRef* flagRef, char* flagName)
{
	OS_FlagPool *poolPtr;
	OS_Flag     *flag = 0;
	OSA_STATUS status;
	OSA_ControlBlock* block;

	CODE_COVERAGE_TAG;
	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	poolPtr = &block->eventPool;

#ifdef OSA_DEBUG
	if (flagRef == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_REF);
	}

#endif

	/* Get a free reference */
#ifndef OSA_USE_DYNAMIC_REFS
	CODE_COVERAGE_TAG;
	OSA_GET_REF(poolPtr, flag, sizeof(OS_Flag));
	if(flag == OS_NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_INVALID_PTR);
	}
#else
	CODE_COVERAGE_TAG;
	flag = (OS_Flag*)malloc(sizeof(OS_Flag));
	flag->refCheck = (void *)flag;
#endif

	OSA_FLAG_CREATE(&flag->os_ref, flagName, status);

	/* Check for flag creation error */
	if ( status == OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		/* Store the queue info into the queueRef */
		*flagRef = (OSFlagRef *)flag;
	}
	else
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		OSA_DELETE_REF(poolPtr, flag);
	}

	return (status);
}

/***********************************************************************
*
* Name:        OSAFlagDelete()
*
* Description: Delete a flag.
*
* Parameters:
*   OSFlagRef flagRef         - flag reference
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSAFlagDelete( OSFlagRef flagRef )
{
	OS_Flag         *flag = (OS_Flag *)flagRef;
	OS_FlagPool     *poolPtr;
	OSA_STATUS status;
	OSA_ControlBlock* block;

	CODE_COVERAGE_TAG;
	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	poolPtr = &block->eventPool;

	/* Validate the reference */
	OSA_REF_CHECK(flagRef, flag);
	OSA_ASSERT(block != NULL);

	/* Delete the flag */
	OSA_FLAG_DELETE(&flag->os_ref, status);

	if (status == OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		OSA_DELETE_REF(poolPtr, flag);
	}
	return(status);
}
#endif //OSA_C_SERVICE_INIT

#if defined (OSA_C_SERVICE_RUN)
/***********************************************************************
*
* Name:        OSAFlagSet()
*
* Description: Set event flag in target task's event register
*
* Parameters:
*
*   OSFlagRef flagRef   - event flag reference who's event flags will be set
*   UINT32    mask      - flags to set
*   UINT32    operation - OSA_FLAG_AND, OSA_FLAG_OR
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*  OS_INVALID_MODE     - operation is invalid
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSAFlagSet( OSFlagRef flagRef, UINT32 mask, UINT32 operation )
{
	OS_Flag     *flag = (OS_Flag *)flagRef;
	OSA_STATUS status;

	CODE_COVERAGE_TAG;

	/* Validate the reference */
	OSA_REF_CHECK(flagRef, flag);

#ifdef OSA_DEBUG
	CODE_COVERAGE_TAG;

	if ( (operation != OSA_FLAG_AND) && (operation != OSA_FLAG_OR) )
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_MODE);
	}
#endif

	OSA_FLAG_SET_BITS(&flag->os_ref, mask, operation, status);

	return(status);
}


/***********************************************************************
*
* Name:        OSAFlagWait()
*
* Description: Block until one or more of the requested event flags
*              is set.
*
* Parameters:
*
*   OSFlagRef flagRef   - event flag to be monitored
*   UINT32    mask      - bit mask of bits to be monitored in event register
*   UINT32    operation - OSA_FLAG_AND/AND_CLEAR, OSA_FLAG_OR/OR_CLEAR
*   UINT32*   flags     - all event flags that were set in the register
*   UINT32    timeout   - OSA_SUSPEND, OSA_NO_SUSPEND, or timeout
*                         1 - 4,294,967,293
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*  OS_INVALID_MODE     - operation is invalid
*  OS_FLAG_DELETED     - service returned because event flag was deleted
*  OS_TIMEOUT          - timeout while suspended waiting
*
* Notes:
*
*
***********************************************************************/
OSA_STATUS OSAFlagWait( OSFlagRef flagRef, UINT32 mask, UINT32 operation,
			UINT32* flags, UINT32 timeout )
{
	OS_Flag     *eventPtr = (OS_Flag *)flagRef;
	OSA_STATUS status = OS_FAIL;

	CODE_COVERAGE_TAG;
#ifdef OSA_DEBUG
	CODE_COVERAGE_TAG;
	/* Validate the reference */
	OSA_REF_CHECK(flagRef, eventPtr);

	if (flags == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_INVALID_POINTER);
	}
	if ( (operation != OSA_FLAG_AND) && (operation != OSA_FLAG_AND_CLEAR) &&
	     (operation != OSA_FLAG_OR) && (operation != OSA_FLAG_OR_CLEAR) )
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_INVALID_MODE);
	}
#endif

	OSA_FLAG_WAIT(&eventPtr->os_ref, mask, operation, flags, timeout, status);

	return(status);
}


/***********************************************************************
*
* Name:        OSAFlagPeek()
*
* Description: Read the current value of the event flags.
*
* Parameters:
*
*   OSFlagRef flagRef   - event flag to be read
*   UINT32*   flags     - all event flags that were set in the register
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*
* Notes:
*
*    This function returns all set events, but does not clear any.
*
***********************************************************************/
OSA_STATUS OSAFlagPeek( OSFlagRef flagRef, UINT32* flags )
{
	OS_Flag     *eventPtr = (OS_Flag *)flagRef;
	OSA_STATUS status = OS_FAIL;

	CODE_COVERAGE_TAG;
	/* Validate the reference */
	OSA_REF_CHECK(flagRef, eventPtr);

	OSA_FLAG_PEEK(eventPtr->os_ref, flags, status);
	return(status);
}
#endif  //OSA_C_SERVICE_RUN
#endif  //OSA_FLAGS


#if (OSA_SEMAPHORES)
/*======================================================================
 *  OSA Semaphore Management
 *====================================================================*/
#if defined (OSA_C_SERVICE_INIT)
/***********************************************************************
*
* Name:        OS_SemaPoolInit()
*
* Description: Initialize OS Semaphore Pool
*
* Parameters:  None
*
* Returns:     Nothing
*
* Notes:
*
***********************************************************************/
static OSA_STATUS OS_SemaPoolInit(OSA_ControlBlock* block)
{
	OS_SemaPool *semaPool;

#ifndef OSA_USE_DYNAMIC_REFS
	OS_Sema     *sema;
	UINT16 index;
#endif
	OSA_STATUS status;

	CODE_COVERAGE_TAG;
	OSA_ASSERT(block != NULL);

	semaPool = &block->semaPool;

	OSA_SEMA_CREATE(&semaPool->poolSemaphore, (char *)"SemaPool", 1, OS_FIFO, status);

	if (status != OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_FAIL);
	}

	/* initialize the list */
	semaPool->head = NULL;

#ifndef OSA_USE_DYNAMIC_REFS
	CODE_COVERAGE_TAG;
	semaPool->free = &semaPool->semaphores[0];
	sema = semaPool->free;

	for (index = 0; index < OSA_SEMAPHORES_MAX; index++)
	{
		CODE_COVERAGE_TAG;
		sema->next = sema + 1;
		sema->refCheck = NULL;
		sema++;
	}

	(sema - 1)->next = NULL;
#endif

	semaPool->numCreated = 0;
	semaPool->counter = 0;
	return(status);
}

/*======================================================================
 *  OSA Semaphore Services
 *
 *   OSASemaphoreCreate()  - Create a semaphore
 *   OSASemaphoreCreateGlobal() - Create a semaphore
 *   OSASemaphoreDelete()  - Delete a semaphore
 *   OSASemaphoreAcquire() - Acquire the semaphore
 *   OSASemaphoreRelease() - Release the semaphore
 *   OSASemaphorePoll()    - Poll a semaphore
 *====================================================================*/

/***********************************************************************
*
* Name:        OSASemaphoreCreate()
*
* Description: Create a semaphore
*
* Parameters:
*  OSSemaRef *semaRef         - OS assigned semaphore reference
*  UINT32    initialCount     - initial count of the semaphore
*  UINT8     waitingMode      - scheduling of waiting tasks
*                               OS_FIFO, OS_PRIORITY
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*  OS_INVALID_MODE     - invalid waiting mode
*  OS_NO_SEMAPHORES    - no semaphores available in the system
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSASemaphoreCreate( OSSemaRef *semaRef, UINT32 initialCount,
			       UINT8 waitingMode )
{
	OS_Sema         *sema = 0;
	OS_SemaPool     *poolPtr;
	OSA_STATUS status;
	UINT8 name[OSA_NAME_SIZE + 1];
	OSA_ControlBlock* block;

	CODE_COVERAGE_TAG;
	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	poolPtr = &block->semaPool;

#ifndef OSA_USE_DYNAMIC_REFS
	CODE_COVERAGE_TAG;
	if (poolPtr->numCreated == OSA_SEMAPHORES_MAX)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_NO_SEMAPHORES);
	}
#endif

#ifdef OSA_DEBUG
	CODE_COVERAGE_TAG;
	if (semaRef == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_REF);
	}
#endif

	/* Get a free reference */
	OSA_GET_REF(poolPtr, sema, sizeof(OS_Sema));
	if(sema == OS_NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_INVALID_PTR);
	}
	memset((void*)&sema->os_ref, 0, sizeof(OS_Sema_t));
	memset(name, 0, OSA_NAME_SIZE + 1);
	sprintf((char*)name, "OsSem%d", poolPtr->counter++);

	/* Create the user's semaphore */
	OSA_SEMA_CREATE(&sema->os_ref, NULL, initialCount, waitingMode, status);

	if (status == OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		*semaRef = (OSSemaRef *)sema;
	}
	else
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		OSA_DELETE_REF(poolPtr, sema);
	}

	return(status);
}

/***********************************************************************
*
* Name:        OSASemaphoreCreateGlobal()
*
* Description: Create or attach to a global named semaphore which is
*              sharable between processes
*
* Parameters:
*  OSSemaRef *semaRef         - OS assigned semaphore reference
*  char      *semaName        - name of semaphore
*  UINT32    initialCount     - initial count of the semaphore
*  UINT8     waitingMode      - scheduling of waiting tasks
*                               OS_FIFO, OS_PRIORITY
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*  OS_INVALID_MODE     - invalid waiting mode
*  OS_NO_SEMAPHORES    - no semaphores available in the system
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSASemaphoreCreateGlobal( OSSemaRef *semaRef, char *semaName,
				     UINT32 initialCount, UINT8 waitingMode )
{
	OS_Sema         *sema = 0;
	OS_SemaPool     *poolPtr;
	OSA_STATUS status;
	OSA_ControlBlock* block;

	CODE_COVERAGE_TAG;
	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	poolPtr = &block->semaPool;

	if (poolPtr->numCreated == OSA_SEMAPHORES_MAX)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_NO_SEMAPHORES);
	}
#ifdef OSA_DEBUG
	CODE_COVERAGE_TAG;
	if (semaRef == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_REF);
	}
#endif

	/* Get a free reference */
	OSA_GET_REF(poolPtr, sema, sizeof(OS_Sema));
	if(sema == OS_NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_INVALID_PTR);
	}
	memset((void*)&sema->os_ref, 0, sizeof(OS_Sema_t));

	/* Create the user's semaphore */
	OSA_SEMA_CREATE(&sema->os_ref, semaName, initialCount, waitingMode, status);

	if (status == OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		*semaRef = (OSSemaRef *)sema;
	}
	else
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		OSA_DELETE_REF(poolPtr, sema);
	}

	return(status);
}

/***********************************************************************
*
* Name:        OSASemaphoreDelete()
*
* Description: Delete a semaphore
*
* Parameters:
*  OSSemaRef semaRef         - semaphore reference
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSASemaphoreDelete( OSSemaRef semaRef )
{
	OS_Sema         *semaPtr = (OS_Sema*)semaRef;
	OS_SemaPool     *poolPtr;
	OSA_STATUS status;
	OSA_ControlBlock* block;

	CODE_COVERAGE_TAG;
	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	poolPtr = &block->semaPool;

	/* Validate the reference */
	OSA_REF_CHECK(semaRef, semaPtr);

	/* Call the OS specific function */
	OSA_SEMA_DELETE(semaPtr->os_ref, status);

	if (status == OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		OSA_DELETE_REF(poolPtr, semaPtr);
	}

	return(status);
}
#endif //OSA_C_SERVICE_INIT

#if defined (OSA_C_SERVICE_RUN)
/************************************************************************
 *
 * Name:        OSASemaphoreAcquire()
 *
 * Description: Block calling task until lock is acquired
 *
 * Parameters:
 *  OSSemaRef semaRef         - semaphore reference
 *  UINT32    timeout         - OSA_SUSPEND, OSA_NO_SUSPEND, or timeout
 *                              from 1 - 4,294,967,293 ticks
 *
 * Returns:
 *  OS_SUCCESS          - service was completed successfully
 *  OS_INVALID_REF      - reference is NULL
 *  OS_UNAVAILABLE      - semaphore is not available
 *  OS_SEM_DELETED      - semaphore was deleted before it could be acquired
 *  OS_TIMEOUT          - timeout while suspended waiting for the semaphore
 *  OS_FAIL             - OS specific error
 *
 * Notes:
 *
 *
 ***********************************************************************/
OSA_STATUS OSASemaphoreAcquire(OSSemaRef semaRef, UINT32 timeout)
{
	OS_Sema     *semaPtr = (OS_Sema*)semaRef;
	OSA_STATUS status;

	CODE_COVERAGE_TAG;
	/* Validate the reference */
	OSA_REF_CHECK(semaRef, semaPtr);

	OSA_SEMA_ACQUIRE(semaPtr->os_ref, timeout, status);
	OSA_ASSERT_EXT_TO(status == OS_SUCCESS);

	if (status == OS_DELETED)
	{
		CODE_COVERAGE_TAG;
		return(OS_SEM_DELETED);
	}

	return(status);
}


/***********************************************************************
*
* Name:        OSASemaphoreRelease()
*
* Description: Release the semaphore.
*
* Parameters:
*  OSSemaRef semaRef         - semaphore reference
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*
* Notes:
*
*
***********************************************************************/
OSA_STATUS OSASemaphoreRelease(OSSemaRef semaRef)
{
	OS_Sema     *semaPtr = (OS_Sema*)semaRef;
	OSA_STATUS status;

	CODE_COVERAGE_TAG;
	/* Validate the reference */
	OSA_REF_CHECK(semaRef, semaPtr);

	OSA_SEMA_RELEASE(semaPtr->os_ref, status);

	return(status);
}


/***********************************************************************
*
* Name:        OSASemaphorePoll()
*
* Description: Poll the semaphore.
*
* Parameters:
*  OSSemaRef semaRef         - semaphore reference
*  UINT32    count           - semaphore count
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*
* Notes:
*
*
***********************************************************************/
OSA_STATUS OSASemaphorePoll(OSSemaRef semaRef, UINT32* count)
{
	OS_Sema     *semaPtr = (OS_Sema*)semaRef;
	OSA_STATUS status;

	CODE_COVERAGE_TAG;
#ifdef OSA_DEBUG
	CODE_COVERAGE_TAG;
	/* Validate the reference */
	OSA_REF_CHECK(semaRef, semaPtr);

	if (count == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT(FALSE);
		return(OS_INVALID_POINTER);
	}
#endif

	OSA_SEMA_POLL(semaPtr->os_ref, count, status);

	return(status);
}
#endif  //OSA_C_SERVICE_RUN
#endif  //OSA_SEMAPHORES


#if (OSA_MUTEXES)
/*======================================================================
 *  OSA Mutex Management
 *====================================================================*/
#if defined (OSA_C_SERVICE_INIT)
/***********************************************************************
*
* Name:        OS_MutexPoolInit()
*
* Description: Initialize OS Mutex Pool
*
* Parameters:  None
*
* Returns:     Nothing
*
* Notes:
*
***********************************************************************/
static OSA_STATUS OS_MutexPoolInit(OSA_ControlBlock* block)
{
	OS_MutexPool    *pool;

#ifndef OSA_USE_DYNAMIC_REFS
	OS_Mutex        *mutex;
	UINT32 index;
#endif
	OSA_STATUS status;

	CODE_COVERAGE_TAG;
	OSA_ASSERT(block != NULL);

	pool = &block->mutexPool;

	OSA_SEMA_CREATE(&pool->poolSemaphore, (char *)"mutxPool", 1, OS_FIFO, status);

	if (status != OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_FAIL);
	}

	/* initialize the list */
	pool->head = NULL;

#ifndef OSA_USE_DYNAMIC_REFS
	CODE_COVERAGE_TAG;
	pool->free = &pool->mutexes[0];
	mutex = pool->free;

	for (index = 0; index < OSA_MUTEXES_MAX; index++)
	{
		CODE_COVERAGE_TAG;
		mutex->owner           = NULL;
		mutex->basePriority    = OSA_LOWEST_PRIORITY;
		mutex->currentPriority = OSA_LOWEST_PRIORITY;
		mutex->highestWaiting  = OSA_LOWEST_PRIORITY;
		mutex->numTasks        = 0;
		mutex->next            = mutex + 1;
		mutex->refCheck        = NULL;
		mutex++;
	}

	(mutex - 1)->next = NULL;
#endif

	pool->numCreated = 0;
	pool->counter = 0;
	return(status);
}


/*======================================================================
 *  OS Mutex Services
 *
 *   OSAMutexCreate()  - Create a mutex
 *   OSAMutexDelete()  - Delete a mutex
 *   OSAMutexLock()    - Lock the mutex
 *   OSAMutexUnlock()  - Unloc the mutex
 *====================================================================*/

/***********************************************************************
*
* Name:        OSAMutexCreate()
*
* Description: Create a mutex
*
* Parameters:
*  OSMutexRef *mutexRef       - OS assigned mutex reference
*  UINT8      waitingMode     - scheduling of waiting tasks
*                               OS_FIFO, OS_PRIORITY
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*  OS_INVALID_MODE     - invalid waiting mode
*  OS_NO_MUTEXES       - no mutexes available in the system
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSAMutexCreate( OSMutexRef *mutexRef, UINT8 waitingMode )
{
	OS_Mutex        *mutex = 0;
	OS_MutexPool    *poolPtr;
	OSA_STATUS status;
	UINT8 name[OSA_NAME_SIZE + 1];
	OSA_ControlBlock* block;

	CODE_COVERAGE_TAG;
	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	poolPtr = &block->mutexPool;

#ifndef OSA_USE_DYNAMIC_REFS
	CODE_COVERAGE_TAG;
	if (poolPtr->numCreated == OSA_MUTEXES_MAX)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_NO_MUTEXES);
	}
#endif

#ifdef OSA_DEBUG
	CODE_COVERAGE_TAG;
	if (mutexRef == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_INVALID_REF);
	}
	if ((waitingMode != OS_FIFO) && (waitingMode != OS_PRIORITY))
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return (OS_INVALID_MODE);
	}
#else
	UNUSEDPARAM(waitingMode)
#endif

	/* Get a free reference */
	OSA_GET_REF(poolPtr, mutex, sizeof(OS_Mutex));
	if(mutex == OS_NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_INVALID_PTR);
	}
	memset((void*)&mutex->os_ref, 0, sizeof(OS_Mutex_t));
	memset(name, 0, OSA_NAME_SIZE + 1);
	sprintf((char*)name, "OsMut%d", poolPtr->counter++);

	/* Create the user's mutex */
	OSA_MUTEX_CREATE(&mutex->os_ref, NULL, waitingMode, status);

	if (status == OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		mutex->owner = NULL;
		mutex->numTasks = 0;
		mutex->basePriority    = OSA_LOWEST_PRIORITY;
		mutex->currentPriority = OSA_LOWEST_PRIORITY;
		mutex->highestWaiting  = OSA_LOWEST_PRIORITY;
		*mutexRef = (OSMutexRef *)mutex;
	}
	else
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		OSA_DELETE_REF(poolPtr, mutex);
	}

	return(status);
}


/***********************************************************************
*
* Name:        OSAMutexDelete()
*
* Description: Delete a mutex
*
* Parameters:
*  OSMutexRef mutexRef       - mutex reference
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSAMutexDelete( OSMutexRef mutexRef )
{
	OS_Mutex        *mutex = (OS_Mutex*)mutexRef;
	OS_MutexPool    *poolPtr;
	OSA_STATUS status;
	OSA_ControlBlock* block;

	CODE_COVERAGE_TAG;
	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	poolPtr = &block->mutexPool;

	/* Validate the reference */
	OSA_REF_CHECK(mutexRef, mutex);

	/* Call the OS specific function */
	OSA_MUTEX_DELETE(mutex->os_ref, status);

	if (status == OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		mutex->owner = NULL;
		mutex->numTasks = 0;
		OSA_DELETE_REF(poolPtr, mutex);
	}

	return(status);
}
#endif //OSA_C_SERVICE_INIT

#if defined (OSA_C_SERVICE_RUN)
/************************************************************************
 *
 * Name:        OSAMutexLock()
 *
 * Description: Lock the mutex.
 *
 * Parameters:
 *  OSMutexRef mutexRef       - mutex reference
 *  UINT32     timeout        - OSA_SUSPEND, OSA_NO_SUSPEND, or timeout
 *                              from 1 - 4,294,967,293 ticks
 *
 * Returns:
 *  OS_SUCCESS          - service was completed successfully
 *  OS_INVALID_REF      - reference is NULL
 *  OS_UNAVAILABLE      - mutex is not available
 *  OS_SEM_DELETED      - mutex was deleted before it could be locked
 *  OS_TIMEOUT          - timeout while suspended waiting for the mutex
 *  OS_FAIL             - calling task already has the mutex locked
 *
 * Notes:
 *
 *
 ***********************************************************************/
OSA_STATUS OSAMutexLock(OSMutexRef mutexRef, UINT32 timeout)
{
	OS_Mutex    *mutex = (OS_Mutex*)mutexRef;
	OSA_STATUS status;

#if !(OSA_WINCE || OSA_SYMBIAN || OSA_LINUX)
	OSA_STATUS rtn;
	UINT8 ownerPriority;
	UINT8 userPriority;
	UINT8 oldPriority;
	OS_Task_t   *currentTask;
#endif

	CODE_COVERAGE_TAG;
	/* Validate the reference */
	OSA_REF_CHECK(mutexRef, mutex);

#if (OSA_WINCE || OSA_SYMBIAN || OSA_LINUX) /* WinCE & Symbian have mutex capabilities so we don't need to do it */
	CODE_COVERAGE_TAG;
	OSA_MUTEX_LOCK(mutex->os_ref, timeout, status);
#else

	OSA_CONTEXT_LOCK;

	/* If the mutex is already locked check if priority inheritance
	 * is required.
	 */
	if (mutex->numTasks)
	{
		CODE_COVERAGE_TAG;
		if (mutex->owner == NULL)
		{
			CODE_COVERAGE_TAG;
			return(OS_FAIL);
		}

		/* Check if this task already has this mutex locked */
		OSA_TASK_EQUAL(mutex->owner, rtn);
		if (rtn)
		{
			CODE_COVERAGE_TAG;
			OSA_CONTEXT_UNLOCK;
			return(OS_FAIL);
		}

		mutex->numTasks++;
		OSA_TASK_IDENTIFY(currentTask);
		OSA_TASK_GET_PRIORITY(currentTask, &userPriority, rtn);

		/* If this task's priority is higher than owner's priority
		 * than set the owner's priority to same as this task. Otherwise
		 * check if we need to adjust the highest task waiting priority.
		 */
		if (userPriority < mutex->currentPriority)
		{
			/* This task has higher priority than owner so match the
			 * owner's priority to this task's priority.
			 */
			CODE_COVERAGE_TAG;
			mutex->currentPriority = userPriority;
			OSA_TASK_CHANGE_PRIORITY(mutex->owner, userPriority, &oldPriority, rtn);
			OSA_ASSERT(rtn == OS_SUCCESS);
			mutex->currentPriority = userPriority;
			mutex->highestWaiting  = userPriority;
		}
		else if (userPriority < mutex->highestWaiting)
		{
			CODE_COVERAGE_TAG;
			mutex->highestWaiting = userPriority;
		}

		OSA_CONTEXT_UNLOCK;

		OSA_MUTEX_LOCK(mutex->os_ref, timeout, status);

		OSA_CONTEXT_LOCK;

		if (status == OS_SUCCESS)
		{
			CODE_COVERAGE_TAG;
			/* Current task is now the owner of the mutex */
			OSA_TASK_IDENTIFY(mutex->owner);

			/* We are the only task left waiting for the mutex */
			if (mutex->numTasks == 1)
			{
				CODE_COVERAGE_TAG;
				mutex->currentPriority = userPriority;
				mutex->highestWaiting  = userPriority;
			}

			/* Match this tasks priority to the highest waiting task */
			if (userPriority > mutex->highestWaiting)
			{
				CODE_COVERAGE_TAG;
				OSA_TASK_CHANGE_PRIORITY(mutex->owner, mutex->highestWaiting, &oldPriority, rtn);
			}

			/* Store this tasks original priority so it can be changed back */
			mutex->basePriority = userPriority;
		}
		else
		{
			CODE_COVERAGE_TAG;
			mutex->numTasks--;

			if (status == OS_DELETED)
			{
				CODE_COVERAGE_TAG;
				status = OS_MUTEX_DELETED;
				OSA_ASSERT_EXT(FALSE);
			}
		}
	}
	else /* Mutex not locked so just lock it and store our priority */
	{
		CODE_COVERAGE_TAG;
		OSA_MUTEX_LOCK(mutex->os_ref, timeout, status);

		if (status == OS_SUCCESS)
		{
			CODE_COVERAGE_TAG;
			OSA_TASK_IDENTIFY(mutex->owner);
			OSA_ASSERT(mutex->owner != NULL);
			OSA_TASK_GET_PRIORITY(mutex->owner, &ownerPriority, rtn);
			mutex->basePriority    = ownerPriority;
			mutex->currentPriority = ownerPriority;
			mutex->numTasks++;
		}

		if (status == OS_DELETED)
		{
			CODE_COVERAGE_TAG;
			status = OS_MUTEX_DELETED;
			OSA_ASSERT_EXT(FALSE);
		}
	}

	OSA_CONTEXT_UNLOCK;
#endif  /* !OSA_WINCE || OSA_SYMBIAN */

	return(status);
}


/***********************************************************************
*
* Name:        OSAMutexUnlock()
*
* Description: Unlock the Mutex.
*
* Parameters:
*  OSMutexRef mutexRef         - mutex reference
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*  OS_FAIL             - calling task is not the owner of the mutex
*
* Notes:
*
*
***********************************************************************/
OSA_STATUS OSAMutexUnlock(OSMutexRef mutexRef)
{
	OS_Mutex    *mutex = (OS_Mutex*)mutexRef;
	OSA_STATUS status;

#if !(OSA_WINCE || OSA_SYMBIAN || OSA_LINUX)
	OSA_STATUS rtn;
	UINT8 oldPriority;
#endif

	CODE_COVERAGE_TAG;
	/* Validate the reference */
	OSA_REF_CHECK(mutexRef, mutex);

#if (OSA_WINCE || OSA_SYMBIAN || OSA_LINUX) /* WinCE & Symbain have mutex capabilities so we don't need to do it */
	CODE_COVERAGE_TAG;
	OSA_MUTEX_UNLOCK(mutex->os_ref, status);
#else

	OSA_CONTEXT_LOCK;

	/* Make sure only the owner can unlock the mutex */
	OSA_TASK_EQUAL(mutex->owner, rtn);
	if (rtn != TRUE)
	{
		CODE_COVERAGE_TAG;
		OSA_CONTEXT_UNLOCK;
		return(OS_FAIL);
	}

	OSA_MUTEX_UNLOCK(mutex->os_ref, status);

	if (status == OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		mutex->numTasks--;
		/* Change the owner's priority back to what it was originally */
		OSA_TASK_CHANGE_PRIORITY(mutex->owner, mutex->basePriority, &oldPriority, rtn);
		mutex->owner  = NULL;
	}

	OSA_CONTEXT_UNLOCK;

#endif  /* !OSA_WINCE */

	return(status);
}
#endif  //OSA_C_SERVICE_RUN
#endif  //OSA_MUTEXES


#if (OSA_TIMERS)
/*======================================================================
 *  OSA Timer Management
 *====================================================================*/
#if defined (OSA_C_SERVICE_INIT)
/***********************************************************************
*
* Name:        OS_TimerPoolInit()
*
* Description: Initialize OS Timer Pool
*
* Parameters:  None
*
* Returns:     Nothing
*
* Notes:       All available timers in the system are created in this function
*              with a disabled setting.
*              In OSATimerStart, a free (disabled) timer will be enabled
*              (started), this timer can be stopped by OSATimerStop.
*              As long as the timer is stopped and it has been created,
*              it can be reused by another call of OSATimerStart.
*
***********************************************************************/
static OSA_STATUS OS_TimerPoolInit(OSA_ControlBlock* block)
{
#ifndef OSA_USE_DYNAMIC_REFS
	UINT32 index;
	OS_Timer        *timerPtr;
#endif
	OS_TimerPool    *timerPoolPtr;
	OSA_STATUS status;

	CODE_COVERAGE_TAG;
	OSA_ASSERT(block != NULL);

	timerPoolPtr = &(block->timerPool);

	/*
	**  Create semaphore used to control access to timerPool
	**  linked lists.
	*/
	OSA_SEMA_CREATE(&timerPoolPtr->poolSemaphore, (char *)"TmrPool", 1, OS_FIFO, status);

	if (status != OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_FAIL);
	}

	/* initialize the list */
	timerPoolPtr->head = NULL;

#ifndef OSA_USE_DYNAMIC_REFS
	CODE_COVERAGE_TAG;
	timerPoolPtr->free = &(timerPoolPtr->timers[0]);
	timerPtr = timerPoolPtr->free;

	for (index = 0; index < OSA_TIMERS_MAX; index++)
	{
		CODE_COVERAGE_TAG;
		OSA_TIMER_CREATE(&(timerPtr->os_ref), status);

		OSA_ASSERT(status == OS_SUCCESS);
		timerPtr->next     = timerPtr + 1;
		timerPtr->refCheck = NULL;
		timerPtr++;
	}

	(timerPtr - 1)->next = NULL;
#endif

	timerPoolPtr->numCreated = 0;
	return(status);
}


/*======================================================================
 *  OSA Timer Services:
 *
 *   OSATimerCreate()    - Create a timer
 *   OSATimerDelete()    - Delete a timer
 *   OSATimerStart()     - Start a timer
 *   OSATimerStop()      - Stop a timer
 *   OSATimerGetStatus() - Get the status of a timer
 *====================================================================*/

/***********************************************************************
*
* Name:        OSATimerCreate()
*
* Description: Initialize a OS_Timer data structure from the timer pool.
*
* Parameters:
*  OSATimerRef* timer  - pointer to timer structure to fill
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*  OS_NO_TIMERS        - no timers available in the system
*
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSATimerCreate( OSATimerRef *timerRef )
{
	OS_Timer*       timer = 0;
	OS_TimerPool    *poolPtr;
	OSA_ControlBlock* block;

	CODE_COVERAGE_TAG;
	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	poolPtr = &block->timerPool;

#ifndef OSA_USE_DYNAMIC_REFS
	CODE_COVERAGE_TAG;
	if (poolPtr->numCreated == OSA_TIMERS_MAX)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_NO_TIMERS);
	}
#endif

#ifdef OSA_DEBUG
	CODE_COVERAGE_TAG;
	if (timerRef == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_INVALID_REF);
	}
#endif

	/* Get a free reference */
	OSA_GET_REF(poolPtr, timer, sizeof(OS_Timer));
	if(timer == OS_NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_INVALID_PTR);
	}
	/* Change the state of the timer */
	timer->state = OS_TIMER_CREATED;

	*timerRef = (OSATimerRef *)timer;

	return(OS_SUCCESS);
}


/***********************************************************************
*
* Name:        OSATimerDelete()
*
* Description: Delete a timer.
*
* Parameters:
*  OSATimerRef  timerRef       - pointer to timer structure to delete
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSATimerDelete( OSATimerRef timerRef )
{
	OSA_STATUS rtn = 0;
	OS_Timer*       timer = (OS_Timer *)timerRef;
	OS_TimerPool    *poolPtr;
	OSA_ControlBlock* block;

	CODE_COVERAGE_TAG;
	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	poolPtr = &block->timerPool;

	/* Validate the reference */
	OSA_REF_CHECK(timerRef, timer);

	switch (timer->state)
	{
	case OS_TIMER_INACTIVE:
		CODE_COVERAGE_TAG;
		OSA_TIMER_DELETE(timer->os_ref, rtn);
		timer->state = OS_TIMER_DEAD;
		OSA_DELETE_REF(poolPtr, timer);
		rtn = OS_SUCCESS;
		break;

	/* Timer must be inactive or just created to be deleted */
	case OS_TIMER_ACTIVE:
		CODE_COVERAGE_TAG;
		rtn = OS_FAIL;
		break;

	case OS_TIMER_CREATED:
		CODE_COVERAGE_TAG;
		timer->state = OS_TIMER_DEAD;
		OSA_DELETE_REF(poolPtr, timer);
		rtn = OS_SUCCESS;
		break;

	default:
		CODE_COVERAGE_TAG;
		rtn = OS_FAIL;
		break;
	}

	return(rtn);
}
#endif //OSA_C_SERVICE_INIT

#if defined (OSA_C_SERVICE_RUN)
/***********************************************************************
*
* Name:        OSATimerStart()
*
* Description: Start a timer.
*
* Parameters:
*
*   OSATimerRef timerRef       - timer reference
*   UINT32     initialTime     - initial expiration time in ticks
*   UINT32     rescheduleTime  - timer period after initial expiration
*   void*      callBackRoutine - timer call-back routine
*   UINT32     timerArgc       - argument to pass to timer call-back routine
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*  OS_INVALID_POINTER  - callBackRoutine is NULL
*
* Notes:
*
*
***********************************************************************/
OSA_STATUS OSATimerStart( OSATimerRef timerRef, UINT32 initialTime,
			  UINT32 rescheduleTime, void (*callBackRoutine)(UINT32),
			  UINT32 timerArgc )
{
	OSA_STATUS status;
	OS_Timer*    timer = (OS_Timer *)timerRef;
	OSATimerStatus timerStatus;

	CODE_COVERAGE_TAG;
#ifdef OSA_DEBUG
	/* Validate the reference */
	OSA_REF_CHECK(timerRef, timer);

	if (callBackRoutine == NULL)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_INVALID_PTR);
	}

	OSA_ASSERT(timerRef != OS_NULL);
#endif

	/* Check the state of the timer */
	OSATimerGetStatus(timerRef, &timerStatus);

	/* Do OS specific timer stuff */
	OSA_TIMER_START(timer->os_ref, initialTime, rescheduleTime,  \
			callBackRoutine, timerArgc, status);

	if (status == OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		timer->state = OS_TIMER_ACTIVE;
	}
	else
		OSA_ASSERT_EXT(FALSE);

	return(status);
}


/***********************************************************************
*
* Name:        OSATimerStop()
*
* Description: Stop a timer.
*
* Parameters:
*
*   OSATimerRef timerRef    - timer reference
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*
* Notes:
*
*
***********************************************************************/
OSA_STATUS OSATimerStop( OSATimerRef timerRef )
{
	OSA_STATUS status;
	OS_Timer*    timer = (OS_Timer *)timerRef;

	/* Validate the reference */
	OSA_REF_CHECK(timerRef, timer);


	if ( timer->state == OS_TIMER_ACTIVE )
	{
		CODE_COVERAGE_TAG;
		OSA_TIMER_STOP(timer->os_ref, status);

		if (status == OS_SUCCESS)
		{
			CODE_COVERAGE_TAG;
			timer->state = OS_TIMER_INACTIVE;
		}
	}
	else
	{
		if ( timer->state == OS_TIMER_DEAD )
		{
			CODE_COVERAGE_TAG;
			status = OS_FAIL;
		}
		else
		{
			CODE_COVERAGE_TAG;
			status = OS_SUCCESS;
		}
	}

	return(status);
}


/***********************************************************************
*
* Name:        OSATimerGetStatus()
*
* Description: Get the status of a timer.
*
* Parameters:
*
*  OSATimerRef     timerRef - timer reference
*  OSATimerStatus* status   - timer status information
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_REF      - reference is NULL
*
* Notes:
*
*
***********************************************************************/
OSA_STATUS OSATimerGetStatus( OSATimerRef timerRef, OSATimerStatus *status )
{
	OSA_STATUS rtn;
	OS_Timer*   timer = (OS_Timer *)timerRef;

	CODE_COVERAGE_TAG;
	/* Validate the reference */
	OSA_REF_CHECK(timerRef, timer);

	/* Timer was only just created */
	if (timer->state == OS_TIMER_CREATED)
	{
		CODE_COVERAGE_TAG;
		status->status = OS_DISABLED;
		status->expirations = 0;
		return(OS_SUCCESS);
	}

	OSA_TIMER_STATUS(timer->os_ref, status, rtn);

	return(rtn);
}
#endif  //OSA_C_SERVICE_RUN
#endif  //OSA_TIMERS


#if defined (OSA_C_SERVICE_RUN)
/*======================================================================
 *  Operating System Real-Time Clock Services:
 *
 *   OSAGetClockRate()  - Read the current system clock rate
 *   OSAGetTicks()      - Read the real-time clock tick counter
 *   OSATick()          - Process the real-time clock interrupt
 *====================================================================*/
/***********************************************************************
*
* Name:        OSAGetClockRate()
*
* Description: Get the current system clock rate.
*
* Parameters:  None
*
* Returns:     UNIT32  - current clock rate (ms / tick)
*
* Notes:
*
*
***********************************************************************/
UINT32 OSAGetClockRate(void)
{
	CODE_COVERAGE_TAG;
	/* Return the configured clock rate */
	return(OSA_TICK_FREQ_IN_MILLISEC);
}


/***********************************************************************
*
* Name:        OSAGetTicks()
*
* Description: Get the current system clock (ticks)
*
* Parameters:  None
*
* Returns:     UNIT32  - current system clock ticks
*
* Notes:
*
*
***********************************************************************/
UINT32 OSAGetTicks(void)
{
	UINT32 ticks;

	CODE_COVERAGE_TAG;
	OSA_GET_TIME(ticks);
	return(ticks);
}

/***********************************************************************
* Name : OSAGetSystemTime
*
* Description : Gets the current time relative to system clock ticks
*
* Parameters :
*    UINT32 secsPtr        Pointer to the seconds
*    UINT16 milliSecsPtr   Pointer to the milli-seconds
*
* Returns      :
*      OS_SUCCESS          The service is completed successfully
*      OS_FAIL             OS specific error
*
***********************************************************************/

OSA_STATUS OSAGetSystemTime( UINT32 *secsPtr, UINT16 *milliSecsPtr)
{
	UNUSEDPARAM(secsPtr)
	UNUSEDPARAM(milliSecsPtr)
	OSA_STATUS rtn;

	CODE_COVERAGE_TAG;
	OSA_GET_SYSTEM_TIME(secsPtr, milliSecsPtr, rtn);
	return rtn;
}

/***********************************************************************
*
* Name:        OSATick()
*
* Description: Process operating system real-time clock interrupt
*
* Parameters:  None
*
* Returns:     None
*
* Notes:
*
*
***********************************************************************/
void OSATick(void)
{
	OSA_ControlBlock *block = NULL;

	CODE_COVERAGE_TAG;
	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	OSA_CLOCK_TICK(block);
}
#endif //OSA_C_SERVICE_RUN


#if (OSA_INTERRUPTS)
/*======================================================================
 *  OSA ISR Management
 *====================================================================*/
#if defined (OSA_C_SERVICE_INIT)
/***********************************************************************
*
* Name:        OS_IsrPoolInit()
*
* Description: Initialize OS ISR Pool
*
* Parameters:  None
*
* Returns:     Nothing
*
* Notes:
*
***********************************************************************/
static void OS_IsrPoolInit(OSA_ControlBlock* block)
{
	OS_IsrPool  *pool;

	CODE_COVERAGE_TAG;
	OSA_ASSERT(block != NULL);

	pool = &block->isrPool;

	pool->numCreated = 0;
	return;
}

/*======================================================================
 *  OSA Interrupt Control:
 *
 *   OSAIsrCreate()      - Installs a routine to an IRQ
 *   OSAIsrDelete()      - Restore IRQ to default service routine
 *   OSAIsrDisable()     - Disable processor IRQ Exceptions
 *   OSAIsrEnable()      - Enable processor IRQ Exceptions
 *====================================================================*/

/***********************************************************************
*
* Name:        OSAIsrCreate()
*
* Description: Install an ISR rountine to an IRQ
*
* Parameters:
*  UINT32  isrNum              - interrupt number to attach ISR to (0 - 31)
*  void    *fisrRoutine(UINT8) - first level interrupt routine
*  void    *sisrRoutine(void)  - second level interrupt routine
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_VECTOR   - invalid interrupt vector
*  OS_INVALID_POINTER  - function pointer is NULL
*
* Notes:       First level ISR will be called when the specified
*              interrupt is asserted. The second level ISR will be
*              executed once for each time isrNotify(isrNum) is called
*              from the first level interrupt handler.
*
***********************************************************************/
OSA_STATUS OSAIsrCreate(UINT32 isrNum, void (*fisrRoutine)(UINT32),
			void (*sisrRoutine)(void))
{
	OSA_STATUS status = OS_SUCCESS;
	OSA_ControlBlock *block = NULL;

#ifdef OSA_NU_TARGET
	OS_IsrPool       *pool;
#endif
	CODE_COVERAGE_TAG;
#ifdef OSA_NU_TARGET
	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	CODE_COVERAGE_TAG;
	pool = &block->isrPool;

	CODE_COVERAGE_TAG;
	if (pool->numCreated == INTC_MAX_INTERRUPT_SOURCES)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_NO_INTERRUPTS);
	}

	/* Must install a Low level ISR first */
	if (fisrRoutine)
	{
		CODE_COVERAGE_TAG;
		if (INTCBind((INTC_InterruptSources)isrNum, fisrRoutine) != INTC_RC_OK)
		{
			CODE_COVERAGE_TAG;
			OSA_ASSERT_EXT(FALSE);
			return(OS_FAIL);
		}
	}

	/* Create the user's ISR */
	OSA_ISR_CREATE_SISR(isrNum, &pool->isrs[pool->numCreated].os_ref, sisrRoutine, \
			    OSA_SISR_PRIORITY, &pool->stack[OSA_SISR_STACK_SIZE * pool->numCreated],  \
			    OSA_SISR_STACK_SIZE, status);

	if (status == OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		pool->isrs[pool->numCreated].isrNum = isrNum;
		pool->numCreated++;
	}
	else
		OSA_ASSERT_EXT(FALSE);
#endif

	return(status);
}


/***********************************************************************
*
* Name:        OSAIsrdelete()
*
* Description: Reset the requested interrupt vector to use the default
*              ISR routine.
*
* Parameters:
*  UINT32  isrNum          - interrupt number to default ISR to (0 - 31)
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_VECTOR   - invalid interrupt vector
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSAIsrDelete(UINT32 isrNum)
{
#ifdef OSA_NU_TARGET
	OS_IsrPool       *pool;
#endif
	OSA_STATUS status = OS_SUCCESS;
	OSA_ControlBlock *block = NULL;

	CODE_COVERAGE_TAG;
	if (isrNum > INTC_MAX_INTERRUPT_SOURCES)
	{
		CODE_COVERAGE_TAG;
		return(OS_INVALID_VECTOR);
	}

#ifdef OSA_NU_TARGET
	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	CODE_COVERAGE_TAG;
	OSA_ASSERT(block != NULL);
	pool = &block->isrPool;

	/* Must uninstall the Low level ISR */
	if (INTCUnbind((INTC_InterruptSources)isrNum) != INTC_RC_OK)
	{
		CODE_COVERAGE_TAG;
		return(OS_FAIL);
	}

	/* Call the OS specific function */
	OSA_ISR_DELETE_SISR(&pool->isrs[isrNum].os_ref, status);

	if (status == OS_SUCCESS)
	{
		CODE_COVERAGE_TAG;
		/* Decrease the number of created isrs */
		pool->numCreated--;
		pool->isrs[pool->numCreated].isrNum = 0;
	}
#endif

	return(status);
}
#endif //OSA_C_SERVICE_INIT

#if defined (OSA_C_SERVICE_RUN)
/***********************************************************************
*
* Name:        OSAIsrNotify()
*
* Description: Activate the second level interrupt service routine for
*              the specified interrupt number.
*
* Parameters:
*  UINT32  isrNum          - interrupt number to activate (0 - 31)
*
* Returns:
*  OS_SUCCESS          - service was completed successfully
*  OS_INVALID_VECTOR   - invalid interrupt vector
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSAIsrNotify(UINT32 isrNum)
{
	OSA_STATUS status = OS_FAIL;
	OSA_ControlBlock *block = NULL;

#ifdef OSA_NU_TARGET
	OS_IsrPool      *pool;
	UINT8 i;
#endif
	CODE_COVERAGE_TAG;
#ifdef OSA_NU_TARGET
#ifdef OSA_DEBUG
	CODE_COVERAGE_TAG;
	if (isrNum > INTC_MAX_INTERRUPT_SOURCES)
	{
		CODE_COVERAGE_TAG;
		OSA_ASSERT_EXT(FALSE);
		return(OS_INVALID_VECTOR);
	}
	OSA_ASSERT(block != NULL);
#endif

	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);

	pool = &block->isrPool;

	CODE_COVERAGE_TAG;
	for (i = 0; i < OSA_INTERRUPTS; i++)
	{
		CODE_COVERAGE_TAG;
		if (isrNum == pool->isrs[i].isrNum)
			break;
	}

	/* Call the OS specific function */
	OSA_ISR_NOTIFY_SISR(&pool->isrs[i].os_ref, status);
#endif


	return(status);
}
#endif  //OSA_C_SERVICE_RUN
#endif  /* OSA_INTERRUPTS */


#if defined (OSA_C_SERVICE_RUN)
/***********************************************************************
*
* Name:        OSAIsrDisable()
*
* Description: Disable processor IRQ Exceptions
*
* Parameters:
*  UINT32  mask    - mask of interrupts to disable
*
* Returns:     UINT32  - previous interrupt mask
*
* Notes:       Each call to OSIsrDisable must be paired with a
*              single call to OSIsrEnable(). This means that
*              once OSIsrDisable() is called it must not be
*              called again until OSIsrEnable() is called. All
*              calls to OSIsrEnable must be preceded by a
*              single call to OSIsrDisable().
*
***********************************************************************/
UINT32 OSAIsrDisable(UINT32 mask)
{
	UNUSEDPARAM(mask)
	UINT32 old_val;

	CODE_COVERAGE_TAG;
	OSA_INTERRUPT_DISABLE(mask, old_val);
	return(old_val);
}


/***********************************************************************
*
* Name:        OSAIsrEnable()
*
* Description: Enable processor IRQ Exceptions
*
* Parameters:
*  UINT32  mask    - mask of interrupts to enable
*
* Returns:     UINT32  - previous interrupt mask
*
* Notes:       see OS_InterruptDisable().
*
*
***********************************************************************/
UINT32 OSAIsrEnable(UINT32 mask)
{
	UNUSEDPARAM(mask)
	UINT32 old_val;

	CODE_COVERAGE_TAG;
	OSA_INTERRUPT_ENABLE(mask, old_val);
	return(old_val);
}


/***********************************************************************
*
* Name:        OSAContextLock()
*
* Description: Disable process swapping
*
* Parameters:  None
*
* Returns:     OS_SUCCESS
*
* Notes:       Each call to OSContextLock must be paired with a
*              single call to OSContextUnlock(). This means that
*              once OSContextLock() is called it must not be
*              called again until OSContextUnlock() is called. All
*              calls to OSContextLock must be preceded by a
*              single call to OSContextUnlock().
*
***********************************************************************/
OSA_STATUS OSAContextLock(void)
{
	CODE_COVERAGE_TAG;
	OSA_CONTEXT_LOCK;
	return(OS_SUCCESS);
}


/***********************************************************************
*
* Name:        OSAContextUnlock()
*
* Description: Enable process swapping
*
* Parameters:  None
*
* Returns:     OS_SUCCESS
*
* Notes:       see OS_InterruptDisable().
*
*
***********************************************************************/
OSA_STATUS OSAContextUnlock(void)
{
	CODE_COVERAGE_TAG;
	OSA_CONTEXT_UNLOCK;
	return(OS_SUCCESS);
}

/***********************************************************************
*
* Function Name:	OSASysContextLock
*
* Description:	    Locks context (both task preemption and isr disable)
*
* Inputs:		    None
*
* Outputs:         PrevContext - Context as it was before the lock.
*
* Return Value:	void
*
* Notes:		    It allows nesting calls (lock, lock - unlock, unlock)
*                  and other OS function calls
*                  Added to resolve SCR 1943843
***********************************************************************/
OSA_STATUS OSASysContextLock(OSASysContext *prevContext)
{
	CODE_COVERAGE_TAG;
#ifdef OSA_SYS_CONTEXT_LOCK
	OSA_SYS_CONTEXT_LOCK(prevContext);
#else
	UNUSEDPARAM(prevContext)
#endif
	return(OS_SUCCESS);
}

/***********************************************************************
*
* Name:	         OSASysContextUnlock
*
* Description:	     Unconditinally unlock context
*
* Inputs:		     None.
*
* Outputs:          None.
*
* Return Value:	 void
*
* Notes:		     Use with care.
*                   Added to resolve SCR 1943843
***********************************************************************/
OSA_STATUS OSASysContextUnlock(void)
{
	CODE_COVERAGE_TAG;
#ifdef OSA_SYS_CONTEXT_UNLOCK
	OSA_SYS_CONTEXT_UNLOCK;
#endif
	return(OS_SUCCESS);
}

/***********************************************************************
*
* Function Name:	OSASysContextRestore
*
* Description:	    Sets the context according to the input parameter.
*
* Inputs:		    PrevContext - Context is set as defined by this parameter.
*
* Outputs:         None.
*
* Return Value:	void
*
* Notes:		    It restors also nesting calls (lock, lock - unlock, unlock)
*                  and other OS function calls
*                  Added to resolve SCR 1943843
***********************************************************************/
OSA_STATUS OSASysContextRestore(OSASysContext *prevContext)
{
	CODE_COVERAGE_TAG;
#ifdef OSA_SYS_CONTEXT_RESTORE
	OSA_SYS_CONTEXT_RESTORE(prevContext)
#else
	UNUSEDPARAM(prevContext)
#endif
	return(OS_SUCCESS);
}
#endif //OSA_C_SERVICE_RUN



#if defined (OSA_C_SERVICE_INIT)
extern int eeh_draw_panic_text(const char *panic_text, size_t panic_len, int do_timer);
/****************************************************************************
*
* Name:        OSA_AssertFail()
*
* Description: Our own plain-Jane __assertfail() function.
*
* Parameters:  char *file    - file name
*              unsigned line - line number in file
*
* Returns:     This function does not return to its caller.
*
* Notes:       This function is called (using our OSA_ASSERT macro) if any of
*              our assertions fail -- meaning that we successfully trapped
*              a run-time error that was probably going to blow us out of
*              the water! It reports the name of the source file and the
*              line number containing the ASSERT that failed. This is the
*              version called when tracing is disabled.
*
*              File name and line number information is supplied by the
*              C pre-processor when the ASSERT macro is expanded.
*
****************************************************************************/
void OSA_AssertFail(char *file, unsigned line)
{
	OSA_STATUS status;

	CODE_COVERAGE_TAG;
	OSA_INTERRUPT_DISABLE(OSA_DISABLE_INTERRUPTS, status);
	if (status == 0) status = 0;  //avoid Compile WARNING

	/* Save the assert information in RAM */
	memcpy(OSA_assertFile, file, OSA_MAX_ASSERT_FILENAME_LEN);
	OSA_assertFile[OSA_MAX_ASSERT_FILENAME_LEN] = 0;

#if (OSA_PC_TEST || OSA_LINUX)
	CODE_COVERAGE_TAG;
	OSA_CONTEXT_LOCK;
	OSA_assertLine = line;
	printf("\n*** ASSERT!! *** \n File: %s\n Line: %d\n", OSA_assertFile, OSA_assertLine);
	printf(" Bad Parameter or Function/Parameter is not supported by the OS");
	do {
		char buffer[512];
		sprintf(buffer, "ASSERT FAIL AT FILE %s LINE %d", OSA_assertFile, OSA_assertLine);
		eeh_draw_panic_text(buffer, strlen(buffer), 0);
	} while (0);
#endif



#ifdef USE_UTILS_ASSERT
	ASSERT(0);
#else
#ifndef OSA_LINUX
	/* Get stuck here */
	for (;; )
	{
		CODE_COVERAGE_TAG;
	}
#endif
#endif
}
#endif //OSA_C_SERVICE_INIT


#ifdef OSA_TLS
#if defined (OSA_C_SERVICE_INIT)
/****************************************************************************
*
* Name:        OSATaskSetTls()
*
* Description: The function sets the Thread local storage (TLS)
*              to the memory location pointed by arg.
*
* Parameters:  void *arg    - pointer to the memory to be used as TLS
*
* Returns:     OS_SUCCESS
*
* Notes:       None
*
****************************************************************************/
OSA_STATUS OSATaskSetTls(void *arg)
{
	OS_Task_t *currentTaskRef;
	OS_Task *taskPtr;
	OS_TaskPool     *poolPtr;
	OSA_STATUS status;
	OS_Task         *activeListHead;
	OSA_ControlBlock *block = NULL;

	CODE_COVERAGE_TAG;
	OSA_TASK_IDENTIFY(currentTaskRef)
	taskPtr = (OS_Task*)currentTaskRef;

	/* Check to make sure that the thread was created by OSA */
	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	OSA_ASSERT(block != NULL);
	poolPtr = &block->taskPool;

	activeListHead = OS_NULL;
	activeListHead = poolPtr->head;

	status = OS_FAIL;
	while (activeListHead != OS_NULL)    /* search for the entry */
	{
		CODE_COVERAGE_TAG;
		if (activeListHead->refCheck == taskPtr->refCheck)
		{
			CODE_COVERAGE_TAG;
			status = OS_SUCCESS; /* found the entry */
			break;
		}
		activeListHead = activeListHead->next;
	}

	if (status != OS_FAIL)
	{
		CODE_COVERAGE_TAG;
		taskPtr->tlsPtr = arg;
	}

	return status;
}


/****************************************************************************
*
* Name:        OSATaskGetTls()
*
* Description: The function gets the Thread local storage (TLS) location
*              and assign it to the pointer arg.
*
* Parameters:  void **arg    - pointer to the memory to be used as TLS
*
* Returns:     OS_SUCCESS
*
* Notes:      None
*
****************************************************************************/
OSA_STATUS OSATaskGetTls(void **arg)
{
	OS_Task_t *currentTaskRef;
	OS_Task *taskPtr;
	OS_TaskPool     *poolPtr;
	OSA_STATUS status;
	OS_Task         *activeListHead;
	OSA_ControlBlock *block = NULL;


	CODE_COVERAGE_TAG;
	OSA_TASK_IDENTIFY(currentTaskRef)
	taskPtr = (OS_Task*)currentTaskRef;

	/* Check to make sure that the thread was created by OSA */
	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	OSA_ASSERT(block != NULL);
	poolPtr = &block->taskPool;

	activeListHead = OS_NULL;
	activeListHead = poolPtr->head;

	status = OS_FAIL;
	while (activeListHead != OS_NULL)    /* search for the entry */
	{
		CODE_COVERAGE_TAG;
		if (activeListHead->refCheck == taskPtr->refCheck)
		{
			CODE_COVERAGE_TAG;
			status = OS_SUCCESS;  /* found the entry */
			break;
		}
		activeListHead = activeListHead->next;
	}

	if (status != OS_FAIL)
	{
		CODE_COVERAGE_TAG;
		*arg = taskPtr->tlsPtr;
	}

	return status;

}
#endif  //OSA_C_SERVICE_INIT
#endif  //OSA_TLS


#ifdef OSA_DEBUG
#if defined (OSA_C_SERVICE_INIT)
/****************************************************************************
*
* Name:        OSAGetMemPoolStats
*
* Description: The function gets the memory ussage
*
* Parameters:  unsigned long *pUsedMem - pointer to the memory to be used
*                                       for saving the mem stats
*
* Returns:     OS_SUCCESS
*
* Notes:      None
*
****************************************************************************/
OSA_STATUS OSAGetMemPoolStats(unsigned long *pUsedMem)
{
	OSA_ControlBlock* block;

	CODE_COVERAGE_TAG;
	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	OSA_GET_MEM_POOL_STATS(pUsedMem, block);
	return(OS_SUCCESS);
}
#endif  //OSA_C_SERVICE_INIT
#endif  /* OSA_DEBUG */


#ifdef OSA_NUCLEUS

#ifdef OSA_STATS
#if defined (OSA_C_SERVICE_INIT)

//Stats    Stats   Stats   Stats   Stats    Stats   Stats    Stats   Stats
//========================================================================

/****************************************************************************
*
* Name:        all below
*
* Description: These functions get OSA objects and displays them on ICAT
*
* Returns:     They are triggered by ICAT (user action needed)
*
* Notes:      None
*
****************************************************************************/

/****************************************************************************/
/* tasks,   tasks,    tasks,   tasks,   tasks,   tasks,   tasks,   tasks    */
/****************************************************************************/
//ICAT EXPORTED FUNCTION - OSAstats, stats, getTaskStats
void getTaskStats()
{
	OS_TaskPool      *taskPoolPtr;
	OSA_ControlBlock *block;

	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	taskPoolPtr = &block->taskPool;
	DIAG_FILTER(OSA, tasks, counter, DIAG_INFORMATION)
	diagPrintf("configured=%d created=%d totalCreated %d", OSA_TASKS,
		   taskPoolPtr->numCreated, taskPoolPtr->totalNumCreated);

	DIAG_FILTER(OSA, tasks, RAMstats, DIAG_INFORMATION)
	diagPrintf("taskPoolAlloc=%d bytes taskPoolAvail=%d bytes",
		   sizeof(OS_TaskPool),
		   (sizeof(OS_TaskPool) -
		    ((taskPoolPtr->numCreated) * (sizeof(OS_Task)))));

}

/****************************************************************************/
/* memPools,   memPools,    memPools,   memPools,   memPools,   memPools,   */
/****************************************************************************/
//ICAT EXPORTED FUNCTION - OSAstats, stats, getMemPoolStats
void getMemPoolStats()
{
	OS_MemPool       *memPoolPtr;
	OSA_ControlBlock *block;

	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	memPoolPtr = &block->memPool;
	DIAG_FILTER(OSA, memPool, counter, DIAG_INFORMATION)
	diagPrintf("configured=%d created=%d", OSA_MEM_POOLS, memPoolPtr->numCreated);

	DIAG_FILTER(OSA, memPool, RAMstats, DIAG_INFORMATION)
	diagPrintf("memPoolAlloc=%d bytes memPoolAvail=%d bytes",
		   sizeof(OS_MemPool),
		   (sizeof(OS_MemPool) -
		    ((memPoolPtr->numCreated) * (sizeof(OS_Mem)))));
}


/****************************************************************************/
/* sema4s,   sema4s,    sema4s,   sema4s,   sema4s,   sema4s,   sema4s,     */
/****************************************************************************/
//ICAT EXPORTED FUNCTION - OSAstats, stats, getSema4Stats
void getSema4Stats()
{
	OS_SemaPool       *semaPool;
	OSA_ControlBlock  *block;

	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	semaPool = &block->semaPool;
	DIAG_FILTER(OSA, sema4s, counter, DIAG_INFORMATION)
	diagPrintf("configured=%d created=%d", OSA_SEMAPHORES, semaPool->numCreated);
	DIAG_FILTER(OSA, sema4s, RAMstats, DIAG_INFORMATION)
	diagPrintf("sema4PoolAlloc=%d sema4PoolAvail=%d",
		   sizeof(OS_SemaPool),
		   (sizeof(OS_SemaPool) -
		    ((semaPool->numCreated) * (sizeof(OS_Sema)))));
}




/****************************************************************************/
/* msgQs,   msgQs,    msgQs,   msgQs,   msgQs,   msgQs,   msgQs,  msgQs     */
/****************************************************************************/

//ICAT EXPORTED FUNCTION - OSAstats, stats, getMsgQStats
void getMsgQStats()
{
	OS_MsgQPool       *poolPtr;
	OSA_ControlBlock  *block;

	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	poolPtr = &block->msgQPool;
	DIAG_FILTER(OSA, msgQs, counter, DIAG_INFORMATION)
	diagPrintf("configured=%d created=%d", OSA_MSG_QUEUES, poolPtr->numCreated);
	DIAG_FILTER(OSA, msgQs, RAMstats, DIAG_INFORMATION)
	diagPrintf("msgQPoolAlloc=%d msgQPoolAvail=%d",
		   sizeof(OS_MsgQPool),
		   (sizeof(OS_MsgQPool) -
		    ((poolPtr->numCreated) * (sizeof(OS_MsgQueue)))));
}


/****************************************************************************/
/* Mboxes,   Mboxes,    Mboxes,   Mboxes,   Mboxes,   Mboxes,   Mboxes,     */
/****************************************************************************/

//ICAT EXPORTED FUNCTION - OSAstats, stats, getMboxStats
void getMboxStats()
{
	OS_MboxPool       *mboxPoolPtr;
	OSA_ControlBlock  *block;

	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	mboxPoolPtr = &block->mboxPool;
	DIAG_FILTER(OSA, Mboxes, counter, DIAG_INFORMATION)
	diagPrintf("configured=%d created=%d", OSA_MBOX_QUEUES, mboxPoolPtr->numCreated);
	DIAG_FILTER(OSA, Mboxes, RAMstats, DIAG_INFORMATION)
	diagPrintf("mboxPoolAlloc=%d mboxPoolAvail=%d",
		   sizeof(OS_MboxPool),
		   (sizeof(OS_MboxPool) -
		    ((mboxPoolPtr->numCreated) * (sizeof(OS_Mbox)))));
}



/****************************************************************************/
/* flags,   flags,    flags,   flags,   flags,   flags,   flags,  flags     */
/****************************************************************************/
//ICAT EXPORTED FUNCTION - OSAstats, stats, getFlagStats
void getFlagStats()
{
	OS_FlagPool       *poolPtr;
	OSA_ControlBlock  *block;

	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	poolPtr = &(block->eventPool);
	DIAG_FILTER(OSA, flags, counter, DIAG_INFORMATION)
	diagPrintf("configured=%d created=%d", OSA_FLAGS_MAX, poolPtr->numCreated);
	DIAG_FILTER(OSA, flags, RAMstats, DIAG_INFORMATION)
	diagPrintf("flagPoolAlloc=%d flagPoolAvail=%d ",
		   sizeof(OS_FlagPool),
		   (sizeof(OS_FlagPool) -
		    ((poolPtr->numCreated) * (sizeof(OS_Flag)))));
}

/****************************************************************************/
/* timers,   timers,    timers,   timers,   timers,   timers,   timers,     */
/****************************************************************************/

//ICAT EXPORTED FUNCTION - OSAstats, stats, getTimerStats
void getTimerStats()
{

	OS_TimerPool      *poolPtr;
	OSA_ControlBlock  *block;

	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	poolPtr = &block->timerPool;
	DIAG_FILTER(OSA, timers, counter, DIAG_INFORMATION)
	diagPrintf("configured=%d created=%d", OSA_TIMERS_MAX, poolPtr->numCreated);
	DIAG_FILTER(OSA, timers, RAMstats, DIAG_INFORMATION)
	diagPrintf("timerPoolAlloc=%d imerPoolAvail=%d",
		   sizeof(OS_TimerPool),
		   (sizeof(OS_TimerPool) -
		    ((poolPtr->numCreated) * (sizeof(OS_Timer)))));
}

#endif  //OSA_C_SERVICE_INIT
#endif  //OSA_STATS
#endif  //OSA_NUCLEUS

