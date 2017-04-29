/*
 * Audio timer
 */

#include <stdio.h>
#if defined(BIONIC)
#include <time.h>
#else
#include <sys/time.h>
#endif

/* debug */
#define TIMER_DEBUG 0
#if TIMER_DEBUG
#define dbg(format, arg ...) printf("audio_timer: " format "\n", ## arg)
#else
#define dbg(format, arg ...)
#endif

#ifndef NSEC_PER_USEC
#define NSEC_PER_USEC	1000L
#endif
#ifndef NSEC_PER_SEC
#define NSEC_PER_SEC	1000000000L
#endif

void getnstimeofday(struct timespec *tv)
{
	struct timeval x;

	gettimeofday(&x, NULL);
	tv->tv_sec = x.tv_sec;
	tv->tv_nsec = x.tv_usec * NSEC_PER_USEC;
}

void timespec_sub(struct timespec *t1, const struct timespec *t2)
{
	while (t1->tv_nsec < t2->tv_nsec)
	{
		t1->tv_nsec += NSEC_PER_SEC;
		t1->tv_sec--;
	}

	t1->tv_nsec -= t2->tv_nsec;
	t1->tv_sec -= t2->tv_sec;
}

void timespec_add(struct timespec *t1, const struct timespec *t2)
{
	if (t1->tv_nsec >= NSEC_PER_SEC)
	{
		t1->tv_nsec -= NSEC_PER_SEC;
		t1->tv_sec++;
	}

	t1->tv_nsec += t2->tv_nsec;
	t1->tv_sec += t2->tv_sec;
}

int timespec_cmp (const struct timespec *t1, const struct timespec *t2)
{
	signed long s = t2->tv_sec - t1->tv_sec;

	if (s != 0)
	{
		return s;
	}
	else
	{
		return t2->tv_nsec - t1->tv_nsec;
	}
}

