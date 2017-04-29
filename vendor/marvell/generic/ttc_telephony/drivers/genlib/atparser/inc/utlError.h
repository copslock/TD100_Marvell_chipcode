/*****************************************************************************
* Utility Library
*
* Error-handling utilities header file
*
*****************************************************************************/

#ifndef _UTL_ERROR_INCLUDED
#define _UTL_ERROR_INCLUDED

#include <assert.h>
#include <errno.h>

#include <utlTypes.h>
#include <string.h> 
#include "pxa_dbg.h"

/*--- Macros ----------------------------------------------------------------*/

#define utlHERE __FILE__, __func__, __LINE__

#define utlHERE_FILE file_path_p
#define utlHERE_FUNC func_name_p
#define utlHERE_LINE line_num

#define utlHERE_DECLARATION const char *utlHERE_FILE, const char *utlHERE_FUNC, const size_t utlHERE_LINE

#define utlHERE_VALUES utlHERE_FILE, utlHERE_FUNC, utlHERE_LINE

extern int eeh_draw_panic_text(const char *panic_text, size_t panic_len, int do_timer);

#define utlAssert(test) if (!utlDoAssert(utlHERE, test, # test)) { \
		char buffer[512]; \
		sprintf(buffer, "ASSERT FAIL AT FILE %s FUNC %s LINE %d", __FILE__, __FUNCTION__, __LINE__); \
		eeh_draw_panic_text(buffer, strlen(buffer), 0); }

#define utlError(...)    ERRMSG(__VA_ARGS__)
#define utlSigError(...) ERRMSG(__VA_ARGS__)

/*--- Prototypes ------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

extern bool utlDoAssert(utlHERE_DECLARATION, const int result, const char *assertion_p);
extern void utlDoError(utlHERE_DECLARATION, const char *msg_p, ...);
extern void utlDoSigError(utlHERE_DECLARATION, const char *msg_p, ...);

#ifdef __cplusplus
}
#endif

#endif /* _UTL_ERRNO_INCLUDED */

