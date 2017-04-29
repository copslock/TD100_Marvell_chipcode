/*****************************************************************************
* Utility Library
*
* Time utilities header file
*
*****************************************************************************/

#ifndef _UTL_TIME_INCLUDED
#define _UTL_TIME_INCLUDED

#include <stdlib.h>

#include <utlTypes.h>


/*--- Data Types ------------------------------------------------------------*/

/*--- time ---*/
typedef int utlSecond_T;
typedef int utlNanosecond_T;

/*--- relative time ---*/
typedef struct utlRelativeTime_S {
	utlSecond_T seconds;
	utlNanosecond_T nanoseconds;
}             utlRelativeTime_T, *utlRelativeTime_P;
typedef const utlRelativeTime_T  *utlRelativeTime_P2c;

/*--- epoch-referenced absolute time ---*/
typedef struct utlAbsoluteTime_S {
	utlSecond_T seconds;
	utlNanosecond_T nanoseconds;
}             utlAbsoluteTime_T, *utlAbsoluteTime_P;
typedef const utlAbsoluteTime_T  *utlAbsoluteTime_P2c;


/*--- Prototypes ------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

extern utlReturnCode_T utlCurrentTime(const utlAbsoluteTime_P time_p);
extern utlReturnCode_T utlFormatTime(      char                *buf_p,
					   const size_t n,
					   const utlAbsoluteTime_P2c time_p);
extern utlReturnCode_T utlFormatDate(      char                *buf_p,
					   const size_t n,
					   const utlAbsoluteTime_P2c time_p);
extern utlReturnCode_T utlFormatDateTime(      char                *buf_p,
					       const size_t n,
					       const utlAbsoluteTime_P2c time_p);
extern utlReturnCode_T utlPause(const utlRelativeTime_P2c time_p);

#ifdef __cplusplus
}
#endif

#endif /* _UTL_TIME_INCLUDED */

