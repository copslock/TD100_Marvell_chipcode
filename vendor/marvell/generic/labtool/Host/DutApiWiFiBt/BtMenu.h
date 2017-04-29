/** @file BtMenu.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#include "../DutCommon/MenuDisplay.h"



#define BT_CMDNAME_0			"-----------------------------------------------------------------------\n"\
"\t\tW87xx (BT) TEST MENU\n-----------------------------------------------------------------------\n"
#define BT_CMDDESC_0			CMDDESC_BLANK

#define BT_CMDNAME_1			CMDNAME_UNKNOWN
#define BT_CMDDESC_1			CMDDESC_BLANK

#define BT_CMDNAME_2			CMDNAME_UNKNOWN
#define BT_CMDDESC_2			CMDDESC_BLANK

#define BT_CMDNAME_3			CMDNAME_UNKNOWN
#define BT_CMDDESC_3			CMDDESC_BLANK

#define BT_CMDNAME_4			CMDNAME_UNKNOWN
#define BT_CMDDESC_4			CMDDESC_BLANK

#define BT_CMDNAME_5			CMDNAME_UNKNOWN
#define BT_CMDDESC_5			CMDDESC_BLANK

#define BT_CMDNAME_6			CMDNAME_UNKNOWN
#define BT_CMDDESC_6			CMDDESC_BLANK

#define BT_CMDNAME_7			CMDNAME_UNKNOWN
#define BT_CMDDESC_7			CMDDESC_BLANK

#define BT_CMDNAME_8			CMDNAME_UNKNOWN
#define BT_CMDDESC_8			CMDDESC_BLANK

#if defined(_W8780_)

#define BT_CMDNAME_9			"Get TRSW Antenna\n"
#define BT_CMDDESC_9			CMDDESC_BLANK

#define BT_CMDNAME_10			"Set TRSW Antenna\n"
#define BT_CMDDESC_10			"\t(Antenna Option: 0 for MAIN; 1 for AUX)\n"\
"\t\tExample: 10 0\n"

#else

#define BT_CMDNAME_9			CMDNAME_UNKNOWN
#define BT_CMDDESC_9			CMDDESC_BLANK

#define BT_CMDNAME_10			CMDNAME_UNKNOWN
#define BT_CMDDESC_10			CMDDESC_BLANK

#endif // #if defined(_W8780_)

#define BT_CMDNAME_11			"Get BT Channel\n"
#define BT_CMDDESC_11			"\t\tBT2: Flag for BT 2nd channel. Default =0 (false)\n"\
"\t\tExample: 11 0\n"


#define BT_CMDNAME_12			"Set BT Channel\n"
#define BT_CMDDESC_12			"\t(Channel=0) (BT2=0)\n"\
"\t\tChannel: Channel Number (valid number 0-78))\n"\
"\t\tBT2: Flag for BT 2nd channel. Default =0 (false)\n"\
"\t\tExample: 12 40 0\n"

#define BT_CMDNAME_13			"Set BT Channel by given Frequency\n"
#define BT_CMDDESC_13			"\t(Channel-Freq-In-MHz) (BT2=0)\n"\
"\t\tChannel-Freq-In-MHz: Channel Frequency In MHz)\n"\
"\t\tBT2: Flag for BT 2nd channel. Default =0 (false)\n"\
"\t\tExample: 13 2412 0\n"


#if defined(_W8688_)
#define BT_CMDNAME_14			"FE BTonly mode\n"
#define BT_CMDDESC_14			CMDDESC_BLANK
#else	// #if defined(_W8688_)
#define BT_CMDNAME_14			CMDNAME_UNKNOWN
#define BT_CMDDESC_14			CMDDESC_BLANK
#endif	// #if defined(_W8688_)

#define BT_CMDNAME_15			"Get Power Level Value\n"
#define BT_CMDDESC_15			"\t\treturns Power Level Value in 0.5dB steps.\n"\
"\t\tExample: 15\n"

#define BT_CMDNAME_16			"Set Power Level Value\n"
#define BT_CMDDESC_16			"\t(pwr) (IsEDR=0)\n"\
"\t\tPower Level in 0.5dB steps \n"\
"\t\tIsEDR: Flag to spcify Data Rate. 0(BDR) 1(EDR) \n"\
"\t\tExample: 16 2.5 0\n"

#if 0
#define BT_CMDNAME_17			CMDNAME_UNKNOWN
#define BT_CMDDESC_17			CMDDESC_BLANK
#else
#define BT_CMDNAME_17			"Continuous Tx\n"
#define BT_CMDDESC_17			"\t(Enable=0) (DataRate(0x)=10)\n"\
			"\t(PayloadPattern=2)\n"\
			"\tEnable: 0: off; 1: on\n"\
			"\tDataRate: 1 = 1M (GFSK); 2 = 2M (DQPSK); 3 = 3M (8PSK)\n"\
			"\tPayloadPattern: 0: all 0, 1: all 1, 2: PN9, 3: 0xAA, 4: 0xF0\n"\
 			"\t\tExample: 17 1 3 2\n"
#endif

#if 0
#define BT_CMDNAME_18			CMDNAME_UNKNOWN
#define BT_CMDDESC_18			CMDDESC_BLANK
#else
#define BT_CMDNAME_18			"Tx Cw\n"
#define BT_CMDDESC_18			"\t(Enable)\n"\
	"\t\tEnable: 0: off; 1: on\n"\
"\t\tExample: 18 1\n"
#endif

#define BT_CMDNAME_19			CMDNAME_UNKNOWN
#define BT_CMDDESC_19			CMDDESC_BLANK


#define BT_CMDNAME_20			"Get Power Level Max Min Value\n"
#define BT_CMDDESC_20			"\t\treturns Power Level  Max Min Value in 0.5dB steps.\n"\
"\t\tExample: 20\n"

//#define BT_CMDNAME_21			"Set Power Level Max Min Value\n"
//#define BT_CMDDESC_21			"\t\tPower Level Max Min Value in 0.5dB steps \n"\
//"\t\tmax: Max Value, (double)\t\tmin: Min Value, (double).\n"\
//"\t\tExample: 21 4 -6\n"

#define BT_CMDNAME_21			"Step Power Level\n"
#define BT_CMDDESC_21			"\t(step)\n"\
"\t\tStep Power Level in 0.5dB steps\n"\
"\t\tstep: Step Value, (double).\n"\
"\t\tExample: 21 -2.5\n"

#define BT_CMDNAME_22			CMDNAME_UNKNOWN
#define BT_CMDDESC_22			CMDDESC_BLANK

#define BT_CMDNAME_23			"Get Power Gain\n"
#define BT_CMDDESC_23			"\t\treturns Power Gain register value. 16 bit hex value\n"\
"\t\tExample: 23\n"

#define BT_CMDNAME_24			"Set Power Gain\n"
#define BT_CMDDESC_24			"\t\tSet Power Gain register value. 16-bit hex value\n"\
"\t\tExample: 24 37\n"

#define BT_CMDNAME_25			"Dutycycle Tx\n"
#define BT_CMDDESC_25			"\t(Enable=0) (DataRate=1)\n"\
			"\t(PayloadPattern=2) (DutyWeight=95) (PayloadOnly=0)\n"\
			"\tEnable: 0: off; 1: on\n"\
			"\tDataRate: 1 = 1M (GFSK); 2 = 2M (DQPSK); 3 = 3M (8PSK)\n"\
			"\tPayloadPattern: 0: all 0, 1: all 1, 2: PN9, 3: 0xAA, 4: 0xF0\n"\
			"\tDutyWeight: On-Time percentage. Default 95. (Range:2 - 98)\n"\
			"\tPayloadOnly: tx Payload only, no header (true, false)\n"\
			"\t\tExample: 25 1 3 2 95 0\n"
/* 
			"\t\t\t  DM1 = 0x01;   DM3 = 0x03;   DM5 = 0x05; (GFSK, 1M FEC)\n"\
			"\t\t\t  DH1 = 0x11;   DH3 = 0x13;   DH5 = 0x15; (GFSK, 1M)\n"\
			"\t\t\t2-DH1 = 0x21; 2-DH3 = 0x23; 2-DH5 = 0x25; (DQPSK, 2M)\n"\
			"\t\t\t3-DH1 = 0x31; 3-DH3 = 0x33; 3-DH5 = 0x35; (8PSK, 3M)\n"\
#define BT_CMDNAME_25			CMDNAME_UNKNOWN
#define BT_CMDDESC_25			CMDDESC_BLANK
*/

#define BT_CMDNAME_26			CMDNAME_UNKNOWN
#define BT_CMDDESC_26			CMDDESC_BLANK

#define BT_CMDNAME_27			CMDNAME_UNKNOWN
#define BT_CMDDESC_27			CMDDESC_BLANK

#define BT_CMDNAME_28			CMDNAME_UNKNOWN
#define BT_CMDDESC_28			CMDDESC_BLANK

#define BT_CMDNAME_29			CMDNAME_UNKNOWN
#define BT_CMDDESC_29			CMDDESC_BLANK

#ifdef _HCI_PROTOCAL_
#define BT_CMDNAME_30			CMDNAME_UNKNOWN
#define BT_CMDDESC_30			CMDDESC_BLANK
#else	// #ifdef _HCI_PROTOCAL_
#define BT_CMDNAME_30			"Marvell RSSI Report\n"
#define BT_CMDDESC_30			"\t(linkId) (clear)\n"\
								"\t\tlinkId: Link ID. Default 0\n"\
								"\t\tclear: 0=read only, 1=clear. Default 0.\n" 
#endif	// #ifdef _HCI_PROTOCAL_

#ifndef _W8790_
#define BT_CMDNAME_31			"Marvell Rx Result Report\n"
#define BT_CMDDESC_31			CMDDESC_BLANK

#define BT_CMDNAME_32			"Marvell Rx Test\n"
#define BT_CMDDESC_32			"\t(RxChannel=10) (FrameCount=1000) (PayloadLength=-1)\n"\
			"\t(PacketType(0x)=35)\n"\
			"\t(PayloadPattern=2)\n"\
			"\t(TxBdAddress xx-xx-xx-xx-xx-xx=00-00-88-c0-ff-ee)\n"\
			"\n"\
			"\t\tRxChannel : Rx Channel. Default 10.\n"\
			"\t\tFrameCount: number of frames to Rx. Default 1000.\n"\
			"\t\tPayloadLength: PayLoad Length in Byte. -1=max for the \n\t\t\tPacketType.Default -1.\n"\
			"\t\tPacketType(Rate): Default 0x35.\n"\
			"\t\t\t  DM1 = 0x01;   DM3 = 0x03;   DM5 = 0x05; (GFSK, 1MFEC)\n"\
			"\t\t\t  DH1 = 0x11;   DH3 = 0x13;   DH5 = 0x15; (GFSK, 1M)\n"\
			"\t\t\t2-DH1 = 0x21; 2-DH3 = 0x23; 2-DH5 = 0x25; (DQPSK,2M)\n"\
			"\t\t\t3-DH1 = 0x31; 3-DH3 = 0x33; 3-DH5 = 0x35; (8PSK, 3M)\n"\
			"\t\tPayloadPattern: 0: all 0, 1: all 1, 2: PN9, 3: 0xAA, 4: 0xF0.\n\t\t\tDefault 2.\n"\
			"\t\tTxBdAddress: Tx's BD address. Default: 00-00-88-c0-ff-ee.\n"\
			"\t\tExample: 32 40 100 -1 35 2 00-00-88-c0-ff-ee\n"
#else	//#ifndef _W8790_
#define BT_CMDNAME_31			CMDNAME_UNKNOWN
#define BT_CMDDESC_31			CMDDESC_BLANK

#define BT_CMDNAME_32			CMDNAME_UNKNOWN
#define BT_CMDDESC_32			CMDDESC_BLANK
#endif	//#ifndef _W8790_

 


//#define BT_CMDNAME_33			CMDNAME_UNKNOWN
//#define BT_CMDDESC_33			CMDDESC_BLANK

#define BT_CMDNAME_33			"Counted Frame Tx\n"
#define BT_CMDDESC_33			"\t(Count=100) (TxPower=4) (PacketType(0x)=11)\n"\
			"\t(PayloadPattern=2) (PayloadLenInByte=-1) (HopMode=0) (Interval=1)(Whitening=0)\n"\
			"\tCount: Number of Frame to Tx. Default 100\n"\
			"\tTxPower: Tx Power Level in dBm, integer. Default 4 (dBm)\n"\
			"\tPacketType(Rate.Slot):\n"\
			"\t\tTesting Pattern and ACL:\n"\
			"\t\t\t  DM1 = 0x01;   DM3 = 0x03;   DM5 = 0x05; (GFSK, 1M FEC)\n"\
			"\t\t\t  DH1 = 0x11;   DH3 = 0x13;   DH5 = 0x15; (GFSK, 1M)\n"\
			"\t\t\t2-DH1 = 0x21; 2-DH3 = 0x23; 2-DH5 = 0x25; (DQPSK, 2M)\n"\
			"\t\t\t3-DH1 = 0x31; 3-DH3 = 0x33; 3-DH5 = 0x35; (8PSK, 3M)\n"\
\
			"\t\tSco:\n"\
			"\t\t\t  HV1 = 0x11;   HV2 = 0x12;   HV3 = 0x13; (GFSK, 1M)\n"\
			"\t\teSco:\n"\
			"\t\t\t  EV3 = 0x13;   EV4 = 0x14;   EV5 = 0x15; (GFSK, 1M)\n"\
			"\t\t\t2-EV3 = 0x23; 2-EV5 = 0x25;               (DQPSK, 2M)\n"\
			"\t\t\t3-EV3 = 0x33; 3-EV5 = 0x35;               (8PSK, 3M)\n"\
\
			"\tPayloadPattern: 0: all 0, 1: all 1, 2: PN9, 3: 0xAA, 4: 0xF0\n"\
			"\t\t\t5: PRBS ACL, 6: PRBS SCO, 7: PRBS ESCO\n"\
			"\tPayloadLenInByte: PacketType dependent, -1 for max possible.\n"\
			"\tHopMode: HopMode (on=1, random hopping, off=0 fixed channel)\n"\
			"\tWhitening: Whitening the whole packet. (true=1, false=0)\n"





#define BT_CMDNAME_34			CMDNAME_UNKNOWN
#define BT_CMDDESC_34			CMDDESC_BLANK

#define BT_CMDNAME_35			"Reset BRF\n"
#define BT_CMDDESC_35			CMDDESC_BLANK

#define BT_CMDNAME_36			CMDNAME_UNKNOWN
#define BT_CMDDESC_36			CMDDESC_BLANK

#define BT_CMDNAME_37			CMDNAME_UNKNOWN
#define BT_CMDDESC_37			CMDDESC_BLANK

#define BT_CMDNAME_38			CMDNAME_UNKNOWN
#define BT_CMDDESC_38			CMDDESC_BLANK

#if !defined (_WLAN_) && !defined (_NO_EEPROM_)
#ifndef _FLASH_
#define BT_CMDNAME_39			"SPI download from binary file\n"
#define BT_CMDDESC_39			"\t\tWill prompt for file name.\n"
#else //#ifndef _FLASH_
#define BT_CMDNAME_39			CMDNAME_UNKNOWN
#define BT_CMDDESC_39			CMDDESC_BLANK
#endif //#ifndef _FLASH_

#define BT_CMDNAME_40			"SPI Verify from binary file\n"
#define BT_CMDDESC_40			"\t\tWill prompt for file name.\n"

#define BT_CMDNAME_41			"Dump E2PROM content\n"
#define BT_CMDDESC_41			"\t\tAddr: starting address. Default 0.\n\t\tlen: number of DWORDs to be dumpped\n"

#define BT_CMDNAME_42			"Get E2PROM type\n"
#define BT_CMDDESC_42			"\t(if) (addrLen) (device)\n"\
					"\t\tIf:\tinterface type: 0=detect-again, 1=SPI, 2=I2C\n"\
					"\t\taddrLen:\taddress width in byte. valid 1 - 3.\n"\
					"\t\tdevice:\tdevice type: 1=eeprom, 2=flash\n" 

#define BT_CMDNAME_43			"Force E2PROM type\n"
#define BT_CMDDESC_43			"\t(if) (addrLen) (device)\n"\
					"\t\tIf:\tinterface type: 0=detect-again, 1=SPI, 2=I2C\n"\
					"\t\taddrLen:\taddress width in byte. valid 1 - 3.\n"\
					"\t\tdevice:\tdevice type: 1=eeprom, 2=flash\n"
#else   //	#if !defined (_WLAN_) && !defined (_NO_EEPROM_)
#define BT_CMDNAME_39			CMDNAME_UNKNOWN
#define BT_CMDDESC_39			CMDDESC_BLANK

#define BT_CMDNAME_40			CMDNAME_UNKNOWN
#define BT_CMDDESC_40			CMDDESC_BLANK

#define BT_CMDNAME_41			CMDNAME_UNKNOWN
#define BT_CMDDESC_41			CMDDESC_BLANK

#define BT_CMDNAME_42			CMDNAME_UNKNOWN
#define BT_CMDDESC_42			CMDDESC_BLANK

#define BT_CMDNAME_43			CMDNAME_UNKNOWN
#define BT_CMDDESC_43			CMDDESC_BLANK
#endif  //	#if !defined (_WLAN_) && !defined (_NO_EEPROM_)

#define BT_CMDNAME_44			CMDNAME_UNKNOWN
#define BT_CMDDESC_44			CMDDESC_BLANK

#define BT_CMDNAME_45			"Read BD Address From EEPROM/Flash and FW\n"
#define BT_CMDDESC_45			CMDDESC_BLANK

#if defined(_W8780_) || defined (_W8688_) || defined (_W8689_) || defined (_W8787_) || defined(_W8782_) 
#define BT_CMDNAME_46			"Change Bd Address in EEPROM\n"
#define BT_CMDDESC_46			"\t Bd Address.\n"\
"\t\tBd Address in format of 00.52.43.20.ab.cd\n"

#else	// #if defined(_W8780_) || defined (_W8688_) || defined (_W8689_) || defined (_W8787_) || defined(_W8782_)
#define BT_CMDNAME_46			CMDNAME_UNKNOWN
#define BT_CMDDESC_46			CMDDESC_BLANK
#endif	// #if defined(_W8780_) || defined (_W8688_) || defined (_W8689_) || defined (_W8787_) || defined(_W8782_)

#if !defined (_WLAN_) && !defined (_NO_EEPROM_)
#if defined(_IF_USB_) 

#define BT_CMDNAME_47			"Get USB Attribute\n"
#define BT_CMDDESC_47			CMDDESC_BLANK

#define BT_CMDNAME_48			"Set USB Attribute\n"
#define BT_CMDDESC_48			"\t((0x)(Attribute(8-bit)) (MaxPower=0(0 - 250)))\n"
 
#else //#if defined(_IF_USB_) 
#define BT_CMDNAME_47			"Change Uart Rate\n"
#define BT_CMDDESC_47			"\t(Uart-Rate in decimal.)\n"

#define BT_CMDNAME_48			CMDNAME_UNKNOWN
#define BT_CMDDESC_48			CMDDESC_BLANK
#endif //#if defined(_IF_USB_) 

 
#define BT_CMDNAME_49			CMDNAME_UNKNOWN
#define BT_CMDDESC_49			CMDDESC_BLANK

#define BT_CMDNAME_50			CMDNAME_UNKNOWN
#define BT_CMDDESC_50			CMDDESC_BLANK

#ifdef _FLASH_
#define BT_CMDNAME_51			"Erase flash\n"
#define BT_CMDDESC_51			"\t((0x )(section-starting-address)\n"
#else //#ifdef _FLASH_
#define BT_CMDNAME_51			CMDNAME_UNKNOWN
#define BT_CMDDESC_51			CMDDESC_BLANK
#endif //#ifdef _FLASH_

#define BT_CMDNAME_52			CMDNAME_UNKNOWN
#define BT_CMDDESC_52			CMDDESC_BLANK

#define BT_CMDNAME_53			"Set Cal from file:CalBtDataFile.txt.\n"
#define BT_CMDDESC_53			CMDDESC_BLANK

#define BT_CMDNAME_54			"Get Cal from EEPROM/Flash into files.\n"
#define BT_CMDDESC_54			CMDDESC_BLANK

#else // #if !defined (_WLAN_) && !defined (_NO_EEPROM_)

#define BT_CMDNAME_47			CMDNAME_UNKNOWN
#define BT_CMDDESC_47			CMDDESC_BLANK

#define BT_CMDNAME_48			CMDNAME_UNKNOWN
#define BT_CMDDESC_48			CMDDESC_BLANK

#define BT_CMDNAME_49			CMDNAME_UNKNOWN
#define BT_CMDDESC_49			CMDDESC_BLANK

#define BT_CMDNAME_50			CMDNAME_UNKNOWN
#define BT_CMDDESC_50			CMDDESC_BLANK

#define BT_CMDNAME_51			CMDNAME_UNKNOWN
#define BT_CMDDESC_51			CMDDESC_BLANK

#define BT_CMDNAME_52			CMDNAME_UNKNOWN
#define BT_CMDDESC_52			CMDDESC_BLANK

#define BT_CMDNAME_53			CMDNAME_UNKNOWN
#define BT_CMDDESC_53			CMDDESC_BLANK

#define BT_CMDNAME_54			CMDNAME_UNKNOWN
#define BT_CMDDESC_54			CMDDESC_BLANK
#endif // #if !defined (_WLAN_) && !defined (_NO_EEPROM_)


#define BT_CMDNAME_55			CMDNAME_UNKNOWN
#define BT_CMDDESC_55			CMDDESC_BLANK

#define BT_CMDNAME_56			CMDNAME_UNKNOWN
#define BT_CMDDESC_56			CMDDESC_BLANK

#define BT_CMDNAME_57			CMDNAME_UNKNOWN
#define BT_CMDDESC_57			CMDDESC_BLANK

#define BT_CMDNAME_58			CMDNAME_UNKNOWN
#define BT_CMDDESC_58			CMDDESC_BLANK

#define BT_CMDNAME_59			CMDNAME_UNKNOWN
#define BT_CMDDESC_59			CMDDESC_BLANK

#define BT_CMDNAME_60			CMDNAME_UNKNOWN
#define BT_CMDDESC_60			CMDDESC_BLANK

#define BT_CMDNAME_61			CMDNAME_UNKNOWN
#define BT_CMDDESC_61			CMDDESC_BLANK

#define BT_CMDNAME_62			"Read BTU Reg\n"
#define BT_CMDDESC_62			"\t(0x)(addr) (count =1)\n"

#define BT_CMDNAME_63			"Write BTU Reg\n"
#define BT_CMDDESC_63			"\t(0x 0x)(addr data)\n"

#define BT_CMDNAME_64			"Read BRF Reg\n"
#define BT_CMDDESC_64			"\t(0x)(addr) (count =1)\n"

#define BT_CMDNAME_65			"Write BRF Reg\n"
#define BT_CMDDESC_65			"\t(0x 0x)(addr data)\n"

#define BT_CMDNAME_66			CMDNAME_UNKNOWN
#define BT_CMDDESC_66			CMDDESC_BLANK

#define BT_CMDNAME_67			CMDNAME_UNKNOWN
#define BT_CMDDESC_67			CMDDESC_BLANK

#define BT_CMDNAME_68			"Peek SOC Mem\n"
#define BT_CMDDESC_68			"\t(0x)(addr) (count =1)\n"

#define BT_CMDNAME_69			"Poke SOC Mem\n"
#define BT_CMDDESC_69			"\t(0x 0x)(addr data)\n"

#if  !defined (_WLAN_) && !defined (_NO_EEPROM_)
#define BT_CMDNAME_70			"Peek Spi\n"
#define BT_CMDDESC_70			"\t(0x)(addr) (count =1)\n"

#define BT_CMDNAME_71			"Poke Spi\n"
#define BT_CMDDESC_71			"\t(0x 0x)(addr data)\n"
#else // #if !defined (_WLAN_) && !defined (_NO_EEPROM_)
#define BT_CMDNAME_70			CMDNAME_UNKNOWN
#define BT_CMDDESC_70			CMDDESC_BLANK

#define BT_CMDNAME_71			CMDNAME_UNKNOWN
#define BT_CMDDESC_71			CMDDESC_BLANK
#endif //#if !defined (_WLAN_) && !defined (_NO_EEPROM_)


#if defined (_W8689_) || defined (_W8787_)  || defined(_W8782_)
#define BT_CMDNAME_72			"Read Efuse Data\n"
#define BT_CMDDESC_72			"\t(0x)(block) (WordCount =16)\n"

#define BT_CMDNAME_73			"Write Efuse Data\n"
#define BT_CMDDESC_73			"\t(0x 0x)(block WordCount=1)\n"

#else //#if defined (_W8689_) || defined (_W8787_)  

#define BT_CMDNAME_72			CMDNAME_UNKNOWN
#define BT_CMDDESC_72			CMDDESC_BLANK

#define BT_CMDNAME_73			CMDNAME_UNKNOWN
#define BT_CMDDESC_73			CMDDESC_BLANK
#endif //#if defined (_W8689_) || defined (_W8787_) 


#define BT_CMDNAME_74			CMDNAME_UNKNOWN
#define BT_CMDDESC_74			CMDDESC_BLANK

#if !defined (_NO_EEPROM_)
#define BT_CMDNAME_75			"Load Customized Settings\n"
#define BT_CMDDESC_75			CMDDESC_BLANK
#else	// #if !defined (_NO_EEPROM_)
#define BT_CMDNAME_75			CMDNAME_UNKNOWN	//	"Load Customized Settings\n"
#define BT_CMDDESC_75			CMDDESC_BLANK
#endif // #if !defined (_NO_EEPROM_)

#define BT_CMDNAME_76			CMDNAME_UNKNOWN
#define BT_CMDDESC_76			CMDDESC_BLANK

#define BT_CMDNAME_77			CMDNAME_UNKNOWN
#define BT_CMDDESC_77			CMDDESC_BLANK

#define BT_CMDNAME_78			"Enable Device Under Test Mode\n"
#define BT_CMDDESC_78			CMDDESC_BLANK
 
#define BT_CMDNAME_79			"Periodic Inquiry Request\n"
#define BT_CMDDESC_79			"\t(enable(0) (0x)MaxPer(8) (0x)MinPer(7) (0x)InqLn(4))\n"\
				"\t\tMaxPer: Max Period Length. 0x0003-0xFFFF=3.84 - 83884.8 Sec\n"\
				"\t\tMinPer: Min Period Length. 0x0002-0xFFFF=2.56 - 83884.8 Sec\n"\
				"\t\tInqLn:  Inquiry Length.    0x01-0x30*1.28 sec = 1-61.44 sec\n"

#define BT_CMDNAME_80			"Reset BT HW\n"
#define BT_CMDDESC_80			CMDDESC_BLANK

#define BT_CMDNAME_81			CMDNAME_UNKNOWN
#define BT_CMDDESC_81			CMDDESC_BLANK

#define BT_CMDNAME_82			CMDNAME_UNKNOWN
#define BT_CMDDESC_82			CMDDESC_BLANK

#define BT_CMDNAME_83			CMDNAME_UNKNOWN
#define BT_CMDDESC_83			CMDDESC_BLANK

#define BT_CMDNAME_84			CMDNAME_UNKNOWN
#define BT_CMDDESC_84			CMDDESC_BLANK

#define BT_CMDNAME_85			CMDNAME_UNKNOWN
#define BT_CMDDESC_85			CMDDESC_BLANK

#define BT_CMDNAME_86			CMDNAME_UNKNOWN
#define BT_CMDDESC_86			CMDDESC_BLANK

#define BT_CMDNAME_87			CMDNAME_UNKNOWN
#define BT_CMDDESC_87			CMDDESC_BLANK

#define BT_CMDNAME_88			"FW/HW Version\n"
#define BT_CMDDESC_88			CMDDESC_BLANK

#if defined(_W8790_)
#define BT_CMDNAME_89			"Load Dut configuration file.\n"
#define BT_CMDDESC_89			"\t(FileName)\n"
#else // #if defined(_W8790_)
#define BT_CMDNAME_89			CMDNAME_UNKNOWN
#define BT_CMDDESC_89			CMDDESC_BLANK

#endif // #if defined(_W8790_)

#define BT_CMDNAME_90			CMDNAME_UNKNOWN
#define BT_CMDDESC_90			CMDDESC_BLANK

#define BT_CMDNAME_91			CMDNAME_UNKNOWN
#define BT_CMDDESC_91			CMDDESC_BLANK

#define BT_CMDNAME_92			CMDNAME_UNKNOWN
#define BT_CMDDESC_92			CMDDESC_BLANK

#define BT_CMDNAME_93			CMDNAME_UNKNOWN
#define BT_CMDDESC_93			CMDDESC_BLANK

#define BT_CMDNAME_94			CMDNAME_UNKNOWN
#define BT_CMDDESC_94			CMDDESC_BLANK

#if defined(_W8790_)
#define BT_CMDNAME_95			"Get Rf XTAL control\n"
#define BT_CMDDESC_95			CMDDESC_BLANK

#define BT_CMDNAME_96			"Set Rf XTAL control\n"
#define BT_CMDDESC_96			"\t(setting(8-bit))\n"
#else // #if defined(_W8790_)
#define BT_CMDNAME_95			CMDNAME_UNKNOWN
#define BT_CMDDESC_95			CMDDESC_BLANK

#define BT_CMDNAME_96			CMDNAME_UNKNOWN
#define BT_CMDDESC_96			CMDDESC_BLANK

#endif // #if defined(_W8790_)

#define BT_CMDNAME_97			CMDNAME_UNKNOWN
#define BT_CMDDESC_97			CMDDESC_BLANK

#define BT_CMDNAME_98			CMDNAME_UNKNOWN
#define BT_CMDDESC_98			CMDDESC_BLANK

#define BT_CMDNAME_99			"Exit\n"
#define BT_CMDDESC_99			CMDDESC_BLANK

#if defined(_W8688_)
#define BT_CMDNAME_100			"Set Pcm Loop Back Mode\n"
#define BT_CMDDESC_100			CMDDESC_BLANK
#else //#ifdef _W8688_
#define BT_CMDNAME_100			CMDNAME_UNKNOWN
#define BT_CMDDESC_100			CMDDESC_BLANK
#endif //#ifdef _W8688_

#define BT_CMDNAME_101			CMDNAME_UNKNOWN
#define BT_CMDDESC_101			CMDDESC_BLANK

#define BT_CMDNAME_102			CMDNAME_UNKNOWN
#define BT_CMDDESC_102			CMDDESC_BLANK

#define BT_CMDNAME_103			CMDNAME_UNKNOWN
#define BT_CMDDESC_103			CMDDESC_BLANK

#define BT_CMDNAME_104			CMDNAME_UNKNOWN
#define BT_CMDDESC_104			CMDDESC_BLANK

#define BT_CMDNAME_105			CMDNAME_UNKNOWN
#define BT_CMDDESC_105			CMDDESC_BLANK

#define BT_CMDNAME_106			CMDNAME_UNKNOWN
#define BT_CMDDESC_106			CMDDESC_BLANK

#define BT_CMDNAME_107			CMDNAME_UNKNOWN
#define BT_CMDDESC_107			CMDDESC_BLANK

#define BT_CMDNAME_108			CMDNAME_UNKNOWN
#define BT_CMDDESC_108			CMDDESC_BLANK

#define BT_CMDNAME_109			CMDNAME_UNKNOWN
#define BT_CMDDESC_109			CMDDESC_BLANK

#define BT_CMDNAME_110			CMDNAME_UNKNOWN
#define BT_CMDDESC_110			CMDDESC_BLANK

#define BT_CMDNAME_111			"Get state of FH 2Channel mode\n"
#define BT_CMDDESC_111			CMDDESC_BLANK

#define BT_CMDNAME_112			"Set state of FH 2Channel mode\n"
#define BT_CMDDESC_112			"\t(Enable)\n"\
"\t\tEnable: 0=false, 1=true. Default=0 false\n"

#define BT_CMDNAME_113			"Get Power Control Class\n"
#define BT_CMDDESC_113			"\t\tmode: 0:MRVL_Class2, 1:MRVL_Class1.5\n"

#define BT_CMDNAME_114			"Set Power Control Class\n"
#define BT_CMDDESC_114			"\t(mode)\n"\
"\t\tmode: 0:MRVL_Class2, 1:MRVL_Class1.5. Default=0.\n"

#define BT_CMDNAME_115			"Get Disable Btu Pwr Ctl\n"
#define BT_CMDDESC_115			CMDDESC_BLANK

#define BT_CMDNAME_116			"Set Disable Btu Pwr Ctl\n"
#define BT_CMDDESC_116			"\t(mode)\n"\
"\t\tmode : 0=enable, 1=disable. Default=0.\n"

#define BT_CMDNAME_117			CMDNAME_UNKNOWN
#define BT_CMDDESC_117			CMDDESC_BLANK

#define BT_CMDNAME_118			CMDNAME_UNKNOWN
#define BT_CMDDESC_118			CMDDESC_BLANK

#define BT_CMDNAME_119			CMDNAME_UNKNOWN
#define BT_CMDDESC_119			CMDDESC_BLANK

#define BT_CMDNAME_120			CMDNAME_UNKNOWN
#define BT_CMDDESC_120			CMDDESC_BLANK

#define BT_CMDNAME_121			CMDNAME_UNKNOWN
#define BT_CMDDESC_121			CMDDESC_BLANK

#define BT_CMDNAME_122			CMDNAME_UNKNOWN
#define BT_CMDDESC_122			CMDDESC_BLANK

#define BT_CMDNAME_123			CMDNAME_UNKNOWN
#define BT_CMDDESC_123			CMDDESC_BLANK

#define BT_CMDNAME_124			CMDNAME_UNKNOWN
#define BT_CMDDESC_124			CMDDESC_BLANK

#define BT_CMDNAME_125			CMDNAME_UNKNOWN
#define BT_CMDDESC_125			CMDDESC_BLANK

#define BT_CMDNAME_126			CMDNAME_UNKNOWN
#define BT_CMDDESC_126			CMDDESC_BLANK

#define BT_CMDNAME_127			CMDNAME_UNKNOWN
#define BT_CMDDESC_127			CMDDESC_BLANK

#define BT_CMDNAME_128			CMDNAME_UNKNOWN
#define BT_CMDDESC_128			CMDDESC_BLANK
 
#define BT_CMDNAME_129			CMDNAME_UNKNOWN
#define BT_CMDDESC_129			CMDDESC_BLANK

#define BT_CMDNAME_130			CMDNAME_UNKNOWN
#define BT_CMDDESC_130			CMDDESC_BLANK

#define BT_CMDNAME_131			CMDNAME_UNKNOWN
#define BT_CMDDESC_131			CMDDESC_BLANK

#define BT_CMDNAME_132			CMDNAME_UNKNOWN
#define BT_CMDDESC_132			CMDDESC_BLANK

#define BT_CMDNAME_133			CMDNAME_UNKNOWN
#define BT_CMDDESC_133			CMDDESC_BLANK

#define BT_CMDNAME_134			CMDNAME_UNKNOWN
#define BT_CMDDESC_134			CMDDESC_BLANK

#define BT_CMDNAME_135			CMDNAME_UNKNOWN
#define BT_CMDDESC_135			CMDDESC_BLANK

#define BT_CMDNAME_136			CMDNAME_UNKNOWN
#define BT_CMDDESC_136			CMDDESC_BLANK

#define BT_CMDNAME_137			CMDNAME_UNKNOWN
#define BT_CMDDESC_137			CMDDESC_BLANK

#define BT_CMDNAME_138			CMDNAME_UNKNOWN
#define BT_CMDDESC_138			CMDDESC_BLANK

#define BT_CMDNAME_139			CMDNAME_UNKNOWN
#define BT_CMDDESC_139			CMDDESC_BLANK

#define BT_CMDNAME_140			CMDNAME_UNKNOWN
#define BT_CMDDESC_140			CMDDESC_BLANK

#define BT_CMDNAME_141			CMDNAME_UNKNOWN
#define BT_CMDDESC_141			CMDDESC_BLANK

#define BT_CMDNAME_142			CMDNAME_UNKNOWN
#define BT_CMDDESC_142			CMDDESC_BLANK

#define BT_CMDNAME_143			CMDNAME_UNKNOWN
#define BT_CMDDESC_143			CMDDESC_BLANK

#define BT_CMDNAME_144			CMDNAME_UNKNOWN
#define BT_CMDDESC_144			CMDDESC_BLANK

#define BT_CMDNAME_145			CMDNAME_UNKNOWN
#define BT_CMDDESC_145			CMDDESC_BLANK

#define BT_CMDNAME_146			CMDNAME_UNKNOWN
#define BT_CMDDESC_146			CMDDESC_BLANK

#define BT_CMDNAME_147			CMDNAME_UNKNOWN
#define BT_CMDDESC_147			CMDDESC_BLANK

#define BT_CMDNAME_148			CMDNAME_UNKNOWN
#define BT_CMDDESC_148			CMDDESC_BLANK

#define BT_CMDNAME_149			CMDNAME_UNKNOWN
#define BT_CMDDESC_149			CMDDESC_BLANK

#define BT_CMDNAME_150			CMDNAME_UNKNOWN
#define BT_CMDDESC_150			CMDDESC_BLANK

#define BT_CMDNAME_151			CMDNAME_UNKNOWN
#define BT_CMDDESC_151			CMDDESC_BLANK

#define BT_CMDNAME_152			CMDNAME_UNKNOWN
#define BT_CMDDESC_152			CMDDESC_BLANK

#define BT_CMDNAME_153			CMDNAME_UNKNOWN
#define BT_CMDDESC_153			CMDDESC_BLANK

#define BT_CMDNAME_154			CMDNAME_UNKNOWN
#define BT_CMDDESC_154			CMDDESC_BLANK

#define BT_CMDNAME_155			CMDNAME_UNKNOWN
#define BT_CMDDESC_155			CMDDESC_BLANK

#define BT_CMDNAME_156			CMDNAME_UNKNOWN
#define BT_CMDDESC_156			CMDDESC_BLANK

#define BT_CMDNAME_157			CMDNAME_UNKNOWN
#define BT_CMDDESC_157			CMDDESC_BLANK

#define BT_CMDNAME_158			CMDNAME_UNKNOWN
#define BT_CMDDESC_158			CMDDESC_BLANK

#define BT_CMDNAME_159			CMDNAME_UNKNOWN
#define BT_CMDDESC_159			CMDDESC_BLANK

#define BT_CMDNAME_160			CMDNAME_UNKNOWN
#define BT_CMDDESC_160			CMDDESC_BLANK

#define BT_CMDNAME_161			CMDNAME_UNKNOWN
#define BT_CMDDESC_161			CMDDESC_BLANK

#define BT_CMDNAME_162			CMDNAME_UNKNOWN
#define BT_CMDDESC_162			CMDDESC_BLANK

#define BT_CMDNAME_163			CMDNAME_UNKNOWN
#define BT_CMDDESC_163			CMDDESC_BLANK

#define BT_CMDNAME_164			CMDNAME_UNKNOWN
#define BT_CMDDESC_164			CMDDESC_BLANK

#define BT_CMDNAME_165			CMDNAME_UNKNOWN
#define BT_CMDDESC_165			CMDDESC_BLANK

#define BT_CMDNAME_166			CMDNAME_UNKNOWN
#define BT_CMDDESC_166			CMDDESC_BLANK

#define BT_CMDNAME_167			CMDNAME_UNKNOWN
#define BT_CMDDESC_167			CMDDESC_BLANK

#define BT_CMDNAME_168			CMDNAME_UNKNOWN
#define BT_CMDDESC_168			CMDDESC_BLANK

#define BT_CMDNAME_169			CMDNAME_UNKNOWN
#define BT_CMDDESC_169			CMDDESC_BLANK

#define BT_CMDNAME_170			CMDNAME_UNKNOWN
#define BT_CMDDESC_170			CMDDESC_BLANK

#define BT_CMDNAME_171			CMDNAME_UNKNOWN
#define BT_CMDDESC_171			CMDDESC_BLANK

#define BT_CMDNAME_172			CMDNAME_UNKNOWN
#define BT_CMDDESC_172			CMDDESC_BLANK

#define BT_CMDNAME_173			CMDNAME_UNKNOWN
#define BT_CMDDESC_173			CMDDESC_BLANK

#define BT_CMDNAME_174			CMDNAME_UNKNOWN
#define BT_CMDDESC_174			CMDDESC_BLANK

#define BT_CMDNAME_175			CMDNAME_UNKNOWN
#define BT_CMDDESC_175			CMDDESC_BLANK

#define BT_CMDNAME_176			CMDNAME_UNKNOWN
#define BT_CMDDESC_176			CMDDESC_BLANK

#define BT_CMDNAME_177			CMDNAME_UNKNOWN
#define BT_CMDDESC_177			CMDDESC_BLANK

#define BT_CMDNAME_178			CMDNAME_UNKNOWN
#define BT_CMDDESC_178			CMDDESC_BLANK

#define BT_CMDNAME_179			CMDNAME_UNKNOWN
#define BT_CMDDESC_179			CMDDESC_BLANK

#define BT_CMDNAME_180			CMDNAME_UNKNOWN
#define BT_CMDDESC_180			CMDDESC_BLANK

#define BT_CMDNAME_181			CMDNAME_UNKNOWN
#define BT_CMDDESC_181			CMDDESC_BLANK

#define BT_CMDNAME_182			CMDNAME_UNKNOWN
#define BT_CMDDESC_182			CMDDESC_BLANK

#define BT_CMDNAME_183			CMDNAME_UNKNOWN
#define BT_CMDDESC_183			CMDDESC_BLANK

#define BT_CMDNAME_184			CMDNAME_UNKNOWN
#define BT_CMDDESC_184			CMDDESC_BLANK

#define BT_CMDNAME_185			CMDNAME_UNKNOWN
#define BT_CMDDESC_185			CMDDESC_BLANK

#define BT_CMDNAME_186			CMDNAME_UNKNOWN
#define BT_CMDDESC_186			CMDDESC_BLANK

#define BT_CMDNAME_187			CMDNAME_UNKNOWN
#define BT_CMDDESC_187			CMDDESC_BLANK

#define BT_CMDNAME_188			CMDNAME_UNKNOWN //"FW on-line release note\n"
#define BT_CMDDESC_188			CMDDESC_BLANK

#define BT_CMDNAME_189			CMDNAME_UNKNOWN
#define BT_CMDDESC_189			CMDDESC_BLANK

#define BT_CMDNAME_190			CMDNAME_UNKNOWN
#define BT_CMDDESC_190			CMDDESC_BLANK

#define BT_CMDNAME_191			CMDNAME_UNKNOWN
#define BT_CMDDESC_191			CMDDESC_BLANK

#define BT_CMDNAME_192			CMDNAME_UNKNOWN
#define BT_CMDDESC_192			CMDDESC_BLANK

#define BT_CMDNAME_193			CMDNAME_UNKNOWN
#define BT_CMDDESC_193			CMDDESC_BLANK

#define BT_CMDNAME_194			CMDNAME_UNKNOWN
#define BT_CMDDESC_194			CMDDESC_BLANK

#define BT_CMDNAME_195			CMDNAME_UNKNOWN
#define BT_CMDDESC_195			CMDDESC_BLANK

#define BT_CMDNAME_196			CMDNAME_UNKNOWN
#define BT_CMDDESC_196			CMDDESC_BLANK

#define BT_CMDNAME_197			CMDNAME_UNKNOWN
#define BT_CMDDESC_197			CMDDESC_BLANK

#define BT_CMDNAME_198			CMDNAME_UNKNOWN
#define BT_CMDDESC_198			CMDDESC_BLANK

#define BT_CMDNAME_199			CMDNAME_UNKNOWN
#define BT_CMDDESC_199			CMDDESC_BLANK

#define BT_CMDNAME_200			CMDNAME_UNKNOWN
#define BT_CMDDESC_200			CMDDESC_BLANK

#define BT_CMDNAME_201			CMDNAME_UNKNOWN
#define BT_CMDDESC_201			CMDDESC_BLANK

#define BT_CMDNAME_202			CMDNAME_UNKNOWN
#define BT_CMDDESC_202			CMDDESC_BLANK

#define BT_CMDNAME_203			CMDNAME_UNKNOWN
#define BT_CMDDESC_203			CMDDESC_BLANK

#define BT_CMDNAME_204			CMDNAME_UNKNOWN
#define BT_CMDDESC_204			CMDDESC_BLANK

#define BT_CMDNAME_205			CMDNAME_UNKNOWN
#define BT_CMDDESC_205			CMDDESC_BLANK

#define BT_CMDNAME_206			CMDNAME_UNKNOWN
#define BT_CMDDESC_206			CMDDESC_BLANK

#define BT_CMDNAME_207			CMDNAME_UNKNOWN
#define BT_CMDDESC_207			CMDDESC_BLANK

#define BT_CMDNAME_208			CMDNAME_UNKNOWN
#define BT_CMDDESC_208			CMDDESC_BLANK

#define BT_CMDNAME_209			CMDNAME_UNKNOWN
#define BT_CMDDESC_209			CMDDESC_BLANK

#define BT_CMDNAME_210			CMDNAME_UNKNOWN
#define BT_CMDDESC_210			CMDDESC_BLANK

#define BT_CMDNAME_211			CMDNAME_UNKNOWN
#define BT_CMDDESC_211			CMDDESC_BLANK

#define BT_CMDNAME_212			CMDNAME_UNKNOWN
#define BT_CMDDESC_212			CMDDESC_BLANK

#define BT_CMDNAME_213			CMDNAME_UNKNOWN
#define BT_CMDDESC_213			CMDDESC_BLANK

#define BT_CMDNAME_214			CMDNAME_UNKNOWN
#define BT_CMDDESC_214			CMDDESC_BLANK

#define BT_CMDNAME_215			CMDNAME_UNKNOWN
#define BT_CMDDESC_215			CMDDESC_BLANK

#define BT_CMDNAME_216			CMDNAME_UNKNOWN
#define BT_CMDDESC_216			CMDDESC_BLANK

#define BT_CMDNAME_217			CMDNAME_UNKNOWN
#define BT_CMDDESC_217			CMDDESC_BLANK

#define BT_CMDNAME_218			CMDNAME_UNKNOWN
#define BT_CMDDESC_218			CMDDESC_BLANK

#define BT_CMDNAME_219			CMDNAME_UNKNOWN
#define BT_CMDDESC_219			CMDDESC_BLANK

#define BT_CMDNAME_220			CMDNAME_UNKNOWN
#define BT_CMDDESC_220			CMDDESC_BLANK

#define BT_CMDNAME_221			CMDNAME_UNKNOWN
#define BT_CMDDESC_221			CMDDESC_BLANK

#define BT_CMDNAME_222			CMDNAME_UNKNOWN
#define BT_CMDDESC_222			CMDDESC_BLANK

#define BT_CMDNAME_223			CMDNAME_UNKNOWN
#define BT_CMDDESC_223			CMDDESC_BLANK

#define BT_CMDNAME_224			CMDNAME_UNKNOWN
#define BT_CMDDESC_224			CMDDESC_BLANK

#define BT_CMDNAME_225			"Dutycycle Tx\n"
#define BT_CMDDESC_225			"\t(Enable=0) (PacketType(0x)=11)\n"\
			"\t(PayloadPattern=2) (PayloadLenInByte=-1) (HopMode=0) (Interval=1)(Whitening=0)\n"\
			"\tEnable: 0: off; 1: on\n"\
			"\tPacketType(Rate.Slot):\n"\
			"\t\tTesting Pattern and ACL:\n"\
			"\t\t\t  DM1 = 0x01;   DM3 = 0x03;   DM5 = 0x05; (GFSK, 1M FEC)\n"\
			"\t\t\t  DH1 = 0x11;   DH3 = 0x13;   DH5 = 0x15; (GFSK, 1M)\n"\
			"\t\t\t2-DH1 = 0x21; 2-DH3 = 0x23; 2-DH5 = 0x25; (DQPSK, 2M)\n"\
			"\t\t\t3-DH1 = 0x31; 3-DH3 = 0x33; 3-DH5 = 0x35; (8PSK, 3M)\n"\
\
			"\t\tSco:\n"\
			"\t\t\t  HV1 = 0x11;   HV2 = 0x12;   HV3 = 0x13; (GFSK, 1M)\n"\
			"\t\teSco:\n"\
			"\t\t\t  EV3 = 0x13;   EV4 = 0x14;   EV5 = 0x15; (GFSK, 1M)\n"\
			"\t\t\t2-EV3 = 0x23; 2-EV5 = 0x25;               (DQPSK, 2M)\n"\
			"\t\t\t3-EV3 = 0x33; 3-EV5 = 0x35;               (8PSK, 3M)\n"\
\
			"\tPayloadPattern: 0: all 0, 1: all 1, 2: PN9, 3: 0xAA, 4: 0xF0\n"\
			"\t\t\t5: PRBS ACL, 6: PRBS SCO, 7: PRBS ESCO\n"\
			"\tPayloadLenInByte: PacketType dependent, -1 for max possible.\n"\
			"\tHopMode: HopMode (on=1, random hopping, off=0 fixed channel)\n"\
			"\tWhitening: Whitening the whole packet. (true=1, false=0)\n"

#define BT_CMDNAME_226			CMDNAME_UNKNOWN
#define BT_CMDDESC_226			CMDDESC_BLANK

#define BT_CMDNAME_227			CMDNAME_UNKNOWN
#define BT_CMDDESC_227			CMDDESC_BLANK

#define BT_CMDNAME_228			CMDNAME_UNKNOWN
#define BT_CMDDESC_228			CMDDESC_BLANK

#define BT_CMDNAME_229			CMDNAME_UNKNOWN
#define BT_CMDDESC_229			CMDDESC_BLANK

#define BT_CMDNAME_230			CMDNAME_UNKNOWN
#define BT_CMDDESC_230			CMDDESC_BLANK

#if 1 //ndef _HCI_PROTOCAL_
#define BT_CMDNAME_231			CMDNAME_UNKNOWN
#define BT_CMDDESC_231			CMDDESC_BLANK

#define BT_CMDNAME_232			CMDNAME_UNKNOWN
#define BT_CMDDESC_232			CMDDESC_BLANK

#define BT_CMDNAME_233			CMDNAME_UNKNOWN
#define BT_CMDDESC_233			CMDDESC_BLANK
#else	// #ifdef _HCI_PROTOCAL_
#define BT_CMDNAME_231			"Comb Command Hold\n"
#define BT_CMDDESC_231			"\t(mode) (0x)(MaxInterval) (0x)(MinInterval) (PartnerLocalControlable)) \n"\
						"\t\t mode: 0=off, 1=on. Defualt 1.\n"\
						"\t\tMaxInterval: Max Interval in Hex. Defualt 800\n"\
						"\t\tMinInterval: Min Interval in Hex. Defualt 400\n"\
 						"\t\tPartnerLocalControlable: Partner Local Controlable, bool.  Defualt 1. \n"


#define BT_CMDNAME_232			"Comb Command Sniff mode\n"
#define BT_CMDDESC_232			"\t(mode) (0x)(MaxInterval) (0x)(MinInterval) (Attemp) (TimeOut)\n"\
								"\t(PartnerLocalControlable=1)) \n"\
						"\t\t mode: 0=off, 1=on. Defualt 1.\n"\
						"\t\tMaxInterval: Max Interval in Hex. Defualt 800\n"\
						"\t\tMinInterval: Min Interval in Hex. Defualt 400\n"\
						"\t\tAttemp:  Attemps. Defualt 1.\n"\
						"\t\tTimeOut: TimeOut. Defualt 1.\n"\
 						"\t\tPartnerLocalControlable: Partner Local Controlable, bool.  Defualt 1. \n"\

#define BT_CMDNAME_233			"Comb Command Park mode\n"
#define BT_CMDDESC_233			"\t(mode) (0x)(MaxInterval) (0x)(MinInterval) (PartnerLocalControlable)) \n"\
						"\t\t mode: 0=off, 1=on. Defualt 1.\n"\
						"\t\tMaxInterval: Max Interval in Hex. Defualt 800\n"\
						"\t\tMinInterval: Min Interval in Hex. Defualt 400\n"\
 						"\t\tPartnerLocalControlable: Partner Local Controlable, bool.  Defualt 1. \n"
#endif	// #ifdef _HCI_PROTOCAL_

#define BT_CMDNAME_234			CMDNAME_UNKNOWN
#define BT_CMDDESC_234			CMDDESC_BLANK

#define BT_CMDNAME_235			CMDNAME_UNKNOWN
#define BT_CMDDESC_235			CMDDESC_BLANK

#define BT_CMDNAME_236			CMDNAME_UNKNOWN
#define BT_CMDDESC_236			CMDDESC_BLANK

#define BT_CMDNAME_237			CMDNAME_UNKNOWN
#define BT_CMDDESC_237			CMDDESC_BLANK

#define BT_CMDNAME_238			CMDNAME_UNKNOWN
#define BT_CMDDESC_238			CMDDESC_BLANK

#define BT_CMDNAME_239			CMDNAME_UNKNOWN
#define BT_CMDDESC_239			CMDDESC_BLANK

#define BT_CMDNAME_240			CMDNAME_UNKNOWN
#define BT_CMDDESC_240			CMDDESC_BLANK

#define BT_CMDNAME_241			CMDNAME_UNKNOWN
#define BT_CMDDESC_241			CMDDESC_BLANK

#define BT_CMDNAME_242			CMDNAME_UNKNOWN
#define BT_CMDDESC_242			CMDDESC_BLANK

#define BT_CMDNAME_243			CMDNAME_UNKNOWN
#define BT_CMDDESC_243			CMDDESC_BLANK

#define BT_CMDNAME_244			CMDNAME_UNKNOWN
#define BT_CMDDESC_244			CMDDESC_BLANK

#define BT_CMDNAME_245			CMDNAME_UNKNOWN
#define BT_CMDDESC_245			CMDDESC_BLANK

#define BT_CMDNAME_246			CMDNAME_UNKNOWN
#define BT_CMDDESC_246			CMDDESC_BLANK

#define BT_CMDNAME_247			CMDNAME_UNKNOWN
#define BT_CMDDESC_247			CMDDESC_BLANK

#define BT_CMDNAME_248			CMDNAME_UNKNOWN
#define BT_CMDDESC_248			CMDDESC_BLANK

#define BT_CMDNAME_249			CMDNAME_UNKNOWN
#define BT_CMDDESC_249			CMDDESC_BLANK

#define BT_CMDNAME_250			CMDNAME_UNKNOWN
#define BT_CMDDESC_250			CMDDESC_BLANK

#define BT_CMDNAME_251			CMDNAME_UNKNOWN
#define BT_CMDDESC_251			CMDDESC_BLANK

#define BT_CMDNAME_252			CMDNAME_UNKNOWN
#define BT_CMDDESC_252			CMDDESC_BLANK

#define BT_CMDNAME_253			CMDNAME_UNKNOWN
#define BT_CMDDESC_253			CMDDESC_BLANK

#define BT_CMDNAME_254			CMDNAME_UNKNOWN
#define BT_CMDDESC_254			CMDDESC_BLANK

#define BT_CMDNAME_255			CMDNAME_UNKNOWN
#define BT_CMDDESC_255			CMDDESC_BLANK

#define BT_CMDNAME_256			CMDNAME_UNKNOWN
#define BT_CMDDESC_256			CMDDESC_BLANK

#define BT_CMDNAME_257			CMDNAME_UNKNOWN
#define BT_CMDDESC_257			CMDDESC_BLANK

#define BT_CMDNAME_258			CMDNAME_UNKNOWN
#define BT_CMDDESC_258			CMDDESC_BLANK

#define BT_CMDNAME_259			CMDNAME_UNKNOWN
#define BT_CMDDESC_259			CMDDESC_BLANK

 
/*
#define BT_CMDNAME_0			CMDNAME_UNKNOWN
#define BT_CMDDESC_0			CMDDESC_BLANK

#define BT_CMDNAME_1			CMDNAME_UNKNOWN
#define BT_CMDDESC_1			CMDDESC_BLANK

#define BT_CMDNAME_2			CMDNAME_UNKNOWN
#define BT_CMDDESC_2			CMDDESC_BLANK

#define BT_CMDNAME_3			CMDNAME_UNKNOWN
#define BT_CMDDESC_3			CMDDESC_BLANK

#define BT_CMDNAME_4			CMDNAME_UNKNOWN
#define BT_CMDDESC_4			CMDDESC_BLANK

#define BT_CMDNAME_5			CMDNAME_UNKNOWN
#define BT_CMDDESC_5			CMDDESC_BLANK

#define BT_CMDNAME_6			CMDNAME_UNKNOWN
#define BT_CMDDESC_6			CMDDESC_BLANK

#define BT_CMDNAME_7			CMDNAME_UNKNOWN
#define BT_CMDDESC_7			CMDDESC_BLANK

#define BT_CMDNAME_8			CMDNAME_UNKNOWN
#define BT_CMDDESC_8			CMDDESC_BLANK

#define BT_CMDNAME_9			CMDNAME_UNKNOWN
#define BT_CMDDESC_9			CMDDESC_BLANK



typedef struct  CmdMenu
{
	char       *pCmdName;
	char       *pCmdDescriptor;
} CmdMenu;
*/
CmdMenu BtCmdNames[256]=
{
	{BT_CMDNAME_0, BT_CMDDESC_0},
	{BT_CMDNAME_1, BT_CMDDESC_1},
	{BT_CMDNAME_2, BT_CMDDESC_2},
	{BT_CMDNAME_3, BT_CMDDESC_3},
	{BT_CMDNAME_4, BT_CMDDESC_4},
	{BT_CMDNAME_5, BT_CMDDESC_5},
	{BT_CMDNAME_6, BT_CMDDESC_6},
	{BT_CMDNAME_7, BT_CMDDESC_7},
	{BT_CMDNAME_8, BT_CMDDESC_8},
	{BT_CMDNAME_9, BT_CMDDESC_9},

	{BT_CMDNAME_10, BT_CMDDESC_10},
	{BT_CMDNAME_11, BT_CMDDESC_11},
	{BT_CMDNAME_12, BT_CMDDESC_12},
	{BT_CMDNAME_13, BT_CMDDESC_13},
	{BT_CMDNAME_14, BT_CMDDESC_14},
	{BT_CMDNAME_15, BT_CMDDESC_15},
	{BT_CMDNAME_16, BT_CMDDESC_16},
	{BT_CMDNAME_17, BT_CMDDESC_17},
	{BT_CMDNAME_18, BT_CMDDESC_18},
	{BT_CMDNAME_19, BT_CMDDESC_19},
	
	{BT_CMDNAME_20, BT_CMDDESC_20},
	{BT_CMDNAME_21, BT_CMDDESC_21},
	{BT_CMDNAME_22, BT_CMDDESC_22},
	{BT_CMDNAME_23, BT_CMDDESC_23},
	{BT_CMDNAME_24, BT_CMDDESC_24},
	{BT_CMDNAME_25, BT_CMDDESC_25},
	{BT_CMDNAME_26, BT_CMDDESC_26},
	{BT_CMDNAME_27, BT_CMDDESC_27},
	{BT_CMDNAME_28, BT_CMDDESC_28},
	{BT_CMDNAME_29, BT_CMDDESC_29},

	{BT_CMDNAME_30, BT_CMDDESC_30},
	{BT_CMDNAME_31, BT_CMDDESC_31},
	{BT_CMDNAME_32, BT_CMDDESC_32},
	{BT_CMDNAME_33, BT_CMDDESC_33},
	{BT_CMDNAME_34, BT_CMDDESC_34},
	{BT_CMDNAME_35, BT_CMDDESC_35},
	{BT_CMDNAME_36, BT_CMDDESC_36},
	{BT_CMDNAME_37, BT_CMDDESC_37},
	{BT_CMDNAME_38, BT_CMDDESC_38},
	{BT_CMDNAME_39, BT_CMDDESC_39},

	{BT_CMDNAME_40, BT_CMDDESC_40},
	{BT_CMDNAME_41, BT_CMDDESC_41},
	{BT_CMDNAME_42, BT_CMDDESC_42},
	{BT_CMDNAME_43, BT_CMDDESC_43},
	{BT_CMDNAME_44, BT_CMDDESC_44},
	{BT_CMDNAME_45, BT_CMDDESC_45},
	{BT_CMDNAME_46, BT_CMDDESC_46},
	{BT_CMDNAME_47, BT_CMDDESC_47},
	{BT_CMDNAME_48, BT_CMDDESC_48},
	{BT_CMDNAME_49, BT_CMDDESC_49},

	{BT_CMDNAME_50, BT_CMDDESC_50},
	{BT_CMDNAME_51, BT_CMDDESC_51},
	{BT_CMDNAME_52, BT_CMDDESC_52},
	{BT_CMDNAME_53, BT_CMDDESC_53},
	{BT_CMDNAME_54, BT_CMDDESC_54},
	{BT_CMDNAME_55, BT_CMDDESC_55},
	{BT_CMDNAME_56, BT_CMDDESC_56},
	{BT_CMDNAME_57, BT_CMDDESC_57},
	{BT_CMDNAME_58, BT_CMDDESC_58},
	{BT_CMDNAME_59, BT_CMDDESC_59},

	{BT_CMDNAME_60, BT_CMDDESC_60},
	{BT_CMDNAME_61, BT_CMDDESC_61},
	{BT_CMDNAME_62, BT_CMDDESC_62},
	{BT_CMDNAME_63, BT_CMDDESC_63},
	{BT_CMDNAME_64, BT_CMDDESC_64},
	{BT_CMDNAME_65, BT_CMDDESC_65},
	{BT_CMDNAME_66, BT_CMDDESC_66},
	{BT_CMDNAME_67, BT_CMDDESC_67},
	{BT_CMDNAME_68, BT_CMDDESC_68},
	{BT_CMDNAME_69, BT_CMDDESC_69},

	{BT_CMDNAME_70, BT_CMDDESC_70},
	{BT_CMDNAME_71, BT_CMDDESC_71},
	{BT_CMDNAME_72, BT_CMDDESC_72},
	{BT_CMDNAME_73, BT_CMDDESC_73},
	{BT_CMDNAME_74, BT_CMDDESC_74},
	{BT_CMDNAME_75, BT_CMDDESC_75},
	{BT_CMDNAME_76, BT_CMDDESC_76},
	{BT_CMDNAME_77, BT_CMDDESC_77},
	{BT_CMDNAME_78, BT_CMDDESC_78},
	{BT_CMDNAME_79, BT_CMDDESC_79},

	{BT_CMDNAME_80, BT_CMDDESC_80},
	{BT_CMDNAME_81, BT_CMDDESC_81},
	{BT_CMDNAME_82, BT_CMDDESC_82},
	{BT_CMDNAME_83, BT_CMDDESC_83},
	{BT_CMDNAME_84, BT_CMDDESC_84},
	{BT_CMDNAME_85, BT_CMDDESC_85},
	{BT_CMDNAME_86, BT_CMDDESC_86},
	{BT_CMDNAME_87, BT_CMDDESC_87},
	{BT_CMDNAME_88, BT_CMDDESC_88},
	{BT_CMDNAME_89, BT_CMDDESC_89},

	{BT_CMDNAME_90, BT_CMDDESC_90},
	{BT_CMDNAME_91, BT_CMDDESC_91},
	{BT_CMDNAME_92, BT_CMDDESC_92},
	{BT_CMDNAME_93, BT_CMDDESC_93},
	{BT_CMDNAME_94, BT_CMDDESC_94},
	{BT_CMDNAME_95, BT_CMDDESC_95},
	{BT_CMDNAME_96, BT_CMDDESC_96},
	{BT_CMDNAME_97, BT_CMDDESC_97},
	{BT_CMDNAME_98, BT_CMDDESC_98},
	{BT_CMDNAME_99, BT_CMDDESC_99},

	{BT_CMDNAME_100, BT_CMDDESC_100},
	{BT_CMDNAME_101, BT_CMDDESC_101},
	{BT_CMDNAME_102, BT_CMDDESC_102},
	{BT_CMDNAME_103, BT_CMDDESC_103},
	{BT_CMDNAME_104, BT_CMDDESC_104},
	{BT_CMDNAME_105, BT_CMDDESC_105},
	{BT_CMDNAME_106, BT_CMDDESC_106},
	{BT_CMDNAME_107, BT_CMDDESC_107},
	{BT_CMDNAME_108, BT_CMDDESC_108},
	{BT_CMDNAME_109, BT_CMDDESC_109},

	{BT_CMDNAME_110, BT_CMDDESC_110},
	{BT_CMDNAME_111, BT_CMDDESC_111},
	{BT_CMDNAME_112, BT_CMDDESC_112},
	{BT_CMDNAME_113, BT_CMDDESC_113},
	{BT_CMDNAME_114, BT_CMDDESC_114},
	{BT_CMDNAME_115, BT_CMDDESC_115},
	{BT_CMDNAME_116, BT_CMDDESC_116},
	{BT_CMDNAME_117, BT_CMDDESC_117},
	{BT_CMDNAME_118, BT_CMDDESC_118},
	{BT_CMDNAME_119, BT_CMDDESC_119},

	{BT_CMDNAME_120, BT_CMDDESC_120},
	{BT_CMDNAME_121, BT_CMDDESC_121},
	{BT_CMDNAME_122, BT_CMDDESC_122},
	{BT_CMDNAME_123, BT_CMDDESC_123},
	{BT_CMDNAME_124, BT_CMDDESC_124},
	{BT_CMDNAME_125, BT_CMDDESC_125},
	{BT_CMDNAME_126, BT_CMDDESC_126},
	{BT_CMDNAME_127, BT_CMDDESC_127},
	{BT_CMDNAME_128, BT_CMDDESC_128},
	{BT_CMDNAME_129, BT_CMDDESC_129},

	{BT_CMDNAME_130, BT_CMDDESC_130},
	{BT_CMDNAME_131, BT_CMDDESC_131},
	{BT_CMDNAME_132, BT_CMDDESC_132},
	{BT_CMDNAME_133, BT_CMDDESC_133},
	{BT_CMDNAME_134, BT_CMDDESC_134},
	{BT_CMDNAME_135, BT_CMDDESC_135},
	{BT_CMDNAME_136, BT_CMDDESC_136},
	{BT_CMDNAME_137, BT_CMDDESC_137},
	{BT_CMDNAME_138, BT_CMDDESC_138},
	{BT_CMDNAME_139, BT_CMDDESC_139},

	{BT_CMDNAME_140, BT_CMDDESC_140},
	{BT_CMDNAME_141, BT_CMDDESC_141},
	{BT_CMDNAME_142, BT_CMDDESC_142},
	{BT_CMDNAME_143, BT_CMDDESC_143},
	{BT_CMDNAME_144, BT_CMDDESC_144},
	{BT_CMDNAME_145, BT_CMDDESC_145},
	{BT_CMDNAME_146, BT_CMDDESC_146},
	{BT_CMDNAME_147, BT_CMDDESC_147},
	{BT_CMDNAME_148, BT_CMDDESC_148},
	{BT_CMDNAME_149, BT_CMDDESC_149},
	

	{BT_CMDNAME_150, BT_CMDDESC_150},
	{BT_CMDNAME_151, BT_CMDDESC_151},
	{BT_CMDNAME_152, BT_CMDDESC_152},
	{BT_CMDNAME_153, BT_CMDDESC_153},
	{BT_CMDNAME_154, BT_CMDDESC_154},
	{BT_CMDNAME_155, BT_CMDDESC_155},
	{BT_CMDNAME_156, BT_CMDDESC_156},
	{BT_CMDNAME_157, BT_CMDDESC_157},
	{BT_CMDNAME_158, BT_CMDDESC_158},
	{BT_CMDNAME_159, BT_CMDDESC_159},
	

	{BT_CMDNAME_160, BT_CMDDESC_160},
	{BT_CMDNAME_161, BT_CMDDESC_161},
	{BT_CMDNAME_162, BT_CMDDESC_162},
	{BT_CMDNAME_163, BT_CMDDESC_163},
	{BT_CMDNAME_164, BT_CMDDESC_164},
	{BT_CMDNAME_165, BT_CMDDESC_165},
	{BT_CMDNAME_166, BT_CMDDESC_166},
	{BT_CMDNAME_167, BT_CMDDESC_167},
	{BT_CMDNAME_168, BT_CMDDESC_168},
	{BT_CMDNAME_169, BT_CMDDESC_169},
	

	{BT_CMDNAME_170, BT_CMDDESC_170},
	{BT_CMDNAME_171, BT_CMDDESC_171},
	{BT_CMDNAME_172, BT_CMDDESC_172},
	{BT_CMDNAME_173, BT_CMDDESC_173},
	{BT_CMDNAME_174, BT_CMDDESC_174},
	{BT_CMDNAME_175, BT_CMDDESC_175},
	{BT_CMDNAME_176, BT_CMDDESC_176},
	{BT_CMDNAME_177, BT_CMDDESC_177},
	{BT_CMDNAME_178, BT_CMDDESC_178},
	{BT_CMDNAME_179, BT_CMDDESC_179},
	

	{BT_CMDNAME_180, BT_CMDDESC_180},
	{BT_CMDNAME_181, BT_CMDDESC_181},
	{BT_CMDNAME_182, BT_CMDDESC_182},
	{BT_CMDNAME_183, BT_CMDDESC_183},
	{BT_CMDNAME_184, BT_CMDDESC_184},
	{BT_CMDNAME_185, BT_CMDDESC_185},
	{BT_CMDNAME_186, BT_CMDDESC_186},
	{BT_CMDNAME_187, BT_CMDDESC_187},
	{BT_CMDNAME_188, BT_CMDDESC_188},
	{BT_CMDNAME_189, BT_CMDDESC_189},
	

	{BT_CMDNAME_190, BT_CMDDESC_190},
	{BT_CMDNAME_191, BT_CMDDESC_191},
	{BT_CMDNAME_192, BT_CMDDESC_192},
	{BT_CMDNAME_193, BT_CMDDESC_193},
	{BT_CMDNAME_194, BT_CMDDESC_194},
	{BT_CMDNAME_195, BT_CMDDESC_195},
	{BT_CMDNAME_196, BT_CMDDESC_196},
	{BT_CMDNAME_197, BT_CMDDESC_197},
	{BT_CMDNAME_198, BT_CMDDESC_198},
	{BT_CMDNAME_199, BT_CMDDESC_199},
	
	{BT_CMDNAME_200, BT_CMDDESC_200},
	{BT_CMDNAME_201, BT_CMDDESC_201},
	{BT_CMDNAME_202, BT_CMDDESC_202},
	{BT_CMDNAME_203, BT_CMDDESC_203},
	{BT_CMDNAME_204, BT_CMDDESC_204},
	{BT_CMDNAME_205, BT_CMDDESC_205},
	{BT_CMDNAME_206, BT_CMDDESC_206},
	{BT_CMDNAME_207, BT_CMDDESC_207},
	{BT_CMDNAME_208, BT_CMDDESC_208},
	{BT_CMDNAME_209, BT_CMDDESC_209},
	
	{BT_CMDNAME_210, BT_CMDDESC_210},
	{BT_CMDNAME_211, BT_CMDDESC_211},
	{BT_CMDNAME_212, BT_CMDDESC_212},
	{BT_CMDNAME_213, BT_CMDDESC_213},
	{BT_CMDNAME_214, BT_CMDDESC_214},
	{BT_CMDNAME_215, BT_CMDDESC_215},
	{BT_CMDNAME_216, BT_CMDDESC_216},
	{BT_CMDNAME_217, BT_CMDDESC_217},
	{BT_CMDNAME_218, BT_CMDDESC_218},
	{BT_CMDNAME_219, BT_CMDDESC_219},
	
	{BT_CMDNAME_220, BT_CMDDESC_220},
	{BT_CMDNAME_221, BT_CMDDESC_221},
	{BT_CMDNAME_222, BT_CMDDESC_222},
	{BT_CMDNAME_223, BT_CMDDESC_223},
	{BT_CMDNAME_224, BT_CMDDESC_224},
	{BT_CMDNAME_225, BT_CMDDESC_225},
	{BT_CMDNAME_226, BT_CMDDESC_226},
	{BT_CMDNAME_227, BT_CMDDESC_227},
	{BT_CMDNAME_228, BT_CMDDESC_228},
	{BT_CMDNAME_229, BT_CMDDESC_229},
	
	{BT_CMDNAME_230, BT_CMDDESC_230},
	{BT_CMDNAME_231, BT_CMDDESC_231},
	{BT_CMDNAME_232, BT_CMDDESC_232},
	{BT_CMDNAME_233, BT_CMDDESC_233},
	{BT_CMDNAME_234, BT_CMDDESC_234},
	{BT_CMDNAME_235, BT_CMDDESC_235},
	{BT_CMDNAME_236, BT_CMDDESC_236},
	{BT_CMDNAME_237, BT_CMDDESC_237},
	{BT_CMDNAME_238, BT_CMDDESC_238},
	{BT_CMDNAME_239, BT_CMDDESC_239},
	
	{BT_CMDNAME_240, BT_CMDDESC_240},
	{BT_CMDNAME_241, BT_CMDDESC_241},
	{BT_CMDNAME_242, BT_CMDDESC_242},
	{BT_CMDNAME_243, BT_CMDDESC_243},
	{BT_CMDNAME_244, BT_CMDDESC_244},
	{BT_CMDNAME_245, BT_CMDDESC_245},
	{BT_CMDNAME_246, BT_CMDDESC_246},
	{BT_CMDNAME_247, BT_CMDDESC_247},
	{BT_CMDNAME_248, BT_CMDDESC_248},
	{BT_CMDNAME_249, BT_CMDDESC_249},
	
	{BT_CMDNAME_250, BT_CMDDESC_250},
	{BT_CMDNAME_251, BT_CMDDESC_251},
	{BT_CMDNAME_252, BT_CMDDESC_252},
	{BT_CMDNAME_253, BT_CMDDESC_253},
	{BT_CMDNAME_254, BT_CMDDESC_254}, 


	// last cmd
	{CMDNAME_LAST, CMDDESC_BLANK}
};
