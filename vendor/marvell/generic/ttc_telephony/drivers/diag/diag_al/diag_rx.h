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
*     File name:      diag_rx  .h                                                 *
*     Programmer:     Shiri Dolev                                                 *
*                                                                                 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                                 *
*       Create Date:  November, 2003                                              *
*                                                                                 *
*       Description: DIAG Rx Definitions - Handle the receive commands functions. *                                                                                 *
*                                                                                 *
*       Notes:                                                                    *
*                                                                                 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if !defined (_DIAG_RX_H_)
#define _DIAG_RX_H_

/****************************
*			Defines			*
****************************/

#define RETURN_SERVICE_MODULE_ID        0xFFFF

/*****************************************/
/***    INTERNAL SERVICES           ******/
/***    (special SAP (0) and serviceID  **/
/***                                    **/
/*****************************************/
#define DB_VERSION_SERVICE                      0
#define TRANSFER_DB_SERVICE                     1
#define GET_REF_CLK_RATE_SERVICE        2
#define SET_FILTER_LEVEL_SERVICE        3
#define ICAT_READY_NOTIFY                       4
#define GET_FILTER_SERVICE                      5
#define ENTER_BOOTLOADER_MODE       6
#define EXTERNAL_CONNECTION_CEASED  7
#define EXTERNAL_CONNECTION_QUERY       8
#define SEND_FULL_FRAME_NUMBER_SERVICE          9
#define ATOMIC_READY_NOTIFY_AND_DB_VER          10

// DiagComm now passes the length of Diag PDU and the pointer to it (USB 32-bit overhead is not visible)
// used in OLD header... and in extIf to verify input from USB...
#define RX_PDU_HEADER_SIZE ((2 * sizeof(UINT8)) + sizeof(UINT16) + (2 * sizeof(UINT32)))

// SAP mask is the bits relevant to the SAP value, as in this
// field, some bits are/can-be used for special signals
// ICAT MSG SAP is 8 bits, currently in use 1000 1111
#define SAP_MASK_ICAT_MSG                                       0x8F

#define DIAG_RX_COMMAND_RETRIES                         0       //No Re-tries
#define DIAG_CMI_COMMAND_RETRIES                        0       //No Re-tries

#define SIZE_OF_RX_PDU                                          13
/////////////////////////////////////////////////
/********************************
*			Typedefs			*
********************************/

typedef void (*FunctionPtr)(void *, UINT32);                    // call by pointer to function
typedef void (*RSVPFunctionPtr)(void *, UINT32, UINT32);        // call by pointer to function

typedef union {
	UINT8 packedMsgData[SIZE_OF_RX_PDU];
	UINT8 msgData[SIZE_OF_RX_PDU];
}PackedUnPackedData;


typedef struct
{
	UINT32 msgLength;
	PackedUnPackedData pdu;
}RxMsgRec;

typedef struct
{
	UINT16 totalLength;
	BOOL isChecksum;
	UINT16 headerLength;
	UINT16 headerVersion;
	UINT8 source;
	UINT8 destination;
	UINT16 messageSAP;
	UINT8 numOfPaddingBytes;
	BOOL isSapRelatedData;
	BOOL isTimeStamp;
	BOOL isFrameNumber;
	BOOL isMsgCounter;
	UINT32 sapRelatedData;
	UINT32 timeStamp;
	UINT32 frameNumber;
	UINT32 msgCounter;
	UINT8   *payloadData;
	UINT8   *msg;
}RxMsgHolder;

typedef enum
{
	waitForSoT,
	waitForTotalLength,
	waitForHeaderLength,
	waitForHeaderVersion,
	waitForSrc,
	waitForDest,
	waitForSAP,
	waitForEndOfData,
	waitForChecksum,
	waitForEoT
} PacketHandlingState;

typedef struct
{
	PacketHandlingState currentState;
	UINT16 remaminingBytes;
	BOOL isChecksum;
	UINT16 checksum;
} PacketHandlingStatus;

typedef struct
{
#if defined DIAGNEWHEADER
	PacketHandlingStatus packetHandlingStatus;
	RxMsgHolder rxMsgHolder;
#endif  /* DIAGNEWHEADER */
	UINT32 rxIF;
	UINT32 dataLen;
	UINT8  *data;
} pLongMsg;

typedef struct {
	UINT16 moduleID;
	UINT16 commandID;
	UINT8 isFiltered;
	//int isFiltered;
}reportFilterStatus;

// DiagComm function called by DiagBuff to initiate TX after idle period
typedef enum  {
	DIAG_COM_CompletePacketState,
	DIAG_COM_PacketFirstChunkState,
	DIAG_COM_PacketNextChunkState,
	DIAG_COM_PacketLastChunkState
}DIAG_COM_RX_PacketHandlingState;

typedef struct
{
	UINT32 rxIF;
	CHAR *                              buffer;                                                             //_diagUSBRxLisrMsgDataPtr;
	UINT16 data_offset;                                                                                     //bytes offset in buffer
	//TBDIY to remove the header and get a clean Diag Rx PDU
	UINT16 total_bytes_expected;                                                                            //total packet length in bytes
	UINT16 total_bytes_received;                                                                            //indicates bytes received at every moment
	UINT16 current_chunk_bytes;                                                                             //length of last received chcunk of a packet in bytes
	UINT32 nReTriesToPerform;                                                                               // number of retry to perform on failure of diagRecieveCommand
	// failure can be due to no-memory or diag-rx-queue full
	BOOL end_of_packet;
	DIAG_COM_RX_PacketHandlingState handling_state;
}DIAG_COM_RX_Packet_Info;

typedef struct
{
	BOOL bIsIfBusy;
	UINT32 NBuffTxNotify;   // keeps number of tx-notifications bufferes tillhandled in task
	DIAG_COM_RX_Packet_Info  *RxPacket;
	void*  TxFlgRef;
	void* TxTaskRef;
	void*  RxFlgRef;
	void* RxTaskRef;
} GlobalIfData;


/********************************
*			Prototypes			*
********************************/

#if defined (DIAGNEWHEADER)
void rxMessageExtractor(UINT32 comind);
BOOL rxCorrectnessChecker(UINT32 comind);
BOOL rxPacketHandler(UINT32 comind, DIAG_COM_RX_Packet_Info *p_packet, UINT16 length, UINT8 *msgData, UINT16 *bytesHandled);
#endif /* DIAGNEWHEADER */

extern UINT32 b_ExportedPtrStat; //Enable or disable the print of the exported function pointer when invoked

#endif  /* _DIAG_RX_H_ */

