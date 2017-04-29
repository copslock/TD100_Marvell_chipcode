/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/* ===========================================================================
   File        : ci_msg.h
   Description : Data types file for the MSG Service Group
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

#if !defined(_CI_MSG_H_)
#define _CI_MSG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ci_api_types.h"

/** \addtogroup  SpecificSGRelated
 * @{ */

#define CI_MSG_VER_MAJOR 3
#define CI_MSG_VER_MINOR 0

/* -----------------------------------------------------------------------------
 *    MSG Configuration definitions
 * ----------------------------------------------------------------------------- */

/* Maximum number of CBMI (mids) list entries for Get/Set CBM Types primitives */
#define CIMSG_MAX_MIDS_LIST_SIZE        30      /* For local mids List        */
#define CIMSG_MAX_MIDS_RANGELST_SIZE    5       /* For mids RangeLst[ ] array */
#define CIMSG_MAX_MIDS_INDVLIST_SIZE    30      /* For mids IndvList[ ] array */

/* Maximum number of LP (dcss) list entries for Get/Set CBM Types primitives */
#define CIMSG_MAX_DCSS_LIST_SIZE        30      /* For local dcss List        */
#define CIMSG_MAX_DCSS_RANGELST_SIZE    3       /* For dcss RangeLst[ ] array */
#define CIMSG_MAX_DCSS_INDVLIST_SIZE    30      /* For dcss IndvList[ ] array */

/* Maximum values for CBMI (mids) and LP (dcss) themselves */
#define CIMSG_MAX_MIDS_VALUE            999
#define CIMSG_MAX_DCSS_VALUE            63   /* refer to 3G TS 23.038 */

#define CI_MSG_MAX_SUPPORTED_SERVICES   3

/* ----------------------------------------------------------------------------- */

/* CI_MSG Primitive ID definitions */
//ICAT EXPORTED ENUM
/** Summary of primitives */
typedef enum CI_MSG_PRIM {
	CI_MSG_PRIM_GET_SUPPORTED_SERVICES_REQ = 1,                     /**< \brief Query the SMS services supported by the cellular subsystem \details  */
	CI_MSG_PRIM_GET_SUPPORTED_SERVICES_CNF,                         /**< \brief Provides a list of SMS services supported by the cellular subsystem \details  */
	CI_MSG_PRIM_SELECT_SERVICE_REQ,                                 /**< \brief Select a SMS messaging service \details  */
	CI_MSG_PRIM_SELECT_SERVICE_CNF,                                 /**< \brief Confirmation to selection request to a MSG messaging service \details  */
	CI_MSG_PRIM_GET_CURRENT_SERVICE_INFO_REQ,                       /**< \brief Query information about current SMS messaging service being used \details  */
	CI_MSG_PRIM_GET_CURRENT_SERVICE_INFO_CNF,                       /**< \brief Provides information about current MSG messaging service being used \details  */
	CI_MSG_PRIM_GET_SUPPORTED_STORAGES_REQ,                         /**< \brief Query the memory storages supported for read, write and receive related \details  */
	CI_MSG_PRIM_GET_SUPPORTED_STORAGES_CNF,                         /**< \brief Provides lists of memory storages supported for read, write and receive related \details  */
	CI_MSG_PRIM_SELECT_STORAGE_REQ,                                 /**< \brief Select memory storage for read, write or receive related operation. \details For type CI_MSG_STRGOPERTYPE_RECV, regardless of the storage selected, received CBMs are always stored in
									 *  CI_MSG_STORAGE_BM, received status reports are always stored in CI_MSG_STORAGE_SR unless
									 *  CI_MSG_PRIM_CONFIG_MSG_IND_REQ requests received messages to be forwarded to application subsystem directly without storing
									 *  at cellular side. */
	CI_MSG_PRIM_SELECT_STORAGE_CNF,                                 /**< \brief Report a set of memory storages information being used for read, write and receive related operations. \details Cellular subsystem should have a set of default storages for different types of storage operations. */
	CI_MSG_PRIM_GET_CURRENT_STORAGE_INFO_REQ,                       /**< \brief Query information about current memory storages being used for read, write and receive related operations. \details  */
	CI_MSG_PRIM_GET_CURRENT_STORAGE_INFO_CNF,                       /**< \brief Provides information about current memory storages being used for read, write and receive related operations. \details  */
	CI_MSG_PRIM_READ_MESSAGE_REQ,                                   /**< \brief Read message from the selected memory storage \details The memory storage, upon which the read access is operated, is decided by the CI_MSG_PRIM_SELECT_STORAGE_REQ with type set
									 *  to CI_MSG_STRGOPERTYPE_READ_DELETE. The message read could be SMS message or CB message depends on the type of the memory storage */
	CI_MSG_PRIM_READ_MESSAGE_CNF,                                   /**< \brief Report a message read from the selected memory storages. \details Application subsystem has to be able to understand PDU formatted according to TS 23.040 and TS 23.041.
									 *  if the read message is a SMS message, the pData consists of TS 24.011 SC address followed by TS 23.040  TPDU; if the read message is a CBS message, the pData consists of TS 23.041 TPDU. */
	CI_MSG_PRIM_DELETE_MESSAGE_REQ,                                 /**< \brief Delete message(s) from the selected memory storage. \details The memory storage, upon which the delete access is operated, is decided by the CI_MSG_PRIM_SELECT_STORAGE_REQ with type
									 *  set to CI_MSG_STRGOPERTYPE_READ_DELETE. The message deleted can be SMS message(s) or CB message(s) depends on the type of the memory storage. */
	CI_MSG_PRIM_DELETE_MESSAGE_CNF,                                         /**< \brief Report message(s) deleted from the selected memory storages. \details  */
	CI_MSG_PRIM_SEND_MESSAGE_REQ,                                   /**< \brief Send a SMS message to the SMSC. \details The sent SMS message is a SMS message with type of CI_SMS_MSG_TYPE_SUBMIT, refer to TS 23.040 v3.8.0 9.2.2.2.
									 *  This primitive only apply to SMS messages. */
	CI_MSG_PRIM_SEND_MESSAGE_CNF,                                           /**< \brief Confirmation of sending a SMS message to the SMC. \details  */
	CI_MSG_PRIM_WRITE_MESSAGE_REQ,                                  /**< \brief Store a SMS message to the selected memory storage with operation type of CI_MSG_STRGOPERTYPE_WRITE_SEND. \details The stored SMS message can have type of either CI_MSG_MSG_TYPE_SUBMIT or CI_MSG_MSG_TYPE_COMMAND, refer to
									 *  TS 23.040 v3.8.0 9.2.2.2 and 9.2.2.4 */
	CI_MSG_PRIM_WRITE_MESSAGE_CNF,                                          /**< \brief Confirmation of storing a SMS message to the selected memory storage with operation type of CI_MSG_STRGOPERTYPE_WRITE_SEND. \details  */
	CI_MSG_PRIM_SEND_COMMAND_REQ,                                   /**< \brief Send a SMS command message to the SMSC. \details The context of pData should follow TS 23.040 v3.8.0 9.2.2.4, i.e it is a SMS-COMMAND TPDU.
									 *  This primitive is only applied to SMS messages. */
	CI_MSG_PRIM_SEND_COMMAND_CNF,                                           /**< \brief Confirmation of sending a SMS command message to the SMC. \details  */
	CI_MSG_PRIM_SEND_STORED_MESSAGE_REQ,                                    /**< \brief Send a stored SMS message in the selected memory storage with operation type of CI_MSG_STRGOPERTYPE_WRITE_SEND to the SMSC. \details  */
	CI_MSG_PRIM_SEND_STORED_MESSAGE_CNF,                                    /**< \brief Confirmation of sending a stored SMS message from the selected memory storage with operation type of CI_MSG_STRGOPERTYPE_WRITE_SEND to the SMSC. \details  */
	CI_MSG_PRIM_CONFIG_MSG_IND_REQ,                                 /**< \brief Configures whether or not receiving messages should be indicated and how. \details For SMS-Deliver messages, CBM messages and SMS-STATUS-REPORT messages, the default settings are all CI_MSG_MSG_IND_DISABLE.
									 *  CI_MSG_PRIM_SELECT_STORAGE_REQ primitive with CI_MSG_STRGOPERTYPE_RECV should be used to detect if cellular subsystem support mobile terminated SM messages and CB messages and where the received messages will be stored. */
	CI_MSG_PRIM_CONFIG_MSG_IND_CNF,                                         /**< \brief Confirmation of the configuration request to new message indication \details  */
	CI_MSG_PRIM_NEWMSG_INDEX_IND,                                           /**< \brief Indicates that a SMS message received and stored at cellular side. \details Applicable only when CI_MSG_MSG_IND_INDEX is enabled. */
	CI_MSG_PRIM_NEWMSG_IND,                                                 /**< \brief Indicates that an SMS message was received and stored at the cellular side. \details Applicable only when CI_MSG_MSG_IND_MSG is enabled. */
	CI_MSG_PRIM_NEWMSG_RSP,                                                 /**< \brief Acknowledges the reception of a SMS message from CI_MSG_PRIM_NEWMSG_IND. \details Applicable only when cellular subsystem support CI_MSG_SERVICE_WITH_ACK service and CI_MSG_MSG_IND_MSG is enabled.
										 *  If ME does not get acknowledgement within required time (network timeout), ME should send RP-ERROR to the network */
	CI_MSG_PRIM_GET_SMSC_ADDR_REQ,                                          /**< \brief Get the SMSC address, through which mobile originated SMs are transmitted \details  */
	CI_MSG_PRIM_GET_SMSC_ADDR_CNF,                                          /**< \brief Reports the SMSC address \details  */
	CI_MSG_PRIM_SET_SMSC_ADDR_REQ,                                          /**< \brief Updates the SMSC address, through which mobile originated SMs are transmitted \details  */
	CI_MSG_PRIM_SET_SMSC_ADDR_CNF,                                          /**< \brief Confirmation to update request of the SMSC address \details  */
	CI_MSG_PRIM_GET_MOSMS_SERVICE_CAP_REQ,                                  /**< \brief Gets the MO SMS service capability \details  */
	CI_MSG_PRIM_GET_MOSMS_SERVICE_CAP_CNF,                                  /**< \brief Reports the MO SMS service capability \details  */
	CI_MSG_PRIM_GET_MOSMS_SERVICE_REQ,                                      /**< \brief Gets the current MO SMS service configuration \details  */
	CI_MSG_PRIM_GET_MOSMS_SERVICE_CNF,                                      /**< \brief Reports the current MO SMS service configuration \details  */
	CI_MSG_PRIM_SET_MOSMS_SERVICE_REQ,                                      /**< \brief Sets the MO SMS service configuration \details  */
	CI_MSG_PRIM_SET_MOSMS_SERVICE_CNF,                                      /**< \brief Confirms a request to set the MO SMS service configuration \details  */
	CI_MSG_PRIM_GET_CBM_TYPES_CAP_REQ,                                      /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_MSG_PRIM_GET_CBM_TYPES_CAP_CNF,                                      /**< \brief  \details NOT SUPPORTED REMOVE FROM API  */
	CI_MSG_PRIM_GET_CBM_TYPES_REQ,                                          /**< \brief Gets the current CBM types setting \details  */
	CI_MSG_PRIM_GET_CBM_TYPES_CNF,                                          /**< \brief Reports the current CBM types setting \details  */
	CI_MSG_PRIM_SET_CBM_TYPES_REQ,                                          /**< \brief Sets a CBM types setting that decides which types of CBMs are to be received by the client \details  */
	CI_MSG_PRIM_SET_CBM_TYPES_CNF,                                          /**< \brief Confirms a request to set the CBM types \details  */
	CI_MSG_PRIM_SELECT_STORAGES_REQ,                        /**< \brief Select memory storages for read, write and receive related operation \details For type CI_MSG_STRGOPERTYPE_RECV, regardless of the storage selected, received CBMs are always stored in
								 *   CI_MSG_STORAGE_BM, received status reports are always stored in CI_MSG_STORAGE_SR unless
								 *   CI_MSG_PRIM_CONFIG_MSG_IND_REQ requests received messages to be forwarded to application subsystem directly without
								 *   storing at cellular side */
	CI_MSG_PRIM_SELECT_STORAGES_CNF,                                        /**< \brief Report a set of memory storages information being used for read, write and receive related operations. \details Cellular subsystem should have a set of default storages for different types of storage operations */

	CI_MSG_PRIM_WRITE_MSG_WITH_INDEX_REQ,                                   /**< \brief Request to write a message given an index to the current storage \details  */
	CI_MSG_PRIM_WRITE_MSG_WITH_INDEX_CNF,                                   /**< \brief Confirms a request to write a message given an index to the current storage. \details  */

	CI_MSG_PRIM_MESSAGE_WAITING_IND,                                /**< \brief Due to the current protocol stack limitations, Sac will send this notification based
									 * on the TC_DCS (data coding scheme) info provided in the SIG_APEX_SM_DELIVERY_IND and
									 * SIG_APEX_SM_STORED_IND signal \details As per TS 23.040: "The Messages Waiting is the service element that enables the PLMN to
									 * provide the HLR, SGSN and VLR with which the recipient MS is associated with the information
									 * that there is a message in the originating SC waiting to be delivered to the MS. The service
									 * element is only used in case of previous unsuccessful delivery attempt(s) due to temporarily
									 * absent mobile or MS memory capacity exceeded."
									 * "There are three levels of "Message Waiting" indication: The first level is to set the
									 * Protocol Identifier to "Return Call message". The second level uses the Data Coding Scheme
									 * TP_DCS (see 3GPP TS 23.038 [9]) to indicate the type of message waiting and whether there are
									 * some messages or no messages. The third level provides the maximum detail level for analysis by
									 * the mobile and is extracted from TP_UDH; this information shall be stored by the ME in the
									 * Message Waiting Indication Status (EF_MWIS file) on the SIM (see 3GPP TS 51.011 [16]) or
									 * USIM (see 3GPP TS 31.102 [30]) when present or otherwise should be stored in the ME."
									 *
									 * Message Waiting Status Information may be provided in the TC_DCS or TC_UDH of
									 * SMS_DELIVER, SMS_SUBMIT_REPORT, SMS_STATUS_REPORT messages, as
									 * specified in TS 23.040 (section '9.2.3.24.2	Special SMS Message Indication') and
									 * TS 23.038 (section '4. SMS Data Coding Scheme'). Where disagreement occurs, TP_UDH
									 * shall override the TP_DCS.
									 *
									 * The 'indicationActive' field provides the indication sense (how is presented to the user)
									 * for the message waiting status on systems connected to the GSM/UMTS PLMN. It is specified
									 * only by TP_DCS.
									 * The 'msgCount' field is specified only through TP_UDH. When only TP_DCS is provided, the
									 * 'msgCount' is set to 0. (Currently it will always be set to 0 because the protocol stack
									 * doesn't support the third level of MWI.) */
	CI_MSG_PRIM_STORAGE_STATUS_IND,                                 /**< \brief Notification of an Short Messages Storage status change \details The notification will be sent when a change occurs in any of the
									 * parameters listed in this primitive defition. SAC will maintain
									 * the current values of these parameters and will check their status
									 * on the receiving of the the following protocol stack signals:
									 * SmsReadyInd, SmsStatusCnf, SmsReadCnf, SmsDeliveryInd, SmsStoreInd,
									 * SmsMsgReceivedInd, SmsDeleteCnf, SmsRecordChangedInd.
									 * The 'memCapExceeded' & 'usedRecords' parameters are specified in
									 * each of these signals; the other parameters are specified by SmsReadyInd
									 * and SmsStatusCnf signals.
									 *
									 * The name of the primitive parameters is as per 3GPP spec and the actual
									 * information can be retrieved from the SIM card by reading the EF_SMS & EF_SMSS. */

	/* ADD NEW COMMON PRIMITIVES HERE, BEFORE 'CI_MSG_PRIM_LAST_COMMON_PRIM' */
/*Michal Bukai - SMS Full Feature*/
	CI_MSG_PRIM_RESET_MEMCAP_FULL_REQ,      /**< \brief Requests to reset memory full status. \details The PS will send the networkan indication that there is enough memory to send at least one SMS. The network can start forwarding the pending messages that were held due to memory full status  */
	CI_MSG_PRIM_RESET_MEMCAP_FULL_CNF,      /**< \brief Confirm the request to reset memory full status. \details */

       CI_MSG_PRIM_SET_RECORD_STATUS_REQ,  /**<\brief Requests to set RECORD status. \details The PS will update its cache with the  status to sync-up with AP  */
       CI_MSG_PRIM_SET_RECORD_STATUS_CNF,  /**<\brief confirm the requst to set RECORD status. \details  */
#ifdef AT_CUSTOMER_HTC
       CI_MSG_PRIM_SMS_REJECT_CAUSE_IND,   /**< \brief Indicates SMS reject cause code. */
#endif

	CI_MSG_PRIM_LOCK_SMS_STATUS_REQ,	/**<\brief Requests to set RECORD status. \details The PS will update its cache with the  status to sync-up with AP  */
	CI_MSG_PRIM_LOCK_SMS_STATUS_CNF,	/**<\brief confirm the requst to set RECORD status. \details  */

	/* END OF COMMON PRIMITIVES LIST */
	CI_MSG_PRIM_LAST_COMMON_PRIM

	/* the  customer specific extension primitives will be added starting from
	 * CI_MSG_PRIM_firstCustPrim = CI_MSG_PRIM_LAST_COMMON_PRIM as the first identifier.
	 * The actual primitive names and IDs are defined in the associated
	 * 'ci_msg_cust_xxx.h' file.
	 */

	/* DO NOT ADD ANY MORE PRIMITIVES HERE */

} _CiMsgPrim;

/* specify the number of default common DEV primitives */
#define CI_MSG_NUM_COMMON_PRIM ( CI_MSG_PRIM_LAST_COMMON_PRIM - 1 )
/**@}*/

/** \brief  Service type enumeration */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIMSGSERVICE_TAG {
	CI_MSG_SERVICE_NORMAL = 0,                      /**<    application subsystem will NOT acknowledge receipt of messages routed
											directly from cellular subsystem */
	CI_MSG_SERVICE_WITH_ACK,                        /**<    application subsystem will acknowledge receipt of messages routed
											directly from cellular subsystem */
	CI_MSG_SERVICE_MANUFACTURER = 128       /**<    manufacturer specific */
} _CiMsgService;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Service type enumeration.
 * \sa CIMSGSERVICE_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiMsgService;

/* specify the number of default common DEV primitives */
#define CI_MSG_SERVICE_MAX 3 /*This is a hard coded value of the number of message services that are enumerated*/
/**@}*/

/** \brief  Result code enumeration */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIRC_MSG_TAG {
	CIRC_MSG_SUCCESS = 0,                                           /**< Request completed successfully */
	CIRC_MSG_FAIL,                                                  /**< General Failure (catch-all) */

	CIRC_MSG_SERVICEME_RESERVED = 301,                              /**< SMS service of ME reserved */
	CIRC_MSG_INVALID_PDU_PARAMETER,                                 /**< invalid PDU parameter */
	CIRC_MSG_PHSIM_PIN_REQUIRED,                                    /**< SIM personalization key (CPK) required, for all */
	CIRC_MSG_INVALID_MEM_INDEX,                                     /**< invalid memory index */
	CIRC_MSG_SIM_MEM_FULL,                                          /**< SIM memory full */
	CIRC_MSG_SC_ADDR_UNKNOWN,                                       /**< SC address unknown */
	CIRC_MSG_NO_ACK_EXPECTED,                                       /**< no acknowledgement expected */
	CIRC_MSG_ERROR_UNSPEC,                                          /**< SM_CAUSE_PROTOCOL_ERROR_UNSPEC */
	CIRC_MSG_FDN_FAILURE,                                           /**< FDN failure */
	CIRC_MSG_OPERATION_IN_PROGRESS,                                 /**< request is rejected - one request is already in progress */
	CIRC_MSG_NUM_RESCODES                                           /**< number of result codes */
} _CiMsgRc;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Result code enumeration.
 * \sa CIRC_MSG_TAG */
/** \remarks Common Data Section */
typedef UINT16 CiMsgRc;
/**@}*/

/*Tal Porat/Michal Bukai - Network Selection*/

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief TP Failure-Cause.
 * \sa CIMSGRPCAUSE_TAG */
/** \remarks Common Data Section */
typedef UINT16 CiMsgCause;
/**@}*/

/** \brief MsgTypes structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiMsgMsgTypesInfo_struct {
	CiBoolean mtSupported;  /**< mobile terminated message supported flag. \sa CCI API Ref Manual */
	CiBoolean moSupported;  /**< mobile originate message supported flag. \sa CCI API Ref Manual */
	CiBoolean bmSupported;  /**< broadcast message supported flag. \sa CCI API Ref Manual */
} CiMsgMsgTypesInfo;

/** \brief Msg storage */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIMSGSTORAGE_TAG {
	CI_MSG_STORAGE_BM = 0,          /**< broadcast message storage (in volatile memory) */
	CI_MSG_STORAGE_ME,              /**< ME message storage */
	CI_MSG_STORAGE_MT,              /**< any of the storages associated with ME */
	CI_MSG_STORAGE_SM,              /**< SIM message storage */
	CI_MSG_STORAGE_SR,              /**< status report storage */

	CI_MSG_NUM_STORAGES
} _CiMsgStorage;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Msg storage.
 * \sa CIMSGSTORAGE_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiMsgStorage;
/**@}*/

/** \brief Storage operation type enumeration */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIMSGSTORAGEOPERTYPE_TAG {
	CI_MSG_STRGOPERTYPE_READ_DELETE = 0,                    /**< storage from which messages are read and deleted, default SIM */
	CI_MSG_STRGOPERTYPE_WRITE_SEND,                         /**< storage to which writing and sending operations are made */
	CI_MSG_STRGOPERTYPE_RECV,                               /**< storage to which received SMs are preferred to be stored */

	CI_MSG_NUM_STRGOPERTYPES
} _CiMsgStorageOperType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Storage operation type enumeration.
 * \sa CIMSGSTORAGEOPERTYPE_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiMsgStorageOperType;
/**@}*/

/** \brief Storage list info */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiMsgStorageList_struct {
	UINT8 len;                                                                                      /**< [1..CI_MSG_NUM_STORAGES] */
	CiMsgStorage storageList[CI_MSG_NUM_STORAGES];                                                  /**< Msg storages list. \sa CiMsgStorage */
} CiMsgStorageList;

/** \brief Msg storage info */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiMsgStorageInfo_struct {
	CiMsgStorage storage;                   /**< memory storage. \sa CiMsgStorage */
	UINT16 used;                            /**< used number of message locations in the storage */
	UINT16 total;                           /**< total number of message locations in the storage */
}CiMsgStorageInfo;

/** \brief Msg storage setting info structure */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiMsgStorageSetting_struct {
	CiMsgStorageInfo setting[CI_MSG_NUM_STRGOPERTYPES];     /**< Msg storage info. \sa CiMsgStorageInfo */
} CiMsgStorageSetting;

/** \brief Msg type */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIMSGMSGTYPE_TAG {
	CI_MSG_MSG_TYPE_DELIVER = 0,                    /**<SMS-DELIVER PDU */
	CI_MSG_MSG_TYPE_DELIVER_REPORT,                 /**< SMS-DELIVER-REPORT PDU */
	CI_MSG_MSG_TYPE_SUBMIT,                         /**< SMS-SUBMIT PDU */
	CI_MSG_MSG_TYPE_SUBMIT_REPORT,                  /**< SMS-SUBMIT-REPORT PDU */
	CI_MSG_MSG_TYPE_STATUS_REPORT,                  /**< SMS-STATUS-REPORT PDU */
	CI_MSG_MSG_TYPE_COMMAND,                        /**< SMS-COMMAND PDU */
	CI_MSG_MSG_TYPE_CBS,                            /**< CBS PDU */

	CI_MSG_MSG_NUM_TYPES
} _CiMsgMsgType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Msg type.
 * \sa CIMSGMSGTYPE_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiMsgMsgType;
/**@}*/

/** \brief Status of the message */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIMSGMSGSTATUS_TAG {
	CI_MSG_MSG_STAT_REC_UNREAD = 0,                 /**< received unread message */
	CI_MSG_MSG_STAT_READ,                           /**< received read message */
	CI_MSG_MSG_STAT_STO_UNSENT,                     /**< stored unsent message (only applicable to SMs) */
	CI_MSG_MSG_STAT_STO_SENT,                       /**< stored sent message (only applicable to SMs) */
	CI_MSG_MSG_STAT_ALL,                            /**< all messages */
	CI_MSG_MSG_STAT_REC_EMPTY,                      /**< SCR1623261: added to correct the empty slot read problem */

	CI_MSG_MSG_NUM_STAT
} _CiMsgMsgStatus;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Status of the message.
 * \sa CIMSGMSGSTATUS_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiMsgMsgStatus;
/**@}*/

/** \brief message PDU */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPdu_struct {
	UINT16 len;                                                                     /**< length of the TPDU */
	/* !!!!!!!!!!!!!!!!!!!
	 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
	 * & CCI_APP_NONCONTIGUOUS flags must be removed.
	 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
	 * # Start Contiguous Code Section # and # End Contiguous Code Section #
	 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
	 */
#ifndef CCI_MSG_CONTIGUOUS
	UINT8   *pData; /**< pointer to the TPDU (Transfer Protocol Data Unit).
			   SMS message TPDU format follows TS 23.040 v3.8.0 9.2;
			   CBS message TPDU format follows TS 23.041 v3.4.0 9.4.1 */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	UINT8   *pDataTempNonContPtr; /*Temporary pointer used to ensure the correct structure size for a non contiguous app*/
#endif
/* # Start Contiguous Code Section#*/
	UINT8 data[ CI_MAX_CI_MSG_PDU_SIZE ];           /**< TPDU (Transfer Protocol Data Unit) */
/* # END Contiguous Code Section#*/
#endif
} CiMsgPdu;

/** \brief Del Flag enumeration */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIMSGMSGDELFLAG_TAG {
	CI_MSG_MSG_DELFLAG_INDEX = 0,                           /**< delete message at the specified index */
	CI_MSG_MSG_DELFLAG_ALL_READ,                            /**< delete all messages that have been read */
	CI_MSG_MSG_DELFLAG_ALL_READ_OR_SENT,                    /**< delete all messages that have been read or sent */
	CI_MSG_MSG_DELFLAG_ALL_READ_OR_MO,                      /**< delete all messages that have been read or mobile orginated */

	CI_MSG_MSG_NUM_DELFLAGS
} _CiMsgMsgDelFlag;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Common Data Section.
 * \sa CIMSGMSGDELFLAG_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiMsgMsgDelFlag;
/**@}*/

/** \brief Message Indication setting enumeration */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIMSGMSGINDSETTING_TAG {
	CI_MSG_MSG_IND_DISABLE = 0,             /**< store received message at cellular side,
										   disable indication of new message received */
	CI_MSG_MSG_IND_INDEX,                   /**< store received message at cellular side,
										   indicate memory storage and memory location */
	CI_MSG_MSG_IND_MSG,                     /**< don't store received message at cellular side,
										   forward message to application side directly */
	CI_MSG_MSG_IND_CLASS3_MSG,      /**< don't store CLASS 3 message (SM/CBM) at
										  cellular side  - forward the message to application side directly */
	CI_MSG_MSG_NUM_INDS
} _CiMsgMsgIndSetting;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Message Indication setting enumeration.
 * \sa CIMSGMSGINDSETTING_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiMsgMsgIndSetting;
/**@}*/

/** \brief MO SMS Service Info enumeration */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CIMSGMOSMSSRVCFG_TAG {
	CI_MSG_MOSMS_SRV_PS = 0,                                /**< PS domain */
	CI_MSG_MOSMS_SRV_CS,                                    /**< CS domain */
	CI_MSG_MOSMS_SRV_PS_PREFERRED,                          /**< PS preferred, use CS if PS not available */
	CI_MSG_MOSMS_SRV_CS_PREFERRED,                          /**< CS preferred, use PS if CS not available */
	CI_MSG_MOSMS_NUM_SRVS
} _CiMsgMoSmsSrvCfg;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief MO SMS Service Info enumeration.
 * \sa CIMSGMOSMSSRVCFG_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiMsgMoSmsSrvCfg;
/**@}*/

/** \brief CBM message Types Set info */
/** \remarks Common Data Section */
//ICAT EXPORTED STRUCT
typedef struct CiMsgCbmTypesSet_struct {
	CiBoolean mode;                         /**< mode, TRUE: enable mids and dcss; FALSE: disable; \sa CCI API Ref Manual */
	CiNumericList mids;                     /**< CBM message identifiers, see 3GPP TS 23.041, v3.4.0, 9.4.1.2.2; \sa CCI API Ref Manual */
	CiNumericList dcss;                     /**< CBM data coding schemes, see 3GPP TS 23.038, v3.3.0, 5; \sa CCI API Ref Manual */
} CiMsgCbmTypesSet;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_GET_SUPPORTED_SERVICES_REQ"> */
typedef CiEmptyPrim CiMsgPrimGetSupportedServicesReq;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_GET_SUPPORTED_SERVICES_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimGetSupportedServicesCnf_struct {
	CiMsgRc rc;                                                                                             /**< Result code. \sa CiMsgRc */
	UINT8 len;                                                                                              /**< Length to the supported SMS services list [1- CI_MSG_MAX_SUPPORTED_SERVICES] */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */
#ifndef CCI_MSG_CONTIGUOUS
	CiMsgService                        *pServiceLst;                                               /**< Supported SMS services list. \sa CiMsgService */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	CiMsgService             *pServiceLstTempNonContPtr;            /*Temporary pointer used to ensure the correct structure size for a non contiguous app*/
#endif
/* # Start Contiguous Code Section # */
	CiMsgService serviceLst[CI_MSG_SERVICE_MAX];                            /**< Supported SMS services list. \sa CiMsgService */
/* # End Contiguous Code Section # */
#endif
} CiMsgPrimGetSupportedServicesCnf;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_SELECT_SERVICE_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimSelectServiceReq_struct {
	CiMsgService service;                           /**< SMS message service. \sa CiMsgService */
} CiMsgPrimSelectServiceReq;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_SELECT_SERVICE_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimSelectServiceCnf_struct {
	CiMsgRc rc;                                                             /**<  Result code applicable values are: CIRC_MSG_SUCCESS or CIRC_MSG_SERVICEME_RESERVED. \sa CiMsgRc  */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */
#ifndef CCI_MSG_CONTIGUOUS
	CiMsgMsgTypesInfo           *pMsgTypesInfo;     /**< types of message supported, optional if rc is not CIRC_MSG_SUCCESS. \sa CiMsgMsgTypesInfo_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	CiMsgMsgTypesInfo           *pMsgTypesInfoTempNonContPtr; /*Temporary pointer used to ensure the correct structure size for a non contiguous app*/
#endif
/* # Start Contiguous Code Section #*/
	CiMsgMsgTypesInfo msgTypesInfo;                 /**< types of message supported, optional if rc is not CIRC_MSG_SUCCESS. \sa CiMsgMsgTypesInfo_struct */
/* # End Contiguous Code Section #*/
#endif
} CiMsgPrimSelectServiceCnf;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_GET_CURRENT_SERVICE_INFO_REQ"> */
typedef CiEmptyPrim CiMsgPrimGetCurrentServiceInfoReq;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_GET_CURRENT_SERVICE_INFO_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimGetCurrentServiceInfoCnf_struct {
	CiMsgRc rc;                                                             /**< Result code. \sa CiMsgRc */
	CiMsgService service;                                                   /**< Current SMS service setting. \sa CiMsgService */
/* !!!!!!!!!!!!!!!!!!!
 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
 * & CCI_APP_NONCONTIGUOUS flags must be removed.
 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
 * # Start Contiguous Code Section # and # End Contiguous Code Section #
 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
 */
#ifndef CCI_MSG_CONTIGUOUS
	CiMsgMsgTypesInfo                   *pMsgTypesInfo; /**< Types of message supported by current SMS messaging service. \sa CiMsgMsgTypesInfo_struct  */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	CiMsgMsgTypesInfo                   *pMsgTypesInfoTempNonContPtr; /*Temporary pointer used to ensure the correct structure size for a non contiguous app*/
#endif
/* # Start Contiguous Code Section # */
	CiMsgMsgTypesInfo msgTypesInfo;                 /**< Types of message supported by current SMS messaging service. \sa CiMsgMsgTypesInfo_struct  */
/* # End Contiguous Code Section # */
#endif

} CiMsgPrimGetCurrentServiceInfoCnf;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_GET_SUPPORTED_STORAGES_REQ"> */
typedef CiEmptyPrim CiMsgPrimGetSupportedStoragesReq;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_GET_SUPPORTED_STORAGES_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimGetSupportedStoragesCnf_struct {
	CiMsgRc rc;                                                                                                     /**< Result code. \sa CiMsgRc */
	UINT8 len;                                                                                                      /**< Length of the storage lists. */
	CiMsgStorageList lists[ CI_MSG_NUM_STRGOPERTYPES ];
} CiMsgPrimGetSupportedStoragesCnf;


/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_SELECT_STORAGE_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimSelectStorageReq_struct {
	CiMsgStorageOperType type;                              /**< Storage operation type. \sa CiMsgStorageOperType */
	CiMsgStorage storage;                                   /**< Storage to be associated with the storage operation type. \sa CiMsgStorage */
} CiMsgPrimSelectStorageReq;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_SELECT_STORAGE_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimSelectStorageCnf_struct {
	CiMsgRc rc;                                             /**< Result code. \sa CiMsgRc */
	/* !!!!!!!!!!!!!!!!!!!
	 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
	 * & CCI_APP_NONCONTIGUOUS flags must be removed.
	 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
	 * # Start Contiguous Code Section # and # End Contiguous Code Section #
	 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
	 */
#ifndef CCI_MSG_CONTIGUOUS
	CiMsgStorageSetting         *pSet;      /**< set of memory storages information being used, optional when rc is not CIRC_MSG_SUCCESS. \sa CiMsgStorageSetting_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	CiMsgStorageSetting         *pSetTempNonContPtr; /*Temporary pointer used to ensure the correct structure size for a non contiguous app*/
#endif
/* # Start Contiguous Code Section #*/
	CiMsgStorageSetting set;                        /**< set of memory storages information being used, optional when rc is not CIRC_MSG_SUCCESS. \sa CiMsgStorageSetting_struct */
/* # End Contiguous Code Section #*/
#endif
} CiMsgPrimSelectStorageCnf;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_GET_CURRENT_STORAGE_INFO_REQ"> */
typedef CiEmptyPrim CiMsgPrimGetCurrentStorageInfoReq;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_GET_CURRENT_STORAGE_INFO_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimGetCurrentStorageInfoCnf_struct {
	CiMsgRc rc;                                                             /**< Result code. \sa CiMsgRc */
	/* !!!!!!!!!!!!!!!!!!!
	 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
	 * & CCI_APP_NONCONTIGUOUS flags must be removed.
	 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
	 * # Start Contiguous Code Section # and # End Contiguous Code Section #
	 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
	 */
#ifndef CCI_MSG_CONTIGUOUS
	CiMsgStorageSetting         *pSet;                      /**< Current memory storages being used. \sa CiMsgStorageSetting_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	CiMsgStorageSetting         *pSetTempNonContPtr; /*Temporary pointer used to ensure the correct structure size for a non contiguous app*/
#endif
/* # Start Contiguous Code Section # */
	CiMsgStorageSetting set;                                        /**< Current memory storages being used. \sa CiMsgStorageSetting_struct */
/* # End Contiguous Code Section # */
#endif
} CiMsgPrimGetCurrentStorageInfoCnf;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_READ_MESSAGE_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimReadMessageReq_struct {
	UINT16 index;           /**< Index into the memory storage. */
} CiMsgPrimReadMessageReq;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_READ_MESSAGE_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimReadMessageCnf_struct {
	CiMsgRc rc;                                                     /**< Result code. \sa CiMsgRc */
	UINT8 status;                                                   /**< Status of the message. \sa CiMsgMsgStatus */
	/* !!!!!!!!!!!!!!!!!!!
	 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
	 * & CCI_APP_NONCONTIGUOUS flags must be removed.
	 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
	 * # Start Contiguous Code Section # and # End Contiguous Code Section #
	 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
	 */
#ifndef CCI_MSG_CONTIGUOUS
	CiMsgPdu    *pPdu;                              /**< The read message. \sa CiMsgPdu_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	CiMsgPdu    *pPduTempNonContPtr; /*Temporary pointer used to ensure the correct structure size for a non contiguous app*/
#endif
	/*# Start Contiguous Code Section #*/
	CiBoolean pduPresent;                                   /**< TRUE - if present; \sa CCI API Ref Manual */
	CiMsgPdu pdu;                                           /**< The read message. \sa CiMsgPdu_struct */
	/*# End Contiguous Code Section #*/
#endif
	CiOptAddressInfo optSca;                                /**< SCR #1777605: Optional Service Center Address */
} CiMsgPrimReadMessageCnf;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_DELETE_MESSAGE_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimDeleteMessageReq_struct {
	CiMsgMsgDelFlag flag;                   /**< Delete flag, default: CI_MSG_MSG_DELFLAG_INDEX. \sa CiMsgMsgDelFlag */
	UINT16 index;                           /**< Index, ignored if flag is not CI_MSG_MSG_DELFLAG_INDEX. */
} CiMsgPrimDeleteMessageReq;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_DELETE_MESSAGE_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimDeleteMessageCnf_struct {
	CiMsgRc rc;     /**< Result code. \sa CiMsgRc */
} CiMsgPrimDeleteMessageCnf;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_SEND_MESSAGE_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimSendMessageReq_struct
{
	/* !!!!!!!!!!!!!!!!!!!
	 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
	 * & CCI_APP_NONCONTIGUOUS flags must be removed.
	 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
	 * # Start Contiguous Code Section # and # End Contiguous Code Section #
	 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
	 */
#ifndef CCI_MSG_CONTIGUOUS
	CiMsgPdu            *pPdu;                      /**< the send message. \sa CiMsgPdu_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	CiMsgPdu            *pPduTempNonContPtr; /*Temporary pointer used to ensure the correct structure size for a non contiguous app*/
#endif
/* # Start Contiguous Code Section #*/
	CiMsgPdu pdu;                                   /**< the send message. \sa CiMsgPdu_struct */
/* # End Contiguous Code Section #*/
#endif
	CiBoolean bMoreMessage;                                 /**< TRUE - send SIG_APEX_SM_SEND_MORE_REQ to PS. \sa CCI API Ref Manual */
	UINT8 res1U8[3];                                        /**< (padding) */
	CiOptAddressInfo optSca;                                /**< SCR #1777605: Optional Service Center Address. \sa CCI API Ref Manual */

} CiMsgPrimSendMessageReq;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_SEND_MESSAGE_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimSendMessageCnf_struct {
	CiMsgRc rc;
	UINT8 reference;
	/*Tal Porat/Michal Bukai - Network Selection*/
	UINT8 res1U8;         /* (padding) */
	CiMsgCause rpCause;
	CiMsgCause tpCause;
	/*Tal Porat/Michal Bukai - Network Selection*/
	/* !!!!!!!!!!!!!!!!!!!
	 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
	 * & CCI_APP_NONCONTIGUOUS flags must be removed.
	 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
	 * # Start Contiguous Code Section # and # End Contiguous Code Section #
	 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
	 */
#ifndef CCI_MSG_CONTIGUOUS
	CiMsgPdu            *pAckPdu;                   /**< RP-User-Data element of RP-ACK PDU, i.e. SMS-SUBMIT-REPORT TPDU for RP-ACK, refer
							 *    to TS 23.040 v3.8.0 9.2.2.2a, valid when CI_MSG_SERVICE_WITH_ACK is enabled and
							 *    network supports it. \sa CiMsgPdu_struct*/
#else
#ifdef CCI_APP_NONCONTIGUOUS
	CiMsgPdu            *pAckPduTempNonContPtr; /*Temporary pointer used to ensure the correct structure size for a non contiguous app*/
#endif
/* # Start Contiguous Code Section # */
	CiBoolean ackPduPresent;                        /**< TRUE - if present; \sa CCI API Ref Manual */
	CiMsgPdu ackPdu;                                /**< RP-User-Data element of RP-ACK PDU, i.e. SMS-SUBMIT-REPORT TPDU for RP-ACK, refer
							 *    to TS 23.040 v3.8.0 9.2.2.2a, valid when CI_MSG_SERVICE_WITH_ACK is enabled and
							 *    network supports it. \sa CiMsgPdu_struct*/
/* # End Contiguous Code Section # */
#endif
} CiMsgPrimSendMessageCnf;



/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_WRITE_MESSAGE_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimWriteMessageReq_struct {

	/* !!!!!!!!!!!!!!!!!!!
	 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
	 * & CCI_APP_NONCONTIGUOUS flags must be removed.
	 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
	 * # Start Contiguous Code Section # and # End Contiguous Code Section #
	 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
	 */
#ifndef CCI_MSG_CONTIGUOUS
	CiMsgPdu                *pPdu;                                  /**< PDU data structure. \sa CiMsgPdu_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	CiMsgPdu                *pPduTempNonContPtr; /*Temporary pointer used to ensure the correct structure size for a non contiguous app*/
#endif
/* # Start Contiguous Code Section #*/
	CiMsgPdu pdu;                                                           /**< PDU data structure. \sa CiMsgPdu_struct */
/* # End Contiguous Code Section #*/
#endif
	CiMsgMsgStatus status;                                                  /**< Message status to be set with the message. \sa CiMsgMsgStatus */
	CiBoolean statusPresent;                                                /**< TRUE - if present; \sa CCI API Ref Manual  */
	UINT8 res1U8[2];                                                        /**< (pading) */
	CiOptAddressInfo optSca;                                                /**< optional service center address. \sa CCI API Ref Manual  */
} CiMsgPrimWriteMessageReq;

/* <INUSE> */
/** CI_MSG_PRIM_WRITE_MESSAGE_CNF - Confirmation of storing a SMS message to the selected memory storage with operation type of CI_MSG_STRGOPERTYPE_WRITE_SEND.
 */
/** <paramref name="CI_MSG_PRIM_WRITE_MESSAGE_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimWriteMessageCnf_struct {
	CiMsgRc rc;                                                             /**< Result code. \sa CiMsgRc */
	UINT16 index;                                                           /**< Memory location of the stored message. */
} CiMsgPrimWriteMessageCnf;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_SEND_COMMAND_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimSendCommandReq_struct {
	/* !!!!!!!!!!!!!!!!!!!
	 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
	 * & CCI_APP_NONCONTIGUOUS flags must be removed.
	 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
	 * # Start Contiguous Code Section # and # End Contiguous Code Section #
	 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
	 */
#ifndef CCI_MSG_CONTIGUOUS
	CiMsgPdu    *pPdu;      /**< the send message. \sa CiMsgPdu_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	CiMsgPdu    *pPduTempNonContPtr; /*Temporary pointer used to ensure the correct structure size for a non contiguous app*/
#endif
/* # Start Contiguous Code Section #*/
	CiMsgPdu pdu;           /**< the send message. \sa CiMsgPdu_struct */
/* # End Contiguous Code Section #*/
#endif
} CiMsgPrimSendCommandReq;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_SEND_COMMAND_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimSendCommandCnf_struct {
	CiMsgRc rc;                                             /**< Result code. \sa CiMsgRc */
	UINT8 reference;                                        /**< TP-Message-Reference, refer to TS 23.040 v3.8.0 9.2.3.6 */
	/* !!!!!!!!!!!!!!!!!!!
	 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
	 * & CCI_APP_NONCONTIGUOUS flags must be removed.
	 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
	 * # Start Contiguous Code Section # and # End Contiguous Code Section #
	 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
	 */
#ifndef CCI_MSG_CONTIGUOUS
	CiMsgPdu    *pAckPdu;                   /**< RP-User-Data element of RP-ACK PDU, i.e. SMS-SUBMIT-REPORT TPDU for RP-ACK, refer to
						 *  TS 23.040 v3.8.0 9.2.2.2a, valid when CI_MSG_SERVICE_WITH_ACK is enabled and network
						 *  supports it. \sa CiMsgPdu_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	CiMsgPdu    *pAckPduTempNonContPtr; /*Temporary pointer used to ensure the correct structure size for a non contiguous app*/
#endif
/* # Start Contiguous Code Section # */
	CiBoolean ackPduPresent;                /**< TRUE - if present; \sa CCI API Ref Manual */
	CiMsgPdu ackPdu;                        /**< RP-User-Data element of RP-ACK PDU, i.e. SMS-SUBMIT-REPORT TPDU for RP-ACK, refer to
						 *  TS 23.040 v3.8.0 9.2.2.2a, valid when CI_MSG_SERVICE_WITH_ACK is enabled and network
						 *  supports it. \sa CiMsgPdu_struct */
/* # End Contiguous Code Section # */
#endif
} CiMsgPrimSendCommandCnf;



/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_SEND_STORED_MESSAGE_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimSendStoredMessageReq_struct
{
	/* !!!!!!!!!!!!!!!!!!!
	 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
	 * & CCI_APP_NONCONTIGUOUS flags must be removed.
	 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
	 * # Start Contiguous Code Section # and # End Contiguous Code Section #
	 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
	 */
#ifndef CCI_MSG_CONTIGUOUS
	CiAddressInfo               *pDestAddr; /**<Optional TP-Destination-Address, to replace the address in the stored message. \sa CCI API Ref Manual */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	CiAddressInfo               *pDestAddrTempNonContPtr; /*Temporary pointer used to ensure the correct structure size for a non contiguous app. \sa CCI API Ref Manual*/
#endif
/* # Start Contiguous Code Section # */
	CiAddressInfo destAddr;                 /**<Optional TP-Destination-Address, to replace the address in the stored message. \sa CCI API Ref Manual */
/* # End Contiguous Code Section # */
#endif
	UINT16 index;                                           /**< memory location. */
	CiBoolean bMoreMessage;                                 /**< TRUE - send SIG_APEX_SM_SEND_MORE_REQ to PS; \sa CCI API Ref Manual */
	CiBoolean destAddrPresent;                              /**< , TP-Destination-Address, used to replace the address in the stored message. \sa CCI API Ref Manual */
	CiOptAddressInfo optSca;                                /**< Optional service center address. \sa CCI API Ref Manual */

} CiMsgPrimSendStoredMessageReq;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_SEND_STORED_MESSAGE_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimSendStoredMessageCnf_struct {
	CiMsgRc rc;                                                     /**< Result code. \sa CiMsgRc */
	UINT8 reference;                                                /**< TP-Message-Reference, refer to TS 23.040 v3.8.0 9.2.3.6 */
	/*Tal Porat/Michal Bukai - Network Selection*/
	UINT8 res1U8;                                                   /* (padding) */
	CiMsgCause rpCause;
	CiMsgCause tpCause;
	/*Tal Porat/Michal Bukai - Network Selection*/
	/* !!!!!!!!!!!!!!!!!!!
	 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
	 * & CCI_APP_NONCONTIGUOUS flags must be removed.
	 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
	 * # Start Contiguous Code Section # and # End Contiguous Code Section #
	 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
	 */
#ifndef CCI_MSG_CONTIGUOUS
	CiMsgPdu    *pAckPdu;                           /**< RP-User-Data element of RP-ACK PDU, i.e. SMS-SUBMIT-REPORT TPDU for RP-ACK, refer to
							 *    TS 23.040 v3.8.0 9.2.2.2a, valid when CI_MSG_SERVICE_WITH_ACK is enabled and network
							 *    supports it. \sa CiMsgPdu_struct*/
#else
#ifdef CCI_APP_NONCONTIGUOUS
	CiMsgPdu    *pAckPduTempNonContPtr; /*Temporary pointer used to ensure the correct structure size for a non contiguous app*/
#endif
/* # Start Contiguous Code Section # */
	CiBoolean ackPduPresent;                        /**< TRUE - if present; */
	CiMsgPdu ackPdu;                                /**< RP-User-Data element of RP-ACK PDU, i.e. SMS-SUBMIT-REPORT TPDU for RP-ACK, refer to
							 *    TS 23.040 v3.8.0 9.2.2.2a, valid when CI_MSG_SERVICE_WITH_ACK is enabled and network
							 *    supports it. \sa CiMsgPdu_struct*/
/* # End Contiguous Code Section # */
#endif
} CiMsgPrimSendStoredMessageCnf;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_CONFIG_MSG_IND_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimConfigMsgIndReq_struct {
	CiMsgMsgIndSetting smsDeliverIndSetting;                /**< Indication setting for SMS-Deliver messages. \sa CiMsgMsgIndSetting */
	CiMsgMsgIndSetting cbmIndSetting;                       /**< Indication setting for CBM messages. \sa CiMsgMsgIndSetting */
	CiMsgMsgIndSetting smsStatusReportIndSetting;           /**< Indication setting for SMS-STATUS-REPORT messages. \sa CiMsgMsgIndSetting */
} CiMsgPrimConfigMsgIndReq;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_CONFIG_MSG_IND_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimConfigMsgIndCnf_struct {
	CiMsgRc rc;                                                                             /**< Result code. \sa CiMsgRc */
} CiMsgPrimConfigMsgIndCnf;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_NEWMSG_INDEX_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimNewMsgIndexInd_struct {
	CiMsgMsgType type;                                      /**< Type of the received new message, applicable values are:CI_MSG_MSG_TYPE_DELIVER, CI_MSG_MSG_TYPE_STATUS_REPORT or CI_MSG_MSG_TYPE_CBS. \sa CiMsgMsgType */
	CiMsgStorage storage;                                   /**< Where the message is stored. \sa CiMsgStorage */
	UINT16 index;                                           /**< Memory location. */
} CiMsgPrimNewMsgIndexInd;

/** \brief  Message Coding Tag */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CbCIMessageCoding_Tag
{
	CB_CI_DEFAULT_ALPHABET,
	CB_CI_8_BIT_DATA,
	CB_CI_UCS2_ALPHABET,
	CI_noMoreCodings
}_CbCIMessageCoding;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Message Coding Tag
* \sa CbCIMessageCoding_Tag */
/** \remarks Common Data Section */
typedef UINT8 CbCIMessageCoding;
/**@}*/

/*Michal Bukai - SMS Memory Full Feature*/
/** \brief  Message Type Tag  */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CiMsgStoreDeliveryMsgType_Tag
{
	MSG_STORE_MSG_TYPE,
	MSG_DELIVERY_MSG_TYPE,
	MSG_STORE_DELIVERY_MSG_TYPE_NUM
} _CiMsgStoreDeliveryMsgType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Message Coding Tag
 * \sa CiMsgStoreDeliveryMsgType_Tag */
/** \remarks Common Data Section */
typedef UINT8 CiMsgStoreDeliveryMsgType;
/**@}*/
/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_NEWMSG_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimNewMsgInd_struct {
	CiMsgMsgType type;                                                                      /**< type of the received new message [CI_MSG_MSG_TYPE_DELIVER| CI_MSG_MSG_TYPE_STATUS_REPORT| CI_MSG_MSG_TYPE_CBS]. \sa CiMsgMsgType */
	/*Michal Bukai - SMS Memory Full Feature*/
	INT8 ShortMsgId;                                                                        /**< Short message id. */
	CiMsgStoreDeliveryMsgType msgType;                                                      /**< \sa CiMsgStoreDeliveryMsgType */
	INT16 commandRef;                                                                       /**< Additional reference to hold ABSM transaction */
	UINT8 res1U8[2];                                                                        /* Padding */
	CbCIMessageCoding messageCoding;                                                        /**< Message coding. \sa CbCIMessageCoding */
	/* !!!!!!!!!!!!!!!!!!!
	 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
	 * & CCI_APP_NONCONTIGUOUS flags must be removed.
	 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
	 * # Start Contiguous Code Section # and # End Contiguous Code Section #
	 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
	 */
#ifndef CCI_MSG_CONTIGUOUS
	CiMsgPdu                    *pPdu;                      /**< The received new message. \sa CiMsgPdu_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	CiMsgPdu                    *pPduTempNonContPtr; /*Temporary pointer used to ensure the correct structure size for a non contiguous app*/
#endif
/* # Start Contiguous Code Section # */
	CiMsgPdu pdu;                                           /**< The received new message. \sa CiMsgPdu_struct */
/* # End Contiguous Code Section # */
#endif
	CiOptAddressInfo optSca; /**< Optional Service Center Address. \sa CCI API Ref Manual */                                // SCR #1777605: Optional Service Center Address
} CiMsgPrimNewMsgInd;

/* <NOTINUSE> */
/** <paramref name="CI_MSG_PRIM_NEWMSG_RSP"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimNewMsgRsp_struct {
	CiBoolean success;                                                              /**< TRUE if acknowledgement is positive; FALSE if acknowledgement is negative; \sa CCI API Ref Manual */
	/*Michal Bukai - SMS Memory Full Feature*/
	CiBoolean memory_full;                                                          /**< TRUE if ME memory is full; \sa CCI API Ref Manual */
	CiMsgStoreDeliveryMsgType msgType;                                              /**< \sa CiMsgStoreDeliveryMsgType */
	INT8 ShortMsgId;                                                                /**< Short message id. */
	INT16 commandRef;                                                               /**< Additional reference to hold ABSM transaction */
	UINT8 res1U8[2];                                                                /* Padding */
	/* !!!!!!!!!!!!!!!!!!!
	 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
	 * & CCI_APP_NONCONTIGUOUS flags must be removed.
	 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
	 * # Start Contiguous Code Section # and # End Contiguous Code Section #
	 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
	 */
#ifndef CCI_MSG_CONTIGUOUS
	CiMsgPdu  *pPdu;                        /**< Not in use; \sa  CiMsgPdu_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	CiMsgPdu  *pPduTempNonContPtr; /*Temporary pointer used to ensure the correct structure size for a non contiguous app*/
#endif
/* # Start Contiguous Code Section # */
	CiBoolean pduPresent;                   /**< Not in use */
	CiMsgPdu pdu;                           /**< Not in use; \sa  CiMsgPdu_struct */
/* # End Contiguous Code Section # */
#endif
} CiMsgPrimNewMsgRsp;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_GET_SMSC_ADDR_REQ"> */
typedef CiEmptyPrim CiMsgPrimGetSmscAddrReq;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_GET_SMSC_ADDR_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimGetSmscAddrCnf_struct {
	CiMsgRc rc;                                             /**< Result code. \sa CiMsgRc */
	/* !!!!!!!!!!!!!!!!!!!
	 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
	 * & CCI_APP_NONCONTIGUOUS flags must be removed.
	 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
	 * # Start Contiguous Code Section # and # End Contiguous Code Section #
	 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
	 */
#ifndef CCI_MSG_CONTIGUOUS
	CiAddressInfo               *pSca;              /**< service center address. For details see CommonTypes document. \sa CCI API Ref Manual */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	CiAddressInfo               *pScaTempNonContPtr; /*Temporary pointer used to ensure the correct structure size for a non contiguous app*/
#endif
/* # Start Contiguous Code Section # */
	CiBoolean scaPresent;                                   /**< TRUE - if present; \sa CCI API Ref Manual */
	CiAddressInfo sca;                                      /**< service center address. For details see CommonTypes document. \sa CCI API Ref Manual */
/* # End Contiguous Code Section # */
#endif
} CiMsgPrimGetSmscAddrCnf;


/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_SET_SMSC_ADDR_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimSetSmscAddrReq_struct {
	/* !!!!!!!!!!!!!!!!!!!
	 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
	 * & CCI_APP_NONCONTIGUOUS flags must be removed.
	 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
	 * # Start Contiguous Code Section # and # End Contiguous Code Section #
	 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
	 */
#ifndef CCI_MSG_CONTIGUOUS
	CiAddressInfo  *pSca;                           /**< Pointer to service center address. \sa CCI API Ref Manual  */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	CiAddressInfo  *pScaTempNonContPtr; /*Temporary pointer used to ensure the correct structure size for a non contiguous app*/
#endif
/* # Start Contiguous Code Section # */
	CiAddressInfo sca;                              /**< Service center address. \sa CCI API Ref Manual  */
/* # End Contiguous Code Section # */
#endif
} CiMsgPrimSetSmscAddrReq;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_SET_SMSC_ADDR_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimSetSmscAddrCnf_struct {
	CiMsgRc rc;             /**< Result code. \sa CiMsgRc */
} CiMsgPrimSetSmscAddrCnf;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_GET_MOSMS_SERVICE_CAP_REQ"> */
typedef CiEmptyPrim CiMsgPrimGetMoSmsServiceCapReq;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_GET_MOSMS_SERVICE_CAP_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimGetMoSmsServiceCapCnf_struct {
	CiMsgRc rc;                                             /**< Result code. \sa CiMsgRc */
	CiBitRange bitsMoSmsSrvCfg;                             /**< supported MO SMS services. \sa CCI API Ref Manual */
} CiMsgPrimGetMoSmsServiceCapCnf;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_GET_MOSMS_SERVICE_REQ"> */
typedef CiEmptyPrim CiMsgPrimGetMoSmsServiceReq;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_GET_MOSMS_SERVICE_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimGetMoSmsServiceCnf_struct {
	CiMsgRc rc;                                     /**< Result code. \sa CiMsgRc */
	CiMsgMoSmsSrvCfg cfg;                           /**< MO SMS service configuration. \sa CiMsgMoSmsSrvCfg */
} CiMsgPrimGetMoSmsServiceCnf;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_SET_MOSMS_SERVICE_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimSetMoSmsServiceReq_struct {
	CiMsgMoSmsSrvCfg cfg;                   /**< MO SMS service info. \sa CiMsgMoSmsSrvCfg */
} CiMsgPrimSetMoSmsServiceReq;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_SET_MOSMS_SERVICE_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimSetMoSmsServiceCnf_struct {
	CiMsgRc rc;                     /**< Result code. \sa CiMsgRc */
} CiMsgPrimSetMoSmsServiceCnf;

typedef CiEmptyPrim CiMsgPrimGetCbmTypesCapReq;

//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimGetCbmTypesCapCnf_struct {
	CiMsgRc rc;
	CiBitRange bitsMode;
} CiMsgPrimGetCbmTypesCapCnf;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_GET_CBM_TYPES_REQ"> */
typedef CiEmptyPrim CiMsgPrimGetCbmTypesReq;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_GET_CBM_TYPES_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimGetCbmTypesCnf_struct {
	CiMsgRc rc;                                                             /**< Result code. \sa CiMsgRc */
	/* !!!!!!!!!!!!!!!!!!!
	 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
	 * & CCI_APP_NONCONTIGUOUS flags must be removed.
	 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
	 * # Start Contiguous Code Section # and # End Contiguous Code Section #
	 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
	 */
#ifndef CCI_MSG_CONTIGUOUS
	CiMsgCbmTypesSet    *pSet;                      /**< CBM types setting. \sa CiMsgCbmTypesSet_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	CiMsgCbmTypesSet    *pSetTempNonContPtr; /*Temporary pointer used to ensure the correct structure size for a non contiguous app*/
#endif
/* # Start Contiguous Code Section # */
	CiBoolean setPresent;                                           /**< TRUE - if present;*/
	CiMsgCbmTypesSet set;                                           /**< CBM types setting. \sa CiMsgCbmTypesSet_struct */
/* # End Contiguous Code Section # */
#endif
} CiMsgPrimGetCbmTypesCnf;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_SET_CBM_TYPES_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimSetCbmTypesReq_struct {
	/* !!!!!!!!!!!!!!!!!!!
	 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
	 * & CCI_APP_NONCONTIGUOUS flags must be removed.
	 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
	 * # Start Contiguous Code Section # and # End Contiguous Code Section #
	 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
	 */
#ifndef CCI_MSG_CONTIGUOUS
	CiMsgCbmTypesSet    *pSet;                      /**< CBM types setting. \sa CiMsgCbmTypesSet_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	CiMsgCbmTypesSet    *pSetTempNonContPtr; /*Temporary pointer used to ensure the correct structure size for a non contiguous app*/
#endif
/* # Start Contiguous Code Section # */
	CiMsgCbmTypesSet set;                                   /**< CBM types setting. \sa CiMsgCbmTypesSet_struct */
/* # End Contiguous Code Section # */
#endif
} CiMsgPrimSetCbmTypesReq;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_SET_CBM_TYPES_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimSetCbmTypesCnf_struct {
	CiMsgRc rc;                                                             /**< Result code. \sa CiMsgRc */
} CiMsgPrimSetCbmTypesCnf;


/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_SELECT_STORAGES_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimSelectStoragesReq_struct {
	CiBitRange bitsType;                                                                                            /**< Bit maps of CiMsgStorageOperType. For details see CommonTypes document. \sa CCI API Ref Manual */
	CiMsgStorage storages[CI_MSG_NUM_STRGOPERTYPES];                                                                /**< Storages to be associated with the storage operation type. \sa CiMsgStorage */
} CiMsgPrimSelectStoragesReq;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_SELECT_STORAGES_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimSelectStoragesCnf_struct {
	CiMsgRc rc;                                                             /**< Result code. \sa CiMsgRc */
	CiBitRange bitsType;                                                    /**< Bit maps of CiMsgStorageOperType required. \sa CCI API Ref Manual */
	/* !!!!!!!!!!!!!!!!!!!
	 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
	 * & CCI_APP_NONCONTIGUOUS flags must be removed.
	 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
	 * # Start Contiguous Code Section # and # End Contiguous Code Section #
	 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
	 */
#ifndef CCI_MSG_CONTIGUOUS
	CiMsgStorageSetting  *pSet;                             /**< set of memory storages information being used, optional when rc is not CIRC_MSG_SUCCESS. \sa CiMsgStorageSetting_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	CiMsgStorageSetting  *pSetNonContPtr;           /*Temporary pointer used to ensure the correct structure size for a non contiguous app*/
#endif
/* # Start Contiguous Code Section # */
	CiMsgStorageSetting set;                                        /**< set of memory storages information being used, optional when rc is not CIRC_MSG_SUCCESS. \sa CiMsgStorageSetting_struct */
/* # End Contiguous Code Section # */
#endif
} CiMsgPrimSelectStoragesCnf;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_WRITE_MSG_WITH_INDEX_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimWriteMsgWithIndexReq_struct {
	UINT16 index;                                                   /**< Index of the message to be written. */
	CiMsgMsgStatus status;                                          /**< Message status. \sa CiMsgMsgStatus */
	/* !!!!!!!!!!!!!!!!!!!
	 * When RIL completes the transition to contiguous memory, all CCI_xx_CONTIGUOUS
	 * & CCI_APP_NONCONTIGUOUS flags must be removed.
	 * ONLY the code BETWEEN the following 2 comment lines will REMAIN:
	 * # Start Contiguous Code Section # and # End Contiguous Code Section #
	 * All other code OUTSIDE these comments must be REMOVED - ( The backwards compatible code )
	 */
#ifndef CCI_MSG_CONTIGUOUS
	CiMsgPdu            *pPdu;                              /**< Message PDU. \sa CiMsgPdu_struct */
#else
#ifdef CCI_APP_NONCONTIGUOUS
	CiMsgPdu            *pPduTempNonContPtr; /*Temporary pointer used to ensure the correct structure size for a non contiguous app*/
#endif
/* # Start Contiguous Code Section # */
	CiMsgPdu pdu;                                           /**< Message PDU. \sa CiMsgPdu_struct */
/* # End Contiguous Code Section # */
#endif
	CiOptAddressInfo optSca;                                /**< SCR #1777605: Optional Service Center Address. \sa CCI API Ref Manual */
} CiMsgPrimWriteMsgWithIndexReq;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_WRITE_MSG_WITH_INDEX_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimWriteMsgWithIndexCnf_struct {
	CiMsgRc rc;                             /**< Result code. \sa CiMsgRc */
	UINT16 index;                           /**< Index into the memory storage. */
} CiMsgPrimWriteMsgWithIndexCnf;


/** \brief  Message Waiting Indication type (Bits 6..0 show the message indication IE)  */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CI_MSG_MSGWAITING_TYPE_TAG
{
	CI_MSG_MSGWAITING_VOICE = 0x00, /**< Voice Mail Message(s) waiting */
	CI_MSG_MSGWAITING_FAX   = 0x01, /**< Fax Message(s) waiting */
	CI_MSG_MSGWAITING_EMAIL = 0x02, /**< Email Message(s) waiting */
	CI_MSG_MSGWAITING_OTHER = 0x03  /**< Other Message(s) waiting */
} _CiMsgMsgWaitingType;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Message Waiting Indication type (Bits 6..0 show the message indication IE).
 * \sa CI_MSG_MSGWAITING_TYPE_TAG */
/** \remarks Common Data Section */
typedef UINT8 CiMsgMsgWaitingType;
/**@}*/

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_MESSAGE_WAITING_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimMessageWaitingInd_struct
{

  CiMsgMsgWaitingType    	msgType;            		/**< message type: Voice, Fax, Email, other; \sa CiMsgMsgWaitingType */
  CiBoolean              		indicationActive;   	/**< TRUE = active; FALSE = inactive; \sa CCI API Ref Manual */
  UINT8                  		msgCount;           		/**< number of waiting messages of the specified type */
  CiBoolean              		udhPresent;		/**< ?UDHI (User Data Header information) method is present. \sa CCI API Ref Manual */
  CiBoolean              		cphsPresent; 	/**< CPHS (Common PCN Handset Specification) method is present.  \sa CCI API Ref Manual */
  CiBoolean              		dcsPresent;		/**< DCS (TP-Data-Coding-Scheme method) method is present.  \sa CCI API Ref Manual */
  CiBoolean              		cphsLine1;	     /**< Defined in CPHS case.  \sa CCI API Ref Manual */
  CiBoolean              		cphsLine2;	     /**< Defined in CPHS case. \sa CCI API Ref Manual */
  CiBoolean              		storeMessage;    /**< Defined in UDH, CPHS, DCS case.  \sa CCI API Ref Manual */
  CiBoolean              		userDataHeaderPresent; 	/**< user data is present in TP-UD.  \sa CCI API Ref Manual */ 
} CiMsgPrimMessageWaitingInd;

/** \brief MO SMS Status enumeration */
/** \remarks Common Data Section */
//ICAT EXPORTED ENUM
typedef enum CI_MSG_STATUS_TAG
{
  CI_MSG_SIM_READY   = 0,    	/**< SMS ready for SIM */
  CI_MSG_NOT_READY,    	        /**< SMS not ready */
  CI_MSG_SEND_READY,      	    /**< ready for MO */
  CI_MSG_SEND_RECV_READY,     	/**< ready for both MO/MT */

  CI_MSG_NUM_STATUS
} _CiMsgStatus;

/** \addtogroup  SpecificSGRelated
 * @{ */
/**  \brief Service type enumeration.
 * \sa CI_MSG_STATUS_TAG */
typedef UINT8 CiMsgStatus;
/**@}*/

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_STORAGE_STATUS_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimStorageStatusInd_struct {

  CiBoolean  	memCapExceeded;      	/**< indicates if the memory capacity has been reached/exceeded. \sa CCI API Ref Manual */
  UINT8      	usedRecords;         		/**< currently used number of SMS file records on the SIM */
  UINT8      	totalSmsRecords;     		/**< number of total SMS file records on the SIM */
  UINT8      	firstFreeRecord;     		/**< first free SMS file record */

  CiMsgStatus   smsStatus;     		    /**< status for SMS service group */
} CiMsgPrimStorageStatusInd;

/*Michal Bukai - SMS Memory Full Feature*/
/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_RESET_MEMCAP_FULL_REQ"> */
//changed by sunny on 20110810 for CQ12474
//typedef CiEmptyPrim CiMsgPrimResetMemcapFullReq;
//ICAT EXPORTED STRUCT
#ifndef AT_CUSTOMER_HTC
typedef CiEmptyPrim CiMsgPrimResetMemcapFullReq;
#else
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimResetMemcapFullReq {

	CiBoolean 		memcapFull;			/**< memory full flag */
} CiMsgPrimResetMemcapFullReq;
#endif


/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_RESET_MEMCAP_FULL_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimResetMemcapFullCnf_struct {
	CiMsgRc rc;                                     /**< Result code. \sa CiMsgRc */
} CiMsgPrimResetMemcapFullCnf;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_SET_RECORD_STATUS_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimSetRecordStatusReq_struct {
  UINT16          index;          /**< Index into the memory storage. */
  CiMsgMsgStatus  status;         /**< Message status to be set with the message. \sa CiMsgMsgStatus */
} CiMsgPrimSetRecordStatusReq;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_SET_RECORD_STATUS_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimSetRecordStatusCnf_struct {
  CiMsgRc       rc;     /**< Result code. \sa CiMsgRc */
} CiMsgPrimSetRecordStatusCnf;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_LOCK_SMS_STATUS_REQ"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimLockSmsStatusReq_struct {
  CiBoolean       lockSmsStatus;         /**< lock SMS status in SIM */
} CiMsgPrimLockSmsStatusReq;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_LOCK_SMS_STATUS_CNF"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimLockSmsStatusCnf_struct {
  CiMsgRc       rc;     /**< Result code. \sa CiMsgRc */
} CiMsgPrimLockSmsStatusCnf;

#ifdef AT_CUSTOMER_HTC
/** \brief SMS Reject type values */
/** \remarks Common Data Section */
typedef enum CIMSG_REJECT_TYPE{
    CI_MSG_NONE = 0,      /**< Abnormal reject*/
    CI_MSG_CP_ERROR,      /**< CP Error */
    CI_MSG_RP_ERROR,      /**< RP Error */

    CI_MSG_NUM_REJECT_TYPE

} _CiMsgRejectType;

typedef UINT8 CiMsgRejectType;

typedef enum CIMSG_REJECT_CAUSE_TAG{
    CI_MSG_CAUSE_MMCNM_EST_REJ = 1,
    CI_MSG_CAUSE_MMSMS_REL_IND,
    CI_MSG_CAUSE_TR1M_TIMEOUT,
    CI_MSG_CAUSE_CP_ERROR_NETWORK_FAILURE = 0x11,
    CI_MSG_CAUSE_CP_ERROR_CONGESTION = 0x16,
    CI_MSG_CAUSE_CP_ERROR_PROTOCAL_ERROR_UNSPECIFIED = 0x6F,
    CI_MSG_CAUSE_RP_ERROR_DESTINATION_OUT_OF_ORDER = 0x1B,
    CI_MSG_CAUSE_RP_ERROR_NETWORK_OUT_OF_ORDER = 0x26,
    CI_MSG_CAUSE_RP_ERROR_TEMPORARY_FAILURE = 0x29,
    CI_MSG_CAUSE_RP_ERROR_CONGESTION = 0x2A,
    CI_MSG_CAUSE_RP_ERROR_RESOURCE_UNAVAILABLE_UNSPECIFIED = 0x2F,
    CI_MSG_CAUSE_RP_ERROR_PROTOCAL_ERROR_UNSPECIFIED = 0x6F,
    CI_MSG_CAUSE_RP_ERROR_INTERWORKING_UNSPECIFIED = 0x7F,

    CI_MSG_NUM_REJECT_CAUSE

} _CiMsgRejectCause;

typedef UINT8 CiMsgRejectCause;

/* <INUSE> */
/** <paramref name="CI_MSG_PRIM_SMS_REJECT_CAUSE_IND"> */
//ICAT EXPORTED STRUCT
typedef struct CiMsgPrimSmsRejectCauseInd_struct {
  CiMsgRejectType type;         /**< SMS reject type. \sa CiMsgRejectType */
  CiMsgRejectCause smsCause;     /**< SMS reject cause. \sa CiMsgRejectCause  */
} CiMsgPrimSmsRejectCauseInd;
#endif
/* ADD NEW COMMON PRIMITIVES DEFINITIONS HERE */

#ifdef CI_CUSTOM_EXTENSION
/* it is assumed that only one customized set of extension primitives is
 * to be considered at one time. The selection of the particular customized
 * set is done in the 'ci_msg_cust.h' based on compile flags.
 *
 * Note: if no customer extension primitives are defined for this service group
 * the CI_MSG_NUM_CUST_PRIM will be set to 0 in the "ci_msg_cust.h" file.
 */
#include "ci_msg_cust.h"

#define CI_MSG_NUM_PRIM ( CI_MSG_NUM_COMMON_PRIM + CI_MSG_NUM_CUST_PRIM )

#else

/* if no customer extension is supported, only the default common set is considered */
#define CI_MSG_NUM_PRIM CI_MSG_NUM_COMMON_PRIM

#endif /* CI_CUSTOM_EXTENSION */

#ifdef __cplusplus
}
#endif  //__cplusplus

#endif  /* _CI_MSG_H_ */


/*                      end of ci_msg.h
   --------------------------------------------------------------------------- */
