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
*     File name:      diag_api .c                                                 *
*     Programmer:     Shiri Dolev                                                 *
*                     Ohad Peled                                                  *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                                 *
*       Create Date:  November, 2003                                              *
*                                                                                 *
*       Description: DIAG Tx Definitions - Handle the transmit reports functions. *
*                                                                                 *
*       Notes:                                                                    *
*                                                                                 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "diag_header_external.h"
#include "diag.h"
#include "diag_buff.h"
#include "diag_types.h"

extern globalFlagsStrct gFlg;

//internal data structure
extern _diagIntData_S _diagInternalData;                // limit 8000, warning 5000

DiagBlockReasons diagBlocked = { { 0x00, 0x0, 0x0 } };  //{{0x01,0x0,0x0}};
#define UNUSEDPARAM(param) (void)param;
/*-- Local System  Function Prototypes -----------------------------------------*/

/************************************************************************
* Function: diagStructPrintf_NoText                                    *
************************************************************************
* Description: This function transfers the report parameters to        *
*              diagSendPDU function that sends the report to ICAT.     *
*              This service allows the HS application to send a full   *
*              structure as a parameter within the report to ICAT (the *
*              application needs to specify the structure size).       *
* Parameters:  fmt - pointer to the reprot string                      *
*              data - pointer to the reprot dataStructure              *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
************************************************************************/
#if defined (NO_MACRO_ANSI_C99__VA_ARGS__)
void diagStructPrintf_NoText(void *data, UINT16 length)
#else
void diagStructPrintf_NoText(const DiagTraceDescriptor* traceDesc, void *data, UINT16 length)
#endif
{
	UINT16 localModuleID;
	UINT16 localReportID;
	UINT32 numOfDecreasedBytes;
	TxPDU       *txPduBuffPtr; /* Pointer to 5 KB memory allocation */

#if defined (NO_MACRO_ANSI_C99__VA_ARGS__)
	const DiagTraceDescriptor* traceDesc = diagTraceDesc;
	SetTraceBeFiltered();
#endif

	localModuleID = traceDesc->moduleId;
	localReportID = traceDesc->reportId;
	// now that we have the vars in locals, we can release system
	diag_unlock_G();

	txPduBuffPtr = diagAlignAndHandleErr(length, localReportID, &numOfDecreasedBytes,
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

	/* copy the data to the end of the full new header structure minus the bytes which does not appear */
	memcpy(txPduBuffPtr->data - numOfDecreasedBytes, (UINT8 *)data, length);

	// In case of long message, send pointer to (header + data) and not for data only!!!
#if !defined (DIAGNEWHEADER)
	diagSendPDU(APPLICATION_REPORT, localModuleID, localReportID, (void *)txPduBuffPtr, length, numOfDecreasedBytes);
#else
	diagSendPDU(SAP_TRACE_NOT_ALIGNED_RESPONSE, localModuleID, localReportID, (void *)txPduBuffPtr, length, numOfDecreasedBytes);
#endif  /* DIAGNEWHEADER */
	txPduBuffPtr = 0;

	return;

} /* End of diagStructPrintf   */

/************************************************************************
* Function: diagTextPrintf_NoText                                      *
************************************************************************
* Description: this function sends the report parameters to diagSendPDU    *
*              function that sends the report to the ICAT              *
* Parameters:  text - pointer to the report string                     *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
************************************************************************/
#if defined (NO_MACRO_ANSI_C99__VA_ARGS__)
void diagTextPrintf_NoText(void)
#else
void diagTextPrintf_NoText(const DiagTraceDescriptor* traceDesc)
#endif
{
	UINT16 localModuleID;
	UINT16 localReportID;
	UINT16 length = 0;
	UINT32 numOfDecreasedBytes;
	void        *txPduBuffPtr; /* Pointer to 5 KB memory allocation */

#if defined (NO_MACRO_ANSI_C99__VA_ARGS__)
	const DiagTraceDescriptor* traceDesc = diagTraceDesc;
	SetTraceBeFiltered(); // reset trace block flag
#endif

	localModuleID = traceDesc->moduleId;
	localReportID = traceDesc->reportId;
	// now that we have the vars in locals, we can release system
	diag_unlock_G();

	//Need to Allocate Space for the MSG Header
	txPduBuffPtr = diagAlignAndHandleErr(0, localReportID, &numOfDecreasedBytes,
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

#if !defined (DIAGNEWHEADER)
	diagSendPDU(APPLICATION_REPORT, localModuleID, localReportID, txPduBuffPtr, length, numOfDecreasedBytes);
#else
	diagSendPDU(SAP_TRACE_NOT_ALIGNED_RESPONSE, localModuleID, localReportID, txPduBuffPtr, length, numOfDecreasedBytes);
#endif  /* DIAGNEWHEADER */
	txPduBuffPtr = 0;

	return;

} /* End of diagTextPrintf   */

/************************************************************************
* Function: diagmPrintf                                                *
************************************************************************
* Description: used to send a variable params report                   *
*              to ICAT, it does not depend on the DB                   *
*                                                                      *
* Parameters:  moduleID - CAT 3 definition of the report               *
*              format - the report format                              *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
************************************************************************/
void diagmPrintf( UINT16 moduleID, char *format, ... )
{

	TxPDU *buf;
	UINT16 dataLength;
	UINT32 numOfDecreasedBytes;

	va_list argument_ptr;

	if (!isDiagBlocked())
	{
		buf = diagAlignAndHandleErr(DIAG_MAX_PRINTF_DATA_SIZE, 0, &numOfDecreasedBytes,
#if !defined (DIAGNEWHEADER)
					    TRACE
#else
					    SAP_TRACE_RESPONSE
#endif          /* DIAGNEWHEADER */
					    );
		if (buf == NULL)
		{
			return;
		}

		va_start(argument_ptr, format);

		vsprintf( (char *)(&(buf->data[0]) - numOfDecreasedBytes), format, argument_ptr );

		va_end(argument_ptr);

		dataLength = strlen((char*)(buf->data - numOfDecreasedBytes)) + 1;

#if !defined (DIAGNEWHEADER)
		diagSendPDU(TRACE, moduleID, 0, (void*)buf, dataLength, numOfDecreasedBytes); //changed DIAGM_PRINTF to 0 - compliation problems. (DIAGM_PRINTF is defined in Diagm.h - could possibly be moved to Diag.h
#else
		diagSendPDU(SAP_TRACE_RESPONSE, moduleID, 0, (void*)buf, dataLength, numOfDecreasedBytes);
#endif          /* DIAGNEWHEADER */
		buf = 0;
	}

	return;

}

/*
   API to alloc diag buffer (assuming it is a trace) from its pool. If no memory available.
   report the congestion condition, count dropped traces (also in statistics).
   When congestion condition is cleared - report situation change back.
 */

void * diagAllocBufferWithMemErr(UINT32 size)
{
	void * buf;

	buf = malloc(size);
#if defined (MY_TEST_ENABLE_DIAG)
	// next logic must be protected for re-entrancy - as it updates global variables
	diag_lock(&cpsr);
	if (buf != NULL)
	{
		// In case of success after memory allocation failure:
		if (_diagInternalData.diagMsgsLost_MemAlloc)
		{
			diagSendErrMsg(DIAG_COMMDEV_EXT, diagErrMemoryOK, _diagInternalData._diagNumberOfTracesMEMLost);
			/* diagErrMsgMEMTotalLostMsgs */
			_diagInternalData.diagMsgsLost_MemAlloc = FALSE;
			_diagInternalData._diagNumberOfTracesMEMLost = 0;
		}
	}
	else
	{
		if (!_diagInternalData.diagMsgsLost_MemAlloc) //_diagNumberOfTracesMEMLost == 0)
		{
			_diagInternalData.diagMsgsLost_MemAlloc = TRUE;
			diagSendErrMsg(DIAG_COMMDEV_EXT, diagErrNoMemory, 0);
			// type of err (mem, queu, comdev(), number for rel)
		}
		_diagInternalData._diagNumberOfTracesMEMLost++;
		_diagInternalData.diagNumberOfTracesMEMLostTotal++;
		_diagInternalData.diagNumberOfTracesMEMLostBytes += size;
		// collect the data for statistics
#if defined (DO_APPS_STATS)             // the amount of time the log is produced not in our main path - we do not care!
		d_stat_CP_AP.AP_bytes_produced += (size);
		d_stat_CP_AP.AP_bytes_dropped += (size);
#endif
#if defined (DO_COMM_STATS)  // COMM side
		d_stat_CP_AP.CP_bytes_produced += (size);
		d_stat_CP_AP.CP_bytes_dropped += (size);
#endif
	}
	diag_unlock(cpsr);
#endif
	return buf;
}

/************************************************************************
* Function: diagAlignAndHandleErr                                      *
************************************************************************
* Description: align buffer for message and handle error message       *
*              if buffer can not be allocated		                    *
*                                                                      *
* Parameters:  lendata - lenght of data in addition to header/footer   *
*				msgID - for reporting traces too big (warning, discard)	*
*				// NEW header related data:								*
*				*numOfDecreasedBytes (output) - returns numer of bytes	*
*					of header that are not used							*
*				sap_type - the SAP type (for final header calculation   *
*                                                                      *
* Return value: pointer to allocated buffer (NULL if failed)           *
*                                                                      *
* Notes:                                                               *
************************************************************************/
TxPDU * diagAlignAndHandleErr(UINT32 lendata, UINT32 msgID, UINT32 *numOfDecreasedBytes, UINT16 sap_type)
{

	UNUSEDPARAM(msgID)

	TxPDU               *txPduBuffPtr;

#if defined (MY_TEST_ENABLE_DIAG)
	DIAG_CHECK_CONTEXT_IS_NOT_INT
	if (lendata > _diagInternalData.MsgLimits.diagMaxMsgOutLimit)
	{
		// issue notice and dont send the message itself.
		diagSendErrLenMsg(GetConnectedDev(), diagErrMSGmaxLen, lendata, msgID);
		return NULL;
	}
	else if (lendata > _diagInternalData.MsgLimits.diagMaxMsgOutWarning)
	{
		// issue a warning and continue as usuall
		diagSendErrLenMsg(GetConnectedDev(), diagErrMsgwarningLen, lendata, msgID);
	}
#endif
	/* num of bytes which should be decrease from full structure of new header should take in care the optional */
	/* fields. In version 2 of new header these fields are: time stamp, frame number and counter. In addition,   */
	/* SAP related data is optional field, yet the appearance of this parameter will be checked in other place,  */
	/* is it depends in the type of each diag message */
	*numOfDecreasedBytes = 0;

#if defined (DIAGNEWHEADER)
	// we assume the globals setting is correct from the time we start issue
	// the trace, till we actually send it... we look at the globals here and
	// determine what we will add to the header, but the actual work is done
	// in (setTxDiagTraceHeader) - by looking again at the globals (which might change till then (chances are small, but..)
	*numOfDecreasedBytes = (gFlg.TimeStampExist ? 0 : SIZE_OF_OPTIONS_ONE_FIELD) +
			       (gFlg.MsgCounterExist ? 0 : SIZE_OF_OPTIONS_ONE_FIELD) +
			       (gFlg.FrameNumberExist ? 0 : SIZE_OF_OPTIONS_ONE_FIELD);

#endif  /* DIAGNEWHEADER */

	// will we add SAP related data for this sap? relevant also in old header - the signals did not
	// have it - so we mark it
	if (!(IS_SAP_HAS_RELATED_DATA(sap_type)))
		*numOfDecreasedBytes += SIZE_OF_OPTIONS_ONE_FIELD;

	//	txPduBuffPtr = (TxPDU *)DiagAlignMalloc (sizeof(TxPDU)+lendata+DIAG_SIZEOF(TxSSPPDUFooter)+TX_PDU_FOOTER_SIZE);
	txPduBuffPtr = (TxPDU *)diagAllocBufferWithMemErr(TX_PDU_HEADER_SIZE + lendata + TX_PDU_FOOTER_SIZE);
	return txPduBuffPtr;

}

/************************************************************************
* Function: isDiagBlocked                                      *
************************************************************************
* Description: Returns TRUE if no trace/report can be sent out         *
*              Returns FALSE if trace/report can be sent out           *
*                                                                      *
* Parameters:  NONE													   *
*                                                                      *
* Return value: TRUE - when trace/report CAN NOT be sent	           *
*				FALSE if report / trace CAN BE SENT OUT				   *
*                                                                      *
* Notes:                                                               *
************************************************************************/
UINT32 isDiagBlocked(void)
{
	return diagBlocked.all;
}

/************************************************************************
* Function: isDiagConnected                                      *
************************************************************************
* Description: Returns TRUE if diag is connected					   *
*              Returns FALSE if diag is not connected		           *
*                                                                      *
* Parameters:  NONE													   *
*                                                                      *
* Return value: TRUE - when diag is connected to ICAT		           *
*				FALSE - when diag is not connected to ICAT			   *
*                                                                      *
* Notes:                                                               *
************************************************************************/
UINT32 isDiagConnected(void)
{
	if (diagBlocked.flags.icatNotReady)
		return FALSE;
	else
		return TRUE;
}

/************************************************************************
* Function: isDiagTracePermitted                                      *
************************************************************************
* Description: Returns TRUE if "client" permitted to send traces	   *
*              Returns FALSE if sending prohibited                         *
*                       and message should be just discarded           *
* Parameters:  NONE													   *
*                                                                      *
* Return value: TRUE/FALSE                                                 *
************************************************************************/
UINT32 isDiagTracePermitted(void)
{
	if (diagBlocked.flags.traceProhibited)
		return FALSE;
	else
		return TRUE;
}

/************************************************************************
* Function: isDiagTracePermitted                                      *
************************************************************************
* Description: Prohibit Diag "client" to send traces,				   *
*                      all messages would be just discarded            *
*                      Usable under ASSERT conditions and others       *
* Parameters:  0 to permit tracing									   *
*             !0 to prohibit tracing								   *
************************************************************************/
void setDiagTraceProhibited(UINT8 setProhibit)
{
	if (setProhibit != 0) diagBlocked.flags.traceProhibited = 1;
	else diagBlocked.flags.traceProhibited = 0;
}

/************************************************************************
* Function: setDiagBlockPDU                                      *
************************************************************************
* Description: Blocks the sendPDU item in the diagBlocked struct       *
*                                                                      *
* Parameters:  NONE													   *
*                                                                      *
* Return value: none												   *
*                                                                      *
* Notes:                                                               *
************************************************************************/
void setDiagBlockPDU(void)
{
	diagBlocked.flags.blockSendPDU = TRUE;
}

