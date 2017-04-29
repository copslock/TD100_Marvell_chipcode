/*****************************************************************************
* Utility Library
*
* Memory-allocation utilities header file
*
*****************************************************************************/

#ifndef _UTL_MALLOC_INCLUDED
#define _UTL_MALLOC_INCLUDED

#include <stdlib.h>

#include <utlTypes.h>
#include <utlError.h>


/*--- Macros ----------------------------------------------------------------*/

#define utlFree(buf_p)             utlDoFree(utlHERE, (void *)buf_p), buf_p = NULL
#define utlFreeConst(buf_p)        utlDoFree(utlHERE, (void *)buf_p)
#define utlMalloc(siz)             utlDoMalloc(utlHERE, siz)
#define utlRealloc(buf_p, siz)     utlDoRealloc(utlHERE, (void *)buf_p, siz)
#define utlCalloc(number_of, siz)  utlDoCalloc(utlHERE,       number_of, siz)


/*--- Prototypes ------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

extern void utlDoFree(utlHERE_DECLARATION, void   *buf_p);
extern void *utlDoMalloc(utlHERE_DECLARATION, size_t siz);
extern void *utlDoRealloc(utlHERE_DECLARATION, void   *buf_p,
			  size_t siz);
extern void *utlDoCalloc(utlHERE_DECLARATION, size_t number_of,
			 size_t siz);

#ifdef __cplusplus
}
#endif

#endif /* _UTL_MALLOC_INCLUDED */

