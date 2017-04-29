/** @file WlanMfgCmd.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

// Command Code

#ifndef _MFG_CMD_
#define _MFG_CMD_

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

#define BASE_COMMANDCODE   (0x1000)
//#define BASE_CODE_DELTA		0x8000
//#define BASE_RESPONSECODE  (BASE_COMMANDCODE +0x8000)
#define RSP_CODE(a) (0x8000|a)
typedef enum
{
    MFG_CMD_NONE                   = (BASE_COMMANDCODE + 0x0000),
    MFG_CMD_MAC_REG                = (BASE_COMMANDCODE + 0x0001),
    MFG_CMD_RF_REG                 = (BASE_COMMANDCODE + 0x0002),
    MFG_CMD_BB_REG                 = (BASE_COMMANDCODE + 0x0003),
    MFG_CMD_TX_ANT                 = (BASE_COMMANDCODE + 0x0004),
    MFG_CMD_RX_ANT                 = (BASE_COMMANDCODE + 0x0005),
    MFG_CMD_TX_RATE                = (BASE_COMMANDCODE + 0x0006),
    MFG_CMD_22M_RATE               = (BASE_COMMANDCODE + 0x0007),
    MFG_CMD_72M_RATE               = (BASE_COMMANDCODE + 0x0008),
    MFG_CMD_TX_CONT                = (BASE_COMMANDCODE + 0x0009),
    MFG_CMD_RF_CHAN                = (BASE_COMMANDCODE + 0x000A),
    MFG_CMD_RF_POW                 = (BASE_COMMANDCODE + 0x000B),
    MFG_CMD_WAKEUP_RFBBP           = (BASE_COMMANDCODE + 0x000C),
    MFG_CMD_PW_MODE                = (BASE_COMMANDCODE + 0x000D),
    MFG_CMD_CLR_RX_MP              = (BASE_COMMANDCODE + 0x000E),
    MFG_CMD_CLR_RX_UP              = (BASE_COMMANDCODE + 0x000F),
    MFG_CMD_CLR_RX_ERR             = (BASE_COMMANDCODE + 0x0010),
    MFG_CMD_GET_RX_MP              = (BASE_COMMANDCODE + 0x0011),
    MFG_CMD_GET_RX_UP              = (BASE_COMMANDCODE + 0x0012),
    MFG_CMD_GET_RX_ERR             = (BASE_COMMANDCODE + 0x0013),
    MFG_CMD_RSSI_PWOPT             = (BASE_COMMANDCODE + 0x0014),
    MFG_CMD_GET_RSSI               = (BASE_COMMANDCODE + 0x0015),
    MFG_CMD_TUNED_BB               = (BASE_COMMANDCODE + 0x0016),
    MFG_CMD_EN_CST                 = (BASE_COMMANDCODE + 0x0017),
    MFG_CMD_RX_ED_THRS             = (BASE_COMMANDCODE + 0x0018),
    MFG_CMD_BCN_INT                = (BASE_COMMANDCODE + 0x0019),
    MFG_CMD_GET_HW_VERS            = (BASE_COMMANDCODE + 0x001A),
    MFG_CMD_CTRL_BCN               = (BASE_COMMANDCODE + 0x001B),
    MFG_CMD_CHA_BCN_obs            = (BASE_COMMANDCODE + 0x001C),
    MFG_CMD_MAC_ADD                = (BASE_COMMANDCODE + 0x001D),
    MFG_CMD_CCODE                  = (BASE_COMMANDCODE + 0x001E), 
    MFG_CMD_PIDVID                 = (BASE_COMMANDCODE + 0x001F), 
    MFG_CMD_CLASS_ID               = (BASE_COMMANDCODE + 0x0020), 
    MFG_CMD_TX_FRAME               = (BASE_COMMANDCODE + 0x0021), 
    MFG_CMD_CALIB_DATA             = (BASE_COMMANDCODE + 0x0022), 
    MFG_CMD_VGACTRL                = (BASE_COMMANDCODE + 0x0023),
    MFG_CMD_RFPWRVAL               = (BASE_COMMANDCODE + 0x0024),
    MFG_CMD_STEP_RF_PWR				= (BASE_COMMANDCODE + 0x0025),
    MFG_CMD_DUTY_CYCLE				= (BASE_COMMANDCODE + 0x0026),
    MFG_CMD_SPI_EEPROM				= (BASE_COMMANDCODE + 0x0027),
	MFG_CMD_SPI_MEM					= (BASE_COMMANDCODE + 0x0028),
	MFG_CMD_SPI_DL					= (BASE_COMMANDCODE + 0x0029),
    MFG_CMD_FW_VERS					= (BASE_COMMANDCODE + 0x002A),
    MFG_CMD_XOSCCAL					= (BASE_COMMANDCODE + 0x002B),
    MFG_CMD_RFPWRRANGE				= (BASE_COMMANDCODE + 0x002C),
    MFG_CMD_RFPWRSETTING			= (BASE_COMMANDCODE + 0x002D),
    MFG_CMD_PSM_CYCLE				= (BASE_COMMANDCODE + 0x002E),
    MFG_CMD_ENB_SSID				= (BASE_COMMANDCODE + 0x002F),
    MFG_CMD_LCCAPCTRL               = (BASE_COMMANDCODE + 0x0030),
    MFG_CMD_RSSI_SN_NPWR            = (BASE_COMMANDCODE + 0x0031), // new. for RSSI S/N NoiseFloor report
    MFG_CMD_SOFTRESET               = (BASE_COMMANDCODE + 0x0032), // new for soft reset
    MFG_CMD_RFPWRCALED              = (BASE_COMMANDCODE + 0x0033), // new for set rf power using cal data
    MFG_CMD_RF_BAND_AG         		= (BASE_COMMANDCODE + 0x0034), // new for load RF/BB settings, power table and ch14 PA bias from EEPROM MS8380 only
    MFG_CMD_RC_CAL					= (BASE_COMMANDCODE + 0x0035), // new for 8020/8030
    MFG_CMD_RF_RCCTRL				= (BASE_COMMANDCODE + 0x0036), // new for 8020/8030
    MFG_CMD_RFXTAL_CTRL             = (BASE_COMMANDCODE + 0x0037), // new for 8020/8030
    MFG_CMD_LD_PWRTBL               = (BASE_COMMANDCODE + 0x0038), // new for 8020/8030 // load power table
    MFG_CMD_LNAGAIN               	= (BASE_COMMANDCODE + 0x0039), // new for 8020/8030
    MFG_CMD_LDTXRXRFSETTINGS       	= (BASE_COMMANDCODE + 0x003a), // new for 8020/8030
    MFG_CMD_RF_POW_CTRL_LOOPMODE    = (BASE_COMMANDCODE + 0x003b), // new for 8015
    MFG_CMD_RF_PA_THERMAL_SCALER    = (BASE_COMMANDCODE + 0x003c), // new for 8015
    MFG_CMD_RF_RANGE_TABLE			= (BASE_COMMANDCODE + 0x003d), // new for 8015
    MFG_CMD_RELEASENOTE				= (BASE_COMMANDCODE + 0x003E),  
    MFG_CMD_LOADSETS_EEPROM         = (BASE_COMMANDCODE + 0x003F), // new for load RF/BB settings, power table and ch14 PA bias from EEPROM MS8380 only
    MFG_CMD_HWRESETRF               = (BASE_COMMANDCODE + 0x0040), // new for soft reset

	MFG_CMD_BCA_WLACTTXCONF_STATUS	= (BASE_COMMANDCODE + 0x0041),
    MFG_CMD_BCA_SYSTEM_CONFIG		= (BASE_COMMANDCODE + 0x0042),
    MFG_CMD_RFCHANFREQ             	= (BASE_COMMANDCODE + 0x0043), // new for soft reset
    MFG_CMD_11N_CHANNELBW      		= (BASE_COMMANDCODE + 0x0044), // new for soft reset
	MFG_CMD_RF_FREQSUBBAND     		= (BASE_COMMANDCODE + 0x0045),  
    MFG_CMD_PAEXT              		= (BASE_COMMANDCODE + 0x0046), // new for PA external add for CF8385PX
	MFG_CMD_RF_CALPWRFIELDS			= (BASE_COMMANDCODE + 0x0047),  
	MFG_CMD_RF_CALCLOSELOOP			= (BASE_COMMANDCODE + 0x0048), // new for power cal; move closeloops adjust to FW 
	MFG_CMD_TXIQCALMODE				= (BASE_COMMANDCODE + 0x0049), // new for Tx IQ cal, enter/exist mode 
	MFG_CMD_TXIQSETTING				= (BASE_COMMANDCODE + 0x004A), // new for Tx IQ cal, settings
	MFG_CMD_RF_LOSPURS				= (BASE_COMMANDCODE + 0x004B), // new for Lo Spurs cal.  
	MFG_CMD_LOAD_LOTBL				= (BASE_COMMANDCODE + 0x004C), // new for Lo Spurs cal, load tbl.  
	MFG_CMD_LOAD_IQTBL				= (BASE_COMMANDCODE + 0x004D), // new for IQ cal, load tbl.  
	MFG_CMD_LOAD_RXGAINTBL			= (BASE_COMMANDCODE + 0x004E), // new for IQ cal, load tbl.  
	MFG_CMD_RF_RXPATHGAIN			= (BASE_COMMANDCODE + 0x004F), // new for Rx Path Gain Mismatch.  
	MFG_CMD_RF_RXPATHGAINRESET		= (BASE_COMMANDCODE + 0x0050), // new for Rx Path Gain Mismatch.  
	MFG_CMD_SCRAMBLER_SEED			= (BASE_COMMANDCODE + 0x0051), // new for fix scrambler seed.  		
	MFG_CMD_RXIQCALMODE				= (BASE_COMMANDCODE + 0x0052), // new for Tx IQ cal, enter/exist mode 
	MFG_CMD_RXIQSETTING				= (BASE_COMMANDCODE + 0x0053), // new for Tx IQ cal, settings
	MFG_CMD_RXIQESTMATED			= (BASE_COMMANDCODE + 0x0054), // new for Tx IQ cal, settings
	MFG_CMD_CCKFLTRSCALE			= (BASE_COMMANDCODE + 0x0055), // new for CCK filter scaling factor change
	MFG_CMD_INITSETTING_OR			= (BASE_COMMANDCODE + 0x0056), // new for init Setting override
    MFG_CMD_BF_BBU_TONES            = (BASE_COMMANDCODE + 0x0057),   // access to BF BBU tone memory
    MFG_CMD_BF_MAC_SBF              = (BASE_COMMANDCODE + 0x0058),   // access to BF MAC SBF mac addresses
    MFG_CMD_BF_MAC_RBFMAC           = (BASE_COMMANDCODE + 0x0059),   // access to BF MAC RBF mac addresses
    MFG_CMD_BF_MAC_RBFLOCK          = (BASE_COMMANDCODE + 0x005a),   // access to BF MAC RBF lock, reserve, txbfon, valid bits
    MFG_CMD_BF_MAC_RBFTIMEOUT       = (BASE_COMMANDCODE + 0x005b),   // access to BF MAC RBF aging enable bits and timer thresh
    MFG_CMD_BF_MAC_RBFTIMERES       = (BASE_COMMANDCODE + 0x005c),   // access to BF MAC RBF timer resolution
    MFG_CMD_EXTSDRAMDIAG            = (BASE_COMMANDCODE + 0x005d),   // new for external SDRAM diagnostic
	MFG_CMD_GPIOCTRL				= (BASE_COMMANDCODE + 0x005E), // new for GPIO control
    MFG_CMD_BBPSQDIAG               = (BASE_COMMANDCODE + 0x005f),
	MFG_CMD_TRSWITCHMODE			= (BASE_COMMANDCODE + 0x0060),
	MFG_CMD_THERMALCOMP				= (BASE_COMMANDCODE + 0x0061),
	MFG_CMD_THERMALSENSOR_RB		= (BASE_COMMANDCODE + 0x0062),
	MFG_CMD_ARMREADULONGARRAY       = (BASE_COMMANDCODE + 0x0063),
	MFG_CMD_OR_VERSION				= (BASE_COMMANDCODE + 0x0064),
//	MFG_CMD_EXTLANMODE				= (BASE_COMMANDCODE + 0x0065),
	MFG_CMD_TXRXIQCALMODE			= (BASE_COMMANDCODE + 0x0065), // use mfg_CmdTxRxCal_t
    MFG_CMD_RF_DEBUG				= (BASE_COMMANDCODE + 0x0070),

	MFG_CMD_BCASETTING			    = (BASE_COMMANDCODE + 0x0071),
    MFG_CMD_SWT_MODULETYPE          = (BASE_COMMANDCODE + 0x0072), // new for soft reset
 	MFG_CMD_LDO_CFG			        = (BASE_COMMANDCODE + 0x0073),
	MFG_CMD_TX_FASTCAL				= (BASE_COMMANDCODE + 0x0074),
	MFG_CMD_TX_FASTVER	            = (BASE_COMMANDCODE + 0x0075),
	MFG_CMD_RX_FASTVER				= (BASE_COMMANDCODE + 0x0076),
	MFG_CMD_PM_REG				    = (BASE_COMMANDCODE + 0x0077),
 	MFG_CMD_PM_RAIL			        = (BASE_COMMANDCODE + 0x0078),
  	MFG_CMD_LNA_EXT_SETTING	        = (BASE_COMMANDCODE + 0x0079), // use mfg_LnaModeSetting_t
	MFG_CMD_DPDT_SETTING			= (BASE_COMMANDCODE + 0x007A),
	MFG_CMD_TXIQVALUE				= (BASE_COMMANDCODE + 0x007B),
	MFG_CMD_THERMALREADING			= (BASE_COMMANDCODE + 0x007C), // use mfg_CmdPwrMode_t
//	MFG_CMD_TXRXIQCALMODE			= (BASE_COMMANDCODE + 0x007D), // use mfg_CmdTxRxCal_t
	MFG_CMD_RXIQVALUE				= (BASE_COMMANDCODE + 0x007E), // use mfg_CmdTxRxIq_t 
	MFG_CMD_TXIQESTVALUE			= (BASE_COMMANDCODE + 0x007F), // use mfg_CmdTxRxIq_t 
	MFG_CMD_RXIQESTVALUE			= (BASE_COMMANDCODE + 0x0080), // use mfg_CmdTxRxIq_t 

	MFG_CMD_CAU_REG					= (BASE_COMMANDCODE + 0x0081), // use mfg_CmdRfReg_t 
	MFG_CMD_RFU_FW					= (BASE_COMMANDCODE + 0x0082), // use mem_t 
	MFG_CMD_RFU_FW_UPDATE			= (BASE_COMMANDCODE + 0x0083), // use mfg_CmdSoftResetHW_t 
	MFG_CMD_THERMAL_TEST			= (BASE_COMMANDCODE + 0x0084), // use mfg_CmdThermalTest_t 
	MFG_CMD_E2P_TYPE				= (BASE_COMMANDCODE + 0x0085), // use mfg_CmdE2PType_t
	MFG_CMD_RELOAD_CALDATA			= (BASE_COMMANDCODE + 0x0086), // use mfg_Cmd_t
	MFG_CMD_CALDATA_EXT				= (BASE_COMMANDCODE + 0x0087), // use mfg_Cmd_t
	MFG_CMD_DPD_INFO				= (BASE_COMMANDCODE + 0x0088), // use mfg_CmdDPDInfo_t
	
	MFG_CMD_CLOSE_LOOP_CAL 	        = (BASE_COMMANDCODE + 0x0165), // mfg_CmdCloseLoopCal
} ;
 
#define Host_Act_Get		0
#define Host_Act_Set		1

   //
   // Type of manufacturing actions send by the host
   //
  
 
typedef struct  PkHeader
{
 	WORD		cmd;		// message type
	WORD		len;
	WORD		seq;
	WORD		result;
	DWORD       MfgCmd;
} PACK PkHeader;

typedef struct mfg_Cmd_t
{
	PkHeader	header;
	DWORD		action;
	DWORD		Error;  
}	PACK mfg_Cmd_t;

typedef struct {
	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	DWORD signature;
	DWORD sector_erase;
    DWORD address;
	DWORD lenInByte;
	BYTE byte[0];
} PACK spi_CmdEeprom_t;

typedef struct 
{
	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	DWORD signature;
	DWORD sector_erase;
	DWORD len;
	DWORD src;          // source addr
	DWORD dest;         // destination addr
} PACK spi_CmdEepromPgm_t;

/*typedef struct {
	PkHeader	header;
	DWORD		action;
} PACK sflash_pgm_resp_t;
*/
typedef struct {
	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
   DWORD dest;         // destination addr
   DWORD seglen;       // segment length of firmware in units of byte
   BYTE seg[0];
} PACK mem_t;

typedef struct mfg_RFU_FW_t{
	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
    DWORD       offset;         // destination addr
    DWORD       seglen;       // segment length of firmware in units of byte
#if defined (_MIMO_)
	DWORD 		deviceId; 
#endif // #if defined (_MIMO_)
    BYTE        seg[0];
} PACK mfg_RFU_FW_t;

#define SIGNATURE_SPIEEPROM    0xc5c01688


// read or wite MAC registers
typedef struct mfg_CmdMacReg_t
{
	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	DWORD		address;
	DWORD		data;
#if defined (_MIMO_)
	DWORD 		deviceId; 
#endif // #if defined (_MIMO_)
}	PACK mfg_CmdMacReg_t;
   //
   // MAC register structure
   //


// read or wite RF registers
typedef struct host_CmdRfReg_t
{
	PkHeader	header;
	DWORD       action;
	DWORD		Error; 
	DWORD		address;
	DWORD		data;
#if defined (_MIMO_)
	DWORD 		deviceId; 
#endif // #if defined (_MIMO_)
}	PACK mfg_CmdRfReg_t;
   //
   // RF register structure
   //

// read or wite BBP registers
typedef struct host_CmdBbReg_t
{
	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	DWORD		address;
	DWORD		data;
#if defined (_MIMO_)
	DWORD 		deviceId; 
#endif // #if defined (_MIMO_)
}	PACK mfg_CmdBbReg_t;
   //
   // BB register structure
   //


// read or wite Tx Ant Selection MFG_CMD_TX_ANT
typedef struct mfg_CmdTxAnt_t 
{
	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	DWORD		antSelec;
#if defined (_MIMO_)
	DWORD 		deviceId; 
#endif // #if defined (_MIMO_)
}	PACK mfg_CmdTxAnt_t;
   //
   // Tx antenna selection structure
   //

// read or wite Rx Ant Selection MFG_CMD_RX_ANT
typedef struct mfg_CmdRxAnt_t
{
   	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	DWORD		antSelec;
#if defined (_MIMO_)
	DWORD 		deviceId; 
#endif // #if defined (_MIMO_)
} PACK mfg_CmdRxAnt_t;
   //
   // Rx antenna selection structure
   //



// read or wite tx Datarate Selection MFG_CMD_TX_RATE
typedef struct mfg_CmdTxRate_t
{
   	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	DWORD		txRate;
#if defined (_MIMO_)
	DWORD 		deviceId; 
#endif // #if defined (_MIMO_)
} PACK mfg_CmdTxRate_t;
   //
   // Tx data rate structure
   //


// read or wite 22MB enable Selection MFG_CMD_22M_RATE
typedef struct mfg_Cmd2272Rate_t
{
   	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	DWORD		txRx2272Rate;
} PACK mfg_Cmd2272Rate_t;
   //
   // 22 Mbps data rate structure
   //


// read or wite preamble Selection
typedef struct mfg_CmdPreamble_t
{
   	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	DWORD		shortPreamble;
} PACK mfg_CmdPreamble_t;
   //
   // preamble structure
   //


// MFG_CMD_TX_CONT
typedef struct mfg_CmdTxCont_t
{
   	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	DWORD		enableTx;
	DWORD		cwMode;
    DWORD		framePattern;
	DWORD		csMode;
	DWORD		ActSubCh; 
#if defined (_MIMO_)
	DWORD 		deviceId; 
#endif // #if defined (_MIMO_)
} PACK mfg_CmdTxCont_t;
   //
   // Tx continuous mode structure
   //


// MFG_CMD_EN_CST
typedef struct mfg_CmdCst_t
{
   	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	DWORD		enableCst;
#if defined (_MIMO_)
	DWORD 		deviceId; 
#endif // #if defined (_MIMO_)
} PACK mfg_CmdCst_t;
   //
   // Carrier Suppression Test structure
   //

// MFG_CMD_EN_CST
typedef struct mfg_CmdThermalComp_t
{
   	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	DWORD		enable;
	DWORD		tempRef;
	DWORD		tempRefSlope;
	DWORD		slopeNumerator;
	DWORD		slopeNumerator1;
	DWORD		slopeDenominator;
	DWORD		calInterval;
	DWORD		PTargetDelta;
#if defined (_MIMO_)
	DWORD 		deviceId; 
	DWORD 		pathId; 
#endif // #if defined (_MIMO_)
} PACK mfg_CmdThermalComp_t;
   //
   // Thermal Compensation
   //

typedef struct mfg_CmdThermalTest_t
{
   	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	DWORD		enable;
	DWORD		PTarget;
	DWORD		tempRef;
	DWORD		slopeNumerator;
	DWORD		slopeDenominator;
	DWORD		calInterval;
	DWORD		temp;
	DWORD		slopeNumerator1;
	DWORD		slopeDenominator1;
} PACK mfg_CmdThermalTest_t;
   //
   // Thermal Compensation
   //
typedef struct mfg_CmdBCASetting_t
{
 	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	DWORD		enable;
	DWORD		BCASetting;
} PACK mfg_CmdBCASetting_t;
   // BCA Setting

typedef struct mfg_CmdDPDTSetting_t
{
 	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	DWORD		DPDTSetting;
} PACK mfg_CmdDPDTSetting_t;
   // DPDT Setting

// MFG_CMD_PW_MODE

typedef struct mfg_CmdPwrMode_t
{
   	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	DWORD		powerMode;
} PACK mfg_CmdPwrMode_t;
   //
   // power mode structure
   //
typedef struct mfg_CmdPsmCycle_t
{
   	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
   	DWORD		numCycle;
   	DWORD		sleepDur;
   	DWORD		rxDur;
   	DWORD		stableDur;
#if defined (_MIMO_)
	DWORD 		deviceId; 
#endif // #if defined (_MIMO_)
} PACK mfg_CmdPsmCycle_t;
   //
   // power mode structure
   //

// MFG_CMD_BCN_INT
typedef struct mfg_CmdBcnInt_t
{
   	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	DWORD		beaconPeriod;
#if defined (_MIMO_)
	DWORD 		deviceId; 
#endif // #if defined (_MIMO_)
} PACK mfg_CmdBcnInt_t;
   //
   // beacon interval structure
   //

typedef struct mfg_CmdCtrlBcn_t
{
   	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	DWORD		enableBcn;
#if defined (_MIMO_)
	DWORD 		deviceId; 
#endif // #if defined (_MIMO_)
} PACK mfg_CmdCtrlBcn_t;
   //
   // control beacon (enable or disable) structure
   //

typedef struct mfg_CmdRfBand_t
{
   	PkHeader	header;
	DWORD		action;
	DWORD		Error;  
	DWORD		band_G0A1; 
#if defined (_MIMO_)
	DWORD 		deviceId; 
#endif // #if defined (_MIMO_)
}	PACK mfg_CmdRfBand_t;
// rf band

 
//MFG_CMD_RF_CHAN
typedef struct mfg_CmdRfChan_t
{
   	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	DWORD		channel;
	DWORD		FreqInMHz;
#if defined (_MIMO_)
	DWORD 		deviceId; 
#endif // #if defined (_MIMO_)
} PACK mfg_CmdRfChan_t;
   //
   // RF channel structure
   //
typedef struct mfg_CmdRfBandChanFreq_t
{
   	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	DWORD		band;
	DWORD		channel;
	DWORD		FreqInMHz;
#if defined (_MIMO_)
	DWORD 		deviceId; 
#endif // #if defined (_MIMO_)
} PACK mfg_CmdRfBandChanFreq_t;


typedef struct mfg_CmdChannelBw_t
{
   	PkHeader	header;
//	DWORD		mfgCmd;
	DWORD 		action;
	DWORD		Error;   
	DWORD		BW20401005MHz; 
#if defined (_MIMO_)
	DWORD 		deviceId; 
#endif // #if defined (_MIMO_)
}	PACK mfg_CmdChannelBw_t;

// set BBP for channel

typedef struct mfg_CmdTunedRfBb_t
{
   	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	DWORD		channel;
} PACK mfg_CmdTunedRfBb_t;
   //
   // tuned RF & BB structure
   //

typedef struct mfg_CmdCckFltrScale_t
{
	PkHeader	header;
//	DWORD		mfgCmd;
	DWORD 		action;
	DWORD		Error;   
	DWORD		scaleMode; // 0: 1x; 1: 0.75x; 2: 0.5x; 3: 0.375x
 	DWORD		deviceId; 
}	PACK mfg_CmdCckFltrScale_t;

//MFG_CMD_RF_POW
typedef struct host_CmdRfPow_t
{   
   	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	DWORD		Pwr4Pa;
	DWORD		CorrOffset;
	DWORD		Corr1A;
   DWORD 	deviceId; 
   DWORD 	pathId;
} PACK mfg_CmdRfPow_t;
   //
   // RF power structure
   //

//MFG_CMD_RFPWRCALED
typedef struct host_CmdRfPowCal_t
{   
   	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	DWORD		Pwr4Pa;
	DWORD		RateG;
	
} PACK host_CmdRfPowCal_t;
   //
   // RF power using cal data structure
   //

//MFG_CMD_RF_POW
typedef struct mfg_CmdRfPowCaled_t
{ 
   	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	DWORD		Pwr4Pa; 
	DWORD		RateG; 
#if defined (_MIMO_)
	DWORD 		deviceId; 
	DWORD 		pathId; 
#endif // #if defined (_MIMO_)
	DWORD		calDataLen;
	BYTE		calDataExt[0];
} PACK mfg_CmdRfPowCaled_t;

//MFG_CMD_WAKEUP_RFBBP
typedef struct mfg_CmdWakeRfBB_t
{
	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
} PACK mfg_CmdWakeRfBB_t;
   //
   // wake up RF and BB structure
   //
typedef struct mfg_CmdTxFrame2_t
{

	PkHeader	header;
	DWORD action;
	DWORD Error;
	DWORD Enable;
	DWORD DataRate;
	DWORD FramePattern;
	DWORD FrameLength;
	DWORD ShortPreamble;
	BYTE  Bssid[6];
//  WORD Reserved;
#if defined (_W8787_) || 	defined (_W8782_)						
	WORD	AdjustBurstSifs;
    DWORD	BurstSifsInUs;
#endif //#if defined (_W8787_) || 	defined (_W8782_)						
#if defined (_MIMO_)
    DWORD	ActSubCh;
    DWORD	ShortGI;
    DWORD	AdvCoding;
    DWORD	TxBfOn;    
    DWORD	GFMode;    
    DWORD	STBC;
	DWORD	DPDEnable;
	DWORD	PowerID;
	DWORD	SignalBw;
    DWORD	deviceId;
#endif // #if defined (_MIMO_)

} PACK mfg_CmdTxFrame2_t;
   //
   // tx frame structure
   //

typedef struct mfg_CmdTxFrame_t
{
   	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	DWORD		dataRate;
	DWORD		framePattern;
	DWORD		frameLength;
	DWORD		frameCount;
	DWORD		shortPreamble;
	BYTE		bssid[6];
    WORD 		Reserved; 
#if defined (_MIMO_)
	DWORD		ActSubCh; 
	DWORD		ShortGI; 
	DWORD		AdvCoding; 
    DWORD		TxBfOn;    
    DWORD		GFMode;    
    DWORD		STBC;
	DWORD		DPDEnable;
	DWORD		PowerID;
	DWORD 		deviceId; 
#endif // #if defined (_MIMO_)
} PACK mfg_CmdTxFrame_t;
   //
   // tx frame structure
   //


typedef struct mfg_CmdDutyCycle_t
{
	PkHeader	header;
	DWORD		Action;
	DWORD		Error; 
    DWORD		Enable;
    DWORD		DataRate;
    DWORD		Percent;
#if defined (_MIMO_)
    WORD		PayloadPattern;
    WORD		PayloadLengthInKB;
#else // #if defined (_MIMO_)
    DWORD		PayloadPattern;
#endif // #if defined (_MIMO_)
    DWORD		ShortPreamble;
#if defined (_MIMO_)
	DWORD		ActSubCh; 
	DWORD		ShortGI; 
	DWORD		AdvCoding; 
    DWORD		TxBfOn;    
    DWORD		GFMode;    
    DWORD		STBC;
	DWORD		DPDEnable;
	DWORD		PowerID;
	DWORD		SignalBw;
	DWORD 		deviceId; 
#endif // #if defined (_MIMO_)
} PACK mfg_CmdDutyCycle_t;
// structure used for the duty cycle
//    2) field2, enable (0 or 1)
//    3) field3, data rate 0 -> 1Mbps, 1-> 2Mbps...
//    4) field4, percentage

typedef struct mfg_CmdHwVers_t
{
   	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	WORD		socId;
	WORD		socVersion;
	WORD		bbId;
	WORD		bbVersion;
	WORD		rfId;
	WORD		rfVersion;
#if defined (_MIMO_)
	DWORD 		deviceId; 
#endif // #if defined (_MIMO_)
}	PACK mfg_CmdHwVers_t;
   //
   // BB and RF hardware version structure
   //

typedef struct mfg_CmdORVers_t
{
   	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	BYTE		OR_ID;
	BYTE		majorVersion;
	BYTE		minorVersion;
	BYTE		customerId;
}	PACK mfg_CmdORVers_t;
   //
   // OR version structure
   //

typedef struct mfg_CmdFwVers_t
{
   	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	DWORD		FwVersion;
	DWORD		MfgVersion; 
}	PACK mfg_CmdFwVers_t;
   //
   // BB and RF hardware version structure
   //

typedef struct mfg_CmdXoscCal_t
{
   	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	DWORD		TU;
	DWORD		xoscCal; 
#if defined (_MIMO_)
	DWORD 		deviceId; 
#endif // #if defined (_MIMO_)
}	PACK mfg_CmdXoscCal_t;
// Calibration of internal Xosc

/*
typedef struct mfg_PwrRangeTbleRow_t
{
DWORD val1Eh; DWORD val1Fh; DWORD val20h; DWORD topPwr; DWORD btmPwr; 
}
PACK mfg_PwrRangeTbleRow_t;
 
typedef struct mfg_CmdRfPwrRangeTbl_t
{
   	PkHeader	header;
//	DWORD		mfgCmd;
	DWORD 		action;
	DWORD		Error;
	DWORD		tableId; //0 ch 1-13, 1 ch14
	
	mfg_PwrRangeTbleRow_t	RangeTable_High1;  
	mfg_PwrRangeTbleRow_t	RangeTable_High2;  
	mfg_PwrRangeTbleRow_t	RangeTable_High3;  
	mfg_PwrRangeTbleRow_t	RangeTable_High4;  
}	PACK mfg_CmdRfPwrRangeTbl_t;

typedef struct mfg_CmdRfPwrRangeTbl_A_t
{
   	PkHeader	header;
//	DWORD		mfgCmd;
	DWORD 		action;
	DWORD		Error;
	DWORD       bandId;  //new for 11a
	DWORD		tableId; //0 ch 1-13, 1 ch14
	
	mfg_PwrRangeTbleRow_t	RangeTable_High1;  
	mfg_PwrRangeTbleRow_t	RangeTable_High2;  
	mfg_PwrRangeTbleRow_t	RangeTable_High3;  
	mfg_PwrRangeTbleRow_t	RangeTable_High4;  
}	PACK mfg_CmdRfPwrRangeTbl_A_t;
*/
typedef struct mfg_CmdRfPwrRange_t
{
   	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	DWORD		RangeIndex_PwrPnt; 
	DWORD		Top; 
	DWORD		Btm; 
#if defined (_MIMO_)
	DWORD 		deviceId; 
	DWORD 		pathId; 
#endif // #if defined (_MIMO_)
}	PACK mfg_CmdRfPwrRange_t;

typedef struct mfg_CmdLoadPwrTbl_t
{
   	PkHeader	header;
	DWORD		action;
	DWORD		Error;   
	DWORD		deviceId; 
	DWORD		pathId; 
	DWORD		bandId; 
	DWORD		subbandId; 
	DWORD		maxPower; 
	DWORD		minPower; 
	DWORD		sizeOfTblInByte; 	
	BYTE		tbl[0]; 
}	PACK mfg_CmdLoadPwrTbl_t;

typedef struct mfg_CmdRfPwrSetting_t
{
   	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	BYTE		PaConf1; 
	BYTE		PaConf2; 
	BYTE		PaConf3; 
	BYTE		Vt_Hi_G; 
	BYTE		Vt_Lo_G; 
	BYTE		InitP_G; 
}	PACK mfg_CmdRfPwrSetting_t;

typedef struct mfg_CmdRfPwrCloseLoopMode_t
{
	PkHeader	header;
	DWORD       action;
	DWORD       Error;
	DWORD       ThermalControlOnly; 
#if defined (_MIMO_)
	DWORD 		deviceId; 
#endif // #if defined (_MIMO_)
} PACK mfg_CmdRfPwrCloseLoopMode_t;
   //
   // mfg_CmdRfPwrCloseLoopMode_t structure
   //
   
typedef struct mfg_CmdRfPaThermalScaler_t
{
	PkHeader	header;
//	DWORD		mfgCmd;
	DWORD		action;
	DWORD		Error;
	DWORD		ThermalScaler;
#if defined (_MIMO_)
	DWORD 		deviceId; 
	DWORD 		pathId; 
#endif // #if defined (_MIMO_)
} PACK mfg_CmdRfPaThermalScaler_t;
   //
   // mfg_CmdRfPaThermalScaler_t structure
   //
typedef struct mfg_CmdStepRfPwr_t
{
   	PkHeader	header;
	DWORD	action;
	DWORD	Error; 
	int		StepRfPwr;
#if defined (_MIMO_)
	DWORD 		deviceId; 
	DWORD 	pathId;
#endif // #if defined (_MIMO_)
} PACK mfg_CmdStepRfPwr_t;
   //
   // stepRfPower structure
   //


typedef struct mfg_CmdEnableSsid_t

{
   	PkHeader	header;
	DWORD		action;
    DWORD		Error; 
    DWORD		mode; 
    char		Ssid[32];
    BYTE		Bssid[6];
	WORD		Reserved;  
#if defined (_MIMO_)
	DWORD 		deviceId; 
#endif // #if defined (_MIMO_)

} PACK mfg_CmdEnableSsid_t;

typedef struct mfg_ReadCounts_t

{
   	PkHeader	header;
	DWORD		action;
    DWORD		Error;  
    DWORD		PcktsCount;
	DWORD		OkCount;
	DWORD		KoCount;
#if defined (_MIMO_)
	DWORD 		deviceId; 
#endif // #if defined (_MIMO_)
} PACK mfg_ReadCounts_t;


typedef struct mfg_CmdRfVgaCtrl_t
{
   	PkHeader	header;
//	DWORD		mfgCmd;
	DWORD 		action;
	DWORD		Error;
	BYTE		extMode; 
	BYTE		value; 
	WORD		Reserved;  
#if defined (_MIMO_)
	DWORD 		deviceId; 
	DWORD 		pathId; 
#endif // #if defined (_MIMO_)
}	PACK mfg_CmdRfVgaCtrl_t;

typedef struct mfg_CmdRfLcCapCtrl_t
{
   	PkHeader	header;
//	DWORD		mfgCmd;
	DWORD 		action;
	DWORD		Error;
	BYTE		extMode; 
	BYTE		value; 
}	PACK mfg_CmdRfLcCapCtrl_t;

typedef struct mfg_CmdRfXTalCtrl_t
{
   	PkHeader	header;
//	DWORD		mfgCmd;
	DWORD 		action;
	DWORD		Error;
	BYTE		extension;
	BYTE		XTAL_CAL;
	WORD		Reserved;
#if defined (_MIMO_)
	DWORD 		deviceId; 
#endif // #if defined (_MIMO_)
}	PACK mfg_CmdRfXTalCtrl_t;

typedef struct mfg_CmdRssiSnNpwr_t
{
   	PkHeader	header;
//	DWORD		mfgCmd;
	DWORD 		action;
	DWORD		Error;  
 	DWORD		stop; 
	DWORD		sample_SNR; 
	DWORD		sample_NPwr; 
	DWORD		RSSI_Calc; 
	DWORD		SNR; 	
	DWORD		NoiseFloor; 
	DWORD		RSSI_recent1; 
	DWORD		SNR_recent1; 	
	DWORD		NoiseFloor_recent1; 
#if defined (_MIMO_)
	DWORD 		deviceId; 
#endif // #if defined (_MIMO_)
}	PACK mfg_CmdRssiSnNpwr_t;
 
typedef struct mfg_CmdSoftResetHW_t
{
   	PkHeader	header;
//	DWORD		mfgCmd;
	DWORD 		action;
	DWORD		Error;    
#if defined (_MIMO_)
	DWORD 		deviceId; 
#endif // #if defined (_MIMO_)
}	PACK mfg_CmdSoftResetHW_t;
 

#define LEN_RELEASENOTE 512 //1024
typedef struct mfg_CmdReleaseNote_t
{
   	PkHeader	header;
//	DWORD		mfgCmd;
	DWORD 		action;
	DWORD		Error;   
	BYTE		releaseNote[LEN_RELEASENOTE]; 
}	PACK mfg_CmdReleaseNote_t;


typedef struct mfg_CmdMacAdd_t
{
   	PkHeader	header;
//	DWORD		mfgCmd;
	DWORD		action;
	DWORD		Error;   
	BYTE		macAddress[6];
} PACK mfg_CmdMacAdd_t;
   //
   // mac address structure
   //
   
typedef struct mfg_CmdTxFastCal_t
{
	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	DWORD		DataStrucLen;
	DWORD		DataStrucPtr;
} PACK mfg_CmdTxFastCal_t;

typedef struct mfg_CmdRxFastVer_t

{
   	PkHeader	header;
	DWORD		action;
    DWORD		Error; 
	DWORD		enable; 
	DWORD		channel;
    char		Ssid[32];
    BYTE		Bssid[6];

} PACK mfg_CmdRxFastVer_t;

typedef struct mfg_CmdModuleType_t
{
   	PkHeader	header;
//	DWORD		mfgCmd;
	DWORD		action;
	DWORD		Error;   
	DWORD		ModuleType;
} PACK mfg_CmdModuleType_t;

typedef struct mfg_CmdLDOConfig_t
{
   	PkHeader	header;
//	DWORD		mfgCmd;
	DWORD		action;
	DWORD		Error;   
	DWORD		enable;
	DWORD		LDOSrc;
} PACK mfg_CmdLDOConfig_t;

typedef struct mfg_PM_Cfg_t {
   	PkHeader	header;
//	DWORD		mfgCmd;
	DWORD		action;
	DWORD		Error;   
    DWORD       RailID;
    DWORD       RailMode;
    DWORD       Volt;
    DWORD       PowerDownMode;  
} PACK mfg_PM_Cfg_t;

typedef struct mfg_LnaModeSetting_t {
   	PkHeader	header;
//	DWORD		mfgCmd;
	DWORD		action;
	DWORD		Error;   
    DWORD       extMode;
    DWORD       LnaPolority;
    DWORD       Bbu_Hi;
    DWORD       Bbu_Lo;
    DWORD       Bbu_GainOffset;  
    DWORD       Rfu_GrBb;
    DWORD       Rfu_GrLpf;  
} PACK mfg_LnaModeSetting_t;

typedef struct mfg_CmdTxRxIq_t
{
   	PkHeader	header;
//	uint32		mfgCmd;
	DWORD 		action;
	DWORD		Error;
	DWORD		extMode; 
	DWORD		Alpha;
	DWORD		Beta;	 
#if defined (_MIMO_)
	DWORD 		deviceId; 
	DWORD 		pathId; 
#endif // #if defined (_MIMO_)
}	PACK mfg_CmdTxRxIq_t;


typedef struct mfg_CmdTxRxCal_t
{
   	PkHeader	header;
//	uint32		mfgCmd;
	DWORD 		action;
	DWORD		Error;
	DWORD		Avg;
	DWORD		TxIqAlpha;
	DWORD		TxIqBeta;	 
	DWORD		RxIqAlpha;
	DWORD		RxIqBeta;	 
}	PACK mfg_CmdTxRxCal_t;

#define ARM_ULONG_ARRAY_SIZE 512

typedef struct {
    PkHeader    header;
    DWORD       action;
    DWORD       Error;
#if defined (_MIMO_)
	DWORD 		DeviceId; 
#endif // #if defined (_MIMO_)
    DWORD       armAddr;
    DWORD       sizeBytes;
    BYTE        buf[ARM_ULONG_ARRAY_SIZE];
}	PACK mfg_CmdArmReadUlongArray_t;


typedef struct mfg_CmdBbpSqDiag_t{
    PkHeader    header;
    DWORD       action;
    DWORD       Error;
#if defined (_MIMO_)
	DWORD 		deviceId; 
#endif // #if defined (_MIMO_)
    DWORD       sizeBytes;
    DWORD       sqStartAddr;
    DWORD       trigTime;
} PACK mfg_CmdBbpSqDiag_t;


typedef struct mfg_CmdCloseLoopCal
{
   	PkHeader	header;
//    DWORD 		mfgCmd;
    DWORD 		action;
    DWORD		Error;
	DWORD	    targetInitP;
	DWORD	    maxThreshHold;
	DWORD	    steps;
	DWORD	    dataRate;
	DWORD	    initP;
	DWORD	    threshHi;
	DWORD	    threshLow;
} PACK mfg_CmdCloseLoopCal;

typedef struct mfg_CmdRxLnaSettings_t
{
   	PkHeader	header;
	DWORD		action;
	DWORD		Error;    
	DWORD		BandId;
	DWORD		SubBandId;
	DWORD		Bbu_RxGainHi;
	DWORD		Bbu_RxGainLo;
	DWORD		Bbu_RxGainOffset;
	DWORD		Rf_RxGainIf;
	DWORD		Rf_RxGainBb;
	DWORD 	deviceId; 
	DWORD	pathId;
}	PACK mfg_CmdRxLnaSettings_t; 

enum{ FIELD_INIP_SIGNALB=1, FIELD_INIP_SIGNALG, 
	  FIELD_INIPCOMMON_SIGNALB,  FIELD_INIPCOMMON_SIGNALG, 
FIELD_DETTH_SIGNALB, FIELD_DETTH_SIGNALG, 
FIELD_ACCUM, FIELD_VGAMAX,  
FIELD_ACCUM_INITVALUE};


typedef struct mfg_CmdRfPwrFields_t
{
   	PkHeader	header;
//	DWORD		mfgCmd;
	DWORD 		action;
	DWORD		Error;   
	DWORD		fieldType;	// 1 initP, 2 DetThs, 3 DacX
	DWORD		value1;		// 1 initP, 2 DetTh_hi, 3 DacX
	DWORD		value2;		// 1 Notinuse, 2 DetTh_lo, 3 Notinuse
	DWORD		pathId;		// in case of initPCommon, it is BandId
	DWORD		deviceId; 
}	PACK mfg_CmdRfPwrFields_t;

// mfg_CmdE2PType_t
typedef struct mfg_CmdE2PType_t
{
   	PkHeader	header;
//	DWORD		mfgCmd;
	DWORD 		action;
	DWORD		Error;   
	DWORD		IfType;
	DWORD AddrWidth;
	DWORD DeviceType;
}	PACK mfg_CmdE2PType_t;

// mfg_CmdFEM_t
typedef struct mfg_CmdFEM_t
{
   	PkHeader	header;
//	DWORD		mfgCmd;
	DWORD 		action;
	DWORD		Error;   
	DWORD		FE_VER;
	DWORD		Capability;
	DWORD		CONCURRENT_CAP_2G_BT;
	DWORD		CONCURRENT_CAP_5G_BT;
	DWORD		Front_End_2G[9];
	DWORD		Front_End_5G[9];
	DWORD		AntDiversityInfo;
}	PACK mfg_CmdFEM_t;
//MFG_CMD_CALDATA_EXT
typedef struct mfg_CmdCalDataExt_t
{ 
   	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	DWORD       caldatastatus;
	DWORD		calDataLen;
	BYTE		calDataExt[0];
} PACK mfg_CmdCalDataExt_t;

//MFG_CMD_DPD_INFO
#define HAL_DPD_MAX_NUM_COEFF    18

typedef struct mfg_CmdDPDInfo_t
{
   	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	DWORD		PowerID;
	DWORD		Memory2Row;
	DWORD		pDPDTbl[HAL_DPD_MAX_NUM_COEFF];
	DWORD		deviceId;
} PACK mfg_CmdDPDInfo_t;

#endif  //#ifndef _MFG_CMD_
