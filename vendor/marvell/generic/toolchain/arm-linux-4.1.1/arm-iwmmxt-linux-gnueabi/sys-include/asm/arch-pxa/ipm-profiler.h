/*
 * Bulverde Performance profiler and Idle profiler Routines
 *
 * Copyright (c) 2003 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef IPM_PROFILER_H
#define	IPM_PROFILER_H

#include <asm/arch/pmu.h>
#include <asm/arch/xscale-pmu.h>

#define IPM_IDLE_PROFILER	1
#define IPM_PMU_PROFILER	2

#define	DEFAULTWINDOWSIZE	200

struct ipm_profiler_result {
	struct pmu_results pmu;
	unsigned int busy_ratio;	/* CPU busy ratio */
	unsigned int window_size;
};

struct ipm_profiler_arg {
	unsigned int size;		/* size of ipm_profiler_arg */
	unsigned int flags;
	unsigned int window_size;	/* in microseconds */
	unsigned int pmn0;
	unsigned int pmn1;
	unsigned int pmn2;
	unsigned int pmn3;
};

extern volatile int 	hlt_counter;

#if 0

#define IPROF_MAX_THRESHOLDS 32
#define	IPM_IDLE_PROFILER	1	

enum PerfState
{
	idle,
	cpuBound,
	memoryBound,
	memoryAndCpuBound
};


struct iprof_threshold
{
    unsigned int num_thresholds;
    unsigned int thresholds[IPROF_MAX_THRESHOLDS];	/* insert low edge @ 0	*/
};

struct iprof_descr
{
	unsigned long CPU_Utilization;	
    unsigned int current_threshold;
    unsigned int previous_threshold;
    struct iprof_threshold threshold;
};

/* Event0 - System is idling state	*/
#define	PERFORMANCE_PROFILER_EVENT0	0x0
	
/* Event1 - System is memory bound	*/
#define	PERFORMANCE_PROFILER_EVENT1	0x1

/* Event2 - System is CPU bound		*/
#define	PERFORMANCE_PROFILER_EVENT2	0x2

/* Event3 - System is memory and CPU bound	*/
#define	PERFORMANCE_PROFILER_EVENT3	0x3


/*	
 *	Below is some example event info.	Hardcoded for testing purposes.
 *	Will eventually define thresholds at init and /proc/sys
 */


unsigned int 	curr_idle_start_time=0;
unsigned int 	curr_idle_stop_time=0;
unsigned int 	curr_time_idle=0;
extern unsigned long 	WindowSize; 
extern volatile int 	hlt_counter;


void ipm_ip_postevent(void);
void ipm_gather_idle_stats(void);
void iprof_timeout_proc(void);
void ipm_idle_profiler_init(void);

extern void default_idle(void);
extern void ipm_event_notify(int  type, int kind, int info);
#endif
#endif
