/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
 *  INTEL CONFIDENTIAL
 *  Copyright 2006 Intel Corporation All Rights Reserved.
 *  The source code contained or described herein and all documents related to the source code (“Material? are owned
 *  by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
 *  its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
 *  Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
 *  treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
 *  transmitted, distributed, or disclosed in any way without Intel’s prior express written permission.
 *
 *  No license under any patent, copyright, trade secret or other intellectual property right is granted to or
 *  conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
 *  estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
 *  Intel in writing.
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: telatci.h
 *
 *  Description:  Telephony AT2CI utilities header file
 *
 *  History:
 *   May 19, 2006 - Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/

/*------------ External include files ---------------------------------------*/
/*------------ Local include definitions ------------------------------------*/
/*--------- Local defines ---------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*-------- Local macro definitions ------------------------------------------*/
/*-------- Local type definitions -------------------------------------------*/
/*--------- Global constants definitions ------------------------------------*/
/*--------- Local constants definitions -------------------------------------*/
/*--------- Global variables declarations ------------------------------------*/
/*-- local function prototypes   --------------------------------------------*/


#ifndef TELATCIUTL_H
#define TELATCIUTL_H

/* General includes */

//#include "gbl_types.h"
#include "ci_cc.h"
#include "ci_ps.h"
#include "ci_dat.h"
#include "osa.h"
#include "teldef.h"

#define CI_PS_3G_ENUM_LOOKUP_END_ELEMENT        (0xFF) //vcy: due to INT16, old value: (0xFFFF)
#define ERROR_RATIO_STRING_LEN                          (10)
#define SUBSCRIBED_VALUE                                (0)
#define MAX_BITRATE                                     (8640)
#define MAX_SDU_SIZE                                    (1520)
#define MAX_TRANSFER_DELAY                              (4000)
#define MCC_DIGIT_LENGTH                                (3)
#define MNC_DIGIT_LENGTH_MIN                            (2)
#define MNC_DIGIT_LENGTH_MAX                            (3)
#define DEFAULT_TGSEND_DATA_SIZE                        (1500)

#define TEL_GPRS_MIN_DIAL_STRING_SIZE                   (3)
#define NUM_GPRS_SC_DIGITS                      (2)
#define TEL_GPRS_SC_DIG_1                               ('9')
#define TEL_GPRS_SC_DIG_2                               TEL_GPRS_SC_DIG_1

#define TEL_GSM_SC_DIG_1                          ('8')
#define TEL_GSM_SC_DIG_2                        GSM_SC_DIG_1


#define NUM_ENTRIES(X) (sizeof(X) / sizeof(X[0]))

#define ATCI_DEFAULT_PARAM_VALUE      255

#define ATCI_IP_MTU                   1500      /* Maximum Transmission Unit over GPRS */
#define ATCI_IP_LEN                   20        /* IP  header Length */
#define ATCI_UDP_LEN                  8         /* UDP header Length */
#define ATCI_TCP_LEN                  20        /* TCP header Length */

#define ATCI_FIRST_CID                0
#define ATCI_DEFAULT_CID              ATCI_FIRST_CID

#define ATCI_MAX_PACKET_SIZE          10000
#define ATCI_MAX_PDU_SIZE             ATCI_IP_MTU - ATCI_IP_LEN - ATCI_UDP_LEN
#define ATCI_DEFAULT_PDU_SIZE         ATCI_MAX_PDU_SIZE
#define ATCI_DEFAULT_PDU_COUNT        20
#define ATCI_DEFAULT_UDP_PORT         9
#define ATCI_PS_ADDRESS_SIZE          40
#define ATCI_MAX_PPP_FRAME_SIZE       180
#define ATCI_MAX_FRAME_SIZE           180


#if !defined (min)
# define min(a, b)    (((a) < (b)) ? (a) : (b))
#endif

#define ATCI_MAX_CMDSTR_LENGTH       40  /* maximum characters per AT command line */

#define ATCI_NULL_TERMINATOR_LENGTH   1
#define ATCI_FACILITY_STR_LENGTH      2

/* PDP data compression, only applicable to SNDCP */
typedef enum  ATCI_PSDCOMP
{
	ATCI_PS_DCOMP_OFF = 0,  /* off, default */
	ATCI_PS_DCOMP_ON,       /* on, manufacturer preferred compression */
	ATCI_PS_DCOMP_V42bis,   /* V.42 bis */

	ATCI_PS_DCOMP_DCOMPS
} _ATCIPsDcomp;

/* PDP header compression */
typedef enum ATCIPSHCOMP
{
	ATCI_PS_HCOMP_OFF = 0,  /* off, default */
	ATCI_PS_HCOMP_TCPIP,  /* RFC 1144 */ // ON(1)  mode in Spec doesn't supported by CI definition.
	ATCI_PS_HCOMP_IP,       /* RFC 2507 */

	ATCI_PS_NUM_HCOMPS
} _ATCiPsHcomp;
/*
   <h_comp>: a numeric parameter that controls PDP header compression (refer 3GPP TS 04.65 [59])
   0 ¨C off (default if value is omitted)
   1 ¨C on (manufacturer preferred compression)
   2 ¨C RFC1144
   3 ¨C RFC2507
   Other values are reserved.
 */
#if 0
#define ATCI_PS_DCOMP_OFF             0
#define ATCI_PS_DCOMP_ON              1
#define ATCI_PS_DCOMP_V42bis        2 // CI_PS_DCOMP_V42bis

#define ATCI_PS_HCOMP_OFF             0
#define ATCI_PS_HCOMP_TCPIP           1
#define ATCI_PS_HCOMP_IP              2
#endif

#define ATCI_PB_STORAGE_STR_LENGTH    4
#define ATCI_MSG_STORAGE_STR_LENGTH   2
#define ATCI_PB_INVALID_INDEX 0 //-1  [Jerry] 0 is invalid PBK ID
/* Max long name length is 16 according to 3GPP 27.007 */
#define ATCI_PLMN_LONG_NAME_LENGTH 16
#define ATCI_PLMN_SHORT_NAME_LENGTH 8

#define ATCI_MAX_INDV_LIST_SIZE             20
#define ATCI_MAX_CI_NUMERIC_RANGE_LIST_SIZE 20

#define ATCI_MAX_QUEUE_SIZE             10

#define ATCI_MAX_SMS_TDPU_SIZE            (232)

#define ATCI_SMS_DELIVER_MTI           ( (UINT8)0x00 )
#define ATCI_SMS_SUBMIT_MTI            ( (UINT8)0x01 )
#define ATCI_SMS_COMMAND_MTI           ( (UINT8)0x02 )
#define ATCI_SMS_STATUS_REPORT_MTI     ( (UINT8)0x02 )

#define ATCI_SMS_TP_RP       0x80       /* TP-RP, reply path */
#define ATCI_SMS_TP_UDHI     0x40       /* TP User Data Header Indicator  */
#define ATCI_SMS_TP_SRI      0x20       /* TP Status Report Indication */
#define ATCI_SMS_TP_SRR      0x20       /* TP Status Report Request */
#define ATCI_SMS_TP_SRQ      0x20       /* TP Status Report Qualifier */
#define ATCI_SMS_TP_VPF      0x18       /* TP Validity Period Format */
#define ATCI_SMS_TP_MMS      0x04       /* TP More Messages to Send */
#define ATCI_SMS_TP_RD       0x04       /* TP Reject Duplicates */

#define ATCI_TP_MTI_MASK     0x03       /* SMS Message-Type Indicator mask */
#define ATCI_BCD_LOW_MASK  0x0F
#define ATCI_BCD_HIGH_MASK 0xF0
#define ATCI_BCD_FILLER    0x0F
#define ATCI_SMS_BCD_POS   2

#define ATCI_SMS_MAX_MSG_LEN             160


#define ATCI_MSG_SUBMIT_FO_DEFAULT  ((UINT8)0x21)       /* Default first octet for SMS-SUBMIT PDU, status report required */ /* NOTE: Voyager cod suggests 17, but17 is NOT SMS-SUBMIT (?) */
#define ATCI_MSG_VP_DEFAULT         ((UINT8)167)        /* 24 hours - see 3GPP TS 23.040 */
#define ATCI_MSG_PID_DEFAULT          ((UINT8)0)        /* see 3GPP TS 23.040 */
#define ATCI_MSG_DCS_DEFAULT          ((UINT8)0)        /* should be 246 (0xF6), but SAC does not support 8-bit alphabet */
#define ATCI_MSG_REF_DEFAULT          ((UINT8)0)        /* Reference=0 means that msg ref is set in SAC */
#define ATCI_MSG_UDH_PRESENT          ((UINT8)1)
#define ATCI_MSG_UDH_NOT_PRESENT      ((UINT8)0)

enum _AtciFacilityTypeTag
{
	ATCI_SS_FAC_MIN = 0,
	ATCI_SS_FAC_CS = ATCI_SS_FAC_MIN,
	ATCI_SS_FAC_PS,
	ATCI_SS_FAC_PF,
	ATCI_SS_FAC_SC,

	ATCI_SS_FAC_AO,
	ATCI_SS_FAC_OI,
	ATCI_SS_FAC_OX,
	ATCI_SS_FAC_AI,

	ATCI_SS_FAC_IR,
	ATCI_SS_FAC_AB,
	ATCI_SS_FAC_AG,
	ATCI_SS_FAC_AC,

	ATCI_SS_FAC_FD,

	ATCI_SS_FAC_NT,
	ATCI_SS_FAC_NM,
	ATCI_SS_FAC_NS,
	ATCI_SS_FAC_NA,

	ATCI_SS_FAC_PN,
	ATCI_SS_FAC_PU,
	ATCI_SS_FAC_PP,
	ATCI_SS_FAC_PC,

	/* Pin2 must ALWAYS be last! */
	ATCI_SS_FAC_P2,

	ATCI_SS_FAC_MAX

};
typedef UINT8 AtciFacilityType;


typedef struct _AtciFacilityDescriptorStruct
{
	AtciFacilityType facTypeId;
	CHAR facTypeStr[ATCI_FACILITY_STR_LENGTH + ATCI_NULL_TERMINATOR_LENGTH];
	INT8 pswdLen;
} AtciFacilityDescriptor;

/*Michal Buaki - Modify this Struct for MEP&UDP support. Facility will divided to 3 logic group: CB, MEP, and all other*/
typedef struct _AtciFacTypeStruct
{
	UINT8 facLogicGroup;
	UINT8 facId;
} AtciFacType;


/* Modes for call forwarding (+CCFC) */
typedef enum ATCI_SS_CCFC_MODE {
	ATCI_SS_MODE_DISABLE = 0,
	ATCI_SS_MODE_ENABLE,
	ATCI_SS_MODE_QUERY_STATUS,
	ATCI_SS_MODE_REGISTER,
	ATCI_SS_MODE_ERASE,
	ATCI_SS_NUM_MODES
} _AtciSsCcfcMode;
typedef UINT8 AtciSsCcfcMode;
/*Michal Buaki - Add this enum for MEP&UDP support. Facility will divided to 3 logic group: CB, MEP, and all other*/
typedef enum ATCI_FAC_LOGIC_GROUP {
	ATCI_FAC_MEP = 0,
	ATCI_FAC_CB,

	ATCI_FAC_OTHER
}_AtciFacilityLogicGroup;
typedef UINT8 AtciFacilityLogicGroup;

/*Michal Buaki - Add this enum for MEP&UDP support*/
typedef enum ATCI_MEP_MODE {
	ATCI_MEP_MODE_ACTIVATE = 0,
	ATCI_MEP_MODE_DEACTIVATE,
	ATCI_MEP_MODE_DISABLE,
	ATCI_MEP_MODE_READ,
	ATCI_MEP_MODE_UNBLOCK,
	ATCI_MEP_MODE_NUM_OPERS
} _AtciMEPMode;
typedef UINT8 AtciMEPMode;

/*Michal Buaki - Add this enum for MEP&UDP support*/
typedef enum ATCI_UDP_MODE {
	ATCI_UDP_MODE_ACTIVATE = 0,
	ATCI_UDP_MODE_DEACTIVATE,
	ATCI_UDP_MODE_READ,
	ATCI_UDP_MODE_NUM_OPERS
} _AtciUDPMode;
typedef UINT8 AtciUDPMode;

/*Michal Buaki - Add this enum for MEP&UDP support*/
typedef enum ATCI_UDP_CAT {
	ATCI_UDP_CAT_UNAUTHERIZED_SIM_DETECTION = 0,
	ATCI_UDP_CAT_INVALID_SIM_DETECTION,
	ATCI_UDP_NUM_CATS
} _AtciUDPCat;
typedef UINT8 AtciUDPCat;


#define ATCI_SS_BS_EXT_ALL_SERVICES           0xFF


#define ATCI_CBST_DEFAULT_SPEED  6
#define ATCI_CBST_DEFAULT_NAME   0
#define ATCI_CBST_DEFAULT_CE     1

/* current USSD operation states */
typedef enum ATCI_USSD_STATE {
	ATCI_USSD_IDLE = 0,
	ATCI_USSD_MO_WAIT_NW_RSP,
	ATCI_USSD_MT_WAIT_TE_RSP,
	ATCI_USSD_MT_TE_RSP_SENT,
	ATCI_USSD_ABORT_SENT,
	ATCI_USSD_NUM_STATES
} _AtciUssdState;
typedef UINT8 AtciUssdState;

/* current USSD operation states */
typedef enum ATCI_DATA_MODE_TYPE {
	ATCI_WAIT_PS_DEFINE_CTX,
	ATCI_WAIT_PS_DEFAULT_CTX,
	ATCI_WAIT_PS_ENTER_DATA,
	ATCI_WAIT_PS_DATA_OK,
	ATCI_WAIT_PS_DEACTIVATE,
	ATCI_PS_CTX_DEFINED,
	ATCI_PS_DATA,
	ATCI_PS_PPP_DATA,
	ATCI_PS_SINK_DATA,

	ATCI_WAIT_CS_DATA_CONN,
	ATCI_WAIT_CS_DATA_OK,
	ATCI_CS_DATA,
	ATCI_CS_PPP_DATA,
	ATCI_CS_SINK_DATA,

	ATCI_WAIT_CS_VOICE,
	ATCI_CS_VOICE,

	ATCI_NUMS_DATA
} _AtciDataConnType;
typedef UINT8 AtciDataConnType;

/* current USSD operation states */
typedef enum ATCI_PPP_STATE {
	ATCI_PPP_OFF = 0,
	ATCI_PPP_REQUESTED,
	ATCI_PPP_ON
} _AtciPppState;
typedef UINT8 AtciPppState;

/* current CSCS character set enum */
typedef enum ATCI_CHARACTER_SET {
	ATCI_CHSET_GSM = 0,
	ATCI_CHSET_HEX,
	ATCI_CHSET_IRA,
	ATCI_CHSET_PCDN,
	ATCI_CHSET_UCS2,
	ATCI_CHSET_8859_N,
	ATCI_CHSET_8859_C,
	ATCI_CHSET_8859_A,
	ATCI_CHSET_8859_G,
	ATCI_CHSET_8859_H,
} _AtciCharacterSet;
typedef UINT8 AtciCharacterSet;


/*************************************************
**  Data Structures
*/

typedef struct AtciSSCntx_Struct
{
	UINT8 queryCallClassStatus;
	UINT8 ussdMode;;
	AtciUssdState currentUssdState;
	UINT8 localCwOption;
}AtciSSCntx;

typedef struct ErrorRatioLookupTag
{
	const INT16 mantissa;
	const INT16 exponent;
	const INT32 enumValue;
} ErrorRatioLookup;

typedef struct EnumParamLookupTag
{
	const INT32 atParam;
	const INT32 bgParam;
} EnumParamLookup;

typedef struct cgSend_struct {
	UINT16 dataSize;
	BOOL isUnackRlc;
	UINT8 sapi;
	UINT8 peakThroughput;
	UINT8 radioPriorityLevel;
}CgSend;

typedef struct AtciQosQuery_Struct {
	BOOL inProgress;
	BOOL qosType;
	UINT8 cid;
}AtciQosQuery;


typedef enum ATCIQOSTYPE
{
	ATCI_QOSTYPE_MIN = 0,   /* Minimum QoS */
	ATCI_QOSTYPE_REQ,       /* Requested QoS */
	ATCI_QOSTYPE_NEG,       /* Negotiated QoS */

	ATCI_QOSTYPE_NUMTYPES
} _AtciQosType;

typedef struct AtciCurrentSetCntx_Struct {
	BOOL bDefined;
	CiPsPdpType pdpType;
	UINT8 pdpAddress[ CI_PS_PDP_IP_V6_SIZE ];
	_AtciConnectionType connectionType;
	CiRequestHandle reqHandle;                /* CiRequestHanlde that the call is originated from */
} AtciCurrentSetCntx;

typedef struct AtciCurrentPsCntx_Struct {
	UINT8 currCid;
	void *reqMsg;
}AtciCurrentCntx;

typedef struct AtciCurrentPsCntxList_Struct {
	AtciCurrentCntx currCntx[NUM_OF_TEL_ATP];
	AtciCurrentSetCntx cInfo[CI_PS_MAX_PDP_CTX_NUM];
}AtciCurrentPsCntxList;


/* Call Information structure */
typedef struct AtCiCcCallInfo_struct {
	CiCcCallId CallId;                                                                      /* Unique Call Identifier               */
	CiBoolean IsEmergency;                                                                  /* Is this an Emergency Call?           */
	CiBoolean IsMPTY;                                                                       /* Is this part of a Multiparty Call?   */
	CiCcCallDirection ReleaseDirection;                                                     /* Release Direction (MT/MO) Indicator  */
	CiCcCallDirection CallDirection;                                                        /* Call Direction (MT/MO) Indicator     */
	CiCcCurrentCState OldState;                                                             /* Previous Call State                  */
	CiCcCurrentCState State;                                                                /* Current Call State                   */
	CiCcCurrentCMode OldMode;                                                               /* Previous Call Mode                   */
	CiCcCurrentCMode Mode;                                                                  /* Current Call Mode                    */
	UINT8 Digits[ CI_MAX_ADDRESS_LENGTH + ATCI_NULL_TERMINATOR_LENGTH ];                    /* Number Digits */
	CiAddressType AddrType;                                                                 /* Address Type */
	CiOptNameInfo Name;                                                                     /* Name Information */
	CiBoolean SsNumberInfoReceived;                                                         /* Have received SS number indication or not, such as +CLIP/+COLP */
	CiBoolean SsNameInfoReceived;                                                           /* Have received SS name indication or not, such as +CNAP */
	CiRequestHandle reqHandle;                                                              /* CiRequestHanlde that the call is originated from */
} AtCiCcCallInfo;

typedef struct AtciCurrentCall_Struct {
	CiCcBasicCMode currBasicMode;                                   /* Current Basic Call Mode           */
	CiCcCallId currCallId;                                          /* Unique Call Identifier               */
	CiRequestHandle currHandle;                                     /* CiRequestHanlde that the call is originated from */
}AtciCurrentCall;

typedef struct AtciCurrentCallsList_Struct {
	AtciCurrentCall currCall[ NUM_OF_TEL_ATP ];
	UINT8 NumCalls;
	AtCiCcCallInfo callInfo[ CICC_MAX_CURRENT_CALLS ];
} AtciCurrentCallsList;

typedef struct _AtciFrameData
{
	UINT8 dataLength;
	CHAR data[ATCI_MAX_FRAME_SIZE];
} AtciFrameData;

typedef struct _AtciFramesQueue
{
	UINT8 currentEntries;
	UINT8 readIdx;
	UINT8 writeIdx;
	AtciFrameData frames[ATCI_MAX_QUEUE_SIZE];
} AtciFramesQueue;

typedef struct _AtciMsgInfo
{
	UINT8 fo;               /* First octet */
	UINT8 vp;               /* Validity Period */
	UINT8 pid;              /* Protocol Identifier */
	UINT8 dcs;              /* Data coding scheme */
	UINT8 msgRef;
	UINT8 udhPresent;       /* User Data Header Present */

	UINT8 addrType;
	UINT8 addrPlan;
	UINT8 maxMsgLen;
	UINT8 stat;    /* Message status, mainly used in +CMGW */
	UINT8 destAddr[ CI_MAX_ADDRESS_LENGTH + ATCI_NULL_TERMINATOR_LENGTH ];
} AtciMsgInfo;

/*************************************************
**  State Machine Enums
*/
typedef enum ATCI_CALL_STATE
{
	CALL_IDLE,
	CALL_IN_PROGRESS,
	CALL_CONNECTED,
	CALL_DISCONNECTED

} _AtciCallState;
typedef UINT8 AtciCallState;

typedef enum  ATCI_GPRS_STATE
{
	ATCI_GPRS_DETACH_REQ = 0,
	ATCI_GPRS_ATTACH_REQ = 1

} _AtciGprsState;
typedef UINT8 AtciGprsState;

typedef enum ATCI_DIAL_NUMBER_TYPE
{
	/* Number format as described in GSM 04.08 section 10.5.4.7 */
	ATCI_DIAL_NUMBER_UNKNOWN       = 129,   /* Unknown type, IDSN format number */
	ATCI_DIAL_NUMBER_RESTRICTED    = 128,   /* Unknown type, unknown number format */
	ATCI_DIAL_NUMBER_NATIONAL      = 161,   /* National number type, IDSN format */
	ATCI_DIAL_NUMBER_INTERNATIONAL = 145,   /* International number type, ISDN format */
	ATCI_DIAL_NUMBER_NET_SPECIFIC  = 177,   /* Network specific number, ISDN format */
	/* Added by Michal Bukai*/
	ATCI_DIAL_NUMBER_DEDICATED     = 193,
	ATCI_DIAL_NUMBER_EXTENSION     = 241,

	ATCI_DIAL_NUMBER_INVALID          /* Added by EGOR for +CPBW command */
} _AtciDialNumberType;

typedef UINT8 AtciDialNumberType;

typedef enum ATCI_SMS_FORMAT_MODE
{
	ATCI_SMS_FORMAT_PDU_MODE = 0,           /* pdu mode - default */
	ATCI_SMS_FORMAT_TXT_MODE = 1,           /* text mode */

	ATCI_SMS_FORMAT_NUM_MODE                /* invalid mode */
} _AtciSmsFormatMode;
typedef UINT8 AtciSmsFormatMode;


typedef enum ATCI_COPS_MODE
{
	ATCI_AUTOMATIC_MODE = 0,
	ATCI_MANUAL_OPERATOR_SELECTION,
	ATCI_MANUAL_DEREGISTER,
	ATCI_SET_FORMAT,
	ATCI_MANUAL_THEN_AUTOMATIC,

	ATCI_MODE_NUMS
} _AtciCopsMode;

typedef UINT8 AtciCopsMode;

typedef enum ATCI_COPS_MODE_FORMAT
{
	ATCI_OP_FORMAT_LONG = 0,
	ATCI_OP_FORMAT_SHORT,
	ATCI_OP_FORMAT_NUM,

	ATCI_OP_FORMAT_NUMS
} _AtciCopsModeFormat;

typedef UINT8 AtciCopsModeFormat;

/*Added by Michal Bukai - at support for AccTchMode parameter*/
typedef enum ATCI_CPOL_ACT
{
	ATCI_ACT_GSM  = 0,
	ATCI_ACT_GSM_COMPACT,
	ATCI_ACT_UTRAN,
	ATCI_ACT_NUMS
} _AtciCpolAct;

typedef UINT8 AtciCpolAct;

 //need to map this value to ATCI_CFUN_OP_DISABLE_SIM
#define ATCI_PLUS_CFUN_OP_DISABLE_SIM 5

typedef enum ATCI_CFUN_OP_TYPE
{
	ATCI_CFUN_OP_MIN_FUNC = 0,      /* Minimum functionality */
	ATCI_CFUN_OP_FULL_FUNC,         /* Full functionality */
	ATCI_CFUN_OP_DISABLE_RF_TX,
	ATCI_CFUN_OP_DISABLE_RF_RX,
	ATCI_CFUN_OP_DISABLE_RF_RXTX,
	ATCI_CFUN_OP_UPDATE_NVM_MIN_FUNC,       /* Minimum functionality */
	ATCI_CFUN_OP_UPDATE_NVM_FULL_FUNC,      /* Full functionality */
	ATCI_CFUN_OP_MIN_FUNC_NO_IMSI_DETACH,   /* Minimum functionality but without IMSI detach */
	ATCI_CFUN_OP_DISABLE_SIM,                 /*disable SIM*/
	ATCI_CFUN_OP_UPDATE_NVM_DISABLE_RF_RXTX = 9,

	ATCI_CFUN_NUM_OP
} _AtciCfunOpType;
typedef UINT8 AtciCfunOpType;

typedef enum ATCI_CHOLD_MODE_TYPE
{
	ATCI_CHLD_MODE_0 = 0,
	ATCI_CHLD_MODE_1,
	ATCI_CHLD_MODE_2,
	ATCI_CHLD_MODE_3,
	ATCI_CHLD_MODE_4,
	ATCI_CHLD_MODE_1X = 10,
	ATCI_CHLD_MODE_2X = 20,
	ATCI_CHLD_MODE_NUMS
} _AtciChldModeTypes;
typedef UINT8 AtciChldModeTypes;

/* Registration Mode indicators - as required */
typedef enum ATCI_REGMODE {
	ATCI_REGMODE_AUTOMATIC = 0,     /* Automatic Registration Request */
	ATCI_REGMODE_MANUAL,            /* Manual Registration Request */
	ATCI_REGMODE_DEREGISTER,        /* Deregistration Request */
	ATCI_REGMODE_SETONLY,
	ATCI_REGMODE_MANUAL_AUTO        /* Manual Request, fallback to Automatic */
} _AtciRegMode;
typedef UINT8 AtciRegMode;

typedef enum ATCI_AOC_MODE {
	ATCI_AOC_MODE_QUERY = 0,
	ATCI_AOC_MODE_DEACTIVATE,
	ATCI_AOC_MODE_ACTIVATE,

	ATCI_AOC_NUM_MODES
} _AtciAocMode;

typedef UINT8 AtciAocMode;

typedef enum ATCI_CB_MODES {
	ATCI_CB_MSG_ACCEPT = 0,
	ATCI_CB_MSG_REJECT,

	ATCI_CB_NUM_MODES
} _AtciCbMode;

typedef UINT8 AtciCbMode;

/* New message indication processing */
typedef enum ATCI_NMI_PROCESSING_MODES {
	ATCI_NMI_BUFFER = 0,    /*  Buffer unsolicited result codes in the TA.    */
				/* If TA result code buffer is full, indications  */
				/* can be buffered in some other place or the     */
				/* oldest indications may be discarded and        */
				/* replaced with the new received indications     */
	ATCI_NMI_DISCARD_FWD,   /*  Discard indication and reject new received    */
				/* message unsolicited result codes when TA-TE    */
				/* link is reserved (e.g. in on-line data mode).  */
				/* Otherwise forward them directly to the TE      */
	ATCI_NMI_BUFFER_FWD,    /*  Buffer unsolicited result codes in the TA     */
				/* when TA-TE link is reserved (e.g. in on-line   */
				/* data mode) and flush them to the TE after      */
				/* reservation. Otherwise forward them directly   */
				/* to the TE                                      */
	ATCI_NMI_FORWARD,       /*  Forward unsolicited result codes directly to  */
				/* the TE. TA-TE link specific inband technique   */
				/* used to embed result codes and data when TA is */
				/* in on-line data mode                           */

	ATCI_NMI_PROCESSING_NUM_MODES
} _AtciNmiProcessingMode;

typedef UINT8 AtciNmiProcessingMode;

/* New message indication routing */
typedef enum ATCI_NMI_MODES {
	ATCI_NMI_DISABLE = 0,   /*  Store received message at cellular side,    */
				/* disable indication of new message received   */
	ATCI_NMI_INDEX,         /*  Store received message at cellular side,    */
				/* indicate memory storage and memory location  */
	ATCI_NMI_MSG,           /*  Store received message at cellular side,    */
				/* forward message to application side directly */
	ATCI_NMI_CLASS3_MSG,    /*  Store CLASS 3 message (SM/CBM) at           */
				/* cellular side  - forward the message to      */
				/* application side directly                    */

	ATCI_NMI_NUM_MODES
} _AtciNmiMode;

typedef UINT8 AtciNmiMode;

/* New message indicatio buffer behaviour */
typedef enum ATCI_NMI_BFR {
	ATCI_NMI_BFR_FLUSH = 0, /*  TA buffer of unsolicited result codes defined   */
				/* within this command is flushed to the TE when    */
				/* <mode> 1...3 is entered (OK response shall be    */
				/* given before flushing the codes)                 */
	ATCI_NMI_BFR_CLEAR,     /*  TA buffer of unsolicited result codes defined   */
				/* within this command is cleared when <mode>       */
				/* 1...3 is entered                                 */

	ATCI_NMI_BFR_NUM_MODES
} _AtciNmiBfr;

typedef UINT8 AtciNmiBfr;

typedef enum ATCI_CLIR_MODES {
	ATCI_CLIR_SUBSCRIPTION = 0,
	ATCI_CLIR_INVOKE,
	ATCI_CLIR_SUPPRESS,

	ATCI_CLIR_NUM_MODES
} _AtciClirMode;

typedef UINT8 AtciClirMode;

typedef enum ATCI_SMS_CUR_CMD {
	ATCI_SMS_CMD_NONE = 0,
	ATCI_SMS_CMD_CMGS,
	ATCI_SMS_CMD_CMGW,
	ATCI_SMS_CMD_CMGC,

	ATCI_SMS_NUM_CMD
} AtciSmsCurCmd;

typedef enum ATCI_SMS_DELETE_MODE {
	ATCI_SMS_DEL_MODE_DEFAULT = 0,
	ATCI_SMS_DEL_INDEX = 0,         /* Delete only one message indicated by the index */
	ATCI_SMS_DEL_RECV_READ,         /* Delete all received read messages */
	ATCI_SMS_DEL_RECV_READ_SENT,    /* Delete all received read messages as well as sent MO messages */
	ATCI_SMS_DEL_RECV_READ_MO,      /* Delete received read messages and all MO messages - sent and unsent */
	ATCI_SMS_DEL_ALL,

	ATCI_SMS_NUM_DEL_MODES
} _AtciSmsDelMode;

typedef enum ATCI_CME_ERROR
{
	CME_PHONE_FAILURE         = 0,                          //phone failure
	CME_NO_CONNECTION         = 1,                          //no connection to phone
	CME_PHONE_ADP_LINK_RSVD         = 2,                    //phone adaptor link reserved
	CME_OPERATION_NOT_ALLOWED         = 3,                  //operation not allowed
	CME_OPERATION_NOT_SUPPORTED     = 4,                    //operation not supported
	CME_PH_SIM_PIN_REQUIRED         = 5,                    //PH SIM PIN required
	CME_PH_FSIM_PIN_REQUIRED         = 6,                   //PH-FSIM PIN required
	CME_PH_FSIM_PUK_REQUIRED         = 7,                   //PH-FSIM PUK required
	CME_NO_SIM         = 10,                                //SIM not inserted
	CME_SIM_PIN_REQUIRED     = 11,                          //SIM PIN required
	CME_SIM_PUK_REQUIRED     = 12,                          //SIM PUK required
	CME_SIM_FAILURE         = 13,                           //SIM failure
	CME_SIM_BUSY     = 14,                                  //SIM busy
	CME_SIM_WRONG     = 15,                                 //SIM wrong
	CME_INCORRECT_PASSWD     = 16,                          //incorrect password
	CME_SIM_PIN2_REQUIRED     = 17,                         //SIM PIN2 required
	CME_SIM_PUK2_REQUIRED     = 18,                         //SIM PUK2 required
	CME_MEMORY_FULL         = 20,                           //memory full
	CME_INVALID_INDEX         = 21,                         //invalid index
	CME_NOT_FOUND     = 22,                                 //not found
	CME_MEMORY_FAILURE         = 23,                        //memory failure
	CME_TEXT_STRING_TOO_LONG         = 24,                  //text string too long
	CME_INVALID_CHAR_IN_STRING         = 25,                //invalid characters in text string
	CME_DAIL_STRING_TOO_LONG         = 26,                  //dial string too long
	CME_INVALID_CHAR_IN_DIAL_STRING         = 27,           //invalid characters in dial string
	CME_NO_NW_SERVICE         = 30,                         //no network service
	CME_NW_TIMEOUT     = 31,                                //network timeout
	CME_NW_NOT_ALLOWED         = 32,                        //network not allowed - emergency calls only
	CME_NW_PIN_REQUIRED     = 40,                           //network personalization PIN required
	CME_NW_PUK_REQUIRED     = 41,                           //network personalization PUK required
	CME_NW_SUB_PIN_REQUIRED         = 42,                   //network subset personalization PIN required
	CME_NW_SUB_PUK_REQUIRED         = 43,                   //network subset personalization PUK required
	CME_SP_PIN_REQUIRED     = 44,                           //service provider personalization PIN required
	CME_SP_PUK_REQUIRED     = 45,                           //service provider personalization PUK required
	CME_CP_PIN_REQUIRED     = 46,                           //corporate personalization PIN required
	CME_CP_PUK_REQUIRED     = 47,                           //corporate personalization PUK required
	CME_HD_KEY_REQUIRED     = 48,                           //hidden key required
	CME_INVALID_PARAM               = 50,                   //Invalid Param
	CME_UNKNOWN     = 100,                                  //unknown
	CME_ILLEGAL_MS     = 103,                               //Illegal MS (#3)
	CME_ILLEGAL_ME     = 106,                               //Illegal ME (#6)
	CME_GPRS_NOT_ALLOWED     = 107,                         //GPRS services not allowed (#7)
	CME_PLMN_NOT_ALLOWED     = 111,                         //PLMN not allowed (#11)
	CME_LA_NOT_ALLOWED         = 112,                       //Location area not allowed (#12)
	CME_ROAMING_NOT_ALLOWED         = 113,                  //Roaming not allowed in this location area (#13)
	CME_SERVICE_OP_NOT_SUPPORTED     = 132,                 //service option not supported (#32)
	CME_SERVICE_OP_NOT_SUBSCRIBED     = 133,                //requested service option not subscribed (#33)
	CME_SERVICE_OP_OUT_OF_ORDER     = 134,                  //service option temporarily out of order (#34)
	CME_PDP_AUTH_FAILURE     = 149,                         //PDP authentication failure
	CME_INVALID_MOBILE_CLASS         = 150,                 //invalid mobile class
	CME_UNSPECIFIED_GPRS_ERR        = 148,                  //unspecified GPRS error
	CME_COMMMAND_TIMEOUT_ERR        = 151,                 //AT command timeout

	CMS_ME_FAILURE = 300,                                   //ME failure
	CMS_SMS_SERVICE_RESV = 301,                             //SMS service of ME reserved
	CMS_OPERATION_NOT_ALLOWED = 302,                        //operation not allowed
	CMS_OPERATION_NOT_SUPPORTED = 303,                      //operation not supported
	CMS_INVALID_PDU_MODE_PARA = 304,                        //invalid PDU mode parameter
	CMS_INVALID_TEXT_MODE_PARA = 305,                       //invalid text mode parameter
	CMS_NO_SIM = 310,                                       //(U)SIM not inserted
	CMS_SIM_PIN_REQUIRED = 311,                             //(U)SIM PIN required
	CMS_PH_SIM_PIN_REQUIRED = 312,                          //PH-(U)SIM PIN required
	CMS_SIM_FAILURE = 313,                                  //(U)SIM failure
	CMS_SIM_BUSY = 314,                                     //(U)SIM busy
	CMS_SIM_WRONG = 315,                                    //(U)SIM wrong
	CMS_SIM_PUK_REQUIRED = 316,                             //(U)SIM PUK required
	CMS_SIM_PIN2_REQUIRED = 317,                            //(U)SIM PIN2 required
	CMS_SIM_PUK2_REQUIRED = 318,                            //(U)SIM PUK2 required
	CMS_MEMORY_FAILURE = 320,                               //memory failure
	CMS_INVALID_MEMORY_INDEX = 321,                         //invalid memory index
	CMS_MEMORY_FULL = 322,                                  //memory full
	CMS_SMSC_ADDR_UNKNOWN = 330,                            //SMSC address unknown
	CMS_NO_NW_SERVICE = 331,                                //no network service
	CMS_NW_TIMEOUT = 332,                                   //network timeout
	CMS_NO_CNMA_ACK_EXPECTED = 340,                         //no +CNMA acknowledgement expected
	CMS_UNKNOWN_ERROR = 500,                                //unknown error

#ifdef AT_LABTOOL_CMNDS
	CME_WIFI_RETURN_ERROR = 501,                            //WIFI labtool return error
	CME_BT_RETURN_ERROR = 502,                              //BT labtool return error
	CME_FM_RETURN_ERROR = 503,                              //FM labtool return error
#endif
#ifdef AT_PRODUCTION_CMNDS
	CME_FILE_EXIST_ERROR = 504,                             //MRD file already exist
	CME_FILE_WITH_SAME_VERSION_EXIST_ERROR = 505,           //MRD file with same version already exist
	CME_FILE_WITH_NEWER_VERSION_EXIST_ERROR = 506,          //MRD file with newer version already exist
#endif
}_AtciCmeError;
typedef UINT16 AtciCmeError;

enum ATCI_RESULT_CODE
{
	ATCI_RESULT_CODE_NULL,                  //Timer will not stop
	ATCI_RESULT_CODE_OK,                    //Timer will stop, OK will be sent
	ATCI_RESULT_CODE_ERROR,                 //Timer will stop, ERROR will be sent
	ATCI_RESULT_CODE_CME_ERROR,             //Timer will Stop, CME ERROR will be sent
	ATCI_RESULT_CODE_CMS_ERROR,             //Timer will Stop, CMS ERROR will be sent
	ATCI_RESULT_CODE_SUPPRESS,              //Timer will Stop, End of AT response without OK or ERROR string
} _AtciResultCode;
typedef UINT8 AtciResultCode;

/* telephony controller struct */
typedef struct telCurCallIdReqhandle_struct
{
	CiCcCallId CallId;                                      /* Unique Call Identifier               */
	CiRequestHandle reqHandle;                              /* CiRequestHanlde that the call is originated from */
}telCurCallIdReqhandle_t;

void atciSvgFreeCnfMem( CiServiceGroupID svgId, CiPrimitiveID primId, void* paras );

void atciSvgFreeIndMem( CiServiceGroupID svgId, CiPrimitiveID primId, void* paras );

int handle_CMEE_code(UINT32 reqHandle, UINT16 errCode);
int handle_CMSE_code(UINT32 reqHandle, UINT16 errCode);
#ifdef AT_CUSTOMER_HTC
enum ATCI_HTC_ERROR_CLASS
{
	ATCI_HTC_ERROR_CLASS_LUP_REJECT = 0,                       //Location update Reject
	ATCI_HTC_ERROR_CLASS_GPRS_ATTACH_REJECT,                  //GPRS Attach /Combination Attach Reject
	ATCI_HTC_ERROR_CLASS_RAU_REJECT,                           //RAU/Combine RAU Reject
	ATCI_HTC_ERROR_CLASS_PDP_ACTIVATION_REJECT,              //PDP activation Reject
	ATCI_HTC_ERROR_CLASS_RADIO_LINK_FAILURE = 6,             //Radio link failure
	ATCI_HTC_ERROR_CLASS_MM_AUTH_FAILURE,                    //MM Authentication fail
	ATCI_HTC_ERROR_CLASS_PS_SECURITY_FAILURE,                //PS security fail
	ATCI_HTC_ERROR_CLASS_NW_CS_AUTH_REJECT,                  //NW CS Auth reject
	ATCI_HTC_ERROR_CLASS_NW_PS_AUTH_REJECT,                  //NW PS Auth reject
	ATCI_HTC_ERROR_CLASS_SMS_REJECT,                          //SMS reject
	ATCI_HTC_ERROR_CLASS_SMS_REJECT_CP_ERROR,               //SMS reject , CP Error
	ATCI_HTC_ERROR_CLASS_SMS_REJECT_RP_ERROR,               //SMS reject , RP Error
} _AtciHtcErrorClass;
#define SPEC_ERROR_CODE_INDICATION_PREFIX  "+HTCerror"
#endif

//used for ^MODE indication and ^SYSINFO commands
enum ATCI_SYSINFO_SERVICE_STATUS
{
	ATCI_SYSINFO_SERVICE_STATUS_NO_SERVICE = 0,
	ATCI_SYSINFO_SERVICE_STATUS_RESTRICTED_SERVICE = 1,
	ATCI_SYSINFO_SERVICE_STATUS_VALID_SERVICE = 2,
	ATCI_SYSINFO_SERVICE_STATUS_RESTRICTED_AREA_SERVICE = 3,
	ATCI_SYSINFO_SERVICE_STATUS_POWER_SAVING = 4,
}_AtciSysinfoServiceStatus;
typedef UINT8 AtciSysinfoServiceStatus;

enum ATCI_SYSINFO_SERVICE_DOMAIN
{
	ATCI_SYSINFO_SERVICE_DOMAIN_NO_SERVICE = 0,
	ATCI_SYSINFO_SERVICE_DOMAIN_CS_ONLY = 1,
	ATCI_SYSINFO_SERVICE_DOMAIN_PS_ONLY = 2,
	ATCI_SYSINFO_SERVICE_DOMAIN_CS_PS = 3,
}_AtciSysinfoServiceDomain;
typedef UINT8 AtciSysinfoServiceDomain;

enum ATCI_SYSINFO_ROAM_STATUS
{
	ATCI_SYSINFO_ROAM_STATUS_NO_ROAMING = 0,
	ATCI_SYSINFO_ROAM_STATUS_ROAMING = 1,
}_AtciSysinfoRoamStatus;
typedef UINT8 AtciSysinfoRoamStatus;

enum ATCI_SYSINFO_SYSTEM_MODE
{
	ATCI_SYSINFO_SYSTEMO_MODE_NO_SERVICE = 0,
	ATCI_SYSINFO_SYSTEMO_MODE_RESERVED_1 = 1,
	ATCI_SYSINFO_SYSTEMO_MODE_RESERVED_2 = 2,
	ATCI_SYSINFO_SYSTEMO_MODE_GSM_GPRS = 3,
	ATCI_SYSINFO_SYSTEMO_MODE_WCDMA = 5,
	ATCI_SYSINFO_SYSTEMO_MODE_TDSCDMA = 15,
}_AtciSysinfoSystemMode;
typedef UINT8 AtciSysinfoSystemMode;

enum ATCI_SYSINFO_SIM_STATUS
{
	ATCI_SYSINFO_SIM_STATUS_INVALID = 0,
	ATCI_SYSINFO_SIM_STATUS_VALID = 1,
	ATCI_SYSINFO_SIM_STATUS_NO_INSERT_UNCHECKED_UNBLOCKED = 255,
}_AtciSysinfoSimStatus;
typedef UINT8 AtciSysinfoSimStatus;


#endif /*AT_CICONV_H*/

