/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/* ===========================================================================
   File        : ci_api_types.h
   Description : Data types file for the CI API

   Notes       :

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

   Unless otherwise agreed by Intel in writing, you may not remove or alter this notice or any other notice embedded
   in Materials by Intel or Intel's suppliers or licensors in any way.
   =========================================================================== */

#if !defined(_CI_API_TYPES_H_)
#define _CI_API_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#define CI_API_TYPES_VER_MAJOR 2
#define CI_API_TYPES_VER_MINOR 0

#define ATCI_SAC_SG_INVALID_HANDLE  (0x11111111L)
#define ATCI_SAC_SH_INVALID_HANDLE  (0x01111111L)

#include "gbl_types.h"
#include "ci_cfg.h"

//ICAT EXPORTED ENUM
typedef enum CISGID {
	CI_SG_ID_CC = 1,
	CI_SG_ID_SS,
	CI_SG_ID_MM,
	CI_SG_ID_PB,
	CI_SG_ID_SIM,
	CI_SG_ID_MSG,
	CI_SG_ID_PS,
	CI_SG_ID_DAT,
	CI_SG_ID_DEV,
	CI_SG_ID_HSCSD,
	CI_SG_ID_DEB,
	CI_SG_ID_ATPI,
	CI_SG_ID_PL,
	CI_SG_ID_OAM,
	CI_SG_ID_NEXTAVAIL /* This must always be the last entry in the list */
} _CiServiceGroupId;

#define CI_SG_NUMIDS CI_SG_ID_NEXTAVAIL - 1

/* CI Shell API return code */
//ICAT EXPORTED ENUM
typedef enum CIRC {
	CIRC_SUCCESS = 0,
	CIRC_FAIL,
	CIRC_INTERLINK_FAIL, /* the link between application subsystem and communication subsystem is broken */
	CIRC_SH_NULL_CONFIRM_CB,
	CIRC_SH_NULL_FREEREQMEM_CB,
	CIRC_SH_INVALID_HANDLE,
	CIRC_SH_INVALID_OPER,
	CIRC_SH_NULL_REQPARAS,
	CIRC_SG_INVALID_HANDLE,
	CIRC_SG_RESERVED_PRIMID,
	CIRC_SG_NULL_REQPARAS,
	CIRC_SG_NULL_RSPPARAS,
	CIRC_NUM_RESCODES
} _CiReturnCode;

typedef UINT16 CiReturnCode;

/* CI Shell Operation Code */
//ICAT EXPORTED ENUM
typedef enum CISHOPER {
	CI_SH_OPER_REGISTERSH = 0,
	CI_SH_OPER_DEREGISTERSH,
	CI_SH_OPER_GETVERSION,
	CI_SH_OPER_REGISTERSG,
	CI_SH_OPER_DEREGISTERSG,
	CI_SH_OPER_REGISTERDEFSG,
	CI_SH_OPER_DEREGISTERDEFSG,
	CI_SH_QUERYEXTENDED,
	CI_SH_QUERYEXTENDEDSERVICEPRIM,
	CI_SH_QUERYEXTENDEDSERVICEGROUP,
	CI_SH_NUMOPERS
} CiShOper;

/* 8 bit defined boolean for CI */
#ifdef ENV_MSVC
typedef    BOOL_MSVC CiBoolean;
#else
typedef    UINT8 CiBoolean;
#endif

/* Service group id */
typedef UINT8 CiServiceGroupID;

/* service primitive id */
/* Primitive IDs above 0xF000 in each service group are reserved for error primitives */
typedef UINT16 CiPrimitiveID;

/* service handle, used to uniquely identify the client who registered for the service group */
typedef UINT32 CiServiceHandle;

/* opaque handle used to identify request/confirm primitives pair */
typedef UINT32 CiRequestHandle;

/* opaque handle used to identify indicate/respond primitives pair */
typedef UINT32 CiIndicationHandle;

/* shell handle, used to uniquely identify the client who registered a callback for shell operations*/
typedef UINT32 CiShHandle;

/* opaque handle used for shell callback registrations */
typedef UINT32 CiShOpaqueHandle;

/* opaque handle used to identify request/confirm operation pair */
typedef UINT32 CiShRequestHandle;

/* opaque handle used for service group registrations */
typedef UINT32 CiSgOpaqueHandle;

/* Confirmation call-back that is going to be called by the server in response to requested shell operation sent from the client */
typedef void (*CiShConfirm)(CiShOpaqueHandle opShHandle, CiShOper oper, void* cnfParas, CiShRequestHandle opHandle);

/* Confirmation call-back that is going to be called by the server in response to request service primitive sent from the client */
typedef void (*CiConfirm)(CiSgOpaqueHandle opSgOpaqueHandle, CiServiceGroupID id, CiPrimitiveID primId, CiRequestHandle reqHandle, void* paras);

/* Indication call-back that is going to be called by the server to indicate events happening */
typedef void (*CiIndicate)(CiSgOpaqueHandle opSgOpaqueHandle, CiServiceGroupID id, CiPrimitiveID primId, CiIndicationHandle indHandle, void* paras);

/* Free memory call-back that is used by the server shell to free memory allocated by the client for the requested shell operation */
typedef void (*CiShFreeReqMem)(CiShOpaqueHandle opShFreeHandle, CiShOper oper, void* reqParas);

/* Free memory call-back that is used by the client shell to free memory allocated by the server for confirmation of the requested shell operation */
typedef void (*CiShFreeCnfMem)(CiShOpaqueHandle opShFreeHandle, CiShOper oper, void* cnfParas);

/* Free memory call-back that is used by the server service group to free memory allocated by the client service group for the request service primitive */
typedef void (*CiSgFreeReqMem)(CiSgOpaqueHandle opSgFreeHandle, CiServiceGroupID id, CiPrimitiveID primId, void* reqParas);

/* Free memory call-back that is used by the server service group to free memory allocated by the client service group for the respond service primitive */
typedef void (*CiSgFreeRspMem)(CiSgOpaqueHandle opSgFreeHandle, CiServiceGroupID id, CiPrimitiveID primId, void* rspParas);

/* Free memory call-back that is used by the client service group to free memory allocated by the server service group for the confirm service primitive */
typedef void (*CiSgFreeCnfMem)(CiSgOpaqueHandle opSgFreeHandle, CiServiceGroupID id, CiPrimitiveID primId, void* cnfParas);

/* Free memory call-back that is used by the client service group to free memory allocated by the server service group for the indicate service primitive */
typedef void (*CiSgFreeIndMem)(CiSgOpaqueHandle opSgFreeHandle, CiServiceGroupID id, CiPrimitiveID primId, void* indParas);


/* primitive list, contains a list of primitive id */
//ICAT EXPORTED STRUCT
typedef struct CiPrimList_struct {
	UINT16 size;            /* actual number of primitive IDs that are held in the pList */
#ifndef CCI_CMN_CONTIGUOUS
	CiPrimitiveID* pList;   /* pointer to a list of service primitives */
#else
	CiPrimitiveID primId[CI_MAX_CI_PRIMITIVE_LIST_SIZE];
#endif

} CiPrimitiveList;

/* structures related to shell operations, refer to 4.2.5 for detail description */
//ICAT EXPORTED STRUCT
typedef struct CiShOperRegisterShCnf_struct {
	CiReturnCode rc;
	CiShHandle handle;
	CiShOpaqueHandle opShFreeHandle;
	CiShFreeCnfMem ciShFreeCnfMem;
} CiShOperRegisterShCnf;

//ICAT EXPORTED STRUCT
typedef struct CiShOperDeregisterShCnf_struct {
	CiReturnCode rc;
} CiShOperDeregisterShCnf;

// ICAT EXPORTED STRUCTURE
typedef struct CiShOperGetVersionReq_struct {
	CiServiceGroupID id;
} CiShOperGetVersionReq;

//ICAT EXPORTED STRUCT
typedef struct CiShOperGetVersionCnf_struct {
	CiReturnCode rc;
	CiServiceGroupID id;
	UINT8 major;
	UINT8 minor;
} CiShOperGetVersionCnf;

// ICAT EXPORTED STRUCTURE
typedef struct CiShOperRegisterDefaultSgReq_struct {
	CiSgOpaqueHandle opSgDefCnfHandle;
	CiConfirm ciCnfDef;
	CiIndicate ciIndDef;
	CiSgOpaqueHandle opSgDefIndHandle;
} CiShOperRegisterDefaultSgReq;

//ICAT EXPORTED STRUCT
typedef struct CiShOperRegisterDefaultSgCnf_struct {
	CiReturnCode rc;
	CiServiceHandle defHandle;
	CiSgOpaqueHandle opSgFreeDefCnfHandle;
	CiSgFreeCnfMem ciSgFreeDefCnfMem;
	CiSgOpaqueHandle opSgFreeDefIndHandle;
	CiSgFreeIndMem ciSgFreeDefIndMem;
} CiShOperRegisterDefaultSgCnf;

// ICAT EXPORTED STRUCTURE
typedef struct CiShOperDeregisterDefaultSgReq_struct {
	CiServiceHandle defHandle;
} CiShOperDeregisterDefaultSgReq;

//ICAT EXPORTED STRUCT
typedef struct CiShOperDeregisterDefaultSgCnf_struct {
	CiReturnCode rc;
} CiShOperDeregisterDefaultSgCnf;

// ICAT EXPORTED STRUCTURE
typedef struct CiShOperRegisterSgReq_struct {
	CiServiceGroupID id;
	CiSgOpaqueHandle opSgCnfHandle;
	CiConfirm ciConfirm;
	CiSgOpaqueHandle opSgIndHandle;
	CiIndicate ciIndicate;
	CiSgOpaqueHandle opSgFreeReqHandle;
	CiSgFreeReqMem ciSgFreeReqMem;
	CiSgOpaqueHandle opSgFreeRspHandle;
	CiSgFreeRspMem ciSgFreeRspMem;
} CiShOperRegisterSgReq;

//ICAT EXPORTED STRUCT
typedef struct CiShOperRegisterSgCnf_struct {
	CiReturnCode rc;
	CiServiceGroupID id;
	CiServiceHandle handle;
	CiSgOpaqueHandle opSgFreeCnfHandle;
	CiSgFreeCnfMem ciSgFreeCnfMem;
	CiSgOpaqueHandle opSgFreeIndHandle;
	CiSgFreeIndMem ciSgFreeIndMem;
} CiShOperRegisterSgCnf;

// ICAT EXPORTED STRUCTURE
typedef struct CiShOperDeregisterSgReq_struct {
	CiServiceHandle handle;
} CiShOperDeregisterSgReq;

//ICAT EXPORTED STRUCT
typedef struct CiShOperDeregisterSgCnf_struct {
	CiReturnCode rc;
	CiServiceGroupID id;
} CiShOperDeregisterSgCnf;


//ICAT EXPORTED STRUCT
typedef struct CiShQueryExtendedReq_struct {
	CiServiceGroupID id;
} CiShQueryExtendedReq;

//ICAT EXPORTED STRUCT
typedef struct CiShQueryExtendedCnf_struct {
	CiReturnCode rc;
	CiServiceGroupID id;
	UINT16 mSize;
#ifndef CCI_CMN_CONTIGUOUS
	CiPrimitiveList* pPrimList;
#else
	CiPrimitiveList ciPrimList;
#endif

} CiShQueryExtendedCnf;

//ICAT EXPORTED STRUCT
typedef struct CiShQueryExtendedServicePrimReq_struct {
	CiServiceGroupID id;
	CiPrimitiveID primId;
} CiShQueryExtendedServicePrimReq;

//ICAT EXPORTED STRUCT
typedef struct CiShQueryExtendedServicePrimCnf_struct {
	CiReturnCode rc;
	CiServiceGroupID id;
	CiPrimitiveID primId;
	CiBoolean supported;
} CiShQueryExtendedServicePrimCnf;

//ICAT EXPORTED STRUCT
typedef struct CiShQueryExtendedServiceGroupReq_struct {
	CiServiceGroupID id;
} CiShQueryExtendedServiceGroupReq;

//ICAT EXPORTED STRUCT
typedef struct CiShQueryExtendedServiceGroupCnf_struct {
	CiReturnCode rc;
	CiServiceGroupID id;
	CiBoolean supported;
} CiShQueryExtendedServiceGroupCnf;

/* Address Type: Type of Number (TON). See [4]. */
//ICAT EXPORTED ENUM
typedef enum CI_ADDR_NUMTYPE {
	CI_NUMTYPE_UNKNOWN      = 0x00,         /* Unknown */
	CI_NUMTYPE_INTERNATIONAL = 0x01,        /* International Number */
	CI_NUMTYPE_NATIONAL     = 0x02,         /* National Number */
	CI_NUMTYPE_NETWORK      = 0x03,         /* Network Specific (e.g. Operator Access) */
	CI_NUMTYPE_DEDICATED    = 0x04,         /* Dedicated Access Number, Short Code */
	CI_NUMTYPE_EXTENSION    = 0x07          /* Reserved for Extension */
						/* << All other codes are reserved >> */
} _CiAddrNumType;

typedef UINT8 CiAddrNumType;

/* Address Type: Numbering Plan Identification (NPI). See [4]. */
//ICAT EXPORTED ENUM
typedef enum CI_ADDR_NUMPLAN {
	CI_NUMPLAN_UNKNOWN   = 0x00,    /* Unknown */
	CI_NUMPLAN_E164_E163 = 0x01,    /* ISDN/Telephony (E.164/E.163) */
	CI_NUMPLAN_DATA_X121 = 0x03,    /* Data Numbering Plan (X.121) */
	CI_NUMPLAN_TELEX_F69 = 0x04,    /* Telex Numbering Plan (F.69) */
	CI_NUMPLAN_NATIONAL  = 0x08,    /* National Numbering Plan */
	CI_NUMPLAN_PRIVATE   = 0x09,    /* Private Numbering Plan */
	CI_NUMPLAN_RSVD_CTS  = 0x0b,    /* Reserved for CTS */
	CI_NUMPLAN_EXTENSION = 0x0f     /* Reserved for Extension */
					/* << All other codes are reserved >> */
} _CiAddrNumPlan;

typedef UINT8 CiAddrNumPlan;

/* Address Type (TON/NPI) structure - also used as part of Address Info, etc. See [4]. */
//ICAT EXPORTED STRUCT
typedef struct CiAddressType_struct {
	CiAddrNumType NumType;  /* TON: Type of Number */
	CiAddrNumPlan NumPlan;  /* NPI: Numbering Plan Identification */
} CiAddressType;

/* Address Information structure - used for Calling Number, Called Number, Phonebook, etc. */
#define CI_MAX_ADDRESS_LENGTH 40

//ICAT EXPORTED STRUCT
typedef struct CiAddressInfo_struct {
	CiAddressType AddrType;                 /* Address Type Information */
	UINT8 Length;                           /* Address Length in digits */
	UINT8 Digits[ CI_MAX_ADDRESS_LENGTH ];  /* Address Digits */

} CiAddressInfo;

/* Optional Address Information structure - used for Calling Number, Called Number, etc. */
//ICAT EXPORTED STRUCT
typedef struct CiOptAddressInfo_struct {
	CiBoolean Present;              /* Address Info present? */
	CiAddressInfo AddressInfo;      /* Address Info */
} CiOptAddressInfo;

/* Subaddress Information structure - used for Calling Number, Called Number, etc. */
#define CI_MAX_SUBADDR_LENGTH 32

//ICAT EXPORTED STRUCT
typedef struct CiSubaddrInfo_struct {
	CiBoolean Present;                      /* Subaddress Info present? */
	UINT8 Type;                             /* Subaddress Type */
	UINT8 Length;                           /* Subaddress Length in characters */
	UINT8 Digits[ CI_MAX_SUBADDR_LENGTH ];  /* Subaddress Digits */
} CiSubaddrInfo;

/* Name Information structure - used for Calling Party Name, Called Party Name, Phonebook, etc. */
#define CI_MAX_NAME_LENGTH  32

//ICAT EXPORTED STRUCT
typedef struct CiNameInfo_struct {
	UINT8 Length;                           /* Name Length in characters */
	UINT8 Name[ CI_MAX_NAME_LENGTH ];       /* Alphanumeric Name */
} CiNameInfo;

/* Optional Name Information structure - used for Calling Number, Called Number, etc. */
//ICAT EXPORTED STRUCT
typedef struct CiOptNameInfo_struct {
	CiBoolean Present;      /* Name Info present? */
	CiNameInfo NameInfo;    /* Name Info */
} CiOptNameInfo;

/* Calling Party (Caller) Information - for CLIP and CoLP indications, Call Info, etc. */
//ICAT EXPORTED STRUCT
typedef struct CiCallerInfo_struct {
	CiOptAddressInfo OptCallerNumber;       /* Optional Caller Phone Number */
	CiSubaddrInfo OptCallerSubaddr;         /* Optional Caller Subaddress */
	CiOptNameInfo OptCallerName;            /* Optional Caller Name (Alpha Tag) */
} CiCallerInfo;

/* Supplementary Service Intermediate (SSI) Notification Information: Call Status values.
 * These are defined as listed for <code1> in the "AT+CSSN" command (see [3]), but the
 * Protocol Stack may not support all of them.
 */
//ICAT EXPORTED ENUM
typedef enum SSI_CALL_STATUS {
	SSI_CS_CFU_ACTIVE = 0,  /* Unconditional Call Forwarding active */
	SSI_CS_CCF_ACTIVE,      /* Conditional CF trigger(s) active */
	SSI_CS_FORWARDED,       /* Call has been forwarded */
	SSI_CS_WAITING,         /* Call is waiting */
	SSI_CS_CUG_CALL,        /* Outgoing CUG call (Index also present) */
	SSI_CS_MO_BARRED,       /* Outgoing (MO) calls are barred */
	SSI_CS_MT_BARRED,       /* Incoming (MT) calls are barred */
	SSI_CS_CLIR_REJECTED,   /* CLIR suppression rejected */
	SSI_CS_DEFLECTED,       /* Call has been deflected */
	/* This one must always be last in the list! */
	SSI_NUM_CS              /* Number of SSI Call Status values */
} _CiSsiCallStatus;

typedef UINT8 CiSsiCallStatus;

/* Supplementary Service Intermediate (SSI) Notification Information */
//ICAT EXPORTED STRUCT
typedef struct CiSsiNotifyInfo_struct {
	CiSsiCallStatus Status; /* Call Status indicator */
	UINT8 CugIndex;         /* CUG Index (if applicable) */
} CiSsiNotifyInfo;

/* Supplementary Service Unsolicited (SSU) Notification Information: Call Status values.
 * These are defined as listed for <code2> in the "AT+CSSN" command (see [3]), but the
 * Protocol Stack may not support all of them.
 */
//ICAT EXPORTED ENUM
typedef enum SSU_CALL_STATUS {
	SSU_CS_FORWARDED = 0,           /* Call has been forwarded */
	SSU_CS_CUG_CALL,                /* Incoming CUG call (Index also present) */
	SSU_CS_HELD,                    /* Call held (Voice Call) */
	SSU_CS_UNHELD,                  /* Call retrieved (Voice Call) */
	SSU_CS_ENTERED_MPTY,            /* Multiparty call entered (Voice call) */
	SSU_CS_RELEASED_HELD,           /* Held call released (Voice call) */
	SSU_CS_FORWARD_CHECK,           /* Forward Check SS message received */
	SSU_CS_ECT_ALERTING,            /* Connecting with remote party in ECT */
	SSU_CS_ECT_CONNECTED,           /* Connected with remote party in ECT */
					/* (Caller Information may also be present) */
	SSU_CS_DEFLECTED,               /* Call has been deflected  */
	SSU_CS_INCOMING_FORWARDED,      /* Additional incoming call forwarded */
	/* This one must always be last in the list! */
	SSU_NUM_CS                      /* Number of SSU Call Status values */
} _CiSsuCallStatus;

typedef UINT8 CiSsuCallStatus;

/* Supplementary Service Unsolicited (SSU) Notification Information */
//ICAT EXPORTED STRUCT
typedef struct CiSsuNotifyInfo_struct {
	CiSsuCallStatus Status;         /* Call Status indicator */
	UINT8 CugIndex;                 /* CUG Index (if applicable) */
	CiCallerInfo CallerInfo;        /* Optional Caller Information */
					/* (Caller Name info is never present) */
} CiSsuNotifyInfo;

/* These limits will almost certainly be defined elsewhere, but they are put here for convenience! */
#define CI_MAX_USSD_LENGTH  229 /* Maximum USS Data length (characters) */

/* Data Bearer Service Type: Data Speed indicators (used by CC and MM Service Groups) */
//ICAT EXPORTED ENUM
typedef enum CI_BSTYPE_SPEED {
	CI_BSTYPE_SPEED_AUTOBAUD = 0, /* Autobauding for 3.1 kHz Non-Transparent  */

	/* 3.1 kHz Audio definitions */
	CI_BSTYPE_SPEED_300_V21,        /* 300 bps (V.21)       */
	CI_BSTYPE_SPEED_1200_V22,       /* 1200 bps (V.22)      */
	CI_BSTYPE_SPEED_1200_75_V23,    /* 1200/75 bps (V.23)   */
	CI_BSTYPE_SPEED_2400_V22BIS,    /* 2400 bps (V.22bis)   */
	CI_BSTYPE_SPEED_2400_V26TER,    /* 2400 bps (V.26ter)   */
	CI_BSTYPE_SPEED_4800_V32,       /* 4800 bps (V.32)      */
	CI_BSTYPE_SPEED_9600_V32,       /* 9600 bps (V.32)      */

	/* V.34 definitions */
	CI_BSTYPE_SPEED_9600_V34,       /* 9600 bps   */
	CI_BSTYPE_SPEED_14400_V34,      /* 14400 bps  */
	CI_BSTYPE_SPEED_19200_V34,      /* 19200 bps  */
	CI_BSTYPE_SPEED_28800_V34,      /* 28800 bps  */
	CI_BSTYPE_SPEED_33600_V34,      /* 33600 bps  */

	/* V.34 definitions */
	CI_BSTYPE_SPEED_1200_V120,      /* 1200 bps   */
	CI_BSTYPE_SPEED_2400_V120,      /* 2400 bps   */
	CI_BSTYPE_SPEED_4800_V120,      /* 4800 bps   */
	CI_BSTYPE_SPEED_9600_V120,      /* 9600 bps   */
	CI_BSTYPE_SPEED_14400_V120,     /* 14.4 kbps  */
	CI_BSTYPE_SPEED_19200_V120,     /* 19.2 kbps  */
	CI_BSTYPE_SPEED_28800_V120,     /* 28.8 kbps  */
	CI_BSTYPE_SPEED_38400_V120,     /* 38.4 kbps  */
	CI_BSTYPE_SPEED_48000_V120,     /* 48.0 kbps  */
	CI_BSTYPE_SPEED_56000_V120,     /* 56.0 kbps  */

	/* V.110 or X.31 Flag Stuffing UDI definitions */
	CI_BSTYPE_SPEED_300_V110,       /* 300 bps    */
	CI_BSTYPE_SPEED_1200_V110,      /* 1200 bps   */
	CI_BSTYPE_SPEED_2400_V110_X31,  /* 2400 bps   */
	CI_BSTYPE_SPEED_4800_V110_X31,  /* 4800 bps   */
	CI_BSTYPE_SPEED_9600_V110_X31,  /* 9600 bps   */
	CI_BSTYPE_SPEED_14400_V110_X31, /* 14.4 kbps  */
	CI_BSTYPE_SPEED_19200_V110_X31, /* 19.2 kbps  */
	CI_BSTYPE_SPEED_28800_V110_X31, /* 28.8 kbps  */
	CI_BSTYPE_SPEED_38400_V110_X31, /* 38.4 kbps  */
	CI_BSTYPE_SPEED_48000_V110_X31, /* 48.0 kbps  */
	CI_BSTYPE_SPEED_56000_V110_X31, /* 56.0 kbps (can also get FTM) */
	CI_BSTYPE_SPEED_64000_X31,      /* 64.0 kbps (can also get FTM) */

	/* Bit Transparent Mode definitions */
	CI_BSTYPE_SPEED_56000_BTM,      /* 56 kbps  */
	CI_BSTYPE_SPEED_64000_BTM,      /* 64 kbps  */

	/* PIAFS (Personal Internet Access Forum Standard) definitions */
	CI_BSTYPE_SPEED_32000_PIAFS,    /* 56.0 kbps (PIAFS32k) */
	CI_BSTYPE_SPEED_64000_PIAFS,    /* 64.0 kbps (PIAFS64k) */

	/* Multimedia Call definitions */
	CI_BSTYPE_SPEED_28800_MM,       /* 28.8 kbps  */
	CI_BSTYPE_SPEED_32000_MM,       /* 32.0 kbps  */
	CI_BSTYPE_SPEED_33600_MM,       /* 33.6 kbps  */
	CI_BSTYPE_SPEED_56000_MM,       /* 56 kbps    */
	CI_BSTYPE_SPEED_64000_MM,       /* 64 kbps    */

	/* This one must always be last in the list! */
	CI_NUM_BSTYPE_SPEEDS          /* Number of Bearer Service Speeds defined */
} _CiBsTypeSpeed;
typedef UINT8 CiBsTypeSpeed;

/* CI minimum password size */
#define CI_MIN_PASSWORD_LENGTH  4 /* Minimum password length (digits) */

/* CI maximum password size */
#define CI_MAX_PASSWORD_LENGTH 16

/* CI password structure */
//ICAT EXPORTED STRUCT
typedef struct CiPassword_struct {
	UINT8 len;      /* length of the password, [CI_MIN_PASSWORD_LENGTH - CI_MAX_PASSWORD_LENGTH] */
#ifndef CCI_CMN_CONTIGUOUS
	UINT8  *pdata;  /* pointer to password values, only ascii digits are allowed */
#else
	UINT8 data[CI_MAX_PASSWORD_LENGTH];
#endif

}CiPassword;

/* CI common char string structure */
//ICAT EXPORTED STRUCT
typedef struct CiString_struct {
	UINT16 len;
#ifndef CCI_CMN_CONTIGUOUS
	CHAR*   val;
#else
	CHAR valStr[CI_MAX_CI_STRING_LENGTH];
#endif

}CiString;
/* Michal Bukai & Boris Tsatkin – AT&T Smart Card support */
/*Added for: AT&T- Smart Card  <CDR-SMCD-911> PB     EMAIL -BT45 */
#define SIZEOF_CiString (sizeof(CiString))
/* CI numeric range structure */
//ICAT EXPORTED STRUCT
typedef struct CiNumericRange_struct {
	UINT32 min;     /* lower limit */
	UINT32 max;     /* upper limit */
}CiNumericRange;

/* CI bit range structure, each bit in this bit mask represents an object */
typedef UINT32 CiBitRange; /* each bit represents a capability */

/* CI numeric list structure */
//ICAT EXPORTED STRUCT
typedef struct CiNumericList_struct {
	CiBoolean hasRange;     /* range of numbers are included */
	CiBoolean hasIndvNums;  /* individual numbers are included */
	UINT16 rangeLstSize;  /* actual number of CiNumericRange structures held in the range list,
				 ignored if hasRange is FALSE */
#ifndef CCI_CMN_CONTIGUOUS
	CiNumericRange* pRangeLst; /* pointer to a list of CiNumericRange structures, NULL if hasRange
				 is FALSE */
#else
	CiNumericRange rangeLst[CI_MAX_CI_NUMERIC_RANGE_LIST_SIZE];
#endif
	UINT16 indvLstSize;   /* actual number of numbers held in the individual number list,
				 ignored if hasIndNums is FALSE */
#ifndef CCI_CMN_CONTIGUOUS
	UINT32* pIndvList;    /* pointer to a list of UINT32 numbers, NULL if hasIndNums is FALSE */
#else
	UINT32 indvList[CI_MAX_INDV_LIST_SIZE];
#endif

} CiNumericList;

/* CI access error types */
//ICAT EXPORTED ENUM
typedef enum CIERRACCTYPE {
	CIERR_ACC_TYPE_CHV1,            /* CHV1 password */
	CIERR_ACC_TYPE_CHV2,            /* CHV2 password */
	CIERR_ACC_TYPE_UNBLOCK_CHV1,    /* Unblock CHV1 password */
	CIERR_ACC_TYPE_UNBLOCK_CHV2,    /* Unblock CHV2 password */
	CIERR_ACC_TYPE_ADMIN,           /* Administrate access right */
	CIERR_ACC_TYPE_CB,              /* call barring password */
	CIERR_ACC_TYPE_INTERNAL         /* internal access type */
} _CiErrAccType;

typedef UINT8 CiErrAccType;

/* CI access error causes */
//ICAT EXPORTED ENUM
typedef enum CIERRACCCAUSE {
	CIERR_ACC_PRIV_REQUIRED,        /* access privilege (e.g. password) is required to gain access right */
	CIERR_ACC_PW_NOT_INIT,          /* password is not initialized or registered */
	CIERR_ACC_PW_BLOCKED,           /* password was blocked because of a series of unsuccessful verification attempts */
	CIERR_ACC_RES_UNAVAIL,          /* required resource is unavailable */
	CIERR_ACC_UNKNOWN               /* unknown error access cause */
} _CiErrAccCause;

typedef UINT8 CiErrAccCause;

/* COMMON ERROR RESULT CODE */
//ICAT EXPORTED ENUM
typedef enum CIERRINPUTCODE {
	/* wrong inputs */
	CIRC_ERR_WRONG_PASSWORD = 0xF000,       /* wrong password */
	CIRC_ERR_BAD_PARAMETER,                 /* parameter invalid or out of range */
	CIRC_ERR_WRONG_INDEX,                   /* invalid index */
	CIRC_ERR_LONG_STR,                      /* string too long */
	CIRC_ERR_WRONG_CHAR,                    /* invalid characters in text string */
	CIRC_ERR_LONG_DIALSTR                   /* dial string too long */
} _CiErrInputCode;

typedef UINT16 CiErrInputCode;

/* CI inter subsystem link error causes */
//ICAT EXPORTED ENUM
typedef enum CIERRINTERLINKCAUSE {
	CIERR_INTERLINK_DOWN,           /* interlink broken, receiver's action TBD  */
	CIERR_INTERLINK_MEM_OVERFLOW    /* memory overflow in the link, receiver's action TBD */
} _CiErrInterLinkCause;

typedef UINT8 CiErrInterLinkCause;

typedef UINT8 CiEmptyPrim;

/* Michal Bukai & Boris Tsatkin – AT&T Smart Card support */
/*** AT&T- Smart Card        -BT1 ; -BT6*/
#define CI_LONG_ADR_LENGTH 50
//ICAT EXPORTED STRUCT
typedef struct CiLongAdrInfo_tag {
	UINT8 Length;                           /* Name Length in characters */
	UINT8 Name[ CI_LONG_ADR_LENGTH ];       /* Alphanumeric Name */
} CiLongAdrInfo;

//ICAT EXPORTED ENUM
typedef enum CI_EDIT_CMD_ACTION {
	CIED_ADD = 0,
	CIED_DELETE = 1,
	CIED_REPLACE = 2,
}_CiEditCmdType;                /*AT&T- Smart Card  -BT1 ****/
typedef UINT8 CiEditCmdType;    /*AT&T- Smart Card  -BT1 ****/
#include "ci_err.h"

#ifdef __cplusplus
}
#endif  //__cplusplus

#endif  /* _CI_API_TYPES_H_ */


/*                      end of ci_api_types.h
   --------------------------------------------------------------------------- */




