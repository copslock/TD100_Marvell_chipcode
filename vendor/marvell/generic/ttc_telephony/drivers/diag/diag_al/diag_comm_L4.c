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

/*********************************************************************
*                      M O D U L E     B O D Y                       *
**********************************************************************
* Title: Diag                                                        *
*                                                                    *
* Filename: diag_comm_L4.c                                           *
*                                                                    *
* Target, platform: Common Platform, SW platform                     *
*                                                                    *
* Authors:Boaz Sommer                                                *
*                                                                    *
* Description: Provide layer 4 of communication                      *
*                                                                    *
* Notes:                                                             *
*                                                                    */
/************* General include definitions ****************************/
#include "linux_types.h"
#include "osa.h"
#include "global_types.h"

//#include "ICAT_config.h"
#include "diag_config.h"

#include "diag_osif.h"
#include INCLUDE_ASSERT_OR_DUMMY

#include "diag.h"
//#include "diag_ram.h"
#include "diag_buff.h"
#include "diag_comm.h"
#include "diag_comm_L2.h"
#include "diag_comm_L4.h"
#include "diag_init.h"
#include "diag_rx.h"
#include "diag_tx.h"
//#include "diag_mem.h"

#define DiagAlignMalloc malloc
#define DiagAlignFree free

#define UNUSEDPARAM(param) (void)param;
typedef enum
{
    FF_FULL_MESSAGE,
    FF_FRAG_MESSAGE,
    FF_FRAG_END = 3
}FragFlagEnum;

typedef enum
{
    MT_NORMAL = 1,
    MT_ACK    = 2,
    MT_RESET  = 4,
    MT_RESET_ACK = 8,
    MT_FINAL     = 0x10
}MessageTypeEnum;

// NOTE : This assumes ONLY ONE interface to be activa at a given time!!
//    when more then one interface of L2/L4-style is active, data per interface
//    should be maintained!!
static  HandshakeMessageEnum    LastHandshakeRxMessage = HS_IDLE;
static  HandshakeMessageEnum    LastHandshakeTxMessage = HS_IDLE;
DiagHSprotocolSendCB cbSendHS_func;

typedef struct
{
    UINT8   SrcAppID;
    UINT8   DstAppID;
    UINT8   FragFlag;
    UINT8   FragIndex;
    UINT8   MsgType;
    UINT8   AckNum;
    UINT16  Length;
}L4HeaderStruct;

/*-----------------1/26/2006 15:09------------------
 * Externs
 * --------------------------------------------------*/
//extern  BOOL diagReceiveCommand(DIAG_COM_RX_Packet_Info *p_packet);
extern GlobalIfData dataExtIf;
extern void dispatchServiceForSAP(UINT8 *Ori_data);
 UINT32 TestComposeUDPPacket(DIAG_COM_RX_Packet_Info  *RxPacket);

static  void                    SetLastRxHandshakeMessage   (HandshakeMessageEnum new_hs_msg);
static  HandshakeMessageEnum    GetLastRxHandshakeMessage   (void);

static  void                    SetLastTxHandshakeMessage   (HandshakeMessageEnum new_hs_msg);
static  HandshakeMessageEnum    GetLastTxHandshakeMessage   (void);


/*-----------------2/1/2006 10:04-------------------
 * The two function below handle keeping info on
 * the last handshake message type.
 * --------------------------------------------------*/
static  void    SetLastRxHandshakeMessage   (HandshakeMessageEnum new_hs_msg)
{
    LastHandshakeRxMessage  =   new_hs_msg;
	return;
}

static  HandshakeMessageEnum    GetLastRxHandshakeMessage   (void)
{

    return  LastHandshakeRxMessage;
}

static  void   SetLastTxHandshakeMessage   (HandshakeMessageEnum new_hs_msg)
{
    LastHandshakeTxMessage  =   new_hs_msg;
	return;
}

static  HandshakeMessageEnum    GetLastTxHandshakeMessage   (void)
{

    return  LastHandshakeTxMessage;
}

void    diagCommL4Init  (COMDEV_NAME rxif)
{
    SendHandshakeMessage   (HS_RESET, rxif);
}

UINT32 diagL4InitHSprotocolWithCB(COMDEV_NAME rxif, DiagHSprotocolSendCB cbFunc)
{
	cbSendHS_func = cbFunc;
	return SendHandshakeMessage   (HS_RESET, rxif);
}

UINT32 TestComposeUDPPacket(DIAG_COM_RX_Packet_Info  *RxPacket)
{
	static void * buff;
	UINT16 buffsize;

	if(RxPacket->handling_state == DIAG_COM_CompletePacketState)
	{
		memcpy(dataExtIf.RxPacket, RxPacket, sizeof(DIAG_COM_RX_Packet_Info));
		dispatchServiceForSAP((UINT8 *)dataExtIf.RxPacket->buffer);
		return dataExtIf.RxPacket->current_chunk_bytes;
	}
	else if(RxPacket->handling_state == DIAG_COM_PacketFirstChunkState)
	{
		memcpy(dataExtIf.RxPacket, RxPacket, sizeof(DIAG_COM_RX_Packet_Info));

		buffsize = RxPacket->total_bytes_expected+sizeof(L4HeaderStruct);
		buff = DiagAlignMalloc(buffsize);
		dataExtIf.RxPacket->buffer = buff;
		memcpy(buff,RxPacket->buffer,RxPacket->current_chunk_bytes);

		return 0;
	}
	else
	{
		UINT16 payloadlen = RxPacket->current_chunk_bytes-sizeof(L4HeaderStruct);
		void * payload = RxPacket->buffer+sizeof(L4HeaderStruct);
		memcpy(buff+dataExtIf.RxPacket->current_chunk_bytes, payload, payloadlen);
		dataExtIf.RxPacket->current_chunk_bytes += payloadlen;
		dataExtIf.RxPacket->total_bytes_received += payloadlen;

		if(RxPacket->handling_state == DIAG_COM_PacketLastChunkState)
		{
			dispatchServiceForSAP((UINT8 *)dataExtIf.RxPacket->buffer);
			DiagAlignFree(buff);
			return dataExtIf.RxPacket->current_chunk_bytes;
		}
		return 0;
	}
}

BOOL    diagCommL4ReceiveL2FullMessage  (UINT8   *buff , UINT32  length, COMDEV_NAME rxif)
{
    DIAG_COM_RX_Packet_Info l2_packet_info;

    l2_packet_info.buffer   =   (CHAR *)buff;
	l2_packet_info.rxIF		= rxif;
    l2_packet_info.data_offset  =   0;
    l2_packet_info.total_bytes_expected =   length;
    l2_packet_info.total_bytes_received =   length;
    l2_packet_info.current_chunk_bytes  =   length;
    l2_packet_info.end_of_packet        =   0;
    l2_packet_info.handling_state       =   DIAG_COM_CompletePacketState;
	l2_packet_info.nReTriesToPerform = diagIntData.nReTriesToPerform;// diagRecieveCommand will try 2000 to alloc/put-in-queue this message

    //return diagReceiveCommand  (&l2_packet_info);
   //dataExtIf.RxPacket = &l2_packet_info;
	//diagCommSetChunk(dataExtIf.RxPacket, transferBuffer, (UINT32) dwBytesTransferred, 0/*TBDIY DIAGNEWHEADER*/);
	//dispatchServiceForSAP((UINT8 *)dataExtIf.RxPacket->buffer);
	TestComposeUDPPacket(&l2_packet_info);
	return TRUE;

}

BOOL    diagCommL4ReceiveL2Packet   (UINT8   *buff , UINT32  length, COMDEV_NAME rxif)
{
    DIAG_COM_RX_Packet_Info l4_packet_info; 
    L4HeaderStruct  *l4Ptr  =   (L4HeaderStruct *)buff;
	UINT8 *temp_ptr;
	CHAR okok[20] = "Enter L4..";

	DIAG_FILTER(VALI_IF,ATCMD_IF,ATOUT_CHAR,0)
	diagPrintf("%s", okok);

    if  (l4Ptr->MsgType != MT_NORMAL)
    {
        HandleHandshakeSequence (l4Ptr->MsgType, rxif);
        return TRUE;
    }

    l4_packet_info.buffer   =   (CHAR *)buff;
	l4_packet_info.rxIF		= rxif;
    l4_packet_info.data_offset  =   sizeof  (L4HeaderStruct);
	//handle non-packed (YK) (l4Ptr)- make sure to set in correct location
	temp_ptr = (UINT8 *)&(l4Ptr->Length);		// value arrive in Little Endean format
    l4_packet_info.total_bytes_expected =   *temp_ptr;	 //put the MSByte
	temp_ptr++;
	l4_packet_info.total_bytes_expected |= *temp_ptr<<8; // put the LMByte
    l4_packet_info.total_bytes_received =   length;
    l4_packet_info.current_chunk_bytes  =   length;
    l4_packet_info.end_of_packet        =   0;
	l4_packet_info.nReTriesToPerform 	= diagIntData.nReTriesToPerform;// diagRecieveCommand will release its buffer if failed to put-in-q

    switch  (l4Ptr->FragFlag)
    {
        case    FF_FULL_MESSAGE:
            l4_packet_info.handling_state       =   DIAG_COM_CompletePacketState;
            break;

        case    FF_FRAG_MESSAGE:
            if  (l4Ptr->FragIndex   ==  1)
                l4_packet_info.handling_state       =   DIAG_COM_PacketFirstChunkState;
            else
                l4_packet_info.handling_state       =   DIAG_COM_PacketNextChunkState;
            break;

        case    FF_FRAG_END:
            l4_packet_info.handling_state       =   DIAG_COM_PacketLastChunkState;
            break;
    }

    //return diagReceiveCommand  (&l4_packet_info);
    //dataExtIf.RxPacket = &l4_packet_info;  //Dont't do such thing, RxPacket has malloced its own space.
	//diagCommSetChunk(dataExtIf.RxPacket, transferBuffer, (UINT32) dwBytesTransferred, 0/*TBDIY DIAGNEWHEADER*/);
	//dispatchServiceForSAP((UINT8 *)dataExtIf.RxPacket->buffer);
	TestComposeUDPPacket(&l4_packet_info);
	return TRUE;


}

void    HandleHandshakeSequence (UINT8   msgType, COMDEV_NAME rxif)
{

    switch  (msgType)
    {

        case    HS_ACK:
            break;

        case    HS_RESET:
            /*-----------------2/1/2006 17:18-------------------
             * If Diag sent Reset to ICAT, it expects ResetAck
             * If Reset is received, probably some communication
             * error happend, so start over with handshake.
             * --------------------------------------------------*/
            if  ((GetLastTxHandshakeMessage() ==  HS_RESET) && (GetLastRxHandshakeMessage() != HS_RESET_ACK))
            {
                SetLastRxHandshakeMessage(HS_IDLE);
                SendHandshakeMessage    (HS_RESET, rxif);
                break;
            }
            SendHandshakeMessage    (HS_RESET_ACK, rxif);
            if  (GetLastRxHandshakeMessage() != HS_RESET_ACK)
            {
                SendHandshakeMessage    (HS_RESET, rxif);
                SetLastRxHandshakeMessage(HS_RESET);
            }
            else
                SetLastRxHandshakeMessage(HS_IDLE);

            break;

        case    HS_RESET_ACK:
            if  (GetLastRxHandshakeMessage() ==  HS_RESET)
                SetLastRxHandshakeMessage(HS_IDLE);
            else
                SetLastRxHandshakeMessage(HS_RESET_ACK);
            break;

        default:
            break;
    }

}

UINT32    SendHandshakeMessage    (HandshakeMessageEnum handshake_msg, COMDEV_NAME rxif)
{
	UNUSEDPARAM(rxif)

	DiagReportItem_S  item;
    UINT8   *handshake_message;
    L4HeaderStruct  *l4Ptr;
    L2HeaderStruct  *l2HeaderPtr;
    L2FooterStruct  *l2FooterPtr;
	UINT8 *temp_ptr;

    UINT32          length  =   sizeof (L4HeaderStruct) + sizeof (L2HeaderStruct) + sizeof (L2FooterStruct);

    handshake_message   =   DiagAlignMalloc (length);

    DIAG_ASSERT(handshake_message != NULL);

    l2HeaderPtr   =   (L2HeaderStruct *)handshake_message;
    l4Ptr   =   (L4HeaderStruct *)(handshake_message + sizeof  (L2HeaderStruct));
    l2FooterPtr =  (L2FooterStruct *) ((UINT8 *)l4Ptr + sizeof(L4HeaderStruct));

	//handle non-packed (YK) (l2HeaderPtr)- make sure to set in correct location
	temp_ptr = (UINT8 *)&(l2HeaderPtr->Stx);
	*temp_ptr++ = L2_STX_BYTE;
	*temp_ptr = L2_STX_BYTE; 
    l2HeaderPtr->ProtocolID =   PID_DIAG | PID_L4_BIT;
    l2HeaderPtr->Length     =   sizeof  (L4HeaderStruct);

    l4Ptr->SrcAppID         =   1;
    l4Ptr->DstAppID         =   2;
    l4Ptr->FragFlag         =   0;
    l4Ptr->FragIndex        =   0;
    l4Ptr->MsgType          =   handshake_msg;
    l4Ptr->AckNum           =   0;
	//handle non-packed (YK) (l4Ptr)- make sure to set in correct location
    temp_ptr = (UINT8 *)&(l4Ptr->Length);
	*temp_ptr++ = 0;
	*temp_ptr = 0;

	//handle non-packed (YK)(l2FooterPtr)- make sure to set in correct location
	temp_ptr = (UINT8 *)&(l2FooterPtr->Etx);
	*temp_ptr++ = L2_ETX_BYTE;
	*temp_ptr = L2_ETX_BYTE; 


    SetLastTxHandshakeMessage   (handshake_msg);
	if (cbSendHS_func)
	{
		UINT32 retcode;
		retcode = (*cbSendHS_func)(handshake_message, length);
		// now that message was sent, let free it
		DiagAlignFree(handshake_message);
		handshake_message=0;
		return retcode;
	}
	else
	{
		item.virtualReportPtr  =   handshake_message;
		item.reportLength      =   length;
		item.allocType         =   Pool_Memory_Allocation_Trace;//UINT8
		//CMI item.clientID             = ??? rxif (DIAG_COMMDEV_CMI+i) for general SendMsg function

		// enforce transimt does not work well on all interfaces. In some, nothing happens and the 
		// message is not sent, nor freed.
		//diagCommEnforceTransmit    ((void *)&item, rxif);
		return 1;

	}
}
