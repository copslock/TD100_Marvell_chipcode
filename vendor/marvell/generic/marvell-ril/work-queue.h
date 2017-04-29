/******************************************************************************
 * * work-queue.h - work queue handling for Marvell-ril.
 *
 * *(C) Copyright 2011 Marvell International Ltd.
 * * All Rights Reserved
 * ******************************************************************************/

#ifndef RIL_WORK_QUEUE_H
#define RIL_WORK_QUEUE_H

#include <time.h>

typedef void (*work_function_t)(void* arg);

struct WorkQueue;
struct WorkQueue* createWorkQueue(const char *name, int channel, long long timeout_msec);
void enqueDelayed(struct WorkQueue *q, work_function_t function, void *arg,  const struct timeval* relativeTime);
void enque(struct WorkQueue *q, work_function_t function, void *arg);

#endif
