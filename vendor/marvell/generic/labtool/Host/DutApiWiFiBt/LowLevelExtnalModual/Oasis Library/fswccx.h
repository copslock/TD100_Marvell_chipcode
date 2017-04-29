#ifndef _FSWCCX_H
#define _FSWCCX_H

/*
	fswccx.h

	(C) Copyright 2003 Funk Software, Inc.

	This file contains definitions for CCX. It should 
	be included after NDIS definitions.

 */

#include <guiddef.h>
//#include <ntddndis.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
	GUIDs for custom OIDs in DEFINE_GUID form
	-----------------------------------------

	The DEFINE_GUID form allocates a GUID structure that can be 
	referenced by name. To cause the DEFINE_GUID macro to allocate the 
	GUID, #define INITGUID prior to including this file; to cause the 
	DEFINE_GUID to simply generate an extern, make sure INITGUID is not 
	defined prior to including this file.
 */

// oids
DEFINE_GUID(GUID_FSW_CCX_CONFIGURATION,			0x21190696, 0x118d, 0x4654, 0x9e, 0x9a, 0xc6, 0x9c, 0xa7, 0xc7, 0x95, 0xb8);
DEFINE_GUID(GUID_FSW_CCX_NETWORK_EAP,			0x0725e492, 0x3025, 0x477c, 0x91, 0xdc, 0xd5, 0xc1, 0x2a, 0x4e, 0xec, 0x1f);
DEFINE_GUID(GUID_FSW_CCX_ROGUE_AP_DETECTED,		0x5858fa82, 0x0dfd, 0x4a4a, 0xbb, 0xc9, 0xdc, 0xc7, 0x8f, 0x63, 0x01, 0x70);
DEFINE_GUID(GUID_FSW_CCX_REPORT_ROGUE_APS,		0x6e72993a, 0x59a7, 0x4a3e, 0xb1, 0x65, 0x0c, 0xec, 0xb3, 0xc5, 0x0c, 0xdc);
DEFINE_GUID(GUID_FSW_CCX_AUTH_SUCCESS,			0x55019653, 0x0454, 0x4309, 0xb8, 0xca, 0xd2, 0xe9, 0xf4, 0xd0, 0xaf, 0x83);
DEFINE_GUID(GUID_FSW_CCX_CCKM_REQUEST,			0xf5190942, 0x6d90, 0x4858, 0x8a, 0xdf, 0x08, 0x6a, 0x2f, 0xa5, 0xb7, 0xeb);
DEFINE_GUID(GUID_FSW_CCX_CCKM_RESULT,			0x1163fca7, 0x9c1a, 0x4e39, 0xa8, 0x79, 0x9f, 0x93, 0xad, 0x1b, 0x84, 0x07);

// status code
DEFINE_GUID(GUID_FSW_CCX_CCKM_START,			0x8c389e47, 0xe511, 0x4d96, 0xae, 0xfe, 0x2f, 0xb7, 0x31, 0xd8, 0x0c, 0x05);

/*
	GUIDs for custom OIDs in #define form
	-------------------------------------

	The same GUIDs constructed using DEFINE_GUID (above) are also 
	available in #define form. This allows a GUID structure to be 
	constructed directly. For example, an NDIS_GUID structure could be 
	constructed as follows:

		NDIS_GUID ng = {CGUID_FSW_CCX_CONFIGURATION, OID_FSW_CCX_CONFIGURATION, 4, fNDIS_GUID_TO_OID};
 */

// oids
#define CGUID_FSW_CCX_CONFIGURATION				{0x21190696, 0x118d, 0x4654, {0x9e, 0x9a, 0xc6, 0x9c, 0xa7, 0xc7, 0x95, 0xb8}}
#define CGUID_FSW_CCX_NETWORK_EAP				{0x0725e492, 0x3025, 0x477c, {0x91, 0xdc, 0xd5, 0xc1, 0x2a, 0x4e, 0xec, 0x1f}}
#define CGUID_FSW_CCX_ROGUE_AP_DETECTED			{0x5858fa82, 0x0dfd, 0x4a4a, {0xbb, 0xc9, 0xdc, 0xc7, 0x8f, 0x63, 0x01, 0x70}}
#define CGUID_FSW_CCX_REPORT_ROGUE_APS			{0x6e72993a, 0x59a7, 0x4a3e, {0xb1, 0x65, 0x0c, 0xec, 0xb3, 0xc5, 0x0c, 0xdc}}
#define CGUID_FSW_CCX_AUTH_SUCCESS				{0x55019653, 0x0454, 0x4309, {0xb8, 0xca, 0xd2, 0xe9, 0xf4, 0xd0, 0xaf, 0x83}}
#define CGUID_FSW_CCX_CCKM_REQUEST				{0xf5190942, 0x6d90, 0x4858, {0x8a, 0xdf, 0x08, 0x6a, 0x2f, 0xa5, 0xb7, 0xeb}}
#define CGUID_FSW_CCX_CCKM_RESULT				{0x1163fca7, 0x9c1a, 0x4e39, {0xa8, 0x79, 0x9f, 0x93, 0xad, 0x1b, 0x84, 0x07}}

// status code
#define CGUID_FSW_CCX_CCKM_START				{0x8c389e47, 0xe511, 0x4d96, {0xae, 0xfe, 0x2f, 0xb7, 0x31, 0xd8, 0x0c, 0x05}}


/*
	Default values for custom OIDs
	------------------------------

	The default values for custom OIDs are defined below. However, a 
	driver may define its own values for custom OIDs, in case the 
	default values conflict with existing custom OIDs.

	Note that the supplicant will dynamically determine the values 
	of the custom OIDs based on GUID, so the actual values used are 
	arbitrary.

	To suppress the definition of the default custom OID values, 
	#define FSW_CCX_NO_DEFAULT_CUSTOM_OIDS prior to including this 
	file.
 */

#ifndef FSW_CCX_NO_DEFAULT_CUSTOM_OIDS

// use new definitions for marvell utility
/*#define OID_FSW_CCX_CONFIGURATION				0xFF010201
#define OID_FSW_CCX_NETWORK_EAP					0xFF010202
#define OID_FSW_CCX_ROGUE_AP_DETECTED			0xFF010203
#define OID_FSW_CCX_REPORT_ROGUE_APS			0xFF010204
#define OID_FSW_CCX_AUTH_SUCCESS				0xFF010205
#define OID_FSW_CCX_CCKM_REQUEST				0xFF010206
#define OID_FSW_CCX_CCKM_RESULT					0xFF010207
*/

// status code
#define NDIS_STATUS_FSW_CCX_CCKM_START			0x60010001

#endif


//
//  Configuration
//

typedef ULONG FSW_CCX_CONFIGURATION;

// configuration bit flags
#define FSW_CCX_CONFIGURATION_ENABLE_CKIP		0x00000001
#define FSW_CCX_CONFIGURATION_ENABLE_ROGUE_AP	0x00000002
#define FSW_CCX_CONFIGURATION_ENABLE_CCKM		0x00000004


//
//  Network EAP enumeration
//

typedef enum _FSW_CCX_NETWORK_EAP
{
	FswCcx_NetworkEapOff = 0,
	FswCcx_NetworkEapOn,
	FswCcx_NetworkEapAllowed,
	FswCcx_NetworkEapPreferred
} FSW_CCX_NETWORK_EAP;


//
//  Rogue AP structure
//

typedef struct _FSW_CCX_ROGUE_AP_DETECTED
{
	USHORT FailureReason;
	NDIS_802_11_MAC_ADDRESS RogueAPMacAddress;
	CHAR RogueAPName[16];
} FSW_CCX_ROGUE_AP_DETECTED;


// FailureReason values
#define FSW_CCX_FR_INVALID_AUTHENTICATION_TYPE		1
#define FSW_CCX_FR_AUTHENTICATION_TIMEOUT			2
#define FSW_CCX_FR_CHALLENGE_FROM_AP_FAILED			3
#define FSW_CCX_FR_CHALLENGE_TO_AP_FAILED			4


//
//  Auth Success structure
//

typedef struct _FSW_CCX_AUTH_SUCCESS
{
	NDIS_802_11_SSID Ssid;
	NDIS_802_11_MAC_ADDRESS BSSID;
} FSW_CCX_AUTH_SUCCESS;


//
//  CCKM Start structure
//

typedef struct _FSW_CCX_CCKM_START
{
	BYTE Timestamp[8];
	NDIS_802_11_MAC_ADDRESS BSSID;
} FSW_CCX_CCKM_START;


//
//  CCKM Request structure
//

// RequestCode values
typedef enum _FSW_CCX_CCKM_REQUEST_CODE
{
	FswCcx_CckmRequestFirstTime = 0,
	FswCcx_CckmRequestFastHandoff,
} FSW_CCX_CCKM_REQUEST_CODE;

typedef struct _FSW_CCX_CCKM_REQUEST
{
	FSW_CCX_CCKM_REQUEST_CODE RequestCode;
	ULONG AssociationRequestIELength;
	BYTE AssociationRequestIE[1];
} FSW_CCX_CCKM_REQUEST;


//
//  CCKM Result enumeration
//

// Result values
typedef enum _FSW_CCX_CCKM_RESULT
{
	FswCcx_CckmResultHandoffSuccess = 0,
	FswCcx_CckmResultHandoffFailure,
} FSW_CCX_CCKM_RESULT;


#ifdef __cplusplus
}
#endif


#endif // _FSWCCX_H
