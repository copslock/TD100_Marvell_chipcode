/*
 *
 *  Audio manager for Tavor EVB Platform
 *
 */
#include <stdio.h>
#include "audio_ringbuf.h"

/* debug */
#define RING_DEBUG 0
#if RING_DEBUG
#define dbg(format, arg ...) printf("ring_buffer: " format "\n", ## arg)
#else
#define dbg(format, arg ...)
#endif

void ring_buffer_init(ringqueue *ringbuf)
{
	ringbuf->ring_in = 0;
	ringbuf->ring_out = 0;
}

void ring_buffer_destroy(ringqueue *ringbuf)
{
	RINGINFO buffer;
	while (ring_buffer_out(ringbuf, &buffer) != 0)
	{
		free(buffer.buf);
	}

	ringbuf->ring_in = 0;
	ringbuf->ring_out = 0;
}

int ring_buffer_is_empty(ringqueue* ringbuf)
{
	return ringbuf->ring_in == ringbuf->ring_out;
}

int ring_buffer_length(ringqueue* ringbuf)
{
	int i = ringbuf->ring_in - ringbuf->ring_out;
	return i;
}


int ring_buffer_in(ringqueue* ringbuf, RINGINFO buffer)
{
	pthread_mutex_lock(&ringbuf->ring_mutex);

	if(ring_buffer_length(ringbuf) == MAX_PACKET_NUM) {
		pthread_mutex_unlock(&ringbuf->ring_mutex);
		return 0;
    }

	ringbuf->ring_in = ringbuf->ring_in + 1;
	ringbuf->ring[ringbuf->ring_in % MAX_PACKET_NUM] = buffer;

	pthread_mutex_unlock(&ringbuf->ring_mutex);

	return 1;
}

int ring_buffer_out(ringqueue* ringbuf, RINGINFO* buffer)
{
	pthread_mutex_lock(&ringbuf->ring_mutex);

	if (ring_buffer_is_empty(ringbuf))
	{
		pthread_mutex_unlock(&ringbuf->ring_mutex);
		return 0;
	}

	ringbuf->ring_out = ringbuf->ring_out + 1;
	if(ringbuf->ring_out == ringbuf->ring_in) {
		ringbuf->ring_in = ringbuf->ring_in % MAX_PACKET_NUM;
		ringbuf->ring_out= ringbuf->ring_out % MAX_PACKET_NUM;
	}

	*buffer = ringbuf->ring[ringbuf->ring_out % MAX_PACKET_NUM];

	pthread_mutex_unlock(&ringbuf->ring_mutex);
	return 1;
}

