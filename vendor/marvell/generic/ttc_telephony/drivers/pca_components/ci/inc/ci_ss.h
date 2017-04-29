/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/* ===========================================================================
   File        : ci_ss.h
   Description : Data types file for the ss service group

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

#if !defined(_CI_SS_H_)
#define _CI_SS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ci_api_types.h"
/** \addtogroup  SpecificSGRelated
 * @{ */

#define CI_SS_VER_MAJOR 3
#define CI_SS_VER_MINOR 0

//ICAT EXPORTED ENUM
/** Summary of primitives */
typedef enum CI_SS_PRIM {
	CI_SS_PRIM_GET_CLIP_STATUS_REQ = 1, /**< \brief Requests status information for the CLIP (Calling Line Identification Presentation)  Supplementary Service.
					 * \details Requests the local CLIP presentation option setting and interrogates the CLIP provision status at the Network.
					 *   If enabled, CLIP information is reported by the CI_CC_PRIM_CLIP_INFO_IND indication.
					 *   If the CLIP service is provisioned, local CLIP presentation is enabled by default.
					 *   There should be no reason for an unsuccessful result. */
	CI_SS_PRIM_GET_CLIP_STATUS_CNF,         /**< \brief Returns status information for the CLIP (Calling Line Identification Presentation) Supplementary Service.
						 * \details Reports the local CLIP presentation option setting and the CLIP provision status at the Network.
						 *  If the CLIP service is provisioned, local CLIP presentation is enabled by default.
						 *  There is no reason for an unsuccessful result. */
	CI_SS_PRIM_SET_CLIP_OPTION_REQ,         /**< \brief Sets the local presentation option for CLIP (Calling Line Identification Presentation) information on incoming calls.
						 * \details This option affects only the presentation of CLIP information at the mobile.
						 * It does not affect the operation of the CLIP service at the Network itself.
						 * The provisioned setting for CLIP is unchanged.
						 *  The default value is 1 (CLIP presentation enabled). This request will fail if the CLIP  service is not provisioned */
	CI_SS_PRIM_SET_CLIP_OPTION_CNF, /**< \brief Confirms a request to set the local presentation option for CLIP (Calling Line Identification Presentation) information on incoming calls.
					 * \details This option affects only the presentation of CLIP information at the mobile.
					 * It does not affect the operation of the CLIP service at the Network itself.
					 * The provisioned setting for CLIP is unchanged. */
	CI_SS_PRIM_GET_CLIR_STATUS_REQ, /**< \brief Requests status information for the CLIR (Calling Line Identification Restriction) Supplementary Service.
					 * \details Requests the local CLIR presentation option setting and interrogates the CLIR provision status at the Network.*/
	CI_SS_PRIM_GET_CLIR_STATUS_CNF, /**< \brief Returns status information for the CLIR (Calling Line Identification Restriction) Supplementary Service.
					 * \details Reports the local CLIR presentation option setting and the CLIR provision status at the Network.
					 *  See the CI_SS_PRIM_SET_CLIR_OPTION_REQ request for definitions of the temporary and permanent CLIR settings.
					 *  There should be no reason for an unsuccessful result.*/
	CI_SS_PRIM_SET_CLIR_OPTION_REQ, /**< \brief Sets the local option to provide CLIR (Calling Line Identification Restriction) information on outgoing calls.
					 * \details Unlike the CLIP service, the CLIR setting may override the Provision setting. The new setting is used as a temporary setting for all outgoing calls
					 *   until it is revoked by using this request to set the Local parameter to 0.
					 *   The Provision setting is still preserved as the permanent setting.
					 *   The default value is 2 (CLIR presentation allowed).
					 *   This request will fail if the CLIR service is not provisioned.*/
	CI_SS_PRIM_SET_CLIR_OPTION_CNF, /**< \brief Confirms a request to set the local option to provide CLIR (Calling Line Identification Restriction) information on outgoing
					 *  calls.  \details */
	CI_SS_PRIM_GET_COLP_STATUS_REQ, /**< \brief Requests status information for the CoLP (Connected Line Identification Presentation) Supplementary Service.
					 * \details Requests the local CoLP presentation option setting and interrogates the CoLP provision status at the Network.
					 *  If enabled, CoLP information is reported by the CI_CC_PRIM_COLP_INFO_IND indication. */
	CI_SS_PRIM_GET_COLP_STATUS_CNF, /**< \brief Returns status information for the COlP (Connected Line Identification Presentation) Supplementary Service.
					 * \details Reports the local CoLP presentation option setting and the CoLP provision status at the Network.
					 *  There should be no reason for an unsuccessful result. */
	CI_SS_PRIM_SET_COLP_OPTION_REQ, /**< \brief Enables or disables local presentation of CoLP (Connected Line Identification Presentation) information for outgoing calls.  \details */
	CI_SS_PRIM_SET_COLP_OPTION_CNF, /**< \brief Confirms a request to set the local CoLP (Connected Line Identification Presentation) Option.
					 * \details  This option affects only the local presentation of CoLP information (at the mobile); it does not affect the operation of the
					 *  CoLP service at the Network itself. The provisioned setting for CoLP is unchanged.*/
	CI_SS_PRIM_GET_CUG_CONFIG_REQ,  /**< \brief Requests current configuration information for the CUG (Closed User Groups) Supplementary Service.
					 * \details If the provisioned CUG information has been temporarily overridden, this request will retrieve the temporary CUG
					 *  configuration information.*/
	CI_SS_PRIM_GET_CUG_CONFIG_CNF,  /**< \brief Returns current configuration information for the CUG (Closed User Groups) Supplementary Service.
					 * \details If the provisioned CUG information has been temporarily overridden, the temporary CUG configuration information will be
					 *  reported. Otherwise, the CUG configuration information is not meaningful. See CI_SS_PRIM_SET_CUG_CONFIG_REQ for more
					 *  information on the CUG override settings. By default, CUG Temporary Mode is disabled. The provisioned (preferential) CUG
					 *  index will be used for all CUG-related outgoing calls, until Temporary Mode is enabled.
					 *  There should be no reason for an unsuccessful result.*/
	CI_SS_PRIM_SET_CUG_CONFIG_REQ,  /**< \brief Sets up configuration information for the CUG (Closed User Groups) Supplementary Service.
					 * \details This request can be used for the following operations:
					 *  Enable CUG Temporary Mode, and use supplied data to override the provisioned CUG information
					 *  Stay in CUG Temporary Mode and use supplied data to modify current information
					 *  Disable (revoke) CUG Temporary Mode, and revert to the provisioned CUG information
					 *  Current CUG information (provisioned or overridden) will be used for all CUG-related outgoing calls until the information is
					 *  changed, or CUG Temporary Mode is revoked.
					 *   If this request is used to disable CUG Temporary Mode, the CUG configuration information is not applicable, and will be ignored.
					 *   CUG Temporary Mode is disabled by default, and the mobile will use the provisioned CUG information for all CUG-related outgoing calls. */
	CI_SS_PRIM_SET_CUG_CONFIG_CNF,  /**< \brief Confirms new configuration information for the CUG Supplementary Service.  \details */
	CI_SS_PRIM_GET_CNAP_STATUS_REQ, /**< \brief Requests status information for the  CNAP (Calling Name Presentation) Supplementary Service.
				   * \details Requests the local CNAP presentation option setting and interrogates the CNAP provision status at the Network.*/
	//Michal Bukai - CNAP support
	CI_SS_PRIM_GET_CNAP_STATUS_CNF,                                                                         /**< \brief Confirmation to CI_SS_PRIM_GET_CNAP_STATUS_REQ  \details */
	//Michal Bukai - CNAP support
	CI_SS_PRIM_SET_CNAP_OPTION_REQ, /**< \brief Update CNAP Presentation Option in ComSide.  \details */    //Michal Bukai - CNAP support
	CI_SS_PRIM_SET_CNAP_OPTION_CNF, /**< \brief Conrirmation to CI_SS_PRIM_SET_CNAP_OPTION_REQ  \details */ //Michal Bukai - CNAP support
	CI_SS_PRIM_REGISTER_CF_INFO_REQ,        /**< \brief Requests Registration of new CF (Call Forwarding) information.
						 * \details This request will attempt to register the following Call Forwarding information:
						 *  The class (or classes) of service
						 *  The Call Forwarding condition (or conditions) desired
						 *  The Call Forwarding number, optionally with an accompanying subaddress
						 *  More than one class of service can be indicated in this request. This can be achieved by setting the appropriate bits in the
						 *  Classes parameter. For more information, see the CiSsCfRegisterInfo structure definitions.
						 *  The Call Forwarding Number must be:
						 *  A valid directory number
						 *  Not the mobile subscriber's own number
						 *  Not a special service number (e.g. "911", "411", etc.)
						 *  Call Forwarding must be registered separately for each of the supported conditions. However, the "All Call Forwarding" or
						 *  "All Conditional Call Forwarding" can be used to set up multiple conditions in a single Registration request. A Registration
						 *  request is sent to the Network, which may accept or reject the request. */
	CI_SS_PRIM_REGISTER_CF_INFO_CNF, /**< \brief Confirms a CF (Call Forwarding) Registration request.
					 * \details On successful Registration, the Call Forwarding service is "automatically" activated with the registered information.
					 *  This information will remain in effect until it is changed by another Registration request, or it is erased. */
	CI_SS_PRIM_ERASE_CF_INFO_REQ,   /**< \brief Requests Erasure of CF (Call Forwarding) information.
					 * \details This request will attempt to erase the following Call Forwarding information:
					 *  The class (or classes) of service
					 *  The Call Forwarding condition (or conditions) desired
					 *  More than one class of service can be indicated in this request. This can be achieved by setting the appropriate bits in
					 *  the Classes parameter. For more information, see the CiSsCfEraseInfo structure definitions.
					 *  Call Forwarding must be erased separately for each of the supported conditions. However, the "All Call Forwarding" or
					 *  "All Conditional Call Forwarding" can be used to set up multiple conditions in a single Erasure request.
					 *  Note that a Registration request with updated information will effectively erase (and override) the information set up by
					 *  the previous Registration. An Erasure request is sent to the Network, which may accept or reject the request. */
	CI_SS_PRIM_ERASE_CF_INFO_CNF,   /**< \brief Confirms a CF (Call Forwarding) Erasure request.
					 * \details On successful Erasure, previously registered Call Forwarding information is erased at the Network, and the Call Forwarding
					 *  service is "automatically" deactivated. */
	CI_SS_PRIM_GET_CF_REASONS_REQ,  /**< \brief Requests a list of supported CF (Call Forwarding) reasons.  \details */
	CI_SS_PRIM_GET_CF_REASONS_CNF,  /**< \brief Returns a list of supported CF (Call Forwarding) reasons.
					 * \details This primitive provides information as specified for the "AT+CCFC=?" test command.
					 *  There should be no reason for an unsuccessful result */
	CI_SS_PRIM_SET_CF_ACTIVATION_REQ, /**< \brief Requests Activation or Deactivation of a CF (Call Forwarding) service.
					 * \details Call Forwarding is "automatically" activated by the Network (for appropriate classes and reasons) on successful Registration.
					 *  See the CI_SS_PRIM_REGISTER_CF_INFO_REQ request (and its confirmation) for more information.
					 *  Call Forwarding is "automatically" deactivated by the Network (for appropriate classes and reasons) on successful Erasure.
					 *  See the CI_SS_PRIM_ERASE_CF_INFO_REQ request (and its confirmation) for more information. This request will fail if the
					 *  Call Forwarding service is not provisioned. */
	CI_SS_PRIM_SET_CF_ACTIVATION_CNF, /**< \brief Confirms an Activation or Deactivation request for a CF (Call Forwarding) service.
					 * \details The Network will reject the Activation request if it conflicts with other Supplementary Services.*/
	CI_SS_PRIM_INTERROGATE_CF_INFO_REQ, /**< \brief Requests Interrogation of CF (Call Forwarding) services.
					 * \details  An Interrogation request is sent to the Network, which may accept or reject the request. */
	CI_SS_PRIM_INTERROGATE_CF_INFO_CNF, /**< \brief Returns the Interrogation result for CF (Call Forwarding) services.
					 * \details If the Result parameter indicates an error, the CF Registration information is not likely to be meaningful.*/
	CI_SS_PRIM_SET_CW_ACTIVATION_REQ, /**< \brief Requests Activation or Deactivation of Call Waiting (CW) for a specified Class of Service.
					 * \details Activates or Deactivates the Call Waiting service at the Network.If provisioned, Call Waiting is activated by default. */
	CI_SS_PRIM_SET_CW_ACTIVATION_CNF, /**< \brief Confirms Activation or Deactivation of CW (Call Waiting) for a specified Class of Service.
					 * \details The Network will reject the Activation request if it conflicts with other Supplementary Services.*/
	CI_SS_PRIM_GET_CW_OPTION_REQ,   /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_SS_PRIM_GET_CW_OPTION_CNF,   /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_SS_PRIM_SET_CW_OPTION_REQ,   /**< \brief Sets the local presentation option for incoming Call Waiting (CW) indications.
					 * \details Call Waiting information (if enabled) is reported by CI_CC_PRIM_CALL_WAITING_IND.
					   If the Call Waiting service has been deactivated at the Network, a CI_SS_PRIM_SET_CW_OPTION_REQ request to enable local Call Waiting indications will have no effect until the service is re-activated by a CI_SS_PRIM_SET_CW_ACTIVATION_REQ request.
					   Local Call Waiting indications are enabled by default.
					 */
	CI_SS_PRIM_SET_CW_OPTION_CNF,   /**< \brief Confirms a request to set the local presentation option for incoming CW (Call waiting) indications.
					 * \details There is no reason for an unsuccessful result.*/
	CI_SS_PRIM_GET_CW_ACTIVE_STATUS_REQ,    /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_SS_PRIM_GET_CW_ACTIVE_STATUS_CNF,    /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_SS_PRIM_GET_USSD_ENABLE_REQ,         /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_SS_PRIM_GET_USSD_ENABLE_CNF,         /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_SS_PRIM_SET_USSD_ENABLE_REQ, /**< \brief Enables or disables unsolicited USSD indications.
					 * \details USSD indications are used to configure reports of incoming USSD information.
					 * Unsolicited USSD indications (CI_SS_PRIM_RECEIVED_USSD_INFO_IND) are enabled by default.
					 * USSD sessions can exist either during a call or out of call.
					 */
	CI_SS_PRIM_SET_USSD_ENABLE_CNF, /**< \brief Confirms a request to enable or disable unsolicited USSD indications.
					 * \details Confirms a request to enable or disable unsolicited USSD indications. */
	CI_SS_PRIM_RECEIVED_USSD_INFO_IND, /**< \brief Reports incoming (received) USSD information.
					 * \details Unsolicited USSD indications (CI_SS_PRIM_RECEIVED_USSD_INFO_IND) are enabled by default.
					 * USSD sessions can exist either during a call or out of call.
					 * USSD indications are used for incoming USSD information.
					 *  The Status field allows the USSD receiver to determine what action is required, or to receive status notifications for the
					 *  current USSD session. The mobile client can use this information to support both Network initiated USSD sessions and mobile
					 *  initiated USSD sessions:
					 *  Status = CISS_USSD_STATUS_NO_INFO_NEEDED is used for Network-initiated USSD-Notify operations. These indicate
					 *  incoming USSD strings, during a Network initiated USSD session.
					 *  Status = CISS_USSD_STATUS_MORE_INFO_NEEDED is used for Network-initiated USSD-Request operations. These indicate
					 *  requests for outgoing USSD strings, during a Mobile initiated USSD sesion.
					 *  These indications can be enabled or disabled (suppressed) using CI_SS_PRIM_SET_USSD_ENABLE_REQ.
					 * Indications are enabled by default.
					 *  USSD sessions can exist either during a call or out of call*/
	CI_SS_PRIM_RECEIVED_USSD_INFO_RSP, /**< \brief Responds to incoming (received) USSD information.
					 * \details USSD sessions can exist either during a call or out of call. */
	CI_SS_PRIM_START_USSD_SESSION_REQ, /**< \brief Starts up a Mobile-Originated (MO) USSD session.
					 * \details This request is used only to initiate (start up) a USSD session from the mobile end.
					 * Thereafter, all mobile-originated USSD   *  message transfers are controlled from the Network end.
					 * The Network sends messages to the mobile to request its USS data,
					 *  and the mobile sends its USS data in response to these requests (by sending CI_SS_PRIM_RECEIVED_USSD_INFO_RSP
					 *  responses for CI_SS_PRIM_RECEIVED_USSD_INFO indications). USSD sessions can be established either during a call or out
					 *  of call. */
	CI_SS_PRIM_START_USSD_SESSION_CNF, /**< \brief Confirms a request to initiate a Mobile-Originated (MO) USSD session.
					 * \details This signal confirms that the USSD information was sent from the mobile.
					 *  The Network may indicate any procedural errors by sending appropriate USSD information to the mobile.
					 *  USSD sessions can be established either during a call or out of call. */
	CI_SS_PRIM_ABORT_USSD_SESSION_REQ, /**< \brief Requests the current USSD session to be aborted.
					 * \details This request is used to abort the current USSD session from the mobile end. The mobile may abort any USSD session,
					 *  whether initiated from the mobile of from the Network. USSD sessions can be aborted either during a call or out of call. */
	CI_SS_PRIM_ABORT_USSD_SESSION_CNF, /**< \brief Confirms a request to abort a USSD session.
					 * \details This signal confirms that the USSD Abort request was sent from the mobile.
					 *   The Network may indicate any procedural errors by sending appropriate USSD information to the mobile.
					 *   USSD sessions can be aborted either during a call or out of call.*/
	CI_SS_PRIM_GET_CCM_OPTION_REQ,  /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_SS_PRIM_GET_CCM_OPTION_CNF,  /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_SS_PRIM_SET_CCM_OPTION_REQ,  /**< \brief Enables or disables periodic unsolicited Current Call Meter (CCM) update indications.
					 * \details Periodic CCM updates are reported by the CI_CC_PRIM_CCM_UPDATE_IND indication.
					   Periodic CCM updates are enabled by default.
					 * Not supported by SAC
					 */
	CI_SS_PRIM_SET_CCM_OPTION_CNF,  /**< \brief Confirms a request to enable or disable unsolicited Current Call Meter (CCM) update indications
					 * \details There is no reason for an unsuccessful result.
					 * Not supported by SAC.  */
	CI_SS_PRIM_GET_AOC_WARNING_ENABLE_REQ,  /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_SS_PRIM_GET_AOC_WARNING_ENABLE_CNF,  /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_SS_PRIM_SET_AOC_WARNING_ENABLE_REQ,  /**< \brief Enables or Disables the Advice of Charge (AoC) warning ("ACM near ACMmax") indication.
						 * \details If the Advice of Charge service is provisioned:
						   SAC can send a CI_CC_PRIM_AOC_WARNING_IND indication during a call when the Accumulated Charge Meter (ACM) is within 30 seconds of the programmed maximum value (ACMmax). This indication can be enabled or disabled by the subscriber.
						   In addition, this indication can be sent if a new incoming or outgoing call is set up when the ACM is within 30 seconds of the programmed ACMmax value.
						   If the Advice of Charge service is not provisioned, SAC will not send an AoC warning indication, even if the indication has been enabled.
						   The AoC warning indication is enabled by default.
						 * Not supported by SAC.
						 */
	CI_SS_PRIM_SET_AOC_WARNING_ENABLE_CNF, /**< \brief Confirms a request to Enable or Disable the Advice of Charge (AoC) warning ("ACM near ACMmax") indication.
					     * \details Not supported by SAC.*/
	CI_SS_PRIM_GET_SS_NOTIFY_OPTIONS_REQ,   /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_SS_PRIM_GET_SS_NOTIFY_OPTIONS_CNF,   /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_SS_PRIM_SET_SS_NOTIFY_OPTIONS_REQ, /**< \brief Sets the Enable/Disable options for Supplementary Service Intermediate (SSI) and Unsolicited (SSU) Notifications.
					     * \details SSI notifications (CI_CC_PRIM_SSI_NOTIFY_IND) and SSU notifications (CI_CC_PRIM_SSU_NOTIFY_IND) are both enabled by default.
					     * Not supported by SAC */
	CI_SS_PRIM_SET_SS_NOTIFY_OPTIONS_CNF, /**< \brief Reports the current Enable/Disable options for Supplementary Service Intermediate (SSI) and Unsolicited (SSU) Notifications.
					     * \details There is no reason for an unsuccessful result.
					     * Not supported by SAC.*/
	CI_SS_PRIM_GET_LOCALCB_LOCKS_REQ,               /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_SS_PRIM_GET_LOCALCB_LOCKS_CNF,               /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_SS_PRIM_GET_LOCALCB_LOCK_ACTIVE_REQ,         /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_SS_PRIM_GET_LOCALCB_LOCK_ACTIVE_CNF,         /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_SS_PRIM_SET_LOCALCB_LOCK_ACTIVE_REQ,         /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_SS_PRIM_SET_LOCALCB_LOCK_ACTIVE_CNF,         /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_SS_PRIM_SET_LOCALCB_NOTIFY_OPTION_REQ,       /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_SS_PRIM_SET_LOCALCB_NOTIFY_OPTION_CNF,       /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_SS_PRIM_CHANGE_CB_PASSWORD_REQ,  /**< \brief Requests the Call Barring (CB) Password to be changed.
					     * \details The Call Barring Supplementary Service may be provisioned with the option "Control of Barring Services by Subscriber using
					     *  Password". In this case, the Service Provider registers an initial (default) Password at provision time. The mobile subscriber
					     *  may change the Password at any time, using this request, and must provide the existing ("Old") Password, a replacement
					     *  ("New") Password, and optional verification of the new password. If new password verification is not supplied, the new
					     *  password is sent to the Network twice.
					     *  Each Password must be an array of digits. The Password length (CI_SS_CB_PASSWORD_LENGTH) is defined by the GSM
					     *  Standard.
					     *  The Network supports one Password per Subscriber, to be used for all Barring Services. Therefore, a Service Code (or
					     *  Facility Lock Identifier) is not required for this request.
					     *  A Registration request will be sent to the Network to register the New Password. Successful Registration replaces the Old
					     *  Password with the New Password, erasing the Old Password. Therefore, there is no need for a specific request to erase the
					     *  Call Blocking Password.
					     *  The Registration request will be rejected if:
					     * -	The Service was provisioned with the option "Control of Barring Services by Service Provider". In this situation, the subscriber cannot activate or deactivate the Call Barring Service.
					     * -	The service is not provisioned.
					     * -	The Old Password is invalid, or does not match the Password currently registered with the Network.
					     * -	The New Password is invalid.
					     * -	The verification of new password is present, but does not match the new password.
					     * The reject cause will be indicated in the Confirmation response to this Request.
					     * If this request fails, the Subscriber is allowed to retry it (with corrected Passwords), for up to three consecutive attempts.
					     * After that, all Call Barring requests that require a Password will be refused until the Password is "re-enabled" by the Service
					     * Provider.
					     */
	CI_SS_PRIM_CHANGE_CB_PASSWORD_CNF,  /**< \brief Confirms a request to change the Call Barring (CB) Password.
					     * \details The CIRC_SS_NOT_REGISTERED result code most likely indicates that no Password was registered when the Call Barring
					     *  Service was provisioned. This will be the case if the Service was provisioned with the option "Control of Barring Services by
					     *  Service Provider".*/
	CI_SS_PRIM_GET_CB_STATUS_REQ,       /**< \brief Requests Call Barring (CB) status information for a specified Class of Service.
					     * \details This status information may be requested only for a single Class of Service (e.g. Voice Service).
					     * An Interrogation request is sent to the Network*/
	CI_SS_PRIM_GET_CB_STATUS_CNF,       /**< \brief Confirms a request for  Call Barring (CB) status information for a specified Class of Service.
					     * \details The CIRC_SS_NOT_PROVISIONED result may indicate one of the following situations:
					     *  -	Call Barring was not provisioned at all;
					     *  -	Call Barring was provisioned with the option "Control of Barring Services by Service Provider". In this case, the
					     *       subscriber cannot activate or deactivate the Service.
					     */
	CI_SS_PRIM_SET_CB_ACTIVATE_REQ,     /**< \brief Requests Call Barring to be Activated or Deactivated for specified Class (or Classes) of Service.
					     * \details Requests Call Barring to be Activated or Deactivated for specified Class (or Classes) of Service.
					     *   If the Call Barring Supplementary Service was provisioned with the option "Control of Barring Services by Service Provider",
					     *   the subscriber is not allowed to activate or deactivate the Call Barring Service using this request. If this is attempted, the
					     *   Network will reject the request, and an appropriate error indication will be returned in the CI_SS_PRIM_SET_CB_ACTIVATE_CNF
					     *   response.
					     *   If the Call Barring Supplementary Service was provisioned with the option "Control of Barring Services by Subscriber using
					     *   Password", a Password may be required for this request.
					     *   An Activation or Deactivation request is sent to the Network, which may accept or reject it. The request will be rejected
					     *   if:
					     *   -	The Service was provisioned with the option "Control of Barring Services by Service Provider". In this situation, the
					     *        subscriber cannot activate or deactivate the Service.
					     *   -	The Service was not provisioned.
					     *   -	The current Network does not support Call Barring.
					     *   According to 3G TS 27.007, the following Call Barring Service Codes may be used only with a Deactivate request:
					     *   -	All Barring Services
					     *   -	All Outgoing Barring Services
					     *   -	All Incoming Barring Services
					     *   If an "Activate Call Barring service" request is used with any of these Service Codes, it will be rejected as an
					     *   "Invalid Request", and will not be sent to the Network.
					     *   If this request fails with a Password error, the Subscriber is allowed to reattempt it (with a corrected Password). If an
					     *   incorrect Password is entered more than three consecutive times, all Call Barring requests that require a Password will be
					     *   rejected (by the Network) until the Password is "re-enabled" by the Service Provider.
					     *   Call Barring (when activated) must not affect the subscriber's ability to place emergency calls.
					     *   The default (initial) Activation state is a SAC configuration issue. */
	CI_SS_PRIM_SET_CB_ACTIVATE_CNF,     /**< \brief Confirms a request to Activate or Deactivate Call Barring (CB) for a specified Class of Service.
					     * \details The CIRC_SS_NOT_REGISTERED result code most likely indicates that no Password was registered when the Call Barring
					     *  Service was provisioned. This will be the case if the Service was provisioned with the option "Control of Barring Services by
					     *  Service Provider". In this situation, the subscriber is not allowed to activate or deactivate the Call Barring Service using the
					     *  CI_SS_PRIM_SET_CB_ACTIVATE_REQ request.*/
	CI_SS_PRIM_GET_CB_TYPES_REQ,            /**< \brief Requests a list of supported Call Barring types (Service Codes).  \details */
	CI_SS_PRIM_GET_CB_TYPES_CNF,        /**< \brief Returns a list of supported Call Barring (CB) types (Service Codes).
					     * \details c.*/
	CI_SS_PRIM_GET_BASIC_SVC_CLASSES_REQ,   /**< \brief Requests a list of supported Basic Service Classes for Call Barring and Call Forwarding.  \details */
	CI_SS_PRIM_GET_BASIC_SVC_CLASSES_CNF, /**< \brief Returns a list of supported Basic Service Classes for Call Barring and Call Forwarding.
					     * \details There is no reason for an unsuccessful result.*/
	CI_SS_PRIM_GET_ACTIVE_CW_CLASSES_REQ,   /**< \brief Requests an indication of Basic Services for which Call Waiting is active.  \details */
	CI_SS_PRIM_GET_ACTIVE_CW_CLASSES_CNF, /**< \brief Returns an indication of Basic Services for which Call Waiting is active.
					     * \details The returned bitmap will indicate Call Waiting status for each of the requested services. If Call Waiting is active for a Basic
					     *  Service, the corresponding bit will be set to 1 in the returned bitmap. Otherwise, the bit will be set to zero.
					     *  For the ActClasses bitmap, only those bits with corresponding bits set to 1 in the ReqClasses bitmap are meaningful.
					     */

	CI_SS_PRIM_GET_CB_MAP_STATUS_REQ,   /**< \brief Requests an indication of Basic Services for the call barring status.  \details */
	CI_SS_PRIM_GET_CB_MAP_STATUS_CNF,   /**< \brief Returns an indication of Basic Services for the call barring status.
					       \details */

#if defined(CCI_POSITION_LOCATION)
	CI_SS_PRIM_PRIVACY_CTRL_REG_REQ,    /**< \brief Request to register support of privacy control services.
					     * \details */
	CI_SS_PRIM_PRIVACY_CTRL_REG_CNF,    /**< \brief Confirmation to the privacy control registration request.
					     * \details */
	CI_SS_PRIM_LOCATION_IND,                        /**< \brief Indication that the location information of the mobile was requested.  \details */
	CI_SS_PRIM_LOCATION_VERIFY_RSP,                 /**< \brief Response to the location verification indication  \details */
	CI_SS_PRIM_GET_LOCATION_REQ,                    /**< \brief Request to obtain the mobile's position location from the network.  \details */
	CI_SS_PRIM_GET_LOCATION_CNF,                    /**< \brief Confirmation to the get location request.  \details */
	CI_SS_PRIM_GET_LCS_NWSTATE_REQ,                 /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_SS_PRIM_GET_LCS_NWSTATE_CNF,                 /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_SS_PRIM_LCS_NWSTATE_CFG_IND_REQ,             /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_SS_PRIM_LCS_NWSTATE_CFG_IND_CNF,             /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_SS_PRIM_LCS_NWSTATE_IND,                     /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
#endif /* CCI_POSITION_LOCATION */

	CI_SS_PRIM_SERVICE_REQUEST_COMPLETE_IND, /**< \brief Supplementary Service Request Complete notification.
					     * \details This notification is sent in reply to a Supplementary Service Request and it's based on the status information returned by the protocol stack.
					     * The returned status is decoded from the Facility Information Element, when present, by the protocol stack as follows: ACTIVE, REGISTERED, PROVISIONED, QUIESCENT. */
	CI_SS_PRIM_GET_COLR_STATUS_REQ,         /**< \brief Interrogation of the CoLR (Connected Line Identification Restriction) Service support  \details */
	CI_SS_PRIM_GET_COLR_STATUS_CNF,         /**< \brief Confirmation to the previous interrogation of the CoLR (Connected Line Identification Restriction) Service support.  \details */

	/*Michal Bukai - CDIP support - Start:*/
	CI_SS_PRIM_GET_CDIP_STATUS_REQ,     /**< \brief Requests status information for the CDIP Supplementary Service.
					     * \details Requests the local CDIP presentation option setting and interrogates the CDIP provision status at the Network.
					       If enabled, CDIP information is reported by the CI_CC_PRIM_CDIP_INFO_IND indication.
					       If the CDIP service is provisioned, local CDIP presentation is enabled by default.
					       There should be no reason for an unsuccessful result.  */
	CI_SS_PRIM_GET_CDIP_STATUS_CNF,     /**< \brief Returns status information for the CDIP Supplementary Service.
					     * \details Reports the local CDIP presentation option setting and the CDIP provision status at the Network.
					     * If the CDIP service is provisioned, local CDIP presentation is enabled by default.
					     *  There is no reason for an unsuccessful result.*/
	CI_SS_PRIM_SET_CDIP_OPTION_REQ,     /**< \brief Sets the local presentation option for CDIP information on incoming calls.
					     * \details This option affects only the local presentation of CDIP information (at the mobile);
					     * it does not affect the operation of the CDIP service at the Network itself.
					     * The provisioned setting for CDIP is unchanged.
					     * By default CDIP presentation is enabled.
					     * This request will fail if CDIP service is not provisioned. */
	CI_SS_PRIM_SET_CDIP_OPTION_CNF,     /**< \brief Confirms a request to set the local presentation option for CDIP information on incoming calls.
					     * \details This option affects only the local presentation of CDIP information (at the mobile);
					     * it does not affect the operation of the CDIP service at the Network itself.
					     * The provisioned setting for CDIP is unchanged*/
	/*Michal Bukai - CDIP support - End*/
	/* ADD NEW COMMON PRIMITIVES HERE, BEFORE 'CI_SS_PRIM_LAST_COMMON_PRIM' */

	/* END OF COMMON PRIMITIVES LIST */
	CI_SS_PRIM_LAST_COMMON_PRIM

	/* the  customer specific extension primitives will be added starting from
	 * CI_SS_PRIM_FIRST_CUST_PRIM = CI_SS_PRIM_LAST_COMMON_PRIM as the first identifier.
	 * The actual primitive names and IDs are defined in the associated
	 * 'ci_ss_cust_xxx.h' file.
	 */

	/* DO NOT ADD ANY MORE PRIMITIVES HERE */

} _CiSsPrim;

/* specify the number of default common CC primitives */
#define CI_SS_NUM_COMMON_PRIM ( CI_SS_PRIM_LAST_COMMON_PRIM - 1 )
/**@}*/


/** \brief Supplementary Services Return Codes. */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIRC_SS {
	CIRC_SS_SUCCESS = 0,                    /**< Request completed successfully */
	CIRC_SS_FAIL,                           /**< General Failure (catch-all, if needed) */
	CIRC_SS_NOT_PROVISIONED,                /**< Supplementary Service not provisioned  */
	CIRC_SS_NOT_REGISTERED,                 /**<* Supplementary Service not registered */
	CIRC_SS_REJECTED,                       /**< Request rejected by Network  */
    CIRC_SS_INCORRECT_PWD,              /**< Incorrect password */
	/* This one must always be last in the list! */
	CIRC_SS_NUM_RESCODES                    /**< Number of result codes defined */
} _CiSsResultCode;
/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief Result code
 *  \sa CIRC_SS
 * \remarks Common Data Section */
typedef UINT16 CiSsResultCode;
/**@}*/

/** \brief Basic Service Class Map allocations -for Supplementary Services */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CISS_BASIC_SERVICE_MAP
{
	CISS_BSMAP_NONE       = 0x00,                         /**< No Services specified */     // SCR #1994470
	CISS_BSMAP_VOICE      = 0x01,                                                           /**< Voice Service  */
	CISS_BSMAP_DATA       = 0x02,                                                           /**< Data Service */
	CISS_BSMAP_FAX        = 0x04,                                                           /**< Fax Service  */
	CISS_BSMAP_SMS        = 0x08,                                                           /**< Short Message Service (SMS)  */
	CISS_BSMAP_DATA_SYNC  = 0x10,                                                           /**< Data circuit sync  */
	CISS_BSMAP_DATA_ASYNC = 0x20,                                                           /**< Data circuit async */
	CISS_BSMAP_PACKET     = 0x40,                                                           /**< Dedicated packet access  */
	CISS_BSMAP_PAD        = 0x80,                                                           /**< Dedicated PAD access */
	CISS_BSMAP_ALL        = 0xFF /**< All Services */                                       // SCR #1994470

} _CiSsBasicServiceMap;
/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief Bit map for SS basic services
 *  \sa CISS_BASIC_SERVICE_MAP
 * \remarks Common Data Section */
typedef UINT8 CiSsBasicServiceMap;
/**@}*/



/* Default BasicServiceMap is (Voice + Data + Fax)  */
#define CISS_BSMAP_DEFAULT  ( CISS_BSMAP_VOICE | CISS_BSMAP_DATA | CISS_BSMAP_FAX )

/* Calling Party (Caller) Information - for CLIP and CoLP indications */
/* ** SCR #1247683: CiSsCallerInfo definition removed */

/* CLI Validity Indicators - used in CLIP and Call Waiting indications */
/* ** SCR #1247683: CiSsCliValidity definition removed */

/* Information for CLIP indications */
/* ** SCR #1247683: CiSsClipInfo definition removed */

/* Information for CoLP indications */
/* ** SCR #1247683: CiSsColpInfo definition removed */

/* Information for Call Waiting (CW) indications */
/* ** SCR #1247683: CiSsCwInfo definition removed */


/** \brief CUG Temporary (Override) Configuration Information.
 *  \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiSsCugConfigInfo_struct {
	UINT8 TempCugIndex;             /**< Temporary CUG Index  */
	UINT8 TempCugInfo;              /**< Temporary CUG Information (Suppression)  */
} CiSsCugConfigInfo;

/* Temporary CUG Mode (see the "AT+CCUG" command) */
#define CI_CUG_TEMPMODE_DISABLED          0     /* Temporary Mode Disabled                  */
#define CI_CUG_TEMPMODE_ENABLED           1     /* Temporary Mode Enabled                   */

/* Temporary CUG Index - the valid range of values should already be defined elsewhere! */
#define CI_CUG_TEMPINDEX_NONE             10    /* No Index (from "AT+CCUG" command)        */

/* Temporary CUG Information (Suppression) is a bitmap. More than one bit may be set at a time */
#define CI_CUG_TEMPINFO_NONE              0x00  /* No Info                                  */
#define CI_CUG_TEMPINFO_SUPPRESS_OA       0x01  /* Suppress Outgoing Access (OA)            */
#define CI_CUG_TEMPINFO_SUPPRESS_PREF_CUG 0x02  /* Suppress Preferential (Provisioned) CUG  */

/** Call Forwarding: Reasons (Conditions) supported by CI.
 *    We may not support all of the "reason" values defined for "AT+CCFC" in [3].
 */
#define CICF_MAX_REASONS  6             /**< As defined by the "AT+CCFC" command  */


/** \brief CF reasons */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CICF_REASON {
	CICF_REASON_CFU = 0,                    /**< Call Forward Unconditional (CFU) */
	CICF_REASON_CFB,                        /**< Call Forward on User Busy (CFB)  */
	CICF_REASON_CFNRY,                      /**< Call Forward on No Reply (CFNRy) */
	CICF_REASON_CFNRC,                      /**< Call Forward on Not Reachable (CFNRc)  */
	CICF_REASON_ALL,                        /**< All Call Forwarding  */
	CICF_REASON_ALL_CONDITIONAL,            /**< All Conditional Call Forwarding  */

	/* This one must always be last! */
	CICF_NUM_REASONS                        /**< Number of supported CF reasons */

} _CiSsCfReason;
/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief CF reason
 *  \sa CICF_REASON
 * \remarks Common Data Section */
typedef UINT8 CiSsCfReason;
/**@}*/

/* Call Forwarding: CFNRy Timer definitions. See [10]. */
#define CI_CFNRY_TIMER_MIN      5       /* Minimum Timer Duration (seconds) */
#define CI_CFNRY_TIMER_MAX      30      /* Maximum Timer Duration (seconds) */
#define CI_CFNRY_TIMER_STEP     5       /* Timer Duration step interval (seconds) */
#define CI_CFNRY_TIMER_DEFAULT  20      /* Default Timer Duration (seconds) */

/** \brief Call Forwarding: Information for Registration requests to the Network. */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiSsCfRegisterInfo_struct {
	CiSsCfReason Reason;                                    /**< Call Forwarding Reason (Condition). \sa CiSsCfReason */
	CiSsBasicServiceMap Classes;                            /**< Classes of Service (bitmap). \sa CiSsBasicServiceMap  */
	CiAddressInfo Number;                                   /**< Call Forwarding Number. \sa CCI API Ref Manual  */
	CiSubaddrInfo OptSubaddr;                               /**< Optional Subaddress. \sa CCI API Ref Manual */
	UINT8 CFNRyDuration;                                    /**< CFNRy Timer Duration (seconds) */
} CiSsCfRegisterInfo;

/** \brief Call Forwarding: Information for Erasure requests to the Network */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiSsCfEraseInfo_struct {
	CiSsCfReason Reason;                            /**< Call Forwarding Reason (Condition). \sa CiSsCfReason */
	CiSsBasicServiceMap Classes;                    /**< Classes of Service (bitmap). \sa CiSsBasicServiceMap  */
} CiSsCfEraseInfo;

/** \brief  Basic Service Class definitions -for Supplementary Services */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CISS_BASIC_SERVICE
{
	CISS_SERVICE_NONE = 0,              /* No Services specified */ // SCR #1994470
	CISS_SERVICE_VOICE,                                             /* Voice Service  */
	CISS_SERVICE_DATA,                                              /* Data Service */
	CISS_SERVICE_FAX,                                               /* Fax Service  */
	CISS_SERVICE_SMS,                                               /* Short Message Service (SMS)  */
	CISS_SERVICE_DATA_SYNC,                                         /* Data circuit sync  */
	CISS_SERVICE_DATA_ASYNC,                                        /* Data circuit async */
	CISS_SERVICE_PACKET,                                            /* Dedicated packet access  */
	CISS_SERVICE_PAD,                                               /* Dedicated PAD access */
	/* This one must always be last in the list! */
	CISS_NUM_SERVICES                                               /* Number of Basic Service options supported */
} _CiSsBasicService;


/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief Basic Service Class -for Supplementary Services
 *  \sa CISS_BASIC_SERVICE
 * \remarks Common Data Section */
typedef UINT8 CiSsBasicService;
/**@}*/



/** \brief USSD Status definitions. See the "AT+CUSD" command in [4]. */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CISS_USSD_STATUS {
	CISS_USSD_STATUS_NO_INFO_NEEDED = 0,                    /**< No further info needed for MO operation  */
	CISS_USSD_STATUS_MORE_INFO_NEEDED,                      /**< Further info needed for MO operation */
	CISS_USSD_STATUS_TERMINATED,                            /**< USSD terminated by Network */
	CISS_USSD_STATUS_OTHER_CLIENT_RESP,                     /**< Other client has responded */
	CISS_USSD_STATUS_NOT_SUPPORTED,                         /**< Operation not supported  */
	CISS_USSD_STATUS_TIMEOUT,                               /**< Network timeout  */
	CISS_USSD_STATUS_INPROGRESS,                           /**< PHASE 2    fail and retry phase 1 */
	/* << Define additional allocations here >> */

	/* This one must always be last in the list! */
	CISS_NUM_USSD_STATUS                                            /**< Number of USSD Status codes defined  */

} _CiSsUssdStatus;
/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief USSD Status
 *  \sa CISS_USSD_STATUS
 * \remarks Common Data Section */
typedef UINT8 CiSsUssdStatus;
/**@}*/



/* SCR #1698551: USSD Character Set definitions. See the AT+CSCS command (TS 27.007) */
//ICAT EXPORTED ENUM
typedef enum CISS_USSD_CHARSET
{
	/* Character Set codes for MT-USSD */
	CISS_USSD_UNKNOWN = 0,          /* Unknown Representation */
	CISS_USSD_CHARSET_IRA,          /* IRA "ASCII" Text String */
	CISS_USSD_CHARSET_HEX,          /* Hexadecimal Digit Character String */
	CISS_USSD_CHARSET_USER,         /* User-Specified Representation */

	/* Character Set codes for MO-USSD */
	CISS_USSD_CHARSET_MO_GSM7BIT,   /* IRA Text -> GSM 7-bit */
	CISS_USSD_CHARSET_MO_UCS2,      /* Hex Digit String -> UCS2 */

	/* << Define additional Character Set definitions here >> */

	/* This one must always be last in the list! */
	CISS_NUM_USSD_CHARSETS          /* Number of USSD Character Set codes defined */

} _CiUssdCharSet;

typedef UINT8 CiUssdCharSet;

/** \brief USSD: Unstructured Supplementary Service Data Information */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiSsUssdInfo_struct {
	CiUssdCharSet CharSet; /**< Data Character Set indicator */                                             // SCR #1698551
	UINT8 DataLength;                                                                                       /**< USS Data length (characters) */
	INT8 Data[ CI_MAX_USSD_LENGTH ];                                                                        /* USS Data */
} CiSsUssdInfo;

/** \brief  Network Call Barring: Service Codes (Call Barring Types). */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CISS_CB_SERVICE_CODE {
	CISS_CB_LOCK_BAOC = 0,          /* Bar All Outgoing Calls.  */
	CISS_CB_LOCK_BOIC,              /* Bar Outgoing International Calls.  */
	CISS_CB_LOCK_BOIC_EXHC,         /* Bar Outgoing International Calls,  */
					/*  except to the Home PLMN Country.  */
	CISS_CB_LOCK_BAIC,              /* Bar All Incoming Calls.  */
	CISS_CB_LOCK_BAIC_ROAMING,      /* Bar Incoming Calls when Roaming outside  */
					/*  of the Home PLMN Country. */
	CISS_CB_LOCK_ALL,               /* All Barring Services.  */
	CISS_CB_LOCK_ALL_OUTGOING,      /* All Outgoing Barring Services */
	CISS_CB_LOCK_ALL_INCOMING,      /* All Incoming Barring Services */

	/* This one must always be last in the list! */

	CISS_NUM_CB_LOCK_CODES /* Number of Call Barring Codes */

} _CiSsCbServiceCode;
/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief Call Barring service code
 *  \sa CISS_CB_SERVICE_CODE
 * \remarks Common Data Section */
typedef UINT16 CiSsCbServiceCode;
/**@}*/



/* Indicator for "No Call Barring Lock in use" */
#define CI_SS_CB_LOCK_NO_LOCK     ( (CiSsCbServiceCode)(-1) )
#define CI_SS_CB_PASSWORD_LENGTH  4 /* Required Password length (digits only) */

/* Local Call Barring: Facility (Lock) Identifiers (applied to incoming calls only).
 * Some appear to be identified in the "AT+CLCK" command (see [3]). The Protocol Stack may
 * not support all (or any) of them - and it may support others.
 *
 * The entire Local Call Barring facility needs further investigation.
 * See [13] Section A.19, which talks about a Barred Dialing List (BDN) in the SIM.
 */
//ICAT EXPORTED ENUM
typedef enum CISS_LOCALCB_LOCK_ID {
	CISS_LOCALCB_LOCK_NM = 0,       /* Bar caller numbers not in ME Phonebook */
	CISS_LOCALCB_LOCK_NS,           /* Bar caller numbers not in SIM Phonebook  */
	CISS_LOCALCB_LOCK_NA,           /* Bar caller numbers not in ME or SIM PB */

	/* << Put any additional definitions here >> */

	/* This one must always be last in the list! */
	CISS_NUM_LOCALCB_LOCKS /* Number of Local Call Barring Lock IDs  */

} _CiSsLocalCbLockId;
typedef UINT8 CiSsLocalCbLockId;

#if defined(CCI_POSITION_LOCATION)

/* Constant definitions for position location */


//ICAT EXPORTED ENUM
typedef enum CISS_LCS_NW_STATE {
	CISS_LCS_AVAILABLE = 0,
	CISS_LCS_NOT_AVAILABLE,
	CISS_LCS_TEMPORARY_NOT_AVAILABLE,

	/* This one must always be last in the list! */
	CISS_NUM_LCS_NW_STATE
} _CiSsLcsNwState;
typedef UINT8 CiSsLcsNwState;



//ICAT EXPORTED STRUCT
typedef struct CiSsPrivateExtension_struct {
	CiBoolean present;
	UINT8 extId;
	UINT8 extType;
} CiSsPrivateExtension;

//ICAT EXPORTED STRUCT
typedef struct CiSsPcsExtensions_struct {
	/* TBD: 3gpp spec 24.080 does not really define anything for this field */
	UINT8 pcsExtention;
} CiSsPcsExtensions;

//ICAT EXPORTED ENUM
typedef enum CISS_SUPP_GAD_SHAPES {
	CISS_ELLIPSOID_PT                    = 0x01,
	CISS_ELLIPSOID_PT_UNCERT_CIRCLE      = 0x02,
	CISS_ELLIPSOID_PT_UNCERT_ELLIPSE     = 0x04,
	CISS_POLYGON                         = 0x08,
	CISS_ELLIPSOID_PT_ALT                = 0x10,
	CISS_ELLIPSOID_PT_ALT_UNCERT_ELIPSOID = 0x20,
	CISS_ELLIPSOID_ARC                   = 0x40,
} _CiSsSuppGadShapes;

typedef UINT8 CiSsSuppGadShapes;
#endif /* CCI_POSITION_LOCATION */

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_GET_CLIP_STATUS_REQ">   */
typedef CiEmptyPrim CiSsPrimGetClipStatusReq;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_GET_CLIP_STATUS_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimGetClipStatusCnf_struct {
	CiSsResultCode Result;          /**< Result code. \sa CiSsResultCode*/
	UINT8 Local;                    /**< Local CLIP enable/disable status */
	UINT8 Provision;                /**< CLIP provision status */
} CiSsPrimGetClipStatusCnf;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_SET_CLIP_OPTION_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSetClipOptionReq_struct {
	UINT8 Local;    /**< 0 - Disable local CLIP presentation; 1 -Enable local CLIP presentation. */
} CiSsPrimSetClipOptionReq;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_SET_CLIP_OPTION_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSetClipOptionCnf_struct {
	CiSsResultCode Result;          /**< Result code. \sa CiSsResultCode */
} CiSsPrimSetClipOptionCnf;

/*Michal Bukai - CDIP support - Start:*/
/** <paramref name="CI_SS_PRIM_GET_CDIP_STATUS_REQ">   */
typedef CiEmptyPrim CiSsPrimGetCdipStatusReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_SS_PRIM_GET_CDIP_STATUS_CNF">   */
typedef struct CiSsPrimGetCdipStatusCnf_struct {
	CiSsResultCode Result;  /**< Result code. \sa CiSsResultCode */
	UINT8 Local;            /**< Local CDIP Enable/Disable Status. */
	UINT8 Provision;        /**< CDIP Provision Status. */
} CiSsPrimGetCdipStatusCnf;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_SS_PRIM_SET_CDIP_OPTION_REQ">   */
typedef struct CiSsPrimSetCdipOptionReq_struct {
	UINT8 Local; /**< Local setting of CDIP Status Permitted values are Enable (1)/Disable (0). */
} CiSsPrimSetCdipOptionReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_SS_PRIM_SET_CDIP_OPTION_CNF">   */
typedef struct CiSsPrimSetCdipOptionCnf_struct {
	CiSsResultCode Result; /**< Result code. \sa CiSsResultCode */
} CiSsPrimSetCdipOptionCnf;
/*Michal Bukai - CDIP support - End*/
/* <INUSE> */
/** <paramref name="CI_SS_PRIM_GET_CLIR_STATUS_REQ">   */
typedef CiEmptyPrim CiSsPrimGetClirStatusReq;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_GET_CLIR_STATUS_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimGetClirStatusCnf_struct {
	CiSsResultCode Result;          /**< Result code. \sa CiSsResultCode */
	UINT8 Local;                    /**< Local CLIR Presentation (Invoke/Suppress/Revoke) Status */
	UINT8 Provision;                /**< CLIR Provision Status */
} CiSsPrimGetClirStatusCnf;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_SET_CLIR_OPTION_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSetClirOptionReq_struct {
	UINT8 Local;    /**< CLIR presentation as per Provision (revoke temporary setting),
			 *    invoke CLIR (temporary setting = restrict presentation),
			 *    Suppress CLIR (temporary setting = allow presentation) [range: 0-2] */
} CiSsPrimSetClirOptionReq;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_SET_CLIR_OPTION_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSetClirOptionCnf_struct {
	CiSsResultCode Result;          /**< Result code. \sa CiSsResultCode */
} CiSsPrimSetClirOptionCnf;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_GET_COLP_STATUS_REQ">   */
typedef CiEmptyPrim CiSsPrimGetColpStatusReq;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_GET_COLP_STATUS_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimGetColpStatusCnf_struct {
	CiSsResultCode Result;                                  /**< Result code. \sa CiSsResultCode */
	UINT8 Local;                                            /**< Local CoLP Presentation (Enable/Disable) Status. [range: 0-1] */
	UINT8 Provision;                                        /**< CoLP Provision Status. [range: 0-2]  */
} CiSsPrimGetColpStatusCnf;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_SET_COLP_OPTION_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSetColpOptionReq_struct {
	UINT8 Local;            /**< 0 -Disable local CoLP presentation; 1 - Enable local CoLP presentation. */
} CiSsPrimSetColpOptionReq;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_SET_COLP_OPTION_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSetColpOptionCnf_struct {
	CiSsResultCode Result;          /**< Result code. \sa CiSsResultCode */
} CiSsPrimSetColpOptionCnf;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_GET_CUG_CONFIG_REQ">   */
typedef CiEmptyPrim CiSsPrimGetCugConfigReq;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_GET_CUG_CONFIG_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimGetCugConfigCnf_struct {
	CiSsResultCode Result;          /**< Result code. \sa CiSsResultCode */
	UINT8 TempMode;                 /**< 0 -CUG Temporary Mode disabled; 1- CUG Temporary Mode enabled. */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_SS_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_SS_CONTIGUOUS
	CiSsCugConfigInfo   *Info; /**< Temporary (override) CUG configuration information. \sa CiSsCugConfigInfo_struct */
#else /* CCI_SS_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
	/*Temporary pointer used to ensure the correct structure size for transmission
	   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8* tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */
	/* # Start Contiguous Code Section # */
	CiSsCugConfigInfo info;                 /**< Temporary (override) CUG configuration information. \sa CiSsCugConfigInfo_struct */
	/* # End Contiguous Code Section # */
#endif  /* CCI_SS_CONTIGUOUS */
} CiSsPrimGetCugConfigCnf;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_SET_CUG_CONFIG_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSetCugConfigReq_struct {
	UINT8 TempMode;                 /**< 0 -Disable CUG Temporary Mode; 1 - Enable CUG Temporary Mode. */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_SS_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_SS_CONTIGUOUS
	CiSsCugConfigInfo   *Info;      /**< Temporary (override) CUG configuration information. \sa CiSsCugConfigInfo_struct */
#else /* CCI_SS_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
	/*Temporary pointer used to ensure the correct structure size for transmission
	   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8* tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */
	/* # Start Contiguous Code Section # */
	CiSsCugConfigInfo info;                 /**< Temporary (override) CUG configuration information. \sa CiSsCugConfigInfo_struct */
	/* # End Contiguous Code Section # */
#endif  /* CCI_SS_CONTIGUOUS */
} CiSsPrimSetCugConfigReq;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_SET_CUG_CONFIG_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSetCugConfigCnf_struct {
	CiSsResultCode Result;          /**< Result code. \sa CiSsResultCode */
} CiSsPrimSetCugConfigCnf;

///////////////////////Michal Bukai - CNAP support

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_GET_CNAP_STATUS_REQ">   */
typedef CiEmptyPrim CiSsPrimGetCnapStatusReq;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_GET_CNAP_STATUS_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimGetCnapStatusCnf_struct {
	CiSsResultCode Result;          /**< Result code. \sa CiSsResultCode */
	UINT8 Local;                    /**< Local CNAP Presentation (Invoke/Suppress/Revoke) Status */
	UINT8 Provision;                /**< CNAP Provision Status */
} CiSsPrimGetCnapStatusCnf;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_SET_CNAP_OPTION_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSetCnapOptionReq_struct {
	UINT8 Local;                    /**< Local CNAP Presentation (Invoke/Suppress/Revoke) Status */
} CiSsPrimSetCnapOptionReq;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_SET_CNAP_OPTION_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSetCnapOptionCnf_struct {
	CiSsResultCode Result;          /**< Result code. \sa CiSsResultCode */
} CiSsPrimSetCnapOptionCnf;

///////////////////////Michal Bukai - CNAP support

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_REGISTER_CF_INFO_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimRegisterCfInfoReq_struct {
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_SS_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_SS_CONTIGUOUS
	CiSsCfRegisterInfo   *Info; /**< Call Forwarding Registration information. \sa CiSsCfRegisterInfo_struct */
#else /* CCI_SS_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
	/*Temporary pointer used to ensure the correct structure size for transmission
	   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8* tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */
	/* # Start Contiguous Code Section # */
	CiSsCfRegisterInfo info;                /**< Call Forwarding Registration information. \sa CiSsCfRegisterInfo_struct */
	/* # End Contiguous Code Section # */
#endif  /* CCI_SS_CONTIGUOUS */
} CiSsPrimRegisterCfInfoReq;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_REGISTER_CF_INFO_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimRegisterCfInfoCnf_struct {
	CiSsResultCode Result;          /**< Result code. \sa CiSsResultCode */
} CiSsPrimRegisterCfInfoCnf;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_ERASE_CF_INFO_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimEraseCfInfoReq_struct {
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_SS_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_SS_CONTIGUOUS
	CiSsCfEraseInfo *Info;  /**< Call Forwarding Erasure information. \sa CiSsCfEraseInfo_struct */
#else /* CCI_SS_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
	/*Temporary pointer used to ensure the correct structure size for transmission
	   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8* tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */
	/* # Start Contiguous Code Section # */
	CiSsCfEraseInfo info;                   /**< Call Forwarding Erasure information. \sa CiSsCfEraseInfo_struct */
	/* # End Contiguous Code Section # */
#endif  /* CCI_SS_CONTIGUOUS */
} CiSsPrimEraseCfInfoReq;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_ERASE_CF_INFO_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimEraseCfInfoCnf_struct {
	CiSsResultCode Result;          /**< Result code. \sa CiSsResultCode */
} CiSsPrimEraseCfInfoCnf;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_GET_CF_REASONS_REQ">   */
typedef CiEmptyPrim CiSsPrimGetCfReasonsReq;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_GET_CF_REASONS_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimGetCfReasonsCnf_struct {
	CiSsResultCode Result;                                                  /**< Result code. \sa CiSsResultCode */
	UINT8 NumReasons;                                                       /**< Number of supported CF reasons [range: 0..CICF_NUM_REASONS] */
	UINT8 Reasons[ CICF_MAX_REASONS ];                                      /**< List of supported CF reasons. \sa CICF_REASON*/
} CiSsPrimGetCfReasonsCnf;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_SET_CF_ACTIVATION_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSetCfActivationReq_struct {
	UINT8 Reason;                                           /**< CF Reason (Condition) to be activated or deactivated. \sa CICF_REASON. */
	CiSsBasicServiceMap Classes;                            /**< Class (or Classes) of Service for which CF is to be activated or deactivated. \sa CiSsBasicServiceMap  */
	CiBoolean Activate;                                     /**< Activates the Call Forwarding service. Deactivates the Call Forwarding service. \sa CCI API Ref Manual */
} CiSsPrimSetCfActivationReq;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_SET_CF_ACTIVATION_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSetCfActivationCnf_struct {
	CiSsResultCode Result;                          /**< Result code. \sa CiSsResultCode */
} CiSsPrimSetCfActivationCnf;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_INTERROGATE_CF_INFO_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimInterrogateCfInfoReq_struct {
	UINT8 Reason;                           /**< CF Reason (Condition) to be interrogated. \sa CICF_REASON. */
	CiSsBasicServiceMap Classes;            /**< Class (or Classes) of Service.\sa CiSsBasicServiceMap */
} CiSsPrimInterrogateCfInfoReq;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_INTERROGATE_CF_INFO_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimInterrogateCfInfoCnf_struct {
	CiSsResultCode Result;                  /**< Result code. \sa CiSsResultCode */
	UINT8 Status;                           /**< 0 - Call Forwarding not active; 1- Call Forwarding active. */
	UINT8 NumCfInfo;                        /**< Number of Info structures */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_SS_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_SS_CONTIGUOUS
	CiSsCfRegisterInfo    *Info;                /**< Array of CF Registration Information structures, for various Basic Services. \sa CiSsCfRegisterInfo_struct */
#else /* CCI_SS_CONTIGUOUS */
	/* # Start Contiguous Code Section # */
	CiSsCfRegisterInfo info[CISS_NUM_SERVICES];     /**< Array of CF Registration Information structures, for various Basic Services. \sa CiSsCfRegisterInfo_struct */
	/* # End Contiguous Code Section # */
#endif /* CCI_SS_CONTIGUOUS */
} CiSsPrimInterrogateCfInfoCnf;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_SET_CW_ACTIVATION_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSetCwActivationReq_struct {
	CiBoolean Activate;                             /**< TRUE - Activate Call Waiting service; FALSE -Deactivate Call Forwarding service. \sa CCI API Ref Manual*/
	CiSsBasicServiceMap Classes;                    /**< Class (or Classes) of Service for which Call Waiting is to be activated or deactivated. \sa CiSsBasicServiceMap */
} CiSsPrimSetCwActivationReq;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_SET_CW_ACTIVATION_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSetCwActivationCnf_struct {
	CiSsResultCode Result;          /**< Result code. \sa CiSsResultCode */
} CiSsPrimSetCwActivationCnf;

typedef CiEmptyPrim CiSsPrimGetCwOptionReq;

//ICAT EXPORTED STRUCT
typedef struct CiSsPrimGetCwOptionCnf_struct {
	CiSsResultCode Result;
	UINT8 Local;
} CiSsPrimGetCwOptionCnf;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_SS_PRIM_SET_CW_OPTION_REQ">   */
typedef struct CiSsPrimSetCwOptionReq_struct {
	CiBoolean Local;        /**< Enables/ disables local presentation of CW indications. \sa CCI API Ref Manual */
} CiSsPrimSetCwOptionReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_SS_PRIM_SET_CW_OPTION_CNF">   */
typedef struct CiSsPrimSetCwOptionCnf_struct {
	CiSsResultCode Result;  /**< Result code. \sa CiSsResultCode */
} CiSsPrimSetCwOptionCnf;

//ICAT EXPORTED STRUCT
typedef struct CiSsPrimGetCwActiveStatusReq_struct {
	CiSsBasicService Class;
} CiSsPrimGetCwActiveStatusReq;

//ICAT EXPORTED STRUCT
typedef struct CiSsPrimGetCwActiveStatusCnf_struct {
	CiSsResultCode Result;
	CiSsBasicService Class;
	UINT8 Status;
} CiSsPrimGetCwActiveStatusCnf;

typedef CiEmptyPrim CiSsPrimGetUssdEnableReq;

//ICAT EXPORTED STRUCT
typedef struct CiSsPrimGetUssdEnableCnf_struct {
	CiSsResultCode Result;
	CiBoolean Enabled;
} CiSsPrimGetUssdEnableCnf;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_SS_PRIM_SET_USSD_ENABLE_CNF">   */
typedef struct CiSsPrimSetUssdEnableReq_struct {
	CiBoolean Enable;       /**< Enable / disable USSD status indication. \sa CCI API Ref Manual */
} CiSsPrimSetUssdEnableReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_SS_PRIM_SET_USSD_ENABLE_CNF">   */
typedef struct CiSsPrimSetUssdEnableCnf_struct {
	CiSsResultCode Result;  /**< Result code. \sa CiSsResultCode */
} CiSsPrimSetUssdEnableCnf;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_RECEIVED_USSD_INFO_IND">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimReceivedUssdInfoInd_struct {
	CiSsUssdStatus Status;          /**< USSD status indication. \sa CiSsUssdStatus */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_SS_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_SS_CONTIGUOUS
	CiSsUssdInfo          *Info; /**< Incoming USSD information. \sa CiSsUssdInfo_struct */
#else /* CCI_SS_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
	/*Temporary pointer used to ensure the correct structure size for transmission
	   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8                 *tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */
	/* # Start Contiguous Code Section # */
	CiSsUssdInfo info;              /**< Incoming USSD information. \sa CiSsUssdInfo_struct */
	/* # End Contiguous Code Section # */
#endif  /* CCI_SS_CONTIGUOUS */
} CiSsPrimReceivedUssdInfoInd;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_RECEIVED_USSD_INFO_RSP">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimReceivedUssdInfoRsp_struct {
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_SS_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_SS_CONTIGUOUS
	CiSsUssdInfo          *Info; /**< Outgoing USSD information. \sa CiSsUssdInfo_struct */
#else /* CCI_SS_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
	/*Temporary pointer used to ensure the correct structure size for transmission
	   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8                 *tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */
	/* # Start Contiguous Code Section # */
	CiSsUssdInfo info;              /**< Outgoing USSD information. \sa CiSsUssdInfo_struct */
	/* # End Contiguous Code Section # */
#endif  /* CCI_SS_CONTIGUOUS */
} CiSsPrimReceivedUssdInfoRsp;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_START_USSD_SESSION_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimStartUssdSessionReq_struct {
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_SS_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_SS_CONTIGUOUS
	CiSsUssdInfo          *Info; /**< Outgoing USSD information. \sa CiSsUssdInfo_struct */
#else /* CCI_SS_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
	/*Temporary pointer used to ensure the correct structure size for transmission
	   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8                 *tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */
	/* # Start Contiguous Code Section # */
	CiSsUssdInfo info;                      /**< Outgoing USSD information. \sa CiSsUssdInfo_struct */
	/* # End Contiguous Code Section # */
#endif  /* CCI_SS_CONTIGUOUS */
} CiSsPrimStartUssdSessionReq;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_START_USSD_SESSION_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimStartUssdSessionCnf_struct {
	CiSsResultCode Result;          /**< Result code. \sa CiSsResultCode */
} CiSsPrimStartUssdSessionCnf;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_ABORT_USSD_SESSION_REQ">   */
typedef CiEmptyPrim CiSsPrimAbortUssdSessionReq;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_ABORT_USSD_SESSION_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimAbortUssdSessionCnf_struct {
	CiSsResultCode Result;          /**< Result code. \sa CiSsResultCode */
} CiSsPrimAbortUssdSessionCnf;

typedef CiEmptyPrim CiSsPrimGetCcmOptionReq;

//ICAT EXPORTED STRUCT
typedef struct CiSsPrimGetCcmOptionCnf_struct {
	CiSsResultCode Result;
	CiBoolean Enabled;
} CiSsPrimGetCcmOptionCnf;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_SS_PRIM_SET_CCM_OPTION_REQ">   */
typedef struct CiSsPrimSetCcmOptionReq_struct {
	CiBoolean Enable; /**< Enable / disable periodic CCM update indications. \sa CCI API Ref Manual */

} CiSsPrimSetCcmOptionReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_SS_PRIM_SET_CCM_OPTION_CNF">   */
typedef struct CiSsPrimSetCcmOptionCnf_struct {
	CiSsResultCode Result;  /**< Result code. \sa CiSsResultCode */
} CiSsPrimSetCcmOptionCnf;

typedef CiEmptyPrim CiSsPrimGetAocWarningEnableReq;

//ICAT EXPORTED STRUCT
typedef struct CiSsPrimGetAocWarningEnableCnf_struct {
	CiSsResultCode Result;
	CiBoolean Enabled;
} CiSsPrimGetAocWarningEnableCnf;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_SS_PRIM_SET_AOC_WARNING_ENABLE_REQ">   */
typedef struct CiSsPrimSetAocWarningEnableReq_struct {
	CiBoolean Enable; /**< Enable / disable AoC warning indication. \sa CCI API Ref Manual */
} CiSsPrimSetAocWarningEnableReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_SS_PRIM_SET_AOC_WARNING_ENABLE_CNF">   */
typedef struct CiSsPrimSetAocWarningEnableCnf_struct {
	CiSsResultCode Result; /**< Result code. \sa CiSsResultCode */
} CiSsPrimSetAocWarningEnableCnf;

typedef CiEmptyPrim CiSsPrimGetSsNotifyOptionsReq;

//ICAT EXPORTED STRUCT
typedef struct CiSsPrimGetSsNotifyOptionsCnf_struct {
	CiSsResultCode Result;
	CiBoolean SsiEnabled;
	CiBoolean SsuEnabled;
} CiSsPrimGetSsNotifyOptionsCnf;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_SS_PRIM_SET_SS_NOTIFY_OPTIONS_REQ">   */
typedef struct CiSsPrimSetSsNotifyOptionsReq_struct {
	CiBoolean SsiEnable;    /**< Enable / disable SSI (Supplementary Service Intermediate) notifications. \sa CCI API Ref Manual */
	CiBoolean SsuEnable;    /**< Enable / disable SSU (Supplementary Service Unsolicited) notifications. \sa CCI API Ref Manual */
} CiSsPrimSetSsNotifyOptionsReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_SS_PRIM_SET_SS_NOTIFY_OPTIONS_CNF">   */
typedef struct CiSsPrimSetSsNotifyOptionsCnf_struct {
	CiSsResultCode Result;  /**< Result code. \sa CiSsResultCode */
} CiSsPrimSetSsNotifyOptionsCnf;

typedef CiEmptyPrim CiSsPrimGetLocalCbLocksReq;

//ICAT EXPORTED STRUCT
typedef struct CiSsPrimGetLocalCbLocksCnf_struct {
	CiSsResultCode Result;
	UINT8 NumLocks;
	CiSsLocalCbLockId Facility[CISS_NUM_LOCALCB_LOCKS ];
} CiSsPrimGetLocalCbLocksCnf;

//ICAT EXPORTED STRUCT
typedef struct CiSsPrimGetLocalCbLockActiveReq_struct {
	CiSsLocalCbLockId Facility;
} CiSsPrimGetLocalCbLockActiveReq;

//ICAT EXPORTED STRUCT
typedef struct CiSsPrimGetLocalCbLockActiveCnf_struct {
	CiSsResultCode Result;
	CiBoolean Active;
} CiSsPrimGetLocalCbLockActiveCnf;

//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSetLocalCbLockActiveReq_struct {
	CiSsLocalCbLockId Facility;
	CiBoolean Enable;
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_SS_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_SS_CONTIGUOUS
	CiPassword   *Password;
#else   /* CCI_SS_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
	/*Temporary pointer used to ensure the correct structure size for transmission
	   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8* tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */
	/* # Start Contiguous Code Section # */
	CiPassword password;
	/* # End Contiguous Code Section # */
#endif  /* CCI_SS_CONTIGUOUS */
} CiSsPrimSetLocalCbLockActiveReq;

//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSetLocalCbLockActiveCnf_struct {
	CiSsResultCode Result;
} CiSsPrimSetLocalCbLockActiveCnf;

//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSetLocalCbNotifyOptionReq_struct {
	CiBoolean Enable;
} CiSsPrimSetLocalCbNotifyOptionReq;

//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSetLocalCbNotifyOptionCnf_struct {
	CiSsResultCode Result;
} CiSsPrimSetLocalCbNotifyOptionCnf;

/* <INUSE> */
//ICAT EXPORTED STRUCT
/** <paramref name="CI_SS_PRIM_CHANGE_CB_PASSWORD_REQ">   */
typedef struct CiSsPrimChangeCbPasswordReq_struct {
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_SS_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_SS_CONTIGUOUS
	CiPassword *OldPassword;                /**< Old (existing) Password. \sa CCI API Ref Manual */
	CiPassword *NewPassword;                /**< New (replacement) Password. \sa CCI API Ref Manual */
	CiPassword *NewPasswdVerification;      /**< Repeat of new password. \sa CCI API Ref Manual */

#else /* CCI_SS_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
	/*Temporary pointer used to ensure the correct structure size for transmission
	   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8* tempNonContPtr1;
	UINT8* tempNonContPtr2;
	UINT8* tempNonContPtr3;
#endif  /* CCI_APP_NONCONTIGUOUS */
	/* # Start Contiguous Code Section # */
	CiPassword oldPassword;                         /**< Old (existing) Password. \sa CCI API Ref Manual */
	CiPassword newPassword;                         /**< New (replacement) Password. \sa CCI API Ref Manual */
	CiPassword newPasswdVerification;               /**< Repeat of new password. \sa CCI API Ref Manual */
	/* # End Contiguous Code Section # */
#endif /* CCI_SS_CONTIGUOUS */
} CiSsPrimChangeCbPasswordReq;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_CHANGE_CB_PASSWORD_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimChangeCbPasswordCnf_struct {
	CiSsResultCode Result;          /**< Result code. \sa CiSsResultCode */
} CiSsPrimChangeCbPasswordCnf;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_GET_CB_STATUS_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimGetCbStatusReq_struct {
	CiSsCbServiceCode Service;              /**< Call Barring Service Code. \sa CiSsCbServiceCode */
	CiSsBasicService Class;                 /**< Class of Service for which CB status information is requested. \sa CiSsCbServiceCode */
} CiSsPrimGetCbStatusReq;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_GET_CB_STATUS_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimGetCbStatusCnf_struct {
	CiSsResultCode Result;          /**< Result code. \sa CiSsResultCode */
	CiBoolean Active;               /**< TRUE - Call Barring is active; FALSE - Call Barring is inactive.\sa CCI API Ref Manual */
} CiSsPrimGetCbStatusCnf;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_SET_CB_ACTIVATE_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSetCbActivationReq_struct {
	CiSsCbServiceCode Service;                      /**< Call Barring Service Code. \sa CiSsCbServiceCode */
	CiBoolean Activate;                             /**< TRUE - Activate Call Barring service; FALSE - Deactivate Call Barring service. \sa CCI API Ref Manual */
	CiSsBasicServiceMap Classes;                    /**< Class (or Classes) of Service for which Call Barring is to be activated or deactivated (bit map). \sa CiSsBasicServiceMap */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_SS_CONTIGUOUS
 * flags & CCI_APP_NONCONTIGUOUS must be removed.
 * The backwards compatibility code must also be removed.
 * Must maintained only the code between the following 2 comment lines:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 */
#ifndef CCI_SS_CONTIGUOUS
	CiPassword   *Password; /**< Call Barring Password (if required). \sa CCI API Ref Manual */
#else /* CCI_SS_CONTIGUOUS */
#ifdef CCI_APP_NONCONTIGUOUS
	/*Temporary pointer used to ensure the correct structure size for transmission
	   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8* tempNonContPtr;
#endif  /* CCI_APP_NONCONTIGUOUS */
	/* # Start Contiguous Code Section # */
	CiPassword password;                    /**< Call Barring Password (if required). \sa CCI API Ref Manual */
	/* # End Contiguous Code Section # */
#endif  /* CCI_SS_CONTIGUOUS */
} CiSsPrimSetCbActivationReq;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_SET_CB_ACTIVATE_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSetCbActivateCnf_struct {
	CiSsResultCode Result;          /**< Result code. \sa CiSsResultCode */
} CiSsPrimSetCbActivateCnf;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_GET_CB_TYPES_REQ">   */
typedef CiEmptyPrim CiSsPrimGetCbTypesReq;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_GET_CB_TYPES_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimGetCbTypesCnf_struct {
	CiSsResultCode Result;                                                                          /**< Result code. \sa CiSsResultCode */
	UINT8 NumCbTypes;                                                                               /**< Number of supported CB Types [range: 0.. CI_NUM_CB_LOCK_CODES] */
	CiSsCbServiceCode CbTypes[ CISS_NUM_CB_LOCK_CODES ];                                            /**< List of supported CB Types. \sa CiSsCbServiceCode */
} CiSsPrimGetCbTypesCnf;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_GET_BASIC_SVC_CLASSES_REQ">   */
typedef CiEmptyPrim CiSsPrimGetBasicSvcClassesReq;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_GET_BASIC_SVC_CLASSES_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimGetBasicSvcClassesCnf_struct {
	CiSsResultCode Result;                                                          /**< Result code. \sa CiSsResultCode */
	UINT8 NumClasses;                                                               /**< Number of supported Classes [range: 0..CICF_NUM_SERVICES] */
	CiSsBasicService Classes[ CISS_NUM_SERVICES ];                                  /**< List of supported Classes. \sa CiSsBasicService */
} CiSsPrimGetBasicSvcClassesCnf;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_GET_ACTIVE_CW_CLASSES_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimGetActiveCwClassesReq_struct {
	CiSsBasicServiceMap Classes;    /**< Bitmap of Basic Services to be interrogated. \sa CiSsBasicServiceMap */
} CiSsPrimGetActiveCwClassesReq;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_GET_ACTIVE_CW_CLASSES_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimGetActiveCwClassesCnf_struct {
	CiSsResultCode Result;                                  /**< Result code. \sa CiSsResultCode */
	CiSsBasicServiceMap ReqClasses;                         /**< Requested bitmap of Basic Services (copied directly from the request). \sa CiSsBasicServiceMap */
	CiSsBasicServiceMap ActClasses;                         /**< Bitmap of Call Waiting status for each of the Basic Services that were interrogated. \sa CiSsBasicServiceMap */
} CiSsPrimGetActiveCwClassesCnf;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_GET_CB_MAP_STATUS_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimGetCbMapStatusReq_struct {
	CiSsCbServiceCode Service;              /**< Call Barring Service Code. \sa CiSsCbServiceCode */
	CiSsBasicServiceMap Classes;            /**< Bitmap of Basic Services to be interrogated. \sa CiSsBasicServiceMap */
} CiSsPrimGetCbMapStatusReq;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_GET_CB_MAP_STATUS_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimGetCbMapStatusCnf_struct {
	CiSsResultCode Result;                                  /**< Result code. \sa CiSsResultCode */
	CiSsCbServiceCode Service;                              /**< Call Barring Service Code. \sa CiSsCbServiceCode */
	CiSsBasicServiceMap ReqClasses;                         /**< Requested bitmap of Basic Services (copied directly from the request). \sa CiSsBasicServiceMap */
	CiSsBasicServiceMap ActClasses;                         /**< Bitmap of Call Waiting status for each of the Basic Services that were interrogated. \sa CiSsBasicServiceMap */
} CiSsPrimGetCbMapStatusCnf;

#if defined(CCI_POSITION_LOCATION)

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_PRIVACY_CTRL_REG_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimPrivacyCtrlRegReq_struct
{
	CiBoolean SupportPrivacy;       /**< TRUE enables support for privacy control services; FALSE disables the support. \sa CCI API Ref Manual */
} CiSsPrimPrivacyCtrlRegReq;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_PRIVACY_CTRL_REG_CNF">   */
/**  -
 */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimPrivacyCtrlRegCnf_struct
{
	CiSsResultCode result;          /**< Result code. \sa CiSsResultCode */
} CiSsPrimPrivacyCtrlRegCnf;

/* MOLR request definitions */

#define   CI_SS_MAX_NUM_PRIVATE_EXTENSIONS                                              10
#define   CI_SS_MAX_EXTERNAL_ADDRESS_SIZE                                               20
#define   CI_SS_MAX_MLC_NUMBER_SIZE                                                             9
#define   CI_SS_MAX_ASSISTANCE_DATA_SIZE                                                38
#define   CI_SS_MAX_EXTENSION_TYPE_SIZE                                                 16              /* Look at IOC for Private MAP extensions */
#define   CI_SS_MAX_EXTENSION_ID_SIZE                                                           16      /* Section 17.7.11  29.002 */

#define     CI_SS_PRIM_L2_MESSAGE_SIZE                                                          251
#define     CI_SS_PRIM_MAX_FACILITY_DATA_LENGTH                                         (CI_SS_PRIM_L2_MESSAGE_SIZE - 2)
#define     CI_SS_PRIM_MAX_LOCATION_ESTIMATE_STRING_SIZE                20
#define     CI_SS_PRIM_MAX_DECIPHERING_KEYS_STRING_SIZE                 15
#define     CI_SS_PRIM_MAX_ADD_LOCATION_ESTIMATE_STRING_SIZE    91


/* MOLR request typedefs */
/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief This is a 'live' invoke handler
 *  \sa CISS_CB_SERVICE_CODE
 * \remarks Common Data Section */
typedef INT16 CiSsPrimInvokeHandle;
/**@}*/


//ICAT EXPORTED ENUM
/** \brief tasks sent to identified by their task ids  . */
/** \remarks Common Data Section */
typedef enum CiSsPrimTaskIdTag
{
	CiSS_PRIM_SAC_TASK_ID,
	CiSS_PRIM_UNKNOWN_TASK                = 0xFF

						/* Any new TaskId which is to be recognised by GENIE should end in the
						** string "_ID"; conversely, any value which is not to be displayed by
						** GENIE should NOT end in this string. */

} _CiSsPrimTaskId;

typedef UINT8 CiSsPrimTaskId;

//ICAT EXPORTED ENUM
typedef enum CiSsPrimSsCodeTag
{
	/* all LCS privacy exception classes */
	CI_SS_PRIM_SS_ALL_LCS_PRIVACY_EXCEPTION = 0xB0,
	/* Allow location by any LCS client */
	CI_SS_PRIM_SS_LCS_UNIVERSAL             = 0xB1,
	/* allow location by any value added LCS client to which
	 * a call is established from the target MS */
	CI_SS_PRIM_SS_LCS_CALL_RELATED          = 0xB2,
	/* allow location by designated external value added LCS clients */
	CI_SS_PRIM_SS_LCS_CALL_UNRELATED        = 0xB3,
	/* allow location by designated PLMN operator LCS clients */
	CI_SS_PRIM_SS_LCS_PLMN_OPERATOR         = 0xB4,
	/* all Mobile Originating location request classes */
	CI_SS_PRIM_SS_LCS_ALL_MOLR_SS           = 0xC0,
	/* allow an MS to request its own location */
	CI_SS_PRIM_SS_LCS_BASIC_SELF_LOCATION   = 0xC1,
	/* allow an MS to perform self location without interaction with
	 * the PLMN for a pre-determined period of time */
	CI_SS_PRIM_SS_LCS_AUTONOMOUS_SELF_LOCATION  = 0xC2,
	/* allow an MS to request transfer of its location to another LCS client */
	CI_SS_PRIM_SS_LCS_TRANSFER_TO_THIRD_PARTY   = 0xC3,
	/* UPGRADE_AGPS */
} _CiSsPrimSsCode;
typedef UINT8 CiSsPrimSsCode;

//ICAT EXPORTED ENUM
typedef enum CiSsPrimSsLcsMolrTypeTag
{
	CI_SS_PRIM_LOCATION_ESTIMATE   = 0,
	CI_SS_PRIM_ASSISTANCE_DATA     = 1,
	CI_SS_PRIM_DECIPHERING_KEYS    = 2
}_CiSsPrimSsLcsMolrType;
typedef UINT8 CiSsPrimSsLcsMolrType;

//ICAT EXPORTED ENUM
typedef enum CiSsPrimSsLcsLocationMethodTag
{
	CI_SS_PRIM_MS_BASED_EOTD       = 0,
	CI_SS_PRIM_MS_ASSISTED_EOTD    = 1,
	CI_SS_PRIM_ASSISTED_GPS        = 2
}_CiSsPrimSsLcsLocationMethod;
typedef UINT8 CiSsPrimSsLcsLocationMethod;


//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSsLcsOptLocationMethodTag
{
	CiBoolean present;
	CiSsPrimSsLcsLocationMethod method;
}CiSsPrimSsLcsOptLocationMethod;

//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSsLcsOptAccuracyTag
{
	CiBoolean present;
	INT8 accuracy;           /* IMPLICIT OCTECT STRING - SIZE 1 */
}CiSsPrimSsLcsOptAccuracy;

//ICAT EXPORTED ENUM
typedef enum CiSsPrimSsLcsResponseTimeCatagoryTag
{
	CI_SS_PRIM_LOW_DELAY           = 0,
	CI_SS_PRIM_DELAY_TOLERANT   = 1
}_CiSsPrimSsLcsResponseTimeCatagory;
typedef UINT8 CiSsPrimSsLcsResponseTimeCatagory;

//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSsLcsOptResponseTimeTag
{
	CiBoolean present;
	CiSsPrimSsLcsResponseTimeCatagory responseTimeCatagory; /* ENUMERATED */
}CiSsPrimSsLcsOptResponseTime;

//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSsLcsExtensionIdTag
{
	INT8 n;      /* OCTET STRING  - should not exceed 16 octets */
	INT8 data[CI_SS_MAX_EXTENSION_ID_SIZE];
}CiSsPrimSsLcsExtensionId;

//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSsLcsExtensionTypeTag
{
	INT8 n;     /* OCTET STRING  - should not exceed 16 octets */
	INT8 data[CI_SS_MAX_EXTENSION_TYPE_SIZE];
}CiSsPrimSsLcsExtensionType;


//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSsLcsPrivateExtListDataTag
{
	CiBoolean extTypePresent;
	CiSsPrimSsLcsExtensionType extType;
	CiSsPrimSsLcsExtensionId extId;
}CiSsPrimSsLcsPrivateExtListData;


//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSsLcsPrivateExtensionListTag
{
	INT8 n;                                            /* 1 to maxNumOfPrivateExtensions */
	CiSsPrimSsLcsPrivateExtListData data[CI_SS_MAX_NUM_PRIVATE_EXTENSIONS];
}CiSsPrimSsLcsPrivateExtensionList;


//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSsLcsPcsExtensionsTag
{
	INT8 _dummy_;                 /* Not used */
}CiSsPrimSsLcsPcsExtensions;

//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSsLcsExtensionContainerTag
{
	CiBoolean privateExtensionListPresent;
	CiSsPrimSsLcsPrivateExtensionList privateExtensionList;
	CiBoolean pcs_ExtensionsPresent;
	CiSsPrimSsLcsPcsExtensions pcs_Extensions;
}CiSsPrimSsLcsExtensionContainer;

//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSsLcsOptExtensionContainerTag
{
	CiBoolean present;
	CiSsPrimSsLcsExtensionContainer data;
}CiSsPrimSsLcsOptExtensionContainer;

//ICAT EXPORTED STRUCT
typedef struct  CiSsPrimSsLcsQosTag
{
	CiSsPrimSsLcsOptAccuracy horizontalAccuracy;                                    /* optional octet string */
	CiBoolean verticalCoordReq;                                                     /* IMPLICIT NULL OPTIONAL */
	CiSsPrimSsLcsOptAccuracy verticalAccuracy;                                      /* optional octet string */
	CiSsPrimSsLcsOptResponseTime responseTime;                                      /* IMPLICIT SEQUENCE */
	CiSsPrimSsLcsOptExtensionContainer optExtensionContainer;                       /* IMPLICIT SEQUENCE */
}CiSsPrimSsLcsQos;

//ICAT EXPORTED STRUCT
typedef struct  CiSsPrimSsLcsOptQosTag
{
	CiBoolean present;
	CiSsPrimSsLcsQos lcsQos;
}CiSsPrimSsLcsOptQos;

//ICAT EXPORTED STRUCT
/** \brief External Address.   */
/** \remarks Common Data Section */
typedef struct CiSsPrimSsLcsOptExternalAddressTag
{
	CiBoolean present;                                              /**< Indicates if address is present. \sa CCI API Ref Manual. */
	INT8 addressLength;                                             /**< Address length - Max is 20 */
	INT8 externalAddress[CI_SS_MAX_EXTERNAL_ADDRESS_SIZE];          /**< External Address */
}CiSsPrimSsLcsOptExternalAddress;


//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSsLcsOptClientExternalIdTag
{
	CiBoolean present;
	CiSsPrimSsLcsOptExternalAddress optExternalAddress;
	CiSsPrimSsLcsOptExtensionContainer optExtensionContainer; /* IMPLICIT SEQUENCE */
}CiSsPrimSsLcsOptClientExternalId;

//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSsLcsOptMlcNumberTag
{
	CiBoolean present;
	INT8 mlcNumberLength;
	INT8 mlcNumber[CI_SS_MAX_MLC_NUMBER_SIZE];
}CiSsPrimSsLcsOptMlcNumber;


//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSsLcsOptGpsAssistanceDataTag
{
	CiBoolean present;
	INT8 assistanceDataLength;
	INT8 assistanceData[CI_SS_MAX_ASSISTANCE_DATA_SIZE];
}CiSsPrimSsLcsOptGpsAssistanceData;

//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSsLcsOptSupportedGADShapesTag
{
	CiBoolean present;
	INT16 supportedGADShapesBitMask;            /*IMPLICIT BIT STRING */
}CiSsPrimSsLcsOptSupportedGADShapes;


//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSsLcsOptLcsServiceTypeIdTag
{
	CiBoolean present;
	INT8 lcsServiceTypeId;
}CiSsPrimSsLcsOptLcsServiceTypeId;

//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSsLcsMOLRDataTag
{
	CiSsPrimSsLcsMolrType lcsMolrType;
	CiSsPrimSsLcsOptLocationMethod lcsOptLocationMethod;
	CiSsPrimSsLcsOptQos lcsOptQos;
	CiSsPrimSsLcsOptClientExternalId lcsOptClientExternalId;
	CiSsPrimSsLcsOptMlcNumber lcsOptMlcNumber;
	CiSsPrimSsLcsOptGpsAssistanceData lcsOptGpsAssistanceData;
	CiSsPrimSsLcsOptSupportedGADShapes lcsOptSupportedGADShapes;
	CiSsPrimSsLcsOptLcsServiceTypeId lcsOptLcsServiceTypeId;
}CiSsPrimSsLcsMOLRData;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_GET_LOCATION_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimGetLocationReq_struct
{
	CiSsPrimTaskId taskId;                                                          /**< Application background task ID (It is copied from APEX interface). \sa CiSsPrimTaskIdTag */
	CiSsPrimInvokeHandle moInvokeHandle;                                            /**< This must correspond to a 'live' invokeHandle associated with an outgoing invoke.  If a matching invoke handle is not found then this signal is not generated (It is copied from APEX interface). \sa CiSsPrimInvokeHandle */
	CiSsPrimSsCode ssCode;                                                          /**< operationCode will be the same as in the originating invoke message. \sa CiSsPrimSsCodeTag */
	CiSsPrimSsLcsMOLRData lcsMOLRData;                                              /**< The LCS mobile originating location request data. \sa CiSsPrimSsLcsMOLRDataTag */
} CiSsPrimGetLocationReq;

//ICAT EXPORTED ENUM
/** \brief SS result paramter values   */
/** \remarks Common Data Section */
typedef enum CiSsPrimSsResultParameterTypeTag
{
	CI_SS_PRIM_SS_NO_DATA                    = 0,
	CI_SS_PRIM_SS_FORWARD_INFO,
	CI_SS_PRIM_SS_CALL_BARRING_INFO,
	CI_SS_PRIM_SS_CUG_INFO,
	CI_SS_PRIM_SS_INFO_DATA,
	CI_SS_PRIM_SS_STATUS,
	CI_SS_PRIM_SS_FWD_TO_NUMBER,
	CI_SS_PRIM_SS_BS_GROUP_LIST,
	CI_SS_PRIM_SS_FWD_FEATURE_LIST,
	CI_SS_PRIM_SS_USS_USER_DATA,
	CI_SS_PRIM_SS_USS_USER_ARG,
	CI_SS_PRIM_SS_RAW_DATA,
	CI_SS_PRIM_SS_PASSWORD,
	CI_SS_PRIM_SS_CLIR_INFO,
	CI_SS_PRIM_SS_LCS_MOLR
}_CiSsPrimSsResultParameterType;
typedef UINT8 CiSsPrimSsResultParameterType;


//ICAT EXPORTED ENUM
/** \brief Operation code   */
/** \remarks Common Data Section */
typedef enum CiSsPrimSsOperationCodeTag
{
	CI_SS_PRIM_MO_LCS_MOLR               = 115,
	CI_SS_PRIM_MT_LCS_LOCATION_NOTIFY    = 116,
}_CiSsPrimSsOperationCode;

typedef UINT8 CiSsPrimSsOperationCode;

//ICAT EXPORTED STRUCT
/**  \brief  Location estimate result */
/** \remarks Common Data Section */
typedef struct CiSsPrimSsLcsOptLocationEstimateResultTag
{
	CiBoolean present;                                              /**< Indicates if result is present. \sa CCI API Ref Manual. */
	INT8 length;
	INT8 data[CI_SS_PRIM_MAX_LOCATION_ESTIMATE_STRING_SIZE];        /**< IMPLICIT OCTET STRING */
}CiSsPrimSsLcsOptLocationEstimateResult;

//ICAT EXPORTED STRUCT
/**  \brief  Deciphering Keys Result  */
/** \remarks Common Data Section */
typedef struct CiSsPrimSsLcsOptDecipheringKeysResultTag
{
	CiBoolean present;                                                      /**< Indicates if result is present. \sa CCI API Ref Manual. */
	INT8 length;
	INT8 data[CI_SS_PRIM_MAX_DECIPHERING_KEYS_STRING_SIZE];                 /**< Fixed size of 15 bytes  IMPLICIT OCTET STRING */
}CiSsPrimSsLcsOptDecipheringKeysResult;

//ICAT EXPORTED STRUCT
/**  \brief  Add Location estimate result */
/** \remarks Common Data Section */
typedef struct CiSsPrimSsLcsOptAddLocationEstimateResultTag
{
	CiBoolean present;                                                      /**< Indicates if result is present. \sa CCI API Ref Manual. */
	INT8 length;
	INT8 data[CI_SS_PRIM_MAX_ADD_LOCATION_ESTIMATE_STRING_SIZE];            /**< IMPLICIT OCTET STRING */
}CiSsPrimSsLcsOptAddLocationEstimateResult;

//ICAT EXPORTED STRUCT
/**  \brief  MOLR Result Info */
/** \remarks Common Data Section */
typedef struct CiSsPrimSsLcsMOLRResultInfoTag
{
	CiSsPrimSsLcsOptLocationEstimateResult optLocEstimateResult;            /**<  Location estimate result - optional. \sa CiSsPrimSsLcsOptLocationEstimateResultTag.*/
	CiSsPrimSsLcsOptDecipheringKeysResult optDecipherKeysResult;            /**<   Deciphering Keys Result - optional. \sa CiSsPrimSsLcsOptDecipheringKeysResultTag.*/
	CiSsPrimSsLcsOptAddLocationEstimateResult optAddLocEstimateResult;      /**<   Add Location estimate result - optional. \sa CiSsPrimSsLcsOptAddLocationEstimateResultTag. */
}CiSsPrimSsLcsMOLRResultInfo;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief   MOLR Result Info. */
/** \remarks Common Data Section */
typedef union CiSsPrimSsResultDataTag
{
	CiSsPrimSsLcsMOLRResultInfo molrResultInfo;    /**< \sa MOLR Result Info CiSsPrimSsLcsMOLRResultInfoTag */
}CiSsPrimSsResultData;

/**@}*/

//ICAT EXPORTED STRUCT
/** <paramref name="CI_SS_PRIM_GET_LOCATION_CNF">   */
typedef struct CiSsPrimGetLocationCnf_struct
{
	CiSsPrimTaskId taskId;                                  /**< Application background task ID (It is copied from APEX interface). \sa CiSsPrimTaskIdTag */
	CiSsPrimInvokeHandle moInvokeHandle;                    /**< This must correspond to a 'live' invokeHandle associated with an outgoing invoke.  If a matching invoke handle is not found then this signal is not generated (It is copied from APEX interface). \sa CiSsPrimInvokeHandle */
	CiSsPrimSsOperationCode ssOperationCode;                /**< "MOLR type" or "Location notify" type. \sa  CiSsPrimSsOperationCodeTag. */
	CiSsPrimSsResultParameterType ssResultParameterType;    /**< Type of SS result parameter (should be SS_LCS_MOLR). \sa CiSsPrimSsResultParameterTypeTag */
	CiSsPrimSsResultData ssResultData;                      /**< MO-LR result info (response to MO-LR request info). \sa CiSsPrimSsResultDataTag */
}CiSsPrimGetLocationCnf;

/* MTLR request definitions */
#define   CI_SS_MAX_LCS_NAME_STRING_LENGTH             63
#define   CI_SS_MAX_LCS_CODEWORD_SIZE                  20


/* MTLR request typedefs */
/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief */
/** \remarks Common Data Section */
typedef signed char CiSsPrimSignedInt8;
/**@}*/


//ICAT EXPORTED ENUM
/**  \brief LCS notification type values  */
/** \remarks Common Data Section */
typedef enum CiSsPrimSsLcsNotificationTypeTag
{
	CI_SS_NOTIFY_LOCATION_ALLOWED                               = 0,
	CI_SS_NOTIFY_AND_VERIFY_LOCATION_ALLOWED_IF_NO_RESPONSE     = 1,
	CI_SS_NOTIFY_AND_VERIFY_LOCATION_NOT_ALLOWED_IF_NO_RESPONSE = 2,
	CI_SS_LOCATION_NOT_ALLOWED                                  = 3
}_CiSsPrimSsLcsNotificationType;

typedef UINT8 CiSsPrimSsLcsNotificationType;

//ICAT EXPORTED ENUM
/**  \brief LCS location information */
/** \remarks Common Data Section */
typedef enum CiSsPrimSsLcsLocationEstimateTypeTag
{
	CI_SS_CURRENT_LOCATION               = 0,
	CI_SS_CURRENT_OR_LAST_KNOWN_LOCATION = 1,
	CI_SS_INITIAL_LOCATION               = 2,
	CI_SS_ACTIVATE_DEFERRED_LOCATION     = 3,
	CI_SS_CANCEL_DEFERRED_LOCATION       = 4
}_CiSsPrimSsLcsLocationEstimateType;

typedef UINT8 CiSsPrimSsLcsLocationEstimateType;

//ICAT EXPORTED STRUCT
/**  \brief LCS location information */
/** \remarks Common Data Section */
typedef struct CiSsPrimSsLcsLocationTypeTag
{
	CiSsPrimSsLcsLocationEstimateType locEstimateType;                                      /**< Lcs Location estimate type. \sa  CiSsPrimSsLcsLocationEstimateTypeTag. */
	CiBoolean deferredLocationTypePresent;                                                  /**< Indicated if deffered location type is presetn. \sa  CCI API Ref Manual. */
	INT16 deferredLocationType;                                                             /**< Deferred location type. */
}CiSsPrimSsLcsLocationType;

//ICAT EXPORTED STRUCT
/** \brief  LCS data coding scheme */
/** \remarks Common Data Section */
typedef struct CiSsPrimSsLcsDataCodingSchemeTag
{
	INT8 scheme;
}CiSsPrimSsLcsDataCodingScheme;

//ICAT EXPORTED STRUCT
/** \brief  LCS name string */
/** \remarks Common Data Section */
typedef struct CiSsPrimSsLcsNameStringTag
{
	INT8 length;
	INT8 name[CI_SS_MAX_LCS_NAME_STRING_LENGTH];
}CiSsPrimSsLcsNameString;

//ICAT EXPORTED ENUM
/** \brief  LCS format indicator values */
/** \remarks Common Data Section */
typedef enum CiSsPrimSsLcsFormatIndicatorTag
{
	CI_SS_FORMAT_INDICATOR_LOGICAL_NAME       =  0,
	CI_SS_FORMAT_INDICATOR_EMAIL_ADDRESS      =  1,
	CI_SS_FORMAT_INDICATOR_MSISDN             =  2,
	CI_SS_FORMAT_INDICATOR_URL                =  3,
	CI_SS_FORMAT_INDICATOR_SIP_URL            =  4
}_CiSsPrimSsLcsFormatIndicator;
typedef UINT8 CiSsPrimSsLcsFormatIndicator;

//ICAT EXPORTED STRUCT
/** \brief  LCS format indicator */
/** \remarks Common Data Section */
typedef struct CiSsPrimSsLcsOptFormatIndicatorTag
{
	CiBoolean present;                              /**< Indicates if format indicator is present. \sa CCI API Ref Manual. */
	CiSsPrimSsLcsFormatIndicator formatIndicator;   /**< format indicator. \sa CiSsPrimSsLcsFormatIndicatorTag. */
}CiSsPrimSsLcsOptFormatIndicator;


//ICAT EXPORTED STRUCT
/** \brief  Client Name */
/** \remarks Common Data Section */
typedef struct CiSsPrimSsLcsOptClientNameTag
{
	CiBoolean present;                                                      /**< Indicates if name is present. \sa CCI API Ref Manual. */
	CiSsPrimSsLcsDataCodingScheme dataCodingScheme;                         /**< Data coding scheme. \sa  CiSsPrimSsLcsDataCodingSchemeTag*/
	CiSsPrimSsLcsNameString nameString;                                     /**< Name string. \sa  CiSsPrimSsLcsNameStringTag*/
	CiSsPrimSsLcsOptFormatIndicator optFormatIndicator;                     /**< Format indicator. \sa  CiSsPrimSsLcsOptFormatIndicatorTag */
}CiSsPrimSsLcsOptClientName;

//ICAT EXPORTED STRUCT
/** \brief LCS requestor ID */
/** \remarks Common Data Section */
typedef struct CiSsPrimSsLcsOptRequestorIDTag
{
	CiBoolean present;                                      /**< Indicates if ID is present. \sa CCI API Ref Manual. */
	CiSsPrimSsLcsDataCodingScheme dataCodingScheme;         /**< Data coding scheme. \sa  CiSsPrimSsLcsDataCodingSchemeTag*/
	CiSsPrimSsLcsNameString requestorIdString;              /**< Name string. \sa  CiSsPrimSsLcsNameStringTag*/
	CiSsPrimSsLcsOptFormatIndicator optFormatIndicator;     /**< Format indicator. \sa  CiSsPrimSsLcsOptFormatIndicatorTag */
}CiSsPrimSsLcsOptRequestorID;

//ICAT EXPORTED STRUCT
/** \brief LCS Codeword */
/** \remarks Common Data Section */
typedef struct CiSsPrimSsLcsOptCodewordTag
{
	CiBoolean present;                                                                                              /**< Indicates if Codeword is present. \sa CCI API Ref Manual. */
	CiSsPrimSsLcsDataCodingScheme dataCodingScheme;                                                                 /**< Data coding scheme. \sa  CiSsPrimSsLcsDataCodingSchemeTag*/
	INT8 length;                                                                                                    /**< codeword length */
	INT8 codewordString[CI_SS_MAX_LCS_CODEWORD_SIZE];                                                               /**< codeword  */
}CiSsPrimSsLcsOptCodeword;

//ICAT EXPORTED STRUCT
/** \brief LCS Service type ID */
/** \remarks Common Data Section */
typedef struct CiSsPrimSsLcsOptServiceTypeIdTag
{
	CiBoolean present;                      /**< Indicates if Service type ID is present. \sa CCI API Ref Manual. */
	CiSsPrimSignedInt8 serviceTypeId;       /**< Service Type ID. \sa CiSsPrimSignedInt8 */
}CiSsPrimSsLcsOptServiceTypeId;

//ICAT EXPORTED STRUCT
/** \brief LCS client location-notification data */
/** \remarks Common Data Section */
typedef struct CiSsPrimSsLcsLocNotifyInfoTag
{
	CiSsPrimSsLcsNotificationType notifyType;                       /**< LCS notification type. \sa CiSsPrimSsLcsNotificationTypeTag  */
	CiSsPrimSsLcsLocationType locationType;                         /**< LCS location information. \sa CiSsPrimSsLcsLocationTypeTag  */
	CiSsPrimSsLcsOptClientExternalId lcsOptClientExternalId;        /**< LCS client external ID - Optional value. \sa CiSsPrimSsLcsOptClientExternalIdTag  */
	CiSsPrimSsLcsOptClientName lcsOptClientName;                    /**< LCS client name - Optional value. \sa CiSsPrimSsLcsOptClientNameTag  */
	CiSsPrimSsLcsOptRequestorID lcsOptRequestorId;                  /**< LCS requestor ID - Optional value. \sa CiSsPrimSsLcsOptRequestorIDTag  */
	CiSsPrimSsLcsOptCodeword lcsOptCodeword;                        /**< LCS Codeword - Optional value. \sa CiSsPrimSsLcsOptCodewordTag */
	CiSsPrimSsLcsOptServiceTypeId lcsOptServiceTypeId;              /**< LCS Service type ID - Optional value. \sa CiSsPrimSsLcsOptServiceTypeIdTag */
}CiSsPrimSsLcsLocNotifyInfo;

/* <INUSE> */
/** <paramref name="CI_SS_PRIM_LOCATION_IND">   */
//ICAT EXPORTED STRUCT
typedef struct CiSsPrimLocationInd_struct
{
	CiSsPrimTaskId taskId;                                          /**< Application background task ID (It is copied from APEX interface).\sa CiSsPrimTaskIdTag */
	CiSsPrimInvokeHandle mtInvokeHandle;                    /**< This must correspond to a 'live' invokeHandle associated with an
								 *     outgoing invoke.  If a matching invoke handle is not found then this
								 *     signal is not generated (It is copied from APEX interface). \sa CiSsPrimInvokeHandle. */
	CiSsPrimSsLcsLocNotifyInfo ssLcsLocNotifyInfo;          /**<  LCS client location-notification data (could be used to identify a
								 *     location service provider for example). \sa CiSsPrimSsLcsLocNotifyInfoTag */
} CiSsPrimLocationInd;

//ICAT EXPORTED ENUM
/** \brief Location notification response values */
/** \remarks Common Data Section */
typedef enum CiSsPrimSsLcsVerificationResponseTag
{
	CI_SS_PERMISSION_DENIED   =  0,
	CI_SS_PERMISSION_GRANTED  =  1
}_CiSsPrimSsLcsVerificationResponse;
typedef UINT8 CiSsPrimSsLcsVerificationResponse;

//ICAT EXPORTED STRUCT
typedef struct CiSsPrimSsLcsLocationNotificationResTag
{
	CiBoolean present;                                      /**< Indicates if Location notification response is present. \sa CCI API Ref Manual. */
	CiSsPrimSsLcsVerificationResponse verificationResponse; /**< Location notification response. \sa CiSsPrimSsLcsVerificationResponseTag. */

}CiSsPrimSsLcsLocationNotificationRes;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_SS_PRIM_LOCATION_VERIFY_RSP">   */
typedef struct CiSsPrimLocationVerifyRsp_struct
{
	CiSsPrimTaskId taskId;                          /**< Application background task ID (It is copied from APEX interface).\sa CiSsPrimTaskIdTag */
	CiSsPrimInvokeHandle mtInvokeHandle;            /**< This must correspond to a 'live' invokeHandle associated with an outgoing invoke.  If a matching invoke handle is not found then this signal is not generated (It is copied from APEX interface). \sa \sa CiSsPrimInvokeHandle.\sa CiSsPrimInvokeHandle. */
	CiSsPrimSsLcsLocationNotificationRes response;  /**< Data indicating to the network entity whether a positioning session can take place. \sa CiSsPrimSsLcsLocationNotificationResTag */
}CiSsPrimLocationVerifyRsp;


typedef CiEmptyPrim CiSsPrimGetLcsNwstateReq;

//ICAT EXPORTED STRUCT

typedef struct CiSsPrimGetLcsNwstateCnf_struct
{
	CiSsResultCode result;
	CiSsLcsNwState nwState;
} CiSsPrimGetLcsNwstateCnf;

//ICAT EXPORTED STRUCT

typedef struct CiSsPrimLcsNwstateCfgIndReq_struct
{
	CiBoolean reportChanges;

} CiSsPrimLcsNwstateCfgIndReq;

//ICAT EXPORTED STRUCT

typedef struct CiSsPrimLcsNwstateCfgIndCnf_struct
{
	CiSsResultCode result;
} CiSsPrimLcsNwstateCfgIndCnf;

//ICAT EXPORTED STRUCT
typedef struct CiSsPrimLcsNwstateInd_struct
{
	CiSsLcsNwState nwState;
} CiSsPrimLcsNwstateInd;

#endif /* CCI_POSITION_LOCATION */

/* ********************************************************************************************* *
 * enum list for the completion status of service request
 */
//ICAT EXPORTED ENUM
/**  \brief SS request completion status values */
/** \remarks Common Data Section */
typedef enum CI_SS_REQ_STATUS {

	CI_SS_ACTIVE,
	CI_SS_REGISTERED,
	CI_SS_PROVISIONED,
	CI_SS_QUIESCENT,

	CI_SS_NUM_REQ_STATUS

} _CiSsReqStatus;
/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief SS request status.
 * \sa CI_SS_REQ_STATUS */
/** \remarks Common Data Section */
typedef UINT8 CiSsReqStatus;
/**@}*/



/* ********************************************************************************************* *
 * CiSsPrimServiceRequestCompleteInd
 *
 * Supplementary Service Request Complete notification, provides
 * the state of an SS
 *
 * This notification is sent in reply to a Supplementary Service Request
 * and it's based on the status information returned by the protocol stack.
 * The returned status is decoded from the Facility Information Element, when
 * present, by the protocol stack as follows: ACTIVE, REGISTERED, PROVISIONED,
 * QUIESCENT.
 *
 * This indication may come right after a CI_SS_xx_CNF reply primitive.
 *
 */
//ICAT EXPORTED STRUCT
/** <paramref name="CI_SS_PRIM_SERVICE_REQUEST_COMPLETE_IND"> */
typedef struct CiSsPrimServiceRequestCompleteInd_struct {

	CiSsReqStatus ReqStatus; /**< Completion status. \sa CiSsReqStatus */

} CiSsPrimServiceRequestCompleteInd;


/* ********************************************************************************************* *
 * CiSsPrimGetColrStatusReq
 *
 * Interrogation of the CoLR Service support
 *
 */
/** <paramref name="CI_SS_PRIM_GET_COLR_STATUS_REQ"> */
typedef CiEmptyPrim CiSsPrimGetColrStatusReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_SS_PRIM_GET_COLR_STATUS_CNF"> */
typedef struct CiSsPrimGetColrStatusCnf_struct {
	CiSsResultCode Result;          /**< Result code. \sa CiSsResultCode*/
	UINT8 Local;                    /**< Local CoLR Presentation Status -- not available */
	UINT8 Provision;                /**< CoLR Provision Status */
} CiSsPrimGetColrStatusCnf;


/* ADD NEW COMMON PRIMITIVES DEFINITIONS HERE */

#ifdef CI_CUSTOM_EXTENSION
/* it is assumed that only one customized set of extension primitives is
 * to be considered at one time. The selection of the particular customized
 * set is done in the 'ci_ss_cust.h' based on compile flags.
 *
 * Note: if no customer extension primitives are defined for this service group
 * the CI_SS_NUM_CUST_PRIM will be set to 0 in the "ci_ss_cust.h" file.
 */
#include "ci_ss_cust.h"

#define CI_SS_NUM_PRIM CI_SS_NUM_COMMON_PRIM + CI_SS_NUM_CUST_PRIM

#else

/* if no customer extension is supported, only the default common set is considered */
#define CI_SS_NUM_PRIM CI_SS_NUM_COMMON_PRIM

#endif /* CI_CUSTOM_EXTENSION */

#ifdef __cplusplus
}
#endif  //__cplusplus

#endif  /* _CI_SS_H_ */

/*                      end of ci_ss.h
   --------------------------------------------------------------------------- */









































































































































































































































































