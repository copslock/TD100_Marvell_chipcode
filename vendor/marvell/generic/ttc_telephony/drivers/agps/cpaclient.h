/*------------------------------------------------------------
(C) Copyright [2006-2011] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/

/********************************************************************
	Filename:	CpaClient.h
	Created:	2009/02/26
	
	Purpose:	Constant definitions and function prototypes for 
				CPA telephony interface to application layer.
				(Marvell AGPS CPA = Control Plane Agent - Client side, over MTIL)
*********************************************************************/

#ifndef _CpaClient_h_
#define _CpaClient_h_

#include "linux_types.h"

int CpaClient_Init(void);
int CpaClient_Deinit(void);

int CpaClient_RecvNetworkDownlinkInd( UINT8 bearer_type,
                                                                          UINT8 *in_msg_data,
                                                                          UINT32 in_msg_size,
                                                                          UINT8 in_session_type,
                                                                          UINT8 in_rrc_state,
                                                                          UINT count);
int CpaClient_RecvRrcStateInd(UINT8 bearer_type, UINT8 in_rrc_state);
int CpaClient_RecvMeasTerminateInd(UINT8 bearer_type);
int CpaClient_RecvResetPosInfoInd(UINT8 bearer_type);
int CpaClient_RecvNetworkUplinkCnf(UINT16 ret_code);
int CpaClient_RecvLocInd(char *pData);
LSM_BOOL CpaClient_SendLcsVerificationResp(unsigned long notifId, unsigned long present, unsigned long verificationResponse);
// MTLR request definitions

// The following define MUST be compatible with MTIL_LCS_MAX_NAME_STRING_LENGTH
#define CPA_LCS_MAX_NAME_STRING_LENGTH			63

// The following define MUST be compatible with MTIL_LCS_MAX_CODEWORD_SIZE
#define CPA_LCS_MAX_CODEWORD_SIZE				20

// The following define MUST be compatible with MTIL_LCS_MAX_NUM_PRIVATE_EXTENSIONS
#define CPA_LCS_MAX_NUM_PRIVATE_EXTENSIONS		10

// The following define MUST be compatible with MTIL_LCS_MAX_EXTERNAL_ADDRESS_SIZE
#define CPA_LCS_MAX_EXTERNAL_ADDRESS_SIZE		20

// The following define MUST be compatible with MTIL_LCS_MAX_ASSISTANCE_DATA_SIZE
#define CPA_LCS_MAX_ASSISTANCE_DATA_SIZE		38

// The following define MUST be compatible with MTIL_LCS_MAX_EXTENSION_TYPE_SIZE
#define CPA_LCS_MAX_EXTENSION_TYPE_SIZE			16

// The following define MUST be compatible with MTIL_LCS_MAX_EXTENSION_ID_SIZE
#define CPA_LCS_MAX_EXTENSION_ID_SIZE			16


// The following enum MUST be compatible with MTIL_LCS_NOTIFICATION_TYPE
typedef enum _CPA_LCS_NOTIFICATION_TYPE {
	CPA_LCS_NOTIFY_LOCATION_ALLOWED = 0,
	CPA_LCS_NOTIFY_AND_VERIFY_LOCATION_ALLOWED_IF_NO_RESPONSE,
	CPA_LCS_NOTIFY_AND_VERIFY_LOCATION_NOT_ALLOWED_IF_NO_RESPONSE,
	CPA_LCS_LOCATION_NOT_ALLOWED,
	CPA_LCS_LCS_NOTIFICATION_TYPE_MAX,
	CPA_LCS_LCS_NOTIFICATION_TYPE_MAX_LAST = 0xFFFFFFFF
} CPA_LCS_NOTIFICATION_TYPE;


// The following enum MUST be compatible with MTIL_LCS_LOCATION_ESTIMATE_TYPE
typedef enum _CPA_LCS_LOCATION_ESTIMATE_TYPE {
	CPA_LCS_CURRENT_LOCATION = 0,
	CPA_LCS_CURRENT_OR_LAST_KNOWN_LOCATION,
	CPA_LCS_INITIAL_LOCATION,
	CPA_LCS_ACTIVATE_DEFERRED_LOCATION,
	CPA_LCS_CANCEL_DEFERRED_LOCATION,
	CPA_LCS_LCS_LOCATION_ESTIMATE_TYPE_MAX,
	CPA_LCS_LCS_LOCATION_ESTIMATE_TYPE_MAX_LAST = 0xFFFFFFFF,
} CPA_LCS_LOCATION_ESTIMATE_TYPE;

typedef unsigned long CPA_LCS_INVOKE_HANDLE;

typedef struct _CPA_LCS_LOCATION_TYPE {
	unsigned long						locEstimateType;			/**< LCS Location estimate type [range: CPA_LCS_LOCATION_ESTIMATE_TYPE] */
	unsigned long						deferredLocationTypePresent;/**< Indicated if deferred location type is present */
	unsigned long						deferredLocationType;		/**< Deferred location type */
} CPA_LCS_LOCATION_TYPE, *P_CPA_LCS_LOCATION_TYPE;


typedef struct CPA_LCS_OPT_EXTERNAL_ADDRESS {
	unsigned long						present;					/**< Indicates if external address is present */
	unsigned long						addressLength;				/**< Address length */
	unsigned char		externalAddress[CPA_LCS_MAX_EXTERNAL_ADDRESS_SIZE];/**< External address */
}CPA_LCS_OPT_EXTERNAL_ADDRESS, *P_CPA_LCS_OPT_EXTERNAL_ADDRESS;


typedef struct _CPA_LCS_EXTENSION_ID {
	unsigned long						nn;							/**< OCTET STRING  - should not exceed 16 octets */
	unsigned char						data[CPA_LCS_MAX_EXTENSION_ID_SIZE];
} CPA_LCS_EXTENSION_ID, *P_CPA_LCS_EXTENSION_ID;


typedef struct _CPA_LCS_EXTENSION_TYPE {
	unsigned long						nn;							/**< OCTET STRING  - should not exceed 16 octets */
	unsigned char						data[CPA_LCS_MAX_EXTENSION_TYPE_SIZE];
} CPA_LCS_EXTENSION_TYPE, *P_CPA_LCS_EXTENSION_TYPE;


typedef struct _CPA_LCS_PRIVATE_EXT_LIST_DATA {
	unsigned long						extTypePresent;				/**< Indicates if private extension list is present */
	CPA_LCS_EXTENSION_TYPE				extType;
	CPA_LCS_EXTENSION_ID				extId;
} CPA_LCS_PRIVATE_EXT_LIST_DATA, *P_CPA_LCS_PRIVATE_EXT_LIST_DATA;


typedef struct _CPA_LCS_PRIVATE_EXTENSION_LIST {
	unsigned long						nn;							/**< 1 to maxNumOfPrivateExtensions */
	CPA_LCS_PRIVATE_EXT_LIST_DATA		data[CPA_LCS_MAX_NUM_PRIVATE_EXTENSIONS];
}CPA_LCS_PRIVATE_EXTENSION_LIST, *P_CPA_LCS_PRIVATE_EXTENSION_LIST;


typedef struct _CPA_LCS_PCS_EXTENSIONS {
	unsigned long						_dummy_;					/**< Not used */
} CPA_LCS_PCS_EXTENSIONS, *P_CPA_LCS_PCS_EXTENSIONS;


typedef struct _CPA_LCS_EXTENSION_CONTAINER {
	unsigned long						privateExtensionListPresent;/**< Indicates if private extension list is present */
	CPA_LCS_PRIVATE_EXTENSION_LIST		privateExtensionList;
	unsigned long						pcsExtensionsPresent;		/**< Indicates if PCS extensions is present */
	CPA_LCS_PCS_EXTENSIONS				pcsExtensions;
} CPA_LCS_EXTENSION_CONTAINER, *P_CPA_LCS_EXTENSION_CONTAINER;


typedef struct _CPA_LCS_OPT_EXTENSION_CONTAINER {
	unsigned long						present;					/**< Indicates if extension container is present */
	CPA_LCS_EXTENSION_CONTAINER			data;
} CPA_LCS_OPT_EXTENSION_CONTAINER, *P_CPA_LCS_OPT_EXTENSION_CONTAINER;


typedef struct _CPA_LCS_OPT_CLIENT_EXTERNAL_ID {
	unsigned long						present;					/**< Indicates if client external ID is present */
	CPA_LCS_OPT_EXTERNAL_ADDRESS		optExternalAddress;
	CPA_LCS_OPT_EXTENSION_CONTAINER		optExtensionContainer;		/**< Implicit sequence */
} CPA_LCS_OPT_CLIENT_EXTERNAL_ID, *P_CPA_LCS_OPT_CLIENT_EXTERNAL_ID;


typedef struct _CPA_LCS_NAME_STRING {
	unsigned long						length;
	unsigned char						name[CPA_LCS_MAX_NAME_STRING_LENGTH];
} CPA_LCS_NAME_STRING, *P_CPA_LCS_NAME_STRING;


typedef struct _CPA_LCS_OPT_FORMAT_INDICATOR {
	unsigned long						present;					/**< Indicates if format indicator is present */
	unsigned long						formatIndicator;			/**< Format indicator */
} CPA_LCS_OPT_FORMAT_INDICATOR, *P_CPA_LCS_OPT_FORMAT_INDICATOR;


typedef struct _CPA_LCS_OPT_CLIENT_NAME {
	unsigned long						present;					/**< Indicates if client name is present */
	unsigned long						dataCodingScheme;			/**< Data coding scheme */
	CPA_LCS_NAME_STRING					clientNameString;			/**< Client name string */
	CPA_LCS_OPT_FORMAT_INDICATOR		optFormatIndicator;			/**< Format indicator - optional value */
} CPA_LCS_OPT_CLIENT_NAME, *P_CPA_LCS_OPT_CLIENT_NAME;


typedef struct _CPA_LCS_OPT_REQUESTER_ID {
	unsigned long						present;					/**< Indicates if ID is present */
	unsigned long						dataCodingScheme;			/**< Data coding scheme */
	CPA_LCS_NAME_STRING					requesterIdString;			/**< Requester ID name string */
	CPA_LCS_OPT_FORMAT_INDICATOR		optFormatIndicator;			/**< Format indicator - optional value */
} CPA_LCS_OPT_REQUESTER_ID, *P_CPA_LCS_OPT_REQUESTER_ID;


typedef struct _CPA_LCS_OPT_CODE_WORD {
	unsigned long						present;					/**< Indicates if codeword is present */
	unsigned long						dataCodingScheme;			/**< Data coding scheme */
	unsigned long						length;						/**< Codeword length */
	unsigned char	codeWordString[CPA_LCS_MAX_CODEWORD_SIZE];		/**< Codeword  */
} CPA_LCS_OPT_CODE_WORD, *P_CPA_LCS_OPT_CODE_WORD;


typedef struct _CPA_LCS_OPT_SERVICE_TYPE_ID {
	unsigned long						present;					/**< Indicates if service type ID is present */
	unsigned long						serviceTypeId;				/**< Service type ID */
} CPA_LCS_OPT_SERVICE_TYPE_ID, *P_CPA_LCS_OPT_SERVICE_TYPE_ID;


// The following define MUST be compatible with MTIL_LCS_LOCATION_IND (defined in MtilAPI.h)
typedef struct _CPA_LCS_LOCATION_IND {
	unsigned long						notifyType;					/**< LCS notification type [range: CPA_LCS_NOTIFICATION_TYPE] */
	CPA_LCS_INVOKE_HANDLE           	invokeHandle;				/**< LCS 'live' invokeHandle associated with an outgoing invoke */
	CPA_LCS_LOCATION_TYPE				locationType;				/**< LCS location information */
	CPA_LCS_OPT_CLIENT_EXTERNAL_ID		optClientExternalId;		/**< LCS client external ID - optional value */
	CPA_LCS_OPT_CLIENT_NAME				optClientName;				/**< LCS client name - optional value */
	CPA_LCS_OPT_REQUESTER_ID			optRequesterId;				/**< LCS requester ID - optional value */
	CPA_LCS_OPT_CODE_WORD				optCodeWord;				/**< LCS codeword - optional value */
	CPA_LCS_OPT_SERVICE_TYPE_ID			optServiceTypeId;			/**< LCS service type ID - optional value */
} CPA_LCS_LOCATION_IND, *P_CPA_LCS_LOCATION_IND;


// The following enum MUST be compatible with MTIL_LCS_VERIFICATION_RESPONSE (defined in MtilAPI.h)
typedef enum _CPA_LCS_VERIFICATION_RESPONSE {
	CPA_LCS_PERMISSION_DENIED = 0,
	CPA_LCS_PERMISSION_GRANTED = 1,
	CPA_LCS_VERIFICATION_RESP_MAX,
	CPA_LCS_VERIFICATION_RESP_MAX_LAST = 0xFFFFFFFF,
} CPA_LCS_VERIFICATION_RESPONSE;


#endif /* _CpaClient_h_ */
