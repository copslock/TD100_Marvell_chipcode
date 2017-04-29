/*
 *
 *  Audio timer
 *
 */


#ifndef __AUDIO_TIMER_H__
#define __AUDIO_TIMER_H__

#include <time.h>

extern void getnstimeofday(struct timespec *tv);

extern void timespec_sub(struct timespec *t1, const struct timespec *t2);
extern void timespec_add(struct timespec *t1, const struct timespec *t2);
extern int timespec_cmp(const struct timespec *t1, const struct timespec *t2);

#endif

