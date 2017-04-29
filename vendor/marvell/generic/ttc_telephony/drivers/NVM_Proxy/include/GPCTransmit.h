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

#include "gpc_api.h"
#include "NVMServer_api.h"
#include "nvm_shared.h"
#include "osa.h"

/*
   This class is responsible for Tx Msg transmition, we Tx in a different thread, main reason is that we want to wait until we get
   confirmation for sending the message
 */
#define GPC_TX_EVENT_MASK               0x01
#define GPC_TRANS_TASK_STACK_SIZE       0x2000

class CGPCTransmit
{
public:
CGPCTransmit();
void DeInit();
BOOL Init(OSAMsgQRef hTxMsgQ_Write, GpcChHandle hNVMChannel);
~CGPCTransmit(void);
BOOL OnTxConfirmation();
void    ThreadRun(void);

void ReSetGPCHandle(GpcChHandle hNVMChannel);
protected:
//This must be implemented, declare as pure in parent class. this is the thread function

OSAMsgQRef m_TxMsgQ_Read;
OSFlagRef m_hTxConfEvent;
OSTaskRef m_hTask;
GpcChHandle m_hNVMChannel;

char m_TaskStack[GPC_TRANS_TASK_STACK_SIZE];
};

class CSHMTransmit
{
public:
CSHMTransmit();
void DeInit();
BOOL Init();
~CSHMTransmit(void);
void    ThreadRun(void);

protected:
//This must be implemented, declare as pure in parent class. this is the thread function

OSAMsgQRef m_TxMsgQ_Read;
OSFlagRef m_hTxConfEvent;
OSTaskRef m_hTask;
char m_TaskStack[GPC_TRANS_TASK_STACK_SIZE];

//UINT8 m_shmRxBuffer[GPC_MAX_DATA_SIZE];
};



