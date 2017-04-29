/*****************************************************************************
* Utility Library
*
* Common Data Types Header File
*
*****************************************************************************/

#ifndef _UTL_TYPES_INCLUDED
#define _UTL_TYPES_INCLUDED

#include <ctype.h>
#include <stdbool.h>


/*--- Data Types ------------------------------------------------------------*/

/*--- function return codes ---*/
typedef int utlReturnCode_T, *utlReturnCode_P;
typedef const utlReturnCode_T  *utlReturnCode_P2c;

#define utlSUCCESS  0
#define utlFAILED  -1

/*--- data identifiers ---*/
typedef unsigned int utlDataId_T, *utlDataId_P;
typedef const utlDataId_T  *utlDataId_P2c;

typedef enum {
	utlDATA_PARITY_UNKNOWN,
	utlDATA_PARITY_NONE,
	utlDATA_PARITY_ODD,
	utlDATA_PARITY_EVEN,
	utlDATA_PARITY_MARK,
	utlDATA_PARITY_SPACE
} utlDataParity_T, *utlDataParity_P;

/*--- macros ----------------------------------------------------------------*/

#define utlNumberOf(n) (sizeof(n) / sizeof((n)[0]))

#define utlAsString(s) # s
#define utlStringize(s) utlAsString(s)

/*--- versions of ctype.h routines that properly handle 8-bit-per-character signed integers ---*/
#define utlIsLower(c)   islower(((c == EOF) && (sizeof(c) > 1)) ? EOF : (int)(0xff & c))
#define utlIsUpper(c)   isupper(((c == EOF) && (sizeof(c) > 1)) ? EOF : (int)(0xff & c))
#define utlIsAlpha(c)   isalpha(((c == EOF) && (sizeof(c) > 1)) ? EOF : (int)(0xff & c))
#define utlIsDigit(c)   isdigit(((c == EOF) && (sizeof(c) > 1)) ? EOF : (int)(0xff & c))
#define utlIsAlNum(c)   isalnum(((c == EOF) && (sizeof(c) > 1)) ? EOF : (int)(0xff & c))
#define utlIsXDigit(c) isxdigit(((c == EOF) && (sizeof(c) > 1)) ? EOF : (int)(0xff & c))
#define utlIsPunct(c)   ispunct(((c == EOF) && (sizeof(c) > 1)) ? EOF : (int)(0xff & c))
#define utlIsGraph(c)   isgraph(((c == EOF) && (sizeof(c) > 1)) ? EOF : (int)(0xff & c))
#define utlIsPrint(c)   isprint(((c == EOF) && (sizeof(c) > 1)) ? EOF : (int)(0xff & c))
#define utlIsBlank(c)   isblank(((c == EOF) && (sizeof(c) > 1)) ? EOF : (int)(0xff & c))
#define utlIsSpace(c)   isspace(((c == EOF) && (sizeof(c) > 1)) ? EOF : (int)(0xff & c))
#define utlIsCntrl(c)   iscntrl(((c == EOF) && (sizeof(c) > 1)) ? EOF : (int)(0xff & c))

#define utlToLower(c)   tolower((int)(0xff & c))
#define utlToUpper(c)   toupper((int)(0xff & c))

#endif /* _UTL_TYPES_INCLUDED */

