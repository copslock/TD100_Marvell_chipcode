/** @file FmMfgCmd.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

// Command Code

#ifndef _FM_MFG_FM_CMD_
#define _FM_MFG_FM_CMD_
 
#if defined(WIN32) || defined(NDIS51_MINIPORT) || defined(NDIS50_MINIPORT) || defined(_LINUX_)
#pragma pack(1) 
#ifdef _LINUX_
#define PACK __attribute__ ((packed))
#else
#define PACK
#endif
#else
#include "win.h"  
#endif
 
#ifdef _BT_USB_ 
#define MRVL_HCI_OID_MFG_	  0xfe000001 //to XP USB driver 
#elif defined (_BT_UART_)
#define MRVL_HCI_OID_MFG_BT	   ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|(HCI_CMD_MRVL_MFG_BT))
#else
#define MRVL_HCI_OID_MFG_BT	   ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|(HCI_CMD_MRVL_MFG_BT))
#endif // #ifdef _BT_USB_ 
 

#define BASE_FM_COMMANDCODE   		(0x3000)
#define BASE_FM_RESPONSEMASK		(0x8000)
#define BASE_FM_RESPONSECODE  		(BASE_FM_COMMANDCODE | BASE_FM_RESPONSEMASK)
#define FMCMD_RSP(x)				(BASE_FM_RESPONSEMASK|(0xFFFF&x))

typedef enum
{
    MFG_FM_CMD_NONE						= (BASE_FM_COMMANDCODE | 0x0000),
	MFG_FM_CMD_FMU_REG					= (BASE_FM_COMMANDCODE | 0x0001),	// mfgFm_CmdFmReg_t
    MFG_FM_CMD_FW_VERS					= (BASE_FM_COMMANDCODE | 0x0002),	// mfgFm_CmdFmDataD_t
    MFG_FM_CMD_HW_VERS					= (BASE_FM_COMMANDCODE | 0x0003),	// mfgFm_CmdFmDataD_t
    MFG_FM_CMD_HW_ID					= (BASE_FM_COMMANDCODE | 0x0004),	// mfgFm_CmdFmDataD_t
    MFG_FM_CMD_VENDOR_ID				= (BASE_FM_COMMANDCODE | 0x0005),	// mfgFm_CmdFmDataD_t

    MFG_FM_CMD_RESET_removed					= (BASE_FM_COMMANDCODE | 0x0006),	// mfgFm_Cmd_t
    MFG_FM_CMD_INIT						= (BASE_FM_COMMANDCODE | 0x0007),	// mfgFm_CmdFmDataD_t	set only
	MFG_FM_CMD_FMPOWERUPMODE			= (BASE_FM_COMMANDCODE | 0x0008),	// mfgFm_CmdFmDataD_t	set only
	MFG_FM_CMD_FMCHAN					= (BASE_FM_COMMANDCODE | 0x0009),	// mfgFm_CmdFmDataD_t	set/get
    MFG_FM_CMD_AFCHAN					= (BASE_FM_COMMANDCODE | 0x000A),	// mfgFm_CmdFmDataD_t	set only
    MFG_FM_CMD_SEARCH_DIR				= (BASE_FM_COMMANDCODE | 0x000B),	// mfgFm_CmdFmDataD_t	set/get
    MFG_FM_CMD_AUTOSEARCHMODE			= (BASE_FM_COMMANDCODE | 0x000C),	// mfgFm_CmdFmDataD_t	set/get
    MFG_FM_CMD_RX_FORCE_MONOMODE		= (BASE_FM_COMMANDCODE | 0x000D),	// mfgFm_CmdFmDataD_t	set/get
    MFG_FM_CMD_RSSI_THRESHOLD			= (BASE_FM_COMMANDCODE | 0x000E),	// mfgFm_CmdFmDataD_t
    MFG_FM_CMD_FMBAND					= (BASE_FM_COMMANDCODE | 0x000F),	// mfgFm_CmdFmDataD_t	set only
    MFG_FM_CMD_CHAN_STEPSIZE			= (BASE_FM_COMMANDCODE | 0x0010),	// mfgFm_CmdFmDataD_t 	set only
    MFG_FM_CMD_CHAN_STEPUPDN			= (BASE_FM_COMMANDCODE | 0x0011),	// mfgFm_CmdFmDataD_t 	set only
    MFG_FM_CMD_CURRENT_RSSI				= (BASE_FM_COMMANDCODE | 0x0012),	// mfgFm_CmdFmDataD_t	get only

    MFG_FM_CMD_AUDIODE_EMPHASIS			= (BASE_FM_COMMANDCODE | 0x0013),	// mfgFm_CmdFmDataD_t	set only
    MFG_FM_CMD_RXMUTEMODE				= (BASE_FM_COMMANDCODE | 0x0014),	// mfgFm_CmdFmDataD_t	set/get
    MFG_FM_CMD_RXAUDIOPAUSELEVEL		= (BASE_FM_COMMANDCODE | 0x0015),	// mfgFm_CmdFmDataD_t	set only
    MFG_FM_CMD_RXAUDIOPAUSEDURATION		= (BASE_FM_COMMANDCODE | 0x0016),	// mfgFm_CmdFmDataD_t	set only 
    MFG_FM_CMD_RXAUDIOPATH				= (BASE_FM_COMMANDCODE | 0x0017),	// mfgFm_CmdFmDataD3_t	set/get
    MFG_FM_CMD_RXRDSDATAPATH_removed		= (BASE_FM_COMMANDCODE | 0x0018),	// mfgFm_CmdFmDataD_t 
    MFG_FM_CMD_RXRDSSYNCSTATUS			= (BASE_FM_COMMANDCODE | 0x0019),	// mfgFm_CmdFmDataD_t	get only
    MFG_FM_CMD_RXCURRENTFLAGS			= (BASE_FM_COMMANDCODE | 0x001A),	// mfgFm_CmdFmDataD_t	get only
    MFG_FM_CMD_RXRDSDATA_not_implement		= (BASE_FM_COMMANDCODE | 0x001B),	// mfgFm_CmdFmDataD_t	not implement
    MFG_FM_CMD_FLUSHRDS					= (BASE_FM_COMMANDCODE | 0x001C),	// mfgFm_CmdFmDataD_t	set only
    MFG_FM_CMD_RDSMEMDEPTH				= (BASE_FM_COMMANDCODE | 0x001D),	// mfgFm_CmdFmDataD_t	set/get
    MFG_FM_CMD_RDSBLOCKB				= (BASE_FM_COMMANDCODE | 0x001E),	// mfgFm_CmdFmDataD_t	set/get
    MFG_FM_CMD_RDSBLOCKBMASK			= (BASE_FM_COMMANDCODE | 0x001F),	// mfgFm_CmdFmDataD_t	set/get
    MFG_FM_CMD_RDSPICODE				= (BASE_FM_COMMANDCODE | 0x0020),	// mfgFm_CmdFmDataD_t	set/get
    MFG_FM_CMD_RDSPICODEMASK			= (BASE_FM_COMMANDCODE | 0x0021),	// mfgFm_CmdFmDataD_t	set/get
    MFG_FM_CMD_RDSMODE					= (BASE_FM_COMMANDCODE | 0x0022),	// mfgFm_CmdFmDataD_t	set/get
    MFG_FM_CMD_IRSMASK					= (BASE_FM_COMMANDCODE | 0x0023),	// mfgFm_CmdFmDataD_t	set/get
    MFG_FM_CMD_RXAUDIOPAUSEMODE			= (BASE_FM_COMMANDCODE | 0x0024),	// mfgFm_CmdFmDataD_t	set only
    MFG_FM_CMD_RXAUDIOSAMPLINGRATE		= (BASE_FM_COMMANDCODE | 0x0025),	// mfgFm_CmdFmDataD2_t	set only

    MFG_FM_CMD_TXOUTPUTPOWER			= (BASE_FM_COMMANDCODE | 0x0026),	// mfgFm_CmdFmDataD_t	set only
    MFG_FM_CMD_TXFREQDEVIATION_EXTENDED	= (BASE_FM_COMMANDCODE | 0x0027),	// TBD_KY reassigned	// mfgFm_CmdFmDataD6_t	set only
    MFG_FM_CMD_TXOUTPUTPOWERMODE		= (BASE_FM_COMMANDCODE | 0x0028),	// mfgFm_CmdFmDataD_t	set only
    MFG_FM_CMD_REFERNCECLKERR			= (BASE_FM_COMMANDCODE | 0x0029),	// mfgFm_CmdFmDataD_t	set only
    MFG_FM_CMD_TXPREEMPHASIS_removed			= (BASE_FM_COMMANDCODE | 0x002A),	// mfgFm_CmdFmDataD_t 
    MFG_FM_CMD_TXFREQDEVIATION			= (BASE_FM_COMMANDCODE | 0x002B),	// mfgFm_CmdFmDataD_t	set only
    MFG_FM_CMD_TXMONOSTEREO				= (BASE_FM_COMMANDCODE | 0x002C),	// mfgFm_CmdFmDataD_t	set only
    MFG_FM_CMD_TXMUTEMODE				= (BASE_FM_COMMANDCODE | 0x002D),	// mfgFm_CmdFmDataD3_t	set only
    MFG_FM_CMD_TXRDSPICODE				= (BASE_FM_COMMANDCODE | 0x002E),	// mfgFm_CmdFmDataD_t	set only
    MFG_FM_CMD_RDSGROUP					= (BASE_FM_COMMANDCODE | 0x002F),	// mfgFm_CmdFmDataD_t	set only
    MFG_FM_CMD_RDSPTY					= (BASE_FM_COMMANDCODE | 0x0030),	// mfgFm_CmdFmDataD_t	set only
    MFG_FM_CMD_RDSAFCODE				= (BASE_FM_COMMANDCODE | 0x0031),	// mfgFm_CmdFmDataD_t	set only
    MFG_FM_CMD_TXRDSMODE				= (BASE_FM_COMMANDCODE | 0x0032),	// mfgFm_CmdFmDataD_t	set only
    MFG_FM_CMD_RDSSCROLLING				= (BASE_FM_COMMANDCODE | 0x0033),	// mfgFm_CmdFmDataD_t	set only
    MFG_FM_CMD_RDSSCROLLRATE			= (BASE_FM_COMMANDCODE | 0x0034),	// mfgFm_CmdFmDataD_t	set only
    MFG_FM_CMD_RDSDISPLAYLEN			= (BASE_FM_COMMANDCODE | 0x0035),	// mfgFm_CmdFmDataD_t	set only
    MFG_FM_CMD_RDSTOGGLEAB				= (BASE_FM_COMMANDCODE | 0x0036),	// mfgFm_CmdFmDataD_t	set only
    MFG_FM_CMD_RDSREPOSITORY			= (BASE_FM_COMMANDCODE | 0x0037),	// mfgFm_CmdFmDataD_t	set only
    MFG_FM_CMD_RDSDATA_not_implemented		= (BASE_FM_COMMANDCODE | 0x0038),	// mfgFm_CmdFmDataD_t  not implemented
    MFG_FM_CMD_SERCHTXFREECHAN_removed			= (BASE_FM_COMMANDCODE | 0x0039),	// mfgFm_CmdFmDataD_t	 
    MFG_FM_CMD_SELECTRXANTTYPE			= (BASE_FM_COMMANDCODE | 0x003A),	// mfgFm_CmdFmDataD_t	set only
    MFG_FM_CMD_CONFIGAUDIOINPUT			= (BASE_FM_COMMANDCODE | 0x003B),	// mfgFm_CmdFmDataD2_t	set only
    MFG_FM_CMD_STOPSEARCH				= (BASE_FM_COMMANDCODE | 0x003C),	// mfgFm_CmdFmDataD_t	set only
    MFG_FM_CMD_SETLOINJECTSIDE_removed			= (BASE_FM_COMMANDCODE | 0x003D),	// mfgFm_CmdFmDataD_t	 
    MFG_FM_CMD_RESETRDSBLER				= (BASE_FM_COMMANDCODE | 0x003E),	// mfgFm_CmdFmDataD_t	get only
    MFG_FM_CMD_GETRDSBLER				= (BASE_FM_COMMANDCODE | 0x003F),	// mfgFm_CmdFmDataD4_t	set only
    MFG_FM_CMD_ENABLETXAUDIOAGC			= (BASE_FM_COMMANDCODE | 0x0040),	// mfgFm_CmdFmDataD_t	set only
    MFG_FM_CMD_FMPOWERDOWN				= (BASE_FM_COMMANDCODE | 0x0041),	// mfgFm_CmdFmDataD_t	set only
	MFG_FM_CMD_SOFTMUTEMODE				= (BASE_FM_COMMANDCODE | 0x0042),	// mfgFm_CmdFmDataD3_t	set/get

    MFG_FM_CMD_CURRENT_CMI				= (BASE_FM_COMMANDCODE | 0x0043),	// NEW mfgFm_CmdFmDataD_t	get only
	MFG_FM_CMD_CMI_THRESHOLD			= (BASE_FM_COMMANDCODE | 0x0044),	// NEW mfgFm_CmdFmDataD_t	set/get
	MFG_FM_CMD_STEREOBLENDINGMODE		= (BASE_FM_COMMANDCODE | 0x0045),	// NEW mfgFm_CmdFmDataD_t	set/get		General FM Channel Ctrl subgroup
 	MFG_FM_CMD_PLLLOCKSTATUS			= (BASE_FM_COMMANDCODE | 0x0046),	// NEW mfgFm_CmdFmDataD_t	get only	General FM Audio Ctrl subgroup
	MFG_FM_CMD_STEREOBLENDINGCONFIG		= (BASE_FM_COMMANDCODE | 0x0047),	// NEW mfgFm_CmdFmDataD_t	set only	General FM Channel Ctrl subgroup
 	MFG_FM_CMD_STEREOSTATUS				= (BASE_FM_COMMANDCODE | 0x0048),	// NEW mfgFm_CmdFmDataD_t	get only	General FM Channel Ctrl subgroup
 	MFG_FM_CMD_I2S_PARAMETER			= (BASE_FM_COMMANDCODE | 0x0049),	// NEW mfgFm_CmdFmDataD3_t	get only	General FM Audio Ctrl subgroup
 	MFG_FM_CMD_FMAUDIOVOLUME			= (BASE_FM_COMMANDCODE | 0x004A),	// NEW mfgFm_CmdFmDataD_t	get only	General FM Audio Ctrl subgroup
   	
} ;
 










// UART PFW codes

#define HOST_ACT_GET		0
#define HOST_ACT_SET		1

#define FmPkHeader BtPkHeader
#define mfgFm_Cmd_t mfgBt_Cmd_t

#define mfgFm_Cmd_Templete_t mfgFm_Cmd_t
 
typedef struct mfgFm_CmdFmDataD_t
{
   	BtPkHeader	header; 
	DWORD		Data;
}	PACK mfgFm_CmdFmDataD_t;



// read or wite FMU registers
typedef struct mfgFm_CmdFmReg_t
{
	BtPkHeader	header; 
	DWORD		Address;
	DWORD		Data;
}	PACK mfgFm_CmdFmReg_t;

 
#define mfgFm_Cmd_Enable_t mfgBt_Enable_t

typedef struct mfgFm_CmdFmDataD2_t
{
   	BtPkHeader	header; 
	DWORD		Data1;
	DWORD		Data2;
} PACK mfgFm_CmdFmDataD2_t;
  

typedef struct mfgFm_CmdFmDataD3_t
{
   	BtPkHeader	header; 
	DWORD		Data1;
	DWORD		Data2;
	DWORD		Data3;
}	PACK mfgFm_CmdFmDataD3_t;

typedef struct mfgFm_CmdFmDataD4_t
{
   	BtPkHeader	header; 
	DWORD		Data1;
	DWORD		Data2;
	DWORD		Data3;
	DWORD		Data4;
}	PACK mfgFm_CmdFmDataD4_t;


typedef struct mfgFm_CmdFmDataD6_t
{
   	BtPkHeader	header; 
	DWORD		Data1;
	DWORD		Data2;
	DWORD		Data3;
	DWORD		Data4;
	DWORD		Data5;
	DWORD		Data6;
}	PACK mfgFm_CmdFmDataD6_t;



 
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
#define HCI_CMD_MRVL_FM_CMD								((uint16)0x0280)	// 

#define HCI_CMD_MRVL_FM_SUBCMD_RXRESET_removed			((uint8)0x00)	// 
#define HCI_CMD_MRVL_FM_SUBCMD_RXINIT					((uint8)0x01)	// 
#define HCI_CMD_MRVL_FM_SUBCMD_POWERUPMODE				((uint8)0x02)	// 
#define HCI_CMD_MRVL_FM_SUBCMD_SETFMCH					((uint8)0x03)	// 
#define HCI_CMD_MRVL_FM_SUBCMD_GETFMCH					((uint8)0x04)	// 
#define HCI_CMD_MRVL_FM_SUBCMD_SETFMAFCH				((uint8)0x05)	// 
#define HCI_CMD_MRVL_FM_SUBCMD_GETFMAFCH_removed		((uint8)0x06)	// 
#define HCI_CMD_MRVL_FM_SUBCMD_SETFMSEARCHDIR			((uint8)0x07)	// 
#define HCI_CMD_MRVL_FM_SUBCMD_GETFMSEARCHDIR			((uint8)0x08)	// 
#define HCI_CMD_MRVL_FM_SUBCMD_SETFMAUTOSEARCHMODE		((uint8)0x09)	// 
#define HCI_CMD_MRVL_FM_SUBCMD_GETFMAUTOSEARCHMODE		((uint8)0x0A)	// 
#define HCI_CMD_MRVL_FM_SUBCMD_SETFMFORCEMONOMODE		((uint8)0x0B)	// changed Bt_FmBCmdB 
#define HCI_CMD_MRVL_FM_SUBCMD_GETFMFORCEMONOMODE		((uint8)0x0C)	// changed Bt_FmBCmdB Bt_HciFmEventB
#define HCI_CMD_MRVL_FM_SUBCMD_SETSTEREOBLENDINGMODE	((uint8)0x0D)	// NEW Bt_FmBCmdB					General FM Channel Ctrl subgroup
#define HCI_CMD_MRVL_FM_SUBCMD_GETSTEREOBLENDINGMODE	((uint8)0x0E)	// NEW Bt_FmBCmd, Bt_HciFmEventB	General FM Channel Ctrl subgroup
#define HCI_CMD_MRVL_FM_SUBCMD_SETAUDIODE_EMPHASIS		((uint8)0x0F)	// Bt_FmBCmdB
#define HCI_CMD_MRVL_FM_SUBCMD_GETAUDIODE_EMPHASIS_removed		((uint8)0x10)	// Bt_FmBCmd, Bt_HciFmEventB

#define HCI_CMD_MRVL_FM_SUBCMD_SETFMCMITHRESHOLD		((uint8)0x11)	// Bt_FmBCmdD
#define HCI_CMD_MRVL_FM_SUBCMD_GETFMCMITHRESHOLD		((uint8)0x12)	// Bt_FmBCmd, Bt_HciFmEventD
#define HCI_CMD_MRVL_FM_SUBCMD_SETFMBAND				((uint8)0x13)	// 
#define HCI_CMD_MRVL_FM_SUBCMD_GETFMBAND_removed		((uint8)0x14)	// 
#define HCI_CMD_MRVL_FM_SUBCMD_SETRXMUTE				((uint8)0x15)	// Bt_FmBCmdB
#define HCI_CMD_MRVL_FM_SUBCMD_GETRXMUTE				((uint8)0x16)	// Bt_FmBCmd, Bt_HciFmEventB
#define HCI_CMD_MRVL_FM_SUBCMD_SETRXAUDIOPAUSELEVEL		((uint8)0x17)	// Bt_FmBCmdW
#define HCI_CMD_MRVL_FM_SUBCMD_GETRXAUDIOPAUSELEVEL_removed		((uint8)0x18)	// Bt_FmBCmd, Bt_HciFmEventW
#define HCI_CMD_MRVL_FM_SUBCMD_GETRXAUDIOPAUSEDURATION_removed	((uint8)0x19)	// Bt_FmBCmd, Bt_HciFmEventW
#define HCI_CMD_MRVL_FM_SUBCMD_SETRXAUDIOPATH			((uint8)0x1A)	// Bt_FmBCmdB3
#define HCI_CMD_MRVL_FM_SUBCMD_GETRXAUDIOPATH			((uint8)0x1B)	// Bt_FmBCmd, Bt_HciFmEventB
#define HCI_CMD_MRVL_FM_SUBCMD_SETRXRDSDATAPATH_removed			((uint8)0x1C)	// Bt_FmBCmdB
#define HCI_CMD_MRVL_FM_SUBCMD_GETRXRDSDATAPATH_removed			((uint8)0x1D)	// Bt_FmBCmd, Bt_HciFmEventB
#define HCI_CMD_MRVL_FM_SUBCMD_GETRXRDSSYNCSTATUS		((uint8)0x1E)	// Bt_FmBCmd, Bt_HciFmEventB
#define HCI_CMD_MRVL_FM_SUBCMD_GETCURRENTFLAGS			((uint8)0x1F)	// Bt_FmBCmd, Bt_HciFmEventD
#define HCI_CMD_MRVL_FM_SUBCMD_GETRDSDATA_notimplemented		((uint8)0x20)	// Bt_FmBCmd, Bt_HciFmEventB
#define HCI_CMD_MRVL_FM_SUBCMD_FLUSHRDS					((uint8)0x21)	// Bt_FmBCmd


#define HCI_CMD_MRVL_FM_SUBCMD_SETRDSMEMDEPTH			((uint8)0x22)	// Bt_FmBCmdB
#define HCI_CMD_MRVL_FM_SUBCMD_GETRDSMEMDEPTH			((uint8)0x23)	// Bt_FmBCmd, Bt_HciFmEventB
#define HCI_CMD_MRVL_FM_SUBCMD_SETRDSBLOCKB				((uint8)0x24)	// Bt_FmBCmdW
#define HCI_CMD_MRVL_FM_SUBCMD_GETRDSBLOCKB				((uint8)0x25)	// Bt_FmBCmd, Bt_HciFmEventW
#define HCI_CMD_MRVL_FM_SUBCMD_SETRDSBLOCKBMASK			((uint8)0x26)	// Bt_FmBCmdW
#define HCI_CMD_MRVL_FM_SUBCMD_GETRDSBLOCKBMASK			((uint8)0x27)	// Bt_FmBCmd, Bt_HciFmEventW
#define HCI_CMD_MRVL_FM_SUBCMD_SETRDSPICODE				((uint8)0x28)	// Bt_FmBCmdW
#define HCI_CMD_MRVL_FM_SUBCMD_GETRDSPICODE				((uint8)0x29)	// Bt_FmBCmd, Bt_HciFmEventW
#define HCI_CMD_MRVL_FM_SUBCMD_SETRDSPICODEMASK			((uint8)0x2A)	// Bt_FmBCmdW
#define HCI_CMD_MRVL_FM_SUBCMD_GETRDSPICODEMASK			((uint8)0x2B)	// Bt_FmBCmd, Bt_HciFmEventW
#define HCI_CMD_MRVL_FM_SUBCMD_SETRDSMODE				((uint8)0x2C)	// changed Bt_FmBCmdB
#define HCI_CMD_MRVL_FM_SUBCMD_GETRDSMODE				((uint8)0x2D)	// changed Bt_FmBCmd, Bt_HciFmEventB
#define HCI_CMD_MRVL_FM_SUBCMD_SETIRSMASK				((uint8)0x2E)	// Bt_FmBCmdD
#define HCI_CMD_MRVL_FM_SUBCMD_GETIRSMASK				((uint8)0x2F)	// Bt_FmBCmd, Bt_HciFmEventD


#define HCI_CMD_MRVL_FM_SUBCMD_GETFWVERSION				((uint8)0x30)	// 
#define HCI_CMD_MRVL_FM_SUBCMD_GETHWVERSION				((uint8)0x31)	// 
#define HCI_CMD_MRVL_FM_SUBCMD_GETHWID					((uint8)0x32)	// 
#define HCI_CMD_MRVL_FM_SUBCMD_GETMANUFACTUREID			((uint8)0x33)	// 
#define HCI_CMD_MRVL_FM_SUBCMD_GETCURRENTRSSI			((uint8)0x34)	// 
#define HCI_CMD_MRVL_FM_SUBCMD_SETSTEPSIZE				((uint8)0x35)	// 
#define HCI_CMD_MRVL_FM_SUBCMD_GETSTEPSIZE_removed				((uint8)0x36)	// 
#define HCI_CMD_MRVL_FM_SUBCMD_CHUP						((uint8)0x37)	// 
#define HCI_CMD_MRVL_FM_SUBCMD_CHDOWN					((uint8)0x38)	// 
#define HCI_CMD_MRVL_FM_SUBCMD_SETRXAUDIOPAUSEDURATION	((uint8)0x3C)	// Bt_FmBCmdW
#define HCI_CMD_MRVL_FM_SUBCMD_SETRXAUDIOPAUSEMODE		((uint8)0x3D)	// Bt_FmBCmdB
#define HCI_CMD_MRVL_FM_SUBCMD_GETPLLLOCLSTATUS			((uint8)0x3E)	// NEW Bt_FmBCmdB		General FM audio ctrl
#define HCI_CMD_MRVL_FM_SUBCMD_SETRXAUDIOSAMPLINGRATE	((uint8)0x3F)	// Bt_FmBCmdB2


#define HCI_CMD_MRVL_FM_SUBCMD_SETTXOUTPUTPOWER			((uint8)0x40)	// Bt_FmBCmdB
#define HCI_CMD_MRVL_FM_SUBCMD_SETTXFREQDEVIATION_EXTENDED			((uint8)0x41)	// Bt_FmBCmdD4
//#define HCI_CMD_MRVL_FM_SUBCMD_FORCETXAUDIOGAIN			((uint8)0x41)	// Bt_FmBCmdBW
#define HCI_CMD_MRVL_FM_SUBCMD_FORCETXPILOTGAIN_removed			((uint8)0x42)	// Bt_FmBCmdB2
#define HCI_CMD_MRVL_FM_SUBCMD_FORCETXRDSGAIN_removed			((uint8)0x43)	// Bt_FmBCmdB2
#define HCI_CMD_MRVL_FM_SUBCMD_SETTXPREEMPHASIS_removed			((uint8)0x44)	// Bt_FmBCmdB
#define HCI_CMD_MRVL_FM_SUBCMD_SETTXFREQDEVIATION		((uint8)0x45)	// Bt_FmBCmdD
#define HCI_CMD_MRVL_FM_SUBCMD_SETTXMONOSTEREO			((uint8)0x46)	// Bt_FmBCmdB
#define HCI_CMD_MRVL_FM_SUBCMD_SETTXMUTE				((uint8)0x47)	// Bt_FmBCmdB3
#define HCI_CMD_MRVL_FM_SUBCMD_SETTXRDSPICODE			((uint8)0x48)	// Bt_FmBCmdW
#define HCI_CMD_MRVL_FM_SUBCMD_SETRDSGROUP				((uint8)0x49)	// Bt_FmBCmdB
#define HCI_CMD_MRVL_FM_SUBCMD_SETRDSPTY				((uint8)0x4A)	// Bt_FmBCmdB
#define HCI_CMD_MRVL_FM_SUBCMD_SETRDSAFCODE				((uint8)0x4B)	// Bt_FmBCmdB
#define HCI_CMD_MRVL_FM_SUBCMD_SETTXRDSMODE				((uint8)0x4C)	// Bt_FmBCmdB
#define HCI_CMD_MRVL_FM_SUBCMD_SETRDSSCROLLING			((uint8)0x4D)	// Bt_FmBCmdB
#define HCI_CMD_MRVL_FM_SUBCMD_SETRDSSCROLLRATE			((uint8)0x4E)	// Bt_FmBCmdB
#define HCI_CMD_MRVL_FM_SUBCMD_SETRDSDISPLAYLEN			((uint8)0x4F)	// Bt_FmBCmdB
#define HCI_CMD_MRVL_FM_SUBCMD_SETRDSTOGGLEAB			((uint8)0x50)	// Bt_FmBCmd
#define HCI_CMD_MRVL_FM_SUBCMD_SETRDSREPOSITORY			((uint8)0x51)	// Bt_FmBCmdB
#define HCI_CMD_MRVL_FM_SUBCMD_SETRDSDATA_notimplemented				((uint8)0x52)	// Bt_FmBCmdBn

#define HCI_CMD_MRVL_FM_SUBCMD_SERCHTXFREECHAN_removed			((uint8)0x54)	// Bt_FmBCmdD
#define HCI_CMD_MRVL_FM_SUBCMD_SELECTRXANTTYPE			((uint8)0x55)	// Bt_FmBCmdB
#define HCI_CMD_MRVL_FM_SUBCMD_CONFIGAUDIOINPUT			((uint8)0x56)	// Bt_FmBCmdB2
#define HCI_CMD_MRVL_FM_SUBCMD_STOPSEARCH				((uint8)0x57)	// Bt_FmBCmd
#define HCI_CMD_MRVL_FM_SUBCMD_SETLOINJECTSIDE_removed			((uint8)0x58)	// Bt_FmBCmdB

#define HCI_CMD_MRVL_FM_SUBCMD_REFCLK_removed			((uint8)0x59)	// Bt_FmBCmdD
#define HCI_CMD_MRVL_FM_SUBCMD_REFCLKERR				((uint8)0x5A)	// Bt_FmBCmdD
#define HCI_CMD_MRVL_FM_SUBCMD_ENABLETXAUDIOAGC			((uint8)0x5B)	// Bt_FmBCmdB
#define HCI_CMD_MRVL_FM_SUBCMD_STEREOBLENDINGCONFIG		((uint8)0x5C)	// NEW Bt_FmBCmdB					General FM Channel Ctrl subgroup 
#define HCI_CMD_MRVL_FM_SUBCMD_SET_TX_OUT_POWER_MODE    ((uint8)0x5D)
#define HCI_CMD_MRVL_FM_SUBCMD_GET_TX_OUT_POWER_MODE_removed    ((uint8)0x5E)
#define HCI_CMD_MRVL_FM_SUBCMD_GETCURRENTCMI			((uint8)0x5F)	// 

#define HCI_CMD_MRVL_FM_SUBCMD_GET_STEREOSTATUS			((uint8)0x60)	// NEW Bt_FmBCmd Bt_HciFmEventB		General FM Channel Ctrl subgroup
#define HCI_CMD_MRVL_FM_SUBCMD_SET_TX_SOFTMUTE_MODE		((uint8)0x61)	// Bt_FmBCmdBWB
#define HCI_CMD_MRVL_FM_SUBCMD_GET_TX_SOFTMUTE_MODE		((uint8)0x62)	// Bt_FmBCmd Bt_HciFmEventB 

#define HCI_CMD_MRVL_FM_SUBCMD_SETFMRSSITHRESHOLD		((uint8)0x63)	// NEW Bt_FmBCmdD
#define HCI_CMD_MRVL_FM_SUBCMD_GETFMRSSITHRESHOLD		((uint8)0x64)	// NEW Bt_FmBCmd, Bt_HciFmEventD

#define HCI_CMD_MRVL_FM_SUBCMD_SETFMAUDIOVOLUME			((uint8)0x65)	// NEW Bt_FmBCmdW					General FM Audio Ctrl subgroup
#define HCI_CMD_MRVL_FM_SUBCMD_GETFMAUDIOVOLUME			((uint8)0x66)	// NEW Bt_FmBCmd, Bt_HciFmEventW	General FM Audio Ctrl subgroup

#define HCI_CMD_MRVL_FM_SUBCMD_GET_I2S_PARAMETER		((uint8)0x68)	// NEW Bt_FmBCmdD Bt_HciFmEventB2	General FM Audio Ctrl subgroup
#define HCI_CMD_MRVL_FM_SUBCMD_GETTXOUTPUTPOWER			((uint8)0x69)	// NEW Bt_FmBCmd Bt_HciFmEventB		General FM Txer Ctrl subgroup


#define HCI_CMD_MRVL_FM_SUBCMD_RDSBLER_RESET			((uint8)0x9D)	// Bt_FmBCmdD
#define HCI_CMD_MRVL_FM_SUBCMD_RDSBLER_STAT				((uint8)0x9E)	// Bt_FmBCmd, Bt_HciFmEventD4

#define HCI_CMD_MRVL_FM_SUBCMD_FMPOWERDOWN				((uint8)0x9F)	// Bt_FmBCmd
#define HCI_CMD_MRVL_FM_SUBCMD_FMCMDMAX					((uint8)0xFF)	// memo: 1-byte HCI opcodes only




#define HCI_EVENT_MRVL_FM_CURRENTFLAGS					((uint16)0x080)	// 4 byte data
#define HCI_EVENT_MRVL_FM_RDSDATA						((uint16)0x081)	// no data? 
#define HCI_EVENT_MRVL_FM_CURRENTRSSI					((uint16)0x082)	// 4 byte data
#define HCI_EVENT_MRVL_FM_MONOSTEROSTATUS				((uint16)0x083)	// 1 byte data
#define HCI_EVENT_MRVL_FM_AUDIOPAUSE					((uint16)0x084)
#define HCI_EVENT_MRVL_FM_RDSSYNCLOST					((uint16)0x085)
#define HCI_EVENT_MRVL_FM_RSSILOW						((uint16)0x086)
#define HCI_EVENT_MRVL_FM_CMILOW						((uint16)0x087) 


typedef struct  Bt_FmBCmd
{
 	BYTE	Cmd;
} PACK Bt_FmBCmd;

typedef struct  Bt_FmBCmdB
{
 	BYTE	Cmd;
	BYTE	Data;
} PACK Bt_FmBCmdB;

typedef struct  Bt_FmBCmdW
{
 	BYTE	Cmd;
	WORD	Data;
} PACK Bt_FmBCmdW;

typedef struct  Bt_FmBCmdBWB
{
 	BYTE	Cmd;
	BYTE	DataB1;
	WORD	DataW;
	BYTE	DataB2;
} PACK Bt_FmBCmdBWB;

typedef struct  Bt_FmBCmdD
{
 	BYTE	Cmd;
	DWORD	Data;
} PACK Bt_FmBCmdD;

typedef struct  Bt_FmBCmdD2
{
 	BYTE	Cmd;
	DWORD	Data1;
	DWORD	Data2;
} PACK Bt_FmBCmdD2;

typedef struct  Bt_FmBCmdD4
{
 	BYTE	Cmd;
	DWORD	Data1;
	DWORD	Data2;
	DWORD	Data3;
	DWORD	Data4;
} PACK Bt_FmBCmdD4;


typedef struct  Bt_FmBCmdDB
{
 	BYTE	Cmd;
	DWORD	DataD;
	BYTE	DataB;
} PACK Bt_FmBCmdDB;


typedef struct  Bt_FmBCmdB2
{
 	BYTE	Cmd;
	BYTE	Data1;
	BYTE	Data2;
} PACK Bt_FmBCmdB2;

typedef struct  Bt_FmBCmdB3
{
 	BYTE	Cmd;
	BYTE	Data1;
	BYTE	Data2;
	BYTE	Data3;
} PACK Bt_FmBCmdB3;

typedef struct  Bt_FmBCmdB6
{
 	BYTE	Cmd;
	BYTE	Data1;
	BYTE	Data2;
	BYTE	Data3;
	BYTE	Data4;
	BYTE	Data5;
	BYTE	Data6;
} PACK Bt_FmBCmdB6;

typedef struct  Bt_FmBCmdBW
{
 	BYTE	Cmd;
	BYTE	DataB;
	WORD	DataW;
} PACK Bt_FmBCmdBW;



typedef struct  Bt_HciCmdCpltEventHdr
{
 	BYTE	NumHciCmd;
 	BYTE	Cmd; 
	BYTE	Error;
} PACK Bt_HciCmdCpltEventHdr;

/*
typedef struct  Bt_VendorCmdCpltEventHdr
{
// 	BYTE	NumHciCmd;
 	BYTE	Cmd; 
} PACK Bt_VendorCmdCpltEventHdr;

typedef struct  Bt_VendorFmEventD
{
	Bt_VendorCmdCpltEventHdr	EventHeader;
	DWORD	Data;
} PACK Bt_VendorFmEventD;
*/

typedef struct  Bt_HciFmEventD
{
	Bt_HciCmdCpltEventHdr	EventHeader;
	DWORD	Data;
} PACK Bt_HciFmEventD;

typedef struct  Bt_HciFmEventD4
{
	Bt_HciCmdCpltEventHdr	EventHeader;
	DWORD	Data1;
	DWORD	Data2;
	DWORD	Data3;
	DWORD	Data4;
} PACK Bt_HciFmEventD4;


typedef struct  Bt_HciFmEventW
{
	Bt_HciCmdCpltEventHdr	EventHeader;
	WORD	Data;
} PACK Bt_HciFmEventW;

typedef struct  Bt_HciFmEventB
{
	Bt_HciCmdCpltEventHdr	EventHeader;
	BYTE	Data;
} PACK Bt_HciFmEventB;

typedef struct  Bt_HciFmEventB2
{
	Bt_HciCmdCpltEventHdr	EventHeader;
	BYTE	Data1;
	BYTE	Data2;
} PACK Bt_HciFmEventB2;



//////////////////////////////////////



#if defined(WIN32) || defined(NDIS51_MINIPORT) || defined(NDIS50_MINIPORT) || defined(_LINUX_)
#pragma pack() 
#endif

#endif
