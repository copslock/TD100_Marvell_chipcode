/** @file DutSharedCalTxt.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#include "../../DutCommon/Clss_os.h"
#include "../../DutCommon/utilities_os.h"
#include "../../DutCommon/utilities.h"
#include "../../DutCommon/mathUtility.h"

#if defined(_WLAN_)
#include DUTCLASS_WLAN_H_PATH	//"../DutWlanApi878XDll/DutWlanApiClss.h"
#endif	// #if defined(_WLAN_)

#include DUTCLASS_WLAN_HD_PATH	//"../DutWlanApi878XDll/DutWlanApiClss.hd"

#include DUTCLASS_SHARED_H_PATH	//"../DutShareApi878XDll/DutSharedClss.h"


#define TAG_STRUCTINFO			"StructureInfo"
#define KEY_STRUCTURE_REV		"STRUCTURE_REV"

#define TAG_MAIN				"Main_Table"
#define KEY_DESIGN_TYPE			"Ref_Design_Type"
#define KEY_MAJOR_VERSION		"Ref_Design_Ver_Major"
#define KEY_MINOR_VERSION		"Ref_Design_Ver_Minor"
#define KEY_DESIGN_VARIATION	"Ref_Design_Variation"

#define KEY_SN					"SN"
#define KEY_ASSEMBLY_HOUSE		"Assembly_house"

#define KEY_SPI_SIZE			"SPI_SIZE"
#define VAL_SPI_SIZE_DEF		"32K"

#define KEY_SOC_VERSION			"SOC_VERSION"
#define KEY_SOC_ID				"SOC_ID"
#define KEY_RF_VERSION			"RF_VERSION" 
#define KEY_RF_ID				"RF_ID"
#define KEY_BBU_VERSION			"BBU_VERSION" 
#define KEY_BBU_ID				"BBU_ID"

#define KEY_TEST_VERSION		"TEST_VERSION"
#define	KEY_MFG_VERSION			"MFG_VERSION"
#define KEY_DLL_VERSION			"DLL_VERSION"

#define KEY_REGION_CODE			"REGION_CODE"

#define KEY_FE_VERSION          "FE_VER"
//FEM capability
#define KEY_FEM_CAP_2G				"FEMCap2G"
#define KEY_FEM_CAP_5G				"FEMCap5G"
#define KEY_FEM_INTERNAL_BT			"FEM_INTERNAL_BT"
#define KEY_FEM_EXTERNAL_2G_LNA		"FEM_EXTERNAL_2G_LNA"
#define KEY_FEM_EXTERNAL_5G_LNA		"FEM_EXTERNAL_5G_LNA"
#define KEY_FEM_ANT_DIV				"FEM_ANT_DIV"
#define KEY_FEM_SAMEANT_2G_BT		"FEM_SAMEANT_2G_BT"
#define KEY_FEM_SAMEANT_5G_BT		"FEM_SAMEANT_5G_BT"

////Concurrency
#define KEY_CONCURRENT_CAP_2G_BT	"CONCURRENT_CAP_2G_BT"
#define KEY_CONCURRENT_CAP_5G_BT	"CONCURRENT_CAP_5G_BT"
// misc flags
#define KEY_MISC_FLAG_CRYSTAL		"ExtXtalSource" 
#define KEY_MISC_FLAG_SLEEPCLOCK	"ExtSleepClk" 
#define KEY_MISC_FLAG_WLANWAKEUP	"WlanWakeUp" 

#define KEY_FEMID_FORMAT			"FemConfId_Dev%d" 
#define KEY_LNA_GAIN				"LnaGainDelta" 
#define KEY_ANT_DIV_INFO			"AntDiversityInfo"

#define KEY_EXTPA_POLAR_G_FORMAT		"ExtPa_Dev%d_G"
#define KEY_EXTPA_POLAR_A_FORMAT		"ExtPa_Dev%d_A" 

#define KEY_EXTLNA_G_FORMAT			"ExtLna_Dev%d_G"
#define KEY_EXTLNA_A_FORMAT			"ExtLna_Dev%d_A"

#define KEY_EXTLNA_POLORITY_G_FORMAT	"ExtLnaPolority_Dev%d_G"
#define KEY_EXTLNA_POLORITY_A_FORMAT	"ExtLnaPolority_Dev%d_A"

#define KEY_ANT_TX_FORMAT			"Ant_Dev%d_TX"
#define KEY_ANT_RX_FORMAT			"Ant_Dev%d_RX" 

#define KEY_ANT_ID_FORMAT			"AntId_Dev%d_Path%d" 
  

#define KEY_BT_COEXISTENCE		"BT_CO_EXISTENCE"
#define KEY_APPMODEFALG			"AppicationModeFalg"
#define KEY_SOC_OR_REV			"Soc_OR_Rev"
#define KEY_RFU_OR_REV			"Rfu_OR_Rev"
#define KEY_MIMO_CONFIG_CNTTX	"Mimo_TxCount"
#define KEY_MIMO_CONFIG_CNTRX	"Mimo_RxCount"
#define KEY_MIMO_CONFIG_MAP_FORMAT	"MimoMap_Dev%d_Path%d" 

//Front-end
#define KEY_FRONT_END_CONFIG_2G "Front_End_2G_Byte_%d"
#define KEY_FRONT_END_CONFIG_5G "Front_End_5G_Byte_%d"

#if defined (_MIMO_)
#define TAG_ANNEX14_FORMAT				"Cal_Data_Tx_BandB_Dev%d_Path%d"
#define TAG_ANNEX15_FORMAT				"Cal_Data_Tx_BandA_Dev%d_Path%d"
#define TAG_ANNEX50_FORMAT				"Cal_Data_Tx_BandA_Dev%d_Path%d_SubBand%d"
#define TAG_ANNEX37_FORMAT				"Cal_Data_Tx_BandG_Dev%d_Path%d"
#else //#if defined (_MIMO_)
#define TAG_ANNEX14				"Cal_Data_Tx_BandB"
#define TAG_ANNEX15				"Cal_Data_Tx_BandA"
#define TAG_ANNEX37				"Cal_Data_Tx_BandG"
#endif //#if defined (_MIMO_)
#define KEY_RFXTAL						"RFXTAL"
#define KEY_CAL_OPTION_RANGE_FORMAT		"Cal_Option_Range%d"
#define KEY_CAL_OPTION_METHOD			"Cal_Method"

#define KEY_THERMALSCALER				"Thermal_Scaler"
#define KEY_INITPCOMMON					"InitP_Common"
#define KEY_NO_CH_SAVED					"NumChSaved"		//2.4G
#define KEY_NO_SUBBAND_SAVED			"NumSubBandSaved"	//5G
#define KEY_NO_CH_SUBBAND_FORMAT		"NumChSavedSubBand%d"	//5G

#define KEY_LCTANK_FLAG					"LC_Tank_Flag"		//2.4G
#define KEY_CHANNEL_CALED_FORMAT		"Channel_Caled%d"
#define KEY_LCCAL_FORMAT				"LcCal_Channel%d"				//2.4G
#define KEY_RANGE_INITP_FORMAT			"CorrInitP_BandB_Channel%d_Range%d"		// 2.4G
#define KEY_RANGE_CORRECTION_FORMAT		"CorrDetTh_BandB_Channel%d_Range%d"
#define KEY_RANGE_INITP_G_FORMAT		"CorrInitP_BandG_Channel%d_Range%d" 
#define KEY_RANGE_CORRECTION_G_FORMAT	"CorrDetTh_BandG_Channel%d_Range%d" 
#define KEY_RANGE_INITP_A_FORMAT		"CorrInitP_BandA_Channel%d_Range%d" 
#define KEY_RANGE_CORRECTION_A_FORMAT	"CorrDetTh_BandA_Channel%d_Range%d"

#define KEY_TXIQMISMATCH_PHASE_FORMAT	"IQ_Mismatch_phase_SubBand%d_Bw%d"
#define KEY_TXIQMISMATCH_AMP_FORMAT		"IQ_Mismatch_amplitude_SubBand%d_Bw%d"

#if defined (_MIMO_)
#define TAG_ANNEX17_FORMAT				"RxLNASetting_Dev%d_Path%d"
#else //#if defined (_MIMO_)
#define TAG_ANNEX17						"RxLNASetting"
#endif //#if defined (_MIMO_)

#define KEY_USE_BBU_LNASETTING			"UseBbuSetting"
#define KEY_BBU_LNA_HI_FORMAT			"Bbu_LnaHi_Band%d_SubBand%d"
#define KEY_BBU_LNA_LO_FORMAT			"Bbu_LnaLo_Band%d_SubBand%d"
#define KEY_BBU_LNA_GAINOFFSET_FORMAT	"Bbu_LnaGainOffset_Band%d_SubBand%d"
#define KEY_RF_LNA_GRBB_FORMAT			"Rf_LnaGrbb_Band%d_SubBand%d"
#define KEY_RF_LNA_GRIF_FORMAT			"Rf_LnaGrif_Band%d_SubBand%d"

#if defined (_MIMO_)
#define TAG_ANNEX19_FORMAT				"MimoCal_Rx_BandG_Dev%d"
#define TAG_ANNEX21_FORMAT				"MimoCal_Rx_BandA_Dev%d"
#define KEY_RXIQMISMATCH_PHASE_FORMAT	"IQ_Mismatch_phase_SubBand%d_Bw%d_Path%d"
#define KEY_RXIQMISMATCH_AMP_FORMAT		"IQ_Mismatch_amplitude_SubBand%d_Bw%d_Path%d"
#define KEY_RXGAINMISMATCH_FORMAT		"RxGain_Mismatch_SubBand%d_Path%d"
#else //#if defined (_MIMO_)
#define TAG_ANNEX18						"MimoCal_Rx_BandG"
#define TAG_ANNEX20						"MimoCal_Rx_BandA"
#define KEY_RXIQMISMATCH_PHASE_FORMAT	"IQ_Mismatch_phase_SubBand%d_Bw%d"
#define KEY_RXIQMISMATCH_AMP_FORMAT		"IQ_Mismatch_amplitude_SubBand%d_Bw%d"
#endif //#if defined (_MIMO_)

#define TAG_ANNEX22				"RfSettingPairs"
#define TAG_ANNEX23				"Bbu8SettingPairs"
#define TAG_ANNEX24				"Bbu16SettingPairs"
#define TAG_ANNEX25				"Soc32SettingPairs"

#define KEY_SETPAIRS_ADDRESS_FORMAT		"Address%d"
#define KEY_SETPAIRS_VALUE_FORMAT		"Value%d"

#define TAG_ANNEX27				"Led"
#define KEY_LED_FORMAT			"Led%d"
#define TAG_ANNEX28				"USB_String"
#define KEY_VENDORSTRING		"VendorString"
#define KEY_PRODUCTSTRING		"ProductString"

#define TAG_ANNEX29			"HwSupportBands"
#define KEY_BAND_2_4G		"HwSupport_Band2_4G"
#define KEY_BAND_4_9G		"HwSupport_Band4_9G"
#define KEY_BAND_5_0G		"HwSupport_Band5_0G"
#define KEY_BAND_5_2G		"HwSupport_Band5_2G"

#define TAG_ANNEX30_FORMAT				"LoSpurs_Dev%d_Path%d"
#define KEY_LOSPURS_SETTING0			"LoSpurs_Band%d_SubBand%d_Setting0"
#define KEY_LOSPURS_SETTING1			"LoSpurs_Band%d_SubBand%d_Setting1"
 
#if defined (_MIMO_)
#define TAG_ANNEX31_FORMAT				"Rssi_G_Dev%d"
#define TAG_ANNEX32_FORMAT				"Rssi_A_Dev%d"
#define TAG_ANNEX47_FORMAT				"Rssi_G_Dev%d_Path%d"
#define TAG_ANNEX48_FORMAT				"Rssi_A_Dev%d_Path%d"
#else // #if defined (_MIMO_)
#define TAG_ANNEX31						"Rssi_G"
#define TAG_ANNEX32						"Rssi_A"
#define TAG_ANNEX47_FORMAT				"Rssi_G_Path%d"
#define TAG_ANNEX48_FORMAT				"Rssi_A_Path%d"
#endif // #if defined (_MIMO_)

//#define KEY_RSSI_NF_FORMAT					"Offset_NF_SubBand%d"
//#define KEY_RSSI_CCK_FORMAT					"Offset_CCK_SubBand%d"
//#define KEY_RSSI_OFMD_FORMAT				"Offset_OFDM_SubBand%d"
//#define KEY_RSSI_MCS_FORMAT					"Offset_MCS_SubBand%d"

#define KEY_RSSI_CCK_BYPASS_FORMAT		"OffsetCCK_ByPass_SubBand%d"
#define KEY_RSSI_CCK_NORMAL_FORMAT		"OffsetCCK_Normal_SubBand%d"
#define KEY_RSSI_OFDM_BYPASS_FORMAT		"OffsetOFDM_ByPass_SubBand%d"
#define KEY_RSSI_OFDM_NORMAL_FORMAT		"OffsetOFDM_Normal_SubBand%d"

#define TAG_ANNEX33_FORMAT				"GBand_20MHz_Dev%d"
#define TAG_ANNEX34_FORMAT				"GBand_40MHz_Dev%d"
#define TAG_ANNEX35_FORMAT				"ABand_20MHz_Dev%d"
#define TAG_ANNEX36_FORMAT				"ABand_40MHz_Dev%d"
#define	KEY_RATE1						"Rate1"
#define KEY_RATE2 						"Rate2"
#define KEY_RATE3						"Rate3"
#define KEY_RATE4						"Rate4"
#define	KEY_CH							"ChNo"
#define	KEY_CH_LOW						"LowerSubChNo"


#define TAG_ANNEX43_CAL_FORMAT			"BT_CalData"
#define TAG_ANNEX44_CONFIG_FORMAT		"BT_Config"

#define	KEY_PPAGAININ6DBMODE			"Gain_Ppa6dBStepMode"
#define KEY_LIMITPWR2C2					"LimitTxPower2C2Level"

#define	KEY_PWRINDBM_C2_FORMAT			"PwrIndBm_C2_Pwr%d"
#define	KEY_GAIN_PPA_C2_FORMAT			"GainPpa_C2_Pwr%d"
#define	KEY_GAIN_PGA_C2_FORMAT			"GainPga_C2_Pwr%d"
#define	KEY_GAIN_BB_C2_FORMAT			"GainBb_C2_Pwr%d"

#define	KEY_PWRINDBM_C1_5_FORMAT		"PwrIndBm_C1_5_Pwr%d"
#define	KEY_GAIN_PPA_C1_5_FORMAT		"GainPpa_C1_5_Pwr%d"
#define	KEY_GAIN_PGA_C1_5_FORMAT		"GainPga_C1_5_Pwr%d"
#define	KEY_GAIN_BB_C1_5_FORMAT			"GainBb_C1_5_Pwr%d" 
 
#define	KEY_GAINTEMPSLOPE				"TempSlope_Gain" 
#define	KEY_REFERENCE_TEMP_RDBK			"TempRd_Ref"
#define	KEY_FE_LOSS				"FELoss"
#define	KEY_XTAL				"Xtal"
#define	KEY_PWRUP_PWRLVL_DBM	"PwrUp_PwrLvl"
 
#define	KEY_CONFIG_MINTXPWR_NORSSI_DBM	"MaxPowerdBm_NoRssi"
#define	KEY_CAL_RSSI_CORR				"RssiCorrection"

#define KEY_CONFIG_LOCALVERSION			"LocalVersion"
#define KEY_CONFIG_REMOTEVERSION		"RemoteVersion"
#define	KEY_CONFIG_DEEPSLEEPSTATE		"DeepSleepState"
#define	KEY_CONFIG_USEENCMODEBRCST		"UseEncModeBrcst"
#define	KEY_CONFIG_USEUNITKEY			"UseUnitKey"
#define	KEY_CONFIG_ONCHIPPROFILE_ENABLE "OnChipProfileEnable"
#define	KEY_CONFIG_JITTER				"DeviceJitter"
#define	KEY_CONFIG_DRIFT				"DeviceDrift"
#define	KEY_CONFIG_ENCKEYMAX			"EncKeyLmax"
#define	KEY_CONFIG_ENCKEYMIN			"EncKeyLmin"
#define	KEY_CONFIG_MAXACLS				"MaxAcls"
#define	KEY_CONFIG_MAXSCOS				"MaxScos"
#define	KEY_CONFIG_MAXACLPKTLEN			"MaxAclPktLen"
#define	KEY_CONFIG_MAXACLPKTS			"MaxAclPkts"
#define	KEY_CONFIG_MAXSCOPKTLEN			"MaxScoPktLen"
#define	KEY_CONFIG_MAXSCOPKTS			"MaxScoPkts"
#define	KEY_CONFIG_CONNRXWINDOW			"ConnRxWnd"
#define	KEY_CONFIG_RSSIGOLDENRANGE_LOW		"RssiGoldenRangeLow"
#define	KEY_CONFIG_RSSIGOLDENRANGE_HIGH		"RssiGoldenRangeHigh"
 
#define	KEY_CONFIG_COMPATIBLEMODE						"CompatibleMode"
#define	KEY_CONFIG_LMP_PWRREQMDGINTERVAL				"LMP_PwrReqMsgInterval"
#define	KEY_CONFIG_LMP_DECRPWRREQMDGINTERVALRATIO		"LMP_DecrPwrReqMsgIntervalRatio"
#define	KEY_CONFIG_NUMOFPWRSTEP4CLASS1P5				"NumOfPwrStep4Clss1p5"
#define	KEY_CONFIG_PWRSTEPSIZE4CLASS1P5					"PwrStepSize4Clss1p5"
#define	KEY_CONFIG_PWRCTRLINCLASS2DEVICE				"PwrCtrlInClss2Device"
#define	KEY_CONFIG_DYMANICRSSILMPTIMEINTERVAL			"DynamicRssiLmpTimeInterval"
#define	KEY_CONFIG_DYMANICRSSILMPTIMEINTERVALTHRESHOLD	"DynamicRssiLmpTimeIntervalThrhld"



#define	KEY_CONFIG_LOCAL_SUPP_CMDS_FORMAT			"LocalSupportedCmds_%d"
#define	KEY_CONFIG_LOCAL_SUPP_FEATS_FORMAT			"LocalSupportedFeatures_%d"
#define	KEY_CONFIG_BDADDRESS			"BdAddress" 
#define	KEY_UART_BAUDRATE				"UartBaudRate" 
#define	KEY_LOCALDEVICENAME				"LocalDeviceName" 

#define	KEY_CONFIG_MASK			"Mask"

#ifndef _W8780_
#define	KEY_CONFIG_MASK_BDADDR_FORMAT			"BtAddrList_%d"
#define	KEY_CONFIG_MASK_LINKKEY_FORMAT			"BtKeyList_%d"
#define	KEY_CONFIG_MASK_SECURITYKEY_FORMAT		"SecurityKey_%d"
#endif //#ifndef _W8780_

#define TAG_ANNEX45								"SleepMode_Config"
#define	KEY_CONFIG_PINOUT_DRVLOW_LOW			"PinOut_DrvLow_Low32"
#define	KEY_CONFIG_PINOUT_DRVLOW_HIGH			"PinOut_DrvLow_High32"
#define	KEY_CONFIG_PININ_PULLDISABLE_LOW			"PinIn_PullDisable_Low32"
#define	KEY_CONFIG_PININ_PULLDISABLE_HIGH			"PinIn_PullDisable_High32"

#define	KEY_FLAG_GPIOCTRLXOSC	"Flag_XoscCtrlByGPIO"
#define	KEY_FLAG_XOSCSETTLING	"XoscSettling"

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
// ANNEX 40 PM PW886
//////////////////////////////////////////////////////////////////

#define TAG_ANNEX40						"PM_PW88x"
#define	KEY_VID							"PM_VID"
#define KEY_PID 						"PM_PID"
#define KEY_CHIPID						"PM_ChipId"
#define KEY_CHIPVERSION					"PM_ChipVersion"
#define	KEY_SETTINGROLLBACK				"PM_UsePrevChipRevSetting"
#define	KEY_RAIL_AVAIL_IN_CHIP			"PM_RailsAvailableInChip"
#define	KEY_PWRDN_BYTE					"PM_PowerDownModeFlagByte"
#define	KEY_NORMALMODE_VOLT_FORMAT		"PM_NormalModeSetting_VoltIn50mv_Rail%d"
#define	KEY_SLEEPMODE_FLAG_FORMAT		"PM_SleepModeFlag_Rail%d"
#define	KEY_SLEEPMODE_VOLT_FORMAT		"PM_SleepModeSetting_VoltIn50mv_Rail%d"
#define	KEY_DPDLP_BYTE					"PM_DeepSleepFlagByte"
//////////////////////////////////////////////////////////////////
// ANNEX 40 PM PW886
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////



#define TAG_ANNEX42								"THERMAL_CAL_PARAM"
#define KEY_THERMAL_CAL_NO_SUBBAND				"THERMAL_CAL_NO_SUBBAND"
#define KEY_THERMAL_CAL_BAND_FORMAT				"THERMAL_CAL_BAND%d"
#define KEY_THERMAL_CAL_SUBBAND_FORMAT			"THERMAL_CAL_SUBBAND%d"
#define KEY_THERMAL_CAL_TEMPREF_MINUS40C_FORMAT "THERMAL_CAL_TEMPREF_MINUS40C%d"
#define KEY_THERMAL_CAL_TEMPREF_0C_FORMAT		"THERMAL_CAL_TEMPREF_0C%d"
#define KEY_THERMAL_CAL_TEMPREF_65C_FORMAT		"THERMAL_CAL_TEMPREF_65C%d"
#define KEY_THERMAL_CAL_TEMPREF_85C_FORMAT		"THERMAL_CAL_TEMPREF_85C%d"
#define KEY_THERMAL_CAL_PPA_COEFF_FORMAT		"THERMAL_CAL_PPA_COEFF%d"
#define KEY_THERMAL_CAL_PA_COEFF_FORMAT			"THERMAL_CAL_PA_COEFF%d"
#define KEY_THERMAL_CAL_DENOM_FORMAT			"THERMAL_CAL_DENOM%d"
#define KEY_THERMAL_CAL_PTARGETDELTA_FORMAT		"THERMAL_CAL_TARGETDELTA%d"
#define KEY_THERMAL_CAL_NUMERATOR_FORMAT		"THERMAL_CAL_NUMERATOR%d"
#define KEY_THERMAL_CAL_NUMERATOR1_FORMAT		"THERMAL_CAL_NUMERATOR1%d"

//Anned 42 VD
#define KEY_THERMAL_CAL_NO_SUBBAND_G			"NO_OF_THERMAL_INF0_BAND_0"
#define KEY_THERMAL_CAL_NO_SUBBAND_A			"NO_OF_THERMAL_INF0_BAND_1"
#define KEY_THERMAL_CAL_RSSICALNUM_NORMAL		"BAND_%d_SUBBAND_%d_RSSICALNUM_NORMAL"
#define KEY_THERMAL_CAL_RSSICALNUM_BYPASS		"BAND_%d_SUBBAND_%d_RSSICALNUM_BYPASS"
#define KEY_THERMAL_CAL_TEMPREF_SLOPE_FORMAT	"BAND_%d_SUBBAND_%d_TEMPREF_SLOPE"
#define KEY_THERMAL_CAL_PTARGET_TEMP            "BAND_%d_SUBBAND_%d_PTARGET_TEMP"
#define KEY_THERMAL_CAL_SLOPENUM				"BAND_%d_SUBBAND_%d_SLOPENUM"
#define KEY_THERMAL_CAL_SLOPENUM2				"BAND_%d_SUBBAND_%d_SLOPENUM2"
#define KEY_THERMAL_CAL_SLOPEDENOM				"BAND_%d_SUBBAND_%d_SLOPEDENOM"
#define KEY_THERMAL_CAL_CALINTERVALINMS			"BAND_%d_SUBBAND_%d_CALINTERVALINMS"
#define KEY_THERMAL_CAL_PPACOEFF				"BAND_%d_SUBBAND_%d_PPACOEFF"
#define KEY_THERMAL_CAL_PACOEFF					"BAND_%d_SUBBAND_%d_PACOEFF"
#define KEY_THERMAL_CAL_XTAL					"BAND_%d_SUBBAND_%d_XTAL"


//Annex49
#define KEY_NO_POWER_LEVEL_SAVED        "NumPowerLevelSaved"
#define KEY_CLOSE_TX_PW_DET 			"CloseTXPWDetection"
#define KEY_EXT_PA_POLARIZE             "ExtPAPolarize"
#define KEY_PDET_HI_LO_GAP				"PDetHiLoGap"
#define KEY_MCS20M_PDET_OFFSET			"PDet_20MHZ_Offset"
#define KEY_MCS20M_INIT_OFFSET			"Init_20MHZ_Offset"
#define KEY_MCS40M_PDET_OFFSET			"PDet_40MHZ_Offset"
#define KEY_MCS40M_INIT_OFFSET			"Init_40MHZ_Offset"
#define KEY_HI_FREQ						"High_Freq_Bound"
#define KEY_LO_FREQ						"High_Freq_Bound"

#define TAG_ANNEX50_FORMAT				"Cal_Data_Tx_BandA_Dev%d_Path%d_SubBand%d"

//Annex50
#define KEY_NO_POWER_LEVEL_SAVED_A			"NumPowerLevelSaved"
#define KEY_CLOSE_TX_PW_DET_A				"CloseTXPWDetection_SubBand%d"
#define KEY_PDET_HI_LO_GAP_A				"PDetHiLoGap_SubBand%d"
#define KEY_MCS20M_PDET_OFFSET_A			"PDet_20MHZ_Offset_SubBand%d"
#define KEY_MCS20M_INIT_OFFSET_A			"Init_20MHZ_Offset_SubBand%d"
#define KEY_MCS40M_PDET_OFFSET_A			"PDet_40MHZ_Offset_SubBand%d"
#define KEY_MCS40M_INIT_OFFSET_A			"Init_40MHZ_Offset_SubBand%d"
#define KEY_HI_FREQ_A						"High_Freq_Bound_SubBand%d"
#define KEY_LO_FREQ_A						"High_Freq_Bound_SubBand%d"

#define TAG_ANNEX52						"LO_LEAKAGE"
#define KEY_LO_LEAKAGE_5G_I_FORMAT		"LO_LEAKAGE_CAL_5G_SUBBAND%d_I_Path%d"
#define KEY_LO_LEAKAGE_5G_Q_FORMAT		"LO_LEAKAGE_CAL_5G_SUBBAND%d_Q_Path%d" 
#define KEY_LO_LEAKAGE_2G_I_FORMAT		"LO_LEAKAGE_CAL_2G_I_Path%d"
#define KEY_LO_LEAKAGE_2G_Q_FORMAT		"LO_LEAKAGE_CAL_2G_Q_Path%d"
#define KEY_CAL_5G_FLAG					"Compatible_5G"
#define KEY_CAL_2G_FLAG					"Compatible_2G"
#define TAG_ANNEX53						"TX_IQ_CAL"//JKLCHEN
#define KEY_TXIQ_LEAKAGE_5G_ALPHA_FORMAT	"TXIQ_CAL_5G_SUBBAND%d_ALPHA_Path%d"
#define KEY_TXIQ_LEAKAGE_5G_BETA_FORMAT		"TXIQ_CAL_5G_SUBBAND%d_BETA_Path%d" 
#define KEY_TXIQ_LEAKAGE_2G_ALPHA_FORMAT	"TXIQ_CAL_2G_ALPLA_Path%d"
#define KEY_TXIQ_LEAKAGE_2G_BETA_FORMAT		"TXIQ_CAL_2G_BETA_Path%d"

#define TAG_ANNEX54						"RX_IQ_CAL"
#define KEY_RXIQ_LEAKAGE_5G_ALPHA_FORMAT	"RXIQ_CAL_5G_SUBBAND%d_ALPHA_Path%d"
#define KEY_RXIQ_LEAKAGE_5G_BETA_FORMAT		"RXIQ_CAL_5G_SUBBAND%d_BETA_Path%d" 
#define KEY_RXIQ_LEAKAGE_2G_ALPHA_FORMAT	"RXIQ_CAL_2G_ALPLA_Path%d"
#define KEY_RXIQ_LEAKAGE_2G_BETA_FORMAT		"RXIQ_CAL_2G_BETA_Path%d"

#define TAG_ANNEX55							"BT_Config"
#define KEY_BT_CONFIG_VERSION						"Version"
#define	KEY_BT_CONFIG_XTAL							"Xtal"
#define	KEY_BT_INITPWRINDBM							"InitPwrIndBm_Pwr"
#define	KEY_BT_FE_LOSS								"FELoss"
#define KEY_BT_FORCECLASS20							"ForceClass2Op"
#define KEY_BT_DISABLE_PWR_CONTROL					"DisablePwrControl"
#define KEY_BT_MISC_FLAG							"MiscFlag"
#define KEY_BT_USED_INTERNAL_SLEEP_CLOCK			"UsedInternalSleepClock"
#define	KEY_BT_CONFIG_RSSIGOLDENRANGE_LOW			"RssiGoldenRangeLow"
#define	KEY_BT_CONFIG_RSSIGOLDENRANGE_HIGH			"RssiGoldenRangeHigh"
#define	KEY_BT_CONFIG_BDADDRESS						"BdAddress"
#define KEY_BT_CONFIG_MIN_ENCRYP_KEY_LEN			"MinEncrKeyLen"
#define KEY_BT_CONFIG_MAX_ENCRYP_KEY_LEN			"MaxEncrKeyLen"

#define TAG_ANNEX57							"DPD_Data_BAND%d_SUBBAND%d"
#define KEY_DPD_MEMORY_ROW					"MemoryRow"
#define KEY_DPD_NUMBER_ENTERIRS				"Number_Of_Enteries"
#define KEY_DPD_POWER_ID					"DPD_Power_Index%d"
#define KEY_DPD_POWER_CODE					"Power_Code_Index%d"
#define KEY_DPD_START_CHAN					"Start_Chan_%d"	
#define KEY_DPD_END_CHAN					"End_Chan_%d"	

#define KEY_DPD_REAL_HI						"RealHi_Index%d_%d"
#define KEY_DPD_REAL_LO						"RealLo_Index%d_%d"
#define KEY_DPD_IMAGE_HI					"ImageHi_Index%d_%d"
#define KEY_DPD_IMAGE_LO					"ImageLo_Index%d_%d"

#define	KEY_BT_CONFIG_MASK_SECURITYKEY_FORMAT		"SecurityKey_%d"
#define KEY_BT_CONFIG_LINK_KEY_MASK					"LinkKeyMask"
#define	KEY_BT_CONFIG_MASK_BDADDR_FORMAT			"BtAddrList_%d"

 
// bool g_writeTemplate=0; 

#include "DutSharedCalFileAccess.cpp"

#if defined (_WLAN_)

#ifndef _LINUX_

int DutSharedClssHeader GetPwrTbleFileName(
#if defined (_MIMO_)
						int DeviceId, int PathId, 
#endif // #if defined (_MIMO_)
						int BandId, int SubBandId,  
						int Upload, char* FileName)
{
 	char	line[255]="";
	char *pUpLoad[3]={"", "_Upload", "_Swap"};
#if !defined (_MIMO_)
	int DeviceId=0, PathId=0; 
#endif // #if defined (_MIMO_)

	char temp[_MAX_PATH]="";
	
	if(getcwd(temp, _MAX_PATH) != NULL)  
		strcat(temp, "\\setup.ini");
	else
		strcpy(temp, "setup.ini");

 //	GetHWVersion(	&socId, &socVersion, &RfId, &RfVersion, 
 //					&BbId,	&BbVersion, deviceId);

 
 
	{		
 		sprintf(line, "W8XXX_PwrTbl_Dev%d_Path%c_%c_%d%s", 
			DeviceId, PathName[PathId], BandName[BandId],	SubBandId, 
					pUpLoad[Upload]);
		strcpy( FileName,line); 
		strcat(FileName, ".txt");
  
		GetPrivateProfileString("SHPwrTbl", line, FileName, FileName,
				255, temp); 
	}	

	return 0;
}
#endif	//#ifndef _LINUX_

int DutSharedClssHeader WritePwrTableFile(
#if defined (_MIMO_)
						int DeviceId, int PathId, 
#endif // #if defined (_MIMO_)
						int BandId, int SubBandId,  
						int HighFreqBondary, int LowFreqBondary, 
						int pPpaGain1_Rng[RFPWRRANGE_NUM], 
					   int pPpaGain2_Rng[RFPWRRANGE_NUM],  
					   int pMaxPwr_Rng[RFPWRRANGE_NUM], 
					   int pMinPwr_Rng[RFPWRRANGE_NUM], 
					   int *pSizeOfPwrTblInByte, BYTE* pPwrTbl, 
					   char* pGivenFileName) 
{ 	
	int		status=ERROR_NONE, i=0;
	FW_POWRTABLE table={0};
#if !defined (_MIMO_)
	int DeviceId=0, PathId=0; 
#endif // #if defined (_MIMO_)
	
	table.DeviceId = DeviceId;
	table.PathId = PathId;
	table.BandId = BandId;
	table.SubBandId = SubBandId;

	table.pwrTbl.HighFreqBound = HighFreqBondary;
	table.pwrTbl.LowFreqBound = LowFreqBondary;

	for (i=0; i<RFPWRRANGE_NUM; i++)
	{
		table.pwrTbl.rngTblLevel[i].PpaGain5_1 = pPpaGain1_Rng[i];
		table.pwrTbl.rngTblLevel[i].PpaGain5_2 = pPpaGain2_Rng[i];
		table.pwrTbl.rngTblLevel[i].TopPwr = pMaxPwr_Rng[i];
		table.pwrTbl.rngTblLevel[i].BtmPwr = pMinPwr_Rng[i];
	}
	
 	 
	memcpy(table.pwrTbl.pwrTblLevel, pPwrTbl,(*pSizeOfPwrTblInByte));


	i = sizeof(FW_POWRTABLE);
	status = WritePwrTableFile(
#if defined (_MIMO_)
	 DeviceId,  PathId,  
#endif // #if defined (_MIMO_)
					BandId,  SubBandId,   
					    &i,  (BYTE*)&table,  pGivenFileName) ; 
	return status;
}
int DutSharedClssHeader WritePwrTableFile(
#if defined (_MIMO_)
										  int DeviceId, int PathId, 
#endif // #if defined (_MIMO_)
										  int BandId, int SubBandId,  
					   int *pSizeOfTblInByte, BYTE* pPwrTbl, char* pGivenFileName)  
{
 	int		status=ERROR_NONE;
 	FILE	*hFile=NULL;
	int		cnt = 0, cntLine=0; 
	int		tempInt[5]={0};
	char	FileName[255]={"W8060_PwrTbl_UpLoad.txt"};
	char	line[255]="";
 	FW_POWRTABLE *pTbl=NULL;
 	int *pTempTblPtr=NULL;
#if !defined (_MIMO_)
	int DeviceId=0, PathId=0; 
#endif // #if defined (_MIMO_)

 	pTbl =(FW_POWRTABLE*)pPwrTbl; 
	
	if ( (DeviceId != pTbl->DeviceId) ||  
	(PathId != pTbl->PathId) || 
	(BandId != pTbl->BandId) || 
	(SubBandId != pTbl->SubBandId) )
		DebugLogRite("Ids Mismatch %d %d %d %d [%d %d %d %d]\n", 
		DeviceId, PathId, BandId, SubBandId,   
		pTbl->DeviceId, pTbl->PathId , pTbl->BandId , pTbl->SubBandId);


	if(NULL == pGivenFileName)
	{
		GetPwrTbleFileName(
#if defined (_MIMO_)
			DeviceId,	PathId, 
#endif // #if defined (_MIMO_)
			BandId, SubBandId,  1, FileName);
	}
 	else
		strcpy(FileName,pGivenFileName);  


	if(NULL == pGivenFileName)
		hFile = fopen(FileName, "w");
	else
		hFile = fopen(FileName, "a");

	if (NULL == hFile)
	{
		DebugLogRite("Cannot Find File \"%s\"\n", FileName);
		return ERROR_ERROR;
	}

	DebugLogRite("WritePwrTableFile to File \"%s\"\n", FileName);
   
   	fprintf(hFile, "; HighFreqBound\t\tLowFreqBound\n" ); 
 	fprintf(hFile, "\t%d\t\t%d\n",  
 			pTbl->pwrTbl.HighFreqBound,
 			pTbl->pwrTbl.LowFreqBound); 
 
   	fprintf(hFile, "; PpaGain5_1\tPpaGain5_2\tTopPwr\tBtmPwr\n" ); 

	for(cnt=0; cnt < RFPWRRANGE_NUM; cnt++)
	{
 		fprintf(hFile, "%x\t%x\t%d\t%d\n", 
 			pTbl->pwrTbl.rngTblLevel[cnt].PpaGain5_1,
 			pTbl->pwrTbl.rngTblLevel[cnt].PpaGain5_2,
 			pTbl->pwrTbl.rngTblLevel[cnt].TopPwr,
 			pTbl->pwrTbl.rngTblLevel[cnt].BtmPwr ); 
	}

   	fprintf(hFile, "; powerLevel\tValInitTgrt\tValPDetThHi\tValPDetThLo\tValDacX\n" ); 

	for(cnt=0; cnt < POWTBL_LEVEL; cnt++)
	{
 		fprintf(hFile, "%d\t%x\t%x\t%x\t%x\n", 
 			pTbl->pwrTbl.pwrTblLevel[cnt].powerLevel,
 			pTbl->pwrTbl.pwrTblLevel[cnt].ValInitTgrt,
 			pTbl->pwrTbl.pwrTblLevel[cnt].ValPDetThHi,
 			pTbl->pwrTbl.pwrTblLevel[cnt].ValPDetThLo,
#if defined(_CAL_REV_D_)
			pTbl->pwrTbl.pwrTblLevel[cnt].Reserve);
#else
			pTbl->pwrTbl.pwrTblLevel[cnt].ValDacX); 
#endif	//#if defined(_CAL_REV_D_)
	}
	 
 	fclose(hFile);
 
 	return status;
}		

int DutSharedClssHeader ReadPwrTableFile(
#if defined (_MIMO_)
						int DeviceId, int PathId, 
#endif // #if defined (_MIMO_)
						int BandId, int SubBandId,  
						int *pHighFreqBondary, int *pLowFreqBondary,
					   int pPpaGain1_Rng[RFPWRRANGE_NUM], 
					   int pPpaGain2_Rng[RFPWRRANGE_NUM],  
					   int pMaxPwr_Rng[RFPWRRANGE_NUM], 
					   int pMinPwr_Rng[RFPWRRANGE_NUM], 
					   int *pSizeOfPwrTblInByte, BYTE* pPwrTbl, 
					   char* pGivenFileName)
{
	int		status=ERROR_NONE, i=0;
	FW_POWRTABLE table={0};
#if !defined (_MIMO_)
	int DeviceId=0, PathId=0; 
#endif // #if defined (_MIMO_)
	
	i = sizeof(FW_POWRTABLE);

	status = ReadPwrTableFile(
#if defined (_MIMO_)
		DeviceId,	PathId, 
#endif // #if defined (_MIMO_)
		BandId, SubBandId,  
		&i, (BYTE*)&table, pGivenFileName);
	if(pHighFreqBondary)
		*pHighFreqBondary	= table.pwrTbl.HighFreqBound;
	if(pLowFreqBondary)
		*pLowFreqBondary		= table.pwrTbl.LowFreqBound;

	for (i=0; i<RFPWRRANGE_NUM; i++)
	{
		pPpaGain1_Rng[i] = table.pwrTbl.rngTblLevel[i].PpaGain5_1;
		pPpaGain2_Rng[i] = table.pwrTbl.rngTblLevel[i].PpaGain5_2;
		pMaxPwr_Rng[i] 	= table.pwrTbl.rngTblLevel[i].TopPwr;
		pMinPwr_Rng[i]	= table.pwrTbl.rngTblLevel[i].BtmPwr;
	}
	
	*pSizeOfPwrTblInByte = sizeof(PWRTBL_DWORD)*POWTBL_LEVEL; 
	 
	memcpy(pPwrTbl, table.pwrTbl.pwrTblLevel, (*pSizeOfPwrTblInByte));

	return status;
}

int DutSharedClssHeader ReadPwrTableFile(
#if defined (_MIMO_)
						int DeviceId, int PathId, 
#endif // #if defined (_MIMO_)
						int BandId, int SubBandId,  
						int *pSizeOfTblInByte, BYTE* pPwrTbl, 
						char* pGivenFileName) 
{
 
	int		status=ERROR_NONE;
	FILE	*hFile=NULL;
	int		cnt = 0, cntLine=0; 
	int		tempInt[5]={0};
	char	FileName[255]={"W8060_PwrTbl.txt"};
	char	line[255]="";
	FW_POWRTABLE *pTbl=NULL;
	int localPpa5_1[RFPWRRANGE_NUM]={0},	localPpa5_2[RFPWRRANGE_NUM]={0};
	int localTop[RFPWRRANGE_NUM]={0},		localBot[RFPWRRANGE_NUM]={0};

	int localPwrLvl[POWTBL_LEVEL]={0},		localValInitTrgt[POWTBL_LEVEL]={0};
	int localValPdetThHi[POWTBL_LEVEL]={0},	localValPdetThLo[POWTBL_LEVEL]={0};
	int localValDacX[POWTBL_LEVEL]={0};
#if !defined (_MIMO_)
	int DeviceId=0, PathId=0; 
#endif // #if defined (_MIMO_)
	
	if(NULL== pGivenFileName) 
		GetPwrTbleFileName(
#if defined (_MIMO_)
							DeviceId,  PathId, 
#endif // #if defined (_MIMO_)
							BandId, SubBandId,  0, FileName);
	else
		strcpy(FileName, pGivenFileName);  

	pTbl =(FW_POWRTABLE*)pPwrTbl; 
	
	hFile = fopen(FileName, "r");
	if (NULL == hFile)
	{
		DebugLogRite("Cannot Find File \"%s\"\n", FileName);
		return ERROR_FILENOTEXIST;
	}
	DebugLogRite("LoadPwrTableFile from File \"%s\"\n", FileName);
  
 
 
 	*pSizeOfTblInByte =0;  
	cntLine=0;

// first line
	do
	{
		if(NULL != fgets(line, 255, hFile) )
		{
			if(strlen(line)>0 && line[0]!=';')
			{	//  HigestFreq, lowestFreq 
				cnt = sscanf(line, "%d %d", 
					&tempInt[0], &tempInt[1] );  
			 
 				if ( cnt != 2) 
						DebugLogRite("Format error on Line: %s\n", line);
				else
				{	
					(*pSizeOfTblInByte) += 4;
					pTbl->pwrTbl.HighFreqBound	= tempInt[0];
 					pTbl->pwrTbl.LowFreqBound		= tempInt[1]; 
					break;
				}
			}

		}
	}while(1);

//	fgets(line, 255, hFile)
	while ((cntLine<RFPWRRANGE_NUM) && (NULL != fgets(line, 255, hFile)) )
	{
		if(strlen(line)>0 && line[0]!=';')
		{
			cnt = sscanf(line, "%x %x %d %d", 
				&tempInt[0], &tempInt[1], &tempInt[2], &tempInt[3],  &tempInt[4]);  
 			if (cnt != 4)
			{
 				DebugLogRite("Format error on Line: %s\n", line);
			}
			else
			{
				(*pSizeOfTblInByte) += 4;
				localPpa5_1[cntLine] = tempInt[0];
				localPpa5_2[cntLine] = tempInt[1];
				localTop[cntLine] = tempInt[2];
				localBot[cntLine] = tempInt[3];

				cntLine++; 
			}
		}
	}


		for (cnt=0; cnt < cntLine; cnt++)
		{
 				pTbl->pwrTbl.rngTblLevel[cnt].PpaGain5_1 = localPpa5_1[cnt];
 				pTbl->pwrTbl.rngTblLevel[cnt].PpaGain5_2 = localPpa5_2[cnt];
 				pTbl->pwrTbl.rngTblLevel[cnt].TopPwr = localTop[cnt];
 				pTbl->pwrTbl.rngTblLevel[cnt].BtmPwr = localBot[cnt];  
		}

	cntLine=0;
	//fgets(line, 255, hFile)
	while ((cntLine<POWTBL_LEVEL) && (NULL != fgets(line, 255, hFile)) )
	{
		if(strlen(line)>0 && line[0]!=';')
		{
			cnt = sscanf(line, "%d %x %x %x %x", 
				&tempInt[0], &tempInt[1], &tempInt[2], &tempInt[3],  &tempInt[4]);  

//			DebugLogRite("line  %s\n", line);
		
 			if (cnt != 5)
			{
 				DebugLogRite("Format error on Line: %s\n", line);
			}
			else
			{
				(*pSizeOfTblInByte) += 4;
				
 //			DebugLogRite("cntLine  %d\n", cntLine);
 				localPwrLvl[cntLine] = tempInt[0]; 
 				
 				localValInitTrgt[cntLine] = tempInt[1]; 
				
 				localValPdetThHi[cntLine] = tempInt[2]; 

				localValPdetThLo[cntLine] = tempInt[3]; 

 				localValDacX[cntLine] = tempInt[4]; 
 				cntLine++;
				
			}
		}
	}
	fclose(hFile);
 
	// sort them 
//	Sort(int NumOfData, int IndexArray[], 
//				   int Data1[], int Data2[], int Data3[], int Data4[]);
 		Sort(cntLine, localPwrLvl, 
				   localValInitTrgt, localValPdetThHi, 
				   localValPdetThLo, localValDacX);
	// save them into structure
 

		for (cnt=0; cnt < cntLine; cnt++)
		{
 			pTbl->pwrTbl.pwrTblLevel[cnt].powerLevel	= localPwrLvl[cnt];
 			pTbl->pwrTbl.pwrTblLevel[cnt].ValInitTgrt = localValInitTrgt[cnt];
 			pTbl->pwrTbl.pwrTblLevel[cnt].ValPDetThHi = localValPdetThHi[cnt];
 			pTbl->pwrTbl.pwrTblLevel[cnt].ValPDetThLo = localValPdetThLo[cnt];
#if defined(_CAL_REV_D_)
			pTbl->pwrTbl.pwrTblLevel[cnt].Reserve=0;
#else
			pTbl->pwrTbl.pwrTblLevel[cnt].ValDacX		= localValDacX[cnt]; 
#endif	//#if defined(_CAL_REV_D_)
		}

	(*pSizeOfTblInByte) += 4;
	pTbl->DeviceId = DeviceId;
	pTbl->PathId = PathId;
	pTbl->BandId = BandId;
	pTbl->SubBandId = SubBandId;
 
	return status;
}
int DutSharedClssHeader ReadPwrTableFile_D(
#if defined (_MIMO_)
						int DeviceId, int PathId, 
#endif // #if defined (_MIMO_)
						int BandId, int SubBandId,  
						int *pSizeOfTblInByte, BYTE* pPwrTbl, 
						char* pGivenFileName) 
{
 
	int		status=ERROR_NONE;
	FILE	*hFile=NULL;
	int		cnt = 0, cntLine=0; 
	int		tempInt[5]={0};
	char	FileName[255]={"W8060_PwrTbl.txt"};
	char	line[255]="";
	FW_POWRTABLE *pTbl=NULL;
	int localPpa5_1[RFPWRRANGE_NUM]={0},	localPpa5_2[RFPWRRANGE_NUM]={0};
	int localTop[RFPWRRANGE_NUM]={0},		localBot[RFPWRRANGE_NUM]={0};

	int localPwrLvl[POWTBL_LEVEL]={0},		localValInitTrgt[POWTBL_LEVEL]={0};
	int localValPdetThHi[POWTBL_LEVEL]={0},	localValPdetThLo[POWTBL_LEVEL]={0};
	int localValDacX[POWTBL_LEVEL]={0};
#if !defined (_MIMO_)
	int DeviceId=0, PathId=0; 
#endif // #if defined (_MIMO_)
	
	if(NULL== pGivenFileName) 
		GetPwrTbleFileName(
#if defined (_MIMO_)
							DeviceId,  PathId, 
#endif // #if defined (_MIMO_)
							BandId, SubBandId,  0, FileName);
	else
		strcpy(FileName, pGivenFileName);  

	pTbl =(FW_POWRTABLE*)pPwrTbl; 
	
	hFile = fopen(FileName, "r");
	if (NULL == hFile)
	{
		DebugLogRite("Cannot Find File \"%s\"\n", FileName);
		return ERROR_FILENOTEXIST;
	}
	DebugLogRite("LoadPwrTableFile from File \"%s\"\n", FileName);
  
 
 
 	*pSizeOfTblInByte =0;  
	cntLine=0;

// first line
	do
	{
		if(NULL != fgets(line, 255, hFile) )
		{
			if(strlen(line)>0 && line[0]!=';')
			{	//  HigestFreq, lowestFreq 
				cnt = sscanf(line, "%d %d", 
					&tempInt[0], &tempInt[1] );  
			 
 				if ( cnt != 2) 
						DebugLogRite("Format error on Line: %s\n", line);
				else
				{	
					(*pSizeOfTblInByte) += 4;
					pTbl->pwrTbl.HighFreqBound	= tempInt[0];
 					pTbl->pwrTbl.LowFreqBound		= tempInt[1]; 
					break;
				}
			}

		}
	}while(1);
/*
//	fgets(line, 255, hFile)
	while ((cntLine<RFPWRRANGE_NUM) && (NULL != fgets(line, 255, hFile)) )
	{
		if(strlen(line)>0 && line[0]!=';')
		{
			cnt = sscanf(line, "%x %x %d %d", 
				&tempInt[0], &tempInt[1], &tempInt[2], &tempInt[3],  &tempInt[4]);  
 			if (cnt != 4)
			{
 				DebugLogRite("Format error on Line: %s\n", line);
			}
			else
			{
				(*pSizeOfTblInByte) += 4;
				localPpa5_1[cntLine] = tempInt[0];
				localPpa5_2[cntLine] = tempInt[1];
				localTop[cntLine] = tempInt[2];
				localBot[cntLine] = tempInt[3];

				cntLine++; 
			}
		}
	}
*/
/*
		for (cnt=0; cnt < cntLine; cnt++)
		{
 				pTbl->pwrTbl.rngTblLevel[cnt].PpaGain5_1 = localPpa5_1[cnt];
 				pTbl->pwrTbl.rngTblLevel[cnt].PpaGain5_2 = localPpa5_2[cnt];
 				pTbl->pwrTbl.rngTblLevel[cnt].TopPwr = localTop[cnt];
 				pTbl->pwrTbl.rngTblLevel[cnt].BtmPwr = localBot[cnt];  
		}
*/
	cntLine=0;
	//fgets(line, 255, hFile)
	while ((cntLine<POWTBL_LEVEL_REV_D) && (NULL != fgets(line, 255, hFile)) )
	{
		if(strlen(line)>0 && line[0]!=';')
		{
			cnt = sscanf(line, "%d %x %x %x %x", 
				&tempInt[0], &tempInt[1], &tempInt[2], &tempInt[3],  &tempInt[4]);  

//			DebugLogRite("line  %s\n", line);
		
 			if (cnt != 5)
			{
 				DebugLogRite("Format error on Line: %s\n", line);
			}
			else
			{
				(*pSizeOfTblInByte) += 4;
				
 //			DebugLogRite("cntLine  %d\n", cntLine);
 				localPwrLvl[cntLine] = tempInt[0]; 
 				
 				localValInitTrgt[cntLine] = tempInt[1]; 
				
 				localValPdetThHi[cntLine] = tempInt[2]; 

				localValPdetThLo[cntLine] = tempInt[3]; 

 				localValDacX[cntLine] = tempInt[4]; 
 				cntLine++;
				
			}
		}
	}
	fclose(hFile);
 
	// sort them 
//	Sort(int NumOfData, int IndexArray[], 
//				   int Data1[], int Data2[], int Data3[], int Data4[]);
#if 0 //jklchen do not Sort
 		Sort(cntLine, localPwrLvl, 
				   localValInitTrgt, localValPdetThHi, 
				   localValPdetThLo, localValDacX);
#endif
	// save them into structure
 

		for (cnt=0; cnt < cntLine; cnt++)
		{
 			pTbl->pwrTbl.pwrTblLevel[cnt].powerLevel	= localPwrLvl[cnt];
 			pTbl->pwrTbl.pwrTblLevel[cnt].ValInitTgrt = localValInitTrgt[cnt];
 			pTbl->pwrTbl.pwrTblLevel[cnt].ValPDetThHi = localValPdetThHi[cnt];
 			pTbl->pwrTbl.pwrTblLevel[cnt].ValPDetThLo = localValPdetThLo[cnt];
#if defined(_CAL_REV_D_)
			pTbl->pwrTbl.pwrTblLevel[cnt].Reserve=0;
#else
			pTbl->pwrTbl.pwrTblLevel[cnt].ValDacX		= localValDacX[cnt]; 
#endif	//#if defined(_CAL_REV_D_)

		}

	(*pSizeOfTblInByte) += 4;
	pTbl->DeviceId = DeviceId;
	pTbl->PathId = PathId;
	pTbl->BandId = BandId;
	pTbl->SubBandId = SubBandId;
 
	return status;
}
int DutSharedClssHeader ReadPwrTableFile_RevD(
#if defined (_MIMO_)
						int DeviceId, int PathId, 
#endif // #if defined (_MIMO_)
						int BandId, int SubBandId,  
						int *pHighFreqBondary, int *pLowFreqBondary,
					   int *pSizeOfPwrTblInByte, BYTE* pPwrTbl, 
					   char* pGivenFileName)
{
	int		status=ERROR_NONE, i=0;
	FW_POWRTABLE table={0};
#if !defined (_MIMO_)
	int DeviceId=0, PathId=0; 
#endif // #if defined (_MIMO_)
	
	i = sizeof(FW_POWRTABLE);

	status = ReadPwrTableFile_D(
#if defined (_MIMO_)
		DeviceId,	PathId, 
#endif // #if defined (_MIMO_)
		BandId, SubBandId,  
		&i, (BYTE*)&table, pGivenFileName);

	if(pHighFreqBondary)
		*pHighFreqBondary	= table.pwrTbl.HighFreqBound;
	if(pLowFreqBondary)
		*pLowFreqBondary		= table.pwrTbl.LowFreqBound;


	
	*pSizeOfPwrTblInByte = sizeof(PWRTBL_DWORD)*POWTBL_LEVEL; 
	 
	memcpy(pPwrTbl, table.pwrTbl.pwrTblLevel, (*pSizeOfPwrTblInByte));

	return status;
}

int DutSharedClssHeader WritePwrTableFile_RevD(
#if defined (_MIMO_)
						int DeviceId, int PathId, 
#endif // #if defined (_MIMO_)
						int BandId, int SubBandId,  
						int HighFreqBondary, int LowFreqBondary, 
					   int *pSizeOfPwrTblInByte, BYTE* pPwrTbl, 
					   char* pGivenFileName) 
{ 	
	int		status=ERROR_NONE, i=0;
	char	FileName[255]={"PwrTble_Band0_Subband0_Upload"};
	FILE	*hFile=NULL;
	PWRTBL_DWORD pwrTblLevel[8];
	PWRTBL_DWORD *table=NULL;
	
	
#if !defined (_MIMO_)
	int DeviceId=0, PathId=0; 
#endif // #if defined (_MIMO_)
		 	 
	{
		i = sizeof(FW_POWRTABLE);
		if(NULL == pGivenFileName)
		{
			GetPwrTbleFileName(
	#if defined (_MIMO_)
				DeviceId,	PathId, 
	#endif // #if defined (_MIMO_)
				BandId, SubBandId,  1, FileName);
		}
 		else
			strcpy(FileName,pGivenFileName);  


		if(NULL == pGivenFileName)
			hFile = fopen(FileName, "w");
		else
			hFile = fopen(FileName, "a");

		if (NULL == hFile)
		{
			DebugLogRite("Cannot Find File \"%s\"\n", FileName);
			return ERROR_ERROR;
		}

		DebugLogRite("WritePwrTableFile to File \"%s\"\n", FileName);
   
   		fprintf(hFile, "; HighFreqBound\t\tLowFreqBound\n" ); 
 		fprintf(hFile, "\t%d\t\t%d\n", 
					HighFreqBondary, LowFreqBondary);
 //;				pTbl->pwrTbl.HighFreqBound,
 //;				pTbl->pwrTbl.LowFreqBound); 
 
   		//;fprintf(hFile, "; PpaGain5_1\tPpaGain5_2\tTopPwr\tBtmPwr\n" ); 

	
   		fprintf(hFile, "; powerLevel\tValInitTgrt\tValPDetThHi\tValPDetThLo\tValDacX\n" ); 

		if(pPwrTbl)
		{
			 int cnt=0;
			 memcpy(pwrTblLevel, pPwrTbl, sizeof(pwrTblLevel));
		
			for(cnt=0; cnt < 4; cnt++)
			{
 				fprintf(hFile, "%d\t%x\t%x\t%x\t%x\n", 
 					pwrTblLevel[cnt].powerLevel,
 					pwrTblLevel[cnt].ValInitTgrt,
 					pwrTblLevel[cnt].ValPDetThHi,
 					pwrTblLevel[cnt].ValPDetThLo,
#if defined(_CAL_REV_D_)
					pwrTblLevel[cnt].Reserve); 

#else
					pwrTblLevel[cnt].ValDacX); 
#endif	//#if defined(_CAL_REV_D_)
			}
	 
 			fclose(hFile);
		}
	}
	return status;
}
#endif //#if defeined (_WLAN_)

/*
void DutSharedClssHeader FullPath(char *path)
{
	if (path)
	{
#ifndef _LINUX_
		if(getcwd(path, _MAX_PATH) != NULL)  
			strcat(path, "\\setup.ini");
		else
#endif	//#ifndef _LINUX_
		strcpy(path, "setup.ini");
	}

}

int DutSharedClssHeader LoadPwrTableFile(
					 int tableId,
					 int *max,
					 int *min, 
					 int *sizeOfTbl, 
					 BYTE* tbl) 
{
	int status=ERROR_NONE;
 	FILE * hFile=NULL;
	int  cnt = 0; 
	int tempInt[5]={0};
	char FileName[255]="";	
	char line[255]="";
	int PwrMax=0, PwrMin=0; 
	int i;

	for (i = POW_MIN; i< (POW_MAX+1)*4;i++)
	{
		tbl[i]=0xff;
	}
  
// Get filenames from Setup.ini file
	{
		char temp[_MAX_PATH]="";

		FullPath(temp);

		if (tableId == 0)
		{
			GetPrivateProfileString("PeregrinePwrTbl","PwrTblG", "PwrTblG.TXT", FileName,
				255, temp); 
		}
		else
		{
			GetPrivateProfileString("PeregrinePwrTbl14","PwrTblG14", "PwrTblG14.TXT", FileName,
				255, temp); 
		}
	}
	hFile = fopen(FileName, "r");
	if (NULL == hFile)
	{
		DebugLogRite("Cannot Find File \"%s\"\n", FileName);
		return ERROR_ERROR;
	}
	DebugLogRite("LoadPwrTableFile from File \"%s\"\n", FileName);
  
	PwrMax=POW_MAX;
	PwrMin=POW_MIN; 
 

	*max = PwrMin;
	*min = PwrMax;
	*sizeOfTbl =0; 

	while (NULL != fgets(line, 255, hFile))
	{
		if(strlen(line)>0)
		{
			cnt = sscanf(line, "%d %x %x %x %x", 
				&tempInt[0], &tempInt[1], &tempInt[2], &tempInt[3],  &tempInt[4]);  

 			if ((cnt != 5) || (tempInt[0] > PwrMax) || (tempInt[0] < PwrMin))
				DebugLogRite("Format error on Line: %s\n", line);
			else
			{
				if(*max < tempInt[0]) *max = tempInt[0];
				if(*min > tempInt[0]) *min = tempInt[0]; 

				*sizeOfTbl += 4;

				tbl[tempInt[0]*4]   = (char)tempInt[1];
				tbl[tempInt[0]*4+1] = (char)tempInt[2];
				tbl[tempInt[0]*4+2] = (char)tempInt[3];
				tbl[tempInt[0]*4+3] = (char)tempInt[4];
			}

		}
	 
	}
 
	fclose(hFile);

	return status;
}

int DutSharedClssHeader SavePwrTableFile(int band,
					 int tableId,
					 int *sizeOfTblInByte, 
					 BYTE* tbl, 
					 char* givenFileName)
{
	int status=ERROR_NONE;
 	FILE * hFile=NULL;
	int  cnt = 0,i; 
	int tempInt[5]={0};
	char FileName[255]="";	//{"PwrTblG_UpLoad.txt"};
	char line[255]="";
 	int band;

	status = GetBandAG(&band);
 
// Get filenames from Setup.ini file
	{
 		char temp[_MAX_PATH]="";

		FullPath(temp);

		if (band == BAND_802_11G)
		{
			if (tableId == 0)
				GetPrivateProfileString("PeregrinePwrTbl","PwrTblG_UpLoad","PwrTblG_UpLoad.TXT", FileName,
				255, temp); 
			else
				GetPrivateProfileString("PeregrinePwrTbl14", "PwrTblG_UpLoad14","PwrTblG_UpLoad14.TXT", FileName,
				255, temp); 
		}
		else
		{
			switch(tableId)
			{
			case 0:
				break;
			case 1:
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				break;
			default:
				break;
			}
		}
	}

	if (givenFileName != NULL)
		strcpy (FileName, givenFileName);
 
	hFile = fopen(FileName, "w");
	DebugLogRite("SavePwrTableFile to File \"%s\"\n", FileName);
  
	for (i=0; i<*sizeOfTblInByte/4;i++)
	{
		sprintf(line, "%d \t%x %x %x %x\n", i, tbl[4*i],tbl[4*i+1],tbl[4*i+2],tbl[4*i+3]);
		fputs(line, hFile);
	}
	 
	fclose(hFile);

	return status;
}
*/

