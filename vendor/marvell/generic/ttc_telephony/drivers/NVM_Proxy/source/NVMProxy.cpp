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
** posted, transmitted, distributed, or disclosed in any way without Intel?s
** prior express written permission.
**
** No license under any patent, copyright, trade secret or other intellectual
** property right is granted to or conferred upon you by disclosure or
** delivery of the Materials, either expressly, by implication, inducement,
** estoppel or otherwise. Any license under such intellectual property rights
** must be express and approved by Intel in writing.
**
*****************************************************************************/
#include "NVMProxy.h"
#include "NVMServer_defs.h"
#include "parser.h"
#include "pxa_dbg.h"
#include "osa.h"
#include "string.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#ifdef BIONIC
#include <hardware_legacy/power.h>
#endif
#include <linux/param.h>
#ifndef BIONIC
#include <sys/msg.h>
#endif
#include "NVMDiag.h"
#include "BackNVM.h"
#include "msocket_api.h"
#ifdef DEBUG_NVM_PROXY
#include <sys/time.h>
#endif

#define NVM_POLL_TASK_STACK_SIZE 1024
OSTaskRef NVMPollTaskRef = NULL;
UINT8 NVMPollTaskStack[NVM_POLL_TASK_STACK_SIZE];

void PrintBuffer(const PUINT8 pBuffer, UINT bufferLength, LPCTSTR pHeader);
void shmTask(void *input) __attribute__((noreturn));

extern "C" void ffs_pwrSafe_Init(char* nvm_root_dir);

CNVMProxy* CNVMProxy::m_hInstance = NULL;

CNVMProxy::CNVMProxy(void)
{
	ASSERT(m_hInstance == NULL);
	m_hInstance = this;
//	m_ServDesc = 0;
//	m_hTxQueue = NULL;
//	m_gpcTransmitThread = NULL;
}

CNVMProxy::~CNVMProxy(void)
{
	DeInit();

}

void CNVMProxy::NVMSendMsgToCP(int procId, unsigned char *msg, int msglen)
{
	int ret;
	ShmApiMsg *pShmArgs;

#ifdef DEBUG_NVM_PROXY
	struct timeval start;
	struct timeval end;
	float timeuse;
#endif
	ASSERT(msg != NULL);

	pShmArgs = (ShmApiMsg *)msg;
	pShmArgs->svcId = NVMSvc_Id;
	pShmArgs->msglen = msglen;
	pShmArgs->procId = procId;
#ifdef DEBUG_NVM_PROXY
	gettimeofday(&start, NULL);
#endif
	ret = msend(nvmsrvsockfd, msg, msglen + SHM_HEADER_SIZE, 0);
#ifdef DEBUG_NVM_PROXY
	gettimeofday(&end, NULL);
	timeuse = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
	timeuse /= 1000000;
	DEBUGMSG("[NVM Proxy] msend %d bytes using time %f s \r\n", ret, timeuse);
#endif
	if (ret < 0)
	{
		DEBUGMSG("[NVM Proxy] send to error\n");
	}

}

void CNVMProxy::InitSHMService()
{
	ShmApiMsg startmsg;

	g_NVMChanConnected = FALSE;

	/* We should wait until the handshaking with CP NVM Client */
	while (TRUE)
	{
		//init handshake send CiStubStartReq_MSG and wait for server ack CiStubStartCnf_MSG
		if (g_NVMChanConnected == TRUE)
		{

			RETAILMSG("[NVM Proxy] InitSHMService done!\n");
			break;

		}
		NVMSendMsgToCP(NVMStartReqProcId, (unsigned char*)&startmsg, 0);

		OSATaskSleep(100);

	}


}

void shmPollTask(void *input)
{
	CNVMProxy *obj = (CNVMProxy *)input;

	obj->InitSHMService();
}

void CNVMProxy::shmTxRxThread()
{
	ShmApiMsg *shmArgs;
	int ret;
	int nvmDatalen;
	PUINT8 pTxBuff;
	int NumberRead = 0;
	unsigned char *pNvmData;
	unsigned char *pTxData;
	OSA_STATUS status;

	pTxBuff = (UINT8*)NVM_MEM_ALLOC(NVM_TX_BUFFER_SIZE); //VVV
	ASSERT(pTxBuff);
	memset(pTxBuff, 0, NVM_TX_BUFFER_SIZE);


	while (1)
	{
		// Use the message type (long) as the space for GPC_PDU_HEADER
		ASSERT(sizeof(long) == 4);

		ret = mrecv(nvmsrvsockfd, m_rxBuffer, NVM_RX_BUFFER_SIZE, 0);
		if (ret <= 0)
			continue;
		ASSERT(ret > 0);

		shmArgs = (ShmApiMsg *)m_rxBuffer;
		DEBUGMSG("[shmTxRxThread] procId = %d\n", shmArgs->procId);
		switch (shmArgs->procId)
		{
		case NVMStartCnfProcId:
			g_NVMChanConnected = TRUE;
			break;
		case NVMReqProcId:

			//wait for handled response

			pNvmData = m_rxBuffer + SHM_HEADER_SIZE;
			nvmDatalen = shmArgs->msglen;
			pTxData = pTxBuff + SHM_HEADER_SIZE;

			NumberRead = HandleReqBufferAndResponse(CNVMProxy::m_hInstance->m_ClientID, pNvmData, nvmDatalen, pTxData);


			// Get the length from PDU head
			//	NumberRead = *((int*)pTxData);

			// Clear the PDU head
			//*((long*)pTxData) = 0;

			//fill in tx counter
			//	*((long*)pTxData) = TxCount;
			//	DEBUGMSG("[NVM Proxy]TxCount %d send %d byptes\n",TxCount,NumberRead);
			//	TxCount++;

			//	NVMSendMsgToCP(NVMCnfProcId, pTxBuff, NumberRead+4);
			NVMSendMsgToCP(NVMCnfProcId, pTxBuff, NumberRead);

			memset(pTxBuff, 0, NVM_TX_BUFFER_SIZE);

			//OSATaskSleep(1000);
			break;
		case MsocketLinkdownProcId:
			/*We should close all the files that are open */
			DPRINTF("NVM: received MsocketLinkdownProcId!\n");
			g_NVMChanConnected = FALSE;
			break;
		case MsocketLinkupProcId:
			DPRINTF("NVM: received MsocketLinkupProcId!\n");
			if (NVMPollTaskRef)
				OSATaskDelete(NVMPollTaskRef);
			status = OSATaskCreate(&NVMPollTaskRef, NVMPollTaskStack, NVM_POLL_TASK_STACK_SIZE, 30,
					       (CHAR*)"SHM Poll Task", shmPollTask, this);
			ASSERT(status == OS_SUCCESS);
			break;
		}
	}
}

void shmTask(void *input)
{
	CNVMProxy *obj = (CNVMProxy *)input;

	obj->shmTxRxThread();
}

BOOL CNVMProxy::Init(void)
{
	BOOL bRet;
	NVM_STATUS_T res;
	OSA_STATUS status;

	//change NVM working dir according to env "NVM_ROOT_DIR"
	char *nvm_root_dir = getenv(NVM_ROOT_DIR);

	DEBUGMSG("[NVM Proxy] nvm_root_dir = %s\n", nvm_root_dir);
	if (nvm_root_dir != NULL)
		chdir(nvm_root_dir);
	ffs_pwrSafe_Init(nvm_root_dir);

#if 0
	//init BackNVM
	BackNVMPhase1Init();
	BackNVMPhase2Init();
	DPRINTF("BackNVM initialized\n");

	//init NVM client for ACAT
	res = NVMS_Register(NVM_ACAT_CLIENT_NAME, &nvm_acat_client_id);
	if (res != NVM_STATUS_SUCCESS)
	{
		DEBUGMSG("[NVM Proxy] Could not register NVM[ACAT] with NVM server,error %d\r\n", res);
	}

	//init IPC for ACM
	if (m_clientIPCACM.InitServer(NVM_IPC_ACM) == FALSE)
	{
		DEBUGMSG("[NVM Proxy] Could not init IPC server for ACM.\r\n");
	}

	if (m_clientIPCEeh.InitServer(NVM_IPC_EEH) == FALSE)
	{
		DEBUGMSG("[NVM Proxy] Could not init IPC server for ACM.\r\n");
	}
#endif
	//register to nvm server
	m_hInstance = this;
	bRet = TRUE;
	res = NVMS_Register( NVM_CLIENT_COMM_NAME, &m_ClientID);
	if (res != NVM_STATUS_SUCCESS)
	{
		DEBUGMSG("[NVM Proxy] Could not register Comm with NVM server, error %d\r\n", res);
	}
	else
	{
		DEBUGMSG("[NVM Proxy] Comm registerd with NVM server\r\n");


		nvmsrvsockfd = msocket(NVMSRV_PORT);
		DEBUGMSG("[NVM Proxy] nvmsrvsockfd=0x%x\r\n", nvmsrvsockfd);

		status = OSATaskCreate(&m_shmTask, m_shmTaskStack, SHM_TASK_STACK_SIZE, 30,
				       (CHAR*)"SHM TxRx Task", shmTask, this);
		ASSERT(status == OS_SUCCESS);

		status = OSATaskCreate(&NVMPollTaskRef, NVMPollTaskStack, NVM_POLL_TASK_STACK_SIZE, 30,
				       (CHAR*)"SHM Poll Task", shmPollTask, this);
		ASSERT(status == OS_SUCCESS);

	}
	return bRet;
}

void CNVMProxy::DeInit(void)
{
	NVMS_DeRegister( m_ClientID);
//	OSAMsgQDelete(m_hTxQueue);
	m_clientIPCACM.DeInitServer();
	m_clientIPCEeh.DeInitServer();
}
#if 0
BOOL CNVMProxy::CreateTxMsgQ()
{
	OSA_STATUS status = OSAMsgQCreate(
		&m_hTxQueue,
#if defined(OSA_QUEUE_NAMES)
		"nvmMsgQ",
#endif
		GPC_MAX_DATA_SIZE,
		30,
		OSA_PRIORITY);

	ASSERT(status == OS_SUCCESS);
	return TRUE;

}

void CNVMProxy::MyParserCallback( PUINT8 pRspBuffer, UINT32 dwRspBufferLen)
{
	static UINT8 ProxyMsg[GPC_MAX_DATA_SIZE];
	struct msgbuf* msg;

	ASSERT(pRspBuffer != NULL);
	ASSERT(dwRspBufferLen < sizeof(ProxyMsg));
	*((UINT32*)ProxyMsg) = dwRspBufferLen;
	memcpy(ProxyMsg + sizeof(UINT32), pRspBuffer, dwRspBufferLen);
	DEBUGMSG("%s send %d bytes\n", __FUNCTION__, dwRspBufferLen);
	ASSERT(OS_SUCCESS == OSAMsgQSend(m_hInstance->m_hTxQueue,
					 dwRspBufferLen + sizeof(UINT32), ProxyMsg, OSA_SUSPEND));
}
#endif

void PrintBuffer(const PUINT8 pBuffer, UINT bufferLength, LPCTSTR pHeader)
{
	UNUSEDPARAM(pBuffer)
	DEBUGMSG("[NVM Proxy] Printing %s buffer, buffer length %d\r\n", pHeader, bufferLength);
}
