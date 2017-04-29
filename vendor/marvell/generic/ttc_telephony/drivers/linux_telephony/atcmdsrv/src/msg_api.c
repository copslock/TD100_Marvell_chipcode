/******************************************************************************
*(C) Copyright 2008~2009 Marvell International Ltd.
* All Rights Reserved
******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: msg_api.c
 *
 *  Description: API interface implementation for SMS service
 *
 *  History:
 *   Sept 12, 2008 - Qiang Xu Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/
#include "ci_api_types.h"
#include "ci_api.h"
#include "telatci.h"
#include "teldef.h"
#include "telatparamdef.h"
#include "tel3gdef.h"
#include "telutl.h"
#include "ci_msg.h"
#include "utlMalloc.h"
#include "telmsg.h"
#include "sim_api.h"
#include <pthread.h>

/************************************************************************************
 *
 * SMS related const and global variables
 *
 *************************************************************************************/

extern AtciSmsFormatMode gSmsFormatMode;
extern CiServiceHandle gAtciSvgHandle[CI_SG_NUMIDS + 1];

/* Array of SMS storage names */
const char* gMsgStorageStr[] =
{
	"BM",   /* broadcast message storage (in volatile memory) */
	"ME",   /* ME message storage */
	"MT",   /* any of the storages associated with ME */
	"SM",   /* SIM message storage */
	"SR"    /* status report storage */
};

/* Maintain value of AT+CNMI=, the temp value is copied to curr value upon receiving of CI Cnf msg */
struct {
	CiMsgMsgIndSetting mode;
	CiMsgMsgIndSetting mt;
	CiMsgMsgIndSetting bm;
	CiMsgMsgIndSetting ds;
	CiMsgMsgIndSetting bfr;
} gCurrNewMsgIndSetting, gTempNewMsgIndSetting;
struct msg_request_list{
		struct msg_request_list * next;
		void *pParam;
		int indHandle;
};
struct msg_request_buff{
		struct msg_request_list msg_list_head;
		pthread_mutex_t buf_mutx;
		unsigned int buf_count;
};
struct msg_ack_flag{
	unsigned int ack;
	pthread_mutex_t ack_mutx;  //maybe  take no actual effect now, but reserved.
};
static struct msg_request_buff msg_requests = { {NULL, NULL, 0}, PTHREAD_MUTEX_INITIALIZER, 0}; //the instance of the queue buffer for msg ind reuqests
static struct msg_ack_flag msg_ack = { 1, PTHREAD_MUTEX_INITIALIZER};	//the instance of ack for RIL resoponse

/* Maintain value of AT+CSMP, +CMGW,+CMGS, and used in libTXT2PDU when sending/storing msg after Ctrl+Z
 *  The record value includes text mode para, MT addr, type of addr, etc
 */
static AtciMsgInfo gAtSmsMessage =
{
	ATCI_MSG_SUBMIT_FO_DEFAULT,
	ATCI_MSG_VP_DEFAULT,
	ATCI_MSG_PID_DEFAULT,
	ATCI_MSG_DCS_DEFAULT,
	ATCI_MSG_REF_DEFAULT,
	ATCI_MSG_UDH_NOT_PRESENT,
	0,
	0,
	0,
	CI_MSG_MSG_STAT_STO_UNSENT,
	{ 0 }
};

/* Record current operation message index, used in listing or deleting all msg one CI request by one */
static UINT32 gCurrMsgIndex = 0;

/* Record the number of existed message and total available number in mem1 (for read, list and delete) */
static UINT32 gMem1MsgNum = 0;
static UINT32 gMem1TotalNum = 0;
static CiBoolean gMemCapExceeded = FALSE;

/* Record option of More Message to Send, set by +CMMS and used in CI request to send msg */
static UINT32 gModeMoreMsgSent = 0;

/* Record the new ind msg to be acked by +CNMA */
static UINT32 gNewMsgAckCounter = 0;

/*Michal Bukai - SMS Full Feeature support*/
UINT32 g_iNewMsgAck = 0;


/* enum to record current operation, used when processing same CI Cnf msg */
typedef enum
{
	AT_OPERATION_LIST_MSG = 1,
	AT_OPERATION_READ_MSG,
	AT_OPERATION_SEND_MSG,
	AT_OPERATION_STORE_MSG,
	AT_OPERATION_STORE_MSG_WITH_INDEX,
	AT_OPERATION_SEND_STORED_MSG,
	AT_OPERATION_DEL_MSG,
	AT_OPERATION_DEL_ALL_MSG,
	AT_OPERATION_GET_STORAGE_INFO,
	AT_OPERATION_QUERY_DEL_MSG,
	AT_MSG_OPERATION_NULL
} AT_MSG_OPERATION;

/* Local variable to record current operation */
static AT_MSG_OPERATION gCurrOperation = AT_MSG_OPERATION_NULL;

/* Global Mutex to protect gCurrOperation
 * NOTE: it is a recursive mutex
 */
static pthread_mutex_t gMsgOperLock = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;

#define LOCK_MSGCTX    do{ \
	pthread_mutex_lock(&gMsgOperLock); \
	DBGMSG("[%s] Line(%d): gMsgOperLock is locked\n", __FUNCTION__, __LINE__); \
}while(0)

#define UNLOCK_MSGCTX  do{ \
	pthread_mutex_unlock(&gMsgOperLock); \
	DBGMSG("[%s] Line(%d): gMsgOperLock is unlocked\n", __FUNCTION__, __LINE__); \
}while(0)

/* Local variable to save ind handle of MT msg */
//static int gMtMsgIndHandle;

/* enum used in encoding/decoding SMS text */
typedef enum
{
	ATCI_MSG_CODING_DEFAULT           = 0x00,
	ATCI_MSG_CODING_8BIT              = 0x01,
	ATCI_MSG_CODING_UCS2              = 0x02,
	ATCI_MSG_CODING_RESERVED          = 0x03,

	ATCI_MSG_CODING_NUM_OF
} AtciMsgCodingType;

/* Struct used in readSmsDeliverPdu() */
typedef struct
{
	UINT8 tsYear;
	UINT8 tsMonth;
	UINT8 tsDay;
	UINT8 tsHour;
	UINT8 tsMinute;
	UINT8 tsSecond;
	UINT8 tsTimezone;
	UINT8 tsZoneSign;
}TIMESTAMP;

/* Struct used in readSmsDeliverPdu() */
typedef struct {
	UINT32 type;                            // @field DCS type
	UINT32 msgClass;                        // @field message class (Only for RIL_DCSTYPE_GENERAL and RIL_DCSTYPE_MSGCLASS)
	UINT32 alphabet;                        // @field DCS alphabet
	UINT32 indication;                      // @field indication (Only for RIL_DCSTYPE_MSGWAIT)
} DCS;

/* Enum used in readSmsDeliverPdu() */
typedef enum
{
	MSGFLAG_MORETOSEND = 0x01,
	MSGFLAG_STATUSREPORTRETURNED = 0x02,
	MSGFLAG_HEADER = 0x04,
	MSGFLAG_REPLYPATH = 0x08
}MSGFLAG;

/* Struct used in readSmsDeliverPdu() */
typedef struct
{
	MSGFLAG bMsgFlag;
	UINT8 srcAddr[ CI_MAX_ADDRESS_LENGTH + 1];
	UINT32 protocolId;
	TIMESTAMP timeStamp;
	DCS bDCS;
	UINT32 bMsgLength;                      // length of body
	UINT8 msgBuf[512];                      // msg body buffer
}SHORTMESSAGE;

#define MSGPROTOCOL_UNKNOWN                 (0x00)
#define MSGPROTOCOL_SMETOSME                (0x01)

/* Used when read protocol ID from SMS-DELIVER PDU in text mode */
static const UINT32 protocolIDValues[] =
{
	0x00,   // MSGPROTOCOL_UNKNOWN
	0x00,   // MSGPROTOCOL_SMETOSME
	0x20,   // MSGPROTOCOL_IMPLICIT
	0x21,   // MSGPROTOCOL_TELEX
	0x22,   // MSGPROTOCOL_TELEFAX_GROUP3
	0x23,   // MSGPROTOCOL_TELEFAX_GROUP4
	0x24,   // MSGPROTOCOL_VOICEPHONE
	0x25,   // MSGPROTOCOL_ERMES
	0x26,   // MSGPROTOCOL_PAGING
	0x27,   // MSGPROTOCOL_VIDEOTEX
	0x28,   // MSGPROTOCOL_TELETEX
	0x29,   // MSGPROTOCOL_TELETEX_PSPDN
	0x2a,   // MSGPROTOCOL_TELETEX_CSPDN
	0x2b,   // MSGPROTOCOL_TELETEX_PSTN
	0x2c,   // MSGPROTOCOL_TELETEX_ISDN
	0x2d,   // MSGPROTOCOL_UCI
	0x30,   // MSGPROTOCOL_MSGHANDLING
	0x31,   // MSGPROTOCOL_X400
	0x32,   // MSGPROTOCOL_EMAIL
	0x38,   // MSGPROTOCOL_SCSPECIFIC1
	0x39,   // MSGPROTOCOL_SCSPECIFIC2
	0x3a,   // MSGPROTOCOL_SCSPECIFIC3
	0x3b,   // MSGPROTOCOL_SCSPECIFIC4
	0x3c,   // MSGPROTOCOL_SCSPECIFIC5
	0x3d,   // MSGPROTOCOL_SCSPECIFIC6
	0x3e,   // MSGPROTOCOL_SCSPECIFIC7
	0x3f,   // MSGPROTOCOL_GSMSTATION
	0x40,   // MSGPROTOCOL_SM_TYPE0
	0x41,   // MSGPROTOCOL_RSM_TYPE1
	0x42,   // MSGPROTOCOL_RSM_TYPE2
	0x43,   // MSGPROTOCOL_RSM_TYPE3
	0x44,   // MSGPROTOCOL_RSM_TYPE4
	0x45,   // MSGPROTOCOL_RSM_TYPE5
	0x46,   // MSGPROTOCOL_RSM_TYPE6
	0x47,   // MSGPROTOCOL_RSM_TYPE7
	0x5f,   // MSGPROTOCOL_RETURNCALL
	0x7d,   // MSGPROTOCOL_ME_DOWNLOAD
	0x7e,   // MSGPROTOCOL_DEPERSONALIZATION
	0x7f,   // MSGPROTOCOL_SIM_DOWNLOAD
};
#define NUM_PROTOCOLIDS (sizeof(protocolIDValues) / sizeof(UINT32))

#define READ_SMS_OPTION_NUM 5

/*          Primitive ID                     SMS status to list
  * PRI_MSG_PRIM_LIST_MESSAGE_REQ        CI_MSG_MSG_STAT_REC_UNREAD
  * PRI_MSG_PRIM_LIST_MESSAGE_REQ + 1    CI_MSG_MSG_STAT_READ
  * PRI_MSG_PRIM_LIST_MESSAGE_REQ + 2    CI_MSG_MSG_STAT_STO_UNSENT
  * PRI_MSG_PRIM_LIST_MESSAGE_REQ + 3    CI_MSG_MSG_STAT_STO_SENT
  * PRI_MSG_PRIM_LIST_MESSAGE_REQ + 4    CI_MSG_MSG_STAT_ALL
  */
#define PRI_MSG_PRIM_LIST_MESSAGE_REQ (CI_MSG_PRIM_LAST_COMMON_PRIM + 1)
#define PRI_MSG_PRIM_QUERY_DEL_MESSAGE_REQ (CI_MSG_PRIM_LAST_COMMON_PRIM + READ_SMS_OPTION_NUM + 1)
#define PRI_MSG_PRIM_DEL_ALL_MESSAGE_REQ (CI_MSG_PRIM_LAST_COMMON_PRIM + READ_SMS_OPTION_NUM + 2)

static char gMsgValidIndex[TEL_AT_SMS_INDEX_VAL_MAX * 4 + 20];

#ifdef AT_CUSTOMER_HTC
/* enum of RP cause definition mobile originating SM transfer*/
typedef enum
{
	CAUSE_UNASSIGNED_NO               = 0x01,
	CAUSE_OPER_DETERM_BARRING        = 0x08,
	CAUSE_CALL_BARRED                 = 0x0a,
	CAUSE_SHORT_MSG_TRANSFER_REJECTED = 0x15,
	CAUSE_DESTINATION_OUT_OF_SERVICE = 0x1b,
	CAUSE_UNIDENTIFIED_SUBSCRIBER    = 0x1c,
	CAUSE_FACILITY_REJECTED           = 0x1d,
	CAUSE_UNKNOWN_SUBSCRIBER          = 0x1e,
	CAUSE_NETWORK_OUT_OF_ORDER        = 0x26,
	CAUSE_TEMPOARY_FAILURE            = 0x29,
	CAUSE_CONGESTION                   =0x2a,
	CAUSE_RESOURCE_UNAVAILABLE_UNSPECIFIED = 0x2f,
	CAUSE_REQ_FAC_NOT_SUBSCR          = 0x32,
	CAUSE_REQ_FACIL_NOT_IMPL          = 0x45,
	CAUSE_INVALID_SHORT_MSG_TRANSFER_REF_VALUE = 0x51,
	CAUSE_INVALID_MSG_UNSPECIFIED     = 0x5f,
	CAUSE_INVALID_MANDATORY_INFORMATION = 0x60,
	CAUSE_MSG_TYPE_NON_EXISTENT_OR_NOT_IMPL = 0x61,
	CAUSE_MSG_NOT_COMPATIBLE_WITH_SHORT_MSG_PROTOCAL_STATE = 0x62,
	CAUSE_INFORMATION_ELEMENT_NON_EXISTENT_OR_NOT_IMPL = 0x63,
	CAUSE_PROTOCOL_ERROR_UNSPECIFIED    = 0x6f,
	CAUSE_INTERWORKING_UNSPECIFIED      = 0x7f,
} SmsSentRpCause;
#endif
static void __processNewMsgInd(const void *pParam, const int indHandle);
/************************************************************************************
 *
 * Internal library function used to encode SMS data when sending CI request
 *
 *************************************************************************************/
void resetMsgParas(void)
{
	gSmsFormatMode = ATCI_SMS_FORMAT_PDU_MODE;
	gCurrMsgIndex = 0;
	gMem1MsgNum = 0;
	gMem1TotalNum = 0;
	gMemCapExceeded = FALSE;
	gModeMoreMsgSent = 0;
	gNewMsgAckCounter = 0;
	g_iNewMsgAck = 0;
}

void resetMsgOperFlag(void)
{
	LOCK_MSGCTX;
	gCurrOperation = AT_MSG_OPERATION_NULL;
	UNLOCK_MSGCTX;
}

static CHAR libNumToHex(CHAR num)
{
	if ( num >= 0x0 && num <= 0x9 )
	{
		num += '0';
	}
	else if ( num >= 0xA && num <= 0xF )
	{
		num -= 0xA;
		num += 'A';
	}
	else
	{
		DPRINTF("## numToHex ## Not a hex number\n");
	}

	return num;
}

/* --------------------------------------------------------------------------
 * Function     : libPutPackedBcd (copy of sacShPutPackedBcd)
 * Description  : Stores a BCD Digit into a packed BCD array
 * Parameters   : pBcd   - Pointer to packed BCD octet array
 *                idxBcd - BCD index to store
 *                bcdVal - BCD digit value
 *                reversed - whether the nibbles in the number are reversed
 * Returns      : Nothing
 * Notes        : The output buffer contains two BCD digits per octet
 * -------------------------------------------------------------------------- */
static void libPutPackedBcd( UINT8 *pBcd, UINT8 idxBcd, UINT8 bcdVal, BOOL reversed )
{
	BOOL ls4bits = FALSE;
	UINT8 *pOut = &pBcd[ idxBcd / 2 ];

	if ((( (idxBcd & 0x01) == 0 ) && ( reversed == TRUE ) ) ||
	    (( (idxBcd & 0x01) != 0 ) && ( reversed == FALSE ) ))
		ls4bits = TRUE;

	if ( ls4bits == TRUE )
	{
		/* Even digit -> LS 4 bits */
		*pOut = ( *pOut & ATCI_BCD_HIGH_MASK ) | ( bcdVal & ATCI_BCD_LOW_MASK );
	}
	else
	{
		/* Odd digit -> MS 4 bits */
		*pOut = ( *pOut & ATCI_BCD_LOW_MASK ) | ( (bcdVal << 4) & ATCI_BCD_HIGH_MASK );
	}
}


/* --------------------------------------------------------------------------
 * Function     : atciPutPackedSMSC
 * Description  : Stores a SMSC address into a packed array
 * Parameters   : optSca - SMSC address information sent from SAC
 *                smscString - pointer to SMSC address output string
 *                smscStrLength - pointer to the length of SMSC address output string
 * Returns      : Nothing
 * -------------------------------------------------------------------------- */
static void libPutPackedSMSC( CiOptAddressInfo optSca, char *smscString, UINT16 *smscStrLength)
{
	INT32 i;

	*smscString = (CHAR)0;
	*smscStrLength = 0;

	if ( (optSca.Present) && (optSca.AddressInfo.Length > 0) )
	{
		/* addLen = Byte of address length + Bytes of address */
		*smscStrLength = 1 + (optSca.AddressInfo.Length / 2) + (optSca.AddressInfo.Length % 2) + 1;

		/* length of SMSC address */
		smscString[0] = '0';
		smscString[1] = libNumToHex( (optSca.AddressInfo.Length / 2) + (optSca.AddressInfo.Length % 2) + 1);

		/* type-of-address of the SMSC */
		smscString[2] = libNumToHex(8 + optSca.AddressInfo.AddrType.NumType);
		smscString[3] = libNumToHex(optSca.AddressInfo.AddrType.NumPlan);

		for ( i = 0; i < optSca.AddressInfo.Length; i += 2 )
		{
			/* check the length of the phone number is odd to add a trailing 'F' */
			if ( i == optSca.AddressInfo.Length - 1 )
			{
				smscString[4 + i + 1] = optSca.AddressInfo.Digits[i];
				smscString[4 + i]     = 'F';
			}
			else
			{
				smscString[4 + i + 1] = optSca.AddressInfo.Digits[i];
				smscString[4 + i]     = optSca.AddressInfo.Digits[i + 1];
			}
		}
	}
	else
	{
		*smscStrLength = 2;

		/* set null as default */
		sprintf((char *)smscString, "0100"); /* 1st byte(01): smsc length; 2nd byte(00): null smsc */
	}

	return;
}


/****************************************************************
 *  F@: libEncodeGsm7BitData - encode data into GSM 7-bit alphabet
 * This is a copy of SAC sacShEncodeGsm7BitData function
 */
static UINT16 libEncodeGsm7BitData( UINT8 *pInBuf, UINT16 inLen, UINT8 *pOutBuf )
{

	UINT16 inIdx, outIdx;
	UINT8 bits;

	inIdx   = 0;
	outIdx  = 0;
	bits    = 0;
	while ( inIdx < inLen )
	{
		pOutBuf[outIdx] = pInBuf[inIdx++] >> bits;
		if ( inIdx == inLen )
		{
			++outIdx;
			break;
		}
		pOutBuf[outIdx] |= pInBuf[inIdx] << (7 - bits);
		bits = ++outIdx % 7;
		if ( bits == 0 )
		{
			inIdx++;
		}
	}
	return outIdx;
}

/****************************************************************
 *  F@: libMsgEncodeUserData - encode user data for SMS
 * This is a copy of SAC sacMsgEncodeUserData function
 */
static void libMsgEncodeUserData(  BOOL userDataHeaderPresent, AtciMsgCodingType msgCoding, UINT8 *pDst,
				   UINT8 *pSrc, UINT8 msgLen, UINT8 offset, UINT8 *pLen )
{
	UINT8 udl, udhl1, new_udhl1;
	UINT16 len;

	udl       = msgLen;
	udhl1     = 0;
	new_udhl1 = 0;
	len       = 0;
	pDst[offset] = 0;

	if ( msgLen )
	{
		switch (msgCoding)
		{
		case ATCI_MSG_CODING_DEFAULT:
			if ( userDataHeaderPresent )
			{
				/* copy UD header */
				udhl1 = pSrc[0] + 1;
				memcpy( &pDst[ offset + 1 ], pSrc, udhl1 );
				/* fiil in bits */
				if ( udhl1 % 7 )
				{
					new_udhl1 = udhl1 + 7 - (udhl1 % 7);
				}
				else
				{
					new_udhl1 = udhl1;
				}
			}

			ASSERT( udl - udhl1 <= ATCI_SMS_MAX_MSG_LEN );

			len = libEncodeGsm7BitData(&pSrc[udhl1], udl - udhl1, &pDst[ offset + 1 + new_udhl1 ]);

			ASSERT( len <= 0xFF );

			pDst[ offset ] = (UINT8)(( (new_udhl1 * 8) / 7 ) + ( udl - udhl1 ));
			len += new_udhl1 + 1;
			break;
		default:
			len = msgLen;
			pDst[ offset++ ] = (UINT8)msgLen;
			memcpy( &pDst[ offset ], pSrc, msgLen );
			break;
		}
	}

	ASSERT( offset + len <= 0xFF );
	*pLen    = (UINT8)(len + offset);
}


/****************************************************************
 *  F@: libTXT2PDU - Converts a message in text format into PDU to be sent to CCI
 *  based on SAC sacMsgMakeSmsDeliveryTpdu function
 *
 * ASSUMPTIONS ABOUT THE MESSAGE BEING CONVERTED:
 *
 * 1. Converts outgoing SMS only (SMS-SUBMIT) (TP-MTI=01)
 * 2. (For now) SMSC address is supplied by the phone (using AT+SMSC command) - not encoded in PDU
 * 3. TP-RP=0; TP-UDHI=0; TP-SRR=0; TP-VPF=10 (for now); TP-RD=0
 * 4. TP-message reference is set by the phone (octet 3 is 00)
 * 5. TP-PID=00
 * 6. TP-DCS=00
 */
 #define ATCI_SMS_BCD_POS  2

static UINT8 libTXT2PDU( UINT8* msgData, UINT8 msgLen, UINT8* pTpdu )
{
	UINT8 offset = 0;
	UINT8 len;

	/* Copy the first octet */
	pTpdu[ offset++ ] = gAtSmsMessage.fo;

	/* Message Reference */
	pTpdu[ offset++ ] = gAtSmsMessage.msgRef;

	/* Originating Address (TP-OA) */
	{
		UINT8 idx;
		UINT8 *data;

		data = pTpdu + offset;

		/* Set the Address Length octet */
		*data++ = strlen( (char *)gAtSmsMessage.destAddr );

		/* Format the TON/NPI octet */
		*data++ = (UINT8)((DialNumTypeGSM2CI(gAtSmsMessage.addrType) << 4) | gAtSmsMessage.addrPlan | 0x80);

		/* Format the BCD digits */
		for ( idx = 0; idx < strlen( (char *)gAtSmsMessage.destAddr ); idx++ )
		{
			libPutPackedBcd( data, idx, gAtSmsMessage.destAddr[ idx ], TRUE );
		}

		/* check if we need to tack on a filler */
		if ( idx & 0x01 )
		{
			/* Yup -- do it! , reversed nibbles */
			libPutPackedBcd( data, idx, ATCI_BCD_FILLER, TRUE );
			++idx;
		}

		/* We're done -- update the PDU byte index */
		offset += idx / 2 + ATCI_SMS_BCD_POS;
	}

	/* Protocol Identifier (TP-PID) */
	pTpdu[ offset++ ] = gAtSmsMessage.pid;

	/* Data Coding Scheme (TP-DCS) */
	pTpdu[ offset++ ] = gAtSmsMessage.dcs;

	/* Now for the Message Data (TP-UDL + TP-UD) */
	libMsgEncodeUserData( gAtSmsMessage.udhPresent, gAtSmsMessage.dcs, pTpdu, msgData, msgLen, offset, &len);

	return len;
}

/************************************************************************************
 * F@: libMsgStorageStr2Enum - convert PB storage from string to enum value
 *
 */
static CiMsgStorage libMsgStorageStr2Enum(CHAR * MsgStorageStr)
{
	INT32 i;

	for ( i = 0; i < CI_MSG_NUM_STORAGES; i++ )
	{
		if ( strcasecmp((char *)MsgStorageStr, gMsgStorageStr[i]) == 0 )
		{
			break;
		}
	}

	return i;
}
#if 0
/************************************************************************************
 * F@: libCheckCurrOperationStatus - check whether gCurrOperation is NULL before new operation
 *
 */
static BOOL libCheckCurrOperationStatus(UINT32 atHandle, AT_MSG_OPERATION OperationID)
{
	char AtRspBuf [500];

	/* If other SMS operation is working, quit this new operation to avoid confliction */
	if ( gCurrOperation != AT_MSG_OPERATION_NULL )
	{
		sprintf(AtRspBuf, "\n***Other SMS Operation is going, OpeID=%d\n", OperationID);
		ERRMSG(AtRspBuf);
		ATRESP(atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR,  NULL);
		return(FALSE);
	}
	else
	{
		return(TRUE);
	}
}
#endif
/************************************************************************************
 *
 * Implementation of SMS related AT commands by sending Request to CCI. Called by Telmsg.c
 *
 *************************************************************************************/

/****************************************************************************************
*  FUNCTION:  MSG_QuerySmsService
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to query SMS service, implementation of AT+CSMS=?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MSG_QuerySmsService(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMsgPrimGetSupportedServicesReq   *getSupportedServicesReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MSG], CI_MSG_PRIM_GET_SUPPORTED_SERVICES_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MSG_PRIM_GET_SUPPORTED_SERVICES_REQ), (void *)getSupportedServicesReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MSG_GetSmsService
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get current SMS service, implementation of AT+CSMS?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MSG_GetSmsService(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMsgPrimGetCurrentServiceInfoReq   *getCurrentServiceReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MSG], CI_MSG_PRIM_GET_CURRENT_SERVICE_INFO_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MSG_PRIM_GET_CURRENT_SERVICE_INFO_REQ), (void *)getCurrentServiceReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MSG_SetSmsService
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to set current SMS service, implementation of AT+CSMS=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MSG_SetSmsService(UINT32 atHandle, const UINT32 service)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMsgPrimSelectServiceReq   *selectServiceReq;

	selectServiceReq = utlCalloc(1, sizeof(CiMsgPrimSelectServiceReq));
	if (selectServiceReq == NULL)
		return CIRC_FAIL;

	selectServiceReq->service = service;
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MSG], CI_MSG_PRIM_SELECT_SERVICE_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MSG_PRIM_SELECT_SERVICE_REQ), (void *)selectServiceReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MSG_QueryStorage
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to quary list of supported SMS storage, implementation of AT+CPMS=?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MSG_QuerySupportStorage(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMsgPrimGetSupportedStoragesReq   *getSupportedStoragesReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MSG], CI_MSG_PRIM_GET_SUPPORTED_STORAGES_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MSG_PRIM_GET_SUPPORTED_STORAGES_REQ), (void *)getSupportedStoragesReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MSG_GetStorageInfo
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get current SMS storage info, maybe called in several AT implementation, such as +CMGL, +CMGD=?, +CPMS?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MSG_GetStorageInfo(UINT32 atHandle, CiPrimitiveID priPrimId)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMsgPrimGetCurrentStorageInfoReq   *getStorageInfoReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MSG], CI_MSG_PRIM_GET_CURRENT_STORAGE_INFO_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, priPrimId), (void *)getStorageInfoReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MSG_GetPrefStorage
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get current preferred SMS storage, implementation of AT+CPMS?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MSG_GetPrefStorage(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	ret = MSG_GetStorageInfo(atHandle, CI_MSG_PRIM_GET_CURRENT_STORAGE_INFO_REQ);
	return ret;
}

/****************************************************************************************
*  FUNCTION:  MSG_SetPrefStorage
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to set current preferred SMS storage, implementation of AT+CPMS=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MSG_SetPrefStorage(UINT32 atHandle, CHAR *mem1Str, CHAR *mem2Str, CHAR *mem3Str,
				INT16 mem1StrLen, INT16 mem2StrLen, INT16 mem3StrLen)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMsgPrimSelectStoragesReq   *selectStoragesReq;
	BOOL paramValid = FALSE;
	CiMsgStorage storage;

	selectStoragesReq = utlCalloc(1, sizeof(CiMsgPrimSelectStoragesReq));
	if (selectStoragesReq == NULL)
		return CIRC_FAIL;

	if ( (mem1StrLen > 0) && ((storage = libMsgStorageStr2Enum(mem1Str)) < CI_MSG_NUM_STORAGES ))
	{
		/* MEM1 parameter is good, set the bit  mapping MEM1 to 1  */
		selectStoragesReq->bitsType |= (1UL) << CI_MSG_STRGOPERTYPE_READ_DELETE;
		selectStoragesReq->storages[CI_MSG_STRGOPERTYPE_READ_DELETE] = storage;
		paramValid = TRUE;
	}

	if ( (mem2StrLen > 0) && ((storage = libMsgStorageStr2Enum(mem2Str)) < CI_MSG_NUM_STORAGES ))
	{
		/* MEM2 parameter is good, set the bit  mapping MEM2 to 1 */
		selectStoragesReq->bitsType |= (1UL) << CI_MSG_STRGOPERTYPE_WRITE_SEND;
		selectStoragesReq->storages[CI_MSG_STRGOPERTYPE_WRITE_SEND] = storage;
	}

	if ( (mem3StrLen > 0) && ((storage = libMsgStorageStr2Enum(mem3Str)) < CI_MSG_NUM_STORAGES ))
	{
		/* MEM3 parameter is good, set the bit  mapping MEM3 to 1 */
		selectStoragesReq->bitsType |= (1UL) << CI_MSG_STRGOPERTYPE_RECV;
		selectStoragesReq->storages[CI_MSG_STRGOPERTYPE_RECV] = storage;
	}

	if(paramValid)  /* Only MEM1 is mandatory. If MEM1 is wrong, the para of this command is not supported */
	{
		ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MSG], CI_MSG_PRIM_SELECT_STORAGES_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MSG_PRIM_SELECT_STORAGES_REQ), (void *)selectStoragesReq );
	}
	else
	{
		utlFree(selectStoragesReq);
	}

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MSG_GetSrvCenterAddr
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get current SMS Service Center Address, implementation of AT+CSCA?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MSG_GetSrvCenterAddr(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMsgPrimGetSmscAddrReq   *getSrvCenterAddrReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MSG], CI_MSG_PRIM_GET_SMSC_ADDR_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MSG_PRIM_GET_SMSC_ADDR_REQ), (void *)getSrvCenterAddrReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MSG_SetSrvCenterAddr
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to set SMS service center address, implementation of AT+CSCA=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MSG_SetSrvCenterAddr(UINT32 atHandle, const CHAR *addressStr, UINT16 addrStrLen, UINT32 type)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMsgPrimSetSmscAddrReq         *setSrvCenterAddrReq;

	setSrvCenterAddrReq = utlCalloc(1, sizeof(CiMsgPrimSetSmscAddrReq));
	if (setSrvCenterAddrReq == NULL)
		return CIRC_FAIL;

	if ( addressStr[0] == CI_INTERNATIONAL_PREFIX )
	{
		type = ATCI_DIAL_NUMBER_INTERNATIONAL;
		addrStrLen = addrStrLen - 1;
		memcpy( setSrvCenterAddrReq->sca.Digits, &addressStr[1], (size_t)addrStrLen  );
	}
	else
	{
		type = ATCI_DIAL_NUMBER_UNKNOWN;
		memcpy( setSrvCenterAddrReq->sca.Digits, &addressStr[0], addrStrLen );
	}

	setSrvCenterAddrReq->sca.Length = (UINT8)addrStrLen;
	setSrvCenterAddrReq->sca.AddrType.NumType = DialNumTypeGSM2CI((UINT8)type);
	setSrvCenterAddrReq->sca.AddrType.NumPlan = CI_NUMPLAN_E164_E163;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MSG], CI_MSG_PRIM_SET_SMSC_ADDR_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MSG_PRIM_SELECT_SERVICE_REQ), (void *)setSrvCenterAddrReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MSG_GetMsgTextMode
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get current message text mode parameter saved in global variable, implementation of AT+CSMP?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MSG_GetMsgTextMode(UINT32 atHandle, UINT8 *fo, UINT8 *vp, UINT8 *pid, UINT8 *dcs)
{
	CiReturnCode ret = CIRC_SUCCESS;

	UNUSEDPARAM(atHandle);

	*fo = gAtSmsMessage.fo;
	*vp = gAtSmsMessage.vp;
	*pid = gAtSmsMessage.pid;
	*dcs = gAtSmsMessage.dcs;
	return ret;
}

/****************************************************************************************
*  FUNCTION:  MSG_SetMsgTextMode
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to set message text mode parameter and save in global variable, implementation of AT+CSMP=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MSG_SetMsgTextMode(UINT32 atHandle, const UINT8 fo, const UINT8 vp, const UINT8 pid, const UINT8 dcs)
{
	CiReturnCode ret = CIRC_SUCCESS;

	UNUSEDPARAM(atHandle);

	gAtSmsMessage.fo = fo;
	gAtSmsMessage.vp = vp;
	gAtSmsMessage.pid = pid;
	gAtSmsMessage.dcs = dcs;
	return ret;
}

/****************************************************************************************
*  FUNCTION:  MSG_GetCbMsgType
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get current Cell Broadcast message types, implementation of AT+CSCB?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MSG_GetCbMsgType(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMsgPrimGetCbmTypesReq   *getCbMsgTypeReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MSG], CI_MSG_PRIM_GET_CBM_TYPES_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MSG_PRIM_GET_CBM_TYPES_REQ), (void *)getCbMsgTypeReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MSG_SetCbMsgType
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to set Cell Broadcast message types, implementation of AT+CSCB=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MSG_SetCbMsgType(UINT32 atHandle, BOOL mode, CHAR *mids, CHAR *dcss)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMsgPrimSetCbmTypesReq   *setCbMsgTypeReq;

/*	static CiMsgCbmTypesSet CbmTypeSet;
	static UINT32                   midsIndvNumList[ATCI_MAX_INDV_LIST_SIZE];
	static CiNumericRange   midsRangeList[ATCI_MAX_CI_NUMERIC_RANGE_LIST_SIZE];
	static UINT32                   dcssIndvNumList[ATCI_MAX_INDV_LIST_SIZE];
	static CiNumericRange   dcssRangeList[ATCI_MAX_CI_NUMERIC_RANGE_LIST_SIZE];
 */
	setCbMsgTypeReq = utlCalloc(1, sizeof(CiMsgPrimSetCbmTypesReq));
	if (setCbMsgTypeReq == NULL)
		return CIRC_FAIL;

/*
	memcpy(setCbMsgTypeReq->pSet.mids.rangeLst, midsRangeList, sizeof(CbmTypeSet.mids.rangeLst));
	memcpy(setCbMsgTypeReq->pSet.mids.indvList, midsIndvNumList,sizeof(CbmTypeSet.mids.indvList));
	memcpy(setCbMsgTypeReq->pSet.dcss.rangeLst, dcssRangeList, sizeof(CbmTypeSet.dcss.rangeLst));
	memcpy(setCbMsgTypeReq->pSet.dcss.indvList, dcssIndvNumList, sizeof(CbmTypeSet.dcss.indvList));
 */
	setCbMsgTypeReq->set.mode = !mode; /* CI has mode defined differently from ATCI: 1 - enable, 0 disable */

	if ( (str2NumericList( mids, &(setCbMsgTypeReq->set.mids) ) == FALSE)
	     || (str2NumericList( dcss, &(setCbMsgTypeReq->set.dcss) ) == FALSE ) )
	{
		/* Invalid numeric list parameter */
		utlFree(setCbMsgTypeReq);
		return CIRC_FAIL;
	}

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MSG], CI_MSG_PRIM_SET_CBM_TYPES_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MSG_PRIM_SET_CBM_TYPES_REQ), (void *)setCbMsgTypeReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MSG_GetNewMsgIndSetting
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get current setting about new message indication, implementation of AT+CNMI?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MSG_GetNewMsgIndSetting(UINT32 atHandle, UINT32 *mode, UINT32 *mt, UINT32 *bm, UINT32 *ds, UINT32 *bfr)
{
	UNUSEDPARAM(atHandle);
	CiReturnCode ret = CIRC_SUCCESS;

	*mode = gCurrNewMsgIndSetting.mode;
	*mt     = gCurrNewMsgIndSetting.mt;
	*bm    = gCurrNewMsgIndSetting.bm;
	*ds      = gCurrNewMsgIndSetting.ds;
	*bfr     = gCurrNewMsgIndSetting.bfr;
	return ret;
}

/****************************************************************************************
*  FUNCTION:  MSG_SetNewMsgIndSetting
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to set setting about new message indication, implementation of AT+CNMI=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MSG_SetNewMsgIndSetting(UINT32 atHandle, const INT32 mode, const INT32 mt, const INT32 bm, const INT32 ds, const INT32 bfr)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMsgPrimConfigMsgIndReq   *configNMIReq;

	configNMIReq = utlCalloc(1, sizeof(CiMsgPrimConfigMsgIndReq));
	if (configNMIReq == NULL)
		return CIRC_FAIL;

	/* Fill in the request structure */
	memset(configNMIReq, 0, sizeof(CiMsgPrimConfigMsgIndReq));
	configNMIReq->smsDeliverIndSetting   = mt;
	configNMIReq->cbmIndSetting             = bm;

	// SAC is not mapping  smsStatusReportIndSetting to protocol stack dsCnmiVal correctly, do workaround here
	//ConfigNMIReq.smsStatusReportIndSetting = ds;
	if (ds == 0)
	{
		configNMIReq->smsStatusReportIndSetting = CI_MSG_MSG_IND_DISABLE;
	}
	else if (ds == 1)
	{
		configNMIReq->smsStatusReportIndSetting = CI_MSG_MSG_IND_MSG;
	}
	else if (ds == 2)
	{
		configNMIReq->smsStatusReportIndSetting = CI_MSG_MSG_IND_INDEX;
	}
	/* Save setting to global variable, and copy to gCurrNewMsgIndSetting upon receiving of CI Cnf msg */
	gTempNewMsgIndSetting.mode = mode;
	gTempNewMsgIndSetting.mt	 = mt;
	gTempNewMsgIndSetting.bm	= bm;
	gTempNewMsgIndSetting.ds	  = ds;
	gTempNewMsgIndSetting.bfr	  = bfr;

	/* NOTE: SAC does not take MODE and BFR variables */
	//FIX: IS it causing SAC not reporting new message for Qtopia?

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MSG], CI_MSG_PRIM_CONFIG_MSG_IND_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MSG_PRIM_CONFIG_MSG_IND_REQ), (void *)configNMIReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MSG_ListMsgStart
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to start list message operation, get storage info first and read SMS list in CI cnf msg, implementation of AT+CMGL=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MSG_ListMsgStart(UINT32 atHandle, const CiMsgMsgStatus readSmsOption)
{
	CiReturnCode ret = CIRC_FAIL;
	ret = MSG_GetStorageInfo(atHandle, PRI_MSG_PRIM_LIST_MESSAGE_REQ + readSmsOption);
	return ret;
}

/****************************************************************************************
*  FUNCTION:  MSG_ReadMsg
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to read one SMS specified by index, implementation of AT+CMGR=<index>
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
static CiReturnCode MSG_ReadMsg_(UINT32 reqHandle, const UINT32 msgIndex)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMsgPrimReadMessageReq     *readMsgReq;

	readMsgReq = utlCalloc(1, sizeof(CiMsgPrimReadMessageReq));
	if(readMsgReq == NULL) {
		return CIRC_FAIL;
	}

	readMsgReq->index = msgIndex;
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MSG], CI_MSG_PRIM_READ_MESSAGE_REQ,
			 reqHandle, (void *)readMsgReq );

	return ret;
}

CiReturnCode MSG_ReadMsg(UINT32 atHandle, const UINT32 msgIndex)
{
	UINT32 reqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_MSG_PRIM_READ_MESSAGE_REQ);

	return MSG_ReadMsg_(reqHandle, msgIndex);
}

/****************************************************************************************
 * interface to read one SMS specified by index, implementation of AT+CRSM=178,28476,... */
CiReturnCode MSG_CRSM_ReadMsg(UINT32 atHandle, const UINT32 msgIndex)
{
	UINT32 reqHandle = MAKE_CRSM_CI_REQ_HANDLE(atHandle, CI_MSG_PRIM_READ_MESSAGE_REQ);

	return MSG_ReadMsg_(reqHandle, msgIndex);
}

/****************************************************************************************
*  FUNCTION:  MSG_SetSentMsgInfo
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to set msg info about sending SMS, used in implementation of AT+CMGS=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MSG_SetSentMsgInfo(UINT32 atHandle, const UINT8 smsFormatMode, const UINT8 maxMsgLen, const CiAddressInfo *pDestAddr)
{
	UNUSEDPARAM(atHandle)
	CiReturnCode ret = CIRC_SUCCESS;

	gAtSmsMessage.maxMsgLen = maxMsgLen;
	if ( (smsFormatMode == ATCI_SMS_FORMAT_TXT_MODE) && (pDestAddr != NULL) )
	{
		strncpy((char *)(gAtSmsMessage.destAddr), (char *)(pDestAddr->Digits), (size_t)(pDestAddr->Length));
		gAtSmsMessage.destAddr[pDestAddr->Length] = '\0';
		gAtSmsMessage.addrType = pDestAddr->AddrType.NumType;
		gAtSmsMessage.addrPlan = pDestAddr->AddrType.NumPlan;
	}
	return ret;
}

/****************************************************************************************
*  FUNCTION:  MSG_SendStoredMsg
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to send msg from storage, the SMS is specified by index, implementation of AT+CMSS=<index>[,<da>[,<toda>]]
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MSG_SendStoredMsg(UINT32 atHandle, const UINT32 msgIndex, const CiAddressInfo newDestAddr)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMsgPrimSendStoredMessageReq     *sendStoredMsgReq;

	sendStoredMsgReq = utlCalloc(1, sizeof(CiMsgPrimSendStoredMessageReq));
	if(sendStoredMsgReq == NULL) {
		return CIRC_FAIL;
	}
	sendStoredMsgReq->index = msgIndex;

	/* send more message flag based on CMMS setting */
	sendStoredMsgReq->bMoreMessage = (CiBoolean)gModeMoreMsgSent;

	/* If new SMS addr is given, it shall be used instead of original one stored with message */
	if (newDestAddr.Length > 0)
	{
		sendStoredMsgReq->destAddr = newDestAddr;
		sendStoredMsgReq->destAddrPresent = TRUE;

	}
	else
	{
		memset(&(sendStoredMsgReq->destAddr), 0, sizeof(CiAddressInfo));
		sendStoredMsgReq->destAddrPresent = FALSE;
	}

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MSG], CI_MSG_PRIM_SEND_STORED_MESSAGE_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MSG_PRIM_SEND_STORED_MESSAGE_REQ), (void *)sendStoredMsgReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MSG_SetStoredMsgInfo
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to set msg info about sending SMS, used in implementation of AT+CMGW=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MSG_SetStoredMsgInfo(UINT32 atHandle, const UINT8 smsFormatMode, const UINT8 maxMsgLen, const CiAddressInfo *pDestAddr, const UINT8 stat)
{
	UNUSEDPARAM(atHandle)
	CiReturnCode ret = CIRC_SUCCESS;

	gAtSmsMessage.maxMsgLen = maxMsgLen;
	gAtSmsMessage.stat = stat;
	if ( (smsFormatMode == ATCI_SMS_FORMAT_TXT_MODE) && (pDestAddr != NULL) )
	{
		strncpy((char *)(gAtSmsMessage.destAddr), (char *)(pDestAddr->Digits), (size_t)(pDestAddr->Length));
		gAtSmsMessage.destAddr[pDestAddr->Length] = '\0';
		gAtSmsMessage.addrType = pDestAddr->AddrType.NumType;
		gAtSmsMessage.addrPlan = pDestAddr->AddrType.NumPlan;
	}
	return ret;
}


/****************************************************************************************
*  FUNCTION:  MSG_QueryDelMsg
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to query info about deleting msg, like list of msg and list of supported delflag, implementation of AT+CMGD=?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MSG_QueryDelMsg(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;

	ret = MSG_GetStorageInfo(atHandle, PRI_MSG_PRIM_QUERY_DEL_MESSAGE_REQ);

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MSG_DeleteMsg
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to delete one SMS specified by index or some kind of all msg specified by delFlag, implementation of AT+CMGD=<index>[,<delflag>]
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MSG_DeleteMsg_(UINT32 reqHandle, const UINT32 msgIndex, const UINT8 delFlag)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMsgPrimDeleteMessageReq     *deleteSmsReq;

	deleteSmsReq = utlCalloc(1, sizeof(CiMsgPrimDeleteMessageReq));
	if (deleteSmsReq == NULL) {
		return CIRC_FAIL;
	}

	deleteSmsReq->index = msgIndex;
	deleteSmsReq->flag = delFlag;
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MSG], CI_MSG_PRIM_DELETE_MESSAGE_REQ,
			 reqHandle, (void *)deleteSmsReq );

	return ret;
}

CiReturnCode MSG_DeleteMsg(UINT32 atHandle, const UINT32 msgIndex, const UINT8 delFlag)
{
	UINT32 reqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_MSG_PRIM_DELETE_MESSAGE_REQ);

	return MSG_DeleteMsg_(reqHandle, msgIndex, delFlag);
}

/****************************************************************************************
 * interface to delete one SMS specified by index, implementation of AT+CRSM=220,28476,... */
CiReturnCode MSG_CRSM_DeleteMsg(UINT32 atHandle, const UINT32 msgIndex, const UINT8 delFlag)
{
	UINT32 reqHandle = MAKE_CRSM_CI_REQ_HANDLE(atHandle, CI_MSG_PRIM_DELETE_MESSAGE_REQ);

	return MSG_DeleteMsg_(reqHandle, msgIndex, delFlag);
}


/****************************************************************************************
*  FUNCTION:  MSG_DeleteAllMsgStart
*
*  PARAMETERS:
*
*  DESCRIPTION: entrance interface to delete all msg, implementation of AT+CMGD=<index>,4
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MSG_DeleteAllMsgStart(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	ret = MSG_GetStorageInfo(atHandle, PRI_MSG_PRIM_DEL_ALL_MESSAGE_REQ);

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MSG_GetModeMoreMsgToSend
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get current mode about more message to send, implementation of AT+CMMS?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MSG_GetModeMoreMsgToSend(UINT32 atHandle, UINT32 *mode)
{
	UNUSEDPARAM(atHandle)
	CiReturnCode ret = CIRC_SUCCESS;

	*mode = gModeMoreMsgSent;
	return ret;
}

/****************************************************************************************
*  FUNCTION:  MSG_SetModeMoreMsgToSend
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to set mode about more message to send, implementation of AT+CMMS=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MSG_SetModeMoreMsgToSend(UINT32 atHandle, const UINT32 mode)
{
	UNUSEDPARAM(atHandle)
	CiReturnCode ret = CIRC_SUCCESS;

	gModeMoreMsgSent = mode;
	return ret;
}

/****************************************************************************************
*  FUNCTION:  MSG_QueryMoService
*
*  PARAMETERS:
*
*  DESCRIPTION: Interface to query supported service list for MO SMS, implementation of AT+CGSMS=?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MSG_QueryMoService(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMsgPrimGetMoSmsServiceCapReq     *getMoSmsServiceCapReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MSG], CI_MSG_PRIM_GET_MOSMS_SERVICE_CAP_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MSG_PRIM_GET_MOSMS_SERVICE_CAP_REQ), (void *)getMoSmsServiceCapReq );

	return ret;
}
#if 0
/****************************************************************************************
*  FUNCTION:  MSG_RSTMemFull
*
*  PARAMETERS:
*
*  DESCRIPTION: implementation of AT*RSTMemFull
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MSG_RSTMemFull(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MSG], CI_MSG_PRIM_RESET_MEMCAP_FULL_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MSG_PRIM_RESET_MEMCAP_FULL_REQ),
			 NULL);

	return ret;
}
#endif
/****************************************************************************************
*  FUNCTION:  MSG_CMEMFULL
*
*  PARAMETERS:
*
*  DESCRIPTION: implementation of AT+CMEMFULL
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MSG_CMEMFULL(UINT32 atHandle, BOOL bMemFull)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMsgPrimResetMemcapFullReq		*resetMemcapFullReq = NULL;
#ifdef AT_CUSTOMER_HTC
	resetMemcapFullReq = utlCalloc(1, sizeof(CiMsgPrimResetMemcapFullReq));
	if (resetMemcapFullReq == NULL) {
		return CIRC_FAIL;
	}
	resetMemcapFullReq->memcapFull = bMemFull;
#else
	if(bMemFull == 1)
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		return ret;
	}
#endif
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MSG], CI_MSG_PRIM_RESET_MEMCAP_FULL_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MSG_PRIM_RESET_MEMCAP_FULL_REQ), resetMemcapFullReq);

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MSG_GetMoService
*
*  PARAMETERS:
*
*  DESCRIPTION: Interface to get current service of MO SMS, implementation of AT+CGSMS?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MSG_GetMoService(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMsgPrimGetMoSmsServiceReq     *getMoSmsServiceReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MSG], CI_MSG_PRIM_GET_MOSMS_SERVICE_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MSG_PRIM_GET_MOSMS_SERVICE_REQ), (void *)getMoSmsServiceReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MSG_SetMoService
*
*  PARAMETERS:
*
*  DESCRIPTION: Interface to set service for MO SMS, implementation of AT+CGSMS=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MSG_SetMoService(UINT32 atHandle, UINT32 service)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMsgPrimSetMoSmsServiceReq     *setMoSmsServiceReq;

	setMoSmsServiceReq = utlCalloc(1, sizeof(CiMsgPrimSetMoSmsServiceReq));
	if (setMoSmsServiceReq == NULL)
		return CIRC_FAIL;

	setMoSmsServiceReq->cfg = service;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MSG], CI_MSG_PRIM_SET_MOSMS_SERVICE_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MSG_PRIM_SET_MOSMS_SERVICE_REQ), (void *)setMoSmsServiceReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MSG_SendMsg
*
*  PARAMETERS:
*
*  DESCRIPTION: Interface to send msg after enter SMS content  and press Ctrl+Z, implementation of AT+CMGS=...<ctrl+z>
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MSG_SendMsg(UINT32 reqHandle, INT32 formatMode, const CHAR *pSmsData, const INT32 smsDataLength, const CHAR *pSmscData, const INT32 smscLength)
{
	CiReturnCode ret = CIRC_FAIL;
	UINT8 pduData[ CI_MAX_CI_MSG_PDU_SIZE ];
	UINT8 smscData[ CI_MAX_ADDRESS_LENGTH ];
	CiAddressInfo smscPdu;
	CiMsgPdu smsPdu;
	INT32 i;
	char AtIndBuf[50];
	INT32 offset = 0;
	INT32 inSMSLen = smsDataLength;

	if (pSmsData == NULL)
	{
		ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		return CIRC_SUCCESS;
	}

	do {
		/* Text format */
		if ( formatMode == ATCI_SMS_FORMAT_TXT_MODE )
		{
			if (offset + ATCI_SMS_MAX_MSG_LEN <= smsDataLength)
			{
				inSMSLen = ATCI_SMS_MAX_MSG_LEN;
			}
			else
			{
				inSMSLen = smsDataLength % ATCI_SMS_MAX_MSG_LEN;
			}
			smsPdu.len = libTXT2PDU( (UINT8 *)pSmsData + offset, inSMSLen, pduData );

		}
		/* PDU format */
		else
		{
			/*  Convert from hexadecimal digits pairs to bytes in place */
			if ( (smsDataLength % 2) || (smscLength % 2) )
			{
				ATRESP( reqHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, "PDU format: smsDataLength is not even");
				return CIRC_FAIL;
			}
			if(smscLength > 0)
			{
				smscPdu.AddrType.NumType = (UINT8)((hexToNum(pSmscData[0]) >> 4) & 0x07);
				smscPdu.AddrType.NumPlan = (UINT8)(hexToNum(pSmscData[1])&0x0f);
				smscPdu.Length = (smscLength - 2)/2;
				for (i = 0; i < smscLength-2; i += 2)
				{
					smscData[i / 2] = (hexToNum(pSmscData[i+2]) << 4) + hexToNum(pSmscData[i+3]);
				}
				memcpy(smscPdu.Digits, smscData, CI_MAX_ADDRESS_LENGTH*sizeof(UINT8));
			}
			for ( i = 0; i < smsDataLength; i += 2 )
			{
				pduData[i / 2] = (hexToNum(pSmsData[i]) << 4) + hexToNum(pSmsData[i + 1]);
			}
			smsPdu.len = smsDataLength / 2;
		}
		/* Fill in the pdu data buffer */
		memcpy(smsPdu.data, pduData, CI_MAX_CI_MSG_PDU_SIZE * sizeof(UINT8));

		/* Send CI request based on the SMS operation in progress */
		if ( GET_REQ_ID(reqHandle) == CI_MSG_PRIM_SEND_MESSAGE_REQ )
		{
			CiMsgPrimSendMessageReq     *sendSmsReq;
			sendSmsReq = utlCalloc(1, sizeof(CiMsgPrimSendMessageReq));
			if (sendSmsReq == NULL) {
				ATRESP( reqHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
				return CIRC_FAIL;
			}

			sendSmsReq->pdu = smsPdu;
			sendSmsReq->bMoreMessage = gModeMoreMsgSent;
			if(smscLength > 0) {
				sendSmsReq->optSca.Present = TRUE;
				sendSmsReq->optSca.AddressInfo = smscPdu;
			}

			ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MSG], CI_MSG_PRIM_SEND_MESSAGE_REQ,
					 reqHandle, (void *)sendSmsReq );
			if(ret != CIRC_SUCCESS)
			{
				ATRESP( reqHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
				return CIRC_FAIL;
			}
		}
		else if ( GET_REQ_ID(reqHandle) == CI_MSG_PRIM_WRITE_MESSAGE_REQ )
		{
			CiMsgPrimWriteMessageReq     *writeSmsReq;
			writeSmsReq = utlCalloc(1, sizeof(CiMsgPrimWriteMessageReq));
			if (writeSmsReq == NULL) {
				ATRESP( reqHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
				return CIRC_FAIL;
			}

			writeSmsReq->pdu = smsPdu;
			writeSmsReq->status = gAtSmsMessage.stat;
			writeSmsReq->statusPresent = TRUE;
			if(smscLength > 0) {
				writeSmsReq->optSca.Present = TRUE;
				writeSmsReq->optSca.AddressInfo = smscPdu;
			}

			/* it need to check if SIM memory is full before writing SMS to SIM*/
			if (gMemCapExceeded)
			{
				sprintf(AtIndBuf, "+MMSG: %d, %d\r\n", 1, 1);
				ATRESP( IND_REQ_HANDLE, 0, 0, (char *)AtIndBuf );
				ATRESP(reqHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_MEMORY_FULL, "SIM memory is full");
				utlFree(writeSmsReq);
				return CIRC_FAIL;
			}
			else
			{
				ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MSG], CI_MSG_PRIM_WRITE_MESSAGE_REQ,
						 reqHandle, (void *)writeSmsReq );
				if(ret != CIRC_SUCCESS)
				{
					ATRESP( reqHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
					return CIRC_FAIL;
				}
			}
		}
		else
		{
			ATRESP(reqHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, "Wrong operation ID when sending or storing msg");
			return CIRC_FAIL;
		}
		offset += inSMSLen;
	} while (offset < smsDataLength);

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MSG_CRSM_WriteMsgWithIndex
*
*  PARAMETERS:
*
*  DESCRIPTION: Interface to send msg with index, implementation of AT+CRSM=220,28476,<sw1>,<sw2>,<length>,<data>
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MSG_CRSM_WriteMsgWithIndex(UINT32 atHandle, INT32 index, const CiMsgPdu *smsPdu)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMsgPrimWriteMsgWithIndexReq  *writeSmsReqWithIndex;
	if( smsPdu == NULL)
		return ret;

	writeSmsReqWithIndex = utlCalloc(1, sizeof(CiMsgPrimWriteMsgWithIndexReq));
	if (writeSmsReqWithIndex == NULL)
		return CIRC_FAIL;

	writeSmsReqWithIndex->index = index;
	memcpy(&writeSmsReqWithIndex->pdu, smsPdu, sizeof(CiMsgPdu));
	writeSmsReqWithIndex->status = CI_MSG_MSG_STAT_STO_UNSENT;   //[Jerry] this status should be set by +CRSM

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MSG], CI_MSG_PRIM_WRITE_MSG_WITH_INDEX_REQ,
			 MAKE_CRSM_CI_REQ_HANDLE(atHandle, CI_MSG_PRIM_WRITE_MSG_WITH_INDEX_REQ), (void *)writeSmsReqWithIndex );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MSG_NewMsgAck
*
*  PARAMETERS:
*
*  DESCRIPTION: Interface to do new message acknowledgement, implementation of AT+CNMA (txt mode) or +CNMA=<n> (PDU mode)
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MSG_NewMsgAck(UINT32 atHandle, UINT32 reply)
{
	CiReturnCode ret = CIRC_FAIL;

	//CiMsgPrimNewMsgRsp     *newMsgRsp;

	g_iNewMsgAck = reply;

	/* If there is no ind msg waiting for ack, report error and return */
	if ( gNewMsgAckCounter == 0 )
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_NO_CNMA_ACK_EXPECTED, NULL);
		return ret;
	}
	else
	{
		gNewMsgAckCounter--;
	}

	// newMsgRsp = utlCalloc(1, sizeof(CiMsgPrimNewMsgRsp));
	// if ( (reply == 0) || (reply == 1))
	// {
	//         newMsgRsp->success = TRUE;
	// }
	// else if (reply == 2)
	// {
	//         newMsgRsp->success = FALSE;
	// }
	// else
	// {
	//         ret = ATRESP( atHandle,ATCI_RESULT_CODE_CMS_ERROR,CMS_OPERATION_NOT_ALLOWED,NULL);
	//         return ret;
	// }

	// newMsgRsp->pduPresent = 0;

	//ret = ciRespond( gAtciSvgHandle[CI_SG_ID_MSG], CI_MSG_PRIM_NEWMSG_RSP,  gMtMsgIndHandle, (void *)newMsgRsp );

	/* Reply to AT parser to stop timer of this AT command session */
	ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);

	return ret;
}

/************************************************************************************
 *
 * Internal library function used to decode SMS data when processing CI Cnf msg or CI Ind msg
 *
 *************************************************************************************/

/****************************************************************
 *  F@: libConvertCiMsgStatTypeToInteger - convert SMS status digit to enum status type
 *  **************************************************************/
static BOOL libConvertCiMsgStatTypeToInteger( INT32 *statInt, CiMsgMsgStatus msgStatType )
{
	BOOL ret = TRUE;

	switch ( msgStatType )
	{
	case CI_MSG_MSG_STAT_REC_UNREAD:
		*statInt = 0;
		break;
	case CI_MSG_MSG_STAT_READ:
		*statInt = 1;
		break;
	case CI_MSG_MSG_STAT_STO_UNSENT:
		*statInt = 2;
		break;
	case CI_MSG_MSG_STAT_STO_SENT:
		*statInt = 3;
		break;
	case CI_MSG_MSG_STAT_ALL:
		*statInt = 4;
		break;
	default:
		ret = FALSE;
		break;
	}
	return ret;
}


/* --------------------------------------------------------------------------
 * Function    : libReadSmsAddress
 * Description : Formats an SmsAddress structure from Raw PDU data
 * Parameters  : pPdu  - Pointer to raw SMS PDU data
 *               pIdx  - Byte index to Address field in PDU
 *               pAddr - Pointer to output Address string
 * Returns     : Nothing
 * Notes       : Updates the PDU byte index (points past the Address field).
 *
 *               See TS 23.040 Section 9.1.2.5 for the PDU Address field format.
 * -------------------------------------------------------------------------- */
static void libReadSmsAddress( UINT8 *pPdu, UINT8 *pIdx, UINT8 *pAddr )
{
	UINT8 idx, *data, addrLen, outIdx = 0;

	/* Assertions */
	if ( (pPdu == NULL) || (pIdx == NULL) || (pAddr == NULL) )
	{
		DBGMSG("PDU not valid.\n");
		return;
	}

	data = pPdu + *pIdx;

	/* Pick up the Address Length (number of BCD digits) */
	addrLen = *data++;

	/* We don't care about Numbering Plan here */
	/* Just insert a '+' if this is international number */
	if ( ((*data++ >> 4) & 0x07 ) == CI_NUMTYPE_INTERNATIONAL )
	{
		pAddr[ outIdx++ ] = CI_INTERNATIONAL_PREFIX;
	}

	/* Now for the BCD digits */
	for ( idx = 0; idx < addrLen; idx++ )
	{
		if ( (idx & 0x01) == 0 )
		{
			/* Even digit - return value from LS 4 bits */
			pAddr[ outIdx++ ] = ( data[ idx / 2 ] & ATCI_BCD_LOW_MASK ) + '0';
		}
		else
		{
			/* Odd digit - return value from MS 4 bits */
			pAddr[ outIdx++ ] = ( ( data[ idx / 2 ] & ATCI_BCD_HIGH_MASK ) >> 4 ) + '0';
		}
	}

	/* Add NULL-terminator */
	pAddr[ outIdx++ ] = '\0';

	/* We're done -- update the PDU byte index */
	*pIdx += (UINT8)( ((idx + 1) / 2) + ATCI_SMS_BCD_POS );
}

/****************************************************************
 *  F@: libDecodeGsm7BitData - Decode text from 7-bit GSM alphabet
 *  **************************************************************/
static UINT16 libDecodeGsm7BitData( UINT8 *pInBuf, UINT16 inLen, UINT8 *pOutBuf )  // GSM 7bit -> CI
{
	UINT16 inIdx = 0, outIdx = 0;
	UINT8 bits = 0;

	while ( inIdx < inLen )
	{
		if ( bits == 0 )
		{
			pOutBuf[outIdx] = 0;
		}
		pOutBuf[outIdx++] |= ((pInBuf[inIdx] << bits) & 0x7F);
		pOutBuf[outIdx] = pInBuf[inIdx] >> (7 - bits);
		bits = ++inIdx % 7;
		//if( bits == 0 && inIdx < inLen)
		if ( bits == 0 )
		{
			outIdx++;
		}
	}

	return outIdx;
}


/****************************************************************
 *  F@: libMsgDecodeUserData - decode SMS text
 *  **************************************************************/
static void libMsgDecodeUserData( BOOL userDataHeaderPresent, AtciMsgCodingType msgCoding,
				  UINT8 *pDst, UINT8 *pSrc, UINT8 msgLen, UINT8 offset, UINT8 *pLen )
{
	UINT8 udl = msgLen, udhl1 = 0, new_udhl1 = 0, len = 0;
	UINT8 idx = offset, msgParsed = 0;
	UINT16 dlen;
	UINT32 msgHdrLen = 0, msgBodyLen = 0;

	DBGMSG("%s msgLen: %d, offset: %d\n", __FUNCTION__, msgLen, offset);
	DBGMSG("atciMsgDecodeUserData: msglen = %d\n", pSrc[offset]);

	if ( msgLen )
	{
		switch (msgCoding)
		{
		case ATCI_MSG_CODING_DEFAULT:
			udl = pSrc[ offset ];
			if ( userDataHeaderPresent )
			{
				udhl1 = pSrc[ offset + 1 ] + 1;

				if ( !(udhl1 > 0) )
				{

					DBGMSG("udhl1 < 0.\n");
					return;
				}

				memcpy( pDst, &pSrc[ offset + 1 ], udhl1);
				/* user header should at 7-byte boundary.
				   SCR1623257 Need to correct this code to 7bit boundaries */
				if ( udhl1 % 7 )
				{
					new_udhl1 = udhl1 + 7 - (udhl1 % 7);
				}
				else
				{
					new_udhl1 = udhl1;
				}

				if ( !(udl > udhl1) )
				{
					DBGMSG("udl < udhl1.\n");
					return;
				}

			}

			len = udl - udhl1;

			dlen = libDecodeGsm7BitData(&pSrc[ offset + 1 + new_udhl1], msgLen - new_udhl1, &pDst[udhl1]);

			if ( !(dlen <= 0xFF) )
			{
				DBGMSG("dlen >= 0xFF.\n");
				return;
			}

			len = udl;


			break;
		case ATCI_MSG_CODING_UCS2:
			udl = pSrc[offset];
			if (udl)
			{
				idx++;

				if (userDataHeaderPresent)
				{
					/* parse msg header */
					msgHdrLen = pSrc[idx];
					msgParsed = 1;
					if (msgHdrLen)
					{
						msgParsed += msgHdrLen;
						idx += msgParsed;
					}


				}
				else
				{
					msgParsed = 0;
				}
				msgBodyLen = (udl - msgParsed) / 2;

				if (msgBodyLen)
				{
					msgParsed = msgBodyLen * 2;
				}
				memcpy(pDst, &pSrc[idx], msgParsed);
				#if 0
				for (i = 0; i < msgParsed; i++)
				{
					pDst[i] = pSrc[idx + i];
				}
				#endif

				len = msgBodyLen;
			}
			break;
		default:
			len = pSrc[ offset++ ];
			memcpy( pDst, &pSrc[ offset ], len );
			break;
		}
	}

	/* Insert NULL-termination */
	pDst[ len ] = '\0';

	*pLen = len;
	return;
}

/****************************************************************
 *  F@: libReadSmsProtocolId - Get SMS protocol ID
 *  **************************************************************/
static void libReadSmsProtocolId( UINT8 *pPdu, UINT8 *pIdx, UINT32 *protocolId )
{
	UINT16 i;
	BOOL fRet = FALSE;
	if ((0 == (pPdu[*pIdx] & 0xe0)) || (0x80 == (pPdu[*pIdx] & 0xc0))) // Reserved or unsupported values should be treated as 0x00
	{
		*protocolId = MSGPROTOCOL_SMETOSME;
		fRet = TRUE;
	}
	else
	{
		for (i = 0; i < NUM_PROTOCOLIDS; i++)
		{
			if (protocolIDValues[i] == pPdu[*pIdx])
			{
				*protocolId = i;
				fRet = TRUE;
				break;
			}
		}

		if (!fRet)
		{
			*protocolId = MSGPROTOCOL_UNKNOWN;
			fRet = TRUE;
		}
	}
	*pIdx += 1;
	return;
}

/****************************************************************
 *  F@: libReadSmsDcs - Get SMS DCS (data coding scheme)
 *  **************************************************************/
static void libReadSmsDcs( UINT8 *pPdu, UINT8 *pIdx, DCS *dDCS)
{
	UINT8 bDCS;
	BOOL tFlag = FALSE;

	bDCS = pPdu[*pIdx];
	DBGMSG("bDCS=%d\n", bDCS);

	switch (bDCS & 0xf0)
	{
	case 0x00:
	case 0x10:
	case 0x20:
	case 0x30:
		dDCS->type = DCSTYPE_GENERAL;

		if (bDCS & 0x10)
		{
			switch (bDCS & 0x03)
			{
			case 0x00:
				dDCS->msgClass = DCSMSGCLASS_0;
				break;

			case 0x01:
				dDCS->msgClass = DCSMSGCLASS_1;
				break;

			case 0x02:
				dDCS->msgClass = DCSMSGCLASS_2;
				break;

			case 0x03:
				dDCS->msgClass = DCSMSGCLASS_3;
				break;

			default:
				ERRMSG("msg class error\n");
				return;
			}
		}

		switch (bDCS & 0x0c)
		{
		case 0x00:
			dDCS->alphabet = ATCI_MSG_CODING_DEFAULT;
			break;

		case 0x04:
			dDCS->alphabet = ATCI_MSG_CODING_8BIT;
			break;

		case 0x08:
			dDCS->alphabet = ATCI_MSG_CODING_UCS2;
			break;

		default:
			ERRMSG("coding scheme error\n");
			return;
		}
		break;

	case 0xd0:
		dDCS->alphabet = ATCI_MSG_CODING_DEFAULT;
		tFlag = TRUE;
	//
	// Fall through
	//
	case 0xe0:
		dDCS->type = DCSTYPE_MSGWAIT;
		if (!tFlag)
		{
			dDCS->alphabet = ATCI_MSG_CODING_UCS2;
		}

		switch (bDCS & 0x03)
		{
		case 0x00:
			dDCS->indication = DCSINDICATION_VOICEMAIL;
			break;
		case 0x01:
			dDCS->indication = DCSINDICATION_FAX;
			break;
		case 0x02:
			dDCS->indication = DCSINDICATION_EMAIL;
			break;
		case 0x03:
			dDCS->indication = DCSINDICATION_OTHER;
			break;
		default:
			ERRMSG("error DCS indication\n");
			return;
		}
		break;

	case 0xf0:
		dDCS->type = DCSTYPE_MSGCLASS;

		if (bDCS & 0x04)
		{
			dDCS->alphabet = ATCI_MSG_CODING_8BIT;
		}
		else
		{
			dDCS->alphabet = ATCI_MSG_CODING_DEFAULT;
		}

		switch (bDCS & 0x03)
		{
		case 0x00:
			dDCS->msgClass = DCSMSGCLASS_0;
			break;
		case 0x01:
			dDCS->msgClass = DCSMSGCLASS_1;
			break;
		case 0x02:
			dDCS->msgClass = DCSMSGCLASS_2;
			break;
		case 0x03:
			dDCS->msgClass = DCSMSGCLASS_3;
			break;
		default:
			ERRMSG("error msg class \n");
		}
		break;

	}
	*pIdx += 1;
}

/****************************************************************
 *  F@: libReadSmsTimeStamp - Get SMS time stamp
 *  **************************************************************/
static void libReadSmsTimeStamp( UINT8 *pPdu, UINT8 *pIdx, TIMESTAMP *tm)
{
	tm->tsYear = ((pPdu[ *pIdx ] & ATCI_BCD_HIGH_MASK) >> 4) +
		     ((pPdu[ *pIdx ] & ATCI_BCD_LOW_MASK) * 10);
	(*pIdx)++;

	tm->tsMonth = ((pPdu[ *pIdx ] & ATCI_BCD_HIGH_MASK) >> 4) +
		      ((pPdu[ *pIdx ] & ATCI_BCD_LOW_MASK) * 10);
	(*pIdx)++;

	tm->tsDay = ((pPdu[ *pIdx ] & ATCI_BCD_HIGH_MASK) >> 4) +
		    ((pPdu[ *pIdx ] & ATCI_BCD_LOW_MASK) * 10);
	(*pIdx)++;

	tm->tsHour = ((pPdu[ *pIdx] & ATCI_BCD_HIGH_MASK) >> 4) +
		     ((pPdu[ *pIdx ] & ATCI_BCD_LOW_MASK) * 10);
	(*pIdx)++;

	tm->tsMinute = ((pPdu[ *pIdx ] & ATCI_BCD_HIGH_MASK) >> 4) +
		       ((pPdu[ *pIdx ] & ATCI_BCD_LOW_MASK) * 10);
	(*pIdx)++;

	tm->tsSecond = ((pPdu[ *pIdx ] & ATCI_BCD_HIGH_MASK) >> 4) +
		       ((pPdu[ *pIdx ] & ATCI_BCD_LOW_MASK) * 10);
	(*pIdx)++;

	tm->tsTimezone = ((pPdu[ *pIdx ] & ATCI_BCD_HIGH_MASK) >> 4) +
			 ((pPdu[ *pIdx ] & ATCI_BCD_LOW_MASK) * 10);
	(*pIdx)++;

	/* if MSB is 1, GMT difference is negative */
	/* Which MSB? Before we "reverse" the nibbles or after????? */
	if ( tm->tsTimezone & 0x80 )
		tm->tsZoneSign = '-';
	else
		tm->tsZoneSign = '+';

	tm->tsTimezone = (tm->tsTimezone & 0x7F) / 4; /* one unit is 15 minutes, so divide by 4 */

}

/****************************************************************
 *  F@: convert SMS enum status type to status string
 *
 */
static BOOL libConvertCiMsgStatTypeToString(  CiMsgMsgStatus status, char *statStr)
{
	switch (status)
	{
	case CI_MSG_MSG_STAT_REC_UNREAD:
		sprintf(statStr, "REC UNREAD");
		break;
	case CI_MSG_MSG_STAT_READ:
		sprintf(statStr, "REC READ");
		break;
	case CI_MSG_MSG_STAT_STO_UNSENT:
		sprintf(statStr, "STO UNSENT");
		break;
	case CI_MSG_MSG_STAT_STO_SENT:
		sprintf(statStr, "STO SENT");
		break;
	case CI_MSG_MSG_STAT_ALL:
		sprintf(statStr, "ALL");
		break;
	default:
		sprintf(statStr, "UNKNOWN");
		return FALSE;
	}

	return TRUE;
}

/****************************************************************
 *  F@: readSmsDeliverPdu - read SMS-DELIVER PDU in text mode
 *  **************************************************************/
static void readSmsDeliverPdu(CiRequestHandle atHandle, CHAR *AtRspBuf, CiMsgPdu *pPdu )
{
	UINT8 idx;
	UINT16 unicodeMsgText[140];
	SHORTMESSAGE deliverMessage;
	char tempBuf [200];
	UINT32 i;
	BOOL hdrpresent = FALSE;

	memset(&deliverMessage, 0, sizeof(deliverMessage));

	idx = 0;
	if (!(pPdu->data[idx] & 0x04))
	{
		deliverMessage.bMsgFlag |= MSGFLAG_MORETOSEND;
	}
	// GSM 03.40 section 9.2.3.4
	if (pPdu->data[idx]  & 0x20)
	{
		deliverMessage.bMsgFlag |= MSGFLAG_STATUSREPORTRETURNED;
	}
	// GSM 03.40 section 9.2.3.23
	if (pPdu->data[idx]  & 0x40)
	{
		deliverMessage.bMsgFlag |= MSGFLAG_HEADER;
		hdrpresent = TRUE;
	}
	// GSM 03.40 section 9.2.3.17
	if (pPdu->data[idx]  & 0x80)
	{
		deliverMessage.bMsgFlag |= MSGFLAG_REPLYPATH;
	}
	DBGMSG("%s: bMsgFlag is : %d\n", __FUNCTION__, deliverMessage.bMsgFlag);


	/* Decode source address */
	idx++;
	libReadSmsAddress( pPdu->data, &idx, deliverMessage.srcAddr );
	DBGMSG("%s: srcAddr: %s.\n", __FUNCTION__, deliverMessage.srcAddr);


	/* decode protocol ID */
	libReadSmsProtocolId(pPdu->data, &idx, &deliverMessage.protocolId);
	DBGMSG("bProtocolId=%d\n", deliverMessage.protocolId);


	/* decode DCS */
	libReadSmsDcs(pPdu->data, &idx, &deliverMessage.bDCS);
	DBGMSG("alphabet=%d\n", deliverMessage.bDCS.alphabet);


	/* Decode timestamp  - take into account reversed nibbles */
	/* Date conversion does not work at the moment */
	libReadSmsTimeStamp(pPdu->data, &idx, &deliverMessage.timeStamp);

	/* Other fields are optional - option is set using CSDH - not implemented */
	sprintf( tempBuf, "\"%s\",,\"%02d.%02d.%02d ", deliverMessage.srcAddr,
		 deliverMessage.timeStamp.tsYear, deliverMessage.timeStamp.tsMonth,
		 deliverMessage.timeStamp.tsDay );
	strcat((char *)AtRspBuf, tempBuf);
	sprintf( tempBuf, "%02d:%02d:%02d ", deliverMessage.timeStamp.tsHour,
		 deliverMessage.timeStamp.tsMinute, deliverMessage.timeStamp.tsSecond );
	strcat((char *)AtRspBuf, tempBuf);
	sprintf( tempBuf, "GMT%c%d\"\r\n", deliverMessage.timeStamp.tsZoneSign, deliverMessage.timeStamp.tsTimezone );
	strcat((char *)AtRspBuf, tempBuf);

	libMsgDecodeUserData( hdrpresent, deliverMessage.bDCS.alphabet, deliverMessage.msgBuf, pPdu->data,
			      pPdu->len - idx - 1, idx, (UINT8 *)&deliverMessage.bMsgLength);

	if (deliverMessage.bDCS.alphabet == ATCI_MSG_CODING_DEFAULT)
	{

		i = 0;
		while (deliverMessage.msgBuf[i] != '\0')
		{
			DBGMSG("0x%x ", deliverMessage.msgBuf[i]);
			i++;
		}
		DBGMSG("%s: msgText = %s.\n", __FUNCTION__, deliverMessage.msgBuf);
		strcat((char *)AtRspBuf, (char *)deliverMessage.msgBuf);
		ATRESP( atHandle, 0, 0, (char *)AtRspBuf );

	}
	else if (deliverMessage.bDCS.alphabet == ATCI_MSG_CODING_UCS2)
	{

		for (i = 0; i < deliverMessage.bMsgLength; i++)
		{
			unicodeMsgText[i] = deliverMessage.msgBuf[2 * i] << 8 | deliverMessage.msgBuf[2 * i + 1];
		}

		unicodeMsgText[i] = '\0';
		i = 0;
		while (unicodeMsgText[i] != '\0')
		{
			DBGMSG("0x%x ", unicodeMsgText[i]);
			i++;
		}
		strcat((char *)AtRspBuf, (char *)deliverMessage.msgBuf);    //[add by Jerry, 2008/9/27]
		ATRESP( atHandle, 0, 0, (char *)AtRspBuf );
	}

	return;

}

/****************************************************************
 * F@: readSmsSubmitPdu - read SMS-SUBMIT PDU in text mode
 *  **************************************************************/
static void readSmsSubmitPdu( CiRequestHandle atHandle, CHAR *AtRspBuf, CiMsgPdu *pPdu )
{
	CHAR destAddr[ CI_MAX_ADDRESS_LENGTH + ATCI_NULL_TERMINATOR_LENGTH ];
	UINT8 idx, msgLen;
	UINT8 msgText[ ATCI_MAX_SMS_TDPU_SIZE ];

	/* Decode destination address */
	idx = 2; /* Address length should be in 3rd octet */
	libReadSmsAddress( pPdu->data, &idx, (UINT8 *)destAddr );

	idx += 2; /* skipping TP-PID and TP-DCS octets */

	/* Other fields are optional - option is set using CSDH - not implemented */
	strcat((char *)AtRspBuf, (char *)destAddr);
	ATRESP( atHandle, ATCI_RESULT_CODE_NULL, 0, (char *)AtRspBuf );

	/* Print the message text */
	libMsgDecodeUserData( FALSE, ATCI_MSG_CODING_DEFAULT, msgText, pPdu->data, pPdu->len - idx, idx, &msgLen );
	ATRESP( atHandle, 0, 0, (char *)msgText );

}

/****************************************************************
 *  F@: msgReadCnfForAtListMsg - process CI confirmation for AT+CMGL command
 *****************************************************************/
static void msgReadCnfForAtListMsg( CiRequestHandle atHandle, CiMsgPrimReadMessageCnf*  readPduCnf, CiReturnCode rc, CiMsgMsgStatus status)
{
	INT32 statInt, i;
	char AtRspBuf [500], TempBuf[3], statStr[20], PDURspBuf[400];
	CHAR smscBuf[CI_MAX_ADDRESS_LENGTH * 2 + ATCI_NULL_TERMINATOR_LENGTH];
	UINT16 smscLen = 0, AtRspBufLen = 0;

	/* Check if the received message meets the criteria to be displayed */
	if (( rc == CIRC_MSG_SUCCESS ) && ( readPduCnf->status != CI_MSG_MSG_STAT_REC_EMPTY ) &&
	    (( status == CI_MSG_MSG_STAT_ALL ) || ( status == readPduCnf->status )))
	{
		if ( gSmsFormatMode == ATCI_SMS_FORMAT_PDU_MODE )
		{
			DPRINTF("msgReadCnfForAtListMsg ATCI_SMS_FORMAT_PDU_MODE (start)\r\n");

			memset(AtRspBuf, 0x00, sizeof(AtRspBuf));
			libConvertCiMsgStatTypeToInteger( &statInt, readPduCnf->status );
			sprintf( (char *)AtRspBuf, "+CMGL: %d,%d,,%d\r\n", gCurrMsgIndex, statInt, readPduCnf->pdu.len );
			AtRspBufLen = strlen((char *)AtRspBuf);

			/* add an additional address field for the SMSC (SMS Center), followed by the TPDU data part. */
			memset(smscBuf, 0x00, sizeof(smscBuf));
			libPutPackedSMSC(readPduCnf->optSca, (char *)smscBuf, &smscLen);
			memcpy( &AtRspBuf[AtRspBufLen], smscBuf, (smscLen * 2) );
			AtRspBufLen += (smscLen * 2);

			/* TPDU data part */
			memset(PDURspBuf, 0x00, sizeof(PDURspBuf) );
			for ( i = 0; i < readPduCnf->pdu.len; i++ )
			{
				sprintf((char *)TempBuf, "%02X", readPduCnf->pdu.data[i]);
				strcat((char *)PDURspBuf, (char *)TempBuf);
			}

			memcpy(&AtRspBuf[AtRspBufLen], PDURspBuf, (readPduCnf->pdu.len * 2));
			AtRspBufLen += (readPduCnf->pdu.len * 2);

			/* Output OK when all msg are listed */
			ATRESP( atHandle, 0, 0, (char *)AtRspBuf );
		}
		else /* TEXT MODE */
		{

			if ( libConvertCiMsgStatTypeToString(readPduCnf->status, statStr) == FALSE)
			{
				strcpy(statStr, "UNKNOWN");
				ERRMSG("%s: status of message in mem unknown:%d\n", __FUNCTION__, readPduCnf->status);
			}

			sprintf( (char *)AtRspBuf, "+CMGL: %d,\"%s\",", gCurrMsgIndex, statStr );

			/* Determine the type of PDU we are dealing with */
			/* Note that when Service Centre Address is present, first octet is not actually first... */
			switch ( readPduCnf->pdu.data[0] & ATCI_TP_MTI_MASK )
			{
			case ATCI_SMS_DELIVER_MTI:
				readSmsDeliverPdu( atHandle, (CHAR *)AtRspBuf, &(readPduCnf->pdu) );
				break;

			case ATCI_SMS_SUBMIT_MTI:
				readSmsSubmitPdu( atHandle, (CHAR *)AtRspBuf, &(readPduCnf->pdu) );
				break;

			default:

				/* COMMAND or STATUS REPORT not supported */
				ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
				break;
			}
		}
	}

	return;
}

/****************************************************************
 *  F@: msgReadCnfForAtReadMsg - process CI confirmation for AT+CMGR command
 *****************************************************************/
static void msgReadCnfForAtReadMsg( CiRequestHandle atHandle, CiMsgPrimReadMessageCnf*  readPduCnf )
{
	INT32 i;
	char AtRspBuf [500], TempBuf[3], statStr[20], PDURspBuf[400];
	CHAR smscBuf[CI_MAX_ADDRESS_LENGTH * 2 + ATCI_NULL_TERMINATOR_LENGTH];
	UINT16 smscLen = 0, AtRspBufLen = 0;

	/* Currently in CI cnf msg only the PDU mode is supported */
	if ( gSmsFormatMode == ATCI_SMS_FORMAT_PDU_MODE )
	{
		DPRINTF("msgReadCnfForAtReadMsg ATCI_SMS_FORMAT_PDU_MODE (start)\r\n");

		memset(AtRspBuf, 0x00, sizeof(AtRspBuf));
		sprintf( (char *)AtRspBuf, "+CMGR: %d,,%d\r\n", readPduCnf->status, readPduCnf->pdu.len );
		AtRspBufLen = strlen((char *)AtRspBuf);

		/* add an additional address field for the SMSC (SMS Center), followed by the TPDU data part. */
		memset(smscBuf, 0x00, sizeof(smscBuf));
		libPutPackedSMSC(readPduCnf->optSca, (char *)smscBuf, &smscLen);
		memcpy( &AtRspBuf[AtRspBufLen], smscBuf, (smscLen * 2) );
		AtRspBufLen += (smscLen * 2);

		/* TPDU data part */
		memset(PDURspBuf, 0x00, sizeof(PDURspBuf) );
		for ( i = 0; i < readPduCnf->pdu.len; i++ )
		{
			sprintf((char *)TempBuf, "%02X", readPduCnf->pdu.data[i]);
			strcat((char *)PDURspBuf, (char *)TempBuf);
		}

		memcpy(&AtRspBuf[AtRspBufLen], PDURspBuf, (readPduCnf->pdu.len * 2));
		AtRspBufLen += (readPduCnf->pdu.len * 2);

		ATRESP(atHandle, 0, 0, (char *)AtRspBuf );
	}
	/* Text mode */
	else
	{
		if ( libConvertCiMsgStatTypeToString(readPduCnf->status, statStr) == FALSE)
		{
			strcpy(statStr, "UNKNOWN");
			ERRMSG("%s: status of message in mem unknown:%d\n", __FUNCTION__, readPduCnf->status);
		}

		sprintf( (char *)AtRspBuf, "+CMGR: \"%s\",", statStr );

		/* Determine the type of PDU we are dealing with */
		/* Note that when Service Centre Address is present, first octet is not actually first... */
		/* We are ignoring this fact for now */
		switch ( readPduCnf->pdu.data[0] & ATCI_TP_MTI_MASK )
		{
		case ATCI_SMS_DELIVER_MTI:
			readSmsDeliverPdu( atHandle, (CHAR *)AtRspBuf, &(readPduCnf->pdu) );
			break;

		case ATCI_SMS_SUBMIT_MTI:
			readSmsSubmitPdu( atHandle, (CHAR *)AtRspBuf, &(readPduCnf->pdu) );
			break;

		default:
			/* COMMAND or STATUS REPORT not supported */
			ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
			break;
		}
	}

	/* Output OK in last line */
	ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL );
	return;
}

/************************************************************************************
 *
 * Implementation of processing reply or indication of SMS related AT commands. Called by msgCnf() or msgInd
 *
 *************************************************************************************/

/****************************************************************
 *  F@: processQuerySmsServiceConf - Process CI cnf msg of +CSMS=?
 */
static void processQuerySmsServiceConf(UINT32 atHandle, const void *paras)
{
	CHAR AtRspBuf [500], TempBuf[300];
	INT32 i;
	CiMsgPrimGetSupportedServicesCnf  *getSupportedServicesCnf = (CiMsgPrimGetSupportedServicesCnf *)paras;

	if ( getSupportedServicesCnf->rc == CIRC_MSG_SUCCESS )
	{
		sprintf( (char *)AtRspBuf, "+CSMS: (%d", getSupportedServicesCnf->serviceLst[0] );
		for ( i = 1; i < getSupportedServicesCnf->len; i++ )
		{
			sprintf( (char *)TempBuf, ",%d", getSupportedServicesCnf->serviceLst[i] );
			strcat( (char *)AtRspBuf, (char *)TempBuf );
		}
		strcat( (char *)AtRspBuf, (char *)")" );
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)AtRspBuf );
	}
	else
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
	}
	return;
}

/****************************************************************
 *  F@: processGetSmsServiceConf - Process CI cnf msg of +CSMS?
 */
static void processGetSmsServiceConf(UINT32 atHandle, const void *paras)
{
	CHAR AtRspBuf [500];
	CiMsgPrimGetCurrentServiceInfoCnf  *getCurrentServiceCnf = (CiMsgPrimGetCurrentServiceInfoCnf *)paras;

	if ( getCurrentServiceCnf->rc == CIRC_MSG_SUCCESS )
	{
		sprintf( (char *)AtRspBuf, "+CSMS: %d,%d,%d,%d", getCurrentServiceCnf->service,
			 getCurrentServiceCnf->msgTypesInfo.mtSupported,
			 getCurrentServiceCnf->msgTypesInfo.moSupported,
			 getCurrentServiceCnf->msgTypesInfo.bmSupported );
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)AtRspBuf );
	}
	else
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
	}
	return;
}

/****************************************************************
 *  F@: processSetSmsServiceConf - Process CI cnf msg of +CSMS=
 */
static void processSetSmsServiceConf(UINT32 atHandle, const void *paras)
{
	CHAR AtRspBuf [500];
	CiMsgPrimSelectServiceCnf  *selectServiceCnf = (CiMsgPrimSelectServiceCnf *)paras;

	if ( selectServiceCnf->rc == CIRC_MSG_SUCCESS )
	{
		sprintf( (char *)AtRspBuf, "+CSMS: %d,%d,%d", selectServiceCnf->msgTypesInfo.mtSupported,
			 selectServiceCnf->msgTypesInfo.moSupported,
			 selectServiceCnf->msgTypesInfo.bmSupported );
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)AtRspBuf );
	}
	else
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
	}
	return;
}

/****************************************************************
 *  F@: processQuerySupportStorageConf - Process CI cnf msg of +CPMS=?
 */
static void processQuerySupportStorageConf(UINT32 atHandle, const void *paras)
{
	CHAR AtRspBuf [500];
	INT32 i, j;
	CiMsgPrimGetSupportedStoragesCnf  *getSupportedStoragesCnf = (CiMsgPrimGetSupportedStoragesCnf *)paras;

	if (getSupportedStoragesCnf->rc == CIRC_MSG_SUCCESS )
	{
		sprintf( (char *)AtRspBuf, "+CPMS: " );
		for ( i = 0; i < getSupportedStoragesCnf->len; i++ )
		{
			if ( i > 0 )
			{
				strcat( (char *)AtRspBuf, "," );
			}
			/* Display (list of supported <mem_i>s) */
			strcat( (char *)AtRspBuf, "(\"" );
			for ( j = 0; j < getSupportedStoragesCnf->lists[i].len; j++ )
			{
				if ( j > 0 )
				{
					strcat( (char *)AtRspBuf, "," );
				}
				strcat( (char *)AtRspBuf, gMsgStorageStr[getSupportedStoragesCnf->lists[i].storageList [j]]);
			}
			strcat( (char *)AtRspBuf, "\")" );
		}
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)AtRspBuf );

	}
	else
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
	}
	return;
}

/****************************************************************
 *  F@: processGetStorageInfoConf - Process CI cnf msg for +CPMS?, +CMGL, +CMGD=?
 */
			static void processGetStorageInfoConf(UINT32 reqHandle, const void *paras)
			{
				CHAR AtRspBuf [500];
				CiMsgPrimGetCurrentStorageInfoCnf  *getStorageInfoCnf = (CiMsgPrimGetCurrentStorageInfoCnf *)paras;

				if ( getStorageInfoCnf->rc != CIRC_MSG_SUCCESS )
				{
					ATRESP( reqHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
					return;
				}

				/* save info of mem1 (for read, list and delete) */
				gMem1MsgNum = getStorageInfoCnf->set.setting[CI_MSG_STRGOPERTYPE_READ_DELETE].used;
				gMem1TotalNum = getStorageInfoCnf->set.setting[CI_MSG_STRGOPERTYPE_READ_DELETE].total;

				/* Response of +CPMS? */
				if ( GET_REQ_ID(reqHandle) == CI_MSG_PRIM_GET_CURRENT_STORAGE_INFO_REQ)
				{
					sprintf( (char *)AtRspBuf, "+CPMS: \"%s\",%d,%d,\"%s\",%d,%d,\"%s\",%d,%d",
						 gMsgStorageStr[ getStorageInfoCnf->set.setting[CI_MSG_STRGOPERTYPE_READ_DELETE].storage ],
						 getStorageInfoCnf->set.setting[CI_MSG_STRGOPERTYPE_READ_DELETE].used,
						 getStorageInfoCnf->set.setting[CI_MSG_STRGOPERTYPE_READ_DELETE].total,
						 gMsgStorageStr[ getStorageInfoCnf->set.setting[CI_MSG_STRGOPERTYPE_WRITE_SEND].storage ],
						 getStorageInfoCnf->set.setting[CI_MSG_STRGOPERTYPE_WRITE_SEND].used,
						 getStorageInfoCnf->set.setting[CI_MSG_STRGOPERTYPE_WRITE_SEND].total,
						 gMsgStorageStr[ getStorageInfoCnf->set.setting[CI_MSG_STRGOPERTYPE_RECV].storage ],
						 getStorageInfoCnf->set.setting[CI_MSG_STRGOPERTYPE_RECV].used,
						 getStorageInfoCnf->set.setting[CI_MSG_STRGOPERTYPE_RECV].total );
					ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, (char *)AtRspBuf );
				}

				/* Response of +CMGD=? */
				else if ( GET_REQ_ID(reqHandle) == PRI_MSG_PRIM_QUERY_DEL_MESSAGE_REQ)
				{
					if(gMem1MsgNum == 0)
					{
						sprintf( (char *)AtRspBuf, "+CMGD: (),(0-4)");
						ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, (char *)AtRspBuf );
					}
					else
					{
						sprintf(gMsgValidIndex, "+CMGD: (");
						gCurrMsgIndex = 1;
						if(MSG_ReadMsg_(reqHandle, gCurrMsgIndex) != CIRC_SUCCESS)
						{
							ATRESP( reqHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
						}
					}
				}

				/* Response of +CMGL */
				else if ((GET_REQ_ID(reqHandle) >= PRI_MSG_PRIM_LIST_MESSAGE_REQ) && (GET_REQ_ID(reqHandle) < PRI_MSG_PRIM_LIST_MESSAGE_REQ + READ_SMS_OPTION_NUM))
				{
					/* Start reading the existed message from beginning */
					gCurrMsgIndex = 1;

					if(MSG_ReadMsg_(reqHandle, gCurrMsgIndex) != CIRC_SUCCESS)
					{
						ATRESP( reqHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
					}
				}
				/* Response of +CMGD=<index>,4*/
				else if(GET_REQ_ID(reqHandle) == PRI_MSG_PRIM_DEL_ALL_MESSAGE_REQ)
				{
					/* Start delete the existed message from beginning */
					gCurrMsgIndex = 1;

					if(MSG_DeleteMsg_(reqHandle, gCurrMsgIndex, CI_MSG_MSG_DELFLAG_INDEX) != CIRC_SUCCESS)
					{
						ATRESP( reqHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
					}
				}
				/* The received CI cnf msg is not matched with gCurrOperation, report error */
				else
				{
					ATRESP( reqHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
				}

				return;
			}

/****************************************************************
 *  F@: processSetPrefStorageConf - Process CI cnf msg of +CPMS=
 */
			static void processSetPrefStoragesConf(UINT32 atHandle, const void *paras)
			{
				CHAR AtRspBuf [500];
				CiMsgPrimSelectStoragesCnf  *selectStoragesCnf = (CiMsgPrimSelectStoragesCnf *)paras;

				if ( selectStoragesCnf->rc == CIRC_MSG_SUCCESS )
				{
					sprintf( (char *)AtRspBuf, "+CPMS: %d,%d,%d,%d,%d,%d",
						 selectStoragesCnf->set.setting[CI_MSG_STRGOPERTYPE_READ_DELETE].used,
						 selectStoragesCnf->set.setting[CI_MSG_STRGOPERTYPE_READ_DELETE].total,
						 selectStoragesCnf->set.setting[CI_MSG_STRGOPERTYPE_WRITE_SEND].used,
						 selectStoragesCnf->set.setting[CI_MSG_STRGOPERTYPE_WRITE_SEND].total,
						 selectStoragesCnf->set.setting[CI_MSG_STRGOPERTYPE_RECV].used,
						 selectStoragesCnf->set.setting[CI_MSG_STRGOPERTYPE_RECV].total );
					ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)AtRspBuf );
				}
				else
				{
					ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
				}
				return;
			}

/****************************************************************
 *  F@: processGetSmsCenterAddrConf - Process CI cnf msg of +CSCA?
 */
			static void processGetSmsCenterAddrConf(UINT32 atHandle, const void *paras)
			{
				CHAR AtRspBuf [500], TempBuf[300];
				CiMsgPrimGetSmscAddrCnf  *getSmsCenterAddrCnf = (CiMsgPrimGetSmscAddrCnf *)paras;

											//[Jerry, 2008/10/9] workround for CP. It seems the sca addr is valid, but the flag getSmsCenterAddrCnf->scaPresent is 0.
				if ( (getSmsCenterAddrCnf->rc == CIRC_MSG_SUCCESS ))    // && (getSmsCenterAddrCnf->scaPresent) )
				{
					const char *plus = NULL;
					if ( getSmsCenterAddrCnf->sca.AddrType.NumType == CI_NUMTYPE_INTERNATIONAL )
						plus = "+";
					else
						plus = "";

					memcpy( TempBuf, getSmsCenterAddrCnf->sca.Digits, getSmsCenterAddrCnf->sca.Length );
					TempBuf[ getSmsCenterAddrCnf->sca.Length ] = '\0';
					sprintf( (char *)AtRspBuf, "+CSCA: \"%s%s\",%d", plus, TempBuf,
						 DialNumTypeCI2GSM( getSmsCenterAddrCnf->sca.AddrType.NumType ) );
					ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)AtRspBuf );
				}
				else
				{
					ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
				}
				return;
			}

/****************************************************************
 *  F@: processSetSmsCenterAddrConf - Process CI cnf msg of +CSCA=
 */
			static void processSetSmsCenterAddrConf(UINT32 atHandle, const void *paras)
			{
				CiMsgPrimSetSmscAddrCnf  *setSmsCenterAddrCnf = (CiMsgPrimSetSmscAddrCnf *)paras;

				if (setSmsCenterAddrCnf->rc == CIRC_MSG_SUCCESS )
				{
					ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
				}
				else
				{
					ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
				}
				return;
			}

/****************************************************************
 *  F@: processResetMemcapFullConf - Process CI cnf msg of
 */
			static void processResetMemcapFullConf(UINT32 atHandle, const void *paras)
			{
				CiMsgPrimResetMemcapFullCnf  *resetMemcapFullCnf = (CiMsgPrimResetMemcapFullCnf *)paras;

				if (resetMemcapFullCnf->rc == CIRC_MSG_SUCCESS )
				{
					ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
				}
				else
				{
					ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
				}
				return;
			}


/****************************************************************
 *  F@: processQueryCbMsgTypeConf - Process CI cnf msg of +CSCB=?
 */
			static void processQueryCbMsgTypeConf(UINT32 atHandle, const void *paras)
			{
				CHAR AtRspBuf [500];
				UINT32 msgBitsRange[2], i;
				CiMsgPrimGetCbmTypesCapCnf  *getCBMTypesCapCnf = (CiMsgPrimGetCbmTypesCapCnf *)paras;

				if ( getCBMTypesCapCnf->rc == CIRC_MSG_SUCCESS )
				{
					/* Find type range */
					msgBitsRange[0] = -1;
					for ( i = 0; i < (INT32)(8 * sizeof(UINT32)); i++ )
					{
						if ( ((INT32)msgBitsRange[0] == -1) && (getCBMTypesCapCnf->bitsMode & 1) )
						{
							msgBitsRange[0] = i;
							msgBitsRange[1] = i;
						}
						else if ( getCBMTypesCapCnf->bitsMode & 1 )
						{
							msgBitsRange[1] = i;
						}
						getCBMTypesCapCnf->bitsMode >>=  1;
					}
					sprintf( (char *)AtRspBuf, "+CSCB:(%d-%d)", msgBitsRange[0], msgBitsRange[1] );
					ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)AtRspBuf );
				}
				else
				{
					ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
				}
				return;
			}

/****************************************************************
 *  F@: processGetCbMsgTypeConf - Process CI cnf msg of +CSCB?
 */
			static void processGetCbMsgTypeConf(UINT32 atHandle, const void *paras)
			{
				CHAR AtRspBuf [500];
				CiMsgPrimGetCbmTypesCnf  *getCBMTypesCnf = (CiMsgPrimGetCbmTypesCnf *)paras;

				if ( getCBMTypesCnf->rc == CIRC_MSG_SUCCESS )
				{
					sprintf( (char *)AtRspBuf, "+CSCB:%d,", !(getCBMTypesCnf->set.mode) ); /* difference in AT and CI definitions */
					PrintNumericList( &(getCBMTypesCnf->set.mids), AtRspBuf);
					strcat( (char *)AtRspBuf, "," );
					PrintNumericList( &(getCBMTypesCnf->set.dcss), AtRspBuf);
					ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)AtRspBuf );
				}
				else
				{
					ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
				}
				return;
			}

/****************************************************************
 *  F@: processSetCbMsgTypeConf - Process CI cnf msg of +CGSMS=
 */
			static void processSetCbMsgTypeConf(UINT32 atHandle, const void *paras)
			{
				CiMsgPrimSetCbmTypesCnf  *setCBMTypeCnf = (CiMsgPrimSetCbmTypesCnf *)paras;

				if ( setCBMTypeCnf->rc == CIRC_MSG_SUCCESS )
				{
					ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
				}
				else
				{
					ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
				}
				return;
			}

/****************************************************************
 *  F@: processSetNewMsgIndSettingConf - Process CI cnf msg of +CNMI=
 */
			static void processSetNewMsgIndSettingConf(UINT32 atHandle, const void *paras)
			{
				CiMsgPrimConfigMsgIndCnf  *setNewMsgIndSettingCnf = (CiMsgPrimConfigMsgIndCnf *)paras;

				if ( setNewMsgIndSettingCnf->rc == CIRC_MSG_SUCCESS )
				{
					gCurrNewMsgIndSetting = gTempNewMsgIndSetting;
					ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
				}
				else
				{
					ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
				}
				return;
			}

/****************************************************************
 *  F@: processReadMsgConf - Process CI cnf msg of +CMGR or +CMGL
 */
			static void processReadMsgConf(UINT32 reqHandle, const void *paras)
			{
				CiMsgPrimReadMessageCnf  *readMsgCnf = (CiMsgPrimReadMessageCnf *)paras;
				UINT32 atHandle = GET_AT_HANDLE(reqHandle);

				/* Response of +CMGR=<index> */
				if ( GET_REQ_ID(reqHandle) == CI_MSG_PRIM_READ_MESSAGE_REQ)
				{
					if ( readMsgCnf->rc == CIRC_MSG_SUCCESS )
					{
						msgReadCnfForAtReadMsg(atHandle, readMsgCnf );
					}
					else if ( readMsgCnf->rc == CIRC_MSG_INVALID_MEM_INDEX )
					{
						ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_INVALID_MEMORY_INDEX, NULL);
					}
					else
					{
						ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
					}
				}

				/* Response of +CMGL, need to read one by one */
				else if ( (GET_REQ_ID(reqHandle) >= PRI_MSG_PRIM_LIST_MESSAGE_REQ) && (GET_REQ_ID(reqHandle) < PRI_MSG_PRIM_LIST_MESSAGE_REQ + READ_SMS_OPTION_NUM))
				{
					if ( (readMsgCnf->rc == CIRC_MSG_SUCCESS) || (readMsgCnf->rc == CIRC_MSG_INVALID_MEM_INDEX) )
					{
						/* Read next msg, if all msg are listed, the function will return TRUE */
						msgReadCnfForAtListMsg(atHandle, readMsgCnf, readMsgCnf->rc, (CiMsgMsgStatus)(GET_REQ_ID(reqHandle) - PRI_MSG_PRIM_LIST_MESSAGE_REQ) );

						/* It was the last message, reset index.  */
						if ( gCurrMsgIndex == gMem1TotalNum)
						{
							ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);

							/* reset the read msgs counter for each confirmation */
							gCurrMsgIndex = 0;

						}
						/* It was not the last msg, continue to read */
						else
						{
							/* Update the read msgs counter for each confirmation */
							gCurrMsgIndex++;

							/* Read next record */
							if(MSG_ReadMsg_(reqHandle, gCurrMsgIndex) != CIRC_SUCCESS)
							{
								ATRESP( atHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
							}
						}
					}
					else
					{
						ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
					}
				}
				else if ( GET_REQ_ID(reqHandle) == PRI_MSG_PRIM_QUERY_DEL_MESSAGE_REQ)
				{
					char tmpBuf[10];
					if (  (readMsgCnf->rc == CIRC_MSG_SUCCESS) || (readMsgCnf->rc == CIRC_MSG_INVALID_MEM_INDEX) )
					{
						if(readMsgCnf->rc == CIRC_MSG_SUCCESS)
						{
							sprintf(tmpBuf, "%d,", gCurrMsgIndex);
							strcat(gMsgValidIndex, tmpBuf);
							gMem1MsgNum--;
						}
						/* It was the last message, reset index.  */
						if ( (gCurrMsgIndex == gMem1TotalNum) || (gMem1MsgNum == 0))
						{
							if(gMsgValidIndex[strlen(gMsgValidIndex) - 1] == ',')
								gMsgValidIndex[strlen(gMsgValidIndex) - 1] = '\0';
							strcat(gMsgValidIndex, "),(0-4)");
							ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, gMsgValidIndex);

							/* reset the read msgs counter for each confirmation */
							gCurrMsgIndex = 0;

						}
						/* It was not the last msg, continue to read */
						else
						{
							/* Update the read msgs counter for each confirmation */
							gCurrMsgIndex++;

							/* Read next record */
							if(MSG_ReadMsg_(reqHandle, gCurrMsgIndex) != CIRC_SUCCESS)
							{
								ATRESP( atHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
							}
						}
					}
					else
					{
						ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
					}
				}

				/* The received CI cnf msg is not matched with gCurrOperation, report error */
				else
				{
					ATRESP( atHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
				}

				return;
			}
#ifdef AT_CUSTOMER_HTC
/****************************************************************
 *  F@: libSmsSentErrorCause2CmsError - Mapping between unsuccessful rc and rpCause with CMS ERROR
 */
			static UINT16 libSmsSentErrorCause2CmsError(CiMsgRc rc, CiMsgCause rpCause, CiMsgCause tpCause)
			{
				UINT16 CmsError;
				if(rc == CIRC_MSG_FAIL && tpCause == 0x0100)
				{
					switch(rpCause)
					{
					case CAUSE_CALL_BARRED:
						CmsError = 0;
						break;
					case CAUSE_UNASSIGNED_NO:
					case CAUSE_OPER_DETERM_BARRING:
					case CAUSE_UNIDENTIFIED_SUBSCRIBER:
					case CAUSE_FACILITY_REJECTED:
					case CAUSE_UNKNOWN_SUBSCRIBER:
					case CAUSE_REQ_FAC_NOT_SUBSCR:
					case CAUSE_REQ_FACIL_NOT_IMPL:
						CmsError = CMS_UNKNOWN_ERROR;
						break;
					default:
						CmsError = CMS_NW_TIMEOUT;
						break;
					}
				}
				else
					CmsError = CMS_NW_TIMEOUT;
				return CmsError;
			}
#endif
/****************************************************************
 *  F@: processSendMsgConf - Process CI cnf msg of +CMGS
 */
			static void processSendMsgConf(UINT32 atHandle, const void *paras)
			{
				CHAR AtRspBuf [500];
				CiMsgPrimSendMessageCnf  *sendMsgCnf = (CiMsgPrimSendMessageCnf *)paras;

				if ( sendMsgCnf->rc == CIRC_MSG_SUCCESS )
				{
					sprintf( (char *)AtRspBuf, "+CMGS: %d", sendMsgCnf->reference );
					ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)AtRspBuf);
				}
				else
				{
					/* +CMS ERROR: <err> */
				#ifdef AT_CUSTOMER_HTC
					UINT16 CmsError;
					CmsError = libSmsSentErrorCause2CmsError(sendMsgCnf->rc, sendMsgCnf->rpCause, sendMsgCnf->tpCause);
					ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CmsError, NULL);
				#else
					ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
				#endif
				}
				return;
			}

/****************************************************************
 *  F@: processSendStoredMsgConf - Process CI cnf msg of +CMSS
 */
			static void processSendStoredMsgConf(UINT32 atHandle, const void *paras)
			{
				CiMsgPrimSendStoredMessageCnf   *sendStoredMsgCnf = (CiMsgPrimSendStoredMessageCnf *)paras;
				CHAR AtRspBuf [500];

				if ( sendStoredMsgCnf->rc == CIRC_MSG_SUCCESS )
				{
					sprintf( (char *)AtRspBuf, "+CMSS: %d\r\n", sendStoredMsgCnf->reference );
					ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)AtRspBuf);
				}
				else
				{
					/* +CMS ERROR: <err> */
				#ifdef AT_CUSTOMER_HTC
					UINT16 CmsError;
					CmsError = libSmsSentErrorCause2CmsError(sendStoredMsgCnf->rc, sendStoredMsgCnf->rpCause, sendStoredMsgCnf->tpCause);
					ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CmsError, NULL);
				#else
					ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
				#endif
				}
				return;
			}

/****************************************************************
 *  F@: processWriteMsgConf - Process CI cnf msg of +CMGW
 */
			static void processWriteMsgConf(UINT32 atHandle, const void *paras)
			{
				CiMsgPrimWriteMessageCnf   *writeMsgCnf = (CiMsgPrimWriteMessageCnf*)paras;
				CHAR AtRspBuf [500];

				if (writeMsgCnf->rc == CIRC_MSG_SUCCESS )
				{
					sprintf((char *)AtRspBuf, "+CMGW: %d", writeMsgCnf->index );
					ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)AtRspBuf );
				}
				else
				{
					ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
				}
				return;
			}

/****************************************************************
 *  F@: processDelMsgConf - Process CI cnf msg of +CMGD
 */
			static void processDelMsgConf(UINT32 reqHandle, const void *paras)
			{
				CiMsgPrimDeleteMessageCnf  *deleteMsgCnf = (CiMsgPrimDeleteMessageCnf *)paras;

				/* Response of AT command to delete one message or delete some kind of all messages for once */
				if ( GET_REQ_ID(reqHandle) == CI_MSG_PRIM_DELETE_MESSAGE_REQ )
				{
					if ( deleteMsgCnf->rc == CIRC_MSG_SUCCESS )
					{
						ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);
					}
					else
					{
						/* +CMS ERROR: <err> */
						ATRESP( reqHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
					}
				}

				/* Response of AT command to delete all messages one by one */
				else if ( GET_REQ_ID(reqHandle) == PRI_MSG_PRIM_DEL_ALL_MESSAGE_REQ)
				{
					/* The msg in mem1 hasn't been totally deleted, continue deleting. Because there may be empty space, we need to del all index */
					if ( gCurrMsgIndex < gMem1TotalNum)
					{
						/* Delete next msg */
						gCurrMsgIndex++;
						if(MSG_DeleteMsg_(reqHandle, gCurrMsgIndex, CI_MSG_MSG_DELFLAG_INDEX) != CIRC_SUCCESS)
						{
							ATRESP( reqHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
						}
					}

					/* All messages in mem1 have been deleted */
					else
					{
						gCurrMsgIndex = 0;
						ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);
					}
				}

				/* The received CI cnf msg is not matched with gCurrOperation, report error */
				else
				{
					ATRESP( reqHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
				}
				return;
			}

/****************************************************************
 *  F@: processQueryMoServiceConf - Process CI cnf msg of +CGSMS=?
 */
			static void processQueryMoServiceConf(UINT32 atHandle, const void *paras)
			{
				CHAR AtRspBuf [500];
				UINT32 msgBitsRange[2], i;
				CiMsgPrimGetMoSmsServiceCapCnf  *moSmsServiceCapCnf = (CiMsgPrimGetMoSmsServiceCapCnf *)paras;

				if ( moSmsServiceCapCnf->rc == CIRC_MSG_SUCCESS )
				{
					/* Find services range */
					msgBitsRange[0] = -1;
					for ( i = 0; i < (INT32)(8 * sizeof(UINT32)); i++ )
					{
						if ( ((INT32)msgBitsRange[0] == -1) && (moSmsServiceCapCnf->bitsMoSmsSrvCfg & 1) )
						{
							msgBitsRange[0] = i;
							msgBitsRange[1] = i;
						}
						else if ( moSmsServiceCapCnf->bitsMoSmsSrvCfg & 1 )
						{
							msgBitsRange[1] = i;
						}
						moSmsServiceCapCnf->bitsMoSmsSrvCfg >>= 1;
					}

					sprintf( (char *)AtRspBuf, "+CGSMS:(%d-%d)", msgBitsRange[0], msgBitsRange[1] );
					ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)AtRspBuf);
				}
				else
				{
					/* +CMS ERROR: <err> */
					ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
				}
				return;
			}

/****************************************************************
 *  F@: processGetMoServiceConf - Process CI cnf msg of +CGSMS?
 */
			static void processGetMoServiceConf(UINT32 atHandle, const void *paras)
			{
				CHAR AtRspBuf [500];
				CiMsgPrimGetMoSmsServiceCnf  *getMoServiceCnf = (CiMsgPrimGetMoSmsServiceCnf *)paras;

				if ( getMoServiceCnf->rc == CIRC_MSG_SUCCESS )
				{
					sprintf( (char *)AtRspBuf, "+CGSMS:%d", getMoServiceCnf->cfg );
					ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)AtRspBuf);
				}
				else
				{
					/* +CMS ERROR: <err> */
					ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
				}
				return;
			}

/****************************************************************
 *  F@: processSetMoServiceConf - Process CI cnf msg of +CGSMS=
 */
			static void processSetMoServiceConf(UINT32 atHandle, const void *paras)
			{
				CiMsgPrimSetMoSmsServiceCnf  *setMoServiceCnf = (CiMsgPrimSetMoSmsServiceCnf *)paras;

				if ( setMoServiceCnf->rc == CIRC_MSG_SUCCESS )
				{
					ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
				}
				else
				{
					ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
				}
				return;
			}

/****************************************************************************************
*
*  DESCRIPTION: Callback function when receiving response to SMS related CI request
*
****************************************************************************************/
			void msgCnf(CiSgOpaqueHandle opSgCnfHandle,
				    CiServiceGroupID svgId,
				    CiPrimitiveID primId,
				    CiRequestHandle reqHandle,
				    void*             paras)
			{
				UNUSEDPARAM(opSgCnfHandle)

				/* By default, the gCurrOperation will be set as AT_MSG_OPERATION_NULL unless this flag is set as FALSE manually */
				BOOL bProcessed = FALSE;
				UINT32 atHandle = GET_AT_HANDLE(reqHandle);

				/* Validate the para pointer */
				if ( (paras == NULL) && (primId < CI_MSG_PRIM_LAST_COMMON_PRIM) )
				{
					ERRMSG("para pointer of CI cnf msg is NULL!");
					return;
				}
#if 0
				{
					int i;
					char *p = paras;
					printf("\n#####[Enter %s][Prim=%d,gCurrOperation=%d] Data: ", __FUNCTION__, primId, gCurrOperation);
					for (i = 0; i < 30; i++)
						printf("%02x", *p++);
					printf("\n");
				}
#endif

				if (GET_CRSM_FLAG(reqHandle) == TRUE)
				{
					bProcessed = SIM_ConvertCrsmAtReply(atHandle, primId, paras);
				}

				if (!bProcessed)
				{
					switch (primId)
					{
					case CI_MSG_PRIM_DELETE_MESSAGE_CNF:
						{
							processDelMsgConf(reqHandle, paras);
							break;
						}

					case CI_MSG_PRIM_WRITE_MESSAGE_CNF:
						{
							processWriteMsgConf(atHandle, paras);
							break;
						}

					case CI_MSG_PRIM_SEND_STORED_MESSAGE_CNF:
						{
							processSendStoredMsgConf(atHandle, paras);
							break;
						}

					case CI_MSG_PRIM_SEND_MESSAGE_CNF:
						{
							processSendMsgConf(atHandle, paras);
							break;
						}

					case CI_MSG_PRIM_READ_MESSAGE_CNF:
						{
							processReadMsgConf(reqHandle, paras);
							break;
						}

					case CI_MSG_PRIM_GET_CURRENT_STORAGE_INFO_CNF:
						{
							processGetStorageInfoConf(reqHandle, paras);
							break;
						}

					case CI_MSG_PRIM_GET_MOSMS_SERVICE_CAP_CNF:
						{
							processQueryMoServiceConf(atHandle, paras);
							break;
						}

					case CI_MSG_PRIM_GET_MOSMS_SERVICE_CNF:
						{
							processGetMoServiceConf(atHandle, paras);
							break;
						}

					case CI_MSG_PRIM_SET_MOSMS_SERVICE_CNF:
						{
							processSetMoServiceConf(atHandle, paras);
							break;
						}

					case CI_MSG_PRIM_GET_CBM_TYPES_CAP_CNF:
						{
							processQueryCbMsgTypeConf(atHandle, paras);
							break;
						}

					case CI_MSG_PRIM_GET_CBM_TYPES_CNF:
						{
							processGetCbMsgTypeConf(atHandle, paras);
							break;
						}

					case CI_MSG_PRIM_SET_CBM_TYPES_CNF:
						{
							processSetCbMsgTypeConf(atHandle, paras);
							break;
						}

					case CI_MSG_PRIM_CONFIG_MSG_IND_CNF:
						{
							processSetNewMsgIndSettingConf(atHandle, paras);
							break;
						}

					case CI_MSG_PRIM_GET_SUPPORTED_SERVICES_CNF:
						{
							processQuerySmsServiceConf(atHandle, paras);
							break;
						}

					case CI_MSG_PRIM_GET_CURRENT_SERVICE_INFO_CNF:
						{
							processGetSmsServiceConf(atHandle, paras);
							break;
						}

					case CI_MSG_PRIM_SELECT_SERVICE_CNF:
						{
							processSetSmsServiceConf(atHandle, paras);
							break;
						}

					case CI_MSG_PRIM_GET_SUPPORTED_STORAGES_CNF:
						{
							processQuerySupportStorageConf(atHandle, paras);
							break;
						}

					case CI_MSG_PRIM_SELECT_STORAGES_CNF:
						{
							processSetPrefStoragesConf(atHandle, paras);
							break;
						}

					case CI_MSG_PRIM_GET_SMSC_ADDR_CNF:
						{
							processGetSmsCenterAddrConf(atHandle, paras);
							break;
						}

					case CI_MSG_PRIM_SET_SMSC_ADDR_CNF:
						{
							processSetSmsCenterAddrConf(atHandle, paras);
							break;
						}

					case CI_ERR_PRIM_HASNOSUPPORT_CNF:
					case CI_ERR_PRIM_HASINVALIDPARAS_CNF:
						{
							ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_INVALID_PDU_MODE_PARA, NULL );
							break;
						}
					case CI_MSG_PRIM_RESET_MEMCAP_FULL_CNF:
						{
							processResetMemcapFullConf(atHandle, paras);
							break;
						}

					default:
						break;
					}
				}

				/* Free up the confirmation memory */
				atciSvgFreeCnfMem( svgId, primId, paras );

				return;
			}

/****************************************************************
 *  F@: processNewMsgIndexInd - Process CI ind msg of +CMTI:
 */
			static void processNewMsgIndexInd(const void *pParam)
			{
				char AtRspBuf [500], storageStr[20];
				CiMsgPrimNewMsgIndexInd    *newMsgIndexInd = (CiMsgPrimNewMsgIndexInd*)pParam;

				if ( (newMsgIndexInd->storage > 0)  && (newMsgIndexInd->storage < CI_MSG_NUM_STORAGES) )
				{
					strcpy(storageStr, gMsgStorageStr[newMsgIndexInd->storage]);
				}
				else
				{
					sprintf(storageStr, "UNKNOWN");
				}
				sprintf( (char *)AtRspBuf, "+CMTI: \"%s\",%d\r\n", storageStr, newMsgIndexInd->index );
				ATRESP( IND_REQ_HANDLE, 0, 0, (char *)AtRspBuf );

				return;
			}

/****************************************************************
 *  F@: processNewMsgInd - Process CI ind msg of +CBM: / +CDS:  / +CMT:
 *** Now just used to send AT cmd  to RIL, its other functions are implemented in other places.
 */
			static void processNewMsgInd(const void *pParam, const int indHandle)
			{
				UNUSEDPARAM(indHandle)

				CiMsgPrimNewMsgInd   *newMsgInd = (CiMsgPrimNewMsgInd*)pParam;
				INT32 i;
				char TempBuf[3];
				int AtRspBufMaxlen = newMsgInd->pdu.len * 2 + 30 + CI_MAX_ADDRESS_LENGTH * 2 + ATCI_NULL_TERMINATOR_LENGTH;
				char *AtRspBuf = (char *)utlMalloc( AtRspBufMaxlen);

				char *pduBuf = (char *)utlMalloc( newMsgInd->pdu.len * 2 + 10);
				//static CiMsgPrimNewMsgRsp newMsgRsp; /*Michal Bukai - Add support for MT SMS handle in case of memory full*/
				char smscBuf[CI_MAX_ADDRESS_LENGTH * 2 + ATCI_NULL_TERMINATOR_LENGTH];
				UINT16 smscLen = 0;
				INT32 AtRspBufLen = 0;

#if 0
				/*Michal Bukai - send response in case of MSG_STORE_MSG_TYPE*/
				if ( newMsgInd->msgType == MSG_STORE_MSG_TYPE )
				{
					if ( g_iNewMsgAck == 0 )
					{
						newMsgRsp.success = TRUE;
						newMsgRsp.memory_full = FALSE;
					}
					else if ( g_iNewMsgAck == 1 )
					{
						newMsgRsp.success = FALSE;
						newMsgRsp.memory_full = TRUE;
					}
					else if ( g_iNewMsgAck == 2 )
					{
						newMsgRsp.success = FALSE;
						newMsgRsp.memory_full = FALSE;
					}

					/*build response parameters acording to indication parameters*/
					newMsgRsp.commandRef = newMsgInd->commandRef;
					newMsgRsp.ShortMsgId = newMsgInd->ShortMsgId;
					newMsgRsp.msgType = newMsgInd->msgType;

					/*sent response*/
					ciRespond(gAtciSvgHandle[CI_SG_ID_MSG], CI_MSG_PRIM_NEWMSG_RSP, indHandle, (void *)&newMsgRsp);
				}
#endif
				if (gSmsFormatMode == ATCI_SMS_FORMAT_PDU_MODE)
				{
					DPRINTF("processNewMsgInd , type is SMS PDU mode");

					memset(pduBuf, 0x00, (newMsgInd->pdu.len * 2 + 10));
					memset(smscBuf, 0x00, sizeof(smscBuf));
					memset(AtRspBuf, 0x00, AtRspBufMaxlen);

					/* store the smsc address in smscBuf */
					libPutPackedSMSC(newMsgInd->optSca, (char *)smscBuf, (UINT16 *)&smscLen);

					/* TPDU */
					for ( i = 0; i < newMsgInd->pdu.len; i++ )
					{
						sprintf((char *)TempBuf, "%02x", newMsgInd->pdu.data[i]);
						strcat(pduBuf, TempBuf);
					}

					switch (newMsgInd->type)
					{
					case CI_MSG_MSG_TYPE_CBS:
						{
							/* Broadcast message */
							/* Prefix Command string */
							sprintf( (char *)AtRspBuf, "+CBM: %d\r\n", newMsgInd->pdu.len);
							AtRspBufLen = strlen((char *)AtRspBuf);

							memcpy( &AtRspBuf[AtRspBufLen], smscBuf, (smscLen * 2) );
							AtRspBufLen += (smscLen * 2);
							memcpy( &AtRspBuf[AtRspBufLen], pduBuf, (newMsgInd->pdu.len * 2) );
							AtRspBufLen += (newMsgInd->pdu.len * 2);

							ATRESP( IND_REQ_HANDLE, 0, 0, (char *)AtRspBuf );
							break;
						}

					case CI_MSG_MSG_TYPE_STATUS_REPORT:
						{
							/* Status report */
							sprintf( (char *)AtRspBuf, "+CDS: %d\r\n", newMsgInd->pdu.len);
							AtRspBufLen = strlen((char *)AtRspBuf);
							memcpy( &AtRspBuf[AtRspBufLen], smscBuf, (smscLen * 2) );
							AtRspBufLen += (smscLen * 2);
							memcpy( &AtRspBuf[AtRspBufLen], pduBuf, (newMsgInd->pdu.len * 2) );
							AtRspBufLen += (newMsgInd->pdu.len * 2);

							ATRESP( IND_REQ_HANDLE, 0, 0, (char *)AtRspBuf );
							break;
						}

					default:
						{
							/* SMS (PDU = SCA + TPDU from CP msg)*/
							sprintf( (char *)AtRspBuf, "+CMT:,%d\r\n", newMsgInd->pdu.len);
							AtRspBufLen = strlen((char *)AtRspBuf);
							memcpy( &AtRspBuf[AtRspBufLen], smscBuf, (smscLen * 2) );
							AtRspBufLen += (smscLen * 2);
							memcpy( &AtRspBuf[AtRspBufLen], pduBuf, (newMsgInd->pdu.len * 2) );
							AtRspBufLen += (newMsgInd->pdu.len * 2);

							ATRESP( IND_REQ_HANDLE, 0, 0, (char *)AtRspBuf );
							break;
						}
					}
				}
				else if (gSmsFormatMode == ATCI_SMS_FORMAT_TXT_MODE)
				{
					if ( (newMsgInd->pdu.data[0] & ATCI_TP_MTI_MASK)  == ATCI_SMS_DELIVER_MTI)
					{
						sprintf( AtRspBuf, "+CMT: ");
						readSmsDeliverPdu( IND_REQ_HANDLE, (CHAR *)AtRspBuf, &(newMsgInd->pdu) );
					}
					else
					{
						/* COMMAND or STATUS REPORT not supported */
						ATRESP( IND_REQ_HANDLE, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
					}
				}

				utlFree(AtRspBuf);
				utlFree(pduBuf);

				/* Count the indication msg needed to be acknowledged by +CNMA */
#if 0
				if ((newMsgInd->type == CI_MSG_MSG_TYPE_DELIVER) || (newMsgInd->type == CI_MSG_MSG_TYPE_STATUS_REPORT))
				{
					/* Save CI ind handle, which will be used when sending response msg */
					gMtMsgIndHandle = indHandle;
					gNewMsgAckCounter++;
				}
#endif
				return;
			}

/****************************************************************
 *  F@: processStorageStatusInd - Process CI ind msg of CI_MSG_PRIM_STORAGE_STATUS_IND
 */
			static void processStorageStatusInd(const void *pParam)
			{
				char AtIndBuf [500];
				CiMsgPrimStorageStatusInd  *msgMemStatInd;

				/* Update storage info. Refer to comments of CiMsgPrimStorageStatusInd definition for more detail */
				msgMemStatInd = (CiMsgPrimStorageStatusInd *)pParam;

				gMemCapExceeded = msgMemStatInd->memCapExceeded;
				sprintf(AtIndBuf, "+MMSG: %d, %d\r\n", 1, gMemCapExceeded == TRUE ? 1 : 0);
				ATRESP( IND_REQ_HANDLE, 0, 0, (char *)AtIndBuf );

				return;
			}
#ifdef AT_CUSTOMER_HTC
/****************************************************************
 *  F@: processSmsRejectCauseInd - Process CI ind msg of CI_MSG_PRIM_SMS_REJECT_CAUSE_IND
 */
			static void processSmsRejectCauseInd(const void *pParam)
			{
				char AtIndBuf [500];
				CiMsgPrimSmsRejectCauseInd  *msgPrimSmsRejectCauseInd;
				CiMsgRejectType msgRejectType;

				msgPrimSmsRejectCauseInd = (CiMsgPrimSmsRejectCauseInd *)pParam;

				switch(msgPrimSmsRejectCauseInd->type)
				{
				case  CI_MSG_NONE:
					msgRejectType = ATCI_HTC_ERROR_CLASS_SMS_REJECT;
					break;
				case  CI_MSG_CP_ERROR:
					msgRejectType = ATCI_HTC_ERROR_CLASS_SMS_REJECT_CP_ERROR;
					break;
				case  CI_MSG_RP_ERROR:
					msgRejectType = ATCI_HTC_ERROR_CLASS_SMS_REJECT_RP_ERROR;
					break;
				default:
					ERRMSG("invalid SMS reject cause type %d", msgPrimSmsRejectCauseInd->type);
					return;
				}
				sprintf(AtIndBuf, "%s: %X,%X\r\n", SPEC_ERROR_CODE_INDICATION_PREFIX, msgRejectType, msgPrimSmsRejectCauseInd->smsCause);
				ATRESP( IND_REQ_HANDLE, 0, 0, AtIndBuf );

				return;
			}
#endif
/****************************************************************************************
*
*  DESCRIPTION: Callback function when receiving indication CI message related with SMS
*
****************************************************************************************/
			void msgInd(CiSgOpaqueHandle opSgIndHandle,
				    CiServiceGroupID svgId,
				    CiPrimitiveID primId,
				    CiIndicationHandle indHandle,
				    void*               pParam)
			{
				UNUSEDPARAM(opSgIndHandle)

#if 0
				{
					int i;
					char *p = pParam;

					printf("\n#####[Enter %s][Prim=%d,gCurrOperation=%d] Data: ", __FUNCTION__, primId, gCurrOperation);
					for (i = 0; i < 30; i++)
						printf("%02x", *p++);
					printf("\n");
				}
#endif


				switch (primId)
				{
				case CI_MSG_PRIM_STORAGE_STATUS_IND:
					{
						processStorageStatusInd(pParam);
						break;
					}

				case CI_MSG_PRIM_NEWMSG_INDEX_IND:
					{
						processNewMsgIndexInd(pParam);
						break;
					}

				case CI_MSG_PRIM_NEWMSG_IND:
					{
						__processNewMsgInd(pParam, indHandle);
						break;
					}
#ifdef AT_CUSTOMER_HTC
				case CI_MSG_PRIM_SMS_REJECT_CAUSE_IND:
					{
						processSmsRejectCauseInd(pParam);
						break;
					}
#endif
				case CI_MSG_PRIM_MESSAGE_WAITING_IND:
				default:
					break;
				}


				/* Free up the indication memory */
				atciSvgFreeIndMem( svgId, primId, pParam );

				return;
			}

/********************************************************************
 pre-process  Process CI ind msg of +CBM: / +CDS:  / +CMT:
 For +CDS:  / +CMT: the msg is added to a queue buffer to wait the response of RIL
**********************************************************************/
static void __processNewMsgInd(const void *pParam, const int indHandle)
{
	CiMsgPrimNewMsgInd   *newMsgInd = (CiMsgPrimNewMsgInd*)pParam;
	if (newMsgInd->type == CI_MSG_MSG_TYPE_CBS )
	{
		processNewMsgInd(pParam,  indHandle);
		return ;
	}
	pthread_mutex_lock(&msg_requests.buf_mutx); //get the msg queue lock

	pthread_mutex_lock(&msg_ack.ack_mutx); //get the ack lock
	if(msg_ack.ack)
	{
		DPRINTF("Get ACK : %d, Report to RIL directly!\n",msg_ack.ack);
		processNewMsgInd(pParam,  indHandle);
		msg_ack.ack--;	//consume ack;
	}

	pthread_mutex_unlock(&msg_ack.ack_mutx); //release the ack lock

	struct msg_request_list * new_msg_ptr = (struct msg_request_list *)utlMalloc(sizeof(struct msg_request_list)); //malloc, rember to free!

	void * newpParam = utlMalloc(sizeof(CiMsgPrimNewMsgInd)); //malloc, rember to free!
	memset(newpParam, 0, sizeof(CiMsgPrimNewMsgInd));
	memcpy(newpParam, pParam, sizeof(CiMsgPrimNewMsgInd)); //Notice, CiMsgPrimNewMsgInd without pointer, or it may need deep clone!

	new_msg_ptr->next= NULL;
	new_msg_ptr ->pParam =newpParam;
	new_msg_ptr ->indHandle = indHandle;

	struct msg_request_list * cur_msg_ptr = &msg_requests.msg_list_head;  //Inqueue, add to the tail.
	DPRINTF("Add SMS Ind to the buff\n");
	while(cur_msg_ptr ->next != NULL)
	{
		cur_msg_ptr = cur_msg_ptr->next;
	}
	cur_msg_ptr->next = new_msg_ptr;
	msg_requests.buf_count ++;

	pthread_mutex_unlock(&msg_requests.buf_mutx); // release the msg queue lock
}
/******************************************
*  FUNCTION:  MSG_NewMsgAck
*
*  PARAMETERS:
*
*  DESCRIPTION: Interface to do new message acknowledgement, implementation of AT+CNMA (txt mode) or *CNMA=<n> (PDU mode)
*
*  RETURNS: CiReturnCode
***************************************************/
CiReturnCode __MSG_NewMsgAck(UINT32 atHandle, UINT32 reply)
{
	pthread_mutex_lock(&msg_requests.buf_mutx); //get the msg queue lock
	if(msg_requests.buf_count)				//if the queues is not empty
	{
		DPRINTF("BUFF is not empty :%d, Get out to send to CP\n", msg_requests.buf_count);
		CiMsgPrimNewMsgInd   *newMsgInd = NULL;
		CiMsgPrimNewMsgRsp newMsgRsp;
		int indHandle;

		struct msg_request_list *cur_msg_ptr = &msg_requests.msg_list_head; //dequeue
		cur_msg_ptr = cur_msg_ptr->next;
		msg_requests.msg_list_head.next = cur_msg_ptr->next;
		msg_requests.buf_count--;

		newMsgInd = (CiMsgPrimNewMsgInd *)cur_msg_ptr->pParam;	//get the data
		indHandle = cur_msg_ptr->indHandle;

		if ( newMsgInd->msgType == MSG_STORE_MSG_TYPE )
		{
			if (reply == 0 )
			{
				newMsgRsp.success = TRUE;
				newMsgRsp.memory_full = FALSE;
			}
			else if ( reply == 1 )
			{
				newMsgRsp.success = FALSE;
				newMsgRsp.memory_full = TRUE;
			}
			else if ( reply == 2 )
			{
				newMsgRsp.success = FALSE;
				newMsgRsp.memory_full = FALSE;
			}
			newMsgRsp.commandRef = newMsgInd->commandRef;
			newMsgRsp.ShortMsgId = newMsgInd->ShortMsgId;
			newMsgRsp.msgType = newMsgInd->msgType;
			DPRINTF("Send response to CP:%d\n", reply);
			ciRespond(gAtciSvgHandle[CI_SG_ID_MSG], CI_MSG_PRIM_NEWMSG_RSP, indHandle, (void *)&newMsgRsp); //send to CP
		}
		utlFree(cur_msg_ptr->pParam);
		utlFree(cur_msg_ptr);  //free the node

		if(msg_requests.buf_count)
		{
			DPRINTF("BUFF is not empty :%d, Sent to RIL for next mesg\n", msg_requests.buf_count);
			struct msg_request_list * next_msg_ptr = msg_requests.msg_list_head.next;
			processNewMsgInd(next_msg_ptr->pParam, next_msg_ptr ->indHandle);
		}
		else
		{
			pthread_mutex_lock(&msg_ack.ack_mutx);
			msg_ack.ack++;					// product ack
			pthread_mutex_unlock(&msg_ack.ack_mutx);
		}
	}
	else   //normally, cannot goto this branch
	{
		pthread_mutex_unlock(&msg_requests.buf_mutx); // release the buff lock
		return  ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_NO_CNMA_ACK_EXPECTED, NULL);
	}

	pthread_mutex_unlock(&msg_requests.buf_mutx); // release the buff lock

	return ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
}
