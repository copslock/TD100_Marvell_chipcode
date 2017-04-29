/** @file BtMfgCmd.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

// Command Code

#ifndef _BT_MFG_BT_CMD_
#define _BT_MFG_BT_CMD_
 
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
 
#include "./hci_opcodes.h"

#ifdef _BT_USB_ 
#define MRVL_HCI_OID_MFG_BT	  0xfe000001 //to XP USB driver 
#elif defined (_BT_UART_)
#define MRVL_HCI_OID_MFG_BT	   ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|(HCI_CMD_MRVL_MFG_BT))
#else
#define MRVL_HCI_OID_MFG_BT	   ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|(HCI_CMD_MRVL_MFG_BT))
#endif
 

#define BASE_BT_COMMANDCODE		0x2000
#define BASE_BT_RESPONSEMASK	(0x8000)
#define BASE_BT_RESPONSECODE	(BASE_BT_COMMANDCODE | BASE_BT_RESPONSEMASK)
#define BTCMD_RSP(x)			(BASE_BT_RESPONSEMASK|(0xFFFF&x))

typedef enum
{
    MFG_BT_CMD_NONE						= (BASE_BT_COMMANDCODE | 0x0000),
    MFG_BT_CMD_MEM_REG					= (BASE_BT_COMMANDCODE | 0x0001),
    MFG_BT_CMD_RF_REG					= (BASE_BT_COMMANDCODE | 0x0002),
    MFG_BT_CMD_BTU_REG					= (BASE_BT_COMMANDCODE | 0x0003),
    MFG_BT_CMD_SPI_EEPROM				= (BASE_BT_COMMANDCODE | 0x0004),
	MFG_BT_CMD_SPI_MEM					= (BASE_BT_COMMANDCODE | 0x0005),
	MFG_BT_CMD_SPI_DL					= (BASE_BT_COMMANDCODE | 0x0006),
    MFG_BT_CMD_RF_CHAN					= (BASE_BT_COMMANDCODE | 0x0007),
    MFG_BT_CMD_FW_VERS					= (BASE_BT_COMMANDCODE | 0x0008),
    MFG_BT_CMD_GET_HW_VERS				= (BASE_BT_COMMANDCODE | 0x0009),
    MFG_BT_CMD_TX_CONT_obs					= (BASE_BT_COMMANDCODE | 0x000A),
    MFG_BT_CMD_EN_CST_obs					= (BASE_BT_COMMANDCODE | 0x000B),
    MFG_BT_CMD_DUTY_CYCLE				= (BASE_BT_COMMANDCODE | 0x000C),
    MFG_BT_CMD_TX_FRAME					= (BASE_BT_COMMANDCODE | 0x000D), 
    MFG_BT_CMD_RFXTAL_CTRL				= (BASE_BT_COMMANDCODE | 0x000E), //   
    MFG_BT_CMD_RF_PA_THERMAL_SCALER_obs		= (BASE_BT_COMMANDCODE | 0x000F), //  
    MFG_BT_CMD_PW_CLASS_MODE			= (BASE_BT_COMMANDCODE | 0x0010),
    MFG_BT_CMD_RF_BRFRESET				= (BASE_BT_COMMANDCODE | 0x0011), //  
    MFG_BT_CMD_RF_PABIAS_obs				= (BASE_BT_COMMANDCODE | 0x0012),
    MFG_BT_CMD_RF_PPAPGA_PRIORITY_obs		= (BASE_BT_COMMANDCODE | 0x0013),
    MFG_BT_CMD_RF_XPA_POLORITY_obs			= (BASE_BT_COMMANDCODE | 0x0014),
    MFG_BT_CMD_FIXVGACTRL					= (BASE_BT_COMMANDCODE | 0x0015),
    MFG_BT_CMD_PWRLVL_MAXMIN			= (BASE_BT_COMMANDCODE | 0x0016),
    MFG_BT_CMD_PWRLVL					= (BASE_BT_COMMANDCODE | 0x0017), //mfgBt_BtTxPower_t
    MFG_BT_CMD_RF_POW_TH_obs				= (BASE_BT_COMMANDCODE | 0x0018),
    MFG_BT_CMD_STEP_RF_PWR				= (BASE_BT_COMMANDCODE | 0x0019),
    MFG_BT_CMD_RESETACCUM_obs				= (BASE_BT_COMMANDCODE | 0x001A), //    
    MFG_BT_CMD_BTU_PWRCTL_BPS			= (BASE_BT_COMMANDCODE | 0x001B), //    
    MFG_BT_CMD_HOP2CH					= (BASE_BT_COMMANDCODE | 0x001C),

    MFG_BT_CMD_XOSCCAL_obs					= (BASE_BT_COMMANDCODE | 0x001D),
 	MFG_BT_CMD_RF_CALCLOSELOOP_obs			= (BASE_BT_COMMANDCODE | 0x001E), // 
 	MFG_BT_CMD_RX_INFO_obs					= (BASE_BT_COMMANDCODE | 0x001F), // 
 	MFG_BT_CMD_BCA_obs						= (BASE_BT_COMMANDCODE | 0x0020), // 
 
	MFG_BT_CMD_EXTENDEDPWRCTRLRANGEMODE_obs	= (BASE_BT_COMMANDCODE | 0x0021), // mfgBt_Enable_t
 	MFG_BT_CMD_BBGAININITVALUE_obs			= (BASE_BT_COMMANDCODE | 0x0022), // mfgBt_Enable_t
 	MFG_BT_CMD_BBGAINMAXMIN_obs				= (BASE_BT_COMMANDCODE | 0x0023), // mfgBt_CmdPpaPgaMaxMinV_t
  	MFG_BT_CMD_FIXEDBBGAIN_obs 				= (BASE_BT_COMMANDCODE | 0x0024), // mfgBt_Enable_t
	MFG_BT_CMD_PPAGAINSTEPMODE_obs			= (BASE_BT_COMMANDCODE | 0x0025), // mfgBt_Enable_t
 	MFG_BT_CMD_AUTOZEROMODE_obs				= (BASE_BT_COMMANDCODE | 0x0026), // mfgBt_CmdCst_t
 	MFG_BT_CMD_TXHILOWPWRMODE_obs			= (BASE_BT_COMMANDCODE | 0x0027), // mfgBt_Enable_t
 
	MFG_BT_CMD_EFUSE					= (BASE_BT_COMMANDCODE | 0x0028), // mfgBt_CmdEfuse_t
 	MFG_BT_CMD_RELEASENOTE				= (BASE_BT_COMMANDCODE | 0x0029), // mfgBt_CmdReleaseNote_t
    MFG_BT_CMD_FMU_REG					= (BASE_BT_COMMANDCODE | 0x002A), // mfgBt_Enable_t

	MFG_BT_CMD_THERMREADING_obs				= (BASE_BT_COMMANDCODE | 0x002B), // mfgBt_Enable_t
	MFG_BT_CMD_CHIPCLASSCAPACITY		= (BASE_BT_COMMANDCODE | 0x002C), // mfgBt_Enable_t
	MFG_BT_CMD_CURRENT_OR_REV			= (BASE_BT_COMMANDCODE | 0x002D), // use mfgBt_CmdFwVers_t
	MFG_BT_CMD_CURRENT_APPMODE			= (BASE_BT_COMMANDCODE | 0x002E), // use mfgBt_CmdFwVers_t
#if !defined (_W8780_) 
	MFG_BT_CMD_FE_BTONLY				= (BASE_BT_COMMANDCODE | 0x002F), // use mfgBt_Enable_t  0=main, 1=aux 
#else //#if !defined (_W8780_) 
	MFG_BT_CMD_ANT_TRSW					= (BASE_BT_COMMANDCODE | 0x002F), // use mfgBt_Enable_t  0=main, 1=aux 
#endif // #if !defined (_W8780_) 

	MFG_BT_CMD_MRVLTXTEST				= (BASE_BT_COMMANDCODE | 0x0030), // use Bt_Mfg_TxTest_t  set only
	MFG_BT_CMD_MRVLRXTEST				= (BASE_BT_COMMANDCODE | 0x0031), // use Bt_MfgCmd_RxTest_t / Bt_MfgRsp_RxTest   get=abort+result/set=start
	MFG_BT_CMD_MRVLRSSI					= (BASE_BT_COMMANDCODE | 0x0032), // use Bt_Mfg_RssiRd_t  get=result/set=clear
    MFG_BT_CMD_TX_CW					= (BASE_BT_COMMANDCODE | 0x0033),

	MFG_BT_CMD_HCI_RESETBT				= (BASE_BT_COMMANDCODE | 0x0040), // use mfgBt_Cmd_t   set only
	MFG_BT_CMD_HCI_TESTMODE				= (BASE_BT_COMMANDCODE | 0x0041), // use mfgBt_Cmd_t   set only
 	MFG_BT_CMD_HCI_INQUIRY				= (BASE_BT_COMMANDCODE | 0x0042), // use mfgBt_Cmd_t   set only
	MFG_BT_CMD_PFW_RELOADCAL			= (BASE_BT_COMMANDCODE | 0x0043), // use mfgBt_Cmd_t 
	MFG_BT_CMD_BTU_TEMPCTRL				= (BASE_BT_COMMANDCODE | 0x0044), // use mfgBt_Cmd_t 
	MFG_BT_CMD_PCMLOOPBACK				= (BASE_BT_COMMANDCODE | 0x0045), // use mfgBt_PcmLoopBack_t 
    MFG_BT_CMD_E2PROM_TYPE				= (BASE_BT_COMMANDCODE | 0x0046), // mfgBt_CmdE2PType_t
	MFG_BT_CMD_HCI_FWBDADDRESS			= (BASE_BT_COMMANDCODE | 0x0047), // use mfgBt_CmdBdAddress_t   get only


#ifdef _ENGINEERING_
 	MFG_BT_CMD_DBG_TEMPREADING			= (BASE_BT_COMMANDCODE | 0x0300), // use mfgBt_Cmd_t   set only
#endif // #ifdef _ENGINEERING_
 
  
} ;
 

#define HOST_ACT_GET		0
#define HOST_ACT_SET		1

   //
   // Type of manufacturing actions send by the host
   //
 
typedef struct  Bt_BdAddress
{
	BYTE       BdAddress[SIZE_BDADDRESS_INBYTE];
} PACK Bt_BdAddress;
 
typedef struct  Bt_BufferSizeInfo
{
	WORD	MaxAclPktLength;
 	BYTE	MaxScoPktLength;
	WORD	TotalNumAclPkt;
	WORD	TotalNumScoPkt;
} PACK Bt_BufferSizeInfo;


typedef struct  Bt_Pfw_TxTest
{
	BYTE		RxOnStart;
	BYTE		SyntOnStart;
	BYTE		TxOnStart;
	BYTE		PhdOffStart;
	BYTE		PayloadPattern; 
	BYTE		RandomHopMode;
	BYTE		BtChannel;
	BYTE		RxChannel;
	BYTE		TestInterval;
	BYTE		PacketType;
	WORD		payloadLength;
	BYTE		Whitening;
	DWORD		TxCnt;
	BYTE		TxPwrLvl;
} PACK Bt_Pfw_TxTest;

 

typedef struct  Bt_PfwCmd_RxTest
{ 
	BYTE		PayloadPattern; // test scenario
	BYTE		BtChannel;
	BYTE		RxChannel;
	BYTE		PacketType;
	DWORD		PacketNumber;
	WORD		payloadLength;
//	BYTE		Whitening;		// shall be 1
//	BYTE		RxDelayPacket;	// shall be 0

//	WORD		CorrelationWindow; // shall be same as packetNumber
//	BYTE		PRBS[LEN_PRBS_INBYTE];
	BYTE		TxAmAddress;
	BYTE		TxBdAddress[SIZE_BDADDRESS_INBYTE];
	BYTE		ReportErrors;
//	BYTE		CorrelationThreshold;
	BYTE		reserved[2];
} PACK Bt_PfwCmd_RxTest;

typedef struct  Bt_PfwRsp_RxTest
{
//	BYTE		Reserved[3]; // test scenario
	BYTE		Status_Complete0_Abort1; // test scenario
	DWORD		TotalPcktCountExpected;
	DWORD		NoRxCount;
	DWORD		SuccessfulCorrelation;
	DWORD		HecMatchCount;
	DWORD		HecMatchCrcPckts;
	DWORD		PayLoadHdrErrorCount;
	DWORD		CrcErrCount;
	DWORD		TotalPacketRxedCount;		// shall be 1
	DWORD		PacketOkCount;		// shall be 1
	DWORD		DroppedPacketCount;		// shall be 1
	DWORD		PER; // shall be same as packetNumber
	DWORD		TotalBitsCountExpected;
	DWORD		TotalBitsCountError_LostDrop;
	DWORD		BER;

	DWORD		TotalByteCountRxed;		// shall be 1
	DWORD		TotalBitErrorRxed;		// shall be 1
	DWORD		RSSI;	// shall be 0

} PACK Bt_PfwRsp_RxTest;


 
typedef struct  BtPkHeader
{
#if defined(_BT_USB_) || defined(_BT_SDIO_) 
 	WORD		cmd;		// message type
	WORD		len;
	WORD		seq;
	WORD		result;
#endif
	DWORD       MfgCmd;
	DWORD		Action;
	DWORD		Error; 
} PACK BtPkHeader;

typedef struct mfgBt_Cmd_t
{
	BtPkHeader	header;
}	PACK mfgBt_Cmd_t;

#define mfgBt_Cmd_Templete_t mfgBt_Cmd_t
 

typedef struct {
	BtPkHeader	header; 
	DWORD signature;
	DWORD sector_erase;
    DWORD address;
	DWORD lenInByte;
	BYTE byte[0];
} PACK spi_CmdEeprom_t;

typedef struct mfgBt_CmdE2PType_t
{
	BtPkHeader	header; 
	DWORD		IfType;
	DWORD		AddrWidth;
	DWORD		DeviceType;
	DWORD		Reserved;
}	PACK mfgBt_CmdE2PType_t;

/*
typedef struct 
{
	BtPkHeader	header; 
	DWORD signature;
	DWORD sector_erase;
	DWORD len;
	DWORD src;          // source addr
	DWORD dest;         // destination addr
} PACK spi_CmdEepromPgm_t;

 
typedef struct {
	BtPkHeader	header; 
} PACK sflash_pgm_resp_t;
 
typedef struct {
	BtPkHeader	header; 
   DWORD dest;         // destination addr
   DWORD seglen;       // segment length of firmware in units of byte
   BYTE seg[0];
} PACK mem_t;
*/
#define SIGNATURE_SPIEEPROM    0xc5c01688


// read or wite MAC registers
typedef struct mfgBt_CmdMemReg_t
{
	BtPkHeader	header; 
	DWORD		address;
	DWORD		data;
}	PACK mfgBt_CmdMemReg_t;
   //
   // MAC register structure
   //

typedef struct mfgBt_CmdHwVers_t
{
   	BtPkHeader	header; 
	WORD		socId;
	WORD		socVersion;
	WORD		bbId;
	WORD		bbVersion;
	WORD		rfId;
	WORD		rfVersion;
}	PACK mfgBt_CmdHwVers_t;
   //
   // BB and RF hardware version structure
   //

typedef struct mfgBt_CmdFwVers_t
{
   	BtPkHeader	header; 
	DWORD		FwVersion;
	DWORD		MfgVersion; 
}	PACK mfgBt_CmdFwVers_t;
   //
   // BB and RF hardware version structure
   //

//MFG_BT_CMD_RF_CHAN
typedef struct mfgBt_CmdRfChan_t
{
   	BtPkHeader	header; 
	DWORD		channel;
	DWORD		ChanFreqInMHz;
	DWORD		Bt2;
} PACK mfgBt_CmdRfChan_t;
   //
   // RF channel structure
   //

 
//MFG_BT_CMD_RF_POW
typedef struct host_CmdRfPaBias_t
{   
   	BtPkHeader	header; 
	DWORD		PerformanceMode;
	WORD		PaBias1; 
	WORD		PaBias2; 
} PACK host_CmdRfPaBias_t;
   //
   // RF power structure
   // 
 
 

  
typedef struct mfgBt_Enable_t
{
   	BtPkHeader	header; 
	DWORD		Enable;
} PACK mfgBt_Enable_t;
  

typedef struct mfgBt_CmdPwrMaxMinV_t
{
   	BtPkHeader	header; 
	DWORD		MaxValue; 
	DWORD		MinValue; 
}	PACK mfgBt_CmdPwrMaxMinV_t;



typedef struct mfgBt_PpaPgaInitV_t
{
   	BtPkHeader	header; 
	DWORD		ext_enable;
	DWORD		PpaValue;
	DWORD		PgaValue;
} PACK mfgBt_PpaPgaInitV_t;
   //
typedef struct mfgBt_PwrTh_t
{
   	BtPkHeader	header; 
	WORD		ClassMode;
	WORD		LoopMode;
	DWORD		ThHi;
	DWORD		ThLo;
} PACK mfgBt_PwrTh_t;

/*
// MFG_BT_CMD_TX_CONT
typedef struct mfgBt_CmdTxCont_t
{
   	BtPkHeader	header; 
	DWORD		enableTx; 
    DWORD		PacketType;
    DWORD		framePattern;
    DWORD		PayloadOnly;
} PACK mfgBt_CmdTxCont_t;
   //
   // Tx continuous mode structure
   //
*/


typedef struct mfgBt_CmdBrfDutyCycle_t
{
	BtPkHeader	header; 
    DWORD		Enable;
    DWORD		DataRate;
    DWORD		Percent;
    DWORD		PayloadPattern;
    DWORD		payloadOnly;
} PACK mfgBt_CmdBrfDutyCycle_t;
// structure used for the duty cycle
//    2) field2, enable (0 or 1)
//    3) field3, data rate 0 -> 1Mbps, 1-> 2Mbps...
//    4) field4, percentage


// MFG_BT_CMD_EN_CST
typedef struct mfgBt_CmdCst_t
{
   	BtPkHeader	header; 
	DWORD		Enable;
	DWORD		FreqOffsetInKHz;
} PACK mfgBt_CmdCst_t;
   //
   // Carrier Suppression Test structure
   //


 

typedef struct mfgBt_CmdXoscCal_t
{
   	BtPkHeader	header; 
	DWORD		TU;
	DWORD		xoscCal; 
}	PACK mfgBt_CmdXoscCal_t;
// Calibration of internal Xosc

/*
typedef struct mfgBt_CmdRfPwrCloseLoopMode_t
{
	BtPkHeader	header; 
	DWORD CloseLoopDisable;
} PACK mfgBt_CmdRfPwrCloseLoopMode_t;
   //
   // mfgBt_CmdRfPwrCloseLoopMode_t structure
   //
 */  
   
typedef struct mfgBt_CmdRfPaThermalScaler_t
{
	BtPkHeader	header; 
	DWORD		ThermalScaler;
} PACK mfgBt_CmdRfPaThermalScaler_t;
   //
   // mfgBt_CmdRfPaThermalScaler_t structure
   //
typedef struct mfgBt_CmdStepRfPwr_t
{
   	BtPkHeader	header; 
	int  StepRfPwr;
} PACK mfgBt_CmdStepRfPwr_t;
   //
   // stepRfPower structure
   //


typedef struct mfgBt_CmdPpaPgaMaxMinV_t
{
   	BtPkHeader	header;
	WORD		Ppa1Pga0; 
	WORD		Enable; 
	WORD		MaxValue; 
	WORD		MinValue; 
}	PACK mfgBt_CmdPpaPgaMaxMinV_t;


 

typedef struct mfgBt_CmdRfXTalCtrl_t
{
   	BtPkHeader	header;  
	DWORD		extMode; 
	DWORD		value; 
}	PACK mfgBt_CmdRfXTalCtrl_t;

 /*
 typedef struct mfgBt_CmdRfCloseLoopAdj_t
{
  	BtPkHeader	header; 
	DWORD		CloseLoopOption;
	DWORD		PacketType; 
	DWORD		TargetInitPpa;  
	DWORD		TargetInitPga;  
	DWORD		TargetValueMargin_Looking4ThHi;   
	DWORD		ValThHi;  
	DWORD		ValThLo;   
}	PACK mfgBt_CmdRfCloseLoopAdj_t;
*/

typedef struct mfgBt_CmdRxInfo_t
{
  	BtPkHeader	header; 
	DWORD		LinkIndex;
	DWORD		RssiAvg; 
	DWORD		RssiLast;  
	DWORD		RxCrcOkCnt;  
	DWORD		RxHecMatchCnt;   
	DWORD		RxCrcErrorCnt;  
	DWORD		RxFECCorrectCnt;   
} PACK mfgBt_CmdRxInfo_t;
 

#define LEN_RELEASENOTE		64
typedef  struct mfgBt_CmdReleaseNote_t
{
   	BtPkHeader	header; 
	BYTE		releaseNote[LEN_RELEASENOTE]; 
}	PACK mfgBt_CmdReleaseNote_t;


 
#define EFUSEBLOCKSIZE_INWORD 16
typedef struct mfgBt_CmdEfuse_t
{
   	BtPkHeader	header; 
	DWORD		BlockNo;
	DWORD		LenInWord;
	WORD		Data[EFUSEBLOCKSIZE_INWORD];		 
}	PACK mfgBt_CmdEfuse_t;


 


typedef struct  Bt_Mfg_TxTest_t
{
  	BtPkHeader	header; 
	DWORD		Enable;

//	BYTE		RxOnStart;
//	BYTE		SyntOnStart;
//	BYTE		TxOnStart;
//	BYTE		PhdOffStart;

//	BYTE		BtChannel;
//	BYTE		RxChannel;
	DWORD		PacketType;
	DWORD		PayloadPattern; 

	DWORD		PayloadLength;
	DWORD		TestInterval;
	DWORD		RandomHopMode;  
	DWORD		Whitening;
	DWORD		TxCnt;
	DWORD		TxPwrLvl;
} PACK Bt_Mfg_TxTest_t;

 
typedef struct  Bt_MfgCmd_RxTest_t
{ 
  	BtPkHeader	header; 

//	DWORD		TxChannel;
	DWORD		RxChannel;
	DWORD		PacketType;
	DWORD		PayloadPattern; // test scenario

	DWORD		PayloadLength;
	DWORD		PacketNumber;

//	BYTE		PRBS[LEN_PRBS_INBYTE];
	BYTE		TxBdAddress[SIZE_BDADDRESS_INBYTE];
	WORD		TxAmAddress;
//	DWORD		CorrelationWindow; // shall be same as packetNumber
//	BYTE		Whitening;		// shall be 1
//	BYTE		RxDelayPacket;	// shall be 0

//	BYTE		ReportErrors;
//	BYTE		CorrelationThreshold;
} PACK Bt_MfgCmd_RxTest_t;

typedef struct  Bt_MfgRsp_RxTest_t
{
  	BtPkHeader	header; 
	DWORD		Status_Complete0_Abort1; // test scenario
	DWORD		TotalPcktCountExpected;
	DWORD		NoRxCount;
	DWORD		SuccessfulCorrelation;
	DWORD		HecMatchCount;
	DWORD		HecMatchCrcPckts;
	DWORD		PayLoadHdrErrorCount;
	DWORD		CrcErrCount;
	DWORD		TotalPacketRxedCount;		// shall be 1
	DWORD		PacketOkCount;		// shall be 1
	DWORD		DroppedPacketCount;		// shall be 1
	DWORD		PER; // shall be same as packetNumber
	DWORD		TotalBitsCountExpected;
	DWORD		TotalBitsCountError_LostDrop;
	DWORD		BER;

	DWORD		TotalByteCountRxed;		// shall be 1
	DWORD		TotalBitErrorRxed;		// shall be 1
	DWORD		RSSI;	// shall be 0
} PACK Bt_MfgRsp_RxTest_t;


typedef struct  Bt_Mfg_RssiRd_t
{
  	BtPkHeader	header; 
	DWORD		LinkId; 
	DWORD		RssiAvg; 
	DWORD		RssiLast;  
} PACK Bt_Mfg_RssiRd_t;


typedef struct  Bt_Mfg_PeriodicInquiry_t
{
  	BtPkHeader	header; 
	DWORD		MaxPeriod; 
	DWORD		MinPeriod; 
	DWORD		InquryLength;  
} PACK Bt_Mfg_PeriodicInquiry_t;


typedef struct  mfgBt_PcmLoopBack_t
{
  	BtPkHeader	header; 
	DWORD		Enable; 
	DWORD		useGpio12AsDin;  
} PACK mfgBt_PcmLoopBack_t;


#ifdef _ENGINEERING_
// DBG
typedef struct  Bt_Mfg_Debug_TempCtrl_t
{
  	BtPkHeader	header; 
	DWORD		TempAvg; 
	DWORD		TempCur; 
	DWORD		CorrDelta; 
	DWORD		Temp10Avg; 
	DWORD		CtrlActive; 
} PACK Bt_Mfg_Debug_TempCtrl_t;
#endif // #ifdef _ENGINEERING_



typedef struct  mfgBt_CmdBdAddress_t
{
  	BtPkHeader	header; 
//	BYTE       BdAddress[SIZE_BDADDRESS_INBYTE];
	Bt_BdAddress	Address;  
} PACK mfgBt_CmdBdAddress_t;

//////////////////////////////////////



typedef struct  Bt_PfwCmd_RfDynamicClkCtrl
{ 
	WORD		DynClkTimer;		// C66[11:0]	BRF timer for a guard period after rifRx and rifTx.
	BYTE		MultiSlotProtect;		// C66[12]		This enables a override of the finish clock timer
	BYTE		AutoDetect;			// C66[13]		This enables the automatic detection of a completed frame.
	BYTE		DynClkValue;		// C66[14]		Setting for the manual override of the dynamic clock control.
	BYTE		DynClkEnable;		// C66[15]		This is an automatic control of the BRF clock.
	WORD		DynClkStartTime;	// C68[14:0]	This is the start time based on the frame timer as a reference. Range: 0-4999.
	WORD		DynClkFinishTime;	// C6A[14:0]	This is the finish time based on the frame timer as a reference. Range: 0-4999.
	BYTE		DynClkTimedEnable;	// C6A[15]		Enable for the timed finish. The finish time is ignored if this bit is set.
} PACK Bt_PfwCmd_RfDynamicClkCtrl;


//enum{
//PFWBT_SLEEPMODE_VAL_WLAN_PD,		//0
//PFWBT_SLEEPMODE_VAL_SLEEP,		//1
//PFWBT_SLEEPMODE_VAL_DEEPSLEEP,	//2
//PFWBT_SLEEPMODE_VAL_FULLPOWER,	//3
//} PFWBT_SLEEPMODE_VAL;

typedef struct  Bt_PfwCmd_SetBtSleepMode
{ 
	BYTE		PowerMode;		//  
} PACK Bt_PfwCmd_SetBtSleepMode;

typedef struct  Bt_PfwRsp_MrvlLowPowerModeCtrolResp
{ 
	BYTE		PowerMode;		// "BT-Only-State (Wlan Power Down)", 0x00
								// "Low Power State (Sleep Mode)", 0x01
								// "Auto-Sleep State (Deep Sleep Mode)", 0x02
								// "Full Power State (Full Power Mode)", 0x03  
	BYTE		Status;			// "Succeed", 0x00; "Aborted", 0x01
} PACK Bt_PfwRsp_MrvlLowPowerModeCtrolResp;



//enum{
//PFWBT_HOSTWAKEUPMETHOD_VAL_NONEED,		//0
//PFWBT_HOSTWAKEUPMETHOD_VAL_DTR,		//1
//PFWBT_HOSTWAKEUPMETHOD_VAL_BREAK,	//2
//PFWBT_HOSTWAKEUPMETHOD_VAL_GPIO,	//3
//} PFWBT_HOSTWAKEUPMETHOD_VAL;

//enum{
//PFWBT_DEVICEWAKEUPMETHOD_VAL_DSR,		//0
//PFWBT_DEVICEWAKEUPMETHOD_VAL_BREAK,	//1
//PFWBT_DEVICEWAKEUPMETHOD_VAL_GPIO,	//2
//} PFWBT_HOSTWAKEUPMETHOD_VAL;


typedef struct  Bt_PfwCmd_WakeUpMethod
{ 
	BYTE		HostWakeUpMethod;		// "Host Wake No Need", 0x00
										// "Host Wake up by DTR", 0x01
										// "Host Wake up by Break", 0x02
										// "Host Wake up by GPIO", 0x03
	BYTE		HostWakeUpGPIO;			// value 0 - 7 for GPIO0 - 7, 0xFF = invalid
	BYTE		DeviceWakeUpMethod;		// "Device Wake up by DSR", 0x00
 										// "Device Wake up by Break", 0x01
										// "Device Wake up by GPIO", 0x02

	BYTE		DeviceWakeUpGPIO;		// value 0 - 7 for GPIO0 - 7, 0xFF = invalid

} PACK Bt_PfwCmd_WakeUpMethod;

const BYTE Pfw_ModuleShutDownBringUp_Map[]={0xF1, 0xF2, 0xF5, 0xE1, 0xE2, 0xE3, 0xE4};
typedef struct  Bt_PfwCmd_MrvlHiuModuleConfig
{ 
	BYTE		ModuleShutDownBringUp;	// "Bring up BT Module (HIF Keeps Up)", 0xF1
										// "Shut Off BT Module (HIF Keeps Up)", 0xF2
 										// "Host Interface Control", 0xF5
										// "Power Up WLAN Module (HIF Also Up)", 0xE1
 										// "Power Down WLAN Module (HIF Also Down)", 0xE2
 										// "Power Up WLAN Module (HIF Keeps Up)", 0xE3
 										// "Power Down WLAN Module (HIF Keeps Up)", 0xE4

	BYTE		HostIfActive;			// "Activate", 0x01
										// "Deactivate", 0x00

	BYTE		HostIfType;				// "All Interfaces", 0x00
										// "Interfaces used by WLAN", 0x01
										// "Interfaces used by BT", 0x02
 										// "Interfaces used by FM (Not supported yet)", 0x03
										// "Interfaces used by GPS (Not supported yet)", 0x04
} PACK Bt_PfwCmd_MrvlHiuModuleConfig;

typedef struct  Bt_PfwRsp_MrvlHiuModuleConfig
{ 
	BYTE		ModuleShutDownBringUp;	// "Bring up BT Module (HIF Keeps Up)", 0xF1
										// "Shut Off BT Module (HIF Keeps Up)", 0xF2
 										// "Host Interface Control", 0xF5
										// "Power Up WLAN Module (HIF Also Up)", 0xE1
 										// "Power Down WLAN Module (HIF Also Down)", 0xE2
 										// "Power Up WLAN Module (HIF Keeps Up)", 0xE3
 										// "Power Down WLAN Module (HIF Keeps Up)", 0xE4

	BYTE		HostIfActive;			// "Activate", 0x01
										// "Deactivate", 0x00

	BYTE		HostIfType;				// "All Interfaces", 0x00
										// "Interfaces used by WLAN", 0x01
										// "Interfaces used by BT", 0x02
 										// "Interfaces used by FM (Not supported yet)", 0x03
										// "Interfaces used by GPS (Not supported yet)", 0x04
	BYTE		Status;			// "Succeed", 0x00; "Aborted", 0x01
} PACK Bt_PfwRsp_MrvlHiuModuleConfig;

typedef struct  Bt_PfwCmd_SetMrvlRadioPerformanceMode
{ 
	BYTE		MrvlRxPowerMode;		// "Disable, (Manual Rx Mode)", 0x00
										// "AUTO RX, RSSI Based", 0x01
 										// "AUTO RX, CRC Based", 0x02
 										// "AUTO RX, RSSI & CRC Based", 0x03
	BYTE		PerfModeBdrRssiTh;		// in dBm [For AUTO RX mode] Default = "0xB3"
	BYTE		PerfModeEdrRssiTh;		// in dBm [For AUTO RX mode] Default = "0xB7"
	BYTE		NormModeBdrRssiTh;		// in dBm [For AUTO RX mode] Default = "0xB8"
	BYTE		NormModeEdrRssiTh;		// in dBm [For AUTO RX mode] Default = "0xBC"

	BYTE		PerfModeBdrCrcTh;		// [For AUTO RX mode] Default = "0x10"
	BYTE		PerfModeEdrCrcTh;		// [For AUTO RX mode] Default = "0x20"
	BYTE		NormModeBdrCrcTh;		// [For AUTO RX mode] Default = "0x20"
	BYTE		NormModeEdrCrcTh;		// [For AUTO RX mode] Default = "0x30"
	BYTE		TxModeInTestMode;		// "Normal Mode", 0x00; "Performance Mode", 0x01
	BYTE		RxModeInTestMode;		// "Normal Mode", 0x00; "Performance Mode", 0x01
	BYTE		RxModeInOperationMode;	// "Normal Mode", 0x00; "Performance Mode", 0x01
	WORD		RssiCrcSampleInterval;	// "in 1.25ms frames [For AUTO RX mode]" Default = "0x00F0"
	BYTE		LowPowerScanMode;		// "Disable", 0x00; "Enable", 0x01
} PACK Bt_PfwCmd_SetMrvlRadioPerformanceMode;



typedef struct  Bt_PfwCmd_MrvlArmDynamicClkCtrl
{ 
	BYTE		SetNcoMode;				//  "Disable", 0x00; "Enable", 0x01
	BYTE		DynamicClockChange;		// Triggered by UART baudrate or Packet type
										// "Disable", 0x00; "Enable", 0x01
} PACK Bt_PfwCmd_MrvlArmDynamicClkCtrl;


typedef struct  Bt_PfwCmd_BtXCompModeConfig
{ 
	BYTE		BTCoexistenceMode;				//  "Disable", 0x00; "Enable", 0x01
	BYTE		PriorityMode_Wlan:1;			// 0 = Ignore WLAN, 1 = No Req when WLAN Active
	BYTE		PriorityMode_UpdateDefInPta:1;	// Update Default Settings in PTA
												// "Disable", 0x00; "Enable", 0x01
	WORD		PriorityDelay_BitMap;					// Unit: 250ns
	WORD		HighPriority_BitMap;					// "Bit map (12Bits):Bit11-ACL,BCST,SCO,eSCO,eSCOInstant,ParkS,ParkM,InqScan,Inq,PageScan,Page,Bit0-Switch"
} PACK Bt_PfwCmd_BtXCompModeConfig;

typedef struct  Bt_PfwCmd_BtMrvlModeConfig
{ 
	BYTE		BTCoexistenceMode;				//  "Disable", 0x00; "Enable", 0x01
	BYTE		PriorityMode_Wlan:1;			// 0 = Ignore WLAN, 1 = No Req when WLAN Active
	BYTE		PriorityMode_UpdateDefInPta:1;	// Update Default Settings in PTA
												// "Disable", 0x00; "Enable", 0x01
	WORD		PriorityDelay_BitMap;					// Unit: 250ns
	WORD		HighPriority_BitMap;					// "Bit map (12Bits):Bit11-ACL,BCST,SCO,eSCO,eSCOInstant,ParkS,ParkM,InqScan,Inq,PageScan,Page,Bit0-Switch"
	WORD		MediumPriority_BitMap;					// "Bit map (12Bits):Bit11-ACL,BCST,SCO,eSCO,eSCOInstant,ParkS,ParkM,InqScan,Inq,PageScan,Page,Bit0-Switch"
	WORD		BtEndTimer;						// "0 means End Timer is disabled. Unit: 250ns"
	BYTE		BtFreqLowBand;
	BYTE		BtFreqHighBand;
} PACK Bt_PfwCmd_BtMrvlModeConfig;

typedef struct  Bt_PfwCmd_BtMrvlMode2Config
{ 
	BYTE		BTCoexistenceMode;				//  "Disable", 0x00; "Enable", 0x01
	BYTE		NonPeriodicMode_Enable;				// Allow Tx Collisions: Disable", 0x00; "Enable", 0x01
	BYTE		NonPeriodicMode_BtTimerTlead;	// "Unit: 1us"  Default = "0x0D"
	BYTE		NonPeriodicMode_BtTimerTtail;	// "Unit: 1us"  Default = "0x23"
	WORD		PeriodicMode_BtSyncPeriod;		// "Unit: 625us" Default = "0x0012"
	WORD		PeriodicMode_BtSyncPulseWidth;	// "Unit: 1us" Default = "0x0271"
	BYTE		PeriodicMode_BtTsyncLead;		// "Unit: 250ns" Default = "0x01"
	BYTE		PeriodicMode_BtSyncMode;		// Hardware Controlled =1
} PACK Bt_PfwCmd_BtMrvlMode2Config;


typedef struct mfgBt_BtTxPower_t
{
   	BtPkHeader	header; 
	DWORD		BtTxPower;
	DWORD		IsEDR;
} PACK mfgBt_BtTxPower_t;


 

/*
// MFG_BT_CMD_PW_MODE

typedef struct mfgBt_CmdPwrMode_t
{
   	BtPkHeader	header;
	DWORD		action; 
} PACK mfgBt_CmdPwrMode_t;
   //
   // power mode structure
   //
typedef struct mfgBt_CmdPsmCycle_t
{
   	BtPkHeader	header; 
   	DWORD		numCycle;
   	DWORD		sleepDur;
   	DWORD		rxDur;
   	DWORD		stableDur;
} PACK mfgBt_CmdPsmCycle_t;
   //
   // power mode structure
   //
*/  
 
 
/*
//MFG_BT_CMD_RFPWRCALED
typedef struct host_CmdRfPowCal_t
{   
   	BtPkHeader	header; 
	DWORD		Pwr4Pa;
	DWORD		RateG;
	
} PACK host_CmdRfPowCal_t;
   //
   // RF power using cal data structure
   //


//MFG_BT_CMD_RF_POW
typedef struct mfgBt_CmdRfPowCaled_t
{   
   	BtPkHeader	header; 
	DWORD		Pwr4Pa; 
	DWORD		RateG; 
	DWORD		calDataLen;
	DWORD		caldataext;
} PACK mfgBt_CmdRfPowCaled_t;
*/
 

 

   


 
/*
typedef struct mfgBt_CmdSoftResetHW_t
{
   	BtPkHeader	header;  
}	PACK mfgBt_CmdSoftResetHW_t;
 

#define LEN_RELEASENOTE 256
typedef struct mfgBt_CmdReleaseNote_t
{
   	BtPkHeader	header;
//	DWORD		mfgCmd;
	DWORD 		action;
	DWORD		Error;   
	BYTE		releaseNote[LEN_RELEASENOTE]; 
}	PACK mfgBt_CmdReleaseNote_t;
*/

/*
typedef struct mfgBt_CmdMacAdd_t
{
   	BtPkHeader	header;
//	DWORD		mfgCmd;
	DWORD		action;
	DWORD		Error;   
	BYTE		macAddress[6];
} PACK mfgBt_CmdMacAdd_t;
   //
   // mac address structure
   //


typedef struct host_CmdClrRxMp_t
{
   	BtPkHeader	header;
	DWORD		action;
}	PACK mfgBt_CmdClrRxMp_t;
   //
   // clear Rx multicast packet structure
   //

typedef struct host_CmdClrRxP_t
{
   	BtPkHeader	header;
	DWORD		action;
} PACK mfgBt_CmdClrRxP_t;
   //
   // clear Rx packet structure
   //

typedef struct host_CmdClrRxErr_t
{
   	BtPkHeader	header;
	DWORD		action;
} PACK mfgBt_CmdClrRxErr_t;
   //
   // clear Rx error packet structure
   //

typedef struct host_CmdGetRxMp_t
{
	BtPkHeader	header;
	DWORD		action;
	DWORD		mcPacket;
} PACK mfgBt_CmdGetRxMp_t;
   //
   // Rx multicast packet structure
   //

typedef struct host_CmdGetRxP_t
{
   	BtPkHeader	header;
	DWORD		action;
	DWORD		rxPacket;
} PACK mfgBt_CmdGetRxP_t;
   //
   // Rx unicast packet structure
   //

typedef struct host_CmdGetRxErr_t
{
   	BtPkHeader	header;
	DWORD		action;
	DWORD		errPacket;
} PACK mfgBt_CmdGetRxErr_t;
   //
   // Rx error packet structure
   //

  

typedef struct host_CmdHwVers_t
{
   	BtPkHeader	header;
	DWORD		action;
	DWORD		bbVersion;
	DWORD		rfVersion;
}	PACK mfgBt_CmdHwVers_t;
   //
   // BB and RF hardware version structure
   //

 
typedef struct host_CmdMacAdd_t
{
   	BtPkHeader	header;
	DWORD		action;
	BYTE		macAddress[6];
} PACK mfgBt_CmdMacAdd_t;
   //
   // mac address structure
   //

typedef struct host_CmdCcode_t
{
   	BtPkHeader	header;
	DWORD		action;
	WORD		countryCode; 
} PACK mfgBt_CmdCcode_t;
   //
   // mac address structure
   //

typedef struct host_CmdPidVid_t
{
   	BtPkHeader	header;
	DWORD		action;
	WORD		pid;
	WORD		vid;
} PACK mfgBt_CmdPidVid_t;
   //
   // PID (PCI Device ID) 
   // and VID (PCI Vendor ID) structure
   //

typedef struct mfgBt_CmdClassId_t
{
   	BtPkHeader	header;
	DWORD		action;
	WORD		classId;
} PACK mfgBt_CmdClassId_t;
   //
   // Class Id structure
   //

 

typedef struct host_CmdCalibData_t
{
   	BtPkHeader	header;
	DWORD	Action;
	BYTE	BrdDesc[8];
	BYTE	Revision;
	BYTE	PaOption;
	BYTE	ExtPA;
	BYTE	AntCfg;
	WORD	PaCal[14];
	BYTE	CcaRange[4];
	BYTE	CcaThrpt[4];
	BYTE	CountryCode[2];
	BYTE	Customer[2];
	BYTE	Led[4];
	BYTE	Reserved[6];
	WORD	Chk;
} PACK mfgBt_CmdCalibData_t;
   //
   // calibration data structure
   //
 
 
*/


#if defined(WIN32) || defined(NDIS51_MINIPORT) || defined(NDIS50_MINIPORT) || defined(_LINUX_)
#pragma pack() 
#endif

#endif
