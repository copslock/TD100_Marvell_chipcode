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

/*******************************************************************************
*               MODULE HEADER FILE
********************************************************************************
* Title: diag
*
* Filename: diag_pdu.h
*
* Target, platform: Common Platform, SW platform
*
* Authors:  Yoel Lavi
*
* Description:
*
* Last Updated:
*
* Notes:
*******************************************************************************/

#if !defined (_DIAG_PDU_H_)
#define _DIAG_PDU_H_

////////////#include "ICAT_config.h"
#include "global_types.h" //for UINT16 definition

/*-- Header constants in tx/rx buffer ----------------------------------*/
#define TO_ICAT                                                                 0
#define DIAG_NEW_HEADER_SOT                                             0x6D6D
#define DIAG_NEW_HEADER_EOT                                             0x9292
#define SOT_FIRST_BYTE                                                  0x6D
#define SOT_SECOND_BYTE                                                 0x6D
#define EOT_FIRST_BYTE                                                  0x92
#define EOT_SECOND_BYTE                                                 0x92

#define DIAG_L2L4_PROTOCOL_STX                                  0x7E7E
#define DIAG_L2L4_PROTOCOL_ETX                                  0x8181
#define DIAG_L2L4_PROTOCOL_ETX_BYTE                             0x81
#define DIAG_L2L4_PROTOCOL_ID_DIAG                              0x02

/*-----------------5/29/2005 12:49------------------
 * Support long messages in L2
 * --------------------------------------------------*/
#define DIAG_L2L4_PROTOCOL_ID_DIAG_LONG_MESSAGE  0x03

#define DIAG_L2L4_MAX_SHORT_MESSAGE_SIZE         0xFF

#define DIAG_L2L4_SET_LENGTH_IN_SAP_FIELD(length)    ( ( ( length  ) >> 6 ) & 0x3C )
#define DIAG_L2L4_SET_LENGTH_IN_LENGTH_FIELD(length) ( ( length  ) & 0xFF )

/*-- Type definitions (T type) -----------------------------------------*/
typedef struct
{
	UINT16 STX;             // = DIAG_L2L4_PROTOCOL_STX
	UINT8 ProtocolID;
	UINT8 Length;
}TxL2L4PDUHeader; //TxSSPPDUHeader;

typedef struct
{
#ifdef  DIAG_SSP_USE_CHKSUM       // by default not active (what about UART?)
	UINT16 Chksum;
#endif
	UINT16 ETX;

}TxL2L4PDUFooter; //TxSSPPDUFooter;


/* the code below defines sizes of header in footer, depending on old/new header and interfaces defined (relevant for old header only */
#if defined (DIAGNEWHEADER)
/* maximum size of new header - including all 4 options data */
	#define TX_PDU_HEADER_SIZE      32
/* maximum size of new msg structure footer - padding + checksum + EoT */
	#define TX_PDU_FOOTER_SIZE  8
#else /* DIAGNEWHEADER */
	#define TX_PDU_HEADER_SIZE      16
/* in old msg structure footer does not exist for USB, but is exist for L2,L4 - we always allocate
   space, not always using it.. */
	#define TX_PDU_FOOTER_SIZE  6           // for worst case - SSP / UDP/ new-UART
#endif /* DIAGNEWHEADER */
// size of signal header (we dont have in it SAP related data (no ModuleID, msgID data))
#define SIZE_OF_TX_PDU_SIG_HEADER (TX_PDU_HEADER_SIZE - 4)        // For signals, there is NO SAP related data anyway.

typedef struct
{
	UINT8 txPduHeader[TX_PDU_HEADER_SIZE];
	UINT8 data[1];
}TxPDU;

/*-- Public Functions Prototypes ---------------------------------------*/

#endif // _DIAG_PDU_H_


