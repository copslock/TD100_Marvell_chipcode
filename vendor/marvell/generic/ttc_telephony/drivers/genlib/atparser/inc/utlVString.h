/*****************************************************************************
* Utility Library
*
* Variable-length string utilities header file
*
*****************************************************************************/

#ifndef _UTL_VSTRING_INCLUDED
#define _UTL_VSTRING_INCLUDED

#include <stdarg.h>
#include <stdlib.h>

#include <utlTypes.h>


/*--- Data Types ------------------------------------------------------------*/

typedef struct utlVString_S {
	char   *s_p;
	size_t length;  /* length of string in characters */
	size_t size;    /* size   of buffer in characters */
}             utlVString_T, *utlVString_P;
typedef const utlVString_T  *utlVString_P2c;

#define utlEMPTY_VSTRING { NULL, 0, 0 }

#define utlVString(v_string_p)       ((v_string_p)->s_p)
#define utlVStringLength(v_string_p) ((v_string_p)->length)
#define utlVStringEnd(v_string_p)    ((v_string_p)->s_p + (v_string_p)->length)

#define utlVStringTruncate(v_string_p, c_p) { *(c_p) = '\0'; (v_string_p)->length = (c_p) - (v_string_p)->s_p; }

#define utlInitVString(v_string_p) { \
		(v_string_p)->s_p    = NULL;	  \
		(v_string_p)->length = 0;	   \
		(v_string_p)->size   = 0;	    \
}


/*--- Prototypes ------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

extern utlReturnCode_T utlVStringReserve(const utlVString_P v_string_p,
					 const size_t n);
extern utlReturnCode_T utlVStringCat(const utlVString_P v_string_p,
				     const char           *s_p);
extern utlReturnCode_T utlVStringFree(const utlVString_P v_string_p);
extern int             utlVStringVPrintF(const utlVString_P v_string_p,
					 const char           *format_p,
					 va_list va_arg_p);
extern int             utlVStringPrintF(const utlVString_P v_string_p,
					const char           *format_p,
					...);
extern int             utlVStringPuts(const utlVString_P2c v_string_p,
				      FILE                 *file_p);

#ifdef __cplusplus
}
#endif

#endif /* _UTL_VSTRING_INCLUDED */

