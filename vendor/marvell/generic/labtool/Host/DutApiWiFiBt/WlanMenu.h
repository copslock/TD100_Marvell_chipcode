#ifndef __WLAN_MENU_H__
#define __WLAN_MENU_H__
#include "../DutCommon/MenuDisplay.h"

#if defined(_W8782_)
#define WLAN_CMDNAME_0			"--------------------------------------------------------\n"\
								"		W8782 (802.11a/g/b/n) TEST MENU\n"\
								"--------------------------------------------------------\n"
#else
#define WLAN_CMDNAME_0			"--------------------------------------------------------\n"\
								"		W8787 (802.11a/g/b/n) TEST MENU\n"\
								"--------------------------------------------------------\n"
#endif //_W8782_
#define WLAN_CMDDESC_0			CMDDESC_BLANK

#define WLAN_CMDNAME_1			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_1			CMDDESC_BLANK

#define WLAN_CMDNAME_2			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_2			CMDDESC_BLANK

#define WLAN_CMDNAME_3			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_3			CMDDESC_BLANK

#define WLAN_CMDNAME_4			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_4			CMDDESC_BLANK

#define WLAN_CMDNAME_5			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_5			CMDDESC_BLANK

#define WLAN_CMDNAME_6			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_6			CMDDESC_BLANK

#define WLAN_CMDNAME_7			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_7			CMDDESC_BLANK

#define WLAN_CMDNAME_8			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_8			CMDDESC_BLANK

#if defined(_W8782_)
#define WLAN_CMDNAME_9			"Get Antenna\n"
#define WLAN_CMDDESC_9			"\t0 - Main Antenna, 1 - AUX Antenna\n"

#define WLAN_CMDNAME_10			"Set Antenna\n"
#define WLAN_CMDDESC_10			"\t0 - Main Antenna, 1 - AUX Antenna\n"
#else
#define WLAN_CMDNAME_9			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_9			CMDDESC_BLANK

#define WLAN_CMDNAME_10			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_10			CMDDESC_BLANK
#endif //#if defined(_W8782_)

#define WLAN_CMDNAME_11			"Get RF Channel\n"
#define WLAN_CMDDESC_11			"\t(Get current RF channel number \n"
									

#define WLAN_CMDNAME_12			"Set RF Channel\n"
#define WLAN_CMDDESC_12			"\t(channel)\n"\
								"\t(Set RF Channel number \n"\
                                "\tExample: 12 1\n"

#if defined(_W8787_)
#define WLAN_CMDNAME_13			"Get RF DataRate\n"
#define WLAN_CMDDESC_13			"\t(1 for  1M;  2 for  2M;  3 for 5.5M; 4 for 11M;  5 for 22M; \n"\
                				"\t 6 for  6M;  7 for  9M;  8 for 12M;  9 for 18M; 10 for 24M; \n"\
				                "\t 11 for 36M; 12 for 48M; 13 for 54M; 14 for 72M;\n"\
								"\t 15 for  MCS0;  16 for  MCS1;  17 for  MCS2;\n"\
								"\t 18 for  MCS3;  19 for  MCS4;   20 for  MCS5;\n"\
								"\t 21 for  MCS6;  22 for  MCS7;   23 for  MCS8;\n"\
								"\t 24 for  MCS9;  25 for  MCS10;  26 for  MCS11;\n"\
								"\t 27 for  MCS12; 28 for  MCS13;  29 for  MCS14;\n"\
								"\t 30 for  MCS15)\n"

#define WLAN_CMDNAME_14			"Set RF DataRate\n"
#define WLAN_CMDDESC_14			"\t(rate)\n"\
								"\t(1 for  1M;  2 for  2M;  3 for 5.5M; 4 for 11M,  5 for 22M, \n"\
                				"\t 6 for  6M;  7 for  9M;  8 for 12M;  9 for 18M, 10 for 24M, \n"\
				                "\t 11 for 36M; 12 for 48M; 13 for 54M; 14 for 72M;\n"\
								"\t 15 for  MCS0;  16 for  MCS1;  17 for  MCS2;\n"\
								"\t 18 for  MCS3;  19 for  MCS4;   20 for  MCS5;\n"\
								"\t 21 for  MCS6;  22 for  MCS7;   23 for  MCS8;\n"\
								"\t 24 for  MCS9;  25 for  MCS10;  26 for  MCS11;\n"\
								"\t 27 for  MCS12; 28 for  MCS13;  29 for  MCS14;\n"\
								"\t 30 for  MCS15)\n"\
                                "\tExample: 14 1\n"
#else
#define WLAN_CMDNAME_13			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_13			CMDDESC_BLANK

#define WLAN_CMDNAME_14			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_14			CMDDESC_BLANK
#endif //#if defined(_W8787_)

#define WLAN_CMDNAME_15			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_15			CMDDESC_BLANK

#define WLAN_CMDNAME_16			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_16			CMDDESC_BLANK

#define WLAN_CMDNAME_17			"Set Continuous Tx Mode \n"
#define WLAN_CMDDESC_17			"\t(enable) (datarate) (pattern=0) (carrier_supresssion_mode=0)\n"\
								"\tExample: 17 1 4\n"

#define WLAN_CMDNAME_18			"Set CW Tx Mode\n"
#define WLAN_CMDDESC_18			"\t(Enable)\n"\
                            	"\tEnable: 0: off; 1: on\n"\
                                "\tExample: 18 1\n"

#if defined(_W8787_)
#define WLAN_CMDNAME_19			"Set Carrier Suppression Tx Mode\n"
#define WLAN_CMDDESC_19			"\t(Enable)\n"\
                            	"\tEnable: 0: off; 1: on\n"\
                                "\tExample: 19 1\n"
#else
#define WLAN_CMDNAME_19			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_19			CMDDESC_BLANK
#endif //#if defined(_W8787_)

#ifndef _DEVELOPMENT_PHASE_
#define WLAN_CMDNAME_20			"Get Beacon period\n"
#define WLAN_CMDDESC_20			"\treturns Beacon period\n\n"\

#define WLAN_CMDNAME_21			"Set Beacon period\n"
#define WLAN_CMDDESC_21			"\tperiod\n"\
                                 "\t\tExample: 21 -2.5\n"
#else
#define WLAN_CMDNAME_20			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_20			CMDDESC_BLANK

#define WLAN_CMDNAME_21			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_21			CMDDESC_BLANK
#endif  //#ifndef _DEVELOPMENT_PHASE_

#define WLAN_CMDNAME_22			"Set Power at Antenna Using Cal data\n"
#define WLAN_CMDDESC_22			"\t(channel) (power_level) ([modulation=0:0=b, 1=g])\n"\
                                 "\tExample: 22 1 13\n"
                
#define WLAN_CMDNAME_23			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_23			CMDDESC_BLANK

#define WLAN_CMDNAME_24			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_24			CMDDESC_BLANK

#define WLAN_CMDNAME_25			"Set DutyCycle Tx Mode \n"
#define WLAN_CMDDESC_25			"\t(enable=0(0|1)) (dataRate=4) "\
        						"[(payloadweight =50)(pattern=0)(shortPreamble=0(0|1))]\n"\
								"\t(ActSubCh=3(lower=0, upper=1, Both path 2 or 3)\n"\
								"\t(ShortGI=0(0|1)), (AdvCoding=0(0|1))\n"\
								"\t(TxBfOn=0(0|1)) (GFMode=0(0|1))  (STBC=0 (2bits))\n"\
								"\t(Compensation=0 (1 for Enable, 0 for Disable)) (PowerID=0 (4bit))\n"\
								"\t(SignalBw=-1 (-1=Follwing deviceBw as in cmd112/111; 0=20MHz; 1=40MHz))\n"\
		                    	"\tExample: 25 1 4\n"

#define WLAN_CMDNAME_26			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_26			CMDDESC_BLANK

#define WLAN_CMDNAME_27			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_27			CMDDESC_BLANK

#if defined(_W8787_)
#define WLAN_CMDNAME_28			"Put DUT into Deep Sleep Mode\n"
#define WLAN_CMDDESC_28			CMDDESC_BLANK
#else
#define WLAN_CMDNAME_28			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_28			CMDDESC_BLANK
#endif //#if defined(_W8787_)

#define WLAN_CMDNAME_29			"Get RF Band\n"
#define WLAN_CMDDESC_29			"\treturns RF Band\n"\
                                "\t0 - 2.4G, 1- 5G\n"

#define WLAN_CMDNAME_30			"Set RF Band\n"
#define WLAN_CMDDESC_30			"\t(band)\n"\
                                "\t0 - 2.4G, 1- 5G\n"

#define WLAN_CMDNAME_31			"Clear received packet Count\n"
#define WLAN_CMDDESC_31			CMDDESC_BLANK

#define WLAN_CMDNAME_32			"Get received packet Count\n"
#define WLAN_CMDDESC_32			"\tReturns counts for Rx, multi-cast and Error packets\n"

#define WLAN_CMDNAME_33			"Tx MultiCast Packet\n"

#if defined(_W8782_)
#define WLAN_CMDDESC_33			"\t(len=0x400) (Count=0x64) (rate=4) \n"\
                    			"\t(pattern=0xAA) (shortPreamble=1) (bssid-xx.xx.xx.xx.xx.xx)\n"\
                                "\t(Compensation Enable=0) (PowerID=0)\n"\
		                    	"\tExample: 33 400 64\n"
#else
#define WLAN_CMDDESC_33			"\t(len=400(0x)) (Count=64(0x)) (rate=4) \n"\
                    			"\t(pattern=AA) (shortPreamble=1)\n"\
								"\t(ActSubCh=3(lower=0, upper=1, Both path 2 or 3)\n"\
								"\t(ShortGI=0(0|1)) (AdvCoding=0(0|1))\n"\
								"\t(TxBfOn=0(0|1)) (GFMode=0(0|1))  (STBC=0 (2bits))\n"\
								"\t(Compensation=0 (1 for Enable, 0 for Disable)) (PowerID=0 (4bit))\n"\
								"\t(bssid-xx.xx.xx.xx.xx.xx) \n"\
		                    	"\tExample: 33 400 64\n"
#endif	//#if defined(_W8782_)
		                    	

#define WLAN_CMDNAME_34			"Enable Bssid Filter\n"
#define WLAN_CMDDESC_34			"\t(enable=0) (bssid)\n"\
								"\tenable: 0: off; 1: on\n"\
								"\t(bssid-xx.xx.xx.xx.xx.xx) \n"\
								"\tExample: 34 1 00.11.22.33.44.55\n"

#if defined (_W8787_) || 	defined (_W8782_)						
#define WLAN_CMDNAME_35			"Adjust Packet Gap with Sifs\n"
#define WLAN_CMDDESC_35			"\t(enable=0(0|1)) (rate=4) \n"\
								"\t(AdjustTxBurstGap=0(0|1)) (BurstSifsInUs=(0)(0-255))\n"\
                    			"\t(len=400(0x)) (pattern=AA) (shortPreamble=1)\n"\
								"\t(ActSubCh=3(lower=0, upper=1, Both path 2 or 3)\n"\
								"\t(ShortGI=0(0|1)) (AdvCoding=0(0|1))\n"\
								"\t(TxBfOn=0(0|1)) (GFMode=0(0|1))  (STBC=0 (2bits))\n"\
								"\t(Compensation=0 (1 for Enable, 0 for Disable)) (PowerID=0 (4bit))\n"\
								"\t(SignalBw=-1 (-1=Follwing deviceBw as in cmd112/111; 0=20MHz; 1=40MHz))\n"\
								"\t(bssid-xx.xx.xx.xx.xx.xx) \n"\
		                    	"\tExample: 35 1 4\n"
#else
#define WLAN_CMDNAME_35			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_35			CMDDESC_BLANK
#endif //#if defined (_W8787_) || 	defined (_W8782_)						

#define WLAN_CMDNAME_36			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_36			CMDDESC_BLANK

#define WLAN_CMDNAME_37			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_37			CMDDESC_BLANK

#define WLAN_CMDNAME_38			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_38			CMDDESC_BLANK

#ifdef _FLASH_
#define WLAN_CMDNAME_39			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_39			CMDDESC_BLANK
#else //#ifdef _FLASH_
#define WLAN_CMDNAME_39			"SPI download from binary file\n"
#define WLAN_CMDDESC_39			"\tWill prompt for file name\n"
#endif //#ifdef _FLASH_

#define WLAN_CMDNAME_40			"SPI Verify from binary file\n"
#define WLAN_CMDDESC_40			"\tWill prompt for file name\n"

#define WLAN_CMDNAME_41			"Dump E2PROM content\n"
#define WLAN_CMDDESC_41			"\t(Addr=0(0x)) (len=10(0x))\n"\
								"\tAddr: starting address in hex\n"\
                                "\tlen: number of DWORDs to be dumpped in hex\n"

#if defined(_W8787_)
#define WLAN_CMDNAME_42			"Get E2PROM type\n"
#define WLAN_CMDDESC_42			"\t(if) (addrLen) (device)\n"\
            					"\tIf:\tinterface type: 0=detect-again, 1=SPI, 2=I2C\n"\
            					"\taddrLen:\taddress width in byte. valid 1 - 3\n"\
			            		"\tdevice:\tdevice type: 1=eeprom, 2=flash\n" 

#define WLAN_CMDNAME_43			"Force E2PROM type\n"
#define WLAN_CMDDESC_43			"\t(if) (addrLen) (device)\n"\
            					"\tIf:\tinterface type: 0=detect-again, 1=SPI, 2=I2C\n"\
			        		    "\taddrLen:\taddress width in byte. valid 1 - 3\n"\
    					        "\tdevice:\tdevice type: 1=eeprom, 2=flash\n"
#else
#define WLAN_CMDNAME_42			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_42			CMDDESC_BLANK

#define WLAN_CMDNAME_43			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_43			CMDDESC_BLANK
#endif //#if defined(_W8787_)

#define WLAN_CMDNAME_44			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_44			CMDDESC_BLANK

#define WLAN_CMDNAME_45			"Read MAC Address From EEPROM\n"
#define WLAN_CMDDESC_45			CMDDESC_BLANK

#define WLAN_CMDNAME_46			"Write MACAddress in EEPROM\n"
#define WLAN_CMDDESC_46			"\t MACAddress (xx.xx.xx.xx.xx.xx)\n"

#define WLAN_CMDNAME_47			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_47			CMDDESC_BLANK

#define WLAN_CMDNAME_48			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_48			CMDDESC_BLANK
 
#define WLAN_CMDNAME_49			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_49			CMDDESC_BLANK

#define WLAN_CMDNAME_50			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_50			CMDDESC_BLANK

#ifdef _FLASH_
#define WLAN_CMDNAME_51			"Erase flash\n"
#define WLAN_CMDDESC_51			"\t((0x )(section-starting-address)\n"
#else //#ifdef _FLASH_
#define WLAN_CMDNAME_51			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_51			CMDDESC_BLANK
#endif //#ifdef _FLASH_

#define WLAN_CMDNAME_52			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_52			CMDDESC_BLANK

#define WLAN_CMDNAME_53			"Set Cal from file\n"
#define WLAN_CMDDESC_53			CMDDESC_BLANK

#define WLAN_CMDNAME_54			"Get Cal from EEPROMinto files\n"
#define WLAN_CMDDESC_54			CMDDESC_BLANK

#define WLAN_CMDNAME_55			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_55			CMDDESC_BLANK

#define WLAN_CMDNAME_56			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_56			CMDDESC_BLANK

#define WLAN_CMDNAME_57			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_57			CMDDESC_BLANK

#define WLAN_CMDNAME_58			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_58			CMDDESC_BLANK

#define WLAN_CMDNAME_59			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_59			CMDDESC_BLANK

#if defined(_W8787_)
#define WLAN_CMDNAME_60			"Read MAC Reg\n"
#define WLAN_CMDDESC_60			"\t(addr=0(0x)) [count=1(0x)]\n"\
								"\taddr: MAC address in hex, onl last three digits required\n"\
								"\tcount: number of registers to be read in hex\n"\
		                    	"\tExample: 60 1b8 a\n"

#define WLAN_CMDNAME_61			"Write MAC Reg\n"
#define WLAN_CMDDESC_61			"\t(addr=0(0x)))(data=0(0x))\n"\
								"\taddr: MAC address to be written to in hex\n"\
								"\tdata: data to be written in hex\n"\
		                    	"\tExample: 61 1b8 12ab34cd\n"

#define WLAN_CMDNAME_62			"Read BBU Reg\n"
#define WLAN_CMDDESC_62			"\t(addr=0(0x)) (count=1(0x))\n"\
								"\taddr: BBU address to read from in hex\n"\
								"\tcount:number of registers to read in hex\n"\
								"\tExample: 62 1a a\n"

#define WLAN_CMDNAME_63			"Write BBU Reg\n"
#define WLAN_CMDDESC_63			"\t(addr=0(0x))(data=0(0x))\n"\
								"\taddr: BBU address to write to in hex\n"\
								"\tdata: data to be written in hex\n"\
								"\tExample: 63 1a a\n"

#define WLAN_CMDNAME_64			"Read RF Reg\n"
#define WLAN_CMDDESC_64			"\t(addr=0(0x)) (count=1(0x))\n"\
								"\taddr: RF address to read from in hex\n"\
								"\tcount:number of registers to read in hex\n"\
								"\tExample: 64 1a a\n"

#define WLAN_CMDNAME_65			"Write RF Reg\n"
#define WLAN_CMDDESC_65			"\t(addr=0(0x))(data=0(0x))\n"\
								"\taddr: RF address to write to in hex\n"\
								"\tdata: data to be written in hex\n"\
								"\tExample: 65 1a a\n"

#define WLAN_CMDNAME_66			"Read CAU Reg\n"
#define WLAN_CMDDESC_66			"\t(addr=0(0x)) (count=1(0x))\n"\
								"\taddr: CAU address to read from in hex\n"\
								"\tcount:number of registers to read in hex\n"\
								"\tExample: 66 1a a\n"

#define WLAN_CMDNAME_67			"Write CAU Reg\n"
#define WLAN_CMDDESC_67			"\t(addr=0(0x))(data=0(0x))\n"\
								"\taddr: CAU address to write to in hex\n"\
								"\tdata: data to be written in hex\n"\
								"\tExample: 67 1a a\n"

#define WLAN_CMDNAME_68			"Peek SOC Mem\n"
#define WLAN_CMDDESC_68			"\t(addr=0(0x)) (count=1(0x))\n"\
								"\taddr: SOC address to read from in hex\n"\
								"\tcount:number of registers to read in hex\n"\
								"\tExample: 68 1a2b a\n"

#define WLAN_CMDNAME_69			"Poke SOC Mem\n"
#define WLAN_CMDDESC_69			"\t(addr=0(0x))(data=0(0x))\n"\
								"\taddr: SOC address to write to in hex\n"\
								"\tdata: data to be written in hex\n"\
								"\tExample: 69 1a a\n"

#define WLAN_CMDNAME_70			"Peek Spi\n"
#define WLAN_CMDDESC_70			"\t(addr=0(0x))\n"\
								"\taddr: SPI address to read from in hex\n"\
								"\tExample: 70 9c\n"

#define WLAN_CMDNAME_71			"Poke Spi\n"
#define WLAN_CMDDESC_71			"\t(addr=0(0x))(data=0(0x))\n"\
								"\taddr: SPI address to write to in hex\n"\
								"\tdata: data to be written in hex\n"\
								"\tExample: 71 80 abcd1234\n"
#else
#define WLAN_CMDNAME_60			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_60			CMDDESC_BLANK

#define WLAN_CMDNAME_61			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_61			CMDDESC_BLANK

#define WLAN_CMDNAME_62			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_62			CMDDESC_BLANK

#define WLAN_CMDNAME_63			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_63			CMDDESC_BLANK

#define WLAN_CMDNAME_64			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_64			CMDDESC_BLANK

#define WLAN_CMDNAME_65			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_65			CMDDESC_BLANK

#define WLAN_CMDNAME_66			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_66			CMDDESC_BLANK

#define WLAN_CMDNAME_67			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_67			CMDDESC_BLANK

#define WLAN_CMDNAME_68			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_68			CMDDESC_BLANK

#define WLAN_CMDNAME_69			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_69			CMDDESC_BLANK

#define WLAN_CMDNAME_70			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_70			CMDDESC_BLANK

#define WLAN_CMDNAME_71			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_71			CMDDESC_BLANK
#endif //#if defined(_W8787_)

#define WLAN_CMDNAME_72			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_72			CMDDESC_BLANK

#define WLAN_CMDNAME_73			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_73			CMDDESC_BLANK

#define WLAN_CMDNAME_74			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_74			CMDDESC_BLANK

#define WLAN_CMDNAME_75			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_75			CMDDESC_BLANK

#define WLAN_CMDNAME_76			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_76			CMDDESC_BLANK

#define WLAN_CMDNAME_77			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_77			CMDDESC_BLANK

#define WLAN_CMDNAME_78			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_78			CMDDESC_BLANK
 
#define WLAN_CMDNAME_79			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_79			CMDDESC_BLANK

#define WLAN_CMDNAME_80			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_80			CMDDESC_BLANK

#define WLAN_CMDNAME_81			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_81			CMDDESC_BLANK

#define WLAN_CMDNAME_82			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_82			CMDDESC_BLANK

#define WLAN_CMDNAME_83			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_83			CMDDESC_BLANK

#define WLAN_CMDNAME_84			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_84			CMDDESC_BLANK

#define WLAN_CMDNAME_85			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_85			CMDDESC_BLANK

#define WLAN_CMDNAME_86			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_86			CMDDESC_BLANK

#define WLAN_CMDNAME_87			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_87			CMDDESC_BLANK

#define WLAN_CMDNAME_88			"FW/HW Version\n"
#define WLAN_CMDDESC_88			CMDDESC_BLANK

#define WLAN_CMDNAME_89			"Load Dut configuration file\n"
#define WLAN_CMDDESC_89			"\t(FileName)\n"

#define WLAN_CMDNAME_90			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_90			CMDDESC_BLANK

#define WLAN_CMDNAME_91			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_91			CMDDESC_BLANK

#define WLAN_CMDNAME_92			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_92			CMDDESC_BLANK

#define WLAN_CMDNAME_93			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_93			CMDDESC_BLANK

#define WLAN_CMDNAME_94			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_94			CMDDESC_BLANK

#if defined(_W8787_)
#define WLAN_CMDNAME_95			"Get Rf XTAL control\n"
#define WLAN_CMDDESC_95			CMDDESC_BLANK

#define WLAN_CMDNAME_96			"Set Rf XTAL control\n"
#define WLAN_CMDDESC_96			"\t(setting(8-bit))\n"
#else
#define WLAN_CMDNAME_95			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_95			CMDDESC_BLANK

#define WLAN_CMDNAME_96			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_96			CMDDESC_BLANK
#endif //#if defined(_W8787_)

#define WLAN_CMDNAME_97			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_97			CMDDESC_BLANK

#define WLAN_CMDNAME_98			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_98			CMDDESC_BLANK

#define WLAN_CMDNAME_99			"Exit\n"
#define WLAN_CMDDESC_99			CMDDESC_BLANK

#define WLAN_CMDNAME_100		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_100		CMDDESC_BLANK

#define WLAN_CMDNAME_101		"Get rf control mode\n"
#define WLAN_CMDDESC_101		"\t-1 - internal control\n"\
								"\t 0 - close loop\n"\
                                "\t 1 - open loop\n"

#define WLAN_CMDNAME_102		"Set rf control mode\n"
#define WLAN_CMDDESC_102		"\t(mode)\n\t-1 - internal control\n"\
                                "\t 0 - close loop\n"\
								"\t 1 - open loop\n"

#define WLAN_CMDNAME_103		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_103		CMDDESC_BLANK

#define WLAN_CMDNAME_104		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_104		CMDDESC_BLANK

#define WLAN_CMDNAME_105		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_105		CMDDESC_BLANK

#define WLAN_CMDNAME_106		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_106		CMDDESC_BLANK

#define WLAN_CMDNAME_107		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_107		CMDDESC_BLANK

#define WLAN_CMDNAME_108		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_108		CMDDESC_BLANK

#define WLAN_CMDNAME_109		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_109		CMDDESC_BLANK

#define WLAN_CMDNAME_110		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_110		CMDDESC_BLANK

#define WLAN_CMDNAME_111		"Get Channel BW\n"
#define WLAN_CMDDESC_111		"\t(0: 20MHz, 1: 40MHz, 4: 10MHz, 5: 5MHz\n\t\t2 and 3 are invalid for this chip)\n"

#define WLAN_CMDNAME_112		"Set Channel BW\n"
#define WLAN_CMDDESC_112		"\t(0: 20MHz, 1: 40MHz, 4: 10MHz, 5: 5MHz\n\t\t2 and 3 are invalid for this chip)\n"

#define WLAN_CMDNAME_113		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_113		CMDDESC_BLANK

#define WLAN_CMDNAME_114	    CMDNAME_UNKNOWN
#define WLAN_CMDDESC_114		CMDDESC_BLANK

#define WLAN_CMDNAME_115		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_115		CMDDESC_BLANK

#define WLAN_CMDNAME_116		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_116		CMDDESC_BLANK

#define WLAN_CMDNAME_117		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_117		CMDDESC_BLANK

#if defined(_W8787_)
#define WLAN_CMDNAME_118		"Get Tx IQ Setting\n"
#define WLAN_CMDDESC_118		CMDDESC_BLANK

#define WLAN_CMDNAME_119		"Set Tx IQ Setting\n"
#define WLAN_CMDDESC_119		"\t(amp)(phase)\n"
#else
#define WLAN_CMDNAME_118		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_118		CMDDESC_BLANK

#define WLAN_CMDNAME_119		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_119		CMDDESC_BLANK
#endif //#if defined(_W8787_)

#define WLAN_CMDNAME_120		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_120		CMDDESC_BLANK

#if defined(_W8787_)
#define WLAN_CMDNAME_121		"Thermal Compensation\n"
#define WLAN_CMDDESC_121		"\t(enable=0) [(SlopNum_hot=26) (SlopNum_cold=13) (SlopDenom=5)\n"\
								"\t(TempAtCal=70(0x)) (TempSlope=70(0x)) (CalInMs=500) (PTargetDelta=20(0x))]\n"

#define WLAN_CMDNAME_122		"Set FEM\n"
#define WLAN_CMDDESC_122		"\tit will read from a file which is specified in \"setup.ini\"\n"\
                                "\tsection[HW_CONFIG], field \"FEM_FILENAME\"\n"
#else
#define WLAN_CMDNAME_121		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_121		CMDDESC_BLANK

#define WLAN_CMDNAME_122		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_122		CMDDESC_BLANK
#endif //#if defined(_W8787_)

#define WLAN_CMDNAME_123		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_123		CMDDESC_BLANK

#define WLAN_CMDNAME_124		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_124		CMDDESC_BLANK

#define WLAN_CMDNAME_125		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_125		CMDDESC_BLANK

#define WLAN_CMDNAME_126		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_126		CMDDESC_BLANK

#define WLAN_CMDNAME_127		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_127		CMDDESC_BLANK

#define WLAN_CMDNAME_128		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_128		CMDDESC_BLANK
 
#define WLAN_CMDNAME_129		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_129		CMDDESC_BLANK

#if defined(_W8787_)
#define WLAN_CMDNAME_130		"Perform Tx/Rx IQ Cal (avg(10))\n"
#define WLAN_CMDDESC_130		CMDDESC_BLANK
#else
#define WLAN_CMDNAME_130		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_130		CMDDESC_BLANK
#endif //#if defined(_W8787_)

#if defined(_W8787_)
#define WLAN_CMDNAME_131		"Get Rx IQ Setting\n"
#define WLAN_CMDDESC_131		CMDDESC_BLANK

#define WLAN_CMDNAME_132		"Set Rx IQ Setting\n"
#define WLAN_CMDDESC_132		"\t(amp)(phase)\n"
#else
#define WLAN_CMDNAME_131		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_131		CMDDESC_BLANK

#define WLAN_CMDNAME_132		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_132		CMDDESC_BLANK
#endif //#if defined(_W8787_)

#define WLAN_CMDNAME_133		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_133		CMDDESC_BLANK

#define WLAN_CMDNAME_134		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_134		CMDDESC_BLANK

#define WLAN_CMDNAME_135		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_135		CMDDESC_BLANK

#define WLAN_CMDNAME_136		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_136		CMDDESC_BLANK

#define WLAN_CMDNAME_137		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_137		CMDDESC_BLANK

#define WLAN_CMDNAME_138		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_138		CMDDESC_BLANK

#define WLAN_CMDNAME_139		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_139		CMDDESC_BLANK

#if defined(_W8787_)
#define WLAN_CMDNAME_140		"Download RFU FW\n"
#define WLAN_CMDDESC_140		CMDDESC_BLANK
#else
#define WLAN_CMDNAME_140		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_140		CMDDESC_BLANK
#endif //#if defined(_W8787_)

#define WLAN_CMDNAME_141		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_141		CMDDESC_BLANK

#define WLAN_CMDNAME_142		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_142		CMDDESC_BLANK

#define WLAN_CMDNAME_143		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_143		CMDDESC_BLANK

#define WLAN_CMDNAME_144		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_144		CMDDESC_BLANK

#define WLAN_CMDNAME_145		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_145		CMDDESC_BLANK

#define WLAN_CMDNAME_146		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_146		CMDDESC_BLANK

#define WLAN_CMDNAME_147		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_147		CMDDESC_BLANK

#define WLAN_CMDNAME_148		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_148		CMDDESC_BLANK

#define WLAN_CMDNAME_149		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_149		CMDDESC_BLANK

#define WLAN_CMDNAME_150		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_150		CMDDESC_BLANK

#define WLAN_CMDNAME_151		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_151		CMDDESC_BLANK

#define WLAN_CMDNAME_152		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_152		CMDDESC_BLANK

#define WLAN_CMDNAME_153		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_153		CMDDESC_BLANK

#define WLAN_CMDNAME_154		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_154		CMDDESC_BLANK

#define WLAN_CMDNAME_155		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_155		CMDDESC_BLANK

#define WLAN_CMDNAME_156		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_156		CMDDESC_BLANK

#define WLAN_CMDNAME_157		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_157		CMDDESC_BLANK

#define WLAN_CMDNAME_158		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_158		CMDDESC_BLANK

#define WLAN_CMDNAME_159		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_159		CMDDESC_BLANK

#define WLAN_CMDNAME_160		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_160		CMDDESC_BLANK

#define WLAN_CMDNAME_161		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_161		CMDDESC_BLANK

#define WLAN_CMDNAME_162			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_162			CMDDESC_BLANK

#define WLAN_CMDNAME_163			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_163			CMDDESC_BLANK

#define WLAN_CMDNAME_164			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_164			CMDDESC_BLANK

#define WLAN_CMDNAME_165			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_165			CMDDESC_BLANK

#define WLAN_CMDNAME_166			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_166			CMDDESC_BLANK

#define WLAN_CMDNAME_167			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_167			CMDDESC_BLANK

#define WLAN_CMDNAME_168			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_168			CMDDESC_BLANK

#define WLAN_CMDNAME_169			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_169			CMDDESC_BLANK

#define WLAN_CMDNAME_170			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_170			CMDDESC_BLANK

#define WLAN_CMDNAME_171			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_171			CMDDESC_BLANK

#define WLAN_CMDNAME_172			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_172			CMDDESC_BLANK

#define WLAN_CMDNAME_173			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_173			CMDDESC_BLANK

#define WLAN_CMDNAME_174			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_174			CMDDESC_BLANK

#define WLAN_CMDNAME_175			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_175			CMDDESC_BLANK

#define WLAN_CMDNAME_176			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_176			CMDDESC_BLANK

#define WLAN_CMDNAME_177			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_177			CMDDESC_BLANK

#define WLAN_CMDNAME_178			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_178			CMDDESC_BLANK

#define WLAN_CMDNAME_179			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_179			CMDDESC_BLANK

#define WLAN_CMDNAME_180			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_180			CMDDESC_BLANK

#define WLAN_CMDNAME_181			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_181			CMDDESC_BLANK

#define WLAN_CMDNAME_182			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_182			CMDDESC_BLANK

#define WLAN_CMDNAME_183			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_183			CMDDESC_BLANK

#define WLAN_CMDNAME_184			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_184			CMDDESC_BLANK

#define WLAN_CMDNAME_185			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_185			CMDDESC_BLANK

#define WLAN_CMDNAME_186			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_186			CMDDESC_BLANK

#define WLAN_CMDNAME_187			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_187			CMDDESC_BLANK

#define WLAN_CMDNAME_188			CMDNAME_UNKNOWN	//"FW on-line release note\n"
#define WLAN_CMDDESC_188			CMDDESC_BLANK

#define WLAN_CMDNAME_189			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_189			CMDDESC_BLANK

#define WLAN_CMDNAME_190			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_190			CMDDESC_BLANK

#define WLAN_CMDNAME_191			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_191			CMDDESC_BLANK

#define WLAN_CMDNAME_192			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_192			CMDDESC_BLANK

#define WLAN_CMDNAME_193			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_193			CMDDESC_BLANK

#define WLAN_CMDNAME_194			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_194			CMDDESC_BLANK

#define WLAN_CMDNAME_195			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_195			CMDDESC_BLANK

#define WLAN_CMDNAME_196			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_196			CMDDESC_BLANK

#define WLAN_CMDNAME_197			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_197			CMDDESC_BLANK

#define WLAN_CMDNAME_198			CMDNAME_UNKNOWN
//#define WLAN_CMDNAME_198			"BBU CW RSSI reading\n"
#define WLAN_CMDDESC_198			CMDDESC_BLANK

#if defined(_W8787_)
#define WLAN_CMDNAME_199			"MAC RSSI reading\n" 
#define WLAN_CMDDESC_199			CMDDESC_BLANK
#else
#define WLAN_CMDNAME_199		CMDNAME_UNKNOWN
#define WLAN_CMDDESC_199		CMDDESC_BLANK
#endif //#if defined(_W8787_)

//#if defined (_W8782_)
#if 0
#define WLAN_CMDNAME_200			"Get Compensation Table\n"
#define WLAN_CMDDESC_200			"\tpower ID\n"\
									"\t# of row(1 for memory in 2 row, 0 for memory in 2 row)\n"
#else
#define WLAN_CMDNAME_200			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_200			CMDDESC_BLANK
#endif

//#if defined (_W8782_)
#if 0
#define WLAN_CMDNAME_201			"Set Compensation Table\n"
#define WLAN_CMDDESC_201			"\tpower ID\n"\
									"\t# of row(1 for memory 2 row, 0 for memory 1 row)\n"\
									"\tCompensation table entry in 4 byte hex value(18 enties in DWORD for memory 2 row, 9 enties in DWORD for memory 1 row)\n"
#else
#define WLAN_CMDNAME_201			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_201			CMDDESC_BLANK
#endif //_W8782_

#define WLAN_CMDNAME_202			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_202			CMDDESC_BLANK

#define WLAN_CMDNAME_203			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_203			CMDDESC_BLANK

#define WLAN_CMDNAME_204			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_204			CMDDESC_BLANK

#define WLAN_CMDNAME_205			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_205			CMDDESC_BLANK

#define WLAN_CMDNAME_206			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_206			CMDDESC_BLANK

#define WLAN_CMDNAME_207			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_207			CMDDESC_BLANK

#define WLAN_CMDNAME_208			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_208			CMDDESC_BLANK

#define WLAN_CMDNAME_209			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_209			CMDDESC_BLANK

#define WLAN_CMDNAME_210			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_210			CMDDESC_BLANK

//#if defined(_W8787_)
#if 0
#define WLAN_CMDNAME_211			"Get a Channel's Freq information\n"
#define WLAN_CMDDESC_211			"\t(band) (chan)\n"  
#else
#define WLAN_CMDNAME_211			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_211			CMDDESC_BLANK
#endif //#if 0

#define WLAN_CMDNAME_212			"Set Channel by Freq (in MHz)\n"
#define WLAN_CMDDESC_212			"\tFreq (in MHz)\n"\
									"\tneed to be in the right band first\n"

#define WLAN_CMDNAME_213			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_213			CMDDESC_BLANK

#define WLAN_CMDNAME_214			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_214			CMDDESC_BLANK

#define WLAN_CMDNAME_215			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_215			CMDDESC_BLANK

#define WLAN_CMDNAME_216			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_216			CMDDESC_BLANK

#define WLAN_CMDNAME_217			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_217			CMDDESC_BLANK

#define WLAN_CMDNAME_218			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_218			CMDDESC_BLANK

#define WLAN_CMDNAME_219			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_219			CMDDESC_BLANK

#define WLAN_CMDNAME_220			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_220			CMDDESC_BLANK

#define WLAN_CMDNAME_221			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_221			CMDDESC_BLANK

#define WLAN_CMDNAME_222			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_222			CMDDESC_BLANK

#define WLAN_CMDNAME_223			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_223			CMDDESC_BLANK

#define WLAN_CMDNAME_224			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_224			CMDDESC_BLANK

#define WLAN_CMDNAME_225			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_225			CMDDESC_BLANK

#define WLAN_CMDNAME_226			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_226			CMDDESC_BLANK

#define WLAN_CMDNAME_227			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_227			CMDDESC_BLANK

#define WLAN_CMDNAME_228			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_228			CMDDESC_BLANK

#define WLAN_CMDNAME_229			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_229			CMDDESC_BLANK

#define WLAN_CMDNAME_230			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_230			CMDDESC_BLANK

#define WLAN_CMDNAME_231			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_231			CMDDESC_BLANK

#define WLAN_CMDNAME_232			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_232			CMDDESC_BLANK

#define WLAN_CMDNAME_233			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_233			CMDDESC_BLANK

#define WLAN_CMDNAME_234			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_234			CMDDESC_BLANK

#define WLAN_CMDNAME_235			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_235			CMDDESC_BLANK

#define WLAN_CMDNAME_236			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_236			CMDDESC_BLANK

#define WLAN_CMDNAME_237			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_237			CMDDESC_BLANK

#define WLAN_CMDNAME_238			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_238			CMDDESC_BLANK

#define WLAN_CMDNAME_239			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_239			CMDDESC_BLANK

#define WLAN_CMDNAME_240			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_240			CMDDESC_BLANK

#define WLAN_CMDNAME_241			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_241			CMDDESC_BLANK

#define WLAN_CMDNAME_242			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_242			CMDDESC_BLANK

#define WLAN_CMDNAME_243			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_243			CMDDESC_BLANK

#define WLAN_CMDNAME_244			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_244			CMDDESC_BLANK

#define WLAN_CMDNAME_245			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_245			CMDDESC_BLANK

#define WLAN_CMDNAME_246			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_246			CMDDESC_BLANK

#define WLAN_CMDNAME_247			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_247			CMDDESC_BLANK

#define WLAN_CMDNAME_248			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_248			CMDDESC_BLANK

#define WLAN_CMDNAME_249			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_249			CMDDESC_BLANK

#define WLAN_CMDNAME_250			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_250			CMDDESC_BLANK

#define WLAN_CMDNAME_251			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_251			CMDDESC_BLANK

#define WLAN_CMDNAME_252			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_252			CMDDESC_BLANK

#define WLAN_CMDNAME_253			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_253			CMDDESC_BLANK

#define WLAN_CMDNAME_254			CMDNAME_UNKNOWN
#define WLAN_CMDDESC_254			CMDDESC_BLANK

CmdMenu WlanCmdNames[256]=
{
	{WLAN_CMDNAME_0, WLAN_CMDDESC_0},
	{WLAN_CMDNAME_1, WLAN_CMDDESC_1},
	{WLAN_CMDNAME_2, WLAN_CMDDESC_2},
	{WLAN_CMDNAME_3, WLAN_CMDDESC_3},
	{WLAN_CMDNAME_4, WLAN_CMDDESC_4},
	{WLAN_CMDNAME_5, WLAN_CMDDESC_5},
	{WLAN_CMDNAME_6, WLAN_CMDDESC_6},
	{WLAN_CMDNAME_7, WLAN_CMDDESC_7},
	{WLAN_CMDNAME_8, WLAN_CMDDESC_8},
	{WLAN_CMDNAME_9, WLAN_CMDDESC_9},

	{WLAN_CMDNAME_10, WLAN_CMDDESC_10},
	{WLAN_CMDNAME_11, WLAN_CMDDESC_11},
	{WLAN_CMDNAME_12, WLAN_CMDDESC_12},
	{WLAN_CMDNAME_13, WLAN_CMDDESC_13},
	{WLAN_CMDNAME_14, WLAN_CMDDESC_14},
	{WLAN_CMDNAME_15, WLAN_CMDDESC_15},
	{WLAN_CMDNAME_16, WLAN_CMDDESC_16},
	{WLAN_CMDNAME_17, WLAN_CMDDESC_17},
	{WLAN_CMDNAME_18, WLAN_CMDDESC_18},
	{WLAN_CMDNAME_19, WLAN_CMDDESC_19},
	
	{WLAN_CMDNAME_20, WLAN_CMDDESC_20},
	{WLAN_CMDNAME_21, WLAN_CMDDESC_21},
	{WLAN_CMDNAME_22, WLAN_CMDDESC_22},
	{WLAN_CMDNAME_23, WLAN_CMDDESC_23},
	{WLAN_CMDNAME_24, WLAN_CMDDESC_24},
	{WLAN_CMDNAME_25, WLAN_CMDDESC_25},
	{WLAN_CMDNAME_26, WLAN_CMDDESC_26},
	{WLAN_CMDNAME_27, WLAN_CMDDESC_27},
	{WLAN_CMDNAME_28, WLAN_CMDDESC_28},
	{WLAN_CMDNAME_29, WLAN_CMDDESC_29},

	{WLAN_CMDNAME_30, WLAN_CMDDESC_30},
	{WLAN_CMDNAME_31, WLAN_CMDDESC_31},
	{WLAN_CMDNAME_32, WLAN_CMDDESC_32},
	{WLAN_CMDNAME_33, WLAN_CMDDESC_33},
	{WLAN_CMDNAME_34, WLAN_CMDDESC_34},
	{WLAN_CMDNAME_35, WLAN_CMDDESC_35},
	{WLAN_CMDNAME_36, WLAN_CMDDESC_36},
	{WLAN_CMDNAME_37, WLAN_CMDDESC_37},
	{WLAN_CMDNAME_38, WLAN_CMDDESC_38},
	{WLAN_CMDNAME_39, WLAN_CMDDESC_39},

	{WLAN_CMDNAME_40, WLAN_CMDDESC_40},
	{WLAN_CMDNAME_41, WLAN_CMDDESC_41},
	{WLAN_CMDNAME_42, WLAN_CMDDESC_42},
	{WLAN_CMDNAME_43, WLAN_CMDDESC_43},
	{WLAN_CMDNAME_44, WLAN_CMDDESC_44},
	{WLAN_CMDNAME_45, WLAN_CMDDESC_45},
	{WLAN_CMDNAME_46, WLAN_CMDDESC_46},
	{WLAN_CMDNAME_47, WLAN_CMDDESC_47},
	{WLAN_CMDNAME_48, WLAN_CMDDESC_48},
	{WLAN_CMDNAME_49, WLAN_CMDDESC_49},

	{WLAN_CMDNAME_50, WLAN_CMDDESC_50},
	{WLAN_CMDNAME_51, WLAN_CMDDESC_51},
	{WLAN_CMDNAME_52, WLAN_CMDDESC_52},
	{WLAN_CMDNAME_53, WLAN_CMDDESC_53},
	{WLAN_CMDNAME_54, WLAN_CMDDESC_54},
	{WLAN_CMDNAME_55, WLAN_CMDDESC_55},
	{WLAN_CMDNAME_56, WLAN_CMDDESC_56},
	{WLAN_CMDNAME_57, WLAN_CMDDESC_57},
	{WLAN_CMDNAME_58, WLAN_CMDDESC_58},
	{WLAN_CMDNAME_59, WLAN_CMDDESC_59},

	{WLAN_CMDNAME_60, WLAN_CMDDESC_60},
	{WLAN_CMDNAME_61, WLAN_CMDDESC_61},
	{WLAN_CMDNAME_62, WLAN_CMDDESC_62},
	{WLAN_CMDNAME_63, WLAN_CMDDESC_63},
	{WLAN_CMDNAME_64, WLAN_CMDDESC_64},
	{WLAN_CMDNAME_65, WLAN_CMDDESC_65},
	{WLAN_CMDNAME_66, WLAN_CMDDESC_66},
	{WLAN_CMDNAME_67, WLAN_CMDDESC_67},
	{WLAN_CMDNAME_68, WLAN_CMDDESC_68},
	{WLAN_CMDNAME_69, WLAN_CMDDESC_69},

	{WLAN_CMDNAME_70, WLAN_CMDDESC_70},
	{WLAN_CMDNAME_71, WLAN_CMDDESC_71},
	{WLAN_CMDNAME_72, WLAN_CMDDESC_72},
	{WLAN_CMDNAME_73, WLAN_CMDDESC_73},
	{WLAN_CMDNAME_74, WLAN_CMDDESC_74},
	{WLAN_CMDNAME_75, WLAN_CMDDESC_75},
	{WLAN_CMDNAME_76, WLAN_CMDDESC_76},
	{WLAN_CMDNAME_77, WLAN_CMDDESC_77},
	{WLAN_CMDNAME_78, WLAN_CMDDESC_78},
	{WLAN_CMDNAME_79, WLAN_CMDDESC_79},

	{WLAN_CMDNAME_80, WLAN_CMDDESC_80},
	{WLAN_CMDNAME_81, WLAN_CMDDESC_81},
	{WLAN_CMDNAME_82, WLAN_CMDDESC_82},
	{WLAN_CMDNAME_83, WLAN_CMDDESC_83},
	{WLAN_CMDNAME_84, WLAN_CMDDESC_84},
	{WLAN_CMDNAME_85, WLAN_CMDDESC_85},
	{WLAN_CMDNAME_86, WLAN_CMDDESC_86},
	{WLAN_CMDNAME_87, WLAN_CMDDESC_87},
	{WLAN_CMDNAME_88, WLAN_CMDDESC_88},
	{WLAN_CMDNAME_89, WLAN_CMDDESC_89},

	{WLAN_CMDNAME_90, WLAN_CMDDESC_90},
	{WLAN_CMDNAME_91, WLAN_CMDDESC_91},
	{WLAN_CMDNAME_92, WLAN_CMDDESC_92},
	{WLAN_CMDNAME_93, WLAN_CMDDESC_93},
	{WLAN_CMDNAME_94, WLAN_CMDDESC_94},
	{WLAN_CMDNAME_95, WLAN_CMDDESC_95},
	{WLAN_CMDNAME_96, WLAN_CMDDESC_96},
	{WLAN_CMDNAME_97, WLAN_CMDDESC_97},
	{WLAN_CMDNAME_98, WLAN_CMDDESC_98},
	{WLAN_CMDNAME_99, WLAN_CMDDESC_99},

	{WLAN_CMDNAME_100, WLAN_CMDDESC_100},
	{WLAN_CMDNAME_101, WLAN_CMDDESC_101},
	{WLAN_CMDNAME_102, WLAN_CMDDESC_102},
	{WLAN_CMDNAME_103, WLAN_CMDDESC_103},
	{WLAN_CMDNAME_104, WLAN_CMDDESC_104},
	{WLAN_CMDNAME_105, WLAN_CMDDESC_105},
	{WLAN_CMDNAME_106, WLAN_CMDDESC_106},
	{WLAN_CMDNAME_107, WLAN_CMDDESC_107},
	{WLAN_CMDNAME_108, WLAN_CMDDESC_108},
	{WLAN_CMDNAME_109, WLAN_CMDDESC_109},

	{WLAN_CMDNAME_110, WLAN_CMDDESC_110},
	{WLAN_CMDNAME_111, WLAN_CMDDESC_111},
	{WLAN_CMDNAME_112, WLAN_CMDDESC_112},
	{WLAN_CMDNAME_113, WLAN_CMDDESC_113},
	{WLAN_CMDNAME_114, WLAN_CMDDESC_114},
	{WLAN_CMDNAME_115, WLAN_CMDDESC_115},
	{WLAN_CMDNAME_116, WLAN_CMDDESC_116},
	{WLAN_CMDNAME_117, WLAN_CMDDESC_117},
	{WLAN_CMDNAME_118, WLAN_CMDDESC_118},
	{WLAN_CMDNAME_119, WLAN_CMDDESC_119},

	{WLAN_CMDNAME_120, WLAN_CMDDESC_120},
	{WLAN_CMDNAME_121, WLAN_CMDDESC_121},
	{WLAN_CMDNAME_122, WLAN_CMDDESC_122},
	{WLAN_CMDNAME_123, WLAN_CMDDESC_123},
	{WLAN_CMDNAME_124, WLAN_CMDDESC_124},
	{WLAN_CMDNAME_125, WLAN_CMDDESC_125},
	{WLAN_CMDNAME_126, WLAN_CMDDESC_126},
	{WLAN_CMDNAME_127, WLAN_CMDDESC_127},
	{WLAN_CMDNAME_128, WLAN_CMDDESC_128},
	{WLAN_CMDNAME_129, WLAN_CMDDESC_129},

	{WLAN_CMDNAME_130, WLAN_CMDDESC_130},
	{WLAN_CMDNAME_131, WLAN_CMDDESC_131},
	{WLAN_CMDNAME_132, WLAN_CMDDESC_132},
	{WLAN_CMDNAME_133, WLAN_CMDDESC_133},
	{WLAN_CMDNAME_134, WLAN_CMDDESC_134},
	{WLAN_CMDNAME_135, WLAN_CMDDESC_135},
	{WLAN_CMDNAME_136, WLAN_CMDDESC_136},
	{WLAN_CMDNAME_137, WLAN_CMDDESC_137},
	{WLAN_CMDNAME_138, WLAN_CMDDESC_138},
	{WLAN_CMDNAME_139, WLAN_CMDDESC_139},

	{WLAN_CMDNAME_140, WLAN_CMDDESC_140},
	{WLAN_CMDNAME_141, WLAN_CMDDESC_141},
	{WLAN_CMDNAME_142, WLAN_CMDDESC_142},
	{WLAN_CMDNAME_143, WLAN_CMDDESC_143},
	{WLAN_CMDNAME_144, WLAN_CMDDESC_144},
	{WLAN_CMDNAME_145, WLAN_CMDDESC_145},
	{WLAN_CMDNAME_146, WLAN_CMDDESC_146},
	{WLAN_CMDNAME_147, WLAN_CMDDESC_147},
	{WLAN_CMDNAME_148, WLAN_CMDDESC_148},
	{WLAN_CMDNAME_149, WLAN_CMDDESC_149},
	

	{WLAN_CMDNAME_150, WLAN_CMDDESC_150},
	{WLAN_CMDNAME_151, WLAN_CMDDESC_151},
	{WLAN_CMDNAME_152, WLAN_CMDDESC_152},
	{WLAN_CMDNAME_153, WLAN_CMDDESC_153},
	{WLAN_CMDNAME_154, WLAN_CMDDESC_154},
	{WLAN_CMDNAME_155, WLAN_CMDDESC_155},
	{WLAN_CMDNAME_156, WLAN_CMDDESC_156},
	{WLAN_CMDNAME_157, WLAN_CMDDESC_157},
	{WLAN_CMDNAME_158, WLAN_CMDDESC_158},
	{WLAN_CMDNAME_159, WLAN_CMDDESC_159},
	

	{WLAN_CMDNAME_160, WLAN_CMDDESC_160},
	{WLAN_CMDNAME_161, WLAN_CMDDESC_161},
	{WLAN_CMDNAME_162, WLAN_CMDDESC_162},
	{WLAN_CMDNAME_163, WLAN_CMDDESC_163},
	{WLAN_CMDNAME_164, WLAN_CMDDESC_164},
	{WLAN_CMDNAME_165, WLAN_CMDDESC_165},
	{WLAN_CMDNAME_166, WLAN_CMDDESC_166},
	{WLAN_CMDNAME_167, WLAN_CMDDESC_167},
	{WLAN_CMDNAME_168, WLAN_CMDDESC_168},
	{WLAN_CMDNAME_169, WLAN_CMDDESC_169},
	

	{WLAN_CMDNAME_170, WLAN_CMDDESC_170},
	{WLAN_CMDNAME_171, WLAN_CMDDESC_171},
	{WLAN_CMDNAME_172, WLAN_CMDDESC_172},
	{WLAN_CMDNAME_173, WLAN_CMDDESC_173},
	{WLAN_CMDNAME_174, WLAN_CMDDESC_174},
	{WLAN_CMDNAME_175, WLAN_CMDDESC_175},
	{WLAN_CMDNAME_176, WLAN_CMDDESC_176},
	{WLAN_CMDNAME_177, WLAN_CMDDESC_177},
	{WLAN_CMDNAME_178, WLAN_CMDDESC_178},
	{WLAN_CMDNAME_179, WLAN_CMDDESC_179},
	

	{WLAN_CMDNAME_180, WLAN_CMDDESC_180},
	{WLAN_CMDNAME_181, WLAN_CMDDESC_181},
	{WLAN_CMDNAME_182, WLAN_CMDDESC_182},
	{WLAN_CMDNAME_183, WLAN_CMDDESC_183},
	{WLAN_CMDNAME_184, WLAN_CMDDESC_184},
	{WLAN_CMDNAME_185, WLAN_CMDDESC_185},
	{WLAN_CMDNAME_186, WLAN_CMDDESC_186},
	{WLAN_CMDNAME_187, WLAN_CMDDESC_187},
	{WLAN_CMDNAME_188, WLAN_CMDDESC_188},
	{WLAN_CMDNAME_189, WLAN_CMDDESC_189},
	

	{WLAN_CMDNAME_190, WLAN_CMDDESC_190},
	{WLAN_CMDNAME_191, WLAN_CMDDESC_191},
	{WLAN_CMDNAME_192, WLAN_CMDDESC_192},
	{WLAN_CMDNAME_193, WLAN_CMDDESC_193},
	{WLAN_CMDNAME_194, WLAN_CMDDESC_194},
	{WLAN_CMDNAME_195, WLAN_CMDDESC_195},
	{WLAN_CMDNAME_196, WLAN_CMDDESC_196},
	{WLAN_CMDNAME_197, WLAN_CMDDESC_197},
	{WLAN_CMDNAME_198, WLAN_CMDDESC_198},
	{WLAN_CMDNAME_199, WLAN_CMDDESC_199},
	
	{WLAN_CMDNAME_200, WLAN_CMDDESC_200},
	{WLAN_CMDNAME_201, WLAN_CMDDESC_201},
	{WLAN_CMDNAME_202, WLAN_CMDDESC_202},
	{WLAN_CMDNAME_203, WLAN_CMDDESC_203},
	{WLAN_CMDNAME_204, WLAN_CMDDESC_204},
	{WLAN_CMDNAME_205, WLAN_CMDDESC_205},
	{WLAN_CMDNAME_206, WLAN_CMDDESC_206},
	{WLAN_CMDNAME_207, WLAN_CMDDESC_207},
	{WLAN_CMDNAME_208, WLAN_CMDDESC_208},
	{WLAN_CMDNAME_209, WLAN_CMDDESC_209},
	
	{WLAN_CMDNAME_210, WLAN_CMDDESC_210},
	{WLAN_CMDNAME_211, WLAN_CMDDESC_211},
	{WLAN_CMDNAME_212, WLAN_CMDDESC_212},
	{WLAN_CMDNAME_213, WLAN_CMDDESC_213},
	{WLAN_CMDNAME_214, WLAN_CMDDESC_214},
	{WLAN_CMDNAME_215, WLAN_CMDDESC_215},
	{WLAN_CMDNAME_216, WLAN_CMDDESC_216},
	{WLAN_CMDNAME_217, WLAN_CMDDESC_217},
	{WLAN_CMDNAME_218, WLAN_CMDDESC_218},
	{WLAN_CMDNAME_219, WLAN_CMDDESC_219},
	
	{WLAN_CMDNAME_220, WLAN_CMDDESC_220},
	{WLAN_CMDNAME_221, WLAN_CMDDESC_221},
	{WLAN_CMDNAME_222, WLAN_CMDDESC_222},
	{WLAN_CMDNAME_223, WLAN_CMDDESC_223},
	{WLAN_CMDNAME_224, WLAN_CMDDESC_224},
	{WLAN_CMDNAME_225, WLAN_CMDDESC_225},
	{WLAN_CMDNAME_226, WLAN_CMDDESC_226},
	{WLAN_CMDNAME_227, WLAN_CMDDESC_227},
	{WLAN_CMDNAME_228, WLAN_CMDDESC_228},
	{WLAN_CMDNAME_229, WLAN_CMDDESC_229},
	
	{WLAN_CMDNAME_230, WLAN_CMDDESC_230},
	{WLAN_CMDNAME_231, WLAN_CMDDESC_231},
	{WLAN_CMDNAME_232, WLAN_CMDDESC_232},
	{WLAN_CMDNAME_233, WLAN_CMDDESC_233},
	{WLAN_CMDNAME_234, WLAN_CMDDESC_234},
	{WLAN_CMDNAME_235, WLAN_CMDDESC_235},
	{WLAN_CMDNAME_236, WLAN_CMDDESC_236},
	{WLAN_CMDNAME_237, WLAN_CMDDESC_237},
	{WLAN_CMDNAME_238, WLAN_CMDDESC_238},
	{WLAN_CMDNAME_239, WLAN_CMDDESC_239},
	
	{WLAN_CMDNAME_240, WLAN_CMDDESC_240},
	{WLAN_CMDNAME_241, WLAN_CMDDESC_241},
	{WLAN_CMDNAME_242, WLAN_CMDDESC_242},
	{WLAN_CMDNAME_243, WLAN_CMDDESC_243},
	{WLAN_CMDNAME_244, WLAN_CMDDESC_244},
	{WLAN_CMDNAME_245, WLAN_CMDDESC_245},
	{WLAN_CMDNAME_246, WLAN_CMDDESC_246},
	{WLAN_CMDNAME_247, WLAN_CMDDESC_247},
	{WLAN_CMDNAME_248, WLAN_CMDDESC_248},
	{WLAN_CMDNAME_249, WLAN_CMDDESC_249},
	
	{WLAN_CMDNAME_250, WLAN_CMDDESC_250},
	{WLAN_CMDNAME_251, WLAN_CMDDESC_251},
	{WLAN_CMDNAME_252, WLAN_CMDDESC_252},
	{WLAN_CMDNAME_253, WLAN_CMDDESC_253},
	{WLAN_CMDNAME_254, WLAN_CMDDESC_254}, 

	// last cmd
	{CMDNAME_LAST, CMDDESC_BLANK}
};
#endif  //#ifndef __WLAN_MENU_H__
