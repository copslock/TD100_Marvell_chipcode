// (C) Copyright 2006 Marvell International Ltd.
// All Rights Reserved
//
/******************************************************************************
**
** INTEL CONFIDENTIAL
** Copyright 2000-2005 Intel Corporation All Rights Reserved.
**
** The source code contained or described herein and all documents
** related to the source code (Material) are owned by Intel Corporation
** or its suppliers or licensors.  Title to the Material remains with
** Intel Corporation or its suppliers and licensors. The Material contains
** trade secrets and proprietary and confidential information of Intel
** or its suppliers and licensors. The Material is protected by worldwide
** copyright and trade secret laws and treaty provisions. No part of the
** Material may be used, copied, reproduced, modified, published, uploaded,
** posted, transmitted, distributed, or disclosed in any way without Intel’s
** prior express written permission.
**
** No license under any patent, copyright, trade secret or other intellectual
** property right is granted to or conferred upon you by disclosure or
** delivery of the Materials, either expressly, by implication, inducement,
** estoppel or otherwise. Any license under such intellectual property rights
** must be express and approved by Intel in writing.
**
*****************************************************************************/
#ifndef __PARSER_H__
#define __PARSER_H__


#include "nvm_shared.h"
#include "NVMServer_defs.h"
#include "NVMServer_api.h"

typedef enum
{
	NVM_PARSER_RESULT_DONE = 0,
	NVM_PARSER_RESULT_INVALID_OP_CODE,
	NVM_PARSER_RESULT_NO_TRANSMIT_FUNCTION,
	NVM_PARSER_RESULT_NO_MEMORY,
	NVM_PARSER_RESULT_CANNOT_FREE_MEMORY,
	NVM_PARSER_RESULT_FAIL
}NVM_PARSER_RESULT;

/*******************************************************************************
* Function:		ResponseNotify
********************************************************************************
* Description:	This function will be called by HandleReqBufferAndResponse if
*				there is responses need to be send to GPC task.
*
* Parameters:	pResponseBuffer [in] - Pointer of response data buffer
*				dwResponseBufferLen [in] - Response data buffer length
*
* Return value:
*
* Notes: After this function return, the response buffer will be gone. Client has
*		 responsibility to backup buffer or make sure the transmission could be
*		 done before this function return.
*******************************************************************************/
typedef void (*ResponseNotify)( PUINT8 pResponseBuffer, UINT32 dwResponseBufferLen);

/*******************************************************************************
* Function:		HandleReqBufferAndResponse
********************************************************************************
* Description:	API to parse incoming Rx Buffer and then handle, prepare response
*				Tx Buffer and send it out.
*
* Parameters:	ClientID [in] - Client's ID
*				pRxBuffer [in] - Incoming Rx Buffer from GPC
*				dwBufferLen [in] - Incoming Rx Buffer length
*
* Return value:	Return result after this function called
*
* Notes: This function will send response data out via callback funtion.
*		 Make sure register callback function before use it.
*******************************************************************************/
int HandleReqBufferAndResponse( NVM_CLIENT_ID ClientID,
				PUINT8 pRxBuffer, UINT32 dwBufferLen,
				PUINT8 pRspBuffer);

#endif

