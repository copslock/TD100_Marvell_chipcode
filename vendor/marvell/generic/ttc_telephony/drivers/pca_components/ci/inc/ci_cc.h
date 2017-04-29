/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/* ===========================================================================
   File        : ci_cc.h
   Description : Data types file for the CC Service Group

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

#if !defined(_CI_CC_H_)
#define _CI_CC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ci_api_types.h"
/** \addtogroup  SpecificSGRelated
 * @{ */

#define CI_CC_VER_MAJOR 3
#define CI_CC_VER_MINOR 0

/* -----------------------------------------------------------------------------
 *    CC Configuration definitions
 * ----------------------------------------------------------------------------- */
/* default value for the interval between two DTMF tones in a string */
#define CICC_DTMF_DEFAULT_INTERVAL        30

/* Maximum length (bytes) of a DTMF digit string (CiCcPrimSendDtmfStringReq) */
#define CICC_MAX_DTMF_STRING_LENGTH   32

/* Maximum Number of Calls (Active and Held) allowed at any time */
#define CICC_MAX_CURRENT_CALLS        7 /* Dictated by Multiparty rules */

/* ----------------------------------------------------------------------------- */

/* CI_CC Primitive ID definitions */
//ICAT EXPORTED ENUM
/** Summary of primitives */
typedef enum CI_CC_PRIM {
	CI_CC_PRIM_GET_NUMBERTYPE_REQ = 1, /**< \brief Get Type of Number Request. \details   */
	CI_CC_PRIM_GET_NUMBERTYPE_CNF, /**< \brief Get Type of Number Confirmation. \details See CI_CC_PRIM_SET_NUMBERTYPE_REQ for internal default information.
					 There should be no reason for an unsuccessful result.
				       */
	CI_CC_PRIM_SET_NUMBERTYPE_REQ,  /**<  \brief  Sets the Type of Number to be used for subsequent outgoing call requests.
					 * \details Default values for the Address Type fields are based on the outgoing call requests themselves (in the Dial String parameter field).
					 * See also the CI_CC_PRIM_MAKE_CALL_REQ request.
					 * For International calls (where the '+' character appears as a prefix to the Dial String), the values are:
					 * Type of Number = CI_NUMTYPE_INTERNATIONAL
					 * Numbering Plan = CI_NUMPLAN_E164_E163
					 * For all other calls (this is the SAC internal default), the values are:
					 * Type of Number = CI_NUMTYPE_UNKNOWN
					 * Numbering Plan = CI_NUMPLAN_E164_E163 */
	CI_CC_PRIM_SET_NUMBERTYPE_CNF,                  /**< \brief Confirms a request to set the Type of Number to be used for subsequent outgoing call requests. \details */
	CI_CC_PRIM_GET_SUPPORTED_CALLMODES_REQ,         /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_CC_PRIM_GET_SUPPORTED_CALLMODES_CNF,         /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_CC_PRIM_GET_CALLMODE_REQ,                    /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_CC_PRIM_GET_CALLMODE_CNF,                    /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_CC_PRIM_SET_CALLMODE_REQ,                    /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_CC_PRIM_SET_CALLMODE_CNF,                    /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_CC_PRIM_GET_SUPPORTED_DATA_BSTYPES_REQ,      /**< \brief Requests supported Bearer Service Type parameter settings for data calls. \details  */
	CI_CC_PRIM_GET_SUPPORTED_DATA_BSTYPES_CNF,      /**< \brief Reports supported Bearer Service Type parameter settings for data calls. \details  There should be no reason for an unsuccessful result.*/
	CI_CC_PRIM_GET_DATA_BSTYPE_REQ,                 /**< \brief Requests currently selected Bearer Service Type information for outgoing (and incoming) data calls.\details */
	CI_CC_PRIM_GET_DATA_BSTYPE_CNF,                 /**< \brief Reports currently selected Bearer Service Type information for outgoing (and incoming) data calls. \details There should be no reason for an unsuccessful result.*/
	CI_CC_PRIM_SET_DATA_BSTYPE_REQ, /**< \brief Selects Bearer Service Type information for outgoing (and incoming) data calls.
					 * \details This information (or the defaults) is used when outgoing Data Calls (or Multi-Mode Calls with a Data component)
					 * are originated. It may also be used during mobile terminated Data Call setup.
					 * Not all combinations of the Data Bearer Service Type parameters are supported for GSM/UMTS.
					 * Other Bearer Capability information for outgoing and incoming calls is set up from internal defaults */
	CI_CC_PRIM_SET_DATA_BSTYPE_CNF, /**< \brief Confirms a request to select Bearer Service Type information for outgoing (and incoming) data calls.
					 *  \details This information may be "negotiated" with the Network as required, on a per-call basis. */
	CI_CC_PRIM_GET_AUTOANSWER_ACTIVE_REQ,   /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_CC_PRIM_GET_AUTOANSWER_ACTIVE_CNF,   /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_CC_PRIM_SET_AUTOANSWER_ACTIVE_REQ,   /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_CC_PRIM_SET_AUTOANSWER_ACTIVE_CNF,   /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_CC_PRIM_LIST_CURRENT_CALLS_REQ,  /**< \brief  Requests a list of current calls that are present in the mobile.
					     * \details Requests Current Call Information for all Calls.
					     * CI_CC_PRIM_GET_CALLINFO_REQ may also be used to request Call Information for a specified Call. */
	CI_CC_PRIM_LIST_CURRENT_CALLS_CNF,              /**< \brief  Returns a list of current calls that are present in the mobile.
							 *   \details Returns an array of Current Call Information structures. CI_CC_PRIM_GET_CALLINFO_REQ may also be used to
							 *   request Call Information for a specified Call. If there are no Current Calls, NumCalls = 0, and the CallInfo array
							 *   contains no useful information. There should be no reason for an unsuccessful result. */
	CI_CC_PRIM_GET_CALLINFO_REQ,                    /**< \brief Requests Current Call Information for a specified Call Identifier.\details */
	CI_CC_PRIM_GET_CALLINFO_CNF,        /**< \brief Returns Current Call Information for a specified Call Identifier.
					     *   \details Use CI_CC_PRIM_LIST_CURRENT_CALLS_REQ to request a list of current Call Identifiers. For a data call, the
					     *  Bearer Service information may have been "negotiated" with the Network. If Result indicates an error, the Call
					     *  Information structure contains no useful information
					     */
	CI_CC_PRIM_MAKE_CALL_REQ, /**< \brief Requests the Origination of an Outgoing Call.
				 * \details Uses the default Call Mode and current Bearer Service Type information (or the appropriate defaults).
				 *  The default Call Mode is  "Single Mode", therefore CI_CC_PRIM_MAKE_CALL_REQ request will use
				 * the BasicCMode parameter to indicate which Mode is required.
				 *    Bearer Service Type is set by CI_CC_PRIM_SET_DATA_BSTYPE_REQ. Other Bearer Capability information is set up from internal defaults.
				 *    Call Options can be used to:
				 *    - Enable or disable Closed User Group (CUG) information for this call.
				 *    - Override the CLIR option for this call (force Enable or Disable).
				 *    CUG Information is configured by CI_SS_PRIM_SET_CUG_CONFIG_REQ.
				 *        The default CLIR option is set up by CI_SS_PRIM_SET_CLIR_OPTION_REQ.
				 *    Valid dial digits are defined in Referenced Documentation , "Mobile Radio Interface layer 3 specification; Core Network Protocols - Stage 3" ,revision 3.11.0 ,3GPP TS 24.008.
				 *    The supplied dial string may incorporate a prefix '+' character, to indicate that this is an international call. In this
				 *    situation, SAC will use the default Address Type information (if necessary) for an international call. For details,
				 *    see the CI_CC_PRIM_SET_NUMBERTYPE_REQ request.
				 *    SAC will use the Dial String digits themselves to determine whether an Emergency Call is being requested. For
				 *    the Emergency Call support requirements (together with a list of default emergency numbers).
				 *    SAC will also support "Fixed Dialing Mode" (if enabled in the SIM), using the Fixed Dialing Numbers (FDN) List
				 *    that is stored on the SIM. For the Fixed Number Dialing support requirements, see  Referenced Documentation ,
				 *    "Technical Specification Group Services and System Aspects. Service Aspects; Service principles" ,revision
				 *    3.13.0 ,3GPP TS 22.101 ,Section A.24 */
	CI_CC_PRIM_MAKE_CALL_CNF,       /**< \brief Confirms a request to originate an outgoing Call. \details */
	CI_CC_PRIM_CALL_PROCEEDING_IND, /**< \brief  Presents a Call Proceeding indication for an outgoing call in progress.
				     * \details This indication is triggered by a CALL PROCEEDING notification from the Network. It indicates that the outgoing
				     *  call request has been accepted, and is proceeding through the Network. If in-band tones are available from the
				     *  Network, the Receive Audio path should be enabled, so that the subscriber can hear the tones.No
				     *  explicit response is required.  */
	CI_CC_PRIM_MO_CALL_FAILED_IND,  /**< \brief Presents a Call Failed indication for an outgoing (Mobile Originated) call.
					 *   \details The reason for the call failure is indicated by the Cause parameter. If in-band tones are available from the
					 *  Network, the Receive Audio path should be enabled, so that the subscriber can hear the tones.
					 *  No explicit response is required. */
	CI_CC_PRIM_ALERTING_IND, /**< \brief Presents an Alerting indication for an outgoing call in progress.
				 * \details This indication is triggered by an ALERTING notification from the Network, and indicates that the called party's phone is alerting
				 *    If in-band tones (Ringback) are available from the Network, the Receive Audio path should be
				 *    enabled, so that the subscriber can hear the tones. Otherwise, the tones must be generated locally. */
	CI_CC_PRIM_CONNECT_IND, /**< \brief Presents a Connect indication for an outgoing or incoming call.
				 * \details This indication is triggered by a CONNECT notification from the Network, indicating that the called party has accepted and
				 *   answered the call, or an incoming call has been answered. If not already done, the Audio paths should now be enabled. No explicit response is required. */
	CI_CC_PRIM_DISCONNECT_IND, /**< \brief Presents a Disconnect indication for Call Clearing.
				 * \details This indication is triggered by:
				 *   - A DISCONNECT message received from the Network, indicating Network-Initiated Call Clearing
				 *   - Mobile-Initiated Call Clearing (Hangup)
				 *   The reason for Call Clearing is indicated by the Cause parameter. If not already done, the Transmit and Receive
				 *   Audio paths should now be disabled. No explicit response is required */
	CI_CC_PRIM_INCOMING_CALL_IND,   /**< \brief  Presents an Incoming Call indication.
					 * \details SAC allocates a unique Call Identifier, which must be used for all subsequent requests that are directed at this call.
					 * The incoming call SETUP from the Network has already been accepted, and the Mobile has sent an ALERTING indication to the Network in response.
					 * The subscriber may answer the call, using the CI_CC_PRIM_ANSWER_CALL_REQ request.
					 * The subscriber may refuse the call (if allowed), using the CI_CC_PRIM_REFUSE_CALL_REQ request.
					 * In either case, the subscriber must be alerted of the incoming call. */
	CI_CC_PRIM_CALL_WAITING_IND, /**< \brief Reports Call Waiting (CW) information (when enabled) for an incoming call.
				  * \details AC allocates a unique Call Identifier, which must be used for all subsequent requests that are directed at this call.
				  *  Call Waiting indications may be enabled or disabled by the CI_SS_SET_CW_OPTION_REQ request.
				  *  This indication is enabled by default.
				  *  This indication will be received only if the Call Waiting Supplementary Service is provisioned.
				  *  The Active Call must first be held, and the subscriber can then decide to answer or refuse the Waiting Call.
				  *  No explicit response is required.*/
	CI_CC_PRIM_HELD_CALL_IND, /**< \brief Reminds the Subscriber that there is a Held Call.
			      * \details SAC will send this indication if an Active Call is Released while a Held Call exists.
			      * The Held Call can then be Released or Retrieved, as desired.
			      *  No explicit response is required, although a request is needed to take the call off Hold.*/
	CI_CC_PRIM_ANSWER_CALL_REQ, /**< \brief Answers an Incoming Call.
				 *   \details If Auto-Answer is active (see CI_CC_PRIM_SET_AUTOANSWER_ACTIVE_REQ),
				 * SAC may answer the incoming call automatically, and the CI_CC_PRIM_ANSWER_CALL_REQ request may not be required.
				 *  This request will not Answer a Waiting Call.
				 * The Active Call must first be Held or Released, which will trigger SAC to send a
				 * CI_CC_PRIM_INCOMING_CALL_IND indication for the Waiting Call. This call can then be Answered.
				 *  This request will not switch modes for a multi-mode call (Alternating Voice/Data, Alternating Voice/Fax or Voice followed by Data). To do this, use the CI_CC_PRIM_SWITCH_CALLMODE_REQ request. */
	CI_CC_PRIM_ANSWER_CALL_CNF, /**< \brief  Confirms a request to Answer an Incoming Call.
				 * \details The Call Identifier is included as a crosscheck, or confirmation.*/
	CI_CC_PRIM_REFUSE_CALL_REQ,     /**< \brief Refuses an Incoming Call (may be a Call Waiting).*/
	CI_CC_PRIM_REFUSE_CALL_CNF,     /**< \brief Confirms a request to refuse an Incoming Call. */
	CI_CC_PRIM_MT_CALL_FAILED_IND,  /**< \brief  */
	CI_CC_PRIM_HOLD_CALL_REQ,  /**< \brief Requests an Active Call to be Held.
				    * \details This request can be used to hold the Active Call, if there is one.
				    *   The subscriber may subsequently:
				    *   Retrieve the Held Call
				    *   Set up another (outgoing) Call
				    *   Accept an Incoming Call
				    *   If another Active Call is set up, the user may subsequently:
				    *   Alternate between the Active Call and the Held Call
				    *   Disconnect the Active Call
				    *   Disconnect the Held Call
				    *   Disconnect both the Active Call and the Held Call
				    *   GSM allows only one Call to be Held at any time (except for Calls that are part of a Multiparty Call).
				    *   If a Held Call already exists, this request will fail with an error result.*/
	CI_CC_PRIM_HOLD_CALL_CNF, /**< \brief Confirms a request to Hold an Active Call. */
	CI_CC_PRIM_RETRIEVE_CALL_REQ, /**< \brief Requests a Held Call to be Retrieved.
				    * \details  This request can be used to retrieve a Held Call, if there is one.
				    * For GSM, the HOLD service will not allow more than one Held Call (non-Multiparty) at any time.
				    * The CallId parameter is provided here to accommodate MultiCall procedures for other 3G protocols.
				    * This request should not be used to perform operations on a Multiparty (MPTY) Call.
				    * If this is attempted, the request will fail with an error result.*/
	CI_CC_PRIM_RETRIEVE_CALL_CNF,   /**< \brief Confirms a request to Retrieve a Held Call.
					 * \details If Result is CIRC_CC_REJECTED, the Cause information is reported as received in a Retrieve Reject message from the Network*/
	CI_CC_PRIM_SWITCH_ACTIVE_HELD_REQ, /**< \brief Requests an Active Call and a Held Call to be Switched (Shuttle Request).
					  * \details This request is used to "shuttle" back and forth between an Active Call and a Held Call.
					  * The Active Call is held and the Held Call is retrieved (becomes the Active Call).
					  * The shuttle operation is handled at the Network.
					  * The Mobile must send a Hold Request for the current Active Call, immediately followed (see below) by a Retrieve Request for the Held Call.
					  * The HOLD service will not support more than one Held Call at any time. To avoid this, the Network must receive the Retrieve Request within 5 seconds of receiving the Hold Request.
					  * The same result could be achieved by sending a CI_CC_PRIM_HOLD_CALL_REQ request for the Active Call,
					  * followed by a CI_CC_RETRIEVE_CALL_REQ request for the Held Call.
					  * However, the above timing requirements may not be fulfilled by this method, and the shuttle operation could fail.
					  * Use the CI_CC_PRIM_SHUTTLE_MPTY_REQ request for "shuttle" operations that involve Multiparty (MPTY) Conference Calls. */
	CI_CC_PRIM_SWITCH_ACTIVE_HELD_CNF, /**< \brief Confirms a request for an Active Call and a Held Call to be Switched (Shuttle Request).
					 * \details */
	CI_CC_PRIM_CALL_DEFLECT_REQ,            /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_CC_PRIM_CALL_DEFLECT_CNF,            /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_CC_PRIM_EXPLICIT_CALL_TRANSFER_REQ, /**< \brief Requests an Explicit Call Transfer (ECT) for an Active Call and a Held Call.
					    * \details This request is used to connect together an Active Call and a Held Call, and then to exit both calls.
					    * The GSM Standard allows ECT for a Held Call and an Outgoing Call in the 'Alerting' state
					    * (an MO Call that has been presented to the Called Party, but has not yet been answered).
					    * This is described in the Standard as 'a network option', and is therefore a valid operation only if the Network supports it.
					    * For more information, see 3GPP TS 22.091.
					    * The ECT service requires the provision of the Call Hold (HOLD) Supplementary Service.
					    * If the subscriber has an Active Call, a Held Call and a Call Waiting,
					    * then after successful completion of the ECT request, the subscriber will receive an Incoming Call indication for the Waiting Call.
					    * Multiparty (MPTY) Calls cannot be transferred by this (or any other) request.*/
	CI_CC_PRIM_EXPLICIT_CALL_TRANSFER_CNF,  /**< \brief Confirms an Explicit Call Transfer (ECT) request.
						 * \details If the ECT request is successfully completed, there will be a normal Call between the original Held party and
						 *  Active/Alerting party (see 3GPP TS 22.091, Section 5.8). SAC will remove the Call Identifiers for the original
						 *  Calls. An attempt to use these Call Identifiers in any subsequent call related request will fail with Result =
						 *  CIRC_CC_INVALID_CALLID.	*/
	CI_CC_PRIM_RELEASE_CALL_REQ, /**< \brief Requests Release (Hangup) of a Call (Mobile Originated Call Clearing).
				  * \details Requests disconnect of an Active Call or a Held Call.
				  *  If there is a Held Call and the Active Call is Released, SAC will send a CI_CC_PRIM_HELD_CALL_IND indication for the Held Call. The subscriber may then either Retrieve the Held Call or Release it.
				  *  If there is a Waiting Call and the Active Call is Released, SAC will send a CI_CC_PRIM_INCOMING_CALL_IND indication for the Waiting Call, which can then be Answered or Refused.
				  *  This request is also used to release individual calls in a Multiparty Call.
				  *  If CallId is CICC_NO_CALL_ID and there is only one call in progress, this call will be released. */
	CI_CC_PRIM_RELEASE_CALL_CNF, /**< \brief  Confirms a request to Release (Hangup) an Active or Held Call (Mobile Originated Call Clearing).*/
	CI_CC_PRIM_RELEASE_ALL_CALLS_REQ, /**< \brief Requests Release (Disconnect) of All Calls (Mobile Originated Call Clearing).
				       * \details For GSM, there can be only one Active Call and only one Held Call.*/
	CI_CC_PRIM_RELEASE_ALL_CALLS_CNF, /**< \brief Confirms a request to Release (Disconnect) All Calls (Mobile Originated Call Clearing).
				       * \details If the calls are successfully released (disconnected), SAC will remove all individual Call Identifiers.
				       *  An attempt to use any of these Call Identifiers in subsequent call related requests will fail with Result =
				       *  CIRC_CC_INVALID_CALLID.*/
	CI_CC_PRIM_SWITCH_CALLMODE_REQ,         /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_CC_PRIM_SWITCH_CALLMODE_CNF,         /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_CC_PRIM_ESTABLISH_MPTY_REQ, /**< \brief Requests Establishment of a Multiparty (MPTY) Conference Call.
				    * \details This request is used to establish a Multiparty Call, starting with an Active Call and a Held Call.
				    *   If this request completes successfully, the Active and Held calls are joined into a Multiparty Call, which will
				    *   then become Active. The original Call Identifiers still exist for the two calls, and both calls are now marked as
				    *   MPTY Calls in their respective Call Information structures.*/
	CI_CC_PRIM_ESTABLISH_MPTY_CNF, /**< \brief Confirms a request to establish a Multiparty (MPTY) Conference Call.
				    * \details If Result indicates an error, the MPTY Identifier is not valid and it should be ignored.*/
	CI_CC_PRIM_ADD_TO_MPTY_REQ, /**< \brief Requests a new Call to be added to an existing Multiparty (MPTY) Conference.
				 * \details Before this request is sent, the existing MPTY Call will be Held and the New Call will be Active.
				 * On successful completion of this request, the "expanded" MPTY Call will be Active.
				 *    If a call was previously "split" from the MPTY Call, this request can be used to add the Call back into the Conference. */
	CI_CC_PRIM_ADD_TO_MPTY_CNF, /**< \brief Confirms a request to establish a Multiparty (MPTY) Conference Call. */
	CI_CC_PRIM_HOLD_MPTY_REQ, /**< \brief Requests an Active Multiparty (MPTY) Call to be Held.
				* \details On successful completion of this request, the Active MPTY Call (if there is one) will be Held. */
	CI_CC_PRIM_HOLD_MPTY_CNF,       /**< \brief Confirms a request to Hold an Active Multiparty (MPTY) Conference Call. */
	CI_CC_PRIM_RETRIEVE_MPTY_REQ, /**< \brief Requests a Held Multiparty (MPTY) Call to be Retrieved.
				    * \details On successful completion of this request, the Held MPTY Call (if there is one) will be Active.*/
	CI_CC_PRIM_RETRIEVE_MPTY_CNF,   /**< \brief Confirms a request to Retrieve a Held Multiparty (MPTY) Conference Call.  */
	CI_CC_PRIM_SPLIT_FROM_MPTY_REQ, /**< \brief Requests a Call to be Split out from a Multiparty (MPTY) Conference.
				      *  \details On successful completion of this request the existing MPTY Call will be Held, except for the "Split" Call, which will be Active.
				      * Use the CI_CC_PRIM_ADD_TO_MPTY_REQ request to add the Split Call back into the MPTY Call. */
	CI_CC_PRIM_SPLIT_FROM_MPTY_CNF, /**< \brief Confirms a request to Split out a Call from a Multiparty (MPTY) Conference Call.*/
	CI_CC_PRIM_SHUTTLE_MPTY_REQ, /**< \brief Request to "Shuttle" between a Multiparty (MPTY) Conference Call and a Single Call.
				   * \details The subscriber can use this request to "shuttle" back and forth between a Multiparty (MPTY) Call and a separate Single Call.
				   * This is similar to the "shuttle" operation between an Active Call and a Held call, and it works like this:
				   *  -	Single Call Active; MPTY Call Held   MPTY Call Active, Single Call Held
				   *  -	MPTY Call Active; Single Call Held   Single Call Active, MPTY Call Held
				   *  The shuttle operation is handled at the Network. The Mobile must send a Hold (or Hold MPTY) Request for the current Active Call/MPTY, immediately followed (see below) by a Retrieve (or Retrieve MPTY) Request for the Held Call/MPTY.
				   *  There cannot be more than one Held Call at any time. To avoid this, the Network must receive the Retrieve Request within 5 seconds of receiving the Hold Request.
				   *  The same result could be achieved by sending a CI_CC_PRIM_HOLD_CALL_REQ or CI_CC_PRIM_HOLD_MPTY_REQ request for the Active Call/MPTY, followed by a CI_CC_RETRIEVE_CALL_REQ or CI_CC_RETRIEVE_MPTY_REQ request for the Held Call/MPTY. However, the above timing requirements may not be fulfilled by this method, and the shuttle operation could fail.
				   * */
	CI_CC_PRIM_SHUTTLE_MPTY_CNF,    /**< \brief Confirms a request to "Shuttle" between a Multiparty (MPTY) Conference Call and a Single Call. */
	CI_CC_PRIM_RELEASE_MPTY_REQ, /**< \brief  Requests a Multiparty (MPTY) Conference Call to be Released.
				    * \details  On successful completion of this request, all calls for the MPTY Call (if there is one) will be Released.
				    *  To release calls individually from the MPTY Call, use the CI_CC_RELEASE_CALL_REQ request.
				    */
	CI_CC_PRIM_RELEASE_MPTY_CNF,    /**< \brief Confirms a request to Release an Active Multiparty (MPTY) Conference Call.*/
	CI_CC_PRIM_START_DTMF_REQ, /**< \brief Request to start sending a DTMF Digit to the Network during an Active Call.
				   * \details Valid DTMF digits are defined by the GSM Standard.
				   * DTMF digits can be sent only on an active speech connection, where a traffic channel has been allocated.
				   *  A DTMF request cannot be sent while a previous DTMF request is still in progress. If this is attempted, the request will fail with an error result.
				   *  For this request, the Application Layer must handle the timing of individual DTMF digits, and must stop the transmission by issuing a CI_CC_PRIM_STOP_DTMF_REQ request at the appropriate time.
				   */
	CI_CC_PRIM_START_DTMF_CNF, /**< \brief Confirms a request to start sending a DTMF Digit to the Network during an Active Call.
			       * \details
			       *  This confirmation will be received in any of the following situations:
			       *   -	 The DTMF Digit was sent successfully
			       *   -	 A DTMF Start Reject message was received from the Network
			       *   -	 SAC did not accept the DTMF request
			       *   -	 There was a timeout on receiving a response from the Network
			       *  The Application Layer must handle the timing of individual DTMF digits, and must stop the transmission by issuing a CI_CC_PRIM_STOP_DTMF_REQ request at the appropriate time.
			       *  */
	CI_CC_PRIM_STOP_DTMF_REQ, /**< \brief Request to stop sending a DTMF Digit to the Network during an Active Call.
			      * \details DTMF digits can be sent only on an active speech connection, where a traffic channel has been allocated.
			      *   A DTMF request cannot be sent while a previous DTMF request is still in progress. If this is attempted, the request will fail with an error result.
			      *   Similarly, if this request is received when no DTMF tone is currently active, the request will fail with an error result. */
	CI_CC_PRIM_STOP_DTMF_CNF, /**< \brief Confirms a request to stop sending a DTMF Digit to the Network during an Active Call.
			       * \details This confirmation will be received in any of the following situations:
			       *  -	The DTMF Digit was stopped successfully
			       *  -	A DTMF Stop Reject message was received from the Network
			       *  -	SAC did not accept the DTMF request
			       *  -	There was a timeout on receiving a response from the Network. */

	CI_CC_PRIM_GET_DTMF_PACING_REQ, /**< \brief Requests the current DTMF Pacing Configuration values.
				    * \details The DTMF Pacing Configuration values will initially be set to internal defaults.
				    *   These values can be changed by a CI_CC_SET_DTMF_CONFIG_REQ request.
				    *   The pacing configuration values are used by CI when sending strings of DTMF digits to the Network. In this situation,
				    *   CI will "handshake" with the Network for each digit in turn, and will use the configured tone duration and inter-digit
				    *   intervals to "pace" the individual tones. */
	CI_CC_PRIM_GET_DTMF_PACING_CNF, /**< \brief Reports the current DTMF Pacing Configuration values.
				    * \details There should be no reason for an unsuccessful result.
				    *  On GSM, the Network will enforce the minimum inter-digit interval. SAC will not use the Interval field in the
				    *  CiCcDtmfPacing structure. */
	CI_CC_PRIM_SET_DTMF_PACING_REQ, /**< \brief Sets the DTMF Pacing Configuration values.
				     * \details If this request is not invoked, SAC will use default configuration values (See define#CICC_MIN_DTMF_DURATION
				     *  for Duration and define#CICC_DTMF_DEFAULT_INTERVAL for Interval).
				     *  The Pacing Configuration values are used by SAC when sending strings of DTMF digits to the Network
				     *  (see the CI_CC_PRIM_SEND_DTMF_STRING_REQ request). In this situation, SAC will "handshake" with the
				     *  Network for each digit in turn, and will use the configured tone duration and inter-digit intervals to "pace" the
				     *  individual tones.
				     *  SAC does not check for ridiculously large values in the supplied DTMF Pacing Configuration structure. However,
				     *  if any of the Pacing Configuration values are set below the specified minimum values (See define#CICC_MIN_DTMF_DURATION
				     *  for Duration and define#CICC_DTMF_DEFAULT_INTERVAL for Interval), this request will fail with an error result.
				     *  On GSM, the Network will enforce the minimum inter-digit interval. SAC will not use the Interval field in the
				     *  CiCcDtmfPacing structure. */
	CI_CC_PRIM_SET_DTMF_PACING_CNF, /**< \brief Confirms a request to set the DTMF Pacing Configuration values.
				     * \details SAC will return an error result if any of the Pacing Configuration values are less then specified minimum values
				     *  in the request (See define#CICC_MIN_DTMF_DURATION for Duration and define#CICC_DTMF_DEFAULT_INTERVAL for Interval)*/
	CI_CC_PRIM_SEND_DTMF_STRING_REQ, /**< \brief Requests a String of DTMF Digits to be sent on an Active Call.
				     * \details Valid DTMF digits are defined by the GSM Standard.
				     *   DTMF digits can be sent only on an active speech connection, where a traffic channel has been allocated.
				     *   If this is not the case, this request will fail with an error result.
				     *   As the GSM protocol provides only a single-digit DTMF control procedure, SAC will send the DTMF digits
				     *   individually, using Start DTMF and Stop DTMF requests to the Network.
				     *   The DTMF "pacing" parameters (tone duration and inter-digit interval) can be specified by the
				     *   CI_CC_PRIM_SET_DTMF_PACING_REQ request. See define#CICC_MIN_DTMF_DURATION
				     *   and define#CICC_DTMF_DEFAULT_INTERVAL for the parameter defaults.
				     *   On GSM, the Network will enforce the minimum inter-digit interval. SAC will not use the Interval field in the
				     *   CiCcDtmfPacing structure.*/
	CI_CC_PRIM_SEND_DTMF_STRING_CNF, /**< \brief Confirms a request to send a String of DTMF Digits on an Active Call.
				     * \details This confirmation will be received in any of the following situations:
				     *  -	The complete DTMF String was sent successfully
				     *  -	A DTMF Reject message (Start or Stop) was received from the Network
				     *  -	SAC did not accept the DTMF request
				     *  -	There was a timeout on receiving a response from the Network*/
	CI_CC_PRIM_CLIP_INFO_IND, /**< \brief Reports CLIP information (when enabled) for an incoming call.
			      * \details CLIP indications may be enabled or disabled by CI_SS_PRIM_SET_CLIP_OPTION_REQ.
			      *  This indication is enabled by default.
			      *  No explicit response is required. */
	CI_CC_PRIM_COLP_INFO_IND, /**< \brief Reports CoLP information (when enabled locally) for an outgoing call.
			       * \details CoLP indications may be enabled or disabled by the CI_SS_PRIM_SET_COLP_OPTION_REQ.
			       *   This indication is enabled by default.
			       *   No explicit response is required.*/
	CI_CC_PRIM_CCM_UPDATE_IND, /**< \brief Unsolicited Periodic Current Call Meter (CCM) update indication.
				* \details CCM indications may be enabled or disabled by CI_SS_PRIM_SET_CCM_OPTION_REQ.
				* If enabled indications are reported periodically, not more than once every 10 seconds during a call.
				* This indication is enabled by default.
				* No explicit response is required.*/
	CI_CC_PRIM_GET_CCM_VALUE_REQ, /**< \brief Requests the current value of the Current Call Meter (CCM).
				   * \details The CCM value is normally requested only during a call (that is why this request is in the CC service group), but
				   *  it may be requested at any time. Other quantities related to Call Charging (ACM, ACMmax and PUCT) are accessible
				   *  through the SIM Manager interface. For more information, see the SIM Service Group primitives. */
	CI_CC_PRIM_GET_CCM_VALUE_CNF, /**< \brief Reports the current value of the Current Call Meter (CCM).
				   * \details If the Result Code indicates failure, the CCM value is not useful, and it should be ignored. */
	CI_CC_PRIM_AOC_WARNING_IND, /**< \brief Unsolicited Advice of Charge (AoC) warning indication.
				   * \details If the Advice of Charge service is provisioned, SAC can send a warning indication during a call when the Accumulated Charge Meter (ACM) is within 30 seconds of the programmed maximum (ACMmax) value.
				   * This indication can be enabled or disabled by the CI_SS_PRIM_SET_AOC_WARNING_ENABLE_REQ request.
				     In addition, this indication can be sent if a new Incoming or Outgoing Call is set up when the ACM is within 30 seconds of the programmed ACMmax value.
				     This indication is enabled by default.
				     No explicit response is required.
				   */
	CI_CC_PRIM_SSI_NOTIFY_IND,      /**< \brief Supplementary Service Intermediate (SSI) Notification.
					 * \details SSI notifications (if enabled - see CI_SS_PRIM_SET_SS_NOTIFY_OPTIONS_REQ) are triggered by receipt of an intermediate Supplementary Service notification at any of the following times:
					 * After mobile originated Call Setup, but before any Call Setup results are received
					 * This notification is enabled by default.
					 * No explicit response is required.*/

	CI_CC_PRIM_SSU_NOTIFY_IND, /**< \brief Supplementary Service Unsolicited (SSU) Notification.
				 * \details SSU notifications (if enabled - see CI_SS_PRIM_SET_SS_NOTIFY_OPTIONS_REQ) are triggered by receipt of an unsolicited Supplementary Service notification at any of the following times:
				 * During mobile terminated Call Setup
				 * During a call
				 * Whenever a Forward Check Supplementary Service notification is received (in call or out of call)
				 * This notification is enabled by default.
				 * No response is required.*/
	CI_CC_PRIM_LOCALCB_NOTIFY_IND, /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_CC_PRIM_GET_ACM_VALUE_REQ, /**< \brief Requests the current value of the Accumulated Call Meter (ACM).
				      * \details The ACM value holds accumulated CCM units for the current call (if there is one) and all previous calls since the
				      *  ACM was last reset. */

	CI_CC_PRIM_GET_ACM_VALUE_CNF, /**< \brief Reports the current value of the Accumulated Call Meter Maximum value (ACMmax).
				   * \details If the Result code indicates failure, the ACMmax value is not useful, and it should be ignored.*/
	CI_CC_PRIM_RESET_ACM_VALUE_REQ, /**< \brief Requests the Accumulated Call Meter (ACM) to be reset to zero.
				     * \details The ACM value holds accumulated CCM units for the current call (if there is one) and all previous calls since the ACM was last reset.
				     *  This operation requires prior PIN2 verification.*/
	CI_CC_PRIM_RESET_ACM_VALUE_CNF, /**< \brief Confirms a request to reset the Accumulated Call Meter (ACM) to to zero.
					 * \details If the Result code indicates failure, the ACM value was not reset. */
	CI_CC_PRIM_GET_ACMMAX_VALUE_REQ, /**< \brief Requests the current value of the Accumulated Call Meter Maximum value (ACMmax).
				      * \details The ACMmax value holds the maximum value allowed for the ACM.
				      *  If the ACM is "close" to the ACMmax value, SAC can issue a warning indication (if enabled). F
				      * or more information see the CI_CC_PRIM_AOC_WARNING_IND indication.*/
	CI_CC_PRIM_GET_ACMMAX_VALUE_CNF, /**< \brief Reports the current value of the Accumulated Call Meter Maximum value (ACMmax).
					 * \details If the Result code indicates failure, the ACMmax value is not useful, and it should be ignored.*/
	CI_CC_PRIM_SET_ACMMAX_VALUE_REQ, /**< \brief Requests the Accumulated Call Meter Maximum (ACMmax) to be set to the supplied value.
					 * \details The ACMmax value holds the maximum value allowed for the ACM.
					 *   If the ACM is close to the ACMmax value, SAC can issue a warning indication (if enabled). For more information see the CI_CC_PRIM_AOC_WARNING_IND indication.
					 *   Setting ACMmax to zero will disable it, and will effectively remove the maximum ACM limit. There will obviously be no CI_CC_PRIM_AOC_WARNING_IND indications, whether they are enabled locally or not.
					 *   This operation requires prior PIN2 verification.*/
	CI_CC_PRIM_SET_ACMMAX_VALUE_CNF, /**< \brief Confirms a request to set the Accumulated Call Meter Maximum (ACMmax) value.
				      * If the Result code indicates failure, the ACMmax value was not changed.*/
	CI_CC_PRIM_GET_PUCT_INFO_REQ, /**< \brief Requests the current Price per Unit and Currency Table (PUCT) information.
				   * \details The PUCT information is used to enable the Application to calculate the cost of a call, in a currency chosen by the subscriber.*/
	CI_CC_PRIM_GET_PUCT_INFO_CNF, /**< \brief Reports the current Price per Unit and Currency Table (PUCT) information.
				   * \details If the Result code indicates failure, the PUCT information is not useful, and it should be ignored. */
	CI_CC_PRIM_SET_PUCT_INFO_REQ, /**< \brief Requests the current Price per Unit and Currency Table (PUCT) information to be updated.
				   *  \details The PUCT information is used to enable the Application to calculate the cost of a call, in a currency chosen by the subscriber.
				   *  This operation requires prior PIN2 validation.*/
	CI_CC_PRIM_SET_PUCT_INFO_CNF, /**< \brief Confirms a request to update the current Price per Unit and Currency Table (PUCT) information.
				   *  \details If the Result code indicates failure, the PUCT information was not updated.*/
	CI_CC_PRIM_GET_BASIC_CALLMODES_REQ, /**< \brief  Requests the Basic Call Modes currently supported for outgoing calls.
					 * \details The Basic Call Mode must be used when placing an outgoing call request while the current Call Mode is set to "Single Mode".
					 * See CI_CC_PRIM_MAKE_CALL_REQ for more information.
					 *  See CI_CC_PRIM_SET_CALLMODE_REQ for default Call Mode information. */

	CI_CC_PRIM_GET_BASIC_CALLMODES_CNF, /**< \brief  Returns the Basic Call Modes currently supported for outgoing calls.
					 * \details See CI_CC_PRIM_SET_CALLMODE_REQ for default Call Mode information.
					 *  To place an outgoing call when the Call Mode is set to Single Mode, the Basic Call Mode must be specified in the outgoing call request.
					 * Use this request to return the supported Basic Call Modes (or Types).
					 *  There should be no reason for an unsuccessful result. */
	CI_CC_PRIM_GET_CALLOPTIONS_REQ, /**< \brief Requests the Call Options currently supported for outgoing calls.
				     * \details See CI_CC_PRIM_MAKE_CALL_REQ for more information.*/
	CI_CC_PRIM_GET_CALLOPTIONS_CNF, /**< \brief Returns the Call Options currently supported for outgoing calls.
				     * \details Returns the Call Options currently supported for outgoing calls.
				     *  There should be no reason for an unsuccessful result.*/
	CI_CC_PRIM_GET_DATACOMP_CAP_REQ, /**< \brief Gets V.42 bis data compression configuration capability.
				      * \details Mandatory if V.42 bis is supported.*/
	CI_CC_PRIM_GET_DATACOMP_CAP_CNF, /**< \brief Reports V.42 bis data compression configuration capability.
				      * \details Mandatory if V.42 bis is supported.*/
	CI_CC_PRIM_GET_DATACOMP_REQ,    /**< \brief Gets curret V.42 bis data compression information.
					 *  \details Mandatory if V.42 bis is supported.*/
	CI_CC_PRIM_GET_DATACOMP_CNF, /**< \brief Reports curret V.42 bis data compression information.
				    *  \details Mandatory if V.42 bis is supported.*/
	CI_CC_PRIM_SET_DATACOMP_REQ,    /**< \brief Configures V.42 bis data compression.
					 * \details Mandatory if V.42 bis is supported.*/
	CI_CC_PRIM_SET_DATACOMP_CNF, /**< \brief Confirms a request to configure V.42 bis data compression.
				     *  \details Mandatory if V.42 bis is supported.*/
	CI_CC_PRIM_GET_RLP_CAP_REQ, /**< \brief Gets RLP configuration capability for NT data calls.
				     * \details Mandatory if RLP is supported. */
	CI_CC_PRIM_GET_RLP_CAP_CNF, /**< \brief Reports RLP configuration capability for NT data calls.
				     * \details Mandatory if RLP is supported. */
	CI_CC_PRIM_GET_RLP_CFG_REQ, /**< \brief Gets the current RLP configuration for a RLP version.
				     * \details Mandatory if RLP is supported. */
	CI_CC_PRIM_GET_RLP_CFG_CNF, /**< \brief Reports the current RLP configuration for the requested version.
				     * \details Mandatory if RLP is supported. */
	CI_CC_PRIM_SET_RLP_CFG_REQ, /**< \brief Configures the RLP for NT data calls.
				     * \details Mandatory if RLP is supported. */
	CI_CC_PRIM_SET_RLP_CFG_CNF, /**< \brief Confirms a request to configure the RLP for NT data calls.
				     * \details Mandatory if RLP is supported. */
	CI_CC_PRIM_DATA_SERVICENEG_IND, /**< \brief Notification report of bearer service, during connect negotiation for data calls.
				     * \details No explicit response is required.
				     *   This Notification can be enabled or disabled by the CI_CC_PRIM_ENABLE_DATA_SERVICENEG_REQ request. It is disabled by
				     *   default. */
	CI_CC_PRIM_ENABLE_DATA_SERVICENEG_IND_REQ, /**< \brief Enable/disable bearer service reporting during connect negotiation for
						*  data calls. */
	CI_CC_PRIM_ENABLE_DATA_SERVICENEG_IND_CNF, /**< \brief Confirms a request to enable/disable bearer service reporting during
						*  connect negotiation for data calls.
						* \details Mandatory if RLP is supported. */
	CI_CC_PRIM_SET_UDUB_REQ, /**< \brief Sets User Determined User Busy (UDUB) for a Waiting or Incoming Call.
			      *  \details Use this request if the subscriber opts to refuse a Waiting or Incoming Call by setting a User Determined User Busy (UDUB) condition.
			      * This informs the Network that the call may be redirected to another number (if already set up).
			      *  The Network may clear the call with a Busy indication to the calling party.*/
	CI_CC_PRIM_SET_UDUB_CNF, /**< \brief Confirms a request to set UDUB for a waiting or Incoming Call.
			       *  \details */
	CI_CC_PRIM_GET_SUPPORTED_CALLMAN_OPS_REQ, /**< \brief Requests a list of supported Call Manipulation Operation Codes for
					       *   Supplementary Services within a call.
					       *  \details These operations are described in TS 22.030, Section 6.5.5.1, and are implemented
					       *   by the "AT+CHLD" command in TS 27.007 Section 7.13.*/
	CI_CC_PRIM_GET_SUPPORTED_CALLMAN_OPS_CNF, /**< \brief Returns a list of supported Call Manipulation Operation Codes for
						*  Supplementary Services within a call.
						*  \details */
	CI_CC_PRIM_MANIPULATE_CALLS_REQ, /**< \brief Call Manipulation request for Supplementary Services within a call.
				      *  \details This primitive performs the operations described in TS 22.030, Section 6.5.5.1,
				      * and are implemented by the "AT+CHLD" command in TS 27.007 Section 7.13.
				      *  The CallId parameter is used only for the CI_CC_MANOP_RLS_CALL and CI_CC_MANOP_HOLD_ALL_EXCEPT_ONE operations.*/
	CI_CC_PRIM_MANIPULATE_CALLS_CNF, /**< \brief Confirms a Call Manipulation request for Supplementary Services within a call.
					*  \details */
	CI_CC_PRIM_LIST_CURRENT_CALLS_IND, /**< \brief Presents an indication to list the current call information.
					  *  \details */

	CI_CC_PRIM_CALL_DIAGNOSTIC_IND, /** \brief< Request for the diagnostic octets of a specific call.
				  *  \details Diagnostic information is sent by the protocol stack in one of the following
				  * notifications: CcDisconnectInd, CcDisconnectedInd, CcDisconnectingInd, CcFailureInd.
				  * SAC processes these signals and sends a corresponding CI indication primitive
				  * (CI_CC_PRIM_DISCONNECT_IND, CI_CC_PRIM_MO_CALL_FAILED_IND, CI_CC_PRIM_MT_CALL_FAILED_IND,
				  *  CI_DAT_PRIM_NOK_IND) but it does not include the diagnostic info.
				  *
				  * SAC will send the CiCcCustPrimCallDiagnosticInd each time it receives any of the
				  * mentioned notifications from the protocol stack.
				  */
	CI_CC_PRIM_DTMF_EVENT_IND, /**< \brief  Notification of a DTMF event.
				  *  \details This notification is sent each time SAC receives a confirmation signal from the protocol
				  * stack indicating that a start/stop DTMF request has been successfully completed.
				  * In case of a sigle DTMF tone, the notification will be sent along with the CiCcPrimStartDtmfCnf
				  * and CiCcPrimStopDtmfCnf.
				  * In case of a DTMF tone sequence, the notification will be sent for each tone in the sequence
				  * when that tone is started/stopped.
				  * A DTMF aborted indication is sent when a Start or Stop DTMF request is rejected by SAC or the
				  * network for various reasons (see GSM TS 04.08 section 8.4 and section 5.5.7.2.); also if no
				  * answer is received from the network*/
	CI_CC_PRIM_CLEAR_BLACK_LIST_REQ, /**< \brief Request to clear the Call BlackList.
				      *  \details To clear the entire blacklist is not yet supported by the protocol stack.
				      * A Blacklist is created by the protocol stack with the numbers that
				      * are used to initiate MO calls and are marked as 'auto-dial' numbers.
				      *
				      * In that case SAC would have to create its own blacklist and save all
				      * the 'auto-dial' numbers that are reported by the protocol stack as 'blacklisted'.
				      * For clearing the entire list, it will send a separate 'clear blacklist'
				      * request for each dial number in the blacklist.
				      * NOTE: to support the BlackList functionality CI must add the capability of
				      *       specifying if a number is 'auto_dial', when an MO call is requested.*/
	CI_CC_PRIM_CLEAR_BLACK_LIST_CNF, /**< \brief returns the completion status of the request
				       *  \details */
	CI_CC_PRIM_SET_CTM_STATUS_REQ, /**< \brief Status of CTM state when CTM jackis connected and CTM is enabled in MENU.
				     *  \details */
	CI_CC_PRIM_SET_CTM_STATUS_CNF, /**< \brief Confirms status of CTM state when CTM jackis connected and CTM is enabled in MENU.
				    *  \details */
	CI_CC_PRIM_CTM_NEG_REPORT_IND, /**< \brief CTM negotiation status report
				     *  \details */
	CI_CC_PRIM_CDIP_INFO_IND, /*Michal Bukai - CDIP support */
	/**< \brief  Reports CDIP information (when enabled) for an incoming call.
	 *  \details CDIP indications may be enabled or disabled by the CI_SS_SET_CDIP_OPTION_REQ request.
	   This indication is enabled by default.
	   No explicit response is required.
	 */
#ifdef AT_CUSTOMER_HTC
	CI_CC_PRIM_SET_FW_LIST_REQ,  /**< \brief set MT call fire wall database.
							  *  \details */
	CI_CC_PRIM_SET_FW_LIST_CNF,  /**< \brief Confirms a setting MT call fire wall database request.
							  *  \details */
#endif
	/* ADD NEW COMMON PRIMITIVES HERE, BEFORE 'CI_CC_PRIM_LAST_COMMON_PRIM' */

	/* END OF COMMON PRIMITIVES LIST */
	CI_CC_PRIM_LAST_COMMON_PRIM

	/* the  customer specific extension primitives will be added starting from
	 * CI_CC_PRIM_FIRST_CUST_PRIM = CI_CC_PRIM_LAST_COMMON_PRIM as the first identifier.
	 * The actual primitive names and IDs are defined in the associated
	 * 'ci_cc_cust_xxx.h' file.
	 */

	/* DO NOT ADD ANY MORE PRIMITIVES HERE */

} _CiCcPrim;

/* specify the number of default common CC primitives */
#define CI_CC_NUM_COMMON_PRIM ( CI_CC_PRIM_LAST_COMMON_PRIM - 1 )
/**@}*/
/* the total number of primitives, CI_CC_NUM_PRIM, is calculated at the end
 * of this file based on existing customer extensions definitions, if any.
 */

/** \brief Call Control Return Codes. */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIRC_CC {
	CIRC_CC_SUCCESS = 0,                            /**< Request completed successfully         */
	CIRC_CC_FAIL,                                   /**< General Failure (catch-all)            */
	CIRC_CC_INCOMPLETE_INFO,                        /**< Incomplete information for request     */
	CIRC_CC_BAD_DIALSTRING,                         /**< Invalid character(s) in Dial String    */
	CIRC_CC_INVALID_ADDRESS,                        /**< Invalid Address (Phone Number)         */
	CIRC_CC_INVALID_CALLID,                         /**< Invalid Call Identifier                */
	CIRC_CC_INVALID_MPTYID,                         /**< Invalid MPTY Identifier                */
	CIRC_CC_NO_SERVICE,                             /**< No Network service                     */
	CIRC_CC_FDN_ONLY,                               /**< Only Fixed Dialing Numbers allowed     */
	CIRC_CC_EMERGENCY_ONLY,                         /**< Only Emergency Calls allowed           */
	CIRC_CC_CALL_BARRED,                            /**< Calls are barred                       */
	CIRC_CC_NO_MORE_CALLS,                          /**< No more calls allowed                  */
	CIRC_CC_NO_MORE_TIME,                           /**< No more Airtime left                   */
	CIRC_CC_NOT_PROVISIONED,                        /**< Service not provisioned                */
	CIRC_CC_CANNOT_SWITCH,                          /**< Call Mode cannot be switched           */
	CIRC_CC_SWITCH_FAILED,                          /**< Failed to switch Call Mode             */
	CIRC_CC_REJECTED,                               /**< Request rejected by Network            */
	CIRC_CC_TIMEOUT,                                /**< Request timed out                      */
	CIRC_CC_SIM_ACCESS_DENIED,                      /**< SIM access related error (CHV needed?) */

	/* << Define additional specific CC result codes here >> */

	/* This one must always be last in the list! */
	CIRC_CC_NUM_RESCODES            /**< Number of result codes defined */
} _CiCcResultCode;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**   \brief Result code
 *  \sa CIRC_CC
 * \remarks Common Data Section */
typedef UINT16 CiCcResultCode;

/**   \brief Call Identifier (Call ID)*/
/** \remarks Common Data Section */
typedef UINT16 CiCcCallId;

#define CICC_CALLID_MIN   0x0001        /* Minimum valid Call Identifier value */
#define CICC_CALLID_MAX   0xFFFF        /* Maximum valid Call Identifier value */
#define CICC_CALLID_NONE  0x0000        /* Indicates no Call Identifier assigned */

/**  \brief Multiparty Call (MPTY) Identifier */
/** \remarks Common Data Section */
typedef UINT16 CiCcMptyId;

#define CICC_MPTYID_MIN   0x0001        /* Minimum valid MPTY Identifier value */
#define CICC_MPTYID_MAX   0xFFFF        /* Maximum valid MPTY Identifier value */
#define CICC_MPTYID_NONE  0x0000        /* Indicates no MPTY Identifier assigned */
/**@}*/
/**  \brief Call Mode indicators. */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CICC_CMODE {
	CICC_CMODE_SINGLE = 0,          /* Single Mode (see below) */
	CICC_CMODE_ALT_VOICE_FAX,       /* Alternating Voice/Fax (Teleservice 61) */
	CICC_CMODE_ALT_VOICE_DATA,      /* Alternating Voice/Data (Bearer Svc 61) */
	CICC_CMODE_VOICE_THEN_DATA,     /* Voice followed by Data (Bearer Svc 81) */
	/* This one must always be last in the list! */
	CICC_NUM_CMODES                 /* Number of Call Modes defined */
} _CiCcCallMode;

typedef UINT8 CiCcCallMode;

/* Default (Reset) Call Mode */
#define CICC_CMODE_DEFAULT  CICC_CMODE_SINGLE

/** \brief Basic Call Mode indicators -- needed only if Call Mode is set for "Single Mode" (default) */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CICC_BASIC_CMODE {
	CICC_BASIC_CMODE_VOICE = 0,             /**< Basic Call Mode: Voice */
	CICC_BASIC_CMODE_FAX,                   /**< Basic Call Mode: Fax   */
	CICC_BASIC_CMODE_DATA,                  /**< Basic Call Mode: Data  */

	/* This one must always be last in the list! */
	CICC_NUM_BASIC_CMODES   /* Number of Basic Call Modes defined */
} _CiCcBasicCMode;
/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Basic call mode indicator.
 * \sa CICC_BASIC_CMODE */
/** \remarks Common Data Section */
typedef UINT8 CiCcBasicCMode;
/**@}*/

/** \brief Current Call Mode indicator values. */
//ICAT EXPORTED ENUM
typedef enum CICC_CURRENT_CMODE {
	CICC_CURRENT_CMODE_VOICE = 0,                                           /**< Voice only */
	CICC_CURRENT_CMODE_DATA,                                                /**< Data only */
	CICC_CURRENT_CMODE_FAX,                                                 /**< Fax only */
	CICC_CURRENT_CMODE_VOICE_FB_DATA_IS_VOICE,                              /**< Voice followed by Data, Voice Mode */
	CICC_CURRENT_CMODE_ALT_VOICE_DATA_IS_VOICE,                             /**< Alternating Voice/Data, Voice Mode */
	CICC_CURRENT_CMODE_ALT_VOICE_FAX_IS_VOICE,                              /**< Alternating Voice/Fax, Voice Mode  */
	CICC_CURRENT_CMODE_VOICE_FB_DATA_IS_DATA,                               /**< Voice followed by Data, Data Mode  */
	CICC_CURRENT_CMODE_ALT_VOICE_DATA_IS_DATA,                              /**< Alternating Voice/Data, Data Mode  */
	CICC_CURRENT_CMODE_ALT_VOICE_FAX_IS_FAX,                                /**< Alternating Voice/Fax, Fax Mode    */
	CICC_CURRENT_CMODE_UNKNOWN,                                             /**< Unknown Call Mode */

	/* This one must always be last in the list! */
	CICC_NUM_CURRENT_CMODES                                                 /**< Number of Current Call Modes defined */
} _CiCcCurrentCMode;
/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Call Mode indicator.
 * \sa CICC_CURRENT_CMODE */
/** \remarks Common Data Section */
typedef UINT8 CiCcCurrentCMode;
/**@}*/


/**  \brief Current Call State indicator values */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CICC_CURRENT_CSTATE {
	CICC_CURRENT_CSTATE_ACTIVE = 0,                 /**< Call is Active     */
	CICC_CURRENT_CSTATE_HELD,                       /**< Call is Held       */
	CICC_CURRENT_CSTATE_DIALING,                    /**< Dialing (MO Call)  */
	CICC_CURRENT_CSTATE_ALERTING,                   /**< Alerting (MO Call) */
	CICC_CURRENT_CSTATE_INCOMING,                   /**< Incoming MT Call   */
	CICC_CURRENT_CSTATE_WAITING,                    /**< MT Call Waiting    */
	CICC_CURRENT_CSTATE_OFFERING,                   /**< MT Call Offering (Call Setup)  */

	/* This one must always be last in the list! */
	CICC_NUM_CURRENT_CSTATES    /* Number of Current Call States defined  */
} _CiCcCurrentCState;
/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Call State indicator.
 * \sa CICC_CURRENT_CSTATE */
/** \remarks Common Data Section */
typedef UINT8 CiCcCurrentCState;
/**@}*/



/**  \brief Call Direction values
 */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CICC_CALL_DIRECTION {
	CICC_MO_CALL = 0,       /**< Mobile Originated Call */
	CICC_MT_CALL            /**< Mobile Terminated Call */
} _CiCcCallDirection;
/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Call Direction indicator.
 * \sa CICC_CALL_DIRECTION */
/** \remarks Common Data Section */
typedef UINT8 CiCcCallDirection;
/**@}*/


/**  \brief Call Type indicator values */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CICC_CALLTYPE {

	/* Single Modes */
	CICC_CALLTYPE_ASYNC = 0,                                        /**< Asynchronous Transparent Data                */
	CICC_CALLTYPE_SYNC,                                             /**< Synchronous Transparent Data                 */
	CICC_CALLTYPE_REL_ASYNC,                                        /**< Asynchronous Non-Transparent Data            */
	CICC_CALLTYPE_REL_SYNC,                                         /**< Synchronous Non-Transparent Data             */
	CICC_CALLTYPE_FAX,                                              /**< Facsimile                                    */
	CICC_CALLTYPE_VOICE,                                            /**< Voice                                        */

	/* Voice Mode followed by Data Mode (Bearer Service 81) */
	CICC_CALLTYPE_VOICE_THEN_ASYNC,                 /**< VOICE followed by ASYNC                      */
	CICC_CALLTYPE_VOICE_THEN_SYNC,                  /**< VOICE followed by SYNC                       */
	CICC_CALLTYPE_VOICE_THEN_REL_ASYNC,             /**< VOICE followed by REL_ASYNC                  */
	CICC_CALLTYPE_VOICE_THEN_REL_SYNC,              /**< VOICE followed by REL_SYNC                   */

	/* Alternating Voice/Data Mode, Voice Mode first (Bearer Service 61)                */
	CICC_CALLTYPE_ALT_VOICE_ASYNC,                  /**< Alternating VOICE/ASYNC, VOICE first         */
	CICC_CALLTYPE_ALT_VOICE_SYNC,                   /**< Alternating VOICE/SYNC, VOICE first          */
	CICC_CALLTYPE_ALT_VOICE_REL_ASYNC,              /**< Alternating VOICE/REL_ASYNC, VOICE first     */
	CICC_CALLTYPE_ALT_VOICE_REL_SYNC,               /**< Alternating VOICE/REL_SYNC, VOICE first      */

	/* Alternating Voice/Data Mode, Data Mode first (Bearer Service 61) */
	CICC_CALLTYPE_ALT_ASYNC_VOICE,                  /**< Alternating ASYNC/VOICE, ASYNC first         */
	CICC_CALLTYPE_ALT_SYNC_VOICE,                   /**< Alternating SYNC/VOICE, SYNC first           */
	CICC_CALLTYPE_ALT_REL_ASYNC_VOICE,              /**< Alternating REL_ASYNC/VOICE, REL_ASYNC first */
	CICC_CALLTYPE_ALT_REL_SYNC_VOICE,               /**< Alternating REL_SYNC/VOICE, REL_SYNC first   */

	/* Alternating Voice/Facsimile Modes (Teleservice 61) */
	CICC_CALLTYPE_ALT_VOICE_FAX,                    /**< Alternating VOICE/FAX, VOICE first           */
	CICC_CALLTYPE_ALT_FAX_VOICE,                    /**< Alternating VOICE/FAX, FAX first             */

	/* This one must always be last in the list! */
	CICC_NUM_CALLTYPES                              /**< Number of Call Types defined */
} _CiCcCallType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Call Type indicators
 * \sa CICC_CALLTYPE */
/** \remarks Common Data Section */
typedef UINT8 CiCcCallType;
/**@}*/


/**  \brief Data Bearer Service Type: Bearer Service "Name" indicator values */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CICC_BSTYPE_NAME {

	/* UDI or 3.1 kHz Modem definitions */
	CICC_BSTYPE_NAME_DATA_ASYNC_UDI = 0,            /**< Data Circuit Asynchronous (UDI, 3.1kHz)  */
	CICC_BSTYPE_NAME_DATA_SYNC_UDI,                 /**< Data Circuit Synchronous (UDI, 3.1 kHz)  */
	CICC_BSTYPE_NAME_PAD_ASYNC_UDI,                 /**< PAD Access Asynchronous (UDI)            */
	CICC_BSTYPE_NAME_PACKET_SYNC_UDI,               /**< Packet Access Synchronous (UDI)          */

	/* RDI definitions */
	CICC_BSTYPE_NAME_DATA_ASYNC_RDI = 0,    /**< Data Circuit Asynchronous (RDI)          */
	CICC_BSTYPE_NAME_DATA_SYNC_RDI,         /**< Data Circuit Synchronous (RDI)           */
	CICC_BSTYPE_NAME_PAD_ASYNC_RDI,         /**< PAD Access Asynchronous (RDI)            */
	CICC_BSTYPE_NAME_PACKET_SYNC_RDI,       /**< Packet Access Synchronous (RDI)          */

	/* This one must always be last in the list! */
	CICC_NUM_BSTYPE_NAMES                           /**< Number of Bearer Service Names defined */
} _CiCcBsTypeName;
/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Bearer Service "Name" indicators
 * \sa CICC_BSTYPE_NAME */
/** \remarks Common Data Section */
typedef UINT8 CiCcBsTypeName;
/**@}*/


/** \brief Data Bearer Service Type: Quality of Service Attribute or Connection Element indicator values */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CICC_BSTYPE_CE {
	CICC_BSTYPE_CE_TRANSPARENT_ONLY = 0,    /**< Transparent required               */
	CICC_BSTYPE_CE_NONTRANSPARENT_ONLY,     /**< Non-transparent required           */
	CICC_BSTYPE_CE_PREFER_TRANSPARENT,      /**< Either, Transparent preferred      */
	CICC_BSTYPE_CE_PREFER_NONTRANSPARENT,   /**< Either, Non-transparent preferred  */

	/* This one must always be last in the list! */
	CICC_NUM_BSTYPE_CE                                              /**< Number of CE Indicators defined */
} _CiCcBsTypeCe;
/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Quality of Service Attribute or Connection Element indicators
 * \sa CICC_BSTYPE_CE */
/** \remarks Common Data Section */
typedef UINT8 CiCcBsTypeCe;
/**@}*/


/** \brief Data Bearer Service Type Information */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiCcDataBsTypeInfo_struct {
	CiBsTypeSpeed Speed;                    /**< Data Speed indicator. \sa CCI API Ref Manual */
	CiCcBsTypeName Name;                    /**< Bearer Service Name indicator. \sa CiCcBsTypeName  */
	CiCcBsTypeCe Ce;                        /**< Connection Element. \sa CiCcBsTypeCe      */
} CiCcDataBsTypeInfo;

/** \brief  Supported Data Bearer Service Type Parameter Settings */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiCcSuppDataBsTypes_struct {
	CiNumericRange bsTypeSpeedsRange;       /**< Supported Data Speeds. \sa CCI API Ref Manual  */
	CiNumericRange bsTypeNamesRange;        /**< Supported Bearer Service Names. \sa CCI API Ref Manual */
	CiNumericRange bsTypeCeRange;           /**< Supported Connection Elements. \sa CCI API Ref Manual  */
} CiCcSuppDataBsTypes;

/** \brief  Call Information structure.
 */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiCcCallInfo_struct {
	CiCcCallId CallId;                                      /**< Unique Call Identifier. \sa CiCcCallId  */
	CiBoolean IsEmergency;                                  /**< Is this an Emergency Call?  \sa CCI API Ref Manual         */
	CiBoolean IsMPTY;                                       /**< Is this part of a Multiparty Call? \sa CCI API Ref Manual  */
	CiCcCallDirection Direction;                            /**< Call Direction (MT or MO) Indicator. \sa CiCcCallDirection  */
	CiCcCurrentCState State;                                /**< Current Call State. \sa CiCcCurrentCState                 */
	CiCcCurrentCMode Mode;                                  /**< Current Call Mode. \sa CiCcCurrentCMode                 */
#ifndef CCI_CC_CONTIGUOUS
	CiCallerInfo        *CallerInfo;                        /**< Caller Information. \sa CCI API Ref Manual  */
	CiCcDataBsTypeInfo  *DataSvcInfo;                       /**< Service Information (for Data Calls). \sa CiCcDataBsTypeInfo_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *          tempNoncontPtr1;               /**< remove when both COMM and APP side use contiguous flags. */
	UINT8 *          tempNoncontPtr2;               /**< remove when both COMM and APP side use contiguous flags. */
#endif
	CiCallerInfo callerInfo;                        /**<  Caller Information. \sa CCI API Ref Manual */
	CiCcDataBsTypeInfo dataSvcInfo;                 /**< Service Information (for Data Calls). \sa CiCcDataBsTypeInfo_struct   */
#endif

	CiBoolean IsAutoDial;                   /**<  Is this an Auto Dial Call? \sa CCI API Ref Manual  */
} CiCcCallInfo;

/** \brief  MakeCall (Dial) Options (bitmap assignments) - for individual outgoing call requests */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CICC_CALLOPTIONS {
	CICC_CALLOPTIONS_NONE           = 0x00,         /**< CLIR as provisioned; no CUG info         */
	CICC_CALLOPTIONS_CLIR_ALLOW     = 0x01,         /**< Allow CLI presentation on this call     */
	CICC_CALLOPTIONS_CLIR_RESTRICT  = 0x02,         /**< Restrict CLI presentation on this call  */
	CICC_CALLOPTIONS_CUG_ENABLE     = 0x04          /**< Enable CUG information on this call      */
} _CiCcCallOptions;
/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief MakeCall (Dial) Options bitmap.
 * \sa CICC_CALLOPTIONS */
/** \remarks Common Data Section */
typedef UINT8 CiCcCallOptions;
/**@}*/





/** \brief   Outgoing (Make) Call Information.
 */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiCcMakeCallInfo_struct {
	CiCcBasicCMode BasicCMode;                                      /**< Basic Call Mode (if applicable). \sa CiCcBasicCMode */
	CiCcCallOptions Options;                                        /**< Call Options bitmap. \sa CiCcCallOptions    */
#ifndef CCI_CC_CONTIGUOUS
	CHAR            *DialString;                                    /**< Pointer to null-terminated Dial String */
#else
	CHAR dialString[CI_MAX_ADDRESS_LENGTH];
#endif

	CiBoolean IsAutoDial;                                           /**< Is this an Auto Dial Call? \sa CCI API Ref Manual   */
	CiBoolean IsCtmCall;                                            /**< IS this call is CTM TTY MO Call? \sa CCI API Ref Manual */

} CiCcMakeCallInfo;

/* Auto-Answer Timer Interval */
#define CICC_AUTOANSWER_INTERVAL  10  /* Interval is in seconds */

/** \brief  Status or Failure Cause codes. \details Derived from TS 24.008, Table 10.5.123 */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CICC_CAUSE {
	CICC_CAUSE_UNKNOWN = 0,                                         /**< Unknown cause (includes "None")  */
	CICC_CAUSE_UNASSIGNED_NUMBER,                                   /**< Unassigned (unallocated) number  */
	CICC_CAUSE_NO_ROUTE_TO_DEST,                                    /**< No route to Destination  */
	CICC_CAUSE_CHAN_UNACCEPTABLE,                                   /**<* Channel unacceptable */
	CICC_CAUSE_OPERATOR_BARRING,                                    /**< Operator determined barring  */
	CICC_CAUSE_NORMAL_CLEARING,                                     /**< Normal call clearing */
	CICC_CAUSE_USER_BUSY,                                           /**< User busy  */
	CICC_CAUSE_NO_USER_RESPONSE,                                    /**< No user responding */
	CICC_CAUSE_ALERT_NO_ANSWER,                                     /**< User alerting, no answer */
	CICC_CAUSE_CALL_REJECTED,                                       /**< Call rejected  */
	CICC_CAUSE_NUMBER_CHANGED,                                      /**< Number changed */
	CICC_CAUSE_PREEMPTION,                                          /**< Pre-emption  */
	CICC_CAUSE_NONSELECTED_USER_CLEAR,                              /**< Non selected user clearing */
	CICC_CAUSE_DEST_OUT_OF_ORDER,                                   /**< Destination out of order */
	CICC_CAUSE_INVALID_NUMFORMAT,                                   /**< Invalid number format (incomplete) */
	CICC_CAUSE_FACILITY_REJECT,                                     /**< Facility rejected  */
	CICC_CAUSE_STATUSENQ_RESPONSE,                                  /**< Response to STATUS ENQUIRY */
	CICC_CAUSE_NORMAL_UNSPECIFIED,                                  /**< Normal, unspecified  */
	CICC_CAUSE_NO_CCT_AVAILABLE,                                    /**< No circuit/channel available */
	CICC_CAUSE_NETWORK_OUT_OF_ORDER,                                /**< Network out of order */
	CICC_CAUSE_TEMP_FAILURE,                                        /**< Temporary failure  */
	CICC_CAUSE_CONGESTION,                                          /**< Switching equipment congestion */
	CICC_CAUSE_ACCESSINFO_DISCARDED,                                /**< Access information discarded */
	CICC_CAUSE_CIRCUIT_UNAVAILABLE,                                 /**< Requested circuit/channel unavailable  */
	CICC_CAUSE_RESOURCES_UNAVAILABLE,                               /**< Resources unavailable, unspecified */
	CICC_CAUSE_QOS_UNAVAIL,                                         /**< Quality of Service (QoS) unavailable */
	CICC_CAUSE_FACILITY_NOTSUBSCRIBED,                              /**< Requested facility not subscribed  */
	CICC_CAUSE_MT_CALLBARRING_IN_CUG,                               /**< Incoming (MT) calls barred within CUG  */
	CICC_CAUSE_BEARERCAP_NOTAUTHORIZED,                             /**< Bearer capability not authorized */
	CICC_CAUSE_BEARERCAP_UNAVAILABLE,                               /**< Bearer capability not available  */
	CICC_CAUSE_SVC_UNAVAILABLE,                                     /**< Service or option not available  */
	CICC_CAUSE_BEARERSVC_NOT_IMPLEMENTED,                           /**< Bearer service not implemented */
	CICC_CAUSE_ACMMAX_REACHED,                                      /**< ACM equal to, or greater than, ACMmax  */
	CICC_CAUSE_FACILITY_NOT_IMPLEMENTED,                            /**< Requested facility not implemented */
	CICC_CAUSE_BEARERCAP_RDI_ONLY,                                  /**< Only RDI bearer capability is available  */
	CICC_CAUSE_SVC_NOT_IMPLEMENTED,                                 /**< Service or option not implemented  */
	CICC_CAUSE_INVALID_TRANSACTID,                                  /**< Invalid transaction ID value */
	CICC_CAUSE_NOT_CUG_MEMBER,                                      /**< User not member of CUG */
	CICC_CAUSE_DEST_INCOMPATIBLE,                                   /**< Incompatible destination */
	CICC_CAUSE_INCORRECT_MESSAGE,                                   /**< Semantically incorrect message */
	CICC_CAUSE_TRANSIT_NETWORK_INVALID,                             /**< Invalid transit network selection  */
	CICC_CAUSE_NO_SUCH_MSGTYPE,                                     /**< Message type non-existent or not */
	/*   implemented  */
	CICC_CAUSE_MSGTYPE_WRONG_STATE,                                 /**< Message type incompatible with current */
	/*   protocol state */
	CICC_CAUSE_NO_SUCH_IE,                                          /**< Information element non-existent or not  */
	/*   implemented  */
	CICC_CAUSE_CONDITIONAL_IE_ERROR,                                /**< Conditional IE error */
	CICC_CAUSE_MSG_WRONG_STATE,                                     /**< Message incompatible with current  */
	/*   protocol state */
	CICC_CAUSE_RECOVERY_AFTER_TIMEOUT,                              /**< Recovery after timer expiry  */
	CICC_CAUSE_PROTOCOL_ERROR,                                      /**< Protocol error, unspecified  */
	CICC_CAUSE_INTERWORKING,                                        /**< Interworking, unspecified  */

    CICC_CAUSE_ABNORMAL,              			/**< Abnormal release  *//*Added for CQ8292*/
    CICC_CAUSE_ERROR_REESTABLISHMENT_BARRED,       /**< Reestablishment barred  */
    CICC_CAUSE_CELL_SELECTION_IN_PROGRESS,         /**< Cell seection in progress  */
    CICC_CAUSE_LOWER_LAYER_FAILURE,                /**< Lower layer failure  */
    CICC_CAUSE_RACH_FAIL,                          /**< Rach fail  */
    
    CICC_CAUSE_FDN_BLOCKED,                        /**< FDN Mismatch  */
    CICC_CAUSE_ACCESS_CLASS_BARRED,                /**< Cell barred  */

	/* This one must always be last in the list! */
	CICC_NUM_CAUSES                                                 /**< Number of Cause Codes defined  */
} _CiCcCause;
/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Status or Failure Cause codes.
 * \sa CICC_CAUSE */
/** \remarks Common Data Section */
typedef UINT8 CiCcCause;
/**@}*/
/** \brief  Extended Call Type Information */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiCcExtCallType_struct {
	CiCcCallType CallType;                          /**< Call Type indicator. \sa CiCcCallType   */
#ifndef CCI_CC_CONTIGUOUS
	CiSubaddrInfo *Subaddress;                      /**< Optional Subaddress information. \sa CCI API Ref Manual */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *          tempNoncontPtr;        /**<  remove when both COMM and APP side use contiguous flags */
#endif
	CiSubaddrInfo subaddress;
#endif

} CiCcExtCallType;

/* Call Deflection (CD) Address Information */
//ICAT EXPORTED STRUCT
typedef struct CiCcCdAddressInfo_struct {
	CiBoolean Present;              /* Is Call Deflection info present? */
#ifndef CCI_CC_CONTIGUOUS
	CiAddressInfo *Number;          /* Call Deflection Number           */
	CiSubaddrInfo *Subaddress;      /* Optional Subaddress information  */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *          tempNoncontPtr1;       /* remove when both COMM and APP side use contiguous flags */
	UINT8 *          tempNoncontPtr2;       /* remove when both COMM and APP side use contiguous flags */
#endif
	CiAddressInfo number;
	CiSubaddrInfo subaddress;
#endif

} CiCcCdAddressInfo;

/** \brief  DTMF Pacing: Configuration Information */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiCcDtmfPacing_struct {
	UINT16 Duration;                /**< DTMF Tone Duration (ms)    */
	UINT16 Interval;                /**< Inter-Digit Interval (ms)  */
} CiCcDtmfPacing;

/* DTMF Pacing: Minimum Values - also used as defaults */
#define CICC_MIN_DTMF_DURATION  300     /* Minimum DTMF Tone Duration (ms)        */
#define CICC_MIN_DTMF_INTERVAL  65      /* Minimum DTMF Inter-Digit Interval (ms) */

/** \brief  CLI Validity Indicator values.
 * \details Used in CLIP, CoLP and Call Waiting indications */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CICC_CLI_VALIDITY {
	CLI_VALIDITY_VALID = 0,         /**< CLI Info is valid            */
	CLI_VALIDITY_WITHHELD,          /**< CLI Info withheld by Caller  */
	CLI_VALIDITY_UNAVAILABLE        /**< CLI Info is unavailable      */
} _CiCcCliValidity;
/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief CLI Validity Indicator.
 * \sa CICC_CLI_VALIDITY */
/** \remarks Common Data Section */
typedef UINT8 CiCcCliValidity;
/**@}*/


/** \brief  Information for CLIP indications */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiCcClipInfo_struct {
	CiCcCliValidity CliValidity;            /**< CLI Validity indicator. \sa CiCcCliValidity             */
#ifndef CCI_CC_CONTIGUOUS
	CiCallerInfo    *CallerInfo;            /**< Calling Party (Caller) Information. \sa CCI API Ref Manual  */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *          tempNoncontPtr; /**< remove when both COMM and APP side use contiguous flags */
#endif
	CiCallerInfo callerInfo;
#endif

} CiCcClipInfo;

/*Michal Bukai - CDIP support - Start:*/
/** \brief  Information for CDIP indications */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiCcCdipInfo_struct {
	CiCcCliValidity CliValidity;    /**< CLI Validity indicator. \sa CiCcCliValidity */
#ifndef CCI_CC_CONTIGUOUS
	CiCallerInfo    *CallerInfo;    /**< Called line Information. \sa CCI API Ref Manual */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *          tempNoncontPtr; /**< remove when both COMM and APP side use contiguous flags */
#endif
	CiCallerInfo callerInfo;
#endif

} CiCcCdipInfo;
/*SCR #1203743*/
/** \brief   Information for CoLP indications */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiCcColpInfo_struct {

	CiCcCliValidity CliValidity;            /**< CLI Validity indicator. \sa CiCcCliValidity */
#ifndef CCI_CC_CONTIGUOUS
	CiCallerInfo    *CallerInfo;            /**< Connected Party Information. \sa CCI API Ref Manual */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *          tempNoncontPtr; /**< remove when both COMM and APP side use contiguous flags */
#endif
	CiCallerInfo callerInfo;
#endif

} CiCcColpInfo;

/** \brief  Information for Call Waiting (CW) indications */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiCcCwInfo_struct {
	CiCcCliValidity CliValidity;            /**< CLI Validity Indicator. \sa CiCcCliValidity   */
#ifndef CCI_CC_CONTIGUOUS
	CiCallerInfo    *CallerInfo;            /**< Calling Party (Caller) Information. \sa CCI API Ref Manual */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *          tempNoncontPtr;        /**< Remove when both COMM and APP side use contiguous flags */
#endif
	CiCallerInfo callerInfo;                /**< Calling Party (Caller) Information. \sa CCI API Ref Manual */
#endif

} CiCcCwInfo;

/* PUCT Information */
#define CICC_MAX_CURR_LENGTH  3 /* Maximum length of Currenct Code string */

/** \brief  PUCT information */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiCcPuctInfo_struct {
	CHAR curr[ CICC_MAX_CURR_LENGTH ];                                      /**< Currency string              */
	UINT16 eppu;                                                            /**< Extended Price per Unit      */
	CiBoolean negExp;                                                       /**< TRUE if Exponent is negative. \sa CCI API Ref Manual */
	UINT8 exp;                                                              /**< Modulus of Exponent          */
} CiCcPuctInfo;

/** \brief  Data compression direction values */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CICCDATACOMPDIR {
	CI_CC_DATACOMP_DIR_NONE = 0,    /**< negotiated, no compression (V.42 bis P0=0)                 */
	CI_CC_DATACOMP_DIR_TX,          /**< Transmit only                                                              */
	CI_CC_DATACOMP_DIR_RX,          /**< Receive only                                                               */
	CI_CC_DATACOMP_DIR_BOTH,        /**< Both directions, accept any direction (V.42 bis P0=11)     */

	/* This one must always be last in the list! */
	CI_CC_DATACOMPS_NUM_DIRS
} _CiCcDataCompDir;
/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Data compression direction
 * \sa CICCDATACOMPDIR */
/** \remarks Common Data Section */
typedef UINT8 CiCcDataCompDir;
/**@}*/



/** \brief  Data compression info structure  */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiCcDataCompInfo_struct {
	CiCcDataCompDir dir;                            /**< data compression direction, default: CI_CC_DATACOMP_BOTH. \sa CiCcDataCompDir */
	CiBoolean zNegRequired;                         /**< compression negotiation is required or not, default: FALSE. \sa CCI API Ref Manual */
	UINT16 maxDict;                                 /**< max number of dictionary entries to be negotiated, [512-65535]         */
	UINT8 maxStrLen;                                /**< max string length to be negotiated (V.42 bis P2), [6-250], default: 6  */
} CiCcDataCompInfo;

/** \brief  Data compression capability */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiCcDataCompCap_struct {
	CiBitRange bitsDir;                                             /**< supported direction values. \sa CCI API Ref Manual  */
	CiBitRange bitsNegComp;                                         /**< supported negotiation values. \sa CCI API Ref Manual  */
	CiNumericRange maxDictRange;                                    /**< range of supported max number of dictionary entries. \sa CCI API Ref Manual  */
	CiNumericRange maxStrLenRange;                                  /**< range of supported max string length to be negotiated. \sa CCI API Ref Manual  */
} CiCcDataCompCap;

/** \brief  RLP config structure.\details For valid range, default and recommended value for each parameter refer to
 * 3GPP TS 24.022, 3.4.0, 5.5.
 */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiCcRlpCfg_struct {
	UINT8 winIWS;                           /**< IWF-to-MS window size                                                      */
	UINT8 winMWS;                           /**< MS-to-IWF window size                                                      */
	UINT8 ackTimer;                         /**< acknowledgement timer (T1), units of 10 ms                         */
	UINT8 reTxAttempts;                     /**< retransmission attempts (N2)                                               */
	UINT8 ver;                              /**< RLP version number, [0-2], default: 0, recommend: 2        */
	UINT8 reSeqPeriod;                      /**< resequencing period (T4), units of 10 ms                           */
	UINT8 initialT1;                        /**< Acknowledgement Timer(T1), units of 10 ms                          */
	UINT8 initialN2;                        /**< Retransmission attempts (N2), units of 10 ms                       */
} CiCcRlpCfg;

/** \brief  RLP capability */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiCcRlpCap_struct {
	CiNumericRange winIWSRange;                             /**< range of supported IWF-to-MS window size. \sa CCI API Ref Manual. */
	CiNumericRange winMWSRange;                             /**< range of supported MS-to-IWF window size. \sa CCI API Ref Manual. */
	CiNumericRange ackTimerRange;                           /**< range of supported acknowledgement timer. \sa CCI API Ref Manual.  */
	CiNumericRange reTxAttemptsRange;                       /**< range of supported retransmission attempts. \sa CCI API Ref Manual. */
	CiBitRange bitsVer;                                     /**< supported RLP version. \sa CCI API Ref Manual.                                     */
	CiNumericRange reSeqPeriodRange;                        /**< range of supported resequencing period. \sa CCI API Ref Manual.    */
} CiCcRlpCap;

/**  \brief Call Manipulate Operation Codes.
 *       \details See TS 22.030 Section 6.5.5.1 and TS 27.007 Sections 7.13 (AT+CHLD) and 7.14 (AT+CTFR).
 *    Note that where both a Held Call and a Waiting Call exist, the Waiting Call will always
 *    take precedence for the Call Manipulate procedure.
 */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CICCCALLMANOP {

	CI_CC_MANOP_RLS_HELD_OR_UDUB = 0,                       /**< "AT+CHLD=0" - Release all held calls or set User Determined User Busy (UDUB) for a waiting call  */
	CI_CC_MANOP_RLS_ACT_ACCEPT_OTHER,                       /**< "AT+CHLD=1" - Release all active calls (if any exist) and accept the other (held or waiting) call*/
	CI_CC_MANOP_RLS_CALL,                                   /**< "AT+CHLD=1X" - Release specific active call*/
	CI_CC_MANOP_HOLD_ACT_ACCEPT_OTHER,                      /**< "AT+CHLD=2" */
	CI_CC_MANOP_HOLD_ALL_EXCEPT_ONE,                        /**< "AT+CHLD=2X" - Place all active calls (if any exist) on hold and accept other (held or waiting) call*/
	CI_CC_MANOP_ADD_HELD_TO_MPTY,                           /**< "AT+CHLD=3" - Add a held call to the conversation (Multiparty)*/
	CI_CC_MANOP_ECT,                                        /**< "AT+CHLD=4" - Connects the two calls and disconnects the subscriber from both calls (ECT)*/
	CI_CC_MANOP_CALL_REDIRECT,                              /**< "AT+CTFR" ("4*<number><SEND>") - Redirect an incoming or a waiting call to the specified followed by SEND directory number*/
	CI_CC_MANOP_CCBS,                                       /**< "AT+CHLD=5" - Activates the Completion of Calls to Busy Subscriber Request*/
	CI_CC_MANOP_NUM_OPS

} _CiCcCallManOp;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Call Manipulate Operation Code.
 * \sa CICCCALLMANOP */
/** \remarks Common Data Section */
typedef UINT8 CiCcCallManOp;
/**@}*/

/* SCR #123023 -- BEGIN */

/* Parameter definitions for CI "Release All Calls" request */
//ICAT EXPORTED ENUM
typedef enum CICC_RELEASE_WHICHCALLS
{
	CICC_WHICHCALLS_ALL_ACTIVE = 0,
	CICC_WHICHCALLS_ALL_HELD,
	CICC_WHICHCALLS_ALL_CALLS

} _CiCcReleaseWhichCalls;

typedef UINT8 CiCcReleaseWhichCalls;

/* SCR #123023 -- END */

/* -------------------------- CC Service Group Primitives ------------------------------ */
/** <paramref name="CI_CC_PRIM_GET_NUMBERTYPE_REQ">   */
typedef CiEmptyPrim CiCcPrimGetNumberTypeReq;   /**< No paramters */

//ICAT EXPORTED STRUCT
/** <paramref name="CI_CC_PRIM_GET_NUMBERTYPE_CNF">   */
typedef struct CiCcPrimGetNumberTypeCnf_struct {
	CiCcResultCode Result;  /**< Result code. \sa CiCcResultCode */
#ifndef CCI_CC_CONTIGUOUS
	CiAddressType *NumType; /**< Type of Number (Address Type). \sa CCI API Ref Manual */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *          tempNoncontPtr; /* remove when both COMM and APP side use contiguous flags */
#endif
	CiAddressType numType;
#endif

} CiCcPrimGetNumberTypeCnf;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_CC_PRIM_SET_NUMBERTYPE_REQ">   */
typedef struct CiCcPrimSetNumberTypeReq_struct {
#ifndef CCI_CC_CONTIGUOUS
	CiAddressType *NumType; /**< Type of Number (Address Type). \sa CCI API Ref Manual  */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *          tempNoncontPtr; /* remove when both COMM and APP side use contiguous flags */
#endif
	CiAddressType numType;
#endif

} CiCcPrimSetNumberTypeReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_CC_PRIM_SET_NUMBERTYPE_CNF">   */
typedef struct CiCcPrimSetNumberTypeCnf_struct {
	CiCcResultCode Result;  /**< Result code. \sa CiCcResultCode */
} CiCcPrimSetNumberTypeCnf;
/** <paramref name="CI_CC_PRIM_GET_SUPPORTED_CALLMODES_REQ">   */
typedef CiEmptyPrim CiCcPrimGetSupportedCallModesReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_CC_PRIM_GET_SUPPORTED_CALLMODES_CNF">   */
typedef struct CiCcPrimGetSupportedCallModesCnf_struct {
	CiCcResultCode Result;                  /**< Result code. \sa CiCcResultCode */
	UINT8 NumModes;                         /**< Number of supported Call Modes. */
	CiCcCallMode Modes[ CICC_NUM_CMODES ];  /**< Supported Call Modes. \sa CICC_CMODE */
} CiCcPrimGetSupportedCallModesCnf;
/** <paramref name="CI_CC_PRIM_GET_CALLMODE_REQ">   */
typedef CiEmptyPrim CiCcPrimGetCallModeReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_CC_PRIM_GET_CALLMODE_CNF">   */
typedef struct CiCcPrimGetCallModeCnf_struct {
	CiCcResultCode Result;  /**< Result code. \sa CiCcResultCode */
	CiCcCallMode Mode;      /**< Current Call Mode. \sa CICC_CMODE */
} CiCcPrimGetCallModeCnf;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_CC_PRIM_SET_CALLMODE_REQ">   */
typedef struct CiCcPrimSetCallModeReq_struct {
	CiCcCallMode Mode;      /**< Selected Call Mode. \sa CICC_CMODE */
} CiCcPrimSetCallModeReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_CC_PRIM_SET_CALLMODE_CNF">   */
typedef struct CiCcPrimSetCallModeCnf_struct {
	CiCcResultCode Result;  /**< Result code. \sa CiCcResultCode */
} CiCcPrimSetCallModeCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_GET_SUPPORTED_DATA_BSTYPES_REQ"> */
typedef CiEmptyPrim CiCcPrimGetSupportedDataBsTypesReq;

/* <INUSE> */

//ICAT EXPORTED STRUCT
/** <paramref name="CI_CC_PRIM_GET_SUPPORTED_DATA_BSTYPES_CNF">   */
typedef struct CiCcPrimGetSupportedDataBsTypesCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
#ifndef CCI_CC_CONTIGUOUS
	CiCcSuppDataBsTypes *Types;     /**< Supported Data Bearer Service Type parameter settings. \sa CiCcSuppDataBsTypes_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *          tempNoncontPtr; /**< Remove when both COMM and APP side use contiguous flags */
#endif
	CiCcSuppDataBsTypes types;
#endif

} CiCcPrimGetSupportedDataBsTypesCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_GET_DATA_BSTYPE_REQ">   */
typedef CiEmptyPrim CiCcPrimGetDataBsTypeReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_GET_DATA_BSTYPE_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimGetDataBsTypeCnf_struct {
	CiCcResultCode Result;                          /**< Result code. \sa CiCcResultCode */
#ifndef CCI_CC_CONTIGUOUS
	CiCcDataBsTypeInfo  *Info;                      /**< Current Data Bearer Service Type Information. \sa CiCcDataBsTypeInfo_struct */

#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *          tempNoncontPtr;                /**< Remove when both COMM and APP side use contiguous flags */
#endif
	CiCcDataBsTypeInfo info;                        /**< Current Data Bearer Service Type Information. \sa CiCcDataBsTypeInfo_struct */
#endif

} CiCcPrimGetDataBsTypeCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_SET_DATA_BSTYPE_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimSetDataBsTypeReq_struct {
#ifndef CCI_CC_CONTIGUOUS
	CiCcDataBsTypeInfo  *Info;     /**< Selected Data Bearer Service Type Information. \sa CiCcDataBsTypeInfo_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *          tempNoncontPtr; /**< remove when both COMM and APP side use contiguous flags */
#endif
	CiCcDataBsTypeInfo info;
#endif

} CiCcPrimSetDataBsTypeReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_SET_DATA_BSTYPE_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimSetDataBsTypeCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
} CiCcPrimSetDataBsTypeCnf;

typedef CiEmptyPrim CiCcPrimGetAutoAnswerActiveReq;

//ICAT EXPORTED STRUCT
typedef struct CiCcPrimGetAutoAnswerActiveCnf_struct {
	CiCcResultCode Result;  /**< Result code. \sa CiCcResultCode */
	CiBoolean Active;
} CiCcPrimGetAutoAnswerActiveCnf;

//ICAT EXPORTED STRUCT
typedef struct CiCcPrimSetAutoAnswerActiveReq_struct {
	CiBoolean Active;
} CiCcPrimSetAutoAnswerActiveReq;

//ICAT EXPORTED STRUCT
typedef struct CiCcPrimSetAutoAnswerActiveCnf_struct {
	CiCcResultCode Result;  /**< Result code. \sa CiCcResultCode */
} CiCcPrimSetAutoAnswerActiveCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_LIST_CURRENT_CALLS_REQ"> */
typedef CiEmptyPrim CiCcPrimListCurrentCallsReq;

/* <INUSE> */
/**  <paramref name="CI_CC_PRIM_LIST_CURRENT_CALLS_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimListCurrentCallsCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
	UINT8 NumCalls;                 /**<  Number of Current Calls */
#ifndef CCI_CC_CONTIGUOUS
	CiCcCallInfo   *CallInfo;       /**<  Call Information List. \sa CiCcCallInfo_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *         tempNoncontPtr; /**< remove when both COMM and APP side use contiguous flags */
#endif
	CiCcCallInfo callInfo[ CICC_MAX_CURRENT_CALLS ];
#endif

} CiCcPrimListCurrentCallsCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_GET_CALLINFO_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimGetCallInfoReq_struct {
	CiCcCallId CallId;      /**< Call Identifier. \sa CiCcCallId */
} CiCcPrimGetCallInfoReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_GET_CALLINFO_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimGetCallInfoCnf_struct {
	CiCcResultCode Result;                  /**< Result code. \sa CiCcResultCode */
#ifndef CCI_CC_CONTIGUOUS
	CiCcCallInfo    *Info;                  /**< Current Call Information. \sa CiCcCallInfo_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *          tempNoncontPtr; /**< remove when both COMM and APP side use contiguous flags */
#endif
	CiCcCallInfo info;
#endif

} CiCcPrimGetCallInfoCnf;

/* <INUSE> */
/**  <paramref name="CI_CC_PRIM_MAKE_CALL_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimMakeCallReq_struct {
#ifndef CCI_CC_CONTIGUOUS
	CiCcMakeCallInfo  *Info;        /**< Outgoing (Make) Call Information. \sa CiCcMakeCallInfo_struct*/
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *          tempNoncontPtr; /**< remove when both COMM and APP side use contiguous flags */
#endif
	CiCcMakeCallInfo info;
#endif

} CiCcPrimMakeCallReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_MAKE_CALL_CNF">   */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimMakeCallCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
	CiCcCallId CallId;              /**< Unique Call Identifier. \sa CiCcCallId */
} CiCcPrimMakeCallCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_CALL_PROCEEDING_IND">   */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimCallProceedingInd_struct {
	CiCcCallId CallId;              /**< Call Identifier. \sa CiCcCallId */
	CiBoolean InBandTones;          /**< Indicates if In-band tones are available from Network. \sa CCI API Ref Manual */
} CiCcPrimCallProceedingInd;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_MO_CALL_FAILED_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimMoCallFailedInd_struct {
	CiCcCallId CallId;              /**< Call Identifier. \sa CiCcCallId */
	CiCcCause Cause;                /**< Cause Code. \sa CiCcCause */
	CiBoolean InBandTones;          /**< Indicates if In-band tones are available from Network. \sa CCI API Ref Manual */
	/* SCR #1255830 */
} CiCcPrimMoCallFailedInd;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_ALERTING_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimAlertingInd_struct {
	CiCcCallId CallId;              /**< Call Identifier. \sa CiCcCallId */
	CiBoolean InBandTones;          /**< Indicates if In-band tones are available from Network. \sa CCI API Ref Manual */
} CiCcPrimAlertingInd;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_CONNECT_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimConnectInd_struct {
	CiCcCallId CallId;              /**< Call Identifier. \sa CiCcCallId */
} CiCcPrimConnectInd;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_DISCONNECT_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimDisconnectInd_struct {
	CiCcCallId CallId;              /**< Call Identifier. \sa CiCcCallId */
	CiCcCause Cause;                /**< Disconnect Cause Code. \sa CiCcCause */
} CiCcPrimDisconnectInd;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_INCOMING_CALL_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimIncomingCallInd_struct {
	CiCcCallId CallId;                      /**< Incoming Call Identifier. \sa CiCcCallId */
#ifndef CCI_CC_CONTIGUOUS
	CiCcExtCallType *CallType;              /**< Extended Call Type Information. \sa CiCcExtCallType_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *          tempNoncontPtr; /**< remove when both COMM and APP side use contiguous flags */
#endif
	CiCcExtCallType callType;
#endif

} CiCcPrimIncomingCallInd;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_CALL_WAITING_IND">  */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimCallWaitingInd_struct {
	CiCcCallId CallId;              /**< Waiting Call Identifier. \sa CiCcCallId */
#ifndef CCI_CC_CONTIGUOUS
	CiCcCwInfo  *Info;              /**< Call Waiting information for incoming call. \sa CiCcCwInfo_struct */
#else
	CiCcCwInfo info;
#endif

} CiCcPrimCallWaitingInd;

/* <INUSE> */
/**  <paramref name="CI_CC_PRIM_HELD_CALL_IND">   */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimHeldCallInd_struct {
	CiCcCallId CallId;      /**< Held Call Identifier. \sa CiCcCallId */
} CiCcPrimHeldCallInd;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_ANSWER_CALL_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimAnswerCallReq_struct {
	CiCcCallId CallId; /**< Call Identifier. \sa CiCcCallId */
} CiCcPrimAnswerCallReq;

/* <INUSE> */
/**	 <paramref name="CI_CC_PRIM_ANSWER_CALL_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimAnswerCallCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
	CiCcCallId CallId;              /**< Call Identifier. \sa CiCcCallId */
} CiCcPrimAnswerCallCnf;
/* <INUSE> */
/** <paramref name="CI_CC_PRIM_REFUSE_CALL_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimRefuseCallReq_struct {
	CiCcCallId CallId;              /**< Call Identifier. \sa CiCcCallId */
	UINT16   Cause;             /**< Cause Code. */
} CiCcPrimRefuseCallReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_REFUSE_CALL_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimRefuseCallCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
} CiCcPrimRefuseCallCnf;
/** <paramref name="CI_CC_PRIM_MT_CALL_FAILED_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimMtCallFailedInd_struct {
	CiCcCallId CallId;              /**< Call Identifier. \sa CiCcCallId */
	CiCcCause Cause;                /**< Call failure cause. \sa CiCcCause */
	/* SCR #1255830 */
	CiBoolean InBandTones;          /**< Indicates if In-band tones are available from Network. \sa CCI API Ref Manual */
} CiCcPrimMtCallFailedInd;

/* <INUSE> */
/**  <paramref name="CI_CC_PRIM_HOLD_CALL_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimHoldCallReq_struct {
	CiCcCallId CallId;      /**< Call Identifier for the Active Call. \sa CiCcCallId */
} CiCcPrimHoldCallReq;

/* <NOTINUSE> */
/**  <paramref name="CI_CC_PRIM_HOLD_CALL_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimHoldCallCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
	CiCcCause Cause;                /**< Not in use */
} CiCcPrimHoldCallCnf;

/* <INUSE> */
/**  <paramref name="CI_CC_PRIM_RETRIEVE_CALL_REQ">   */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimRetrieveCallReq_struct {
	CiCcCallId CallId;      /**< Call Identifier for the Held Call. \sa CiCcCallId */
} CiCcPrimRetrieveCallReq;

/* <NOTINUSE> */
/** <paramref name="CI_CC_PRIM_RETRIEVE_CALL_CNF">  */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimRetrieveCallCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
	CiCcCause Cause;                /**< Not in use */
} CiCcPrimRetrieveCallCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_SWITCH_ACTIVE_HELD_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimSwitchActiveHeldReq_struct {
	CiCcCallId Active;      /**< Call Identifier for the Active Call. \sa CiCcCallId */
	CiCcCallId Held;        /**< Call Identifier for the Held Call. \sa CiCcCallId*/
} CiCcPrimSwitchActiveHeldReq;

/* <NOTINUSE> */
/** <paramref name="CI_CC_PRIM_SWITCH_ACTIVE_HELD_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimSwitchActiveHeldCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
	CiCcCause Cause;                /**< Not in use */
} CiCcPrimSwitchActiveHeldCnf;

//ICAT EXPORTED STRUCT
typedef struct CiCcPrimCallDeflectReq_struct {
	CiCcCallId CallId;
#ifndef CCI_CC_CONTIGUOUS
	CiCcCdAddressInfo *CdAddress;
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *          tempNoncontPtr; /* remove when both COMM and APP side use contiguous flags */
#endif
	CiCcCdAddressInfo cdAddress;
#endif

} CiCcPrimCallDeflectReq;

//ICAT EXPORTED STRUCT
typedef struct CiCcPrimCallDeflectCnf_struct {
	CiCcResultCode Result;
	CiCcCause Cause;
} CiCcPrimCallDeflectCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_EXPLICIT_CALL_TRANSFER_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimExplicitCallTransferReq_struct {
	CiCcCallId ActiveCall;          /**< Call Identifier for the Active Call. \sa CiCcCallId */
	CiCcCallId HeldCall;            /**< Call Identifier for the Held Call. \sa CiCcCallId*/
} CiCcPrimExplicitCallTransferReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_EXPLICIT_CALL_TRANSFER_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimExplicitCallTransferCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
} CiCcPrimExplicitCallTransferCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_RELEASE_CALL_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimReleaseCallReq_struct {
	CiCcCallId CallId;      /**< Call Identifier. \sa CiCcCallId */
} CiCcPrimReleaseCallReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_RELEASE_CALL_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimReleaseCallCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
} CiCcPrimReleaseCallCnf;

/* <NOTINUSE> */
/** <paramref name="CI_CC_PRIM_RELEASE_ALL_CALLS_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimReleaseAllCallsReq_struct {
	UINT8 WhichCalls;       /**< Not in use */
} CiCcPrimReleaseAllCallsReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_RELEASE_ALL_CALLS_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimReleaseAllCallsCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
} CiCcPrimReleaseAllCallsCnf;

//ICAT EXPORTED STRUCT
typedef struct CiCcPrimSwitchCallModeReq_struct {
	CiCcCallId CallId;
} CiCcPrimSwitchCallModeReq;

//ICAT EXPORTED STRUCT
typedef struct CiCcPrimSwitchCallModeCnf_struct {
	CiCcResultCode Result;
} CiCcPrimSwitchCallModeCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_ESTABLISH_MPTY_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimEstablishMptyReq_struct {
	CiCcCallId ActiveCall;          /**< Call Identifier for the Active Call. \sa CiCcCallId */
	CiCcCallId HeldCall;            /**< Call Identifier for the Held Call. \sa CiCcCallId */
} CiCcPrimEstablishMptyReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_ESTABLISH_MPTY_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimEstablishMptyCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
	CiCcMptyId MptyId;              /**< MPTY Identifier. \sa CiCcMptyId */
} CiCcPrimEstablishMptyCnf;

/* <INUSE> */
/**  <paramref name="CI_CC_PRIM_ADD_TO_MPTY_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimAddToMptyReq_struct {
	CiCcMptyId MptyId;      /**< MPTY Identifier for the Conference. \sa CiCcMptyId */
	CiCcCallId CallId;      /**< Call Identifier for the New Call. \sa CiCcCallId */
} CiCcPrimAddToMptyReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_ADD_TO_MPTY_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimAddToMptyCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
} CiCcPrimAddToMptyCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_HOLD_MPTY_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimHoldMptyReq_struct {
	CiCcMptyId MptyId;      /**< MPTY Identifier. \sa CiCcMptyId  */
} CiCcPrimHoldMptyReq;

/* <INUSE> */
/**	<paramref name="CI_CC_PRIM_HOLD_MPTY_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimHoldMptyCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
} CiCcPrimHoldMptyCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_RETRIEVE_MPTY_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimRetrieveMptyReq_struct {
	CiCcMptyId MptyId;      /**< MPTY Identifier. \sa CiCcMptyId */
} CiCcPrimRetrieveMptyReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_RETRIEVE_MPTY_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimRetrieveMptyCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
} CiCcPrimRetrieveMptyCnf;

/* <INUSE> */
/**	<paramref name="CI_CC_PRIM_SPLIT_FROM_MPTY_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimSplitFromMptyReq_struct {
	CiCcMptyId MptyId;      /**< MPTY Identifier for the Conference. \sa CiCcMptyId */
	CiCcCallId CallId;      /**< Call Identifier for the Call to be split out. \sa CiCcCallId */
} CiCcPrimSplitFromMptyReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_SPLIT_FROM_MPTY_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimSplitFromMptyCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
} CiCcPrimSplitFromMptyCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_SHUTTLE_MPTY_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimShuttleMptyReq_struct {
	CiCcMptyId MptyId;      /**< MPTY Identifier for the Conference. \sa CiCcMptyId */
	CiCcCallId CallId;      /**< Call Identifier for the Single Call. \sa CiCcCallId */
} CiCcPrimShuttleMptyReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_SHUTTLE_MPTY_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimShuttleMptyCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
} CiCcPrimShuttleMptyCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_RELEASE_MPTY_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimReleaseMptyReq_struct {
	CiCcMptyId MptyId;      /**< MPTY Identifier. \sa CiCcMptyId */
} CiCcPrimReleaseMptyReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_RELEASE_MPTY_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimReleaseMptyCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
} CiCcPrimReleaseMptyCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_START_DTMF_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimStartDtmfReq_struct {
	CiCcCallId CallId;      /**< Call Identifier. \sa CiCcCallId */
	UINT8 Digit;            /**< DTMF digit */
} CiCcPrimStartDtmfReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_START_DTMF_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimStartDtmfCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
} CiCcPrimStartDtmfCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_STOP_DTMF_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimStopDtmfReq_struct {
	CiCcCallId CallId;      /**< Call Identifier. \sa CiCcCallId */
} CiCcPrimStopDtmfReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_STOP_DTMF_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimStopDtmfCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
} CiCcPrimStopDtmfCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_GET_DTMF_PACING_REQ"> */
typedef CiEmptyPrim CiCcPrimGetDtmfPacingReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_GET_DTMF_PACING_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimGetDtmfPacingCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
	CiCcDtmfPacing Pacing;          /**< DTMF Pacing Configuration. \sa CiCcDtmfPacing_struct */
} CiCcPrimGetDtmfPacingCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_SET_DTMF_PACING_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimSetDtmfPacingReq_struct {
	CiCcDtmfPacing Pacing;          /**< DTMF Pacing Configuration. \sa CiCcDtmfPacing_struct */
} CiCcPrimSetDtmfPacingReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_SET_DTMF_PACING_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimSetDtmfPacingCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
} CiCcPrimSetDtmfPacingCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_SEND_DTMF_STRING_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimSendDtmfStringReq_struct {
	CiCcCallId CallId;              /**< Call Identifier. \sa CiCcCallId. */
#ifndef CCI_CC_CONTIGUOUS
	UINT8       *Digits;
#else
	UINT8 digits[CICC_MAX_DTMF_STRING_LENGTH];      /**< DTMF digits. */
#endif

} CiCcPrimSendDtmfStringReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_SEND_DTMF_STRING_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimSendDtmfStringCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
} CiCcPrimSendDtmfStringCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_CLIP_INFO_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimClipInfoInd_struct {
    CiCcCallId   CallId;		/**< Call Identifier. \sa CiCcCallId. */
#ifndef CCI_CC_CONTIGUOUS
	CiCcClipInfo  *Info;            /**< CLIP information for incoming call. \sa CiCcClipInfo_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *          tempNoncontPtr;                /**< remove when both COMM and APP side use contiguous flags */
#endif
	CiCcClipInfo info;                              /**< CLIP information for incoming call. \sa CiCcClipInfo_struct */
#endif

} CiCcPrimClipInfoInd;


/* Michal Bukai - CDIP Support - Start:*/
/** <paramref name="CI_CC_PRIM_CDIP_INFO_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimCdipInfoInd_struct {
#ifndef CCI_CC_CONTIGUOUS
	CiCcCdipInfo  *Info;   /**< CDIP information for incoming call. \sa  CiCcCdipInfo_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *          tempNoncontPtr;        /* remove when both COMM and APP side use contiguous flags */
#endif
	CiCcCdipInfo info;                      /**< CDIP information for incoming call. \sa  CiCcCdipInfo_struct */
#endif

} CiCcPrimCdipInfoInd;
/* Michal Bukai - CDIP Support - End*/

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_COLP_INFO_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimColpInfoInd_struct {
#ifndef CCI_CC_CONTIGUOUS
	CiCcColpInfo  *Info;                /**< CoLP information for incoming call. \sa CiCcColpInfo_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *          tempNoncontPtr;                        /**< Remove when both COMM and APP side use contiguous flags */
#endif
	CiCcColpInfo info;                                      /**< CoLP information for incoming call. \sa CiCcColpInfo_struct */
#endif

} CiCcPrimColpInfoInd;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_CCM_UPDATE_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimCcmUpdateInd_struct {
	UINT32 Ccm;                     /**< Current CCM reading in Home units. Unsigned 24-bit integer. */
	UINT32 Duration;                /**< Current call duration in seconds. Unsigned 24-bit integer. */
} CiCcPrimCcmUpdateInd;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_GET_CCM_VALUE_REQ"> */
typedef CiEmptyPrim CiCcPrimGetCcmValueReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_GET_CCM_VALUE_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimGetCcmValueCnf_struct {
	CiCcResultCode Result;                  /**< Result code. \sa CiCcResultCode */
	UINT32 Ccm;                             /**< Current CCM value in Home units. Unsigned 24-bit integer. */
	UINT32 Duration;                        /**< Current call duration in seconds. Unsigned 24-bit integer. */
} CiCcPrimGetCcmValueCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_AOC_WARNING_IND"> */
typedef CiEmptyPrim CiCcPrimAocWarningInd;

//ICAT EXPORTED STRUCT
/**  <paramref name="CI_CC_PRIM_SSI_NOTIFY_IND"> */
typedef struct CiCcPrimSsiNotifyInd_struct {
#ifndef CCI_CC_CONTIGUOUS
	CiSsiNotifyInfo *Info; /**< Supplementary Service Intermediate (SSI) notification information. \sa CI SS Spec */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *          tempNoncontPtr;        /* remove when both COMM and APP side use contiguous flags */
#endif
	CiSsiNotifyInfo info;                   /**< Supplementary Service Intermediate (SSI) notification information. \sa CI SS Spec */
#endif

} CiCcPrimSsiNotifyInd;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_CC_PRIM_SSU_NOTIFY_IND"> */
typedef struct CiCcPrimSsuNotifyInd_struct {
#ifndef CCI_CC_CONTIGUOUS
	CiSsuNotifyInfo *Info;  /**< Supplementary Service Unsolicited (SSU) notification information. \sa CI SS Spec */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *          tempNoncontPtr;        /* remove when both COMM and APP side use contiguous flags */
#endif
	CiSsuNotifyInfo info;                   /**< Supplementary Service Unsolicited (SSU) notification information. \sa CI SS Spec */
#endif

} CiCcPrimSsuNotifyInd;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_GET_ACM_VALUE_REQ"> */
typedef CiEmptyPrim CiCcPrimGetAcmValueReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_GET_ACM_VALUE_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimGetAcmValueCnf_struct {
	CiCcResultCode Result;                  /**< Result code. \sa CiCcResultCode */
	UINT32 Acm;                             /**< Current ACM value. Unsigned 24-bit integer. */
} CiCcPrimGetAcmValueCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_RESET_ACM_VALUE_REQ"> */
typedef CiEmptyPrim CiCcPrimResetAcmValueReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_RESET_ACM_VALUE_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimResetAcmValueCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
} CiCcPrimResetAcmValueCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_GET_ACMMAX_VALUE_REQ"> */
typedef CiEmptyPrim CiCcPrimGetAcmMaxValueReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_GET_ACMMAX_VALUE_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimGetAcmMaxValueCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
	UINT32 AcmMax;                  /**< Current ACMmax value. Unsigned 24-bit integer */
} CiCcPrimGetAcmMaxValueCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_SET_ACMMAX_VALUE_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimSetAcmMaxValueReq_struct {
	UINT32 AcmMax;          /**< New ACMmax value. Unsigned 24-bit integer. */
} CiCcPrimSetAcmMaxValueReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_SET_ACMMAX_VALUE_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimSetAcmMaxValueCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */

} CiCcPrimSetAcmMaxValueCnf;
/* <INUSE> */
/** <paramref name="CI_CC_PRIM_GET_PUCT_INFO_REQ"> */
typedef CiEmptyPrim CiCcPrimGetPuctInfoReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_GET_PUCT_INFO_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimGetPuctInfoCnf_struct {
	CiCcResultCode Result;                  /**< Result code. \sa CiCcResultCode */
#ifndef CCI_CC_CONTIGUOUS
	CiCcPuctInfo    *Info;                  /**< Current PUCT information. \sa CiCcPuctInfo_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *          tempNoncontPtr;                /**< Remove when both COMM and APP side use contiguous flags */
#endif
	CiCcPuctInfo info;                              /**< Current PUCT information. \sa CiCcPuctInfo_struct */
#endif

} CiCcPrimGetPuctInfoCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_SET_PUCT_INFO_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimSetPuctInfoReq_struct {
#ifndef CCI_CC_CONTIGUOUS
	CiCcPuctInfo  *Info;               /**< New PUCT information. \sa CiCcPuctInfo_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *          tempNoncontPtr;                        /**< Remove when both COMM and APP side use contiguous flags */
#endif
	CiCcPuctInfo info;                                      /**< New PUCT information. \sa CiCcPuctInfo_struct */
#endif

} CiCcPrimSetPuctInfoReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_SET_PUCT_INFO_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimSetPuctInfoCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
} CiCcPrimSetPuctInfoCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_GET_BASIC_CALLMODES_REQ"> */
typedef CiEmptyPrim CiCcPrimGetBasicCallModesReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_GET_BASIC_CALLMODES_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimGetBasicCallModesCnf_struct {
	CiCcResultCode Result;                                                                  /**< Result code. \sa CiCcResultCode */
	UINT8 NumModes;                                                                         /**< Number of supported Basic Call Modes */
	CiCcBasicCMode Modes[ CICC_NUM_BASIC_CMODES ];                                          /**< Supported Basic Call Modes. \sa CiCcBasicCMode  */
} CiCcPrimGetBasicCallModesCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_GET_CALLOPTIONS_REQ"> */
typedef CiEmptyPrim CiCcPrimGetCallOptionsReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_GET_CALLOPTIONS_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimGetCallOptionsCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
	CiCcCallOptions Options;        /**< Supported Call Options bitmap. \sa CiCcCallOptions */
} CiCcPrimGetCallOptionsCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_GET_DATACOMP_CAP_REQ"> */
typedef CiEmptyPrim CiCcPrimGetDataCompCapReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_GET_DATACOMP_CAP_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimGetDataCompCapCnf_struct {
	CiCcResultCode Result;                          /**< Result code. \sa CiCcResultCode */
#ifndef CCI_CC_CONTIGUOUS
	CiCcDataCompCap *pCap;                          /**< Data compression configuration capability. \sa CiCcDataCompCap_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *                     tempNoncontPtr;             /**< Remove when both COMM and APP side use contiguous flags */
#endif
	CiCcDataCompCap cap;                                    /**< Data compression configuration capability. \sa CiCcDataCompCap_struct */
#endif

} CiCcPrimGetDataCompCapCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_GET_DATACOMP_REQ"> */
typedef CiEmptyPrim CiCcPrimGetDataCompReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_GET_DATACOMP_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimGetDataCompCnf_struct {
	CiCcResultCode Result;                          /**< Result code. \sa CiCcResultCode */
#ifndef CCI_CC_CONTIGUOUS
	CiCcDataCompInfo    *pInfo;                     /**< Data compression information. \sa CiCcDataCompInfo_struct */

#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *                     tempNoncontPtr;             /**< Remove when both COMM and APP side use contiguous flags */
#endif
	CiCcDataCompInfo info;                                  /**< Data compression information. \sa CiCcDataCompInfo_struct */
#endif

} CiCcPrimGetDataCompCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_SET_DATACOMP_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimSetDataCompReq_struct {
#ifndef CCI_CC_CONTIGUOUS
	CiCcDataCompInfo            *pInfo;        /**< Data compression information. \sa CiCcDataCompInfo_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8                                       *tempNoncontPtr;    /**< Remove when both COMM and APP side use contiguous flags */
#endif
	CiCcDataCompInfo info;                                          /**< Data compression information. \sa CiCcDataCompInfo_struct */
#endif

} CiCcPrimSetDataCompReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_SET_DATACOMP_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimSetDataCompCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
} CiCcPrimSetDataCompCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_GET_RLP_CAP_REQ"> */
typedef CiEmptyPrim CiCcPrimGetRlpCapReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_GET_RLP_CAP_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimGetRlpCapCnf_struct {
	CiCcResultCode Result;                  /**< Result code. \sa CiCcResultCode */
#ifndef CCI_CC_CONTIGUOUS
	CiCcRlpCap      *pCap;                  /**< Data compression configuration capability. \sa CiCcRlpCap_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8 *          tempNoncontPtr;                /**< Remove when both COMM and APP side use contiguous flags */
#endif
	CiCcRlpCap cap;                                 /**< Data compression configuration capability. \sa CiCcRlpCap_struct */
#endif

} CiCcPrimGetRlpCapCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_GET_RLP_CFG_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimGetRlpCfgReq_struct {
	UINT8 ver;              /**< RLP version.  */
} CiCcPrimGetRlpCfgReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_GET_RLP_CFG_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimGetRlpCfgCnf_struct {
	CiCcResultCode Result;                                  /**< Result code. \sa CiCcResultCode */
#ifndef CCI_CC_CONTIGUOUS
	CiCcRlpCfg                  *pCfg;                      /**< RLP configuration. \sa CiCcRlpCfg_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8                                * tempNoncontPtr;                  /**< Remove when both COMM and APP side use contiguous flags */
#endif
	CiCcRlpCfg cfg;                                                         /**< RLP configuration. \sa CiCcRlpCfg_struct */
#endif

} CiCcPrimGetRlpCfgCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_SET_RLP_CFG_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimSetRlpCfgReq_struct {
#ifndef CCI_CC_CONTIGUOUS
	CiCcRlpCfg          *pCfg;                    /**< RLP configuration. \sa CiCcRlpCfg_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8                       *tempNoncontPtr;                            /**< Remove when both COMM and APP side use contiguous flags */
#endif
	CiCcRlpCfg cfg;                                                         /**< RLP configuration. \sa CiCcRlpCfg_struct */
#endif

} CiCcPrimSetRlpCfgReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_SET_RLP_CFG_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimSetRlpCfgCnf_struct {
	CiCcResultCode Result;                  /**< Result code. \sa CiCcResultCode */
} CiCcPrimSetRlpCfgCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_DATA_SERVICENEG_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimDataServiceNegInd_struct {
	CiBoolean isAsync;                      /**< Sync/Async Indication. \sa CCI API Ref Manual */
	CiBoolean isTransparent;                /**< Transparent/Non-transparent Indication. \sa CCI API Ref Manual */
} CiCcPrimDataServiceNegInd;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_ENABLE_DATA_SERVICENEG_IND_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimEnableDataServiceNegIndReq_struct {
	CiBoolean enable;       /**<  TRUE: enable reporting; FALSE: disable reporting, default. \sa CCI API Ref Manual */
} CiCcPrimEnableDataServiceNegIndReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_ENABLE_DATA_SERVICENEG_IND_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimEnableDataServiceNegIndCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
} CiCcPrimEnableDataServiceNegIndCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_SET_UDUB_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimSetUDUBReq_struct {
	CiCcCallId CallId;      /**< Call Identifier. \sa CiCcCallId */
} CiCcPrimSetUDUBReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_SET_UDUB_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimSetUDUBCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
} CiCcPrimSetUDUBCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_GET_SUPPORTED_CALLMAN_OPS_REQ"> */
typedef CiEmptyPrim CiCcPrimGetSupportedCallManOpsReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_GET_SUPPORTED_CALLMAN_OPS_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimGetSupportedCallManOpsCnf_struct {
	CiCcResultCode Result;                                                                  /**< Result code. \sa CiCcResultCode */
	UINT8 NumOps;                                                                           /**< Number of Operation Codes [0..CI_CC_MANOP_NUM_OPS - 1] */
	CiCcCallManOp OpCodes[ CI_CC_MANOP_NUM_OPS ];                                           /**< Array of supported Operation Codes. \sa CiCcCallManOp */
}   CiCcPrimGetSupportedCallManOpsCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_MANIPULATE_CALLS_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimManipulateCallsReq_struct {
	CiCcCallManOp OpCode;                   /**< Call Manipulation Operation Code. \sa CiCcCallManOp */
	CiCcCallId CallId;                      /**< Call Identifier. \sa CiCcCallId */
} CiCcPrimManipulateCallsReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_MANIPULATE_CALLS_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimManipulateCallsCnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
} CiCcPrimManipulateCallsCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_LIST_CURRENT_CALLS_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimListCurrentCallsInd_struct {
	UINT8 NumCalls;                                                                 /**< Number of current calls [0..CICC_MAX_CURRENT_CALLS] */
#ifndef CCI_CC_CONTIGUOUS
	CiCcCallInfo        *CallInfo;                                                  /**< Call Information List. \sa CiCcCallInfo_struct */
#else
	CiCcCallInfo callInfo[ CICC_MAX_CURRENT_CALLS ];                                /**< Call Information List. \sa CiCcCallInfo_struct */
#endif

} CiCcPrimListCurrentCallsInd;

/** \brief  DTMF tone status indicators. */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CICC_TONESTATUS {
	CICC_TONE_STARTED,      /**< DTMF tone started */
	CICC_TONE_STOPPED,      /**< DTMF tone stopped normally */
	CICC_TONE_ABORTED       /**< DTMF tone aborted due to abnormal condition */
} _CiCcToneStatus;
/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief DTMF tone status indicator.
 * \sa CICC_TONESTATUS */
/** \remarks Common Data Section */
typedef UINT8 CiCcToneStatus;
/**@}*/

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_DTMF_EVENT_IND"> */

//ICAT EXPORTED STRUCT
typedef struct CiCcPrimDtmfEventInd_struct {

	CiCcCallId CallId;                      /**< Identifier of the call for which DTMF operation was required. \sa CiCcCallId */
	UINT8 ToneDigit;                        /**< Indicates the DTMF tone digit; valid = "0123456789*#ABCD" */
	CiBoolean SingleTone;                   /**< Indicates a single DTMF tone or a tone sent as part of a DTMF string. \sa CCI API Ref Manual */
	CiCcToneStatus ToneStatus;              /**< Indicates the DTMF tone status (started, aborted or stopped). \sa CiCcToneStatus*/

} CiCcPrimDtmfEventInd;

/* customer specific data types */

/* The cause IE is a type 4 information element with a minimum length of 4 octets
 * and a maximum length of 32 octets. The diagnostic field starts in byte 5. */
#define CICC_MAX_CAUSE_DIAGNOSTIC_LENGTH  28

//ICAT EXPORTED STRUCT
/** \brief  Diagnostics info */
/** \remarks Common Data Section */
typedef struct _CiCcDiagnosticInfo_struct {

	UINT16 Length;
	UINT8 Data[CICC_MAX_CAUSE_DIAGNOSTIC_LENGTH];

}  CiCcDiagnosticInfo;

//ICAT EXPORTED ENUM
/** \brief Coding standard.
 * \details Coding standard is defined in (octet 3 of Cause Information Element) See TS 24.008 10.5.4.11.
 * This coding standard is also used in the 'Progress Indicator', 'Call State',
 * 'High Layer Compatibility', 'Low Layer Compatibility'. */
/** \remarks Common Data Section */
typedef enum CiCcCodingStandardTag
{
	CICC_CODING_CCITT_Q931           =   0,                 /**< Coding as specified in ITU-T Rec. Q.931 */
	CICC_CODING_OTHER_INTERNATL      =   1,                 /**< Reserved for other international standards */
	CICC_CODING_NATIONAL             =   2,                 /**< National standard */
	CICC_CODING_GSM_NETWORK          =   3                  /**< Standard defined for the GSM PLMNS */
} _CiCcCodingStandard;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Coding standard (octet 3 of Cause Information Element) as per 24.008 10.5.4.11.
 * \sa CiCcCodingStandardTag */
/** \remarks Common Data Section */
typedef UINT8 CiCcCodingStandard;
/**@}*/

//ICAT EXPORTED ENUM
/** \brief Location */
/** \details Location is defined in (octet 3 of Cause Information Element) See TS 24.008 10.5.4.11.*/
/** \remarks Common Data Section */
typedef enum CiCcLocationTag
{
	CICC_LOC_USER                    =   0,                 /**< user */
	CICC_LOC_PRIV_LOCAL              =   1,                 /**< private network serving the local user */
	CICC_LOC_PUB_LOCAL               =   2,                 /**< public network serving the local user */
	CICC_LOC_TRANSIT                 =   3,                 /**< transit network */
	CICC_LOC_PUB_REMOTE              =   4,                 /**< public network serving the remote user */
	CICC_LOC_PRIV_REMOTE             =   5,                 /**< private network serving the remote user */
	CICC_LOC_INTERNATIONAL           =   7,                 /**< international network */
	CICC_LOC_BEYOND_IWF              =   10                 /**< network beyond interworking point */
} _CiCcLocation;
/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Location (octet 3 of Cause Information Element) as per 24.008 10.5.4.11.
 * \sa CiCcLocationTag */
/** \remarks Common Data Section */
typedef UINT8 CiCcLocation;
/**@}*/
/** \brief Recommendation.
 * \details Recommendation is defined in (octet 3a of Cause Information Element) See TS 24.008 10.5.4.11.
 * Recommendation shall be ignored if the coding standard is "CICC_CODING_GSM_NETWORK".
 * For all other coding standards 'Recommendation' will be coded according to the specified coding standard.
 */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CiCcRecommendationTag
{
	CICC_REC_Q931       =   0,              /**< Recommendation Q.931: "ISDN user-network interface layer 3 specification for basic control" */
	CICC_REC_GSM        =   1,              /**< Recommendation GSM */
	CICC_REC_X21                =   3,      /**< Recommendation X.21: "Interface between data terminal equipment (DTE) and data circuit-terminating equipment (DCE) for synchronous operation on public data networks" */
	CICC_REC_X25                =   4       /**< Recommendation X.25: "Interface between data terminal equipment (DTE) and data circuit-terminating equipment (DCE) for terminals operating in the packet mode and connected to public data networks by dedicated circuit" */
} _CiCcRecommendation;
/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Recomendation.
 * \sa CiCcRecommendationTag */
/** \remarks Common Data Section */
typedef UINT8 CiCcRecommendation;

/**  \brief Status or Failure Cause codes.
 * \sa CiCcCause */
/** \remarks Common Data Section */
typedef CiCcCause CiCcCustCause;
/**@}*/
/** \brief Diagnostic info */
//ICAT EXPORTED STRUCT
/** \remarks Common Data Section */
typedef struct _CiCcDiagnostic_struct {

	CiBoolean InfoPresent;                                          /**< TRUE - is present; FALSE - is not present. \sa CCI API Ref Manual */
	CiCcCodingStandard Coding;                                      /**< Coding standard (octet 3 of Cause Information Element) as per 24.008 10.5.4.11. \sa CiCcCodingStandard*/
	CiCcLocation Location;                                          /**< Location (octet 3 of Cause Information Element) as per 24.008 10.5.4.11. \sa CiCcLocation */
	CiCcRecommendation Recommendation;                              /**< Recomendation. \sa CiCcRecommendation */
	CiCcCustCause Cause;                                            /**< Cause Code. \sa CiCcCustCause */
	CiCcDiagnosticInfo Info;                                        /**< The diagnostic info. \sa _CiCcDiagnosticInfo_struct */

} CiCcDiagnostic;

/* customer specific primitives */

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_CALL_DIAGNOSTIC_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimCallDiagnosticInd_struct {

	CiCcCallId CallId;                      /**< Call Identifier. \sa CiCcCallId */
	CiCcDiagnostic Diagnostic1;             /**< The first diagnostic info field. \sa _CiCcDiagnosticInfo_struct */
	CiCcDiagnostic Diagnostic2;             /**< The second diagnostic info field. \sa _CiCcDiagnosticInfo_struct */

} CiCcPrimCallDiagnosticInd;


/* <INUSE> */
/** <paramref name="CI_CC_PRIM_CLEAR_BLACK_LIST_REQ">  */
typedef CiEmptyPrim CiCcPrimClearBlackListReq;


/* <INUSE> */
/** <paramref name="CI_CC_PRIM_CLEAR_BLACK_LIST_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimClearBlackListCnf_struct {

	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */

} CiCcPrimClearBlackListCnf;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_SET_CTM_STATUS_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimSetCtmStatusReq_struct {
	CiBoolean Active;               /**< Current CTM Status. \sa CCI API Ref Manual */
}CiCcPrimSetCtmStatusReq;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_SET_CTM_STATUS_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimSetCtmStatuscnf_struct {
	CiCcResultCode Result;          /**< Result code. \sa CiCcResultCode */
} CiCcPrimSetCtmStatusCnf;

/** \brief CTM negotiation report status - values */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CiCcCTMNegReportType_Tags {
	CTM_STARTED = 1,                /**< Started */
	CTM_SUCCEDED,                   /**< Succeded */
	CTM_FAILED                      /**< Failed */
} _CiCcCTMNegReportType;
/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief CTM negotiation report status.
 * \sa CiCcCTMNegReportType_Tags */
/** \remarks Common Data Section */
typedef UINT8 CiCcCTMNegReportType;
/**@}*/

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_CTM_NEG_REPORT_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimCTMNegReportInd_struct {
	CiCcCTMNegReportType CTMNegReport;              /**< Neg status result. \sa CiCcCTMNegReportType */
}CiCcPrimCTMNegReportInd;
#ifdef AT_CUSTOMER_HTC
/* Fire wall Information structure - used for Black/White List Phone Number. */
#define CI_MAX_FW_STRING_LENGTH 40
#define CI_MAX_FW_NUM_LIST 5
#define CI_MAX_FW_AC_CODE_LENGTH 5

/** \brief MT call firewall config - values */
//ICAT EXPORTED ENUM
typedef enum CiCcFwConfig_Tags {
    CICC_FW_CFG_DISABLE=0,      /**< Disable MT call firewall */
    CICC_FW_CFG_RECEIVE_ALL,    /**< Receive all incoming call */
    CICC_FW_CFG_REJECT_ALL,     /**< Reject all incoming call */
    CICC_FW_CFG_REJECT_BLACK,   /**< Reject incoming call in black list */
    CICC_FW_CFG_RECEIVE_WHITE,  /**< Only receive incoming call in white list */

    CICC_FW_NUM_CFG
} _CiCcFwConfig;
/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief MT call firewall config.
 * \sa CiCcFwConfig_Tags */
typedef UINT8 CiCcFwConfig;
/**@}*/

/** \brief FWDB Type - values */
//ICAT EXPORTED ENUM
typedef enum CiCcFwType_Tags {
    CICC_FW_TYPE_WHITE=0,      /**< add/update/delete white list  */
    CICC_FW_TYPE_BLACK,        /**< add/update/delete black list  */
    CICC_FW_TYPE_FLAG_AC,      /**< update the flag of receiveing unknown incoming call or country code and area code */
    CICC_FW_TYPE_SPECIAL,      /**< provide some special commands. */

    CICC_FW_NUM_TYPE
} _CiCcFwType;
/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief FWDB Type.
 * \sa CiCcFwType_Tags */
typedef UINT8 CiCcFwType;
/**@}*/

/** \brief   MT Call firewall List Information.
*/
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiCcFwListInfo_struct {
    UINT16   Index;                  /**< The record index in the database */
    UINT16   Length;                 /**< Input phone number length */
    CHAR     PhoneNumString[CI_MAX_FW_STRING_LENGTH];
} CiCcFwListInfo;

/** \brief   MT Call firewall Area or Country Code Information.
*/
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiCcFwFlagAcCodeInfo_struct {
    UINT8      Index;                /**< 0: Receive unknown incoming call; 1: country code; 2: area code. */
    CiBoolean  Flag_Unknown_Call;     /**<0: not receive unknown imcoming call; 1: receive when firewall is enabled; Only used for Index=0. */
    CHAR       A_C_Code[CI_MAX_FW_AC_CODE_LENGTH];    /**< Area code or Country code; Only used for Index=1/2. */
} CiCcFwFlagAcCodeInfo;

/** \brief FWDB special command.
*/
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiCcFwSpecialCmdInfo_struct {
    UINT8      Index;                /**< hard coded 0: clean all records. */
    UINT8      Value;                /**< 0: white list; 1: black list. */
} CiCcFwSpecialCmdInfo;

/* <INUSE> */
/** <paramref name="CI_CC_PRIM_SET_FW_LIST_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiCcPrimSetFwListReq_struct {
    CiCcFwType       Type;                         /**< Type. \sa CiCcFwType */

    UINT8            NumRecords;                   /**< The number of input phone number record. */
    CiCcFwListInfo   ListInfo[CI_MAX_FW_NUM_LIST]; /**< If type=0/1, FWDB cmd info.  \sa CiCcFwListInfo */

    CiCcFwFlagAcCodeInfo FlagAcCodeInfo;           /**< If type=2, FWDB cmd info. \sa CiCcFwFlagAcCodeInfo */

    CiCcFwSpecialCmdInfo SpecialCmdInfo;           /**< If type=3, FWDB special cmd info. \sa CiCcFwSpecialCmdInfo */
} CiCcPrimSetFwListReq;

//ICAT EXPORTED STRUCT
/** <paramref name="CI_CC_PRIM_SET_FW_LIST_CNF">   */
typedef struct CiCcPrimSetFwListCnf_struct{
    CiCcResultCode  Result; /**< Result code. \sa CiCcResultCode */
} CiCcPrimSetFwListCnf;
#endif
/* ADD NEW COMMON PRIMITIVES DEFINITIONS HERE */


#ifdef CI_CUSTOM_EXTENSION
/* it is assumed that only one customized set of extension primitives is
 * to be considered at one time. The selection of the particular customized
 * set is done in the 'ci_cc_cust.h' based on compile flags.
 *
 * Note: if no customer extension primitives are defined for this service group
 * the CI_CC_NUM_CUST_PRIM will be set to 0 in the "ci_cc_cust.h" file.
 */
#include "ci_cc_cust.h"

#define CI_CC_NUM_PRIM ( CI_CC_NUM_COMMON_PRIM + CI_CC_NUM_CUST_PRIM )

#else

/* if no customer extension is supported, only the default common set is considered */
#define CI_CC_NUM_PRIM CI_CC_NUM_COMMON_PRIM

#endif /* CI_CUSTOM_EXTENSION */


#ifdef __cplusplus
}
#endif  //__cplusplus

#endif  /* _CI_CC_H_ */


/*                      end of ci_cc.h
   --------------------------------------------------------------------------- */

