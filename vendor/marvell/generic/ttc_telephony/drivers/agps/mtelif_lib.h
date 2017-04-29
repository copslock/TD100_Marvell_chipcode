/*------------------------------------------------------------
(C) Copyright [2006-2008] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/


/*
 * mtelif_lib.h
 *
 * Use to provide telephony interface to application layer
 */
#ifndef __MTELIF_H__
#define __MTELIF_H__

#include <stddef.h>
#include "linux_types.h"

typedef enum
{
	ACT_GSM = 0,
	ACT_TDSCDMA = 0,
	ACT_GSM_COMPACT,		/* is not supported */
	ACT_UTRAN,

	NUM_ACT
} AccTechMode;

typedef void (*onMTSmsPayload)(const unsigned char *sms_payload, int dcs, int length);

// open the telephony interface, expecting -d </dev/tty_device>
int mtelif_open(int argc, char **argv);
 
// close the telephony interface
int mtelif_close();

// establish PS connection if there is none, use existing connection if there is one
int EstablishPSconnection(unsigned char* apn, unsigned char* userid, unsigned char* password, int usePPP);
int DisconnectGPRSconnection();
int IsPSconnectionEstablish();

// register for MT Sms payload notification
int RegisterMTSmsPayloadNotification(onMTSmsPayload smsCb);

// register for MT Sms SUPL payload notification
// int RegisterMTSmsSUPLPayloadNotification(onMTSmsPayload smsCb);

typedef struct CellInfo_struct
{
	unsigned int  CountryCode;		/* 3-digit Mobile Country Code */
	unsigned int  NetworkCode;		/* 3-digit Mobile Network Code */
	unsigned int  lac;				/* Location Area Code */
	unsigned int  cellId;			/* Cell ID */
} MCellInfo;

// Support for NITZ (Network Informed Time Zone)
typedef struct _AGPS_NITZ_STATUS_MSG {
	unsigned long		ret_code;		// 0 = OK, >0 = Error Code
	unsigned long		year;			// Range: 0..999 (units: years since 1/1/2000)
	unsigned long		month;			// Range: 1..12
	unsigned long		day;			// Range: 1..31
	unsigned long		hour;			// Range: 0..23
	unsigned long		minute;			// Range: 0..59
	unsigned long		sec;			// Range: 0..59
	signed   long		time_zone;		// Range: -48..+47 (units: 15 minutes / 0.25 hour)
	unsigned long		uncertainty;	// Range: 0..1000 (units: seconds), 0=Unknown/Not defined
} AGPS_NITZ_STATUS_MSG, *P_AGPS_NITZ_STATUS_MSG;

int GetCurCellInfo(MCellInfo *pInfo);

int GetIMSI(unsigned char* imsi_str);

int GetAccTechMode(AccTechMode* acc_tech_mode);
int isTDPlatform(void);
int UpaClient_GetNitzInfo(AGPS_NITZ_STATUS_MSG* pNitzStatus);

int MRIL_Client_Init(void);
int MRIL_Client_DeInit(void);

//ICAT EXPORTED ENUM
typedef enum CIRCDEV_TAG {
	CIRC_DEV_SUCCESS = 0,           /**< request completed successfully */
	CIRC_DEV_FAILURE,               /**< phone failure */
	CIRC_DEV_NO_CONNECTION,         /**< no connection to phone */
	CIRC_DEV_UNKNOWN,               /**< unknown error */

	CIRC_DEV_NUM_RESCODES
} _CiDevRc;

#define CI_DEV_MAX_APGS_MSG_SIZE		500
#define CI_DEV_MAX_NUM_NWDL_MSG_IND     4
/**@}*/

/** \brief  RRC state */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CiDEVLPRRCSTATE_TAG
{
	CI_DEV_LP_RRC_STATE_CELL_DCH = 0,
	CI_DEV_LP_RRC_STATE_CELL_FACH,
	CI_DEV_LP_RRC_STATE_CELL_PCH,
	CI_DEV_LP_RRC_STATE_URA_PCH,
	CI_DEV_LP_RRC_STATE_IDLE
} _CiDevLpRRCState;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief RRC state
 * \sa CiDEVLPRRCSTATE_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiDevLpRRCState;
/**@}*/

/** \brief  Radio bearer type */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIDEVLPBEARERTYPE_TAG
{
	CI_DEV_LP_RRC = 0,
	CI_DEV_LP_RRLP,
	CI_DEV_LP_RRC_SIB15,
	CI_DEV_LP_SS_AGPS_ASSIST_REQ,
	CI_DEV_LP_NUM_OF_BEARER_TYPE
} _CiDevLpBearerType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Radio bearer type
 * \sa CIDEVLPBEARERTYPE_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiDevLpBearerType;
/**@}*/

/** \brief  Emergency Call Indication */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CiDEVLPSESSIONTYPE_TAG
{
	CI_DEV_LP_NO_EMERGENCY = 0x0,
	CI_DEV_LP_EMERGENCY
}_CiDevLpSessionType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Emergency Call Indication
 * \sa CiDEVLPSESSIONTYPE_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiDevLpSessionType;
/**@}*/

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_LP_NWDL_MSG_IND">   */
typedef struct CiDevPrimLpNwdlMsgInd_struct {
	CiDevLpBearerType BearerType;                                                                                                   /**< Bearer Type info. \sa CiDevLpBearerType */
	UINT8 msg_data[CI_DEV_MAX_APGS_MSG_SIZE];                                                                                       /**< Encoded RRLP/RRC data. */
	UINT32 msg_size;                                                                                                                /**< Size of data  */
	CiDevLpSessionType sessionType;                                                                                                 /**< Session type info . \sa CiDevLpSessionType */
	CiDevLpRRCState RrcState;                                                                                                       /**< RRC state. \sa CiDevLpRRCState */
	UINT8 count;                                                                                                                    /**< Ordinal number of the message. [range: 0-3] */
} CiDevPrimLpNwdlMsgInd;

typedef    UINT8 CiBoolean;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_LP_NWUL_MSG_REQ">   */
typedef struct CiDevPrimLpNwulMsgReq_struct {
	UINT8 msg_data[CI_DEV_MAX_APGS_MSG_SIZE];                                                                                       /**< The OTA message */
	UINT32 msg_data_len;                                                                                                            /**< Specifies the number of bytes contained in msg_data */
	UINT8 count;                                                                                                                    /**< Ordinal number of the message. [range: 0-3] */
	CiDevLpBearerType bearer_type;                                                                                                  /**< Radio bearer type. \sa CiDevLpBearerType */
	CiBoolean isFinalResponse;                                                                                                      /**< isFinalResponse Flag. \sa CCI API Ref Manual */
}CiDevPrimLpNwulMsgReq;

typedef UINT16 CiDevRc;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_LP_NWUL_MSG_CNF">   */
typedef struct CiDevPrimLpNwulMsgCnf_struct {
	CiDevRc result;                 /**< Result code. \sa CiDevRc. */
	UINT8 res1U8[2];                /**< (padding) */
}CiDevPrimLpNwulMsgCnf;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_LP_NWDL_MSG_IND">   */
typedef struct CiDevPrimLpRrcStateInd_struct {
	CiDevLpBearerType bearer_type;                          /**< Radio bearer type. \sa CiDevLpBearerType */
	CiDevLpRRCState rrc_state;                              /**< Current RRC state. \sa CiDevLpRRCState */
	UINT8 res1U8[2];                                        /**< (padding) */
}CiDevPrimLpRrcStateInd;

/** <paramref name="CI_DEV_PRIM_LP_MEAS_TERMINATE_IND">   */
//ICAT EXPORTED STRUCT
typedef struct CiDevPrimLpMeasTerminateInd_struct {
	CiDevLpBearerType bearer_type;                          /**< Radio bearer type. \sa CiDevLpBearerType */
	UINT8 res1U8[3];                                        /**< (padding) */
}CiDevPrimLpMeasTerminateInd;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_LP_RESET_STORED_UE_POS_IND">   */
typedef struct CiDevPrimLpResetStoreUePosInd_struct {
	CiDevLpBearerType bearer_type;                          /**< Radio bearer type. \sa CiDevLpBearerType */
	UINT8 res1U8[3];                                        /**< (padding) */
}CiDevPrimLpResetStoreUePosInd;

#endif
