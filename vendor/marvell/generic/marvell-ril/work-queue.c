/******************************************************************************
 * * work-queue.c - execution of queued functions in a thread
 *
 * *(C) Copyright 2011 Marvell International Ltd.
 * * All Rights Reserved
 * ******************************************************************************/

#include <pthread.h>
#include <alloca.h>

#include "work-queue.h"
#include "marvell-ril.h"

struct WorkEntry {
	work_function_t work_function;
	void *arg;
	struct WorkEntry *next;
};

struct WorkQueue {
	struct WorkEntry *head;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	ThreadContext context;
};

struct WorkDispachInfo {
	work_function_t work_function;
	void *arg;
	struct WorkQueue *q;
};

static struct WorkEntry* newWorkEntry(work_function_t function, void *arg)
{
	struct WorkEntry* entry = malloc(sizeof(struct WorkEntry));
	entry->work_function = function;
	entry->arg = arg;
	entry->next = NULL;
	return entry;
}

static struct WorkEntry* removeFromQueueHead(struct WorkQueue *q)
{
	struct timespec *abstime;
	pthread_mutex_lock(&q->mutex);
	while (q->head == NULL) {
		pthread_cond_wait(&q->cond, &q->mutex);
	}
	struct WorkEntry *p = q->head;
	q->head = p->next;
	pthread_mutex_unlock(&q->mutex);
	return p;
}

static void *WorkQueueRunner(void *arg)
{
	struct WorkQueue* q = (struct WorkQueue*)arg;
	pthread_setname_np(pthread_self(), q->context.threadname);
	setThreadContext(&q->context);
	while(1) {
		struct WorkEntry* p = removeFromQueueHead(q);
		p->work_function(p->arg);
		free(p);
	}
	return NULL;
}

void enque(struct WorkQueue *q, work_function_t function, void *arg)
{
	struct WorkEntry* entry = newWorkEntry(function, arg);
	pthread_mutex_lock(&q->mutex);
	struct WorkEntry **p = &q->head;
	while(*p) p = &(*p)->next;
	*p = entry;
	pthread_cond_broadcast(&q->cond);
	pthread_mutex_unlock(&q->mutex);
}

static void enqueTimedCallback(void *param)
{
	struct WorkDispachInfo* info = (struct WorkDispachInfo*)param;
	enque(info->q, info->work_function, info->arg);
	free(info);
}

void enqueDelayed(struct WorkQueue *q, work_function_t function, void *arg,  const struct timeval* relativeTime)
{
	if (relativeTime == NULL || (relativeTime->tv_sec == 0 && relativeTime->tv_usec == 0))
		enque(q, function, arg);
	else {
		struct WorkDispachInfo* info = malloc(sizeof(struct WorkDispachInfo));
		info->q = q;
		info->work_function = function;
		info->arg = arg;
		RIL_requestTimedCallback(enqueTimedCallback, info, relativeTime);
	}
}

struct WorkQueue* createWorkQueue(const char *name, int channel, long long timeout_msec)
{
	struct WorkQueue* q = calloc(1, sizeof(struct WorkQueue));
	q->head = NULL;
	pthread_mutex_init(&q->mutex, NULL);
	pthread_cond_init(&q->cond, NULL);
	
	strlcpy(q->context.threadname, name, sizeof(q->context.threadname));
	q->context.write_channel = channel;
	q->context.channel_timeout_msec = timeout_msec;
	
	pthread_t tid;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&tid, &attr, WorkQueueRunner, q);
	return q;
}


