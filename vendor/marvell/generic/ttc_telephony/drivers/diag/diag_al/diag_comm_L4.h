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

#if !defined (__DIAG_COMM_L4_H__)
#define __DIAG_COMM_L4_H__

#include "diag_comm.h"

typedef enum
{
    HS_IDLE  = 0,
    HS_ACK   = 2,
    HS_RESET = 4,
    HS_RESET_ACK = 8

//    HS_ALIGN_16=0xFFFF  // used to make this enum 16 bit in size

}HandshakeMessageEnum;

typedef UINT32 (*DiagHSprotocolSendCB)(UINT8 *, UINT32);

DIAG_EXPORT void    diagCommL4Init  (COMDEV_NAME rxif);
DIAG_EXPORT BOOL    diagCommL4ReceiveL2FullMessage  (UINT8   *buff , UINT32  length, COMDEV_NAME rxif);
DIAG_EXPORT BOOL    diagCommL4ReceiveL2Packet   (UINT8   *buff , UINT32  length, COMDEV_NAME rxif);
DIAG_EXPORT void    HandleHandshakeSequence (UINT8   msgType, COMDEV_NAME rxif);
DIAG_EXPORT UINT32  SendHandshakeMessage    (HandshakeMessageEnum handshake_msg, COMDEV_NAME rxif);
DIAG_EXPORT UINT32  diagL4InitHSprotocolWithCB(COMDEV_NAME rxif, DiagHSprotocolSendCB cbFunc);
#endif	//#ifndef	__DIAG_COMM_L4_H__

