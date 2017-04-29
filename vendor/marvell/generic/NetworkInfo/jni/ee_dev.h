/*
 * * (C) Copyright 2010 Marvell International Ltd.
 * * All Rights Reserved
 * *
 * * MARVELL CONFIDENTIAL
 * * Copyright 2008 ~ 2010 Marvell International Ltd All Rights Reserved.
 * * The source code contained or described herein and all documents related to
 * * the source code ("Material") are owned by Marvell International Ltd or its
 * * suppliers or licensors. Title to the Material remains with Marvell International Ltd
 * * or its suppliers and licensors. The Material contains trade secrets and
 * * proprietary and confidential information of Marvell or its suppliers and
 * * licensors. The Material is protected by worldwide copyright and trade secret
 * * laws and treaty provisions. No part of the Material may be used, copied,
 * * reproduced, modified, published, uploaded, posted, transmitted, distributed,
 * * or disclosed in any way without Marvell's prior express written permission.
 * *
 * * No license under any patent, copyright, trade secret or other intellectual
 * * property right is granted to or conferred upon you by disclosure or delivery
 * * of the Materials, either expressly, by implication, inducement, estoppel or
 * * otherwise. Any license under such intellectual property rights must be
 * * express and approved by Marvell in writing.
 * *
 * */

#ifndef EE_DEV_H
#define EE_DEV_H 1
 
#ifdef __cplusplus
 extern "C" {
#endif

typedef unsigned char BOOL;
typedef int UINT8;
typedef int UINT16;
typedef int UINT32;

typedef signed char CHAR;
typedef int INT8;
typedef int INT16;
typedef int INT32;
typedef char CiBoolean;



/** \brief GMM Reject cause (10.5.3.6) sent to MM from the network  */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIDEV_ENGMODE_GMM_REJ_CAUSE_CODE_TYPE {
	CI_DEV_EM_GMM_REJ_CAUSE_GPRS_SERVICE_NOT_ALLOWED = 7,
	CI_DEV_EM_GMM_REJ_CAUSE_GPRS_SERVICE_AND_NON_GPRS_SERVICE_NOT_ALLOWED = 8,
	CI_DEV_EM_GMM_REJ_CAUSE_MS_IDENTITY_CANNOT_BE_DERIVED_BY_NW = 9,
	CI_DEV_EM_GMM_REJ_CAUSE_IMPLICITLY_DETACHED = 10,
	CI_DEV_EM_GMM_REJ_CAUSE_GPRS_SERVICES_NOT_ALLOWED_IN_PLMN = 14,
	CI_DEV_EM_GMM_REJ_CAUSE_MSC_TEMPORARILY_NOT_REACHABLE = 16,
	CI_DEV_EM_GMM_REJ_CAUSE_NO_PDP_CONTEXT_ACTIVATED = 40,
	CI_DEV_EM_GMM_REJ_CAUSE_ALIGN_32_BIT = 0XFFFFFFF
} _CiDevEngGMMRejectCauseCodeType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief GMM Reject cause (10.5.3.6) sent to MM from the network
 * \sa CIDEV_ENGMODE_GMM_REJ_CAUSE_CODE_TYPE */
/** \remarks Common Data Section */
typedef UINT32 DevEngGMMRejectCauseCodeType;


/** \brief Reject cause (10.5.3.6) sent in CM Service Reject, Abort, MM-Status and Location Updating Reject messages to MM from the network */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIDEV_ENGMODE_MM_REJECTCAUSE_CODE_TYPE {
	CI_DEV_EM_MM_REJ_CAUSE_IMSI_UNKNOWN_IN_HLR = 2,                                                                                                 /**< MS identification cause */
	CI_DEV_EM_MM_REJ_CAUSE_ILLEGAL_MS = 3,                                                                                                          /**< MS identification cause */
	CI_DEV_EM_MM_REJ_CAUSE_IMSI_UNKNOWN_IN_VLR = 4,                                                                                                 /**< MS identification cause */
	CI_DEV_EM_MM_REJ_CAUSE_IMEI_NOT_ACCEPTED = 5,                                                                                                   /**< MS identification cause */
	CI_DEV_EM_MM_REJ_CAUSE_ILLEGAL_ME = 6,                                                                                                          /**< MS identification cause */
	CI_DEV_EM_MM_REJ_CAUSE_PLMN_NOT_ALLOWED = 11,                                                                                                   /**< Subscription options */
	CI_DEV_EM_MM_REJ_CAUSE_LOCATION_AREA_NOT_ALLOWED = 12,                                                                                          /**< Subscription options */
	CI_DEV_EM_MM_REJ_CAUSE_ROAMING_NOT_ALLOWED_IN_THIS_LOCATION_AREA = 13,                                                                          /**< Subscription options */
	CI_DEV_EM_MM_REJ_CAUSE_NO_SUITABLE_CELLS_IN_LOCATION_AREA = 15,                                                                                 /**< Subscription options */
	CI_DEV_EM_MM_REJ_CAUSE_NETWORK_FAILURE = 17,                                                                                                    /**< PLMN specific network failures and congestion/Authentication Failures*/
	CI_DEV_EM_MM_REJ_CAUSE_MAC_FAILURE = 20,                                                                                                        /**< PLMN specific network failures and congestion/Authentication Failures*/
	CI_DEV_EM_MM_REJ_CAUSE_SYNC_FAILURE = 21,                                                                                                       /**< PLMN specific network failures and congestion/Authentication Failures*/
	CI_DEV_EM_MM_REJ_CAUSE_CONGESTION = 22,                                                                                                         /**< PLMN specific network failures and congestion/Authentication Failures*/
	CI_DEV_EM_MM_REJ_CAUSE_GSM_AUTHENTICATION_UNACCEPTABLE = 23,                                                                                    /**< PLMN specific network failures and congestion/Authentication Failures */
	CI_DEV_EM_MM_REJ_CAUSE_SERVICE_OPTION_NOT_SUPPORTED = 32,                                                                                       /**< Nature of request */
	CI_DEV_EM_MM_REJ_CAUSE_REQUEST_SERVICE_OPTION_NOT_SUBSCRIBED = 33,                                                                              /**< Nature of request */
	CI_DEV_EM_MM_REJ_CAUSE_SERVICE_OPTION_TEMPORARILY_OUT_OF_ORDER = 34,                                                                            /**< Nature of request */
	CI_DEV_EM_MM_REJ_CAUSE_CALL_CANNOT_BE_IDENTIFIED = 38,                                                                                          /**< Nature of request */
	CI_DEV_EM_MM_REJ_CAUSE_SEMANTICALLY_INCORRECT_MESSAGE = 95,                                                                                     /**< Invalid message */
	CI_DEV_EM_MM_REJ_CAUSE_INVALID_MANDATORY_INFORMATION = 96,                                                                                      /**< Invalid message */
	CI_DEV_EM_MM_REJ_CAUSE_MESSAGE_TYPE_NONEXISTENT_OR_NOT_IMPLEMENTED = 97,                                                                        /**< Invalid message */
	CI_DEV_EM_MM_REJ_CAUSE_MESSAGE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE = 98,                                                                         /**< Invalid message */
	CI_DEV_EM_MM_REJ_CAUSE_INFORMATION_ELEMENT_NONEXISTENT_OR_NOT_IMPLEMENTED = 99,                                                                 /**< Invalid message */
	CI_DEV_EM_MM_REJ_CAUSE_CONDITIONAL_IE_ERROR = 100,                                                                                              /**< Invalid message */
	CI_DEV_EM_MM_REJ_CAUSE_PROTOCOL_ERROR_UNSPECIFIED = 111,                                                                                        /**< Invalid message */
	CI_DEV_EM_MM_REJ_CAUSE_CI_DEV_ALIGN_32_BIT = 0XFFFFFFF                                                                                          /**< is used for allignment */

} _CiDevEngMMRejectCauseCodeType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Reject cause (10.5.3.6) sent in CM Service Reject, Abort, MM-Status and Location Updating Reject messages to MM from the network
 * \sa CIDEV_ENGMODE_MM_REJECTCAUSE_CODE_TYPE */
/** \remarks Common Data Section */
typedef UINT32 DevEngMMRejectCauseCodeType;


/** \brief Engineering Mode: Mode Type/State  */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIDEV_ENGMODE_STATE_TAG
{
	CI_DEV_EM_GSM_IDLE_STATE = 0,                   /**< Mode is : GSM in Idle */
	CI_DEV_EM_GSM_DEDICATED_STATE,                  /**< Mode is : GSM in dedicated */
	CI_DEV_EM_GPRS_EGPRS_PTM_STATE,                 /**< Mode is : GSM + at least one PDP Ctx is activated*/
	CI_DEV_EM_INVALID_STATE,

	/* This must be the last entry */
	CI_DEV_NUM_EM_STATES
} _CiDevEngModeState;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Engineering Mode: Mode Type/State.
 * \sa CIDEV_ENGMODE_STATE_TAG */
/** \remarks Common Data Section */
typedef UINT8 DevEngModeState;
/**@}*/

/** \brief Engineering Mode: Network Type */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIDEV_ENGMODE_NETWORK_TAG
{
	CI_DEV_EM_NETWORK_GSM = 0,              /**< GSM Network */
	CI_DEV_EM_NETWORK_UMTS,                 /**< UMTS Network */

	/* This must be the last entry */
	CI_DEV_NUM_EM_NETWORKS
} _CiDevEngModeNetwork;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Engineering Mode: Network Type.
 * \sa CIDEV_ENGMODE_NETWORK_TAG */
/** \remarks Common Data Section */
typedef UINT8 DevEngModeNetwork;


/** \brief Engineering Mode: 3G (UMTS) Serving Cell Measurements structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct DevUmtsServingCellMeasurements_struct
{
	INT16 cpichRSCP;                /**<    CPICH Received Signal Code Power; In UMTS FDD messages RSCP is
						transmitted as an integer value in the range of -120 dBm to -25 dBm.
						The value is coded into integers from -5 to 99 according to 3GPP's 25.133  */
	INT16 utraRssi;                         /**<    UTRA Carrier RSSI.range 0 - 63.
									UTRA_carrier_RSSI_LEV _00: UTRA carrier RSSI < -94 dBm
									UTRA_carrier_RSSI_LEV _01: -94 dBm  ?UTRA carrier RSSI < -93 dBm
									UTRA_carrier_RSSI_LEV _02: -93 dBm  ?UTRA carrier RSSI < -92 dBm
									UTRA_carrier_RSSI_LEV _61: -32 dBm  ?UTRA carrier RSSI < -33 dBm
									UTRA_carrier_RSSI_LEV _62: -33 dBm  ?UTRA carrier RSSI < -32 dBm
									UTRA_carrier_RSSI_LEV _63: -32 dBm  ?UTRA carrier RSSI	   */
	INT16 cpichEcN0;                        /**< CPICH Ec/N0 */
	INT16 sQual;                            /**< Cell Selection Quality (Squal) */
	INT16 sRxLev;                           /**< Cell Selection Rx Level (Srxlev) */
	INT16 txPower;                          /**< UE Transmitted Power */

} DevUmtsServingCellMeasurements;

/** \brief Engineering Mode: 3G (UMTS) Serving Cell Measurements structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct DevTDServingCellMeasurements_struct
{
	INT16 pccpchRSCP;         /**<  PCCPCH Received Signal Code Power; In UMTS TDD messages RSCP is
						transmitted as an integer value in the range of -120 dBm to -25 dBm.
						The value is coded into integers from -5 to 99 according to 3GPP's 25.133  */
	INT16 utraRssi;                         /**<    UTRA Carrier RSSI.range 0 - 63.
									UTRA_carrier_RSSI_LEV _00: UTRA carrier RSSI < -94 dBm
									UTRA_carrier_RSSI_LEV _01: -94 dBm  ?UTRA carrier RSSI < -93 dBm
									UTRA_carrier_RSSI_LEV _02: -93 dBm  ?UTRA carrier RSSI < -92 dBm
									UTRA_carrier_RSSI_LEV _61: -32 dBm  ?UTRA carrier RSSI < -33 dBm
									UTRA_carrier_RSSI_LEV _62: -33 dBm  ?UTRA carrier RSSI < -32 dBm
									UTRA_carrier_RSSI_LEV _63: -32 dBm  ?UTRA carrier RSSI	   */
	INT16 sRxLev;                           /**< Cell Selection Rx Level (Srxlev) */
	INT16 txPower;                          /**< UE Transmitted Power */

} DevTDServingCellMeasurements;





/** \brief Engineering Mode: 3G (UMTS) Serving Cell PLMN/Cell Parameters structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct DevUmtsServingCellParameters_struct
{
	UINT8 rac;                                      /**< Routing Area Code */
	UINT8 nom;                                      /**< Network Operation Mode */

	UINT16 mcc;                                     /**< Mobile Country Code */
	UINT16 mnc;                                     /**< Mobile Network Code */
	UINT16 lac;                                     /**< Location Area Code */
	UINT32 ci;                                      /**< Cell Identity; as per 3G TS 25.331, 10.3.2.2 (28 bits) */
	UINT16 uraId;                                   /**< URA Identity */
	UINT16 psc;                                     /**< Primary Scrambling Code */
	UINT16 arfcn;                                   /**< Absolute Radio Frequency Channel Number */

	UINT16 t3212;                                   /**< Periodic LA Update Timer (T3212) in minutes */
	UINT16 t3312;                                   /**< Periodic RA Update Timer (T3312) in minutes */

	CiBoolean hcsUsed;                              /**< Hierarchical Cell Structure used? \sa CCI API Ref Manual */
	CiBoolean attDetAllowed;                        /**< Attach-Detach allowed? \sa CCI API Ref Manual */


	UINT16 csDrxCycleLen;                   /**< CS Domain DRX Cycle Length */
	UINT16 psDrxCycleLen;                   /**< PS Domain DRX Cycle Length */
	UINT16 utranDrxCycleLen;                /**< UTRAN DRX Cycle Length */
	CiBoolean HSDPASupport;                 /**< TRUE - Serving Cell supports HSDPA; FALSE - other. \sa CCI API Ref Manual */
	CiBoolean HSUPASupport;                 /**< TRUE - Serving Cell supports HSUPA; FALSE - other. \sa CCI API Ref Manual */
} DevUmtsServingCellParameters;


/** \brief Engineering Mode: 3G (UMTS) Serving Cell PLMN/Cell Parameters structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct DevTDServingCellParameters_struct
{
	UINT8 rac;                                      /**< Routing Area Code */
	UINT8 nom;                                      /**< Network Operation Mode */

	UINT16 mcc;                                     /**< Mobile Country Code */
	UINT16 mnc;                                     /**< Mobile Network Code */
	UINT16 lac;                                     /**< Location Area Code */
	UINT32 ci;                                      /**< Cell Identity; as per 3G TS 25.331, 10.3.2.2 (28 bits) */
	UINT16 uraId;                                   /**< URA Identity */
	INT8 cellParameterId;                           /**< Cell parameter id */
	UINT8 res1U8;                                   /**< (padding) */
	UINT16 arfcn;                                   /**< Absolute Radio Frequency Channel Number */

	UINT16 t3212;                                   /**< Periodic LA Update Timer (T3212) in minutes */
	UINT16 t3312;                                   /**< Periodic RA Update Timer (T3312) in minutes */

	CiBoolean hcsUsed;                              /**< Hierarchical Cell Structure used? \sa CCI API Ref Manual */
	CiBoolean attDetAllowed;                        /**< Attach-Detach allowed? \sa CCI API Ref Manual */


	UINT16 csDrxCycleLen;                   /**< CS Domain DRX Cycle Length */
	UINT16 psDrxCycleLen;                   /**< PS Domain DRX Cycle Length */
	UINT16 utranDrxCycleLen;                /**< UTRAN DRX Cycle Length */
	CiBoolean HSDPASupport;                 /**< TRUE - Serving Cell supports HSDPA; FALSE - other. \sa CCI API Ref Manual */
	CiBoolean HSUPASupport;                 /**< TRUE - Serving Cell supports HSUPA; FALSE - other. \sa CCI API Ref Manual */
} DevTDServingCellParameters;

/** \brief Cipher algorithm type */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIPHER_ALGORITHM_TYPE
{
	CIPHER_ALGORITHM_TYPE_UEA0,          /**< as per 3G TS 25.331, 10.3.3.4 */
	CIPHER_ALGORITHM_TYPE_UEA1           /**< as per 3G TS 25.331, 10.3.3.4 */
} _CiCipherAlgorithmType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Cipher algorithm type
* \sa CI_CIPHER_ALGORITHM_TYPE */
/** \remarks Common Data Section */
typedef UINT8 CipherAlgorithmType;
/**@}*/

/** \brief Cipher algorithm info */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CipherAlgorithm_struct
{
	CiBoolean algPresent;                           /**< indicates if an algorithm is defined. \sa CCI API Ref Manual */
	CipherAlgorithmType cipherAlg;                /**< Cipher Algorithm type. \sa CiCipherAlgorithmType  */
	CiBoolean cipherOn;                             /* Ciphering Status = On/Off. \sa CCI API Ref Manual */
}  CipherAlgorithmInfo;

/** \brief Engineering Mode: 3G User Equipment (UE) RRC State */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIDEV_ENGMODE_UERRC_STATE_TAG
{
	CI_DEV_EM_UERRC_DEACTIVATED = 0,                                /**< (UE) RRC State DEACTIVATED RRC State */
	CI_DEV_EM_UERRC_SUSPENDED,                                      /**< (UE) RRC State SUSPENDED RRC State */
	CI_DEV_EM_UERRC_IDLE,                                           /**< (UE) RRC State IDLE RRC State */
	CI_DEV_EM_UERRC_CONN_URA_PCH,                                   /**< (UE) RRC State CONN_URA_PCH RRC State */
	CI_DEV_EM_UERRC_CONN_CELL_PCH,                                  /**< (UE) RRC State CONN_CELL_PCH RRC State */
	CI_DEV_EM_UERRC_CONN_CELL_FACH,                                 /**< (UE) RRC State DEACTIVATED RRC State */
	CI_DEV_EM_UERRC_CONN_CELL_DCH,                                  /**< (UE) RRC State CONN_CELL_FACH RRC State */

	/* This must be the last entry */
	CI_DEV_NUM_EM_UERRC_STATES
} _CiDevEngModeUeRrcState;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Engineering Mode: 3G User Equipment (UE) RRC State.
 * \sa CIDEV_ENGMODE_UERRC_STATE_TAG */
/** \remarks Common Data Section */
typedef UINT8 DevEngModeUeRrcState;


/** \brief Engineering Mode: 3G (UMTS) UE Operation Status structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct DevUmtsUeOperationStatus_struct
{
	DevEngModeUeRrcState rrcState;                                                                                /**< RRC State. \sa CiDevEngModeUeRrcState */
	UINT8 numLinks;                                                                                                 /**< Number of Radio Links */

	UINT16 srncId;                                                                                                  /**<  U-RNTI: SRNC Identifier */
	UINT32 sRnti;                                                                                                   /**<  U-RNTI: S-RNTI */
	CipherAlgorithmInfo csCipherInfo;                                                                             /**<  CS domain Ciphering info. \sa CiCipherAlgorithm_struct */
	CipherAlgorithmInfo psCipherInfo;                                                                             /**<  PS domain Ciphering info. \sa CiCipherAlgorithm_struct */
	CiBoolean HSDPAActive;                                                                                          /**<  TRUE- HSDPA is activated currently; FALSE - other.\sa CCI API Ref Manual  */
	CiBoolean HSUPAActive;                                                                                          /**<  TRUE- HSUPA is activated currently; FALSE - other.\sa CCI API Ref Manual  */
	UINT16 MccLastRegisteredNetwork;                                                                                /**<  Mcc of last registered network */
	UINT16 MncLastRegisteredNetwork;                                                                                /**<  Mnc of last registered network */
	INT32 TMSI;                                                                                                     /**<  TMSI */
	INT32 PTMSI;                                                                                                    /**<  PTMSI */
	CiBoolean IsSingleMmRejectCause;                                                                                /**<  TRUE - only one MM Reject cause reported during the last EngInfo period; FALSE - other. \sa CCI API Ref Manual  */
	CiBoolean IsSingleGmmRejectCause;                                                                               /**<  TRUE - only one GMM Reject cause reported during the last EngInfo period; FALSE - other. \sa CCI API Ref Manual  */
	DevEngMMRejectCauseCodeType MMRejectCause;                                                                    /**<  The one that was reported during the last EngInfo period Reject cause (10.5.3.6) sent in CM Service Reject, Abort, MM-Status and Location Updating Reject messages to MM from the network. \sa CiDevEngMMRejectCauseCodeType */
	DevEngGMMRejectCauseCodeType GMMRejectCause;                                                                  /**<  This one is, reported during the last EngInfo period, GMM Reject cause (10.5.3.6) sent to MM from the network.  \sa CiDevEngGMMRejectCauseCodeType */
	UINT8 mmState;                                                                                                  /**<  MM state defined in 4.1.2.1.1. For details see enum MmState in Mm_comm.h */
	UINT8 gmmState;                                                                                                 /**<  MM state defined in 4.1.2.1.1. For details see enum MmState in Mm_comm.h */
	UINT8 gprsReadyState;                                                                                           /**<  0 - IDLE_STATE / 1 - STANDBY_STATE / 2 - READY_STATE. For details see enum GprsReadyState in grrmrtyp.h */
	UINT16 readyTimerValueInSecs;                                                                                   /**<  MM ready timer value in sec [value >0] */
	UINT8 NumActivePDPContext;                                                                                      /**<  Number of active PDP Contexts */
	UINT32 ULThroughput;                                                                                            /**< UL throughput in octets per second */
	UINT32 DLThroughput;                                                                                            /**< DL throughput in octets per second */
} DevUmtsUeOperationStatus;


/** \brief  Engineering Mode 3G (UMTS) Intra-Frequency/Inter-Frequency FDD Cell Information structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiDevUmtsFddNeighborInfo_struct
{
	/* Measurements */
	INT16 cpichRSCP;                /**< CPICH Received Signal Code Power */
	INT16 utraRssi;                 /**< UTRA Carrier RSSI */
	INT16 cpichEcN0;                /**< CPICH Ec/N0 */
	INT16 sQual;                    /**< Cell Selection Quality (Squal) */
	INT16 sRxLev;                   /**< Cell Selection Rx Level (Srxlev) */

	/* PLMN/Cell Parameters */
	UINT16 mcc;                     /**< Mobile Country Code */
	UINT16 mnc;                     /**< Mobile Network Code */
	UINT16 lac;                     /**< Location Area Code */
	UINT16 ci;                      /**< Cell Identity */
	UINT16 arfcn;                   /**< Absolute Radio Frequency Channel Number */
	UINT16 psc;                     /**< Primary Scrambling Code */
} DevUmtsFddNeighborInfo;

typedef struct CiDevUmtsTddNeighborInfo_struct
{
	/* Measurements */
	INT16 pccpchRSCP;       /**< PCCPCH Received Signal Code Power */
	INT16 utraRssi;         /**< UTRA Carrier RSSI */
	INT16 sRxLev;           /**< Cell Selection Rx Level (Srxlev) */

	/* PLMN/Cell Parameters */
	UINT16 mcc;                     /**< Mobile Country Code */
	UINT16 mnc;                     /**< Mobile Network Code */
	UINT16 lac;                     /**< Location Area Code */
	UINT16 ci;                      /**< Cell Identity */
	UINT16 arfcn;                   /**< Absolute Radio Frequency Channel Number */
	INT8 cellParameterId;           /**< Cell parameter id */
	UINT8 res1U8[3];                /**< (padding) */
} DevUmtsTddNeighborInfo;

#define DEV_MAX_UMTS_NEIGHBORING_CELLS 32
#define DEV_MAX_GSM_NEIGHBORING_CELLS 6
#define DEV_MAX_REJECTCAUSE_HISTORY   10

//ICAT EXPORTED ENUM
typedef enum CIDEV_ENGMODE_CHANNEL_TYPE_TAG
{
	CI_DEV_EM_TCH_F             =   1,
	CI_DEV_EM_TCH_H            =   2,
	CI_DEV_EM_SDCCH_4        =   4,
	CI_DEV_EM_SDCCH_8        =   8
} _CiDevEngChannelType;

typedef UINT8 CiDevEngChannelType;

/** \brief Packet Idle type */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIDEV_PACKET_IDLE_TYPE_TAG
{
	CI_DEV_PACKET_IDLE_NONE = 0,
	CI_DEV_PACKET_IDLE_GPRS,
	CI_DEV_PACKET_IDLE_EDGE,

	/* This must be the last entry */
	CI_DEV_NUM_PACKET_IDLE
} _CiDevPacketIdleType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Packet Idle type.
 * \sa CIDEV_PACKET_IDLE_TYPE_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiDevPacketIdleType;
/**@}*/

/** \brief Cell priority values  */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIDEV_CELL_PRIORITY_TYPE_TAG {
	CI_DEV_CELL_PRIORITY_NORMAL = 0,                /**< Normal cell priority */
	CI_DEV_CELL_PRIORITY_BARRED,                    /**< Barred cell priority */
	CI_DEV_CELL_PRIORITY_LOW,                       /**< Low cell priority */
	CI_DEV_NUM_CELL_PRIORITY
} _CiDevCellPrioriytType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Cell priority values
 * \sa CIDEV_CELL_PRIORITY_TYPE_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiDevCellPrioriytType;
/**@}*/

/** \brief Engineering Mode: 2G (GSM) Serving Cell Info structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiDevGsmServingCellInfo_struct
{
	UINT8 rxSigLevel;                       /**< Receive Signal Level [range: 0h-3Fh] */
	UINT8 rxSigLevelFull;                   /**< Receive Signal Level accessed over all TDMA frames  [range: 0h-3Fh]*/
	UINT8 rxSigLevelSub;                    /**< Receive Signal Level accessed over subset of TDMA frames  [range: 0h-3Fh]*/
	UINT8 rxQualityFull;                    /**< Receive Quality accessed over all TDMA frames [range: 0-7] */
	UINT8 rxQualitySub;                     /**< Receive Quality accessed over subset of TDMA frames [range: 0-7] */
	UINT8 rac;                              /**< Routing Area Code [range: 0-1 (1 bit)] */
	UINT8 bsic;                             /**< Base Transceiver Station Identity Code [range: 0h-3Fh (6 bits)] */
	UINT8 nom;                              /**< Network Operation Mode [range: MODE_1= 0 / MODE_2= 1 / MODE_3= 2] */
	UINT8 nco;                              /**< Network Control Order  [range: NC_0=0 / NC_1=1 / NC_2=2 / NC_RESET=3] */
	UINT8 bs_pa_mfrms;                      /**< Number of multiframes between paging messages sent [range: 0-7] */

	UINT16 mcc;                             /**< Mobile Country Code [range:  0-999 (3 digits)] */
	UINT16 mnc;                             /**< Mobile Network Code [range: 0-99 (2 digits)] */
	UINT16 lac;                             /**< Location Area Code [range: 0h-FFFFh (2 octets)] */
	UINT16 ci;                              /**< Cell Identity [range: 0h-FFFFh (2 octets)] */
	UINT16 arfcn;                           /**< Absolute Radio Frequency Channel Number [range:  0-1023] */

	INT16 C1;                               /**< Path loss criterion parameter #1 */

	INT16 C2;                               /**< Path loss criterion parameter #2 */

	INT16 C31;                              /**< GPRS Signal Level Threshold Criterion Parameter*/

	INT16 C32;                              /**< GPRS Cell Ranking Criterion Parameter */


	UINT16 t3212;                                                                   /**< Periodic LA Update Timer (T3212) in minutes */
	UINT16 t3312;                                                                   /**< Periodic RA Update Timer (T3312) in minutes */

	CiBoolean pbcchSupport;                                                         /**< Support of PBCCH. \sa CCI API Ref Manual */
	UINT8 TxPowerLevel;                                                             /**< Tx Power Level [range: 0h-3Fh] */
	UINT8 timingAdv;                                                                /**< Timing Advance [range 0-63] */
	CiBoolean hoppingChannel;                                                       /**< Hopping Channel */
	CiBoolean EGPRSSupport;                                                         /**< EGPRS Support capability */
	CiDevEngChannelType ChType;                                                     /**< Values are TCH_F = 1, TCH_H = 2, SDCCH_4 = 4, SDCCH_8 = 8. \sa CIDEV_ENGMODE_CHANNEL_TYPE */
	CiBoolean nccPermitted;                                                         /**< The NCC Permitted parameter sets the NCCs (Network Color Codes) that the mobile station is permitted to report. \sa CCI API Ref Manual */
	UINT8 RadioLinkTimeout;                                                         /**< Radio Link Timeout [range: value >=0] */
	UINT16 hoCount;                                                                 /**< Handovers counter [range: value>=0] */
	UINT16 hoSuccessCount;                                                          /**< Success Handovers counter [range: value>=0] */
	UINT16 chanAssCount;                                                            /**< Channel Assignment counter [range: value>=0]*/
	UINT16 chanAssSuccessCount;                                                     /**< Success Channel Assignment counter [range: value>=0]*/

	UINT16 arfcnTch;                                                                /**< ARFCN for taffic channel,only valid for Dedicated state [range:  0-1023]*/
	UINT8 timeSlot;                                                                 /**< Time Slot,only valid for Dedicated state*/
	CiDevPacketIdleType isInPacketIdle;                                             /**< only valid for Idle state. \sa CiDevPacketIdleType */
	CiBoolean IsForbiddenLA;                                                        /**< Indicates if cell belongs to forbidden location area. FALSE: Cell is not in forbidden LA or forbidden status is unknown; TRUE: Cell is in forbidden LA. \sa CCI API Ref Manual */
	CiDevCellPrioriytType CellPriority;                                             /**< Cell priority for cell selection or reselection. Cell priority can be normal, low or barred. \sa CiDevCellPrioriytType */
	UINT8 HSN;                                                                      /**< Hopping sequence number. Value 0 means cyclic hopping is done*/
//	CiDevHoppingGroup HoppingGroup;                                                 /**< List of ARFCNs assigned for frequency hopping . \sa CiDevHoppingGroup */
} DevGsmServingCellInfo;

/** \brief GPRS Attach type  */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIDEV_ENGMODE_GPRS_ATTACH_TYPE {
	CI_DEV_EM_GPRS_ATTACH_TYPE_GPRS_ONLY_ATTACH = 0,                                        /**< GPRS only */
	CI_DEV_EM_GPRS_ATTACH_TYPE_GPRS_ATTACH_WHILE_IMSI_ATTACHED,                             /**< GPRS attach while IMSI attached */
	CI_DEV_EM_GPRS_ATTACH_TYPE_COMBINED_IMSI_ATTACH,                                        /**< The combined GPRS attach */
	CI_DEV_EM_GPRS_ATTACH_TYPE_ALIGN_32_BIT = 0XFFFFFFF
} _CiDevEngGPRSAttachType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief GPRS Attach type
 * \sa CIDEV_ENGMODE_GPRS_ATTACH_TYPE */
/** \remarks Common Data Section */
typedef UINT32 CiDevEngGPRSAttachType;
/**@}*/

/** \brief MAc Mode Type - Methods of allocating uplink radio resources  */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIDEV_ENGMODE_MACMODE_TYPE {
	CI_DEV_EM_MACMODE_TYPE_MAC_DYNAMIC_ALLOC = 0,                                           /**< dynamic allocation (DA) */
	CI_DEV_EM_MACMODE_TYPE_MAC_EXTENDED_DYNAMIC_ALLOC,                                      /**< extended dynamic allocation (EDA) */
	CI_DEV_EM_MACMODE_TYPE_MAC_FIXED_ALLOC_NOT_HALF_DUPLEX,                                 /**< fixed not half duplex allocation */
	CI_DEV_EM_MACMODE_TYPE_MAC_FIXED_ALLOC_HALF_DUPLEX,                                     /**< fixed half duplex allocation */
	CI_DEV_EM_MACMODE_TYPE_MAC_UNKNOWN_ALLOC_MODE,                                          /**< unknown allocation */
	CI_DEV_EM_MACMODE_TYPE_CI_DEV_ALIGN_32_BIT = 0XFFFFFFF
} _CiDevEngMacModeType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief MAc Mode Type - Methods of allocating uplink radio resources
 * \sa CIDEV_ENGMODE_MACMODE_TYPE */
/** \remarks Common Data Section */
typedef UINT32 CiDevEngMacModeType;
/**@}*/

/* Not in use. Network Control Order */
//ICAT EXPORTED ENUM
typedef enum CIDEV_ENGMODE_NETWORK_CTRL_TYPE {
	CI_DEV_EM_NW_CTRL_NC_0 = 0,
	CI_DEV_EM_NW_CTRL_NC_1,
	CI_DEV_EM_NW_CTRL_NC_2,
	CI_DEV_EM_NW_CTRL_NC_RESERVED,
	CI_DEV_EM_NW_CTRL_ALIGN_32_BIT = 0XFFFFFFF
} _CiDevEngNetworkControlType;

typedef UINT32 CiDevEngNetworkControlType;

/** \brief Network mode  */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIDEV_ENGMODE_NW_MODE_TYPE {
	CI_DEV_EM_NW_MODE_TYPE_NOM1 = 0,                /**< NW provides simultaneous CS and PS */
	CI_DEV_EM_NW_MODE_TYPE_NOM2,                    /**< UE remain attached to PS while receiving CS*/
	CI_DEV_EM_NW_MODE_TYPE_NOM3,                    /**< UE can be connected to CS or PS */
	CI_DEV_EM_NW_MODE_TYPE_ALIGN_32_BIT = 0XFFFFFFF
} _CiDevEngNetworkModeType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Network mode
 * \sa CIDEV_ENGMODE_NW_MODE_TYPE */
/** \remarks Common Data Section */
typedef UINT32 CiDevEngNetworkModeType;
/**@}*/

/** \brief Coding schemes  */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIDEV_ENGMODE_CODINGSCHEME_TYPE {
	CI_DEV_EM_CS_1 = 0,                                                     /**< CS1 */
	CI_DEV_EM_CS_2,                                                         /**< CS2 */
	CI_DEV_EM_CS_3,                                                         /**< CS3 */
	CI_DEV_EM_CS_4,                                                         /**< CS4 */
	CI_DEV_EM_MCS_1,                                                        /**< MCS1 */
	CI_DEV_EM_MCS_2,                                                        /**< MCS2 */
	CI_DEV_EM_MCS_3,                                                        /**< MCS3 */
	CI_DEV_EM_MCS_4,                                                        /**< MCS4 */
	CI_DEV_EM_MCS_5,                                                        /**< MCS5 */
	CI_DEV_EM_MCS_6,                                                        /**< MCS6 */
	CI_DEV_EM_MCS_7,                                                        /**< MCS7 */
	CI_DEV_EM_MCS_8,                                                        /**< MCS8 */
	CI_DEV_EM_MCS_9,                                                        /**< MCS9 */
	CI_DEV_EM_ALIGN_32_BIT = 0XFFFFFFF
} _CiDevEngCodingSchemeType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Coding schemes
 * \sa CIDEV_ENGMODE_CODINGSCHEME_TYPE */
/** \remarks Common Data Section */
typedef UINT32 CiDevEngCodingSchemeType;
/**@}*/

/** \brief LinkQualMeasMode  */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIDEV_ENGMODE_EGPR_SLQ_MEAS_MODE_TYPE {
	CI_DEV_EM_NO_LINK_QUAL_MEASUREMENTS = 0,                        /**< No report */
	CI_DEV_EM_LINK_QUAL_INT_MEAS_ONLY,                              /**< Report  interference measurements ( ?values) only */
	CI_DEV_EM_LINK_QUAL_BEP_MEAS_ONLY,                              /**< Report  only mean BEP measurements */
	CI_DEV_EM_LINK_QUAL_INT_AND_BEP_MEAS,                           /**< Report both interference and BEP measurements */
	CI_DEV_EM_EGPR_SLQ_MEAS_MODE_TYPE_ALIGN_32_BIT = 0XFFFFFFF
}_CiDevEngEGPRSLQMeasModeType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief LinkQualMeasMode
 * \sa CIDEV_ENGMODE_EGPR_SLQ_MEAS_MODE_TYPE */
/** \remarks Common Data Section */
typedef UINT32 CiDevEngEGPRSLQMeasModeType;
/**@}*/


/** \brief GPRS service type */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIDEV_GPRS_SERVICE_TYPE_TAG
{
	CI_DEV_GPRS_SERVICE_TYPE_GRPS = 0,
	CI_DEV_GPRS_SERVICE_TYPE_EDGE,

	/* This must be the last entry */
	CI_DEV_NUM_SERVICE_TYPE
} _CiDevGprsServiceType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief GPRS service type.
 * \sa CIDEV_GPRS_SERVICE_TYPE_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiDevGprsServiceType;
/**@}*/

/** \brief Modulation scheme */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIDEV_MODULATION_SCHEME_TAG
{
	CI_DEV_MS_GMSK = 0,                     /**< GMSK */
	CI_DEV_MS_8PSK = 1,                     /**< 8PSK */
	CI_DEV_MS_INVALID = 0xff,               /**< Invalid value */
	CI_DEV_NUM_MS                           /**< This must be the last entry */
} _CiDevModulationScheme;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Modulation scheme.
 * \sa CIDEV_MODULATION_SCHEME_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiDevModulationScheme;
/**@}*/


/** \brief Packet data information  */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiDevGPRSPTMInfo_struct
{
	CiBoolean GPRSAttached;                                                                                         /**< TRUE - MS is GPRS attached; FALSE - other. \sa CCI API Ref Manual */
	INT8 EGPRSBEPPeriod;                                                                                            /**< BEP Meas. Averaging coeff  */
	CiBoolean IsSingleGmmRejectCause;                                                                               /**< TRUE - only one GMM Reject cause reported during the last EngInfo period; FALSE - other.  \sa CCI API Ref Manual */
	UINT8 NumActivePDPContext;                                                                                      /**< 0 means no active PDP context [range: 0-7] */

	CiDevEngGPRSAttachType GPRSAttachType;                                                                          /**< GPRS Attach type. \sa CiDevEngGPRSAttachType */
	CiDevEngMacModeType MacMode;                                                                                    /**< Mac Mode type. \sa CiDevEngMacModeType */
	CiDevEngNetworkControlType NetworkControl;                                                                      /**< Not in use. Network Control Order. \sa CiDevEngNetworkControlType */
	CiDevEngNetworkModeType NetworkMode;                                                                            /**< Network Mode type. \sa CiDevEngNetworkModeType */
	CiDevEngCodingSchemeType CodingSchemeUL;                                                                        /**< UL Coding Scheme. \sa CiDevEngCodingSchemeType */
	CiDevEngCodingSchemeType CodingSchemeDL;                                                                        /**< DL Coding Scheme. \sa CiDevEngCodingSchemeType*/
	CiDevEngEGPRSLQMeasModeType EGPRSLQMeasurementMode;                                                             /**< LinkQualMeasMode. \sa CiDevEngEGPRSLQMeasModeType */
	DevEngGMMRejectCauseCodeType GMMRejectCause;                                                                  /**< This one is, reported during the last EngInfo period, GMM Reject cause (10.5.3.6) sent to MM from the network. \sa CiDevEngGMMRejectCauseCodeType */

	UINT8 cValue;                                                                                                   /**< C Value */
	UINT8 txPower;                                                                                                  /**< Transmit Power of every block - TBD: phase 2 */
	UINT8 ulTimeSlot;                                                                                               /**< Uplink Time Slot allocation bitmap. */
	UINT8 dlTimeSlot;                                                                                               /**< Downlink Time Slot allocation bitmap. */

	CiDevGprsServiceType gprsServiceType;                                                                           /**< GPRS service type GPRS/EDGE. \sa CiDevGprsServiceType */
	CiDevModulationScheme ulMod;                                                                                    /**< Uplink Modulation - TBD: phase 2. \sa CiDevModulationScheme */
	CiDevModulationScheme dlMod;                                                                                    /**< Downlink Modulation - TBD: phase 2. \sa CiDevModulationScheme */
	UINT8 USFGranularity;                                                                                           /**< USF granularity defines the number of RLC/MAC blocks to transmit if USF is present. 0 - the mobile station shall transmit one RLC/MAC block, 1 - the mobile station shall transmit four consecutive RLC/MAC blocks */
	UINT32 ULThroughput;                                                                                            /**< UL throughput in octets per second */
	UINT32 DLThroughput;                                                                                            /**< DL throughput in octets per second */
//	CiDevEgprsBepCvMeanInfo egprsBep;                                                                               /**< BEP Period - TBD: phase 2. \sa CiDevEgprsBepCvMeanInfo_struct */
} DevGPRSPTMInfo;

/** \brief Engineering Mode: 2G (GSM) Neighboring Cell Info structure  */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiDevGsmNeighboringCellInfo_struct
{
	UINT8 rxSigLevel;                               /**< Receive Signal Level */
	UINT8 bsic;                                     /**< Base Transceiver Station Identity Code */
	UINT8 rac;                                      /**< Routing Area code */
	UINT8 res1U8;                                   /**< (padding) */

	UINT16 mcc;                                     /**< Mobile Country Code */
	UINT16 mnc;                                     /**< Mobile Network Code */
	UINT16 lac;                                     /**< Location Area Code */
	UINT16 ci;                                      /**< Cell Identity */
	UINT16 arfcn;                                   /**< Absolute Radio Frequency Channel Number */

	INT16 C1;                                       /**< Path loss criterion parameter #1 */
	INT16 C2;                                       /**< Path loss criterion parameter #2 */
	INT16 C31;                                      /**< GPRS Signal Level Threshold Criterion Parameter */
	INT16 C32;                                      /**< GPRS Cell Ranking Criterion Parameter */

	CiBoolean IsForbiddenLA;                        /**< Indicates if cell belongs to forbidden location area. FALSE: Cell is not in forbidden LA or forbidden status is unknown; TRUE: Cell is in forbidden LA. \sa CCI API Ref Manual */
	CiDevCellPrioriytType CellPriority;             /**< Cell priority for cell selection or reselection. Cell priority can be normal, low or barred. \sa CiDevCellPrioriytType */
} DevGsmNeighboringCellInfo;

/** <paramref name="CI_MM_PRIM_AIR_INTERFACE_REJECT_CAUSE_IND"> */
//ICAT EXPORTED STRUCT
typedef struct DevRejectCause_struct
{
	DevEngGMMRejectCauseCodeType mmCause;                              /**< MM state  \sa CiMmCause */
	DevEngMMRejectCauseCodeType gmmCause;                            /**< GMM state  \sa CiGmmCause */
} DevRejectCause;

#ifdef __cplusplus
}
#endif

#endif /*EE_DEV_H */

 

