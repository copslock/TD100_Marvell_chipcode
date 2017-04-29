
#include <windows.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <winioctl.h>

#include "Mrv8000xConfig.h"



//=====================================================================
//	Device Id
//=====================================================================
#define MARVELL_WLAN_DEVICE		_T("VEN_11AB&DEV_1F") // CB/MB chip, for 1FA4, 1FA5, 1FB4, 1FB5 
#define MARVELL_WLAN_DEVICE2	_T("VID_1286&PID_1FA")	// USB
#define MARVELL_WLAN_DEVICE3	_T("MARVELL-88W8300_802.11B_PC_CARD") // CF8380
#define MARVELL_WLAN_DEVICE4	_T("VEN_11AB&DEV_2A") // CB-55
#define MARVELL_WLAN_DEVICE5	_T("Marvell_Semiconductor-Eagle_802.11b_PC_Card-FB97") // CF8385


//====================================================================
//	Event Notification
//====================================================================
#define WM_EVENT_NOTIFY						WM_APP

 
#define COMMAND_EVENT_ASSOC_FAIL			0
#define COMMAND_EVENT_ASSOC_SUCCESS			1
#define COMMAND_EVENT_EXIT					2
#define	COMMAND_EVENT_ADHOC_SUCCESS			3
#define	COMMAND_EVENT_ADHOC_FAIL			4
#define	COMMAND_EVENT_DISCONNECT			5
#define COMMAND_EVENT_BSSID_LIST			6
//#define COMMAND_EVENT_QUERY_REG			    7
#define COMMAND_EVENT_QUERY_DONE			7
#define COMMAND_EVENT_HW_RADIO_ON			8 
#define COMMAND_EVENT_HW_RADIO_OFF			9

#define COMMAND_EVENT_EAPOL_RCV				132
#define COMMAND_EVENT_EAPOL_TX				133
#define COMMAND_EVENT_WPA2_AUTH				134
#define COMMAND_EVENT_PMKID_LIST			135
#define COMMAND_EVENT_CCX_CCKM_START		136
#define COMMAND_EVENT_ZC_ACTIVE				137

// Turbo Setup Event code 
#define	COMMAND_EVENT_TS_START				0xF0000000
#define	COMMAND_EVENT_TS_RESPONSE			0xF0000001
#define	COMMAND_EVENT_TS_CHALLENGE			0xF0000002
#define COMMAND_EVENT_TS_CHALLENGE_RESP		0xF0000003
#define COMMAND_EVENT_TS_STATUS				0xF0000004
#define COMMAND_EVENT_TS_STATUS_RESP		0xF0000005
#define COMMAND_EVENT_TS_END				0xF0000006
#define COMMAND_EVENT_TS_SUCCESS			0xF0000007
#define COMMAND_EVENT_TS_FAILURE			0xF0000008
#define COMMAND_EVENT_TS_WPA_FAIL			0xF0000009

//=============================================
//	Notification Code
//=============================================
#define NOTIFY_EVENT_TERMINATE				1000	// was 101
#define	NOTIFY_EVENT_CARD_ARRIVAL			1001
#define	NOTIFY_EVENT_REFRESH_SCREEN			1002
#define NOTIFY_EVENT_SCANNING				1003
#define NOTIFY_EVENT_DONE_SCANNING			1004
#define NOTIFY_EVENT_AUTH_STATUS			1005	
#define NOTIFY_EVENT_TRACE					1006
#define NOTIFY_EVENT_TURBO_SETUP			1007
#define NOTIFY_EVENT_WPA_SUCCESS			1008
#define NOTIFY_EVENT_TS_DONE				1009
#define NOTIFY_EVENT_TS_FAIL				1010
#define NOTIFY_EVENT_TS_CONFIG				1011
#define NOTIFY_EVENT_SUPP_STATUS			1012

#define STATUS_SUCCESS			0	
#define STATUS_FAILURE			1
#define STATUS_IN_PROGRESS		2
#define STATUS_INVALID_BUFFER	3 
#define STATUS_WAIT_TIMEOUT		4

#ifdef IEEE_802_11a
#define MAX_CHANNEL_NUM			32
#else
#define MAX_CHANNEL_NUM			14
#endif
#define MAX_SSID_LENGTH			32
#define MAX_DATA_BUFFER			40
#define MAC_ADDR_LENGTH			sizeof(NDIS_802_11_MAC_ADDRESS)
#define ADAPTER_DESC_BUF		100
#define ADAPTER_NAME_BUF		100
#define ADAPTER_REGPATH_BUF		200
#define AUTO_CHAN_SELECT		0
#define AUTO_DATA_RATE			0xFF
#define WEP_KEY_BUF_LEN			32
#define SSID_MAX_LEN			32
//#define MODE_802_11B			0
//#define MODE_802_11G			1
#define MAX_NUM_DATA_RATES		14

#ifdef CF83XX
#define SCAN_DELAY				8000
#else
#define SCAN_DELAY				5000
#endif


#ifdef IEEE_802_11a
#define ASSOC_DELAY				15000	// was 6000
#else
#define ASSOC_DELAY				8000	// was 6000
#endif

#define	FIRST_CHAN_FREQ			2412 // MHz
#define CHANNEL_14				2484 // MHz

//WPASupp
#define SCANLIST_BUFFER_SIZE		24576 //4096
#define NODE_STRLEN					40
#define HASH_SIZE					20
#define THREAD_EXIT_TIMEOUT			3000
#define PASSPHRASE_SIZE				128


//=======================================================
//	802.11a channel
//  Do not support channels 100-140 at this moment
//=======================================================
//#ifdef IEEE_802_11a
//#define Channel80211aRangeCnt	4
#define Channel80211aRangeCnt	3

//static ULONG n80211aBaseChan[Channel80211aRangeCnt] = {36, 52, 100, 149};
//static ULONG n80211aBaseChanFreq[Channel80211aRangeCnt] = {5180, 5260, 5500, 5745};
//static ULONG n80211aChanCnt[Channel80211aRangeCnt] = {4, 4, 11, 5};
static ULONG n80211aBaseChan[Channel80211aRangeCnt] = {36, 52, 149};
static ULONG n80211aBaseChanFreq[Channel80211aRangeCnt] = {5180, 5260, 5745};
static ULONG n80211aChanCnt[Channel80211aRangeCnt] = {4, 4, 5};

#define LAST_CHANNEL			(n80211aBaseChan[Channel80211aRangeCnt-1] + \
								(n80211aChanCnt[Channel80211aRangeCnt-1]-1) * 4)

//#endif

typedef enum _WINDOWS_VERSION
{
	Mrv8000xOSWinXP,
	Mrv8000xOSWin2K,
	Mrv8000xOSWinNT,
	Mrv8000xOSWindows
} WINDOWS_VERSION;


typedef enum _LINK_STATUS
{
	Mrv8000xCardUnplugged,
	Mrv8000xLinkUp,
	Mrv8000xLinkDown, 
	Mrv8000xScanning 
} LINK_STATUS;

typedef enum _MRV8000X_PREFER_LIST
{
    Mrv8000xNotPreferList,
	Mrv8000xInPreferList 
} MRV8000X_PREFER_LIST, *PMRV8000X_PREFER_LIST;


typedef enum _MRV8000X_WLAN_MODE
{
    Mrv8000xWlanMode802_11_G,
	Mrv8000xWlanMode802_11_B,
	Mrv8000xWlanMode802_11_A,
	Mrv8000xWlanModeAuto = 0xFF
} MRV8000X_WLAN_MODE, *PMRV8000X_WLAN_MODE;

typedef enum _MRV8000X_AUTHENTICATION
{
	Mrv8000xOpenSystem,
	Mrv8000xSharedKey,
	Mrv8000xWPAPSK,	
	Mrv8000xWPALeap,
	Mrv8000xAutoSwitch,
	Mrv8000xWPAEapTls,
	Mrv8000xWPAPeap,
	Mrv8000xWPAEapTtls,
	Mrv8000xCCXLeap,
	Mrv8000xWPA2PSK,
	Mrv8000xWPA2EapTls,
	Mrv8000xWPA2Peap,
	Mrv8000xWPA2EapTtls
} MRV8000X_AUTHENTICATION, *PMRV8000X_AUTHENTICATION;


typedef enum _MRV8000X_SECURITY
{
    Mrv8000xSecurityOff,
	Mrv8000xWEP,
	Mrv8000xTKIP,
	Mrv8000xAES
} MRV8000X_SECURITY_OPTION, *PMRV8000X_SECURITY_OPTION;

/*typedef enum _MRV8000X_WPAAUTH
{
    Mrv8000xEapTls,
	Mrv8000xPeap,
	Mrv8000xLeap
//	Mrv8000xEapTtls,
//	Mrv8000xLeap
} MRV8000X_WPAAUTH_PROTOCOL, *PMRV8000X_WPAAUTH_PROTOCOL;
*/

typedef enum _MRV8000X_WPAAUTHMETHOD
{
	Mrv8000xMsChapV2
} MRV8000X_WPAAUTH_METHOD, *PMRV8000X_WPAAUTH_METHOD;

typedef enum _MRV8000X_PREAMBLE
{
    Mrv8000xRadioPreambleAuto=1,
	Mrv8000xRadioPreambleShort,
	Mrv8000xRadioPreambleLong
} MRV8000X_RADIO_PREAMBLE, *PMRV8000X_RADIO_PREAMBLE;


typedef enum _MRV8000X_TXPOWER
{
	Mrv8000xTxPowerLow=1,
	Mrv8000xTxPowerMedium,
	Mrv8000xTxPowerHigh,
	Mrv8000xTxPowerAuto
} MRV8000X_TXPOWER, *PMRV8000X_TXPOWER;

typedef enum _MRV8000X_ANTENNA
{
    Mrv8000xAntenna1=1,
	Mrv8000xAntenna2=2,
	Mrv8000xDiversity=0xffff
} MRV8000X_ANTENNA, *PMRV8000X_ANTENNA;

typedef enum _MRV8000X_RADIO_CTRL
{
    Mrv8000xRadioOff,
	Mrv8000xRadioOn,
	Mrv8000xHwRadioOff
} MRV8000X_RADIO_CTRL, *PMRV8000X_RADIO_CTRL;

typedef enum _MRV8000X_BOOST_MODE
{	
	Mrv8000xBoostModeEnabled,
	Mrv8000xBoostModeDisabled
} MRV8000X_BOOST_MODE, *PMRV8000X_BOOST_MODE;

typedef struct _MRV8000X_REG_ACCESS
{	
	DWORD Index;
	DWORD Value; 
} MRV8000X_REG_ACCESS, *PMRV8000X_REG_ACCESS;

typedef struct _MRV8000X_SSID_SCAN
{	
	ULONG SSIDLength;
	UCHAR SSID[SSID_MAX_LEN];
	UCHAR Channel[MAX_CHANNEL_NUM];
} MRV8000X_SSID_SCAN, *PMRV8000X_SSID_SCAN;

typedef struct _MRV8000X_BSSID_SCAN
{	
	UCHAR MacAddr[MAC_ADDR_LENGTH];
	UCHAR Channel[MAX_CHANNEL_NUM];
} MRV8000X_BSSID_SCAN, *PMRV8000X_BSSID_SCAN;


typedef struct _CONNECTIONINFO
{
	TCHAR	ProfileName[MAX_DATA_BUFFER];
	TCHAR	SSID[SSID_MAX_LEN];	// Empty string means ANY SSID
	ULONG	WirelessMode;
	ULONG   NetworkMode;
	ULONG	Channel;
	ULONG	AuthMode;
	ULONG	WepStatus;
	ULONG   WepTxKey;
 	ULONG   WepKeySize;
	UCHAR	WepKeyVal1[WEP_KEY_BUF_LEN];
//	ULONG   WepKeySize2;
	UCHAR   WepKeyVal2[WEP_KEY_BUF_LEN];
//	ULONG   WepKeySize3;
	UCHAR	WepKeyVal3[WEP_KEY_BUF_LEN];
//	ULONG   WepKeySize4;
	UCHAR	WepKeyVal4[WEP_KEY_BUF_LEN];
} CONNECTIONINFO, *PCONNECTIONINFO;

typedef struct _BEACON_STATS
{
	ULONG	 BeaconRx;
	ULONG  BeaconLoss;
} MRVL_BEACON_STATS,  *PMRVL_BEACON_STATS;

typedef struct _RSSI_STATS
{
	ULONG	Below20;
	ULONG   Below40;
	ULONG	Below60;
	ULONG	Above60;
} MRVL_RSSI_STATS,  *PMRVL_RSSI_STATS;

typedef  UCHAR   MRVL_802_11_RATES[MAX_NUM_DATA_RATES];  // Set of 8 data rates

/*
typedef struct _MRVL_WLAN_BSSID
{
    ULONG                               Length;             // Length of this structure
    NDIS_802_11_MAC_ADDRESS             MacAddress;         // BSSID
    UCHAR                               Reserved[2];
    NDIS_802_11_SSID                    Ssid;               // SSID
    ULONG                               Privacy;            // WEP encryption requirement
    NDIS_802_11_RSSI                    Rssi;               // receive signal
                                                            // strength in dBm
    NDIS_802_11_NETWORK_TYPE            NetworkTypeInUse;
    NDIS_802_11_CONFIGURATION           Configuration;
    NDIS_802_11_NETWORK_INFRASTRUCTURE  InfrastructureMode;
    MRVL_802_11_RATES                   SupportedRates;
	ULONG								WirelessMode;
} MRVL_WLAN_BSSID, *PMRVL_WLAN_BSSID;
*/

typedef struct _MRVL_WLAN_BSSID
{
    ULONG                               Length;             // Length of this structure
    NDIS_802_11_MAC_ADDRESS             MacAddress;         // BSSID
    UCHAR                               Reserved[2];
    NDIS_802_11_SSID                    Ssid;               // SSID
    ULONG                               Privacy;            // WEP encryption requirement
    NDIS_802_11_RSSI                    Rssi;               // receive signal
                                                            // strength in dBm
    NDIS_802_11_NETWORK_TYPE            NetworkTypeInUse;
    NDIS_802_11_CONFIGURATION           Configuration;
    NDIS_802_11_NETWORK_INFRASTRUCTURE  InfrastructureMode;
    MRVL_802_11_RATES	                SupportedRates;
	ULONG								WirelessMode;  		// Marvell-specific for GUI
	ULONG 								Channel;			// Marvell-specific for GUI
    UCHAR                               TurboSetupIE[32];   // Current version takes 13 bytes.
} MRVL_WLAN_BSSID, *PMRVL_WLAN_BSSID;

typedef struct _MRVL_802_11_BSSID_LIST
{
    ULONG           NumberOfItems;      // in list below, at least 1
    MRVL_WLAN_BSSID Bssid[1];
} MRVL_802_11_BSSID_LIST, *PMRVL_802_11_BSSID_LIST;

typedef struct _MRVL_EVENT_BUFFER 
{
 ULONG  Length;
 ULONG  EventCode;
 UCHAR Buffer[1];
} MRVL_EVENT_BUFFER , *PMRVL_EVENT_BUFFER ;

typedef struct _MRVL_EVENT_BUFFER_LIST
{
	ULONG  NumOfEvents;
	MRVL_EVENT_BUFFER  Events[1];
} MRVL_EVENT_BUFFER_LIST, *PMRVL_EVENT_BUFFER_LIST;


#ifndef OASISLIBRARY_EXPORTS
typedef struct _MRVL_DATA_BUFFER
{
	HANDLE			Event;
    ULONG		  	BufferSize;
	PUCHAR			Buffer;

} MRVL_DATA_BUFFER, *PMRVL_DATA_BUFFER;
#endif

typedef enum _MRV8000X_ODSTK_TRACE_LEVEL
{
    Mrv8000x_ODSTK_TL_NONE = 0,
	Mrv8000x_ODSTK_TL_ERROR,
	Mrv8000x_ODSTK_TL_WARNING,
	Mrv8000x_ODSTK_TL_NORMAL,
	Mrv8000x_ODSTK_TL_DETAIL
} MRV8000X_ODSTK_TRACE_LEVEL, *PMRV8000X_ODSTK_TRACE_LEVEL;

// odstk supplicant status types
typedef enum _MRV8000X_ODSTK_STATUS
{
	Mrv8000xOdstkCallback,
	Mrv8000xOdstkLink,
	Mrv8000xOdstkAuth
} MRV8000X_ODSTK_STATUS, *PMRV8000X_ODSTK_STATUS;


typedef enum _MRVL_FSW_CCX_NETWORK_EAP
{
	Mrv8000xFswCcx_NetworkEapOff = 0,
	Mrv8000xFswCcx_NetworkEapOn,
	Mrv8000xFswCcx_NetworkEapAllowed,
	Mrv8000xFswCcx_NetworkEapPreferred
} MRVL_FSW_CCX_NETWORK_EAP;


//========================================================
//	Turbo Setup
//========================================================
#define TURBOSETUP_SETUP_REQ_START	0x0001
#define TURBOSETUP_SETUP_REQ_STOP 	0x0002
#define TURBOSETUP_SETUP_IP_FIXD    0x0001
#define TURBOSETUP_SETUP_IP_DHCP    0x0002

typedef enum _MRVL_TURBOSETUP_SECURITY
{
	Mrv8000xTSNoSecurity,
	Mrv8000xTSWPAPSK,
	Mrv8000xTSWEP64,
	Mrv8000xTSWEP128
} MRVL_TURBOSETUP_SECURITY;

typedef struct _MRVL_TURBOSETUP_SEC_INFO
{
    UCHAR Length;                       // Length of MRVL_TURBOSETUP_SEC_INFO
    UCHAR Version;                      // Version, current version is 0x01
    UCHAR PasswordStr[32];              // Up to 32 bytes of password, null terminated if length is less than 32
    UCHAR ChallengeText[128];           // 128 bytes challenge text, null terminated if length is less than 128
    USHORT IPAddressOption;
    UCHAR CurrentClientIPAddress[4];
} MRVL_TURBOSETUP_SEC_INFO, *PMRVL_TURBOSETUP_SEC_INFO;


typedef struct _MRVL_TURBOSETUP_SETUP_REQ
{
    UCHAR Length;                       // Length of MRVL_TURBOSETUP_SEC_INFO
    UCHAR Version;                      // Version, current version is 0x01
    USHORT Action;                      // TURBOSETUP_SETUP_REQ_START or TURBOSETUP_SETUP_REQ_STOP
	USHORT Security;
	USHORT WEPKeyIndex;
} MRVL_TURBOSETUP_SETUP_REQ, *PMRVL_TURBOSETUP_SETUP_REQ;


typedef struct _MRVL_TURBOSETUP_AP_INFO
{
	ULONG SsidLength;
	UCHAR Ssid[32];
	UCHAR APWinsName[16];
}MRVL_TURBOSETUP_AP_INFO, *PMRVL_TURBOSETUP_AP_INFO;




//====================================
//		DEVICE  DEFINITIONS
//====================================
#define MARVELL_8000x_DEV0		"\\\\.\\MrvlDev0"
#define MARVELL_8000x_DEV1		"\\\\.\\MrvlDev1"
#define MARVELL_8000x_DEV2		"\\\\.\\MrvlDev2"
#define MARVELL_8000x_DEV3		"\\\\.\\MrvlDev3"

#define MARVELL_8000x_GRU0		"\\\\.\\MrvlGru0"
#define MARVELL_8000x_GRU1		"\\\\.\\MrvlGru1"
#define MARVELL_8000x_GRU2		"\\\\.\\MrvlGru2"
#define MARVELL_8000x_GRU3		"\\\\.\\MrvlGru3"

//====================================
//		OID  DEFINITIONS
//====================================
#define OID_MRVL_REGION_CODE				0xFF010205
#define OID_MRVL_RADIO_PREAMBLE				0xFF010206
#define OID_MRVL_BSSID_SCAN					0xFF010207
#define OID_MRVL_SSID_SCAN					0xFF010208
#define OID_MRVL_RF_CHANNEL					0xFF010209
#define OID_MRVL_WLAN_MODE					0xFF01020A
#define OID_MRVL_CHIP_VERSION				0xFF01020B
#define OID_MRVL_BBP_REG_ACCESS				0xFF01020C
#define OID_MRVL_MAC_REG_ACCESS				0xFF01020D
#define OID_MRVL_RF_REG_ACCESS				0xFF01020E
#define OID_MRVL_HIDDEN_SSID				0xFF01020F

#define OID_MRVL_DISABLE_RADIO				0xFF010210
#define OID_MRVL_ENABLE_RADIO				0xFF010211
#define OID_MRVL_CHANNEL_RANGE				0xFF010212
#define OID_MRVL_SIGNAL_THRESHOLD			0xFF010213
#define OID_MRVL_TX_FAILURE_THRESHOLD		0xFF010214
//#define OID_MRVL_INTEROPERABILITY			0xFF010215
#define OID_MRVL_BOOST_MODE					0xFF010215
#define OID_MRVL_BEACON_STATS				0xFF010216
#define OID_MRVL_RSSI_STAT 					0xFF010217
#define OID_MRVL_RESET_STAT					0xFF010218

#define OID_MRVL_TURBOSETUP_SEC_INFO		0xFF010219
#define OID_MRVL_TURBOSETUP_SETUP_REQ		0xFF01021A
#define OID_MRVL_TURBOSETUP_AP_SSID			0xFF01021B

#define OID_MRVL_EAPOL_TX					0xFF01021C
#define OID_MRVL_BSSID_LIST					0xFF01021D

//#define OID_MRVL_ADHOC_B_RATES_ONLY			0xFF01021E
#define OID_MRVL_ADHOC_G_MODE				0xFF01021E
#define OID_MRVL_ADHOC_WLAN_MODE			0xFF01021E
#define OID_MRVL_TX_POWER_LEVEL				0xFF01021F
#define OID_MRVL_EVENT_CODE					0xFF010220
#define OID_MRVL_GUI_MONITOR				0xFF010221

// CCX
#define OID_FSW_CCX_CONFIGURATION			0xFFCCA000
#define OID_FSW_CCX_NETWORK_EAP				0xFFCCA001
#define OID_FSW_CCX_ROGUE_AP_DETECTED		0xFFCCA002
#define OID_FSW_CCX_REPORT_ROGUE_APS		0xFFCCA003
#define OID_FSW_CCX_AUTH_SUCCESS			0xFFCCA004
#define OID_FSW_CCX_CCKM_START				0xFFCCA005
#define OID_FSW_CCX_CCKM_REQUEST			0xFFCCA006
#define OID_FSW_CCX_CCKM_RESULT				0xFFCCA007

//====================================
//		EAPOL Packet Type
//====================================
#define EAPOL_ORD_PACKET					0x8E88
#define EAPOL_PREAUTH_PACKET				0xC788

//====================================
//		MISC  DEFINITIONS
//====================================
#define STATUS_UNSUCCESSFUL					((NTSTATUS)0xC0000001L)
#define NDIS_STATUS_SUCCESS                 ((NDIS_STATUS)STATUS_SUCCESS)
#define NDIS_STATUS_PENDING                 ((NDIS_STATUS)STATUS_PENDING)
#define NDIS_STATUS_FAILURE                 ((NDIS_STATUS)STATUS_UNSUCCESSFUL)


#define DEFAULT_RSSI_OFFSET			96	// the lowest RSSI indication sent from driver is -96
#define MAX_SQ						100

#define SET_AUTO_PREAMBLE			0x05 // bit 2 set to 1, bit 0 set to 1 for radio on
#define SET_SHORT_PREAMBLE			0x03 // bit 1 set to 1, bit 0 set to 1 for radio on
#define SET_LONG_PREAMBLE			0x01 // bit 1 set to 0, bit 0 set to 1 for radio on

#define AUTO_RATE					0
#define SET_OID_DELAY				100 // milliseconds

#define INVALID_REG_VALUE			0xFF
#define INVALID_READ_VALUE			0xFF

typedef ULONG NTSTATUS;


#define ADAPTER_REGPATH_BUF_SIZE	2000

#define INVALID_PROFILE				_T(" ")

#define EVENT_BUFFER_SIZE			16384//8192 should be enough, but we make it much bigger just for safety sake
#define EVENT_QUERY_BUF_SIZE		16384




//====================================
//		Internal Functions
//====================================
int Mrv8000xQueryInfo(HANDLE hAdapter, ULONG OidCode, LPBYTE lpData, ULONG nDataLen);
int Mrv8000xQueryInfo(HANDLE hAdapter, ULONG OidCode, LPBYTE lpData, ULONG nDataLen, DWORD &nSize);
int Mrv8000xSetInfo(HANDLE hAdapter, ULONG OidCode, LPBYTE lpData, ULONG nDataLen); 
int Mrv8000xQueryHardwareSpec(HANDLE hAdapter); 

DWORD WINAPI EventHandler(LPVOID lpvParam);

DWORD WINAPI WPALinkThread(LPVOID lpvParam);
void __stdcall WPACallback(void* hReference, ODSTK_CB_INFO* pCallbackInfo);

int Mrv8000xReceiveEapol(LPBYTE Buf);
void IndicateAuthStatus(ULONG nAuthState);
void IndicateLinkStatus(ULONG nLinkState);




/*
//OASISLIBRARY_API int Mrv8000xQueryInfo(HANDLE hAdapter, ULONG OidCode, LPBYTE lpData, ULONG nDataLen);
OASISLIBRARY_API int Mrv8000xInitialize(HWND hWnd);
OASISLIBRARY_API int Mrv8000xRegisterEvent(HANDLE hAdapter, LPBYTE lpBuf, ULONG nSize);
OASISLIBRARY_API int Mrv8000xRegisterExitEvent(HANDLE hAdapter, LPBYTE lpBuf, ULONG nSize);
OASISLIBRARY_API int Mrv8000xUnlockPages(HANDLE hAdapter);
OASISLIBRARY_API int Mrv8000xRegisterConnInfo(HANDLE hAdapter); 
OASISLIBRARY_API int Mrv8000xRevertConnInfo(HANDLE hAdapter); 
OASISLIBRARY_API int Mrv8000xGetEventCode(HANDLE hAdapter, LPBYTE lpBuf, ULONG nSize);
 
OASISLIBRARY_API BOOL Mrv8000xGetConnectStatus(HANDLE hAdapter);
OASISLIBRARY_API int Mrv8000xSetDisassociate(HANDLE hAdapter);
OASISLIBRARY_API int Mrv8000xGetMacAddress(HANDLE hAdapter, LPBYTE lpBuf, ULONG nSize);
OASISLIBRARY_API int Mrv8000xQueryDeviceDesc(HANDLE hAdapter, LPTSTR DevDesc, ULONG nSize);
OASISLIBRARY_API int Mrv8000xQueryScanList(HANDLE hAdapter, LPBYTE lpBuf, ULONG nSize);
OASISLIBRARY_API int Mrv8000xQuerySsid(HANDLE hAdapter, LPBYTE lpBuf, ULONG nSize);
OASISLIBRARY_API int Mrv8000xQueryBssid(HANDLE hAdapter, NDIS_802_11_MAC_ADDRESS lpBuf);
OASISLIBRARY_API int Mrv8000xQueryAuthMode(HANDLE hAdapter, DWORD &dwMode);
OASISLIBRARY_API int Mrv8000xQueryAuthMode(HANDLE hAdapter, DWORD &dwMode);
OASISLIBRARY_API int Mrv8000xQueryNetworkMode(HANDLE hAdapter, DWORD &dwMode);
OASISLIBRARY_API int Mrv8000xQueryWepStatus(HANDLE hAdapter, DWORD &dwStatus);
OASISLIBRARY_API int Mrv8000xQuerySignalInfo(HANDLE hAdapter, ULONG &nRSSI, ULONG &nSQ);
OASISLIBRARY_API int Mrv8000xQueryRssi(HANDLE hAdapter, ULONG &nRSSI);
OASISLIBRARY_API int Mrv8000xQueryLinkSpeed(HANDLE hAdapter, ULONG &nLinkSpeed);
OASISLIBRARY_API int Mrv8000xQueryPreamble(HANDLE hAdapter, ULONG &nPreamble);
OASISLIBRARY_API int Mrv8000xQueryTxAntenna(HANDLE hAdapter, ULONG &nAnt);
OASISLIBRARY_API int Mrv8000xQueryRxAntenna(HANDLE hAdapter, ULONG &nAnt);
OASISLIBRARY_API int Mrv8000xQueryFragThreshold(HANDLE hAdapter, ULONG &nFrag);
OASISLIBRARY_API int Mrv8000xQueryRTSThreshold(HANDLE hAdapter, ULONG &nRTS);
OASISLIBRARY_API int Mrv8000xQueryChannel(HANDLE hAdapter, ULONG &nChan);
OASISLIBRARY_API int Mrv8000xQueryPowerMode(HANDLE hAdapter, ULONG &nPowerMode);
OASISLIBRARY_API int Mrv8000xQueryDataRate(HANDLE hAdapter, ULONG &nRate); 
OASISLIBRARY_API int Mrv8000xQueryHardwareStatus(HANDLE hAdapter, ULONG &nInfo);
OASISLIBRARY_API int Mrv8000xQueryChipVersion(HANDLE hAdapter, ULONG &nInfo);
OASISLIBRARY_API int Mrv8000xQueryFirmwareVersion(HANDLE hAdapter, ULONG &nInfo);
OASISLIBRARY_API int Mrv8000xQueryRegionCode(HANDLE hAdapter, ULONG &nInfo);
OASISLIBRARY_API int Mrv8000xQueryStatistics(HANDLE hAdapter, PNDIS_802_11_STATISTICS lpBuf);
OASISLIBRARY_API int Mrv8000xQueryBeaconStatistics(HANDLE hAdapter, PMRVL_BEACON_STATS lpBuf);
OASISLIBRARY_API int Mrv8000xQueryRssiStat(HANDLE hAdapter, PMRVL_RSSI_STATS lpBuf);
OASISLIBRARY_API int Mrv8000xQueryWlanMode(HANDLE hAdapter, ULONG &nMode);
OASISLIBRARY_API int Mrv8000xQueryRadioControl(HANDLE hAdapter, ULONG &nCtrl);
//OASISLIBRARY_API int Mrv8000xQueryInterOperabilityMode(HANDLE hAdapter, ULONG &nMode);
OASISLIBRARY_API int Mrv8000xQueryBoostMode(HANDLE hAdapter, ULONG &nMode);
OASISLIBRARY_API int Mrv8000xQueryRegistryPath(HANDLE hAdapter, LPTSTR lpBuf, ULONG nSize); 
OASISLIBRARY_API int Mrv8000xQueryRegistry(LPCTSTR lpRegPath, LPCTSTR lpKeyName, LPTSTR lpData, ULONG nSize);
OASISLIBRARY_API int Mrv8000xQueryRegistry(LPCTSTR lpRegPath, LPCTSTR lpKeyName, DWORD &dwData);
OASISLIBRARY_API int Mrv8000xQueryLastConn(HANDLE hAdapter, PCONNECTIONINFO pBuf);
OASISLIBRARY_API int Mrv8000xQueryDriverVersion(HANDLE hAdapter, LPTSTR lpBuf, ULONG nSize);
OASISLIBRARY_API int Mrv8000xQueryDriverDate(HANDLE hAdapter, LPTSTR lpBuf, ULONG nSize);
OASISLIBRARY_API int Mrv8000xQueryProfileInUse(HANDLE hAdapter, LPTSTR lpProfile, ULONG nSize);
OASISLIBRARY_API int Mrv8000xQueryAdapterName(HANDLE hAdapter, LPTSTR lpProfile, ULONG nSize);
OASISLIBRARY_API int Mrv8000xQueryDhcp9x(HANDLE hAdapter);
OASISLIBRARY_API int Mrv8000xQueryTxPower(HANDLE hAdapter, ULONG &nPower);
OASISLIBRARY_API int Mrv8000xQueryConfiguration(HANDLE hAdapter, PNDIS_802_11_CONFIGURATION lpBuf);
//*
OASISLIBRARY_API int Mrv8000xScan(HANDLE hAdapter);
OASISLIBRARY_API int Mrv8000xSsidScan(HANDLE hAdapter, PMRV8000X_SSID_SCAN pBuf);
OASISLIBRARY_API int Mrv8000xBssidScan(HANDLE hAdapter, PMRV8000X_BSSID_SCAN pBuf);
OASISLIBRARY_API int Mrv8000xSetSsid(HANDLE hAdapter, LPTSTR lpBuf, ULONG nSize, BOOL bHidden); 
OASISLIBRARY_API int Mrv8000xSetSsid(HANDLE hAdapter, LPTSTR lpBuf, ULONG nSize); 
OASISLIBRARY_API int Mrv8000xSetAuthMode(HANDLE hAdapter, DWORD dwMode); 
OASISLIBRARY_API int Mrv8000xSetNetworkMode(HANDLE hAdapter, DWORD dwMode);
OASISLIBRARY_API int Mrv8000xSetWepStatus(HANDLE hAdapter, DWORD dwStatus);
OASISLIBRARY_API int Mrv8000xSetWepKey(HANDLE hAdapter, UINT nIndex, UCHAR* pKeyVal, UINT nKeySize, BOOL bIsTxKey);
OASISLIBRARY_API int Mrv8000xSetPreamble(HANDLE hAdapter, ULONG nPreamble);
OASISLIBRARY_API int Mrv8000xSetTxAntenna(HANDLE hAdapter, ULONG nAnt);
OASISLIBRARY_API int Mrv8000xSetRxAntenna(HANDLE hAdapter, ULONG nAnt);
OASISLIBRARY_API int Mrv8000xSetFragThreshold(HANDLE hAdapter, ULONG nFrag);
OASISLIBRARY_API int Mrv8000xSetRTSThreshold(HANDLE hAdapter, ULONG nRTS);
OASISLIBRARY_API int Mrv8000xSetChannel(HANDLE hAdapter, ULONG nCurChan);
OASISLIBRARY_API int Mrv8000xSetPowerMode(HANDLE hAdapter, ULONG nPowerMode);
OASISLIBRARY_API int Mrv8000xSetDataRate(HANDLE hAdapter, ULONG nRate);
OASISLIBRARY_API int Mrv8000xSetWlanMode(HANDLE hAdapter, ULONG nMode);
OASISLIBRARY_API int Mrv8000xSetRadioControl(HANDLE hAdapter, ULONG nCtrl);
//OASISLIBRARY_API int Mrv8000xSetInterOperabilityMode(HANDLE hAdapter, ULONG nMode);
OASISLIBRARY_API int Mrv8000xSetBoostMode(HANDLE hAdapter, ULONG nMode);
OASISLIBRARY_API int Mrv8000xSetTxPower(HANDLE hAdapter, ULONG nPower);
//OASISLIBRARY_API int Mrv8000xSetAdHocGMode(HANDLE hAdapter, ULONG bUse80211bRates);
OASISLIBRARY_API int Mrv8000xSetAdHocWlanMode(HANDLE hAdapter, ULONG nAdHocWlanMode);
OASISLIBRARY_API int Mrv8000xSetGUIMonitorMode(HANDLE hAdapter, ULONG nMode);
OASISLIBRARY_API int Mrv8000xSetConfiguration(HANDLE hAdapter, PNDIS_802_11_CONFIGURATION lpBuf);

// Turbo Setup
OASISLIBRARY_API int Mrv8000xQueryTurboSetupAPInfo(HANDLE hAdapter, PMRVL_TURBOSETUP_AP_INFO);
OASISLIBRARY_API int Mrv8000xSetTSSecurityInfo(HANDLE hAdapter, PMRVL_TURBOSETUP_SEC_INFO pSec);
OASISLIBRARY_API int Mrv8000xSetTSSetupRequest(HANDLE hAdapter, PMRVL_TURBOSETUP_SETUP_REQ pReq);
//* /
OASISLIBRARY_API int Mrv8000xResetStatistics(HANDLE hAdapter);
OASISLIBRARY_API int Mrv8000xSetRegistry(LPCTSTR lpRegPath, LPCTSTR lpKeyName, LPCTSTR lpValue, DWORD nSize, BOOL bIsMultiStr);
OASISLIBRARY_API int Mrv8000xSetRegistry(LPCTSTR lpRegPath, LPCTSTR lpKeyName, DWORD nValue);
OASISLIBRARY_API int Mrv8000xSetProfileInUse(HANDLE hAdapter, LPTSTR lpBuf, ULONG nSize);
/*
// WPA
OASISLIBRARY_API int Mrv8000xSetEAPOLTx(HANDLE hAdapter, const BYTE* pSourceMac, const BYTE* pDestMac, const BYTE* pData, ULONG nDataSize);
OASISLIBRARY_API int Mrv8000xSetAddKey(HANDLE hAdapter, void* pData, ULONG nSize);
OASISLIBRARY_API int Mrv8000xSetRemoveKey(HANDLE hAdapter, void* pData, ULONG nSize);
OASISLIBRARY_API int Mrv8000xQueryAssocInfo(HANDLE hAdapter, LPBYTE lpBuf, DWORD &nSize);
OASISLIBRARY_API int Mrv8000xQueryScanListLength(HANDLE hAdapter, LPBYTE lpBuf, ULONG nSize, DWORD &nBufSize);
OASISLIBRARY_API int Mrv8000xSetWPAWepKey(HANDLE hAdapter, PNDIS_802_11_WEP pKeyVal, UINT nKeySize);

//WPA2
OASISLIBRARY_API int Mrv8000xSetPMKID(HANDLE hAdapter, PNDIS_802_11_PMKID pPMKID, ULONG nSize);
OASISLIBRARY_API int Mrv8000xSetPreAuthEAPOLTx(HANDLE hAdapter, const BYTE* pSourceMac, const BYTE* pDestMac, const BYTE* pData, ULONG nDataSize);
* /
// CCX
OASISLIBRARY_API int Mrv8000xSetFswCCXConfiguration(HANDLE hAdapter, ULONG nConfiguration);
OASISLIBRARY_API int Mrv8000xSetFswCCXRogueAP(HANDLE hAdapter, LPBYTE pRogue, ULONG nSize);
OASISLIBRARY_API int Mrv8000xSetFswCCXReportRogueAPs(HANDLE hAdapter, NDIS_802_11_MAC_ADDRESS lpBssid);
OASISLIBRARY_API int Mrv8000xSetFswCCXNetworkEap(HANDLE hAdapter, ULONG nEapType);
OASISLIBRARY_API int Mrv8000xSetFswCCXNotifyAuthSuccess(HANDLE hAdapter, LPBYTE pNotify, ULONG nSize);
OASISLIBRARY_API int Mrv8000xSetFswCCXCCKMRequest(HANDLE hAdapter, LPBYTE pRequest, ULONG nSize);
OASISLIBRARY_API int Mrv8000xSetFswCCXCCKMResult(HANDLE hAdapter, LPBYTE pResult, ULONG nSize);
*/
// Register Access
/*
OASISLIBRARY_API int Mrv8000xQueryRfRegister(HANDLE hAdapter, DWORD index, DWORD &Value);
OASISLIBRARY_API int Mrv8000xQueryBbRegister(HANDLE hAdapter, DWORD index, DWORD &Value);
OASISLIBRARY_API int Mrv8000xQueryMacRegister(HANDLE hAdapter, DWORD index, DWORD &Value);

OASISLIBRARY_API int Mrv8000xSetRfRegister(HANDLE hAdapter, DWORD index, DWORD Value);
OASISLIBRARY_API int Mrv8000xSetBbRegister(HANDLE hAdapter, DWORD index, DWORD Value);
OASISLIBRARY_API int Mrv8000xSetMacRegister(HANDLE hAdapter, DWORD index, DWORD Value);


//===============================================================================
//	Added by Fred for testing purpose, features can be found in GUI Test Page
//===============================================================================
OASISLIBRARY_API int Mrv8000xSetChannelRange(HANDLE hAdapter, ULONG dwMode);
OASISLIBRARY_API int Mrv8000xQueryChannelRange(HANDLE hAdapter, ULONG &dwData);
OASISLIBRARY_API int Mrv8000xSetSignalThreshold(HANDLE hAdapter, ULONG nSignal);
OASISLIBRARY_API int Mrv8000xQuerySignalThreshold(HANDLE hAdapter, ULONG &nSignal);
OASISLIBRARY_API int Mrv8000xSetTxFailThreshold(HANDLE hAdapter, ULONG nVal);
OASISLIBRARY_API int Mrv8000xQueryTxFailThreshold(HANDLE hAdapter, ULONG &nVal);


//===============================================================================
//  Functions for WPA support
//===============================================================================
// WPASupp
OASISLIBRARY_API int Mrv8000xWPAInitialize();
OASISLIBRARY_API void Mrv8000xWPATerminate();
OASISLIBRARY_API void Mrv8000xWPAPortDown();
OASISLIBRARY_API void Mrv8000xWPAPortUp();
OASISLIBRARY_API void Mrv8000xWPATrace(ULONG nLevel);
OASISLIBRARY_API int Mrv8000xWPASetNetworkInfo(HANDLE hAdapter,
											   ULONG AuthMode,
											   ULONG Security, 
											   LPBYTE lpWPAPassPhrase,
											   LPTSTR lpSSID,
											   LPBYTE lpMacAddress,
											   LPBYTE lpBSSID);
OASISLIBRARY_API int Mrv8000xWPACreateIdentityTLS(LPTSTR lpWPALoginName, 
												 LPBYTE lpWPAHash);
OASISLIBRARY_API int Mrv8000xWPACreateIdentityPEAP(LPTSTR lpWPALoginName, 
												   LPTSTR lpWPADomain, 
												   LPTSTR lpWPAPassword);
OASISLIBRARY_API int Mrv8000xWPACreateIdentityTtls(LPTSTR lpWPALoginName, 
												   LPTSTR lpWPADomain, 
												   LPTSTR lpWPAPassword, 
												   PTSTR lpWPAAnonymousName);
OASISLIBRARY_API int Mrv8000xWPACreateIdentityLEAP(LPTSTR lpWPALoginName, 
												   LPTSTR lpWPADomain, 
												   LPTSTR lpWPAPassword);


//==================================================
//			Simplex API
//==================================================
OASISLIBRARY_API int Mrv8000xSetWPAInformation(HANDLE hAdapter,
											   ULONG NetworkMode,
											   ULONG AuthMode,
											   ULONG Security, 
											   LPBYTE lpWPAPassPhrase,
											   LPTSTR lpSSID);

OASISLIBRARY_API int Mrv8000xGetWPASuppStatus(ULONG nType);

*/


