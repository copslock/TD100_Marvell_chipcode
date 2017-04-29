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

#if !defined (__DIAG_COMM_L2_H__)
#define __DIAG_COMM_L2_H__

#include "diag_comm.h"

#define     L2_STX      0x7E7E
#define     L2_STX_BYTE 0x7E
#define     L2_ETX      0x8181
#define     L2_ETX_BYTE 0x81
#define     L2_STX_LOW  0x7E

#define     L2_ETX_SIZEOF   2

#define PID_LONG        0x01
#define PID_DIAG        0x02
#define PID_L4_BIT      0x04

/*-----------------5/29/2005 10:24------------------
 * Protocol ID structure:
 *
 * --------------------------------------------------
 * Address  |Checksum  |      SAP        | Protocol |
 * Exists   |Exists    |                 | ID       |
 * --------------------------------------------------
 *    1     |   1      |       4         |     2    |
 * --------------------------------------------------*/
#define DIAG_SSP_MASK_SAP_FIELD 0x3C
#define DIAG_SSP_MASK_PID_FIELD 0x03

#define DIAG_SSP_LONG_MESSAGE_ID    0x03
#define DIAG_SSP_GET_LONG_MESSAGE_LENGTH(pid,length)   ( ( ( (pid) & DIAG_SSP_MASK_SAP_FIELD ) << 6 ) | (length) )


#define DIAG_SSP_CHECK_IF_LONG_MESSAGE(pid)   (((pid) & DIAG_SSP_MASK_PID_FIELD) == DIAG_SSP_LONG_MESSAGE_ID)


typedef struct
{
    UINT16  Stx;
    UINT8   ProtocolID;
    UINT8   Length;
}L2HeaderStruct;

typedef struct
{
#if defined (DIAG_SSP_USE_CHKSUM)
    UINT16  ChksumOrEtx;
#endif
    UINT16  Etx;
}L2FooterStruct;

typedef struct
{
    UINT32  Index;
    UINT32  Length;
    UINT8   *Buffer;
}L2DataStruct;

DIAG_EXPORT void    diagCommL2Init  (void);
DIAG_EXPORT void    diagCommL2ReceiveL1Packet   (UINT8   *buff , UINT32  length , BOOL isRecursive, COMDEV_NAME rxif);


#endif  // #ifndef __DIAG_COMM_L2_H__
