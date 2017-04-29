/*****************************************************************************
* Utility Library
*
* Tracing utilities header file
*
*****************************************************************************/

#ifndef _UTL_TRACE_INCLUDED
#define _UTL_TRACE_INCLUDED

#include <stdio.h>
#include "pxa_dbg.h"



/*--- Macros ----------------------------------------------------------------*/

/*--- the following C99-compliant varargs macro is meant to be used inside
      "utlTrace()" macros as follows:

	 utlTrace(myTraceString,
		  utlPrintTrace("my trace message, value=%d\n", value);
		 );

      This allows one to easily upgrade the trace library to allow trace
      output redirection.
 */
#define utlPrintTrace(...) DBGMSG(__VA_ARGS__)

#define utlTrace(name, source_code) { source_code }

#endif /* _UTL_TRACE_INCLUDED */

