/*
 *
 *  Audio server ipc
 *
 */


#ifndef __AUDIO_RINGBUF_H__
#define __AUDIO_RINGBUF_H__

#include <pthread.h>


#define MAX_PACKET_NUM 102
#define DELAY_PACKET_NUM 5

typedef struct {
	int len;
	char* buf;
} RINGINFO;

typedef struct {
	int ring_in;
	int ring_out;
	pthread_mutex_t ring_mutex;
	RINGINFO ring[MAX_PACKET_NUM];
} ringqueue;

extern void ring_buffer_init(ringqueue *ringbuf);
extern void ring_buffer_destroy(ringqueue *ringbuf);
extern int ring_buffer_in(ringqueue* ringbuf, RINGINFO buffer);
extern int ring_buffer_out(ringqueue* ringbuf, RINGINFO* buffer);
extern int ring_buffer_length(ringqueue* ringbuf);

#endif

