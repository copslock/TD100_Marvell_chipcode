/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/******************************************************************************
**
**  INTEL CONFIDENTIAL
**  Copyright 2006 Intel Corporation All Rights Reserved.
**  The source code contained or described herein and all documents related to the source code ("Material") are owned
**  by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
**  its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
**  Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
**  treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
**  transmitted, distributed, or disclosed in any way without Intel’s prior express written permission.
**
**  No license under any patent, copyright, trade secret or other intellectual property right is granted to or
**  conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
**  estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
**  Intel in writing.
**
**  Unless otherwise agreed by Intel in writing, you may not remove or alter this notice or any other notice embedded
**  in Materials by Intel or Intel’s suppliers or licensors in any way.

**
**  FILENAME:       ci_trace.h
**
**  PURPOSE:        This is the header file for the CCI Stub abstracted tracing interface
**                  definition.
**
******************************************************************************/


#ifndef _CCI_TRACE_H_
#define _CCI_TRACE_H_

//#include "msl_trace.h"

enum _CiTraceLevel
{
	CCI_TRACE_ERROR     = 0x0001,
	CCI_TRACE_API       = 0x0002,
	CCI_TRACE_INFO      = 0x0004,
	CCI_TRACE_DEBUG     = 0x0008,
	CCI_TRACE_MISC      = 0x0010,
	CCI_TRACE_INIT      = 0x0100,
	CCI_TRACE_EXCEPT        = 0x0200,
	CCI_TRACE_CHECKSUM  = 0x0400,
	CCI_TRACE_WARNING       = 0x0800,

};
//typedef UINT32 CCI_TRACE_LEVEL;

/*************************/
/* CCI Stub Trace Macros */
/*************************/

#ifndef CCI_TRACE_ON /* Tracing is disabled */

  #define CCI_TRACE(cat1, cat2, cat3, level, fmt)
  #define CCI_TRACE1(cat1, cat2, cat3, level, fmt, val1)
  #define CCI_TRACE2(cat1, cat2, cat3, level, fmt, val1, val2)
  #define CCI_TRACE3(cat1, cat2, cat3, level, fmt, val1, varl, val3)
  #define CCI_TRACEBUF(cat1, cat2, cat3, level, fmt, buffer, length)
  #define CCI_ASSERT(cond)   do { if (!(cond)) { \
					  char buffer[512]; \
					  sprintf(buffer, "ASSERT FAIL AT FILE %s FUNC %s LINE %d", __FILE__, __FUNCTION__, __LINE__); \
					  /*eeh_draw_panic_text(buffer, strlen(buffer), 0);*/ }	\
} while (0)

#else /* Tracing is enabled */


  #if defined ENV_LINUX
	#include <pxa_dbg.h>
	#define CCI_TRACE(cat1, cat2, cat3, level, fmt) { if (CI_TRACE_LEVEL & level) DPRINTF(fmt "\n"); }
	#define CCI_TRACE1(cat1, cat2, cat3, level, fmt, val1) { if (CI_TRACE_LEVEL & level) DPRINTF(fmt "\n", val1); }
	#define CCI_TRACE2(cat1, cat2, cat3, level, fmt, val1, val2) { if (CI_TRACE_LEVEL & level) DPRINTF(fmt "\n", val1, val2); }
	#define CCI_TRACE3(cat1, cat2, cat3, level, fmt, val1, val2, val3) { if (CI_TRACE_LEVEL & level) DPRINTF(fmt "\n", val1, val2, val3); }
	#define CCI_TRACEBUF(cat1, cat2, cat3, level, fmt, buffer, length) { if (CI_TRACE_LEVEL & level) DBGMSG(fmt " buffer=%x, length=%d\n", buffer, length); }
	#define CCI_ASSERT(cond) \
	{																					  \
		if ((cond) == 0)															      \
		{										    \
			CCI_TRACE1(CI_CLIENT_STUB, ASSERT, __LINE__, CCI_TRACE_ERROR, "Assert line %d", __LINE__); \
			CCI_TRACE1(CI_CLIENT_STUB, ASSERT, __LINE__, CCI_TRACE_ERROR, "Assert file %s", __FILE__); \
			OSATaskSleep(500);								\
			while (1) { printf("ASSERT at %d %s", __LINE__, __FILE__); OSATaskSleep(1000); }	    \
		}										    \
	}
   #else        //CP
	#define CCI_TRACE(cat1, cat2, cat3, level, fmt) { if (CI_TRACE_LEVEL & level) UARTprintf(fmt "\r\n"); }
	#define CCI_TRACE1(cat1, cat2, cat3, level, fmt, val1) { if (CI_TRACE_LEVEL & level) UARTprintf(fmt "\r\n", val1); }
	#define CCI_TRACE2(cat1, cat2, cat3, level, fmt, val1, val2) { if (CI_TRACE_LEVEL & level) UARTprintf(fmt "\r\n", val1, val2); }
	#define CCI_TRACE3(cat1, cat2, cat3, level, fmt, val1, val2, val3) { if (CI_TRACE_LEVEL & level) UARTprintf(fmt "\r\n", val1, val2, val3); }
	#define CCI_TRACEBUF(cat1, cat2, cat3, level, fmt, buffer, length) { if (CI_TRACE_LEVEL & level) UARTprintf(fmt " buffer=%x, length=%d\r\n", buffer, length); }


  #endif

#endif /*  CCI_TRACE_ON */

#if defined (INTEL_2CHIP_PLAT)
/* Do ASSERT always! This is not component but system configuration. */
#include "asserts.h"
#include "utils.h"
#undef  CCI_ASSERT
#define CCI_ASSERT(cond)     ASSERT(cond)
#endif

#endif /* _CCI_TRACE_H */

