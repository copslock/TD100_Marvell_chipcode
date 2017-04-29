/** @file FmMenu.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#include "../DutCommon/MenuDisplay.h"



#define FM_CMDNAME_0			"-----------------------------------------------------------------------\n"\
"\t\tW87xx (FM) TEST MENU\n-----------------------------------------------------------------------\n"
#define FM_CMDDESC_0			CMDDESC_BLANK

#define FM_CMDNAME_1			CMDNAME_UNKNOWN
#define FM_CMDDESC_1			CMDDESC_BLANK

#define FM_CMDNAME_2			CMDNAME_UNKNOWN
#define FM_CMDDESC_2			CMDDESC_BLANK

#define FM_CMDNAME_3			CMDNAME_UNKNOWN
#define FM_CMDDESC_3			CMDDESC_BLANK

#define FM_CMDNAME_4			CMDNAME_UNKNOWN
#define FM_CMDDESC_4			CMDDESC_BLANK

#define FM_CMDNAME_5			CMDNAME_UNKNOWN
#define FM_CMDDESC_5			CMDDESC_BLANK

#define FM_CMDNAME_6			CMDNAME_UNKNOWN
#define FM_CMDDESC_6			CMDDESC_BLANK

#define FM_CMDNAME_7			CMDNAME_UNKNOWN
#define FM_CMDDESC_7			CMDDESC_BLANK

#define FM_CMDNAME_8			CMDNAME_UNKNOWN
#define FM_CMDDESC_8			CMDDESC_BLANK

#define FM_CMDNAME_9			CMDNAME_UNKNOWN
#define FM_CMDDESC_9			CMDDESC_BLANK

#define FM_CMDNAME_10			CMDNAME_UNKNOWN
#define FM_CMDDESC_10			CMDDESC_BLANK


#define FM_CMDNAME_11			CMDNAME_UNKNOWN
#define FM_CMDDESC_11			CMDDESC_BLANK


#define FM_CMDNAME_12			CMDNAME_UNKNOWN
#define FM_CMDDESC_12			CMDDESC_BLANK

#define FM_CMDNAME_13			CMDNAME_UNKNOWN
#define FM_CMDDESC_13			CMDDESC_BLANK

#define FM_CMDNAME_14			CMDNAME_UNKNOWN
#define FM_CMDDESC_14			CMDDESC_BLANK

#define FM_CMDNAME_15			CMDNAME_UNKNOWN
#define FM_CMDDESC_15			CMDDESC_BLANK

#define FM_CMDNAME_16			CMDNAME_UNKNOWN
#define FM_CMDDESC_16			CMDDESC_BLANK

#define FM_CMDNAME_17			CMDNAME_UNKNOWN
#define FM_CMDDESC_17			CMDDESC_BLANK

#define FM_CMDNAME_18			CMDNAME_UNKNOWN
#define FM_CMDDESC_18			CMDDESC_BLANK

#define FM_CMDNAME_19			CMDNAME_UNKNOWN
#define FM_CMDDESC_19			CMDDESC_BLANK


#define FM_CMDNAME_20			CMDNAME_UNKNOWN
#define FM_CMDDESC_20			CMDDESC_BLANK

#define FM_CMDNAME_21			CMDNAME_UNKNOWN
#define FM_CMDDESC_21			CMDDESC_BLANK

#define FM_CMDNAME_22			CMDNAME_UNKNOWN
#define FM_CMDDESC_22			CMDDESC_BLANK

#define FM_CMDNAME_23			CMDNAME_UNKNOWN
#define FM_CMDDESC_23			CMDDESC_BLANK

#define FM_CMDNAME_24			CMDNAME_UNKNOWN
#define FM_CMDDESC_24			CMDDESC_BLANK

#define FM_CMDNAME_25			CMDNAME_UNKNOWN
#define FM_CMDDESC_25			CMDDESC_BLANK

#define FM_CMDNAME_26			CMDNAME_UNKNOWN
#define FM_CMDDESC_26			CMDDESC_BLANK

#define FM_CMDNAME_27			CMDNAME_UNKNOWN
#define FM_CMDDESC_27			CMDDESC_BLANK

#define FM_CMDNAME_28			CMDNAME_UNKNOWN
#define FM_CMDDESC_28			CMDDESC_BLANK

#define FM_CMDNAME_29			CMDNAME_UNKNOWN
#define FM_CMDDESC_29			CMDDESC_BLANK

#define FM_CMDNAME_30			CMDNAME_UNKNOWN
#define FM_CMDDESC_30			CMDDESC_BLANK

#define FM_CMDNAME_31			CMDNAME_UNKNOWN
#define FM_CMDDESC_31			CMDDESC_BLANK

#define FM_CMDNAME_32			CMDNAME_UNKNOWN
#define FM_CMDDESC_32			CMDDESC_BLANK

#define FM_CMDNAME_33			CMDNAME_UNKNOWN
#define FM_CMDDESC_33			CMDDESC_BLANK

#define FM_CMDNAME_34			CMDNAME_UNKNOWN
#define FM_CMDDESC_34			CMDDESC_BLANK

#define FM_CMDNAME_35			CMDNAME_UNKNOWN
#define FM_CMDDESC_35			CMDDESC_BLANK

#define FM_CMDNAME_36			CMDNAME_UNKNOWN
#define FM_CMDDESC_36			CMDDESC_BLANK

#define FM_CMDNAME_37			CMDNAME_UNKNOWN
#define FM_CMDDESC_37			CMDDESC_BLANK

#define FM_CMDNAME_38			CMDNAME_UNKNOWN
#define FM_CMDDESC_38			CMDDESC_BLANK

#if !defined (_WLAN_) && !defined (_NO_EEPROM_)

#ifndef _FLASH_
#define FM_CMDNAME_39			"SPI download from binary file\n"
#define FM_CMDDESC_39			CMDDESC_BLANK
#else // #ifndef _FLASH_
#define FM_CMDNAME_39			CMDNAME_UNKNOWN
#define FM_CMDDESC_39			CMDDESC_BLANK
#endif // #ifndef _FLASH_

#define FM_CMDNAME_40			"SPI Verify from binary file\n"
#define FM_CMDDESC_40			CMDDESC_BLANK

#define FM_CMDNAME_41			"Dump E2PROM content\n"
#define FM_CMDDESC_41			CMDDESC_BLANK

#define FM_CMDNAME_42			"Get E2PROM type\n"
#define FM_CMDDESC_42			"\t(if) (addrLen) (device)\n"\
					"\t\tIf:\tinterface type: 0=detect-again, 1=SPI, 2=I2C\n"\
					"\t\taddrLen:\taddress width in byte. valid 1 - 3.\n"\
					"\t\tdevice:\tdevice type: 1=eeprom, 2=flash\n" 

#define FM_CMDNAME_43			"Force E2PROM type\n"
#define FM_CMDDESC_43			"\t(if) (addrLen) (device)\n"\
					"\t\tIf:\tinterface type: 0=detect-again, 1=SPI, 2=I2C\n"\
					"\t\taddrLen:\taddress width in byte. valid 1 - 3.\n"\
					"\t\t device:\tdevice type: 1=eeprom, 2=flash\n"

#define FM_CMDNAME_44			CMDNAME_UNKNOWN
#define FM_CMDDESC_44			CMDDESC_BLANK

#define FM_CMDNAME_45			CMDNAME_UNKNOWN
#define FM_CMDDESC_45			CMDDESC_BLANK

#define FM_CMDNAME_46			CMDNAME_UNKNOWN
#define FM_CMDDESC_46			CMDDESC_BLANK

#if defined(_IF_USB_) 

#define FM_CMDNAME_47			"Get USB Attribute\n"
#define FM_CMDDESC_47			CMDDESC_BLANK

#define FM_CMDNAME_48			"Set USB Attribute\n"
#define FM_CMDDESC_48			"((0x)(Attribute(8-bit)) (MaxPower=0(0 - 250)))\n"
 
#else //#if defined(_IF_USB_) 
#define FM_CMDNAME_47			CMDNAME_UNKNOWN
#define FM_CMDDESC_47			CMDDESC_BLANK

#define FM_CMDNAME_48			CMDNAME_UNKNOWN
#define FM_CMDDESC_48			CMDDESC_BLANK
#endif //#if defined(_IF_USB_) 

 
#define FM_CMDNAME_49			CMDNAME_UNKNOWN
#define FM_CMDDESC_49			CMDDESC_BLANK

#define FM_CMDNAME_50			CMDNAME_UNKNOWN
#define FM_CMDDESC_50			CMDDESC_BLANK

#ifdef _FLASH_
#define FM_CMDNAME_51			"Erase flash\n"
#define FM_CMDDESC_51			"(0x )(section-starting-address)\n"
#else //#ifdef _FLASH_
#define FM_CMDNAME_51			CMDNAME_UNKNOWN
#define FM_CMDDESC_51			CMDDESC_BLANK
#endif //#ifdef _FLASH_

#define FM_CMDNAME_52			CMDNAME_UNKNOWN
#define FM_CMDDESC_52			CMDDESC_BLANK

#define FM_CMDNAME_53			"Set Cal from file:CalBtDataFile.txt\n"
#define FM_CMDDESC_53			CMDDESC_BLANK

#define FM_CMDNAME_54			"Get Cal\n"
#define FM_CMDDESC_54			CMDDESC_BLANK

#else	// #if !defined (_WLAN_) && !defined (_NO_EEPROM_)

#define FM_CMDNAME_39			CMDNAME_UNKNOWN
#define FM_CMDDESC_39			CMDDESC_BLANK

#define FM_CMDNAME_40			CMDNAME_UNKNOWN
#define FM_CMDDESC_40			CMDDESC_BLANK

#define FM_CMDNAME_41			CMDNAME_UNKNOWN
#define FM_CMDDESC_41			CMDDESC_BLANK

#define FM_CMDNAME_42			CMDNAME_UNKNOWN
#define FM_CMDDESC_42			CMDDESC_BLANK

#define FM_CMDNAME_43			CMDNAME_UNKNOWN
#define FM_CMDDESC_43			CMDDESC_BLANK

#define FM_CMDNAME_44			CMDNAME_UNKNOWN
#define FM_CMDDESC_44			CMDDESC_BLANK

#define FM_CMDNAME_45			CMDNAME_UNKNOWN
#define FM_CMDDESC_45			CMDDESC_BLANK

#define FM_CMDNAME_46			CMDNAME_UNKNOWN
#define FM_CMDDESC_46			CMDDESC_BLANK

#define FM_CMDNAME_47			CMDNAME_UNKNOWN
#define FM_CMDDESC_47			CMDDESC_BLANK

#define FM_CMDNAME_48			CMDNAME_UNKNOWN
#define FM_CMDDESC_48			CMDDESC_BLANK

#define FM_CMDNAME_49			CMDNAME_UNKNOWN
#define FM_CMDDESC_49			CMDDESC_BLANK

#define FM_CMDNAME_50			CMDNAME_UNKNOWN
#define FM_CMDDESC_50			CMDDESC_BLANK

#define FM_CMDNAME_51			CMDNAME_UNKNOWN
#define FM_CMDDESC_51			CMDDESC_BLANK

#define FM_CMDNAME_52			CMDNAME_UNKNOWN
#define FM_CMDDESC_52			CMDDESC_BLANK

#define FM_CMDNAME_53			CMDNAME_UNKNOWN
#define FM_CMDDESC_53			CMDDESC_BLANK

#define FM_CMDNAME_54			CMDNAME_UNKNOWN
#define FM_CMDDESC_54			CMDDESC_BLANK

#endif	//#if !defined (_WLAN_) && !defined (_NO_EEPROM_)

#define FM_CMDNAME_55			CMDNAME_UNKNOWN
#define FM_CMDDESC_55			CMDDESC_BLANK

#define FM_CMDNAME_56			CMDNAME_UNKNOWN
#define FM_CMDDESC_56			CMDDESC_BLANK

#define FM_CMDNAME_57			CMDNAME_UNKNOWN
#define FM_CMDDESC_57			CMDDESC_BLANK

#define FM_CMDNAME_58			CMDNAME_UNKNOWN
#define FM_CMDDESC_58			CMDDESC_BLANK

#define FM_CMDNAME_59			CMDNAME_UNKNOWN
#define FM_CMDDESC_59			CMDDESC_BLANK

#define FM_CMDNAME_60			CMDNAME_UNKNOWN
#define FM_CMDDESC_60			CMDDESC_BLANK

#define FM_CMDNAME_61			CMDNAME_UNKNOWN
#define FM_CMDDESC_61			CMDDESC_BLANK

#define FM_CMDNAME_62			CMDNAME_UNKNOWN
#define FM_CMDDESC_62			CMDDESC_BLANK

#define FM_CMDNAME_63			CMDNAME_UNKNOWN
#define FM_CMDDESC_63			CMDDESC_BLANK

#define FM_CMDNAME_64			CMDNAME_UNKNOWN
#define FM_CMDDESC_64			CMDDESC_BLANK

#define FM_CMDNAME_65			CMDNAME_UNKNOWN
#define FM_CMDDESC_65			CMDDESC_BLANK

#define FM_CMDNAME_66			"Read FMU Reg\n"
#define FM_CMDDESC_66			"\t(0x)(addr(12-bit)) (count =1)\n"\
	"\t\t addr is 12 bit value, aligned at 4.\n"

#define FM_CMDNAME_67			"Write FMU Reg\n"
#define FM_CMDDESC_67			"\t(0x 0x)(addr(12-bit) data)\n"\
	"\t\t addr is 12 bit value, aligned at 4.\n"

#define FM_CMDNAME_68			"Peek SOC Mem\n"
#define FM_CMDDESC_68			"\t(0x)(addr) (count =1)\n"

#define FM_CMDNAME_69			"Poke SOC Mem\n"
#define FM_CMDDESC_69			"\t(0x 0x)(addr data)\n"


#if !defined (_WLAN_) && !defined (_NO_EEPROM_)
#define FM_CMDNAME_70			"Peek Spi\n"
#define FM_CMDDESC_70			"\t(0x)(addr) (count =1)\n"

#define FM_CMDNAME_71			"Poke Spi\n"
#define FM_CMDDESC_71			"\t(0x 0x)(addr data)\n"
#else	// #if !defined (_WLAN_) && !defined (_NO_EEPROM_)
#define FM_CMDNAME_70			CMDNAME_UNKNOWN
#define FM_CMDDESC_70			CMDDESC_BLANK

#define FM_CMDNAME_71			CMDNAME_UNKNOWN
#define FM_CMDDESC_71			CMDDESC_BLANK
#endif // #if !defined (_WLAN_) && !defined (_NO_EEPROM_)

#if defined (_W8689_) || defined (_W8787_) || defined(_W8782_) 
#define FM_CMDNAME_72			"Read Efuse Data\n"
#define FM_CMDDESC_72			"\t(0x)(block) (WordCount =16)\n"

#define FM_CMDNAME_73			"Write Efuse Data\n"
#define FM_CMDDESC_73			"\t(0x 0x)(block WordCount=1)\n"

#else //#if defined (_W8689_) || defined (_W8787_) || defined(_W8782_)

#define FM_CMDNAME_72			CMDNAME_UNKNOWN
#define FM_CMDDESC_72			CMDDESC_BLANK

#define FM_CMDNAME_73			CMDNAME_UNKNOWN
#define FM_CMDDESC_73			CMDDESC_BLANK
#endif //#if defined (_W8689_) || defined (_W8787_)  || defined(_W8782_)


#define FM_CMDNAME_74			CMDNAME_UNKNOWN
#define FM_CMDDESC_74			CMDDESC_BLANK

#define FM_CMDNAME_75			CMDNAME_UNKNOWN
#define FM_CMDDESC_75			CMDDESC_BLANK

#define FM_CMDNAME_76			CMDNAME_UNKNOWN
#define FM_CMDDESC_76			CMDDESC_BLANK

#define FM_CMDNAME_77			CMDNAME_UNKNOWN
#define FM_CMDDESC_77			CMDDESC_BLANK

#define FM_CMDNAME_78			CMDNAME_UNKNOWN
#define FM_CMDDESC_78			CMDDESC_BLANK
 
#define FM_CMDNAME_79			CMDNAME_UNKNOWN
#define FM_CMDDESC_79			CMDDESC_BLANK

#define FM_CMDNAME_80			CMDNAME_UNKNOWN
#define FM_CMDDESC_80			CMDDESC_BLANK

#define FM_CMDNAME_81			CMDNAME_UNKNOWN
#define FM_CMDDESC_81			CMDDESC_BLANK

#define FM_CMDNAME_82			CMDNAME_UNKNOWN
#define FM_CMDDESC_82			CMDDESC_BLANK

#define FM_CMDNAME_83			CMDNAME_UNKNOWN
#define FM_CMDDESC_83			CMDDESC_BLANK

#define FM_CMDNAME_84			CMDNAME_UNKNOWN
#define FM_CMDDESC_84			CMDDESC_BLANK

#define FM_CMDNAME_85			CMDNAME_UNKNOWN
#define FM_CMDDESC_85			CMDDESC_BLANK

#define FM_CMDNAME_86			CMDNAME_UNKNOWN
#define FM_CMDDESC_86			CMDDESC_BLANK

#define FM_CMDNAME_87			CMDNAME_UNKNOWN
#define FM_CMDDESC_87			CMDDESC_BLANK

// ==================================
// ===   General FM Information   ===
// ==================================

#define FM_CMDNAME_88			"FW/HW Version\n"
#define FM_CMDDESC_88			CMDDESC_BLANK

#define FM_CMDNAME_89			"Load Dut configuration file.\n"
#define FM_CMDDESC_89			"\t(FileName)\n"

#define FM_CMDNAME_90			CMDNAME_UNKNOWN
#define FM_CMDDESC_90			CMDDESC_BLANK

#define FM_CMDNAME_91			CMDNAME_UNKNOWN
#define FM_CMDDESC_91			CMDDESC_BLANK

#define FM_CMDNAME_92			CMDNAME_UNKNOWN
#define FM_CMDDESC_92			CMDDESC_BLANK

#define FM_CMDNAME_93			CMDNAME_UNKNOWN
#define FM_CMDDESC_93			CMDDESC_BLANK

#define FM_CMDNAME_94			CMDNAME_UNKNOWN
#define FM_CMDDESC_94			CMDDESC_BLANK

#if defined(_W8790_)
#define FM_CMDNAME_95			"Get Rf XTAL control\n"
#define FM_CMDDESC_95			CMDDESC_BLANK

#define FM_CMDNAME_96			"Set Rf XTAL control\n"
#define FM_CMDDESC_96			"\t(setting(8-bit))\n"
#else // #if defined(_W8790_)
#define FM_CMDNAME_95			CMDNAME_UNKNOWN
#define FM_CMDDESC_95			CMDDESC_BLANK

#define FM_CMDNAME_96			CMDNAME_UNKNOWN
#define FM_CMDDESC_96			CMDDESC_BLANK
#endif // #if defined(_W8790_)

#define FM_CMDNAME_97			CMDNAME_UNKNOWN
#define FM_CMDDESC_97			CMDDESC_BLANK

#define FM_CMDNAME_98			CMDNAME_UNKNOWN
#define FM_CMDDESC_98			CMDDESC_BLANK

#define FM_CMDNAME_99			"Exit\n"
#define FM_CMDDESC_99			CMDDESC_BLANK

//==================================
//===     General FM Control     ===
//==================================

//#define FM_CMDNAME_100			"Fm Reset\n"
//#define FM_CMDDESC_100			CMDDESC_BLANK

#define FM_CMDNAME_100			"Fm Initialization\n"
#define FM_CMDDESC_100			"\t(XtalFreq=38400(KHz))\n"\
"\t\tXtalFreq: XTAL frequency in KHz. Default 38400 KHz\n"
//#define FM_CMDDESC_101			"\t(XtalFreq=38400(KHz) (Error=0)(ppm))\n"\
//\
//"\t\tError: Reference Frequency error in ppm. Default 0 ppm\n"


#define FM_CMDNAME_101			CMDNAME_UNKNOWN
#define FM_CMDDESC_101			CMDDESC_BLANK


#define FM_CMDNAME_102			"Fm Set Reference Clock Error\n"
#define FM_CMDDESC_102			"\tError: Reference Clock Error in ppm. Default 0.\n"

#define FM_CMDNAME_103			"Fm Set Mode\n"
#define FM_CMDDESC_103			"\t(Power Up Mode=0)\n"\
				"\t\tPower Up Mode : Fm Power Up Mode. Default 0.\n"\
				"\t\t0:Stop receiver;\n"\
				"\t\t1:Start RDS receiver only;\n"\
				"\t\t2:Start audio receiver only;\n"\
				"\t\t3:Start audio and RDS receiver\n"\
				"\t\t5:Start RDS Transmitter only;\n"\
				"\t\t6:Start audio Transmitter only;\n"\
				"\t\t7:Start audio and RDS Transmitter\n"\
				"\t\tA:Start CODEC mode\n"

#define FM_CMDNAME_104			"Fm Get Current Rssi\n"
#define FM_CMDDESC_104			CMDDESC_BLANK

#define FM_CMDNAME_105			"Fm Get Current CMI\n"
#define FM_CMDDESC_105			CMDDESC_BLANK

#define FM_CMDNAME_106			"Select Fm Rx AntennaType\n"
#define FM_CMDDESC_106			"\t(Type)\n"\
				"\t\tType: 0: 75Ohm, 1: TxLoop \n"

#define FM_CMDNAME_107			"Get Fm Irs Mask\n"
#define FM_CMDDESC_107			"\tOff=0, On=1 for each ISR, in below order.\n"\
			"\t\tRssiLow=0,\tNewRdsData=0,\tRssiIndicator=1,\tBandLimit=0,\n"\
			"\t\tSyncLost=0,\tSearchStop=0,\tBMatch=0,\t\tPiMatched=0,\n"\
			"\t\tMonoStereoChanged=0,\t\tAudioPause=0,\t\tCmiLow=0\n"

#define FM_CMDNAME_108			"Set Fm Irs Mask\n"
#define FM_CMDDESC_108			"\tMask:\tOff=0, On=1 for each ISR, in below order.\n"\
			"\t\tRssiLow=0,\tNewRdsData=0,\tRssiIndicator=1,\tBandLimit=0,\n"\
			"\t\tSyncLost=0,\tSearchStop=0,\tBMatch=0,\t\tPiMatched=0,\n"\
			"\t\tMonoStereoChanged=0,\t\tAudioPause=0,\t\tCmiLow=0\n"

#define FM_CMDNAME_109			"Fm Get Current Flags\n"
#define FM_CMDDESC_109			CMDDESC_BLANK

#define FM_CMDNAME_110			"Fm Power Down\n"
#define FM_CMDDESC_110			CMDDESC_BLANK

#define FM_CMDNAME_111			CMDNAME_UNKNOWN
#define FM_CMDDESC_111			CMDDESC_BLANK

#define FM_CMDNAME_112			CMDNAME_UNKNOWN
#define FM_CMDDESC_112			CMDDESC_BLANK

#define FM_CMDNAME_113			CMDNAME_UNKNOWN
#define FM_CMDDESC_113			CMDDESC_BLANK

#define FM_CMDNAME_114			CMDNAME_UNKNOWN
#define FM_CMDDESC_114			CMDDESC_BLANK

#define FM_CMDNAME_115			CMDNAME_UNKNOWN
#define FM_CMDDESC_115			CMDDESC_BLANK

#define FM_CMDNAME_116			CMDNAME_UNKNOWN
#define FM_CMDDESC_116			CMDDESC_BLANK

#define FM_CMDNAME_117			CMDNAME_UNKNOWN
#define FM_CMDDESC_117			CMDDESC_BLANK

#define FM_CMDNAME_118			CMDNAME_UNKNOWN
#define FM_CMDDESC_118			CMDDESC_BLANK

#define FM_CMDNAME_119			CMDNAME_UNKNOWN
#define FM_CMDDESC_119			CMDDESC_BLANK

// ==================================
// === General FM Channel Control ===
// ==================================

#define FM_CMDNAME_120			"Fm Get Channel\n"
#define FM_CMDDESC_120			CMDDESC_BLANK

#define FM_CMDNAME_121			"Fm Set Channel\n"
#define FM_CMDDESC_121			"\t(chFreqInKHz)\n"\
"\t\tchFreqInKHz : Channel Frequency in KHz.\n"

#define FM_CMDNAME_122			CMDNAME_UNKNOWN
#define FM_CMDDESC_122			CMDDESC_BLANK

#define FM_CMDNAME_123			"Fm Set AF Channel\n"
#define FM_CMDDESC_123			"\t(chFreqInKHz) (AfMode)\n"\
"\t\tchFreqInKHz : AF channel Frequency in KHz. Default 95600 KHz.\n"\
"\t\tAfMode : AF Mode. 0=RSSI based, 1=CMI based, 2=RSSI and CMI based. Default 0.\n"

#define FM_CMDNAME_124			"Fm Get Search Direction\n"
#define FM_CMDDESC_124			CMDDESC_BLANK

#define FM_CMDNAME_125			"Fm Set Search Direction\n"
#define FM_CMDDESC_125			"\t(Dir=0)\n"\
"\t\tDir : Search Direction. 0=Forward; 1=Backward. Default 0.\n"

#define FM_CMDNAME_126			"Fm Get Auto Search Mode\n"
#define FM_CMDDESC_126			CMDDESC_BLANK

#define FM_CMDNAME_127			"Fm Set Auto Search Mode\n"
#define FM_CMDDESC_127			"\t(mode)\n"\
"\t\tmode : Search Mode. 0=Auto; 1=Manual; 2=AF Jump. Default 0.\n"

#define FM_CMDNAME_128			"Fm Stop Search\n"
#define FM_CMDDESC_128			CMDDESC_BLANK
 
#define FM_CMDNAME_129			CMDNAME_UNKNOWN	//"Fm Set Lo Injection Side\n"
#define FM_CMDDESC_129			CMDDESC_BLANK	//"\t\tSide:\n"\
//			"\t\t0 = LOINJECTION_HIGHSIDE Flo=Frx+Fif\n"\
//			"\t\t0 = LOINJECTION_LOWSIDE  Flo=Frx-Fif\n"\
//			"\t\t0 = LOINJECTION_AUTO \n" 

#define FM_CMDNAME_130			"Fm Get Force Mono Mode\n"
#define FM_CMDDESC_130			"\tMode: 0=false; 1=true;\n" 

#define FM_CMDNAME_131			"Fm Set Force Mono Mode\n"
#define FM_CMDDESC_131			"\t(mode=0)\n"\
			"\t\tMode: 0=false; 1=true;\n" 

#define FM_CMDNAME_132			"Fm Get Rssi Threshold\n"
#define FM_CMDDESC_132			CMDDESC_BLANK

#define FM_CMDNAME_133			"Fm Set Rssi Threshold\n"
#define FM_CMDDESC_133			"\t(threshold=30)\n"\
"\t\tthreshold : Rssi Threshold in hex. Default 30.\n"

#define FM_CMDNAME_134			CMDNAME_UNKNOWN	// "Fm Get Band\n"
#define FM_CMDDESC_134			CMDDESC_BLANK	// "\tBand ID: 0:US; 1:European; 2:Japan; 3:China.\n"

#define FM_CMDNAME_135			"Fm Set Band\n"
#define FM_CMDDESC_135			"\t(BandId=0 (0:US; 1:European; 2:Japan; 3:China)\n"\
"\t\tBandId : Band ID. 0:US; 1:European; 2:Japan; 3:China. Default 0.\n"

#define FM_CMDNAME_136			CMDNAME_UNKNOWN
#define FM_CMDDESC_136			CMDDESC_BLANK

#define FM_CMDNAME_137			"Fm Set Channel Step Size\n"
#define FM_CMDDESC_137			"\t\tstep: Channel search/step step size in KHz.  Default 100 KHz.\n"

#define FM_CMDNAME_138			"Fm Move Ch Up/Down\n"
#define FM_CMDDESC_138			"\t\tdir : Dirction of move channel Up/Down. 0=up; 1=down. Default 0.\n"

#define FM_CMDNAME_139			"Fm Get CMI Threshold\n"
#define FM_CMDDESC_139			CMDDESC_BLANK

#define FM_CMDNAME_140			"Fm Set CMI Threshold\n"
#define FM_CMDDESC_140			"\t(threshold=30)\n"\
"\t\tthreshold : CMI Threshold in hex. Default 30.\n"

//#define FM_CMDNAME_139			CMDNAME_UNKNOWN
//#define FM_CMDDESC_139			CMDDESC_BLANK

//#define FM_CMDNAME_140			CMDNAME_UNKNOWN
//#define FM_CMDDESC_140			CMDDESC_BLANK

#define FM_CMDNAME_141			"FM Get Stereo Blending Mode\n"
#define FM_CMDDESC_141			"\t\tMode: 0=Switched Blending, 1=Soft Blending.\n"


#define FM_CMDNAME_142			"FM Set Stereo Blending Mode\n"
#define FM_CMDDESC_142			"\t(Mode=0)\n"\
	"\t\tMode: 0=Switched Blending, 1=Soft Blending. Default=0.\n"

#define FM_CMDNAME_143			"FM Stereo Blending Configuration\n"
#define FM_CMDDESC_143			"\t(TimeConstant=0)\n"\
	"\t\tTimeConstant: Time Constant, hex, 8-bit. Default=0.\n"

#define FM_CMDNAME_144			"FM Get Stereo Status\n"
#define FM_CMDDESC_144			"\tStatus: 0=mono, 1=Stereo, 2=Blending.\n"

#define FM_CMDNAME_145			"Fm Set Force Stereo Mode\n"
#define FM_CMDDESC_145			"\tMode: 0=false; 1=true;\n" 
 

#define FM_CMDNAME_146			CMDNAME_UNKNOWN
#define FM_CMDDESC_146			CMDDESC_BLANK

#define FM_CMDNAME_147			CMDNAME_UNKNOWN
#define FM_CMDDESC_147			CMDDESC_BLANK

#define FM_CMDNAME_148			CMDNAME_UNKNOWN
#define FM_CMDDESC_148			CMDDESC_BLANK

#define FM_CMDNAME_149			CMDNAME_UNKNOWN
#define FM_CMDDESC_149			CMDDESC_BLANK

// ==================================
// ===  General FM Audio Control  ===
// ==================================

#define FM_CMDNAME_150			CMDNAME_UNKNOWN		// "Get Fm Rx Audio De-emphasis\n"
#define FM_CMDDESC_150			CMDDESC_BLANK		//	"\t\tmode: 0=NONE, 1=50US, 2=75US\n"

#define FM_CMDNAME_151			"Set Fm Audio Emphasis (mode=0) \n"
#define FM_CMDDESC_151			"\t(mode=0)\n"\
"\t\tmode: Audio Emphasis Mode for Tx and Rx path. 0=NONE, 1=50US, 2=75US \n"

#define FM_CMDNAME_152			"Get Fm Rx Mute Mode\n"
#define FM_CMDDESC_152			"\t\tMute Mode.\n"\
			"\t\t0=All Muter OFF\n"\
			"\t\t1=Left/Right AC Mute\n"\
			"\t\t2=Left Hard Mute only\n"\
			"\t\t3=Left Hard Mute - Right AC Mute\n"\
			"\t\t4=Right Hard Mute only\n"\
			"\t\t5=Hard Hard Mute - Left AC Mute\n"\
			"\t\t6=Left/Right hard Mute\n"

#define FM_CMDNAME_153			"Set Fm Rx Mute Mode\n"
#define FM_CMDDESC_153			"\t(mode)\n"\
			"\t\tmode: Mute Mode.\n"\
			"\t\tMute Mode.\n"\
			"\t\t0=All Muter OFF\n"\
			"\t\t1=Left/Right AC Mute\n"\
			"\t\t2=Left Hard Mute only\n"\
			"\t\t3=Left Hard Mute - Right AC Mute\n"\
			"\t\t4=Right Hard Mute only\n"\
			"\t\t5=Hard Hard Mute - Left AC Mute\n"\
			"\t\t6=Left/Right hard Mute\n"

#define FM_CMDNAME_154			"Get Rx Audio Path\n"
#define FM_CMDDESC_154			"\t\tAudioPath:	0=AUDIO_FM,  1=I2S_FM, 2=I2S_AIU, 3=SQU, 4=SPDIF\n"

#define FM_CMDNAME_155			"Set Rx Audio Path\n"
#define FM_CMDDESC_155			"\t(AudioPath) (I2sOperation) (I2sMode)\n"\
			"\t\tAudioPath:	0=AUDIO_FM,  1=I2S_FM, 2=I2S_AIU, 3=SQU, 4=SPDIF\n"\
			"\t\tI2sOperation: 0=SLAVE, 1=MASTER\n"\
			"\t\tI2sMode: 0=I2S, 1=MSB, 2=LSB, 3=PCM\n"

#define FM_CMDNAME_156			"Set Fm Audio Samping Rate\n"
#define FM_CMDDESC_156			"\t(rate) (Bclk/LRClk)\n"\
			"\t\trate : Sampling Rate. 0=8KHz,	1=11.025KHz, 2=12KHz, 3=16KHz,\n"\
			"\t\t\t4=22.05KHz, 5=24KHz,	6=32KHz, 7=44.1KHz, 8=48KHz\n"\
			"\t\tBclk/LRClk: 0=32X,	1=50X,	2=64X\n"

#define FM_CMDNAME_157			"Enable Audio Pause\n"
#define FM_CMDDESC_157			"\t(mode)\n"\
"\t\tmode: Pause Mode. 0=OFF, 1=PAUSE-START-ONLY,\n\t\t\t2=PAUSE-STOP-ONLY, 3=PAUSE-START-STOP\n"

#define FM_CMDNAME_158			CMDNAME_UNKNOWN //"Get Rx Audio Pause Duration in ms\n"
#define FM_CMDDESC_158			CMDDESC_BLANK

#define FM_CMDNAME_159			"Set Rx Audio Pause Duration in ms\n"
#define FM_CMDDESC_159			"\t(duration)ms\n"\
"\t\tduration in ms, decimal\n"

#define FM_CMDNAME_160			CMDNAME_UNKNOWN //"Get Rx Audio Pause Level\n"
#define FM_CMDDESC_160			CMDDESC_BLANK	//"\t\tLevel: 16 bit value, hex\n"

#define FM_CMDNAME_161			"Set Rx Audio Pause Level\n"
#define FM_CMDDESC_161			"\t(Level)\n"\
"\t\tLevel: 16 bit value, hex\n"

#define FM_CMDNAME_162			"Get Rx Soft Mute Mode\n"
#define FM_CMDDESC_162			"\tEnable: 0=OFF, 1=ON.\n"

#define FM_CMDNAME_163			"Set Rx Soft Mute Mode\n"
#define FM_CMDDESC_163			"\t(enable)  (MuteThreshold) (AudioAttenuation)\n"\
"\t\tEnable: 0=OFF, 1=ON. Default 0.\n"\
"\t\tMute Threshold: (0.37 dB/LSB). Default 119.\n"\
"\t\tAudio Attenuation: (3 dB/LSB, range 0 - 7). Default 0.\n"

#define FM_CMDNAME_164			"Get PLL Lock Status\n"
#define FM_CMDDESC_164			"\t0=unlocked, 1=locked\n"

#define FM_CMDNAME_165			"FM Get I2S Parameters\n"
#define FM_CMDDESC_165			"\t(chFreqInKHz)\n"\
			"\t\tchFreqInKHz : Channel Frequency in KHz.\n"\
			"\t\tSampling Rate: 0=8KHz,	1=11.025KHz, 2=12KHz, 3=16KHz,\n"\
			"\t\t\t4=22.05KHz, 5=24KHz,	6=32KHz, 7=44.1KHz, 8=48KHz\n"\
			"\t\tBclk/LRClk: 0=32X,	1=50X,	2=64X\n"

#define FM_CMDNAME_166			"FM Get Audio Volume\n"
#define FM_CMDDESC_166			CMDDESC_BLANK

#define FM_CMDNAME_167			"FM Set Audio Volume\n"
#define FM_CMDDESC_167			"\tvolume\n"\
"\t\tvolume : Audio Volume, between 0-2048\n"

#define FM_CMDNAME_168			CMDNAME_UNKNOWN
#define FM_CMDDESC_168			CMDDESC_BLANK

#define FM_CMDNAME_169			CMDNAME_UNKNOWN
#define FM_CMDDESC_169			CMDDESC_BLANK

// ==================================
// === General FM RDS Rx Control  ===
// ==================================

#define FM_CMDNAME_170			CMDNAME_UNKNOWN
#define FM_CMDDESC_170			CMDDESC_BLANK

#define FM_CMDNAME_171			CMDNAME_UNKNOWN
#define FM_CMDDESC_171			CMDDESC_BLANK

#define FM_CMDNAME_172			"Get Rx Rds Sync Status\n"
#define FM_CMDDESC_172			CMDDESC_BLANK

#define FM_CMDNAME_173			"Rx Rds Flush\n"
#define FM_CMDDESC_173			CMDDESC_BLANK

#define FM_CMDNAME_174			"Get Rx Rds Memory Depth\n"
#define FM_CMDDESC_174			"\t\tDepth: decimal, multiple of 4.Default=4\n"


#define FM_CMDNAME_175			"Set Rx Rds Memory Depth\n"
#define FM_CMDDESC_175			"\t(Depth=4) \n"\
			"\t\tDepth: decimal, multiple of 4. Default=4\n"

#define FM_CMDNAME_176			"Get Rx Rds Block B\n"
#define FM_CMDDESC_176			"\t\tBlockB: hex, 16-bit.\n"

#define FM_CMDNAME_177			"Set Rx Rds Block B (0x)(BlockB=0)\n"
#define FM_CMDDESC_177			"\t(BlockB=0)\n"\
"\t\tBlockB: hex, 16-bit. Default=0.\n"

#define FM_CMDNAME_178			"Get Rx Rds Block B Mask\n"
#define FM_CMDDESC_178			CMDDESC_BLANK

#define FM_CMDNAME_179			"Set Rx Rds Block B Mask\n"
#define FM_CMDDESC_179			"\t(BlockB=0)\n"\
"\t\tBlockB: hex, 16-bit. Default=0.\n"

#define FM_CMDNAME_180			"Get Rx Rds PI Code\n"
#define FM_CMDDESC_180			CMDDESC_BLANK

#define FM_CMDNAME_181			"Set Rx Rds PI Code\n"
#define FM_CMDDESC_181			"\t(PiCode=1234)\n"\
"\t\tPiCode: PI Code, hex, 16-bit. Default=1234\n"

#define FM_CMDNAME_182			"Get Rx Rds PI Code Mask\n"
#define FM_CMDDESC_182			CMDDESC_BLANK

#define FM_CMDNAME_183			"Set Rx Rds PI Code Mask\n"
#define FM_CMDDESC_183			"\t(PiCode=FFFF)\n"\
"\t\tPiCode: PI Code Mask, hex, 16-bit. Default=FFFF\n"

#define FM_CMDNAME_184			"Get Rx Rds Mode\n"
#define FM_CMDDESC_184			"\t\tMode: 0=Standard, 1=RBDS w/o E blocks\n"

#define FM_CMDNAME_185			"Set Rx Rds Mode\n"
#define FM_CMDDESC_185			"\t(Mode=0)\n"\
"\t\tMode: 0=Standard, 1=RBDS w/o E blocks\n"

#define FM_CMDNAME_186			CMDNAME_UNKNOWN
#define FM_CMDDESC_186			CMDDESC_BLANK

#define FM_CMDNAME_187			CMDNAME_UNKNOWN
#define FM_CMDDESC_187			CMDDESC_BLANK

#define FM_CMDNAME_188			CMDNAME_UNKNOWN	//"FW on-line release note\n"
#define FM_CMDDESC_188			CMDDESC_BLANK

#define FM_CMDNAME_189			CMDNAME_UNKNOWN
#define FM_CMDDESC_189			CMDDESC_BLANK

// ==================================
// ===   General FM Tx Control    ===
// ==================================

#define FM_CMDNAME_190			"Get Output Power\n"
#define FM_CMDDESC_190			"\tpwr: Tx output power level. Decimal, max=255\n"

#define FM_CMDNAME_191			"Set Output Power\n"
#define FM_CMDDESC_191			"\t(pwr=119)\n"\
"\t\tpwr: Tx output power level. Decimal, max=255. Default=119\n"

#define FM_CMDNAME_192			"Set Frequency Deviation in Hz\n"
#define FM_CMDDESC_192			"\t(Deviation=75000)\n"\
"\t\tDeviation: Frequency Deviation in Hz. Decimal. Default=75000\n"

#define FM_CMDNAME_193			CMDNAME_UNKNOWN	//	"Search Tx Free Channel in KHz\n"
#define FM_CMDDESC_193			CMDDESC_BLANK	//"\t(StartFreq=95600)\n"\
// "\t\tStartFreq: Start Channel Frequency in KHz. Default=95600\n"

#define FM_CMDNAME_194			"Set Tx Mono Stereo Mode\n"
#define FM_CMDDESC_194			"\t(Mode=0)\n"\
"\t\tMode: 0=Mono; 1=Stereo. Default 0.\n"

#define FM_CMDNAME_195			"Set Tx Mute Mode\n"
#define FM_CMDDESC_195			"\t(Mute=0)\n"\
			"\t\tmode: Mute Mode. 0 = OFF, 1 = ON. Default 0.\n" 

#define FM_CMDNAME_196			"Configure FM Audio Input\n"
#define FM_CMDDESC_196			"\t(InputGain=1) (ActiveAudioCtrl=3)\n"\
			"\t\tInputGain: 0=2.5,	1=1.25, 2=0.833. Default=1\n"\
			"\t\tActiveAudioCtrl: 1=R-ONLY,	2=L-ONLY,	3=STEREO.\n\t\t\tDefault=3\n"

#define FM_CMDNAME_197			"Enable Tx Audio AGC\n"
#define FM_CMDDESC_197			"\t(Enable=0)\n"\
"\t\tEnable: 0=OFF, 1=ON. Default 0.\n"


 

#define FM_CMDNAME_198			"Frequency Deviation Extended\n"
#define FM_CMDDESC_198			"\t (LPR) (LMR) (Pilot) (Rds)\n"\
			"\t\tLPR deviation, 32-bit. Default=22500.\n"\
			"\t\tLMR deviation, 32-bit. Default=22500.\n"\
			"\t\tPilot deviation, 32-bit. Default=6750.\n"\
			"\t\tRds deviation, 32-bit. Default=2000.\n"

#define FM_CMDNAME_199			CMDNAME_UNKNOWN	// "Get Tx Power Mode\n"
#define FM_CMDDESC_199			CMDDESC_BLANK	// "\tTx Power Mode : 0=High, 1=Low\n"

#define FM_CMDNAME_200			"Set Tx Power Mode\n"
#define FM_CMDDESC_200			"\t(mode)\n"\
"\tTx Power Mode : 0=High, 1=Low. Default 0. \n"

#define FM_CMDNAME_201			"Set Tx Rds PI Code\n"
#define FM_CMDDESC_201			"\t(PiCode=1234)\n"\
"\t\tPiCode: PI Code, hex. Default=1234.\n"

#define FM_CMDNAME_202			"Set Tx Rds Group\n"
#define FM_CMDDESC_202			"\t(Group=0)\n"\
"\t\tGroup: 0=PS0A, 1=PS0B, 2=RT2A, 3=RT2B. Default=0.\n"

#define FM_CMDNAME_203			"Set Tx Rds PTY\n"
#define FM_CMDDESC_203			"\t(Pty=0)\n"\
"\t\tPty: valid range 0-1F. Default=0.\n"

#define FM_CMDNAME_204			"Set Tx Rds AF Code\n"
#define FM_CMDDESC_204			"\t(Code=202)\n"\
"\t\tCode: decial, max=255. Default=202.\n"

#define FM_CMDNAME_205			"Set Tx Rds Mode\n"
#define FM_CMDDESC_205			"\t(mode=0)\n"\
"\t\tmode: RDS Mode, 0=PS_RT, 1=RAW. Default=0.\n"

#define FM_CMDNAME_206			"Set Tx Rds Scrolling\n"
#define FM_CMDDESC_206			"\t(mode=0)\n"\
"\t\tmode : enable. 0=OFF, 1=ON. Default 0.\n"

#define FM_CMDNAME_207			"Set Tx Rds Scrolling Rate in chars/scroll\n"
#define FM_CMDDESC_207			"\t(rate=1)\n"\
"\t\tRate: Scrolling Rate in chars/scroll. Decial, max=255. Default=1\n"

#define FM_CMDNAME_208			"Set Tx Rds Display Length in chars\n"
#define FM_CMDDESC_208			"\t(Len=8)\n"\
"\t\tLen: Display Length in chars, decial, max=255. Default=8\n"

#define FM_CMDNAME_209			"Set Tx Rds Toggle AB\n"
#define FM_CMDDESC_209			CMDDESC_BLANK

#define FM_CMDNAME_210			"Set Tx Rds Repository\n"
#define FM_CMDDESC_210			"\t(mode=0)\n"\
"\t\tmode: Repository Mode, 0=G0, 1=G1, 2=G2. Default=0.\n"

#define FM_CMDNAME_211			CMDNAME_UNKNOWN
#define FM_CMDDESC_211			CMDDESC_BLANK

#define FM_CMDNAME_212			CMDNAME_UNKNOWN
#define FM_CMDDESC_212			CMDDESC_BLANK

#define FM_CMDNAME_213			CMDNAME_UNKNOWN
#define FM_CMDDESC_213			CMDDESC_BLANK

#define FM_CMDNAME_214			CMDNAME_UNKNOWN
#define FM_CMDDESC_214			CMDDESC_BLANK

#define FM_CMDNAME_215			CMDNAME_UNKNOWN
#define FM_CMDDESC_215			CMDDESC_BLANK

#define FM_CMDNAME_216			CMDNAME_UNKNOWN
#define FM_CMDDESC_216			CMDDESC_BLANK

#define FM_CMDNAME_217			CMDNAME_UNKNOWN
#define FM_CMDDESC_217			CMDDESC_BLANK

#define FM_CMDNAME_218			CMDNAME_UNKNOWN
#define FM_CMDDESC_218			CMDDESC_BLANK

#define FM_CMDNAME_219			CMDNAME_UNKNOWN
#define FM_CMDDESC_219			CMDDESC_BLANK

// ==================================
// === RDS Block Error measurment ===
// ==================================

#define FM_CMDNAME_220			"Reset RDS stat\n"	//CMDNAME_UNKNOWN
#define FM_CMDDESC_220			"\t(period)\n"\
"\t\tperiod : period counter limit. 32 bit.\n" 

#define FM_CMDNAME_221			"Get RDS stat\n"		// CMDNAME_UNKNOWN
#define FM_CMDDESC_221			CMDDESC_BLANK

#define FM_CMDNAME_222			CMDNAME_UNKNOWN
#define FM_CMDDESC_222			CMDDESC_BLANK

#define FM_CMDNAME_223			CMDNAME_UNKNOWN
#define FM_CMDDESC_223			CMDDESC_BLANK

#define FM_CMDNAME_224			CMDNAME_UNKNOWN
#define FM_CMDDESC_224			CMDDESC_BLANK

#define FM_CMDNAME_225			CMDNAME_UNKNOWN
#define FM_CMDDESC_225			CMDDESC_BLANK

#define FM_CMDNAME_226			CMDNAME_UNKNOWN
#define FM_CMDDESC_226			CMDDESC_BLANK

#define FM_CMDNAME_227			CMDNAME_UNKNOWN
#define FM_CMDDESC_227			CMDDESC_BLANK

#define FM_CMDNAME_228			CMDNAME_UNKNOWN
#define FM_CMDDESC_228			CMDDESC_BLANK

#define FM_CMDNAME_229			CMDNAME_UNKNOWN
#define FM_CMDDESC_229			CMDDESC_BLANK

#define FM_CMDNAME_230			CMDNAME_UNKNOWN
#define FM_CMDDESC_230			CMDDESC_BLANK

#define FM_CMDNAME_231			CMDNAME_UNKNOWN
#define FM_CMDDESC_231			CMDDESC_BLANK

#define FM_CMDNAME_232			CMDNAME_UNKNOWN
#define FM_CMDDESC_232			CMDDESC_BLANK

#define FM_CMDNAME_233			CMDNAME_UNKNOWN
#define FM_CMDDESC_233			CMDDESC_BLANK

#define FM_CMDNAME_234			CMDNAME_UNKNOWN
#define FM_CMDDESC_234			CMDDESC_BLANK

#define FM_CMDNAME_235			CMDNAME_UNKNOWN
#define FM_CMDDESC_235			CMDDESC_BLANK

#define FM_CMDNAME_236			CMDNAME_UNKNOWN
#define FM_CMDDESC_236			CMDDESC_BLANK

#define FM_CMDNAME_237			CMDNAME_UNKNOWN
#define FM_CMDDESC_237			CMDDESC_BLANK

#define FM_CMDNAME_238			CMDNAME_UNKNOWN
#define FM_CMDDESC_238			CMDDESC_BLANK

#define FM_CMDNAME_239			CMDNAME_UNKNOWN
#define FM_CMDDESC_239			CMDDESC_BLANK

#define FM_CMDNAME_240			CMDNAME_UNKNOWN
#define FM_CMDDESC_240			CMDDESC_BLANK

#define FM_CMDNAME_241			CMDNAME_UNKNOWN
#define FM_CMDDESC_241			CMDDESC_BLANK

#define FM_CMDNAME_242			CMDNAME_UNKNOWN
#define FM_CMDDESC_242			CMDDESC_BLANK

#define FM_CMDNAME_243			CMDNAME_UNKNOWN
#define FM_CMDDESC_243			CMDDESC_BLANK

#define FM_CMDNAME_244			CMDNAME_UNKNOWN
#define FM_CMDDESC_244			CMDDESC_BLANK

#define FM_CMDNAME_245			CMDNAME_UNKNOWN
#define FM_CMDDESC_245			CMDDESC_BLANK

#define FM_CMDNAME_246			CMDNAME_UNKNOWN
#define FM_CMDDESC_246			CMDDESC_BLANK

#define FM_CMDNAME_247			CMDNAME_UNKNOWN
#define FM_CMDDESC_247			CMDDESC_BLANK

#define FM_CMDNAME_248			CMDNAME_UNKNOWN
#define FM_CMDDESC_248			CMDDESC_BLANK

#define FM_CMDNAME_249			CMDNAME_UNKNOWN
#define FM_CMDDESC_249			CMDDESC_BLANK

#define FM_CMDNAME_250			CMDNAME_UNKNOWN
#define FM_CMDDESC_250			CMDDESC_BLANK

#define FM_CMDNAME_251			CMDNAME_UNKNOWN
#define FM_CMDDESC_251			CMDDESC_BLANK

#define FM_CMDNAME_252			CMDNAME_UNKNOWN
#define FM_CMDDESC_252			CMDDESC_BLANK

#define FM_CMDNAME_253			CMDNAME_UNKNOWN
#define FM_CMDDESC_253			CMDDESC_BLANK

#define FM_CMDNAME_254			CMDNAME_UNKNOWN
#define FM_CMDDESC_254			CMDDESC_BLANK

#define FM_CMDNAME_255			CMDNAME_UNKNOWN
#define FM_CMDDESC_255			CMDDESC_BLANK

#define FM_CMDNAME_256			CMDNAME_UNKNOWN
#define FM_CMDDESC_256			CMDDESC_BLANK

#define FM_CMDNAME_257			CMDNAME_UNKNOWN
#define FM_CMDDESC_257			CMDDESC_BLANK

#define FM_CMDNAME_258			CMDNAME_UNKNOWN
#define FM_CMDDESC_258			CMDDESC_BLANK

#define FM_CMDNAME_259			CMDNAME_UNKNOWN
#define FM_CMDDESC_259			CMDDESC_BLANK

 
/*
#define FM_CMDNAME_0			CMDNAME_UNKNOWN
#define FM_CMDDESC_0			CMDDESC_BLANK

#define FM_CMDNAME_1			CMDNAME_UNKNOWN
#define FM_CMDDESC_1			CMDDESC_BLANK

#define FM_CMDNAME_2			CMDNAME_UNKNOWN
#define FM_CMDDESC_2			CMDDESC_BLANK

#define FM_CMDNAME_3			CMDNAME_UNKNOWN
#define FM_CMDDESC_3			CMDDESC_BLANK

#define FM_CMDNAME_4			CMDNAME_UNKNOWN
#define FM_CMDDESC_4			CMDDESC_BLANK

#define FM_CMDNAME_5			CMDNAME_UNKNOWN
#define FM_CMDDESC_5			CMDDESC_BLANK

#define FM_CMDNAME_6			CMDNAME_UNKNOWN
#define FM_CMDDESC_6			CMDDESC_BLANK

#define FM_CMDNAME_7			CMDNAME_UNKNOWN
#define FM_CMDDESC_7			CMDDESC_BLANK

#define FM_CMDNAME_8			CMDNAME_UNKNOWN
#define FM_CMDDESC_8			CMDDESC_BLANK

#define FM_CMDNAME_9			CMDNAME_UNKNOWN
#define FM_CMDDESC_9			CMDDESC_BLANK



typedef struct  CmdMenu
{
	char       *pCmdName;
	char       *pCmdDescriptor;
} CmdMenu;
*/
CmdMenu FmCmdNames[256]=
{
	{FM_CMDNAME_0, FM_CMDDESC_0},
	{FM_CMDNAME_1, FM_CMDDESC_1},
	{FM_CMDNAME_2, FM_CMDDESC_2},
	{FM_CMDNAME_3, FM_CMDDESC_3},
	{FM_CMDNAME_4, FM_CMDDESC_4},
	{FM_CMDNAME_5, FM_CMDDESC_5},
	{FM_CMDNAME_6, FM_CMDDESC_6},
	{FM_CMDNAME_7, FM_CMDDESC_7},
	{FM_CMDNAME_8, FM_CMDDESC_8},
	{FM_CMDNAME_9, FM_CMDDESC_9},

	{FM_CMDNAME_10, FM_CMDDESC_10},
	{FM_CMDNAME_11, FM_CMDDESC_11},
	{FM_CMDNAME_12, FM_CMDDESC_12},
	{FM_CMDNAME_13, FM_CMDDESC_13},
	{FM_CMDNAME_14, FM_CMDDESC_14},
	{FM_CMDNAME_15, FM_CMDDESC_15},
	{FM_CMDNAME_16, FM_CMDDESC_16},
	{FM_CMDNAME_17, FM_CMDDESC_17},
	{FM_CMDNAME_18, FM_CMDDESC_18},
	{FM_CMDNAME_19, FM_CMDDESC_19},
	
	{FM_CMDNAME_20, FM_CMDDESC_20},
	{FM_CMDNAME_21, FM_CMDDESC_21},
	{FM_CMDNAME_22, FM_CMDDESC_22},
	{FM_CMDNAME_23, FM_CMDDESC_23},
	{FM_CMDNAME_24, FM_CMDDESC_24},
	{FM_CMDNAME_25, FM_CMDDESC_25},
	{FM_CMDNAME_26, FM_CMDDESC_26},
	{FM_CMDNAME_27, FM_CMDDESC_27},
	{FM_CMDNAME_28, FM_CMDDESC_28},
	{FM_CMDNAME_29, FM_CMDDESC_29},

	{FM_CMDNAME_30, FM_CMDDESC_30},
	{FM_CMDNAME_31, FM_CMDDESC_31},
	{FM_CMDNAME_32, FM_CMDDESC_32},
	{FM_CMDNAME_33, FM_CMDDESC_33},
	{FM_CMDNAME_34, FM_CMDDESC_34},
	{FM_CMDNAME_35, FM_CMDDESC_35},
	{FM_CMDNAME_36, FM_CMDDESC_36},
	{FM_CMDNAME_37, FM_CMDDESC_37},
	{FM_CMDNAME_38, FM_CMDDESC_38},
	{FM_CMDNAME_39, FM_CMDDESC_39},

	{FM_CMDNAME_40, FM_CMDDESC_40},
	{FM_CMDNAME_41, FM_CMDDESC_41},
	{FM_CMDNAME_42, FM_CMDDESC_42},
	{FM_CMDNAME_43, FM_CMDDESC_43},
	{FM_CMDNAME_44, FM_CMDDESC_44},
	{FM_CMDNAME_45, FM_CMDDESC_45},
	{FM_CMDNAME_46, FM_CMDDESC_46},
	{FM_CMDNAME_47, FM_CMDDESC_47},
	{FM_CMDNAME_48, FM_CMDDESC_48},
	{FM_CMDNAME_49, FM_CMDDESC_49},

	{FM_CMDNAME_50, FM_CMDDESC_50},
	{FM_CMDNAME_51, FM_CMDDESC_51},
	{FM_CMDNAME_52, FM_CMDDESC_52},
	{FM_CMDNAME_53, FM_CMDDESC_53},
	{FM_CMDNAME_54, FM_CMDDESC_54},
	{FM_CMDNAME_55, FM_CMDDESC_55},
	{FM_CMDNAME_56, FM_CMDDESC_56},
	{FM_CMDNAME_57, FM_CMDDESC_57},
	{FM_CMDNAME_58, FM_CMDDESC_58},
	{FM_CMDNAME_59, FM_CMDDESC_59},

	{FM_CMDNAME_60, FM_CMDDESC_60},
	{FM_CMDNAME_61, FM_CMDDESC_61},
	{FM_CMDNAME_62, FM_CMDDESC_62},
	{FM_CMDNAME_63, FM_CMDDESC_63},
	{FM_CMDNAME_64, FM_CMDDESC_64},
	{FM_CMDNAME_65, FM_CMDDESC_65},
	{FM_CMDNAME_66, FM_CMDDESC_66},
	{FM_CMDNAME_67, FM_CMDDESC_67},
	{FM_CMDNAME_68, FM_CMDDESC_68},
	{FM_CMDNAME_69, FM_CMDDESC_69},

	{FM_CMDNAME_70, FM_CMDDESC_70},
	{FM_CMDNAME_71, FM_CMDDESC_71},
	{FM_CMDNAME_72, FM_CMDDESC_72},
	{FM_CMDNAME_73, FM_CMDDESC_73},
	{FM_CMDNAME_74, FM_CMDDESC_74},
	{FM_CMDNAME_75, FM_CMDDESC_75},
	{FM_CMDNAME_76, FM_CMDDESC_76},
	{FM_CMDNAME_77, FM_CMDDESC_77},
	{FM_CMDNAME_78, FM_CMDDESC_78},
	{FM_CMDNAME_79, FM_CMDDESC_79},

	{FM_CMDNAME_80, FM_CMDDESC_80},
	{FM_CMDNAME_81, FM_CMDDESC_81},
	{FM_CMDNAME_82, FM_CMDDESC_82},
	{FM_CMDNAME_83, FM_CMDDESC_83},
	{FM_CMDNAME_84, FM_CMDDESC_84},
	{FM_CMDNAME_85, FM_CMDDESC_85},
	{FM_CMDNAME_86, FM_CMDDESC_86},
	{FM_CMDNAME_87, FM_CMDDESC_87},
	{FM_CMDNAME_88, FM_CMDDESC_88},
	{FM_CMDNAME_89, FM_CMDDESC_89},

	{FM_CMDNAME_90, FM_CMDDESC_90},
	{FM_CMDNAME_91, FM_CMDDESC_91},
	{FM_CMDNAME_92, FM_CMDDESC_92},
	{FM_CMDNAME_93, FM_CMDDESC_93},
	{FM_CMDNAME_94, FM_CMDDESC_94},
	{FM_CMDNAME_95, FM_CMDDESC_95},
	{FM_CMDNAME_96, FM_CMDDESC_96},
	{FM_CMDNAME_97, FM_CMDDESC_97},
	{FM_CMDNAME_98, FM_CMDDESC_98},
	{FM_CMDNAME_99, FM_CMDDESC_99},

	{FM_CMDNAME_100, FM_CMDDESC_100},
	{FM_CMDNAME_101, FM_CMDDESC_101},
	{FM_CMDNAME_102, FM_CMDDESC_102},
	{FM_CMDNAME_103, FM_CMDDESC_103},
	{FM_CMDNAME_104, FM_CMDDESC_104},
	{FM_CMDNAME_105, FM_CMDDESC_105},
	{FM_CMDNAME_106, FM_CMDDESC_106},
	{FM_CMDNAME_107, FM_CMDDESC_107},
	{FM_CMDNAME_108, FM_CMDDESC_108},
	{FM_CMDNAME_109, FM_CMDDESC_109},

	{FM_CMDNAME_110, FM_CMDDESC_110},
	{FM_CMDNAME_111, FM_CMDDESC_111},
	{FM_CMDNAME_112, FM_CMDDESC_112},
	{FM_CMDNAME_113, FM_CMDDESC_113},
	{FM_CMDNAME_114, FM_CMDDESC_114},
	{FM_CMDNAME_115, FM_CMDDESC_115},
	{FM_CMDNAME_116, FM_CMDDESC_116},
	{FM_CMDNAME_117, FM_CMDDESC_117},
	{FM_CMDNAME_118, FM_CMDDESC_118},
	{FM_CMDNAME_119, FM_CMDDESC_119},

	{FM_CMDNAME_120, FM_CMDDESC_120},
	{FM_CMDNAME_121, FM_CMDDESC_121},
	{FM_CMDNAME_122, FM_CMDDESC_122},
	{FM_CMDNAME_123, FM_CMDDESC_123},
	{FM_CMDNAME_124, FM_CMDDESC_124},
	{FM_CMDNAME_125, FM_CMDDESC_125},
	{FM_CMDNAME_126, FM_CMDDESC_126},
	{FM_CMDNAME_127, FM_CMDDESC_127},
	{FM_CMDNAME_128, FM_CMDDESC_128},
	{FM_CMDNAME_129, FM_CMDDESC_129},

	{FM_CMDNAME_130, FM_CMDDESC_130},
	{FM_CMDNAME_131, FM_CMDDESC_131},
	{FM_CMDNAME_132, FM_CMDDESC_132},
	{FM_CMDNAME_133, FM_CMDDESC_133},
	{FM_CMDNAME_134, FM_CMDDESC_134},
	{FM_CMDNAME_135, FM_CMDDESC_135},
	{FM_CMDNAME_136, FM_CMDDESC_136},
	{FM_CMDNAME_137, FM_CMDDESC_137},
	{FM_CMDNAME_138, FM_CMDDESC_138},
	{FM_CMDNAME_139, FM_CMDDESC_139},

	{FM_CMDNAME_140, FM_CMDDESC_140},
	{FM_CMDNAME_141, FM_CMDDESC_141},
	{FM_CMDNAME_142, FM_CMDDESC_142},
	{FM_CMDNAME_143, FM_CMDDESC_143},
	{FM_CMDNAME_144, FM_CMDDESC_144},
	{FM_CMDNAME_145, FM_CMDDESC_145},
	{FM_CMDNAME_146, FM_CMDDESC_146},
	{FM_CMDNAME_147, FM_CMDDESC_147},
	{FM_CMDNAME_148, FM_CMDDESC_148},
	{FM_CMDNAME_149, FM_CMDDESC_149},
	

	{FM_CMDNAME_150, FM_CMDDESC_150},
	{FM_CMDNAME_151, FM_CMDDESC_151},
	{FM_CMDNAME_152, FM_CMDDESC_152},
	{FM_CMDNAME_153, FM_CMDDESC_153},
	{FM_CMDNAME_154, FM_CMDDESC_154},
	{FM_CMDNAME_155, FM_CMDDESC_155},
	{FM_CMDNAME_156, FM_CMDDESC_156},
	{FM_CMDNAME_157, FM_CMDDESC_157},
	{FM_CMDNAME_158, FM_CMDDESC_158},
	{FM_CMDNAME_159, FM_CMDDESC_159},
	

	{FM_CMDNAME_160, FM_CMDDESC_160},
	{FM_CMDNAME_161, FM_CMDDESC_161},
	{FM_CMDNAME_162, FM_CMDDESC_162},
	{FM_CMDNAME_163, FM_CMDDESC_163},
	{FM_CMDNAME_164, FM_CMDDESC_164},
	{FM_CMDNAME_165, FM_CMDDESC_165},
	{FM_CMDNAME_166, FM_CMDDESC_166},
	{FM_CMDNAME_167, FM_CMDDESC_167},
	{FM_CMDNAME_168, FM_CMDDESC_168},
	{FM_CMDNAME_169, FM_CMDDESC_169},
	

	{FM_CMDNAME_170, FM_CMDDESC_170},
	{FM_CMDNAME_171, FM_CMDDESC_171},
	{FM_CMDNAME_172, FM_CMDDESC_172},
	{FM_CMDNAME_173, FM_CMDDESC_173},
	{FM_CMDNAME_174, FM_CMDDESC_174},
	{FM_CMDNAME_175, FM_CMDDESC_175},
	{FM_CMDNAME_176, FM_CMDDESC_176},
	{FM_CMDNAME_177, FM_CMDDESC_177},
	{FM_CMDNAME_178, FM_CMDDESC_178},
	{FM_CMDNAME_179, FM_CMDDESC_179},
	

	{FM_CMDNAME_180, FM_CMDDESC_180},
	{FM_CMDNAME_181, FM_CMDDESC_181},
	{FM_CMDNAME_182, FM_CMDDESC_182},
	{FM_CMDNAME_183, FM_CMDDESC_183},
	{FM_CMDNAME_184, FM_CMDDESC_184},
	{FM_CMDNAME_185, FM_CMDDESC_185},
	{FM_CMDNAME_186, FM_CMDDESC_186},
	{FM_CMDNAME_187, FM_CMDDESC_187},
	{FM_CMDNAME_188, FM_CMDDESC_188},
	{FM_CMDNAME_189, FM_CMDDESC_189},
	

	{FM_CMDNAME_190, FM_CMDDESC_190},
	{FM_CMDNAME_191, FM_CMDDESC_191},
	{FM_CMDNAME_192, FM_CMDDESC_192},
	{FM_CMDNAME_193, FM_CMDDESC_193},
	{FM_CMDNAME_194, FM_CMDDESC_194},
	{FM_CMDNAME_195, FM_CMDDESC_195},
	{FM_CMDNAME_196, FM_CMDDESC_196},
	{FM_CMDNAME_197, FM_CMDDESC_197},
	{FM_CMDNAME_198, FM_CMDDESC_198},
	{FM_CMDNAME_199, FM_CMDDESC_199},
	
	{FM_CMDNAME_200, FM_CMDDESC_200},
	{FM_CMDNAME_201, FM_CMDDESC_201},
	{FM_CMDNAME_202, FM_CMDDESC_202},
	{FM_CMDNAME_203, FM_CMDDESC_203},
	{FM_CMDNAME_204, FM_CMDDESC_204},
	{FM_CMDNAME_205, FM_CMDDESC_205},
	{FM_CMDNAME_206, FM_CMDDESC_206},
	{FM_CMDNAME_207, FM_CMDDESC_207},
	{FM_CMDNAME_208, FM_CMDDESC_208},
	{FM_CMDNAME_209, FM_CMDDESC_209},
	
	{FM_CMDNAME_210, FM_CMDDESC_210},
	{FM_CMDNAME_211, FM_CMDDESC_211},
	{FM_CMDNAME_212, FM_CMDDESC_212},
	{FM_CMDNAME_213, FM_CMDDESC_213},
	{FM_CMDNAME_214, FM_CMDDESC_214},
	{FM_CMDNAME_215, FM_CMDDESC_215},
	{FM_CMDNAME_216, FM_CMDDESC_216},
	{FM_CMDNAME_217, FM_CMDDESC_217},
	{FM_CMDNAME_218, FM_CMDDESC_218},
	{FM_CMDNAME_219, FM_CMDDESC_219},
	
	{FM_CMDNAME_220, FM_CMDDESC_220},
	{FM_CMDNAME_221, FM_CMDDESC_221},
	{FM_CMDNAME_222, FM_CMDDESC_222},
	{FM_CMDNAME_223, FM_CMDDESC_223},
	{FM_CMDNAME_224, FM_CMDDESC_224},
	{FM_CMDNAME_225, FM_CMDDESC_225},
	{FM_CMDNAME_226, FM_CMDDESC_226},
	{FM_CMDNAME_227, FM_CMDDESC_227},
	{FM_CMDNAME_228, FM_CMDDESC_228},
	{FM_CMDNAME_229, FM_CMDDESC_229},
	
	{FM_CMDNAME_230, FM_CMDDESC_230},
	{FM_CMDNAME_231, FM_CMDDESC_231},
	{FM_CMDNAME_232, FM_CMDDESC_232},
	{FM_CMDNAME_233, FM_CMDDESC_233},
	{FM_CMDNAME_234, FM_CMDDESC_234},
	{FM_CMDNAME_235, FM_CMDDESC_235},
	{FM_CMDNAME_236, FM_CMDDESC_236},
	{FM_CMDNAME_237, FM_CMDDESC_237},
	{FM_CMDNAME_238, FM_CMDDESC_238},
	{FM_CMDNAME_239, FM_CMDDESC_239},
	
	{FM_CMDNAME_240, FM_CMDDESC_240},
	{FM_CMDNAME_241, FM_CMDDESC_241},
	{FM_CMDNAME_242, FM_CMDDESC_242},
	{FM_CMDNAME_243, FM_CMDDESC_243},
	{FM_CMDNAME_244, FM_CMDDESC_244},
	{FM_CMDNAME_245, FM_CMDDESC_245},
	{FM_CMDNAME_246, FM_CMDDESC_246},
	{FM_CMDNAME_247, FM_CMDDESC_247},
	{FM_CMDNAME_248, FM_CMDDESC_248},
	{FM_CMDNAME_249, FM_CMDDESC_249},
	
	{FM_CMDNAME_250, FM_CMDDESC_250},
	{FM_CMDNAME_251, FM_CMDDESC_251},
	{FM_CMDNAME_252, FM_CMDDESC_252},
	{FM_CMDNAME_253, FM_CMDDESC_253},
	{FM_CMDNAME_254, FM_CMDDESC_254}, 


	// last cmd
	{CMDNAME_LAST, CMDDESC_BLANK}
};
