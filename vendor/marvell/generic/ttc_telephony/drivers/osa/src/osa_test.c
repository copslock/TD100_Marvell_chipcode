/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.

 *(C) Copyright 2007 Marvell International Ltd.
 * All Rights Reserved
 */

#include "osa.h"
#include "osa_types.h"
#include <string.h>
#include <stdio.h>
#include <sys/time.h>

static struct proc_dir_entry *osa_proc_entry;

typedef struct {
	int op;
	char content[32];
} Internal_Message;

#define MAX_SEND_TASK 5
#define MAX_TEST_MESSAGE_NUM 100000

static OS_Task_t sendTask[MAX_SEND_TASK];
static OS_Task_t recvTask;
static int num = 0;
static unsigned int start, end;

static unsigned int osa_get_tick_count(void)
{
	struct timeval t;
	int ms;

	gettimeofday(&t, NULL);

	ms  = (t.tv_sec * 1000);
	ms += (t.tv_usec / 1000);

	return ms;
}

static void osa_send_task(void *argv)
{
	Internal_Message *msg;
	OS_MsgQ_t *msgQ = (OS_MsgQ_t*)argv;
	UINT32 i = 0;
	int seq = num;

	num++;
	while (i < MAX_TEST_MESSAGE_NUM / MAX_SEND_TASK)
	{
		msg = (Internal_Message*)LinuxMalloc(sizeof(Internal_Message));
		sprintf(msg->content, "(%d)%d", seq, i);
		i++;
		OSA_QueueSend(msgQ, sizeof(Internal_Message), (UINT8*)msg, OS_SUSPEND);
		LinuxFree(msg);
	}
}

static void osa_recv_task(void* argv)
{
	Internal_Message *msg;
	OS_MsgQ_t *msgQ = (OS_MsgQ_t*)argv;
	UINT32 i = 0;
	UINT32 next[MAX_SEND_TASK];
	UINT32 seq;
	UINT32 count;

	memset(next, 0, sizeof(UINT32) * MAX_SEND_TASK);
	while (i < MAX_TEST_MESSAGE_NUM)
	{
		msg = (Internal_Message*)LinuxMalloc(sizeof(Internal_Message));
		OSA_QueueRecv(msgQ, (UINT8*)msg, sizeof(Internal_Message), OS_SUSPEND);
		i++;
		sscanf(msg->content, "(%d)%d", &seq, &count);
		if (next[seq] != count)
		{
			printf("ERROR: receive content does not keep order\n");
			exit(0);
		}
		next[seq] = count + 1;
		LinuxFree(msg);
	}
	OSA_QueueDelete(msgQ);
	LinuxFree(msgQ);
	printf("DONE\n");
	end = osa_get_tick_count();
	printf("Finish receiving, spend total %d ms\n", end - start);
}

static int osa_msg_test1(void)
{
	UINT8 *msg;
	OSA_STATUS status;
	int ret = 0;
	OS_MsgQ_t msgQ;

	OSA_QueueCreate(&msgQ, "message queue", sizeof(Internal_Message), 20, 0, OS_FIFO);
	msg = (UINT8*)LinuxMalloc(sizeof(Internal_Message) + 1);
	/* Test handling large message */
	status = OSA_QueueSend(&msgQ, sizeof(Internal_Message) + 1, (UINT8*)msg, OS_SUSPEND);
	if (status != OS_INVALID_SIZE)
	{
		ret = -1;
		printf("ERROR: large message should not be sent\n");
	}

	/* Test pass little memory than existing message */
	OSA_QueueSend(&msgQ, sizeof(Internal_Message), (UINT8*)msg, OS_SUSPEND);
	status = OSA_QueueRecv(&msgQ, (UINT8*)msg, sizeof(Internal_Message) - 1, OS_SUSPEND);
	if (status != OS_INVALID_SIZE)
	{
		ret = -1;
		printf("ERROR: small memory to receive message\n");
	}

	LinuxFree(msg);
	OSA_QueueDelete(&msgQ);
	printf("DONE\n");
	return ret;
}

static int osa_msg_test2(void)
{
	OS_MsgQ_t *msgQ = (OS_MsgQ_t*)LinuxMalloc(sizeof(OS_MsgQ_t));
	int i;

	num = 0;
	start = osa_get_tick_count();
	OSA_QueueCreate(msgQ, "message queue", sizeof(Internal_Message), 20, 0, OS_FIFO);
	OSA_TaskCreate(&recvTask, "recv message", 0, 0, osa_recv_task, msgQ);
	for (i = 0; i < MAX_SEND_TASK; i++)
	{
		OSA_TaskCreate(&sendTask[i], "send message", 0, 0, osa_send_task, msgQ);
	}
	return 0;
}

static int osa_init(void)
{
	int result = 0;
	OSA_ControlBlock* block = NULL;

	OSA_GetControlBlock((void*)&block, sizeof(OSA_ControlBlock), FALSE);
	printf("OSA init\n");
	return 0;
}

int main()
{
	osa_init();
	osa_msg_test1();
	osa_msg_test2();
	while (1)
	{
		sleep(1);
	}
	;
	return 0;
}

