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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **
 *                                                                                 *
 *     File name:      diag_tx  .c                                                 *
 *     Programmer:     Shiri Dolev                                                 *
 *                                                                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                 *
 *       Create Date:  November, 2003                                              *
 *                                                                                 *
 *       Description: DIAG Tx Definitions - Handle the transmit reports functions. *
 *                                                                                 *
 *       Notes:                                                                    *
 *                                                                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "global_types.h"

#include "diag_config.h"
#include "osa.h"
#include "diag_types.h"
#include "diag.h"
#include "diag_pdu.h"
#include "diag_tx.h"
#include "diag_header_external.h"
#include "diag_buff.h"
#include "diag_init.h"
#include "diag_osif.h"
#include "diag_init.h"

#define DIAG_CORE_SELECTOR_RX_MASK              0x80
#define DIAG_CORE_SELECTOR_RX_SHIFT             7
#define DIAG_CORE_SELECTOR_TX_MASK              0x8000
#define DIAG_CORE_SELECTOR_TX_SHIFT             15
#define DIAG_CORE_APP_MASK                              0x80

#define DIAG_APPS 1
// Flag is full frame number should be set (also in the module ID field) or only
// in the counter field (for back ward compatibility with ICAT (2.6 and before)
//  - the full FN is set only on ICAT command)
/* global flags indicating which optional data should be sent in the tx
   messages */
globalFlagsStrct gFlg = {
	TRUE,
	TRUE,
	TRUE,
	FALSE
};
extern int hPort;
extern Diag_Target diag_target;
UINT32* diagFrameNumberPtr = NULL;
extern UINT32 SendCommMsg(UINT8 *msg, UINT32 len);

void    diagCalcUpdateTxL2L4MessageLength  (TxL2L4PDUHeader *header , UINT16 *messageLength , UINT32 *padBytes)
{
    UINT16  messageLengthInLongs;

    if  (*messageLength > DIAG_L2L4_MAX_SHORT_MESSAGE_SIZE)
    {
        messageLengthInLongs    =   ((*messageLength) >> 2);
        if  ((*messageLength) & 0x3)   // there's a remainder in the division
        {
            messageLengthInLongs    += 1;
            *padBytes = (4 - ((*messageLength) & 0x3));
        }

        header->ProtocolID    =   DIAG_L2L4_PROTOCOL_ID_DIAG_LONG_MESSAGE;

        header->ProtocolID    |=   DIAG_L2L4_SET_LENGTH_IN_SAP_FIELD(messageLengthInLongs);

        header->Length  =   DIAG_L2L4_SET_LENGTH_IN_LENGTH_FIELD(messageLengthInLongs);
		*messageLength = messageLengthInLongs*4;
    }
	else
	{
		header->Length = (UINT8)*messageLength;
		header->ProtocolID = DIAG_L2L4_PROTOCOL_ID_DIAG;
		*padBytes=0;
	}

}

/***********************************************************************************
 *
 * 	Name:			setL2Header
 *
 * 	Parameters:     UINT8 *pHeader - a pointer to an allocated Diag message header
 *					UINT16 *pduLen -the message length (Header + body)
 *
 * 	Returns:        Void
 *
 * 	Descriptiton:	1. The function edits the message header  to
 *					(long) L2 message such as TCP/UDP/UART/SSP.
 *					2. The function edits the STX header and adds the ETX footer.
 *                  3. The function adds zero padding as necessary.
***********************************************************************************/
void setL2Header(UINT8 *pHeader, UINT16 *pduLen)
{
#if !defined (DIAGNEWHEADER)
	UINT16      msgLen = 0;
	UINT32      padBytes = 0;
	UINT8		*zeroPadsPtr;
	TxL2L4PDUFooter	*footerPtr;
	UINT8 *tmp_ptr;
	TxPDUHeader *pTxPDUHeader;

	msgLen = *pduLen - sizeof (TxL2L4PDUHeader);
	pTxPDUHeader = (TxPDUHeader *)pHeader;

	diagCalcUpdateTxL2L4MessageLength (
	    (TxL2L4PDUHeader *) &pTxPDUHeader->extIfhdr.L2L4Header,
	    &msgLen,
	    &padBytes);

	/* handle SSP header */
	pTxPDUHeader->extIfhdr.L2L4Header.STX = DIAG_L2L4_PROTOCOL_STX;

	/* handle SSP footer */
	footerPtr   =   (TxL2L4PDUFooter *)(((UINT8 *)pHeader) + *pduLen + padBytes);
	//handle non-packed (YK) (footerPtr)- make sure to set in correct location
	tmp_ptr = (UINT8 *)&(footerPtr->ETX);
	*tmp_ptr++	=	DIAG_L2L4_PROTOCOL_ETX_BYTE;
	*tmp_ptr	=	DIAG_L2L4_PROTOCOL_ETX_BYTE;
	zeroPadsPtr =   (UINT8 *)pHeader;

	while	((padBytes--) > 0)
		zeroPadsPtr[(*pduLen)++] = 0;

	*pduLen += sizeof(TxL2L4PDUFooter);
#endif //!defined (DIAGNEWHEADER)
}

unsigned int get_packet_len(char * buff)
{
	TxPDUHeader *pTxPDUHeader;
	pTxPDUHeader = (TxPDUHeader *)buff;
	unsigned int packetLen;
	if( (int)pTxPDUHeader % 2 ==0)
			packetLen = pTxPDUHeader->extIfhdr.rawHeader.length;
	else
	{
		UINT8 * pdata = (UINT8 *)pTxPDUHeader;
		UINT8 LByte = (*pdata);
		UINT8 HByte = *(pdata+1);
		packetLen = HByte*256 + LByte;
	}
	return packetLen;
}
/***************************************************************************************************
***** If buff gets more than one packet, returns the rest size(>0). If buff gets parf of one packet, returns the missing size(<0).
*****************************************************************************************************
*/
int diag_disc_rx(struct diag_target_buffer* target_buff, char * buff, int size)
{
	unsigned int packet_len;
	char * data = buff;
	int len = size;

	if(target_buff->state == OUT_OF_PACKET)
	{
		target_buff->state = INSIDE_PACKET;
	}
	else
	{
		goto copy_to_buffer;
	}
	if((target_buff->chunk_size + len)< sizeof(UINT32))
		{
			memcpy(target_buff->chunk+target_buff->chunk_size,data,len);
			target_buff->chunk_size += len;
			return -2048;
		}
		else
		{
			memcpy(target_buff->chunk+target_buff->chunk_size,data,sizeof(UINT32));
			target_buff->chunk_size += sizeof(UINT32);
			data += sizeof(UINT32);
			len -= sizeof(UINT32);

copy_to_buffer:
			packet_len = get_packet_len(target_buff->chunk);
			if(len <= (int)(packet_len - target_buff->chunk_size))
			{
				memcpy(target_buff->chunk+target_buff->chunk_size,data,len);
				target_buff->chunk_size += len;
				len -= len;
			}
			else
			{
				memcpy(target_buff->chunk+target_buff->chunk_size,data,packet_len - target_buff->chunk_size);
				len -= (packet_len - target_buff->chunk_size);
				target_buff->chunk_size = packet_len;
				target_buff->state = OUT_OF_PACKET;
			}
			return len - (packet_len - target_buff->chunk_size);
		}

}
int diag_disc_flush(struct diag_target_buffer* target_buff)
{
	target_buff->chunk_size = 0;
	target_buff->packet_len = 0;
	target_buff->state = OUT_OF_PACKET;
	return 0;
}

UINT16 GetOnePacket(UINT8 *RawHeader, UINT8 * NewHeader, UINT16 pduLen, BOOL *Wraped)
{
	UINT16 packetLen;
	TxPDUHeader *pTxPDUHeader;
	pTxPDUHeader = (TxPDUHeader *)RawHeader;

	if(*Wraped)
	{
		if( (int)pTxPDUHeader % 2 ==0)
			packetLen = pTxPDUHeader->extIfhdr.rawHeader.length;
		else  //work round for Borqs system. Litten Edian supposed.
		{
			 UINT8 * pdata = (UINT8 *)pTxPDUHeader;
                      UINT8 LByte = (*pdata);
                      UINT8 HByte = *(pdata+1);
                      packetLen = HByte*256 + LByte;
		}
		if(packetLen <= pduLen)
		{
			memcpy(NewHeader, RawHeader, packetLen);
			*Wraped = TRUE;
			return packetLen;
		}
		else
		{
			memcpy(NewHeader, RawHeader, pduLen);
			*Wraped = FALSE;
			return pduLen;
		}

	}
	else
	{
		memcpy(NewHeader, RawHeader, pduLen);
		return pduLen;
	}
	//DIAGPRINT("****DIAG:GetOnePacket: find a packet of length:%d\n",packetLen);

}

VOID    setTxDiagTraceHeader (UINT8 *pHeader, UINT16 *pduLen, UINT16 type, UINT16 moduleID, UINT16 msgID)
{
	extern diagInternalDataS diagIntData;
	TxPDUHeader *pTxPDUHeader = (TxPDUHeader *)pHeader;

#if !defined (DIAGNEWHEADER)

	//CMI messages are using the USB header format
	if (diagIntData.m_eConnectionType == tUSBConnection)
	{
		pTxPDUHeader->extIfhdr.rawHeader.length = *pduLen;
		pTxPDUHeader->extIfhdr.rawHeader.PID = 0;
	}
	else
	//if (diagIntData.m_eConnectionType!=tUSBConnection)
	{
//#ifdef UART_NON_RAW_DIAG
		if(diag_target.raw_diag_support == 0)
			setL2Header(pHeader, pduLen);
		else{
//#else
			pTxPDUHeader->extIfhdr.rawHeader.length = *pduLen;
			pTxPDUHeader->extIfhdr.rawHeader.PID = 0;
		}
//#endif
	}


	pTxPDUHeader->DiagSAP = type;   // SAP On Tx is 16 bits, is it used for rx???
	pTxPDUHeader->DiagSAP |= DIAG_CORE_SELECTOR_TX_MASK;

	pTxPDUHeader->moduleID = moduleID;

	if (diagFrameNumberPtr)
	{
		pTxPDUHeader->counter = (UINT16)(*(diagFrameNumberPtr));    // first & second LSBytes (Byte-0, byte-1)
		if (gFlg.FrameNumberExist)
		{
			pTxPDUHeader->moduleID = (moduleID & 0xFF) | ((*diagFrameNumberPtr) >> 8
								      & 0xFF00); // 3rd byte (byte-2)
		}
	}
	pTxPDUHeader->msgID = msgID;

	pTxPDUHeader->timeStamp = GET_TIMESTAMP;
#else   /* DIAGNEWHEADER */

	/* Flags indicating which options fields should be included. These local definitions are required to avoid race conditions */
	// we assume the globals setting is correct from the time we start issue
	// the trace, till we actually send it... we look at the globals here and
	// determine what we will add to the header, but the actual work is done
	// in (setTxDiagTraceHeader) - by looking again at the globals (which might change till then (chances are small, but..)

	globalFlagsStrct lFlg = {
		gFlg.TimeStampExist,
		gFlg.MsgCounterExist,
		gFlg.ChecksumExist,
		gFlg.FrameNumberExist
	};

	/* variable holding num of padding bytes needed in the message */
	UINT8 localPaddingSize;

	/* variable holding number of option fields already included, in order to insert new values in right place of option field */
	UINT8 numOfOptionsAlreadyIncluded = 0;

	/* due to the fact that new header is dynamic and some of the optional fields might not appear
	 * this var will hold the num of bytes decreased from the full header structure */

	UINT8 numOfDecreasedBytes;


	pTxPDUHeader->sot = DIAG_NEW_HEADER_SOT;

	/* msg total length is length of pdu plus 2 bytes of total length plus optional checksum, plus optional padding bytes */
	if (lFlg.ChecksumExist)
	{
		/* if checksum exists, the data + checksum + EoT should be 4 bytes aligned */
		localPaddingSize = (*pduLen + 2 + 2) & 0x03;
		if (localPaddingSize != 0) localPaddingSize = 4 - localPaddingSize;

		/* MS bit should indicate that checksum exist */
		pTxPDUHeader->msgTotalLen = CHECKSUM_BIT;
	}
	else
	{
		/* if checksum does not exist, data + EoT by itself should be 4 bytes aligned */
		localPaddingSize = (*pduLen + 2) & 0x03;
		if (localPaddingSize != 0) localPaddingSize = 4 - localPaddingSize;

		/* if checksum does not exist, MS bit should be zero */
		pTxPDUHeader->msgTotalLen = 0x0000;
	}

	pTxPDUHeader->headerVersion = DIAG_NEW_HEADER_VERSION;

#if defined (DIAG_APPS)
	pTxPDUHeader->source = DIAG_APPS_USER;
#else   /* tx messages should be from com side */
	pTxPDUHeader->source = DIAG_COM_USER;
#endif /* DIAG_APPS */

	/* tx messages' destination is ICAT user, unless it is a C2C message */
	if ((type != SAP_C2C_REQUEST) && (type != SAP_C2C_REQUEST))
	{
		pTxPDUHeader->destination = DIAG_ICAT_USER;
	}
	else
	/* It is a C2C message, therefore destination is the other core */
	{
	#if defined (DIAG_APPS)
		pTxPDUHeader->destination = DIAG_COM_USER;
	#else
		pTxPDUHeader->destination = DIAG_APPS_USER;
	#endif  /* DIAG_APPS */
	}

	pTxPDUHeader->msgType_Sap = type;

	/************************************************************************\
	* put values to padding bytes in options field
	\************************************************************************/
	/* if padding size is 2 or 3 than MS bit of padding size should be on */
	if (localPaddingSize & 0x02)
	{
		pTxPDUHeader->options[3] |= PADDING_MSB_DATA_BIT;
	}

	/* if padding size is 1 or 3 than LS bit of padding size should be on */
	if (localPaddingSize & 0x01)
	{
		pTxPDUHeader->options[3] |= PADDING_LSB_DATA_BIT;
	}

	/* handle SAP related data */
	/* Some of the SAPs include SAP related data and some not */

	// should we handle the SAP for SAP related data?
	if (IS_SAP_HAS_RELATED_DATA(type))
	{
		switch (type)
		{
		/* These cases should include SAP related data */
		case SAP_HANDSHAKE:
		case SAP_INTERNAL_SERVICE_REQUEST:
		case SAP_INTERNAL_SERVICE_RESPONSE:
		case SAP_C2C_REQUEST:
		case SAP_C2C_RESPONSE:
		case SAP_ERROR:
			pTxPDUHeader->options[3] |= SAP_RELATED_DATA_BIT;
			pTxPDUHeader->optionsData[numOfOptionsAlreadyIncluded] = (UINT32)msgID;
			numOfOptionsAlreadyIncluded++;
			break;

		case SAP_COMMAND_SERVICE_REQUEST:
		case SAP_FIXUP_COMMAND_SERVICE_REQUEST:
		case SAP_TRACE_RESPONSE:
		case SAP_TRACE_NOT_ALIGNED_RESPONSE:
			pTxPDUHeader->options[3] |= SAP_RELATED_DATA_BIT;
			/* In this SAP the 4 bytes are divided to module Id (16 MS bits) and msgID (16 LS bits) */
			pTxPDUHeader->optionsData[numOfOptionsAlreadyIncluded] = moduleID << 16;
			pTxPDUHeader->optionsData[numOfOptionsAlreadyIncluded] |= msgID;
			numOfOptionsAlreadyIncluded++;
			break;

		/* These cases should not include SAP related data, therefore nothing should be done */
		/* - already filtered out by IS_SAP_HAS_RELATED_DATA
			     case SAP_SIGNAL_SERVICE_REQUEST:
			     case SAP_LOGGED_SIGNAL_RESPONSE:
			     case SAP_SIGNAL_OR_FILTER_REQUEST:
			     case SAP_LOGGED_SPECIALSIGNAL_RESPONSE:
				     // do nothing
				    break;
		 */

		default:
			//              DIAG_FILTER(SW_PLAT,DIAG,WRONG_SAP,WRONG_SAP)
			//              diagPrintf("Unknown SAP value %d", type);
			break;
		}
	}

	/* handle time stamp optional data */
	if (lFlg.TimeStampExist)
	{
		pTxPDUHeader->options[3] |= TIME_STAMP_BIT;
		pTxPDUHeader->optionsData[numOfOptionsAlreadyIncluded] = GET_TIMESTAMP;
		numOfOptionsAlreadyIncluded++;
	}

	/* handle frame number optional data */
	if ((lFlg.FrameNumberExist) && (diagFrameNumberPtr))
	{
		pTxPDUHeader->options[3] |= FRAME_NUMBER_BIT;
		pTxPDUHeader->optionsData[numOfOptionsAlreadyIncluded] = *(diagFrameNumberPtr);
		numOfOptionsAlreadyIncluded++;
	}

	/* we should decrease from pduLen bytes of SoT and bytes of disabled options fields
	 *  and should add padding bytes */
	pTxPDUHeader->msgTotalLen += *pduLen + localPaddingSize - SIZE_OF_SOT_PATTERN;

	/* calc number of decreased bytes from full structure of new header in order to find total header size */
	if (lFlg.MsgCounterExist)
	{
		pTxPDUHeader->options[3] |= MESSAGE_COUNTER_BIT;
		/************************************************************************\
		* message counter optional data is not handled here, but in BufferPDUs functions
		* because maybe a message will not have place in the queue
		\************************************************************************/
		numOfOptionsAlreadyIncluded++;
	}
	numOfDecreasedBytes = (NUM_OF_OPTIONS_FIELDS - numOfOptionsAlreadyIncluded) * SIZE_OF_OPTIONS_ONE_FIELD;

	/* header length is TX_PDU_HEADER_SIZE minus bytes of SoT and total length fields, and minus
	 * the disabled options fields */
	pTxPDUHeader->headerTotalLen = TX_PDU_HEADER_SIZE - SIZE_OF_SOT_PATTERN - SIZE_OF_TOTAL_LENGTH - numOfDecreasedBytes;

	/* set padding bytes to zero */
	memset(&pTxPDUHeader[*pduLen], 0, localPaddingSize);

	/* update pduLen to the end of padding bytes */
	*pduLen += localPaddingSize;

	/* update pduLen to the place of the beginning of the EoT pattern */
	/* if checksum exist, pduLen will be placed after checksum */
	if (lFlg.ChecksumExist)
	{
		*pduLen += SIZE_OF_CHECKSUM;
	}

	/* insert EoT pattern in the correct place */
	*(UINT16 *)((UINT8 *)pTxPDUHeader + *pduLen) = DIAG_NEW_HEADER_EOT;

	/* update pduLen to the end of the whole message */
	*pduLen += SIZE_OF_EOT_PATTERN;

#endif  /* DIAGNEWHEADER */
}

/************************************************************************
* Function: diagSendPDU                                                *
************************************************************************
* Description: receives report data, composes Tx PDU according to the  *
*              Tx PDU template and Calls diagBufferPDUExtIf to insert  *
*              the new PDU into the Rx Queue.                          *
*                                                                      *
* Parameters: type - RSVP or Regular trace                             *
*             moduleID - the module ID given by the Pre-Compiler       *
*             msgID - the report ID given by the Pre-Compiler          *
*             data - pointer to the report data; in case of long       *
*                    message and not RSVP, it's pointer to the         *
*                    (header + data)                                   *
*             dataLenght - the report length                           *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
************************************************************************/

void diagSendPDU (UINT8 type,
		  UINT16 moduleID,
		  UINT16 msgID,
		  void *txPduBuffPtr,
		  UINT16 dataLength,
		  UINT32 numOfDecreasedBytes)
{
	TxPDU        *pTxPdu;
	UINT16 pduLength;

#if !defined DIAGNEWHEADER
	UINT16 pduLengthNoFooter;       // when sedning message on INT-if, lets
					// not send the footer and padding

#endif
	pTxPdu = ((TxPDU *)txPduBuffPtr);

	pduLength = dataLength + TX_PDU_HEADER_SIZE;

#if defined DIAGNEWHEADER
	/* in new header we should decrease the optional fields which does not appear in the message */
	pduLength -= numOfDecreasedBytes;

#else
	DIAG_UNUSED_PARAM(numOfDecreasedBytes)
	/* put values in pduLength and pduLengthNoFooter */
	pduLengthNoFooter = pduLength;  //pduLength may be incresed by the footer size when invoking setTxDiagTraceHeader
					//so we save the original length in pduLengthNoFooter
#endif /* DIAGNEWHEADER */

	setTxDiagTraceHeader((UINT8 *)pTxPdu, &pduLength, (UINT16)type, moduleID, msgID);

	//  Trace should be sent OVER USB.
	{
		//diagBufferPDUExtIf((UINT8 *)(txPduBuffPtr), pduLength, Pool_Memory_Allocation_Trace, 1);
		if (hPort > 0 || diag_target.hPort >0)
		{
			//write(hPort,txPduBuffPtr,pduLength);
			SendCommMsg(txPduBuffPtr, pduLength);
		}
	}

	//Free this buffer after finish tx
	free(txPduBuffPtr);

	return;

} /* End of diagSendPDU   */


