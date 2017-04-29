/*****************************************************************************
* Utility Library
*
* String matching utilities header file
*
*****************************************************************************/

#ifndef _UTL_STR_MATCH_INCLUDED
#define _UTL_STR_MATCH_INCLUDED

#include <string.h>

#include <utlTypes.h>


/*--- Macros ----------------------------------------------------------------*/

/*--- additional return value for incremental comparisons ---*/
#define utlTOO_SHORT -2
/* The value assigned to the above macro assumes:
     utlFAILED  == -1
     utlSUCCESS ==  0 */
#if (utlFAILED == utlTOO_SHORT)
"Error: impractical value defined for utlFAILED";
#endif
#if (utlSUCCESS == utlTOO_SHORT)
"Error: impractical value defined for utlSUCCESS";
#endif


/*--- Prototypes ------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

extern int  utlStrIncrMatch(const char   *s_p,
			    const size_t size,
			    const char   *pattern_p,
			    const bool ignore_case,
			    const bool largest_match);

extern bool utlStrMatch(const char   *s_p,
			const char   *pattern_p,
			const bool ignore_case);

#ifdef __cplusplus
}
#endif

#endif /* _UTL_STR_MATCH_INCLUDED */

