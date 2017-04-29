/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/* ===========================================================================
   File        : ci_mm.h
   Description : Data types file for the MM Service Group

   Notes       :

   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code ("Material") are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intels prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.

   Unless otherwise agreed by Intel in writing, you may not remove or alter this notice or any other notice embedded
   in Materials by Intel or Intels suppliers or licensors in any way.

   =========================================================================== */

#if !defined(_CI_MM_H_)
#define _CI_MM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ci_api_types.h"

/** \addtogroup  SpecificSGRelated
 * @{ */

#define CI_MM_VER_MAJOR 3
#define CI_MM_VER_MINOR 1

/* ----------------------------------------------------------------------------- */

/* CI_MM Primitive ID definitions */
//ICAT EXPORTED ENUM
/** Summary of primitives */
//ICAT EXPORTED ENUM
typedef enum CI_MM_PRIM
{
	CI_MM_PRIM_GET_NUM_SUBSCRIBER_NUMBERS_REQ = 1,                  /**< \brief Requests the number of subscriber number entries in the MSISDN list \details */
	CI_MM_PRIM_GET_NUM_SUBSCRIBER_NUMBERS_CNF,                      /**< \brief Confirms the request to return the number of subscriber number entries in the MSISDN list \details Requires the SIM to be inserted and ready, plus prior PIN1 validation. */
	CI_MM_PRIM_GET_SUBSCRIBER_INFO_REQ,                             /**< \brief Requests subscriber information for a specified entry in the MSISDN list  \details This information is stored on the SIM, so for this request to succeed, the SIM must be inserted and ready.
									 *   Access to the MSISDN list requires prior PIN1 (CHV1) validation.
									 *   Use the CI_MM_PRIM_GET_NUM_SUBSCRIBER_NUMBERS_REQ to determine the number of MSISDN list entries. */
	CI_MM_PRIM_GET_SUBSCRIBER_INFO_CNF,                             /**< \brief Confirms the request to return subscriber information for a specified entry in the MSISDN list \details */
	CI_MM_PRIM_GET_SUPPORTED_REGRESULT_OPTIONS_REQ,                 /**< \brief Requests the supported settings for the unsolicited network registration reporting option \details */
	CI_MM_PRIM_GET_SUPPORTED_REGRESULT_OPTIONS_CNF,                 /**< \brief Confirms the request to return the supported settings for the unsolicited network registration reporting option \details There should be no reason for an error result. */
	CI_MM_PRIM_GET_REGRESULT_OPTION_REQ,                            /**< \brief Requests the current reporting option for Unsolicited Network Registration Result Indications \details See CI_MM_PRIM_SET_REGRESULT_OPTION for default information. */
	CI_MM_PRIM_GET_REGRESULT_OPTION_CNF,                            /**< \brief Confirms the request to return the current reporting option for Unsolicited Network Registration Result Indications \details There should be no reason for an error result. */
	CI_MM_PRIM_SET_REGRESULT_OPTION_REQ,                            /**< \brief Request to set the reporting option for Unsolicited Network Registration Result Indications \details Unsolicited Registration Result Indications (CI_MM_PRIM_REG_RESULT_IND) are sent (if enabled) only if the reported information
									 *  has changed since the last indication.
									 *  CIMM_REGRESULT_STATUS is the default reporting option. */
	CI_MM_PRIM_SET_REGRESULT_OPTION_CNF,                            /**< \brief Confirms a request to set the reporting option for Unsolicited Network Registration Result Indications \details */
	CI_MM_PRIM_REGRESULT_IND,                                                       /**< \brief Indicates the Unsolicited Network Registration Result  \details Receipt of this indication (and the information it contains) can be configured by the
											 *  CI_MM_PRIM_SET_REGRESULT_OPTION_REQ request.
											 *  If this indication is enabled, the current registration status (if available) is reported.
											 *  As a configuration option, current cell information (if available) can also be included.
											 *  This information can also be requested at any time, using the CI_CC_PRIM_GET_REGRESULT_INFO_REQ request.
											 *  No explicit response is required. */
	CI_MM_PRIM_GET_REGRESULT_INFO_REQ,                              /**< \brief Requests the most recent registration result information \details See CI_MM_PRIM_SET_REGRESULT_OPTION for default information. */
	CI_MM_PRIM_GET_REGRESULT_INFO_CNF,                              /**< \brief Confirms the request to return the most recent registration result information \details Use the CI_MM_PRIM_GET_REGRESULT_OPTION_REQ request to get the current registration result reporting option.
									 *   This option setting may affect the availability of registration result information.
									 *   The current registration status and location information (if available) are included. */
	CI_MM_PRIM_GET_SUPPORTED_ID_FORMATS_REQ,                /**< \brief Requests a list of supported format indicators for the network/operator ID information
								 * \details These format indicators are used in the CiMmNetOpIdInfo structure, to indicate how SAC should format the network or operator
								 *   identification information.  */
	CI_MM_PRIM_GET_SUPPORTED_ID_FORMATS_CNF,                /**< \brief Confirms the request and returns a list of supported format indicators for the network/operator ID information
								 * \details There should be no reason for an error result. */
	CI_MM_PRIM_GET_ID_FORMAT_REQ,                                                   /**< \brief  Requests the currently selected network operator ID format indicator  \details The network operator ID format indicator selects which of the supported formats SAC will use to represent the network/operator ID. */
	CI_MM_PRIM_GET_ID_FORMAT_CNF,                                                   /**< \brief  Confirms the request to return the currently selected network operator ID format indicator  \details There should be no reason for an error result. */
	CI_MM_PRIM_SET_ID_FORMAT_REQ,                                                   /**< \brief  Requests to set the network/operator ID format indicator \details The network operator ID format indicator selects which of the supported formats SAC will use to represent the network/operator ID
											 *  when reporting network operator information. The default format indicator is set for a numeric network ID. */
	CI_MM_PRIM_SET_ID_FORMAT_CNF,                                                   /**< \brief  Confirms the request to set the network/operator ID format indicator \details */
	CI_MM_PRIM_GET_NUM_NETWORK_OPERATORS_REQ,                                       /**< \brief  Requests the number of operators present in the network \details */
	CI_MM_PRIM_GET_NUM_NETWORK_OPERATORS_CNF,                                       /**< \brief  Confirms the request to get the number of operators present in the network \details */
	CI_MM_PRIM_GET_NETWORK_OPERATOR_INFO_REQ,               /**< \brief  Requests information about a specified operator present in the network  \details Use CI_CC_PRIM_GET_NUM_NETWORK_OPERATORS_REQ to determine the number of operators present in the network,
								 *  if there are any. This number determines the range of values for the Index parameter.
								 *  Index values start at 1, which indicates the first operator in the network (usually the home network operator). */
	CI_MM_PRIM_GET_NETWORK_OPERATOR_INFO_CNF,               /**< \brief  Confirms the request to get information about a specified operator present in the network  \details There may be no operators currently present in the network. In that case, the network operator status information is not
								 *  included.
								 *  Status for network operators present should be indexed in the following order of precedence (with the highest precedence listed first):
								 *  Home network operator (if present)
								 *  Operators for networks that are referenced in the SIM
								 *  Other network operators that are present
								 *  The network and operator ID information is presented in all supported formats. If information for any of the formats is unavailable, SAC indicates this in the CiMmNetOpStatusInfo structure as follows:
								 *  Unavailable operator ID has its Length field set to zero.
								 *  Unavailable network ID has its fields set to CIMM_COUNTRYCODE_NONE and CIMM_NETWORKCODE_NONE. */
	CI_MM_PRIM_GET_NUM_PREFERRED_OPERATORS_REQ,     /**< \brief Requests the number of entries in the preferred network operators list  \details The preferred network operators list is stored on the SIM in the EFPLMNSel file.
							 *  The maximum number of entries in the EFPLMNSel file is specified when the SIM is provisioned, but the file must accommodate at least
							 *  8 PLMN entries. See [1] for more information. */
	CI_MM_PRIM_GET_NUM_PREFERRED_OPERATORS_CNF,     /**< \brief Confirms the request to get the number of entries in the preferred network operators list  \details If the SIM is not present and ready, SAC sets the NumPref parameter to zero. */
	CI_MM_PRIM_GET_PREFERRED_OPERATOR_INFO_REQ,     /**< \brief  Requests information for a specified entry in the preferred network operators list  \details Use  CI_CC_PRIM_GET_NUM_PREFERRED_OPERATORS_REQ to determine the number of entries in the preferred network
							 *  operators list. This number determines the range of values for the Index parameter.
							 *  The preferred operator list is stored in the EFPLMNSel file on the SIM, and requires the Card Holder Verification password CHV1
							 *  (if enabled) to be established before access to this file is allowed.
							 *  The maximum number of entries in the EFPLMNSel file is specified when the SIM is provisioned, but the file must accommodate at least
							 *  8 PLMN entries. See [1] for more information. */
	CI_MM_PRIM_GET_PREFERRED_OPERATOR_INFO_CNF,     /**< \brief Confirms the request to get information for a specified entry in the preferred network operators list
							 * \details The network/operator ID information is presented in the default format, or in the format set by the most recent
							 *  CI_CC_PRIM_SET_ID_FORMAT_REQ request. */
	CI_MM_PRIM_ADD_PREFERRED_OPERATOR_REQ,                          /**< \brief Requests a new entry to be added to the preferred network operators list  \details Adds a new entry to the end of the Preferred Operators List.
									 *  The Preferred Operators List is stored in the EFPLMNSel file on the SIM, and requires a Card Holder Verification password CHV1
									 *  (if enabled) to be established before access to this file is allowed. */
	CI_MM_PRIM_ADD_PREFERRED_OPERATOR_CNF,                          /**< \brief Confirms a request to add a new entry to the preferred network operators list  \details The network/operator ID information must be presented in the default format, or in the format set by the most recent CI_CC_PRIM_SET_ID_FORMAT_REQ request.
									 *  If the request fails, the list is unchanged. The maximum number of entries in the EFPLMNSel file is specified when the SIM is
									 *  provisioned, but the file must accommodate at least 8 PLMN entries. See [1] for more information.
									 *  The number of entries in the list is returned regardless of the success/failure of the request. */
	CI_MM_PRIM_DELETE_PREFERRED_OPERATOR_REQ,               /**< \brief Requests an entry to be deleted from the preferred network operators list  \details Use CI_CC_PRIM_GET_NUM_PREFERRED_OPERATORS_REQ to determine the number of entries in the preferred network
								 *  operators list. This number determines the range of values for the Index parameter.
								 *  The preferred operator list is stored in the EFPLMNSel file on the SIM, and requires a Card Holder Verification password CHV1
								 *  (if enabled) to be verified before access to this file is allowed. */
	CI_MM_PRIM_DELETE_PREFERRED_OPERATOR_CNF,               /**< \brief Confirms a request to delete an entry from the preferred network operators list  \details If the request fails, the list is unchanged.
								 *  The maximum number of entries in the EFPLMNSel file is specified when the SIM is provisioned, but the file must accommodate at
								 *  least 8 PLMN entries. See "Cellular Interface Application Programming Interface", revision i0.6, for more information.
								 *  The number of entries in the list is returned regardless of the success/failure of the request. */
	CI_MM_PRIM_GET_CURRENT_OPERATOR_INFO_REQ,                                       /**< \brief Requests information about the current network operator (if there is one)   \details */
	CI_MM_PRIM_GET_CURRENT_OPERATOR_INFO_CNF,                                       /**< \brief Confirms the request to get information about the current network operator (if there is one)  \details */
	CI_MM_PRIM_AUTO_REGISTER_REQ,                                                   /**< \brief Requests automatic registration  \details Uses PLNM lists stored on the SIM, so an installed SIM is required.
											 *  The handset is always in automatic PLMN selection mode, except when a manual registration request is received.
											 *  After completing a manual registration operation, SAC resets the registration mode to automatic. Therefore, the application layer
											 *  does not need to use this request to reset the current registration mode to automatic.
											 *  The PLMN selection mode (registration mode) is not saved to NVRAM; it is always set to automatic mode during SAC initialization. */
	CI_MM_PRIM_AUTO_REGISTER_CNF,                                                   /**< \brief Confirms a request for automatic registration \details */
	CI_MM_PRIM_MANUAL_REGISTER_REQ,                                         /**< \brief  Requests manual registration \details The registration result itself is relayed by a CI_MM_PRIM_REGRESULT_IND indication. It can also be retrieved on demand, using
										 *  the CI_MM_PRIM_GET_REGRESULT_INFO_REQ request.
										 * On successful completion of this request, SAC resets the registration mode to CIMM_REGMODE_AUTOMATIC. */
	CI_MM_PRIM_MANUAL_REGISTER_CNF,                                         /**< \brief Confirms a request for manual registration  \details The registration result is relayed by CI_MM_PRIM_REGRESULT_IND, if this is enabled. The information can also
										 *  be retrieved on demand, using CI_MM_PRIM_GET_REGRESULT_INFO_REQ.
										 *  On successful completion of this request, SAC resets the current registration mode to automatic. */
	CI_MM_PRIM_DEREGISTER_REQ,                                              /**< \brief Requests deregistration  \details */
	CI_MM_PRIM_DEREGISTER_CNF,                                              /**< \brief Confirms a request for deregistration  \details The deregistration result is relayed by CI_MM_PRIM_REGRESULT_IND. It can also be retrieved on demand, using
										 *  CI_MM_PRIM_GET_REGRESULT_INFO_REQ. */
	CI_MM_PRIM_GET_SIGQUALITY_IND_CONFIG_REQ,                               /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_MM_PRIM_GET_SIGQUALITY_IND_CONFIG_CNF,                               /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_MM_PRIM_SET_SIGQUALITY_IND_CONFIG_REQ,               /**< \brief Requests the current configuration for unsolicited signal quality indications \details Unsolicited signal quality indications can be configured in one of two ways:
								 *   -	Report signal quality information periodically. The time interval is specified in 100 ms units.
								 *   -	Report signal quality information when the RSS changes by more than a specified threshold. The threshold is specified in dBm.
								 *   These two configuration options are mutually exclusive. */
	CI_MM_PRIM_SET_SIGQUALITY_IND_CONFIG_CNF,                               /**< \brief Confirms a request to set the current configuration for unsolicited signal quality indications \details */
	CI_MM_PRIM_SIGQUALITY_INFO_IND,                                         /**< \brief Indicates the unsolicited signal quality   \details This indication can be configured by CI_MM_PRIM_SET_SIGQUALITY_IND_CONFIG_REQ.
										 *  The RSS value is reported in dBm, and should be in the range -113dBm through -51 dBm.
										 *  The bit error rate (BER) is reported as an encoded value between 0 and 7. The upper layers should convert this value to a suitable
										 *  BER representation.
										 *  No explicit response is required. */

// SCR #1401348
	CI_MM_PRIM_ENABLE_NETWORK_MODE_IND_REQ,                 /**< \brief Requests that network mode indication be enabled or disabled  \details The network mode indication (if enabled) is sent whenever the current network mode changes.
								 *  By default, the network mode indication is disabled. */
	CI_MM_PRIM_ENABLE_NETWORK_MODE_IND_CNF,                 /**< \brief Confirms a request to enable or disable network mode indication. \details By default, the network mode indication is disabled. */
	CI_MM_PRIM_NETWORK_MODE_IND,                                                    /**< \brief Indicates the current network mode   \details Each of the CiMmNetworkMode parameters indicates the PDP status for their indicated system:
											 *  - gprsActive (1 - gprs is active, 0 - gprs is inactive)
											 *  - egprsActive (1 - egprs is active, 0 - egprs is inactive)
											 *  - hsdpaActive (1 - hsdpa is active, 0 - hsdpa is inactive)
											 *  - hsupaActive (1 - hsupa is active, 0 - hsupa is inactive)
											 *  This indication can be enabled or disabled by CI_MM_PRIM_ENABLE_NETWORK_MODE_IND_REQ.
											 *  By default, this indication is disabled.
											 *  No explicit response is required. */
	CI_MM_PRIM_GET_NITZ_INFO_REQ,                                   /**< \brief Requests the current network identity and time zone (NITZ) information
									 * \details NITZ information is updated by the protocol stack whenever it changes, for example, when acquiring or re-acquiring network service. */
	CI_MM_PRIM_GET_NITZ_INFO_CNF,                                   /**< \brief Confirms a request for current network identity and time zone (NITZ) information  \details */
	CI_MM_PRIM_NITZ_INFO_IND,                                       /**< \brief Indicates the status of the current network identity and time zone (NITZ) information
									 * \details NITZ information is reported by the protocol stack whenever it changes, for example, when acquiring or re-acquiring network service.
									 * NITZ indications are enabled by default. */

	CI_MM_PRIM_CIPHERING_STATUS_IND,                                        /**< \brief Indicates a ciphering status change   \details
										 * The protocol stack sends a cipher indication signal to the application layer
										 * specifying the CS and PS ciphering status. SAC captures this signal and sends a
										 * 'CiMmPrimCipheringStatusInd' notification.
										 * The authentication and ciphering procedure is always initiated and controlled by the network.
										 * The following events trigger a cipher notification:
										 *		- A request by the network to authenticate and/or set the ciphering mode
										 *		- GPRS authenticate confirmation from the SIM
										 *		- Processing the authentication result
										 *		- Failure to release a CS connection
										 *		- Invalidating the GPRS parameters that are stored on the SIM and
										 *		  marking the SIM as invalid for GPRS services
										 *		- Receiving a SYNC signal indicating ciphering mode setting or some channel assignment or modification
										 *
										 * An additional parameter is needed to indicate if display of
										 * the ciphering indicator is required. This parameter is provided in the
										 * OFM bit (first bit) of the 'additional information' entry (bytes 2 and 3) of the EF_AD (administrative
										 * data) SIM/USIM file.
										 */
	CI_MM_PRIM_AIR_INTERFACE_REJECT_CAUSE_IND,              /**< \brief  Indicates an air interface reject cause code  \details
								 * The protocol stack sends an air interface reject cause code indication due to errors that
								 * can occur during MM/GMM procedures such as LU/RA update reject, authentication reject, etc.
								 * These reject codes are intended to enable vendors to give specific visual/audible feedback to the user.
								 */
	/* Michal Bukai - Selection of preferred PLMN list +CPLS - START */
	CI_MM_PRIM_SELECT_PREFERRED_PLMN_LIST_REQ, /**< \brief Requests to select the preferred PLMN list
						 * \details The selected preffered PLMN list will be used when operation on the list is required */
	CI_MM_PRIM_SELECT_PREFERRED_PLMN_LIST_CNF,      /**<  \brief Confirms the request to select a preferred PLMN list */
	CI_MM_PRIM_GET_PREFERRED_PLMN_LIST_REQ,         /**< \brief Requests to read what is the selected preferred PLMN list */
	CI_MM_PRIM_GET_PREFERRED_PLMN_LIST_CNF,         /**< \brief Confirms the response and returns the type of the selected preferred PLMN list */
	/* Michal Bukai - Selection of preferred PLMN list +CPLS - END */
	CI_MM_PRIM_BANDIND_IND, /**< \brief  Indicates the current band
				 * \details Indications are sent when the band changes and band indications are enabled. */
	CI_MM_PRIM_SET_BANDIND_REQ,     /**< \brief  Requests to enable/disable band indications  \details  */
	CI_MM_PRIM_SET_BANDIND_CNF,     /**< \brief  Confirms the request to enable/disable band indications  \details  */
	CI_MM_PRIM_GET_BANDIND_REQ,     /**< \brief  Requests the status of band indications (enabled/ disabled) and an indication of the current band  \details  */
	CI_MM_PRIM_GET_BANDIND_CNF,     /**< \brief  Confirms the request and returns the status of band indications (enabled/ disabled) and an indication of the current band \details  */
	CI_MM_PRIM_SERVICE_RESTRICTIONS_IND,

	/* ADD NEW COMMON PRIMITIVES HERE, BEFORE 'CI_MM_PRIM_LAST_COMMON_PRIM' */
	/*Michal Bukai - Cell Lock - Start*/
    CI_MM_PRIM_CELL_LOCK_REQ,		/**< \brief Requests to activate or to deactivate cell lock \details  */
    CI_MM_PRIM_CELL_LOCK_CNF,		/**< \brief Confirms the request and activates or deactivates cell lock \details  */    
    CI_MM_PRIM_CELL_LOCK_IND,		/**< \brief Indicates the status of cell lock \details  */
	/*Michal Bukai - Cell Lock - End*/

	/* ADD NEW COMMON PRIMITIVES HERE, BEFORE 'CI_MM_PRIM_LAST_COMMON_PRIM' */

	/* END OF COMMON PRIMITIVES LIST */
	CI_MM_PRIM_LAST_COMMON_PRIM

	/* The customer specific extension primitives are added starting from
	 * CI_MM_PRIM_firstCustPrim = CI_MM_PRIM_LAST_COMMON_PRIM as the first identifier.
	 * The actual primitive names and IDs are defined in the associated
	 * 'ci_mm_cust_xxx.h' file.
	 */

	/* DO NOT ADD ANY MORE PRIMITIVES HERE */

} _CiMmPrim;

/* specify the number of default common MM primitives */
#define CI_MM_NUM_COMMON_PRIM ( CI_MM_PRIM_LAST_COMMON_PRIM - 1 )
/**@}*/

/** \brief Mobility Management Return Codes  */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIRC_MM
{
	CIRC_MM_SUCCESS = 0,                                    /**< Request completed successfully */
	CIRC_MM_FAIL,                                           /**< General failure (catch-all)    */
	CIRC_MM_INCOMPLETE_INFO,                                /**< Incomplete information for request */
	CIRC_MM_INVALID_ADDRESS,                                /**< Invalid address (phone number) */
	CIRC_MM_NO_SERVICE,                                     /**< No network service */
	CIRC_MM_NOT_REGISTERED,                                 /**< Not currently registered */
	CIRC_MM_REJECTED,                                       /**< Request rejected by network */
	CIRC_MM_TIMEOUT,                                        /**< Request timed out */
	CIRC_MM_UNAVAILABLE,                                    /**< Information not available */
	CIRC_MM_NO_MORE_ENTRIES,                                /**< No more entries in list */
	CIRC_MM_NO_MORE_ROOM,                                   /**< No more room in list */
	CIRC_MM_PLMN_LIST_SIM_NOK,                              /**< PLMN list SIM is not OK */
	CIRC_MM_PLMN_LIST_NOT_FOUND,                            /**< PLMN list is not found */
	CIRC_MM_PLMN_LIST_NOT_ALLOWED,                          /**< PLMN list is not allowed */
	CIRC_MM_PLMN_LIST_MANUAL_NOT_ALLOWED,                   /* manual selection of */
	CIRC_MM_PLMN_LIST_MANUAL_NOT_ALLOWED_IN_DEDICATED_MODE, /* PLMN list is not allowed in dedicated mode*/

	/* This one must always be last in the list! */
	CIRC_MM_NUM_RESCODES                    /**< Number of result codes defined */
} _CiMmResultCode;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief Mobility Management Return Codes
 *  \sa CIRC_MM
 * \remarks Common Data Section */
typedef UINT16 CiMmResultCode;
/**@}*/

/* Tal Porat/Michal Bukai - Network Selection - start */

/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief MM state
 * \remarks Common Data Section */
typedef UINT32 CiMmCause;
/**@}*/

/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief GMM state
 * \remarks Common Data Section */
typedef UINT32 CiGmmCause;
/**@}*/

/* Tal Porat/Michal Bukai - Network Selection - end */

/** \brief Subscriber Number Information: Associated Network Services  */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIMM_SERVICE {
	CIMM_SERVICE_ASYNC_MODEM = 0,                   /**< Asynchronous modem */
	CIMM_SERVICE_SYNC_MODEM,                        /**< Synchronous modem  */
	CIMM_SERVICE_PAD_ASYNC,                         /**< PAD access (asynchronous)  */
	CIMM_SERVICE_PACKET_SYNC,                       /**< Packet access (synchronous)  */
	CIMM_SERVICE_VOICE,                             /**< Voice  */
	CIMM_SERVICE_FAX,                               /**< Fax    */

	/* This one must always be last in the list! */
	CIMM_NUM_SERVICES                               /**< Number of network services defined */
} _CiMmService;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief Subscriber Number Information: Associated network Services
 *  \sa CIMM_SERVICE
 * \remarks Common Data Section */
typedef UINT8 CiMmService;
/**@}*/

/** \brief Service Information: Information Transfer Capability (ITC) Indicators  */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIMM_ITC {
	CIMM_ITC_3_1_KHZ = 0,                   /**< 3.1 kHz */
	CIMM_ITC_UDI,                           /**< Unrestricted digital information (UDI) */

	/* This one must always be last in the list! */
	CIMM_NUM_ITC                            /**< Number of ITC indicators defined */
} _CiMmITC;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief Service Information: Information Transfer Capability (ITC) Indicators
 *  \sa CIMM_ITC
 * \remarks Common Data Section */
typedef UINT8 CiMmITC;
/**@}*/

/** \brief Service Information */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiMmServiceInfo_struct {
	CiBoolean Present;                              /**< Service information present? \sa CCI API Ref Manual */
	CiMmService SvcType;                            /**< Associated service type  \sa CiMmService */
	CiBsTypeSpeed Speed;                            /**< Connection speed \sa CCI API Ref Manual  */
	CiMmITC Itc;                                    /**< Information transfer capability  \sa CiMmITC. */
} CiMmServiceInfo;

/** \brief Subscriber Service Information */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiMmSubscriberInfo_struct {
	CiAddressInfo Number;                   /**< Subscriber number (MSISDN) \sa CCI API Ref Manual */
	CiOptNameInfo AlphaTag;                 /**< Associated alpha tag (optional)  \sa CCI API Ref Manual */
	CiMmServiceInfo SvcInfo;                /**< Service information (optional) \sa CiMmServiceInfo_struct */
} CiMmSubscriberInfo;

/** \brief Registration Result Indication: Reporting Options  */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIMM_REGRESULT_OPTION {
	CIMM_REGRESULT_DISABLE = 0,     /**< Disable reporting */
	CIMM_REGRESULT_STATUS,          /**< Report registration status only */
	CIMM_REGRESULT_CELLINFO,        /**< Report status and current cell information */

	/* This one must always be last in the list! */
	CIMM_NUM_REGRESULT_OPTIONS /**< Number of options defined */
} _CiMmRegResultOption;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief Registration Result Indication: Reporting Options
 *  \sa CIMM_REGRESULT_OPTION
 * \remarks Common Data Section */
typedef UINT8 CiMmRegResultOption;

/* Default Reporting Option */
#define CIMM_REGRESULT_DEFAULT  ((UINT8)CIMM_REGRESULT_STATUS)
/**@}*/

/** \brief Registration result information: registration status indicators  */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIMM_REGSTATUS {
	CIMM_REGSTATUS_NOT_SEARCHING = 0,               /**< Not registered, not searching operators */
	CIMM_REGSTATUS_HOME,                            /**< Registered with home network */
	CIMM_REGSTATUS_SEARCHING,                       /**< Not registered, searching operators */
	CIMM_REGSTATUS_DENIED,                          /**< Registration denied */
	CIMM_REGSTATUS_UNKNOWN,                         /**< Registration status unknown */
	CIMM_REGSTATUS_ROAMING,                         /**< Registered, roaming */
	CIMM_REGSTATUS_EMERGENCY_ONLY,                  /**< Only emergency services are available*/

	/* This one must always be last in the list! */
	CIMM_NUM_REGSTATUS                              /**< Number of status values defined */
} _CiMmRegStatus;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief Registration result information: Registration status indicators
 *  \sa CIMM_REGSTATUS
 * \remarks Common Data Section */
typedef UINT8 CiMmRegStatus;
/**@}*/

/** \brief Registration mode values */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIMM_REGMODE {
	CIMM_REGMODE_AUTOMATIC = 0,                     /**< Automatic registration request */
	CIMM_REGMODE_MANUAL,                            /**< Manual registration request */
	CIMM_REGMODE_DEREGISTER,                        /**< Deregistration request */
	CIMM_REGMODE_MANUAL_AUTO,                       /**< Manual request, fallback to automatic */

	/* This one must always be last in the list! */
	CIMM_NUM_REGMODES                               /**< Number of mode indicators defined */
} _CiMmRegMode;
/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief Registration Mode \sa CIMM_REGMODE
 *  \sa CIMM_REGSTATUS
 * \remarks Common Data Section */
typedef UINT8 CiMmRegMode;
/**@}*/



/* Default Registration Mode */
#define CIMM_REGMODE_DEFAULT  ((UINT8)CIMM_REGMODE_AUTOMATIC)

/** \brief Registration result information: optional current cell information structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiMmCellInfo_struct {
	CiBoolean Present;              /**< Current cell information present? \sa CCI API Ref Manual */
	UINT16 LocArea;                 /**< Location area code */
	UINT32 CellId;                  /**< Cell Identifier. GSM case: 16 least significant bits ,WCDMA case: CellId - 16 least significant bits ,RNCID - 12 most significant bits */
} CiMmCellInfo;

/** \brief Registration Result Information Structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiMmRegResultInfo_struct {
	CiMmRegStatus Status;   /**< Registration status  \sa CiMmRegStatus  */
	CiMmCellInfo CellInfo;  /**< Current cell information (optional)  \sa CiMmRegStatus */
} CiMmRegResultInfo;

/** \brief  Network operator identification information: network/operator ID format indicators */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIMM_NETOP_ID_FORMAT {
	CIMM_NETOP_ID_FORMAT_ALPHA_LONG = 0,            /**< Operator ID: long alphanumeric */
	CIMM_NETOP_ID_FORMAT_ALPHA_SHORT,               /**< Operator ID: short alphanumeric */
	CIMM_NETOP_ID_FORMAT_NETWORK,                   /**< Network ID (numeric) */

	/* This one must always be last in the list! */
	CIMM_NUM_NETOP_ID_FORMATS         /**< Number of format indicators defined */
} _CiMmNetOpIdFormat;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Network operator identification information: network/operator ID format indicators
 * \sa CIMM_NETOP_ID_FORMAT */
/** \remarks Common Data Section */
typedef UINT8 CiMmNetOpIdFormat;

/* Default network/operator ID Format indicator */
#define CIMM_NETOP_ID_FORMAT_DEFAULT  ((UINT8)CIMM_NETOP_ID_FORMAT_NETWORK)

/* Operator Identification Information: Maximum Alphanumeric ID lengths */
#define CIMM_MAX_OPER_ID_LONG   32 /* Long alphanumeric  */     //Michal Bukai change from 16 to 32 - same as RIL define
#define CIMM_MAX_OPER_ID_SHORT  16 /* Short alphanumeric */     //Michal Bukai change from 8 to 16 - same as RIL define
/**@}*/

/** \brief  Network operator identification information: network identification structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiMmNetworkId_struct {
	UINT16 CountryCode;     /**< 3-digit country code */
	UINT16 NetworkCode;     /**< 3-digit network code */
} CiMmNetworkId;

/* Network ID field values used if the information is unavailable*/
#define CIMM_COUNTRYCODE_NONE ((UINT16)0)
#define CIMM_NETWORKCODE_NONE ((UINT8)0)

/* Network operator Identification Information: Operator Identification structure */
#define CIMM_MAX_OPER_ID_LEN  CIMM_MAX_OPER_ID_LONG     /* Maximum length (chars) of Operator Id */

#define CIMM_MAX_LSA_IDENTITY  3                        /* Maximum LSA identity */

/** \brief  Operator  ID structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiMmOperatorId_struct {
	UINT8 Length;                                                           /**< Operator ID length (characters) */
	char Id[ CIMM_MAX_OPER_ID_LEN ];                                        /**< Operator ID */
} CiMmOperatorId;
/** \brief  Netwrok or operator ID*/
/** \remarks Common Data Section */
typedef  union CiMmNetOpId_tag {
	CiMmNetworkId NetworkId;                /**< Network ID  \sa CiMmNetworkId_struct */
	CiMmOperatorId OperatorId;              /**< Operator ID \sa CiMmOperatorId_struct   */
} CiMmNetOpIdUnion;
/** \brief  Network or operator identification */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiNetOpIdInfo_struct {
	CiBoolean Present;                                      /**< Indicates if network or operator ID is present \sa CCI API Ref Manual */
	CiMmNetOpIdFormat Format;                               /**< ID format: network or operator  \sa CiMmNetOpIdFormat   */
	CiMmNetOpIdUnion CiMmNetOpId;                           /**< ID \sa CiMmNetOpId_tag*/
} CiMmNetOpIdInfo;

/** \brief  Network operator status indicators - for the network operator list */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIMM_NETOPSTATUS {
	CIMM_NETOP_UNKNOWN = 0, /**< Operator status unavailable */
	CIMM_NETOP_AVAILABLE,   /**< Operator is available */
	CIMM_NETOP_CURRENT,     /**< Current operator */
	CIMM_NETOP_FORBIDDEN,   /**< Operator is forbidden  */

	/* This one must always be last in the list! */
	CIMM_NUM_NETOPSTATUS    /**< Number of status indicators defined  */
} _CiMmNetOpStatus;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Network operator status indicators - for the network operator list
 * \sa CIMM_NETOPSTATUS */
/** \remarks Common Data Section */
typedef UINT8 CiMmNetOpStatus;

/* Default network operator Status */
#define CIMM_NETOPSTATUS_DEFAULT  ((UINT8)CIMM_NETOP_UNKNOWN)
/**@}*/

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Received signal strength indication (RSSI), measured in dBm */
/** \remarks Common Data Section */
typedef INT8 CiMmRssi;        /**< RSSI value */    /* value down to : -128dBm */
/**@}*/

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Encoded bit error rate (BER) indication */
/** \remarks Common Data Section */
typedef UINT8 CiMmEncodedBER; /**< Reported as an index between 0-7, inclusive  */
/**@}*/

/** \brief  Access technology modes (added in release 4; See TC 27.007) */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIMM_ACT_TECH_MODE
{
	CI_MM_ACT_GSM = 0,              /**< GSM */
	CI_MM_ACT_GSM_COMPACT,          /**< Not supported */
	CI_MM_ACT_UTRAN,                /**< UTRAN */

	CI_MM_NUM_ACT
} _CiMmAccTechMode;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Access technology modes (added in release 4; see TC 27.007)
 * \sa CIMM_ACT_TECH_MODE */
/** \remarks Common Data Section */
typedef UINT8 CiMmAccTechMode;
/**@}*/

/** \brief  Network operator status information structure for the network operator list.
 * If the supported network/operator ID formats are changed, this structure must change accordingly.  */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiNetOpStatusInfo_struct {
	CiMmNetOpStatus Status;                         /**< Network operator status  \sa CiMmNetOpStatus */
	CiMmRssi Rssi;                                  /**< RSSI value in dBm  \sa CiMmRssi  */
	CiMmOperatorId LongAlphaId;                     /**< Long alphanumeric operator ID  \sa CiMmOperatorId_struct   */
	CiMmOperatorId ShortAlphaId;                    /**< Short alphanumeric operator ID  \sa CiMmOperatorId */
	CiMmNetworkId NetworkId;                        /**< Network ID information  \sa CiMmNetworkId */
	CiMmAccTechMode AccTchMode;                     /**< Network access technology (GSM, UTRAN, etc.)  \sa CiMmAccTechMode */
} CiMmNetOpStatusInfo;

/** \brief  Signal quality indications: configuration options */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIMM_SIGQUAL_OPTIONS {
	CIMM_SIGQUAL_OPTION_INTERVAL = 0,       /**< Time interval - used for periodic reports */
	CIMM_SIGQUAL_OPTION_THRESHOLD,          /**< RSSI threshold */
	CIMM_SIGQUAL_OPTION_DISABLE,            /**< Disable indications */

	/* This one must always be last in the list! */
	CIMM_NUM_SIGQUAL_OPTIONS                        /**< Number of status indicators defined  */
} _CiMmSigQualOpts;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Signal Quality Indications: Configuration Options
 * \sa CIMM_SIGQUAL_OPTIONS */
/** \remarks Common Data Section */
typedef UINT8 CiMmSigQualOpts;
/** @} */

/* Signal Quality Indications: Configuration Defaults */
#define CIMM_SIGQUAL_OPTION_DEFAULT     ( (UINT8)CIMM_SIGQUAL_OPTION_THRESHOLD )
#define CIMM_SIGQUAL_THRESHOLD_DEFAULT  ( (UINT8)10 )
#define CIMM_SIGQUAL_INTERVAL_DEFAULT   ( (UINT8)20 )   /* 2 seconds */

/** \brief  Signal quality reports configuration: configuration values depend on report type interval or threshold */
/** \remarks Common Data Section */
typedef    union CfgUnion_Tag {
	UINT8 Interval;                         /**< Time Interval in 100ms units  */
	UINT8 Threshold;                        /**< RSSI threshold in dBm  */
} CfgUnion;
/** \brief  Signal quality indications configuration */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiMmSigQualityConfig_struct {
	CiMmSigQualOpts Option;         /**< Signal quality report type interval or theshold \sa CiMmSigQualOpts */
	CfgUnion Cfg;                   /**< Configuration \sa CfgUnion_Tag*/
} CiMmSigQualityConfig;

/** \brief  Signal quality indications: information structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiMmSigQualityInfo_struct {
	CiMmRssi Rssi;                  /**< RSSI value in dBm  \sa CiMmRssi  */
	CiMmEncodedBER BER;             /**< Encoded bit error rate  \sa CiMmEncodedBER */
} CiMmSigQualityInfo;

/** \brief Network mode indication values */
/*-----------------5/5/2009 1:44PM------------------
 *  SCR #1401348:
 * --------------------------------------------------*/
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIMM_NETWORK_MODE
{
	CI_MM_NETWORK_MODE_GSM = 0,     /**< GSM */
	CI_MM_NETWORK_MODE_UMTS,        /**< UMTS */

	CI_MM_NUM_NETWORK_MODES
} _CiMmNetworkMode;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Network mode indication */
/*  SCR #1401348 */
/** \sa CIMM_NETWORK_MODE */
/** \remarks Common Data Section */
typedef UINT8 CiMmNetworkMode;
/**@}*/

/** \brief  Cell capability network mode enumeration */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIMM_NETWORK_MODE_REPORT
{
	CI_MM_NETWORK_MODE_REPORT_GSM = 0,                              /**< GSM only */
	CI_MM_NETWORK_MODE_REPORT_UMTS,                                 /**< 3G only */
	CI_MM_NETWORK_MODE_REPORT_UMTS_HSDPA,                           /**< 3G and HSDPA capabilities */
	CI_MM_NETWORK_MODE_REPORT_UMTS_HSUPA,                           /**< 3G and HSUPA capabilities */
	CI_MM_NETWORK_MODE_REPORT_UMTS_HSDPA_HSUPA,                     /**< 3G, HSDPA, and HSDPA capabilities */
	CI_MM_NETWORK_MODE_REPORT_GSM_EGPRS,                            /**< GSM, GPRS, and EGPRS capabilities */
	CI_MM_NETWORK_MODE_REPORT_GSM_GPRS,                             /**< GSM and GPRS capabilities */
	CI_MM_NUM_NETWORK_MODE_REPORTS
} _CiMmNetworkModeReport;

//ICAT EXPORTED ENUM
/** \brief  UMTS band values */
/** \remarks Common Data Section */

typedef enum CI_UMTS_BANDS_TYPE
{
	CI_BAND_1,      /**< UMTS Band1 */
	CI_BAND_2,      /**< UMTS Band2 */
	CI_BAND_3,      /**< UMTS Band3 */
	CI_BAND_4,      /**< UMTS Band4 */
	CI_BAND_5,      /**< UMTS Band5 */
	CI_BAND_6,      /**< UMTS Band6 */
	CI_BAND_7,      /**< UMTS Band7 */
	CI_BAND_8,      /**< UMTS Band8 */
	CI_BAND_9,      /**< UMTS Band9 */
	CI_BAND_GSM     /**< Band GSM */
}_CiUmtsBandsType;
/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief UMTS band values
 * \sa CI_UMTS_BANDS_TYPE */
/** \remarks Common Data Section */
typedef UINT8 CiUmtsBandsType;
/**@}*/

//ICAT EXPORTED ENUM
/** \brief  GSM band values */
/** \remarks Common Data Section */

typedef enum CI_GSM_BANDS_TYPE
{
	CI_GSM_BAND        = 0, /**< PGSM 900 (standard or primary) */
	CI_DCS_BAND        = 1, /**< DCS GSM 1800*/
	CI_PCS_BAND        = 2, /**< PCS GSM 1900*/
	CI_EGSM_BAND       = 3, /**< EGSM 900 (extended)*/
	CI_GSM_450_BAND    = 4, /**< GSM 450*/
	CI_GSM_480_BAND    = 5, /**< GSM 480*/
	CI_GSM_850_BAND    = 6, /**< GSM 850 */

	CI_NUM_BANDS,
#if defined (UPGRADE_3G)
	CI_UMTS_BAND       = 0xFE,      /**< UMTS */
#endif
	CI_INVALID_BAND    = 0xFF       /**< Invalid band */
}_CiGsmBandsType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief GSM band values
 * \sa CI_GSM_BANDS_TYPE */
/** \remarks Common Data Section */
typedef UINT8 CiGsmBandsType;
/**@}*/



//ICAT EXPORTED STRUCT
/**  \brief Current band: band values depend on access technology*/
/** \remarks Common Data Section */
typedef union CiMmCurrentBandTag
{
	CiGsmBandsType gsmBand;                 /**< access technology is GSM \sa CiGsmBandsType */
	CiGsmBandsType gsmCompactBand;          /**< Not used */
	CiUmtsBandsType umtsBand;               /**< access technology is UMTS \sa CiUmtsBandsType */
}CiMmCurrentBand;

//ICAT EXPORTED STRUCT
/** \brief  Current band */
/** \remarks Common Data Section */
typedef struct CiMmCurrentbandInfo_struct
{
	CiMmAccTechMode accessTechnology;       /**< Access technology \sa CiMmAccTechMode */
	CiMmCurrentBand currentBand;            /**< Current band \sa CiMmCurrentBandTag */
}
CiMmCurrentBandInfo;



//typedef CurrentModeBand CiMmCurrentModeBand;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief SCR #1401348: Network Mode indication
 * \sa CIMM_NETWORK_MODE_REPORT */
/** \remarks Common Data Section */
typedef UINT8 CiMmNetworkModeReport;
/**@}*/

/** \brief  NW mode indication support */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiMmNetworkHsMode_struct
{
	CiBoolean gprsActive;                   /**< TRUE - is active; FALSE - is not; \sa CCI API Ref Manual */
	CiBoolean egprsActive;                  /**< TRUE - is active; FALSE - is not; \sa CCI API Ref Manual */
	CiBoolean hsdpaActive;                  /**< TRUE - is active; FALSE - is not; \sa CCI API Ref Manual */
	CiBoolean hsupaActive;                  /**< TRUE - is active; FALSE - is not; \sa CCI API Ref Manual */
} CiMmNetworkHsMode;

/** \brief  Daylight Savings Time Indicator */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIMM_DSTIND
{
	CI_MM_DST_IND_NO_ADJUSTMENT = 0,
	CI_MM_DST_IND_PLUS_ONE_HOUR,
	CI_MM_DST_IND_PLUS_TWO_HOURS,

	CI_MM_NUM_DST_INDS
} _CiMmDstInd;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Daylight Savings Time Indicator
 * \sa CIMM_DSTIND */
/** \remarks Common Data Section */
typedef UINT8 CiMmDstInd;
/**@}*/

/** \brief  Universal time. See TS 23.040, Section 9.2.3.11 for time zone */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiMmUniTime_struct
{
	UINT8 year;                             /**< Year   [ 00..99 ] */
	UINT8 month;                            /**< Month  [ 1..12 ] */
	UINT8 day;                              /**< Day    [ 1..31 ] */
	UINT8 hour;                             /**< Hour   [ 0..59 ] */
	UINT8 minute;                           /**< Minute [ 0..59 ] */
	UINT8 second;                           /**< Second [ 0..59 ] */
	INT8 locTimeZone;                       /**< Local time zone */
} CiMmUniTime;

/** \brief  LSA localized service area identity */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiMmLsaIdentity_struct
{
	UINT8 length;                           /**< length */
	UINT8 data[CIMM_MAX_LSA_IDENTITY];      /**< data */
} CiMmLsaIdentity;

//Michal Bukai - add boolean parameters which indicate which information the NITZ include

/** \brief  NITZ information structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiMmNitzInfo_struct
{
	CiMmOperatorId longAlphaId;                                     /**< Long alphanumeric operator ID  \sa CiMmOperatorId_struct  */
	CiBoolean longAlphaIdPresent;                                   /**< TRUE - if long alphanumeric ID is present  \sa CCI API Ref Manual */
	CiMmOperatorId shortAlphaId;                                    /**< Short alphanumeric operator ID  \sa CiMmOperatorId_struct */
	CiBoolean shortAlphaIdPresent;                                  /**< TRUE - if short alphanumeric ID is present  \sa CCI API Ref Manual */
	CiMmUniTime uniTime;                                            /**< Universal time  \sa CiMmUniTime_struct */
	CiBoolean uniTimePresent;                                       /**< TRUE - if universal time is present  \sa CCI API Ref Manual */
	INT8 locTimeZone;                                               /**< local time zone */
	CiBoolean locTimeZonePresent;                                   /**< TRUE - if local time zone is present  \sa CCI API Ref Manual */
	CiMmDstInd dstInd;                                              /**< Daylight savings indicator  \sa CiMmDstInd */
	CiBoolean dstIndPresent;                                        /**< TRUE - if daylight saving indicator present  \sa CCI API Ref Manual */
	CiMmLsaIdentity lsaIdentity;                                    /**< LSA - localized service area identity  \sa CiMmLsaIdentity */
} CiMmNitzInfo;

/** \brief  Enumeration for different way preferred operator could be added */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIMM_ADDPREFOP_TYPES
{
	CI_MM_ADDPREFOP_FIRST_AVAILABLE = 0,
	CI_MM_ADDPREFOP_INSERT_AT_INDEX,

	CI_MM_NUM_ADD_PREFOP_TYPES
} _CiMmAddPrefOpType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Enumeration for different way preferred operator could be added
 * \sa CIMM_ADDPREFOP_TYPES */
/** \remarks Common Data Section */
typedef UINT8 CiMmAddPrefOpType;
/**@}*/

/*Michal Bukai - Selection of preferred PLMN list + CPLS - START */
//ICAT EXPORTED ENUM
/** \brief  Preferred PLMN list type values */
/** \remarks Common Data Section */
typedef enum CIMM_LIST_INDEX_TYPE {
	CI_MM_LIST_INDEX_USER_CONTROLLED_WACTSUCCESS = 0,       /**< User controlled PLMN selector with Access Technology EFPLMNwAcT. if not found in the SIM/UICC then select PLMN preferred list EFPLMNsel */
	CI_MM_LIST_INDEX_OPERATOR_CONTROLLED_WACT,              /**< Operator controlled PLMN selector with Access Technology  FOPLMNwAcT */
	CI_MM_LIST_INDEX_HPLMN_WACT,                            /**< HPLMN selector with Access Technology EFHPLMNwAcT */

	CI_MM_NUM_LIST_INDEX_TYPES

} _CiMmListIndexType;
/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Prefferred PLMN list type */
/** \sa CIMM_LIST_INDEX_TYPE */
/** \remarks Common Data Section */
typedef UINT8 CiMmListIndexType;
/**@}*/


/*Michal Bukai - Selection of preferred PLMN list + CPLS - END */

/* -------------------------- CC Primitives ------------------------------- */

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_GET_NUM_SUBSCRIBER_NUMBERS_REQ"> */
typedef CiEmptyPrim CiMmPrimGetNumSubscriberNumbersReq;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_GET_NUM_SUBSCRIBER_NUMBERS_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimGetNumSubscriberNumbersCnf_struct {
	CiMmResultCode Result;                  /**< Result code \sa CiMmResultCode */
	UINT8 NumMSISDN;                        /**< Number of entries in the MSISDN list  */
} CiMmPrimGetNumSubscriberNumbersCnf;


/* <INUSE> */
/** <paramref name="CI_MM_PRIM_GET_SUBSCRIBER_INFO_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimGetSubscriberInfoReq_struct {
	UINT8 Index;                    /**< MSISDN list entry number [1..number of MSISDN list entries] */
} CiMmPrimGetSubscriberInfoReq;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_GET_SUBSCRIBER_INFO_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimGetSubscriberInfoCnf_struct {
	CiMmResultCode Result;                          /**< Result code \sa CiMmResultCode */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_MM_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backward compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_MM_CONTIGUOUS
	CiMmSubscriberInfo  *Info;                      /**< Subscriber information \sa CiMmSubscriberInfo_struct */
#else /* CCI_MM_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
	/*Temporary pointer used to ensure the correct structure size for transmission
	   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8                         *tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */
	/* # Start Contiguous Code Section # */
	CiMmSubscriberInfo info;                        /**< Subscriber information \sa CiMmSubscriberInfo_struct */
	/* # End Contiguous Code Section # */
#endif  /* CCI_MM_CONTIGUOUS */
} CiMmPrimGetSubscriberInfoCnf;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_GET_SUPPORTED_REGRESULT_OPTIONS_REQ"> */
typedef CiEmptyPrim CiMmPrimGetSupportedRegResultOptionsReq;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_GET_SUPPORTED_REGRESULT_OPTIONS_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimGetSupportedRegResultOptionsCnf_struct {
	CiMmResultCode Result;                                                                                                  /**< Result code \sa CiMmResultCode */
	UINT8 NumOptions;                                                                                                       /**< Number of supported options */
	CiMmRegResultOption Option[ CIMM_NUM_REGRESULT_OPTIONS ];                                                               /**< Supported options  \sa CiMmRegResultOption*/
} CiMmPrimGetSupportedRegResultOptionsCnf;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_GET_REGRESULT_OPTION_REQ"> */
typedef CiEmptyPrim CiMmPrimGetRegResultOptionReq;
/** <paramref name="CI_MM_PRIM_GET_BANDIND_REQ"> */
typedef CiEmptyPrim CiMmPrimGetBandIndReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_MM_PRIM_GET_BANDIND_CNF"> */
typedef struct CiMmPrimGetBandIndCnf_struct {
	CiMmResultCode result;                  /**< Result code \sa CiMmResultCode */
	CiBoolean enableBandInd;                /**< Enable status  \sa CCI API Ref Manual */
	CiMmCurrentBandInfo currentBand;        /**< Current band \sa CiMmCurrentbandInfo_struct */

}CiMmPrimGetBandIndCnf;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_GET_REGRESULT_OPTION_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimGetRegResultOptionCnf_struct {
	CiMmResultCode Result;                          /**< Result code \sa CiMmResultCode */
	CiMmRegResultOption Option;                     /**< Reporting option  \sa CiMmRegResultOption */
} CiMmPrimGetRegResultOptionCnf;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_SET_REGRESULT_OPTION_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimSetRegResultOptionReq_struct {
	CiMmRegResultOption Option;             /**< Reporting option  \sa CiMmRegResultOption */
} CiMmPrimSetRegResultOptionReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_MM_PRIM_SET_BANDIND_REQ"> */
typedef struct CiMmPrimSetBandIndReq_struct {
	CiBoolean enableBandInd;              /**< Enable/Disable band indications  \sa CCI API Ref Manual */
}CiMmPrimSetBandIndReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_MM_PRIM_SET_BANDIND_CNF"> */
typedef struct CiMmPrimSetBandIndCnf_struct {
	CiMmResultCode result;             /**< Result code \sa CiMmResultCode */
}CiMmPrimSetBandIndCnf;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_SET_REGRESULT_OPTION_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimSetRegResultOptionCnf_struct {
	CiMmResultCode Result;                          /**< Result code \sa CiMmResultCode */
} CiMmPrimSetRegResultOptionCnf;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_REGRESULT_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimRegResultInd_struct {
	CiMmRegStatus RegStatus;                        /**< Registration status  \sa CiMmRegStatus */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_MM_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_MM_CONTIGUOUS
	CiMmCellInfo                        *Info;              /**< Current cell information \sa CiMmCellInfo_struct */
#else /* CCI_MM_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
	/*Temporary pointer used to ensure the correct structure size for transmission
	   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8                                 *tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */
	/* # Start Contiguous Code Section # */
	CiMmCellInfo info;                                      /**< Current cell information \sa CiMmCellInfo_struct */
	/* # End Contiguous Code Section # */
#endif  /* CCI_MM_CONTIGUOUS */
} CiMmPrimRegResultInd;
//ICAT EXPORTED STRUCT
/** <paramref name="CI_MM_PRIM_BANDIND_IND"> */
typedef struct CiMmPrimBandIndInd_struct
{
	CiMmCurrentBandInfo currentBand;  /**< Current band \sa CiMmCurrentbandInfo_struct */
}
CiMmPrimBandIndInd;
/* <INUSE> */
/** <paramref name="CI_MM_PRIM_GET_REGRESULT_INFO_REQ"> */
typedef CiEmptyPrim CiMmPrimGetRegResultInfoReq;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_GET_REGRESULT_INFO_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimGetRegResultInfoCnf_struct {
	CiMmResultCode Result;          /**< Result code \sa CiMmResultCode */
	CiMmRegStatus RegStatus;        /**< Registration status  \sa CiMmRegStatus */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_MM_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backward compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_MM_CONTIGUOUS
	CiMmCellInfo        *Info;                      /**< Current cell information \sa CiMmCellInfo_struct */
#else /* CCI_MM_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
	/*Temporary pointer used to ensure the correct structure size for transmission
	   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8                 *tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */
	/* # Start Contiguous Code Section # */
	CiMmCellInfo info;                              /**< Current cell information \sa CiMmCellInfo_struct */
	/* # End Contiguous Code Section # */
#endif  /* CCI_MM_CONTIGUOUS */
} CiMmPrimGetRegResultInfoCnf;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_GET_SUPPORTED_ID_FORMATS_REQ"> */
typedef CiEmptyPrim CiMmPrimGetSupportedIdFormatsReq;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_GET_SUPPORTED_ID_FORMATS_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimGetSupportedIdFormatsCnf_struct {
	CiMmResultCode Result;                                                                                  /**< Result code \sa CiMmResultCode */
	UINT8 NumFormats;                                                                                       /**< Number of supported formats  */
	CiMmNetOpIdFormat Format[ CIMM_NUM_NETOP_ID_FORMATS ];                                                  /**< Supported formats  \sa CiMmNetOpIdFormat */
} CiMmPrimGetSupportedIdFormatsCnf;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_GET_ID_FORMAT_REQ"> */
typedef CiEmptyPrim CiMmPrimGetIdFormatReq;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_GET_ID_FORMAT_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimGetIdFormatCnf_struct {
	CiMmResultCode Result;                          /**< Result code \sa CiMmResultCode */
	CiMmNetOpIdFormat Format;                       /**< Current format  \sa CiMmNetOpIdFormat */
} CiMmPrimGetIdFormatCnf;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_SET_ID_FORMAT_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimSetIdFormatReq_struct {
	CiMmNetOpIdFormat Format;       /**< Current format \sa CiMmNetOpIdFormat  */
} CiMmPrimSetIdFormatReq;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_SET_ID_FORMAT_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimSetIdFormatCnf_struct {
	CiMmResultCode Result;          /**< Result code \sa CiMmResultCode */
} CiMmPrimSetIdFormatCnf;

/* <INUSE> */
/**	CI_MM_PRIM_GET_NUM_NETWORK_OPERATORS_REQ - Requests the number of operators present in the network
 */
typedef CiEmptyPrim CiMmPrimGetNumNetworkOperatorsReq;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_GET_NUM_NETWORK_OPERATORS_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimGetNumNetworkOperatorsCnf_struct {
	CiMmResultCode Result;                  /**< Result code \sa CiMmResultCode */
	UINT8 NumOperators;                     /**< Number of operators present  */
} CiMmPrimGetNumNetworkOperatorsCnf;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_GET_NETWORK_OPERATOR_INFO_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimGetNetworkOperatorInfoReq_struct {
	UINT8 Index;                                            /**< Numeric index, specifying the network operator for which information is requested [1..number of operators present]  */
} CiMmPrimGetNetworkOperatorInfoReq;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_GET_NETWORK_OPERATOR_INFO_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimGetNetworkOperatorInfoCnf_struct {
	CiMmResultCode Result;                  /**< Result code \sa CiMmResultCode */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_MM_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_MM_CONTIGUOUS
	CiMmNetOpStatusInfo         *OpStatus;          /**< Network operator status information, if available \sa CiNetOpStatusInfo_struct */
#else /* CCI_MM_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
	/*Temporary pointer used to ensure the correct structure size for transmission
	   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8                                 *tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */
	/* # Start Contiguous Code Section # */
	CiMmNetOpStatusInfo opStatus;                   /**< Network operator status information, if available \sa CiNetOpStatusInfo_struct */
	/* # End Contiguous Code Section # */
#endif  /* CCI_MM_CONTIGUOUS */
} CiMmPrimGetNetworkOperatorInfoCnf;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_GET_NUM_PREFERRED_OPERATORS_REQ"> */
typedef CiEmptyPrim CiMmPrimGetNumPreferredOperatorsReq;

/* <INUSE> */
//ICAT EXPORTED STRUCT
/** <paramref name="CI_MM_PRIM_GET_NUM_PREFERRED_OPERATORS_CNF"> */
typedef struct CiMmPrimGetNumPreferredOperatorsCnf_struct {
	CiMmResultCode Result;                                  /**< Result code \sa CiMmResultCode */
	UINT8 NumPref;                                          /**< Number of entries in the list  */
} CiMmPrimGetNumPreferredOperatorsCnf;


/* <NOTINUSE> */
/** <paramref name="CI_MM_PRIM_GET_PREFERRED_OPERATOR_INFO_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimGetPreferredOperatorInfoReq_struct {
	UINT8 Index;                    /**< Not in use */
} CiMmPrimGetPreferredOperatorInfoReq;

/* <NOTINUSE> */
/** <paramref name="CI_MM_PRIM_GET_PREFERRED_OPERATOR_INFO_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimGetPreferredOperatorInfoCnf_struct {
	CiMmResultCode Result;                                  /**< Result code \sa CiMmResultCode */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_MM_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_MM_CONTIGUOUS
	CiMmNetOpIdInfo             *Info;                      /**< Network/operator ID information \sa CiNetOpIdInfo_struct */
#else /* CCI_MM_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
	/*Temporary pointer used to ensure the correct structure size for transmission
	   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8                                 *tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */
	/* # Start Contiguous Code Section # */
	CiMmNetOpIdInfo info;                                   /**< Network/operator ID information \sa CiNetOpIdInfo_struct */
	/* # End Contiguous Code Section # */
#endif  /* CCI_MM_CONTIGUOUS */
	CiMmAccTechMode AccTchMode;             /**< Not in use. Access Radio technology; default is GSM. \sa CiMmAccTechMode */
} CiMmPrimGetPreferredOperatorInfoCnf;


/* <INUSE> */
/** <paramref name="CI_MM_PRIM_ADD_PREFERRED_OPERATOR_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimAddPreferredOperatorReq_struct {
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_MM_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_MM_CONTIGUOUS
	CiMmNetOpIdInfo     *Info;                      /**< New network/operator ID information \sa CiNetOpIdInfo_struct  */
#else /* CCI_MM_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
	/*Temporary pointer used to ensure the correct structure size for transmission
	   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8                         *tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */
	/* # Start Contiguous Code Section # */
	CiMmNetOpIdInfo info;                           /**< New network/operator ID information \sa CiNetOpIdInfo_struct  */
	/* # End Contiguous Code Section # */
#endif  /* CCI_MM_CONTIGUOUS */
	CiMmAddPrefOpType addPrefOpType;        /**< \sa CiMmAddPrefOpType */
	UINT8 res1U8;                                                   /**< (padding) just in case */
	UINT16 index;                                           /**< Entry number to add  */
	CiMmAccTechMode AccTchMode;                             /**< ACT parameter for COLP command \sa CiMmAccTechMode */
} CiMmPrimAddPreferredOperatorReq;

/* <NOTINUSE> */
/** <paramref name="CI_MM_PRIM_ADD_PREFERRED_OPERATOR_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimAddPreferredOperatorCnf_struct {
	CiMmResultCode Result;                                  /**< Result code \sa CiMmResultCode */
	UINT8 NumPref;                                          /**< Not in use */
} CiMmPrimAddPreferredOperatorCnf;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_DELETE_PREFERRED_OPERATOR_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimDeletePreferredOperatorReq_struct {
	UINT8 Index;                    /**< Index (entry number) to delete */
} CiMmPrimDeletePreferredOperatorReq;

/* <NOTINUSE> */
/** <paramref name="CI_MM_PRIM_DELETE_PREFERRED_OPERATOR_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimDeletePreferredOperatorCnf_struct {
	CiMmResultCode Result;                                  /**< Result code \sa CiMmResultCode */
	UINT8 NumPref;                                          /**< Not in use */
} CiMmPrimDeletePreferredOperatorCnf;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_GET_CURRENT_OPERATOR_INFO_REQ"> */
typedef CiEmptyPrim CiMmPrimGetCurrentOperatorInfoReq;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_GET_CURRENT_OPERATOR_INFO_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimGetCurrentOperatorInfoCnf_struct {
	CiMmResultCode Result;                                  /**< Result code \sa CiMmResultCode */
	CiMmRegMode RegMode;                                    /**< Current registration mode \sa CiMmRegMode */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_MM_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_MM_CONTIGUOUS
	CiMmNetOpIdInfo             *Info;                      /**< Current network/operator ID information \sa CiNetOpIdInfo_struct */
#else /* CCI_MM_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
	/*Temporary pointer used to ensure the correct structure size for transmission
	   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8                           *tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */
	/* # Start Contiguous Code Section # */
	CiMmNetOpIdInfo info;                                   /**< Current network/operator ID information \sa CiNetOpIdInfo_struct */
	/* # End Contiguous Code Section # */
#endif  /* CCI_MM_CONTIGUOUS */
	CiMmAccTechMode AccTchMode;                     /**< Access radio technology; default is GSM \sa CiMmAccTechMode */
} CiMmPrimGetCurrentOperatorInfoCnf;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_AUTO_REGISTER_REQ"> */
typedef CiEmptyPrim CiMmPrimAutoRegisterReq;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_AUTO_REGISTER_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimAutoRegisterCnf_struct {
	CiMmResultCode Result;          /**< Result code \sa CiMmResultCode */
} CiMmPrimAutoRegisterCnf;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_MANUAL_REGISTER_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimManualRegisterReq_struct {
	CiBoolean AutoFallback;                                 /**< TRUE - Fallback to automatic registration; FALSE - No fallback to automatic registration  \sa CCI API Ref Manual */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_MM_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_MM_CONTIGUOUS
	CiMmNetOpIdInfo             *Info;                              /**< Network operator identification information \sa CiNetOpIdInfo_struct */
#else /* CCI_MM_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
	/*Temporary pointer used to ensure the correct structure size for transmission
	   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8                                 *tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */
	/* # Start Contiguous Code Section # */
	CiMmNetOpIdInfo info;                                           /**< Network operator identification information \sa CiNetOpIdInfo_struct */
	/* # End Contiguous Code Section # */
#endif  /* CCI_MM_CONTIGUOUS */
	CiMmAccTechMode AccTchMode;                     /**< Access radio technology; default is GSM \sa CiMmAccTechMode */
} CiMmPrimManualRegisterReq;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_MANUAL_REGISTER_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimManualRegisterCnf_struct {
	CiMmResultCode Result;                  /**< Result code \sa CiMmResultCode */
} CiMmPrimManualRegisterCnf;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_DEREGISTER_REQ"> */
typedef CiEmptyPrim CiMmPrimDeregisterReq;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_DEREGISTER_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimDeregisterCnf_struct {
	CiMmResultCode Result;                  /**< Result code \sa CiMmResultCode */
} CiMmPrimDeregisterCnf;

typedef CiEmptyPrim CiMmPrimGetSigQualityIndConfigReq;

//ICAT EXPORTED STRUCT
typedef struct CiMmPrimGetSigQualityIndConfigCnf_struct {
	CiMmResultCode Result;
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_MM_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_MM_CONTIGUOUS
	CiMmSigQualityConfig *Config;
#else   /* CCI_MM_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
	/*Temporary pointer used to ensure the correct structure size for transmission
	   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8* tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */
	/* # Start Contiguous Code Section # */
	CiMmSigQualityConfig config;
	/* # End Contiguous Code Section # */
#endif  /* CCI_MM_CONTIGUOUS */
} CiMmPrimGetSigQualityIndConfigCnf;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_SET_SIGQUALITY_IND_CONFIG_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimSetSigQualityIndConfigReq_struct {
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_MM_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_MM_CONTIGUOUS
	CiMmSigQualityConfig        *Config;                            /**< Signal quality configuration  \sa CiMmSigQualityConfig_struct */
#else /* CCI_MM_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
	/*Temporary pointer used to ensure the correct structure size for transmission
	   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8                                 *tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */
	/* # Start Contiguous Code Section # */
	CiMmSigQualityConfig config;                            /**< Signal quality configuration  \sa CiMmSigQualityConfig_struct */
	/* # End Contiguous Code Section # */
#endif  /* CCI_MM_CONTIGUOUS */
} CiMmPrimSetSigQualityIndConfigReq;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_SET_SIGQUALITY_IND_CONFIG_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimSetSigQualityIndConfigCnf_struct {
	CiMmResultCode Result;                  /**< Result code \sa CiMmResultCode */
} CiMmPrimSetSigQualityIndConfigCnf;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_SIGQUALITY_INFO_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimSigQualityInfoInd_struct {
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_MM_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_MM_CONTIGUOUS
	CiMmSigQualityInfo  *Info;                              /**< Signal quality information \sa CiMmSigQualityInfo_struct  */
#else /* CCI_MM_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
	/*Temporary pointer used to ensure the correct structure size for transmission
	   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8                         * tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */
	/* # Start Contiguous Code Section # */
	CiMmSigQualityInfo info;                                        /**< Signal quality information \sa CiMmSigQualityInfo_struct  */
	/* # End Contiguous Code Section # */
#endif  /* CCI_MM_CONTIGUOUS */
} CiMmPrimSigQualityInfoInd;

/* SCR #1401348-- START */

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_ENABLE_NETWORK_MODE_IND_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimEnableNetworkModeIndReq_struct
{
	CiBoolean enable;       /**< TRUE - Enable network mode indication; FALSE - Disable network mode indication (default) \sa CCI API Ref Manual */
} CiMmPrimEnableNetworkModeIndReq;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_ENABLE_NETWORK_MODE_IND_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimEnableNetworkModeIndCnf_struct {
	CiMmResultCode Result;          /**< Result code \sa CiMmResultCode */
} CiMmPrimEnableNetworkModeIndCnf;


//Michal Bukai - NW Mode Indication support



/* <INUSE> */
/** <paramref name="CI_MM_PRIM_NETWORK_MODE_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimNetworkModeInd_struct
{
/* Michal Bukai enter Vadim modification for NetworkModeIndication */
#if 0
	CiMmNetworkMode mode;
#endif
	CiMmNetworkModeReport mode;                                             /**< Cell capabilities network mode \sa CiMmNetworkModeReport */
	CiBoolean gprsActive;                                                   /**< Active high speed service: gprsActive (1 - gprs is active, 0 - gprs is inactive) \sa CCI API Ref Manual */
	CiBoolean egprsActive;                                                  /**< Active high speed service: egprsActive (1 - egprs is active, 0 - egprs is inactive) \sa CCI API Ref Manual */
	CiBoolean hsdpaActive;                                                  /**< Active high speed service: hsdpaActive (1 - hsdpa is active, 0 - hsdpa is inactive) \sa CCI API Ref Manual */
	CiBoolean hsupaActive;                                                  /**< Active high speed service: hsupaActive (1 - hsupa is active, 0 - hsupa is inactive) \sa CCI API Ref Manual */

} CiMmPrimNetworkModeInd;

/* SCR #1401348-- END */

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_GET_NITZ_INFO_REQ"> */
typedef CiEmptyPrim CiMmPrimGetNitzInfoReq;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_GET_NITZ_INFO_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimGetNitzInfoCnf_struct
{
	CiMmResultCode Result;                                          /**< Result code \sa CiMmResultCode */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_MM_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_MM_CONTIGUOUS
	CiMmNitzInfo                          *Info;                    /**< NITZ information \sa CiMmNitzInfo_struct */
#else /* CCI_MM_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
	/*Temporary pointer used to ensure the correct structure size for transmission
	   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8                                 *tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */
	/* # Start Contiguous Code Section # */
	CiMmNitzInfo info;                                              /**< NITZ information \sa CiMmNitzInfo_struct */
	/* # End Contiguous Code Section # */

#endif  /* CCI_MM_CONTIGUOUS */
} CiMmPrimGetNitzInfoCnf;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_NITZ_INFO_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimNitzInfoInd_struct
{
#ifndef CCI_MM_CONTIGUOUS
	CiMmNitzInfo          *Info;                                    /**< NITZ information \sa CiMmNitzInfo_struct */
#else /* CCI_MM_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
	/*Temporary pointer used to ensure the correct structure size for transmission
	   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8                 *tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */
	/* # Start Contiguous Code Section # */
	CiMmNitzInfo info;                                              /**< NITZ information \sa CiMmNitzInfo_struct */
	/* # End Contiguous Code Section # */

#endif  /* CCI_MM_CONTIGUOUS */
} CiMmPrimNitzInfoInd;

/* <INUSE> */
/** <paramref name="CI_MM_PRIM_CIPHERING_STATUS_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimCipheringStatusInd_struct {
	CiBoolean CsCipheringOn;                        /**< TRUE if ON, FALSE if OFF; \sa CCI API Ref Manual */
	CiBoolean PsCipheringOn;                        /**< TRUE if ON, FALSE if OFF; \sa CCI API Ref Manual */
	CiBoolean CipheringIndicatorOn;                 /**< TRUE if required, FALSE if not required; \sa CCI API Ref Manual */

} CiMmPrimCipheringStatusInd;

/*Tal Porat/Michal Bukai - Network Selection*/
#ifdef AT_CUSTOMER_HTC
/** \brief MM Registartion/Authentication Reject type values */
/** \remarks Common Data Section */
typedef enum CIMM_REJECT_TYPE{
    CI_MM_LUP_REJECT = 0,      /**< Location Update Reject */
    CI_MM_AUTH_FAIL,           /**< MM Authentication fail */
    CI_MM_CS_AUTH_RJECT,       /**< NW CS Auth reject */

    CI_MM_NUM_REJECT_TYPE

} _CiMmRejectType;

typedef UINT8 CiMmRejectType;

/** \brief GMM Registartion/Authentication Reject type values */
/** \remarks Common Data Section */
typedef enum CIGMM_REJECT_TYPE{
    CI_GMM_GPRS_ATTACH_REJECT = 0,  /**< GPRS Attach/Combined Attach Reject */
    CI_GMM_RAU_REJECT,              /**< RAU/Combined RAU Reject */
    CI_GMM_PS_SECURITY_FAIL,        /**< PS security fail */
    CI_GMM_PS_AUTH_RJECT,           /**< NW PS Auth reject */

    CI_GMM_NUM_REJECT_TYPE

} _CiGmmRejectType;

typedef UINT8 CiGmmRejectType;

/** <paramref name="CI_MM_PRIM_AIR_INTERFACE_REJECT_CAUSE_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimAirInterfaceRejectCauseInd_struct
{
       CiMmRejectType  mmRejectType;   /**< MM Reject type  \sa CiMmRejectType */
	CiMmCause		mmCause;		/**< MM state  \sa CiMmCause */
       CiGmmRejectType gmmRejectType;  /**< GMM Reject type  \sa CiGmmRejectType */
	CiGmmCause		gmmCause; 		/**< GMM state  \sa CiGmmCause */
} CiMmPrimAirInterfaceRejectCauseInd;
#else
/** <paramref name="CI_MM_PRIM_AIR_INTERFACE_REJECT_CAUSE_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimAirInterfaceRejectCauseInd_struct
{
	CiMmCause mmCause;                              /**< MM state  \sa CiMmCause */
	CiGmmCause gmmCause;                            /**< GMM state  \sa CiGmmCause */
} CiMmPrimAirInterfaceRejectCauseInd;
#endif
/*Michal Bukai - Selection of preferred PLMN list +CPLS - START */
//ICAT EXPORTED STRUCT
/** <paramref name="CI_MM_PRIM_SELECT_PREFERRED_PLMN_LIST_REQ"> */
typedef struct CiMmPrimSelectPreferredPlmnListReq_struct {
	CiMmListIndexType ListIndex;    /**< Preferred PLMN list type \sa CiMmListIndexType*/
} CiMmPrimSelectPreferredPlmnListReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_MM_PRIM_SELECT_PREFERRED_PLMN_LIST_CNF"> */
typedef struct CiMmPrimSelectPreferredPlmnListCnf_struct {
	CiMmResultCode Result;      /**< Result code \sa CiMmResultCode */
} CiMmPrimSelectPreferredPlmnListCnf;
/** <paramref name="CI_MM_PRIM_GET_PREFERRED_PLMN_LIST_REQ"> */
typedef CiEmptyPrim CiMmPrimGetPreferredPlmnListReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_MM_PRIM_GET_PREFERRED_PLMN_LIST_CNF"> */
typedef struct CiMmPrimGetPreferredPlmnListCnf_struct {
	CiMmResultCode Result;          /**< Result code \sa CiMmResultCode */
	CiMmListIndexType ListIndex;    /**< Preferred PLMN list type \sa CiMmListIndexType*/
} CiMmPrimGetPreferredPlmnListCnf;
/*Michal Bukai - Selection of preferred PLMN list +CPLS - END */

//ENS CSP start
/* ********************************************************************************************* *
 * CiMmPrimServiceRestrictionsInd
 *
 * Notification of allowing/disallowing Manual PLMN selection option in the user menu.
 *
 * The protocol stack sends an indication regarding whether manual PLMN selection appearance in the user's menu.
 * the decision regarding allowing / disallowing the appearance of this option in the user's menu is done by SIM
 * (could be sent by OTA).
 */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimServiceRestrictionsInd_struct
{
	CiBoolean manualPlmnSelectionAllowed;
} CiMmPrimServiceRestrictionsInd;

typedef enum CIMM_CELL_LOCK_MODE
{
    CIMM_CELL_LOCK_MODE_NONE  = 0, /**< Cell/Freq Lock disabled */
    CIMM_CELL_LOCK_MODE_LOCKFREQ  = 1, /**< Freq Lock enabled */
    CIMM_CELL_LOCK_MODE_LOCKCELL  = 2, /**< Cell Lock enabled */

    CIMM_NUM_CELLLOCK_MODES,
}_CiMmCellLockMode;
typedef UINT8 CiMmCellLockMode;
typedef struct CiMmPrimCellLockReq_struct{
    CiMmCellLockMode  mode;  /**< Cell lock mode \sa CiMmCellLockMode */

    UINT16          arfcn;              /**< Absolute radio frequency channel number; GSM number 0-1023, TD number 10054-10121 and 9404-9596 */
    UINT8           cellParameterId;    /**< Cell parameter ID This parameter if valid for 3G cells only 0-127*/
} CiMmPrimCellLockReq;
typedef struct CiMmPrimCellLockCnf_struct{
    CiMmResultCode  	result;			/**< Result code \sa CiMmResultCode */
} CiMmPrimCellLockCnf;

//ICAT EXPORTED STRUCT
typedef struct CiMmPrimCellLockInd_struct{
    CiMmCellLockMode  mode;  /**< Cell lock mode \sa CiMmCellLockMode */

    UINT16          arfcn;              /**< Absolute radio frequency channel number; GSM number 0-1023, TD number 10054-10121 and 9404-9596 */
    UINT8           cellParameterId;    /**< Cell parameter ID This parameter if valid for 3G cells only 0-127*/
} CiMmPrimCellLockInd;

/*Michal Buaki - HOME ZONE support*/
/**********************************/
/* <NOT IN INUSE> */
/** <paramref name="CI_MM_PRIM_HOMEZONE_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiMmPrimHomeZoneInd_struct{
	CiBoolean		ZoneInd;		/**< TRUE - display HomeZone/CitiZone Indication for the zone specified in "ZoneId" field, FALSE -remove HomeZone/CitiZone indication \sa CCI API Ref Manual */
	UINT8			ZoneId;			/**< Zone ID*/
	CiBoolean		IsCityZone;		/**< TRUE - detected zone is CityZone, FALSE - detected zone is HomeZone \sa CCI API Ref Manual */
	CiBoolean		ZoneTagPreset;	/**< TRUE - Zone TAG is included, FALSE - Zone TAG is not included \sa CCI API Ref Manual */
	CiString 		ZoneTag;		/**< 13-character string coded in the short message alphabet given in GSM 03.38 with bit 8 set to Zero. 0xff indicates end of string \sa CCI API Ref Manual */
} CiMmPrimHomeZoneInd;

/* ADD NEW COMMON PRIMITIVES DEFINITIONS HERE */

#ifdef CI_CUSTOM_EXTENSION
/* it is assumed that only one customized set of extension primitives is
 * to be considered at one time. The selection of the particular customized
 * set is done in the 'ci_mm_cust.h' based on compile flags.
 *
 * Note: if no customer extension primitives are defined for this service group
 * the CI_MM_NUM_CUST_PRIM is set to 0 in the "ci_mm_cust.h" file.
 */
#include "ci_mm_cust.h"

#define CI_MM_NUM_PRIM ( CI_MM_NUM_COMMON_PRIM + CI_MM_NUM_CUST_PRIM )

#else

/* if no customer extension is supported, only the default common set is considered */
#define CI_MM_NUM_PRIM CI_MM_NUM_COMMON_PRIM

#endif /* CI_CUSTOM_EXTENSION */

#ifdef __cplusplus
}
#endif  //__cplusplus

#endif  /* _CI_MM_H_ */


/*                      end of ci_mm.h
   --------------------------------------------------------------------------- */

