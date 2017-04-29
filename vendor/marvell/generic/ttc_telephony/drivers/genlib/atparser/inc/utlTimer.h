/*****************************************************************************
* Utility Library
*
* Timer utilities header file
*
*****************************************************************************/

#ifndef _UTL_TIMER_INCLUDED
#define _UTL_TIMER_INCLUDED

#include <stdio.h>

#include <utlTypes.h>
#include <utlTime.h>
#include <utlVString.h>


/*--- Data Types ------------------------------------------------------------*/

typedef unsigned long utlTimerId_T;
typedef unsigned long utlTimeOutCount_T, *utlTimeOutCount_P;

#define utlTIMER_INFINITY ((utlTimeOutCount_T) ~0)

typedef enum {
	utlTIMER_QUERY_IS_RUNNING,
	utlTIMER_QUERY_IS_ABSOLUTE,
	utlTIMER_QUERY_NUM_CYCLES,
	utlTIMER_QUERY_CYCLES_LEFT,
	utlTIMER_QUERY_ELAPSED_TIME,
	utlTIMER_QUERY_TIME_TO_TIMEOUT
} utlTimerQuery_T;

typedef utlReturnCode_T (*utlTimerFunction_P)(const utlTimerId_T id,
					      const utlTimeOutCount_T time_out_count,
					      void                *arg_p,
					      const utlAbsoluteTime_P2c curr_time_p);


/*--- Prototypes ------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

extern utlReturnCode_T utlInitTimers(void);
extern utlTimerId_T    utlStartAbsoluteTimer(const utlAbsoluteTime_P2c tmo_time_p,
					     const utlTimerFunction_P function_p,
					     void                *arg_p);
extern utlTimerId_T    utlStartTimer(const utlRelativeTime_P2c period_p,
				     const utlTimeOutCount_T num_cycles,
				     const utlTimerFunction_P function_p,
				     void                *arg_p);
extern utlReturnCode_T utlStopTimer(const utlTimerId_T timer_id);
extern utlReturnCode_T utlQueryTimer(const utlTimerId_T timer_id,
				     const utlTimerQuery_T what,
				     ...);
extern bool            utlIsTimerRunning(const utlTimerId_T timer_id);

#if defined(utlDEBUG) || defined(utlTEST)
extern void        utlVStringDumpTimers(const utlVString_P v_string_p,
					const char                *prefix_p);
extern void        utlDumpTimers(      FILE                *file_p);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _UTL_TIMER_INCLUDED */

