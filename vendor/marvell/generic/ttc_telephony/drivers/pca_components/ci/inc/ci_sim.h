/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/* ===========================================================================
   File        : ci_sim.h
   Description : Data types file for the SIM Service Group
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

#if !defined(_CI_SIM_H_)
#define _CI_SIM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ci_api_types.h"
/** \addtogroup  SpecificSGRelated
 * @{ */

#define CI_SIM_VER_MAJOR 3
#define CI_SIM_VER_MINOR 0

/* ----------------------------------------------------------------------------- */

/* CI_SIM Primitive ID definitions */
//ICAT EXPORTED ENUM
/** Summary of primitives */
typedef enum CI_SIM_PRIM
{
	CI_SIM_PRIM_EXECCMD_REQ = 1,                                                    /**< \brief Execute a SIM command \details   */
	CI_SIM_PRIM_EXECCMD_CNF,                                                        /**< \brief Confirm execution of a SIM command \details   */
	CI_SIM_PRIM_DEVICE_IND,                                                         /**< \brief Indication of current SIM status changed \details   */
	CI_SIM_PRIM_PERSONALIZEME_REQ,                                                  /**< \brief Activate/Deactivate/Disable/Query ME personalization \details ME is locked with certain types of SIMs through the personalization activation process  */
	CI_SIM_PRIM_PERSONALIZEME_CNF,                                                  /**< \brief Confirmation to personalization request \details   */
	CI_SIM_PRIM_OPERCHV_REQ,                                                        /**< \brief Verify/Change/Disable/Enable/Unblock/Query CHVs \details   */
	CI_SIM_PRIM_OPERCHV_CNF,                                                        /**< \brief Confirmation to CHV operation request \details   */
	CI_SIM_PRIM_DOWNLOADPROFILE_REQ,                                /**< \brief Downloads the profile that shows ME's capabilities relevant to SIM Application Toolkit functionality to SIM \details The functionality of this primitive is equivalent to use SIM command "TERMINAL PROFILE" in CI_SIM_PRIM_EXECCMD_REQ primitive.
									 *  This primitive saves upper layer effort to build a header for the Terminal Profile SIM command.
									 *  If the pProfile pointer is NULL for this request, the Communications Interface will assume that the Application Layer does not support
									 *  SIM Application Toolkit operations  */
	CI_SIM_PRIM_DOWNLOADPROFILE_CNF,                                                /**< \brief Confirmation to terminal profile download \details   */
	CI_SIM_PRIM_ENDATSESSION_IND,                                                   /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_SIM_PRIM_PROACTIVE_CMD_IND,                                                  /**< \brief Indication of a SIM AT proactive command to be executed \details If this primitive is supported, upper layer need not and should not send FETCH command in CI_SIM_PRIM_EXECCMD_REQ to achieve  proactive command from SIM. Enabled by CI_SIM_PRIM_ENABLE_SIMAT_INDS_REQ request.  */
	CI_SIM_PRIM_PROACTIVE_CMD_RSP,                                                  /**< \brief Execution response to the SIM AT proactive command, i.e. a TERMINAL RESPONSE command \details   */
	CI_SIM_PRIM_ENVELOPE_CMD_REQ,                                                   /**< \brief Executes an ENVELOPE command \details   */
	CI_SIM_PRIM_ENVELOPE_CMD_CNF,                                                   /**< \brief Confirms execution of an ENVELOPE command \details   */
	CI_SIM_PRIM_GET_SUBSCRIBER_ID_REQ,                                              /**< \brief Requests subscriber id \details   */
	CI_SIM_PRIM_GET_SUBSCRIBER_ID_CNF,                                              /**< \brief Confirmation to get subscriber id request \details   */
	CI_SIM_PRIM_GET_PIN_STATE_REQ,                                                  /**< \brief Requests current PIN state \details   */
	CI_SIM_PRIM_GET_PIN_STATE_CNF,                                                  /**< \brief Reports current PIN state \details   */
	CI_SIM_PRIM_GET_TERMINALPROFILE_REQ,                                            /**< \brief Requests the SIMAT terminal profile set by the client using \details   */
	CI_SIM_PRIM_GET_TERMINALPROFILE_CNF,                                            /**< \brief Reports that SIMAT terminal profile set by the client \details   */
	CI_SIM_PRIM_ENABLE_SIMAT_INDS_REQ,                                              /**< \brief Enable/disable reporting SIMAT related indications such as proactive SIM command indication, SIMAT session ended indication. \details   */
	CI_SIM_PRIM_ENABLE_SIMAT_INDS_CNF,                                              /**< \brief Confirmation to request to enable/disable SIMAT related indications \details   */
	CI_SIM_PRIM_LOCK_FACILITY_REQ,                                                  /**< \brief Lock/unlock/query a SIM-related ME facility \details   */
	CI_SIM_PRIM_LOCK_FACILITY_CNF,                                                  /**< \brief Confirms a request to Lock/unlock/query a SIM-related ME facility \details   */
	CI_SIM_PRIM_GET_FACILITY_CAP_REQ,                                               /**< \brief Requests the bitmask of supported SIM-related facility codes \details   */
	CI_SIM_PRIM_GET_FACILITY_CAP_CNF,                                               /**< \brief Reports the bitmask of supported SIM-related facility codes \details   */
	CI_SIM_PRIM_GET_SIMAT_NOTIFY_CAP_REQ,                                           /**< \brief Requests SIM Application Toolkit (SIMAT) Notification Capability information. \details   */
	CI_SIM_PRIM_GET_SIMAT_NOTIFY_CAP_CNF,                                           /**< \brief Reports SIM Application Toolkit (SIMAT) Notification Capability information \details   */
	CI_SIM_PRIM_GET_CALL_SETUP_ACK_IND,                                             /**< \brief Indicates that the SIM Application Toolkit (SIMAT) has initiated an outgoing CALL SETUP operation, and requests confirmation/acknowledgment from the Mobile User \details The Application returns the required acknowledgment in a CI_SIM_PRIM_GET_CALL_SETUP_ACK_RSP response  */
	CI_SIM_PRIM_GET_CALL_SETUP_ACK_RSP,                     /**< \brief Returns an acknowledgment from the Mobile User for an outgoing CALL SETUP indication from the SIM Application Toolkit (SIMAT). \details The Mobile User may accept (allow) or reject (disallow) the SIMAT initiated CALL SETUP operation.
								 *     If the User allows the CALL SETUP, it will proceed. If the User disallows the CALL SETUP, it will be aborted.
								 *     If the CALL SETUP is allowed to proceed, the MO Call Progression will be managed by the normal Call Control procedures. See the
								 *     CI CC Service Group API definition for more information.  */

	/* Service Provider Name */
	CI_SIM_PRIM_GET_SERVICE_PROVIDER_NAME_REQ,      /**< \brief Gets the Service Provider Name, as stored on SIM or USIM \details Requires no PIN status to read this information  */
	CI_SIM_PRIM_GET_SERVICE_PROVIDER_NAME_CNF,      /**< \brief Returns the Service Provider Name, as stored on SIM or USIM \details The Service Provider Name is coded as 7-bit GSM characters, with the most-significant bit of each character set to zero.
							 *  The Service Provider Name pointer is NULL if the result code indicates an error.  */

	/* Message Waiting Information */
	CI_SIM_PRIM_GET_MESSAGE_WAITING_INFO_REQ,               /**< \brief Gets the Message Waiting information stored on SIM or USIM \details Requires PIN status to read this information  */
	CI_SIM_PRIM_GET_MESSAGE_WAITING_INFO_CNF,               /**< \brief Returns the Message Waiting information stored on SIM or USIM \details If the result code indicates an error, the Message waiting Status information is not useful.
								 *  There is a difference between the message categories defined for 2G and 3G SIM storage. This will be rationalized by the CCI implementation.  */
	CI_SIM_PRIM_SET_MESSAGE_WAITING_INFO_REQ,               /**< \brief Updates the Message Waiting information on SIM or USIM. \details Requires PIN status to write this information. There is a difference between the message categories defined for 2G and 3G SIM storage. This will be rationalized by the CCI implementation.  */
	CI_SIM_PRIM_SET_MESSAGE_WAITING_INFO_CNF,               /**< \brief Confirms a request to set the Message Waiting information on SIM or USIM \details   */

	/* SIM Service Table */
	CI_SIM_PRIM_GET_SIM_SERVICE_TABLE_REQ,                          /**< \brief Gets the SIM Service Table from SIM or USIM  \details Requires PIN status to read this information. If CPHS features are not supported by the handset, this information is unavailable.  */
	CI_SIM_PRIM_GET_SIM_SERVICE_TABLE_CNF,                          /**< \brief Returns the SIM Service Table (if available) from SIM or USIM \details If CPHS features are not supported by the handset, this information is unavailable.  */

	/* CPHS Customer Service Profile */
	CI_SIM_PRIM_GET_CUSTOMER_SERVICE_PROFILE_REQ,   /**< \brief Gets the CPHS Customer Service Profile from SIM or USIM. \details Requires PIN status to read this information. If CPHS features are not supported by the handset, this information is unavailable. */
	CI_SIM_PRIM_GET_CUSTOMER_SERVICE_PROFILE_CNF,   /**< \brief Returns the CPHS Customer Service Profile (if available) from SIM or USIM. \details If CPHS features are not supported by the handset, this information is unavailable.  */

	/* Display Alpha and Icon Identifiers */
	CI_SIM_PRIM_SIMAT_DISPLAY_INFO_IND,                             /**< \brief Returns the CPHS Customer Service Profile (if available) from SIM or USIM. \details   */

	/* Default Language */
	CI_SIM_PRIM_GET_DEFAULT_LANGUAGE_REQ,           /**< \brief Requests the default language stored on the SIM/USIM card \details   */
	CI_SIM_PRIM_GET_DEFAULT_LANGUAGE_CNF,                   /**< \brief Returns the first entry in the language preference SIM/USIM file EF_LP. \details Extract from ETSI TS 102.221: "the language code is a pair of alpha-numeric characters, as defined in ISO 639 [30].
								 *  Each alpha-numeric character shall be coded on one byte using the SMS default 7-bit coded alphabet as defined in TS 23.038
								 *  ("Man-machine Interface (MMI) of the User Equipment" ,revision 3.4.0 ,Doc Number 3GPP TS 22.030)
								 *  with bit 8 set to 0.". 'FF FF' means undefined default language  */

	/* Generic SIM commands */
	CI_SIM_PRIM_GENERIC_CMD_REQ,                                    /**< \brief Request to send a generic command to the SIM/USIM card
									 * \details The request signature reflects the structure of a SIM application protocol data unit (APDU),
									 * as per ETSI 102.221. Exeption is the 'class of instruction' element that is transparent for the client application.
									 * Note that updating a file using this command will only update the file on the SIM it will not trigger REFRESH of the ME memory. */
	CI_SIM_PRIM_GENERIC_CMD_CNF,                                    /**< \brief Returns the response from SIM or USIM in reply to a previous generic command \details This confirmation is similar in functionality to 'CiSimPrimExecCmdCnf  */

	/* Indication of card type, status and PIN state */
	CI_SIM_PRIM_CARD_IND,                                                   /**< \brief Notifies the client application about a change in the PIN status of the SIM/USIM card \details This indication is sent each time CI_SIM_PRIM_DEVICE_IND is sent.  */

	CI_SIM_PRIM_IS_EMERGENCY_NUMBER_REQ,                                    /**< \brief Request to determine if the specified dial number is an emergency call code \details   */
	CI_SIM_PRIM_IS_EMERGENCY_NUMBER_CNF,                                    /**< \brief Returns the indication that the specified number is or is not an emergency call code \details If SIM card is present, the specified number is searched in the EF_ECC SIM card file. If SIM card is not present, the specified number is searched in a default table of possible Emergenacy call codes, as per TS 22.101. */

	CI_SIM_PRIM_SIM_OWNED_IND,                                              /**< \brief Returns the indication that the SIM is owned or not owned \details This indication is sent each time a SIM-OK notification is received
										 * from the protocol stack indicating that the SIM card can be accessed
										 * and providing the current security information about the SIM.
										 * The CiSimPrimSimOwnedInd specifies the SIM is owned or not owned
										 * based on the fact that IMSI has NOT changed since the last SIM-OK notification  */
	CI_SIM_PRIM_SIM_CHANGED_IND,                                    /**< \brief Returns the indication the IMSI on the current SIM card has changed or not \details  This indication is sent each time a SIM-OK notification is received
									 * from the protocol stack indicating that the SIM card can be accessed
									 * and providing the current information about the SIM.
									 * The CiSimPrimSimChangedInd specifies if the IMSI has changed since
									 * the last SIM-OK notification. */
	CI_SIM_PRIM_DEVICE_STATUS_REQ,                                          /**< \brief Request SIM status \details   */
	CI_SIM_PRIM_DEVICE_STATUS_CNF,                                          /**< \brief Returns current SIM status \details   */
	CI_SIM_PRIM_READ_MEP_CODES_REQ,                                         /**< \brief   Read the MEP codes for a specified category. This operation does not require password.  \details */
	CI_SIM_PRIM_READ_MEP_CODES_CNF,                                         /**< \brief   Confirmation to read MEP code group request. \details */
	CI_SIM_PRIM_UDP_LOCK_REQ,                                               /**< \brief   Activate/Deactivate/ Query UDP lock.  Note that an operation can be done on one category at a time. Password is required for unlock operation. \details */
	CI_SIM_PRIM_UDP_LOCK_CNF,                                               /**< \brief   Confirmation to operation on ME personalization. \details */
	CI_SIM_PRIM_UDP_CHANGE_PASSWORD_REQ,                                    /**< \brief   Set a new password for UDP lock. \details */
	CI_SIM_PRIM_UDP_CHANGE_PASSWORD_CNF,                                    /**< \brief   Confirmation to UDP_CHANGE_PASSWORD request. \details */
	CI_SIM_PRIM_UDP_ASL_REQ,                                                /**< \brief  Manipulates the UDP authorized SIM list. \details */
	CI_SIM_PRIM_UDP_ASL_CNF,                                                /**< \brief  Confirmation to CI_SIM_PRIM_UDP_ASL_REQ.  \details */
/**< Michal Bukai - Virtual SIM support - START */
	CI_SIM_PRIM_SET_VSIM_REQ,
	CI_SIM_PRIM_SET_VSIM_CNF,
	CI_SIM_PRIM_GET_VSIM_REQ,
	CI_SIM_PRIM_GET_VSIM_CNF,
/**< Michal Bukai - Virtual SIM support - END */
/*Michal Bukai - OTA support for AT&T - START*/
	CI_SIM_PRIM_CHECK_MMI_STATE_IND,        /**< \brief  Indicates a request to check MMI state  \details */
	CI_SIM_PRIM_CHECK_MMI_STATE_RSP,        /**< \brief  Responds with the current MMI state  \details */
/*Michal Bukai - OTA support for AT&T - END*/
/*Michal Bukai - additional SIMAT primitives - START*/
/*new add five IND */
	CI_SIM_PRIM_SIMAT_CC_STATUS_IND,					/**< \brief Indicates the SIM Application Toolkit (SIMAT) call control status response
														* \details If call control service in SIMAT is activated, all dialled digit strings, supplementary service control strings and USSD strings are passed to the UICC before the call setup request,
														* the supplementary service operation or the USSD operation is sent to the network.
														* The SIMAT has the ability to allow, bar or modify the request.
														* In addition SIMAT has the ability to replace the request by another operation, for instance call request may be replaced by SS or USSD operation.
														*/
	CI_SIM_PRIM_SIMAT_SEND_CALL_SETUP_RSP_IND,		/**< \brief Indicates the response sent to SIM Application Toolkit (SIMAT) after call setup. \details */
	CI_SIM_PRIM_SIMAT_SEND_SS_USSD_RSP_IND, 			/**< \brief Indicates the response sent to SIM Application Toolkit (SIMAT) after SS or USSD operation. \details */
	CI_SIM_PRIM_SIMAT_SM_CONTROL_STATUS_IND, 			/**< \brief Indicates the SIM Application Toolkit (SIMAT) short message control status response.
														\details If SM control service in SIMAT is activated, all MO short messages are passed to the UICC before the short message is sent to the network.
														* The SIMAT has the ability to allow, bar or modify the destination address.
														*/
	CI_SIM_PRIM_SIMAT_SEND_SM_RSP_IND,				/**< \brief Indicates the response sent to SIM Application Toolkit (SIMAT) after SM operation. \details */
/*Michal Bukai - additional SIMAT primitives - END*/

	/* ADD NEW COMMON PRIMITIVES HERE, BEFORE 'CI_SIM_PRIM_LAST_COMMON_PRIM' */


	/* END OF COMMON PRIMITIVES LIST */
	CI_SIM_PRIM_LAST_COMMON_PRIM

	/* the  customer specific extension primitives will be added starting from
	 * CI_SIM_PRIM_firstCustPrim = CI_SIM_PRIM_LAST_COMMON_PRIM as the first identifier.
	 * The actual primitive names and IDs are defined in the associated
	 * 'ci_sim_cust_xxx.h' file.
	 */

	/* DO NOT ADD ANY MORE PRIMITIVES HERE */





} _CiSimPrim;

/* specify the number of default common SIM primitives */
#define CI_SIM_NUM_COMMON_PRIM ( CI_SIM_PRIM_LAST_COMMON_PRIM - 1 )

#define CI_SIM_MAX_CMD_DATA_SIZE  261 

/**@}*/

#define EF_SMS        28476
#define EF_FDN        28475
#define EF_EXT2       28491/*Michal Bukai - Extension2 support - update FDN PB with long numbers */
//#define SIM_SELECT         164
#define READ_BINARY        176
#define READ_RECORD        178
#define GET_RESPONSE        192
#define UPDATE_BINARY        214
#define UPDATE_RECORD        220
#define _STATUS                 242
#define RETRIEVE_DATA       203
#define SET_DATA        219
#define LAST_RECORD        25
#define FIRST_RECORD        25
#define INVALID_INDEX   -1
#define EMPTY           0xFF

/** \brief SIM command structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiSimCmdReq_Struct {
	UINT16 len;                                                                             /**< length of SIM command, [5-CI_SIM_MAX_CMD_DATA_SIZE] */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_SIM_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_SIM_CONTIGUOUS
	UINT8       *pData;                                                                     /* pointer to SIM command; optional if len is 0, format according to 3GPP TS 11.11, v8.6.0, 9 */

#else /* CCI_SIM_CONTIGUOUS */
	/* # Start Contiguous Code Section # */
	UINT8 data[CI_SIM_MAX_CMD_DATA_SIZE];           /**< SIM command data */
	/* # End Contiguous Code Section # */
#endif /* CCI_SIM_CONTIGUOUS */
} CiSimCmdReq;

/** \brief Conrirmation to SIM command structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiSimCmdRsp_Struct {
	UINT16 len;                                                                     /**< length of the SIM response, [2-CI_SIM_MAX_CMD_DATA_SIZE] */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_SIM_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_SIM_CONTIGUOUS
	UINT8  *pData; /* pointer to SIM response, format according to 3GPP TS 11.11, v8.6.0, 9 */

#else /* CCI_SIM_CONTIGUOUS */
	/* # Start Contiguous Code Section # */
	UINT8 data[CI_SIM_MAX_CMD_DATA_SIZE];           /**< SIM response, format according to 3GPP TS 11.11, v8.6.0, 9 */
	/* # End Contiguous Code Section # */
#endif /* CCI_SIM_CONTIGUOUS */

} CiSimCmdRsp;

/* Description of the SIM behaviour from the CCI point of view; use of
 * CiSimStatus & CiSimPinState
 *
 * After SIM card is inserted the protocol stack needs to perform an
 * initialization phase in order to obtain and validate all the required
 * information about the SIM card.
 * If PIN1 is enabled, then the protocol stack would need to perform PIN
 * validation before the initialization; the PIN must be obtained from the
 * application, so the protocol stack sends a 'PIN required' notification and
 * waits for the response.
 * If PIN1 is not required then the initialization is started immediately.
 * SIM can be accessed only after the successful completion of the initialization phase;
 * (appropriate notification (ex: SIM OK) is sent to the upper layers.
 * at any time, an indication of SIM error would mean that SIM is invalid and
 * SIM related functionality cannot be used; exception is the error indicating
 * that SIM was rejected by the network, in which case access to the phone book
 * should still be possible.
 * CCI sends the' CiSimPrimDeviceInd/CiSimPrimCardInd' notification each time a change
 * occurs in the SIM status as well as the PIN state.
 * Based on the above behavior design, the following diagram shows the SIM status & PIN
 * state transitions at the CCI level:
 *
 * 1)	at startup
 *
 *    status = not ready
 *    pinState = removed
 *
 * 2)	CCI received 'SIM card inserted' indication from protocol stack; CCI sends:
 *
 *    status = inserted
 *    pinState = waitInit
 *
 * 3)	CCI received 'PIN required' indication from protocol stack; CCI sends:
 *
 *    status = inserted
 *    pinState = pin1Required/unblockPin1Required/others
 *
 * 4)	APP sends the PIN to CCI; CCI sends:
 *
 *    status = inserted
 *    pinState = waitInit
 *
 * 5)	CCI received 'SIM is OK' (initialization completed & SIM ready to be accessed)
 *    indication from protocol stack; CCI sends:
 *
 *    status = ready
 *    pinState = ready
 *
 * 6)	in the ready state, if CCI receives 'SIM rejected by the network' indication from
 *    protocol stack; CCI sends:
 *
 *    status = ready
 *    pinState = rejectedByNetwork
 *
 * 7)	at any other time, if CCI receives 'SIM is NOT OK' indication from protocol stack; CCI sends:
 *
 *    status = not ready
 *    pinState = initError, mepError, removed, etc.
 *
 */

/** \brief SIM Status enumeration */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CISIMSTATUS_TAG {
	CI_SIM_ST_READY = 0,    /**< SIM is initialized/validated and it can be accessed */
	CI_SIM_ST_NOT_READY,    /**< SIM is not inserted or has not been succesfully initialized/validated */

	CI_SIM_ST_INSERTED,     /**< SIM is inserted and is being initialized/validated */

	CI_SIM_NUM_STATUSES
} _CiSimStatus;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief SIM Status enumeration.
 * \sa CISIMSTATUS_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiSimStatus;
/**@}*/

/** \brief SIM PIN state enums */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CISIMPINSTATE_TAG {
	CI_SIM_PIN_ST_READY,                                                            /**< SIM is not expecting password */

	CI_SIM_PIN_ST_CHV1_REQUIRED,                                                    /**< CHV1 is expecting */
	CI_SIM_PIN_ST_CHV2_REQUIRED,                                                    /**< CHV2 is expecting */
	CI_SIM_PIN_ST_UNBLOCK_CHV1_REQUIRED,                                            /**< unblock CHV1 is expecting */
	CI_SIM_PIN_ST_UNBLOCK_CHV2_REQUIRED,                                            /**< unblock CHV2 is expecting */

	/* Note: the '**CK' states are not fully supported at this time */
	CI_SIM_PIN_ST_PCK_REQUIRED,                                             /**< PCK for PH-SIM(s) is is expecting */
	CI_SIM_PIN_ST_NCK_REQUIRED,                                             /**< NCK is expecting */
	CI_SIM_PIN_ST_NSCK_REQUIRED,                                            /**< NSCK is expecting */
	CI_SIM_PIN_ST_SPCK_REQUIRED,                                            /**< SPCK is expecting */
	CI_SIM_PIN_ST_CCK_REQUIRED,                                             /**< CCK is expecting */

	CI_SIM_PIN_ST_UNBLOCK_PCK_REQUIRED,                                     /**< Unblock PCK for PH-SIM(s) expected */
	CI_SIM_PIN_ST_UNBLOCK_NCK_REQUIRED,                                     /**< Unblock NCK expected */
	CI_SIM_PIN_ST_UNBLOCK_NSCK_REQUIRED,                                    /**< Unblock NSCK expected */
	CI_SIM_PIN_ST_UNBLOCK_SPCK_REQUIRED,                                    /**< Unblock SPCK expected */
	CI_SIM_PIN_ST_UNBLOCK_CCK_REQUIRED,                                     /**< Unblock CCK expected */

	/* note, according to 3GPP TS 22.022, v3.4.0, there is no standard way to unblock a locked */
	/* personalization key */

	/* Note: the 'HIDDENKEY' states are not supported at this time */
	CI_SIM_PIN_ST_HIDDENKEY_REQUIRED,                                       /**< expecting Key for hidden phone book entries */
	CI_SIM_PIN_ST_UNBLOCK_HIDDENKEY_REQUIRED,                               /**< expecting code to unblock the hidden Key */

	CI_SIM_PIN_ST_UNIVERSALPIN_REQUIRED,                                    /**< expecting the Universal PIN */
	CI_SIM_PIN_ST_UNBLOCK_UNIVERSALPIN_REQUIRED,                            /**< expecting code to unblock the Universal PIN */

	CI_SIM_PIN_ST_CHV1_BLOCKED,                                             /**< use of CHV1 is blocked */
	CI_SIM_PIN_ST_CHV2_BLOCKED,                                             /**< use of CHV2 is blocked */
	CI_SIM_PIN_ST_UNIVERSALPIN_BLOCKED,                                     /**< use of the Universal PIN is blocked */
	CI_SIM_PIN_ST_UNBLOCK_CHV1_BLOCKED,                                     /**< use of code to unblock the CHV1 is blocked */
	CI_SIM_PIN_ST_UNBLOCK_CHV2_BLOCKED,                                     /**< use of code to unblock the CHV2 is blocked */
	CI_SIM_PIN_ST_UNBLOCK_UNIVERSALPIN_BLOCKED,                             /**< use of code to unblock the Universal PIN is blocked */

	CI_SIM_PIN_ST_NETWORK_REJECTED,                                 /**< SIM was rejected by the network - IMSI is unknown in
									 *    the HLR, ME is illegal or MS is illegal. (see GSM 03.22) */
	CI_SIM_PIN_ST_WAIT_INITIALISATION,                                              /**< SIM is being initialized; waiting for completion */
	CI_SIM_PIN_ST_INIT_FAILED,                                                      /**< SIM initialisation failed */
	CI_SIM_PIN_ST_REMOVED,                                                          /**< SIM was removed */
	CI_SIM_PIN_ST_WRONG_SIM,                                                        /**< SIM was inserted but was not accepted by the protocol stack */
	CI_SIM_PIN_ST_GENERAL_ERROR,                                                    /**< SIM access encountered a serious error */
	CI_SIM_PIN_ST_MEP_ERROR,                                                        /**< Error in checking or accessing ME personalisation data */
	CI_SIM_PIN_ST_UDP_ERROR,                                                        /**< Error in checking or accessing UDP personalisation data */
	CI_SIM_PIN_ST_CPHS_ERROR,                                                       /**<  Error in accessing the CPHS data */

	CI_SIM_PIN_NUM_STATES
} _CiSimPinState;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief SIM PIN state enums.
 * \sa CISIMPINSTATE_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiSimPinState;
/**@}*/

/** \brief Result code enumeration */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CISIMRC_TAG {
	CIRC_SIM_OK = 0,                                /**< success */
	CIRC_SIM_FAILURE,                               /**< failure */
	CIRC_SIM_MEM_PROBLEM,                           /**< SIM memory problem */
	CIRC_SIM_SIMAT_BUSY,                            /**< SIM Application Toolkit is busy */
	CIRC_SIM_INFO_UNAVAILABLE,                      /**< Requested information is unavailable */
	CIRC_SIM_NOT_INSERTED,                          /**< CME 10  */
	CIRC_SIM_PIN_REQUIRED,                          /**< CME 11  */
	CIRC_SIM_PUK_REQUIRED,                          /**< CME 12  */
	CIRC_SIM_BUSY,                                  /**< CME 14  */
	CIRC_SIM_WRONG,                                 /**< CME 15  */
	CIRC_SIM_INCORRECT_PASSWORD,                    /**< CME 16  */
	CIRC_SIM_PIN2_REQUIRED,                         /**< CME 17  */
	CIRC_SIM_PUK2_REQUIRED,                         /**< CME 18  */
	CIRC_SIM_OPERATION_NOT_ALLOWED,                 /**< CME 3   */
	CIRC_SIM_MEMORY_FULL,                           /**< CME20   */
	CIRC_SIM_UNKNOWN,                               /**< CME 100 */
	CIRC_SIM_PERSONALISATION_DISABLED,              /**< Operation failed since personalization is disabled */
	CIRC_SIM_PERSONALISATION_BLOCKED,               /**< Operation failed since personalization is blocked */
	CIRC_SIM_PERSONALISATION_UNKNOWN,               /**< operation failed since personalization database is not available */
	CIRC_SIM_PERSONALISATION_NOT_SUPPORTED,         /**< Operation failed since personalization is not supported */

	CIRC_SIM_NUM_RESCODES
} _CiSimRc;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Result code enumeration.
 * \sa CISIMRC_TAG */
/** \remarks Common Data Section */
typedef UINT16 CiSimRc;
/**@}*/

/*Michal Bukai - OTA support for AT&T - START*/
/*********************************************/

/** \brief MMI State values */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CISIMMMISTATE_TAG {
	CI_SIM_MMI_IDLE = 0,                            /**< MMI is in idle screen and user did not enter any keys */
	CI_SIM_MMI_BUSY,                                /**< There is an acitve menu or the user entered keys on the idle screen */

	CI_SIM_MMI_NUM_STATE
} _CiSimMMIState;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief MMI State enumeration.
 * \sa CISIMMMISTATE_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiSimMMIState;
/**@}*/

/*********************************************/
/** \brief CHV value enumeration */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CISIMCHVNO_TAG {
	CI_SIM_CHV_1 = 1,       /**< CHV1 */
	CI_SIM_CHV_2,           /**< CHV2 */

	CI_SIM_CHV_NEXT_FREE
} _CiSimChvNo;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief CHV value enumeration.
 * \sa CISIMCHVNO_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiSimChvNo;

#define CI_SIM_NUM_CHVS ( CI_SIM_CHV_NEXT_FREE - 1 )
/**@}*/

/** \brief CHV operation value enumeration */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CISIMCHVOPER_TAG {
	CI_SIM_CHV_VERIFY = 0,  /**< verify CHV value */
	CI_SIM_CHV_CHANGE,      /**< change CHV value */
	CI_SIM_CHV_DISABLE,     /**< disable the need for CHV verification, only applied to CHV1 */
	CI_SIM_CHV_ENABLE,      /**< enable the need for CHV verification, only applied to CHV1 */
	CI_SIM_CHV_UNBLOCK,     /**< unblock CHV */
	CI_SIM_CHV_QUERY,       /**< query CHV enable/disable status, only applied to CHV1 */

	CI_SIM_CHV_NUM_OPERS
} _CiSimChvOper;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief CHV value enumeration.
 * \sa CISIMCHVOPER_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiSimChvOper;

#define CI_SIM_MIN_CHV_SIZE           4
#define CI_SIM_MAX_CHV_SIZE           8
#define CI_SIM_UNBLOCK_CHV_SIZE       8

#define CI_SIM_MAX_ME_PROFILE_SIZE    20 /* according to 3GPP TS 11.14, 5.2 */

#define CI_SIM_ME_NORMAL_CK_MIN_SIZE  8
#define CI_SIM_ME_SIM_CK_MIN_SIZE     6
#define CI_SIM_ME_CK_MAX_SIZE         16
/**@}*/

/*******************************************************/
/**< MEP UDP support - START    */
/*******************************************************/
#define  CI_SIM_MEP_MAX_NUMBER_OF_CODES                         3
#define  CI_SIM_MEP_NUM_MEP_SUBSET_DIGITS               1
#define  CI_SIM_MEP_MAX_IMSI_LENGTH                             8
#define  CI_SIM_MEP_NUM_MEP_NETWORKS                            3
#define  CI_SIM_UDP_ICCID_LEN                           10
#define  CI_SIM_UDP_MAX_NUMBER_OF_ASL_ELEMENTS          10

/** \brief Operation on ME personalization enumeration */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CISIMPERSMEOPER_TAG {
	CI_SIM_PERSME_ACTIVATE_PERSONALISATION,         /**< Activate operation. Only this operation need SIM present */
	CI_SIM_PERSME_DEACTIVATE_PERSONALISATION,       /**< Deactivate operation. Can be performed regardless of SIM presence */
	CI_SIM_PERSME_DISABLE_PERSONALISATION,          /**< Disable operation. can be performed regardless of SIM presence */
	CI_SIM_PERSME_READ_PERSONALIZATION_STATUS,      /**< Read operation. Can be performed regardless of SIM presence */
	CI_SIM_PERSME_UNBLOCK_PERSONALISATION,          /**< Unblock operation. Can be performed regardless of SIM presence */
	CI_SIM_PERSME_NUM_OPERS
} _CiSimPersOper;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Operation on ME personalization enumeration.
 * \sa CISIMPERSMEOPER_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiSimPersOper;
/**@}*/

/** \brief personalization category enumeration */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CISIMPERSCAT_TAG {
	CI_SIM_PERS_CAT_SIM,                                    /**< SIM MEP Category */
	CI_SIM_PERS_CAT_NETWORK,                                /**< Network MEP Category */
	CI_SIM_PERS_CAT_NETWORKSUBSET,                          /**< Sub network MEP Category */
	CI_SIM_PERS_CAT_SERVICEPROVIDER,                        /**< Service Provider MEP Category */
	CI_SIM_PERS_CAT_CORPORATE,                              /**< Corporate MEP Category */

	CI_SIM_PERS_NUM_CATS
} _CiSimPersCat;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief personalization category enumeration.
 * \sa CISIMPERSCAT_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiSimPersCat;
/**@}*/


/** \brief   BCD structure. */
/** \details 9906-4268 From GSM 02.22 Page 9, the MEP network subset test shall be
 * performed on digits 6 and 7 of the IMSI, which translates to a single
 * msin array element since they are stored as 2 BCD's per byte. So change
 * number of subset digits to 1
 */
/** \remarks Common data section */
typedef UINT8 CiSimMEP_BCD;     /* 0-9 and 15 (for unused) */

/** \brief  International mobile subscriber identity. */
/** \details */
/** \remarks Common data section */
//ICAT EXPORTED STRUCT
typedef struct CiSimMEP_IMSI_Tag
{
	UINT8 length;
	UINT8 contents [CI_SIM_MEP_MAX_IMSI_LENGTH];
} CiSimMEP_IMSI;

/** \brief   Type for Mobile Network Code */
/** \details */
/** \remarks Common data section */
typedef UINT16 CiSimMEP_MNC;

/** \brief  Type for Mobile Country Code */
/** \details */
/** \remarks Common data section */
typedef UINT16 CiSimMEP_MCC;


/** \brief   Type for access technology Id. */
/** \details For coding of the access technology field, see ETSI TS 131 102.
 * A bitmap is used. the meaning of each bit is as follows
 * b=1 access technology selected
 * b=0 accesstechnology not selected
 *
 * first byte : bit 8 specifies whether UTRAN is selected
 * second byte: bit 8 specifies whether GSM is selected
 *              bit 7 specifies whether GSM COMPACT is selected
 */
/** \remarks Common data section */
typedef UINT16 CiSimMEP_AccessTechnologyId;

/** \brief Type for Public Land Mobile Network definition. */
/** \details */
/** \remarks Common data section */
//ICAT EXPORTED STRUCT
typedef struct CiSimMEP_PLMN_Tag
{
	CiSimMEP_MCC mcc;                                               /**< Mobile Country Code.  \sa CiSimMEP_MCC */
	CiSimMEP_MNC mnc;                                               /**< Mobile Network Code. \sa CiSimMEP_MNC */
	CiSimMEP_AccessTechnologyId accessTechnology;                   /**< Access Technology. \sa CiSimMEP_AccessTechnologyId */
} CiSimMEP_PLMN;

/** \brief Type for PLMN with MNC length. */
/** \details */
/** \remarks Common data section */
//ICAT EXPORTED STRUCT
typedef struct CiSimMEPCodeType_PLMN_Tag
{
	CiSimMEP_PLMN plmn;                                             /**< PLMN. For details see typedef::CiSimMEP_PLMN */
	CiBoolean mncThreeDigitsDecoding;                               /**< Is three digit encoding used. */
} CiSimMEPCodeType_PLMN;


/** \brief MEP NetworkSubset code structure. See 3GPP TS 122.022 MEP  */
/** \details */
/** \remarks Common data section */
//ICAT EXPORTED STRUCT
typedef struct CiSimMEPCodeType_NS_Tag
{
	CiSimMEP_BCD networkSubsetId;             /**< Bits 0-3 = IMSI digit 6, Bits 4-7 = IMSI digit 7. For details see typedef::CiSimMEP_BCD */
} CiSimMEPCodeType_NS;

/** \brief MEP Service provider code structure. See 3GPP TS 122.022 MEP  */
/** \details */
/** \remarks Common data section */
//ICAT EXPORTED STRUCT
typedef struct CiSimMEPCodeType_SP_Tag
{
	UINT8 serviceproviderId;
} CiSimMEPCodeType_SP;

/** \brief MEP Corporate code structure. See 3GPP TS 122.022 MEP  */
/** \details */
/** \remarks Common data section */
//ICAT EXPORTED STRUCT
typedef struct CiSimMEPCodeType_CP_Tag
{
	UINT8 corporateId;
} CiSimMEPCodeType_CP;

/** \brief MEP SIM/USIM code structure. See 3GPP TS 122.022 MEP  */
/** \details */
/** \remarks Common data section */
//ICAT EXPORTED STRUCT
typedef struct CiSimMEPCodeType_SIMUSIM_Tag
{
	CiSimMEP_IMSI simId;
} CiSimMEPCodeType_SIMUSIM;

/** \brief  MEP code union structure */
/** \details */
/** \remarks Common data section */
typedef union CiSimMEPCodeType_Tag
{
	CiSimMEPCodeType_PLMN Network;
	CiSimMEPCodeType_NS NetworkSubset;
	CiSimMEPCodeType_SP SP;
	CiSimMEPCodeType_CP Corporate;
	CiSimMEPCodeType_SIMUSIM SimUsim;
} CiSimMEPCodeType;

/** \brief  Operation on UDP lock. */
/** \details */
/** \remarks Common data section */
//ICAT EXPORTED ENUM
typedef enum CISIMUDPOPER {
	CI_SIM_UDP_OPER_ACTIVATE_PERSONALISATION,
	CI_SIM_UDP_OPER_DEACTIVATE_PERSONALISATION,
	CI_SIM_UDP_READ_PERSONALIZATION_STATUS,
	CI_SIM_UDP_OPER_NUM
} _CiSimUDPOper;

typedef UINT8 CiSimUDPOper;

/** \brief  UDP category. */
/** \details */
/** \remarks Common data section */
//ICAT EXPORTED ENUM
typedef enum CISIMUDPCAT {
	CI_SIM_UDP_CAT_UNAUTHERIZED_SIM_DETECTION,
	CI_SIM_UDP_CAT_INVALID_SIM_DETECTION,
	CI_SIM_UDP_CAT_NUM
} _CiSimUDPCat;

typedef UINT8 CiSimUDPCat;

/** \brief  UDP category. */
/** \details */
/** \remarks Common data section */
//ICAT EXPORTED ENUM
typedef enum CiSIMUDPSTATUS {
	CI_SIM_UDP_UNKNOWN,
	CI_SIM_UDP_INACTIVE,
	CI_SIM_UDP_ACTIVE,
	CI_SIM_UDP_IS_NOT_SUPPORTED,
	CI_SIM_UDP_STATUS_NUM
} _CiSimUDPStatus;

typedef UINT8 CiSimUDPStatus;

/** \brief  SIM ICCID structure. */
/** \details */
/** \remarks Common data section */
//ICAT EXPORTED STRUCT
typedef struct CiSimIccid_Tag
{
	UINT8 data[CI_SIM_UDP_ICCID_LEN];
}CiSimIccid;

/** \brief  UDP ASL operation enumeration. */
/** \details */
/** \remarks Common data section */
//ICAT EXPORTED ENUM
typedef enum CiSIMUDPASLOPER {
	CI_SIM_UDP_ASL_ADD_CURRENT_SIM,
	CI_SIM_UDP_ASL_DELETE,
	CI_SIM_UDP_ASL_DELETE_ALL,
	CI_SIM_UDP_ASL_READ_ALL,
	CI_SIM_UDP_ASL_OPER_NUM
}_CiSimUDPASLOper;

typedef UINT8 CiSimUDPASLOper;

/** \brief  RTC data structure. */
/** \details */
/** \remarks Common data section */
//ICAT EXPORTED STRUCT
typedef struct CiRtcDate_Tag
{
	UINT16 year;            /**< RTC year. */
	UINT8 month;            /**< RTC month (1-12). */
	UINT8 day;              /**< RTC day of month (1-31). */
} CiRtcDate;

/** \brief  UDP ASL entry data structure. */
/** \details */
/** \remarks Common data section */
//ICAT EXPORTED STRUCT
typedef struct CiSimUDPASLEntry_Tag
{
	CiSimIccid iccid;               /**< \sa  CiSimUDPSimIccid */
	CiRtcDate updateTime;           /**< \sa  CiSimUDPRtcDate */
} CiSimUDPASLEntry;

/** \brief MEP personalization status indicator per category enumeration */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CISIMPERSSTATUS {
	CI_SIM_MEP_ACTIVATED,                   /**< Activated */
	CI_SIM_MEP_DEACTIVATED,                 /**< Deactivated */
	CI_SIM_MEP_DISABLED,                    /**< Disabled */
	CI_SIM_MEP_BLOCKED,                     /**< Blocked */
	CI_SIM_MEP_UNKNOWN,                     /**< Unknown */
	CI_SIM_PERS_ST_IS_NOT_SUPPORTED,

	CI_SIM_PERS_NUM_STATUSES
} _CiSimPersStatus;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief personalization category enumeration.
 * \sa CISIMPERSSTATUS_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiSimPersStatus;
/**@}*/

/*******************************************************/
/**< SimLock MEP UDP support - END      */
/*******************************************************/

/** \brief SIMAT terminal profile. Refer to 3GPP TS 11.14, v3.10.0, 5 */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiSimTermProfile_Struct {
	UINT8 len;                                                                      /**< length of the terminal profile, [1- CI_SIM_MAX_ME_PROFILE_SIZE] */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_SIM_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_SIM_CONTIGUOUS
	/* Pointer to the terminal profile data.
	 * Content conforms to 3GPP TS 11.14, v3.10.0, 5.2; if empty, communication
	 * subsystem will assume ME doesn't support SIMAT */
	UINT8 *pData;

#else   /* CCI_SIM_CONTIGUOUS */
	/* # Start Contiguous Code Section # */
	UINT8 data[CI_SIM_MAX_ME_PROFILE_SIZE];         /**< the terminal profile data.
							 * Content conforms to 3GPP TS 11.14, v3.10.0, 5.2; if empty, communication
							 * subsystem will assume ME doesn't support SIMAT */
	/* # End Contiguous Code Section # */
#endif  /* CCI_SIM_CONTIGUOUS */

} CiSimTermProfile;

/** \brief SIM-related facility lock mode */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CISIMFACLCKMODE_TAG {
	CI_SIM_FACLCK_MODE_UNLOCK = 0,
	CI_SIM_FACLCK_MODE_LOCK,
	CI_SIM_FACLCK_MODE_QUERY,

	CI_SIM_FACLCK_NUM_MODES
} _CiSimFacLckMode;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief SIM-related facility lock mode.
 * \sa CISIMFACLCKMODE_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiSimFacLckMode;
/**@}*/

/** \brief SIM-related facility code */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CISIMFACCODE_TAG {
	CI_SIM_FAC_CODE_SIM = 0, /**< lock SIM, needs to be unlocked with CHV1 */
	CI_SIM_FAC_CODE_FDN,    /**< enable FDN feature, needs CHV2 verfication,
				 *  refer to 3GPP 11.11 and 3GPP 11.14 */

	/* The following facility codes are related to personalization. Refer to 3GPP TS 22.022 */
	CI_SIM_FAC_CODE_PERS_FSIM,      /**< SIM personalization, lock phone with the first
					 *   inserted SIM. May not be supported by the server */

	CI_SIM_FAC_NUM_CODES
} _CiSimFacCode;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief SIM-related facility code.
 * \sa CISIMFACCODE_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiSimFacCode;
/**@}*/

/** \brief SIM-related facility lock status */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CISIMFACSTATUS_TAG {
	CI_SIM_FACLCK_ST_DEACTIVE = 0,
	CI_SIM_FACLCK_ST_ACTIVE,

	CI_SIM_FACLCK_NUM_STS
} _CiSimFacStatus;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief SIM-related facility lock status.
 * \sa CISIMFACSTATUS_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiSimFacStatus;

#define CI_SIM_MAX_LINEARFIXEDFILE_RECORDS    254
#define CI_SIM_MAX_LINEARFIXEDFILE_RECORD_SZE 255
/**@}*/

/** \brief SIM Toolkit (SIMAT) Notification Support Options */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CISIMATNOTIFYSUPPORT_TAG {
	CI_SIMATNOT_NOT_IMPLEMENTED = 0,
	CI_SIMATNOT_APP_IMPLEMENTS,
	CI_SIMATNOT_SIMAT_IMPLEMENTS_NO_NOTIFY,
	CI_SIMATNOT_SIMAT_IMPLEMENTS_NOTIFY,
	CI_SIMATNOT_SIMAT_IMPLEMENTS_WITH_APP_INPUT,

	CI_NUM_SIMATNOT_SUPPORT_OPTIONS
} _CiSimatNotifySupport;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief SIM Toolkit (SIMAT) Notification Support Options.
 * \sa CISIMATNOTIFYSUPPORT_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiSimatNotifySupport;
/**@}*/

/** \brief SIM Application Toolkit (SIMAT) Notification Capabilities structure. See 3GPP TS 11.14 (v8.11.0) Section 6 */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiSimatNotifyCap_struct {
	CiSimatNotifySupport capDisplayText;                    /**< DISPLAY TEXT, which displays text or an icon on screen. A high priority is available, to replace anything else on screen. \sa CiSimatNotifySupport */
	CiSimatNotifySupport capGetInkey;                       /**< GET INKEY, which sends text or an icon to the display and requests a single character response in return. It is intended to allow a dialogue between the SIM and the user, particularly for selecting an option from a menu. \sa CiSimatNotifySupport*/
	CiSimatNotifySupport capGetInput;                       /**< GET INPUT, which sends text or an icon to the display and requests a response in return. It is intended to allow a dialogue between the SIM and the user. \sa CiSimatNotifySupport*/
	CiSimatNotifySupport capMoreTime;                       /**< MORE TIME, which does not request any action from the ME. The ME is required to respond with TERMINAL RESPONSE (OK) as normal - see below. The purpose of the MORE TIME command is to provide a mechanism for the SIM Application Toolkit task in the SIM to request more processing time. \sa CiSimatNotifySupport */
	CiSimatNotifySupport capPlayTone;                       /**< PLAY TONE, which requests the ME to play a tone in its earpiece, ringer, or other appropriate loudspeaker. \sa CiSimatNotifySupport */
	CiSimatNotifySupport capPollInterval;                   /**< POLL INTERVAL, which negotiates how often the ME sends STATUS commands to the SIM during idle mode. Polling is disabled with POLLING OFF. Use of STATUS for the proactive SIM is described in TS 11.11. \sa CiSimatNotifySupport */
	CiSimatNotifySupport capRefresh;                        /**< REFRESH, which requests the ME to carry out a SIM initialization according to TS 11.11 subclause 12.2.1, and/or advises the ME that the contents or structure of EFs on the SIM have been changed. The command also makes it possible to restart a card session by resetting the SIM. \sa CiSimatNotifySupport */
	CiSimatNotifySupport capSetupMenu;                      /**< SETUP MENU The SIM shall supply a set of menu items, which shall be integrated with the menu system (or other MMI facility) in order to give the user the opportunity to choose one of these menu items at his own discretion. Each item comprises a short identifier (used to indicate the selection), a text string and optionally an icon identifier, contained in an item icon identifier list data object located at the end of the list of items. \sa CiSimatNotifySupport*/
	CiSimatNotifySupport capSelectItem;                     /**< SELECT ITEM, where the SIM supplies a list of items, and the user is expected to choose one. The ME presents the list in an implementation-dependent way. \sa CiSimatNotifySupport */
	CiSimatNotifySupport capSendSMS;                        /**< SEND DATA, which requests the ME to send on the specified channel data provided by the SIM (if class "e" is supported). \sa CiSimatNotifySupport */
	CiSimatNotifySupport capSendSS;                         /**< SEND SS, which sends an SS request to the network. \sa CiSimatNotifySupport */
	CiSimatNotifySupport capSendUSSD;                       /**< SEND USSD, which sends a USSD string to the network. \sa CiSimatNotifySupport */
	CiSimatNotifySupport capSetupCall;              /**< SET UP CALL, of which there are three types:
												  -   set up a call, but only if not currently busy on another call;
												  -   set up a call, putting all other calls (if any) on hold;
												  -   set up a call, disconnecting all other calls (if any); \sa CiSimatNotifySupport*/
	CiSimatNotifySupport capPollingOff;                     /**< POLLING OFF - This command disables the Proactive Polling (defined in TS 11.11 [20]). SIM Presence Detection (defined in TS 11.11 [20]) is not affected by this command. \sa CiSimatNotifySupport */
	CiSimatNotifySupport capSetupIdleModeText;              /**< SETUP IDLE MODE TEXT - new command. \sa CiSimatNotifySupport */
	CiSimatNotifySupport capProvideLocalInfo;       /**< PROVIDE LOCAL INFORMATION -
												This command requests the ME to send current local information to the SIM. At present, this information is restricted to:
													-   location information: the mobile country code (MCC), mobile network code (MNC), location area code (LAC) and cell ID of the current serving cell;
													-   the IMEI of the ME;
													-   the Network Measurement Results and the BCCH channel list;
													-   the current date, time and time zone;
													-   the current ME language setting;
													 -   and the Timing Advance. . \sa CiSimatNotifySupport*/
	CiSimatNotifySupport capSetupEventList;                 /**< SET UP EVENT LIST - The SIM shall use this command to supply a set of events. This set of events shall become the current list of events for which the ME is to monitor. \sa CiSimatNotifySupport */
	CiSimatNotifySupport capLaunchBrowser;                  /**< LAUNCH BROWSER, which requests a browser inside a browser enabled ME to interpret the content corresponding to a URL. \sa CiSimatNotifySupport */
} CiSimatNotifyCap;

#define CI_NUM_SIMAT_NOTIFY_CAPS  ( sizeof( CiSimatNotifyCap ) / sizeof( CiSimatNotifySupport ) )

/* Maximum length of Alpha ID's for SIM Application Toolkit operations - in particular for the
 * SIMAT "Call Setup Get Ack" indication. According to the Protocol Stack configuration header
 * file, this size must be defined to 241 to "satisfy certain test cases". */
#define CI_SIMAT_MAX_ALPHATAG_LENGTH  241

/** \brief SIM Application Toolkit (SIMAT) Alpha Tag structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiSimatAlphaTag_struct {
	UINT16 len;                                                                             /**< Length */
	UINT8 tag[ CI_SIMAT_MAX_ALPHATAG_LENGTH ];                                              /**< tag data */
} CiSimatAlphaTag;

/** \brief Message Waiting Flag (Indicator) bitmaps. See ETSI TS 131 102 V5.7.0 (2003-12) */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CI_SIM_MSGWAITING_FLAGS_TAG
{
	CI_SIM_MSG_WAITING_VOICE = 0x01,        /**< Voice Mail Message(s) waiting */
	CI_SIM_MSG_WAITING_FAX   = 0x02,        /**< Fax Message(s) waiting */
	CI_SIM_MSG_WAITING_EMAIL = 0x04,        /**< Email Message(s) waiting */
	CI_SIM_MSG_WAITING_OTHER = 0x08         /**< Other Message(s) waiting */
} _CiSimMsgWaitingFlags;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Message Waiting Flag (Indicator) bitmaps. See ETSI TS 131 102 V5.7.0 (2003-12).
 * \sa CI_SIM_MSGWAITING_FLAGS_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiSimMsgWaitingFlags;
/**@}*/

/** \brief Message Waiting Status Information. See ETSI TS 131 102 V5.7.0 (2003-12) */
/** \remarks Common Data Section */

/* AT&T- Smart Card  <CDR-SMCD-610>620><630> <990> Support Message Waiting Ind files: MWIS / VMWF   --BT45*/
//ICAT EXPORTED STRUCT
typedef struct CiSimMsgWaitingInfo_struct
{
	UINT8 flags;                     /*  NumberMsgExist flag
					    IF "flags" == TRUE : next values contains message numbers		(for MWIS file)
					    IF FALSE:     Next value are only flags: 0- No Msg, Else Msg Exist (for VMWF file) */
	UINT8 numVoice;                 /* Number of Voice messages */
	UINT8 numFax;                   /* Number of Fax messages */
	UINT8 numEmail;                 /* Number of Email messages */
	UINT8 numOther;                 /* Number of "Other" messages */
} CiSimMsgWaitingInfo;

/* SIM Service Table structure. See ETSI TS 131 102 V5.7.0 (2003-12) */
#define CI_SIM_MAX_SERVICETABLE_SVCS  56
#define CI_SIM_MAX_SERVICETABLE_SIZE  ( ( CI_SIM_MAX_SERVICETABLE_SVCS + 7 ) / 8 )

/** \brief Sim Service table structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiSimServiceTable_struct
{
	UINT8 size;                                                                             /**< Actual number of bytes used in services[] array */
	UINT8 services[ CI_SIM_MAX_SERVICETABLE_SIZE ];                                         /**< Sim Service table */
} CiSimServiceTable;

/** \brief CPHS Customer Service Profile structure. See CPHS document, Phase 2, ver 4.2.
 *          The field order in this structure and the coding of the bitmaps are as defined in Section B.4.7 of the CPHS document.*/
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiSimCustServiceProfile_struct
{
	UINT8 cspCallOffering;                          /**< Bitmap: Call Offering Services */
	UINT8 cspCallRestriction;                       /**< Bitmap: Call Restriction Services */
	UINT8 cspOtherSuppServices;                     /**< Bitmap: Other Supplementary Services */
	UINT8 cspCallCompletion;                        /**< Bitmap: Call Completion Services */
	UINT8 cspTeleServices;                          /**< Bitmap: Teleservices */
	UINT8 cspCphsTeleServices;                      /**< Bitmap: CPHS Teleservices */
	UINT8 cspCphsFeatures;                          /**< Bitmap: CPHS Features */
	UINT8 cspNumberIdent;                           /**< Bitmap: Number Identification Services */
	UINT8 cspPhase2PlusServices;                    /**< Bitmap: Phase 2+ Services */
	UINT8 cspValueAddedServices;                    /**< Bitmap: Value Added Services */
	UINT8 cspInformationNumbers;                    /**< Bitmap: CPHS Information Numbers */
} CiSimCustServiceProfile;

/** \brief SIM Toolkit (SIMAT) display cmd type */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CISIMATDISPLAYCMDTYPE_TAG {
	CI_SIMAT_DISPLAYCMD_SMS = 0,
	CI_SIMAT_DISPLAYCMD_USSD,
	CI_SIMAT_DISPLAYCMD_SS,
	CI_SIMAT_DISPLAYCMD_SD,                                 /**< to SEND DTMF */
	CI_NUM_SIMAT_NUM_DISPLAYCMD_TYPES
} _CiSimatDisplayCmdType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief SIM Toolkit (SIMAT) display cmd type.
 * \sa CISIMATDISPLAYCMDTYPE_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiSimatDisplayCmdType;
/**@}*/

/** \brief SIM Toolkit (SIMAT) icon display - See 3GPP 11.14 ver 8.15.0 sec. 12.31 */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CISIMATICONDISPLAY_TAG {
	CI_SIMAT_DISPLAY_ICONID_ONLY = 0,
	CI_SIMAT_DISPLAY_ICONID_WITH_ALPHAID,
	CI_NUM_SIMAT_NUM_DISPLAY_ICON_TYPES
} _CiSimatIconDisplay;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief SIM Toolkit (SIMAT) icon display - See 3GPP 11.14 ver 8.15.0 sec. 12.31.
 * \sa CISIMATICONDISPLAY_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiSimatIconDisplay;
/**@}*/

/** \brief SIM Application Toolkit (SIMAT) Icon Tag structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiSimatIconTag_struct {
	CiSimatIconDisplay display;                     /**< \sa CiSimatIconDisplay */
	UINT8 tag;                                      /**< Icon Tag data. */
} CiSimatIconTag;

/* undefined default language */
#define CI_SIM_LANG_UNSPECIFIED 0xFFFF

/** \brief Generic command types as defined in 3gpp TS 11.11 sec 9.2 as well as in ETSI TS 102.221; some naming inconsitencies are noted.  */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CISIMGENERICCMDTYPE_TAG
{
	CI_SIM_SELECT             = 0xa4,
	CI_SIM_STATUS             = 0xf2,

	CI_SIM_READ_BINARY        = 0xb0,
	CI_SIM_UPDATE_BINARY      = 0xd6,
	CI_SIM_READ_RECORD        = 0xb2,
	CI_SIM_UPDATE_RECORD      = 0xdc,

	CI_SIM_SEARCH_RECORD      = 0xa2,                               /**< named as in ETSI TS 102.221 sec 10.1.2 */
	CI_SIM_SEEK               = 0xa2,                               /**< named as in 3gpp TS 11.11 sec 9.2 */
	CI_SIM_INCREASE           = 0x32,

	CI_SIM_VERIFY             = 0x20,                               /**< named as in ETSI TS 102.221 sec 10.1.2 */
	CI_SIM_VERIFY_CHV         = 0x20,                               /**< named as in 3gpp TS 11.11 sec 9.2 */

	CI_SIM_CHANGE_PIN         = 0x24,                               /**< named as in ETSI TS 102.221 sec 10.1.2 */
	CI_SIM_CHANGE_CHV         = 0x24,                               /**< named as in 3gpp TS 11.11 sec 9.2 */

	CI_SIM_DISABLE_PIN        = 0x26,                               /**< named as in ETSI TS 102.221 sec 10.1.2 */
	CI_SIM_DISABLE_CHV        = 0x26,                               /**< named as in 3gpp TS 11.11 sec 9.2 */

	CI_SIM_ENABLE_PIN         = 0x28,                               /**< named as in ETSI TS 102.221 sec 10.1.2 */
	CI_SIM_ENABLE_CHV         = 0x28,                               /**< named as in 3gpp TS 11.11 sec 9.2 */

	CI_SIM_UNBLOCK_PIN        = 0x2c,                               /**< named as in ETSI TS 102.221 sec 10.1.2 */
	CI_SIM_UNBLOCK_CHV        = 0x2c,                               /**< named as in 3gpp TS 11.11 sec 9.2 */

	CI_SIM_DEACTIVATE_FILE    = 0x04,                               /**< named as in ETSI TS 102.221 sec 10.1.2 */
	CI_SIM_INVALIDATE         = 0x04,                               /**< named as in 3gpp TS 11.11 sec 9.2 */

	CI_SIM_ACTIVATE_FILE      = 0x44,                               /**< named as in ETSI TS 102.221 sec 10.1.2 */
	CI_SIM_REHABILITATE       = 0x44,                               /**< named as in 3gpp TS 11.11 sec 9.2 */

	CI_SIM_AUTHENTICATE       = 0x88,                               /**< named as in ETSI TS 102.221 sec 10.1.2 */
	CI_SIM_RUN_GSM_ALGORITHM  = 0x88,                               /**< named as in 3gpp TS 11.11 sec 9.2 */

	CI_SIM_SLEEP              = 0xfa,                               /**< defined only in 3gpp TS 11.11 sec 9.2 */
	CI_SIM_GET_RESPONSE       = 0xc0,                               /**< defined only in 3gpp TS 11.11 sec 9.2 */

	CI_SIM_TERMINAL_PROFILE   = 0x10,
	CI_SIM_ENVELOPE           = 0xC2,
	CI_SIM_FETCH              = 0x12,
	CI_SIM_TERMINAL_RESPONSE  = 0x14,

	CI_SIM_MANAGE_CHANNEL     = 0x70,               /**< defined only in ETSI TS 102.221 */

	/* this should always be the last entry  */
	CI_SIM_INVALID_CMD        = 0x00

} _CiSimGenericCmdType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Generic command types as defined in 3gpp TS 11.11 sec 9.2 as well as in ETSI TS 102.221; some naming inconsitencies are noted.
 * \sa CISIMGENERICCMDTYPE_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiSimGenericCmdType;
/**@}*/

/** \brief Generic access mode types for linear files; as defined in 3gpp TS 11.11 sec 9.2 as well as in ETSI TS 102.221  */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CISIMGENERICRWMODETYPE_TAG
{
	CI_SIM_NEXT_REC          =   0x02,
	CI_SIM_PREVIOUS_REC      =   0x03,
	CI_SIM_CURRENT_ABSOLUTE  =   0x04
} _CiSimGenericRwModeType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Generic access mode types for linear files
 * \sa CISIMGENERICRWMODETYPE_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiSimGenericRwModeType;
/**@}*/

/** \brief Generic SELECT mode types as defined in 3gpp TS 11.11 sec 9.2 as well as in ETSI TS 102.221  */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CISIMGENERICSELECTMODETYPE_TAG
{
	CI_SIM_SELECT_BY_FILE_ID = 0x00,
	CI_SIM_SELECT_CHILD_DF = 0x01,
	CI_SIM_SELECT_PARENT_DF = 0x03,
	CI_SIM_SELECT_BY_DF_NAME = 0x04,
	CI_SIM_SELECT_BY_PATH_FROM_MF = 0x08,
	CI_SIM_SELECT_BY_PATH_FROM_DF = 0x09

} _CiSimGenericSelectModeType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Generic access mode types for linear files
 * \sa CISIMGENERICSELECTMODETYPE_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiSimGenericSelectModeType;
/**@}*/

/** \brief specifies the referencing path for the DF/EF file to be selected  */
/** \remarks Common Data Section */
typedef CiString CiSimFilePath;

/** \brief Generic SELECT mode types as defined in 3gpp TS 11.11 sec 9.2 as well as in ETSI TS 102.221   */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CISIMGENERICRESPONSETYPE_TAG
{
	CI_SIM_RETURN_FCP = 0x04,
	CI_SIM_RETURN_NO_DATA = 0x0C

/* NOTE: more to be defined as needed */
} _CiSimGenericResponseType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Generic SELECT mode types as defined in 3gpp TS 11.11 sec 9.2 as well as in ETSI TS 102.221
 * \sa CISIMGENERICRESPONSETYPE_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiSimGenericResponseType;
/**@}*/

/* ********** */
/* Primitives */
/* ********** */

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_EXECCMD_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimExecCmdReq_struct {
	CiSimCmdReq cmd;                        /**< SIM Command. \sa CiSimCmdReq_Struct */
} CiSimPrimExecCmdReq;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_EXECCMD_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimExecCmdCnf_struct {
	CiSimRc rc;             /**< result code. \sa CiSimRc */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_SIM_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_SIM_CONTIGUOUS
	CiSimCmdRsp *pCnf;                      /**< confirmation to last SIM command request, if rc is CI_SIM_CMDREQ_FAILURE, this field is optional. \sa CiSimCmdRsp */

#else /* CCI_SIM_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
/*Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8* tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */
       /* # Start Contiguous Code Section # */
	CiSimCmdRsp cnf;                                /**< confirmation to last SIM command request, if rc is CI_SIM_CMDREQ_FAILURE, this field is optional. \sa CiSimCmdRsp */
	/* # End Contiguous Code Section # */
#endif  /* CCI_SIM_CONTIGUOUS */

} CiSimPrimExecCmdCnf;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_DEVICE_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimDeviceInd_struct {
	CiSimStatus status;                             /**< SIM Status.\sa CiSimStatus */
	CiSimPinState pinState;                         /**< PIN state when SIM becomes ready, ignored if status is NOT CI_SIM_ST_ READY. \sa CiSimPinState */
	/*Michal Bukai - OTA support for AT&T*/
	CiBoolean ProactiveSessionStatus;               /**< Indicates if in proactive session or not. \sa CCI API Ref Manual */
} CiSimPrimDeviceInd;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_PERSONALIZEME_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimPersonalizeMEReq_struct {
	CiSimPersOper oper;                                                     /**< Operation on ME personalization. \sa CiSimPersOper */
	CiSimPersCat cat;                                                       /**< personalization category. \sa CiSimPersCat */
#ifndef CCI_SIM_CONTIGUOUS
	CiPassword                  *pPass;                                     /**< this field is optional if oper is CI_SIM_PERS_OPER_DISABLE or CI_SIM_PERS_OPER_QUERY. \sa CCI API Ref Manual */

#else /* CCI_SIM_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
/*Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8                   *tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */

	/* # Start Contiguous Code Section # */
	CiPassword pass;                                                /**< this field is optional if oper is CI_SIM_PERS_OPER_DISABLE or CI_SIM_PERS_OPER_QUERY. \sa CCI API Ref Manual */
	/* # End Contiguous Code Section # */
#endif /* CCI_SIM_CONTIGUOUS */

} CiSimPrimPersonalizeMEReq;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_PERSONALIZEME_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimPersonalizeMECnf_struct {
	CiSimRc rc;                             /**< Result code. \sa CiSimRc */
	CiSimPersStatus status;                 /**< Personalization indicator status. \sa CiSimPersStatus */
} CiSimPrimPersonalizeMECnf;

/*******************************************************/
/**< MEP UDP support - START    */
/*******************************************************/

/** <paramref name="CI_SIM_PRIM_READ_MEP_CODES REQ" */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimReadMEPCodesReq_struct {
	CiSimPersCat MEPCategory;               /**< personalization category. \sa CiSimPersCat */
} CiSimPrimReadMEPCodesReq;

/** <paramref name="CI_SIM_PRIM_READ_MEP_CODES CNF" */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimReadMEPCodesCnf_struct {
	CiSimRc rc;                                                                                                     /**< Result code. \sa _CiSimRc */
	CiSimPersCat MEPCategory;                                                                                       /**< Personalization category. \sa CiSimPersCat */
	UINT8 NumberOfcodes;                                                                                            /**< The number of codes stored in ME. */
	CiSimMEPCodeType Codes[CI_SIM_MEP_MAX_NUMBER_OF_CODES];                                                         /**< CiSimMEPCodeType is a union that hold the data per personalizaiton category. This field varies per category, e.g. if category is network the field will be PLMN. For details see \sa CiSimMEPCodeType */
} CiSimPrimReadMEPCodesCnf;

/** <paramref name="CI_SIM_PRIM_UDP_LOCK_REQ" */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimUDPLockReq_struct {
	CiSimUDPOper oper;                                                              /**< operation on UDP lock. \sa CiSimUDPOper */
	CiSimUDPCat UDPCategory;                                                        /**< UDP category. \sa CiSimUDPCat */
#ifndef CCI_SIM_CONTIGUOUS
	CiPassword                  *pPass;

#else   /* CCI_SIM_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
/*Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8                   *tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */

	/* # Start Contiguous Code Section # */
	CiPassword Pass;                                                /**< This is the password required to deactivate UDP lock.This field is optional in case of activate or read status operation. For details see CommonTypes document */
	/* # End Contiguous Code Section # */
#endif /* CCI_SIM_CONTIGUOUS */
} CiSimPrimUDPLockReq;

/** <paramref name="CI_SIM_PRIM_UDP_LOCK_CNF" */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimUDPLockCnf_struct {
	CiSimRc rc;                             /**< Result code.Note that the following values are not valid for UDP:CIRC_SIM_ PERSONALISATION_DISABLED & CIRC_SIM_ PERSONALISATION_BLOCKED. \sa _CiSimRc */
	CiSimUDPStatus status;                  /**< UDP lock status. For details see #CiSimUDPStatus */
} CiSimPrimUDPLockCnf;

/** <paramref name="CI_SIM_PRIM_UDP_CHANGE_PASSWORD_REQ" */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimUDPChangePasswordReq_struct {
#ifndef CCI_SIM_CONTIGUOUS
	CiPassword                  *pOldPassword;
	CiPassword              *pNewPassword;
#else   /* CCI_SIM_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
/*Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8                           *tempNonContPtr1;
	UINT8                           *tempNonContPtr2;

#endif  /* CCI_APP_NONCONTIGUOUS */
       /* # Start Contiguous Code Section # */
	CiPassword oldPassword;                 /**< This is the current password required to unlock the UE. For details see CommonTypes document */
	CiPassword newPassword;                 /**< This is the new password. For details see CommonTypes document */
	/* # End Contiguous Code Section # */
#endif /* CCI_SIM_CONTIGUOUS */
} CiSimPrimUDPChangePasswordReq;

/** <paramref name="CI_SIM_PRIM_UDP_CHANGE_PASSWORD_CNF" */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimUDPChangePasswordCnf_struct {
	CiSimRc rc;                     /**< Result code.Note that the following values are not valid for UDP:CIRC_SIM_ PERSONALISATION_DISABLED & CIRC_SIM_ PERSONALISATION_BLOCKED. \sa _CiSimRc */
} CiSimPrimUDPChangePasswordCnf;

/** <paramref name="CI_SIM_PRIM_UDP_ASL_REQ */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimUDPASLReq_struct {
	CiSimUDPASLOper Oper;                                   /**< Operation that should be done on ASL.  \sa CiSimUDPASLOper */
	CiSimIccid Iccid;                                       /**< Required only for delete operation.  \sa CiSimUDPSimIccid */
} CiSimPrimUDPASLReq;

/** <paramref name="CI_SIM_PRIM_UDP_ASL_CNF */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimUDPASLCnf_struct {
	CiSimRc rc;                                                                     /**< Result code.Note that the following values are not valid for UDP:CIRC_SIM_ PERSONALISATION_DISABLED & CIRC_SIM_ PERSONALISATION_BLOCKED. \sa _CiSimRc */
	UINT8 num;                                                                      /**< .Number of entries in ASL - valid if oper was READ_ALL. 0 - means there are no entries in ASL  [range: 0- max number of ASL elements]  */
	CiSimUDPASLEntry TypeAutherizedSIMlist[CI_SIM_UDP_MAX_NUMBER_OF_ASL_ELEMENTS];  /**< Valid if oper was READ_ALL. Reports the ICCID of all SIMs in authorized list and the date they were added. \sa CiSimUDPASLEntry */
} CiSimPrimUDPASLCnf;

/*******************************************************/
/**< MEP UDP support - END      */
/*******************************************************/


/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_OPERCHV_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimOperChvReq_struct {
	CiSimChvOper oper;                                      /**< \sa CiSimChvOper */
	CiSimChvNo chvNo;                                       /**< \sa CiSimChvNo */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_SIM_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_SIM_CONTIGUOUS
	CiPassword                  *pChvVal;                   /**< \sa CCI API Ref Manual */
	CiPassword                  *pNewChvVal;                /**< \sa CCI API Ref Manual */
#else /* CCI_SIM_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
/*Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8                   *tempNonContPtr1;
	UINT8                   * tempNonContPtr2;
#endif  /* CCI_APP_NONCONTIGUOUS */
       /* # Start Contiguous Code Section # */
	CiPassword chvVal;                                      /**< \sa CCI API Ref Manual */
	CiPassword newChvVal;                                   /**< \sa CCI API Ref Manual */
	/* End Contiguous Code Section */
#endif /* CCI_SIM_CONTIGUOUS */

} CiSimPrimOperChvReq;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_OPERCHV_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimOperChvCnf_struct {
	CiSimRc rc;                                             /**< result code. \sa CiSimRc */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_SIM_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_SIM_CONTIGUOUS
	CiBoolean           *pEnabled;                  /**< Not optional if oper is CI_SIM_CHV_QUERY. TRUE: CHV1 is enabled; FALSE: CHV1 is disabled; \sa CCI API Ref Manual */

#else /* CCI_SIM_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
/*Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8           *tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */

	/* # Start Contiguous Code Section # */
	CiBoolean enabled;                              /**< Not optional if oper is CI_SIM_CHV_QUERY. TRUE: CHV1 is enabled; FALSE: CHV1 is disabled; \sa CCI API Ref Manual */
	/* # End Contiguous Code Section # */
#endif  /* CCI_SIM_CONTIGUOUS */

} CiSimPrimOperChvCnf;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_DOWNLOADPROFILE_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimDownloadProfileReq_struct {
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_SIM_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_SIM_CONTIGUOUS
	CiSimTermProfile    *pProfile;                          /**< terminal profile structure. \sa CiSimTermProfile_Struct */
#else /* CCI_SIM_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
/*Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8                   * tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */

	/* # Start Contiguous Code Section # */
	CiSimTermProfile profile;                                               /**< terminal profile structure. \sa CiSimTermProfile_Struct */
	/* # End Contiguous Code Section # */
#endif /* CCI_SIM_CONTIGUOUS */

} CiSimPrimDownloadProfileReq;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_DOWNLOADPROFILE_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimDownloadProfileCnf_struct {
	CiSimRc rc;             /**< result code. \sa CiSimRc */
} CiSimPrimDownloadProfileCnf;

/**	 <paramref name="CI_SIM_PRIM_ENDATSESSION_IND"> */
typedef CiEmptyPrim CiSimPrimEndAtSessionInd;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_PROACTIVE_CMD_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimProactiveCmdInd_struct {
	UINT16 len;                                                                             /**< length of the proactive command data [1-255] */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_SIM_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_SIM_CONTIGUOUS
	UINT8               *pData;                                                     /**< Proactive command data. */
#else /* CCI_SIM_CONTIGUOUS */
      /* # Start Contiguous Code Section # */
	UINT8 data[CI_SIM_MAX_CMD_DATA_SIZE];                   /**< Proactive command data. */
	/* # End Contiguous Code Section # */
#endif  /* CCI_SIM_CONTIGUOUS */
} CiSimPrimProactiveCmdInd;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_PROACTIVE_CMD_RSP"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimProactiveCmdRsp_struct {
	UINT16 len;                                                                             /**< length of the terminal response command data [1-255] */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_SIM_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_SIM_CONTIGUOUS
	UINT8               *pData;                                                     /**< terminal response command data */
#else /* CCI_SIM_CONTIGUOUS */
      /* # Start Contiguous Code Section # */
	UINT8 data[CI_SIM_MAX_CMD_DATA_SIZE];                   /**< terminal response command data */
	/* # End Contiguous Code Section # */
#endif  /* CCI_SIM_CONTIGUOUS */
} CiSimPrimProactiveCmdRsp;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_ENVELOPE_CMD_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimEnvelopeCmdReq_struct {
	UINT16 len;                                                                     /**< length of the envelope command data [1-255] */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_SIM_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_SIM_CONTIGUOUS
	UINT8       *pData;                                                     /**< envelope command data */
#else /* CCI_SIM_CONTIGUOUS */
      /* # Start Contiguous Code Section # */
	UINT8 data[CI_SIM_MAX_CMD_DATA_SIZE];           /**< envelope command data */
	/* # End Contiguous Code Section # */
#endif  /* CCI_SIM_CONTIGUOUS */
} CiSimPrimEnvelopeCmdReq;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_ENVELOPE_CMD_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimEnvelopeCmdCnf_struct {
	CiSimRc rc;                                                                     /**< result code. \sa CiSimRc */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_SIM_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_SIM_CONTIGUOUS
	CiSimCmdRsp *pCnf;                                              /**< confirmation to an ENVELOPE command request; if rc is CI_SIM_CMDREQ_FAILURE, this field is optional. \sa CiSimCmdRsp_Struct */

#else /* CCI_SIM_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
/*Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8* tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */

	/* # Start Contiguous Code Section # */
	CiSimCmdRsp cnf;                                                        /**< confirmation to an ENVELOPE command request; if rc is CI_SIM_CMDREQ_FAILURE, this field is optional. \sa CiSimCmdRsp_Struct */
	/* # End Contiguous Code Section # */
#endif /* CCI_SIM_CONTIGUOUS */

} CiSimPrimEnvelopeCmdCnf;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_GET_SUBSCRIBER_ID_REQ"> */
typedef CiEmptyPrim CiSimPrimGetSubscriberIdReq;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_GET_SUBSCRIBER_ID_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimGetSubscriberIdCnf_struct {
	CiSimRc rc;                             /**< result code. \sa CiSimRc */
	CiString subscriberStr;                 /**< subscriber id string. \sa CCI API Ref Manual */
} CiSimPrimGetSubscriberIdCnf;

/**	 <paramref name="CI_SIM_PRIM_GET_PIN_STATE_REQ"> */
typedef CiEmptyPrim CiSimPrimGetPinStateReq;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_GET_PIN_STATE_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimGetPinStateCnf_struct {
	CiSimRc rc;                                                     /**< result code. \sa CiSimRc */
	CiSimPinState state;                                            /**< current PIN state. \sa CiSimPinState */
	UINT8 chv1NumRetrys;                                            /**< pin1 status  - number of remaining retrys */
	UINT8 chv2NumRetrys;                                            /**< pin2 status  - number of remaining retrys */
	UINT8 puk1NumRetrys;                                            /**< unblock pin1 status - number of remaining retrys */
	UINT8 puk2NumRetrys;                                            /**< unblock pin2 status - number of remaining retrys */
} CiSimPrimGetPinStateCnf;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_GET_TERMINALPROFILE_REQ"> */
typedef CiEmptyPrim CiSimPrimGetTerminalProfileReq;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_GET_TERMINALPROFILE_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimGetTerminalProfileCnf_struct {
	CiSimRc rc;                                                             /**< result code. For details \sa CiSimRc */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_SIM_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_SIM_CONTIGUOUS
	CiSimTermProfile    *pProfile;                          /**< the terminal profile structure, optional if rc is not CIRC_SIM_OK. \sa CiSimTermProfile_Struct */
#else /* CCI_SIM_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
/*Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8                   *tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */

	/* # Start Contiguous Code Section # */
	CiSimTermProfile profile;                                       /**< the terminal profile structure, optional if rc is not CIRC_SIM_OK. \sa CiSimTermProfile_Struct */
	/* # End Contiguous Code Section # */
#endif                                                                  /* CCI_SIM_CONTIGUOUS */

} CiSimPrimGetTerminalProfileCnf;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_ENABLE_SIMAT_INDS_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimEnableSimatIndsReq_struct {
	CiBoolean enable;                                               /**< TRUE: enable the indication report; FALSE: disable the indication report, default; \sa CCI API Ref Manual */
} CiSimPrimEnableSimatIndsReq;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_ENABLE_SIMAT_INDS_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimEnableSimatIndsCnf_struct {
	CiSimRc rc;                                                             /**< result code. \sa CiSimRc */
} CiSimPrimEnableSimatIndsCnf;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_LOCK_FACILITY_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimLockFacilityReq_struct {
	CiSimFacLckMode mode;                                   /**< facility setting mode. \sa CiSimFacLckMode */
	CiSimFacCode fac;                                       /**< facility code. \sa CiSimFacCode */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_SIM_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_SIM_CONTIGUOUS
	CiPassword                          *pPass;                     /**< password, optional if mode is CI_SIM_FACLCK_MODE_QUERY. \sa CCI API Ref Manual */
#else /* CCI_SIM_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
/*Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8                           *tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */

	/* # Start Contiguous Code Section # */
	CiPassword pass;                                                /**< password, optional if mode is CI_SIM_FACLCK_MODE_QUERY. \sa CCI API Ref Manual */
	/* # End Contiguous Code Section # */
#endif  /* CCI_SIM_CONTIGUOUS */

} CiSimPrimLockFacilityReq;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_LOCK_FACILITY_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimLockFacilityCnf_struct {
	CiSimRc rc;                                     /**< result code. \sa CiSimRc */
	CiSimFacStatus status;                          /**< optional if rc is not CIRC_SIM_OK. \sa CiSimFacStatus */
} CiSimPrimLockFacilityCnf;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_GET_FACILITY_CAP_REQ"> */
typedef CiEmptyPrim CiSimPrimGetFacilityCapReq;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_GET_FACILITY_CAP_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimGetFacilityCapCnf_struct {
	CiSimRc rc;                                     /**< result code. \sa CiSimRc */
	CiBitRange bitsFac;                             /**< optional if rc is not CIRC_SIM_OK. \sa CCI API Ref Manual */
} CiSimPrimGetFacilityCapCnf;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_GET_SIMAT_NOTIFY_CAP_REQ"> */
typedef CiEmptyPrim CiSimPrimGetSimatNotifyCapReq;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_GET_SIMAT_NOTIFY_CAP_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimGetSimatNotifyCapCnf_struct {
	CiSimRc rc;                                                             /**< result code. \sa CiSimRc */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_SIM_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_SIM_CONTIGUOUS
	CiSimatNotifyCap            *pCaps;                             /**< SIMAT Notification Capability information. \sa CiSimatNotifyCap_struct */
#else /* CCI_SIM_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
/*Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8                           *tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */

	/* # Start Contiguous Code Section # */
	CiSimatNotifyCap caps;                                          /**< SIMAT Notification Capability information. \sa CiSimatNotifyCap_struct */
	/* # End Contiguous Code Section # */
#endif  /* CCI_SIM_CONTIGUOUS */

} CiSimPrimGetSimatNotifyCapCnf;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_GET_CALL_SETUP_ACK_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimGetCallSetupAckInd_struct {
	CiBoolean alphaIdPresent;                                       /**< Indicates whether the CALL SETUP has an accompanying Alpanumeric ID string. \sa CCI API Ref Manual */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_SIM_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_SIM_CONTIGUOUS
	CiSimatAlphaTag     *pAlphaId;                                  /**< Optional Alphanumeric ID.NULL pointer if Alpha ID not present  \sa CiSimatAlphaTag_struct */
#else /* CCI_SIM_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
/*Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8                   *tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */

	/* # Start Contiguous Code Section # */
	CiSimatAlphaTag alphaId;                                                        /**< Optional Alphanumeric ID. \sa CiSimatAlphaTag_struct */
	/* # End Contiguous Code Section # */
#endif /* CCI_SIM_CONTIGUOUS */

} CiSimPrimGetCallSetupAckInd;


/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_GET_CALL_SETUP_ACK_RSP"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimGetCallSetupAckRsp_struct {
	CiBoolean accept;                                                       /**< CALL SETUP response. \sa CCI API Ref Manual */
} CiSimPrimGetCallSetupAckRsp;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_GET_SERVICE_PROVIDER_NAME_REQ"> */
/*typedef CiEmptyPrim CiSimPrimGetServiceProviderNameReq;*/
typedef struct CiSimPrimGetServiceProviderNameReq_struct{
    UINT8  		type;							/**< SPN type: 0-GSM_SPN, 1-USIM_SPN. \sa CCI API Ref Manual */
} CiSimPrimGetServiceProviderNameReq;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_GET_SERVICE_PROVIDER_NAME_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimGetServiceProviderNameCnf_struct
{
	CiSimRc rc;                                                     /**< result code. \sa CiSimRc */
    UINT8  		    dispRplmn;				/**<  Display RPLMN: 0-no display, 1-display, 99-invalid. \sa CCI API Ref Manual */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_SIM_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_SIM_CONTIGUOUS
	CiNameInfo          *pSpName;                           /**< Service Provider Name, NULL pointer if Name not available. \sa CCI API Ref Manual */
#else /* CCI_SIM_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
/*Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8           *tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */

	/* # Start Contiguous Code Section # */
	CiNameInfo spName;                                      /**< Service Provider Name, NULL pointer if Name not available. \sa CCI API Ref Manual */
	/* # End Contiguous Code Section # */
#endif /* CCI_SIM_CONTIGUOUS */

} CiSimPrimGetServiceProviderNameCnf;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_GET_MESSAGE_WAITING_INFO_REQ"> */
typedef CiEmptyPrim CiSimPrimGetMessageWaitingInfoReq;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_GET_MESSAGE_WAITING_INFO_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimGetMessageWaitingInfoCnf_struct
{
	CiSimRc rc;                                     /**< result code. \sa CiSimRc */
	CiSimMsgWaitingInfo info;                       /**< Message Waiting information. \sa CiSimMsgWaitingInfo_struct */
} CiSimPrimGetMessageWaitingInfoCnf;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_SET_MESSAGE_WAITING_INFO_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimSetMessageWaitingInfoReq_struct
{
	CiSimMsgWaitingInfo info;               /**< Message Waiting information. \sa CiSimMsgWaitingInfo_struct */
} CiSimPrimSetMessageWaitingInfoReq;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_SET_MESSAGE_WAITING_INFO_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimSetMessageWaitingInfoCnf_struct
{
	CiSimRc rc;                                             /**< result code. \sa CiSimRc */
} CiSimPrimSetMessageWaitingInfoCnf;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_GET_SIM_SERVICE_TABLE_REQ"> */
typedef CiEmptyPrim CiSimPrimGetSimServiceTableReq;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_GET_SIM_SERVICE_TABLE_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimGetSimServiceTableCnf_struct
{
	CiSimRc rc;                                             /**< result code. \sa CiSimRc */
	CiBoolean sstPresent;                                   /**< Indicates whether SIM Service Table is present.  \sa CCI API Ref Manual */
	CiSimServiceTable sst;                                  /**< SIM Service Table (if available). \sa CiSimServiceTable_struct */
} CiSimPrimGetSimServiceTableCnf;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_GET_CUSTOMER_SERVICE_PROFILE_REQ"> */
typedef CiEmptyPrim CiSimPrimGetCustomerServiceProfileReq;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_GET_CUSTOMER_SERVICE_PROFILE_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimGetCustomerServiceProfileCnf_struct
{
	CiSimRc rc;                                                     /**< Result code. \sa CiSimRc. */
	CiBoolean cspPresent;                                           /**< Indicates whether Customer Service Profile is present. \sa CCI API Ref Manual */
	CiSimCustServiceProfile csp;                                    /**< Customer Service Profile (if available). \sa CiSimCustServiceProfile_struct */
} CiSimPrimGetCustomerServiceProfileCnf;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_SIMAT_DISPLAY_INFO_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimSimatDisplayInfoInd_struct {
	CiSimatDisplayCmdType type;                             /**< Type of Display Command. \sa CiSimatDisplayCmdType */
	CiBoolean alphaIdPresent;                               /**< Indicates whether alpha Identifier is present. \sa CCI API Ref Manual */
	CiBoolean iconIdPresent;                                /**< Indicates whether Icon Identifer is present. \sa CCI API Ref Manual */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_SIM_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_SIM_CONTIGUOUS
	CiSimatAlphaTag               *pAlphaId;        /*Alpha Id (if available). NULL pointer if Alpha ID not present. Alpha Id (if available). */
	CiSimatIconTag                        *pIconId; /* Icon Id (if available). NULL pointer if Icon ID not present. Alpha Id (if available). */
#else /* CCI_SIM_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
/*Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8                           *tempNonContPtr1;
	UINT8                           *tempNonContPtr2;
#endif  /* CCI_APP_NONCONTIGUOUS */

	/* # Start Contiguous Code Section # */
	CiSimatAlphaTag alphaId;                                        /**< Alpha Id (if available). \sa CiSimatAlphaTag_struct  */
	CiSimatIconTag iconId;                                          /**< Icon Id (if available). \sa CiSimatIconTag_struct */
	/* # End Contiguous Code Section # */
#endif /* CCI_SIM_CONTIGUOUS */

} CiSimPrimSimatDisplayInfoInd;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_GET_DEFAULT_LANGUAGE_REQ"> */
/* request for the default language stored on the SIM card */
typedef CiEmptyPrim CiSimPrimGetDefaultLanguageReq;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_GET_DEFAULT_LANGUAGE_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimGetDefaultLanguageCnf_struct
{
	CiSimRc rc;                             /**< result code. \sa CiSimRc */
	UINT16 language;                        /**< one byte for each alpha-numeric character */

} CiSimPrimGetDefaultLanguageCnf;

#define CI_SIM_MAX_APDU_HEADER_SIZE             6
#define CI_SIM_MAX_APDU_DATA_SIZE               255
#define CI_SIM_MAX_APDU_SIZE                    CI_SIM_MAX_APDU_DATA_SIZE + CI_SIM_MAX_APDU_HEADER_SIZE
#define CI_SIM_MIN_APDU_SIZE                    4

/* request to send a generic command to the SIM card */
/* the request structure reflects the structure of a SIM */
/* application protocol data unit (APDU), as per ETSI 102.221 & TS 11.11 */
/* Note: the class of instruction is transparent */

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_GENERIC_CMD_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimGenericCmdReq_struct
{
	CiSimFilePath path;                                                                                     /**< path of the elementary file. \sa CiSimFilePath */
	CiSimGenericCmdType instruction;                                                                        /**< instruction code. \sa CiSimGenericCmdType */
	UINT8 param1;                                                                                           /**< parameters 1 for the instruction */
	UINT8 param2;                                                                                           /**< parameters 2 for the instruction*/
	UINT8 length;                                                                                           /**< number of bytes in the command data string */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_SIM_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_SIM_CONTIGUOUS
	UINT8                                         *pData;                                                   /**< pointer to the command data string */
#else /* CCI_SIM_CONTIGUOUS	*/
      /* # Start Contiguous Code Section # */
	UINT8 data[CI_SIM_MAX_APDU_DATA_SIZE];                                  /**< command data string */
	/* # End Contiguous Code Section # */
#endif  /* CCI_SIM_CONTIGUOUS	*/

	CiBoolean responseExpected;                                                                     /**< indicates if a response is expected by the application.\sa CCI API Ref Manual */
	UINT8 responseLength;                                                                           /**< number of bytes expected in the response data string */

} CiSimPrimGenericCmdReq;

/* confirmation to a previous request to send a generic command to the SIM card */

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_GENERIC_CMD_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimGenericCmdCnf_struct {
	CiSimRc rc;                                                     /**< result code. \sa CiSimRc */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_SIM_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_SIM_CONTIGUOUS
	CiSimCmdRsp                           *pCnf;            /**< Cnf to SIM command structure. \sa CiSimCmdRsp_Struct */
#else /* CCI_SIM_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
/*Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8                           *tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */
       /* # Start Contiguous Code Section # */
	CiSimCmdRsp cnf;                                                /**< Cnf to SIM command structure. \sa CiSimCmdRsp_Struct */
	/* # End Contiguous Code Section # */
#endif  /* CCI_SIM_CONTIGUOUS */

	UINT8 sw1;                                                      /**< status byte 1 as returned from the card */
	UINT8 sw2;                                                       /**< status byte 2 as returned from the card */

} CiSimPrimGenericCmdCnf;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_CARD_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimCardInd_struct {
	CiSimStatus status;                             /**< SIM Status. \sa CiSimStatus */
	CiSimPinState pinState;                         /**< PIN state when SIM becomes ready, ignored if status is NOT CI_SIM_ST_READY. \sa CiSimPinState */
	CiBoolean cardIsUicc;                           /**< TRUE if USIM card; FALSE if SIM card. \sa CCI API Ref Manual */
#ifdef AT_CUSTOMER_HTC
	CiBoolean isTestCard;                           /**< TRUE if Test card; FALSE if normal/special card. \sa CCI API Ref Manual */
#endif
} CiSimPrimCardInd;

/* size is 6 digits, as specified in TS 11.11 & ETSI TS 131.102 */
/* one byte is added for the null terminator */
#define CI_SIM_ECC_MAX_LENGTH 7

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_IS_EMERGENCY_NUMBER_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimIsEmergencyNumberReq_struct {
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_SIM_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_SIM_CONTIGUOUS
	CHAR     *eccDigits;                                                            /**< Pointer to null-terminated Dial String; \sa CCI API Ref Manual */

#else /* CCI_SIM_CONTIGUOUS */
	/* # Start Contiguous Code Section # */
	CHAR eccDigitsStr[CI_SIM_ECC_MAX_LENGTH];               /**< null-terminated Dial String; \sa CCI API Ref Manual */
	/* # End Contiguous Code Section # */
#endif  /* CCI_SIM_CONTIGUOUS */

} CiSimPrimIsEmergencyNumberReq;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_IS_EMERGENCY_NUMBER_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimIsEmergencyNumberCnf_struct {
	CiSimRc rc;                                     /**< result code. \sa CiSimRc */
	CiBoolean isEmergency;                          /**< TRUE = number is an emergency call code; FALSE = otherwise; \sa CCI API Ref Manual */
} CiSimPrimIsEmergencyNumberCnf;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_OWNED_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimSimOwnedInd_struct {

	CiBoolean isOwned;                      /**< TRUE = The SIM is owned; FALSE = The SIM is not owned; \sa CCI API Ref Manual  */

}  CiSimPrimSimOwnedInd;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_SIM_CHANGED_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimSimChangedInd_struct {

	CiBoolean isChanged;                             /**< TRUE = The current SIM Card has changed; FALSE = The current SIM Card has not changed; \sa CCI API Ref Manual */

} CiSimPrimSimChangedInd;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_DEVICE_STATUS_REQ"> */
typedef CiEmptyPrim CiSimPrimDeviceStatusReq;

/* <NOTINUSE> */
/**	 <paramref name="CI_SIM_PRIM_DEVICE_STATUS_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimDeviceStatusCnf_struct {

	CiSimRc rc;                                                     /**< result code. \sa CiSimRc */
	CiSimStatus SIMstatus;                                          /**< SIM Status. \sa CiSimStatus */
	CiSimPinState pinState;                                         /**< Not in use. \sa CiSimPinState */
}CiSimPrimDeviceStatusCnf;
/* ADD NEW COMMON PRIMITIVES DEFINITIONS HERE */

/**< Michal Bukai - Virtual SIM support - START */

typedef CiEmptyPrim CiSimPrimSetVSimReq;

//ICAT EXPORTED STRUCT
typedef struct CiSimPrimSetVSimCnf_struct {
	CiSimRc result;
}CiSimPrimSetVSimCnf;

typedef CiEmptyPrim CiSimPrimGetVSimReq;

//ICAT EXPORTED STRUCT
typedef struct CiSimPrimGetVSimCnf_struct {
	CiSimRc result;
	CiBoolean status;
}CiSimPrimGetVSimCnf;

/**< Michal Bukai - Virtual SIM support - END */
/*Michal Bukai - OTA support for AT&T - START*/
/*********************************************/

/* <INUSE> */
/** <paramref name="CI_SIM_PRIM_CHECK_MMI_STATE_IND"> */
typedef CiEmptyPrim CiSimPrimCheckMMIStateInd;

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_CHECK_MMI_STATE_RSP"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimCheckMMIStateRsp_struct {
	CiSimMMIState MMIState; /**< Indicates if MMI state is IDLE or BUSY. \sa CiSimPinState */
} CiSimPrimCheckMMIStateRsp;

/*********************************************/

/****************************************************/
/*Michal Bukai - additional SIMAT primitives - START*/
/****************************************************/

/** \brief values of SIMAT CC status response */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CISIMSIMATCCSTATUS_TAG {
    CI_SIM_SIMAT_CC_STATUS_NO_CHANGE = 0,			/**< The request was not modified by SIMAT CC */
    CI_SIM_SIMAT_CC_STATUS_CALL_CHANGED,			/**< SIMAT CC changed a call setup request */
	CI_SIM_SIMAT_CC_STATUS_CALL_BARRED,				/**< SIMAT CC barred a call setup request */
	CI_SIM_SIMAT_CC_STATUS_CALL_REPLACED_BY_SS,		/**< SIMAT CC replaced a call setup request with an SS operation */
	CI_SIM_SIMAT_CC_STATUS_SS_CHANGED,				/**< SIMAT CC changed an SS operation */
	CI_SIM_SIMAT_CC_STATUS_SS_BARRED,				/**< SIMAT CC barred an SS operation  */
	CI_SIM_SIMAT_CC_STATUS_SS_REPLACED_BY_CALL,		/**< SIMAT CC replaced an SS operation request with call setup  */
	CI_SIM_SIMAT_CC_STATUS_SS_FAILED,				/**< SIMAT CC changed an SS operation or replaced a call setup request or a USSD operation with an SS operation and SS operation failed  */
	CI_SIM_SIMAT_CC_STATUS_CALL_FAILED,				/**< SIMAT CC changed a call setup request or replaced an SS operation or a USSD operation with a call setup request and call setup failed  */
	CI_SIM_SIMAT_CC_STATUS_SS_OK,					/**< SIMAT CC changed an SS operation or replaced a call setup request or a USSD operation with an SS operation and SS operation is OK  */
	CI_SIM_SIMAT_CC_STATUS_USSD_FAILED,				/**< SIMAT CC changed a USSD operation or replaced a call setup request or an SS operation with a USSD operation and USSD operation failed  */
	CI_SIM_SIMAT_CC_STATUS_USSD_OK,					/**< SIMAT CC changed a USSD operation or replaced a call setup request or an SS operation with a USSD operation and USSD operation is OK  */
	CI_SIM_SIMAT_CC_STATUS_CALL_REPLACED_BY_USSD,	/**< SIMAT CC replaced a call setup request with a USSD operation  */
	CI_SIM_SIMAT_CC_STATUS_SS_REPLACED_BY_USSD,		/**< SIMAT CC replaced an SS operation request with a USSD operation  */
	CI_SIM_SIMAT_CC_STATUS_USSD_CHANGED,			/**< SIMAT CC changed a USSD operation */
	CI_SIM_SIMAT_CC_STATUS_USSD_BARRED,				/**< SIMAT CC barred a USSD operation */
	CI_SIM_SIMAT_CC_STATUS_USSD_REPLACED_BY_CALL ,	/**< SIMAT CC replaced a USSD operation with call setup */
	CI_SIM_SIMAT_CC_STATUS_USSD_REPLACED_BY_SS,	/**< SIMAT CC replaced a USSD operation with an SS operation  */

    CI_SIM_SIMAT_CC_STATUS_NUM_TYPES
}_CiSimSIMATCcStatus;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief SIMAT CC status response type
 * \sa CISIMSIMATCCSTATUS_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiSimSIMATCcStatus;
/**@}*/

/** \brief SIMAT CC operation values */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CISIMSIMATCCOPERATION_TAG {
    CI_SIM_SIMAT_CC_CALL_SET_UP = 0,		/**< Call setup */
    CI_SIM_SIMAT_CC_SS_OPERATION,			/**< SS operation */
	CI_SIM_SIMAT_CC_USSD_OPERATION,			/**< USSD operation */

    CI_SIM_SIMAT_CC_OPERATION_NUM_TYPES
}_CiSimSIMATCcOperation;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief SIMAT CC operation type
 * \sa CISIMSIMATCCOPERATION_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiSimSIMATCcOperation;
/**@}*/

/** \brief values of SIMAT SM status response */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CISIMSIMATSMSTATUS_TAG {
    CI_SIM_SIMAT_SM_STATUS_NO_CHANGE = 0,	/**< SIMAT SM did not modify an SMS request */
    CI_SIM_SIMAT_SM_STATUS_CHANGED,			/**< SIMAT SM changed an SMS destination address */
	CI_SIM_SIMAT_SM_STATUS_BARRED,			/**< SIMAT CC barred a call setup request */

    CI_SIM_SIMAT_SM_STATUS_NUM_TYPES
}_CiSimSIMATSmStatus;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief SIMAT SM status response
 * \sa CISIMSIMATSMSTATUS_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiSimSIMATSmStatus;
/**@}*/

/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_SIMAT_CC_STATUS_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimSIMATCcStatusInd_struct{
	CiSimSIMATCcStatus		status;			/**< SIMAT CC status response  \sa CiSimSIMATCcStatus */
	CiSimSIMATCcOperation	OperationType;	/**< The operation type that was passed to USIM  \sa CiSimSIMATCcOperation */
	CiBoolean				alphaIdPresent;	/**< Indicates whether the SIMAT CC status response has an accompanying alphanumeric ID string \sa CCI API Ref Manual */
	CiSimatAlphaTag			alphaId;		/**< Optional alphanumeric ID \sa CiSimatAlphaTag_struct  */
	CiBoolean				addressPresent;	/**< Indicates whether the SIMAT CC status response has a changed called number \sa CCI API Ref Manual */
	CiAddressInfo			AddressInfo;	/**< Optional changed called number \sa CiAddressInfo_struct  */
}CiSimPrimSIMATCcStatusInd;


/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_SIMAT_SEND_CALL_SETUP_RSP_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimSIMATSendCallSetupRspInd_struct{
	CiBoolean			status;	/**< Response sent to SIMAT; TRUE: call setup is OK; FALSE: call setup failed. \sa CCI API Ref Manual */
}CiSimPrimSIMATSendCallSetupRspInd;


/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_SIMAT_SEND_SS_USSD_RSP_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimSIMATSendSsUssdRspInd_struct{
	CiBoolean				status;	/**< Response sent to SIMAT; TRUE: SS or USSD operation is OK; FALSE: SS or USSD operation failed. \sa CCI API Ref Manual */
	CiSimSIMATCcOperation	OperationType;	/**< The operation type that was passed to USIM  \sa CiSimSIMATCcOperation */
}CiSimPrimSIMATSendSsUssdRspInd;


/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_SIMAT_SM_CONTROL_STATUS_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimSIMATSmControlStatusInd_struct{
	CiSimSIMATSmStatus	status;			/**< SIMAT SM status response  \sa CiSimSIMATSmStatus */
	CiBoolean			alphaIdPresent;	/**< Indicates whether the SIMAT SM status response has an accompanying alphanumeric ID string \sa CCI API Ref Manual */
	CiSimatAlphaTag		alphaId;		/**< Optional alphanumeric ID \sa CiSimatAlphaTag_struct  */
}CiSimPrimSIMATSmControlStatusInd;


/* <INUSE> */
/**	 <paramref name="CI_SIM_PRIM_SIMAT_SEND_SM_RSP_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiSimPrimSIMATSendSmRspInd_struct{
	CiBoolean			status;	/**< Response sent to SIMAT; TRUE: SM operation is OK;FALSE: SM operation failed. \sa CCI API Ref Manual */
}CiSimPrimSIMATSendSmRspInd;



/****************************************************/
/*Michal Bukai - additional SIMAT primitives - END  */
/****************************************************/
#ifdef CI_CUSTOM_EXTENSION
/* it is assumed that only one customized set of extension primitives is
 * to be considered at one time. The selection of the particular customized
 * set is done in the 'ci_sim_cust.h' based on compile flags.
 *
 * Note: if no customer extension primitives are defined for this service group
 * the CI_SIM_NUM_CUST_PRIM will be set to 0 in the "ci_sim_cust.h" file.
 */
#include "ci_sim_cust.h"

#define CI_SIM_NUM_PRIM ( CI_SIM_NUM_COMMON_PRIM + CI_SIM_NUM_CUST_PRIM )

#else

/* if no customer extension is supported, only the default common set is considered */
#define CI_SIM_NUM_PRIM CI_SIM_NUM_COMMON_PRIM

#endif /* CI_CUSTOM_EXTENSION */

#ifdef __cplusplus
}
#endif  //__cplusplus

#endif  /* _CI_SIM_H_ */


/*                      end of ci_sim.h
   --------------------------------------------------------------------------- */
