#include <tchar.h>

#define REGSTR_CLASS_NET_98		_T("System\\CurrentControlSet\\Services\\Class\\")
#define REGSTR_CLASS_NET_NT5	_T("System\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002BE10318}")
#define TARGET_NAME		 		_T("Marvell Libertas 802.11b/g") //_T("Ethernet") //
#define NAME_PREFIX				_T("\\\\.\\")
#define DEV_PREFIX			    _T("\\Device\\")

#define REGKEY_CFG_MGR_ENUM		_T("Config Manager\\Enum")
#define REGKEY_HW_KEY			_T("HardWareKey")
#define REGKEY_ENUM				_T("Enum")
#define REGKEY_DRIVER			_T("Driver")

#define REGKEY_SERVICE			_T("NetCfgInstanceId") 
#define REGKEY_COMPONENT_ID		_T("ComponentId")
#define REGKEY_DRVDESC			_T("DriverDesc")
#define REGKEY_DRVVER_NT		_T("DriverVersion")
#define REGKEY_DRVVER_98		_T("DriverVer")
#define REGKEY_DRVDATE			_T("DriverDate")
#define REGKEY_DRVREL			_T("DriverRel")

#define REGSTR_ENUM_NETWORK		_T("Enum\\Network\\")
#define REGSTR_ENUM				_T("Enum\\")
#define REGSTR_NETWORK			_T("Network\\")
#define REGKEY_BINDINGS			_T("\\Bindings")
#define REGKEY_MSTCP			_T("MSTCP")
#define REGKEY_IPADDRESS		_T("IPAddress")
#define REGKEY_IPMASK			_T("IPMask")

#define REGKEY_MACADDR_UPPER	_T("MacAddressUpper")
#define REGKEY_MACADDR_LOWER	_T("MacAddressLower")

#define REGKEY_PROFILE_NAME		_T("ProfileName") 
#define REGKEY_ASSOCIATED_SSID	_T("DesiredSSID") 
#define REGKEY_POWER_MODE		_T("PowerMode")
#define REGKEY_AUTH_MODE		_T("AuthMode")
#define REGKEY_WEP_STATUS		_T("WepStatus")
#define REGKEY_TX_KEY			_T("TxWepKey")
#define REGKEY_WEP_KEY1			_T("WepKey1")
#define REGKEY_WEP_KEY2			_T("WepKey2")
#define REGKEY_WEP_KEY3			_T("WepKey3")
#define REGKEY_WEP_KEY4			_T("WepKey4")
#define REGKEY_CHANNEL			_T("Channel")
#define REGKEY_DATA_RATE		_T("DataRate")
#define REGKEY_NETWORK_MODE		_T("NetworkMode")
#define REGKEY_RADIO_PREAMBLE	_T("Preamble")
#define REGKEY_TX_ANTENNA		_T("TxAntenna")
#define REGKEY_RX_ANTENNA		_T("RxAntenna")
#define REGKEY_RTS_THRESHOLD	_T("RTSThsd")
#define REGKEY_FRAG_THRESHOLD	_T("FragThsd")
#define REGKEY_WEPKEY_SIZE		_T("WepKeySize")
#define REGKEY_WIRELESS_MODE	_T("Wireless Mode")
#define REGKEY_RADIO_ON			_T("RadioOn")
#define REGKEY_TX_POWER			_T("TxPowerLvl")
//#define REGKEY_INTEROPERABILITY	_T("InterOperabilityOn")
#define REGKEY_BOOST_MODE		_T("BoostMode")
#define REGKEY_ADHOC_G_MODE		_T("AdhocGMode")
#define REGKEY_ADHOC_WLAN_MODE		_T("AdhocGMode")
#define REGKEY_ADHOC_BEACON_PERIOD	_T("AdhocBeaconPeriod")
#define REGKEY_DTIM_PERIOD		_T("DTIMPeriod")


//======================================
//  Added by Fred for testing purpose
//======================================
#define REGKEY_CH_RANGE				_T("ChannelRange")
#define REGKEY_SIGNAL_THRESHOLD		_T("SignalThsds")
#define REGKEY_TX_FAIL_THRESHOLD	_T("TxFailThsds")


//==================================
//	Registry Functions
//==================================
int GetWindowsVersion();
int GetRegistryPath();
int GetRegistryPathNT5();
int GetRegistryPath98();
int SetRegistryInfo(HANDLE hAdapter, LPCTSTR lpKeyName, LPCTSTR lpValue, DWORD nSize);
int SetRegistryInfo(HANDLE hAdapter, LPCTSTR lpKeyName, DWORD dwValue, BOOL bIsHex);
int QueryRegistryInfo(HANDLE hAdapter, LPCTSTR lpKeyName, DWORD &dwData);
int QueryRegistryInfo(HANDLE hAdapter, LPCTSTR lpKeyName, LPBYTE lpData, DWORD &nSize);
int GetNetTransRegPath9x(LPTSTR szRegKey, ULONG nSize);
