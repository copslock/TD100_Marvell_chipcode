/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code ("Material") are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel's prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                                 *
*     File name:      diag_api_var .c                                             *
*     Programmer:     Shiri Dolev                                                 *
*                     Ohad Peled                                                  *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                                 *
*       Create Date:  November, 2003                                              *
*                                                                                 *
*       Description: DIAG Tx Multiple Params Definitions.	                       *
*                                                                                 *
*       Notes:                                                                    *
*                                                                                 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include <stdarg.h>
#include "diag.h"
#include "diag_osif.h" //for INCLUDE_ASSERT_OR_DUMMY
#include INCLUDE_ASSERT_OR_DUMMY

#include "diag_API_var.h"

#if defined (USE_DIAG_MEM_POOL)
#include "diag_mem.h"
#endif

// This module in same dll where the next two vars are defined (diag_API.c)
// thus we use diag_export defintion.


/************************************************************************
* Function: diagPrintf                                                 *
************************************************************************
* Description: this function compose the report parameters in a data   *
*              buffer. then the buffer is transferred to diagSendPDU       *
*              function that sends the report to the ICAT.             *
* Parameters:  fmt - pointer to the report string.                     *
*              ... - the report params (variable param implementation) *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
************************************************************************/
#if defined (NO_MACRO_ANSI_C99__VA_ARGS__)
void diagPrintf_Extend(const char *fmt, ...)
#else
void diagPrintf_Extend(const DiagTraceDescriptor* traceDesc, const char *fmt, ...)
#endif
{
	va_list params;
	int i;
	void                *ptrToReportData;
	UINT16 reportDataLength = 0;
	UINT16 localModuleID;
	UINT16 localReportID;
	UINT16 numberOfReportParams;
	UINT32 numOfDecreasedBytes;
	const ParameterInfo *reportParamsInfo;
	TxPDU               *txPduBuffPtr;


#if defined (NO_MACRO_ANSI_C99__VA_ARGS__)
	const DiagTraceDescriptor* traceDesc = diagTraceDesc;

	SetTraceBeFiltered(); // reset trace block flag
	if ( isDiagBlocked() )
	{
		diag_unlock_G();
		return;
	}
#endif

	localModuleID = traceDesc->moduleId;
	localReportID = traceDesc->reportId;
	numberOfReportParams = traceDesc->numOfParams;
	reportParamsInfo = traceDesc->parameters;
	// now that we have the vars in locals, we can release system
	diag_unlock_G();

	txPduBuffPtr = diagAlignAndHandleErr(DIAG_MAX_PRINTF_DATA_SIZE, localReportID, &numOfDecreasedBytes,
#if !defined (DIAGNEWHEADER)
					     APPLICATION_REPORT
#else
					     SAP_TRACE_NOT_ALIGNED_RESPONSE
#endif  /* DIAGNEWHEADER */
					     );
	if (txPduBuffPtr == NULL)
	{
		return;
	}

	/* ^^Barak - changed during ACAT integration with Gregory - 4/9/07 (added "- numOfDecreasedBytes") */
	ptrToReportData = txPduBuffPtr->data - numOfDecreasedBytes;

	va_start(params, fmt);        // params will points to the next byte after the string (fmt)

	if ( (numberOfReportParams != DIAG_STRUCT_REPORT) && (numberOfReportParams != DIAG_TEXT_REPORT) && (numberOfReportParams != DIAG_NVM_REPORT))
	{
		for ( i = 0; i < numberOfReportParams; i++)
		{
			switch ( reportParamsInfo[i].types )
			{
			case DIAG_ENUM:   //should be UINT8 - ISAR
			case DIAG_CHAR:
			{
				char *dest = (char *)ptrToReportData;
				DIAG_ASSERT(dest != NULL);
				*dest = (char)va_arg(params, int);
				dest++;
				ptrToReportData = dest;
				reportDataLength++;
				break;
			}

			case DIAG_CHAR_BUFFER_SIZE_KNOWN:
			case DIAG_ENUM_BUFFER_SIZE_KNOWN:
			{
				char *source = (char *)va_arg(params, int *);
				char *dest = (char *)ptrToReportData;
				DIAG_ASSERT(dest != NULL);
				UINT16 bufferLenInBytes = ((sizeof(char)) * (reportParamsInfo[i].repititions));
				if (bufferLenInBytes > MAX_BUF_LEN_BYTES)
				{
					bufferLenInBytes = MAX_BUF_LEN_BYTES;
				}
				if (source != NULL)
				{
					memcpy(dest, source, bufferLenInBytes);
					dest += bufferLenInBytes;
					ptrToReportData = dest;
					reportDataLength += bufferLenInBytes;
				}
				break;
			}

			case DIAG_CHAR_BUFFER_SIZE_UNKNOWN:
			case DIAG_ENUM_BUFFER_SIZE_UNKNOWN:
			{
				char *source = (char *)va_arg(params, int *);
				char *dest = (char *)ptrToReportData;
				DIAG_ASSERT(dest != NULL);
				DIAG_ASSERT(source != NULL);
				UINT16 bufferLenInBytes = ((sizeof(char)) * (*source)) + 1;
				if (bufferLenInBytes > MAX_BUF_LEN_BYTES)
				{
					bufferLenInBytes = MAX_BUF_LEN_BYTES;
				}
				if (source != NULL)
				{
					memcpy(dest, source, bufferLenInBytes);
					dest += bufferLenInBytes;
					ptrToReportData = dest;
					reportDataLength += bufferLenInBytes;
				}
				break;
			}

			case DIAG_SHORT:
			case DIAG_ENUM_SHORT:
			{
				short source = (short)va_arg(params, int);
				UINT8 *dest = (UINT8 *)ptrToReportData;
				DIAG_ASSERT(dest != NULL);
				*dest++ = (UINT8)source;
				source >>= 8;
				*dest++ = (UINT8)source;
				ptrToReportData = (short *)dest;
				reportDataLength += sizeof(short);
				break;
			}

			case DIAG_SHORT_BUFFER_SIZE_KNOWN:
			case DIAG_ENUM_SHORT_BUFFER_SIZE_KNOWN:
			{
				short *source = (short *)va_arg(params, int *);
				short *dest = (short *)ptrToReportData;
				DIAG_ASSERT(dest != NULL);
				UINT16 bufferLenInBytes = ((sizeof(short)) * (reportParamsInfo[i].repititions));
				if (bufferLenInBytes > MAX_BUF_LEN_BYTES)
				{
					bufferLenInBytes = MAX_BUF_LEN_BYTES;
				}
				if (source != NULL)
				{
					memcpy(dest, source, bufferLenInBytes);
					dest += reportParamsInfo[i].repititions;
					ptrToReportData = dest;
					reportDataLength += bufferLenInBytes;
				}
				break;
			}

			case DIAG_SHORT_BUFFER_SIZE_UNKNOWN:
			case DIAG_ENUM_SHORT_BUFFER_SIZE_UNKNOWN:
			{
				short *source = (short *)va_arg(params, int *);
				short *dest = (short *)ptrToReportData;
				DIAG_ASSERT(dest != NULL);
				DIAG_ASSERT(source != NULL);
				UINT16 bufferLenInBytes = ((sizeof(short)) * ((*source) + 1));
				if (bufferLenInBytes > MAX_BUF_LEN_BYTES)
				{
					bufferLenInBytes = MAX_BUF_LEN_BYTES;
				}
				if (source != NULL)
				{
					memcpy(dest, source, bufferLenInBytes);
					dest += ((*source) + 1);
					ptrToReportData = dest;
					reportDataLength += bufferLenInBytes;
				}
				break;
			}

			case DIAG_LONG:
			case DIAG_ENUM_LONG:
			{
				long source = (long)va_arg(params, int);
				UINT8 *dest = (UINT8 *)ptrToReportData;
				DIAG_ASSERT(dest != NULL);
				*dest++ = (UINT8)source;
				source >>= 8;
				*dest++ = (UINT8)source;
				source >>= 8;
				*dest++ = (UINT8)source;
				source >>= 8;
				*dest++ = (UINT8)source;
				ptrToReportData = (long *)dest;
				reportDataLength += sizeof(long);
				break;
			}

			case DIAG_LONG_BUFFER_SIZE_KNOWN:
			case DIAG_ENUM_LONG_BUFFER_SIZE_KNOWN:
			{
				UINT8 *source = (UINT8 *)va_arg(params, long *);
				UINT8 *dest = (UINT8 *)ptrToReportData;
				DIAG_ASSERT(dest != NULL);
				UINT16 bufferLenInBytes = ((sizeof(long)) * (reportParamsInfo[i].repititions));
				if (bufferLenInBytes > MAX_BUF_LEN_BYTES)
				{
					bufferLenInBytes = MAX_BUF_LEN_BYTES;
				}
				if (source != NULL)
				{
					memcpy(dest, source, bufferLenInBytes);
					dest += reportParamsInfo[i].repititions * sizeof(long);
					ptrToReportData = dest;
					reportDataLength += bufferLenInBytes;
				}
				break;
			}

			case DIAG_LONG_BUFFER_SIZE_UNKNOWN:
			case DIAG_ENUM_LONG_BUFFER_SIZE_UNKNOWN:
			{
				long *source = va_arg(params, long *);
				long *dest = (long *)ptrToReportData;
				DIAG_ASSERT(dest != NULL);
				DIAG_ASSERT(source != NULL);
				UINT16 bufferLenInBytes = (UINT16)((sizeof(long)) * ((*source) + 1));
				if (bufferLenInBytes > MAX_BUF_LEN_BYTES)
				{
					bufferLenInBytes = MAX_BUF_LEN_BYTES;
				}
				if (source != NULL)
				{
					memcpy(dest, source, bufferLenInBytes);
					dest += ((*source) + 1);
					ptrToReportData = dest;
					reportDataLength += bufferLenInBytes;
				}
				break;
			}

			case DIAG_STRING:
			{
				char *source = (char *)va_arg(params, int *);
				char *dest = (char *)ptrToReportData;
				DIAG_ASSERT(dest != NULL);
				if (source != NULL)
				{
					strcpy(dest, source);
					dest += (strlen(source) + 1);
					ptrToReportData = dest;
					reportDataLength += (strlen(source) + 1);
				}
				break;
			}

			default:
			{
				DIAG_FILTER(SW_PLAT, DIAG, ERROR_PRINTFTYPE, DIAG_ERROR)
				diagTextPrintf("DIAG - problem in diagPrintfthe -  Report Param Info -> Type is wrong");
			}
			break;

			}       //end of switch()
		}               //end of for()
	}                       //end of if()
	else
	{
		DIAG_FILTER(SW_PLAT, DIAG, PRINTF_ERROR_IN_PARAMS_NUMBER, DIAG_FATAL_ERROR)
		diagTextPrintf("DIAG - problem in diagPrintf - the Report Params number is wrong, API called is printf - but the code is for another API");
	}

	// LONG MESSAGE
	DIAG_ASSERT(reportDataLength <= (DIAG_MAX_PRINTF_DATA_SIZE));

#if !defined (DIAGNEWHEADER)
	diagSendPDU(APPLICATION_REPORT, localModuleID, localReportID, txPduBuffPtr, reportDataLength, numOfDecreasedBytes);
#else
	diagSendPDU(SAP_TRACE_NOT_ALIGNED_RESPONSE, localModuleID, localReportID, txPduBuffPtr, reportDataLength, numOfDecreasedBytes);
#endif  /* DIAGNEWHEADER */
	txPduBuffPtr = 0;

	va_end(params);
} /* End of diagPrintf   */

