/******************************************************************************
*(C) Copyright 2007 Marvell International Ltd.
* All Rights Reserved
******************************************************************************/
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
#include "GPCTransmit.h"
#include "NVMServer_defs.h"
#include "pxa_dbg.h"
#include "string.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

extern void PrintBuffer(const PUINT8 pBuffer, UINT bufferLength, LPCTSTR pHeader);

CGPCTransmit::CGPCTransmit(void)
{
}

CGPCTransmit::~CGPCTransmit(void)
{
	DeInit();
}

BOOL CGPCTransmit::OnTxConfirmation()
{
	// SetEvent( m_hTxConfEvent);
	OSAFlagSet(m_hTxConfEvent, GPC_TX_EVENT_MASK, OSA_FLAG_OR);
	return TRUE;
}

static __attribute__((noreturn)) void Run(void* arg)
{
	CGPCTransmit* obj = (CGPCTransmit*)arg;

	obj->ThreadRun();
}

BOOL CGPCTransmit::Init(OSAMsgQRef hTxMsgQ_Write, GpcChHandle hNVMChannel)
{
	OSA_STATUS status;

	ASSERT(hTxMsgQ_Write);
	m_hNVMChannel = hNVMChannel;

	m_TxMsgQ_Read = hTxMsgQ_Write;
	// m_hTxConfEvent = CreateEvent( NULL, FALSE, FALSE, NULL);
	status = OSAFlagCreate(&m_hTxConfEvent);
	ASSERT(status == OS_SUCCESS);

	//start the transmitter thread
	// ThreadStart();
	status = OSATaskCreate(&m_hTask, m_TaskStack, GPC_TRANS_TASK_STACK_SIZE, 30,
			       (CHAR*)"GPC Transmit Task", Run, this);
	ASSERT(status == OS_SUCCESS);

	return TRUE;
}
void CGPCTransmit::DeInit()
{
	if ( m_TxMsgQ_Read )
	{
		// this will set terminated flag for thread,thread will exit the loop
		// ThreadTerminated(5000);
		OSATaskDelete(m_hTask);
	}
	if ( m_hTxConfEvent != NULL)
	{
		// CloseHandle(m_hTxConfEvent);
		OSAFlagDelete(m_hTxConfEvent);
	}
}


#define MSGMAX  2048

extern "C" long debug_get_tid();
__attribute__((noreturn)) void CGPCTransmit::ThreadRun()
{

	PUINT8 pTxBuff;
	int NumberRead = 0;
	DWORD dwFlag = 0;
	OSA_STATUS status;

	ASSERT(GPC_MAX_DATA_SIZE < MSGMAX);
	pTxBuff = (UINT8*)MslMemAlloc(GPC_MAX_DATA_SIZE + 4); //VVV
	ASSERT(pTxBuff);
	memset(pTxBuff, 0, GPC_MAX_DATA_SIZE);
	ASSERT(m_TxMsgQ_Read >= 0);

	// CeSetThreadPriority(GetCurrentThread(), 151);
	DEBUGMSG("[NVM Proxy] Tx Transmit thread is running \r\n");

	while (1)
	{
		// Use the message type (long) as the space for GPC_PDU_HEADER
		ASSERT(sizeof(long) == 4);
		status = OSAMsgQRecv(m_TxMsgQ_Read, pTxBuff, GPC_MAX_DATA_SIZE, OSA_SUSPEND);
		ASSERT(status == OS_SUCCESS);
		// Get the length from PDU head
		NumberRead = *((int*)pTxBuff);

		// Clear the PDU head
		*((long*)pTxBuff) = 0;

		DEBUGMSG("[NVM Proxy] send %d byptes\n", NumberRead);
		GPC_TransmitRequest(m_hNVMChannel, pTxBuff, NumberRead, 0);

		UINT32 flag;
		// WaitForSingleObject(m_hTxConfEvent,INFINITE);
		OSAFlagWait(m_hTxConfEvent, GPC_TX_EVENT_MASK, OSA_FLAG_OR_CLEAR,
			    &flag, OSA_SUSPEND);
		//we can free the buffer, since we have confermation that Tx was sent
		DEBUGMSG("[NVM Proxy] Tx confirmation recieved\r\n");
		// ZeroMemory(pTxBuff,GPC_MAX_DATA_SIZE);
		memset(pTxBuff, 0, GPC_MAX_DATA_SIZE);
	}
	//MslMemFree(pTxBuff); VVV
	DEBUGMSG("[NVM Proxy] Tx Transmit thread ended\r\n");
	return;
}

void CGPCTransmit::ReSetGPCHandle(GpcChHandle hNVMChannel)
{
	m_hNVMChannel = hNVMChannel;
}
