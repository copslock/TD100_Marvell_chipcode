#ifndef _ODSUPPTK_H
#define _ODSUPPTK_H

#include "ntddndis.h"
#include "fswccx.h"

#ifdef __cplusplus
extern "C"   {
#endif

#ifndef ODSTK_DECL
#ifndef _WIN32_WCE
#define ODSTK_DECL	__declspec(dllimport)
#else
#define ODSTK_DECL 
#endif //_WIN32_WCE
#endif //ODSTK_DECL


#ifdef _UNICODE

#define ODSTK_FN_TRACE_CALLBACK					ODSTK_FN_TRACE_CALLBACKW
#define ODSTK_FN_CALLBACK						ODSTK_FN_CALLBACKW
#define ODSTK_SetTraceCallback					ODSTK_SetTraceCallbackW
#define ODSTK_AllocString						ODSTK_AllocStringW
#define ODSTK_ProcessCallback					ODSTK_ProcessCallbackW
#define ODSTK_CONTEXT_CreateLinkSynchronous		ODSTK_CONTEXT_CreateLinkSynchronousW
#define ODSTK_CONTEXT_CreateLinkAsynchronous	ODSTK_CONTEXT_CreateLinkAsynchronousW
#define ODSTK_IDENTITY_SetUserName				ODSTK_IDENTITY_SetUserNameW
#define ODSTK_IDENTITY_SetWindowsName			ODSTK_IDENTITY_SetWindowsNameW
#define ODSTK_IDENTITY_SetPassword				ODSTK_IDENTITY_SetPasswordW
#define ODSTK_IDENTITY_SetTtlsIdentity			ODSTK_IDENTITY_SetTtlsIdentityW
#define ODSTK_LINK_SetNetworkInfo				ODSTK_LINK_SetNetworkInfoW
#define ODSTK_LINK_SetCallback					ODSTK_LINK_SetCallbackW
#define ODSTK_LINK_PrecomputeWpaPsk				ODSTK_LINK_PrecomputeWpaPskW
#define ODSTK_NETWORK_INFO						ODSTK_NETWORK_INFOW
#define ODSTK_CB_INFO_AUTH_STATUS				ODSTK_CB_INFO_AUTH_STATUSW
#define ODSTK_CB_INFO_GET_PASSWORD				ODSTK_CB_INFO_GET_PASSWORDW
#define ODSTK_CB_INFO_GET_PASSWORD_CHANGE		ODSTK_CB_INFO_GET_PASSWORD_CHANGEW
#define ODSTK_CB_INFO_GET_CHALLENGE_RESPONSE	ODSTK_CB_INFO_GET_CHALLENGE_RESPONSEW
#define ODSTK_CB_INFO							ODSTK_CB_INFOW

#else

#define ODSTK_FN_TRACE_CALLBACK					ODSTK_FN_TRACE_CALLBACKA
#define ODSTK_FN_CALLBACK						ODSTK_FN_CALLBACKA
#define ODSTK_SetTraceCallback					ODSTK_SetTraceCallbackA
#define ODSTK_AllocString						ODSTK_AllocStringA
#define ODSTK_ProcessCallback					ODSTK_ProcessCallbackA
#define ODSTK_CONTEXT_CreateLinkSynchronous		ODSTK_CONTEXT_CreateLinkSynchronousA
#define ODSTK_CONTEXT_CreateLinkAsynchronous	ODSTK_CONTEXT_CreateLinkAsynchronousA
#define ODSTK_IDENTITY_SetUserName				ODSTK_IDENTITY_SetUserNameA
#define ODSTK_IDENTITY_SetWindowsName			ODSTK_IDENTITY_SetWindowsNameA
#define ODSTK_IDENTITY_SetPassword				ODSTK_IDENTITY_SetPasswordA
#define ODSTK_IDENTITY_SetTtlsIdentity			ODSTK_IDENTITY_SetTtlsIdentityA
#define ODSTK_LINK_SetNetworkInfo				ODSTK_LINK_SetNetworkInfoA
#define ODSTK_LINK_SetCallback					ODSTK_LINK_SetCallbackA
#define ODSTK_LINK_PrecomputeWpaPsk				ODSTK_LINK_PrecomputeWpaPskA
#define ODSTK_NETWORK_INFO						ODSTK_NETWORK_INFOA
#define ODSTK_CB_INFO_AUTH_STATUS				ODSTK_CB_INFO_AUTH_STATUSA
#define ODSTK_CB_INFO_GET_PASSWORD				ODSTK_CB_INFO_GET_PASSWORDA
#define ODSTK_CB_INFO_GET_PASSWORD_CHANGE		ODSTK_CB_INFO_GET_PASSWORD_CHANGEA
#define ODSTK_CB_INFO_GET_CHALLENGE_RESPONSE	ODSTK_CB_INFO_GET_CHALLENGE_RESPONSEA
#define ODSTK_CB_INFO							ODSTK_CB_INFOA

#endif


/****************************************************************************
		Handle types
...........................................................................*/

typedef void*						HODSTK_COMMON;
typedef struct _ODSTK_CONTEXT*		HODSTK_CONTEXT;
typedef struct _ODSTK_IDENTITY*		HODSTK_IDENTITY;
typedef struct _ODSTK_LINK*			HODSTK_LINK;
typedef struct _ODSTK_CERT_LIST*	HODSTK_CERT_LIST;
typedef struct _ODSTK_CERT*			HODSTK_CERT;


/****************************************************************************
		General structures
...........................................................................*/

// data blob
typedef struct _ODSTK_BLOB
{
	BYTE* pData;
	ULONG nSize;
} ODSTK_BLOB;


// SHA1 hash (of certificate)
typedef struct _ODSTK_SHA1_HASH
{
	BYTE aHash[20];
} ODSTK_SHA1_HASH;


/****************************************************************************
		Trace enumerations and callback
...........................................................................*/

typedef enum _ODSTK_TRACE_LEVEL
{
	ODSTK_TL_NONE = 0,
	ODSTK_TL_ERROR,
	ODSTK_TL_WARNING,
	ODSTK_TL_NORMAL,
	ODSTK_TL_DETAIL
} ODSTK_TRACE_LEVEL;

typedef enum _ODSTK_TRACE_CLASS
{
	ODSTK_TC_NONE = 0,
	ODSTK_TC_AUTH,
	ODSTK_TC_KEY
} ODSTK_TRACE_CLASS;

// callback method prototype
typedef void __stdcall ODSTK_FN_TRACE_CALLBACKA(
					ODSTK_TRACE_LEVEL nTraceLevel, ODSTK_TRACE_CLASS nTraceClass, 
					LPCSTR pszMessage, const BYTE* pData, ULONG nDataSize);
typedef void __stdcall ODSTK_FN_TRACE_CALLBACKW(
					ODSTK_TRACE_LEVEL nTraceLevel, ODSTK_TRACE_CLASS nTraceClass, 
					LPCWSTR pszMessage, const BYTE* pData, ULONG nDataSize);


/****************************************************************************
		Callbacks
...........................................................................*/

// callback codes
enum
{
	ODSTK_CB_NONE = 0,
	ODSTK_CB_LINK_STATUS,
	ODSTK_CB_AUTH_STATUS,
	ODSTK_CB_MEDIA_CONNECT,
	ODSTK_CB_MEDIA_DISCONNECT,
	ODSTK_CB_SET_WEP,
	ODSTK_CB_ADD_KEY,
	ODSTK_CB_REMOVE_KEY,
	ODSTK_CB_EAPOL_TRANSMIT,
	ODSTK_CB_CCX_ROGUE_AP,
	ODSTK_CB_CCX_REPORT_ROGUE_APS,
	ODSTK_CB_GET_PASSWORD,
	ODSTK_CB_GET_PASSWORD_CHANGE,
	ODSTK_CB_GET_CHALLENGE_RESPONSE,
	ODSTK_CB_VALIDATE_SERVER_CERT,
	ODSTK_CB_START_AUTHENTICATION,
	ODSTK_CB_CONNECTION_ERROR,
	ODSTK_CB_CCX_NOTIFY_AUTH_SUCCESS,
	ODSTK_CB_CCX_CCKM_REQUEST,
	ODSTK_CB_CCX_CCKM_RESULT,
	ODSTK_CB_SET_PMKID_MAP,
	ODSTK_CB_SIGN_WITH_PRIVATE_KEY,
	ODSTK_CB_GET_PKCS12_PASSWORD,
	ODSTK_CB_MEDIA_ROAM,
	ODSTK_CB_PREAUTH_AUTH_STATUS,
	ODSTK_CB_PREAUTH_EAPOL_TRANSMIT,
	ODSTK_CB_PREAUTH_START_AUTHENTICATION,
	ODSTK_CB_PREAUTH_GET_PASSWORD,
	ODSTK_CB_PREAUTH_GET_PASSWORD_CHANGE,
	ODSTK_CB_PREAUTH_GET_CHALLENGE_RESPONSE,
	ODSTK_CB_PREAUTH_VALIDATE_SERVER_CERT,
	ODSTK_CB_PREAUTH_SIGN_WITH_PRIVATE_KEY
};


// link status information
typedef struct _ODSTK_CB_INFO_LINK_STATUS
{
	ULONG nLinkState;
} ODSTK_CB_INFO_LINK_STATUS;


// link state
enum
{
	ODSTK_LINK_STATE_NONE = 0,
	ODSTK_LINK_STATE_IDLE,
	ODSTK_LINK_STATE_AUTHENTICATING,
	ODSTK_LINK_STATE_AUTHENTICATED_KEY_WAIT,
	ODSTK_LINK_STATE_OPEN_AUTHENTICATED,
	ODSTK_LINK_STATE_UNAUTHENTICATED_KEY_WAIT,
	ODSTK_LINK_STATE_OPEN_UNAUTHENTICATED,
	ODSTK_LINK_STATE_OPEN_TIMEOUT,
	ODSTK_LINK_STATE_OPEN_CCKM_HANDOFF
};


// auth status information
typedef struct _ODSTK_CB_INFO_AUTH_STATUSA
{
	BOOL bSuppStatus;
	ULONG nAuthState;
	BOOL bTimeout;
	ULONG nAuthResult;
	ULONG nTlsCipherSuite;
	LPCSTR pszAuthType;
	LPCSTR pszRequestIdentityMessage;
	LPCSTR pszNotification;
} ODSTK_CB_INFO_AUTH_STATUSA;
typedef struct _ODSTK_CB_INFO_AUTH_STATUSW
{
	BOOL bSuppStatus;
	ULONG nAuthState;
	BOOL bTimeout;
	ULONG nAuthResult;
	ULONG nTlsCipherSuite;
	LPCWSTR pszAuthType;
	LPCWSTR pszRequestIdentityMessage;
	LPCWSTR pszNotification;
} ODSTK_CB_INFO_AUTH_STATUSW;


//
// auth result definitions
//

// mask for auth result type (indicating overall result of authentication)
#define ODSTK_AUTH_RESULT_TYPE_MASK						0x00FFF000

// auth result types
#define ODSTK_AUTH_RESULT_TYPE_UNKNOWN					0x00000000
#define ODSTK_AUTH_RESULT_TYPE_SUCCESS					0x00001000
#define ODSTK_AUTH_RESULT_TYPE_GENERAL_FAILURE			0x00002000
#define ODSTK_AUTH_RESULT_TYPE_TIMEOUT					0x00003000
#define ODSTK_AUTH_RESULT_TYPE_INCOMPLETE				0x00004000
#define ODSTK_AUTH_RESULT_TYPE_PARSE_FAILURE			0x00005000
#define ODSTK_AUTH_RESULT_TYPE_NO_CREDENTIALS			0x00006000
#define ODSTK_AUTH_RESULT_TYPE_INTERNAL_FAILURE			0x00007000
#define ODSTK_AUTH_RESULT_TYPE_PROTOCOL_FAILURE			0x00008000
#define ODSTK_AUTH_RESULT_TYPE_CLIENT_REJECT			0x00009000
#define ODSTK_AUTH_RESULT_TYPE_SERVER_REJECT			0x0000A000
#define ODSTK_AUTH_RESULT_TYPE_TLS_FAILURE				0x0000B000
#define ODSTK_AUTH_RESULT_TYPE_NO_CERTIFICATE			0x0000C000
#define ODSTK_AUTH_RESULT_TYPE_NO_AGREED_METHOD			0x0000D000
#define ODSTK_AUTH_RESULT_TYPE_ERROR_CHANGING_PASSWORD	0x0000F000
#define ODSTK_AUTH_RESULT_TYPE_RESTRICTED_LOGON_HOURS	0x00010000
#define ODSTK_AUTH_RESULT_TYPE_NO_PERMISSION			0x00011000
#define ODSTK_AUTH_RESULT_TYPE_ACCT_DISABLED			0x00012000

// mask for TLS subtypes, valid when type is ODSTK_AUTH_RESULT_TYPE_TLS_FAILURE
#define ODSTK_AUTH_RESULT_TLS_TYPE_MASK					0x00000F00

// TLS subtypes
#define ODSTK_AUTH_RESULT_TLS_TYPE_UNKNOWN					0x00000000
#define ODSTK_AUTH_RESULT_TLS_TYPE_ERROR					0x00000100
#define ODSTK_AUTH_RESULT_TLS_TYPE_CLIENT_ALERT			0x00000200
#define ODSTK_AUTH_RESULT_TLS_TYPE_SERVER_ALERT			0x00000300

// mask for alert code, when TLS subtype is ODSTK_AUTH_RESULT_TLS_TYPE_CLIENT_ALERT or 
// ODSTK_AUTH_RESULT_TLS_TYPE_SERVER_ALERT
#define ODSTK_AUTH_RESULT_TLS_ALERT_MASK					0x000000FF


// auth state
enum
{
	ODSTK_AUTH_STATE_NONE = 0,
	ODSTK_AUTH_STATE_LOGOFF,
	ODSTK_AUTH_STATE_DISCONNECTED,
	ODSTK_AUTH_STATE_HELD,
	ODSTK_AUTH_STATE_CONNECTING,
	ODSTK_AUTH_STATE_ACQUIRED,
	ODSTK_AUTH_STATE_AUTHENTICATING,
	ODSTK_AUTH_STATE_AUTHENTICATED
};


// rogue AP information
typedef struct _ODSTK_CB_INFO_CCX_ROGUE_AP
{
	ULONG nEapType;
	FSW_CCX_ROGUE_AP_DETECTED RogueAPDetected;
} ODSTK_CB_INFO_CCX_ROGUE_AP;


// report rogue AP information
typedef struct _ODSTK_CB_INFO_CCX_REPORT_ROGUE_APS
{
	ULONG nEapType;
	BYTE aBSSID[6];
} ODSTK_CB_INFO_CCX_REPORT_ROGUE_APS;


// indicate authentication success for CCX fast roaming assist
typedef struct _ODSTK_CB_INFO_CCX_NOTIFY_AUTH_SUCCESS
{
	ULONG nEapType;
	FSW_CCX_AUTH_SUCCESS AuthSuccess;
} ODSTK_CB_INFO_CCX_NOTIFY_AUTH_SUCCESS;


// add key information
typedef struct _ODSTK_CB_INFO_ADD_KEY
{
	ULONG nCipherSuite;
	ULONG nReserved;
	NDIS_802_11_KEY Key;
} ODSTK_CB_INFO_ADD_KEY;


// get password information
typedef struct _ODSTK_CB_INFO_GET_PASSWORDA
{
	LPSTR pszPassword;
} ODSTK_CB_INFO_GET_PASSWORDA;
typedef struct _ODSTK_CB_INFO_GET_PASSWORDW
{
	LPWSTR pszPassword;
} ODSTK_CB_INFO_GET_PASSWORDW;


// get password change information
typedef struct _ODSTK_CB_INFO_GET_PASSWORD_CHANGEA
{
	LPCSTR pszMessage;
	LPSTR pszOldPassword;
	LPSTR pszNewPassword;
} ODSTK_CB_INFO_GET_PASSWORD_CHANGEA;
typedef struct _ODSTK_CB_INFO_GET_PASSWORD_CHANGEW
{
	LPCWSTR pszMessage;
	LPWSTR pszOldPassword;
	LPWSTR pszNewPassword;
} ODSTK_CB_INFO_GET_PASSWORD_CHANGEW;


// get challenge response information
typedef struct _ODSTK_CB_INFO_GET_CHALLENGE_RESPONSEA
{
	LPCSTR pszChallenge;
	ODSTK_BLOB Response;
} ODSTK_CB_INFO_GET_CHALLENGE_RESPONSEA;
typedef struct _ODSTK_CB_INFO_GET_CHALLENGE_RESPONSEW
{
	LPCWSTR pszChallenge;
	ODSTK_BLOB Response;
} ODSTK_CB_INFO_GET_CHALLENGE_RESPONSEW;


// validate server cert information
typedef struct _ODSTK_CB_INFO_VALIDATE_SERVER_CERT
{
	HODSTK_CERT_LIST hCertList;
	ULONG nValidity;
} ODSTK_CB_INFO_VALIDATE_SERVER_CERT;


// connection error
typedef struct _ODSTK_CB_INFO_CONNECTION_ERROR
{
	ULONG nErrorCode;
} ODSTK_CB_INFO_CONNECTION_ERROR;


// connection error code enumeration
enum
{
	ODSTK_CONNECTION_ERROR_NONE = 0,
	ODSTK_CONNECTION_ERROR_KEY_TIMEOUT,
	ODSTK_CONNECTION_ERROR_AUTHENTICATOR_IE,
	ODSTK_CONNECTION_ERROR_HANDSHAKE_PROTOCOL,
	ODSTK_CONNECTION_ERROR_HANDSHAKE_DECRYPT,
	ODSTK_CONNECTION_ERROR_ACQUIRE_TIMEOUT
};


// get PKCS12 password information (for decrypting private key)
typedef struct _ODSTK_CB_INFO_GET_PKCS12_PASSWORDA
{
	ODSTK_SHA1_HASH Sha1Hash;
	LPSTR pszPKCS12Password;
} ODSTK_CB_INFO_GET_PKCS12_PASSWORDA;
typedef struct _ODSTK_CB_INFO_GET_PKCS12_PASSWORDW
{
	ODSTK_SHA1_HASH Sha1Hash;
	LPWSTR pszPKCS12Password;
} ODSTK_CB_INFO_GET_PKCS12_PASSWORDW;


// sign with private key
typedef struct _ODSTK_CB_INFO_SIGN_WITH_PRIVATE_KEY
{
	ULONG nSignatureType;
	ODSTK_SHA1_HASH Sha1Hash;
	ODSTK_BLOB Hash;
	ODSTK_BLOB Message;
	ULONG nRSAKeySize;
	BOOL bError;
	ODSTK_BLOB Signature;
} ODSTK_CB_INFO_SIGN_WITH_PRIVATE_KEY;


// signature types
enum
{
	ODSTK_SIGNATURE_TYPE_NONE = 0,
	ODSTK_SIGNATURE_TYPE_RSA,
	ODSTK_SIGNATURE_TYPE_DSA
};


// master callback information (union of all callback information structures)
typedef struct _ODSTK_CB_INFOA
{
	ULONG nLength;
	ULONG nCallbackCode;
	HODSTK_LINK hLink;
	BYTE* pBSSID;
	union
	{
		ODSTK_CB_INFO_LINK_STATUS LinkStatus;
		ODSTK_CB_INFO_AUTH_STATUSA AuthStatus;
		ODSTK_CB_INFO_CCX_ROGUE_AP RogueAP;
		ODSTK_CB_INFO_CCX_REPORT_ROGUE_APS ReportRogueAPs;
		ODSTK_CB_INFO_CCX_NOTIFY_AUTH_SUCCESS NotifyAuthSuccess;
		FSW_CCX_CCKM_REQUEST CCKM_Request;
		FSW_CCX_CCKM_RESULT CCKM_Result;
		ODSTK_CB_INFO_ADD_KEY AddKey;
		NDIS_802_11_REMOVE_KEY RemoveKey;
		NDIS_802_11_WEP SetWEP;
		ODSTK_BLOB EapolTransmit;
		ODSTK_CB_INFO_GET_PASSWORDA GetPassword;
		ODSTK_CB_INFO_GET_PASSWORD_CHANGEA GetPasswordChange;
		ODSTK_CB_INFO_GET_CHALLENGE_RESPONSEA GetChallengeResponse;
		ODSTK_CB_INFO_VALIDATE_SERVER_CERT ValidateServerCert;
		ODSTK_CB_INFO_CONNECTION_ERROR ConnectionError;
		NDIS_802_11_PMKID PMKIDMap;
		ODSTK_CB_INFO_GET_PKCS12_PASSWORDA GetPKCS12Password;
		ODSTK_CB_INFO_SIGN_WITH_PRIVATE_KEY SignWithPrivateKey;
	};
} ODSTK_CB_INFOA;
typedef struct _ODSTK_CB_INFOW
{
	ULONG nLength;
	ULONG nCallbackCode;
	HODSTK_LINK hLink;
	BYTE* pBSSID;
	union
	{
		ODSTK_CB_INFO_LINK_STATUS LinkStatus;
		ODSTK_CB_INFO_AUTH_STATUSW AuthStatus;
		ODSTK_CB_INFO_CCX_ROGUE_AP RogueAP;
		ODSTK_CB_INFO_CCX_REPORT_ROGUE_APS ReportRogueAPs;
		ODSTK_CB_INFO_CCX_NOTIFY_AUTH_SUCCESS NotifyAuthSuccess;
		FSW_CCX_CCKM_REQUEST CCKM_Request;
		FSW_CCX_CCKM_RESULT CCKM_Result;
		ODSTK_CB_INFO_ADD_KEY AddKey;
		NDIS_802_11_REMOVE_KEY RemoveKey;
		NDIS_802_11_WEP SetWEP;
		ODSTK_BLOB EapolTransmit;
		ODSTK_CB_INFO_GET_PASSWORDW GetPassword;
		ODSTK_CB_INFO_GET_PASSWORD_CHANGEW GetPasswordChange;
		ODSTK_CB_INFO_GET_CHALLENGE_RESPONSEW GetChallengeResponse;
		ODSTK_CB_INFO_VALIDATE_SERVER_CERT ValidateServerCert;
		ODSTK_CB_INFO_CONNECTION_ERROR ConnectionError;
		NDIS_802_11_PMKID PMKIDMap;
		ODSTK_CB_INFO_GET_PKCS12_PASSWORDW GetPKCS12Password;
		ODSTK_CB_INFO_SIGN_WITH_PRIVATE_KEY SignWithPrivateKey;
	};
} ODSTK_CB_INFOW;


// callback method prototype
typedef void __stdcall ODSTK_FN_CALLBACKA(void* hReference, ODSTK_CB_INFOA* pCallbackInfo);
typedef void __stdcall ODSTK_FN_CALLBACKW(void* hReference, ODSTK_CB_INFOW* pCallbackInfo);


/****************************************************************************
		Network info
...........................................................................*/

typedef struct _ODSTK_NETWORK_INFOA
{
	ULONG nLength;
	ULONG nNetworkMode;
	ULONG nAssociationMode;
	ULONG nAuthenticationType;
	ULONG nKeyType;
	ULONG nEncryptionType;
	BYTE aMacAddress[6];
	BYTE aBSSID[6];
	LPSTR pszPassphrase;
	ODSTK_BLOB SSID;
	ODSTK_BLOB SupplicantIEs;
	ODSTK_BLOB AuthenticatorIEs;
	ODSTK_BLOB AssociationResponseIEs;
} ODSTK_NETWORK_INFOA;
typedef struct _ODSTK_NETWORK_INFOW
{
	ULONG nLength;
	ULONG nNetworkMode;
	ULONG nAssociationMode;
	ULONG nAuthenticationType;
	ULONG nKeyType;
	ULONG nEncryptionType;
	BYTE aMacAddress[6];
	BYTE aBSSID[6];
	LPWSTR pszPassphrase;
	ODSTK_BLOB SSID;
	ODSTK_BLOB SupplicantIEs;
	ODSTK_BLOB AuthenticatorIEs;
	ODSTK_BLOB AssociationResponseIEs;
} ODSTK_NETWORK_INFOW;


// network mode
enum
{
	ODSTK_NETWORK_MODE_NONE = 0,
	ODSTK_NETWORK_MODE_AD_HOC,
	ODSTK_NETWORK_MODE_INFRASTRUCTURE
};


// association mode
enum
{
	ODSTK_ASSOCIATION_MODE_NONE = 0,
	ODSTK_ASSOCIATION_MODE_OPEN,
	ODSTK_ASSOCIATION_MODE_SHARED,
	ODSTK_ASSOCIATION_MODE_WPA,
	ODSTK_ASSOCIATION_MODE_RSN
};


// authentication type
enum
{
	ODSTK_AUTHENTICATION_TYPE_NONE = 0,
	ODSTK_AUTHENTICATION_TYPE_8021X
};


// key type
enum
{
	ODSTK_KEY_TYPE_NONE = 0,
	ODSTK_KEY_TYPE_STATIC,
	ODSTK_KEY_TYPE_DYNAMIC
};


// encryption type
enum
{
	ODSTK_ENCRYPTION_TYPE_NONE = 0,
	ODSTK_ENCRYPTION_TYPE_WEP,
	ODSTK_ENCRYPTION_TYPE_TKIP,
	ODSTK_ENCRYPTION_TYPE_AES
};


/****************************************************************************
		Authentication state machine constants
...........................................................................*/

typedef struct _ODSTK_AUTH_STATE_MACHINE_CONSTANTS
{
	ULONG nLength;
	ULONG nAuthPeriodMs;
	ULONG nHeldPeriodMs;
	ULONG nStartPeriodMs;
	ULONG nStartDelayMs;
	ULONG nMaxStart;
	BOOL bOpenOnTimeout;
} ODSTK_AUTH_STATE_MACHINE_CONSTANTS;


/****************************************************************************
		Identity definitions
...........................................................................*/

// supported EAP types
enum
{
	ODSTK_EAP_TYPE_MD5_CHALLENGE		= 4,
	ODSTK_EAP_TYPE_GENERIC_TOKEN_CARD	= 6,
	ODSTK_EAP_TYPE_TLS					= 13,
	ODSTK_EAP_TYPE_LEAP					= 17,
	ODSTK_EAP_TYPE_TTLS					= 21,
	ODSTK_EAP_TYPE_PEAP					= 25,
	ODSTK_EAP_TYPE_MS_CHAP_V2			= 26
};


// TTLS inner authentication protocols
enum
{
	ODSTK_AUTH_PROTOCOL_NONE = 0,
	ODSTK_AUTH_PROTOCOL_PAP,
	ODSTK_AUTH_PROTOCOL_CHAP,
	ODSTK_AUTH_PROTOCOL_MS_CHAP,
	ODSTK_AUTH_PROTOCOL_MS_CHAP_V2,
	ODSTK_AUTH_PROTOCOL_CERTIFICATE,
	ODSTK_AUTH_PROTOCOL_EAP
};


/****************************************************************************
		Certificate validity codes
...........................................................................*/

enum
{
	ODSTK_CERT_V_OK = 0,
	ODSTK_CERT_V_ERR_CERT_NOT_YET_VALID,
	ODSTK_CERT_V_ERR_CERT_HAS_EXPIRED,
	ODSTK_CERT_V_ERR_CERT_REVOKED,
	ODSTK_CERT_V_ERR_CERT_SIGNATURE_FAILURE,
	ODSTK_CERT_V_ERR_INVALID_PURPOSE,
	ODSTK_CERT_V_ERR_CERT_UNTRUSTED,
	ODSTK_CERT_V_ERR_UNABLE_TO_GET_CRL,
	ODSTK_CERT_V_ERR_APPLICATION_VERIFICATION,
	ODSTK_CERT_V_ERR_UNABLE_TO_GET_ISSUER_CERT
};


/****************************************************************************
		Global methods
...........................................................................*/

ODSTK_DECL HRESULT __stdcall ODSTK_SetTraceCallbackA(ODSTK_FN_TRACE_CALLBACKA* pfnTraceCallback);
ODSTK_DECL HRESULT __stdcall ODSTK_SetTraceCallbackW(ODSTK_FN_TRACE_CALLBACKW* pfnTraceCallback);

ODSTK_DECL HRESULT __stdcall ODSTK_SetTraceLevel(ODSTK_TRACE_LEVEL nTraceLevel);

ODSTK_DECL HRESULT __stdcall ODSTK_Initialize();

ODSTK_DECL HRESULT __stdcall ODSTK_CreateContext(HODSTK_CONTEXT* phContext);

ODSTK_DECL HRESULT __stdcall ODSTK_Alloc(ULONG nBytes, BYTE** ppBuffer);

ODSTK_DECL HRESULT __stdcall ODSTK_AllocStringA(LPCSTR pszString, LPSTR* ppszString);
ODSTK_DECL HRESULT __stdcall ODSTK_AllocStringW(LPCWSTR pszString, LPWSTR* ppszString);

ODSTK_DECL HRESULT __stdcall ODSTK_AllocBlob(ODSTK_BLOB* pBlob, BYTE* pData, ULONG nSize);

ODSTK_DECL HRESULT __stdcall ODSTK_Free(BYTE* pBuffer);

ODSTK_DECL HRESULT __stdcall ODSTK_ProcessCallbackA(ODSTK_CB_INFOA* pInfo);
ODSTK_DECL HRESULT __stdcall ODSTK_ProcessCallbackW(ODSTK_CB_INFOW* pInfo);

ODSTK_DECL HRESULT __stdcall ODSTK_Terminate();


/****************************************************************************
		Common methods for all objects
...........................................................................*/

ODSTK_DECL HRESULT __stdcall ODSTK_AddRef(HODSTK_COMMON h);

ODSTK_DECL HRESULT __stdcall ODSTK_Release(HODSTK_COMMON h);


/****************************************************************************
		Context methods
...........................................................................*/

ODSTK_DECL HRESULT __stdcall ODSTK_CONTEXT_SetTlsCipherSuites(HODSTK_CONTEXT h, 
						ULONG* pCipherSuites, ULONG nNumCipherSuites);

ODSTK_DECL HRESULT __stdcall ODSTK_CONTEXT_SetDhExponentBits(HODSTK_CONTEXT h, 
						ULONG nDhExponentBits);

ODSTK_DECL HRESULT __stdcall ODSTK_CONTEXT_Initialize(HODSTK_CONTEXT h);

ODSTK_DECL HRESULT __stdcall ODSTK_CONTEXT_CreateIdentity(HODSTK_CONTEXT h, HODSTK_IDENTITY* phIdentity);

ODSTK_DECL HRESULT __stdcall ODSTK_CONTEXT_CreateLinkSynchronousA(HODSTK_CONTEXT h, HODSTK_LINK* phLink);
ODSTK_DECL HRESULT __stdcall ODSTK_CONTEXT_CreateLinkSynchronousW(HODSTK_CONTEXT h, HODSTK_LINK* phLink);

ODSTK_DECL HRESULT __stdcall ODSTK_CONTEXT_CreateLinkAsynchronousA(HODSTK_CONTEXT h, HODSTK_LINK* phLink);
ODSTK_DECL HRESULT __stdcall ODSTK_CONTEXT_CreateLinkAsynchronousW(HODSTK_CONTEXT h, HODSTK_LINK* phLink);

ODSTK_DECL HRESULT __stdcall ODSTK_CONTEXT_CreateCertList(HODSTK_CONTEXT h, HODSTK_CERT_LIST* phCertList);


/****************************************************************************
		Identity methods
...........................................................................*/

ODSTK_DECL HRESULT __stdcall ODSTK_IDENTITY_SetUserNameA(HODSTK_IDENTITY h, LPCSTR pszUserName);
ODSTK_DECL HRESULT __stdcall ODSTK_IDENTITY_SetUserNameW(HODSTK_IDENTITY h, LPCWSTR pszUserName);

ODSTK_DECL HRESULT __stdcall ODSTK_IDENTITY_SetWindowsNameA(HODSTK_IDENTITY h, LPCSTR pszWindowsName);
ODSTK_DECL HRESULT __stdcall ODSTK_IDENTITY_SetWindowsNameW(HODSTK_IDENTITY h, LPCWSTR pszWindowsName);

ODSTK_DECL HRESULT __stdcall ODSTK_IDENTITY_SetPasswordA(HODSTK_IDENTITY h, LPCSTR pszPassword);
ODSTK_DECL HRESULT __stdcall ODSTK_IDENTITY_SetPasswordW(HODSTK_IDENTITY h, LPCWSTR pszPassword);

ODSTK_DECL HRESULT __stdcall ODSTK_IDENTITY_SetUserCertificate(HODSTK_IDENTITY h, ODSTK_SHA1_HASH* pSha1Hash);

ODSTK_DECL HRESULT __stdcall ODSTK_IDENTITY_SetUserCertificateChain(HODSTK_IDENTITY h, HODSTK_CERT_LIST hCertList);

ODSTK_DECL HRESULT __stdcall ODSTK_IDENTITY_SetEapTypes(HODSTK_IDENTITY h, ULONG* pTypes, ULONG nNumTypes);

ODSTK_DECL HRESULT __stdcall ODSTK_IDENTITY_SetTtlsInnerAuthProtocol(HODSTK_IDENTITY h, ULONG nAuthProtocol);

ODSTK_DECL HRESULT __stdcall ODSTK_IDENTITY_SetTtlsInnerEapTypes(HODSTK_IDENTITY h, ULONG* pTypes, ULONG nNumTypes);

ODSTK_DECL HRESULT __stdcall ODSTK_IDENTITY_SetPeapInnerEapTypes(HODSTK_IDENTITY h, ULONG* pTypes, ULONG nNumTypes);

ODSTK_DECL HRESULT __stdcall ODSTK_IDENTITY_SetPeapNoPasswordInGTC(HODSTK_IDENTITY h, BOOL bNoPasswordInGTC);

ODSTK_DECL HRESULT __stdcall ODSTK_IDENTITY_SetTtlsIdentityA(HODSTK_IDENTITY h, LPCSTR pszTtlsIdentity);
ODSTK_DECL HRESULT __stdcall ODSTK_IDENTITY_SetTtlsIdentityW(HODSTK_IDENTITY h, LPCWSTR pszTtlsIdentity);

ODSTK_DECL HRESULT __stdcall ODSTK_IDENTITY_Initialize(HODSTK_IDENTITY h);


/****************************************************************************
		Link methods
...........................................................................*/

ODSTK_DECL HRESULT __stdcall ODSTK_LINK_SetCallbackA(HODSTK_LINK h, ODSTK_FN_CALLBACKA* pfnCallback, void* hReference);
ODSTK_DECL HRESULT __stdcall ODSTK_LINK_SetCallbackW(HODSTK_LINK h, ODSTK_FN_CALLBACKW* pfnCallback, void* hReference);

ODSTK_DECL HRESULT __stdcall ODSTK_LINK_Initialize(HODSTK_LINK h);

ODSTK_DECL HRESULT __stdcall ODSTK_LINK_GetAuthStateMachineConstants(HODSTK_LINK h,
						ODSTK_AUTH_STATE_MACHINE_CONSTANTS** ppConstants);

ODSTK_DECL HRESULT __stdcall ODSTK_LINK_SetAuthStateMachineConstants(HODSTK_LINK h,
						ODSTK_AUTH_STATE_MACHINE_CONSTANTS* pConstants);

ODSTK_DECL HRESULT __stdcall ODSTK_LINK_SetRoamTimeoutMs(HODSTK_LINK h, ULONG nRoamTimeoutMs);

ODSTK_DECL HRESULT __stdcall ODSTK_LINK_PortDown(HODSTK_LINK h);

ODSTK_DECL HRESULT __stdcall ODSTK_LINK_SetNetworkInfoA(HODSTK_LINK h, ODSTK_NETWORK_INFOA* pNetworkInfo);
ODSTK_DECL HRESULT __stdcall ODSTK_LINK_SetNetworkInfoW(HODSTK_LINK h, ODSTK_NETWORK_INFOW* pNetworkInfo);

ODSTK_DECL HRESULT __stdcall ODSTK_LINK_SetIdentity(HODSTK_LINK h, HODSTK_IDENTITY hIdentity);

ODSTK_DECL HRESULT __stdcall ODSTK_LINK_SetSessionResumptionTimeout(HODSTK_LINK h, ULONG nSeconds);

ODSTK_DECL HRESULT __stdcall ODSTK_LINK_PrecomputeWpaPskA(HODSTK_LINK h, LPCSTR pszPassphrase, BYTE* pSSID, ULONG nSSIDSize);
ODSTK_DECL HRESULT __stdcall ODSTK_LINK_PrecomputeWpaPskW(HODSTK_LINK h, LPCWSTR pszPassphrase, BYTE* pSSID, ULONG nSSIDSize);

ODSTK_DECL HRESULT __stdcall ODSTK_LINK_PortUp(HODSTK_LINK h);

ODSTK_DECL HRESULT __stdcall ODSTK_LINK_Reauthenticate(HODSTK_LINK h);

ODSTK_DECL HRESULT __stdcall ODSTK_LINK_ReceiveEapol(HODSTK_LINK h, BYTE* pMessage, ULONG nMessageSize);

ODSTK_DECL HRESULT __stdcall ODSTK_LINK_ReceivePreauthEapol(HODSTK_LINK h, BYTE* pBSSID, BYTE* pMessage, ULONG nMessageSize);

ODSTK_DECL HRESULT __stdcall ODSTK_LINK_ReceiveAuthenticationEvent(HODSTK_LINK h, 
						NDIS_802_11_AUTHENTICATION_REQUEST* pAuthenticationRequest);

ODSTK_DECL HRESULT __stdcall ODSTK_LINK_CCKM_StartReassociation(HODSTK_LINK h, FSW_CCX_CCKM_START* pCCKMStart);

ODSTK_DECL HRESULT __stdcall ODSTK_LINK_FlushCachedSecurityAssociations(HODSTK_LINK h);

ODSTK_DECL HRESULT __stdcall ODSTK_LINK_ReceivePMKIDCandidateListEvent(HODSTK_LINK h, 
						NDIS_802_11_PMKID_CANDIDATELIST* pPMKIDCandidateList);

ODSTK_DECL HRESULT __stdcall ODSTK_LINK_Terminate(HODSTK_LINK h);

ODSTK_DECL HRESULT __stdcall ODSTK_LINK_Process(HODSTK_LINK h, ULONG* pnMsWait);

ODSTK_DECL HRESULT __stdcall ODSTK_LINK_Thread(HODSTK_LINK h);


/****************************************************************************
		Cert methods
...........................................................................*/

ODSTK_DECL HRESULT __stdcall ODSTK_CERT_GetDer(HODSTK_CERT h, ODSTK_BLOB* pDer);

ODSTK_DECL HRESULT __stdcall ODSTK_CERT_GetSha1Hash(HODSTK_CERT h, ODSTK_SHA1_HASH* pSha1Hash);


/****************************************************************************
		Cert List methods
...........................................................................*/

ODSTK_DECL HRESULT __stdcall ODSTK_CERT_LIST_AddCert(HODSTK_CERT_LIST h, BYTE* pCertDER, ULONG nCertDERSize);

ODSTK_DECL HRESULT __stdcall ODSTK_CERT_LIST_BuildCertChain(HODSTK_CERT_LIST h, HODSTK_CERT_LIST* phChain, ULONG* pnValidity);

ODSTK_DECL HRESULT __stdcall ODSTK_CERT_LIST_GetNumCerts(HODSTK_CERT_LIST h, ULONG* pnNumCerts);

ODSTK_DECL HRESULT __stdcall ODSTK_CERT_LIST_GetCert(HODSTK_CERT_LIST h, ULONG nIndex, HODSTK_CERT* phCert);

#ifdef __cplusplus
}		  /* extern "C" */
#endif

#endif
