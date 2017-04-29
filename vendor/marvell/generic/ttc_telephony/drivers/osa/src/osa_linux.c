/******************************************************************************

*(C) Copyright 2007 Marvell International Ltd.

* All Rights Reserved

******************************************************************************/
/* ===========================================================================
   File        : osa_linux.c
   Description : POSIX for MontaVista Linux 2.6 operating system specific code
   for the OS Abstraction Layer API.

   Notes       :

   Copyright (c) 2005 Intel CHG. All Rights Reserved
   =========================================================================== */
#ifdef OSA_LINUX


/************************************************************************
 * Project header files
 ***********************************************************************/
#include <sched.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#ifndef BIONIC
#include <sys/shm.h>
#include <sys/msg.h>
#endif
#include <sys/types.h>
#include <sys/ipc.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>

#include "osa.h"
#include "osa_types.h"



#ifdef BIONIC
#include <linux/ashmem.h>

#define ASHMEM_DEVICE   "/dev/ashmem"

#endif /* BIONIC */

////////////////////////////////////////////////////////////////////////////
// DEBUG API
////////////////////////////////////////////////////////////////////////////
#include <pxa_dbg.h>

#ifdef  OSA_LINUX_DEBUG
#define OSA_MSG                 DBGMSG
#else
#define OSA_MSG(fmt, arg ...)     do {} while (0)
#endif

#define OSA_LINUX_MEM_ALIGN     4
void* LinuxMalloc(int size)
{
	char* buf;

	buf = (char*)malloc(size);
	ASSERT(buf);
	ASSERT((((unsigned int)buf) % OSA_LINUX_MEM_ALIGN) == 0);
	return (void*)buf;
}

void LinuxFree(void* buf)
{
	free(buf);
}

/************************************************************************
 * External Interfaces
 ***********************************************************************/


/************************************************************************
 * External Variables
 ***********************************************************************/

#if !defined (BIONIC)
extern int errno;
#endif  /* BIONIC */

extern OSA_ControlBlock osa_ControlBlock;
extern OSA_ControlBlock* OSA_ControlBlockPtr;
extern BOOL OSA_Initialized;

/*********************************************************************
 * FIXME
 ********************************************************************/
//static pthread_mutex_t*         g_mutex_obj = NULL;
//static pthread_mutexattr_t*     g_mutex_attr = NULL;
//static volatile int	g_lock_count=0;

void OSA_ContextLock(void)
{
	/*g_lock_count++;
	   if(g_lock_count>1){
		ERRMSG("Lock error!\n");
	   }*/

	return;
#if 0
	int rtn;
	if (g_mutex_obj == NULL)
	{
		g_mutex_obj = malloc(sizeof(pthread_mutex_t));
		g_mutex_attr = malloc(sizeof(pthread_mutexattr_t));
		ASSERT(g_mutex_obj && g_mutex_attr);
		rtn = pthread_mutexattr_init(g_mutex_attr);
		ASSERT(rtn == 0);
#if !defined (BIONIC) /* this is default in BIONIC */
		rtn = pthread_mutexattr_setpshared(g_mutex_attr, PTHREAD_PROCESS_PRIVATE);
		ASSERT(rtn == 0);
#endif          /* BIONIC */
		rtn = pthread_mutex_init(g_mutex_obj, g_mutex_attr);
		ASSERT(rtn == 0);
	}
	pthread_mutex_lock(g_mutex_obj);
#endif
}

void OSA_ContextUnlock(void)
{

	return;
	//g_lock_count--;
	//pthread_mutex_unlock(g_mutex_obj);
}

#if 0
void OSA_TaskSleep(UINT32 ticks)
{
	unsigned int delay;
	struct timeval begin, end, now;

	gettimeofday(&begin, NULL);
	end.tv_sec = begin.tv_sec + ((begin.tv_usec + ticks * 1000) / 1000000);
	end.tv_usec = (begin.tv_usec + ticks * 1000) % 1000000;
	if (ticks < 1000)
	{
		do {
			sched_yield();
			gettimeofday(&now, NULL);
			if (now.tv_sec > end.tv_sec)
			{
				break;
			}
			else if (now.tv_sec == end.tv_sec && now.tv_usec >= end.tv_usec)
			{
				break;
			}
		} while (1);
	}
	else
	{
		do {
			sleep(1);
			gettimeofday(&now, NULL);
			if (now.tv_sec > end.tv_sec)
			{
				break;
			}
			else if (now.tv_sec == end.tv_sec && now.tv_usec >= end.tv_usec)
			{
				break;
			}
		} while (1);
		//gettimeofday(&end,NULL);
		//DBGMSG("Actually sleep %d usec\n",
		//	(end.tv_sec-begin.tv_sec)*1000+(end.tv_usec-begin.tv_usec)/1000);
	}
}
#else
void OSA_TaskSleep(UINT32 ticks)
{
	struct timespec req = { 0, 0 };
	time_t sec = (int)(ticks / 1000);

	ticks -= sec * 1000;
	req.tv_sec = sec;
	req.tv_nsec = ticks * 1000000L;
	while (nanosleep(&req, &req) == -1)
		if (errno == EINTR)
			continue;
}
#endif

/*********************************************************************
 * OSA Private Methods
 ********************************************************************/
static void OSA_TimerTask(void*);
static void OSA_TickTask(void*);


BOOL OSA_GetControlBlock(void** block, size_t size, BOOL procAttach)
{
	OSA_ControlBlock *temp;
	OSA_STATUS rtn;
	UINT8 i;
	pid_t pid = getpid();

	/* We only want to add this proccess if we're in init phase */
	if (!procAttach)
	{
		*block = OSA_ControlBlockPtr;
		return(TRUE);
	}


#ifdef BIONIC

	int fd = open(ASHMEM_DEVICE, O_RDWR);

	if (fd < 0)
	{
		OSA_MSG("OSA open ashmem failed\n");
		OSA_ERROR(errno);
		OSA_ASSERT(FALSE)
		return(TRUE);
	}

	int ret = ioctl(fd, ASHMEM_SET_SIZE, size);
	if (ret < 0)
	{
		close(fd);
		OSA_MSG("OSA open ashmem failed\n");
		OSA_ERROR(errno);
		OSA_ASSERT(FALSE)
		return(TRUE);
	}
	else
	{
		*block = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		if (*block == MAP_FAILED)
		{
			close(fd);
			OSA_MSG("OSA mmap failed\n");
			OSA_ERROR(errno);
			OSA_ASSERT(FALSE)
			return(TRUE);
		}
		else
#else   /* BIONIC */
      // UPDATED
      //int fd = shmget(12378, size, IPC_CREAT | 0666);
	int fd = shmget(IPC_PRIVATE, size, IPC_CREAT | 0666);

	/* OSA global mem Already exists so we need to attach to it*/
	if ( fd == -1 )
	{
		OSA_MSG("OSA shmget failed\n");
		OSA_ERROR(errno);
		OSA_ASSERT(FALSE)
		return(TRUE);
	}
	else
	{
		*block = shmat(fd, (char *)0x20000000, 0);    /*fixed compiler warning*/
		if (*block == (char*)-1)
		{
			OSA_MSG("OSA shmgat failed\n");
			OSA_ERROR(errno);
			OSA_ASSERT(FALSE)
			return(TRUE);
		}
		else
#endif          /* BIONIC */
		{
			temp = OSA_ControlBlockPtr = (OSA_ControlBlock*)*block;

			OSA_MSG("\n\n OSA_INIT: Block = %X, PID = %X\n\n", (int)temp, getpid());

#ifdef OSA_SINGLE_PROCESS  // Force clearing OSA control block
			temp->attach.numProcAttached = 0;
#endif

			if (temp->attach.numProcAttached == 0)
			{
				OSA_MSG("\n      ** OSA Clearing Block PID = %X **\n", pid);
				memset((void*)*block, 0, sizeof(OSA_ControlBlock));
				OSA_SEMA_CREATE(&temp->attach.sema, NULL, 1, OS_PRIORITY, rtn);
				OSA_SEMA_ACQUIRE(temp->attach.sema, OSA_SUSPEND, rtn);
				temp->attach.proc[0].pid = pid;
				temp->attach.numProcAttached++;
				OSA_SEMA_RELEASE(temp->attach.sema, rtn);
				return(FALSE);
			}
			else
			{
				OSA_SEMA_ACQUIRE(temp->attach.sema, OSA_SUSPEND, rtn);

				/* Scan through the processes and add this to the attached process list */
				for (i = 0; i < OSA_MAX_ATTACHED_PROCESSES; i++)
				{
					if ( temp->attach.proc[i].pid == pid )
					{
						break;
					}
					else if ( temp->attach.proc[i].pid == 0 )
					{
						temp->attach.proc[i].pid = pid;
						temp->attach.numProcAttached++;
						break;
					}
				}
				OSA_SEMA_RELEASE(temp->attach.sema, rtn);
				return(TRUE);
			}
		}
	}
}

/***********************************************************************
*
* Name:        OSA_Init()
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
#if (OSA_TIMERS)
OS_Sema_t timerLock;
int timer_flag = 0;
#endif

void OSA_Initialize(void* block)
{
	OSA_STATUS rt_val;
	OSA_ControlBlock *temp = (OSA_ControlBlock*)block;

	/*
	** Initialize POSIX specific components
	*/

	/* Create the critical section object used for context lock/unlock */

#if (OSA_TIMERS)
	OSA_SEMA_CREATE(&timerLock, NULL, 0, OS_FIFO, rt_val);

	/* Create the semaphore for the timer task */
	OSA_SEMA_CREATE(&temp->OSA_TickSemaphore, NULL, 1, OS_PRIORITY, rt_val);

	/* Clear the internal OS Real-Time Tick counter */
	temp->OSA_RtcCounter = 0L;

	/* Create & start the simulated tick task */
	OSA_TASK_CREATE(&temp->OSATickTask, (void *)NULL, OSA_TICK_STACK, 5,
			(UINT8 *)"OSATck", OSA_TickTask, (void *)temp, rt_val);

	OSA_TASK_RESUME(temp->OSATickTask, rt_val);

	/* Create & start the timer task */
	OSA_TASK_CREATE(&temp->OSATimerTask, (void *)NULL, OSA_TIMER_STACK, 6,
			(UINT8 *)"OSATmr", OSA_TimerTask, (void *)temp, rt_val);

	OSA_TASK_RESUME(temp->OSATimerTask, rt_val);

#endif

}


void OSA_Run(void)
{
	OSA_ControlBlock *block = NULL;
	int rtn;
	UINT8 i;
	OSA_STATUS status;
	pid_t pid = getpid();

	rtn = getchar();    // Should use process wait for child exited

	OSA_GetControlBlock((void**)&block, sizeof(OSA_ControlBlock), FALSE);

	OSA_SEMA_ACQUIRE(block->attach.sema, OSA_SUSPEND, status);

	/* Scan through the processes and remove this from the attached process list */
	for (i = 0; i < OSA_MAX_ATTACHED_PROCESSES; i++)
	{
		if ( block->attach.proc[i].pid == pid )
		{
			block->attach.proc[i].pid = 0;
			block->attach.numProcAttached--;

			OSA_SEMA_RELEASE(block->attach.sema, status);

			if (block->attach.numProcAttached == 0)
			{
				OSA_SEMA_DELETE(block->attach.sema, status);
			}

			break;
		}
	}
}

#if (OSA_TASKS)
/***********************************************************************
*
* Name:        OSA_TaskCreate()
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
OSA_STATUS OSA_TaskCreate( OS_Task_t *task, UINT8 *name, UINT8 priority, UINT32 stackSize,
			   void (*taskStart)(void*), void *argv )
{
	UNUSEDPARAM(name)
	UNUSEDPARAM(stackSize)
	int status = OS_SUCCESS;

	/* Store the tasks parameters to be used in the OSA task wrapper */
	task->priority     = priority;
	task->taskStartPtr = taskStart;
	task->argv         = argv;

	/* Create the task */
	status = pthread_create(&task->task, NULL,    //&task->attr,
				OSA_TaskStartWrapper,
				(void *)task);

	if (status)
	{
		/* Do error code translation to generic OS errors */
		OSA_ERROR(status);
		return(OSA_TranslateErrorCode(status));
	}
	else
	{
		/*
		 * In default case: we set this task as detached case
		 * So when the thread exits, it can release all the resources correctly
		 */
		pthread_detach(task->task);
		return(OS_SUCCESS);
	}
}

/***********************************************************************
*
* Name:        OSA_TaskStartWrapper()
*
* Description: This is a wrapper around the actual threads task so that
*              some things can be done to set up the thread to run in
*              real time and to pass the arguments to the task correctly.
*
* Parameters:
*  void     *argv             - argument to be passed into task function
*
* Returns:  0
*
* Notes:
*
***********************************************************************/
void* OSA_TaskStartWrapper(void *argv)
{
	OS_Task_t   *task = (OS_Task_t *)argv;

	/* Call the threads start routine */
	task->taskStartPtr(task->argv);

	return 0;
}

/***********************************************************************
*
* Name:        OSA_TaskDelete()
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
OSA_STATUS OSA_TaskDelete( OS_Task_t task )
{
	UNUSEDPARAM(task)
	return(OS_SUCCESS);
}

/***********************************************************************
*
* Name:        OSA_TaskSuspend()
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
OSA_STATUS OSA_TaskSuspend( OS_Task_t task )
{
	UNUSEDPARAM(task)
	return(OS_SUCCESS);
}

/***********************************************************************
*
* Name:        OSA_TaskResume()
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
OSA_STATUS OSA_TaskResume( OS_Task_t task )
{
	UNUSEDPARAM(task)
	return(OS_SUCCESS); /* Suspend count was 1 (suspended) */
}

/***********************************************************************
*
* Name:        OSA_TaskGetPriority()
*
* Description: Get the task priority.
*
* Parameters:
*  OSTaskRef taskRef     - reference of task to modify
*  UINT32*   oldPriority - old task priority
*
* Returns:
*
* Notes:
*
***********************************************************************/
OSA_STATUS  OSA_TaskGetPriority( OS_Task_t *task, UINT8 *oldPriority)
{
	UNUSEDPARAM(task)
	UNUSEDPARAM(oldPriority)
	return(OS_SUCCESS);
}

/***********************************************************************
*
* Name:        OSA_TaskChangePriority()
*
* Description: Change the tasks priority.
*
* Parameters:
*  OSTaskRef taskRef     - reference of task to modify
*  UINT8     newPriority - new task priority
*  UINT8*    oldPriority - old task priority
*
* Returns:
*
* Notes:
*
***********************************************************************/
OSA_STATUS  OSA_TaskChangePriority( OS_Task_t *task, UINT8 newPriority, UINT8 *oldPriority)
{
	UNUSEDPARAM(task)
	UNUSEDPARAM(newPriority)
	UNUSEDPARAM(oldPriority)
	return(OS_SUCCESS);
}
#endif

#if (OSA_MSG_QUEUES)

/***********************************************************************
*
* Name:        OSA_QueueCreate()
*
* Description: Create a message queue.
*
* Parameters:
*  OS_MsgQ_t *msgQRef         - reference to the message queue
*  char      *queueName       - name of queue
*  UINT32    maxSize          - pointer to start of task stack area
*  UINT32    maxNumber        - number of bytes in task stack area
*  void      *qAddr           - address of memory to use for the queue
*  UINT8     waitingMode      - scheduling of waiting tasks
*                               OS_FIFO, OS_PRIORITY
*
* Returns:
*
* Notes: mq_open, mq_close, mq_send, mq_recv, mq_notify, mq_getattr, mq_setattr
*
*
***********************************************************************/
OSA_STATUS OSA_QueueCreate(OS_MsgQ_t* msgQRef, char *queueName, UINT32 maxSize,
			   UINT32 maxNumber, void *qAddr, UINT8 waitingMode)
{
	UNUSEDPARAM(waitingMode)

#ifndef OSA_USE_INTERNAL_MESSAGING
	key_t key;
	UINT8  *buf = (UINT8*)queueName;
	UINT8 i;

	for (i = 0; i < strlen((char*)queueName); i++)
		key = key + buf[i];

	//int id = msgget(key, (IPC_CREAT | 0666));
	int id = msgget(IPC_PRIVATE, IPC_CREAT);

	if ( id == -1 )
	{
		/* Do error code translation to generic OS errors */
		OSA_ERROR(errno);
		return(OSA_TranslateErrorCode(errno));
	}
	else
	{
		msgQRef->os_queue = id;
		msgQRef->maxNum   = maxNumber;
		msgQRef->maxSize  = maxSize;
		return(OS_SUCCESS);
	}

#else
	UNUSEDPARAM(queueName)

	OSA_STATUS status;
	UINT32 memSize;
	POSMsgQInfo pMsgQInfo = NULL;
	PLocalMsgQInfo pLocalMsgQInfo = NULL;
	void *memory = NULL;

	if (qAddr)   /* Use the user's memory space for this queue */
	{
		pMsgQInfo = (POSMsgQInfo)qAddr;
	}
	else    /* Allocate memory for the base message queue */
	{
		memSize = OSA_MSG_QUEUE_HEADER_SIZE + (maxNumber * maxSize) + (maxNumber * sizeof(int));
		memory = LinuxMalloc(memSize);
	}

	if (memory)
	{
		pMsgQInfo = (POSMsgQInfo)memory;

		pMsgQInfo->memPtr               = (UINT8*)pMsgQInfo;
		pMsgQInfo->msgPtr               = (UINT8*)((UINT32)pMsgQInfo + sizeof(OSMsgQInfo));
		pMsgQInfo->msgLenPtr            = (UINT32*)((UINT32)pMsgQInfo + sizeof(OSMsgQInfo) + maxNumber * maxSize);
		pMsgQInfo->maxNumMsgs           = maxNumber;
		pMsgQInfo->maxSizeOfMsg         = maxSize;
		pMsgQInfo->readCount            = 0;
		pMsgQInfo->writeCount           = 0;
		pMsgQInfo->msgCount             = 0;

		pLocalMsgQInfo  = (PLocalMsgQInfo)LinuxMalloc(sizeof(LocalMsgQInfo));
		if (pLocalMsgQInfo)
		{
			pLocalMsgQInfo->pMsgQInfo       = pMsgQInfo;
			msgQRef->os_ref                 = pLocalMsgQInfo;
			msgQRef->os_ref->pMsgQInfo      = pMsgQInfo;

			/* Create a handle to block on queue recv with OS_SUSPEND */
			OSA_SEMA_CREATE(&pLocalMsgQInfo->recvEventHandle, NULL, 0, OSA_FIFO, status);

			/* Create a semaphore to block on queue send with OS_SUSPEND */
			OSA_SEMA_CREATE(&pLocalMsgQInfo->sendSemaphore, NULL, maxNumber, OSA_FIFO, status);

			/* Create a semaphore to protect access for pMsgQInfo->readCount & writeCount */
			OSA_SEMA_CREATE(&pLocalMsgQInfo->protectSemaphore, NULL, 1, OSA_FIFO, status);
			return(OS_SUCCESS);
		}
		else
			return(OS_NO_MEMORY);
	}
	else
		return(OS_NO_MEMORY);
#endif
}

/***********************************************************************
*
* Name:        OSA_QueueDelete()
*
* Description: Create a message queue.
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
OSA_STATUS OSA_QueueDelete(OS_MsgQ_t *msgQRef)
{
#ifndef OSA_USE_INTERNAL_MESSAGING

	msgctl(msgQRef->os_queue, IPC_RMID, NULL);

	msgQRef->os_queue = 0;
	msgQRef->maxNum   = 0;
	msgQRef->maxSize  = 0;
	msgQRef->msgCount = 0;
	return(OS_SUCCESS);

#else

	OSA_STATUS status;
	PLocalMsgQInfo pLocalMsgQInfo  = (PLocalMsgQInfo)msgQRef->os_ref;
	POSMsgQInfo pMsgQInfo       = (POSMsgQInfo)pLocalMsgQInfo->pMsgQInfo;

	if (msgQRef->os_ref == NULL)
		return(OS_INVALID_REF);
	OSA_SEMA_DELETE(msgQRef->os_ref->recvEventHandle, status);
	OSA_SEMA_DELETE(msgQRef->os_ref->sendSemaphore, status);
	OSA_SEMA_DELETE(msgQRef->os_ref->protectSemaphore, status);
	LinuxFree(pLocalMsgQInfo);
	LinuxFree(pMsgQInfo);
	return(OS_SUCCESS);

#endif
}

/***********************************************************************
*
* Name:        OSA_QueueSend()
*
* Description: Send a message to a queue.
*
* Parameters:
*  OS_MsgQ_t msgQRef         - reference to the message queue
*  UINT32    size            - message size
*  UINT8     *msg            - message to be sent
*  UINT32    timeout         - OSA_SUSPEND, OSA_NO_SUSPEND, or timeout
*                              1 - 4,294,967,293
*
* Returns:
*
* Notes:
*
*
***********************************************************************/
OSA_STATUS OSA_QueueSend(OS_MsgQ_t *msgQRef, UINT32 size, UINT8 *msg,
			 UINT32 timeout)
{
#ifndef OSA_USE_INTERNAL_MESSAGING
	int flag = 0;
	OS_Msg msgBuf;

	if (timeout == OS_NO_SUSPEND)
		flag = IPC_NOWAIT;
	else if (timeout == OS_SUSPEND)
		flag = 0;
	else
	{
		return(OS_UNSUPPORTED);
	}

	OSA_ASSERT(size > 0);
	if (size > msgQRef->maxSize)
		return OS_INVALID_SIZE;

	msgBuf.msg = (char*)LinuxMalloc(size);
	msgBuf.type = (long)size;
	memcpy(msgBuf.msg, (char *)msg, size);

	OSA_MSG("SEND MSG %d (0x%08X)\n", msgBuf.type, msgBuf.msg);
	OSA_ASSERT(sizeof(msgBuf.msg) == sizeof(char*));
	int status = msgsnd(msgQRef->os_queue, (void*)&msgBuf, sizeof(msgBuf.msg), flag);

	if ( status == -1 )
	{
		LinuxFree(msgBuf.msg);
		/* Do error code translation to generic OS errors */
		OSA_ERROR(errno);
		return(OSA_TranslateErrorCode(errno));
	}
	else
	{
		msgQRef->msgCount++;
		return(OS_SUCCESS);
	}
#else
	PLocalMsgQInfo pLocalMsgQInfo  = (PLocalMsgQInfo)msgQRef->os_ref;
	POSMsgQInfo pMsgQInfo       = (POSMsgQInfo)pLocalMsgQInfo->pMsgQInfo;
	UINT32 msgIndex;
	UINT32 msgOffset;
	OSA_STATUS rtn;

	if (size > pMsgQInfo->maxSizeOfMsg)
		return OS_INVALID_SIZE;

	/* Acquire send semaphore first */
	OSA_SEMA_ACQUIRE(pLocalMsgQInfo->sendSemaphore, timeout, rtn);

	OSA_SEMA_ACQUIRE(pLocalMsgQInfo->protectSemaphore, OS_SUSPEND, rtn);
	if (pMsgQInfo->msgCount >= pMsgQInfo->maxNumMsgs)
	{
		OSA_SEMA_RELEASE(pLocalMsgQInfo->protectSemaphore, rtn);

		OSA_ASSERT(timeout != OS_SUSPEND);
		return OS_QUEUE_FULL;
	}

	OSA_MSG("SEND MSG %d\n", size);
	msgIndex  = pMsgQInfo->writeCount;
	msgOffset = pMsgQInfo->maxSizeOfMsg * msgIndex;
	memcpy(&pMsgQInfo->msgPtr[msgOffset], msg, size);
	pMsgQInfo->msgLenPtr[msgIndex] = size;

	++pMsgQInfo->msgCount;
	++pMsgQInfo->writeCount;
	pMsgQInfo->writeCount = (pMsgQInfo->writeCount) % pMsgQInfo->maxNumMsgs;
	OSA_SEMA_RELEASE(pLocalMsgQInfo->protectSemaphore, rtn);

	/* Increase recv event count */
	OSA_SEMA_RELEASE(pLocalMsgQInfo->recvEventHandle, rtn);
	return OS_SUCCESS;

#endif
}

/***********************************************************************
*
* Name:        OSA_QueueRecv()
*
* Description: Receive a message from a queue.
*
* Parameters:
*  OS_MsgQ_t msgQRef         - reference to the message queue
*  UINT32    size            - max message size to copy to user's buffer
*  UINT8     *msg            - pointer to buffer to store message
*
*
* Returns:
*
* Notes:
*
*
***********************************************************************/
OSA_STATUS OSA_QueueRecv(OS_MsgQ_t *msgQRef, UINT8 *msg, UINT32 size,
			 UINT32 timeout)
{
#ifndef OSA_USE_INTERNAL_MESSAGING
	int flag;
	OS_Msg msgBuf;
	int status;
	int retry;

	if (timeout == OS_NO_SUSPEND)
		flag = IPC_NOWAIT;
	else if (timeout == OS_SUSPEND)
		flag = 0;
	else
	{
		OSA_ASSERT(0);
		return(OS_UNSUPPORTED);
	}

	retry = 0;
	do {
		status = msgrcv(msgQRef->os_queue, (void*)&msgBuf, sizeof(msgBuf.msg), 0, flag);

		if (status != -1)
		{
			OSA_MSG("RCV MSG %d (0x%08X,%d)\n", msgBuf.type, msgBuf.msg, status);
			if (msgBuf.type > size)
			{
				return OS_INVALID_SIZE;
			}
			memcpy(msg, msgBuf.msg, msgBuf.type);
			LinuxFree(msgBuf.msg);
			msgQRef->msgCount--;
			return(OS_SUCCESS);
		}
		else if (errno == EAGAIN)
		{
			return OS_TIMEOUT;
		}
		else if (errno == EINTR)
		{
			retry++;
			if (retry > 100)
			{
				ERRMSG("Too many EINTR errors in msgrcv\n");
				ASSERT(0);
				return OS_FAIL;
			}
			else
			{
				continue;
			}
		}
		else
		{
			ERRMSG("Cannot receive message %d\n", status);
			OSA_ERROR(errno);
		}
	} while (errno == EINTR);

	return( OSA_TranslateErrorCode(errno) );
#else

	PLocalMsgQInfo pLocalMsgQInfo  = (PLocalMsgQInfo)msgQRef->os_ref;
	POSMsgQInfo pMsgQInfo       = (POSMsgQInfo)pLocalMsgQInfo->pMsgQInfo;
	UINT32 msgIndex;
	UINT32 msgOffset;
	OSA_STATUS status;

	if (!msg)
	{
		return OS_INVALID_PTR;
	}

	/* Acquire recv event first */
	OSA_SEMA_ACQUIRE(pLocalMsgQInfo->recvEventHandle, timeout, status);

	OSA_SEMA_ACQUIRE(pLocalMsgQInfo->protectSemaphore, OS_SUSPEND, status);
	if (pMsgQInfo->msgCount == 0)
	{
		OSA_SEMA_RELEASE(pLocalMsgQInfo->protectSemaphore, status);

		OSA_ASSERT(timeout != OS_SUSPEND);
		return OS_QUEUE_EMPTY;
	}

	msgIndex  = pMsgQInfo->readCount;
	msgOffset = pMsgQInfo->maxSizeOfMsg * msgIndex;

	if ((size > pMsgQInfo->maxSizeOfMsg) || (size < pMsgQInfo->msgLenPtr[msgIndex]))
	{
		OSA_SEMA_RELEASE(pLocalMsgQInfo->protectSemaphore, status);
		return(OS_INVALID_SIZE);
	}
	OSA_MSG("RECV MSG %d/%d\n", pMsgQInfo->msgLenPtr[msgIndex], size);
	if (size > pMsgQInfo->msgLenPtr[msgIndex])
	{
		size = pMsgQInfo->msgLenPtr[msgIndex];
	}
	memcpy(msg, &pMsgQInfo->msgPtr[msgOffset], size);
	--pMsgQInfo->msgCount;
	++pMsgQInfo->readCount;
	pMsgQInfo->readCount = (pMsgQInfo->readCount) % pMsgQInfo->maxNumMsgs;

	OSA_SEMA_RELEASE(pLocalMsgQInfo->protectSemaphore, status);

	/* Increase send semaphore ref count*/
	OSA_SEMA_RELEASE(pLocalMsgQInfo->sendSemaphore, status);
	return(OS_SUCCESS);

#endif
}

/***********************************************************************
*
* Name:        OSA_QueuePoll()
*
* Description: Poll for a message from a queue.
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
OSA_STATUS OSA_QueuePoll(OS_MsgQ_t *msgQRef, UINT32 *msgCount)
{

#ifndef OSA_USE_INTERNAL_MESSAGING
	*msgCount = msgQRef->msgCount;
	return(OS_SUCCESS);

#else
	POSMsgQInfo pMsgQInfo = (POSMsgQInfo)msgQRef->os_ref->pMsgQInfo;
	*msgCount = pMsgQInfo->msgCount;
	return(OS_SUCCESS);
#endif
}
#endif

#if (OSA_FLAGS)
/***********************************************************************
*
* Name:        OSA_FlagCreate()
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
OSA_STATUS OSA_FlagCreate( OS_Flag_t *flagRef, char *name )
{
	UNUSEDPARAM(name)

	OSA_STATUS status;

	/* Create the Flag */
	OSA_SEMA_CREATE(&flagRef->semaphore, NULL, 0, OS_FIFO, status);
	if ( status != OS_SUCCESS )
	{
		/* Do error code translation to generic OS errors */
		OSA_ASSERT(FALSE)
	}
	else
	{
		flagRef->flag = 0L;
		return(OS_SUCCESS);
	}

	return(status);
}

/***********************************************************************
*
* Name:        OSA_FlagDelete()
*
* Description: Delete event flag.
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSA_FlagDelete( OS_Flag_t *flagRef )
{
	OSA_STATUS status;

	OSA_SEMA_DELETE(flagRef->semaphore, status);
	if (status == OS_SUCCESS)
		flagRef->flag = 0L;

	return (status);
}

/***********************************************************************
*
* Name:        OSA_FlagSetBits()
*
* Description: Set event flag in target event register
*
* Parameters:
*
*  OSFlagRef flagRef - event flag reference who's event flags will be set
*  UINT32    mask    - flags to set
*
* Returns:
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSA_FlagSetBits( OS_Flag_t *flagRef, UINT32 mask, UINT32 operation )
{
	OSA_STATUS status;
	UINT32 semValue, i;

	if (operation == OSA_FLAG_AND)
	{
		flagRef->flag &= mask;
	}
	else if (operation == OSA_FLAG_OR)
	{
		flagRef->flag |= mask;
	}
	else
	{
		return(OS_INVALID_MODE);
	}

	/* User wants to reset the flag so clear the semaphore */
	if (0L == mask)
	{
		OSA_SEMA_POLL(flagRef->semaphore, &semValue, status);
		if (OS_SUCCESS == status)
		{
			for (i = semValue; i != 0; i--)
				OSA_SEMA_ACQUIRE(flagRef->semaphore, OSA_NO_SUSPEND, status);
		}
		else
			OSA_ASSERT(FALSE);
	}
	else
	{
		OSA_SEMA_RELEASE(flagRef->semaphore, status);
	}

	return(OS_SUCCESS);
}

/***********************************************************************
*
* Name:        OSA_FlagWait()
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
*
*
* Notes:
*
*    This function returns all set events, but only clears those that were
*    requested so that events will not be lost if they are not acted upon
*    immediately.
*
***********************************************************************/
OSA_STATUS OSA_FlagWait( OS_Flag_t *flagRef, UINT32 mask, UINT32 operation,
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

	/* Continue to wait for a match in the requested event mask */
	while (noMatch)
	{
		/* Wait for events to be set */
		OSA_SEMA_ACQUIRE(flagRef->semaphore, timeout, rtn);

		if (rtn == OS_SUCCESS)
		{
			compare = flagRef->flag & mask;

			/* All events must be present. Check the compare value. */
			if ((operation == OSA_FLAG_AND) || (operation == OSA_FLAG_AND_CLEAR) )
			{
				compare = (compare == mask);
			}

			/* Necessary event flags are present */
			if (compare)
			{
				/* Store which event flags were set */
				*flags = flagRef->flag;

				if ( (operation == OSA_FLAG_AND_CLEAR) || (operation == OSA_FLAG_OR_CLEAR) )
				{
					/* Clear waited for event flags */
					flagRef->flag &= ~mask;
					noMatch = FALSE;
					status = OS_SUCCESS;
				}
				else
				{
					noMatch = FALSE;
					status = OS_SUCCESS;

					/* Give others a chance to run */
					OSA_SEMA_RELEASE(flagRef->semaphore, rtn);
				}
			}
			else /* No Match in the event flags */
			{
				if (timeout == OSA_SUSPEND)
				{
					noMatch = TRUE;

					/*FIXME: give others a chance to run! */
					//OSA_SEMA_RELEASE(flagRef->semaphore, rtn);

					continue;
				}
				else if (timeout == OSA_NO_SUSPEND)
				{
					/* Store which event flags were set */
					*flags = flagRef->flag;
					OSA_SEMA_RELEASE(flagRef->semaphore, rtn);
					noMatch = FALSE;
					status = OS_FLAG_NOT_PRESENT;
				}
				else /* Timed out */
				{
					/* Store which event flags were set */
					*flags = flagRef->flag;
					OSA_SEMA_RELEASE(flagRef->semaphore, rtn);
					noMatch = FALSE;
					status = OS_TIMEOUT;
				}
			}
		} /* End of getting flag semaphore */
		else
		{
			/* Semaphore was not available so we won't wait for it */
			if ( (timeout == OSA_NO_SUSPEND) ||
			     (rtn == OS_UNAVAILABLE) )
			{
				/* Store which event flags were set */
				*flags = flagRef->flag;
				status = OS_FLAG_NOT_PRESENT;
				break;
			}
			else if (timeout == OS_TIMEOUT)
			{
				*flags = flagRef->flag;
				status = OS_TIMEOUT;
				break;
			}
			else  /* Semaphore acquire failed */
			{
				status = OS_FAIL;
				break;
			}
		}

	} /* End of while waiting for events to match */

	OSA_ASSERT(status == OS_SUCCESS);
	return(status);
}
#endif

#if (OSA_SEMAPHORES)
/***********************************************************************
*
* Name:        OSA_SemaCreate()
*
* Description: Get a OS_Sema data structure from the semaphore pool
*              and initialize it.
*
* Parameters:
*  OSSemaId  semaId           - application assigned semaphore ID
*  UINT32    initialCount     - initial count of the semaphore
*  UINT8    waitingMode      - scheduling of waiting tasks
*                               OS_FIFO, OS_PRIORITY
*
* Returns:
*  OS_Sema_t *sema            - Nuclues assigned semaphore reference
*
*
* Notes: sem_init, sem_destroy, sem_open, sem_close, sem_trywait, sem_wait,
*        sem_post, sem_getvalue
*
***********************************************************************/
OSA_STATUS OSA_SemaCreate( OS_Sema_t *sema, char *name, UINT32 initialCount,
			   UINT8 waitingMode )
{
	UNUSEDPARAM(name)
	int status;

	ASSERT((waitingMode == OS_FIFO) || (waitingMode == OS_PRIORITY));

	status = sem_init(&sema->os_sema, 0, (int)initialCount);

	if ( status == -1 )
	{
		ASSERT(0);
		/* Do error code translation to generic OS errors */
		OSA_ERROR(errno);
		return(OSA_TranslateErrorCode(errno));
	}
	else
	{
		sema->count = initialCount;
		return(OS_SUCCESS);
	}
}

/***********************************************************************
*
* Name:        OSA_SemaDelete()
*
* Description: Delete a semaphore
*
* Parameters:
*  OS_Sema_t sema         - semaphore reference
*
* Returns:
*  OS_SUCCESS
*  OS_INVALID_REF
*
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSA_SemaDelete( OS_Sema_t *sema )
{
	int status = sem_destroy(&sema->os_sema);

	if ( status == -1 )
	{
		/* Do error code translation to generic OS errors */
		//OSA_ERROR(errno); commented out for BIONIC
		return(OSA_TranslateErrorCode(errno));
	}
	else
	{
		return(OS_SUCCESS);
	}
}

/***********************************************************************
*
* Name:        OSA_SemaAcquire()
*
* Description: Block calling task until lock is acquired
*
* Parameters:
*  OS_Sema_t sema         - semaphore reference
*
* Returns:
*  OS_SUCCESS
*  OS_INVALID_REF
*
* Notes:       It is up to the application to ensure that this function
*              is not called on a deleted semaphore and that it is not already
*              locked by the same task.
*
***********************************************************************/
OSA_STATUS OSA_SemaAcquire( OS_Sema_t *sema, UINT32 timeout )
{
	int status;
	struct timespec ts;

	if (timeout == OSA_NO_SUSPEND)
	{
		do {
			status = sem_trywait(&sema->os_sema);

			if (status == -1)
			{
				if (errno == EINTR || errno == EAGAIN)
					continue;
				else
				{
					/* Do error code translation to generic OS errors */
					OSA_ERROR(errno);
					return(OSA_TranslateErrorCode(errno));
				}
			}
			else
			{
				sema->count--;
				return(OS_SUCCESS);
			}
		} while (1);
	}
	else /*if(timeout == OSA_SUSPEND)*/
	{
		if (timeout != OSA_SUSPEND)
		{
			if (timeout != 0)
			{
				if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
				{
					perror("gettime error!\n");
					return(OS_FAIL);
				}
				else
				{
					if ((ts.tv_nsec + timeout) >= 1000000000)
					{
						ts.tv_sec += 1;
						ts.tv_nsec = ts.tv_nsec + timeout - 1000000000;
					}
					else
					{
						ts.tv_nsec += timeout;
					}
				}

				while ((status = sem_timedwait(&sema->os_sema, &ts)) == -1 && errno == EINTR)
					continue;
				/* Check what happened */

				if (status == -1)
				{
					if (errno == ETIMEDOUT)
					{
						printf("sem_timedwait() timed out\n");
						return(OS_TIMEOUT);
					}
					else
					{
						perror("sem_timedwait");
						return(OS_FAIL);
					}
				}
				else
				{
					printf("sem_timedwait() succeeded\n");
					return(OS_SUCCESS);
				}
			}
			else
				ERRMSG("OSA_SemaAcquire doesn't support timeout\n", timeout);
		}
		do {
			status = sem_wait(&sema->os_sema);

			if (status == -1)
			{
				if (errno == EINTR || errno == EAGAIN)
					continue;
				else
				{
					/* Do error code translation to generic OS errors */
					OSA_ERROR(errno);
					return(OSA_TranslateErrorCode(errno));
				}
			}
			else
			{
				sema->count--;
				return(OS_SUCCESS);
			}
		} while (1);
	}
}

/***********************************************************************
*
* Name:        OSA_SemaRelease()
*
* Description: Release the semaphore
*
* Parameters:
*  OS_Sema_t sema         - semaphore reference
*
* Returns:
*  OS_SUCCESS
*  OS_INVALID_REF
*
* Notes:       It is up to the application to ensure that this function
*              is not called on a deleted semaphore and that it is not already
*              locked by the same task.
*
***********************************************************************/
OSA_STATUS OSA_SemaRelease( OS_Sema_t *sema )
{
	int status;

	status = sem_post(&sema->os_sema);

	if ( status == -1)
	{
		/* Do error code translation to generic OS errors */
		OSA_ERROR(errno);
		return(OSA_TranslateErrorCode(errno));
	}
	else
	{
		sema->count++;
		return(OS_SUCCESS);
	}
}

/***********************************************************************
*
* Name:        OSA_SemaPoll()
*
* Description: Poll the semaphore
*
* Parameters:
*  OS_Sema_t sema         - semaphore reference
*  UINT32    count        - current semaphore count
*
* Returns:
*  OS_SUCCESS
*  OS_INVALID_REF
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSA_SemaPoll( OS_Sema_t *sema, UINT32 *count )
{
	int status;

	status = sem_getvalue(&sema->os_sema, (int*)count);

	if ( status == -1)
	{
		/* Do error code translation to generic OS errors */
		OSA_ERROR(errno);
		return(OSA_TranslateErrorCode(errno));
	}
	else
	{
		return(OS_SUCCESS);
	}
}

#endif

#if (OSA_MUTEXES)
/***********************************************************************
*
* Name:        OSA_MutexCreate()
*
* Description: Create a mutex object.
*
* Parameters:
*  OS_Mutex_t *mutex       pointer of the initialized mutex reference
*
* Returns:
*      OS_SUCCESS          successful
*      OS_INVALID_REF      failed, POSIX error code EINVAL
*      OS_FAIL             failed, rest POSIX error codes
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSA_MutexCreate( OS_Mutex_t *mutex, char *name )
{
	UNUSEDPARAM(name)
	int rtn = -1;
	OSA_STATUS status = OS_FAIL;

#if !defined (BIONIC)
	int pshared;
#endif  /* BIONIC */

	rtn = pthread_mutexattr_init(&mutex->attr);
	if (rtn != 0)
	{
		OSA_ERROR(rtn);
		return OSA_TranslateErrorCode(rtn);
	}

#if !defined (BIONIC) /* BIONIC only supports PTHREAD_PROCESS_PRIVATE which is defined by default */
	if (name == NULL)
	{
		/* by any thread that has access to the mutex(or memory),
		 * even shared by multiple processes */
		pshared = PTHREAD_PROCESS_PRIVATE;
	}
	else
	{  /* only by threads within the same process as the thread
	    * that initialized the mutex */
		pshared = PTHREAD_PROCESS_SHARED;
	}
	rtn = pthread_mutexattr_setpshared(&mutex->attr, PTHREAD_PROCESS_PRIVATE);
	if (rtn != 0)
	{
		OSA_ERROR(rtn);
		return OSA_TranslateErrorCode(rtn);
	}

#endif  /* BIONIC */

#if !defined (BIONIC) /* only PTHREAD_MUTEX_RECURSIVE supported in BIONIC */
	rtn = pthread_mutexattr_settype(&mutex->attr, PTHREAD_MUTEX_RECURSIVE_NP);
#else
	rtn = pthread_mutexattr_settype(&mutex->attr, PTHREAD_MUTEX_RECURSIVE);
#endif
	if (rtn != 0)
	{
		return OSA_TranslateErrorCode(rtn);
	}

	/* Initialize A POSIX mutex, with default attribute value */
	rtn = pthread_mutex_init(&mutex->os_mutex, &mutex->attr);
	if ( rtn == 0 )
	{
		/* successfully */
		mutex->locked = FALSE;
		mutex->owner  = 0;
		status = OS_SUCCESS;
	}
	else
	{
		OSA_ERROR(rtn);
		pthread_mutexattr_destroy(&mutex->attr);
		/* Do error code translation to generic OS errors */
		/* Posix Error Code:
		 * EAGAIN   lacked the necessary resources (other than memory) to initialize another mutex
		 * ENOMEM   Insufficient memory exists to initialize the mutex
		 * EPERM    The caller does not have the privilege to perform the operation
		 * EBUSY    reinitialize an existing mutext
		 * EINVAL   invalid attr value
		 */
		status = OSA_TranslateErrorCode(rtn);
	}
	return status;
}

/***********************************************************************
*
* Name:        OSA_MutexDelete()
*
* Description: Delete a mutex.
*
* Parameters:
*  OS_Mutex_t mutex         - mutex reference
*
* Returns:
*  OS_SUCCESS
*  OS_FAIL
*  OS_INVALID_REF
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSA_MutexDelete( OS_Mutex_t *mutex )
{
	int rtn;
	OS_STATUS status = OS_FAIL;


	rtn = pthread_mutex_destroy(&(mutex->os_mutex));
	if (rtn != 0)
	{
		OSA_ERROR(rtn);
		return OSA_TranslateErrorCode(rtn);
	}

	rtn = pthread_mutexattr_destroy(&mutex->attr);
	if ( rtn == 0 )
	{
		mutex->owner  = 0;
		mutex->locked = FALSE;
		status = OS_SUCCESS;
	}
	else
	{
		OSA_ERROR(rtn);
		status = OSA_TranslateErrorCode(rtn);
	}

	return status;
}

/***********************************************************************
*
* Name:        OSA_MutexLock()
*
* Description: Block calling task until lock is acquired
*
* Parameters:
*  OS_Mutex_t mutex         - mutex reference
*
* Returns:
*  OS_SUCCESS
*  OS_INVALID_REF
*
* Notes:       It is up to the application to ensure that this function
*              is not called on a deleted mutex and that it is not already
*              locked by the same task.
*
***********************************************************************/
OSA_STATUS OSA_MutexLock( OS_Mutex_t *mutex, UINT32 timeout )
{
	int rtn = -1;
	OS_STATUS status = OS_FAIL;


	switch (timeout)
	{
	case OSA_NO_SUSPEND:
		/* return immediately*/
		rtn = pthread_mutex_trylock(&(mutex->os_mutex));
		break;

	case OSA_SUSPEND:
		/* blocked call */
		rtn = pthread_mutex_lock(&(mutex->os_mutex));
		break;

	default:
	{
	#if !defined (BIONIC)
		struct timespec abs_timeout;
		/*  timeout */
		/* Assume the timeout is ms based. TBD to convert into clock rate */
		abs_timeout.tv_sec = timeout / 1000;
		abs_timeout.tv_nsec = (timeout % 1000) * 1000;
		rtn = pthread_mutex_timedlock(&(mutex->os_mutex), &abs_timeout);
	#endif
	}
	break;
	}

	if ( rtn == 0)
	{
		mutex->locked = TRUE;
		mutex->owner = (UINT32)pthread_self();
		status = OS_SUCCESS;
	}
	else
	{
		/* Do error code translation to generic OS errors */
		/* EINVAL
		 * EBUSY
		 * EPERM
		 * ETIMEDOUT
		 * EAGAIN
		 * EDEADLK
		 */
		OSA_ERROR(rtn);
	}

	return status;

}

/***********************************************************************
*
* Name:        OSA_MutexUnlock()
*
* Description: Unlock the mutex
*
* Parameters:
*  OS_Mutex_t mutex         - mutex reference
*
* Returns:
*  OS_SUCCESS
*  OS_FAIL(?)
*
* Notes:       It is up to the application to ensure that this function
*              is not called on a deleted mutex and that it is not already
*              locked by the same task.
*
***********************************************************************/
OSA_STATUS OSA_MutexUnlock( OS_Mutex_t *mutex )
{
	int rtn;
	OS_STATUS status = OS_FAIL;

	rtn = pthread_mutex_unlock(&(mutex->os_mutex));

	if (rtn == 0)
	{
		mutex->owner  = 0;
		mutex->locked = FALSE;
		status = OS_SUCCESS;
	}
	else
	{
		/* Do error code translation to generic OS errors */
		/* EPERM    The current thread does not own the mutex*/
		status = OSA_TranslateErrorCode(rtn);
	}

	return status;
}
#endif

#if (OSA_MEM_POOLS)
/***********************************************************************
*
* Name:        OSA_MemPoolCreate()
*
* Description: Create a memory pool.
*
* Parameters:
*
*  void*       pool            - Application assigned pool reference
*  UINT32       poolSize        - total number of bytes in the memory pool
*
* Returns:
*
*  OS_SUCCESS          - successful completion of the service
*  OS_INVALID_REF      - bad pool reference
*  OS_INVALID_SIZE     - bad pool or partition size
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSA_MemPoolCreate(void *poolRef, char *name, UINT32 poolType,
			     UINT8 *poolBase, UINT32 poolSize, UINT32 partitionSize)
{
	UNUSEDPARAM(poolRef)

	UINT32 numPartitions;
	OS_PartMemHdr   *headerPtr;
	UINT32 index;
	OSA_STATUS status;
	OS_Mem          *poolPtr;
	static OS_PartitionPool_t *pool;
#ifndef BIONIC
	UINT32 size;
	int fd;
#endif
	key_t key = 0;
	UINT32 temp;
	BOOL skipInit = FALSE;
	UINT8           *buf = (UINT8*)name;
	UINT8 i;

	for (i = 0; i < strlen((char*)name); i++)
		key = key + buf[i];

	if (poolType == OSA_VARIABLE)
	{
		return(OS_SUCCESS);
	}

	if (poolType == OSA_GLOBAL)
	{
#ifdef BIONIC
		OSA_MSG("OSAMemPoolCreateGlobal: not support! \n");
		UNUSEDPARAM(poolBase)
		OSA_ASSERT(FALSE)
		return(TRUE);
#else           /* BIONIC */
		/* OSA_GLOBAL reference and pool memory is stored in global mem */
		size = poolSize + sizeof(OS_Mem) + 64;

		fd = shmget(key, size, IPC_CREAT | IPC_EXCL | 0666);

		if ( (fd == -1) && (errno == EEXIST) )
		{
			fd = shmget(key, size, IPC_CREAT | 0666);
			if ( fd == -1 )
			{
				OSA_MSG("OSAMemPoolCreateGlobal: shmget failed\n");
				OSA_ERROR(errno);
				OSA_ASSERT(FALSE)
				return(TRUE);
			}
			else
			{
				/* It exists already so now we need to attach to it*/
				poolPtr = (OS_Mem*)shmat(fd, poolBase, 0);
				if (poolPtr == (OS_Mem*)-1)
				{
					OSA_MSG("OSAMemPoolCreateGlobal: shmgat failed\n");
					OSA_ERROR(errno);
					OSA_ASSERT(FALSE)
					return(TRUE);
				}
				skipInit = TRUE;
			}
		}
		else if ( fd == -1 )
		{
			OSA_MSG("OSAMemPoolCreateGlobal: shmget failed\n");
			OSA_ERROR(errno);
			OSA_ASSERT(FALSE)
			return(TRUE);
		}
		else
		{
			/* Not created yet so now we need to attach to it and clear the mem */
			poolPtr = (OS_Mem*)shmat(fd, poolBase, 0);
			if (poolPtr == (OS_Mem*)-1)
			{
				OSA_MSG("OSAMemPoolCreateGlobal: shmgat failed\n");
				OSA_ERROR(errno);
				OSA_ASSERT(FALSE)
				return(TRUE);
			}
			OSA_MSG("OSAMemPoolCreateGlobal: Clearing mem at: %X", (int)poolPtr);
			memset((void*)poolPtr, 0, (size_t)size);
		}
#endif          /* BIONIC */
	}
	else
	{
		OSA_MSG("OSAMemPoolCreateLocal: not support! \n");
		/* OSA_FIXED reference is stored in control block */
		OSA_ASSERT(FALSE)
		return(TRUE);
	}

#ifndef OSA_SINGLE_PROCESS
	if (!skipInit)
#endif
	{
		OSA_MUTEX_CREATE(&poolPtr->u.fPoolRef.mutex, name, OSA_FIFO, status);
		if (status != OS_SUCCESS)
		{
			OSA_ASSERT(FALSE);
			return(OS_FAIL);
		}
		OSA_SEMA_CREATE(&poolPtr->u.fPoolRef.sema, name, (UINT32)1, (UINT8)OSA_FIFO, status);
		/* Calculate how many partitions we can chop this block into */
		numPartitions = poolSize / (partitionSize + OSA_FIXED_POOL_MEM_OVERHEAD);

		/* Now we can initialize this partition pool */
		pool = (OS_PartitionPool_t*)&poolPtr->u.fPoolRef;
		temp = (UINT32)((UINT32)poolPtr + sizeof(OS_Mem));
		pool->free = (OS_PartMemHdr *)(temp);
		poolPtr->poolBase = (UINT8 *)(temp);
		headerPtr         = pool->free;
		pool->numFree     = numPartitions;
		poolPtr->type     = OSA_GLOBAL;
		poolPtr->poolSize = poolSize;
		poolPtr->partitionSize = partitionSize;
		poolPtr->refCheck = poolPtr;

		/* Init pool list */
		for ( index = 0; index < numPartitions; index++)
		{
			headerPtr->next = (OS_PartMemHdr *)(((UINT32)headerPtr) + (partitionSize + OSA_FIXED_POOL_MEM_OVERHEAD));
			headerPtr->pool = pool;
#ifdef OSA_MEM_CHECK
			headerPtr->size = 0;
			headerPtr->partSize = partitionSize;
#endif
			headerPtr = headerPtr->next;
		}

		(headerPtr - 1)->next =  OS_NULL;
	}

	return(OS_SUCCESS);
}

/***********************************************************************
*
* Name:        OSA_MemPoolDelete()
*
* Description: Delete a memory pool.
*
* Parameters:
*
*  void*       pool            - Application assigned pool reference
*
* Returns:
*
*  OS_SUCCESS          - successful completion of the service
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSA_MemPoolDelete(void *pool, UINT32 poolType)
{
	OS_STATUS status;
	OS_Mem      *poolPtr = (OS_Mem*)pool;

	if ( (poolType == OSA_FIXED) ||
	     (poolType == OSA_GLOBAL) )
	{
		/* Delete the buffer pool */
		OSA_MUTEX_DELETE(poolPtr->u.fPoolRef.mutex, status);
		if (status != OS_SUCCESS)
			return(OS_FAIL);
		OSA_SEMA_DELETE(poolPtr->u.fPoolRef.sema, status);
		if (status != OS_SUCCESS)
			return(OS_FAIL);
		return(OS_SUCCESS);
	}
	else
	{
		return(OS_SUCCESS);
	}
#if 0
	if (poolType == OSA_GLOBAL)
	{
#ifdef BIONIC
		OSA_MSG("OSAMemPoolDeleteGlobal: not support! \n");
		OSA_ASSERT(FALSE)
		return(OS_FAIL);
#else           /* BIONIC */
		int rtn;
		rtn = shmdt(pool);
		if (rtn == -1)
		{
			OSA_MSG("OSAMemPoolDelete shmdt failed\n");
			OSA_ERROR(errno);
			OSA_ASSERT(FALSE)
			return(OS_FAIL);
		}
#endif          /* BIONIC */
	}
#endif
}

/***********************************************************************
*
* Name:        OSA_MemAlloc
*
* Description: Allocate block of memory from OS memory pool
*
* Parameters:
*  OSPoolRef   poolRef     - memory pool reference
*  UINT32      size        - size of memory block in bytes
*
*
* Returns:     void** mem  - non-zero pointer to allocated memory block
*                            or zero if allocation failed.
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSA_MemAlloc(void *pool, UINT32 poolType, UINT32 size, UINT32 partSize, void **mem,
			UINT32 timeout)
{
	void            *memory = NULL;
	OSA_STATUS rtn;
	OS_PartMemHdr   **freeList = OS_NULL;
	OS_PartMemHdr   *memHdr;
	OS_Mem          *poolPtr = (OS_Mem*)pool;

	if ( (poolType == OSA_FIXED) || (poolType == OSA_GLOBAL) )
	{
		OSA_MUTEX_LOCK(poolPtr->u.fPoolRef.mutex, OSA_SUSPEND, rtn);

		/* Check if there are any partitions left */
		if (poolPtr->u.fPoolRef.numFree)
		{
			freeList = &poolPtr->u.fPoolRef.free;
			OSA_GET_FROM_FREE_LIST((*freeList), memHdr);
			poolPtr->u.fPoolRef.numFree--;

#ifdef OSA_MEM_CHECK
			/* Add in A5 signature to the rest of the unused partition
			 * so we can check if the user has over-run before we free
			 */
			memHdr->size = size;
			UINT8 *temp = (UINT8 *)(((UINT32)memHdr) + OSA_FIXED_POOL_MEM_OVERHEAD + size);
			for (UINT32 i = 0; i < (partSize - size); i++, temp++)
				*temp = OSA_MEM_CHECK_SIG;
#else
			UNUSEDPARAM(partSize)
#endif
		}
		else /* None left so we must check for suspend option... */
		{
			if (timeout == OSA_NO_SUSPEND)
			{
				OSA_MUTEX_UNLOCK(poolPtr->u.fPoolRef.mutex, rtn);
				return(OS_NO_MEMORY);
			}
			else /* We are suspending so unprotect it and wait... */
			{
				OSA_MUTEX_UNLOCK(poolPtr->u.fPoolRef.mutex, rtn);
				OSA_SEMA_ACQUIRE(poolPtr->u.fPoolRef.sema, timeout, rtn);
				if (rtn == OS_TIMEOUT)
					return(OS_NO_MEMORY);

				/* There has to be one free now so... get it!*/
				OSA_MUTEX_LOCK(poolPtr->u.fPoolRef.mutex, OSA_SUSPEND, rtn);
				freeList = &poolPtr->u.fPoolRef.free;
				OSA_GET_FROM_FREE_LIST((*freeList), memHdr);
				poolPtr->u.fPoolRef.numFree--;
			}
		}

		OSA_MUTEX_UNLOCK(poolPtr->u.fPoolRef.mutex, rtn);
		*mem = (void *)(((UINT32)memHdr) + OSA_FIXED_POOL_MEM_OVERHEAD);

		return(OS_SUCCESS);
	}
	else
	{
		memory = LinuxMalloc((UINT32)size);
		if (memory)
		{
			*mem = memory;
			return(OS_SUCCESS);
		}
		else
			return(OS_NO_MEMORY);
	}
}

/***********************************************************************
*
* Name:        OSA_MemFree
*
* Description: Deallocate block of memory that was allocated by
*              OSAMemAlloc().
*
* Parameters: void* - pointer to allocated block
*
* Returns:
*
* Notes:
*
***********************************************************************/
OSA_STATUS  OSA_MemFree(void *poolPtr, UINT32 poolType, void* mem)
{
	UNUSEDPARAM(poolPtr)
	OSA_STATUS rtn;
	OS_PartMemHdr   **freeList = OS_NULL;
	OS_PartMemHdr   *memHdr;
	OS_PartitionPool_t  *pPoolPtr;


	if ( (poolType == OSA_FIXED) || (poolType == OSA_GLOBAL) )
	{
		memHdr = (OS_PartMemHdr*)(((UINT32)mem) - OSA_FIXED_POOL_MEM_OVERHEAD);
#ifdef OSA_MEM_CHECK
		/* Check if A5 signature is in the rest of the unused partition
		 * and ASSERT if there is over-run
		 */
		UINT32 size = (memHdr->partSize - memHdr->size);
		UINT8 *temp = (UINT8 *)(((UINT32)memHdr) + OSA_FIXED_POOL_MEM_OVERHEAD + memHdr->size);
		for (UINT32 i = 0; i < size; i++, temp++)
			if (*temp != OSA_MEM_CHECK_SIG)
				OSA_ASSERT(FALSE);
		memHdr->size = 0;
#endif
		pPoolPtr = memHdr->pool;
		OSA_MUTEX_LOCK(pPoolPtr->mutex, OSA_SUSPEND, rtn);
		freeList = &pPoolPtr->free;

		OSA_ADD_TO_FREE_LIST((*freeList), memHdr);
		if (!(pPoolPtr->numFree))
		{
			pPoolPtr->numFree++;
			OSA_SEMA_RELEASE(pPoolPtr->sema, rtn);
		}
		else
		{
			pPoolPtr->numFree++;
		}
		OSA_MUTEX_UNLOCK(pPoolPtr->mutex, rtn);
		return(OS_SUCCESS);
	}
	else
	{
		//ERRMSG("%s: FREE 0x%08X\n",__FUNCTION__,mem);
		LinuxFree(mem);
		return(OS_SUCCESS);
	}
}
#endif

#if (OSA_TIMERS)

#define TIMER_TICK_SYNC_FLAG 0x01

static void clearTimers()
{
	timer_flag = 0;
}

static void enableTimers()
{
	OSA_ControlBlock* block;

	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);

	OSA_SemaRelease(&timerLock);
	timer_flag = 1;
}

static void waitForAvaiableTimer()
{
	if (timer_flag == 0)
	{
		OSA_SemaAcquire(&timerLock, OSA_SUSPEND);
	}
}


/*======================================================================
 *  OS Timer Management
 *====================================================================*/
/***********************************************************************
*
* Name:        OSA_TimerCreate()
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
OSA_STATUS OSA_TimerCreate( OS_Timer_t *timer )
{
	if (timer == NULL)
		return (OS_INVALID_REF);

	/* Initialize the Timer specific data */
	timer->ticksLeft    = 0;
	timer->reschedTicks = 0;
	timer->expirations  = 0;
	timer->callback     = OS_NULL;
	timer->timerArgc    = 0;

	return(OS_SUCCESS);
}

/***********************************************************************
*
* Name:        OSA_TimerDelete()
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
OSA_STATUS OSA_TimerDelete( OS_Timer_t *timer )
{
	if (timer == NULL)
		return (OS_INVALID_REF);
		/* Initialize the Timer specific data */
		timer->ticksLeft    = 0;
	timer->reschedTicks = 0;
	timer->callback     = OS_NULL;
	timer->timerArgc    = 0;

	return(OS_SUCCESS);
}

/***********************************************************************
*
* Name:        OSA_TimerStart()
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
OSA_STATUS OSA_TimerStart( OS_Timer_t *timer, UINT32 ticks, UINT32 resched,
			   void (*handler)(UINT32), UINT32 argc )
{
	if (timer == NULL)
		return (OS_INVALID_REF);

	/* Update the timer data */
	timer->ticksLeft    = ticks;
	timer->reschedTicks = resched;
	timer->callback     = handler;
	timer->timerArgc    = argc;
	timer->expirations  = 0L;

	timer->state  = OSA_TIMER_ACTIVE;
	enableTimers();
	return(OS_SUCCESS);
}

/***********************************************************************
*
* Name:        OSA_TimerStop()
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
OSA_STATUS OSA_TimerStop( OS_Timer_t *timer )
{
	if (timer == NULL)
		return (OS_INVALID_REF);

	/* Set the timer to inactive */
	timer->state = OS_TIMER_INACTIVE;

	/* Do not need to do anything */
	return(OS_SUCCESS);
}

/***********************************************************************
*
* Name:        OSA_TimerTask
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
#if defined(BIONIC)
static void __noreturn OSA_TimerTask(void *argv)
#else
static void OSA_TimerTask(void *argv)
#endif
{
	OS_TimerPool *timerPool;
	OSA_STATUS rt_val;
	OS_Timer    **activeListHead = OS_NULL;
	OS_Timer    *currentNode = OS_NULL;
	OSA_ControlBlock* block = (OSA_ControlBlock*)argv;
	BOOL bFoundActiveTimer = FALSE;

	timerPool = &block->timerPool;

	activeListHead = &timerPool->head;

	while (1)
	{
		/* Wait on the sem to get the next tick */
		OSA_SEMA_ACQUIRE(block->OSA_TickSemaphore, OSA_SUSPEND, rt_val);

		currentNode = *activeListHead;
		bFoundActiveTimer = FALSE;

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
						currentNode->os_ref.callback(currentNode->os_ref.timerArgc);
					}
					else /* Set the timer to inactive and delete it */
					{
						currentNode->os_ref.state = OS_TIMER_INACTIVE;
						currentNode->state = OS_TIMER_INACTIVE;
						currentNode->os_ref.expirations++;

						/* Invoke the handler */
						currentNode->os_ref.callback(currentNode->os_ref.timerArgc);
					}

				}       /* End timer expired */

			}               /* Check if timer is active */

			if (currentNode->os_ref.state == OS_TIMER_ACTIVE)
				bFoundActiveTimer = TRUE;

			/* Go on to next timer */
			currentNode = currentNode->next;

		} /* active list scanned */

		if (!bFoundActiveTimer)
		{
			clearTimers();
		}
	} /* Eternal timer loop  */
}

/***********************************************************************
*
* Name:        OSA_TickTask
*
* Description: This task provides a simulated 1 millisecond tick for the
*              POSIX timer implementation. It sleeps for one millisecond & then
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

#define WAIT_FOR_AVAIALBLE_TIMER() waitForAvaiableTimer()
#if defined(BIONIC)
static void __noreturn OSA_TickTask(void *argv)
#else
static void OSA_TickTask(void *argv)
#endif
{
	OSA_ControlBlock *block = (OSA_ControlBlock*)argv;

	while (1)
	{
		/* Sleep for the configured tick count */
		WAIT_FOR_AVAIALBLE_TIMER();
		OSA_TASK_SLEEP(1);
		OSA_CLOCK_TICK(block);
	}
}
#endif

/***********************************************************************
*
* Name:        OSA_ClockTick
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
void OSA_ClockTick(void *block)
{
	UINT32 rt_val;
	OSA_ControlBlock *temp = (OSA_ControlBlock*)block;

	temp->OSA_RtcCounter++;
	OSA_SEMA_RELEASE(temp->OSA_TickSemaphore, rt_val);
}


/*======================================================================
 *  OS Interrupt Management
 *====================================================================*/
/***********************************************************************
*
* Name:        OSA_SIsrCreate()
*
* Description: Install a second level ISR rountine to an IRQ
*
* Parameters:
*
* Returns:
*
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSA_SIsrCreate (UINT32 isrNum, OS_Hisr_t *sisr, void (*func)(void),
			   UINT8 priority, UINT8 *stack, UINT32 size)
{
	UNUSEDPARAM(isrNum)
	UNUSEDPARAM(sisr)
	UNUSEDPARAM(func)
	UNUSEDPARAM(priority)
	UNUSEDPARAM(stack)
	UNUSEDPARAM(size)

	return(OS_SUCCESS);
}

/***********************************************************************
*
* Name:        OSA_SIsrDelete()
*
* Description: Reset the requested interrupt vector to use the default
*              ISR routine.
*
* Parameters:
*
* Returns:
*
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSA_SIsrDelete (OS_Hisr_t *sisr)
{
	UNUSEDPARAM(sisr)
	return(OS_SUCCESS);
}

/***********************************************************************
*
* Name:        OSA_SIsrNotify()
*
* Description: Activate the second level interrupt service routine for
*              the specified interrupt number.
*
* Parameters:
*  UINT32  isrNum          - interrupt number to activate (0 - 31)
*
* Returns:     Nothing
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSA_SIsrNotify(UINT32 isrNum)
{
	UNUSEDPARAM(isrNum)
	return(OS_SUCCESS);
}

/***********************************************************************
*
* Name:        OSA_TranslateError()
*
* Description: Translates the OS specific error codes to OSA errors.
*
* Parameters:
*  UINT32  osErrCode  - error code returned from the OS to be translated
*
* Returns:     Nothing
*
* Notes:
*
***********************************************************************/
OSA_STATUS OSA_TranslateErrorCode(UINT32 osErrCode)
{

	switch (osErrCode)
	{
	case ENOMSG:
		return(OS_QUEUE_EMPTY);
	case EAGAIN:
		return(OS_UNAVAILABLE);
	case EINVAL:
		return(OS_INVALID_REF);
	case EIDRM:
		return(OS_MSGQ_DELETED);
	case E2BIG:
	case EBADF:
	case EBUSY:
	case ECHILD:
	case EDEADLK:
	case EDOM:
	case EFAULT:
	case EFBIG:
	case EIO:
	case EISDIR:
	case EMFILE:
	case EMLINK:
	case ENAMETOOLONG:
	case ENFILE:
	case ENODEV:
	case ENOENT:
	case ENOEXEC:
	case ENOLCK:
	case ENOMEM:
	case ENOSPC:
	case ENOSYS:
	case ENOTDIR:
	case ENOTEMPTY:
	case ENOTTY:
	case ENXIO:
	case EPERM:
	case EPIPE:
	case ERANGE:
	case EROFS:
	case ESPIPE:
	case ESRCH:
	case EXDEV:
	case EACCES:
	case EEXIST:
	case EINTR:
		OSA_MSG("\nOSA Mapped Error %ld\n", osErrCode);
		return(OS_FAIL);

	default:
		OSA_MSG("\nOSA Unmapped Error %ld\n", osErrCode);
		return(OS_FAIL);
	}
}

#endif /* OSA_LINUX */




////////////////////////////////////////////////////////////////////////////////////////
// Test Program
////////////////////////////////////////////////////////////////////////////////////////
#ifdef OSA_LINUX_CLEAN

int main(int argc, char **argv)
{
	OSA_STATUS status;
	pid_t pid;
	key_t key;
	OSA_ControlBlock *block;

	OSA_MSG("\nMAIN: Starting OSA Clean Utility, PID = %X\n", argc, getpid());

	int fd = shmget(13579, sizeof(OSA_ControlBlock), IPC_CREAT | 0666);

	/* OSA global mem Already exists so we need to attach to it*/
	if ( fd == -1 )
	{
		OSA_MSG("OSA shmget failed\n");
		return(TRUE);
	}
	else
	{
		block = shmat(fd, 0x20000000, 0);
		if (block == (char*)-1)
		{
			OSA_MSG("OSA shmgat failed\n");
			return(TRUE);
		}
		else
		{
			OSA_MSG("\n      ** OSA Clearing Block PID = %X **\n", pid);
			memset((void*)block, NULL, sizeof(OSA_ControlBlock));
		}
	}


	return(0);
}
#endif


