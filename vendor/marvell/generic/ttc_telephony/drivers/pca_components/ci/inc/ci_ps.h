/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/* ===========================================================================
   File        : ci_ps.h
   Description : Data types file for the PS Service Group
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

#if !defined(_CI_PS_H_)
#define _CI_PS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ci_cfg.h"
#include "ci_api_types.h"

/** \addtogroup  SpecificSGRelated
 * @{ */

#define CI_PS_VER_MAJOR 3
#define CI_PS_VER_MINOR 0

/* ----------------------------------------------------------------------------- */

/* CI_PS Primitive ID definitions */
//ICAT EXPORTED ENUM
/** Summary of primitives */
typedef enum CI_PS_PRIM
{
	CI_PS_PRIM_SET_ATTACH_STATE_REQ = 1,                                            /**< \brief Attaches ME to, or detach ME from, the packet domain service. \details */
	CI_PS_PRIM_SET_ATTACH_STATE_CNF,                                                /**< \brief Reports result to PS attach state request. \details */
	CI_PS_PRIM_GET_ATTACH_STATE_REQ,                                                /**< \brief Queries current packet domain service state. \details */
	CI_PS_PRIM_GET_ATTACH_STATE_CNF,                                                /**< \brief Reports current packet domain service state. \details */
	CI_PS_PRIM_DEFINE_PDP_CTX_REQ,                                                  /**< \brief Defines PDP context for a specified cid. \details   */
	CI_PS_PRIM_DEFINE_PDP_CTX_CNF,                                                  /**< \brief Reports handling result for PDP context definition request.
											 * \details If PDP context address field is NULL, dynamic address will be requested.  */
	CI_PS_PRIM_DELETE_PDP_CTX_REQ,                                                  /**< \brief Deletes a PDP context. \details */
	CI_PS_PRIM_DELETE_PDP_CTX_CNF,                                                  /**< \brief Reports handling result for PDP context deletion request. \details */
	CI_PS_PRIM_GET_PDP_CTX_REQ,                                                     /**< \brief Gets a PDP context definition. \details */
	CI_PS_PRIM_GET_PDP_CTX_CNF,                                                     /**< \brief Reports a PDP context setting. \details */
	CI_PS_PRIM_GET_PDP_CTX_CAPS_REQ,                                                /**< \brief Requests the PDP context capabilities supported by the cellular subsystem. \details   */
	CI_PS_PRIM_GET_PDP_CTX_CAPS_CNF,                                                /**< \brief Reports PDP context capabilities supported by the cellular subsystem. \details   */
	CI_PS_PRIM_SET_PDP_CTX_ACT_STATE_REQ,                                           /**< \brief Activate (Deactivate) a(all) PDP context(s). \details */
	CI_PS_PRIM_SET_PDP_CTX_ACT_STATE_CNF,                                           /**< \brief Reports result to PS PDP context activation/deactivation request. \details */
	CI_PS_PRIM_GET_PDP_CTXS_ACT_STATE_REQ,                                          /**< \brief Gets all defined PDP contexts current activation state. \details   */
	CI_PS_PRIM_GET_PDP_CTXS_ACT_STATE_CNF,                                          /**< \brief Reports the current activation state of all defined PDP contexts. \details   */
	CI_PS_PRIM_ENTER_DATA_STATE_REQ,                                        /**< \brief Notifies the cellular subsystem that application subsystem is entering data state, i.e. it is going to send/receive packet data from this point on.
										 * \details This request will trigger PDP attach and/or PDP context activation if they haven't been done.
										 * The paramter optimizedData will define if oprmized ACI data plane will be used.
										 * This paramter must be set to TRUE in order to use the optmized DATA service group primitives. */
	CI_PS_PRIM_ENTER_DATA_STATE_CNF,                                        /**< \brief Confirms data state entry request.
										 * \details After this point the cellular subsystem can start using the DATA service group
										 *  primitives to send and receive data over the packet service domain. */
	CI_PS_PRIM_MT_PDP_CTX_ACT_MODIFY_IND,                                   /**< \brief Indicates about a mobile terminated PDP context. \details   */
	CI_PS_PRIM_MT_PDP_CTX_ACT_MODIFY_RSP,                                   /**< \brief Responds to mobile terminated PDP context indication. \details   */
	CI_PS_PRIM_MT_PDP_CTX_ACTED_IND,                                        /**< \brief Indicates the mobile terminated PDP context is activated after manual or auto answer.
										 * \details The cellular subsystem will assign the cid (context ID) for the MT PDP context.  */
	CI_PS_PRIM_SET_GSMGPRS_CLASS_REQ,                                       /**< \brief Sets mobile class for GSM/GPRS. \details Only applied to GSM/GPRS network  */
	CI_PS_PRIM_SET_GSMGPRS_CLASS_CNF,                                       /**< \brief Confirms mobile class setting for GSM/GPRS. \details Only applied to GSM/GPRS network  */
	CI_PS_PRIM_GET_GSMGPRS_CLASS_REQ,                                       /**< \brief Requests the current setting of GSM/GPRS mobile class. \details Only applied to GSM/GPRS network  */
	CI_PS_PRIM_GET_GSMGPRS_CLASS_CNF,                                       /**< \brief Reports the current GSM/GPRS mobile class. \details Only applied to GSM/GPRS network  */
	CI_PS_PRIM_GET_GSMGPRS_CLASSES_REQ,                                     /**< \brief Requests the supported GSM/GPRS mobile classes. \details   */
	CI_PS_PRIM_GET_GSMGPRS_CLASSES_CNF,                                     /**< \brief Reports the supported GSM/GPRS mobile classes. \details  Only applied to GSM/GPRS network */
	CI_PS_PRIM_ENABLE_NW_REG_IND_REQ,                                       /**< \brief Enables/disables GPRS network registration status reports. \details   */
	CI_PS_PRIM_ENABLE_NW_REG_IND_CNF,                                       /**< \brief Confirms request to enable/disable GPRS network registration status reports. \details   */
	CI_PS_PRIM_NW_REG_IND,                                                                  /**< \brief Indicates GPRS network registration status. \details GPRS network indications may be enabled or disabled by the CI_PS_PRIM_ENABLE_NW_REG_IND_REQ request.
												 *  This indication is disabled by default. No explicit response is required  */
	CI_PS_PRIM_SET_QOS_REQ,                                                                 /**< \brief Sets QoS profile for a PDP context activation.
												 * \details The ME checks the minimum acceptable profile against the negotiated profile returned in the "Activate PDP Context Accept" message.
												 * The required quality of service profile is used when the ME sends an "Activate PDP Context Request" message to the network.
												 * Used only for 2.5G (R97) QoS profile parameters.
												 * Use CI_PS_PRIM_GET_3G_QOS_REQ for 3G (R99) QoS profile parameters.*/
	CI_PS_PRIM_SET_QOS_CNF,                                                                 /**< \brief Confirms the QoS profile setting request.
												 * \details Used only for 2.5G (R97) QoS profile parameters.
												 * Use CI_PS_PRIM_GET_3G_QOS_CNF for 3G (R99) QoS profile parameters.   */
	CI_PS_PRIM_DEL_QOS_REQ,                                                                 /**< \brief Requests to deletes the Qos profile for a PDP context.
												 * \details If a PDP context does not have a minimum or a required
												 *   QoS profile, the QoS will be determined by the network on PDP context activation.
												 * Used only for 2.5G (R97) QoS profile parameters.
												 * Use CI_PS_PRIM_DEL_3G_QOS_REQ for 3G (R99) QoS profile parameters.*/
	CI_PS_PRIM_DEL_QOS_CNF,                                                                 /**< \brief Confirms  deleting the QoS profile setting.
												 * \details Used only for 2.5G (R97) QoS profile parameters.
												 * Use CI_PS_PRIM_DEL_3G_QOS_CNF for 3G (R99) QoS profile parameters.*/
	CI_PS_PRIM_GET_QOS_REQ,                                                                 /**< \brief Requests the QoS profile for a PDP context.
												 * \details Used only for 2.5G (R97) QoS profile parameters.
												 * Use CI_PS_PRIM_GET_3G_QOS_REQ for 3G (R99) QoS profile parameters.*/
	CI_PS_PRIM_GET_QOS_CNF,                                                                 /**< \brief Reports the QoS profile for a PDP context.
												 * \details Used only for 2.5G (R97) QoS profile parameters.
												 * Use CI_PS_PRIM_GET_3G_QOS_CNF for 3G (R99) QoS profile parameters.*/
	CI_PS_PRIM_ENABLE_POWERON_AUTO_ATTACH_REQ,                              /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_PS_PRIM_ENABLE_POWERON_AUTO_ATTACH_CNF,                              /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_PS_PRIM_MT_PDP_CTX_REJECTED_IND,                                     /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_PS_PRIM_PDP_CTX_DEACTED_IND,                                         /**< \brief Indicates the PDP context has been deactivated.
										 * \details Indication will be sent if PS event reports are enabled see CI_PS_PRIM_ENABLE_EVENTS_REPORTING_REQ.*/
	CI_PS_PRIM_PDP_CTX_REACTED_IND,                                         /**< \brief \details NOT SUPPORTED REMOVE FROM API  */
	CI_PS_PRIM_DETACHED_IND,                                                    /**< \brief Indicates the ME has detached from packet service domain.
										     * \details Indication will be sent if PS event reports are enabled see CI_PS_PRIM_ENABLE_EVENTS_REPORTING_REQ.  */
	CI_PS_PRIM_GPRS_CLASS_CHANGED_IND,                                      /**< \brief Indicates the GSM/GPRS mobile class has changed. \details   */
	CI_PS_PRIM_GET_DEFINED_CID_LIST_REQ,                                    /**< \brief Requests the defined PDP context identifiers list. \details   */
	CI_PS_PRIM_GET_DEFINED_CID_LIST_CNF,                                    /**< \brief Reports the PDP context identifiers list. \details   */
	CI_PS_PRIM_GET_NW_REG_STATUS_REQ,                                       /**< \brief Requests the GPRS network registration status. \details   */
	CI_PS_PRIM_GET_NW_REG_STATUS_CNF,                                       /**< \brief Reports the GPRS network registration status. \details   */
	CI_PS_PRIM_GET_QOS_CAPS_REQ,                                            /**< \brief Requests the QoS capabilities.
										 * \details Used only for 2.5G (R97) QoS profile parameters.
										 * Use CI_PS_PRIM_GET_3G_QOS_CAPS_REQ for 3G (R99) QoS profile parameters.*/
	CI_PS_PRIM_GET_QOS_CAPS_CNF,                                            /**< \brief Reports the QoS capabilities.
										 * \details Used only for 2.5G (R97) QoS profile parameters.
										 * Use CI_PS_PRIM_GET_3G_QOS_CAPS_CNF for 3G (R99) QoS profile parameters.*/
	CI_PS_PRIM_ENABLE_EVENTS_REPORTING_REQ,                         /**< \brief Enables/disables PS event reports.
									 * \details By default events reporting indications are enabled.
									 * Event indications include the following:
									 * CI_PS_PRIM_PDP_CTX_DEACTED_IND
									 * CI_PS_PRIM_DETACHED_IND */
	CI_PS_PRIM_ENABLE_EVENTS_REPORTING_CNF,                         /**< \brief Confirms enable/disable of PS event reports. \details   */

	/* SCR #1401348: 3G Quality of Service (QoS) primitives */
	CI_PS_PRIM_GET_3G_QOS_REQ,                                                      /**< \brief Requests the 3G QoS profile for a PDP context.
											 * \details Used only for 3G (R99) QoS profile parameters.
											 * Use CI_PS_PRIM_GET_QOS_REQ to for 2.5G (R97) QoS profile parameters.*/
	CI_PS_PRIM_GET_3G_QOS_CNF,                                                      /**< \brief Reports the 3G QoS profile for a PDP context.
											 * \details Used only for 3G (R99) QoS profile parameters.
											 * Use CI_PS_PRIM_GET_QOS_CNF to for 2.5G (R97) QoS profile parameters.*/
	CI_PS_PRIM_SET_3G_QOS_REQ,                                                      /**< \brief Sets a 3G QoS profile for a PDP context activation.
											 * \details The negotiated QoS profile cannot be written by this request.
											 * If the qosType parameter is set to CI_PS_3G_QOSTYPE_NEG, CCI will return an error indication.
											 *    The Required and Minimum Quality of Service profiles are used when the MT sends an "Activate PDP Context Request" for primary or
											 *    secondary PDP context or a "Modify PDP Context Request" to the network.
											 *    Used only the 3G (R99) QoS profile parameters.
											 * Use CI_PS_PRIM_SET_QOS_REQ to set the 2.5G (R97) QoS profile parameters.*/
	CI_PS_PRIM_SET_3G_QOS_CNF,                                                      /**< \brief Confirms setting a 3G QoS profile for a PDP context.
											 * \details Used only the 3G (R99) QoS profile parameters.
											 *  Use CI_PS_PRIM_SET_QOS_CNF to for 2.5G (R97) QoS profile parameters.*/
	CI_PS_PRIM_DEL_3G_QOS_REQ,                                                      /**< \brief Requests to deletes the 3G QoS profile for a PDP context.
											 * \details The negotiated QoS profile cannot be deleted by this request.
											 * If the qosType parameter is set to CI_PS_3G_QOSTYPE_NEG, CCI will return an error indication.
											 * If a PDP context does not have a minimum or a required QoS profile,
											 * the QoS will be determined by the network on PDP context activation.
											 * Used only the 3G (R99) QoS profile parameters.
											 * Use CI_PS_PRIM_DEL_QOS_REQ  to for 2.5G (R97) QoS profile parameters.*/
	CI_PS_PRIM_DEL_3G_QOS_CNF,                                                      /**< \brief Confirms  deleting the 3G QoS profile.
											 * \details Used only the 3G (R99) QoS profile parameters.
											 * Use CI_PS_PRIM_DEL_QOS_CNF  to for 2.5G (R97) QoS profile parameters.*/

	CI_PS_PRIM_GET_3G_QOS_CAPS_REQ,                                         /**< \brief Requests the 3G QoS capabilities.
										 * \details Used only the 3G (R99) QoS profile parameters.
										 * Use CI_PS_PRIM_GET_QOS_CAPS_REQ  to for 2.5G (R97) QoS profile parameters.*/
	CI_PS_PRIM_GET_3G_QOS_CAPS_CNF,                                         /**< \brief Reports the 3G QoS capabilities.
										 * \details Used only the 3G (R99) QoS profile parameters.
										 * Use CI_PS_PRIM_GET_QOS_CAPS_CNF  to for 2.5G (R97) QoS profile parameters.  */

	/* SCR #1438547: Secondary PDP Context primitives */
	CI_PS_PRIM_DEFINE_SEC_PDP_CTX_REQ,                                      /**< \brief Defines a Secondary PDP Context. \details   */
	CI_PS_PRIM_DEFINE_SEC_PDP_CTX_CNF,                                      /**< \brief Confirms definition of a Secondary PDP Context. \details   */
	CI_PS_PRIM_DELETE_SEC_PDP_CTX_REQ,                              /**< \brief Deletes a Secondary PDP Context.
									 * \details Returns an error if the Secondary PDP Context does not exist  */
	CI_PS_PRIM_DELETE_SEC_PDP_CTX_CNF,                                      /**< \brief Confirms deletion of a Secondary PDP Context. \details   */
	CI_PS_PRIM_GET_SEC_PDP_CTX_REQ,                                         /**< \brief Requests a Secondary PDP Context definition.
										 * \details Returns an error if the Secondary PDP Context does not exist. */
	CI_PS_PRIM_GET_SEC_PDP_CTX_CNF,                                         /**< \brief Reports a Secondary PDP Context definition. Indicates an error if the Secondary PDP Context does not exist. \details   */

	/* SCR #1438547: Traffic Flow Template (TFT) primitives */
	CI_PS_PRIM_DEFINE_TFT_FILTER_REQ,                                       /**< \brief Defines a Traffic Flow Template (TFT) packet filter.
										 * \details Each PDP context connected to a particular PDP Address and APN may have an associated Traffic Flow Template (TFT),
										 * which allows filtering of downlink IP packets. A TFT must contain at least one packet filter, and may contain up to eight packet filters.
										 *  The use of Traffic Flow Templates allows multiple PDP contexts (with different quality of service) to share the same PDP Address.
										 * The TFT Packet Filters are used to route incoming IP packets to their appropriate PDP contexts.
										 * Therefore, only one of these PDP contexts may exist without an associated TFT, and the Network will route downlink packets to this PDP context if none of the TFT Packet filters apply (i.e. the "default" PDP context is the one that has no associated TFT).
										 *  A TFT (if it exists) is always associated with a PDP Context during Secondary PDP Context activation. A TFT may be added to an activated PDP Context (either a Primary or a Secondary Context) using the MS-initiated PDP Context Modify procedure, which is initiated by a CI_PS_PRIM_MODIFY_PDP_CTX_REQ request.
										 *  Traffic Flow Templates are described in 3GPP TS 23.060 v3.16.0 (Release 1999), Section 15.3.
										 *  The Packet Filter contents field (in the CiPsTftFilter structure) is defined as an octet array, preceded by the length (in octets). See Section 3.1.1 of this document. The application is expected to encode this field before being sent to CI.
										 *  The packet filter contents field encoding is specified in 3GPP TS 24.008 v3.11.0 (Release 1999), Table 10.5.162 (Section 10.5.6.12).
										 *  Returns an error if no more TFT Packet Filters are allowed  */
	CI_PS_PRIM_DEFINE_TFT_FILTER_CNF,                                                       /**< \brief Confirms definition of a TFT packet filter. \details   */
	CI_PS_PRIM_DELETE_TFT_REQ,                                                      /**< \brief Deletes the Traffic Flow Template (TFT) associated with a PDP Context. \details Deletes all Packet Filters that comprise the indicated TFT.
											 *    Returns an error in any of the following situations:
											 *    -	No TFT exists for the indicated PDP Context
											 *    -	The PDP Context itself (either Primary or Secondary) is not defined.
											 *    -	More than one PDP Context is using a single PDP Address, and deleting this TFT would violate the rule that only one PDP
											 *        Context using a particular PDP Address may exist without a TFT associated with it.
											 *    See 3GPP TS 23.060 v3.16.0, Section 15.3.1 ("Rules for Operations on TFTs").  */
	CI_PS_PRIM_DELETE_TFT_CNF,                                                              /**< \brief Confirms deletion of a Traffic Flow Template. \details   */
	CI_PS_PRIM_GET_TFT_REQ,                                                                 /**< \brief Gets the Traffic Flow Template (TFT) associated with a PDP Context. \details  Requests a list of all Packet Filters that comprise the TFT for the specified PDP Context.
												 *   Returns an error if no TFT exists for the indicated PDP Context, or if the PDP Context itself (either Primary or Secondary) is not
												 *   defined */
	CI_PS_PRIM_GET_TFT_CNF,                                                                 /**< \brief Reports the Traffic Flow Template (TFT) associated with a PDP Context. \details   */

	/* SCR TBD: PDP Context Modify primitives */
	CI_PS_PRIM_MODIFY_PDP_CTX_REQ,                                                  /**< \brief Modifies a single PDP context, or all active PDP Contexts. \details Allows the Quality of Service (QoS) and/or the Traffic Flow Template (TFT) to be modified for a PDP Context that has already been
											 *  activated. This request can be used for either Primary or Secondary PDP Contexts.
											 *  Before issuing this request, the QoS and/or the TFT should be set up or modified, using the appropriate CI requests  */
	CI_PS_PRIM_MODIFY_PDP_CTX_CNF,                                                  /**< \brief Confirms a request to modify a single PDP context, or all active PDP Contexts. \details   */
	CI_PS_PRIM_GET_ACTIVE_CID_LIST_REQ,                                     /**< \brief Requests a list of context identifiers for all active PDP Contexts. \details This request is similar to CI_PS_PRIM_GET_DEFINED_CID_LIST_REQ, except that is returns information for active PDP Contexts only.
										 *  It is provided here to support the same functionality as the "AT+CGCMOD=?" command. See 3GPP TS 27.007 Section 10.1.11.  */
	CI_PS_PRIM_GET_ACTIVE_CID_LIST_CNF,                                             /**< \brief Reports the active PDP context identifiers list. \details   */

	CI_PS_PRIM_REPORT_COUNTER_REQ,                                          /**< \brief Configures the PDP Context Data Counter Report. \details Data Counters are maintained by the Protocol Stack for all active PDP Contexts, and CCI delivers the PDP Context Data Counter
										 *  report (if enabled) using a CI_PS_PRIM_COUNTER_IND Indication.
										 *  The type parameter specifies how CCI should send Data Counter reports (see Section 3.1.1):
										 *  -	CI_PS_COUNTER_REPORT_ONE_SHOT requests a single CI_PS_PRIM_COUNTER_IND Indication to be sent as soon as the
										 *       information is received from the Protocol Stack. The interval parameter is ignored.
										 *  -	CI_PS_COUNTER_REPORT_PERIODIC requests periodic CI_PS_PRIM_COUNTER_IND Indications, to be sent at intervals specified
										 *       by the interval parameter. The minimum value for the interval parameter is 1 second; if it is set to zero, CCI uses a
										 *       1-second interval.
										 *  -	CI_PS_COUNTER_REPORT_STOP stops the Periodic report cycle, and disables CI_PS_PRIM_COUNTER_IND Indications.
										 *  This request is rejected if:
										 *  -	The Control Plane has not been attached to Packet Domain services
										 *  -	There are no active PDP Contexts.  */
	CI_PS_PRIM_REPORT_COUNTER_CNF,                                          /**< \brief Confirms configuration of the PDP context data counter report. \details   */
	CI_PS_PRIM_RESET_COUNTER_REQ,                                           /**< \brief Resets PDP context data counters. \details Data counters are maintained by the protocol stack for all active PDP contexts. Depending on the parameter settings, this request resets the data counters to zero for one specific PDP context, or for all active PDP contexts.
										 *  This request is rejected if:
										 *  -	The Control Plane is not attached to Packet Domain services;                                                        r
										 *  -	There are no active PDP Contexts;
										 *  -	The doAll parameter is FALSE and the cid parameter is invalid or does not specify an active PDP Context  */
	CI_PS_PRIM_RESET_COUNTER_CNF,                                           /**< \brief Confirms a request to reset PDP context data counter. \details   */
	CI_PS_PRIM_COUNTER_IND,                                                                 /**< \brief Indicates a PDP context data counter report. \details  CCI sends this Indication on request or periodically, as configured by CI_PS_PRIM_REPORT_COUNTER_REQ. If a periodic report cycle
												 *  is stopped, this Indication is disabled.
												 *  The totals indicate the number of bytes (octets) sent and received since the Data Counters were last reset (see
												 *  CI_PS_PRIM_RESET_COUNTER_REQ):
												 *  -	The totalULBytes counter is the total number of uplink data octets before compression (if any)
												 *  -	The totalDLBytes counter is the total number of downlink data octets after decompression (if any) */

	CI_PS_PRIM_SEND_DATA_REQ,                                                       /**< \brief \details NOT SUPPORTED REMOVE FROM API  */

	CI_PS_PRIM_SEND_DATA_CNF,                                                       /**< \brief \details NOT SUPPORTED REMOVE FROM API  */

/* Michal Bukai & Boris Tsatkin ?AT&T Smart Card support - Start*/
/*** AT&T- Smart Card  CI_PS_PRIM_ ACL SERVICE: LIST , SET , EDIT   -BT6 */
	CI_PS_PRIM_SET_ACL_SERVICE_REQ,
	CI_PS_PRIM_SET_ACL_SERVICE_CNF,


	CI_PS_PRIM_GET_ACL_SIZE_REQ,
	CI_PS_PRIM_GET_ACL_SIZE_CNF,

	CI_PS_PRIM_READ_ACL_ENTRY_REQ,
	CI_PS_PRIM_READ_ACL_ENTRY_CNF,

	CI_PS_PRIM_EDIT_ACL_ENTRY_REQ,
	CI_PS_PRIM_EDIT_ACL_ENTRY_CNF,
	/* ADD NEW COMMON PRIMITIVES HERE, BEFORE 'CI_PS_PRIM_LAST_COMMON_PRIM' */
/* Michal Bukai & Boris Tsatkin ?AT&T Smart Card support - End*/

/* Michal Bukai ?PDP authentication - Start*/
	CI_PS_PRIM_AUTHENTICATE_REQ, 	/**< \brief  Requests to add authentication parameters to a defined PDP context.
								* \details The command must be sent after the PDP context was defined and before the PDP context is activated.
								*  The authentication parameters will be sent to the GGSN in a protocol configuration information entry, when PDP context is activated.
								*  In case authentication parameters are already defined for this PDP context the new authentication parameters will replace the existing parameters.
								*  AuthenticationType  =  NONE, will delete authentication parameters defined for this PDP context.  */
	CI_PS_PRIM_AUTHENTICATE_CNF, /**< \brief Confirms the authentication request. \details   */
/* Michal Bukai ?PDP authentication - End*/
/* Michal Bukai ?Fast Dormancy - Start*/
	CI_PS_PRIM_FAST_DORMANT_REQ, 	/**< \brief  Requests to release data radio bearers, in order to speed entry to DRX mode.
								* \details The application should use this request for offline applications such as push mail, 
								* in cases that it knows that data transition is complete and there is no anticipated data transmission in the next minute. 
								* The PS will send "Signalling connection release indication" to the NW requesting to release data radio bearers. */
	CI_PS_PRIM_FAST_DORMANT_CNF, 	/**< \brief Confirms the fast dormancy request.
								* \details   The following result codes can be received:
								* CIRC_PS_SUCCESS - Indicates "Signalling connection release indication" was sent to the NW
								* CIRC_PS_FAILURE Indicates "Signalling connection release indication" was not sent to the NW due to one of the following reasons:
								* Active RAT is not UMTS
								* There no active PDP contexts
								* There is an active CS connection
								* RRC state is not CELL DCH or CELL FACH
								* CIRC_PS SRVOPT_NOT_SUPPORTED -Indicates fast dormancy is not supported. */
/* Michal Bukai ?Fast Dormancy - End*/

  CI_PS_PRIM_GET_CURRENT_JOB_REQ, /**< \brief Requests current ongoing request for PS service group. */
  CI_PS_PRIM_GET_CURRENT_JOB_CNF, /**< \brief Confirms the current job request. */

#ifdef AT_CUSTOMER_HTC
  CI_PS_PRIM_SET_FAST_DORMANCY_CONFIG_REQ, /**< \brief Requests to set configuration of fast dormancy. */
  CI_PS_PRIM_SET_FAST_DORMANCY_CONFIG_CNF, /**< \brief Confirms the configuration of fast dormancy. */

  CI_PS_PRIM_PDP_ACTIVATION_REJECT_CAUSE_IND, /**< \brief Indicates SM cause code for PDP activation reject. */
#endif
	/* END OF COMMON PRIMITIVES LIST */
	CI_PS_PRIM_LAST_COMMON_PRIM

	/* the  customer specific extension primitives will be added starting from
	 * CI_PS_PRIM_firstCustPrim = CI_PS_PRIM_LAST_COMMON_PRIM as the first identifier.
	 * The actual primitive names and IDs are defined in the associated
	 * 'ci_ps_cust_xxx.h' file.
	 */

	/* DO NOT ADD ANY MORE PRIMITIVES HERE */

} _CiPsPrim;

/* specify the number of default common DAT primitives */
#define CI_PS_NUM_COMMON_PRIM ( CI_PS_PRIM_LAST_COMMON_PRIM - 1 )

/* Maximum number of PDP Contexts supported.
 * Implementation specific. Should be 1 if only a single subscribed context is supported, otherwise should be
 * greater than possible NSAPIs, NSAPI only support [5-15] according to [6], 10.5.6.2
 */
//#define CI_PS_MAX_PDP_CTX_NUM CI_CFG_PS_MAX_PDP_CTX_NUM

/* NOTE: THE MAXIMUM NUMBER OF PDP CONTEXTS WAS TEMPORARILY REDUCED TO 8 IN ORDER TO */
/* MAKE THE PRIMITIVE CI_PS_PRIM_GET_3G_QOS_CAPS_CNF SMALLER BECAUSE OF THE SIZE     */
/* LIMIT IN MSL BUFFER (MSL_MTU_SIZE = 1024 BYTES ON COMM SUBSYSTEM). THIS MUST BE   */
/* RESTORED ONCE PRIMITIVE SEGMENTATION IS IMPLEMENTED IN CI STUBS                   */
#define CI_PS_MAX_PDP_CTX_NUM 8
#define CI_PS_MAX_MO_AND_MT_PDP_CTX_NUM    ( CI_PS_MAX_PDP_CTX_NUM + 2 )
/**@}*/

/** \brief CI Return Codes for the PS Service Group. Refer to TS 24.008 v3.11.0. */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIRC_PS
{
	CIRC_PS_SUCCESS = 0,                                    /**< Request completed successfully */
	CIRC_PS_FAILURE,                                        /**< Request failed */

	/* failure to perform an Attach */
	CIRC_PS_ILLEGAL_MS = 3,                         /**< Illegal MS */
	CIRC_PS_ILLEGAL_ME = 6,                         /**< Illegal ME */
	CIRC_PS_GPRS_SERVICES_NOT_ALLOWED,              /**< GPRS service not allowed */
	CIRC_PS_PLMN_NOT_ALLOWED = 11,                  /**< PLMN not allowed */
	CIRC_PS_LA_NOT_ALLOWED,                         /**< Location area not allowed */
	CIRC_PS_ROAMING_NOT_ALLOWED,                    /**< Roaming not allowed in this location area */

	/* failure to Activate a Context */
	CIRC_PS_SRVOPT_NOT_SUPPORTED = 32,      /**< Service option not supported */
	CIRC_PS_SRVOPT_NOT_SUBSCRIBED,          /**< Requested service option not subscribed */
	CIRC_PS_SRVOPT_TEMP_OUT_OF_ORDER,       /**< Service option temporarily out of order */
	CIRC_PS_PDP_AUTHEN_FAILURE = 149,       /**< PDP authentication failure */

	/* other GPRS errors */
	CIRC_PS_INVALID_MS_CLASS,                       /**< Invalid mobile class */
	CIRC_PS_UNSPECIFIED_ERROR = 148,                /**< Unspecified GPRS error */

	/* Additional return codes, not specified in TS 24.008 */
	CIRC_PS_INFO_UNAVAILABLE,                       /**< Requested information is unvailable */
  CIRC_PS_ALREADY_PROCESSING,				   /**< The requested command is already being processed, I.e., this REQ is redundant */
  CIRC_PS_BUSY_WITH_OTHER_JOB,				   /**< The CP is busy processing another command so this one can't be serviced, and CP will not add the REQ into its queue */
	CIRC_PS_NUM_RESCODES                            /**< Number of Result Codes */

} _CiPsRc;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief CI Return Codes for the PS Service Group. Refer to TS 24.008 v3.11.0
 *  \sa CIRC_PS
 * \remarks Common Data Section */
typedef UINT16 CiPsRc;
/**@}*/

/** \brief PDP type values */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIPSPDPTYPE_TAG
{
	CI_PS_PDP_TYPE_PPP = 0,         /**< PPP */
	CI_PS_PDP_TYPE_IP,              /**< IPv4 */
	CI_PS_PDP_TYPE_IPV6,            /**< IPv6 */

	CI_PS_PDP_NUM_TYPES
} _CiPsPdpType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief PDP Type
 *  \sa CIPSPDPTYPE_TAG
 * \remarks Common Data Section */
typedef UINT8 CiPsPdpType;
/**@}*/

/** \brief Action type values */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIPSACTIONTYPE_TAG
{
	CIRC_PS_ACT_IND_ACTION = 0,     /**< Indicates network intiated PDD context activation. */
	CIRC_PS_MODIFY_IND_ACTION,      /**< Indicates network intiated PDD context modification. */
	CIRC_PS_NUMBER_OF_ACTION_TYPES
}_CiPsActionType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief Action type
 *  \sa CIPSACTIONTYPE_TAG
 * \remarks Common Data Section */
typedef UINT8 CiPsActionType;

#define CI_PS_APN_MIN_SIZE 1
#define CI_PS_APN_MAX_SIZE 100
#define CI_PS_PDP_IP_V4_SIZE 4
#define CI_PS_PDP_IP_V6_SIZE 16
/**@}*/

/** \brief Pdp address string */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiPsPdpAddr_struct
{
	UINT8 len;                                                              /**< Length of the address field [CI_PS_PDP_IP_V4_SIZE| CI_PS_PDP_IP_V6_SIZE] */

	/* !!!!!!!!!!!!!!!!!!!
	 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
	 * & CCI_APP_NONCONTIGUOUS flags must be removed.
	 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
	 * # Start Contiguous Code Section # and # End Contiguous Code Section #
	 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
	 */
#ifndef CCI_PS_CONTIGUOUS
	UINT8   *val;                        /**< Address field */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	/*Temporary pointer used to ensure the correct structure size for transmission
	   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	UINT8   *valTempNonContPtr;
#endif
	/* # Start Contiguous Code Section # */
	UINT8 valData[CI_PS_PDP_IP_V6_SIZE];            /**< Address field */
	/* # End Contiguous Code Section # */
#endif
} CiPsPdpAddr; /* PDP address */


/** \brief PDP data compression values, only applicable to SNDCP. */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIPSDCOMP_TAG
{
	CI_PS_DCOMP_OFF = 0,                    /**< Off, this is the default vlaue */
	CI_PS_DCOMP_ON,                         /**< On, manufacturer preferred compression */
	CI_PS_DCOMP_V42bis,                     /**< V.42 bis */

	CI_PS_NUM_DCOMPS
} _CiPsDcomp;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief PDP data compression, only applicable to SNDCP
 *  \sa CIPSDCOMP_TAG
 * \remarks Common Data Section */
typedef UINT8 CiPsDcomp;
/**@}*/

/** \brief PDP header compression values */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIPSHCOMP_TAG
{
	CI_PS_HCOMP_OFF = 0,    /**< Off, this is the default vlaue */
	CI_PS_HCOMP_TCPIP,      /**< TCPIP header compression - RFC 1144 */
	CI_PS_HCOMP_IP,         /**< IP header compression - RFC 2507 */

	CI_PS_NUM_HCOMPS
} _CiPsHcomp;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief PDP header compression
 *  \sa CIPSHCOMP_TAG
 * \remarks Common Data Section */
typedef UINT8 CiPsHcomp;
/**@}*/

/** \brief PDP context definition. */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiPsPdpCtx_struct
{
	UINT8 cid;                                                              /**< PDP Context Identifier, [0- CI_PS_MAX_PDP_CTX_NUM-1] */
	CiPsPdpType type;                                                       /**< PDP type. \sa CiPsPdpType */

/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */
#ifndef CCI_PS_CONTIGUOUS
	CiString               *pApn;                                           /**< APN, length range [CI_PS_APN_MIN_SIZE - CI_PS_APN_MAX_SIZE]. \sa CCI API Ref Manual */
	CiPsPdpAddr   *pAddr;                                                   /**< PDP address.  \sa CiPsPdpAddr_struct */
	CiPsDcomp     *pDcomp;                                                  /**< PDP data compression, only applicable to SNDCP, should be ignored for UMTS.   \sa CiPsDcomp */
	CiPsHcomp     *pHcomp;                                                  /**< PDP header compression.   \sa CiPsHcomp */
	CiString               *pPdParas;                                       /**< PDP specific paramters. \sa CCI API Ref Manual */
#else
#ifdef CCI_APP_NONCONTIGUOUS
/*Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	CiString              *pApnTempNonContPtr;
	CiPsPdpAddr   *pAddrTempNonContPtr;
	CiPsDcomp     *pDcompTempNonContPtr;
	CiPsHcomp     *pHcompTempNonContPtr;
	CiString              *pPdParasTempNonContPtr;
#endif  /*#ifdef CCI_APP_NONCONTIGUOUS*/
/* # Start Contiguous Code Section # */
	CiBoolean apnPresent;                                   /**< Flag indicating that the apn is present (Optional Field). \sa CCI API Ref Manual*/
	CiString apn;                                                   /**< APN, length range [CI_PS_APN_MIN_SIZE - CI_PS_APN_MAX_SIZE]. \sa CCI API Ref Manual */
	CiBoolean addrPresent;                                          /**< Flag indicating that the addr is present (Optional Field). \sa CCI API Ref Manual */
	CiPsPdpAddr addr;                                               /**< PDP address. \sa CiPsPdpAddr_struct */
	CiBoolean dcompPresent;                                         /**< Flag indicating that Data Compression Field is Present. \sa CCI API Ref Manual */
	CiPsDcomp dcomp;                                                /**< PDP data compression, only applicable to SNDCP, should be ignored for UMTS. \sa CiPsDcomp */
	CiBoolean hcompPresent;                                         /**< Flag indicating that Header Compression Field is Present. \sa CCI API Ref Manual */
	CiPsHcomp hcomp;                                                /**< PDP header compression. \sa CiPsHcomp */
	CiBoolean pdParasPresent;                                       /**< Flag indicating that the pdParas is present (Optional Field). \sa CCI API Ref Manual*/
	CiString pdParas;                                               /**< PDP specific paramters. \sa CCI API Ref Manual */
/* # End Contiguous Code Section # */
#endif /*#ifndef CCI_PS_CONTIGUOUS*/
} CiPsPdpCtx;

/** \brief PDP context information. */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiPsPdpCtxInfo_struct
{
	CiBoolean actState; /**< activation state (TRUE = Activate). \sa CCI API Ref Manual */

/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */
#ifndef CCI_PS_CONTIGUOUS
	CiPsPdpCtx *pPdpCtx; /**< PDP context paramters. \sa CiPsPdpCtx_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
/*Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	CiPsPdpCtx *pPdpCtxTempNonContPtr;
#endif
/* # Start Contiguous Code Section # */
	CiPsPdpCtx pdpCtx;   /**< PDP context paramters. \sa CiPsPdpCtx_struct */
/* # Start Contiguous Code Section # */
#endif
} CiPsPdpCtxInfo;

/** \brief PDP context capability. */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiPsPdpCtxCap_struct
{
	CiNumericRange cids;    /**< PDP context identifier [0- CI_PS_MAX_PDP_CTX_NUM-1]. \sa CCI API Ref Manual  */
	CiPsPdpType type;       /**< PDP type. \sa CiPsPdpType  */
	CiBitRange bitsDcomp;   /**< Data compression capability, represented as bit mask. Each bit represents a value in CiPsDcomp  \sa CiPsDcomp*/
	/* each bit represents a capability in CiPsDcomp,
				       e.g. (bitsDcomp&(1<< CI_PS_HCOMP_OFF))!=0 means
					      CI_PS_DCOMP_OFF is supported,
					    (bitsDcomp&(1<<CI_PS_DCOMP_ON))!=0 means
					      CI_PS_DCOMP_ON  is supported */
	CiBitRange bitsHcomp;   /**< Header compression capability, represented as bit mask. Each bit represents a value in CiPsHcomp  \sa CiPsHcomp*/

	/* TBD, not sure <pd1> to <pdn> is going to be presented */
} CiPsPdpCtxCap;

//ICAT EXPORTED STRUCT

/** \brief PDP context capability profiles. */
/** \remarks Common Data Section */
typedef struct CiPsPdpCtxCaps_struct
{
	UINT8 size;         /**< Number of capability profiles - currently only one profile is supported */

/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */
#ifndef CCI_PS_CONTIGUOUS
	CiPsPdpCtxCap *pCaps;  /**< PDP context capabilities. \sa CiPsPdpCtxCap_struct*/
#else
#ifdef CCI_APP_NONCONTIGUOUS
/* Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  THIS POINTER IS NOT USED FOR ANYTHING ...
   Remove when the transition to full contiguous is complete*/
	CiPsPdpCtxCap *pCapsTempNonContPtr;
#endif  /*#ifdef CCI_APP_NONCONTIGUOUS*/
/* # Start Contiguous Code Section # */
	CiPsPdpCtxCap caps[CI_CURRENT_SUPPORT_PDP_NUM_TYPE]; /**< PDP context capabilities. \sa CiPsPdpCtxCap_struct*/
/* # Start Contiguous Code Section # */
#endif  /*CCI_PS_CONTIGUOUS*/
} CiPsPdpCtxCaps;

/** \brief Activated status of the PDP context. */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiPsPdpCtxActState_struct
{
	UINT8 cid;                      /**< PDP context identification, [0- CI_PS_MAX_PDP_CTX_NUM-1] */
	CiBoolean activated;            /**< TRUE = activated; FALSE = deactivated. \sa CCI API Ref Manual */
} CiPsPdpCtxActState;

/** \addtogroup  SpecificSGRelated
 * @{ */
/** \brief pointer to PDP activated status list
 * \sa  CiPsPdpCtxActState_struct
 * \remarks Common Data Section */
typedef CiPsPdpCtxActState *CiPsPdpCtxActStateListPtr;
/**@}*/

/** \brief L2 protocol type values. */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIPSL2P_TAG
{
	CI_PS_L2P_NONE = 0,     /**< Not PPP */
	CI_PS_L2P_PPP,          /**< PPP */

	CI_PS_NUM_L2PS
} _CiPsL2P;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief L2 protocol type
 *  \sa CIPSL2P_TAG
 * \remarks Common Data Section */
typedef UINT8 CiPsL2P;
/**@}*/

/** \brief GSM/GPRS mobile class values. */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIPSGSMGPRSCLASS_TAG
{
	CI_PS_GSMGPRS_CLASS_A = 0,      /**< class A */
	CI_PS_GSMGPRS_CLASS_B,          /**< class B */
	CI_PS_GSMGPRS_CLASS_CS,         /**< class C, GPRS only */
	CI_PS_GSMGPRS_CLASS_CC,         /**< class C, circuit switch only */

	CI_PS_GSMGPRS_NUM_CLASSES
} _CiPsGsmGprsClass;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief Mobile class for GSM/GPRS
 *  \sa CIPSGSMGPRSCLASS_TAG
 * \remarks Common Data Section */
typedef UINT8 CiPsGsmGprsClass;
/**@}*/

//ICAT EXPORTED ENUM
/** \brief Network registration configuration flags values. */
/** \remarks Common Data Section */
typedef enum CIPSNWREGINDFLAG_TAG
{
	CI_PS_NW_REG_IND_DISABLE = 0,           /* Disable network registration status reports */
	CI_PS_NW_REG_IND_ENABLE_STA_ONLY,       /* Enable network registration status reports */
	CI_PS_NW_REG_IND_ENABLE_DETAIL          /* Enable detailed network registration status reports */
} _CiPsNwRegIndFlag;

/** \addtogroup  SpecificSGRelated
 * @{ */
/** \brief Configures nework registration status reports
 *  \sa CIPSNWREGINDFLAG_TAG
 * \remarks Common Data Section */
typedef UINT8 CiPsNwRegIndFlag;
/**@}*/


/** \brief Network registration status values. */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIPSNWREGSTATUS_TAG
{
	CI_PS_NW_REG_STA_NOT_REGED = 0,         /**< Not registered */
	CI_PS_NW_REG_STA_REG_HPLMN,             /**< Registered on home PLMN */
	CI_PS_NW_REG_STA_TRYING,                /**< Not registered, but cellular subsystem is trying to attach or search an operator to register to */
	CI_PS_NW_REG_STA_REG_DENIED,            /**< Registration denied */
	CI_PS_NW_REG_STA_UNKNOWN,               /**< Unknown */
	CI_PS_NW_REG_STA_REG_ROAMING            /**< Registered on visited PLMN */
} _CiPsNwRegStatus;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief Network registration status
 *  \sa CIPSNWREGSTATUS_TAG
 * \remarks Common Data Section */
typedef UINT8 CiPsNwRegStatus;
/**@}*/

/** \brief Network registration information. */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiPsNwRegInfo_struct
{
	CiPsNwRegStatus status;                                 /**< Network registration status. \sa CiPsNwRegStatus */
	CiBoolean present;                                      /**< Indicates if LAC and Cell ID are present. \sa CCI API Ref Manual */
	UINT16 lac;                                             /**< Location Area Code */
    UINT32          		cellId;     			/**< Cell ID */
} CiPsNwRegInfo;

/** \brief 2.5G (R97) QoS: Reliability Class. See 3GPP TS 24.008 v.3.11.0, Section 10.5.6.5 */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIPSQOSRELIABILITYCLASS
{
	CI_PS_QOS_RELIABILITY_CLASS_SUBSCRIBED = 0,     /* Subscribed value */
	CI_PS_QOS_RELIABILITY_CLASS_1,                  /* Ack GTP, LLC, RLC; protected data */
	CI_PS_QOS_RELIABILITY_CLASS_2,                  /* Unack GTP; Ack LLC, RLC; protected data */
	CI_PS_QOS_RELIABILITY_CLASS_3,                  /* Unack GTP, LLC; Ack RLC; protected data */
	CI_PS_QOS_RELIABILITY_CLASS_4,                  /* Unack GTP, LLC, RLC; protected data */
	CI_PS_QOS_RELIABILITY_CLASS_5,                  /* Unack GTP, LLC, RLC; unprotected data */

	CI_PS_QOS_NUM_RELIABILITY_CLASSES
} _CiPsQosReliabilityClass;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief Reliability Class
 *  \sa CIPSQOSRELIABILITYCLASS
 * \remarks Common Data Section */
typedef UINT8 CiPsQosReliabilityClass;
/**@}*/

/** \brief 2.5G (R97) Quality of Service (QoS) Profile. See 3GPP TS 24.008 v.3.11.0, Section 10.5.6.5 */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiPsQosProfile_struct
{
	UINT8 precedence;       /**< Precedence class */
	UINT8 delay;            /**< Delay class */
	UINT8 reliability;      /**< Reliability class */
	UINT8 peak;             /**< Peak throughput */
	UINT8 mean;             /**< Mean throughput */
} CiPsQosProfile;

// --------------- SCR #1401348: BEGIN ------------------------------------

/** \brief Secondary PDP Context, as defined for the "AT+CGDSCONT" command */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiPsSecPdpCtx_struct
{
	UINT8 cid;                                                      /**< PDP Context Identifier */
	UINT8 p_cid;                                                    /**< Primary PDP Context Identifier */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */
#ifndef CCI_PS_CONTIGUOUS
	CiPsDcomp     *pDcomp;                          /**< PDP data compression, only applicable to SNDCP, should be ignored for UMTS. \sa CiPsDcomp */
	CiPsHcomp     *pHcomp;                          /**< PDP header compression. \sa CiPsHcomp */
#else
#ifdef CCI_APP_NONCONTIGUOUS
/* Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  THIS POINTER IS NOT USED FOR ANYTHING ...
   Remove when the transition to full contiguous is complete*/
	CiPsDcomp     *pDcompTempNonContPtr;    /**< PDP data compression, only applicable to SNDCP, should be ignored for UMTS. \sa CiPsDcomp */
	CiPsHcomp     *pHcompTempNonContPtr;    /**< PDP header compression. \sa CiPsDcomp */
#endif /*#ifdef CCI_APP_NONCONTIGUOUS*/
/* # Start Contiguous Code Section # */
	CiBoolean dcompPresent;                         /**< TRUE - if present; \sa CCI API Ref Manual */
	CiBoolean hcompPresent;                         /**< TRUE - if present; \sa CCI API Ref Manual */
	CiPsDcomp dcomp;                                /**< PDP data compression, only applicable to SNDCP, should be ignored for UMTS. \sa CiPsDcomp */
	CiPsHcomp hcomp;                                /**< PDP header compression.  \sa CiPsHcomp */
/* # End Contiguous Code Section # */
#endif /*#ifdef CCI_PS_CONTIGUOUS*/

} CiPsSecPdpCtx;

/** \brief Secondary PDP Context Information */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiPsSecPdpCtxInfo_struct
{
	CiBoolean actState;                                     /**< Activation state. \sa CCI API Ref Manual */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */
#ifndef CCI_PS_CONTIGUOUS
	CiPsSecPdpCtx                 *pPdpCtx;                 /**< Secondary PDP context. \sa CiPsSecPdpCtx_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
/* Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  THIS POINTER IS NOT USED FOR ANYTHING ...
   Remove when the transition to full contiguous is complete*/
	CiPsSecPdpCtx          *pSecPdpCtxTempNonContPtr;
#endif /*#ifdef CCI_APP_NONCONTIGUOUS*/
/* # Start Contiguous Code Section # */
	CiPsSecPdpCtx secPdpCtx;                                /**< Secondary PDP Context Info. \sa CiPsSecPdpCtx_struct */
/* # End Contiguous Code Section # */
#endif /*#ifndef CCI_PS_CONTIGUOUS*/
} CiPsSecPdpCtxInfo;

/** \brief QoS type values */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIPS3GQOSTYPE_TAG
{
	CI_PS_3G_QOSTYPE_MIN = 0,       /**< Minimum QoS */
	CI_PS_3G_QOSTYPE_REQ,           /**< Requested QoS */
	CI_PS_3G_QOSTYPE_NEG,           /**< Negotiated QoS */

	CI_PS_3G_QOSTYPE_NUMTYPES
} _CiPs3GQosType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief QoS type
 *  \sa CIPS3GQOSTYPE_TAG
 * \remarks Common Data Section */
typedef UINT8 CiPs3GQosType;
/**@}*/

/** \brief 3G QoS: traffic class values. See 3GPP TS 27.007 v3.11.0, Sections 10.1.6 through 10.1.8 */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIPS3GTRAFFICCLASS_TAG
{
	CI_PS_3G_TRAFFIC_CLASS_CONVERSATIONAL = 0,      /**< Conversational */
	CI_PS_3G_TRAFFIC_CLASS_STREAMING,               /**< Streaming */
	CI_PS_3G_TRAFFIC_CLASS_INTERACTIVE,             /**< Interactive */
	CI_PS_3G_TRAFFIC_CLASS_BACKGROUND,              /**< Background */
	CI_PS_3G_TRAFFIC_CLASS_SUBSCRIBED,              /**< Subscribed Value */

	CI_PS_3G_TRAFFIC_CLASS_NUMCLASSES
} _CiPs3GTrafficClass;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief 3G QoS: traffic class. See 3GPP TS 27.007 v3.11.0, Sections 10.1.6 through 10.1.8
 *  \sa CIPS3GTRAFFICCLASS_TAG
 * \remarks Common Data Section */
typedef UINT8 CiPs3GTrafficClass;
/**@}*/

/** \brief 3G QoS: Delivery Order values. See 3GPP TS 27.007 v3.11.0, Sections 10.1.6 through 10.1.8. */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIPS3GDLVORDER_TAG
{
	CI_PS_3G_DLV_ORDER_NO = 0,                              /**< Without delivery order ('no') */
	CI_PS_3G_DLV_ORDER_YES,                                 /**< With delivery order ('yes') */
	CI_PS_3G_DLV_ORDER_SUBSCRIBED,                          /**< Subscribed value */

	CI_PS_3G_NUM_DLV_ORDER
} _CiPs3GDlvOrder;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief 3G QoS: Delivery Order values. See 3GPP TS 27.007 v3.11.0, Sections 10.1.6 through 10.1.8
 *  \sa CIPS3GDLVORDER_TAG
 * \remarks Common Data Section */
typedef UINT8 CiPs3GDlvOrder;
/**@}*/

/** \brief 3G QoS: Delivery of Erroneous SDU's. See 3GPP TS 27.007 v3.11.0, Sections 10.1.6 through 10.1.8. */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIPS3GDLVERRORSDU_TAG
{
	CI_PS_3G_DLV_ERROR_SDU_NO = 0,                          /**< Erroneous SDUs are not delivered ('no') */
	CI_PS_3G_DLV_ERROR_SDU_YES,                             /**< Erroneous SDUs are delivered ('yes') */
	CI_PS_3G_DLV_ERROR_SDU_NODETECT,                        /**< No detect ('-') */
	CI_PS_3G_DLV_ERROR_SDU_SUBSCRIBED,                      /**< Subscribed Value */

	CI_PS_3G_NUM_DLV_ERROR_SDU
} _CiPs3GDlvErrorSdu;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief  3G QoS: Delivery of Erroneous SDU's. See 3GPP TS 27.007 v3.11.0, Sections 10.1.6 through 10.1.8.
 *  \sa CIPS3GDLVERRORSDU_TAG
 * \remarks Common Data Section */
typedef UINT8 CiPs3GDlvErrorSdu;
/**@}*/

/** \brief 3G QoS: Residual BER values. (TS 27.007 does not define values), See 3GPP TS 24.008 v3.11.0, Section 10.5.6.5. */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIPS3GRESIDUALBER_TAG
{
	CI_PS_3G_RESIDUAL_BER_SUBSCRIBED = 0,           /**< Subscribed value */
	CI_PS_3G_RESIDUAL_BER_5EM2,                     /**< 5 * 10^-2 */
	CI_PS_3G_RESIDUAL_BER_1EM2,                     /**< 1 * 10^-2 */
	CI_PS_3G_RESIDUAL_BER_5EM3,                     /**< 5 * 10^-3 */
	CI_PS_3G_RESIDUAL_BER_4EM3,                     /**< 4 * 10^-3 */
	CI_PS_3G_RESIDUAL_BER_1EM3,                     /**< 1 * 10^-3 */
	CI_PS_3G_RESIDUAL_BER_1EM4,                     /**< 1 * 10^-4 */
	CI_PS_3G_RESIDUAL_BER_1EM5,                     /**< 1 * 10^-5 */
	CI_PS_3G_RESIDUAL_BER_1EM6,                     /**< 1 * 10^-6 */
	CI_PS_3G_RESIDUAL_BER_6EM8,                     /**< 6 * 10^-8 */

	CI_PS_3G_NUM_RESIDUAL_BER
} _CiPs3GResidualBer;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief  3G QoS: Residual BER. (TS 27.007 does not define values), See 3GPP TS 24.008 v3.11.0, Section 10.5.6.5
 *  \sa CIPS3GRESIDUALBER_TAG
 * \remarks Common Data Section */
typedef UINT8 CiPs3GResidualBer;
/**@}*/

/** \brief 3G QoS: SDU Error Ratio values. (TS 27.007 does not define values). See 3GPP TS 24.008 v3.11.0, Section 10.5.6.5. */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIPS3GSDUERRORRATIO_TAG
{
	CI_PS_3G_SDU_ERROR_RATIO_SUBSCRIBED = 0,        /**< Subscribed value */
	CI_PS_3G_SDU_ERROR_RATIO_1EM2,                  /**< 1 * 10^-2 */
	CI_PS_3G_SDU_ERROR_RATIO_7EM3,                  /**< 7 * 10^-3 */
	CI_PS_3G_SDU_ERROR_RATIO_1EM3,                  /**< 1 * 10^-3 */
	CI_PS_3G_SDU_ERROR_RATIO_1EM4,                  /**< 1 * 10^-4 */
	CI_PS_3G_SDU_ERROR_RATIO_1EM5,                  /**< 1 * 10^-5 */
	CI_PS_3G_SDU_ERROR_RATIO_1EM6,                  /**< 1 * 10^-6 */
	CI_PS_3G_SDU_ERROR_RATIO_1EM1,                  /**< 1 * 10^-1 */

	CI_PS_3G_NUM_SDU_ERROR_RATIOS
} _CiPs3GSduErrorRatio;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief  3G QoS: SDU Error Ratio. (TS 27.007 does not define values). See 3GPP TS 24.008 v3.11.0, Section 10.5.6.5
 *  \sa CIPS3GSDUERRORRATIO_TAG
 * \remarks Common Data Section */
typedef UINT8 CiPs3GSduErrorRatio;
/**@}*/

/** \brief 3G QoS: Traffic Handling Priority values. (TS 27.007 does not define values). See 3GPP TS 24.008 v3.11.0, Section 10.5.6.5. */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIPS3GTRAFFICPRIORITY_TAG
{
	CI_PS_3G_SDU_TRAFFIC_PRIORITY_SUBSCRIBED = 0,           /**< Subscribed value */
	CI_PS_3G_SDU_TRAFFIC_PRIORITY_LEVEL_1,                  /**< Priority Level 1 */
	CI_PS_3G_SDU_TRAFFIC_PRIORITY_LEVEL_2,                  /**< Priority Level 2 */
	CI_PS_3G_SDU_TRAFFIC_PRIORITY_LEVEL_3,                  /**< Priority Level 3 */

	CI_PS_3G_NUM_TRAFFIC_PRIORITIES
} _CiPs3GTrafficPriority;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief  3G QoS: Traffic Handling Priority. (TS 27.007 does not define values). See 3GPP TS 24.008 v3.11.0, Section 10.5.6.5.
 *  \sa CIPS3GTRAFFICPRIORITY_TAG
 * \remarks Common Data Section */
typedef UINT8 CiPs3GTrafficPriority;

/* 3G QoS: Value used for "Subscribed" setting in binary coded QoS parameter fields */
#define CI_3G_QOS_SUBSCRIBED_BINARY_VALUE 0
/**@}*/

/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief  2.5G (R97) Quality of Service (QoS) profile. See TS 24.008 v3.11.0, Section 10.5.6.5 for the parameter field definitions. The profile structure itself is defined above -- this is just an alias.
 * \remarks Common Data Section */
typedef CiPsQosProfile CiPs25GQosProfile;
/**@}*/

/** \brief 3G (R99) Quality of Service (QoS) profile. See TS 24.008 v3.11.0, Section 10.5.6.5 for the parameter field definitions.
 * Note that *all* QoS information is required from the MS, if it's available.
 * We need both the "R97" parameters (defined above) and the 3G ("R99") parameters. */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiPs3GQosProfile_struct
{
	CiPs3GTrafficClass trafficClass;                                                /**< Traffic Class. \sa  CiPs3GTrafficClass */
	CiPs3GDlvOrder deliveryOrder;                                                   /**< Delivery Order. \sa  CiPs3GDlvOrder */
	CiPs3GDlvErrorSdu deliveryOfErrSdu;                                             /**< Delivery of Erroneous SDU's. \sa  CiPs3GDlvErrorSdu */
	CiPs3GResidualBer resBER;                                                       /**< Residual Bit Error Rate. \sa  CiPs3GResidualBer */
	CiPs3GSduErrorRatio sduErrRatio;                                                /**< SDU Error Ratio. \sa  CiPs3GSduErrorRatio */
	CiPs3GTrafficPriority thPriority;                                               /**< Traffic Handling Priority (Interactive Class only). \sa  CiPs3GTrafficPriority */

	UINT8 transDelay;                                                               /**< Transfer Delay (Conversational/Streaming Classes only) */
	UINT8 maxSduSize;                                                               /**< Max SDU Size */
	UINT16 maxULRate;                                                               /**< Max Bit Rate, Uplink */
	UINT16 maxDLRate;                                                               /**< Max Bit Rate, Downlink */
	UINT16 guaranteedULRate;                                                        /**< Guaranteed Bit Rate, Uplink */
	UINT16 guaranteedDLRate;                                                        /**< Guaranteed Bit Rate, Downlink */
} CiPs3GQosProfile;

/** \addtogroup  SpecificSGRelated
 * @{ */
/** \brief  Address mask.
 * \sa  CiPsPdpAddr_struct
 * \remarks Common Data Section */
typedef CiPsPdpAddr CiPsPdpAddrMask;


/* Packet Filter for Traffic Flow Template (TFT), as defined for the "AT+CGTFT" command.
 * References:  3GPP TS 27.007 Section 10.1.3
 *              3GPP TS 23.060 Section 15.3
 *              3GPP TS 24.008 Section 10.5.6.12
 *
 * Note: In reference 24.008 (10.5.6.12) indicates that there are also a single destination
 *       and single source port types, but these are not specified as part of the AT command
 *       in the 27.007 (10.1.3).
 */

#define CI_PS_MAX_TFT_FILTERS           8
#define CI_PS_MAX_IPV4_FILTER_LENGTH    32
#define CI_PS_MAX_IPV6_FILTER_LENGTH    60

#define CI_PS_MAX_FILTER_LENGTH         CI_PS_MAX_IPV4_FILTER_LENGTH
#define CI_PS_MAX_FILTER_CONTENTS       ( CI_PS_MAX_FILTER_LENGTH - 3 )
/**@}*/

/** \brief TftFilter structure. */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiPsTftFilter_struct
{
	UINT8 pfId;                                                                                                     /**< Packet Filter Identifier */
	UINT8 epIndex;                                                                                                  /**< Evaluation Precedence Index */
	UINT8 pIdNextHdr;                                                                                               /**< Protocol number (ipv4)/ next header (ipv6) */
	CiBoolean pIdNextHdrPresent; /**< TRUE - if present; \sa CCI API Ref Manual */                                  //Michal
	UINT8 tosTc;                                                                                                    /**< Type of Service/ Traffic Class */
	CiBoolean tosPresent; /**< TRUE - if present; \sa CCI API Ref Manual */                                         //Michal
	UINT8 tosTcMask;                                                                                                /**< Type of Service/ Traffic Class Mask */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */
#ifndef CCI_PS_CONTIGUOUS
	CiPsPdpAddr                   *pSrcAddr;                /**< Source Address. \sa CiPsPdpAddr_struct */
	CiPsPdpAddrMask       *pSrcAddrMask;                    /**< Source Address Mask - Subnet Mask. \sa CiPsPdpAddrMask */
#else
#ifdef CCI_APP_NONCONTIGUOUS
/* Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  THIS POINTER IS NOT USED FOR ANYTHING ...
   Remove when the transition to full contiguous is complete*/
	CiPsPdpAddr                   *pSrcAddrTempNonContPtr;
	CiPsPdpAddrMask       *pSrcAddrMaskTempNonContPtr;
#endif /*#ifdef CCI_APP_NONCONTIGUOUS*/
#endif  /*#ifndef CCI_PS_CONTIGUOUS*/

	CiNumericRange dstPortRange;            /**< destination port range. \sa CCI API Ref Manual */
	CiBoolean dstPortRangePresent; /**< TRUE - if present; \sa CCI API Ref Manual */                        //Michal
	CiNumericRange srcPortRange;                                                                            /**< source port range. \sa CCI API Ref Manual */
	CiBoolean srcPortRangePresent; /**< TRUE - if present; \sa CCI API Ref Manual */                        //Michal

	UINT32 ipSecSPI;                                                                                        /**< IPSec Security Parameter Index */
	CiBoolean ipSecSPIPresent; /**< TRUE - if present; \sa CCI API Ref Manual */                            //Michal
	UINT32 flowLabel;                                                                                       /**< Flow label */
	CiBoolean flowLabelPresent; /**< TRUE - if present; \sa CCI API Ref Manual */                           //Michal

#ifdef CCI_PS_CONTIGUOUS
/* # Start Contiguous Code Section # */
	CiPsPdpAddr srcAddr;                                    /**< Source Address. \sa CiPsPdpAddr_struct */
	CiPsPdpAddrMask srcAddrMask;                            /**< Source Address Mask - Subnet Mask. \sa CiPsPdpAddrMask */
/* # End Contiguous Code Section # */
#endif /*#ifdef CCI_PS_CONTIGUOUS*/
} CiPsTftFilter;

// --------------- SCR #1401348: END   ------------------------------------

/** \brief PDP Context Identifier structure. */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiPsPdpCtxInd_struct
{
	UINT8 cid;                                                              /**< PDP Context Identifier, [0- CI_PS_MAX_PDP_CTX_NUM-1] */
	CiPsPdpType type;                                                       /**< PDP type. \sa CiPsPdpType */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */
#ifndef CCI_PS_CONTIGUOUS
	CiPsPdpAddr           *pAddr; /**< pointer to PDP address string. \sa CiPsPdpAddr_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
/* Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  THIS POINTER IS NOT USED FOR ANYTHING ...
   Remove when the transition to full contiguous is complete*/
	CiPsPdpAddr                  *pAddrTempNonContPtr;
#endif /*#ifdef CCI_APP_NONCONTIGUOUS*/
/* # Start Contiguous Code Section # */
	CiBoolean addrPresent;                                  /**< TRUE - if present. \sa CCI API Ref Manual */
	CiPsPdpAddr addr;                                       /**< PDP address string. \sa CiPsPdpAddr_struct  */
/* # End Contiguous Code Section # */
#endif /*#ifndef CCI_PS_CONTIGUOUS*/
}CiPsIndicatedPdpCtx;


/** \brief 2.5G (R97) Quality of Service (QoS) capabilities. See TS 24.008 v3.11.0, Section 10.5.6.5 for the range of values defined for each parameter. */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiPsQosCap_struct
{
	CiPsPdpType type;                               /**< Pdp type. \sa CiPsPdpType */
	CiNumericRange precedenceCap;                   /**< Precedence class [0-4]. For details see CommonTypes document. \sa CCI API Ref Manual */
	CiNumericRange delayCap;                        /**< Delay class [0-3]. For details see CommonTypes document. \sa CCI API Ref Manual  */
	CiNumericRange reliabilityCap;                  /**< Reliability class [0-5]. For details see CommonTypes document. \sa CCI API Ref Manual  */
	CiNumericRange peakCap;                         /**< Peak throughput [0-9]. For details see CommonTypes document. \sa CCI API Ref Manual  */
	CiNumericList meanCap;                          /**< Mean throughput [0-18, 31]. For details see CommonTypes document. \sa CCI API Ref Manual  */
} CiPsQosCap;

/** \brief List of Qos capabilities per defined PDP context */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiPsQosCaps_struct
{
	UINT8 size;                                                                                                             /**< Number of defined PDP contexts */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */
#ifndef CCI_PS_CONTIGUOUS
	CiPsQosCap            *pCaps;                                 /**< QoS capabilites, optional if rc is not CIRC_PS_SUCCESS. \sa CiPsQosCap_struc */
#else
#ifdef CCI_APP_NONCONTIGUOUS
/* Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  THIS POINTER IS NOT USED FOR ANYTHING ...
   Remove when the transition to full contiguous is complete*/
	CiPsQosCap            *pCapsTempNonContPtr;
#endif /*#ifdef CCI_APP_NONCONTIGUOUS*/
/* # Start Contiguous Code Section # */
	CiPsQosCap caps[CI_CURRENT_SUPPORT_PDP_NUM_TYPE];                       /**< QoS capabilities, optional if rc is not CIRC_PS_SUCCESS. \sa CiPsQosCap_struc */
/* # End Contiguous Code Section # */
#endif /*#ifdef CCI_APP_NONCONTIGUOUS*/

} CiPsQosCaps;

// --------------- SCR #1401348: BEGIN ------------------------------------

/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief  2.5G (R97) Quality of Service (QoS) capabilities. See TS 24.008 v3.11.0, Section 10.5.6.5 for the range of values defined for each parameter. The capabilities structure is defined above -- this is just an alias.
 * \remarks Common Data Section */
typedef CiPsQosCap CiPs25GQosCap;
/**@}*/

/** \brief 3G (R99) Quality of Service (QoS) capabilities. See TS 24.008 v3.11.0, Section 10.5.6.5 for the range of values defined for each parameter.  */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiPs3GQosCap_struct
{
	CiPsPdpType type;                                       /**< PDP type. \sa CiPsPdpType */
	CiNumericRange trafficClass;                        /**< Traffic Class [0..4]. \sa CCI API Ref Manual */
	CiNumericRange deliveryOrder;                       /**< Delivery Order [0..2]. \sa CCI API Ref Manual */
	CiNumericRange deliverErrSdu;                       /**< Delivery of Erroneous SDU's [0..3]. \sa CCI API Ref Manual */
	CiNumericRange resBER;                              /**< Residual BER [0..9]. \sa CCI API Ref Manual */
	CiNumericRange errRatio;                            /**< SDU Error Ratio [0..7]. \sa CCI API Ref Manual */
	CiNumericRange thPriority;                          /**< Traffic Handling Priority [0..3]. \sa CCI API Ref Manual */

	CiNumericRange transDelay;                          /**< Transfer Delay [0x00..0x3e]. \sa CCI API Ref Manual */
	CiNumericRange maxSduSize;                          /**< Maximum SDU Size [0x00..0x99]. \sa CCI API Ref Manual */
	CiNumericRange maxULRate;                           /**< Max Bit Rate, Uplink [0x00..0xff]. \sa CCI API Ref Manual */
	CiNumericRange maxDLRate;                           /**< Max Bit Rate, Downlink [0x00..0xff]. \sa CCI API Ref Manual */
	CiNumericRange guaranteedULRate;                    /**< Guaranteed Bit Rate, Uplink [0x00..0xff]. \sa CCI API Ref Manual */
	CiNumericRange guaranteedDLRate;                    /**< Guaranteed Bit Rate, Downlink [0x00..0xff]. \sa CCI API Ref Manual */
} CiPs3GQosCap;

/** \brief 3G Quality of Service (QoS) Capabilities array  */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiPs3GQosCaps_struct
{
	UINT8 size;                                                                                             /**< Size. */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */
#ifndef CCI_PS_CONTIGUOUS
	CiPs3GQosCap          *pCaps;                              /**< Pointer to 33G QoS capability per defined PDP context. \sa CiPs3GQosCap_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
/* Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  THIS POINTER IS NOT USED FOR ANYTHING ...
   Remove when the transition to full contiguous is complete*/
	CiPs3GQosCap         *pCapsTempNonContPtr;
#endif /*#ifdef CCI_APP_NONCONTIGUOUS*/
/* # Start Contiguous Code Section # */
	CiPs3GQosCap caps[CI_PS_MAX_MO_AND_MT_PDP_CTX_NUM];             /** 3G QoS capability per defined PDP context. \sa CiPs3GQosCap_struct */
	/*--3/5/2009 10:26AM
	 *  Note: need to check max size of array
	 * ------------*/
/* # End Contiguous Code Section # */
#endif /*#ifndef CCI_PS_CONTIGUOUS*/
} CiPs3GQosCaps;

// --------------- SCR #1401348: END   ------------------------------------

/** \brief Data counter report type values  */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIPS_COUNTERREPORTTYPES
{
	CI_PS_COUNTER_REPORT_ONE_SHOT = 0,              /**< One-shot Report (now) */
	CI_PS_COUNTER_REPORT_PERIODIC,                  /**< Periodic Report (set interval) */
	CI_PS_COUNTER_REPORT_STOP,                      /**< Stop Periodic Report */

	CI_PS_NUM_COUNTER_REPORT_TYPES
} _CiPsCounterReportType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief  Data counter report type.
 *  \sa CIPS_COUNTERREPORTTYPES
 * \remarks Common Data Section */
typedef UINT8 CiPsCounterReportType;
/**@}*/

/* --- Primitive Definitions --- */

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_SET_ATTACH_STATE_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimSetAttachStateReq_struct
{
	CiBoolean state;        /**< state of the PS attachment. TRUE - Attach; FALSE - detach. \sa CCI API Ref Manual */
} CiPsPrimSetAttachStateReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_SET_ATTACH_STATE_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimSetAttachStateCnf_struct
{
	CiPsRc rc;      /**< Result code. \sa CiPsRc */
} CiPsPrimSetAttachStateCnf;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_GET_ATTACH_STATE_REQ">   */
typedef CiEmptyPrim CiPsPrimGetAttachStateReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_GET_ATTACH_STATE_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimGetAttachStateCnf_struct
{
	CiPsRc rc;              /**< Result code. \sa CiPsRc */
	CiBoolean state;        /**< State of the PS attachment. TRUE: attached; FALSE: detached. \sa CCI API Ref Manual  */
} CiPsPrimGetAttachStateCnf;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_DEFINE_PDP_CTX_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimDefinePdpCtxReq_struct
{

/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */
#ifndef CCI_PS_CONTIGUOUS
	CiPsPdpCtx                    *pPdpCtx;   /**< PDP context definition. \sa CiPsPdpCtx_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	/*Temporary pointer used to ensure the correct structure size for transmission
	   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	CiPsPdpCtx                    *pPdpCtxTempNonContPtr;
#endif
/* # Start Contiguous Code Section # */
	CiPsPdpCtx pdpCtx;                              /**< PDP context definition. \sa CiPsPdpCtx_struct */
/* # End Contiguous Code Section # */
#endif
} CiPsPrimDefinePdpCtxReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_DEFINE_PDP_CTX_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimDefinePdpCtxCnf_struct
{
	CiPsRc rc;                                      /**< Result code. \sa CiPsRc */
} CiPsPrimDefinePdpCtxCnf;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_DELETE_PDP_CTX_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimDeletePdpCtxReq_struct
{
	UINT8 cid;      /**< PDP context identifier [0- CI_PS_MAX_PDP_CTX_NUM-1] */
} CiPsPrimDeletePdpCtxReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_DELETE_PDP_CTX_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimDeletePdpCtxCnf_struct
{
	CiPsRc rc;      /**< Result code. \sa CiPsRc  */
} CiPsPrimDeletePdpCtxCnf;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_GET_PDP_CTX_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimGetPdpCtxReq_struct
{
	UINT8 cid;      /**< PDP context identifier.   */
} CiPsPrimGetPdpCtxReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_GET_PDP_CTX_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimGetPdpCtxCnf_struct
{

	CiPsRc rc;                                                      /**< Result code. \sa CiPsRc */

/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */
#ifndef CCI_PS_CONTIGUOUS
	CiPsPdpCtxInfo          *pCtx;                  /**< PDP context information, optional if rc is not CIRC_PS_SUCCESS. \sa CiPsPdpCtxInfo_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
/*Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	CiPsPdpCtxInfo          *pCtxTempNonContPtr;
#endif
/* # Start Contiguous Code Section #*/
	CiBoolean ctxPresent;                                   /**< TRUE - if present. \sa CCI API Ref Manual */
	CiPsPdpCtxInfo ctx;                                     /**< PDP context information, optional if rc is not CIRC_PS_SUCCESS. \sa CiPsPdpCtxInfo_struct */
/* # End Contiguous Code Section #*/
#endif
} CiPsPrimGetPdpCtxCnf;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_GET_PDP_CTX_CAPS_REQ">   */
typedef CiEmptyPrim CiPsPrimGetPdpCtxCapsReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_GET_PDP_CTX_CAPS_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimGetPdpCtxCapsCnf_struct
{
	CiPsRc rc;                                                              /**< Result code. \sa CiPsRc */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */
#ifndef CCI_PS_CONTIGUOUS
	CiPsPdpCtxCaps                *pPdpCtxCaps; /**< PDP context capabilities supported by the cellular subsystem. \sa CiPsPdpCtxCaps_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
/* Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  THIS POINTER IS NOT USED FOR ANYTHING ...
   Remove when the transition to full contiguous is complete*/
	CiPsPdpCtxCaps                *pPdpCtxCapsTempNonContPtr;
#endif  /*#ifdef CCI_APP_NONCONTIGUOUS*/
/* # Start Contiguous Code Section # */
	CiPsPdpCtxCaps pdpCtxCaps;                              /**< PDP context capabilities supported by the cellular subsystem. \sa CiPsPdpCtxCaps_struct */
/* # End Contiguous Code Section # */
#endif  /*CCI_PS_CONTIGUOUS*/

} CiPsPrimGetPdpCtxCapsCnf;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_SET_PDP_CTX_ACT_STATE_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimSetPdpCtxActStateReq_struct
{
	CiBoolean state;                /**< State of the PS attachment. TRUE: activate; FALSE: deactivate; \sa CCI API Ref Manual */
	CiBoolean doAll;                /**< Is the operation done on all valid PDP contexts;  \sa CCI API Ref Manual */
	UINT8 cid;                      /**< PDP Context Identifier, optional if doAll is TRUE */
	CiPsL2P l2p;                    /**< L2 protocol type. \sa CiPsL2P */
} CiPsPrimSetPdpCtxActStateReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_SET_PDP_CTX_ACT_STATE_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimSetPdpCtxActStateCnf_struct
{
	CiPsRc rc;                      /**< Result code. \sa CiPsRc */
} CiPsPrimSetPdpCtxActStateCnf;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_GET_PDP_CTXS_ACT_STATE_REQ">   */
typedef CiEmptyPrim CiPsPrimGetPdpCtxsActStateReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_GET_PDP_CTXS_ACT_STATE_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimGetPdpCtxsActStateCnf_struct
{
	CiPsRc rc;                                                                                                              /**< Result code. \sa CiPsRc */
	UINT8 num;                                                                                                              /**< Number of defined PDP contexts [0-CI_PS_MAX_PDP_CTX_NUM-1] */

/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */
#ifndef CCI_PS_CONTIGUOUS
	CiPsPdpCtxActStateListPtr pLst;          /**< Activation state for the defined PDP contexts. \sa CiPsPdpCtxActStateListPtr */
#else
#ifdef CCI_APP_NONCONTIGUOUS
/* Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  THIS POINTER IS NOT USED FOR ANYTHING ...
   Remove when the transition to full contiguous is complete*/
	CiPsPdpCtxActStateListPtr pLstTempNonContPtr;

#endif  /*#ifdef CCI_APP_NONCONTIGUOUS*/
/* # Start Contiguous Code Section # */
	CiPsPdpCtxActState lst[CI_PS_MAX_MO_AND_MT_PDP_CTX_NUM];        /**< Activation state for the defined PDP contexts. \sa CiPsPdpCtxActState_struct */
/* # End Contiguous Code Section # */
#endif  /*CCI_PS_CONTIGUOUS*/
} CiPsPrimGetPdpCtxsActStateCnf;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_ENTER_DATA_STATE_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimEnterDataStateReq_struct
{
	UINT8 cid;                                      /**< PDP context identifier. */
	CiPsL2P l2p;                                    /**< L2 protocol type. \sa CiPsL2P */
	CiBoolean optimizedData;                        /**< Indicates the optimized ACI data plane will be used.  \sa CCI API Ref Manual */
} CiPsPrimEnterDataStateReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_ENTER_DATA_STATE_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimEnterDataStateCnf_struct
{
	CiPsRc rc;      /**< Result code. \sa CiPsRc */
} CiPsPrimEnterDataStateCnf;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_MT_PDP_CTX_ACT_MODIFY_IND">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimMtPdpCtxActModifyInd_struct
{
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */


#ifndef CCI_PS_CONTIGUOUS
	CiPsPdpCtx                    *pPdpCtx; /**< Pdp context information. \sa CiPsPdpCtx_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
/* Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  THIS POINTER IS NOT USED FOR ANYTHING ...
   Remove when the transition to full contiguous is complete*/
	CiPsPdpCtx                    *pPdpCtxTempNonContPtr;
#endif  /*#ifndef CCI_PS_CONTIGUOUS*/
/* # Start Contiguous Code Section # */
	CiPsPdpCtx pdpCtx;                              /**< Pdp context information. \sa CiPsPdpCtx_struct */
/* # Stop Contiguous Code Section # */
#endif  /*#ifndef CCI_PS_CONTIGUOUS*/
	CiPsActionType actionType;                      /**< Action requested on PDP context - activation or modification. \sa CiPsActionType  */
} CiPsPrimMtPdpCtxActModifyInd;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_MT_PDP_CTX_ACT_MODIFY_RSP">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimMtPdpCtxActModifyRsp_struct {
	CiPsRc rc;                      /**< Result code. \sa CiPsRc */
	UINT8 cid;                      /**< PDP context identifier. */
	CiBoolean accept;               /**< TRUE - accept; FALSE - denied.  \sa CCI API Ref Manual */
	CiPsL2P l2p;                    /**< L2 protocol type. \sa CiPsL2P */
} CiPsPrimMtPdpCtxActModifyRsp;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_MT_PDP_CTX_ACTED_IND">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimMtPdpCtxActedInd_struct
{
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */
#ifndef CCI_PS_CONTIGUOUS
	CiPsPdpCtx            *pPdpCtx; /**< PDP context information. \sa CiPsPdpCtx_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
/* Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  THIS POINTER IS NOT USED FOR ANYTHING ...
   Remove when the transition to full contiguous is complete*/
	CiPsPdpCtx            *pPdpCtxTempNonContPtr;
#endif  /*#ifndef CCI_PS_CONTIGUOUS*/
/* # Start Contiguous Code Section # */
	CiPsPdpCtx pdpCtx;              /**< PDP context information. \sa CiPsPdpCtx_struct */
/* # Stop Contiguous Code Section # */
#endif  /*#ifndef CCI_PS_CONTIGUOUS*/
} CiPsPrimMtPdpCtxActedInd;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_SET_GSMGPRS_CLASS_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimSetGsmGprsClassReq_struct
{
	CiPsGsmGprsClass classType;             /**< Mobile class for GSM/GPRS. \sa CiPsGsmGprsClass */
} CiPsPrimSetGsmGprsClassReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_SET_GSMGPRS_CLASS_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimSetGsmGprsClassCnf_struct
{
	CiPsRc rc;      /**< Result code. \sa CiPsRc */
} CiPsPrimSetGsmGprsClassCnf;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_GET_GSMGPRS_CLASS_REQ">   */
typedef CiEmptyPrim CiPsPrimGetGsmGprsClassReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_GET_GSMGPRS_CLASS_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimGetGsmGprsClassCnf_struct
{
	CiPsRc rc;                                              /**< Result code. \sa CiPsRc */
	CiPsGsmGprsClass classType;                             /**< Mobile class for GSM/GPRS.\sa CiPsGsmGprsClass */
} CiPsPrimGetGsmGprsClassCnf;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_GET_GSMGPRS_CLASSES_REQ">   */
typedef CiEmptyPrim CiPsPrimGetGsmGprsClassesReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_GET_GSMGPRS_CLASSES_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimGetGsmGprsClassesCnf_struct
{
	CiPsRc rc;                                      /**< Result code. \sa CiPsRc  */
	CiBitRange classes;                             /**< Mobile class for GSM/GPRS. For details see CommonTypes document.  \sa CCI API Ref Manual */
} CiPsPrimGetGsmGprsClassesCnf;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_ENABLE_NW_REG_IND_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimEnableNwRegIndReq_struct
{
	CiPsNwRegIndFlag flag;          /**< Configures nework registration status reports.\sa CiPsNwRegIndFlag */
} CiPsPrimEnableNwRegIndReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_ENABLE_NW_REG_IND_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimEnableNwRegIndCnf_struct
{
	CiPsRc rc;                                      /**< Result code. \sa CiPsRc */
} CiPsPrimEnableNwRegIndCnf;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_NW_REG_IND">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimNwRegInd_struct
{
	CiPsNwRegInfo nwRegInfo;                        /**< Network registration information \sa CiPsNwRegInfo_struct */
} CiPsPrimNwRegInd;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_SET_QOS_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimSetQosReq_struct
{
	CiBoolean isMin;                        /**< Indicates if the profile requested is minimum or required QoS profile.  \sa CCI API Ref Manual */
	UINT8 cid;                              /**< PDP context identifier [0- CI_PS_MAX_PDP_CTX_NUM-1] */

/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */
#ifndef CCI_PS_CONTIGUOUS
	CiPsQosProfile                *pQosProf; /**< QoS profile data. \sa CiPsQosProfile_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
/* Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  THIS POINTER IS NOT USED FOR ANYTHING ...
   Remove when the transition to full contiguous is complete*/
	CiPsQosProfile                *pQosProfTempNonContPtr;
#endif /*#ifdef CCI_APP_NONCONTIGUOUS*/
/* # Start Contiguous Code Section # */
	CiPsQosProfile qosProf;                         /**< QoS profile data. \sa CiPsQosProfile_struct */
/* # End Contiguous Code Section # */
#endif /*#ifndef CCI_PS_CONTIGUOUS*/
} CiPsPrimSetQosReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_SET_QOS_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimSetQosCnf_struct
{
	CiPsRc rc;                                              /**< Result code. \sa CiPsRc */
} CiPsPrimSetQosCnf;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_DEL_QOS_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimDelQosReq_struct
{
	CiBoolean isMin;                                /**< Indicates if the profile requested is minimum or required QoS profile.  \sa CCI API Ref Manual */
	UINT8 cid;                                      /**< PDP context identifier [0- CI_PS_MAX_PDP_CTX_NUM-1] */
} CiPsPrimDelQosReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_DEL_QOS_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimDelQosCnf_struct
{
	CiPsRc rc;                                      /**< Result code. \sa CiPsRc */
} CiPsPrimDelQosCnf;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_GET_QOS_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimGetQosReq_struct
{
	CiBoolean isMin;                                /**< Indicates if the profile requested is minimum or required QoS profile.  \sa CCI API Ref Manual */
	UINT8 cid;                                      /**< PDP context identifier [0- CI_PS_MAX_PDP_CTX_NUM-1] */
} CiPsPrimGetQosReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_GET_QOS_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimGetQosCnf_struct
{
	CiPsRc rc;                                                      /**< Result code.\sa CiPsRc */

	/* !!!!!!!!!!!!!!!!!!!
	 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
	 * & CCI_APP_NONCONTIGUOUS flags must be removed.
	 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
	 * # Start Contiguous Code Section # and # End Contiguous Code Section #
	 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
	 */
#ifndef CCI_PS_CONTIGUOUS
	CiPsQosProfile                *pQosProf;        /**< QoS profile, optional if rc is not CIRC_PS_SUCCESS. \sa CiPsQosProfile_struct*/
#else
#ifdef CCI_APP_NONCONTIGUOUS
/* Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  THIS POINTER IS NOT USED FOR ANYTHING ...
   Remove when the transition to full contiguous is complete*/
	CiPsQosProfile                *pQosProfTempNonContPtr;
#endif /*#ifdef CCI_APP_NONCONTIGUOUS*/
/* # Start Contiguous Code Section # */
	CiBoolean qosProfPresent;                                       /**< Not in use. \sa CCI API Ref Manual */
	CiPsQosProfile qosProf;                                         /**< QoS profile, optional if rc is not CIRC_PS_SUCCESS. \sa CiPsQosProfile_struct */
/* # End Contiguous Code Section # */
#endif /*#ifndef CCI_PS_CONTIGUOUS*/
} CiPsPrimGetQosCnf;

//ICAT EXPORTED STRUCT
typedef struct CiPsPrimEnablePoweronAutoAttachReq_struct
{
	CiBoolean autoAttach;
} CiPsPrimEnablePoweronAutoAttachReq;

//ICAT EXPORTED STRUCT
typedef struct CiPsPrimEnablePoweronAutoAttachCnf_struct
{
	CiPsRc rc;
} CiPsPrimEnablePoweronAutoAttachCnf;

//ICAT EXPORTED STRUCT
typedef struct CiPsPrimMtPdpCtxRejectedInd_struct
{
	UINT8 cause;

/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */
#ifndef CCI_PS_CONTIGUOUS
	CiPsIndicatedPdpCtx   *pIndedPdpCtx;
#else
#ifdef CCI_APP_NONCONTIGUOUS
/* Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  THIS POINTER IS NOT USED FOR ANYTHING ...
   Remove when the transition to full contiguous is complete*/
	CiPsIndicatedPdpCtx   *pIndedPdpCtxTempNonContPtr;
#endif /*#ifdef CCI_APP_NONCONTIGUOUS*/
/* # Start Contiguous Code Section # */
	CiPsIndicatedPdpCtx indedPdpCtx;
/* # End Contiguous Code Section # */
#endif /*#ifdef CCI_PS_CONTIGUOUS*/

} CiPsPrimMtPdpCtxRejectedInd;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_PDP_CTX_DEACTED_IND">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimPdpCtxDeactedInd_struct
{
	UINT8 cause;                                                                            /**< Cause for PDP context deactivation */
	CiBoolean isMEInitiated;                                                                /**< TRUE if ME requested PDP context deactivation or PPP connection failure detected in comm . \sa CCI API Ref Manual */
	UINT8 res1U8[2];                                                                        /**< (padding) */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */
#ifndef CCI_PS_CONTIGUOUS
	CiPsIndicatedPdpCtx                   *pIndedPdpCtx;            /**< Indicated PDP context. \sa CiPsPdpCtxInd_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
/* Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  THIS POINTER IS NOT USED FOR ANYTHING ...
   Remove when the transition to full contiguous is complete*/
	CiPsIndicatedPdpCtx                   *pIndedPdpCtxTempNonContPtr;
#endif /*#ifdef CCI_APP_NONCONTIGUOUS*/
/* # Start Contiguous Code Section # */
	CiPsIndicatedPdpCtx indedPdpCtx;                                        /**< Indicated PDP context. \sa CiPsPdpCtxInd_struct */
/* # End Contiguous Code Section # */
#endif /*#ifndef CCI_PS_CONTIGUOUS*/
} CiPsPrimPdpCtxDeactedInd;

//ICAT EXPORTED STRUCT
typedef struct CiPsPrimPdpCtxReactedInd_struct
{
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */
#ifndef CCI_PS_CONTIGUOUS
	CiPsIndicatedPdpCtx   *pIndedPdpCtx;
#else
#ifdef CCI_APP_NONCONTIGUOUS
/* Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  THIS POINTER IS NOT USED FOR ANYTHING ...
   Remove when the transition to full contiguous is complete*/
	CiPsIndicatedPdpCtx   *pIndedPdpCtxTempNonContPtr;
#endif /*#ifdef CCI_APP_NONCONTIGUOUS*/
/* # Start Contiguous Code Section # */
	CiPsIndicatedPdpCtx indedPdpCtx;
/* # End Contiguous Code Section # */
#endif /*#ifndef CCI_PS_CONTIGUOUS*/
} CiPsPrimPdpCtxReactedInd;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_DETACHED_IND">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimDetachedInd_struct
{
	CiBoolean isMeDetach;                   /**< Indicates if detach is ME initiated or network initiated. TRUE indicates ME detach; FALSE indicates NW detach. \sa CCI API Ref Manual */
} CiPsPrimDetachedInd;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_GPRS_CLASS_CHANGED_IND">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimGprsClassChangedInd_struct
{
	CiPsGsmGprsClass classType;     /**< Mobile class for GSM/GPRS. \sa CiPsGsmGprsClass */
	CiBoolean IsMEClassChanged;     /**< TRUE if network mode changed. \sa CCI API Ref Manual */
} CiPsPrimGprsClassChangedInd;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_GET_DEFINED_CID_LIST_REQ">   */
typedef CiEmptyPrim CiPsPrimGetDefinedCidListReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_GET_DEFINED_CID_LIST_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimGetDefinedCidListCnf_struct
{
	CiPsRc rc;                                      /**< Result code. \sa CiPsRc */
	UINT8 size;                                     /**< Size of the cid list [0- CI_PS_MAX_PDP_CTX_NUM] */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */
#ifndef CCI_PS_CONTIGUOUS
	UINT8                 *pCidLst;                                 /**< cid list */
#else
#ifdef CCI_APP_NONCONTIGUOUS
/* Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  THIS POINTER IS NOT USED FOR ANYTHING ...
   Remove when the transition to full contiguous is complete*/
	UINT8                 *pCidLstTempNonContPtr;
#endif /*#ifdef CCI_APP_NONCONTIGUOUS*/
/* # Start Contiguous Code Section # */
	UINT8 cidLst[CI_PS_MAX_MO_AND_MT_PDP_CTX_NUM];                  /**< cid list */
/* # End Contiguous Code Section # */
#endif /*#ifndef CCI_PS_CONTIGUOUS*/
} CiPsPrimGetDefinedCidListCnf;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_GET_NW_REG_STATUS_REQ">   */
typedef CiEmptyPrim CiPsPrimGetNwRegStatusReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_GET_NW_REG_STATUS_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimGetNwRegStatusCnf_struct
{
	CiPsRc rc;                                      /**< Result code. \sa CiPsRc */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */
#ifndef CCI_PS_CONTIGUOUS
	CiPsNwRegInfo         *pNwRegInfo; /**< Network registration information. \sa CiPsNwRegInfo_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
/* Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  THIS POINTER IS NOT USED FOR ANYTHING ...
   Remove when the transition to full contiguous is complete*/
	CiPsNwRegInfo         *pNwRegInfoTempNonContPtr;
#endif /*#ifdef CCI_APP_NONCONTIGUOUS*/
/* # Start Contiguous Code Section # */
	CiPsNwRegInfo nwRegInfo;                /**< Network registration information. \sa CiPsNwRegInfo_struct */
/* # End Contiguous Code Section # */
#endif /*#ifndef CCI_PS_CONTIGUOUS*/
} CiPsPrimGetNwRegStatusCnf;

/* <NOTINUSE> */
/** <paramref name="CI_PS_PRIM_GET_QOS_CAPS_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimGetQosCapsReq_struct
{
	CiBoolean isMin;                /**< Not in use.  */
} CiPsPrimGetQosCapsReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_GET_QOS_CAPS_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimGetQosCapsCnf_struct
{
	CiPsRc rc;                                      /**< Result code. \sa CiPsRc */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */
#ifndef CCI_PS_CONTIGUOUS
	CiPsQosCaps   *pQosCaps; /**< QoS capabilities, optional if rc is not CIRC_PS_SUCCESS. \sa CiPsQosCaps_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
/* Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  THIS POINTER IS NOT USED FOR ANYTHING ...
   Remove when the transition to full contiguous is complete*/
	CiPsQosCaps   *pQosCapsTempNonContPtr;
#endif /*#ifdef CCI_APP_NONCONTIGUOUS*/
	CiBoolean qosCapsPresent;       /**< TRUE - if present; */
	CiPsQosCaps qosCaps;            /**< QoS capabilities, optional if rc is not CIRC_PS_SUCCESS. \sa CiPsQosCaps_struct */
#endif /*#ifndef CCI_PS_CONTIGUOUS*/

} CiPsPrimGetQosCapsCnf;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_ENABLE_EVENTS_REPORTING_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimEnableEventsReportingReq_struct
{
	CiBoolean enable;       /**< TRUE: enable events reporting; FALSE: disable events reporting; default: FALSE;  \sa CCI API Ref Manual */
} CiPsPrimEnableEventsReportingReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_ENABLE_EVENTS_REPORTING_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimEnableEventsReportingCnf_struct
{
	CiPsRc rc;      /**< Result code. \sa CiPsRc */
} CiPsPrimEnableEventsReportingCnf;

// --------------- SCR #1401348: BEGIN ------------------------------------

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_GET_3G_QOS_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimGet3GQosReq_struct
{
	CiPs3GQosType qosType; /**< Specifies 3G minimum, required or negotiated QoS profile. \sa CiPs3GQosType */                      // REQ/MIN/NEG
	UINT8 cid;                                                                                                                      /**< PDP context identifier [0- CI_PS_MAX_PDP_CTX_NUM-1] */
} CiPsPrimGet3GQosReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_GET_3G_QOS_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimGet3GQosCnf_struct
{
	CiPsRc rc;                                                                      /**< Result code. \sa CiPsRc */

/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */
#ifndef CCI_PS_CONTIGUOUS
	CiPs3GQosProfile      *pQosProf;           /**< 3G QoS profile, optional if rc is not CIRC_PS_SUCCESS; \sa CiPs3GQosProfile_struct  */
#else
#ifdef CCI_APP_NONCONTIGUOUS
/* Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  THIS POINTER IS NOT USED FOR ANYTHING ...
   Remove when the transition to full contiguous is complete*/
	CiPs3GQosProfile      *pQosProfTempNonContPtr;
#endif /*#ifdef CCI_APP_NONCONTIGUOUS*/
/* # Start Contiguous Code Section # */
	CiBoolean qosProfPresent;                                       /**< If TRUE, qosProf will have the 3G QoS profile; If FALSE, qosProf doesn't contain useful information;  \sa CCI API Ref Manual */
	CiPs3GQosProfile qosProf;                                       /**< 3G QoS profile, optional if rc is not CIRC_PS_SUCCESS; \sa CiPs3GQosProfile_struct  */
/* # End Contiguous Code Section # */
#endif /*#ifdef CCI_APP_NONCONTIGUOUS*/
} CiPsPrimGet3GQosCnf;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_SET_3G_QOS_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimSet3GQosReq_struct
{
	CiPs3GQosType qosType; /**< Specifies 3G minimum or required QoS profile. \sa CiPs3GQosType */                                                          // REQ/MIN (NEG is not valid here)
	UINT8 cid;                                                                                                                                              /**< PDP context identifier [0- CI_PS_MAX_PDP_CTX_NUM-1] */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */
#ifndef CCI_PS_CONTIGUOUS
	CiPs3GQosProfile      *pQosProf;                      /**< 3G QoS profile. For details see \sa CiPs3GQosProfile_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
/* Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  THIS POINTER IS NOT USED FOR ANYTHING ...
   Remove when the transition to full contiguous is complete*/
	CiPs3GQosProfile      *pQosProfTempNonContPtr;
#endif /*#ifdef CCI_APP_NONCONTIGUOUS*/
/* # Start Contiguous Code Section # */
	CiPs3GQosProfile qosProf;                                                       /**< 3G QoS profile. \sa CiPs3GQosProfile_struct */
/* # End Contiguous Code Section # */
#endif /*#ifdef CCI_APP_NONCONTIGUOUS*/
} CiPsPrimSet3GQosReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_SET_3G_QOS_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimSet3GQosCnf_struct
{
	CiPsRc rc;              /**< Result code. \sa CiPsRc */
} CiPsPrimSet3GQosCnf;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_DEL_3G_QOS_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimDel3GQosReq_struct
{
	CiPs3GQosType qosType; /**< Specifies 3G minimum or required QoS profile. \sa CiPs3GQosType */                          // REQ/MIN (NEG is not valid here)
	UINT8 cid;                                                                                                              /**< PDP context identifier [0- CI_PS_MAX_PDP_CTX_NUM-1] */
} CiPsPrimDel3GQosReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_DEL_3G_QOS_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimDel3GQosCnf_struct
{
	CiPsRc rc;              /**< Result code. \sa CiPsRc */
} CiPsPrimDel3GQosCnf;


/* <NOTINUSE> */
/** <paramref name="CI_PS_PRIM_GET_3G_QOS_CAPS_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimGet3GQosCapsReq_struct
{
	CiPs3GQosType qosType; /**< Not in use */ // REQ/MIN/NEG
} CiPsPrimGet3GQosCapsReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_GET_3G_QOS_CAPS_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimGet3GQosCapsCnf_struct
{
	CiPsRc rc;                                                              /**< Result code. \sa CiPsRc */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */
#ifndef CCI_PS_CONTIGUOUS
	CiPs3GQosCaps         *pQosCaps; /**< 3G QoS capabilities, optional if rc is not CIRC_PS_SUCCESS. \sa CiPs3GQosCaps_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
/* Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  THIS POINTER IS NOT USED FOR ANYTHING ...
   Remove when the transition to full contiguous is complete*/
	CiPs3GQosCaps         *pQosCapsTempNonContPtr; // 3G QosCaps array
#endif /*#ifdef CCI_APP_NONCONTIGUOUS*/
/* # Start Contiguous Code Section # */
	CiBoolean qosCapsPresent;                               /**< If TRUE, qosCaps will have the 3G QoS capabilities; If FALSE, qosCaps doesn't contain useful information; \sa CCI API Ref Manual*/
	CiPs3GQosCaps qosCaps;                                  /**< 3G QoS capabilities, optional if rc is not CIRC_PS_SUCCESS. \sa CiPs3GQosCaps_struct */
/* # End Contiguous Code Section # */
#endif /*#ifndef CCI_PS_CONTIGUOUS*/
} CiPsPrimGet3GQosCapsCnf;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_DEFINE_SEC_PDP_CTX_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimDefineSecPdpCtxReq_struct
{
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */
#ifndef CCI_PS_CONTIGUOUS
	CiPsSecPdpCtx                 *pSecPdpCtx; /**< Secondary PDP context. \sa CiPsSecPdpCtx_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
/* Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  THIS POINTER IS NOT USED FOR ANYTHING ...
   Remove when the transition to full contiguous is complete*/
	CiPsSecPdpCtx          *pSecPdpCtxTempNonContPtr;
#endif /*#ifdef CCI_APP_NONCONTIGUOUS*/
/* # Start Contiguous Code Section # */
	CiPsSecPdpCtx secPdpCtx;                        /**< Secondary PDP context. \sa CiPsSecPdpCtx_struct */
/* # End Contiguous Code Section # */
#endif /*#ifndef CCI_PS_CONTIGUOUS*/
} CiPsPrimDefineSecPdpCtxReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_DEFINE_SEC_PDP_CTX_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimDefineSecPdpCtxCnf_struct
{
	CiPsRc rc;      /**< Result code. \sa CiPsRc */
} CiPsPrimDefineSecPdpCtxCnf;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_DELETE_SEC_PDP_CTX_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimDeleteSecPdpCtxReq_struct
{
	UINT8 cid;              /**< PDP context identifier [0- CI_PS_MAX_PDP_CTX_NUM-1] */
} CiPsPrimDeleteSecPdpCtxReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_DELETE_SEC_PDP_CTX_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimDeleteSecPdpCtxCnf_struct
{
	CiPsRc rc;              /**< Result code. \sa CiPsRc */
} CiPsPrimDeleteSecPdpCtxCnf;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_GET_SEC_PDP_CTX_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimGetSecPdpCtxReq_struct
{
	UINT8 cid;              /**< Secondary PDP context ID [[0- CI_PS_MAX_PDP_CTX_NUM-1]] */
} CiPsPrimGetSecPdpCtxReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_GET_SEC_PDP_CTX_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimGetSecPdpCtxCnf_struct
{
	CiPsRc rc;                                                                      /**< Result code. \sa CiPsRc */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */
#ifndef CCI_PS_CONTIGUOUS
	CiPsSecPdpCtxInfo     *pCtx;                  /**< Secondary PDP Context Information. \sa CiPsSecPdpCtxInfo_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
/* Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  THIS POINTER IS NOT USED FOR ANYTHING ...
   Remove when the transition to full contiguous is complete*/
	CiPsSecPdpCtxInfo     *pCtxTempNonContPtr;
#endif /*#ifdef CCI_APP_NONCONTIGUOUS*/
/* # Start Contiguous Code Section # */
	CiBoolean ctxPresent;                                                   /**< Set to FALSE, if the Result Code parameter indicates an error.  \sa CCI API Ref Manual */
	CiPsSecPdpCtxInfo ctx;                                                  /**< Secondary PDP Context Information. \sa CiPsSecPdpCtxInfo_struct */
/* # End Contiguous Code Section # */
#endif /*#ifdef CCI_PS_CONTIGUOUS*/
} CiPsPrimGetSecPdpCtxCnf;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_DEFINE_TFT_FILTER_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimDefineTftFilterReq_struct
{
	UINT8 cid;                                              /**< Secondary PDP context ID [[0- CI_PS_MAX_PDP_CTX_NUM-1]] */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */
#ifndef CCI_PS_CONTIGUOUS
	CiPsTftFilter  *pFilter;           /**< TFT Filter parameters. \sa CiPsTftFilter_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
/* Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  THIS POINTER IS NOT USED FOR ANYTHING ...
   Remove when the transition to full contiguous is complete*/
	CiPsTftFilter  *pFilterTempNonContPtr;
#endif /*#ifdef CCI_APP_NONCONTIGUOUS*/
/* # Start Contiguous Code Section # */
	CiPsTftFilter filter;                                   /**< TFT Filter parameters. \sa CiPsTftFilter_struct */
/* # End Contiguous Code Section # */
#endif /*#ifdef CCI_PS_CONTIGUOUS*/
} CiPsPrimDefineTftFilterReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_DEFINE_TFT_FILTER_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimDefineTftFilterCnf_struct
{
	CiPsRc rc;              /**< Result code. \sa CiPsRc */
} CiPsPrimDefineTftFilterCnf;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_DELETE_TFT_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimDeleteTftReq_struct
{
	UINT8 cid;                      /**< Context ID [[0- CI_PS_MAX_PDP_CTX_NUM-1]] */
} CiPsPrimDeleteTftReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_DELETE_TFT_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimDeleteTftCnf_struct
{
	CiPsRc rc;                      /**< Result code. \sa CiPsRc */
} CiPsPrimDeleteTftCnf;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_GET_TFT_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimGetTftReq_struct
{
	UINT8 cid;                      /**< Context ID [[0- CI_PS_MAX_PDP_CTX_NUM-1]] */
} CiPsPrimGetTftReq;

/* <NOTINUSE> */
/** <paramref name="CI_PS_PRIM_GET_TFT_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimGetTftCnf_struct
{
	CiPsRc rc;                                                                              /**< Result code. \sa CiPsRc */
	UINT8 numFilters;                                                                       /**< Number of Packet Filters */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */
#ifndef CCI_PS_CONTIGUOUS
	CiPsTftFilter         *pFilters;  /**< Not in use */
#else
#ifdef CCI_APP_NONCONTIGUOUS
/* Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  THIS POINTER IS NOT USED FOR ANYTHING ...
   Remove when the transition to full contiguous is complete*/
	CiPsTftFilter         *pFiltersTempNonContPtr;
#endif  /*#ifdef CCI_APP_NONCONTIGUOUS*/
/* # Start Contiguous Code Section #  */
	CiPsTftFilter filters[CI_PS_MAX_TFT_FILTERS];           /**< Not in use */
/* # End Contiguous Code Section #  */
#endif  /*#ifdef CCI_PS_CONTIGUOUS*/
} CiPsPrimGetTftCnf;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_MODIFY_PDP_CTX_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimModifyPdpCtxReq_struct
{
	CiBoolean doAll;                /**< Modify all active contexts?  \sa CCI API Ref Manual */
	UINT8 cid;                      /**<  If not, modify this one! */
} CiPsPrimModifyPdpCtxReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_MODIFY_PDP_CTX_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimModifyPdpCtxCnf_struct
{
	CiPsRc rc;              /**< Result code. \sa CiPsRc */
} CiPsPrimModifyPdpCtxCnf;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_GET_ACTIVE_CID_LIST_REQ">   */
// Get List of Active Context ID's -- like the "AT+CGCMOD=?" command
typedef CiEmptyPrim CiPsPrimGetActiveCidListReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_GET_ACTIVE_CID_LIST_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimGetActiveCidListCnf_struct
{
	CiPsRc rc;                              /**< Result code. \sa CiPsRc */
	UINT8 size;                             /**< Size of the cid list [0..CI_PS_MAX_PDP_CTX_NUM] */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */
#ifndef CCI_PS_CONTIGUOUS
	UINT8                 *pCidLst;               /**< cid list */
#else
#ifdef CCI_APP_NONCONTIGUOUS
/* Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  THIS POINTER IS NOT USED FOR ANYTHING ...
   Remove when the transition to full contiguous is complete*/
	UINT8                 *pCidLstTempNonContPtr;
#endif /*#ifdef CCI_APP_NONCONTIGUOUS*/
/* # Start Contiguous Code Section #  */
	UINT8 cidLst[CI_PS_MAX_MO_AND_MT_PDP_CTX_NUM];                                  /**< cid list */
/* # End Contiguous Code Section #  */
#endif
} CiPsPrimGetActiveCidListCnf;

// --------------- SCR #1401348: END   ------------------------------------

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_REPORT_COUNTER_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimReportCounterReq_struct
{
	CiPsCounterReportType type;                             /**< Report Type. \sa CiPsCounterReportType */
	UINT16 interval;                                        /**< Report Interval (seconds) for PERIODIC */
} CiPsPrimReportCounterReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_REPORT_COUNTER_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimReportCounterCnf_struct
{
	CiPsRc rc;                      /**< Result code. \sa CiPsRc */
} CiPsPrimReportCounterCnf;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_RESET_COUNTER_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimResetCounterReq_struct
{
	CiBoolean doAll;                        /**< Reset counters for all active contexts? SCR 1980451 -> 1818954 #9.  \sa CCI API Ref Manual*/
	UINT8 cid;                              /**< If not, just for this one! */
} CiPsPrimResetCounterReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_RESET_COUNTER_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimResetCounterCnf_struct
{
	CiPsRc rc;              /**< Result code. \sa CiPsRc */
} CiPsPrimResetCounterCnf;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_COUNTER_IND">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimCounterInd_struct
{
	UINT8 cid;                              /**< PDP Context ID, [0- CI_PS_MAX_PDP_CTX_NUM-1] */
	UINT32 totalULBytes;                    /**< Total bytes sent on Uplink (uncompressed) */
	UINT32 totalDLBytes;                    /**< Total bytes received on Downlink (uncompressed) */
} CiPsPrimCounterInd;

//ICAT EXPORTED STRUCT
typedef struct CiPsPrimSendDataReq_struct
{
	UINT16 pcktsize;        /* packet size range: 0 - 10000, default 1472 */
	UINT16 pcktcount;       /* number of packets to send: 1 - 20, default 1 */
	UINT8 nsapi;            /* PDP Context ID */
	UINT8 PAD1;
	UINT8 PAD2;
	UINT8 PAD3;
} CiPsPrimSendDataReq;

//ICAT EXPORTED STRUCT
typedef struct CiPsPrimSendDataCnf_struct
{
	CiPsRc rc;
} CiPsPrimSendDataCnf;

/* Michal Bukai & Boris Tsatkin ?AT&T Smart Card support - Start*/
/*****************************************************************/
/*
 *** AT&T- Smart Card  CI_PS_PRIM_ ACL SERVICE: LIST , SET , EDIT   -BT6
 */

//ICAT EXPORTED ENUM
typedef enum CiAbgpRequestStatusTag
{
	CI_ABGP_APN_OK,
	CI_ABGP_APN_NRAM_ERROR,
	CI_ABGP_APN_RECORD_NOT_FOUND,
#if defined (UPGRADE_3G)
	CI_ABGP_APN_INVALID_PARAMS,             /*invalid parameters specified in the request*/
	CI_ABGP_APN_SIM_ERROR,                  /*SIM error*/
	CI_ABGP_APN_FILE_NOT_FOUND,             /*File doesn' t exist*/
	CI_ABGP_APN_POWERING_DOWN,              /*powering down*/
	CI_ABGP_APN_ACCESS_DENIED,              /*PIN/PIN2 hasn' t been verified*/
	CI_ABGP_APN_MEMORY_PROBLEM,             /*ACL file too small to store another APN <CDR-SMCD-1124>*/
	CI_ABGP_APN_NOT_ALLOWED,                /*operation not allowed*/
	CI_ABGP_APN_SERVICE_NOT_AVAILCI_ABLE,   /*service not availCI_ABle*/
#endif
	CI_ABGP_ALLIGN = 0xFFFF

} _CiPsSimResult;
typedef UINT16 CiPsSimResult;


/*AT&T- Smart Card  CI_PS_PRIM_SET_ACL_SERVICE_REQ   <CDR-SMCD-1100><1101> -BT6 */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimSetAclReq_struct {
	CiBoolean enable; /*Enanle -1 , disable- 0*/
} CiPsPrimSetAclReq;

/*AT&T- Smart Card  CI_PS_PRIM_SET_ACL_SERVICE_CNF   -BT6 */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimSetAclCnf_struct {
	CiPsRc rc;
	CiPsSimResult SimCause;
	CiBoolean enable;       /*Enanled -1 , disabled- 0*/
} CiPsPrimSetAclCnf;

/*AT&T- Smart Card  CI_PS_PRIM_GET_ACL_SIZE_REQ <CDR-SMCD-1110><1120>  -BT6 */
//ICAT EXPORTED STRUCT
typedef CiEmptyPrim CiPsPrimGetAclSizeReq;

/*AT&T- Smart Card  CI_PS_PRIM_GET_ACL_SIZE_CNF   -BT6 */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimGetAclSizeCnf_struct {
	CiPsRc rc;
	CiPsSimResult SimCause;
	UINT8 totalNumApns;  /*total number of APNs currently held on the SIM in EF ACL (ABGP- apnNumRecords)*/
} CiPsPrimGetAclSizeCnf;


/*AT&T- Smart Card  CI_PS_PRIM_READ_ACL_ENTRY_REQ  <CDR-SMCD-1110><1120>  -BT6 */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimReadAclEntryReq_struct {
	UINT8 Index;        /*APN list entry index to read*/
} CiPsPrimReadAclEntryReq;

/*AT&T- Smart Card  CI_PS_PRIM_READ_ACL_ENTRY_CNF   -BT6 */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimReadAclEntryReqCnf_struct {
	CiPsRc rc;
	CiPsSimResult SimCause;
	CiLongAdrInfo apn;      /*Requested APN (textual string) */
} CiPsPrimReadAclEntryCnf;

/*AT&T- Smart Card  CI_PS_PRIM_EDIT_ACL_ENTRY_REQ  <CDR-SMCD-1120><1123> -BT6 */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimEditAclEntryReq_struct {
	CiEditCmdType EditCommand;
	UINT8 position;         /*APN entry position in the ACL */
	/*To add the "Network provided APN" to the APN Control List, the length of the APN should be set to 0*/
	CiLongAdrInfo apn;      /*TextualAccessPointName*/
} CiPsPrimEditAclEntryReq;

/*AT&T- Smart Card  CI_PS_PRIM_EDIT_ACL_ENTRY_CNF   -BT6 */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimEditAclEntryCnf_struct {
	CiPsRc rc;
	CiPsSimResult SimCause;
	UINT8 position;
} CiPsPrimEditAclEntryCnf;

/* Michal Bukai & Boris Tsatkin ?AT&T Smart Card support - End*/
/*****************************************************************/
/* Michal Bukai ?PDP authentication - Start*/
/*****************************************************************/

//ICAT EXPORTED ENUM
typedef enum CIPSAUTHENTICATIONTYPE_TAG
{
	CI_PS_AUTHENTICATION_TYPE_NONE,	/**< No authentication protocol */
	CI_PS_AUTHENTICATION_TYPE_PAP,	/**< Password authentication protocol */
	CI_PS_AUTHENTICATION_TYPE_CHAP,	/**< Challenge-Handshake authentication protocol */
	CI_PS_AUTHENTICATION_TYPE_NUM

} _CiPsAuthenticationType;

typedef UINT8 CiPsAuthenticationType;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_AUTHENTICATE_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimAuthenticateReq_struct
{
	UINT8  cid;            	/**< PDP context identifier */
	CiPsAuthenticationType	AuthenticationType; /**< Authentication type. \sa CiPsAuthenticationType */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
*/
#ifndef CCI_PS_CONTIGUOUS
	CiString   				*pUserName; 		/**< UserName length is limited to 50 octets. \sa CCI API Ref Manual */
	CiString	 				*pPassword;     	/**< Password length is limited to 50 octets. \sa CCI API Ref Manual */
#else
#ifdef CCI_APP_NONCONTIGUOUS
/*Temporary pointer used to ensure the correct structure size for transmission
   to a non contiguous app.  Remove when the transition to full contiguous is complete*/
	CiString   				*pUserNameTempNonContPtr; 		/**< UserName length is limited to 50 octets. \sa CCI API Ref Manual */
	CiString	 				*pPasswordTempNonContPtr;     	/**< Password length is limited to 50 octets. \sa CCI API Ref Manual */
#endif /*#ifdef CCI_APP_NONCONTIGUOUS*/
/* # Start Contiguous Code Section # */
	CiString   				UserName; 		/**< UserName length is limited to 50 octets. \sa CCI API Ref Manual */
	CiString	 				Password;     	/**< Password length is limited to 50 octets. \sa CCI API Ref Manual */
/* # End Contiguous Code Section # */
#endif/*#ifndef CCI_PS_CONTIGUOUS*/

}CiPsPrimAuthenticateReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_AUTHENTICATE_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimAuthenticateCnf_struct
{
	CiPsRc rc;			/**< Result code. \sa CiPsRc */
} CiPsPrimAuthenticateCnf;

/* Michal Bukai ?PDP authentication - End*/
/*****************************************************************/

/* Michal Bukai ?Fast Dormancy - Start*/
/*****************************************************************/

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_FAST_DORMANT_REQ">   */
typedef CiEmptyPrim CiPsPrimFastDormantReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_FAST_DORMANT_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimFastDormantCnf_struct
{
	CiPsRc rc;			/**< Result code. \sa CiPsRc */
} CiPsPrimFastDormantCnf;

/* Michal Bukai ?Fast Dormancy - End*/

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_GET_CURRENT_JOB_REQ">   */
typedef CiEmptyPrim CiPsPrimGetCurrentJobReq;

/* <INUSE> */
/** <paramref name="CI_PS_PRIM_GET_CURRENT_JOB_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiPsPrimGetCurrentJobCnf_struct
{
  CiPsRc rc;			/**< Result code. \sa CiPsRc */
  CiPrimitiveID primId; /**< Primitive ID. \sa CiPrimitiveID */
} CiPsPrimGetCurrentJobCnf;
#ifdef AT_CUSTOMER_HTC
/* <INUSE> */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIPS_FDY_OPTION{
    CIPS_FDY_DISABLE = 0,   /**< Disable PS power consuming control */
    CIPS_FDY_ENABLE,        /**< Enable PS power consuming control */

    /* This one must always be last in the list! */
    CIPS_NUM_FDY_OPTIONS    /**< Number of options defined */
} _CiPsFDYOpt;

typedef UINT8 CiPsFDYOpt;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_PS_PRIM_SET_FAST_DORMANCY_CONFIG_REQ">   */
typedef struct CiPsPrimSetFastDormancyConfigReq_struct
{
    CiPsFDYOpt  type;     /**< Type. \sa CiPsFDYOpt */
    UINT16      interval; /**< Trigger Interval (seconds), the default value is 3*/
} CiPsPrimSetFastDormancyConfigReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_PS_PRIM_SET_FAST_DORMANCY_CONFIG_CNF">   */
typedef struct CiPsPrimSetFastDormancyConfigCnf_struct
{
    CiPsRc rc;     /**< Result code. \sa  CiPsRc */
} CiPsPrimSetFastDormancyConfigCnf;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_PS_PRIM_PDP_ACTIVATION_REJECT_CAUSE_IND">   */
typedef struct CiPsPrimPdpActivationRejectCauseInd_struct
{
    UINT8 cid;         /**< PDP Context Identifier, [0- CI_PS_MAX_PDP_CTX_NUM-1] */
    UINT8 smCause;     /**< SM cause for PDP activation reject. */
} CiPsPrimPdpActivationRejectCauseInd;
#endif
/*****************************************************************/


#ifdef CI_CUSTOM_EXTENSION
/* it is assumed that only one customized set of extension primitives is
 * to be considered at one time. The selection of the particular customized
 * set is done in the 'ci_ps_cust.h' based on compile flags.
 *
 * Note: if no customer extension primitives are defined for this service group
 * the CI_PS_NUM_CUST_PRIM will be set to 0 in the "ci_ps_cust.h" file.
 */
#include "ci_ps_cust.h"

#define CI_PS_NUM_PRIM ( CI_PS_NUM_COMMON_PRIM + CI_PS_NUM_CUST_PRIM )

#else

/* if no customer extension is supported, only the default common set is considered */
#define CI_PS_NUM_PRIM CI_PS_NUM_COMMON_PRIM

#endif /* CI_CUSTOM_EXTENSION */

#ifdef __cplusplus
}
#endif  //__cplusplus

#endif  /* _CI_PS_H_ */


/*                      end of ci_ps.h
   --------------------------------------------------------------------------- */
