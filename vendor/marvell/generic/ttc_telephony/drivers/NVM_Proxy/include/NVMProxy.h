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
#pragma once

//#include "gpc_api.h"
#include "NVMServer_api.h"
#include "NVMIPCServer.h"
#include "nvm_shared.h"
//#include "GPCTransmit.h"
#include "parser.h"
#include "osa.h"
#include <sys/socket.h>


#define SHM_TASK_STACK_SIZE     1024    //0x2000
#define NVM_RX_BUFFER_SIZE 2048
#define NVM_TX_BUFFER_SIZE 2048         //GPC_MAX_DATA_SIZE+SHM_HEADER_SIZE
class CNVMProxy
{
public:
CNVMProxy(void);
void DeInit(void);
BOOL Init(void);
~CNVMProxy(void);

//void DisconnectGPC();
//bool ReRemoteFound();

//static void gpcRxInd(UINT8 chNum,UINT8 *buf,UINT32 len,GpcStatus status,UINT32 cookie);
//static void gpcTxConf(UINT8 chNum, UINT8 *buf, UINT32 len, GpcStatus status, UINT32 cookie);
//static void gpcCEHEventNotify(GpcEvents event, GpcEventParms parms);
//static void ConnectThread(void* pParam);
void shmTxRxThread() __attribute__((noreturn));
void NVMSendMsgToCP(int procId, unsigned char *msg, int msglen);
void InitSHMService();
protected:
//BOOL InitGPCService();
//BOOL ConnetNVMChannel();
//	BOOL CreateTxMsgQ();
//GpcServiceDescriptor m_ServDesc;

//	static void MyParserCallback( PUINT8 pRspBuffer, UINT32 dwRspBufferLen);

CNVMIPCServer m_clientIPCACM;
CNVMIPCServer m_clientIPCEeh;
static CNVMProxy* m_hInstance;
//BOOL m_bConnected;
//	OSAMsgQRef m_hTxQueue;
NVM_CLIENT_ID m_ClientID;
//GpcChHandle m_hChannel;
//CGPCTransmit* m_gpcTransmitThread;
UINT8 m_rxBuffer[NVM_RX_BUFFER_SIZE];

int nvmsrvsockfd;
OSTaskRef m_shmTask;
char m_shmTaskStack[SHM_TASK_STACK_SIZE];
BOOL g_NVMChanConnected;
};


