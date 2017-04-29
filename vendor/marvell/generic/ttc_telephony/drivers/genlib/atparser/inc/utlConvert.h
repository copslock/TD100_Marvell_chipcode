/*****************************************************************************
* Utility Library
*
*  Data Conversion utilities header file.
*
*****************************************************************************/

#ifndef _UTL_CONVERT_INCLUDED
#define _UTL_CONVERT_INCLUDED

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <utlTypes.h>

#include <utlLinkedList.h>
#include <utlStateMachine.h>
#include <utlTimer.h>
#include <utlVString.h>


/*--- Prototypes ------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

extern size_t utlDecimalToString(      char         *string_p,
				       unsigned int value,
				       const size_t string_siz);
extern size_t utlHexadecimalToString(      char         *string_p,
					   unsigned int value,
					   const size_t string_siz);
extern size_t utlBinaryToString(      char         *string_p,
				      unsigned int value,
				      const size_t string_siz);

#ifdef __cplusplus
}
#endif

#endif /* _UTL_CONVERT_INCLUDED */

