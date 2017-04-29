/** @file DutNdis.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#ifndef _DUT_H_
#define _DUT_H_

//#include <afx.h>

#define VERSION   "030215"

// TODO match with REQ_DATA_SZ of usbhal.h
#define SET_SZ   1072+16 //384
// TODO match with RESP_DATA_SZ of usbhal.h
#define QUERY_SZ 1072+16 //384

#define RET_ERR_USAGE   -2

#define NDEV         128
#define NDESCRIPTION 128
#define NNAME        256

//#ifdef _LINUX_
//typedef unsigned long DWORD;
//typedef unsigned short WORD;
//typedef unsigned char BYTE;
//typedef unsigned int BOOL;
//typedef unsigned char* LPSTR;
//typedef unsigned char  UCHAR;
//typedef unsigned short USHORT;
//typedef unsigned long ULONG;
//typedef unsigned long HANDLE;
//#endif //#ifdef _LINUX_

typedef struct oidstr_t
{
   DWORD oid;
   unsigned char *str;
//    CString str; 
} oidstr_t;

/*
typedef struct _dev_t {
   LPSTR AdapterDescription;
   LPSTR AdapterName;
} dev_t;
*/


// Ensure that the app uses the same data types as driver


#define HostCmd_CMD_802_11_SCAN                 0x0006
#define HostCmd_BSS_TYPE_ANY                    0x0003
#define HostCmd_SCAN_TYPE_PASSIVE               0x0001
#define HostCmd_SCAN_PROBE_DEALY_TIME           0
#define HostCmd_SCAN_802_11_B_CHANNELS          11
#define HostCmd_SCAN_MIN_CH_TIME                6
#define HostCmd_SCAN_MAX_CH_TIME                12
#define EAGLE_ETH_ADDR_LEN                      6
#define EAGLE_MAX_SSID_LENGTH			32
#define EAGLE_MAX_CHANNEL_NUMBER		14
#define EAGLE_MAX_BSSID_LIST           0x0000000e
#define EAGLE_RSSI_DEFAULT_NOISE_VALUE	(-96)

//          Define action or option for HostCmd_CMD_802_11_SCAN
#define HostCmd_CMD_NONE                        0x0000
#define HostCmd_CMD_CODE_DNLD                   0x0001
#define HostCmd_CMD_OP_PARAM_DNLD               0x0002
#define HostCmd_CMD_GET_HW_SPEC                 0x0003
#define HostCmd_CMD_EEPROM_UPDATE               0x0004
#define HostCmd_CMD_802_11_RESET                0x0005
#define HostCmd_CMD_802_11_SCAN                 0x0006
#define HostCmd_CMD_802_11_QUERY_SCAN_RESULT    0x0029
#define HostCmd_CMD_802_11_QUERY_TRAFFIC        0x0009
#define HostCmd_CMD_802_11_QUERY_STATUS         0x000a
#define HostCmd_CMD_802_11_GET_LOG              0x000b
#define HostCmd_CMD_MAC_CONTROL                 0x0028
#define HostCmd_CMD_MAC_MULTICAST_ADR           0x0010
#define HostCmd_CMD_802_11_AUTHENTICATE         0x0011
#define HostCmd_CMD_802_11_DEAUTHENTICATE       0x0024
#define HostCmd_CMD_802_11_ASSOCIATE            0x0012
#define HostCmd_CMD_802_11_REASSOCIATE          0x0025
#define HostCmd_CMD_802_11_DISASSOCIATE         0x0026
#define HostCmd_CMD_802_11_SET_WEP              0x0013
#define HostCmd_CMD_802_11_GET_STAT             0x0014
#define HostCmd_CMD_802_3_GET_STAT              0x0015
#define HostCmd_CMD_802_11_SNMP_MIB             0x0016
#define HostCmd_CMD_MAC_REG_MAP                 0x0017
#define HostCmd_CMD_BBP_REG_MAP                 0x0018
#define HostCmd_CMD_RF_REG_MAP                  0x0023
#define HostCmd_CMD_MAC_REG_ACCESS              0x0019
#define HostCmd_CMD_BBP_REG_ACCESS              0x001a
#define HostCmd_CMD_RF_REG_ACCESS               0x001b
#define HostCmd_CMD_802_11_RADIO_CONTROL        0x001c
#define HostCmd_CMD_802_11_RF_CHANNEL           0x001d
#define HostCmd_CMD_802_11_RF_TX_POWER          0x001e
#define HostCmd_CMD_802_11_RSSI                 0x001f
#define HostCmd_CMD_802_11_RF_ANTENNA           0x0020
#define HostCmd_CMD_802_11_PS_MODE              0x0021
#define HostCmd_CMD_802_11_DATA_RATE            0x0022

// comatose mode
#define HostCmd_CMD_802_11_Comatose				0x0030

// manufacture command support
#ifdef _IF_USB_
#define HostCmd_CMD_MFG_COMMAND					0x0040
#elif defined(_IF_UDP) || defined(_LINUX_)
#define HostCmd_CMD_MFG_COMMAND					0x0089
#else  //SDIO
#define HostCmd_CMD_MFG_COMMAND					0x0040
#endif

#ifdef NDIS50_MINIPORT
#define HostCmd_CMD_DUTY_CYCLE_TEST				0x002A
#endif
#define HostCmd_CMD_802_11_AD_HOC_START         0x002B
#define HostCmd_CMD_802_11_AD_HOC_JOIN          0x002C
#define HostCmd_CMD_SET_ACTIVE_SCAN_SSID		0x002D



#define HostCmd_RET_NONE                        0x8000
#define HostCmd_RET_HW_SPEC_INFO                0x8003
#define HostCmd_RET_EEPROM_UPDATE               0x8004
#define HostCmd_RET_802_11_RESET                0x8005
#define HostCmd_RET_802_11_SCAN                 0x8006
#define HostCmd_RET_802_11_QUERY_SCAN_RESULT    0x8029
#define HostCmd_RET_802_11_QUERY_TRAFFIC        0x8009
#define HostCmd_RET_802_11_STATUS_INFO          0x800a
#define HostCmd_RET_802_11_GET_LOG              0x800b
#define HostCmd_RET_MAC_CONTROL                 0x8028
#define HostCmd_RET_MAC_MULTICAST_ADR           0x8010
#define HostCmd_RET_802_11_AUTHENTICATE         0x8011
#define HostCmd_RET_802_11_DEAUTHENTICATE       0x8024
#define HostCmd_RET_802_11_ASSOCIATE            0x8012
#define	HostCmd_RET_802_11_REASSOCIATE			0x8025
#define HostCmd_RET_802_11_DISASSOCIATE         0x8026
#define HostCmd_RET_802_11_SET_WEP              0x8013
#define HostCmd_RET_802_11_STAT                 0x8014
#define HostCmd_RET_802_3_STAT                  0x8015
#define HostCmd_RET_802_11_SNMP_MIB             0x8016
#define HostCmd_RET_MAC_REG_MAP                 0x8017
#define HostCmd_RET_BBP_REG_MAP                 0x8018
#define HostCmd_RET_RF_REG_MAP                  0x8023
#define HostCmd_RET_MAC_REG_ACCESS              0x8019
#define HostCmd_RET_BBP_REG_ACCESS              0x801a
#define HostCmd_RET_RF_REG_ACCESS               0x801b
#define HostCmd_RET_802_11_RADIO_CONTROL        0x801c
#define HostCmd_RET_802_11_RF_CHANNEL           0x801d
#define HostCmd_RET_802_11_RSSI                 0x801f
#define HostCmd_RET_802_11_RF_TX_POWER          0x801e
#define HostCmd_RET_802_11_RF_ANTENNA           0x8020
#define HostCmd_RET_802_11_PS_MODE              0x8021
#define HostCmd_RET_802_11_DATA_RATE            0x8022

#ifdef NDIS50_MINIPORT 
#define HostCmd_RET_DUTY_CYCLE_TEST             0x802A
#endif

#define HostCmd_RET_802_11_AD_HOC_START         0x802B
#define HostCmd_RET_802_11_AD_HOC_JOIN          0x802C
#define HostCmd_RET_SET_ACTIVE_SCAN_SSID		0x802D


#define HostCmd_ACT_GEN_READ                    0x0000
#define HostCmd_ACT_GEN_WRITE                   0x0001
#define HostCmd_ACT_GEN_GET                     0x0000
#define HostCmd_ACT_GEN_SET                     0x0001
#define HostCmd_ACT_GEN_OFF                     0x0000
#define HostCmd_ACT_GEN_ON                      0x0001




#define HostCmd_TYPE_802_11A                    0x0001
#define HostCmd_TYPE_802_11B                    0x0002

typedef enum
{
   SSID = 0,
   SUPPORTED_RATES,
   FH_PARAM_SET,
   DS_PARAM_SET,
   CF_PARAM_SET,
   TIM,
   IBSS_PARAM_SET,
   CHALLENGE_TEXT = 16
} IEEEtypes_ElementId_e;

typedef struct IEEEtypes_CapInfo_t
{
   UCHAR Ess:1;
   UCHAR Ibss:1;
   UCHAR CfPollable:1;
   UCHAR CfPollRqst:1;
   UCHAR Privacy:1;
   UCHAR ShortPreamble:1;
   UCHAR Pbcc:1;
   UCHAR ChanAgility:1 ;
   UCHAR Rsrvd;
} IEEEtypes_CapInfo_t;

typedef struct IEEEtypes_CfParamSet_t
{
   UCHAR  ElementId;
   UCHAR  Len;
   UCHAR  CfpCnt;
   UCHAR  CfpPeriod;
   USHORT CfpMaxDuration;
   USHORT CfpDurationRemaining;
} IEEEtypes_CfParamSet_t;

typedef struct IEEEtypes_IbssParamSet_t
{
   UCHAR  ElementId;
   UCHAR  Len;
   USHORT AtimWindow;
} IEEEtypes_IbssParamSet_t;

typedef union IEEEtypes_SsParamSet_t
{
   IEEEtypes_CfParamSet_t    CfParamSet;
   IEEEtypes_IbssParamSet_t  IbssParamSet;
} IEEEtypes_SsParamSet_t;

typedef struct IEEEtypes_FhParamSet_t
{
   UCHAR  ElementId;
   UCHAR  Len;
   USHORT DwellTime;
   UCHAR  HopSet;
   UCHAR  HopPattern;
   UCHAR  HopIndex;
} IEEEtypes_FhParamSet_t;

typedef struct IEEEtypes_DsParamSet_t
{
   UCHAR ElementId;
   UCHAR Len;
   UCHAR CurrentChan;
} IEEEtypes_DsParamSet_t;

typedef union IEEEtypes_PhyParamSet_t
{
   IEEEtypes_FhParamSet_t  FhParamSet;
   IEEEtypes_DsParamSet_t  DsParamSet;
} IEEEtypes_PhyParamSet_t;

typedef struct _BSS_DESCRIPTION_SET_ALL_FIELDS
{
    UCHAR                     BSSID[EAGLE_ETH_ADDR_LEN];
    UCHAR                     SSID[EAGLE_MAX_SSID_LENGTH];
    UCHAR                     BSSType;
    USHORT                    BeaconPeriod;
    UCHAR                     DTIMPeriod;
    UCHAR                     TimeStamp[8];
    UCHAR                     LocalTime[8];
    IEEEtypes_PhyParamSet_t   PhyParamSet;
    IEEEtypes_SsParamSet_t    SsParamSet;
    IEEEtypes_CapInfo_t       Cap;
    UCHAR                     DataRates[8];
} BSS_DESCRIPTION_SET_ALL_FIELDS, *PBSS_DESCRIPTION_SET_ALL_FIELDS;


typedef struct _BSS_DESCRIPTION_SET_FIXED_FIELDS {
    UCHAR BSSID[EAGLE_ETH_ADDR_LEN];
    UCHAR SSID[EAGLE_MAX_SSID_LENGTH];
	UCHAR BSSType;
	USHORT BeaconPeriod;
	UCHAR DTIMPeriod;
	UCHAR TimeStamp[8];
	UCHAR LocalTime[8];
} BSS_DESCRIPTION_SET_FIXED_FIELDS, *PBSS_DESCRIPTION_SET_FIXED_FIELDS;

//#pragma pack(1)

typedef struct _HostCmd_DS_802_11_SCAN {
    USHORT Command;
    USHORT Size;
    USHORT SeqNum;
    USHORT Result;
 	USHORT IsAutoAssociation;
	UCHAR BSSType;
	UCHAR BSSID[EAGLE_ETH_ADDR_LEN];
	UCHAR SSID[EAGLE_MAX_SSID_LENGTH];
	UCHAR ScanType;
	USHORT ProbeDelay;
	UCHAR CHList[EAGLE_MAX_CHANNEL_NUMBER];
	USHORT MinCHTime;
	USHORT MaxCHTime;
	USHORT Reserved;
} HostCmd_DS_802_11_SCAN, *PHostCmd_DS_802_11_SCAN;

//#pragma pack()

#define EAGLE_MAX_BSS_DESCRIPTS        10

#define HostCmd_RET_802_11_SCAN                 0x8006

typedef struct _HostCmd_DS_802_11_SCAN_RSP {
    USHORT Command;
    USHORT Size;
    USHORT SeqNum;
    USHORT Result;
    UCHAR BSSID[EAGLE_ETH_ADDR_LEN];
	UCHAR RSSI[EAGLE_MAX_BSS_DESCRIPTS];
	USHORT BSSDescriptSize;
	UCHAR  NumberOfSets;
} HostCmd_DS_802_11_SCAN_RSP, *PHostCmd_DS_802_11_SCAN_RSP;

//          Define data structure for HostCmd_CMD_802_11_RF_CHANNEL
typedef struct _HostCmd_DS_802_11_RF_CHANNEL {
    USHORT Command;
    USHORT Size;
    USHORT SeqNum;
    USHORT Result;
    USHORT Action;
    USHORT RFType;                  // HostCmd_TYPE_802_11A or HostCmd_TYPE_802_11A                                   
    UCHAR ChannelList[EAGLE_MAX_CHANNEL_NUMBER];
    UCHAR CurentChannel;
    UCHAR Reserved;
} HostCmd_DS_802_11_RF_CHANNEL, *PHostCmd_DS_802_11_RF_CHANNEL;

 
//          Define data structure for HostCmd_CMD_MAC_REG_ACCESS
typedef struct _HostCmd_DS_MAC_REG_ACCESS {
    USHORT Command;
    USHORT Size;
    USHORT SeqNum;
    USHORT Result;
    USHORT Action;
    USHORT Offset;
    ULONG Value;
    USHORT Reserved;
} HostCmd_DS_MAC_REG_ACCESS, *PHostCmd_DS_MAC_REG_ACCESS;

//          Define data structure for HostCmd_CMD_BBP_REG_ACCESS
typedef struct _HostCmd_DS_BBP_REG_ACCESS {
    USHORT Command;
    USHORT Size;
    USHORT SeqNum;
    USHORT Result;
    USHORT Action;
    USHORT Offset;
    UCHAR Value;
    UCHAR Reserverd[3];
} HostCmd_DS_BBP_REG_ACCESS, *PHostCmd_DS_BBP_REG_ACCESS;

//          Define data structure for HostCmd_CMD_RF_REG_ACCESS
typedef struct _HostCmd_DS_RF_REG_ACCESS {
    USHORT Command;
    USHORT Size;
    USHORT SeqNum;
    USHORT Result;
    USHORT Action;
    USHORT Offset;
    UCHAR Value;
    UCHAR Reserverd[3];
} HostCmd_DS_RF_REG_ACCESS, *PHostCmd_DS_RF_REG_ACCESS;

#endif   // _usb_h_
