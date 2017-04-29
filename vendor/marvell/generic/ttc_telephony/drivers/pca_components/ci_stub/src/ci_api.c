/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*=========================================================================

   File Name:    ci_api.c

   Description:  This is the implementation of CI Interface functions

   Revision:     Phillip Cho, 0.1

   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code ("Material") are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel’s prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.

   Unless otherwise agreed by Intel in writing, you may not remove or alter this notice or any other notice embedded
   in Materials by Intel or Intel’s suppliers or licensors in any way.

   ======================================================================== */
#include "ci_api_types.h"
#include "ci_api.h"
#include "ci_stub_ttc.h"
#include "ci_mem.h"
#include "ci_trace.h"
#include "ci_task.h"

extern CiServiceGroupID findCiSgId(CiServiceHandle handle);
extern void clientCiSgFreeReqMem(CiRequestArgs *pCiRequestArgs);
/*--------------------------------------------------------------------
 *  Description: Request to register confirmation call-back for shell operations
 *	Input Paras:
 *		ciShConfirm: confirmation call back for shell operation requests
 *		opShHandle: opaque handle identifies the user who call the ciShRegisterReq
 *	Returns:
 *		CIRC_FAIL, if ciShConfirm is NULL; CIRC_INTERLINK_FAIL, if the link is broken, CIRC_SUCCESS, if otherwise.
 *-------------------------------------------------------------------- */
CiReturnCode  ciShRegisterReq (CiShConfirm ciShConfirm,
			       CiShOpaqueHandle opShHandle,
			       CiShFreeReqMem ciShFreeReqMem,
			       CiShOpaqueHandle opShFreeHandle)
{
	UNUSEDPARAM(ciShFreeReqMem)
	UNUSEDPARAM(opShFreeHandle)

	CCI_TRACE(CI_CLIENT_STUB, CI_API, __LINE__, CCI_TRACE_API, "ciShRegisterReq()");

	/* Call CI client stub */
	//return (ciShRegisterReq_1 (ciShConfirm, opShHandle, ciShFreeReqMem, opShFreeHandle));
	return (ciShRegisterReq_1(ciShConfirm, opShHandle));
}


/*--------------------------------------------------------------------
 *  Description: Request to de-register confirmation call-back function for shell operations
 *	Returns:
 *    CIRC_FAIL, if ciShConfirm is NULL;CIRC_INTERLINK_FAIL, if the link is broken, CIRC_SUCCESS, if otherwise.
 *-------------------------------------------------------------------- */
CiReturnCode  ciShDeregisterReq (CiShHandle handle, CiShOpaqueHandle opShHandle)
{
	CCI_TRACE(CI_CLIENT_STUB, CI_API, __LINE__, CCI_TRACE_API, "ciShDeregisterReq()");

	/* Call CI client stub */
	return (ciShDeregisterReq_1(handle, opShHandle));
}


/*--------------------------------------------------------------------
 *  Description:  Send shell operation request from application subsystem to cellular subsystem
 *	Input Parameters:
 *
 *  Output Paramwter: None
 *
 *	Returns:
 * CIRC_FAIL, if ciShConfirm is NULL;CIRC_INTERLINK_FAIL, if the link is broken, CIRC_SUCCESS, if otherwise.
 *-------------------------------------------------------------------- */
CiReturnCode  ciShRequest (CiShHandle shhandle,
			   CiShOper oper,
			   void* reqParas,
			   CiShRequestHandle opHandle)
{
	CCI_TRACE(CI_CLIENT_STUB, CI_API, __LINE__, CCI_TRACE_API, "ciShRequest()");

	/* Call CI client stub */
	return (ciShRequest_1(shhandle, oper, reqParas, opHandle));
}


/*--------------------------------------------------------------------
 *  Description: Send request service primitive from application subsystem into cellular subsystem
 *	Input Parameters:
 *
 *  Output Paramwter: None
 *
 *	Returns:
 * CIRC_FAIL, if ciShConfirm is NULL;CIRC_INTERLINK_FAIL, if the link is broken, CIRC_SUCCESS, if otherwise.
 *-------------------------------------------------------------------- */
CiReturnCode  ciRequest (CiServiceHandle svchandle,
			 CiPrimitiveID primId,
			 CiRequestHandle reqHandle,
			 void* paras)
{
	CiReturnCode ret;
	CiServiceGroupID srvGrID;
	CiRequestArgs ciReqArgs;

	ciReqArgs.handle = svchandle;
	ciReqArgs.primId = primId;
	ciReqArgs.reqHandle = reqHandle;
	ciReqArgs.paras = paras;

	CCI_TRACE3(CI_CLIENT_STUB, CI_API, __LINE__, CCI_TRACE_INFO,
		   "ciRequest(): svchandle 0x%x, primId %d, reqHandle 0x%x", svchandle, primId, reqHandle);
	
	if ( svchandle == ATCI_SAC_SG_INVALID_HANDLE)
		return CIRC_SG_INVALID_HANDLE;

	srvGrID =findCiSgId(svchandle); 
	if ((srvGrID ==CI_SG_ID_SIM)&&(primId==CI_SIM_PRIM_GENERIC_CMD_REQ))	
	{
		ret = ciRequest_Transfer(svchandle, primId, reqHandle,paras);
	}
	else
	{
		/* Call CI client stub */
		ret = ciRequest_1(svchandle, primId, reqHandle, paras);
	}

	/* If the user has registered the req memory release function, we can call it here */
	clientCiSgFreeReqMem((CiRequestArgs *)&ciReqArgs);
	return ret;
}


/*--------------------------------------------------------------------
 *  Description: Send response service primitive from application subsystem into cellular subsystem in response to indication service primitive received
 *	Input Parameters:
 *
 *  Output Paramwter: None
 *
 *	Returns:
 * CIRC_FAIL, if ciShConfirm is NULL;CIRC_INTERLINK_FAIL, if the link is broken, CIRC_SUCCESS, if otherwise.
 *-------------------------------------------------------------------- */
CiReturnCode ciRespond (CiServiceHandle svchandle,
			CiPrimitiveID primId,
			CiIndicationHandle indHandle,
			void* paras)
{
	CCI_TRACE3(CI_CLIENT_STUB, CI_API, __LINE__, CCI_TRACE_INFO,
		   "ciRespond(): svchandle 0x%x, primId %d, indHandle 0x%x", svchandle, primId, indHandle);

	/* Call CI client stub */
	return (ciRespond_1(svchandle, primId, indHandle, paras));
}

/*--------------------------------------------------------------------
 *  Description:
 *	Input Parameters:
 *
 *  Output Paramwter: None
 *
 *	Returns:
 * CIRC_FAIL, if ciConfig is NULL;CIRC_INTERLINK_FAIL, if the link is broken, CIRC_SUCCESS, if otherwise.
 *-------------------------------------------------------------------- */
/*
   CiReturnCode  ciConfig( CiCnfInfoType infoType, void* pCnfInfo )
   {
	CCI_TRACE(CI_CLIENT_STUB, CI_API, __LINE__, CCI_TRACE_API,"ciConfig()");

   switch( infoType )
   {
    case CICNF_INFO_OPT_DATA_BUF_MNGM:
    {
      CiFreeOptDataBuf    ciFreeOptDataBufCallBack;
      CiAllocOptDataBuf   ciAllocOptDataBufCallBack;

      ciFreeOptDataBufCallBack = ((CiCnfOptDataBufMngmInfo*)pCnfInfo)->ciFreeOptDataBuf;
      ciAllocOptDataBufCallBack = ((CiCnfOptDataBufMngmInfo*)pCnfInfo)->ciAllocOptDataBuf;
      break;
    }
    default:
    {
      break;
    }
   }
   }
 */



