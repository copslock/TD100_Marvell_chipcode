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
*     File name:      diag_header_handler.h                                       *
*     Programmer:     Barak Witkowski                                                 *
*                                                                                 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                                 *
*       Create Date:  November, 2006                                              *
*                                                                                 *
*       Description:                                                                                                                       *
*                                                                                 *
*       Notes:                                                                    *
*                                                                                 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#if !defined (_DIAG_HEADER_HANDLER_H)
#define _DIAG_HEADER_HANDLER_H

#include "global_types.h"
#include "diag_pdu.h"
#include "diag_osif.h"
/****************************************
*                                      *
*                       Constants Definition		*
*                                      *
****************************************/
#if !defined (TS_TIMER_ID)
#define TS_TIMER_ID TCR_2
#endif

// #define  SIZE_OF_TX_HEADER sizeof(TxPDUHeader)



/* definition of checksum bit in total length field */
/* MS Bit of total length field is checksumExist bit */
#define CHECKSUM_BIT    (0x8000)

/* ****************************** */
/* options third byte definitions */
/* ****************************** */
/* Bit 0 - Padding LS Bit */
#define PADDING_LSB_DATA_BIT    (0x01)

/* Bit 1 - Padding MS Bit */
#define PADDING_MSB_DATA_BIT    (0x02)

/*
	OPTIONS appear in the follwoing order (if exist):
	- SAP-related
	- TimeStamp
	- Frame Number
	- Message Counter

	(if the options field contains only SAP-related and Message-counter,
	 then after the 4 bytes of options, there will be four bytes of SAP-related data followed by
	 four bytes of Message-Counter

	 if the options field contains  TimeStamp, Frame Number and Message-counter,
	 then after the 4 bytes of options, there will be:
	 four bytes of TimeStamp followed by
	 four bytes of Frame-Number followed by
	 four bytes of Message-Counter
	)
 */
/* Bit 2 - SAP_RELATED_DATA_BIT */
#define SAP_RELATED_DATA_BIT    (0x04)

/* Bit 3 - Timestamp */
#define TIME_STAMP_BIT          (0x08)

/* Bit 4 - Frame number */
#define FRAME_NUMBER_BIT                (0x10)

/* Bit 5 - Message counter */
#define MESSAGE_COUNTER_BIT             (0x20)

/* ***************************************** */
/* options third (and last) byte definitions */
/* ***************************************** */
/* Bit 7 (left most bit) will indicate another option data (4 bytes) */
#define ADDITIONAL_OPTIONS_BIT  (0x80)


/****************************************
*                                      *
*                       Struct Definition			*
*                                      *
****************************************/

/* ====================================== *
*       Diag Message Header Struct		  *
* ====================================== */
/*
 * New Header structure. Currently, header size is static
 * ----------------------------------------------------------------------------------------------------------------------
 */
typedef struct
{
	/* SoT pattern */
	UINT16 sot;

	/* Total length of packet data without the SOT, EOT, but including the Padding. That is the total length
	 * is counted from (including) the Total Length field, till (including) the last byte of the Payload-data
	 * and padding.Total Length is not expected to exceed ~6K bytes. */
	UINT16 msgTotalLen;

	/* length of the header. The length is calculated from that field to the end of all options fields,
	 * which means: right before the start of the payload data */
	UINT16 headerTotalLen;

	/* Version of header, anytime more data is added to the header, version should be updated so SW will know
	 * what is relevant (see OPTIONS field) - each addition of option, or new fields should be indicated by
	 * new header version. */
	UINT16 headerVersion;

	/* The sending source (ICAT, Core-n). */
	UINT8 source;

	/* The destination (ICAT, Core-n). */
	UINT8 destination;

	/* Message type (SAP) - indicate how to manipulate the payload data. It can be accompanied with some
	 * relevant data in the first option (for ModueID, CommandID, ServiceID etc). */
	UINT16 msgType_Sap;

	/* Bit field which indicates presence of data (unassigned bits must be zero, when a new bit is assigned,
	 * the header version should be incremented).
	 * There is an option bit (bit 0, LSB) to expand OPTIONS.
	 * Each optional field (declared by the a bit) should be 4 bytes in size, even if there is no current need 4 bytes.
	 * In order to avoid little endians complications the field will be splitted to 4 different bytes */
	UINT8 options[4];

	/* the options data. Each data is 4 bytes. Data appears according to the bits in the options field
	 * The currently available options field are: SAP related data, counter, frame number and time stamp */
	UINT32 optionsData[NUM_OF_OPTIONS_FIELDS];
} TxPDUNewHeader;

/* current TxPDUHeader header */
typedef struct
{
	union {
		TxL2L4PDUHeader L2L4Header;
		struct {
			UINT16 length;
			UINT16 PID;
			} rawHeader;
	} extIfhdr;
	UINT16 DiagSAP;
	UINT16 counter;
	UINT16 moduleID;
	UINT16 msgID;
	UINT32 timeStamp;
}TxPDUOldHeader;

/* define header struct (old or new) according to the suitable switch */
#if defined (DIAGNEWHEADER)
	#define TxPDUHeader TxPDUNewHeader
#else
	#define TxPDUHeader TxPDUOldHeader
#endif /* DIAGNEWHEADER */
/* current signal Tx Header structure */
typedef struct
{
	union {
		TxL2L4PDUHeader L2L4Header;
		UINT32 length;
	} extIfhdr;
	UINT16 DiagSAP;
	UINT16 counter;
	UINT32 timeStamp;
}TxPDUSigHeader;

typedef struct
{
	UINT8 DiagSAP;
	UINT8 serviceID;
	UINT16 moduleID;
	UINT32 commID;
	UINT32 sourceID;
	UINT8 functionParams[1]; //contains the requested command input parameters
}RxPDU;
typedef struct
{

	struct {
		UINT16 length;
		UINT16 PID;
		} rawHeader;
	UINT8 DiagSAP;
	UINT8 serviceID;
	UINT16 moduleID;
	UINT32 commID;
	UINT32 sourceID;
	UINT32 CMMversion;
}TxPDUCMMHeader;

/* global flags indicating which optional data should be sent in the tx messages */
typedef struct
{
	/* flag indicating whether time stamp should be added to tx messages */
	BOOL TimeStampExist;
	/* flag indicating whether counter should be added to tx messages */
	BOOL MsgCounterExist;
	/* flag indicating whether checksum should be added to tx messages */
	/*^^Barak. in the future this paramter should be per interface */
	BOOL ChecksumExist;
	/* flag indicating whether frame number should be added to tx messages */
	BOOL FrameNumberExist;
} globalFlagsStrct;

/****************************************
*                                      *
*                       Globals Declaration			*
*                                      *
****************************************/
// Interface for FUll Frame number - be placed in the counter field and high byte of module ID.
// Pointer to Frame number - set by L1
// The frame number is stored in the counter field and high byte of ModuleID field
// due to current limitation of the header
DIAG_EXPORT UINT32* diagFrameNumberPtr;

// Flag is full frame number should be set (also in the module ID field) or only
// in the counter field (for back ward compatibility with ICAT (2.6 and before)
//  - the full FN is set only on ICAT command)


#endif // _DIAG_HEADER_HANDLER_H
