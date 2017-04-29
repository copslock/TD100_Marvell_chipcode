/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/* ===========================================================================
   File        : ci_dev.h
   Description : Data types file for the DEV service group

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

#if !defined(_CI_DEV_H_)
#define _CI_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ci_api_types.h"
/** \addtogroup  SpecificSGRelated
 * @{ */

#define CI_DEV_VER_MAJOR 3
#define CI_DEV_VER_MINOR 0
#define CI_DEV_ENG_MAX_ACTIVE_PDP_CONTEXT    7
#define CI_DEV_PDP_IP_V6_SIZE                               16
#define CI_DEV_MAX_APN_NAME                                     100
#define CI_DEV_MAX_ARFCN_LIST                   64 /*Michal Bukai - I-Mate Addition.*/

/* ----------------------------------------------------------------------------- */

/* CI_DEV Primitive ID definitions */
//ICAT EXPORTED ENUM
/** Summary of primitives */
typedef enum CI_DEV_PRIM {
	CI_DEV_PRIM_STATUS_IND = 1,                                                     /**< \brief Indicates that the device status has changed. \details Before receiving CI_DEV_PRIM_STATUS_IND(CI_DEV_STATUS_READY), the client should not begin its initialization process. This indication cannot be disabled.  */
	CI_DEV_PRIM_GET_MANU_ID_REQ,                                                    /**< \brief Get manufacturer id request. \details   */
	CI_DEV_PRIM_GET_MANU_ID_CNF,                                                    /**< \brief Get manufacturer id confirmation. \details   */
	CI_DEV_PRIM_GET_MODEL_ID_REQ,                                                   /**< \brief Get model id. \details   */
	CI_DEV_PRIM_GET_MODEL_ID_CNF,                                                   /**< \brief Confirmation to get model id request. \details   */
	CI_DEV_PRIM_GET_REVISION_ID_REQ,                                                /**< \brief Get revision id request. \details   */
	CI_DEV_PRIM_GET_REVISION_ID_CNF,                                                /**< \brief Confirmation to get revision id request. \details   */
	CI_DEV_PRIM_GET_SERIALNUM_ID_REQ,                                               /**< \brief Get serial number id request. \details   */
	CI_DEV_PRIM_GET_SERIALNUM_ID_CNF,                                               /**< \brief Confirmation to get serial number id request. \details   */
	CI_DEV_PRIM_SET_FUNC_REQ,                                                       /**< \brief Set the level of functionality in cellular subsystem. \details   */
	CI_DEV_PRIM_SET_FUNC_CNF,                                                       /**< \brief Confirmation to set the level of functionality in cellular subsystem. \details   */
	CI_DEV_PRIM_GET_FUNC_REQ,                                                       /**< \brief Get the level of functionality in cellular subsystem. \details   */
	CI_DEV_PRIM_GET_FUNC_CNF,                                                       /**< \brief Confirmation to get request of the level of functionality in cellular subsystem. \details   */
	CI_DEV_PRIM_GET_FUNC_CAP_REQ,                                                   /**< \brief Get the level of functionality capability in cellular subsystem. \details   */
	CI_DEV_PRIM_GET_FUNC_CAP_CNF,                                                   /**< \brief Confirmation to get request of the level of functionality capability in cellular subsystem. \details   */
	CI_DEV_PRIM_SET_GSM_POWER_CLASS_REQ,                                            /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_DEV_PRIM_SET_GSM_POWER_CLASS_CNF,                                            /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_DEV_PRIM_GET_GSM_POWER_CLASS_REQ,                                            /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_DEV_PRIM_GET_GSM_POWER_CLASS_CNF,                                            /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_DEV_PRIM_GET_GSM_POWER_CLASS_CAP_REQ,                                        /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_DEV_PRIM_GET_GSM_POWER_CLASS_CAP_CNF,                                        /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */

	CI_DEV_PRIM_PM_POWER_DOWN_REQ,                                                  /**< \brief Perform Power Down for the current device. \details   */
	CI_DEV_PRIM_PM_POWER_DOWN_CNF,                                                  /**< \brief Confirm that Power Down for the current device has been accomplished. \details   */

	CI_DEV_PRIM_SET_ENGMODE_REPORT_OPTION_REQ,              /**< \brief Sets the option for reporting Engineering Mode information. \details If the "Periodic" option is selected:
								 *   -	For GSM: CCI sends a periodic CI_DEV_PRIM_GSM_ENGMODE_INFO_IND indication containing the current Engineering Mode information.
								 *   -	For UMTS, the following periodic messages are sent:
								 *   CI_DEV_PRIM_UMTS_ENGMODE_SVCCELL_INFO_IND
								 *   CI_DEV_PRIM_UMTS_ENGMODE_INTERFREQ_INFO_IND
								 *   CI_DEV_PRIM_UMTS_ENGMODE_INTRAFREQ_INFO_IND
								 *   CI_DEV_PRIM_UMTS_ENGMODE_INTERRAT_INFO_IND
								 *   The report interval is received as parameter 'interval' of CiDevPrimSetEngmodeRepOptReq structure.
								 *   If the "Request" option is selected:
								 *   -	The Application requests current Engineering Mode information using the CI_DEV_PRIM_GET_ENGMODE_INFO_REQ request.
								 *   The default option is "On Request". See Section ?3.4 for Engineering Mode information delivery scenarios.  */
	CI_DEV_PRIM_SET_ENGMODE_REPORT_OPTION_CNF,                                      /**< \brief Confirms a request to set the option for reporting Engineering Mode information. \details   */
	CI_DEV_PRIM_GET_ENGMODE_INFO_REQ,                               /**< \brief Requests current Engineering Mode information. \details Use this request when the Engineering Mode Report Option is set to "On Request".
									 *   Engineering Mode information is currently available for GSM as well as for UMTS.  */
	CI_DEV_PRIM_GET_ENGMODE_INFO_CNF,                                               /**< \brief Returns current Engineering Mode information. \details Engineering Mode information is currently available for GSM as well as for UMTS.  */
	CI_DEV_PRIM_ENGMODE_INFO_IND,                                                   /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */

	CI_DEV_PRIM_GSM_ENGMODE_INFO_IND,                                               /**< \brief Delivers current GSM Engineering Mode information. \details   */
	CI_DEV_PRIM_UMTS_ENGMODE_SVCCELL_INFO_IND,                                      /**< \brief UMTS EngMode ServingCell Info. \details   */
	CI_DEV_PRIM_UMTS_ENGMODE_INTRAFREQ_INFO_IND,                                    /**< \brief UMTS EngMode Intra-Frequency Info. \details   */
	CI_DEV_PRIM_UMTS_ENGMODE_INTERFREQ_INFO_IND,                                    /**< \brief UMTS EngMode Inter-Frequency Info. \details   */
	CI_DEV_PRIM_UMTS_ENGMODE_INTERRAT_INFO_IND,                                     /**< \brief UMTS EngMode Inter-Rat Info. \details   */
	CI_DEV_PRIM_DO_SELF_TEST_REQ,                                                   /**< \brief Request to perform a device self test. \details   */
	CI_DEV_PRIM_DO_SELF_TEST_CNF,                                                   /**< \brief Confirmation to a request to perform a device self test. \details   */
	CI_DEV_PRIM_DO_SELF_TEST_IND,                                                   /**< \brief Indication of a device self test completion status. \details   */
	CI_DEV_PRIM_SET_RFS_REQ,                                                        /**< \brief Request to change the current Reset Factory Settings. \details   */
	CI_DEV_PRIM_SET_RFS_CNF,                                                        /**< \brief Confirmation to a request to change the current Reset Factory Setting. \details   */
	CI_DEV_PRIM_GET_RFS_REQ,                                                        /**< \brief Request to obtain the current Reset Factory Settings. \details   */
	CI_DEV_PRIM_GET_RFS_CNF,                                                        /**< \brief Confirmation of a request to obtain the current Reset Factory Settings. \details   */
	CI_DEV_PRIM_UMTS_ENGMODE_ACTIVE_SET_INFO_IND,                                   /**< \brief EngMode Active Set Info indication. \details   */
	CI_DEV_PRIM_ACTIVE_PDP_CONTEXT_ENGMODE_IND,                                     /**< \brief Active PDP Context Info. \details   */
	CI_DEV_PRIM_NETWORK_MONITOR_INFO_IND,                                           /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_DEV_PRIM_LP_NWUL_MSG_REQ,                                                    /**< \brief Delivers measurements reports and/or status from AGPS client to PS/NW. \details   */
	CI_DEV_PRIM_LP_NWUL_MSG_CNF,                                                    /**< \brief This is a confirmation primitive from protocol stack to APPS, to CI_DEV_PRIM_LP_NWUL_MSG_REQ. \details   */
	CI_DEV_PRIM_LP_NWDL_MSG_IND,                                                    /**< \brief This is an indication primitive from protocol stack to APPS, indicating of either RRC or
											 *   RRLP positioning-related message (MEASUREMENT CONTROL (setup, modify and release) or ASSISTANCE DATA DELIVERY)
											 *   from the network to the AGPS client running on the APPS. \details   */
	CI_DEV_PRIM_LP_RRC_STATE_IND,                                                   /**< \brief AGPS C-Plane indication primitive from protocol stack to APPS, indicating of RRC state change. \details It should be called by ABPS whenever the RRC state changes while a positioning session is active.  */
	CI_DEV_PRIM_LP_MEAS_TERMINATE_IND,                                              /**< \brief indication primitive from protocol stack to APPS, indicating of RAT termination (either of GSM or WCDMA).. \details   */
	CI_DEV_PRIM_LP_RESET_STORED_UE_POS_IND,                         /**< \brief This is an indication primitive from protocol stack to APPS, which is a test
									 *  I/F message (TIM) requesting the UE to erase all the assistance data that may have been previously stored in the UE, prior
									 *  to the execution of the test. \details   */
	/*Michal Bukai - Silent Reset support - START*/
	CI_DEV_PRIM_COMM_ASSERT_REQ,
	/*Michal Bukai - Silent Reset support - END*/
	/*Michal Bukai *BAND support - START*/
	CI_DEV_PRIM_SET_BAND_MODE_REQ,
	CI_DEV_PRIM_SET_BAND_MODE_CNF,
	CI_DEV_PRIM_GET_BAND_MODE_REQ,
	CI_DEV_PRIM_GET_BAND_MODE_CNF,
	CI_DEV_PRIM_GET_SUPPORTED_BAND_MODE_REQ,
	CI_DEV_PRIM_GET_SUPPORTED_BAND_MODE_CNF,
	/*Michal Bukai *BAND support - END*/
	/*Michal Bukai - IMEI support - START*/
	CI_DEV_PRIM_SET_SV_REQ,                                                         /**< \brief Requests to update the SV digits in IMEISV. \details The default value of the SV digits is the value set during production.  */
	CI_DEV_PRIM_SET_SV_CNF,                                                         /**< \brief Confirms a request to set SV digits in IMEISV. \details   */
	CI_DEV_PRIM_GET_SV_REQ,                                                         /**< \brief Requests to get the SV digits in IMEISV. \details   */
	CI_DEV_PRIM_GET_SV_CNF,                                                         /**< \brief Confirms a request and returns the SV digits in IMEISV. \details The full IMEISV can be read using the primitive CI_DEV_PRIM_GET_SERIALNUM_ID_REQ to read IMEI and this primitive to get SV digits.   */
	/*Michal Bukai - IMEI support - END*/

	CI_DEV_PRIM_AP_POWER_NOTIFY_REQ,
	CI_DEV_PRIM_AP_POWER_NOTIFY_CNF,

	CI_DEV_PRIM_SET_TD_MODE_TX_RX_REQ,					/**< \brief Requests to set Tx or Rx on TD for radio testing.  */
	CI_DEV_PRIM_SET_TD_MODE_TX_RX_CNF,					/**< \brief Confirms the TD Tx/Rx mode request.   */
	CI_DEV_PRIM_SET_TD_MODE_LOOPBACK_REQ,				/**< \brief Requests to set loopback mode on TD for radio testing.   */
	CI_DEV_PRIM_SET_TD_MODE_LOOPBACK_CNF,				/**< \brief Confirms to set loopback mode on TD for radio testing.   */

	CI_DEV_PRIM_SET_GSM_MODE_TX_RX_REQ,					/**< \brief Requests to set Tx/Rx on GSM for radio testing.  */
	CI_DEV_PRIM_SET_GSM_MODE_TX_RX_CNF,					/**< \brief Confirms the GSM Tx/Rx mode request.   */
	CI_DEV_PRIM_SET_GSM_CONTROL_INTERFACE_REQ,			/**< \brief Requests to set GSM for control interface.   */
	CI_DEV_PRIM_SET_GSM_CONTROL_INTERFACE_CNF,			/**< \brief Confirms to set option on GSM for control interface testing.   */
	CI_DEV_PRIM_ENABLE_HSDPA_REQ,
	CI_DEV_PRIM_ENABLE_HSDPA_CNF,
	CI_DEV_PRIM_GET_HSDPA_STATUS_REQ,
	CI_DEV_PRIM_GET_HSDPA_STATUS_CNF,

	CI_DEV_PRIM_READ_RF_TEMPERATURE_REQ,                /**< \brief Requests to read temperature from RF chip.  */
        CI_DEV_PRIM_READ_RF_TEMPERATURE_CNF,                /**< \brief Confirms to read temperature from RF chip.   */

#ifdef AT_CUSTOMER_HTC
	 /*+TPCN support - START*/
    CI_DEV_PRIM_SET_MODE_SWITCH_OPTION_REQ,             /**< \brief Requests to set option for mode switch.  */
    CI_DEV_PRIM_SET_MODE_SWITCH_OPTION_CNF,             /**< \brief Confirms to set option for mode switch.   */
    CI_DEV_PRIM_GET_MODE_SWITCH_OPTION_REQ,             /**< \brief Requests to get option for mode switch.  */
    CI_DEV_PRIM_GET_MODE_SWITCH_OPTION_CNF,             /**< \brief Confirms to get option for mode switch.   */
    /*+TPCN support - END*/

    CI_DEV_PRIM_RADIO_LINK_FAILURE_IND,                 /**< \brief Indicates radio link failure. */
#endif

	/*Mason CMCC Smart Network Monitor support - START*/
    /* AT^DCTS */
    CI_DEV_PRIM_SET_NETWORK_MONITOR_OPTION_REQ,
    CI_DEV_PRIM_SET_NETWORK_MONITOR_OPTION_CNF,
    CI_DEV_PRIM_GET_NETWORK_MONITOR_OPTION_REQ,
    CI_DEV_PRIM_GET_NETWORK_MONITOR_OPTION_CNF,

    /* AT^DEELS */
    CI_DEV_PRIM_SET_PROTOCOL_STATUS_CONFIG_REQ,
    CI_DEV_PRIM_SET_PROTOCOL_STATUS_CONFIG_CNF,
    CI_DEV_PRIM_GET_PROTOCOL_STATUS_CONFIG_REQ,
    CI_DEV_PRIM_GET_PROTOCOL_STATUS_CONFIG_CNF,
    CI_DEV_PRIM_PROTOCOL_STATUS_CHANGED_IND,

    /* AT^DEVEI */
    CI_DEV_PRIM_SET_EVENT_IND_CONFIG_REQ,
    CI_DEV_PRIM_SET_EVENT_IND_CONFIG_CNF,
    CI_DEV_PRIM_GET_EVENT_IND_CONFIG_REQ,
    CI_DEV_PRIM_GET_EVENT_IND_CONFIG_CNF,
    CI_DEV_PRIM_EVENT_REPORT_IND,

    /* AT^DNPR */
    CI_DEV_PRIM_SET_WIRELESS_PARAM_CONFIG_REQ,
    CI_DEV_PRIM_SET_WIRELESS_PARAM_CONFIG_CNF,
    CI_DEV_PRIM_GET_WIRELESS_PARAM_CONFIG_REQ,
    CI_DEV_PRIM_GET_WIRELESS_PARAM_CONFIG_CNF,
    CI_DEV_PRIM_WIRELESS_PARAM_IND,

    /* AT^DUSR */
    CI_DEV_PRIM_SET_SIGNALING_REPORT_CONFIG_REQ,
    CI_DEV_PRIM_SET_SIGNALING_REPORT_CONFIG_CNF,
    CI_DEV_PRIM_GET_SIGNALING_REPORT_CONFIG_REQ,
    CI_DEV_PRIM_GET_SIGNALING_REPORT_CONFIG_CNF,
    CI_DEV_PRIM_SIGNALING_REPORT_IND,
    /*Mason CMCC Smart Network Monitor support -- END*/

	/* ADD NEW COMMON PRIMITIVES HERE, BEFORE 'CI_DEV_PRIM_LAST_COMMON_PRIM' */

	/* END OF COMMON PRIMITIVES LIST */
	CI_DEV_PRIM_LAST_COMMON_PRIM

	/* the  customer specific extension primitives will be added starting from
	 * CI_DEV_PRIM_firstCustPrim = CI_DEV_PRIM_LAST_COMMON_PRIM as the first identifier.
	 * The actual primitive names and IDs are defined in the associated
	 * 'ci_dev_cust_xxx.h' file.
	 */

	/* DO NOT ADD ANY MORE PRIMITIVES HERE */

} _CiDevPrim;

/* specify the number of default common DEV primitives */
#define CI_DEV_NUM_COMMON_PRIM ( CI_DEV_PRIM_LAST_COMMON_PRIM - 1 )
/**@}*/

/*Michal Bukai - I-Mate Addition. Start:*/
/****************************************/

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

/** \brief List of ARFCNs assigned for frequency hopping */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiDevHoppingGroup_struct
{
	UINT8 length;                           /**< Length of the arfcn list. Range is 0-64.0 - means list is not available */
	UINT16 Arfcns[CI_DEV_MAX_ARFCN_LIST];   /**< Absolute radio frequency channel number */
} CiDevHoppingGroup;

/*Michal Bukai - I-Mate Addition. End*/
/****************************************/
/** \brief Devide status. */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIDEVSTATUS_TAG {
	CI_DEV_STATUS_READY = 0,                /**< device is ready to handle requests */
	CI_DEV_STATUS_UNAVAILABLE,              /**< device can not handle requests */
	CI_DEV_STATUS_UNKNOWN,                  /**< device status unknown */
	CI_DEV_STATUS_RINGING,                  /**< device is ringing */
	CI_DEV_STATUS_CALLINPROG,               /**< device has a call in progress */
	CI_DEV_STATUS_ASLEEP,                   /**< device is in a low functionality state */

	CI_DEV_NUM_STATUSES
} _CiDevStatus;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Devide status.
 * \sa CIDEVSTATUS_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiDevStatus;
/**@}*/

/** \brief DEV group Return Codes. */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIRCDEV_TAG {
	CIRC_DEV_SUCCESS = 0,           /**< request completed successfully */
	CIRC_DEV_FAILURE,               /**< phone failure */
	CIRC_DEV_NO_CONNECTION,         /**< no connection to phone */
	CIRC_DEV_UNKNOWN,               /**< unknown error */

	CIRC_DEV_NUM_RESCODES
} _CiDevRc;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief Result code
 *  \sa CIRCDEV_TAG
 * \remarks Common Data Section */
typedef UINT16 CiDevRc;
/**@}*/

/** \brief Phone functionality mode. */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIDEVFUNC_TAG {
	CI_DEV_FUNC_MIN = 0,                            /**< minimum functionality, i.e. lowest power level */
	CI_DEV_FUNC_FULL,                               /**< full functionality, i.e. highest power level */
	CI_DEV_FUNC_DISABLE_TX_RF,                      /**< disable phone transmit RF circuits only */
	CI_DEV_FUNC_DISABLE_RX_RF,                      /**< disable phone receive RF circuits only */
	CI_DEV_FUNC_DISABLE_BOTH_RF,                    /**< disable phone both transmit and receive RF circuits */
	CI_DEV_FUNC_UPDATE_NVM_MIN,                     /**< update NVM file with minimum functionality mode */
	CI_DEV_FUNC_UPDATE_NVM_FULL,                    /**< update NVM file with full functionality mode */
	CI_DEV_FUNC_MIN_NO_IMSI_DETACH,                 /**< minimum functionality but without IMSI detach */
	CI_DEV_FUNC_DISABLE_SIM,                         /**< disable SIM */
	CI_DEV_FUNC_UPDATE_NVM_DISABLE_BOTH_RF = 9,     /**< update NVM file with both transmit and receive RF circuits disable mode */

	CI_DEV_NUM_FUNCS
} _CiDevFunc;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief Phone functionality mode
 *  \sa CIDEVFUNC_TAG
 * \remarks Common Data Section */
typedef UINT8 CiDevFunc;
/**@}*/


/** \brief Phone functionality mode. */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum COMMFEATURECONFIG_TAG {
	CI_DEV_CSD = 1, /**< CSD is supported */
	CI_DEV_FAX,     /**< FAX is supported */
	CI_DEV_PRODUCTION,      /**< Production Mode is in use*/
	CI_DEV_CONVENTIONAL_GPS,/**< Conventional GPS is supported */
	CI_DEV_MS_BASED_GPS,    /**< Ms-Based A-GPS is supported */
	CI_DEV_MS_ASSISTED_GPS, /**< Ms-Assisted A-GPS is supported */

	CI_DEV_NUM_COMM_FEATURE_CONFIG
} _CommFeatureConfig;



/** \brief Band mode. */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIDEVBAND_TAG {
	CI_DEV_BAND_GSM_900 = 0,                /**< GSM_900 band */
	CI_DEV_BAND_GSM_1800,                   /**< GSM_1800 band */
	CI_DEV_BAND_GSM_1900,                   /**< GSM_1900 band */
	CI_DEV_BAND_GSM_400,                    /**< GSM_400 band*/

	CI_DEV_NUM_BANDS
} _CiDevBand;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief Band mode
 *  \sa CIDEVBAND_TAG
 * \remarks Common Data Section */
typedef UINT8 CiDevBand;
/**@}*/

/** \brief Power Classes, according to TS 05.05 v8.12.0, 4.1.1 */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIDEVPWCLS_TAG {
	CI_DEV_PWCLS_DEFAULT = 0,
	CI_DEV_PWCLS_1,
	CI_DEV_PWCLS_2,
	CI_DEV_PWCLS_3,
	CI_DEV_PWCLS_4,
	CI_DEV_PWCLS_5,

	CI_DEV_NUM_PWCLSES
} _CiDevPwCls;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Power Classes.
 * \sa CIDEVPWCLS_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiDevPwCls;

/* Engineering Mode: Reporting Options */

/* Engineering Mode: Maximum Number of Neighboring Cells in Report */
#define CI_DEV_MAX_GSM_NEIGHBORING_CELLS     6          /* GSM Neighbors */
#define CI_DEV_MAX_UMTS_NEIGHBORING_CELLS   32          /* FDD and GSM Neighbors */

/* Maximum number of cells in active set */
#ifndef TD_SUPPORT
#define CI_DEV_MAX_CELLS_IN_AS                          6
#else
#define CI_DEV_MAX_CELLS_IN_AS                          1
#endif
/**@}*/

/** \brief Engineering Mode: Reporting Option type */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIDEV_ENGMODE_REPORTOPTION_TAG
{
	CI_DEV_EM_OPTION_NONE = 0,      /**< EngMode Report Delivery: Turn Off */
	CI_DEV_EM_OPTION_REQUEST,       /**< EngMode Report Delivery: On request */
	CI_DEV_EM_OPTION_PERIODIC,      /**< EngMode Report Delivery: Periodic */
	CI_DEV_NUM_EM_REPORT_OPTIONS
} _CiDevEngModeReportOption;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Engineering Mode: Reporting Option type.
 * \sa CIDEV_ENGMODE_REPORTOPTION_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiDevEngModeReportOption;
/**@}*/

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
typedef UINT8 CiDevEngModeState;
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
typedef UINT8 CiDevEngModeNetwork;
/**@}*/

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
typedef UINT8 CiDevEngModeUeRrcState;
/**@}*/

/* ===================================================================================
		2G (GSM) Engineering Mode Structures
   ===================================================================================*/

/** \brief Network monitor mode */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIDEV_NETWORKMONITOR_MODE_TAG
{
	CI_DEV_NM_IDLE_MODE = 0,                        /**< Current GSM mode - IDLE */
	CI_DEV_NM_DEDICATED_MODE,                       /**< Current GSM mode - DEDICATED  */
	CI_DEV_NM_GPRS_MODE,                            /**< Current GSM mode - during GPRS  */

	/* This must be the last entry */
	CI_DEV_NUM_NM_MODES
} _CiDevNetworkMonitorMode;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Network monitor mode.
 * \sa CIDEV_NETWORKMONITOR_MODE_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiDevNetworkMonitorMode;
/**@}*/

/** \brief BEP status (TBD) */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIDEV_ENGMODE_BEPCVMEAN_STATUS_TAG {
	CI_DEV_BEP_NONE    = 0,
	CI_DEV_BEP_GMSK,
	CI_DEV_BEP_EIGHT_PSK,
	CI_DEV_BEP_GMSK_AND_EIGHT_PSK,
	CI_DEV_NUM_BEP_STATUS
} _CiDevBepCvMeanStatus;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief BEP status (TBD).
 * \sa CIDEV_ENGMODE_BEPCVMEAN_STATUS_TAG */
/** \remarks Common Data Section */
typedef UINT32 CiDevBepCvMeanStatus;
/**@}*/

/** \brief Channel type */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIDEV_CHANNEL_TYPE_TAG
{
	CI_DEV_TCH_F_PLUS_ACCH  = 1,
	CI_DEV_TCH_H_PLUS_ACCH  = 2,
	CI_DEV_SDCCH_4  = 4,
	CI_DEV_SDCCH_8  = 8,

	/* This must be the last entry */
	CI_DEV_NUM_CHANNEL_TYPE
} _CiDevChannelType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Channel type.
 * \sa CIDEV_CHANNEL_TYPE_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiDevChannelType;
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

/** \brief Coding scheme */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIDEV_CODE_SCHEME_TAG
{
	CI_DEV_CS_CS_1 = 0,
	CI_DEV_CS_CS_2,
	CI_DEV_CS_CS_3,
	CI_DEV_CS_CS_4,
	CI_DEV_CS_CS_RACH_8,
	CI_DEV_CS_CS_RACH_11,
	CI_DEV_CS_MCS_1,
	CI_DEV_CS_MCS_2,
	CI_DEV_CS_MCS_3,
	CI_DEV_CS_MCS_4,
	CI_DEV_CS_MCS_5,
	CI_DEV_CS_MCS_6,
	CI_DEV_CS_MCS_7,
	CI_DEV_CS_MCS_8,
	CI_DEV_CS_MCS_9,
	CI_DEV_CS_MCS_5_7,
	CI_DEV_CS_MCS_6_9,
	CI_DEV_CS_INVALID = 0xff,

	/* This must be the last entry */
	CI_DEV_NUM_CS
} _CiDevCodeScheme;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Coding scheme.
 * \sa CIDEV_CODE_SCHEME_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiDevCodeScheme;
/**@}*/

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

/** Not in use - BEP Info */
//ICAT EXPORTED STRUCT
typedef struct CiDevEgprsBepCvMeanInfo_struct
{
	CiDevBepCvMeanStatus status;

	UINT8 gmskMeanBep;
	UINT8 gmskCvBep;
	UINT8 eightPskMeanBep;
	UINT8 eightPskCvBep;
	UINT8 res1U8[3];                                                /**< (padding) */
} CiDevEgprsBepCvMeanInfo;

//ICAT EXPORTED STRUCT
typedef struct CiDevNetworkMonitorNcellInfo_struct
{
	UINT8 bsic;                     /* Base Transceiver Station Identity Code */
	UINT8 rxSigLevel;               /* Receive Signal Level - BCCH */

	UINT16 arfcn;                   /* Absolute Radio Frequency Channel Number */
	INT16 C1;                       /* Path loss criterion parameter #1 */
	INT16 C2;                       /* Path loss criterion parameter #2 */
} CiDevNetworkMonitorNcellInfo;

//ICAT EXPORTED STRUCT
typedef struct CiDevNetworkMonitorIdleInfo_struct
{
	UINT16 arfcn;                   /* Absolute Radio Frequency Channel Number */
	INT16 C1;                       /* Path loss criterion parameter #1 */
	INT16 C2;                       /* Path loss criterion parameter #2 */

	UINT8 bsic;                     /* Base Transceiver Station Identity Code */
	UINT8 rxSigLevel;               /* Receive Signal Level -BCCH */

	CiDevPacketIdleType isInPacketIdle;
	UINT8 txPower;          /* Transmit Power - TBD: phase 2 */
	UINT8 res1U8[2];        /* (padding) */
}CiDevNetworkMonitorIdleInfo;

//ICAT EXPORTED STRUCT
typedef struct CiDevNetworkMonitorDedicatedInfo_struct
{
	UINT16 arfcn;                           /* Absolute Radio Frequency Channel Number */
	INT16 C1;                               /* Path loss criterion parameter #1 */

	UINT8 bsic;                             /* Base Transceiver Station Identity Code */
	UINT8 rxSigLevelFull;                   /* Receive Signal Level accessed over all TDMA frames */
	UINT8 rxSigLevelSub;                    /* Receive Signal Level accessed over subset of TDMA frames*/
	UINT8 rxQualityFull;                    /* Receive Quality accessed over all TDMA frames */
	UINT8 rxQualitySub;                     /* Receive Quality accessed over subset of TDMA frames */
	UINT8 timingAdv;                        /* Inital timing advance or timing advance in SACCH block */

	CiBoolean isChannelHopping;             /* Channeling is hopping*/
	CiDevChannelType channelType;           /* Channel Type*/
	UINT16 arfcnTch;                        /* ARFCN for traffic channel*/

	UINT8 timeSlot;                         /* Server Timeslot */
	UINT8 txPower;                          /* Transmit Power - TBD: phase 2 */
}CiDevNetworkMonitorDedicatedInfo;

//ICAT EXPORTED STRUCT
typedef struct CiDevNetworkMonitorGprsInfo_struct
{
	UINT16 arfcn;                                                           /* Absolute Radio Frequency Channel Number */
	INT16 C1;                                                               /* Path loss criterion parameter #1 */
	INT16 C2;                                                               /* Path loss criterion parameter #2 */

	UINT8 bsic;                                                             /* Base Transceiver Station Identity Code */
	UINT8 rxSigLevelFull;                                                   /* Receive Signal Level accessed over all TDMA frames */
	UINT8 rxSigLevelSub;                                                    /* Receive Signal Level accessed over subset of TDMA frames*/
	UINT8 rxQualityFull;                                                    /* Receive Quality accessed over all TDMA frames */
	UINT8 rxQualitySub;                                                     /* Receive Quality accessed over subset of TDMA frames */
	UINT8 cValue;                                                           /* C Value */
	UINT8 txPower;                                                          /* Transmit Power - TBD: phase 2 */
	UINT8 ulTimeSlot;                                                       /* Uplink Time Slot - TBD: phase 2 */
	UINT8 dlTimeSlot;                                                       /* Downlink Time Slot - TBD: phase 2 */

	CiDevGprsServiceType gprsServiceType;                                   /* GPRS service type - TBD: phase 2 */
	CiDevCodeScheme ulCs;                                                   /* Uplink Code Scheme - TBD: phase 2 */
	CiDevCodeScheme dlCs;                                                   /* Downlink Code Scheme - TBD: phase 2 */
	CiDevModulationScheme ulMod;                                            /* Uplink Modulation - TBD: phase 2 */
	CiDevModulationScheme dlMod;                                            /* Downlink Modulation - TBD: phase 2 */
	CiDevEgprsBepCvMeanInfo egprsBep;                                       /* TBD: phase 2 */
}CiDevNetworkMonitorGprsInfo;

//ICAT EXPORTED STRUCT
typedef struct CiDevNetworkMonitorData_struct
{
	CiDevNetworkMonitorMode mode;           /* Current mode(Idle/Dedicated/GPRS) */
	UINT8 res1U8[3];                        /* (padding) */

	/* Serving Cell information */
	union
	{
		CiDevNetworkMonitorIdleInfo IdleData;
		CiDevNetworkMonitorDedicatedInfo DedicatedData;
		CiDevNetworkMonitorGprsInfo GprsData;
	}svcCellInfo;

	/* Neighboring Cell information */
	UINT8 numNCells;        /* 0..CI_DEV_MAX_GSM_NEIGHBORING_CELLS */
	UINT8 res2U8[3];        /* (padding) */
	CiDevNetworkMonitorNcellInfo nbCellInfo[ CI_DEV_MAX_GSM_NEIGHBORING_CELLS ];
}CiDevNetworkMonitorData;

//ICAT EXPORTED STRUCT
typedef struct CiDevPrimNetworkMonitorInfoInd_struct
{
	CiDevNetworkMonitorData info;
}CiDevPrimNetworkMonitorInfoInd;

//ICAT EXPORTED ENUM
typedef enum CIDEV_ENGMODE_CHANNEL_TYPE_TAG
{
	CI_DEV_EM_TCH_F             =   1,
	CI_DEV_EM_TCH_H            =   2,
	CI_DEV_EM_SDCCH_4        =   4,
	CI_DEV_EM_SDCCH_8        =   8
} _CiDevEngChannelType;

typedef UINT8 CiDevEngChannelType;


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
	/*Michal Bukai - I-Mate Addition. Start:*/
	CiBoolean IsForbiddenLA;                                                        /**< Indicates if cell belongs to forbidden location area. FALSE: Cell is not in forbidden LA or forbidden status is unknown; TRUE: Cell is in forbidden LA. \sa CCI API Ref Manual */
	CiDevCellPrioriytType CellPriority;                                             /**< Cell priority for cell selection or reselection. Cell priority can be normal, low or barred. \sa CiDevCellPrioriytType */
	UINT8 HSN;                                                                      /**< Hopping sequence number. Value 0 means cyclic hopping is done*/
	CiDevHoppingGroup HoppingGroup;                                                 /**< List of ARFCNs assigned for frequency hopping . \sa CiDevHoppingGroup */
	/*Michal Bukai - I-Mate Addition. End*/
} CiDevGsmServingCellInfo;



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
typedef UINT32 CiDevEngMMRejectCauseCodeType;
/**@}*/

/** \brief Band mode */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIDEV_ENGMODE_BAND_MODE_TYPE {
	CI_DEV_EM_BAND_MODE_PGSM_MODE = 0,                                                              /**< GSM_BAND supported */
	CI_DEV_EM_BAND_MODE_EGSM_MODE,                                                                  /**< EGSM_BAND supported */
	CI_DEV_EM_BAND_MODE_DCS_MODE,                                                                   /**< DCS_BAND supported */
	CI_DEV_EM_BAND_MODE_PCS_MODE,                                                                   /**< PCS_MODE supported */
	CI_DEV_EM_BAND_MODE_GSM850_MODE,                                                                /**< GSM850 supported */
	CI_DEV_EM_BAND_MODE_PGSM_DCS_MODE,                                                              /**< GSM_BAND + DCS_BAND supported  */
	CI_DEV_EM_BAND_MODE_EGSM_DCS_MODE,                                                              /**< EGSM_BAND + DCS_BAND supported */
	CI_DEV_EM_BAND_MODE_PGSM_PCS_MODE,                                                              /**< GSM_BAND + PCS_MODE supported */
	CI_DEV_EM_BAND_MODE_EGSM_PCS_MODE,                                                              /**< EGSM_BAND + PCS_MODE supported */
	CI_DEV_EM_BAND_MODE_GSM850_DCS_MODE,                                                            /**< GSM850 + PCS_MODE supported */
	CI_DEV_EM_BAND_MODE_GSM850_PCS_MODE,                                                            /**< GSM850 + PCS_MODE supported */
	CI_DEV_EM_BAND_MODE_EGSM_MODE_LOCK,                                                             /**< LOCK the MS to EGSM mode. Autoband DISABLED. For use in testing only */
	CI_DEV_EM_BAND_MODE_DCS_MODE_LOCK,                                                              /**< LOCK the MS to DCS mode. Autoband DISABLED. For use in testing only */
	CI_DEV_EM_BAND_MODE_PCS_MODE_LOCK,                                                              /**< LOCK the MS to EGSM/PCS mode. Autoband DISABLED. For use in testing and 900/1900 countries only */
	CI_DEV_EM_BAND_MODE_GSM850_MODE_LOCK,                                                           /**< LOCK the MS to GSM850 mode. Autoband DISABLED. For use in testing only */
	CI_DEV_EM_BAND_MODE_PGSM_PCS_MODE_LOCK,                                                         /**< LOCK the MS to PGSM/PCS mode. Autoband DISABLED. For use in testing and 900/1900 countries only */
	CI_DEV_EM_BAND_MODE_EGSM_PCS_MODE_LOCK,                                                         /**< LOCK the MS to EGSM/PCS mode. Autoband DISABLED. For use in testing and 900/1900 countries only */
	CI_DEV_EM_BAND_MODE_EGSM_DCS_MODE_LOCK,                                                         /**< LOCK the MS to EGSM/DCS mode. Autoband DISABLED. For use in testing and 900/1800 countries only */
	CI_DEV_EM_BAND_MODE_GSM850_DCS_MODE_LOCK,                                                       /**< LOCK the MS to GSM850/DCS1800 mode. Autoband DISABLED. For use in testing and 850/1800 countries only */
	CI_DEV_EM_BAND_MODE_INVALID_BAND_MODE,                                                          /**< Invalid band */
	CI_DEV_EM_BAND_MODE_CI_DEV_ALIGN_32_BIT = 0XFFFFFFF
} _CiDevEngBandModeType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Band mode
 * \sa CIDEV_ENGMODE_BAND_MODE_TYPE */
/** \remarks Common Data Section */
typedef UINT32 CiDevEngBandmodeType;
/**@}*/


/** \brief Mobility Managemant information */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiDevGsmGMMInfo_struct
{
	UINT16 MccLastRegisteredNetwork;                                                                        /**< Mcc of last registered network */
	UINT16 MncLastRegisteredNetwork;                                                                        /**< Mnc of last registered network */
	UINT32 TMSI;                                                                                            /**< TMSI */
	UINT32 PTMSI;                                                                                           /**< PTMSI */
	CiBoolean IsSingleMmRejectCause;                                                                        /**< TRUE - only one MM Reject cause reported during the last EngInfo period; FALSE - other. \sa CCI API Ref Manual */
	CiDevEngMMRejectCauseCodeType MMRejectCause;                                                            /**< The one that was reported during the last EngInfo period Reject cause (10.5.3.6) sent in CM Service Reject, Abort, MM-Status and Location Updating Reject messages to MM from the network. \sa CiDevEngMMRejectCauseCodeType */
	CiDevEngBandmodeType currentBandMode;                                                                   /**< Band mode. \sa CiDevEngBandModeType */
	UINT8 mmState;                                                                                          /**< MM state defined in 4.1.2.1.1. For details see enum MmState in Mm_comm.h */
	UINT8 gmmState;                                                                                         /**< GMM state defined in 4.1.2.1.1. For details see enum GmmState in Gmm_comm.h */
	UINT8 gprsReadyState;                                                                                   /**< 0 - IDLE_STATE / 1 - STANDBY_STATE / 2 - READY_STATE. For details see enum GprsReadyState in grrmrtyp.h */
	UINT16 readyTimerValueInSecs;                                                                           /**< MM ready timer value in sec [value >0] */
} CiDevGsmGMMInfo;

/** \brief Channel modes */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIDEV_ENGMODE_AMR_CHANNEL_MODE_TYPE {
	CI_DEV_EM_AMR_CH_MODE_AMR_FR = 0,                                               /**< CHM_SPEECH_FULL_RATE_VER3 channel mode */
	CI_DEV_EM_AMR_CH_MODE_AMR_HR,                                                   /**< CHM_SPEECH_HALF_RATE_VER3 channel mode */
	CI_DEV_EM_AMR_CH_MODE_EFR,                                                      /**< CHM_SPEECH_FULL_RATE_VER2 / CHM_SPEECH_HALF_RATE_VER2 channel modes */
	CI_DEV_EM_AMR_CH_MODE_FR,                                                       /**< CHM_SPEECH_FULL_RATE channel mode */
	CI_DEV_EM_AMR_CH_MODE_HR,                                                       /**< CHM_SPEECH_HALF_RATE channel mode */
	CI_DEV_EM_AMR_CH_MODE_ALIGN_32_BIT = 0XFFFFFFF                                  /**< For alignment */
} _CiDevEngAMRChannelModeType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Channel modes
 * \sa CIDEV_ENGMODE_AMR_CHANNEL_MODE_TYPE */
/** \remarks Common Data Section */
typedef UINT32 CiDevEngAMRChannelModeType;
/**@}*/

/** \brief AMR codec rate  */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIDEV_ENGMODE_AMR_CODE_TYPE {
	CI_DEV_EM_AMR_CODE_TYPE_4_75_KBPS = 1,                                  /**< AMR codec rate enumeration for 4.75 kbit/s codec rate. */
	CI_DEV_EM_AMR_CODE_TYPE_5_15_KBPS,                                      /**< AMR codec rate enumeration for 5.15 kbit/s codec rate. */
	CI_DEV_EM_AMR_CODE_TYPE_5_9_KBPS,                                       /**< AMR codec rate enumeration for 5.9  kbit/s codec rate. */
	CI_DEV_EM_AMR_CODE_TYPE_6_7_KBPS,                                       /**< AMR codec rate enumeration for 6.7  kbit/s codec rate. */
	CI_DEV_EM_AMR_CODE_TYPE_7_4_KBPS,                                       /**< AMR codec rate enumeration for 7.4  kbit/s codec rate. */
	CI_DEV_EM_AMR_CODE_TYPE_7_95_KPBS,                                      /**< AMR codec rate enumeration for 7.95 kbit/s codec rate. */
	CI_DEV_EM_AMR_CODE_TYPE_10_2_KPBS,                                      /**< AMR codec rate enumeration for 10.2 kbit/s codec rate. */
	CI_DEV_EM_AMR_CODE_TYPE_12_2_KBPS,                                      /**< AMR codec rate enumeration for 12.2 kbit/s codec rate. */
	CI_DEV_EM_AMR_CODE_TYPE_ALIGN_32_BIT = 0XFFFFFFF
} _CiDevEngAMRCodeType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief AMR codec rate
 * \sa CIDEV_ENGMODE_AMR_CODE_TYPE */
/** \remarks Common Data Section */
typedef UINT32 CiDevEngAMRCodeType;
/**@}*/

/** \brief Active Code Set (ACS)  */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiDevEngAMRActiveCodeSetType_struct
{
	UINT8 AcsSize;                                  /**< Number of codes in AMR AS - up to 4 */
	UINT8 res1U8[3];                                /**< (padding) */

	CiDevEngAMRCodeType Acs[4];                     /**< AMR Code Type. \sa CiDevEngAMRCodeType */
} CiDevEngAMRActiveCodeSetType;

/** \brief AMR information  */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiDevAMRInfo_struct
{
	CiDevEngAMRChannelModeType ULChannelMode;                                               /**< Current Uplink Channel Mode {AMR FR, AMR HR, EFR, FR, HR}. \sa CiDevEngAMRChannelModeType */
	CiDevEngAMRChannelModeType DLChannelMode;                                               /**< Current Downlink Channel Mode {{AMR FR, AMR HR, EFR, FR, HR}. \sa CiDevEngAMRChannelModeType */
	CiDevEngAMRActiveCodeSetType ActiveCodeSet;                                             /**< Active Code Set (ACS). \sa CiDevEngAMRActiveCodeSetType_struct */
	CiBoolean DTXUl;                                                                        /**< DTX UL ON/OFF. \sa CCI API Ref Manual  */
	CiBoolean DTXDl;                                                                        /**< If at least one DTX during DL is happened then DTXDI =TRUE ,other = FALSE. \sa CCI API Ref Manual */
	INT16 DlCi;                                                                             /**< L1 calculates average of Confidence measure for frequency offset for all the bursts of the multiframe. [Ratio linear value 0-500] */
	UINT8 RxQualSub;                                                                        /**< Used when DTX is on range is 0-7 */
	UINT8 res1U8[3];                                                                        /**< (padding) */
} CiDevAMRInfo;

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
typedef UINT32 CiDevEngGMMRejectCauseCodeType;
/**@}*/

/** \brief IP Address  */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct   CiDevEngIPAddressType_struct
{
	UINT8 len;                                                              /**< length of the address field */
	UINT8 address[CI_DEV_PDP_IP_V6_SIZE];                                   /**< Address field */
} CiDevEngIPAddressType;

/** \brief Delay class  */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIDEV_ENGMODE_DELAY_CLASS_TYPE {
	CI_DEV_EM_GPRS_DELAY_CLASS_SUBSCRIBED = 0,                                              /**<  Subscribed delay class  */
	CI_DEV_EM_GPRS_DELAY_CLASS_1,                                                           /**<  Delay class 1 */
	CI_DEV_EM_GPRS_DELAY_CLASS_PACKET_CELL_CHANGE_ORDER2,                                   /**<  Delay class 2 */
	CI_DEV_EM_GPRS_DELAY_CLASS_3,                                                           /**<  Delay class 3 */
	CI_DEV_EM_GPRS_DELAY_CLASS_4,                                                           /**<  Delay class 4 (best effort) */
	CI_DEV_EM_GPRS_DELAY_CLASS_RESERVED = 7,                                                /**<  Reserved */
	CI_DEV_EM_DELAY_CLASS_TYPE_ALIGN_32_BIT = 0XFFFFFFF                                     /**< Allignment */
} _CiDevEngDelayClassType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Delay class
 * \sa CIDEV_ENGMODE_DELAY_CLASS_TYPE */
/** \remarks Common Data Section */
typedef UINT32 CiDevEngDelayClassType;
/**@}*/

/** \brief Reliability class type  */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIDEV_ENGMODE_RELIABILITY_CLASS_TYPE {
	CI_DEV_EM_GPRS_RELIAB_CLASS_SUBSCRIBED = 0,                                             /**< Subscribed reliability class. */
	CI_DEV_EM_GPRS_RELIAB_CLASS_1,                                                          /**< Unused. If received, it shall be interpreted as '010' (Note) */
	CI_DEV_EM_GPRS_RELIAB_CLASS_2,                                                          /**< Unacknowledged GTP; Acknowledged LLC and RLC, Protected data */
	CI_DEV_EM_GPRS_RELIAB_CLASS_3,                                                          /**< Unacknowledged GTP and LLC; Acknowledged RLC, Protected data */
	CI_DEV_EM_GPRS_RELIAB_CLASS_4,                                                          /**< Unacknowledged GTP, LLC, and RLC, Protected data */
	CI_DEV_EM_GPRS_RELIAB_CLASS_5,                                                          /**< Unacknowledged GTP, LLC, and RLC, Unprotected data */
	CI_DEV_EM_RELIABILITY_CLASS_TYPE_ALIGN_32_BIT = 0XFFFFFFF                               /**< Allignment */
} _CiDevEngReliabilityClassType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Reliability class type
 * \sa CIDEV_ENGMODE_RELIABILITY_CLASS_TYPE */
/** \remarks Common Data Section */
typedef UINT32 CiDevEngReliabilityClassType;
/**@}*/

/** \brief Peak Throughput Class (1 to 9) - Max rate in octets per second  */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIDEV_ENGMODE_PEAK_THR_TYPE {
	CI_DEV_EM_GPRS_PEAK_THRPT_SUBSCRIBED = 0,                               /**< Subscribed peak throughput */
	CI_DEV_EM_GPRS_PEAK_THRPT_UPTO_1KOCT,                                   /**< Up to 1 000 octet/s */
	CI_DEV_EM_GPRS_PEAK_THRPT_UPTO_2KOCT,                                   /**< Up to 2 000 octet/s */
	CI_DEV_EM_GPRS_PEAK_THRPT_UPTO_4KOCT,                                   /**< Up to 4 000 octet/s */
	CI_DEV_EM_GPRS_PEAK_THRPT_UPTO_8KOCT,                                   /**< Up to 8 000 octet/s */
	CI_DEV_EM_GPRS_PEAK_THRPT_UPTO_16KOCT,                                  /**< Up to 16 000 octet/s */
	CI_DEV_EM_GPRS_PEAK_THRPT_UPTO_32KOCT,                                  /**< Up to 32 000 octet/s */
	CI_DEV_EM_GPRS_PEAK_THRPT_UPTO_64KOCT,                                  /**< Up to 64 000 octet/s */
	CI_DEV_EM_GPRS_PEAK_THRPT_UPTO_128KOCT,                                 /**< Up to 128 000 octet/s */
	CI_DEV_EM_GPRS_PEAK_THRPT_UPTO_256KOCT,                                 /**< Up to 256 000 octet/s */
	CI_DEV_EM_GPRS_PEAK_THRPT_RESERVED = 0x0f,                              /**< Reserved */
	CI_DEV_EM_PEAK_THR_TYPE_ALIGN_32_BIT = 0XFFFFFFF                        /**< Allignment */
} _CiDevEngPeakThroughputType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Peak Throughput Class (1 to 9) - Max rate in octets per second
 * \sa CIDEV_ENGMODE_PEAK_THR_TYPE */
/** \remarks Common Data Section */
typedef UINT32 CiDevEngPeakThroughputType;
/**@}*/

/** \brief EngMode Precedence Class Type  */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIDEV_ENGMODE_PRECEDENCE_CLASS_TYPE {
	CI_DEV_EM_GPRS_PRECED_CLASS_SUBSCRIBED = 0,                     /**< Subscribed precedence (In MS to network direction) */
	CI_DEV_EM_GPRS_PRECED_CLASS_1,                                  /**< High priority (In MS to network direction) */
	CI_DEV_EM_GPRS_PRECED_CLASS_2,                                  /**< Normal priority (In MS to network direction) */
	CI_DEV_EM_GPRS_PRECED_CLASS_3,                                  /**< Low priority (In MS to network direction) */
	CI_DEV_EM_GPRS_PRECED_CLASS_RESERVED = 7,                       /**< Reserved (In MS to network direction) */
	CI_DEV_EM_PRECEDENCE_CLASS_TYPE_ALIGN_32_BIT = 0XFFFFFFF
}  _CiDevEngprecedenceClassType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief EngMode Precedence Class Type
 * \sa CIDEV_ENGMODE_PRECEDENCE_CLASS_TYPE */
/** \remarks Common Data Section */
typedef UINT32 CiDevEngprecedenceClassType;
/**@}*/

/** \brief Mean Throughput - Average rate in octets per hour  */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIDEV_ENGMODE_MEANTHROUGHPUT_TYPE {
	CI_DEV_EM_GPRS_MEAN_THRPT_SUBSCRIBED = 0,                       /**< Subscribed precedence. (In MS to network direction and in network to MS direction) */
	CI_DEV_EM_GPRS_MEAN_THRPT_100_OPH,                              /**< 100 octet/h (In MS to network direction and in network to MS direction) */
	CI_DEV_EM_GPRS_MEAN_THRPT_200_OPH,                              /**< 200 octet/h (In MS to network direction and in network to MS direction) */
	CI_DEV_EM_GPRS_MEAN_THRPT_500_OPH,                              /**< 500 octet/h (In MS to network direction and in network to MS direction) */
	CI_DEV_EM_GPRS_MEAN_THRPT_1K_OPH,                               /**< 1 000 octet/h  (In MS to network direction and in network to MS direction) */
	CI_DEV_EM_GPRS_MEAN_THRPT_2K_OPH,                               /**< 2 000 octet/h  (In MS to network direction and in network to MS direction) */
	CI_DEV_EM_GPRS_MEAN_THRPT_5K_OPH,                               /**< 5 000 octet/h  (In MS to network direction and in network to MS direction) */
	CI_DEV_EM_GPRS_MEAN_THRPT_10K_OPH,                              /**< 10 000 octet/h  (In MS to network direction and in network to MS direction) */
	CI_DEV_EM_GPRS_MEAN_THRPT_20K_OPH,                              /**< 20 000 octet/h  (In MS to network direction and in network to MS direction) */
	CI_DEV_EM_GPRS_MEAN_THRPT_50K_OPH,                              /**< 50 000 octet/h (In MS to network direction and in network to MS direction) */
	CI_DEV_EM_GPRS_MEAN_THRPT_100K_OPH,                             /**< 100 000 octet/h  (In MS to network direction and in network to MS direction) */
	CI_DEV_EM_GPRS_MEAN_THRPT_200K_OPH,                             /**< 200 000 octet/h  (In MS to network direction and in network to MS direction) */
	CI_DEV_EM_GPRS_MEAN_THRPT_500K_OPH,                             /**< 500 000 octet/h (In MS to network direction and in network to MS direction) */
	CI_DEV_EM_GPRS_MEAN_THRPT_1M_OPH,                               /**< 1 000 000 octet/h  (In MS to network direction and in network to MS direction) */
	CI_DEV_EM_GPRS_MEAN_THRPT_2M_OPH,                               /**< 2 000 000 octet/h  (In MS to network direction and in network to MS direction) */
	CI_DEV_EM_GPRS_MEAN_THRPT_5M_OPH,                               /**< 5 000 000 octet/h  (In MS to network direction and in network to MS direction) */
	CI_DEV_EM_GPRS_MEAN_THRPT_10M_OPH,                              /**< 10 000 000 octet/h  (In MS to network direction and in network to MS direction) */
	CI_DEV_EM_GPRS_MEAN_THRPT_20M_OPH,                              /**< 20 000 000 octet/h  (In MS to network direction and in network to MS direction) */
	CI_DEV_EM_GPRS_MEAN_THRPT_50M_OPH,                              /**< 50 000 000 octet/h  (In MS to network direction and in network to MS direction) */
	CI_DEV_EM_GPRS_MEAN_THRPT_RESERVED       = 30,                  /**<  Reserved. (In MS to network direction and in network to MS direction) */
	CI_DEV_EM_GPRS_MEAN_THRPT_BEST_EFFORT = 31,                     /**<  Best effort. The value indicates that throughput shall be made available to the MS on a per need and availability basis. (In MS to network direction and in network to MS direction) */
	CI_DEV_EM_MEANTHROUGHPUT_TYPE_ALIGN_32_BIT = 0XFFFFFFF
}  _CiDevMeanThroughputType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Mean Throughput - Average rate in octets per hour
 * \sa CIDEV_ENGMODE_MEANTHROUGHPUT_TYPE */
/** \remarks Common Data Section */
typedef UINT32 CiDevMeanThroughputType;
/**@}*/

/** \brief Quality Of Service information structure  */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct  CiDevEngQualityOfServiceType_struct
{
	CiDevEngDelayClassType delayClass;                              /**< GPRS Delay Class. \sa CiDevEngDelayClassType */
	CiDevEngReliabilityClassType reliabilityClass;                  /**< \sa CiDevEngReliabilityClassType */
	CiDevEngPeakThroughputType peakThroughput;                      /**< \sa CiDevEngPeakThroughputType */
	CiDevEngprecedenceClassType precedenceClass;                    /**< \sa CiDevEngprecedenceClassType */
	CiDevMeanThroughputType meanThroughput;                         /**< \sa CiDevMeanThroughputType */
} CiDevEngQualityOfServiceType;

/** \brief APN type info  */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct  CiDevEngAPNType_struct
{
	INT8 length;
	UINT8 res1U8[3];                                                        /**< (padding) */
	UINT8 name [CI_DEV_MAX_APN_NAME];                                       /**< IP address or an ASCII character string that identifies the GGSN */
} CiDevEngAPNType;

/** \brief PDP Context information  */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiDevEngActivePDPContextinfoType_struct
{
	CiDevEngIPAddressType IPaddress;                                /**< IP Adress. \sa CiDevEngIPAddressType_struct */
	CiDevEngQualityOfServiceType QOS;                               /**< Quality Of Service information. \sa CiDevEngQualityOfServiceType_struct */
	CiDevEngAPNType APN;                                            /**< \sa CiDevEngAPNType_struct */
} CiDevEngActivePDPContextinfoType;

/** <paramref name="CI_DEV_PRIM_ACTIVE_PDP_CONTEXT_ENGMODE_IND">   */
//ICAT EXPORTED STRUCT
typedef struct CiDevPrimActivePDPContextEngModeInd_struct {
	UINT8 Index;                                                                                    /**< PDP Context index */
	CiDevEngActivePDPContextinfoType ActivePDPContextinfo;                                          /**< PDP Context info. \sa CiDevEngActivePDPContextinfoType_struct */
} CiDevPrimActivePDPContextEngModeInd;

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
	CiDevEngGMMRejectCauseCodeType GMMRejectCause;                                                                  /**< This one is, reported during the last EngInfo period, GMM Reject cause (10.5.3.6) sent to MM from the network. \sa CiDevEngGMMRejectCauseCodeType */

	UINT8 cValue;                                                                                                   /**< C Value */
	UINT8 txPower;                                                                                                  /**< Transmit Power of every block - TBD: phase 2 */
	UINT8 ulTimeSlot;                                                                                               /**< Uplink Time Slot allocation bitmap. */
	UINT8 dlTimeSlot;                                                                                               /**< Downlink Time Slot allocation bitmap. */

	CiDevGprsServiceType gprsServiceType;                                                                           /**< GPRS service type GPRS/EDGE. \sa CiDevGprsServiceType */
	CiDevModulationScheme ulMod;                                                                                    /**< Uplink Modulation - TBD: phase 2. \sa CiDevModulationScheme */
	CiDevModulationScheme dlMod;                                                                                    /**< Downlink Modulation - TBD: phase 2. \sa CiDevModulationScheme */
	/*Michal Bukai - I-Mate Addition. Start:*/
	UINT8 USFGranularity;                                                                                           /**< USF granularity defines the number of RLC/MAC blocks to transmit if USF is present. 0 - the mobile station shall transmit one RLC/MAC block, 1 - the mobile station shall transmit four consecutive RLC/MAC blocks */
	UINT32 ULThroughput;                                                                                            /**< UL throughput in octets per second */
	UINT32 DLThroughput;                                                                                            /**< DL throughput in octets per second */
	/*Michal Bukai - I-Mate Addition. End*/
	CiDevEgprsBepCvMeanInfo egprsBep;                                                                               /**< BEP Period - TBD: phase 2. \sa CiDevEgprsBepCvMeanInfo_struct */
} CiDevGPRSPTMInfo;


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
	/*Michal Bukai - I-Mate Addition. Start:*/
	CiBoolean IsForbiddenLA;                        /**< Indicates if cell belongs to forbidden location area. FALSE: Cell is not in forbidden LA or forbidden status is unknown; TRUE: Cell is in forbidden LA. \sa CCI API Ref Manual */
	CiDevCellPrioriytType CellPriority;             /**< Cell priority for cell selection or reselection. Cell priority can be normal, low or barred. \sa CiDevCellPrioriytType */
	/*Michal Bukai - I-Mate Addition. End*/
} CiDevGsmNeighboringCellInfo;

/** \brief Engineering Mode: 2G (UMTS) Neighboring Cell Info structure  */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiDevCiDevGsmUMTSNeighboringCellInfo {
	UINT16 arfcn;                           /**< Absolute Radio Frequency Channel Number */
#ifndef TD_SUPPORT
	UINT16 psc;                             /**< Primary Scrambling Code */
	INT16 cpichRSCP;                        /**< CPICH Received Signal Code Power */
	INT16 cpichEcN0;                        /**< CPICH Ec/N0 */
#else
	INT16 pccpchRSCP;                       /**< PCCPCH Received Signal Code Power */
	INT8 cellParameterId;                   /**< Cell parameter id */
	UINT8 res1U8[3];                        /**< (padding) */
#endif
} CiDevGsmUMTSNeighboringCellInfo;

/** \brief Engineering Mode: GSM Data structure  */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiDevEngModeGsmData_struct
{
	CiDevGsmServingCellInfo svcCellInfo;                                                                                                                    /**< Serving Cell information. \sa CiDevGsmServingCellInfo_struct */

	CiDevGsmGMMInfo GMMInfo;                                                                                                                                /**< Mobility Managemant information.  \sa CiDevGsmGMMInfo_struct */
	CiDevAMRInfo AMRInfo;                                                                                                                                   /**< AMR information. \sa CiDevAMRInfo_struct */
	CiDevGPRSPTMInfo GPRSPTMInfo;                                                                                                                           /**< Packet data information. \sa CiDevGPRSPTMInfo_struct */

	UINT8 numNCells;                                                                                                                                        /**< Number of Neighboring Cells [0..CI_DEV_MAX_GSM_NEIGHBORING_CELLS] */
	UINT8 res1U8[3];                                                                                                                                        /**< (padding) */
	CiDevGsmNeighboringCellInfo nbCellInfo[ CI_DEV_MAX_GSM_NEIGHBORING_CELLS ];                                                                             /**< Neighboring Cell information. \sa CiDevGsmNeighboringCellInfo_struct */

	UINT8 numInterRATNCells;                                                                                                                                /**< Number of InterRAT Cells [0..CI_DEV_MAX_GSM_NEIGHBORING_CELLS] */
	UINT8 res2U8[3];                                                                                                                                        /**< (padding) */
	CiDevGsmUMTSNeighboringCellInfo InterRATCellInfo[ CI_DEV_MAX_GSM_NEIGHBORING_CELLS ];                                                                   /**< InterRAT Cell information. \sa CiDevCiDevGsmUMTSNeighboringCellInfo */

} CiDevEngModeGsmData;

/* ===================================================================================
		3G (UMTS) Engineering Mode Structures
   ===================================================================================*/

/** \brief Engineering Mode: 3G (UMTS) Serving Cell Measurements structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiDevUmtsServingCellMeasurements_struct
{
#ifndef TD_SUPPORT
	INT16 cpichRSCP;                /**<    CPICH Received Signal Code Power; In UMTS FDD messages RSCP is
						transmitted as an integer value in the range of -120 dBm to -25 dBm.
						The value is coded into integers from -5 to 99 according to 3GPP's 25.133  */
#else
	INT16 pccpchRSCP;         /**<  PCCPCH Received Signal Code Power; In UMTS TDD messages RSCP is
						transmitted as an integer value in the range of -120 dBm to -25 dBm.
						The value is coded into integers from -5 to 99 according to 3GPP's 25.133  */
#endif
	INT16 utraRssi;                         /**<    UTRA Carrier RSSI.range 0 - 63.
									UTRA_carrier_RSSI_LEV _00: UTRA carrier RSSI < -94 dBm
									UTRA_carrier_RSSI_LEV _01: -94 dBm  ?UTRA carrier RSSI < -93 dBm
									UTRA_carrier_RSSI_LEV _02: -93 dBm  ?UTRA carrier RSSI < -92 dBm
									UTRA_carrier_RSSI_LEV _61: -32 dBm  ?UTRA carrier RSSI < -33 dBm
									UTRA_carrier_RSSI_LEV _62: -33 dBm  ?UTRA carrier RSSI < -32 dBm
									UTRA_carrier_RSSI_LEV _63: -32 dBm  ?UTRA carrier RSSI	   */
#ifndef TD_SUPPORT
	INT16 cpichEcN0;                        /**< CPICH Ec/N0 */
	INT16 sQual;                            /**< Cell Selection Quality (Squal) */
#endif
	INT16 sRxLev;                           /**< Cell Selection Rx Level (Srxlev) */
	INT16 txPower;                          /**< UE Transmitted Power */

/*  TBD: Transport Channel BLER */

} CiDevUmtsServingCellMeasurements;

/** \brief Engineering Mode: 3G (UMTS) Serving Cell PLMN/Cell Parameters structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiDevUmtsServingCellParameters_struct
{
	UINT8 rac;                                      /**< Routing Area Code */
	UINT8 nom;                                      /**< Network Operation Mode */

	UINT16 mcc;                                     /**< Mobile Country Code */
	UINT16 mnc;                                     /**< Mobile Network Code */
	UINT16 lac;                                     /**< Location Area Code */
	UINT32 ci;                                      /**< Cell Identity; as per 3G TS 25.331, 10.3.2.2 (28 bits) */
	UINT16 uraId;                                   /**< URA Identity */
#ifndef TD_SUPPORT
	UINT16 psc;                                     /**< Primary Scrambling Code */
#else
	INT8 cellParameterId;                           /**< Cell parameter id */
	UINT8 res1U8;                                   /**< (padding) */
#endif
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
} CiDevUmtsServingCellParameters;

/** \brief Cipher algorithm type */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CI_CIPHER_ALGORITHM_TYPE
{
	CI_CIPHER_ALGORITHM_TYPE_UEA0,          /**< as per 3G TS 25.331, 10.3.3.4 */
	CI_CIPHER_ALGORITHM_TYPE_UEA1           /**< as per 3G TS 25.331, 10.3.3.4 */
} _CiCipherAlgorithmType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Cipher algorithm type
* \sa CI_CIPHER_ALGORITHM_TYPE */
/** \remarks Common Data Section */
typedef UINT8 CiCipherAlgorithmType;
/**@}*/

/** \brief Cipher algorithm info */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiCipherAlgorithm_struct
{
	CiBoolean algPresent;                           /**< indicates if an algorithm is defined. \sa CCI API Ref Manual */
	CiCipherAlgorithmType cipherAlg;                /**< Cipher Algorithm type. \sa CiCipherAlgorithmType  */
	CiBoolean cipherOn;                             /* Ciphering Status = On/Off. \sa CCI API Ref Manual */
}  CiCipherAlgorithmInfo;

/** \brief Engineering Mode: 3G (UMTS) UE Operation Status structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiDevUmtsUeOperationStatus_struct
{
	CiDevEngModeUeRrcState rrcState;                                                                                /**< RRC State. \sa CiDevEngModeUeRrcState */
	UINT8 numLinks;                                                                                                 /**< Number of Radio Links */

	UINT16 srncId;                                                                                                  /**<  U-RNTI: SRNC Identifier */
	UINT32 sRnti;                                                                                                   /**<  U-RNTI: S-RNTI */
	CiCipherAlgorithmInfo csCipherInfo;                                                                             /**<  CS domain Ciphering info. \sa CiCipherAlgorithm_struct */
	CiCipherAlgorithmInfo psCipherInfo;                                                                             /**<  PS domain Ciphering info. \sa CiCipherAlgorithm_struct */
	CiBoolean HSDPAActive;                                                                                          /**<  TRUE- HSDPA is activated currently; FALSE - other.\sa CCI API Ref Manual  */
	CiBoolean HSUPAActive;                                                                                          /**<  TRUE- HSUPA is activated currently; FALSE - other.\sa CCI API Ref Manual  */
	UINT16 MccLastRegisteredNetwork;                                                                                /**<  Mcc of last registered network */
	UINT16 MncLastRegisteredNetwork;                                                                                /**<  Mnc of last registered network */
	INT32 TMSI;                                                                                                     /**<  TMSI */
	INT32 PTMSI;                                                                                                    /**<  PTMSI */
	CiBoolean IsSingleMmRejectCause;                                                                                /**<  TRUE - only one MM Reject cause reported during the last EngInfo period; FALSE - other. \sa CCI API Ref Manual  */
	CiBoolean IsSingleGmmRejectCause;                                                                               /**<  TRUE - only one GMM Reject cause reported during the last EngInfo period; FALSE - other. \sa CCI API Ref Manual  */
	CiDevEngMMRejectCauseCodeType MMRejectCause;                                                                    /**<  The one that was reported during the last EngInfo period Reject cause (10.5.3.6) sent in CM Service Reject, Abort, MM-Status and Location Updating Reject messages to MM from the network. \sa CiDevEngMMRejectCauseCodeType */
	CiDevEngGMMRejectCauseCodeType GMMRejectCause;                                                                  /**<  This one is, reported during the last EngInfo period, GMM Reject cause (10.5.3.6) sent to MM from the network.  \sa CiDevEngGMMRejectCauseCodeType */
	UINT8 mmState;                                                                                                  /**<  MM state defined in 4.1.2.1.1. For details see enum MmState in Mm_comm.h */
	UINT8 gmmState;                                                                                                 /**<  MM state defined in 4.1.2.1.1. For details see enum MmState in Mm_comm.h */
	UINT8 gprsReadyState;                                                                                           /**<  0 - IDLE_STATE / 1 - STANDBY_STATE / 2 - READY_STATE. For details see enum GprsReadyState in grrmrtyp.h */
	UINT16 readyTimerValueInSecs;                                                                                   /**<  MM ready timer value in sec [value >0] */
	UINT8 NumActivePDPContext;                                                                                      /**<  Number of active PDP Contexts */
	/*Michal Bukai - I-Mate Addition. Start:*/
	UINT32 ULThroughput;                                                                                            /**< UL throughput in octets per second */
	UINT32 DLThroughput;                                                                                            /**< DL throughput in octets per second */
	/*Michal Bukai - I-Mate Addition. End*/
} CiDevUmtsUeOperationStatus;

/** \brief Engineering Mode 3G (UMTS) Serving Cell Information structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiDevUmtsServingCellInfo_struct
{
	CiBoolean sCellMeasPresent;                                                             /**< TRUE - is present; FALSE - is not present. \sa CCI API Ref Manual */
	CiBoolean sCellParamPresent;                                                            /**< TRUE - is present; FALSE - is not present. \sa CCI API Ref Manual */
	CiBoolean ueOpStatusPresent;                                                            /**< TRUE - is present; FALSE - is not present. \sa CCI API Ref Manual */
	UINT8 res1U8;                                                                           /**< padding */
	CiDevUmtsServingCellMeasurements sCellMeas;                                             /**< SCell Measurements. \sa CiDevUmtsServingCellMeasurements_struct */
	CiDevUmtsServingCellParameters sCellParam;                                              /**< PLMN/Cell Parameters. \sa CiDevUmtsServingCellParameters_struct    */
	CiDevUmtsUeOperationStatus ueOpStatus;                                                  /**< UE Operation Status. \sa CiDevUmtsUeOperationStatus_struct                 */
} CiDevUmtsServingCellInfo;


/** \brief Engineering Mode 3G (UMTS) Active Set Information structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiDevUmtsASInfo_struct
{
	UINT32 ci;                                                      /**< Cell Identity; as per 3G TS 25.331, 10.3.2.2 (28 bits) */
	UINT16 psc;                                                     /**< Primary Scrambling Code */
	CiBoolean HSDPAServingCell;                                     /**< Indicates if this cell is the HSDPA serving cell. \sa CCI API Ref Manual */
	CiBoolean HSUPAServingCell;                                     /**< Indicates if this cell is part for the HSUPA active set - relevant for rel.6. \sa CCI API Ref Manual */
	INT16 cpichRSCP;                                                /**< CPICH Received Signal Code Power; In UMTS FDD messages RSCP is transmitted as an integer value in the range of -120 dBm to -25 dBm. The value is coded into integers from -5 to 99 according to 3GPP's 25.133  */
	INT16 cpichEcN0;                                                /**< CPICH Ec/N0 [dB];  */
	UINT16 mcc;                                                     /**< Mobile Country Code */
	UINT16 mnc;                                                     /**< Mobile Network Code */
	UINT16 lac;                                                     /**< Location Area Code */
	UINT8 rac;                                                      /**< Routing Area Code, range 0-1 (1 bit) */
} CiDevUmtsASInfo;

/** <paramref name="CI_DEV_PRIM_UMTS_ENGMODE_ACTIVE_SET_INFO_IND">   */
//ICAT EXPORTED STRUCT
typedef struct CiDevPrimUmtsEngmodeActiveSetInfoInd_struct
{
	UINT8 NumCells;                                                                                 /**< Number of cells in active set */
	UINT16 arfcn;                                                                                   /**< Absolute Radio Frequency Channel Number; number 0-1023  */
	UINT8 res1U8;                                                                                   /**< (padding) */
	CiDevUmtsASInfo ASinfo[CI_DEV_MAX_CELLS_IN_AS];                                                 /**< Active Set Information. \sa CiDevUmtsASInfo_struct */
} CiDevPrimUmtsEngmodeActiveSetInfo;


/** \brief  Engineering Mode 3G (UMTS) Intra-Frequency/Inter-Frequency FDD Cell Information structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
#ifndef TD_SUPPORT
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
} CiDevUmtsFddNeighborInfo;
#else
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
} CiDevUmtsTddNeighborInfo;
#endif

/** \brief  Engineering Mode 3G (UMTS) Inter-RAT GSM Cell Information structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiDevUmtsGsmNeighborInfo_struct
{
	/* Measurements */
	INT16 gsmRssi;                  /**< GSM Carrier RSSI */
	INT16 rxLev;                    /**< Cell Selection Rx Level */
	INT16 C1;                       /**< Path loss criterion parameter #1 */
	INT16 C2;                       /**< Path loss criterion parameter #2 */

	/* PLMN/Cell Parameters */
	UINT16 mcc;                     /**< Mobile Country Code */
	UINT16 mnc;                     /**< Mobile Network Code */
	UINT16 lac;                     /**< Location Area Code */
	UINT16 ci;                      /**< Cell Identity */
	UINT16 arfcn;                   /**< Absolute Radio Frequency Channel Number */
	UINT8 bsic;                     /**< Base Transceiver Station Identity Code; range 0h-3Fh (6 bits) */
	UINT8 res1U8;
} CiDevUmtsGsmNeighborInfo;

/** \brief  Engineering Mode: UMTS Data structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiDevEngModeUmtsData_struct
{
	/* Serving Cell information */
	CiDevUmtsServingCellInfo svcCellInfo;                                                                                   /**< Serving Cell information.  \sa CiDevUmtsServingCellInfo_struct */
	UINT8 LastsCellParamsListIndex;                                                                                         /**< Number of serving cells in last serving cells list */
	CiDevUmtsServingCellParameters LastsCellParamsList[CI_DEV_MAX_CELLS_IN_AS];                                             /**< PLMN/Cell Parameters of the list.  \sa CiDevUmtsServingCellParameters_struct */

	CiDevPrimUmtsEngmodeActiveSetInfo ASInfo;                                                                               /**< Active set information.  \sa CiDevPrimUmtsEngmodeActiveSetInfoInd_struct */

	/* Neighboring Cell information */
	UINT8 numIntraFreq;                                                                                                                     /**< Number of Intra-Frequency FDD Cells */
	UINT8 numInterFreq;                                                                                                                     /**< Number of Inter-Frequency FDD Cells */
	UINT8 numInterRAT;                                                                                                                      /**< Number of Inter-RAT GSM Cells */
	UINT8 res1U8;                                                                                                                           /**< (padding) */
#ifndef TD_SUPPORT
	CiDevUmtsFddNeighborInfo intraFreq[ CI_DEV_MAX_UMTS_NEIGHBORING_CELLS ];                                                                /**< intraFreq info. \sa CiDevUmtsFddNeighborInfo_struct */
	CiDevUmtsFddNeighborInfo interFreq[ CI_DEV_MAX_UMTS_NEIGHBORING_CELLS ];                                                                /**< interFreq info. \sa CiDevUmtsFddNeighborInfo_struct */
#else
	CiDevUmtsTddNeighborInfo intraFreq[ CI_DEV_MAX_UMTS_NEIGHBORING_CELLS ];                                                                /**< intraFreq info. \sa CiDevUmtsTddNeighborInfo_struct */
	CiDevUmtsTddNeighborInfo interFreq[ CI_DEV_MAX_UMTS_NEIGHBORING_CELLS ];                                                                /**< interFreq info. \sa CiDevUmtsTddNeighborInfo_struct */
#endif
	CiDevUmtsGsmNeighborInfo interRAT[ CI_DEV_MAX_UMTS_NEIGHBORING_CELLS ];                                                                 /**< interRAT info.  \sa CiDevUmtsGsmNeighborInfo_struct */

	CiDevEngActivePDPContextinfoType activePDPContextinfo[CI_DEV_ENG_MAX_ACTIVE_PDP_CONTEXT];                                               /**< ActivePDPContext data.  \sa CiDevEngActivePDPContextinfoType_struct */
} CiDevEngModeUmtsData;


/*===================================================================================
	 El Gato Gordo: Main Engineering Mode Information structures
   ===================================================================================*/

/** \brief  Engineering Mode: GSM Data structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiDevEngModeGsmDataInfo_struct
{
	CiDevEngModeGsmData data;                                                                       /* GSM Data. \sa CiDevEngModeGsmData_struct  */
	CiDevEngActivePDPContextinfoType activePDPContextinfo[CI_DEV_ENG_MAX_ACTIVE_PDP_CONTEXT];       /* ActivePDPContext data. \sa CiDevEngActivePDPContextinfoType_struct */
} CiDevEngModeGsmDataInfo;

/* VADIM fix for CQ00082654 - debug only */
/** \brief  UMTS and GSM Data Information structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiDevEngModeGsmUmtsData_struct {
	CiDevEngModeUmtsData umtsData;                          /**< UMTS Data. \sa CiDevEngModeUmtsData_struct */
	CiDevEngModeGsmDataInfo gsmData;                        /**< GSM Data. \sa CiDevEngModeGsmDataInfo_struct */
} CiDevEngModeGsmUmtsData;
/* VADIM fix for CQ00082654 - debug only */

/** \brief  Main Engineering Mode Information structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiDevEngModeInfo_struct
{
	CiDevEngModeState mode;                                                 /**< Current Mode (Idle/Dedicated). \sa CiDevEngModeState */
	CiDevEngModeNetwork network;                                            /**< Network Type (GSM/UMTS). \sa CiDevEngModeNetwork */
	UINT8 res1U8[2];                                                        /**< (padding) */

/* VADIM fix for CQ00082654 - debug only */
	CiDevEngModeGsmUmtsData data;
	/* UMTS or GSM Data -- depending on Network Type */
//    union
//    {
//      CiDevEngModeUmtsData                    umtsData;       /**< UMTS Data. \sa CiDevEngModeUmtsData_struct */
//      CiDevEngModeGsmDataInfo                gsmData;         /**< GSM Data. \sa CiDevEngModeGsmDataInfo_struct */
//    } data;
/* VADIM fix for CQ00082654 - debug only */

} CiDevEngModeInfo;

/** <paramref name="CI_DEV_PRIM_STATUS_IND">   */
//ICAT EXPORTED STRUCT
typedef struct CiDevPrimStatusInd_struct {
	CiDevStatus status;     /**< Device status.  \sa CiDevStatus */
} CiDevPrimStatusInd;

/** <paramref name="CI_DEV_PRIM_GET_MANU_ID_REQ">   */
typedef CiEmptyPrim CiDevPrimGetManuIdReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_GET_MANU_ID_CNF">   */
typedef struct CiDevPrimGetManuIdCnf_struct {
	CiDevRc rc;                             /**< Result code.  \sa CiDevRc */
	CiString manuStr;                       /**< manufacture id string. refer to "Cellular Interface Application Programming Interface", revision i0.6. Max length is 2048. \sa CCI API Ref Manual */
} CiDevPrimGetManuIdCnf;

/** <paramref name="CI_DEV_PRIM_GET_MODEL_ID_REQ">   */
typedef CiEmptyPrim CiDevPrimGetModelIdReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_GET_MODEL_ID_CNF">   */
typedef struct CiDevPrimGetModelIdCnf_struct {
	CiDevRc rc;                             /**< Result code.   \sa CiDevRc */
	CiString modelStr;                      /**< Model id string.  Refer to "Cellular Interface Application Programming Interface", revision i0.6. Max length is 2048. \sa CCI API Ref Manual  */
} CiDevPrimGetModelIdCnf;

/** <paramref name="CI_DEV_PRIM_GET_REVISION_ID_REQ">   */
typedef CiEmptyPrim CiDevPrimGetRevisionIdReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_GET_REVISION_ID_CNF">   */
typedef struct CiDevPrimGetRevisionIdCnf_struct {
	CiDevRc rc;                             /**< Result code.   \sa CiDevRc */
	CiString revisionStr;                   /**< Revision id string.  Refer to "Cellular Interface Application Programming Interface", revision i0.6. Max length is 2048. \sa CCI API Ref Manual  */
} CiDevPrimGetRevisionIdCnf;

/** <paramref name="CI_DEV_PRIM_GET_SERIALNUM_ID_REQ">   */
typedef CiEmptyPrim CiDevPrimGetSerialNumIdReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_GET_SERIALNUM_ID_CNF">   */
typedef struct CiDevPrimGetSerialNumIdCnf_struct {
	CiDevRc rc;                                     /**< Result code.   \sa CiDevRc */
	CiString serialNumStr;                          /**< Serial number id string.  Refer to "Cellular Interface Application Programming Interface", revision i0.6. Max length is 2048. \sa CCI API Ref Manual */
} CiDevPrimGetSerialNumIdCnf;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_SET_FUNC_REQ">   */
typedef struct CiDevPrimSetFuncReq_struct {
	CiDevFunc func;                 /**< Functionality, relate to power level a phone should draw. \sa CiDevFunc */
	CiBoolean reset;                /**< TRUE-- reset before setting to functionality power level. FALSE - no reset before setting, default. \sa CCI API Ref Manual */
	CiBoolean IsCommFeatureConfig;  /**< TRUE - CommFeatureConfig should be changed, next field present */
	CiBitRange CommFeatureConfig;   /**< communication feature configuration Enable/Disable like CSD/FAX/etc */
} CiDevPrimSetFuncReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_SET_FUNC_CNF">   */
typedef struct CiDevPrimSetFuncCnf_struct {
	CiDevRc rc;             /**< Result code. \sa CiDevRc */
} CiDevPrimSetFuncCnf;

/** <paramref name="CI_DEV_PRIM_GET_FUNC_REQ">   */
typedef CiEmptyPrim CiDevPrimGetFuncReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_GET_FUNC_CNF">   */
typedef struct CiDevPrimGetFuncCnf_struct {
	CiDevRc rc;                     /**< Result code. \sa CiDevRc */
	CiDevFunc func;                 /**< Functionality, relate to power level a phone should draw. \sa CiDevFunc */
	CiBitRange CommFeatureConfig;   /**< communication feature configuration Enable/Disable like CSD/FAX/etc */
} CiDevPrimGetFuncCnf;

/** <paramref name="CI_DEV_PRIM_GET_FUNC_CAP_REQ">   */
typedef CiEmptyPrim CiDevPrimGetFuncCapReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_GET_FUNC_CAP_CNF">   */
typedef struct CiDevPrimGetFuncCapCnf_struct {
	CiDevRc rc;                     /**< Result code. \sa CiDevRc */
	CiBitRange bitsFunc;            /**< Supported functionality setting. Refer to "Cellular Interface Application Programming Interface", revision i0.6. \sa CCI API Ref Manual */
	CiBitRange bitsReset;           /**< Supported reset setting. Refer to "Cellular Interface Application Programming Interface", revision i0.6. \sa CCI API Ref Manual */
} CiDevPrimGetFuncCapCnf;

//ICAT EXPORTED STRUCT
typedef struct CiDevPrimSetGsmPowerClassReq_struct {
	CiDevBand band;
	CiDevPwCls pwcls;
} CiDevPrimSetGsmPowerClassReq;

//ICAT EXPORTED STRUCT
typedef struct CiDevPrimSetGsmPowerClassCnf_struct {
	CiDevRc rc;
} CiDevPrimSetGsmPowerClassCnf;

//ICAT EXPORTED STRUCT
typedef struct CiDevPrimGetGsmPowerClassReq_struct {
	CiDevBand band;
} CiDevPrimGetGsmPowerClassReq;

//ICAT EXPORTED STRUCT
typedef struct CiDevPrimGetGsmPowerClassCnf_struct {
	CiDevRc rc;
	CiDevPwCls curPwCls;
	CiDevPwCls defPwCls;
} CiDevPrimGetGsmPowerClassCnf;

//ICAT EXPORTED STRUCT
typedef struct CiDevPrimGetGsmPowerClassCapReq_struct {
	CiDevBand band;
} CiDevPrimGetGsmPowerClassCapReq;

//ICAT EXPORTED STRUCT
typedef struct CiDevPrimGetGsmPowerClassCapCnf_struct {
	CiDevRc rc;
	CiBitRange bitsPwCls;
} CiDevPrimGetGsmPowerClassCapCnf;

/** <paramref name="CI_DEV_PRIM_PM_POWER_DOWN_REQ">   */
typedef CiEmptyPrim CiDevPrimPmPowerDownReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_PM_POWER_DOWN_CNF">   */
typedef struct CiDevPrimPmPowerDownCnf_struct {
	CiDevRc rc;     /**< Result code. \sa CiDevRc */
} CiDevPrimPmPowerDownCnf;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_SET_ENGMODE_REPORT_OPTION_REQ">   */
typedef struct CiDevPrimSetEngmodeRepOptReq_struct
{
	CiDevEngModeReportOption type;                          /**< Report Type. \sa CiDevEngModeReportOption */
	UINT16 interval;                                        /**< Report Interval (seconds) for PERIODIC */
} CiDevPrimSetEngmodeRepOptReq;


//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_SET_ENGMODE_REPORT_OPTION_CNF">   */
typedef struct CiDevPrimSetEngmodeRepOptCnf_struct
{
	CiDevRc rc;             /**< Result code. \sa  CiDevRc */
	UINT8 res1U8[2];
} CiDevPrimSetEngmodeRepOptCnf;

/** <paramref name="CI_DEV_PRIM_GET_ENGMODE_INFO_REQ">   */
typedef CiEmptyPrim CiDevPrimGetEngmodeInfoReq;


//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_GET_ENGMODE_INFO_CNF">   */
typedef struct CiDevPrimGetEngmodeInfoCnf_struct
{
	CiDevRc rc;                                             /**< Result code. \sa CiDevRc */
	CiDevEngModeState mode;                                 /**< Current Mode (Idle/Dedicated). \sa CiDevEngModeState */
	CiDevEngModeNetwork network;                            /**< Network Type (GSM/UMTS). \sa CiDevEngModeNetwork */
} CiDevPrimGetEngmodeInfoCnf;

//ICAT EXPORTED STRUCT
typedef struct CiDevPrimEngmodeInfoInd_struct
{
	CiDevEngModeInfo info;
} CiDevPrimEngmodeInfoInd;


//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_GSM_ENGMODE_INFO_IND">   */
/*Michal Bukai - Modified this struc to support - I-Mate Addition*/
typedef struct CiDevPrimGsmEngmodeInfoInd_struct
{
	CiDevEngModeState mode;                         /**< Current Mode (Idle/Dedicated). \sa CiDevEngModeState */
	UINT8 releaseVersion;                           /**< 3GPP release versions */
	UINT8 res1U8[2];                                /**< (padding) */
	CiDevEngModeGsmData info;                       /**< GSM Engineering Mode information. \sa CiDevEngModeGsmData_struct */
} CiDevPrimGsmEngmodeInfoInd;


//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_UMTS_ENGMODE_SVCCELL_INFO_IND">   */
typedef struct CiDevPrimUmtsEngmodeSvcCellInfoInd_struct
{
	CiDevEngModeState mode;                                         /**< Current Mode (Idle/Dedicated). \sa CiDevEngModeState */
	UINT8 res1U8[3];                                                /**< (padding) */
	CiDevUmtsServingCellInfo info;                                  /**< Engineering Mode 3G (UMTS) Serving Cell Information. \sa CiDevUmtsServingCellInfo_struct */
} CiDevPrimUmtsEngmodeScellInfoInd;


//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_UMTS_ENGMODE_INTRAFREQ_INFO_IND">   */
typedef struct CiDevPrimUmtsEngmodeIntraFreqInfoInd_struct
{
	UINT8 numIntraFreq;                                                                                                                             /**< Number of Intra-Frequency FDD Cells */
	UINT8 res1U8[3];                                                                                                                                /**< (padding) */
#ifndef TD_SUPPORT
	CiDevUmtsFddNeighborInfo intraFreq[ CI_DEV_MAX_UMTS_NEIGHBORING_CELLS ];                                                                        /**< Intra-Frequency Info. \sa CiDevUmtsFddNeighborInfo_struct */
#else
	CiDevUmtsTddNeighborInfo intraFreq[ CI_DEV_MAX_UMTS_NEIGHBORING_CELLS ];                                                                        /**< Intra-Frequency Info. \sa CiDevUmtsTddNeighborInfo_struct */
#endif
} CiDevPrimUmtsEngmodeIntraFreqInfoInd;


//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_UMTS_ENGMODE_INTERFREQ_INFO_IND">   */
typedef struct CiDevPrimUmtsEngmodeInterFreqInfoInd_struct
{
	UINT8 numInterFreq;                                                                                                                             /**< Number of Inter-Frequency FDD Cells */
	UINT8 res1U8[3];                                                                                                                                /**< (padding) */
#ifndef TD_SUPPORT
	CiDevUmtsFddNeighborInfo interFreq[ CI_DEV_MAX_UMTS_NEIGHBORING_CELLS ];                                                                        /**< Inter-Frequency Info. \sa CiDevUmtsFddNeighborInfo_struct */
#else
	CiDevUmtsTddNeighborInfo interFreq[ CI_DEV_MAX_UMTS_NEIGHBORING_CELLS ];                                                                        /**< Inter-Frequency Info. \sa CiDevUmtsTddNeighborInfo_struct */
#endif
} CiDevPrimUmtsEngmodeInterFreqInfoInd;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_UMTS_ENGMODE_INTERRAT_INFO_IND">   */
typedef struct CiDevPrimUmtsEngmodeInterRatInfoInd_struct
{
	UINT8 numInterRAT;                                                                                                                              /**< Number of Inter-RAT GSM Cells */
	UINT8 res1U8[3];                                                                                                                                /**< (padding) */
	CiDevUmtsGsmNeighborInfo interRAT[ CI_DEV_MAX_UMTS_NEIGHBORING_CELLS ];                                                                         /**< Inter-Rat Info. \sa CiDevUmtsGsmNeighborInfo_struct */
} CiDevPrimUmtsEngmodeInterRatInfoInd;

/** \brief  Test result */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CI_DEV_SELF_TEST_RESULT
{
	CI_DEV_SELF_TEST_OK,
	CI_DEV_SELF_TEST_FAILURE,
	CI_DEV_SELF_TEST_INCOMPATIBLE_SW_VERSION,
	CI_DEV_SELF_TEST_ILLEGAL_BATTERY,

	CI_DEV_NUM_SELF_TEST_RESULTS

} _CiDevSelfTestResult;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Test result
 * \sa CI_DEV_SELF_TEST_RESULT */
/** \remarks Common Data Section */
typedef UINT8 CiDevSelfTestResult;
/**@}*/

/** \brief  RFS Level */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CI_DEV_RFS_LEVEL
{
	CI_DEV_RFS_USER_LEVEL,
	CI_DEV_RFS_DEEP_LEVEL,

	CI_DEV_NUM_RFS_LEVEL

} _CiDevRfsLevel;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief RFS Level
 * \sa CI_DEV_SELF_TEST_RESULT */
/** \remarks Common Data Section */
typedef UINT8 CiDevRfsLevel;
#if !defined (ON_PC)
#define CI_DEV_MAX_APGS_MSG_SIZE    		500
#else
#define CI_DEV_MAX_APGS_MSG_SIZE    		375
#endif
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

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_LP_NWUL_MSG_REQ">   */
typedef struct CiDevPrimLpNwulMsgReq_struct {
	UINT8 msg_data[CI_DEV_MAX_APGS_MSG_SIZE];                                                                                       /**< The OTA message */
	UINT32 msg_data_len;                                                                                                            /**< Specifies the number of bytes contained in msg_data */
	UINT8 count;                                                                                                                    /**< Ordinal number of the message. [range: 0-3] */
	CiDevLpBearerType bearer_type;                                                                                                  /**< Radio bearer type. \sa CiDevLpBearerType */
	CiBoolean isFinalResponse;                                                                                                      /**< isFinalResponse Flag. \sa CCI API Ref Manual */
}CiDevPrimLpNwulMsgReq;

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


/* ********************************************************************************************* */
/* primitives definitions */
/* ********************************************************************************************* */

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_DO_SELF_TEST_REQ">   */
typedef struct CiDevPrimDoSelfTestReq_struct {

	UINT32 appVersion;      /**< Not in use. */

} CiDevPrimDoSelfTestReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_DO_SELF_TEST_CNF">   */
typedef struct CiDevPrimDoSelfTestCnf_struct {

	CiDevRc result;                         /**< Result code.  \sa CiDevRc */
	CiDevSelfTestResult testResult;         /**< Test result.  \sa CiDevSelfTestResult */

} CiDevPrimDoSelfTestCnf;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_DO_SELF_TEST_IND">   */
typedef struct CiCustPrimPerformSelfTestInd_struct {

	CiDevSelfTestResult testResult;         /**< Test result. \sa CiDevSelfTestResult */

} CiDevPrimDoSelfTestInd;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_SET_RFS_REQ">   */
typedef struct CiDevPrimSetRfsReq_struct {

	CiDevRfsLevel level;            /**< Not in use. */

} CiDevPrimSetRfsReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_SET_RFS_CNF">   */
typedef struct CiDevPrimSetRfsCnf_struct {

	CiDevRc result;                         /**< Result code. \sa CiDevRc. */

} CiDevPrimSetRfsCnf;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_GET_RFS_REQ">   */
typedef struct CiDevPrimGetRfsReq_struct {

	CiDevRfsLevel level;            /**< Not in use. */

} CiDevPrimGetRfsReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_GET_RFS_CNF">   */
typedef struct CiDevPrimGetRfsCnf_struct {

	CiDevRc result;                         /**< Result code. \sa CiDevRc. */

} CiDevPrimGetRfsCnf;

/*Michal Bukai - Silent Reset support - START*/
typedef CiEmptyPrim CiDevPrimCommAssertReq;
/*Michal Bukai - Silent Reset support - END*/

/*Michal Bukai *BAND support - START*/

//ICAT EXPORTED ENUM
typedef enum CIDEVNWMODES {
	CI_DEV_NW_GSM  = 0,
	CI_DEV_NW_UMTS,
	CI_DEV_NW_DUAL_MODE,

	CI_DEV_NUM_NW_MODES

} _CiDevNetworkMode;

typedef UINT8 CiDevNetworkMode;

//ICAT EXPORTED ENUM
typedef enum CIDEVGSMBANDMODES {
	CI_DEV_PGSM_900 = 0,
	CI_DEV_DCS_GSM_1800,
	CI_DEV_PCS_GSM_1900,
	CI_DEV_EGSM_900,
	CI_DEV_GSM_450,
	CI_DEV_GSM_480,
	CI_DEV_GSM_850,

	CI_DEV_NUM_GSM_BAND
} _CiDevGSMBandMode;

typedef UINT8 CiDevGSMBandMode;

//ICAT EXPORTED ENUM
typedef enum CIDEVUMTSBANDMODES {
	CI_DEV_UMTS_BAND_1  = 0,
	CI_DEV_UMTS_BAND_2,
	CI_DEV_UMTS_BAND_3,
	CI_DEV_UMTS_BAND_4,
	CI_DEV_UMTS_BAND_5,
	CI_DEV_UMTS_BAND_6,
	CI_DEV_UMTS_BAND_7,
	CI_DEV_UMTS_BAND_8,
	CI_DEV_UMTS_BAND_9,

	CI_DEV_NUM_UMTS_BAND

} _CiDevUMTSBandMode;

typedef UINT8 CiDevUMTSBandMode;

//ICAT EXPORTED STRUCT
typedef struct CiDevPrimSetBandModeReq_struct {
	CiDevNetworkMode networkMode;
	CiDevNetworkMode preferredMode;
	CiBitRange GSMBandMode;
	CiBitRange UMTSBandMode;

	UINT8  roamingConfig; /**< Roaming: 0-not support, 1-support, 2-no change(default) */
	UINT8  srvDomain; /**< Service domain: 0-CS only, 1- PS only, 2-CS and PS, 3 - ANY, 4 -no change(default) */
} CiDevPrimSetBandModeReq;

//ICAT EXPORTED STRUCT
typedef struct CiDevPrimSetBandModeCnf_struct {
	CiDevRc result;
} CiDevPrimSetBandModeCnf;

typedef CiEmptyPrim CiDevPrimGetBandModeReq;

//ICAT EXPORTED STRUCT
typedef struct CiDevPrimGetBandModeCnf_struct {
	CiDevRc result;
	CiDevNetworkMode networkMode;
	CiDevNetworkMode	preferredMode;
	CiBitRange GSMBandMode;
	CiBitRange UMTSBandMode;

	UINT8  roamingConfig; /**< Roaming: 0-not support, 1-support */
	UINT8  srvDomain;  /**< Service domain: 0-CS only, 1- PS only, 2-CS and PS, 3 - ANY*/
} CiDevPrimGetBandModeCnf;

typedef CiEmptyPrim CiDevPrimGetSupportedBandModeReq;

//ICAT EXPORTED STRUCT
typedef struct CiDevPrimGetSupportedBandModeCnf_struct {
	CiDevRc result;
	CiBitRange bitsNetworkMode;
	CiBitRange bitsGSMBandMode;
	CiBitRange bitsUMTSBandMode;
} CiDevPrimGetSupportedBandModeCnf;

/*Michal Bukai *BAND support - END*/

/*Michal Bukai - IMEI support - START*/

//** \brief IMEI SV structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiImei_struct {
	UINT8 len;                      /**< Length */
	CHAR val[CI_MAX_IMEI_SV_LEN];   /**< IMEI digits */
}CiImei;

/** <paramref name="CI_DEV_PRIM_SET_SV_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiDevPrimSetSvReq_struct {
	CiImei SVDigits;                /**< IMEI SV digits.  \sa CiImei */
} CiDevPrimSetSvReq;

/** <paramref name="CI_DEV_PRIM_SET_SV_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiDevPrimSetSvCnf_struct {
	CiDevRc rc; /**< Result code. \sa CiDevRc. */
} CiDevPrimSetSvCnf;

/** <paramref name="CI_DEV_PRIM_GET_SV_REQ">   */
typedef CiEmptyPrim CiDevPrimGetSvReq;


/** <paramref name="CI_DEV_PRIM_GET_SV_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiDevPrimGetSvCnf_struct {
	CiImei SVDigits;                /**< IMEI SV dugits.  \sa CiImei */
	CiDevRc rc;                     /**< Result code. \sa CiDevRc. */
} CiDevPrimGetSvCnf;
/*Michal Bukai - IMEI support - END*/

/** <paramref name="CI_DEV_PRIM_AP_POWER_NOTIFY_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiDevPrimApPowerNotifyReq_struct {
	UINT32	powerState;                /**< AP power state: 1 means suspend; 0 means resume.  \sa powerState */
} CiDevPrimApPowerNotifyReq;

/** <paramref name="CI_DEV_PRIM_AP_POWER_NOTIFY_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiDevPrimApPowerNotifyCnf_struct {
	CiDevRc rc;     /**< Result code. \sa CiDevRc */
} CiDevPrimApPowerNotifyCnf;

//ICAT EXPORTED ENUM
typedef enum CIDEV_TD_RX_TX_OPTION_TAG{
    CI_DEV_TD_TX_START = 0,
    CI_DEV_TD_RX_START,
    CI_DEV_TD_TX_RX_STOP,

    CI_DEV_NUM_TD_TX_RX
} _CiDevTdTxRxOption;

typedef UINT8 CiDevTdTxRxOption;

//ICAT EXPORTED ENUM
typedef enum CIDEV_GSM_RX_TX_OPTION_TAG{
    CI_DEV_GSM_TX_START = 0,
    CI_DEV_GSM_RX_START,
    CI_DEV_GSM_TX_RX_START,
    CI_DEV_GSM_TX_RX_STOP,

    CI_DEV_NUM_GSM_TX_RX
} _CiDevGsmTxRxOption;

typedef UINT8 CiDevGsmTxRxOption;

//ICAT EXPORTED ENUM
typedef enum CIDEV_GSM_CONTROL_MODE_TAG{
    CI_DEV_GSM_CONTROL_READ = 0,
    CI_DEV_GSM_CONTROL_WRITE,
    CI_DEV_GSM_CONTROL_LOOPBACK,

    CI_DEV_NUM_GSM_CONTROL
} _CiDevGsmControlMode;

typedef UINT8 CiDevGsmControlMode;

/** <paramref name="CI_DEV_PRIM_SET_TD_MODE_TX_RX_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiDevPrimSetTdModeTxRxReq_struct {
	CiDevTdTxRxOption option;     /**< TD option. \sa CiDevTdTxRxOption */
	INT16 txRxGain;       /**<  Tx or Rx gain */
	UINT16 freq;           /**<  TD frequency */
} CiDevPrimSetTdModeTxRxReq;

/** <paramref name="CI_DEV_PRIM_SET_TD_MODE_TX_RX_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiDevPrimSetTdModeTxRxCnf_struct {
	CiDevRc rc;     /**< Result code. \sa CiDevRc */
} CiDevPrimSetTdModeTxRxCnf;

/** <paramref name="CI_DEV_PRIM_SET_TD_MODE_LOOPBACK_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiDevPrimSetTdModeLoopbackReq_struct {
	UINT32 regValue;       /**<  The value to be written into RF register */
} CiDevPrimSetTdModeLoopbackReq;

/** <paramref name="CI_DEV_PRIM_SET_TD_MODE_LOOPBACK_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiDevPrimSetTdModeLoopbackCnf_struct {
	CiDevRc rc;     /**< Result code. \sa CiDevRc */
	UINT32 regValue;       /**<  The value to be read from RF register */
} CiDevPrimSetTdModeLoopbackCnf;

/** <paramref name="CI_DEV_PRIM_SET_GSM_MODE_TX_RX_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiDevPrimSetGsmModeTxRxReq_struct {
	CiDevGsmTxRxOption option;     /**< TD option. \sa CiDevGsmTxRxOption */
	CiBitRange gsmBandMode;       /**<  GSM band mode */
	UINT16 arfcn;            /**<  Absolute Radio Frequency Channel Number */
	UINT32 afcDac;           /**<  AFC DAC value*/
	UINT32 txRampScale;      /**<  Tx ramp scale, only valid for Tx or Tx+Rx mode*/
	UINT32 rxGainCode;       /**<  Rx gain code, only valid for Rx or Tx+Rx mode*/
} CiDevPrimSetGsmModeTxRxReq;

/** <paramref name="CI_DEV_PRIM_SET_GSM_MODE_TX_RX_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiDevPrimSetGsmModeTxRxCnf_struct {
	CiDevRc rc;     /**< Result code. \sa CiDevRc */
	signed short rssiDbmValue;          /**<  The Rssi value to be returned in case of GSM RX mode*/
} CiDevPrimSetGsmModeTxRxCnf;

/** <paramref name="CI_DEV_PRIM_SET_GSM_CONTROL_INTERFACE_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiDevPrimSetGsmControlInterfaceReq_struct {
	CiDevGsmControlMode mode;     /**< Operation mode. \sa CiDevGsmControlMode */
	UINT16 addrReg;           /**<  Register address*/
	UINT16 regValue;          /**<  The payload value to be written into RFIC in case of write or loopback mode*/
} CiDevPrimSetGsmControlInterfaceReq;

/** <paramref name="CI_DEV_PRIM_SET_GSM_CONTROL_INTERFACE_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiDevPrimSetGsmControlInterfaceCnf_struct {
	CiDevRc rc;     /**< Result code. \sa CiDevRc */
	UINT16 addrReg;           /**<  Register address*/
	UINT16 regValue;          /**<  The payload value to be read from RFIC in case of read or loopback mode*/
} CiDevPrimSetGsmControlInterfaceCnf;
/** <paramref name="CI_DEV_PRIM_ENABLE_HSDPA_REQ">   */
//ICAT EXPORTED STRUCT
typedef enum CIDEV_HSPA_CONFIG_TAG{
    CI_DEV_HSDPA_OFF_HSUPA_OFF = 0,
    CI_DEV_HSDPA_ON_HSUPA_OFF,
    CI_DEV_HSDPA_ON_HSUPA_ON,

    CI_DEV_NUM_HSPA_CONFIG
} _CiDevHspaConfig;

typedef UINT8 CiDevHspaConfig;

typedef struct CiDevPrimEnableHsdpaReq_struct{
    CiDevHspaConfig   hspaConfig;      /**< HSPA configurations \sa CiDevHspaConfig */
    UINT8             dlCategory;      /**<  DL category, support 1~14 except 12 on TD, default 14*/
    UINT8             ulCategory;      /**<  UL category, only support 6 on TD, fix it if UPA is enabled */
    UINT8             cpcState;        /**<  CPC state, 0:disabled,1:enabled, not supported on TD, hard coded with 0 or ingore it*/
} CiDevPrimEnableHsdpaReq;

/** <paramref name="CI_DEV_PRIM_ENABLE_HSDPA_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiDevPrimEnableHsdpaCnf_struct {
	CiDevRc rc;     /**< Result code. \sa CiDevRc */
} CiDevPrimEnableHsdpaCnf;

/** <paramref name="CI_DEV_PRIM_GET_HSDPA_STATUS_REQ">   */
typedef CiEmptyPrim CiDevPrimGetHsdpaStatusReq;

/** <paramref name="CI_DEV_PRIM_GET_HSDPA_STATUS_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiDevPrimGetHsdpaStatusCnf_struct {
    CiDevRc rc;     /**< Result code. \sa CiDevRc */
    CiDevHspaConfig   hspaConfig;      /**< HSPA configurations \sa CiDevHspaConfig */
    UINT8             dlCategory;      /**<  DL category, support 1~14 except 12 on TD */
    UINT8             ulCategory;      /**<  UL category, only support 6 on TD, fix it if UPA is enabled */
    UINT8             cpcState;        /**<  CPC state, 0:disabled,1:enabled, not supported on TD, hard coded with 0 */
} CiDevPrimGetHsdpaStatusCnf;

//ICAT EXPORTED ENUM
typedef enum CIDEV_RF_TEMP_TYPE_TAG{
    CI_DEV_RF_TEMP_CELSIUS = 0,
    CI_DEV_RF_TEMP_RAW_DATA,

    CI_DEV_NUM_RF_TEMP
} _CiDevRfTempType;

typedef UINT8 CiDevRfTempType;

/** <paramref name="CI_DEV_PRIM_READ_RF_TEMPERATURE_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiDevPrimReadRfTemperatureReq_struct{
    CiDevRfTempType type;     /**< Read type. \sa CiDevRfTempType  */
} CiDevPrimReadRfTemperatureReq;

/** <paramref name="CI_DEV_PRIM_READ_RF_TEMPERATURE_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiDevPrimReadRfTemperatureCnf_struct {
    CiDevRc rc;     /**< Result code. \sa CiDevRc */
    INT32   tempData;  /**< Temperature data */
} CiDevPrimReadRfTemperatureCnf;

#ifdef AT_CUSTOMER_HTC
/*+TPCN support - START*/
//ICAT EXPORTED ENUM
typedef enum CIDEV_MODE_SWITCH_OPTION_TAG{
    CI_DEV_MODE_SWITCH_DEFAULT = 0,
    CI_DEV_MODE_SWITCH_FIELD_TRIAL,
    CI_DEV_MODE_SWITCH_NEL,
    CI_DEV_MODE_SWITCH_AT_ROUTER,
    CI_DEV_MODE_SWITCH_TEST_MODE,

    CI_DEV_NUM_MODE_SWITCH
} _CiDevModeSwitchOption;

typedef UINT8 CiDevModeSwitchOption;

/** <paramref name="CI_DEV_PRIM_SET_MODE_SWITCH_OPTION_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiDevPrimSetModeSwitchOptReq_struct{
    CiDevModeSwitchOption option;  /**<  \sa CiDevModeSwitchOption */
} CiDevPrimSetModeSwitchOptReq;

/** <paramref name="CI_DEV_PRIM_SET_MODE_SWITCH_OPTION_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiDevPrimSetModeSwitchOptCnf_struct {
    CiDevRc rc;     /**< Result code. \sa CiDevRc */
} CiDevPrimSetModeSwitchOptCnf;

/** <paramref name="CI_DEV_PRIM_GET_MODE_SWITCH_OPTION_REQ">   */
typedef CiEmptyPrim CiDevPrimGetModeSwitchOptReq;

/** <paramref name="CI_DEV_PRIM_GET_MODE_SWITCH_OPTION_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiDevPrimGetModeSwitchOptCnf_struct {
    CiDevRc rc;     /**< Result code. \sa CiDevRc */
    CiDevModeSwitchOption option;  /**<  \sa CiDevModeSwitchOption */
} CiDevPrimGetModeSwitchOptCnf;
/*+TPCN support - END*/

/** <paramref name="CI_DEV_PRIM_RADIO_LINK_FAILURE_IND"> */
typedef CiEmptyPrim CiDevPrimRadioLinkFailureInd;
#endif

/*Mason CMCC Smart Network Monitor support - START*/
#define CI_DEV_MAX_TDD_TIMESLOT_ISCP    7
#define CI_DEV_MAX_TDD_DPCH_RSCP        7
#define CI_DEV_MAX_TDD_DEDICATED_PHYCH  96

#define CI_DEV_MAX_GSM_DL_NUM_BLK       4
#define CI_DEV_MAX_GSM_UL_DL_CS_MCS     50
#define CI_DEV_MAX_GSM_NUM_TIMESLOT     4
#define CI_DEV_MAX_GSM_NUM_REPORTED_ARFCN     64
#define CI_DEV_MAX_GSM_NUM_CELL_ALLOCATION    64

#define CI_DEV_MAX_WIRELESS_DATA_LENGTH      1908
#define CI_DEV_MAX_PEER_MSG_LENGTH      356

//ICAT EXPORTED ENUM
typedef enum CIDEV_NW_MONITOR_MODE_TAG{
    CI_DEV_NW_MONIOTR_NORMAL = 16,
    CI_DEV_NW_MONIOTR_DETECT = 96,

    CI_DEV_NUM_RNW_MONIOTR
} _CiDevNwMonitorMode;

typedef UINT8 CiDevNwMonitorMode;

//ICAT EXPORTED ENUM
typedef enum CIDEV_PROTOCOL_STATUS_TAG{
    CI_DEV_PROTOCOL_STATUS_IDLE = 0,
    CI_DEV_PROTOCOL_STATUS_CONNECT,

    CI_DEV_NUM_PROTOCOL_STATUS
} _CiDevProtocolStatus;

typedef UINT8 CiDevProtocolStatus;

//ICAT EXPORTED ENUM
typedef enum CIDEV_EVENT_OPER_TYPE_TAG{
    CI_DEV_EVENT_OPER_CS_VOICE = 0,
    CI_DEV_EVENT_OPER_CS_DATA,
    CI_DEV_EVENT_OPER_PS,
    CI_DEV_EVENT_OPER_SMS,

    CI_DEV_NUM_EVENT_OPER
} _CiDevEventOperType;

typedef UINT8 CiDevEventOperType;

typedef UINT32 CiDevEventId;

//ICAT EXPORTED ENUM
typedef enum CIDEV_SIGNALING_MSG_ID_TAG{
    CI_DEV_SIGNALING_BCH_MSG_IND = 12202,  /* only for TDD */
    CI_DEV_SIGNALING_BCCH_FACH_MSG_IND = 2020,  /* only for TDD */
    CI_DEV_SIGNALING_PCCH_PCH_MSG_IND = 2021,  /* only for TDD */
    CI_DEV_SIGNALING_UL_CCH_MSG_IND = 3005,  /* only for TDD */
    CI_DEV_SIGNALING_DL_CCH_MSG_IND = 3006,  /* only for TDD */

    CI_DEV_SIGNALING_GSM_MSG_IND = 12155,  /* only for GSM */

    CI_DEV_NUM_SIGNALING_MSG_D = 0xFFFF
} _CiDevSignalingMsgId;

typedef UINT32 CiDevSignalingMsgId;

//ICAT EXPORTED ENUM
typedef enum CIDEV_GSM_BAND_TAG{
    CI_DEV_GSM_BAND_PGSM_900 = 0,
    CI_DEV_GSM_BAND_EGSM_900,
    CI_DEV_GSM_BAND_RGSM_900,
    CI_DEV_GSM_BAND_DCS_1800,
    CI_DEV_GSM_BAND_PCS_1900,
    CI_DEV_GSM_BAND_450,
    CI_DEV_GSM_BAND_480,
    CI_DEV_GSM_BAND_850,
    CI_DEV_GSM_BAND_750,

    CI_DEV_NUM_BAND
} _CiDevGsmBand;

typedef UINT8 CiDevGsmBand;

//ICAT EXPORTED STRUCT
typedef struct CiDevPlmnMcc_struct
{
    UINT8   mcc[3];
} CiDevPlmnMcc;

//ICAT EXPORTED STRUCT
typedef struct CiDevPlmnMnc_struct
{
    UINT8   len;
    UINT8   mnc[3];
} CiDevPlmnMnc;

//ICAT EXPORTED STRUCT
typedef struct CiDevTsIscp_struct
{
    UINT8 timeSlot;    /**< TimeSlot (0..6) */
    UINT8 iscp;        /**< ISCP (0..91), real value(dbm)=IE value -116 */
} CiDevTsIscp;

//ICAT EXPORTED STRUCT
typedef struct CiDevTsIscpArray_struct
{
    UINT8 num;
    CiDevTsIscp  data[CI_DEV_MAX_TDD_TIMESLOT_ISCP];
} CiDevTsIscpArray;

//ICAT EXPORTED STRUCT
typedef struct CiDevDpchRscp_struct
{
    UINT8 timeSlot;    /**< TimeSlot (0..6) */
    UINT8 rscp;        /**< DPCH RSCP (0..91), real value(dbm)=IE value -116 */
} CiDevDpchRscp;

//ICAT EXPORTED STRUCT
typedef struct CiDevDpchRscpArray_struct
{
    UINT8 num;
    CiDevDpchRscp  data[CI_DEV_MAX_TDD_DPCH_RSCP];
} CiDevDpchRscpArray;

//ICAT EXPORTED STRUCT
typedef struct CiDevTxPower_struct
{
    UINT8 timeSlot;    /**< TimeSlot (0..6) */
    UINT8 txPower;     /**< UE Transmitted Power (21..104), 0xff means UE TxPower is not available.
                                              real value(dbm)=IE value -71 */
} CiDevTxPower;

//ICAT EXPORTED STRUCT
typedef struct CiDevTxPowerArray_struct
{
    UINT8 num;
    CiDevTxPower  data[CI_DEV_MAX_TDD_DPCH_RSCP];
} CiDevTxPowerArray;

//ICAT EXPORTED STRUCT
typedef struct CiDevTddNcellTsIscp_struct
{
    UINT8    tsNum;    /**< Timeslot number. (0..6)*/
    UINT8    tsIscp;   /**< Timeslot ISCP. (0..91) */
} CiDevTddNcellTsIscp;

//ICAT EXPORTED STRUCT
typedef struct CiDevTddRrcDiagNcellTddPara_struct
{
    UINT8          cellParamId;  /**< Neighbor cell parameter ID. (0..127)*/
    UINT16         uArfcn;       /**< Neighbor cell UARFCN. (0..16383) */
    UINT8          pccpchRscp;   /**< Neighbor cell PCCPCH RSCP. (0..91) */

    UINT8          res1U8;       /**< (padding) */
    UINT8          iscpNum;      /**(0..6) If it equals to 0,nCellTsIscp does not xist. */
    CiDevTddNcellTsIscp  nCellTsIscp[CI_DEV_MAX_TDD_TIMESLOT_ISCP-1];

    UINT32         res2U32;      /**< (padding) */
} CiDevTddRrcDiagNcellTddPara;

//ICAT EXPORTED STRUCT
typedef struct CiDevTddRrcDiagNcellGsmPara_struct
{
    UINT16          arfcn; /**< Neighbor cell ARFCN info. (0..1023)*/
    CiDevGsmBand    band;  /**< Neighbor cell current freq band. */
    UINT8           bsic;  /**< Neighbor cell BSIC. (0..63)*/
    UINT8           rxlev; /**< Neighbor cell GSM received signal level. */
    UINT8   res1U16[3];    /**< (padding) */
} CiDevTddRrcDiagNcellGsmPara;

//ICAT EXPORTED STRUCT
typedef struct CiDevTddNcellParamList_struct
{
    UINT8 numNcellTdd;     /**<  If it equals 0, the IE below will not exist */
    CiDevTddRrcDiagNcellTddPara  nbCellInfoTdd[CI_DEV_MAX_GSM_NEIGHBORING_CELLS];

    UINT8 res1U8;          /**< (padding), always fill with 0xFE */
    UINT8 numNcellGsm;     /**<  If it equals 0, the IE below will not exist */
    CiDevTddRrcDiagNcellGsmPara  nbCellInfoGsm[CI_DEV_MAX_GSM_NEIGHBORING_CELLS];
} CiDevTddNcellParamList;

//ICAT EXPORTED ENUM
typedef enum CIDEV_TDD_CHANNEL_CODE_TAG{
    CI_DEV_TDD_CC1_1 = 0,
    CI_DEV_TDD_CC2_1,
    CI_DEV_TDD_CC2_2,
    CI_DEV_TDD_CC4_1,
    CI_DEV_TDD_CC4_2,
    CI_DEV_TDD_CC4_3,
    CI_DEV_TDD_CC4_4,
    CI_DEV_TDD_CC8_1,
    CI_DEV_TDD_CC8_2,
    CI_DEV_TDD_CC8_3,
    CI_DEV_TDD_CC8_4,
    CI_DEV_TDD_CC8_5,
    CI_DEV_TDD_CC8_6,
    CI_DEV_TDD_CC8_7,
    CI_DEV_TDD_CC8_8,
    CI_DEV_TDD_CC16_1,
    CI_DEV_TDD_CC16_2,
    CI_DEV_TDD_CC16_3,
    CI_DEV_TDD_CC16_4,
    CI_DEV_TDD_CC16_5,
    CI_DEV_TDD_CC16_6,
    CI_DEV_TDD_CC16_7,
    CI_DEV_TDD_CC16_8,
    CI_DEV_TDD_CC16_9,
    CI_DEV_TDD_CC16_10,
    CI_DEV_TDD_CC16_11,
    CI_DEV_TDD_CC16_12,
    CI_DEV_TDD_CC16_13,
    CI_DEV_TDD_CC16_14,
    CI_DEV_TDD_CC16_15,
    CI_DEV_TDD_CC16_16,

    CI_DEV_NUM_TDD_CC
} _CiDevTddChannelCode;

typedef UINT8 CiDevTddChannelCode;

//ICAT EXPORTED ENUM
typedef enum CIDEV_GSM_CODING_SCHEME_TAG {
    CI_DEV_GSM_CS_1 = 7,
    CI_DEV_GSM_CS_2,
    CI_DEV_GSM_CS_3,
    CI_DEV_GSM_CS_4,

    CI_DEV_GSM_MCS_1 = 11,
    CI_DEV_GSM_MCS_2,
    CI_DEV_GSM_MCS_3,
    CI_DEV_GSM_MCS_4,
    CI_DEV_GSM_MCS_5,
    CI_DEV_GSM_MCS_6,
    CI_DEV_GSM_MCS_7,
    CI_DEV_GSM_MCS_8,
    CI_DEV_GSM_MCS_9,

    CI_DEV_GSM_CS_INVALID = 0xFF,

    CI_DEV_NUM_GSM_CS
} _CiDevGsmCodingScheme;

typedef UINT8 CiDevGsmCodingScheme;

//ICAT EXPORTED STRUCT
typedef struct CiDevTddRrcDiagPhych_struct
{
    UINT8   phychId;         /**< PhyCH Id (0..47)*/
    UINT8   phychDirection;  /**< PhyCH Direction, 0:uplink;1:downlink*/
    UINT8   timeSlot;        /**< Timeslot (0..6)*/

    CiDevTddChannelCode   channelCode;     /**< Channel code */
    UINT16  res1U16[2];      /**< (padding) */
} CiDevTddRrcDiagPhych;

//ICAT EXPORTED STRUCT
typedef struct CiDevTddDedicatedPhyCchParam_struct
{
    UINT8   numPhych;       /**< (0..96),0: all the followed item not exist; others(1-96): actual physical channel number; and the followed times exist*/
    CiDevTddRrcDiagPhych  phychInfo[CI_DEV_MAX_TDD_DEDICATED_PHYCH];

    UINT32  res1U32;       /**< (padding) */
    UINT16  workFreq;       /**< Work frequency,(0..16383) UARFCN,real requency(Mhz)=UARFCN/5. It indicates the work frequency */
} CiDevTddDedicatedPhyCchParam;

//ICAT EXPORTED STRUCT
typedef struct CiDevHrnti_struct
{
    CiBoolean   flag;    /**< 0: H-RNTI is not existed, 1:H-RNTI is existed */
    UINT16      hrnti;   /**< (0..65535),exist only when flag=1 */
} CiDevHrnti;

//ICAT EXPORTED STRUCT
typedef struct CiDevErnti_struct
{
    CiBoolean   flag;    /**< 0: E-RNTI is not existed, 1:E-RNTI is existed */
    UINT16      ernti;   /**< (0..65535),exist only when flag=1 */
} CiDevErnti;

//ICAT EXPORTED STRUCT
typedef struct CiDevUrntiCrnti_struct
{
    UINT8       flag;    /**< Bit0:C-RNTI; Bit1:U-RNTI; 0:not existed,1:existed*/
    UINT32      urnti;   /**< (0..4294967295),exist only when flag=1 */
    UINT16      crnti;   /**< (0..65535),exist only when flag=1 */
} CiDevUrntiCrnti;

//ICAT EXPORTED STRUCT
typedef struct CiDevTddData_struct
{
    CiDevTsIscpArray      iscp;    /**< TimeSlot ISCP */
    CiDevDpchRscpArray    rscp;    /**< DPCH RSCP */
    CiDevTxPowerArray     txPower; /**< UE Timeslot Transmitted Power */

    UINT8   sCellPccphRscp;       /**< Scell P-CCPCH RSCP, (0..91) real value(dbm)=IE value -116 */
    UINT32  sCellId;              /**< Scell Cell Identifier, BITSTRING(28) MSB 4 bits are 0 */
    UINT8   sCellParamId;         /**< Scell Cell parameter id, (0..127) */
    UINT16  sCellUarfcn;          /**< Scell UARFCN, (0..16383), real frequency(MHz)=UARFCN/5 */
    UINT8   sCellUtraRssi;        /**< Scell UTRA Carrier RSSI, (0..76) real value(dbm)=IE value - 101 */

    CiDevTddNcellParamList        nbCellInfo;    /**< Neigbour cell parameter list */
    CiDevTddDedicatedPhyCchParam  dedicatedInfo; /**< Dedicated PhyCh parameter */

    CiDevHrnti      hRnti; /**< H-RNTI */
    CiDevErnti      eRnti; /**< E-RNTI */
    CiDevUrntiCrnti rnti;  /**< U-RNTI/C-RNTI */
} CiDevTddData;

//ICAT EXPORTED STRUCT
typedef struct CiDevGsmScellSysInfo_struct
{
    UINT16         arfcn;    /**< Serving cell ARFCN (0..1023) */
    UINT16         ci;       /**< Serving cell cell identity */

    UINT16         res1U16;  /**< (padding) */
    CiDevGsmBand   band;     /**< Serving cell current freq band */

    UINT32         res1U32;  /**< (padding) */
} CiDevGsmScellSysInfo;

//ICAT EXPORTED STRUCT
typedef struct CiDevGsmScellRadioInfo_struct
{
    UINT8         fieldMap;    /**< bit0:C2 exist or not, bit1:C31 exist or not, bit2:C32 exist or not */
    INT16         rxLev;       /**< Serving cell GSM received signal level.(-110..-30) */
    UINT8         bsic;        /**< Serving cell BSIC.(0..63)*/
    INT16         c1;          /**< Serving cell Path loss param C1*/
    INT16         c2;          /**< Serving cell Path loss param C2*/
    INT16         c31;         /**< Serving cell GPRS signal level threshold criterion param C31*/
    INT16         c32;         /**< Serving cell GPRS cell ranking criterion param C32*/
} CiDevGsmScellRadioInfo;

//ICAT EXPORTED STRUCT
typedef struct CiDevGsmNcellSysInfo_struct
{
    UINT16         arfcn;    /**< GSM neigbour cell ARFCN (0..1023) */
    UINT16         res1U16;  /**< (padding) */

    CiDevGsmBand   band;     /**< GSM neigbour cell current freq band */
    UINT8          bsic;     /**< GSM neigbour cell BSIC.(0..63)*/
    UINT16         ci;       /**< GSM Ncell identity */

    CiDevPlmnMcc   mcc;       /**< Mobile country code (3 digitals). \sa  CiDevPlmnMcc */
    CiDevPlmnMnc   mnc;       /**< Mobile network code (2 or 3 digitals). \sa  CiDevPlmnMnc */

    UINT16         lac;       /**< Location area code */
} CiDevGsmNcellSysInfo;

//ICAT EXPORTED STRUCT
typedef struct CiDevGsmNcellRadioInfo_struct
{
    UINT8         fieldMap;    /**< bit0:C2 exist or not, bit1:C31 exist or not, bit2:C32 exist or not */
    INT16         c1;          /**< GSM neigbour cell Path loss param C1*/
    INT16         rxLev;       /**< GSM neigbour cell GSM received signal level.(-110..-30) */

    INT16         c2;          /**< GSM neigbour cell Path loss param C2*/
    INT16         c31;         /**< GSM neigbour cell GPRS signal level threshold criterion param C31*/
    INT16         c32;         /**< GSM neigbour cell GPRS cell ranking criterion param C32*/
} CiDevGsmNcellRadioInfo;

//ICAT EXPORTED STRUCT
typedef struct CiDevGsmNcellInfoGsm_struct
{
    CiDevGsmNcellSysInfo    sysInfo;
    CiDevGsmNcellRadioInfo  radioInfo;
} CiDevGsmNcellInfoGsm;

//ICAT EXPORTED STRUCT
typedef struct CiDevGsmTdNcellInfoTdd_struct
{
    UINT8   cellParamId;         /**< Cell parameter id, (0..127) */
    UINT16  uArfcn;              /**< UARFCN, (0..16383), real frequency(MHz)=UARFCN/5  */
    UINT8   pccpchRSCP;          /**< PCCPCH RSCP, (0..91) real value(dbm)=IE value -116 */
    UINT8   utraRssi;            /**< UTRA carrier RSSI, (0..76) real value(dbm)=IE value - 101  */
} CiDevGsmNcellInfoTdd;

//ICAT EXPORTED STRUCT
typedef struct CiDevGsmNcellInfo_struct
{
    UINT8 numNcellGsm;
    CiDevGsmNcellInfoGsm   nbCellInfoGsm[CI_DEV_MAX_GSM_NEIGHBORING_CELLS];

    UINT8 res1U8[3];       /**< (padding) */
    UINT8 numNcellTdd;     /**<  Note: numNcellGsm+numNcellTdd <= CI_DEV_MAX_GSM_NEIGHBORING_CELLS */
    CiDevGsmNcellInfoTdd   nbCellInfoTdd[CI_DEV_MAX_GSM_NEIGHBORING_CELLS];
} CiDevGsmNcellInfo;

//ICAT EXPORTED ENUM
typedef enum CIDEV_GSM_SPEECH_CODE_TAG{
    CI_DEV_GSM_SPEECH_GSM_HR = 0,
    CI_DEV_GSM_SPEECH_GSM_FR,
    CI_DEV_GSM_SPEECH_GSM_EFR,
    CI_DEV_GSM_SPEECH_HR_AMR,
    CI_DEV_GSM_SPEECH_FR_AMR,

    CI_DEV_NUM_GSM_SPEECH
} _CiDevGsmSpeechCode;

typedef UINT8 CiDevGsmSpeechCode;

//ICAT EXPORTED ENUM
typedef enum CIDEV_GSM_DED_CHANNEL_TYPE_TAG{
    CI_DEV_GSM_DEDICATED_TCH = 0,
    CI_DEV_GSM_DEDICATED_SDCCH,
    CI_DEV_GSM_DEDICATED_PDCH,

    CI_DEV_NUM_GSM_DEDICATED_CHANNEL
} _CiDevGsmDedChannelType;

typedef UINT8 CiDevGsmDedChannelType;

//ICAT EXPORTED STRUCT
typedef struct CiDevGsmCellAlloc_struct
{
    UINT8         numCellAlloc; /**< The number of Cell Allocation.(1~64) */
    UINT16        cellAlloc[CI_DEV_MAX_GSM_NUM_CELL_ALLOCATION]; /**< Cell Allocation list.(0~1024) */
} CiDevGsmCellAlloc;

//ICAT EXPORTED STRUCT
typedef struct CiDevGsmDedArfcn_struct
{
    CiBoolean         hoppingChannel; /**< dedicated channel support arfcn hopping or not */
    UINT16            arfcn;          /**< absolute RF channel number. (0..1023) only exist when hoppingChannel=0 */
    CiDevHoppingGroup hoppingGroup;   /**< Hopping List, exist when hoppingChannel=1 */
    CiDevGsmCellAlloc cellAlloc;      /**< Cell Allocation*/

    UINT8             MAIO;           /**< mobile allocation index offset, (0..63) exist when hoppingChannel=1*/
    UINT8             HSN;            /**< hopping sequence number, (0..63) exist when hoppingChannel=1*/
} CiDevGsmDedArfcn;

//ICAT EXPORTED STRUCT
typedef struct CiDevGsmDedChannelInfo_struct
{
    UINT8                         fieldMap;        /**< Bit0:Ded_arfcn_beforetime;Bit1:Speech code used;Bit2:Ded_arfcn_after_time and tn */
    CiDevGsmDedArfcn              arfcnAfterTime;  /**< Arfcn of dedicated channel after starting timer  */

    UINT8                         tn;              /**< current dedicated maining link timeslot mapping(List of timeslots used.(0..7) */

    CiDevGsmSpeechCode            speechCode;      /**< Speech code used */
    CiDevGsmDedChannelType        channelType;     /**< Channel Type */

    CiDevGsmDedArfcn              arfcnBeforeTime; /**<  Arfcn of dedicated channel before starting timer */
} CiDevGsmDedChannelInfo;

//ICAT EXPORTED STRUCT
typedef struct CiDevGsmDedMeasInfo_struct
{
    UINT8     sCellRxQualFull;    /**< Dedicated channel RXQUAL_FULL,(0..7)*/
    UINT8     sCellRxQualSub;     /**< Dedicated channel RXQUAL_SUB,(0..7)  */
    UINT8     sCellTimingAdv;     /**< TA,(0..63),0xff:invalid value */
    UINT8     sCellTxPower;       /**< power class of current dedicated channel */

    UINT8     sCellRxLevFull;     /**< service cell received signal level full,(0..63) invalid value 0xff,real value(dBm)=IE value-110 */
    UINT8     sCellRxLevSub;      /**< service cell received signal level sub,(0..63) invalid value 0xff,real value(dBm)=IE value-110 */
    UINT8     res1U8;             /**< (padding) */
    UINT8     bler;               /**< Block error rate for all code schemes. The actual ratio should be devided by 100.0xff:invalid value*/
} CiDevGsmDedMeasInfo;

//ICAT EXPORTED STRUCT
typedef struct CiDevGsmTimeSlotCi_struct
{
    UINT8 tsNo;       /**< Timeslot */
    INT16 rxLev;
    INT16 ci;         /**< Bit0~bit7(LSB):decimal part,unsigned data; Bit8~bit15(LSB): integer part, signed data. */
} CiDevGsmTimeSlotCi;

//ICAT EXPORTED STRUCT
typedef struct CiDevGsmArfcnCiRep_struct
{
    UINT16       arfcn;      /**< BCCH Frequency,(0..1023) */
    CiDevGsmBand band;       /**< Frequency band */

    UINT8  numSlot;          /**< The number of slots which this arfcn has appeared at one time. (0..4) */
    CiDevGsmTimeSlotCi slotCiArray[CI_DEV_MAX_GSM_NUM_TIMESLOT];
} CiDevGsmArfcnCiRep;

//ICAT EXPORTED STRUCT
typedef struct CiDevGsmCIInfo_struct
{
    UINT8       repArfcnNum;      /**< The number of the arfcn reported.(0..64) */
    CiDevGsmArfcnCiRep data[CI_DEV_MAX_GSM_NUM_REPORTED_ARFCN];
} CiDevGsmCIInfo;

//ICAT EXPORTED STRUCT
typedef struct CiDevGsmBepCvBepInfo_struct
{
    CiBoolean  isGmskValid;  /**< 0:GMSK_MEAN_BEP and GMSK_CV_BEP is absent; 1:GMSK_MEAN_BEP and GMSK_CV_BEP is present*/
    UINT8      gmskMeanBepCvBep;  /**< GMSK_MEAN_BEP: High 5 bits of 1 octet; GMSK_CV_BEP: Low 3bits of above octet*/

    CiBoolean  is8pskValid;  /**< 0:8PSK_MEAN_BEP and 8PSK_CV_BEP is absent; 1:8PSK_MEAN_BEP and 8PSK_CV_BEP is present*/
    UINT8      psk8MeanBepCvBep;  /**< 8PSK_MEAN_BEP: High 5 bits of 1 octet; 8PSK_CV_BEP: Low 3bits of above octet*/
} CiDevGsmBepCvBepInfo;

//ICAT EXPORTED STRUCT
typedef struct CiDevGsmUlCsMcs_struct
{
    UINT32 frameNo;       /**< Frame number of this UL block */
    CiDevGsmCodingScheme  mcsCs;         /**< 7~10:CS1~CS4; 11~19:MCS1~MCS9 */
} CiDevGsmUlCsMcs;

//ICAT EXPORTED STRUCT
typedef struct CiDevGsmMcsCs_struct
{
    UINT8 tsNo;       /**< Timeslot of this DL block */
    CiDevGsmCodingScheme  mcsCs;         /**< 7~10:CS1~CS4; 11~19:MCS1~MCS9 */
} CiDevGsmMcsCs;

//ICAT EXPORTED STRUCT
typedef struct CiDevGsmDlCsMcs_struct
{
    UINT32 frameNo;       /**< Frame number of this DL block */

    UINT8  blkNum;        /**< The number of DL block received */
    CiDevGsmMcsCs  mcsCs[CI_DEV_MAX_GSM_DL_NUM_BLK];
} CiDevGsmDlCsMcs;

//ICAT EXPORTED STRUCT
typedef struct CiDevGsmMcsCsInfo_struct
{
    UINT8 ulLength;       /**< The number of UL_CS_MCS in peroid 1s*/
    CiDevGsmUlCsMcs    ulCsMcs[CI_DEV_MAX_GSM_UL_DL_CS_MCS];

    UINT8 dlLength;       /**< The number of DL_CS_MCS in peroid 1s*/
    CiDevGsmDlCsMcs    dlCsMcs[CI_DEV_MAX_GSM_UL_DL_CS_MCS];
} CiDevGsmMcsCsInfo;

//ICAT EXPORTED STRUCT
typedef struct CiDevGsmTfiTimeslot_struct
{
    CiBoolean isUlTfiValid;    /**< 0:UL_TFI and UL timeslot allocation is absent; 1: UL_TFI and UL timeslot allocation is present*/
    UINT8     ulTfi;           /**< UL_TFI  */
    UINT8     ulTimeSlotAlloc; /**< UL timeslot allocation */

    CiBoolean isDlTfiValid;    /**< 0:DL_TFI and DL timeslot allocation is absent; 1: DL_TFI and DL timeslot allocation is present*/
    UINT8     dlTfi;           /**< DL_TFI*/
    UINT8     dlTimeSlotAlloc; /**< DL timeslot allocation */
} CiDevGsmTfiTimeslot;

//ICAT EXPORTED STRUCT
typedef struct CiDevGsmData_struct
{
    CiDevGsmScellSysInfo      sCellSysInfo;    /**< Serving cell system information. */
    UINT8 sCellSysInfoDtxInd;        /**< Serving cell reselection system information.
                                                                 (0..2), DTX indicator, 0:the MS may use uplink discontinous transmission,
                                                                                                1:the MS shall use uplink discontinous transmission,
                                                                                                2:the MS shall not use uplink discontinous transmission*/

    CiDevGsmScellRadioInfo    sCellRadioInfo;   /**< Serving cell radio information. */
    CiDevGsmNcellInfo         nCellInfo;        /**< Neighbour cell information. */

    CiDevGsmDedChannelInfo    dedChannelInfo;   /**< Dedicated channel information. */
    CiDevGsmDedMeasInfo       dedMeasInfo;      /**< Dedicated measurement information. */

    CiDevGsmCIInfo            ciInfo;           /**< C/I. */
    CiDevGsmBepCvBepInfo      bepCvBepinfo;     /**< BEP/CV BEP. */
    CiDevGsmMcsCsInfo         mscCsInfo;        /**< MCS/CS (up/down). */
    CiDevGsmTfiTimeslot       tfiTimeSlotInfo;  /**< TFI(up/down) and Timeslot Allocation(up/down). */
} CiDevGsmData;

//ICAT EXPORTED STRUCT
typedef struct CiDevMobileId_struct
{
    UINT8   digitSize;
    UINT8   digit[CI_MAX_IMEI_SV_LEN];
} CiDevMobileId;

//ICAT EXPORTED STRUCT
typedef struct CiDevUeId_struct
{
    CiDevMobileId  imei; /**< IMEI for 14~16 digit. \sa  CiDevMobileId */
    CiDevMobileId  imsi; /**< IMSI for 15 digit. \sa  CiDevMobileId */
    CiDevMobileId  tmsi; /**< TMSI for 4 Hex. \sa  CiDevMobileId */
    CiDevMobileId  ptmsi; /**< PTMSI for 4 Hex. \sa  CiDevMobileId */
} CiDevUeId;

//ICAT EXPORTED STRUCT
typedef struct CiDevMmInfo_struct
{
    CiDevPlmnMcc  mcc;  /**< Mobile country code (3 digitals). \sa  CiDevPlmnMcc */
    CiDevPlmnMnc  mnc;  /**< Mobile network code (2 or 3 digitals). \sa  CiDevPlmnMnc */

    UINT16  lac;    /**< Location area code */
    UINT16  res1U16;  /**< (padding) */
} CiDevMmInfo;

//ICAT EXPORTED STRUCT
typedef struct CiDevCommonData_struct
{
    CiDevUeId      ueId;      /**< UE ID */
    CiDevMmInfo    mmInfo;    /**< MM inforamtion */
} CiDevCommonData;

//ICAT EXPORTED STRUCT
typedef struct CiDevWirelessParam_struct
{
    CiDevTddData     tddInfo;    /**< TDSCDMA parameters  */
    CiDevCommonData  commonInfo; /**< Common parameters  */
    CiDevGsmData     gsmInfo;    /**< GSM parameters  */
} CiDevWirelessParam;

//ICAT EXPORTED ENUM
typedef enum CIDEV_SYSTEM_INFO_TYPE_TAG {
    CI_DEV_SYS_INFO_TYPE_MIB = 0,
    CI_DEV_SYS_INFO_TYPE_SB1,
    CI_DEV_SYS_INFO_TYPE_SB2,
    CI_DEV_SYS_INFO_TYPE1,
    CI_DEV_SYS_INFO_TYPE2,
    CI_DEV_SYS_INFO_TYPE3,
    CI_DEV_SYS_INFO_TYPE4,
    CI_DEV_SYS_INFO_TYPE5,
    CI_DEV_SYS_INFO_TYPE6,
    CI_DEV_SYS_INFO_TYPE7,
    CI_DEV_SYS_INFO_TYPE8,
    CI_DEV_SYS_INFO_TYPE9,
    CI_DEV_SYS_INFO_TYPE10,
    CI_DEV_SYS_INFO_TYPE11,
    CI_DEV_SYS_INFO_TYPE12,
    CI_DEV_SYS_INFO_TYPE13,
    CI_DEV_SYS_INFO_TYPE13_1,
    CI_DEV_SYS_INFO_TYPE13_2,
    CI_DEV_SYS_INFO_TYPE13_3,
    CI_DEV_SYS_INFO_TYPE13_4,
    CI_DEV_SYS_INFO_TYPE14,
    CI_DEV_SYS_INFO_TYPE15,
    CI_DEV_SYS_INFO_TYPE15_1,
    CI_DEV_SYS_INFO_TYPE15_2,
    CI_DEV_SYS_INFO_TYPE15_3,
    CI_DEV_SYS_INFO_TYPE15_4,
    CI_DEV_SYS_INFO_TYPE15_5,
    CI_DEV_SYS_INFO_TYPE16,
    CI_DEV_SYS_INFO_TYPE17,
    CI_DEV_SYS_INFO_TYPE18,
    CI_DEV_SYS_INFO_TYPE5bis,

    CI_DEV_NUM_SYS_FINO
} _CiDevSystemInfoType;

typedef UINT8 CiDevSystemInfoType;

//ICAT EXPORTED STRUCT
typedef struct CiDevBcchFachParam_struct
{
    UINT8     rbId;  /**< Rb identity. Value 34, It is used for message from FACH to BCCH*/
    UINT16    noTb;  /**< Indicates the number of transport blocks transmitted by the peer entity within the TTI, beased on the TFI value*/
} CiDevBcchFachParam;

//ICAT EXPORTED STRUCT
typedef struct CiDevPcchPchParam_struct
{
    UINT8     rbId;   /**< Rb identity. Value 33, It is used for message from PCH to PCCH*/
    UINT16    noTb;   /**< Indicates the number of transport blocks transmitted by the peer entity within the TTI, beased on the TFI value*/
} CiDevPcchPchParam;

//ICAT EXPORTED STRUCT
typedef struct CiDevAmDataInd_struct
{
    UINT8     fieldInd;  /**< Indicate the presence or absence of peer_msg and disc_info. 01:peer_msg exists; 02:disc_info exists*/
    UINT16    discInfo;  /**< Indicates to ULR the discarded RLC SDU in the peer-RLC AM entity. 0~65535*/
} CiDevAmDataInd;

//ICAT EXPORTED STRUCT
typedef struct CiDevTmDataInd_struct
{
    CiBoolean errIndFlag;  /**< Indicate the presence or absence of error_ind. 1means exists. When"err_SDU_delv" is configured as YES and
                                                      there are SDUs received in error, the err_ind parameter is present*/
    UINT8     errInd;      /**< Indicates that the RLC SDU is erroneous*/
} CiDevTmDataInd;

//ICAT EXPORTED STRUCT
typedef struct CiDevDataIndPara_struct
{
    UINT8     rlcMode;  /**< RLC mode. 1:AM;2:UM;3:TM; Other values are reserved*/

    union
    {
      CiDevAmDataInd  dlAmData;
      CiDevTmDataInd  dlTmData;
    }dataIndPara;
} CiDevDataIndPara;

//ICAT EXPORTED STRUCT
typedef struct CiDevDlCchParam_struct
{
    UINT8     rbId;  /**< Rb identity. 0:CCCH;1~32:DCCH;36:MCCH*/
    CiDevDataIndPara    dataIndPara;  /**<  \sa CiDevDataIndPara */
} CiDevDlCchParam;

//ICAT EXPORTED STRUCT
typedef struct CiDevAmDataReq_struct
{
    CiBoolean cnfReq;   /**< If the value is true, uplayer requests RLC to confirm the reception of RLC SDUs by te peer-RLC AM entity.
                                                  If the value is false, no confirmation is requested*/
    CiBoolean fieldInd; /**< Indicate the presence or absence of MUI. When disc_req and CNF are both false, MUI does not exist. other situation, MUI exists.*/
    UINT16    mui;      /**< RLC shall give discarded info using this identifier for SDU. MUI: 0~65535*/
} CiDevAmDataReq;

//ICAT EXPORTED STRUCT
typedef struct CiDevUmDataReq_struct
{
    UINT16    mui;  /**< RLC shall give discarded info using this identifier for SDU. MUI: 0~65535*/
} CiDevUmDataReq;

//ICAT EXPORTED STRUCT
typedef struct CiDevTmDataReq_struct
{
    UINT16    mui;  /**< RLC shall give discarded info using this identifier for SDU. MUI: 0~65535*/
} CiDevTmDataReq;

typedef struct CiDevDataReqPara_struct
{
    UINT8     rlcMode;  /**< RLC mode. 1:AM;2:UM;3:TM; Other values are reserved*/

    union
    {
      CiDevAmDataReq  dlAmData;
      CiDevUmDataReq  dlUmData;
      CiDevTmDataReq  dlTmData;
    }dataReqPara;
} CiDevDataReqPara;

//ICAT EXPORTED STRUCT
typedef struct CiDevUlCchParam_struct
{
    UINT8     rbId;    /**< Rb identity. 0~32*/
    CiBoolean discReq;  /**< If the value is true, uplink layer requests RLC for the discarded RLC SDU by local entity.
                                                  If the value if false,no discarded RLC SDU is requested*/
    CiDevDataReqPara    dataReqPara;  /**<  \sa CiDevDataReqPara */
} CiDevUlCchParam;

//ICAT EXPORTED ENUM
typedef enum CIDEV_GSM_CHANNEL_TYPE_TAG {
    CI_DEV_GSM_PBCCH = 1,
    CI_DEV_GSM_PRACH,
    CI_DEV_GSM_PCCCH,
    CI_DEV_GSM_PDTCH,
    CI_DEV_GSM_PACCH,
    CI_DEV_GSM_BCCH,
    CI_DEV_GSM_N_BCCH,
    CI_DEV_GSM_E_BCCH,
    CI_DEV_GSM_CCCH,
    CI_DEV_GSM_PAG_CH,
    CI_DEV_GSM_RACH,

    CI_DEV_GSM_SDCCH,
    CI_DEV_GSM_SDCCH4_0,  /**< SDCCH/4 subchannel number 0*/
    CI_DEV_GSM_SDCCH4_1,  /**< SDCCH/4 subchannel number 1*/
    CI_DEV_GSM_SDCCH4_2,  /**< SDCCH/4 subchannel number 2*/
    CI_DEV_GSM_SDCCH4_3,  /**< SDCCH/4 subchannel number 3*/
    CI_DEV_GSM_SDCCH8_0,  /**< SDCCH/8 subchannel number 0*/
    CI_DEV_GSM_SDCCH8_1,  /**< SDCCH/8 subchannel number 1*/
    CI_DEV_GSM_SDCCH8_2,  /**< SDCCH/8 subchannel number 2*/
    CI_DEV_GSM_SDCCH8_3,  /**< SDCCH/8 subchannel number 3*/
    CI_DEV_GSM_SDCCH8_4,  /**< SDCCH/8 subchannel number 4*/
    CI_DEV_GSM_SDCCH8_5,  /**< SDCCH/8 subchannel number 5*/
    CI_DEV_GSM_SDCCH8_6,  /**< SDCCH/8 subchannel number 6*/
    CI_DEV_GSM_SDCCH8_7,  /**< SDCCH/8 subchannel number 7*/

    CI_DEV_GSM_SACCH,
    CI_DEV_GSM_FACCH,
    CI_DEV_GSM_TCH_F,
    CI_DEV_GSM_TCH_H_0,  /**< TCH/H subchannel number 0*/
    CI_DEV_GSM_TCH_H_1,  /**< TCH/H subchannel number 1*/

    CI_DEV_GSM_PPCH,
    CI_DEV_GSM_ETCH_F,
    CI_DEV_GSM_ATCH_F,
    CI_DEV_GSM_ATCH_H,
    CI_DEV_GSM_EIACCH_F,
    CI_DEV_GSM_CBCH_4,
    CI_DEV_GSM_CBCH_8,

    CI_DEV_NUM_GSM_CHANNEL
} _CiDevGsmChannelType;

typedef UINT8 CiDevGsmChannelType;

//ICAT EXPORTED ENUM
typedef enum CIDEV_GSM_BURST_TYPE_TAG {
    CI_DEV_GSM_ONE_ACCESS_BURST_OF_TYPE_8_BITS = 0,
    CI_DEV_GSM_ONE_ACCESS_BURST_OF_TYPE_11_BITS,
    CI_DEV_GSM_FOUR_ACCESS_BURST_OF_TYPE_8_BITS,
    CI_DEV_GSM_FOUR_ACCESS_BURST_OF_TYPE_11_BITS,
    CI_DEV_GSM_FREQUENCY_CORRECTION_BURST,
    CI_DEV_GSM_SYNCHRONIZATION_BURST,
    CI_DEV_GSM_DUMMY_BURST,
    CI_DEV_GSM_NORMAL_CS_1_RADIO_BLK_BURST,
    CI_DEV_GSM_NORMAL_CS_2_RADIO_BLK_BURST,
    CI_DEV_GSM_NORMAL_CS_3_RADIO_BLK_BURST,
    CI_DEV_GSM_NORMAL_CS_4_RADIO_BLK_BURST,
    CI_DEV_GSM_NORMAL_BURST = CI_DEV_GSM_NORMAL_CS_1_RADIO_BLK_BURST,

    CI_DEV_NUM_GSM_BURST
} _CiDevGsmBurstType;

typedef UINT8 CiDevGsmBurstType;
typedef UINT8 CiDevGsmSignalName;

//ICAT EXPORTED STRUCT
typedef struct CiDevGsmParam_struct
{
    CiBoolean     isBadFrame;    /**< 1: bad frame; 0:not bad frame*/
    CiBoolean     isUpDownData;  /**< 1: uplink data; 0:downlink data*/
    CiBoolean     isNasMsg;      /**< 1: nas message; 0:not nas message*/

    CiDevGsmChannelType   chType;   /**<  \sa CiDevGsmChannelType */
    CiDevGsmCodingScheme  cs;       /**<  \sa CiDevGsmCodingScheme */
    CiDevGsmBurstType burstType;    /**<  \sa CiDevGsmBurstType */
    CiDevGsmSignalName signalName;  /*add for GSM message decode*/

    UINT8 sapi;        /**< sapi=0 or sapi=3, other reserved */
} CiDevGsmParam;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_SET_NETWORK_MONITOR_OPTION_REQ">   */
typedef struct CiDevPrimSetNetworkMonitorOptReq_struct
{
    CiBoolean Option;         /**<  Always hard coded with zero. \sa CiBoolean  */
    CiDevNwMonitorMode  Mode;       /**< Report mode. \sa CiDevNwMonitorMode */
} CiDevPrimSetNetworkMonitorOptReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_SET_NETWORK_MONITOR_OPTION_CNF">   */
typedef struct CiDevPrimSetNetworkMonitorOptCnf_struct
{
    CiDevRc rc;        /**< Result code. \sa  CiDevRc */
    UINT8   res1U8[2]; /**< (padding) */
} CiDevPrimSetNetworkMonitorOptCnf;

/** <paramref name="CI_DEV_PRIM_GET_NETWORK_MONITOR_OPTION_REQ">   */
typedef CiEmptyPrim CiDevPrimGetNetworkMonitorOptReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_GET_NETWORK_MONITOR_OPTION_CNF">   */
typedef struct CiDevPrimGetNetworkMonitorOptCnf_struct
{
    CiDevRc rc;         /**< Result code. \sa  CiDevRc */
    CiBoolean Option;         /**<  Always hard coded with zero. \sa CiBoolean  */
    CiDevNwMonitorMode  Mode;       /**< Report mode. \sa CiDevNwMonitorMode */
} CiDevPrimGetNetworkMonitorOptCnf;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_SET_PROTOCOL_STATUS_CONFIG_REQ">   */
typedef struct CiDevPrimSetProtocolStatusConfigReq_struct
{
    CiBoolean    Option;         /**<  0: disable unsolicited result code(default); 1:enable unsolicited result code*/
} CiDevPrimSetProtocolStatusConfigReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_SET_PROTOCOL_STATUS_CONFIG_CNF">   */
typedef struct CiDevPrimSetProtocolStatusConfigCnf_struct
{
    CiDevRc rc;         /**< Result code. \sa  CiDevRc */
    UINT8   res1U8[2];  /**< (padding) */
} CiDevPrimSetProtocolStatusConfigCnf;

/** <paramref name="CI_DEV_PRIM_GET_PROTOCOL_STATUS_CONFIG_REQ">   */
typedef CiEmptyPrim CiDevPrimGetProtocolStatusConfigReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_GET_PROTOCOL_STATUS_CONFIG_CNF">   */
typedef struct CiDevPrimGetProtocolStatusConfigCnf_struct
{
    CiDevRc rc;         /**< Result code. \sa  CiDevRc */
    CiBoolean  Option;  /**<  0: disable unsolicited result code(default); 1:enable unsolicited result code*/
} CiDevPrimGetProtocolStatusConfigCnf;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_PROTOCOL_STATUS_CHANGED_IND">   */
typedef struct CiDevPrimProtocolStatusChangedInd_struct
{
    CiDevProtocolStatus status;     /**< Protocol status. \sa CiDevProtocolStatus */
    UINT8   res1U8[3];   /**< (padding) */
} CiDevPrimProtocolStatusChangedInd;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_SET_EVENT_IND_CONFIG_REQ">   */
typedef struct CiDevPrimSetEventIndConfigReq_struct
{
    CiBoolean    Option;         /**<  0: disable intermediate result code(default); 1:enable intermediate result code*/
} CiDevPrimSetEventIndConfigReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_SET_EVENT_IND_CONFIG_CNF">   */
typedef struct CiDevPrimSetEventIndConfigCnf_struct
{
    CiDevRc rc;         /**< Result code. \sa  CiDevRc */
    UINT8   res1U8[2];  /**< (padding) */
} CiDevPrimSetEventIndConfigCnf;

/** <paramref name="CI_DEV_PRIM_GET_EVENT_IND_CONFIG_REQ">   */
typedef CiEmptyPrim CiDevPrimGetEventIndConfigReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_GET_EVENT_IND_CONFIG_CNF">   */
typedef struct CiDevPrimGetEventIndConfigCnf_struct
{
    CiDevRc rc;         /**< Result code. \sa  CiDevRc */
    CiBoolean  Option;  /**<  0: disable unsolicited result code(default); 1:enable unsolicited result code*/
} CiDevPrimGetEventIndConfigCnf;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_EVENT_REPORT_IND">   */
typedef struct CiDevPrimEventReportInd_struct
{
    UINT32       timeStamp;       /**< System tick value, (0,4294967295).*/
    CiBitRange   operationType;   /**< Operation type, bitmap - bit0:CS voice;bit1:CS data;bit2:PS;bit3:SMS \sa CiDevEventOperType */
    CiDevEventId eventId;         /**< Event ID.*/
} CiDevPrimEventReportInd;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_SET_WIRELESS_PARAM_CONFIG_REQ">   */
typedef struct CiDevPrimSetWirelessParamConfigReq_struct
{
    CiBoolean    Option;         /**<  0: disable result code presentation to TE(default); 1:enable result code presentation to TE*/
    UINT16       Interval;       /**< Report Interval (seconds) for PERIODIC, default 3s */
} CiDevPrimSetWirelessParamConfigReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_SET_WIRELESS_PARAM_CONFIG_CNF">   */
typedef struct CiDevPrimSetWirelessParamConfigCnf_struct
{
    CiDevRc rc;         /**< Result code. \sa  CiDevRc */
    UINT8   res1U8[2];  /**< (padding) */
} CiDevPrimSetWirelessParamConfigCnf;

/** <paramref name="CI_DEV_PRIM_GET_WIRELESS_PARAM_CONFIG_REQ">   */
typedef CiEmptyPrim CiDevPrimGetWirelessParamConfigReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_GET_WIRELESS_PARAM_CONFIG_CNF">   */
typedef struct CiDevPrimGetWirelessParamConfigCnf_struct
{
    CiDevRc rc;         /**< Result code. \sa  CiDevRc */
    CiBoolean    Option;         /**<  0: disable result code presentation to TE(default); 1:enable result code presentation to TE*/
    UINT16       Interval;       /**< Report Interval (seconds) for PERIODIC, default 3s */
} CiDevPrimGetWirelessParamConfigCnf;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_WIRELESS_PARAM_IND">   */
typedef struct CiDevPrimWirelessParamInd_struct
{
    CiDevEngModeNetwork network;  /**< Network Type (GSM/UMTS). \sa CiDevEngModeNetwork */

    UINT32       timeStamp;       /**< System tick value, (0,4294967295).*/

    CiDevCommonData  commonInfo;  /**< Common parameters  */

    UINT32       data_size;       /**< Size of data for CiDevTddData/CiDevGsmData */
    UINT8        tddGsmData[CI_DEV_MAX_WIRELESS_DATA_LENGTH];    /**< data for CiDevTddData/CiDevGsmData */
    UINT8        count;           /**< Ordinal number of the data. [range:1-2]  */
} CiDevPrimWirelessParamInd;


//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_SET_SIGNALING_REPORT_CONFIG_REQ">   */
typedef struct CiDevPrimSetSignalingReportConfigReq_struct
{
    CiBoolean    Option;       /**<  0: disable unsolicited result code(default); 1:enable unsolicited result code*/
    CiBoolean    Mode;         /**<  0: disable Uu signaling report(default); 1:enable Uu signaling report*/
} CiDevPrimSetSignalingReportConfigReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_SET_SIGNALING_REPORT_CONFIG_CNF">   */
typedef struct CiDevPrimSetSignalingReportConfigCnf_struct
{
    CiDevRc rc;         /**< Result code. \sa  CiDevRc */
    UINT8   res1U8[2];  /**< (padding) */
} CiDevPrimSetSignalingReportConfigCnf;

/** <paramref name="CI_DEV_PRIM_GET_SIGNALING_REPORT_CONFIG_REQ">   */
typedef CiEmptyPrim CiDevPrimGetSignalingReportConfigReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_GET_SIGNALING_REPORT_CONFIG_CNF">   */
typedef struct CiDevPrimGetSignalingReportConfigCnf_struct
{
    CiDevRc rc;         /**< Result code. \sa  CiDevRc */
    CiBoolean    Option;       /**<  0: disable unsolicited result code(default); 1:enable unsolicited result code*/
    CiBoolean    Mode;         /**<  0: disable Uu signaling report(default); 1:enable Uu signaling report*/
} CiDevPrimGetSignalingReportConfigCnf;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_DEV_PRIM_SIGNALING_REPORT_IND">   */
typedef struct CiDevPrimSignalingReportInd_struct
{
    UINT16       totalMsglen;     /**< The total message length = parameter length + peer message length + 17 */
    UINT8        frameType;       /**< The type of freame, 0xAA: used for trace data; others: reserved.*/

    CiDevSignalingMsgId msgId;    /**< The message identifier. \sa CiDevSignalingMsgId */
    UINT32       timeStamp;       /**< Timrt counter, Unit:ms, the value would be set to 0 when power on.*/

    UINT16       paramLen;        /**< The length of the parameter data in byte, the length can be zero, in such case, the parameter would include nother */
    union
    {
      CiDevSystemInfoType bchParam;
      CiDevBcchFachParam  bcchFachParam;
      CiDevPcchPchParam   pcchPchParam;
      CiDevDlCchParam     dlCchParam;
      CiDevUlCchParam     ulCchParam;
      CiDevGsmParam       gsmParam;
    }paramData;

    UINT16   peerMsgLen;         /**< The length of the peer message in byte */
    UINT16   freeHeaderSpaceLen; /**< The length of the free bit header space in bit. It is said the number of the free header space length bits is just filled for alignment, and not usefull */
    UINT8    peerMsgData[CI_DEV_MAX_PEER_MSG_LENGTH];         /**< Peer message encoded with ASN.1.*/
} CiDevPrimSignalingReportInd;
/*Mason CMCC Smart Network Monitor support -- END*/

/* ADD NEW COMMON PRIMITIVES DEFINITIONS HERE */

#ifdef CI_CUSTOM_EXTENSION
/* it is assumed that only one customized set of extension primitives is
 * to be considered at one time. The selection of the particular customized
 * set is done in the 'ci_dev_cust.h' based on compile flags.
 *
 * Note: if no customer extension primitives are defined for this service group
 * the CI_DEV_NUM_CUST_PRIM will be set to 0 in the "ci_dev_cust.h" file.
 */
#include "ci_dev_cust.h"

#define CI_DEV_NUM_PRIM ( CI_DEV_NUM_COMMON_PRIM + CI_DEV_NUM_CUST_PRIM )

#else

/* if no customer extension is supported, only the default common set is considered */
#define CI_DEV_NUM_PRIM CI_DEV_NUM_COMMON_PRIM

#endif /* CI_CUSTOM_EXTENSION */

#ifdef __cplusplus
}
#endif  //__cplusplus

#endif  /* _CI_DEV_H_ */


/*                      end of ci_dev.h
   --------------------------------------------------------------------------- */

