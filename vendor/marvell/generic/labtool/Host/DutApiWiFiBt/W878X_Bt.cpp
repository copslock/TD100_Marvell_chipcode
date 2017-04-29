/** @file W878X_Bt.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#define _HIDE_MENU_
#define _DEVELOPMENT_PHASE_

#include "../DutCommon/Clss_os.h"
#include "../DutCommon/Dut_error.hc"
#include "../DutCommon/utilities.h"

 
#include "DutApi878XDll.h"
#include "DutBtApi878XDll.h"
#include "W878XApp.h"

#include "DutAppVerNo.h"
#include "DutFuncs.h"
#ifdef _HIDE_MENU_
#include "BtMenu.h"
#endif //#ifdef _HIDE_MENU_

extern int UpdateSaveBtCalData(int DeviceType=0, 
					char *BtCalFlexFileName=NULL);

extern int LoadSaveCalData(
						   int DeviceType=0,
 					char *FlexFileName=NULL,
					char *WlanCalFlexFileName=NULL
#if defined (_MIMO_)
					,
					char *WlanCalFlexPwrTrgtFileFormat=NULL					
#endif // #if defined (_MIMO_)
					);
extern int UpLoadSaveCalData(int *CsC,
							 int DeviceType=0,
					char *FlexFileName=NULL, 
 					char *WlanCalFlexFileName=NULL
#if defined (_MIMO_)
					,
					char *WlanCalFlexPwrTrgtFileFormat=NULL	
#endif // #if defined (_MIMO_)
					);
 
#if defined(_W8780_) || defined (_W8688_) || defined (_W8689_) || defined (_W8787_) || defined(_W8790_) || defined(_W8782_)
extern int ChangeUartRate(int NewBaudRate);
extern int ChangeBdAddress(BYTE *pNewBdAddress);
extern int GetBdAddressFromE2p(BYTE *pBdAddress);
#endif //#endif //#if defined(_W8780_) || defined (_W8688_) || defined (_W8689_) || defined (_W8787_) || defined(_W8790_)
 
 
int BTMenu(int OptionMode, void** theObj=NULL)
{
 	int				status=0;
 	unsigned long	cmd=0, parm1=0, parm2=0, cnt=0;
	char line[_MAX_PATH]="";

 	DebugLogRite("%s\n", Dut_Bt_About());
  	status = Dut_Bt_OpenDevice(theObj);
	DebugLogRite(" Dut_Bt_OpenDevice: 0x%08X\n", status);
	if (status)
	{
		status = Dut_Bt_CloseDevice(theObj);
		if(status) return (status);
		return (0);
	}

#ifdef _HIDE_MENU_
//#ifndef _IF_UDP_
		DisplayMenu(BtCmdNames, 0);
//#endif //_IF_UDP_
#endif //#ifdef _HIDE_MENU_

	do
    {	

#ifndef _HIDE_MENU_
		DebugLogRite ("\n");
		DebugLogRite ("-----------------------------------------------------------------------\n"); 
		DebugLogRite ("\t\t87xx (BT) TEST MENU\n"); 
		DebugLogRite ("-----------------------------------------------------------------------\n"); 
  
#if defined(_W8780_)
		DebugLogRite ("  9. Get TRSW Antenna\n");
		DebugLogRite (" 10. Set TRSW Antenna (0 for MAIN; 1 for AUX)\n");
#endif // #if defined(_W8780_)

		DebugLogRite (" 11. Get BT Channel	(BT2=0)\n");
		DebugLogRite (" 12. Set BT Channel"
			"\t(Channel Number (valid number 0-78)) (BT2=0)\n");

		DebugLogRite (" 13. Set BT Channel by given Frequency"
			"\t(Channel Freq In MHz) (BT2=0)\n");
		DebugLogRite (" 15.Get Power Level Value\n");
		DebugLogRite (" 16.Set Power Level Value (double)(in 0.5db steps)\n");
		DebugLogRite (" 20.Get Power Level Max Min Value\n");
		DebugLogRite (" 21.Step Power Level (double)(step) in 0.5db steps\n");
		DebugLogRite (" 23.Get Power Gain\n");
		DebugLogRite (" 24.Set Power Gain (0x)(gain)\n");

#if !defined (_DEVELOPMENT_PHASE_)
// not supported by this chip  
//		DebugLogRite (" 17. Tx Continous (Enable=0) (DataRate(0x)=11)\n"
//			"\t(PayloadPattern=2) (PayloadOnly=1)\n"
//			"\tEnable: 0: off; 1: on\n"
//			"\tDataRate: 0x10: 1M, 0x20:2M, 0x30: 3M\n"
//			"\tPayloadPattern: 0: all 0, 1: all 1, 2: PN9, 3: 0xAA, 4: 0xF0\n"
//			"\tPayloadOnly: tx Payload only, no header (true, false)\n");

		DebugLogRite (" 18. Tx Cw (Enable=0) \n""\t\tEnable: 0: off; 1: on\n");
//		DebugLogRite (" 19. Tx Carrier Suppression Test signal (Enable=0) \n""\t\tEnable: 0: off; 1: on\n");
//  		DebugLogRite (" 22. Set Caled power (pwr(double) mode(0=NormalMode 1=PerformanceMode) useFeLoss(=0))\n");

		DebugLogRite (" 25. Dutycycle Tx (Enable=0) (PacketType(0x)=11)\n"
			"\t(PayloadPattern=2) (DutyWeight=50) (PayloadOnly=0)\n"
			"\tEnable: 0: off; 1: on\n"
			"\t\tPacketType(Rate.Slot):\n"
			"\t\t\t  DM1 = 0x01;   DM3 = 0x03;   DM5 = 0x05; (GFSK, 1M FEC)\n"
			"\t\t\t  DH1 = 0x11;   DH3 = 0x13;   DH5 = 0x15; (GFSK, 1M)\n"
			"\t\t\t2-DH1 = 0x21; 2-DH3 = 0x23; 2-DH5 = 0x25; (DQPSK, 2M)\n"
			"\t\t\t3-DH1 = 0x31; 3-DH3 = 0x33; 3-DH5 = 0x35; (8PSK, 3M)\n"
			"\tPayloadPattern: 0: all 0, 1: all 1, 2: PN9, 3: 0xAA, 4: 0xF0\n"
			"\tDutyWeight: On-Time percentage\n"
			"\tPayloadOnly: tx Payload only, no header (true, false)\n");
#endif // #if !defined (_DEVELOPMENT_PHASE_)

		
		DebugLogRite ("225. Dutycycle Tx (Enable=0) (PacketType(0x)=11)\n"
			"\t(PayloadPattern=2) (PayloadLenInByte=-1) (HopMode=0) (Interval=1)(Whitening=0)\n"
			"\tEnable: 0: off; 1: on\n"
			"\tPacketType(Rate.Slot):\n"
			"\t\tTesting Pattern and ACL:\n"
			"\t\t\t  DM1 = 0x01;   DM3 = 0x03;   DM5 = 0x05; (GFSK, 1M FEC)\n"
			"\t\t\t  DH1 = 0x11;   DH3 = 0x13;   DH5 = 0x15; (GFSK, 1M)\n"
			"\t\t\t2-DH1 = 0x21; 2-DH3 = 0x23; 2-DH5 = 0x25; (DQPSK, 2M)\n"
			"\t\t\t3-DH1 = 0x31; 3-DH3 = 0x33; 3-DH5 = 0x35; (8PSK, 3M)\n"

			"\t\tSco:\n"
			"\t\t\t  HV1 = 0x11;   HV2 = 0x12;   HV3 = 0x13; (GFSK, 1M)\n"
			"\t\teSco:\n"
			"\t\t\t  EV3 = 0x13;   EV4 = 0x14;   EV5 = 0x15; (GFSK, 1M)\n"
			"\t\t\t2-EV3 = 0x23; 2-EV5 = 0x25;               (DQPSK, 2M)\n"
			"\t\t\t3-EV3 = 0x33; 3-EV5 = 0x35;               (8PSK, 3M)\n"

			"\tPayloadPattern: 0: all 0, 1: all 1, 2: PN9, 3: 0xAA, 4: 0xF0\n"
			"\t\t\t5: PRBS ACL, 6: PRBS SCO, 7: PRBS ESCO\n"
			"\tPayloadLenInByte: PacketType dependent, -1 for max possible.\n"
			"\tHopMode: HopMode (on=1, random hopping, off=0 fixed channel)\n"
			"\tWhitening: Whitening the whole packet. (true=1, false=0)\n");

		if (1)//;(SUPERUSER == OptionMode)
		{
#if defined (_HCI_PROTOCAL_)
#if !defined (_DEVELOPMENT_PHASE_)
		DebugLogRite (" 30. Marvell RSSI Report (linkId) (clear (0=read, 1=clear)\n"
			"\t(This is for after linked with a partner)\n");
#endif // #if !defined (_DEVELOPMENT_PHASE_)
#endif //#if defined (_HCI_PROTOCAL_)


/*
		DebugLogRite (" 31. Marvell Rx Result Report\n");
		DebugLogRite (" 32. Marvell Rx (RxChannel=0) (FrameCount=10) (PayloadLength=-1)\n"
			"\t(PacketType(0x)=11)\n"
			"\t(PayloadPattern=2)\n"
//			"\t(PRBS)\n"
			"\t(TxBdAddress:TxAmAddress xx-xx-xx-xx-xx-xx)\n"
//			"\t(frameSkip=0) (CorrelationWindow=100)\n"
//			"\t(CorrelationThreshold=(0x)1F)\n"
//			"\t(Whitening=0)\n"
			"\n"
			"\t(RxChannel=0: Rx Channel)\n"
			"\tFrameCount: number of frames to Rx\n"
			"\tPayloadLength: PayLoad Length in Byte. -1=max for the PacketType\n"
			"\tPacketType(Rate.Slot):\n"
			"\t\t\t  DM1 = 0x01;   DM3 = 0x03;   DM5 = 0x05; (GFSK, 1M FEC)\n"
			"\t\t\t  DH1 = 0x11;   DH3 = 0x13;   DH5 = 0x15; (GFSK, 1M)\n"
			"\t\t\t2-DH1 = 0x21; 2-DH3 = 0x23; 2-DH5 = 0x25; (DQPSK, 2M)\n"
			"\t\t\t3-DH1 = 0x31; 3-DH3 = 0x33; 3-DH5 = 0x35; (8PSK, 3M)\n"
			"\tPayloadPattern: 0: all 0, 1: all 1, 2: PN9, 3: 0xAA, 4: 0xF0\n"
//			"\tPRBS pattern: Transmitter's BD address\n"
			"\tTxBdAddress: Transmitter's BD address. Default:01-02-03-04-05-06.\n"
//			"\tTxBdAddress:TxAmAddress: Transmitter's BD address and Am Address\n"
//			"\tframeSkip: the tick to skip for multislot packets. 0 means minimum.\n"
//			"\tCorrelationWindow the window in byte to perform correlation\n"
//			"\tCorrelationThreshold: the threshold of correlated signal\n"
	//		"\tWhitening: bool, if the signal is whitenned\n"
			);

		}
*/

#if defined (_HCI_PROTOCAL_)
#if !defined (_DEVELOPMENT_PHASE_)
		DebugLogRite ("333. ACL link test (require a local controllable partner (Golden Unit))\n"
			"\t(FrameCount=100) \n"
			"\t(PacketType(0x)=11) (PayloadLength=-1)\n"
			"\t(PayloadPattern=2)\n"
			"\t(stress=0)\n"
			"\t(DutAsMaster=1)\n"
			"\t(MasterAsTx=1)\n"
			"\n"
			"\tFrameCount: number of frames to Tx\n"
			"\tPayloadLength: PayLoad Length in Byte\n"
			"\tPacketType(Rate.Slot):\n"
			"\t\t\t  DM1 = 0x01;   DM3 = 0x03;   DM5 = 0x05; (GFSK, 1M FEC)\n"
			"\t\t\t  DH1 = 0x11;   DH3 = 0x13;   DH5 = 0x15; (GFSK, 1M)\n"
			"\t\t\t2-DH1 = 0x21; 2-DH3 = 0x23; 2-DH5 = 0x25; (DQPSK, 2M)\n"
			"\t\t\t3-DH1 = 0x31; 3-DH3 = 0x33; 3-DH5 = 0x35; (8PSK, 3M)\n"
			"\tPayloadPattern: 0: all 0, 1: all 1, 2: PN9, 3: 0xAA, 4: 0xF0\n"
			"\tstress: bool, stress test\n"
			"\tDutAsMaster: bool, Dut As Master\n"
//			"\MasterAsTx: bool, Master As Tx\n"
			);


			
			
/*			
		if (0)//;(SUPERUSER == OptionMode)
		{
			
			DebugLogRite ("933. MultiCast Tx (FrameCount=100) (PayloadLength=1024) "
				"(PacketType(0x)=11) "
				"(PayloadPattern=2) (PayloadOnly=0)\n"
				"\tFrameCount: number of frames to Tx\n"
				"\tPayloadLength: PayLoad Length in Byte\n"
				"\tPacketType(Rate.Slot):\n"
				"\t\t\t  DM1 = 0x01;   DM3 = 0x03;   DM5 = 0x05; (GFSK, 1M FEC)\n"
				"\t\t\t  DH1 = 0x11;   DH3 = 0x13;   DH5 = 0x15; (GFSK, 1M)\n"
				"\t\t\t2-DH1 = 0x21; 2-DH3 = 0x23; 2-DH5 = 0x25; (DQPSK, 2M)\n"
				"\t\t\t3-DH1 = 0x31; 3-DH3 = 0x33; 3-DH5 = 0x35; (8PSK, 3M)\n"
				"\tPayloadPattern: 0: all 0, 1: all 1, 2: PN9, 3: 0xAA, 4: 0xF0\n"
				"\tPayloadOnly: tx Payload only, no header (true, false)\n");
		}
*/
#endif // #if !defined (_DEVELOPMENT_PHASE_)
#endif //#if defined (_HCI_PROTOCAL_)

			if(!Dut_Shared_SupportNoE2PROM())
			{
#ifndef _FLASH_
				DebugLogRite (" 39. SPI download from binary file.\n");
#endif // #ifndef _FLASH_
				DebugLogRite (" 40. SPI Verify from binary file\n");
				DebugLogRite (" 41. Dump E2PROM content\n");
				DebugLogRite (" 42. Get E2PROM type (if) (addrLen) (device)\n"
					"\tIf:\tinterface type: 0=detect-again, 1=SPI, 2=I2C\n"
					"\taddrLen:\taddress width in byte. valid 1 - 3.\n"
					"\tdevice:\tdevice type: 1=eeprom, 2=flash\n"
					);
				DebugLogRite (" 43. Force E2PROM type (if) (addrLen) (device)\n"
					"\tIf:\tinterface type: 0=detect-again, 1=SPI, 2=I2C\n"
					"\taddrLen:\taddress width in byte. valid 1 - 3.\n"
					"\tdevice:\tdevice type: 1=eeprom, 2=flash\n"
					);
			}
		// DebugLogRite (" 45. Read BD Address From EEPROM/Flash\n");
		//;DebugLogRite (" 46. Change BD_Address in EEPROM (xx.xx.xx.xx.xx.xx)\n");
		//;DebugLogRite (" 46. Change Uart Baud Rate in EEPROM (rate=115200)\n");

#if defined(_IF_USB_) 
		if (!Dut_Shared_SupportNoE2PROM())
		{
 			DebugLogRite (" 47. Get USB Attribute \n");
 			DebugLogRite (" 48. Set USB Attribute ((0x)(Attribute(8-bit)) (MaxPower=0(0 - 250)))\n");
		}
#endif // #if defined(_IF_USB_) 

#ifdef _FLASH_
		DebugLogRite (" 51. Erase flash		(0x )(section)\n");
#endif //#ifdef _FLASH_

#if !defined (_DEVELOPMENT_PHASE_)
		DebugLogRite (" 53. Set Cal (from file:CalBtDataFile.txt)\n");
		DebugLogRite (" 54. Get Cal\n");
#endif // #if !defined (_DEVELOPMENT_PHASE_)

		if (SUPERUSER == OptionMode)
		{
			DebugLogRite (" 62. Read BTU Reg		(0x)(addr) (count =1)\n");
			DebugLogRite (" 63. Write BTU Reg		(0x 0x)(addr data)\n");
			DebugLogRite (" 64. Read BRF Reg		(0x)(addr) (count =1)\n");
			DebugLogRite (" 65. Write BRF Reg		(0x 0x)(addr data)\n");
 		   
			DebugLogRite (" 68. Peek SOC Mem		(0x)(addr) (count =1)\n");
			DebugLogRite (" 69. Poke SOC Mem		(0x 0x)(addr data)\n");
			if(!Dut_Shared_SupportNoE2PROM())
			{
				DebugLogRite (" 70. Peek Spi		(0x)(addr) (count =1)\n");
				DebugLogRite (" 71. Poke Spi		(0x 0x)(addr data)\n");
#if defined (_W8689_) || defined (_W8787_) || defined (_W8790_)  || defined(_W8782_)
			DebugLogRite (" 72. Read Efuse Data		(0x)(block) (WordCount =16)\n");

			DebugLogRite (" 73. Write Efuse Data	(0x 0x)(block WordCount=1)\n"
				"\t\tRequires 2.5V on VDD25_EFuse pin\n");
#endif //#if defined (_W8689_) || defined (_W8787_) || defined (_W8790_) 

			}
		}
		
		//;DebugLogRite (" 75. LoadCustomizedSettings\n");
		DebugLogRite (" 78. Enable Device Under Test Mode\n");
		DebugLogRite (" 79. Periodic Inquiry Request. (enable(0) (0x)MaxPer(8) (0x)MinPer(7) (0x)InqLn(4))\n"
				"\tMaxPer: Max Period Length. 0x0003-0xFFFF=3.84 - 83884.8 Sec\n"
				"\tMinPer: Min Period Length. 0x0002-0xFFFF=2.56 - 83884.8 Sec\n"
				"\tInqLn:  Inquiry Length.    0x01-0x30*1.28 sec = 1-61.44 sec\n");
 
//		DebugLogRite ("175. Init BRF settings (temperary item)\n");
		DebugLogRite (" 88. FW/HW Version\n");
		DebugLogRite (" 95. Get Rf XTAL control\n");
		DebugLogRite (" 96. Set Rf XTAL control (setting(8 bits))\n");

#if !defined (_DEVELOPMENT_PHASE_)
		DebugLogRite ("111.Get state of FH 2Channel mode\n");
		DebugLogRite ("112.Set state of FH 2Channel mode (Enable=0)\n"			
			"\t\tEnable: 0: off; 1: on\n");
#endif // #if !defined (_DEVELOPMENT_PHASE_)

//		DebugLogRite ("113.Get Power Control Class\n"
//			"\t\tmode: 0:MRVL, 1:Class2, 2:Class1 3: Class2+XPA \n"
//			"\t\t\t(0x)1x: OpenLoop, (0x)2x: FixVga\n");
		DebugLogRite ("113.Get Power Control Class\n"
			"\t\tmode: 0:MRVL_Class2, 1:MRVL_Class1.5\n");

		DebugLogRite ("114.Set Power Control Class (mode=0)\n");

 
///////////////////////////
		DebugLogRite ("115. Get Disable Btu Pwr Ctl\n");
		DebugLogRite ("116. Set Disable Btu Pwr Ctl (mode=0 (0=enable, 1=disable))\n");
#if !defined (_DEVELOPMENT_PHASE_)
		DebugLogRite ("117. Set Btu Temp Ctl (mode=0 (0=enable, 1=disable))\n");
#endif // #if !defined (_DEVELOPMENT_PHASE_)


#ifdef _HCI_PROTOCAL_
		DebugLogRite ("***********************************************************************\n");
		DebugLogRite ("***********************************************************************\n");
		DebugLogRite (	"Comb Commands will perform neccesary steps \n"
						"to ensure this command is meaningful, "
						"like create a ACL link\n");
		DebugLogRite ("***********************************************************************\n");
		DebugLogRite ("231.Comb Command Hold	(mode=0(0=off, 1=on)\n"
						"\t\t(0x)(MaxInterval=800) (0x)(MinInterval=400)\n" 
						"\t\t(PartnerLocalControlable=1)) \n");
		DebugLogRite ("232.Comb Command Sniff	(mode=0(0=off, 1=on)\n "
						"\t\t(0x)(MaxInterval=800) (0x)(MinInterval=400)\n" 
						"\t\t(Attemp=1) (TimeOut=1)\n" 
						"\t\t(PartnerLocalControlable=1)) \n");
		DebugLogRite ("233.Comb Command Park	(mode=0(0=off, 1=on)\n"
						"\t\t(0x)(MaxInterval=800) (0x)(MinInterval=400)\n" 
						"\t\t(PartnerLocalControlable=1)) \n");
		DebugLogRite ("***********************************************************************\n");
		DebugLogRite ("***********************************************************************\n");
		
		
#endif //#ifdef _HCI_PROTOCAL_
#ifdef _HCI_PROTOCAL_
		DebugLogRite ("245.Get BD address from FW\n");
#endif //#ifdef _HCI_PROTOCAL_

 		DebugLogRite ("80.Reset BT HW\n");

/*
#ifdef _HCI_PROTOCAL_
		DebugLogRite ("296.Test NoCalMode\n\t(SwPwrCtrlMode(2) (0x)(RefGain1=0278) (0x)(RefGain2=0178)\n"
						"\t(0x)(RefTemp=65) (0x)(TempGain=5))\n");
		DebugLogRite ("From HW test data up to now:\n"
				"\tSwPwrCtrlMode: SW implementation of TxPwrCtrl. (0=disable, 2=NoCal)\n"
				"\tRefGain1: BRF [0x36] setting for  3dBm (default=0x0278)\n"
				"\tRefGain2: BRF [0x36] setting for -6dBm (default=0x0178)\n"
				"\tRefTemp:  Value Read from temp senser 0x800020E4 at cal (default=0x65)\n"
				"\tTempGain: Temp Gain Compensation, in ADC value per gain step \n\t\t(ie: 1/TempGain) (default=0x5)\n");
#endif //#ifdef _HCI_PROTOCAL_
*/

#if !defined (_DEVELOPMENT_PHASE_)
#if defined (_W8688_) || defined (_W8787_) || defined(_W8782_)
//		DebugLogRite ("329.Get BCA or BT_Coexist mode	(mode)\n");
//		DebugLogRite ("330.Set BCA or BT_Coexist mode	((0x)mode (mode=F)) \n");
#endif // #if defined (_W8688_) || defined (_W8787_)
#endif // #if !defined (_DEVELOPMENT_PHASE_)


#ifdef _HCI_PROTOCAL_
/*
		DebugLogRite ("BT Current Optimizations:\n");
		DebugLogRite ("340.Marvell RF Dynamic Clock Control\n"
			"\t((0x)(DynClkTimer=190) (MultiSlotProtect=1) (AutoDetect=1) (DynClkValue=1)\n"
			"\t(DynClkEnable=1) (0x)(DynClkStartTime=1000) (0x)(DynClkFinishTime=137E)\n"
			"\t(DynClkTimedEnable=1))\n");
		DebugLogRite ( 
				"\tDynClkTimer: BRF timer for a guard period after rifRx and rifTx.(12 bits)\n"
				"\tMultiSlotProtect: This enables a override of the finish clock timer.(0:Disable, 1:Enable: default=1)\n"
				"\tAutoDetect: This enables the automatic detection of a completed frame.(0:Disable, 1:Enable: default=1)\n"
				"\tDynClkValue: Setting for the manual override of the dynamic clock control.(0:Disable, 1:Enable: default=1)\n"
				"\tDynClkEnable: This is an automatic control of the BRF clock.(0:Disable, 1:Enable: default=1)\n"
				"\tDynClkStartTime: This is the start time based on the frame timer as a reference. Range: 0-4999. Default=1000\n"
				"\tDynClkFinishTime: This is the finish time based on the frame timer as a reference. Range: 0-4999. Default=137E\n"
				"\tDynClkTimedEnable: Enable for the timed finish. The finish time is ignored if this bit is set.(0:Disable, 1:Enable: default=1)\n"
 				);

		DebugLogRite ("341.Set BT Sleep Mode\n"
							"\t(Power Mode=3))\n");
		DebugLogRite ( 
				"\tPower Mode:\n"
				"\t\t0:Wlan Power Down; \n"
				"\t\t1:Sleep Mode\n"
				"\t\t2:Deep Sleep Mode\n"
				"\t\t3:Full Power Mode\n"
 				);

		DebugLogRite ("342.Wake up Method\n"
							"\t(Host Wake up Method=0) (Host Wake up GPIO=5)\n"
							"\t(Device Wake up Method=0) (Device Wake up GPIO=4))\n");
		DebugLogRite ( 
				"\tHost Wake up Method: 0:No Need; 1:by DTR; 2:by Break; 3:by GPIO\n"
				"\tHost Wake up GPIO: only when host wakeup by GPIO, 0 - 7 for GPIO0 - GPIO7, -1 for invalid\n"
				"\tDevice Wake up Method: 0:by DSR; 1:by Break; 2:by GPIO\n"
				"\tDevice Wake up GPIO: only when device wakeup by GPIO, 0 - 7 for GPIO0 - GPIO7, -1 for invalid\n"
 				);

		DebugLogRite ("343.Marvell HIU Module Configuration\n"
							"\t(Module ShutDown/BringUp=0) (Host Interface Activity=0)\n"
							"\t(Host Interface Type=0))\n");
		DebugLogRite ( 
				"\tModule ShutDown/BringUp:\n\t\t0:Bring up BT Module (HIF Keeps Up);\n"
				"\t\t1:Shut Off BT Module (HIF Keeps Up);\n"
				"\t\t2:Host Interface Control;\n"
				"\t\t3:Power Up WLAN Module (HIF Also Up);\n"
				"\t\t4:Power Down WLAN Module (HIF Also Down)\n"
				"\t\t5:Power Up WLAN Module (HIF Keeps Up)\n"
				"\t\t6:Power Down WLAN Module (HIF Keeps Up)\n"

				"\tHost Interface Activity: 0:Deactivate; 1:Activate\n"
				"\tHost Interface Type:\n"
				"\t\t0 :All Interfaces; 1:Interfaces used by WLAN;\n"
				"\t\t2:Interfaces used by BT;\n"
	//			"\t\t3:Interfaces used by FM (Not supported yet);\n"
	//			"\t\t4:Interfaces used by GPS (Not supported yet)\n;
 				);

		DebugLogRite ("344.Set Marvell Radio Performance Mode\n"
							"\t(Marvell Rx Power Mode=1)\n"
							"\t(0x)(Performance Mode BDR RSSI Threshold=B3)\n"
							"\t(0x)(Performance Mode EDR RSSI Threshold=B7)\n"
							"\t(0x)(Normal Mode BDR RSSI Threshold=B8)\n"
							"\t(0x)(Normal Mode EDR RSSI Threshold=BC)\n"
							"\t(0x)(Performance Mode BDR CRC Threshold=10)\n"
							"\t(0x)(Performance Mode EDR CRC Threshold=20)\n"
							"\t(0x)(Normal Mode BDR CRC Threshold=20)\n"
							"\t(0x)(Normal Mode EDR CRC Threshold=30)\n"
							"\t(Tx mode in Test Mode=1))\n"
							"\t(Rx in Test Mode=1))\n"
							"\t(Rx in Normal Operation Mode=1))\n"
							"\t(0x)(RSSI/CRC Sample Interval=F0))\n"
							"\t(Low Power Scan Mode=0)\n"
							);
		DebugLogRite ( 
				"\tMarvell Rx Power Mode:\n"
				"\t\t0:Disable, (Manual Rx Mode);\n"
				"\t\t1:AUTO RX, RSSI Based;\n"
				"\t\t2:AUTO RX, CRC Based;\n"
				"\t\t3:AUTO RX, RSSI & CRC Based;\n"
 
				"\tPerformance Mode BDR RSSI Threshold: in dBm [For AUTO RX mode]\n"
				"\tPerformance Mode EDR RSSI Threshold: in dBm [For AUTO RX mode]\n"
				"\tNormal Mode BDR RSSI Threshold: in dBm [For AUTO RX mode]\n"
				"\tNormal Mode EDR RSSI Threshold: in dBm [For AUTO RX mode]\n"
				"\tPerformance Mode BDR CRC Threshold: [For AUTO RX mode]\n"
				"\tPerformance Mode EDR CRC Threshold: [For AUTO RX mode]\n"
				"\tNormal Mode BDR CRC Threshold: [For AUTO RX mode]\n"
				"\tNormal Mode EDR CRC Threshold: [For AUTO RX mode]\n"
				"\tTx mode in Test Mode: [For Manual RX mode]\n"
				"\t\t0:Normal Mode; 1:Performance Mode;\n"
 				"\tRx in Test Mode: [For Manual RX mode]\n"
				"\t\t0:Normal Mode; 1:Performance Mode;\n"
 				"\tRx in Normal Operation Mode: [For Manual RX mode]\n"
				"\t\t0:Normal Mode; 1:Performance Mode;\n"
				"\tRSSI/CRC Sample Interval: in 1.25ms frames [For AUTO RX mode]\n"
				"\tLow Power Scan Mode: enable low power scan mode.(0:Disable, 1:Enable: default=0)\n"
				);

		DebugLogRite ("345.Marvell ARM Dynamic Clock Control\n"
							"\t(Set NCO Mode=1)\n"
							"\t(Dynamic Clock Change=0)\n" 
							);
		DebugLogRite ( 
				"\tSet NCO Mode: (0:Disable, 1:Enable: default=1)\n"
				"\tDynamic Clock Change: Triggered by UART baudrate or Packet type.(0:Disable, 1:Enable: default=0)\n"
				);


// "Bt-Coex Mode"
		if (SUPERUSER == OptionMode)
		{
			DebugLogRite ("350.BlueTooth C-Comp Mode Config\n"
							"\t(BT Coexistence Mode=1)\n"
							"\t(Priority Mode_WLAN=0)\n" 
							"\t(Priority Mode_PTA=0)\n" 
							"\t(0x)(Priority Delay=0)\n" 
							"\t(0x)(High Priority=0FFF)\n" 
							);
		DebugLogRite ( 
				"\tBT Coexistence Mode: (0:Disable, 1:Enable: default=1)\n"
				"\tPriority Mode_WLAN:  (0: Ignore WLAN, 1: No Req when WLAN Active: default=0)\n"
				"\tPriority Mode_PTA:  (0: NOT update Default Settings in PTA, 1: Update Default Settings in PTA: default=0)\n"
				"\tPriority Delay: Unit: 250ns(Default=0)\n"
				"\tHigh Priority:  (Bit Map (12Bits):Bit11-ACL,BCST,SCO,eSCO,eSCOInstant,ParkS,ParkM,InqScan,Inq,PageScan,Page,Bit0-Switch)\n"
				);

		DebugLogRite ("351.BlueTooth Marvell Mode Config\n"
							"\t(BT Coexistence Mode=1)\n"
							"\t(Priority Mode_WLAN=0)\n" 
							"\t(Priority Mode_PTA=0)\n" 
							"\t(0x)(Priority Delay=0)\n" 
							"\t(0x)(High Priority=0FFF)\n" 
							"\t(0x)(Medium Priority=0FFF)\n" 
							"\t(0x)(BT End Timer=0)\n" 
							"\t(0x)(BT_FREQ low band=0)\n" 
							"\t(0x)(BT_FREQ high band=0)\n" 
							);
		DebugLogRite ( 
				"\tBT Coexistence Mode: (0:Disable, 1:Enable: default=1)\n"
				"\tPriority Mode_WLAN:  (0: Ignore WLAN, 1: No Req when WLAN Active: default=0)\n"
				"\tPriority Mode_PTA:  (0: NOT update Default Settings in PTA, 1: Update Default Settings in PTA: default=0)\n"
				"\tPriority Delay: Unit: 250ns(Default=0)\n"
				"\tHigh Priority:  (Bit Map (12Bits):Bit11-ACL,BCST,SCO,eSCO,eSCOInstant,ParkS,ParkM,InqScan,Inq,PageScan,Page,Bit0-Switch)\n"
				"\tMedium Priority:  (Bit Map (12Bits):Bit11-ACL,BCST,SCO,eSCO,eSCOInstant,ParkS,ParkM,InqScan,Inq,PageScan,Page,Bit0-Switch)\n"
				"\tBT End Timer:  Unit: 250ns(range: 16 bits; 0:Disable: default=0)\n"
				"\tBT_FREQ low band: Unit: 8 bits(Default=0)\n"
				"\tBT_FREQ high band: Unit: 8 bits(Default=0)\n"
				);

		DebugLogRite ("352.BlueTooth Marvell Mode 2 Config\n"
							"\t(BT Coexistence Mode=1)\n"
							"\t(Non Periodic Mode: Allow Tx Collisions=0)\n" 
							"\t(0x)(Non Periodic Mode: BT Timer Tlead=D)\n" 
							"\t(0x)(Non Periodic Mode: BT Timer Ttail=23)\n" 
							"\t(0x)(Periodic Mode: BT Sync Period=12)\n" 
							"\t(0x)(Periodic Mode: BT Sync Pulse Width=271)\n" 
							"\t(0x)(Periodic Mode: BT Tsync Lead=5)\n" 
		//					"\t(Periodic Mode: BT Sync Mode=1)\n"  
							);
		DebugLogRite ( 
				"\tBT Coexistence Mode: (0:Disable, 1:Enable: default=1)\n"
				"\tNon Periodic Mode: Allow Tx Collisions:  (0:Disable; 1:Enable: default=0)\n"
				"\tNon Periodic Mode: BT Timer Tlead:  Unit: 1us , (8 bit,Default=D)\n"
				"\tNon Periodic Mode: BT Timer Ttail:  Unit: 1us (8 bit,Default=23)\n"
				"\tPeriodic Mode: BT Sync Period: Unit: 625us (16 bit,Default=12)\n"
				"\tPeriodic Mode: BT Sync Pulse Width: Unit: 1us (16 bit, Default=271)\n"
				"\tPeriodic Mode: BT Tsync Lead: Unit: 250ns (8 bit,Default=5)\n"
//				"\tPeriodic Mode: BT Sync Mode (1:Hardware Controlled; Default=1)\n"
				);
		}
*/
#endif //#ifdef _HCI_PROTOCAL_
 
#ifdef _W8688_
		DebugLogRite ("100.Pcm Loop Back Mode(enable [useGpio12AsDin=0]\n");
		DebugLogRite ("14.FE BTonly mode\n");
#endif //#ifdef _W8688_

		DebugLogRite (" 99. Exit\n"); 
#endif //#ifndef _HIDE_MENU_

        DebugLogRite ("Enter option: ");

////////////////////////////////////////////////////////////////


		fgets(line, sizeof(line), stdin);
		
		cmd=-1; 
        parm1=0;
		parm2=0;
		cnt = sscanf (line, "%d %x %x",&cmd, &parm1, &parm2);
		DebugLogRite("%s\n", line);
#ifdef _HIDE_MENU_
	if( strstr(line, "-h") || strstr(line, "?") )
	{
		DisplayMenu(BtCmdNames, cmd);
	}
	else
#endif //#ifdef _HIDE_MENU_
	{
        switch (cmd)
        {

#if defined(_W8780_)
		case 9: // set txrx ant
  		    status = Dut_Bt_GetTrSwAntenna((int*)&parm1);
 			DebugLogRite(" Dut_Bt_GetTrSwAntenna: 0x%08X\n", status); 
   			DebugLogRite(" Ant: %d\n", parm1);
			break;
		case 10: // set txrx ant
  		    status = Dut_Bt_SetTrSwAntenna(parm1);
 			DebugLogRite(" Dut_Bt_SetTrSwAntenna: 0x%08X\n", status);
            break;
#endif // #if defined(_W8780_)

 

		case 11: // read BTU
 			{	
				int channelNum =0, FreqInMHz=0; 
    	
				DebugLogRite(" Dut_Bt_GetBtChannel: 0x%08X\n", 
					Dut_Bt_GetBtChannel(&channelNum, &FreqInMHz, (parm1)?true:false));
				DebugLogRite("Channel %d (%d MHz)\n", channelNum, FreqInMHz);
			}
			break;
		case 12: // read BTU
 			{	
				DWORD channelNum =0, FreqInMHz=0;
				DWORD BT2=0;
				sscanf (line, "%d %d %d",&cmd, &channelNum, &BT2);
    	
				DebugLogRite(" Dut_Bt_SetBtChannel: 0x%08X\n", 
					Dut_Bt_SetBtChannel(channelNum, (BT2)?true:false));
			}
			break;
		case 13: // read BTU
 			{	
				DWORD FreqInMHz=0;
				DWORD BT2=0;
				sscanf (line, "%d %d %d",&cmd, &FreqInMHz, &BT2);
    	
				DebugLogRite(" Dut_Bt_SetBtChannelByFreq: 0x%08X\n", 
					Dut_Bt_SetBtChannelByFreq(FreqInMHz, BT2?true:false));
			}
			break;

#if defined(_W8688_)
	   case 14:
		   {
			   int status = 0;
			   bool bEnable=0;
			
			   cnt = sscanf (line, "%d %d", 
					&cmd, &bEnable);  

			   status = Dut_Bt_SetFE_BTOnly(bEnable);
			   DebugLogRite("Dut_Bt_SetFE_BTOnly: 0x%08X\n", status);
		   }
		   break;
#endif // #if defined(_W8688_)

	   case 15: // read BTU
 			{	
				double PwrLvlInDB=4.0;
   					
				Dut_Bt_GetBtDisableBtuPwrCtl((bool*)&parm1);
 				if(parm1)
				{
				DebugLogRite(" Dut_Bt_GetBtPwrLvlValue: 0x%08X\n", 
					Dut_Bt_GetBtPwrLvlValue(&PwrLvlInDB));
				DebugLogRite(" BT Pwr Level : %lf\n", PwrLvlInDB);
				}
				else
				{
				DebugLogRite("\"Bt Pwr Lvl Value\" is only valid in \"Bt Disable Btu Pwr Ctl\" mode is true\n");
				}
			}
			break;
		case 16: // read BTU
 			{	
				double PwrLvlInDB=4.0;
				DWORD IsEDR=0;

				Dut_Bt_GetBtDisableBtuPwrCtl((bool*)&parm1);
 				if(parm1)
				{
				sscanf (line, "%d %lf %d",
					&cmd, &PwrLvlInDB, &IsEDR);
    	
				DebugLogRite(" Dut_Bt_SetBtPwrLvlValue: 0x%08X\n", 
					Dut_Bt_SetBtPwrLvlValue(PwrLvlInDB, IsEDR));
				}
				else
				{
				DebugLogRite("\"Bt Pwr Lvl Value\" is only valid in \"Bt Disable Btu Pwr Ctl\" mode is true\n");
				}
			}
			break;

		case 17: // read BTU
 			{	
				DWORD enable =0, packetType=DEF_PACKETTYPE;
				DWORD payloadPattern=DEF_PAYLOADPATTERN;
				sscanf (line, "%d %d %x %x",
					&cmd, &enable, 
					&packetType, &payloadPattern);
    	
				DebugLogRite(" Dut_Bt_BrfTxBtCont: 0x%08X\n", 
					Dut_Bt_BrfTxBtCont(enable?true:false, 
									packetType, 
									payloadPattern));
			}
			break;
 
		case 18: // read BTU
 			{	
				DebugLogRite(" Dut_Bt_TxBtCw: 0x%08X\n", 
					Dut_Bt_TxBtCw(parm1?true:false));
			}
			break;
 
/*
		case 19: // read BTU
 			{	    	

				cnt = sscanf (line, "%d %d %d",
					&cmd, &parm1, &parm2);
				if(cnt==3)
				{
					DebugLogRite(" Dut_Bt_TxBtCst: 0x%08X\n", 
						Dut_Bt_TxBtCst(	parm1?true:false, 
										parm2));
				}
				else
				{
					DebugLogRite(" Dut_Bt_TxBtCst: 0x%08X\n", 
						Dut_Bt_TxBtCst(parm1?true:false));
				}

			}
			break;

		case 22:
			{
				double pwr=0;
				int mode=0, useFeLoss=0;
				sscanf (line, "%d %lf %d %d",
					&cmd, &pwr, &mode, &useFeLoss);
				DebugLogRite(" Dut_Bt_SetRfPowerUseCal: 0x%08X\n", 
					Dut_Bt_SetRfPowerUseCal(pwr, mode, useFeLoss));
			}
			break;
*/		
		
		case 20: //  
 			{	
				double PwrMaxInDB=4.0, PwrMinInDB=-6.0;
    	
				Dut_Bt_GetBtDisableBtuPwrCtl((bool*)&parm1);
 				if(parm1)
				{
				DebugLogRite(" Dut_Bt_GetBtPwrMaxMin: 0x%08X\n", 
					Dut_Bt_GetBtPwrMaxMin(&PwrMaxInDB, &PwrMinInDB));
				DebugLogRite(" BT Pwr Max : %lf\tMin : %lf\n",  
						PwrMaxInDB, PwrMinInDB);
				}
				else
				{
				DebugLogRite("\"Bt Pwr Max Min\" is only valid in \"Bt Disable Btu Pwr Ctl\" mode is true\n");
				}
			}
			break;
 
		case 21: // read BTU
 			{	
				double PwrStepInDB=0.0;

				Dut_Bt_GetBtDisableBtuPwrCtl((bool*)&parm1);
 				if(parm1)
				{
				sscanf (line, "%d %lf",
					&cmd, &PwrStepInDB );
    	
				DebugLogRite(" Dut_Bt_StepPower: 0x%08X\n", 
					Dut_Bt_StepPower(PwrStepInDB));
				}
				else
				{
				DebugLogRite("\"Step Power\" is only valid in \"Bt Disable Btu Pwr Ctl\" mode is true\n");
				}
			}
			break;

		case 23: // read BTU
 			{	
				double PwrStepInDB=0.0;

				Dut_Bt_GetBtDisableBtuPwrCtl((bool*)&parm1);
 				if(parm1)
				{
				sscanf (line, "%d %lf",
					&cmd, &PwrStepInDB );
    	
				DebugLogRite(" Dut_Bt_GetBtPwrGain: 0x%08X\n", 
					Dut_Bt_GetBtPwrGain(&parm1));
				DebugLogRite(" BT Pwr Gain: 0x%08X\n",  parm1);
				}
				else
				{
				DebugLogRite("\"Bt Pwr Gain\" is only valid in \"Bt Disable Btu Pwr Ctl\" mode is true\n");
				}
			}
			break;

		case 24: // read BTU
 			{	
    	
				Dut_Bt_GetBtDisableBtuPwrCtl((bool*)&parm2);
 				if(parm2)
				{
				DebugLogRite(" Dut_Bt_SetBtPwrGain: 0x%08X\n", 
					Dut_Bt_SetBtPwrGain(parm1));
				}
				else
				{
				DebugLogRite("\"Bt Pwr Gain\" is only valid in \"Bt Disable Btu Pwr Ctl\" mode is true\n");
				}
			}
			break;
			
 
 
		case 25: // read BTU
 			{	
				DWORD enable =0, packetType=DEF_PACKETTYPE, payloadPattern=DEF_PAYLOADPATTERN;
				DWORD DutyWeight=95, payloadOnly=0;
				sscanf (line, "%d %d %x %x %d %d",
					&cmd, &enable, 
					&packetType, &payloadPattern, &DutyWeight, &payloadOnly);
    	
				DebugLogRite(" Dut_Bt_BrfTxBtDutyCycle: 0x%08X\n", 
					Dut_Bt_BrfTxBtDutyCycle(	enable?true:false, 
											packetType, 
											payloadPattern, 
											DutyWeight, 
											payloadOnly?true:false));
			}
			break;
		case 225: // read BTU
 			{	
				BOOL enable =0;
				int packetType=DEF_PACKETTYPE, payloadPattern=DEF_PAYLOADPATTERN;
				int PayloadLenghtInByte=PAYLOADLENGTH_MAXIMUN_POSSIBLE, HopMode=0, Interval=1, Whitening=0;
				sscanf (line, "%d %d %x %x %d %d %d %d",
					&cmd, &enable, 
					&packetType, &payloadPattern, 
					&PayloadLenghtInByte, &HopMode, &Interval, &Whitening);
    	
#ifdef _HCI_PROTOCAL_
				DebugLogRite(" Dut_Bt_TxBtDutyCycleHop: 0x%08X\n", 
						Dut_Bt_TxBtDutyCycleHop(
						enable?true:false, 
						packetType,
						payloadPattern, 
						PayloadLenghtInByte, 
						HopMode?true:false, 
						Interval, 
						Whitening?true:false));
#else
				DebugLogRite(" Dut_Bt_MfgMrvlTxTest: 0x%08X\n", 
						Dut_Bt_MfgMrvlTxTest(
						enable?true:false, 
						packetType,
						payloadPattern, 
						PayloadLenghtInByte, 
						HopMode?true:false, 
						Interval, 
						Whitening?true:false));
#endif // #ifdef _HCI_PROTOCAL_
			}
			break;


  
		case 33: // read BTU
 			{	
				BOOL TxCnt =100;
				char TxPowerLvl=4; 
				int packetType=DEF_PACKETTYPE, payloadPattern=DEF_PAYLOADPATTERN;
				int PayloadLenghtInByte=PAYLOADLENGTH_MAXIMUN_POSSIBLE, HopMode=0, Interval=1, Whitening=0;
				sscanf (line, "%d %d %d %x %x %d %d %d %d",
					&cmd, &TxCnt, &TxPowerLvl, 
					&packetType, &payloadPattern, 
					&PayloadLenghtInByte, &HopMode, &Interval, &Whitening);
    	
#ifdef _HCI_PROTOCAL_
				DebugLogRite(" Dut_Bt_TxBtTxCountedCycleHop: 0x%08X\n", 
						Dut_Bt_TxBtTxCountedCycleHop(
						TxCnt, 
						TxPowerLvl,
						packetType,
						payloadPattern, 
						PayloadLenghtInByte, 
						HopMode?true:false, 
						Interval, 
						Whitening?true:false));
#else
				DebugLogRite(" Dut_Bt_MfgMrvlCountedTxTest: 0x%08X\n", 
						Dut_Bt_MfgMrvlCountedTxTest(
						TxCnt, 
						TxPowerLvl,
						packetType,
						payloadPattern, 
						PayloadLenghtInByte, 
						HopMode?true:false, 
						Interval, 
						Whitening?true:false));
#endif // #ifdef _HCI_PROTOCAL_
			}
			break;




//#ifdef _HCI_PROTOCAL_

/*		case 30: // read BTU
 			{	
				if(parm2==1)
					DebugLogRite(" Dut_Bt_MfgMrvlRssiClear: 0x%08X\n", 
						Dut_Bt_MfgMrvlRssiClear(parm1));
				else
				{
					int RssiAvg=0, RssiRaw=0;

					DebugLogRite(" Dut_Bt_MfgMrvlRssiRpt: 0x%08X\n", 
						Dut_Bt_MfgMrvlRssiRpt(parm1, &RssiAvg, &RssiRaw));

					DebugLogRite("RssiAvg\t= %d\n",RssiAvg);
					DebugLogRite("RssiRaw\t= %d\n",RssiRaw);
				}
			}
			break;
*/
		case 32: // read BTU
 			{	
				int status=0;
				int RxChannel=10;
				DWORD FrameCount =1000, packetType=0x35, payloadPattern=DEF_PAYLOADPATTERN;
				DWORD PayloadLength=-1; 
                DWORD t[SIZE_BDADDRESS_INBYTE];
				char TxBdAddress[SIZE_BDADDRESS_INBYTE]={0x00, 0x00, 0x88, 0xc0, 0xff, 0xee};
                int i=0;
  
				sscanf (line, "%d %d %d %d %x %x %x-%x-%x-%x-%x-%x", //:%x %d %d %x %d",
							&cmd, &RxChannel, &FrameCount, &PayloadLength,
							&packetType, &payloadPattern,  
				            &t[0],&t[1],&t[2],&t[3],&t[4],&t[5]);
			
                for(i=0;i<SIZE_BDADDRESS_INBYTE;i++)
                    TxBdAddress[i]=(char)t[i];
				
#ifdef _HCI_PROTOCAL_
				status = Dut_Bt_MrvlRxTest //Dut_Bt_TxBtRxTest
#else	//#ifdef _HCI_PROTOCAL_
				status = Dut_Bt_MfgMrvlRxTest //Dut_Bt_TxBtRxTest
#endif	//#ifdef _HCI_PROTOCAL_

										(	RxChannel, 
											FrameCount, 
											packetType, 
											payloadPattern,	
											PayloadLength,  
											TxBdAddress );
				DebugLogRite(
#ifdef _HCI_PROTOCAL_
					" Dut_Bt_MrvlRxTest: 0x%08X\n", 
#else	//#ifdef _HCI_PROTOCAL_
					" Dut_Bt_MfgMrvlRxTest: 0x%08X\n",
#endif	//#ifdef _HCI_PROTOCAL_
					status);
				}
			break;
		case 31: // read BTU
 			{	
				int status=0;
			 	// report
 				BYTE		Report_Status=0; // status
				DWORD		Report_TotalPcktCount=0;
				DWORD		Report_HecMatchCount=0;
				DWORD		Report_HecMatchCrcPckts=0;
				DWORD		Report_NoRxCount=0;
				DWORD		Report_CrcErrCount=0;
				DWORD		Report_SuccessfulCorrelation=0;
				DWORD		Report_TotalByteCount=0;
				double		Report_BER=0;
				DWORD		Report_PER=0;
				DWORD		Report_AvgRSSI=0;
				DWORD		Report_TotalBitsCountExpected=0;
				DWORD		Report_TotalBitsCountError_LostDrop=0;
					
				status = Dut_Bt_MfgMrvlRxTestResult1  
					(	&Report_Status,  
						&Report_TotalPcktCount,
						&Report_HecMatchCount,
						&Report_HecMatchCrcPckts,
						&Report_NoRxCount,
						&Report_CrcErrCount,
						&Report_SuccessfulCorrelation,
						&Report_TotalByteCount,
						&Report_BER,
						&Report_PER,
						&Report_AvgRSSI,
						&Report_TotalBitsCountExpected,
						&Report_TotalBitsCountError_LostDrop); 

				DebugLogRite(" Dut_Bt_MfgMrvlRxTestResult: 0x%08X\n", status);

				if(!status)
				{
					if(Report_Status)
					DebugLogRite("\nReport\n"); 

					DebugLogRite("Status_Complete0_Abort1\t%8d\n", 			
											Report_Status);
					DebugLogRite("TotalPcktCount\t\t%8d\n",		Report_TotalPcktCount);
					DebugLogRite("SuccessfulCorrelation\t%8d\n",Report_SuccessfulCorrelation);
					DebugLogRite("HecMatchCount\t\t%8d\n", 		Report_HecMatchCount);
					DebugLogRite("HecMatchCrcPckts\t%8d\n",		Report_HecMatchCrcPckts);
					DebugLogRite("NoRxCount\t\t%8d\n",			Report_NoRxCount);
					DebugLogRite("CrcErrCount\t\t%8d\n",		Report_CrcErrCount);
					DebugLogRite("TotalByteCount\t\t%8d\n",		Report_TotalByteCount);
					DebugLogRite("BER\t\t\t%8.6f %% \n",		Report_BER);
					DebugLogRite("PER\t\t\t%5d.00 %%\n",		Report_PER);
					DebugLogRite("Avg RSSI\t\t%6d\n",			Report_AvgRSSI);
					DebugLogRite("TotalBitsCountExpected\t%8d\n",	Report_TotalBitsCountExpected);
					DebugLogRite("TotalBitsCountError\t%d\n", Report_TotalBitsCountError_LostDrop);

					DebugLogRite("\n\n");
				}
				else if (ERROR_MISMATCH == status)
					DebugLogRite("!!! Cmd 32 should be entered before cmd 31 can work properly.\n\n");
			}
			break;


#ifndef _HCI_PROTOCAL_
		case 30: // read BTU
 			{	
				if(parm2==1)
					DebugLogRite(" Dut_Bt_MfgMrvlRssiClear: 0x%08X\n", 
						Dut_Bt_MfgMrvlRssiClear(parm1));
				else
				{
					int RssiAvg=0, RssiRaw=0;

					DebugLogRite(" Dut_Bt_MfgMrvlRssiRpt: 0x%08X\n", 
						Dut_Bt_MfgMrvlRssiRpt(parm1, &RssiAvg, &RssiRaw));

					DebugLogRite("RssiAvg\t= %d\n",RssiAvg);
					DebugLogRite("RssiRaw\t= %d\n",RssiRaw);
				}
			}
			break;

/*		case 32: // read BTU
 			{	
				int status=0;
				int RxChannel=40;
				DWORD FrameCount =100, packetType=0x35; //DEF_PACKETTYPE;
				DWORD payloadPattern=DEF_PAYLOADPATTERN;
				DWORD PayloadLength=1021, payloadOnly=0; 
				char TxBdAddress[SIZE_BDADDRESS_INBYTE]={0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
  
				sscanf (line, "%d %d %d %d %x %x %x-%x-%x-%x-%x-%x", //:%x %d %d %x %d",
							&cmd, &RxChannel, &FrameCount, &PayloadLength,
							&packetType, &payloadPattern,  
							&TxBdAddress[0], &TxBdAddress[1], &TxBdAddress[2],
							&TxBdAddress[3], &TxBdAddress[4], &TxBdAddress[5]); 
			
				
				status = Dut_Bt_MfgMrvlRxTest //Dut_Bt_TxBtRxTest
										(RxChannel, 
										FrameCount, 
										packetType, 
										 payloadPattern,	
										 PayloadLength,  
										 TxBdAddress );
				DebugLogRite(" Dut_Bt_MfgMrvlRxTest: 0x%08X\n", status);
			}
			break;
		case 31: // read BTU
 			{	
				int status=0;
 			 			 // report
				int		Report_Status_Complete0_Abort1=0;  
#ifndef _BTRX_RSSIONLY_
				int		Report_TotalPcktCount=0;
				int		Report_SuccessfulCorrelation=0;
				int		Report_TotalRxedPkt=0; 
				int		Report_HecMatchCount=0;
				int		Report_HecMatchCrcPckts=0;
				int		Report_HdrErrCount=0;
				int		Report_CrcErrCount=0;
				int		Report_NoErrPktCount=0; 
				int		Report_TotalBitExpectedCount=0;
				int		Report_ErrorBits=0;
				double	Report_PER=0;
				double	Report_BER=0;
#endif //#ifndef _BTRX_RSSIONLY_

				int		Report_Rssi=0;
 					 
				status = Dut_Bt_MfgMrvlRxTestResult  
									( 		 // report
										&Report_Status_Complete0_Abort1, // status
#ifndef _BTRX_RSSIONLY_
										&Report_TotalPcktCount,
										&Report_SuccessfulCorrelation,
										&Report_TotalRxedPkt,
										&Report_HecMatchCount,
										&Report_HecMatchCrcPckts,
										&Report_HdrErrCount,
										&Report_CrcErrCount,
										&Report_NoErrPktCount,
										&Report_TotalBitExpectedCount,
										&Report_ErrorBits,
										&Report_PER,
										&Report_BER,
#endif //#ifndef _BTRX_RSSIONLY_
										&Report_Rssi); 
				DebugLogRite(" Dut_Bt_MfgMrvlRxTestResult: 0x%08X\n", status);

				if(!status)
				{
					DebugLogRite("\nReport\n"); 

					DebugLogRite("Status_Complete0_Abort1\t%8d\n", 
						Report_Status_Complete0_Abort1);
#ifndef _BTRX_RSSIONLY_
					DebugLogRite("TotalPcktCount\t\t%8d\n", 	Report_TotalPcktCount);
					DebugLogRite("SuccessfulCorrelation\t%8d\n",Report_SuccessfulCorrelation);
					DebugLogRite("HecMatchCount\t\t%8d\n", 		Report_HecMatchCount);
				//	DebugLogRite("TotalRxedPkt\t\t%8d\n", 		Report_TotalRxedPkt);
					DebugLogRite("HecMatchCrcPckts\t%8d\n", 	Report_HecMatchCrcPckts);
				//	DebugLogRite("HeaderErrCount\t\t%8d\n", 	Report_HdrErrCount);
					DebugLogRite("CrcErrCount\t\t%8d\n", 		Report_CrcErrCount);
				//	DebugLogRite("TotalBitExpectedCount\t%8d\n",Report_TotalBitExpectedCount);
				//	DebugLogRite("NoErrPktCount\t\t%8d\n", 		Report_NoErrPktCount);
				//	DebugLogRite("ErrorBits\t\t%8d\n", 			Report_ErrorBits);
					DebugLogRite("PER\t\t\t%8.2lf %%\n", 		Report_PER);
					DebugLogRite("BER\t\t\t%8.2lf %%\n", 		Report_BER);
#endif //#ifndef _BTRX_RSSIONLY_
					DebugLogRite("Avg RSSI\t\t%6d\n", 			Report_Rssi);
					DebugLogRite("\n\n");
				}
				else if (ERROR_MISMATCH == status)
					DebugLogRite("!!! Cmd 32 should be entered before cmd 31 can work properly.\n\n");


			}
			break;



*/
#endif //#ifndef _HCI_PROTOCAL_

		case 35:
			DebugLogRite("Dut_Bt_SetBtBrfReset: %X\n", Dut_Bt_SetBtBrfReset());
			break;

/*
		case 933: // read BTU
 			{	
				DWORD FrameCount =100, packetType=DEF_PACKETTYPE, payloadPattern=DEF_PAYLOADPATTERN;
				DWORD PayloadLength=1024, payloadOnly=0;
				sscanf (line, "%d %x %x %x %x %d",
					&cmd, &FrameCount, &PayloadLength,
					&packetType, &payloadPattern,  &payloadOnly);
    	
				DebugLogRite(" Dut_Bt_TxBtMultiCastFrames: 0x%08X\n", 
						Dut_Bt_TxBtMultiCastFrames(	FrameCount, 
													PayloadLength, 
													packetType, 
													payloadPattern, 
													payloadOnly?true:false));
			}
			break;

*/


#if !defined (_WLAN_) && !defined (_NO_EEPROM_) 		
#ifndef _FLASH_
		case 39:
		{
			if (Dut_Shared_SupportNoE2PROM())
			{
				DebugLogRite("Unknown Option\n");
			}
			else
			{			
				Dut_Bt_FlexSpiDL(true, "");
				DebugLogRite(" Dut_Bt_FlexSpiDL: 0x%08X\n", status);
 
			}
		}
			break;
#endif //#ifndef _FLASH_


  		case 40:
		{
			if (Dut_Shared_SupportNoE2PROM())
			{
				DebugLogRite("Unknown Option\n");
			}
			else
			{			
				Dut_Bt_FlexSpiCompare(""); 
			}
		}			break;

		case 41:
		{
			int cnt=0; 
			DWORD startingAddress=0, Length=0;
 			DWORD *AddArray=NULL, *DataArray=NULL;
			BYTE *ByteArray=NULL;
			int  Index=0;

			startingAddress = parm1;
			Length = (parm2)? parm2:0x10;

			AddArray = 	(DWORD*)malloc(Length*sizeof(DWORD)); 
			DataArray = (DWORD*)malloc(Length*sizeof(DWORD)); 
			memset(AddArray, 0, Length*sizeof(DWORD));
			memset(DataArray, 0, Length*sizeof(DWORD));

			ByteArray = (BYTE*) DataArray;

			DebugLogRite("Dut_Bt_DumpE2PData 0x%08X\n", 
				Dut_Bt_DumpE2PData( startingAddress,  Length, 
							 AddArray, DataArray ));

			for ( Index =0; Index < (signed long)Length; Index ++)
 				DebugLogRite(" 0x%08X\t:\t0x%08X \n", 
							AddArray[Index], DataArray[Index]); 

 			free(AddArray);
			free(DataArray);

		} 
			break;
		case 43:
		{ 
			DWORD IfType=0, AddrWidth=-1, DeviceType=-1;
				sscanf (line, "%d %d %d %d",
					&cmd, &IfType, &AddrWidth, &DeviceType);

			DebugLogRite("Dut_Bt_ForceE2PromType 0x%08X\n", 
				Dut_Bt_ForceE2PromType(IfType, AddrWidth, DeviceType));
		} 
			 
		case 42:
		{ 
			DWORD IfType=0, AddrWidth=-1, DeviceType=-1;
 
			DebugLogRite("Dut_Bt_GetE2PromType 0x%08X\n", 
				Dut_Bt_GetE2PromType(&IfType, &AddrWidth, &DeviceType));

			DebugLogRite("IfType %d\tAddrWidth %d\tDeviceType %d\n", IfType, AddrWidth, DeviceType);
		} 
			break;
#endif  //	#if !defined (_WLAN_) && !defined (_NO_EEPROM_)

		case 45: // get BD address from FW
			{	
				unsigned char tempBdAddress[SIZE_BDADDRESS_INBYTE]={0}; 

#if !defined(_NO_EEPROM_)
				status = GetBdAddressFromE2p(tempBdAddress);
				DebugLogRite(" Dut_Bt_GetBdAddressFromE2P: 0x%08X\n", status);
				DebugLogRite(" Dut_Bt_GetBdAddressFromE2P: %02X-%02X-%02X-%02X-%02X-%02X\n\n", 
					tempBdAddress[0], tempBdAddress[1], tempBdAddress[2], 
					tempBdAddress[3], tempBdAddress[4], tempBdAddress[5]);
#endif //#if !defined(_NO_EEPROM_)				
 

#ifdef _HCI_PROTOCAL_
				DebugLogRite(" Dut_Bt_GetBDAddress: 0x%08X\n", 
					Dut_Bt_GetBDAddress(tempBdAddress));
#else	//#ifdef _HCI_PROTOCAL_
				DebugLogRite(" Dut_Bt_MfgGetFwBdAddress: 0x%08X\n", 
					Dut_Bt_MfgGetFwBdAddress(tempBdAddress));

#endif //#ifdef _HCI_PROTOCAL_

				DebugLogRite("BD_ADDRESS: %02X-%02X-%02X-%02X-%02X-%02X\n", 
 					tempBdAddress[0], tempBdAddress[1], tempBdAddress[2], 
					tempBdAddress[3], tempBdAddress[4], tempBdAddress[5]);
			}
			break;

#if defined(_W8780_) || defined (_W8688_) || defined (_W8689_) || defined (_W8787_) || defined(_W8782_)

		case 46:  
			{
				BYTE tempBdAddress[SIZE_BDADDRESS_INBYTE]={0};
			int tempInt[6]={0}, cnt=0; 
			cnt = sscanf (line, "%d %x.%x.%x.%x.%x.%x",&cmd, 
				&tempInt[0], &tempInt[1], &tempInt[2],
				&tempInt[3], &tempInt[4], &tempInt[5]);
			if(cnt < 7) 
			{
				DebugLogRite("Invalid BD Address\n"); 
				break;
			}
			tempBdAddress[0]= tempInt[0];
			tempBdAddress[1]= tempInt[1];
			tempBdAddress[2]= tempInt[2];
			tempBdAddress[3]= tempInt[3];
			tempBdAddress[4]= tempInt[4];
			tempBdAddress[5]= tempInt[5];
		
				status = ChangeBdAddress(tempBdAddress);
				DebugLogRite(" ChangeBdAddress: 0x%08X\n", status);

			}
			break;
#endif //#if defined(_W8780_) || defined (_W8688_) || defined (_W8689_) || defined (_W8787_) || defined(_W8782_)

#if !defined(_NO_EEPROM_)

#if defined(_IF_USB_) 
		case 47: // Set continuous Tx
			if (Dut_Shared_SupportNoE2PROM())
			{
				DebugLogRite("Unknown Option\n");
			}
			else
			{
				DebugLogRite(" Dut_Bt_GetUsbAttributes: 0x%08X\n", 
						Dut_Bt_GetUsbAttributes((BYTE*)&parm1, (BYTE*)&parm2));
				if(0xFF == parm1 && 0xFF == parm2) 
					DebugLogRite("Usb Attribute: Values are not  set\n");
				else
					DebugLogRite("Attribute: 0x%02X\nMaxPower : %d\n", parm1, parm2);
			}									  
			break;

		case 48: // Set continuous Tx
			if (Dut_Shared_SupportNoE2PROM())
			{
				DebugLogRite("Unknown Option\n");
			}
			else
			{	
				//BYTE Attribute=0xC0, MaxPower=0;
				BYTE Attribute=0xff, MaxPower=0xff;

 				cnt = sscanf (line, "%d %x %d",&cmd, &Attribute, &MaxPower);
				// check input
				if ((Attribute==0xff) || (MaxPower==0xff))
				{
					DebugLogRite(" ERROR: No valid input\n");
					break;
				}
				// check max power
				if (MaxPower > 0xfa)
				{
					DebugLogRite(" ERROR: Invalid USB max power %d entered\n", MaxPower);
					break;
				}
				// check bit 0 to 4
				if (Attribute & 0x1f)
				{
					DebugLogRite(" ERROR: Invalid USB attribute option 0x%02X, bit 0-4 are not set to all 0\n", Attribute);
					break;
				}
				// check bit 7
				if (!(Attribute & 0x80))
				{
					DebugLogRite(" ERROR: Invalid USB attribute option 0x%02X, bit 7 is not set to 1\n", Attribute);
					break;
				}

				DebugLogRite(" Dut_Bt_SetUsbAttributes: 0x%08X\n", 
						Dut_Bt_SetUsbAttributes(Attribute, MaxPower));
			}
 			break;
#endif //#if defined(_IF_USB_) 

#if defined(_W8780_) || defined (_W8688_) || defined (_W8689_) || defined (_W8787_)  || defined(_W8782_)
		case 47:
			{
				int rate=115200; 

 				cnt = sscanf (line, "%d %d",&cmd, &rate);

				DebugLogRite(" ChangeUartRate: 0x%08X\n", ChangeUartRate(rate));
			}
			break;
#endif //#if defined(_W8780_) || defined (_W8688_) || defined (_W8689_) || defined (_W8787_) || defined(_W8782_)


#ifdef _FLASH_	
			case 51:
			{
 				char charTemp;
				DebugLogRite("To prevent \"DEAD CARD\" problem, "
				"do you like to load build in bootloader?[Y]\n");

				fgets(line, sizeof(line), stdin);
				  sscanf (line, "%c", &charTemp);

				if('N' == charTemp || 'n' == charTemp)
				{	
					DebugLogRite(" Too Risky to do that. Refused to perform it\n");
//					status = Dut_Bt_EraseEEPROM(parm1);
//					DebugLogRite(" Dut_Bt_EraseEEPROM: 0x%08X\n", status);
				}
				else
				{
					Dut_Bt_FlexSpiDL(true, "");
					DebugLogRite(" Dut_Bt_FlexSpiDL: 0x%08X\n", status);
				}
			}
			break;
#endif //#ifndef _FLASH_
		
		case 53:
			if(parm1 == 0)
				DebugLogRite("SaveCalDataBt:\t0x%08X\n", 
					LoadSaveCalData(1) );
			else
				DebugLogRite("UpdateSaveBtCalData:\t0x%08X\n", 
					UpdateSaveBtCalData(1) );
			break;

		case 54:
			DebugLogRite("UpLoadSaveCalData:\t0x%08X\n", 
				UpLoadSaveCalData((int*) &parm1, 1) );
			
			DebugLogRite("CrC %d\n", parm1);
			break;
#endif // #if !defined(_NO_EEPROM_)

		case 62: // read BTU
 			{	
				DWORD Address =0;
				WORD Value=0;
				int i=0, cnt=0;
 				cnt = (parm2==0)?1:parm2;
				for (i=0; i<cnt; i++)
				{
					Address = parm1+i*sizeof(WORD); 
					status = Dut_Bt_ReadBtuRegister(Address, &Value);
					DebugLogRite("Dut_Bt_ReadBtuRegister : 0x%04X\t0x%04X\t", Address, Value);
					DebugLogRite("  0x%08X\n", status);
				}
			}
			break;
		case 63: // write BTU
 			{	
				DWORD Address =0;
				WORD Value=0;
				Address = parm1; 
				Value = (WORD)parm2;
				DebugLogRite(" Dut_Bt_WriteBtuRegister : 0x%08X\n", 
					Dut_Bt_WriteBtuRegister(Address, Value));
			}
			break;

		case 64: // read BRF
			{	
				DWORD	Address =0;
				WORD	Value=0;
				int		i=0, cnt=0;
				int status=0;
 				cnt = parm2==0?1:parm2;
				for (i=0; i<cnt; i++)
				{
					Address = parm1+i; // *sizeof(WORD); 
					status= Dut_Bt_ReadBrfRegister(Address, &Value);
					DebugLogRite("Dut_Bt_ReadBrfRegister : 0x%04X\t0x%02X\t", Address, Value);
					DebugLogRite(": 0x%08X\n", status);
					if(status) break;
				}
			}
			break;
		case 65: // write BRF
			{	
				DWORD Address =0;
				WORD Value=0;
				Address = parm1;
				Value = (WORD)parm2; 
				DebugLogRite(" Dut_Bt_WriteBrfRegister : 0x%08X\n", 
					Dut_Bt_WriteBrfRegister(Address, Value));
			}
			break;

		case 68: // read mem
			{	
				DWORD Address =0, Value=0;
				int i=0, cnt=0;

				sscanf (line, "%d %x %x",&cmd, &parm1, &parm2);
				cnt = parm2==0?1:parm2;
				for (i=0; i<cnt; i++)
				{
					Address = parm1+i*sizeof(DWORD); 
					status = Dut_Bt_ReadMem(Address, &Value);
					DebugLogRite("Dut_Bt_ReadMem : 0x%08X\t0x%08X\t", Address, Value);
					DebugLogRite(" 0x%08X\n", status);
				}
			}
			break;
		case 69: // write mem
			{	
				DWORD Address =0, Value=0;
				Address = parm1; 
				Value = parm2;
				DebugLogRite(" Dut_Bt_WriteMem : 0x%08X\n", 
					Dut_Bt_WriteMem(Address, Value));
			}
			break;
#if !defined(_NO_EEPROM_)
		case 70: // read mem
			{	
				DWORD Address =0, Value=0;
				int i=0, cnt=0;

				sscanf (line, "%d %x %x",&cmd, &parm1, &parm2);
				cnt = parm2==0?1:parm2;
				for (i=0; i<cnt; i++)
				{
					Address = parm1+i*sizeof(DWORD); 
					status = Dut_Bt_SpiReadDword(Address, &Value);
					DebugLogRite("Dut_Bt_SpiReadDword : 0x%08X\t0x%08X\t", Address, Value);
					DebugLogRite("0x%08X\n", status);
				}
			}
			break;
		case 71: // write mem
			{	
				DWORD Address =0, Value=0;
				Address = parm1; 
				Value = parm2;
				DebugLogRite(" Dut_Bt_SpiWriteDword : 0x%08X\n", 
					Dut_Bt_SpiWriteDword(Address, Value)); 
			}
			break;
#endif //	#if !defined(_NO_EEPROM_)



#if !defined(_NO_EFUSE_)

#if defined (_W8689_) || defined (_W8787_) || defined (_W8790_) || defined(_W8782_) 
		case 72: // read mem
			{	
				DWORD block =0, len=16;
				WORD Data[16]={0};
				DWORD i=0;

				sscanf (line, "%d %x %x",&cmd, &block, &len);
 					DebugLogRite(" Dut_Bt_GetEfuseData: 0x%08X\n", 
						Dut_Bt_GetEfuseData(block, len, Data));
 				for (i=0; i<len; i++)
				{
					DebugLogRite("0x%08X\t0x%08X\n", i, Data[i]);
				}
			}
			break;
		case 73: // write mem
			{	
				DWORD block =0, len=16;
				WORD tempD[16]={0};
				WORD Data[16]={0};
				int i=0;

				sscanf (line, "%d %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x",
					&cmd, &block, &len, 
					&tempD[0],	&tempD[1],	&tempD[2],	&tempD[3], 
					&tempD[4],	&tempD[5],	&tempD[6],	&tempD[7], 
					&tempD[8],	&tempD[9],	&tempD[10],	&tempD[11], 
					&tempD[12],	&tempD[13],	&tempD[14],	&tempD[15]);
				for (i=0; i<16; i++)
				{
					Data[i]=tempD[i];
				}
				DebugLogRite(" Dut_Bt_SetEfuseData: 0x%08X\n", 
					Dut_Bt_SetEfuseData(block, len, Data));
			}
			break;
#endif	// #if defined (_W8689_) || defined (_W8787_) || defined (_W8782_) 
#endif // #if !defined(_NO_EFUSE_)

/*
		case 170: // read mem
			{	
				DWORD Address =0;
				BYTE Value=0;
				int i=0, cnt=0;

 				cnt = parm2==0?1:parm2;
				for (i=0; i<cnt; i++)
				{
					Address = parm1+i*sizeof(DWORD); 
					DebugLogRite(" Dut_Bt_SpiReadByte: 0x%08X\t", 
						Dut_Bt_SpiReadByte(Address, &Value));
					DebugLogRite("0x%08X\t0x%08X\n", Address, Value);
				}
			}
			break;

		case 171: // write mem
			{	
				DWORD Address =0;
				BYTE Value=0;
				Address = parm1; 
				Value = parm2;
				DebugLogRite(" Dut_Bt_SpiWriteByte: 0x%08X\n", 
					Dut_Bt_SpiWriteByte(Address, Value));
				DebugLogRite("0x%08X\t0x%08X\n", Address, Value);
			}
			break;
 			*/		
#if !defined (_NO_EEPROM_)
		case 75: // init RF
			{	
				DebugLogRite(" Dut_Bt_LoadCustomizedSettings: 0x%08X\n", 
					Dut_Bt_LoadCustomizedSettings());
			}
			break;
#endif // #if !defined (_NO_EEPROM_)

#ifdef _HCI_PROTOCAL_
		case 78: // init RF
			{	

				if(parm1)
				{ 
					DebugLogRite(" Dut_Bt_ResetBt: 0x%08X\n", 
						Dut_Bt_ResetBt());

					DebugLogRite(" Dut_Bt_SetDeviceDiscoverable: 0x%08X\n", 
						Dut_Bt_SetDeviceDiscoverable()); 
				}
				
#if !defined (_DEVELOPMENT_PHASE_)
				//
				//  Use default BT power for testing
				//
				DebugLogRite(" Dut_Bt_SetRfPowerUseCal: 0x%08X\n", 
					Dut_Bt_SetRfPowerUseCal(1, 0));
#endif // #if !defined (_DEVELOPMENT_PHASE_)

				DebugLogRite(" Dut_Bt_EnableDeviceUnderTestMode: 0x%08X\n", 
					Dut_Bt_EnableDeviceUnderTestMode()); 
			}
			break;
		case 79: // init RF
			{	
				int MaxPeriod=8, MinPeriod=7, InquryLength=4;

				if(parm1)
				{
 					cnt = sscanf (line, "%d %x %x %x %x",&cmd, 
								&parm1, &MaxPeriod, &MinPeriod, &InquryLength);
					DebugLogRite(" Dut_Bt_PeriodicInquery: 0x%08X\n", 
						Dut_Bt_PeriodicInquery(MaxPeriod, MinPeriod, InquryLength)); 
 				}
				else 
					DebugLogRite(" Dut_Bt_ExitPeriodicInquery: 0x%08X\n", 
						Dut_Bt_ExitPeriodicInquery()); 
 

			}
			break;
#else // #ifdef _HCI_PROTOCAL_
		case 78: // reset and enter test mode
			{	
				DebugLogRite(" Dut_Bt_MfgEnterTestMode: 0x%08X\n", 
					Dut_Bt_MfgEnterTestMode()); 
 
#if !defined (_DEVELOPMENT_PHASE_)
				DebugLogRite(" Dut_Bt_SetRfPowerUseCal: 0x%08X\n", 
					Dut_Bt_SetRfPowerUseCal(1, 0));
#endif // #if !defined (_DEVELOPMENT_PHASE_)

			}
			break;
		case 79: // init RF
			{	
				int MaxPeriod=8, MinPeriod=7, InquryLength=4;

				if(parm1)
				{
 					cnt = sscanf (line, "%d %x %x %x %x",&cmd, 
								&parm1, &MaxPeriod, &MinPeriod, &InquryLength);
					DebugLogRite(" Dut_Bt_MfgPeriodicInquery: 0x%08X\n", 
						Dut_Bt_MfgPeriodicInquery(MaxPeriod, MinPeriod, InquryLength)); 
 				}
				else 
					DebugLogRite(" Dut_Bt_MfgExitPeriodicInquery: 0x%08X\n", 
						Dut_Bt_MfgExitPeriodicInquery()); 
 			}
			break;

#endif //#ifdef _HCI_PROTOCAL_

		case 80: // reset BT HW
			{
#ifdef _HCI_PROTOCAL_
 				DebugLogRite(" Dut_Bt_ResetBt: 0x%08X\n", 
					Dut_Bt_ResetBt());
#else //#ifdef _HCI_PROTOCAL_
		//		if(parm1)
 		//		DebugLogRite(" Dut_Bt_MfgPfwReloadCalfromFlash: 0x%08X\n", 
		//			Dut_Bt_MfgPfwReloadCalfromFlash());
		//		else
 				DebugLogRite(" Dut_Bt_MfgResetBt: 0x%08X\n", 
					Dut_Bt_MfgResetBt());
#endif //#ifdef _HCI_PROTOCAL_
			}
			break;

 
       case 88: // load configuration file
			{  
				int SocId=0, SocVer=0, BtuId=0, BtuVer=0, BrfId=0, BrfVer=0;
 				char VersionString[_MAX_PATH]="";
				DWORD version=Dut_Bt_Version();
				int status = 0;

				DebugLogRite("DLL Version : %d.%d.%d.%d\n",  
					(version&0xff000000)>>24,
					(version&0x00ff0000)>>16,
					(version&0x0000ff00)>>8,
					(version&0x000000ff));

				DebugLogRite("LabTool Version: %d.%d.%d.%d\n",
					DUTCTRL_VERSION_MAJOR1,	
					DUTCTRL_VERSION_MAJOR2,
					DUTCTRL_VERSION_MINOR1, 
					DUTCTRL_VERSION_MINOR2); 

				status = Dut_Bt_GetFwVersion(NULL, NULL, VersionString);
				if (status)
					DebugLogRite("Dut_Bt_GetFwVersion : 0x%08X\n",  status);
				else
					DebugLogRite("%s\n", VersionString);

				status = Dut_Bt_GetHwVersion(&SocId, &SocVer, 
											&BtuId, &BtuVer, 
											&BrfId, &BrfVer);
				if (status)
					DebugLogRite("Dut_Bt_GetHwVersion : 0x%08X\n",  status);
				else
 				DebugLogRite("Soc Id : %04X\tSoc Version : %04X\n"
					"Btu Id : %04X\tBtu Version : %04X\n"
					"Brf Id : %04X\tBrf Version : %04X\n",  
						 SocId, SocVer, BtuId, BtuVer, BrfId, BrfVer);
 
				DebugLogRite(" Dut_Bt_GetCurrentORVersion : 0x%08X\n",  
						Dut_Bt_GetCurrentORVersion((int*)&parm1, (int*)&parm2));
				DebugLogRite("OR Version for Soc: %X.%X\nOR Version for RF : %X.%X\n", 
					((parm1 & 0xFFFF0000)>>16), ((parm1 & 0xFFFF)), 
					((parm2 & 0xFFFF0000)>>16), ((parm2 & 0xFFFF)));
			}
            break;

         case 188: // Get Online Release Note
			 {	
				char  releaseNote[MAX_LENGTH]={0};
 				status = Dut_Bt_GetFwReleaseNote(releaseNote);
 				DebugLogRite(" Dut_Bt_GetFwReleaseNote: 0x%08X\n%s\n", status, releaseNote);
		 			
				DebugLogRite(" Dut_Bt_GetCurrentORVersion : 0x%08X\n",  
						Dut_Bt_GetCurrentORVersion((int*)&parm1, (int*)&parm2));
				DebugLogRite("OR Version for Soc: %X.%X\nOR Version for RF : %X.%X\n", 
					((parm1 & 0xFFFF0000)>>16), ((parm1 & 0xFFFF)), 
					((parm2 & 0xFFFF0000)>>16), ((parm2 & 0xFFFF)));

#ifndef _W8790_
				DebugLogRite(" Dut_Bt_GetCurrentAppMode : 0x%08X\n",  
						Dut_Bt_GetCurrentAppMode((int*)&parm1));
				DebugLogRite("Current App Mode: %X\n", 
					parm1);

				status = Dut_Bt_GetChipClassModeCapacity((int*)&parm1);
 				DebugLogRite(" Dut_Bt_GetChipClassModeCapacity: 0x%08X\n"
					"Class Mode Capacity: %X\n", status, parm1);
#endif //#ifndef _W8790_
			 }
             break;

#if defined(_W8790_)
        case 89: // load configuration file
			{  
				char FileName[_MAX_PATH]="";
				cnt = sscanf (line, "%d %s",&cmd, FileName);
				if(strlen(FileName))
				DebugLogRite("LoadInitBtConfigFile [%s]: 0x%08X\n", FileName, 
						LoadInitBtConfigFile(FileName));
				else
				DebugLogRite("File Name not specified\n"); 

			}
            break;

		case 95:
			status = Dut_Bt_GetBtXtal ((BYTE*)&parm1, (BYTE*)&parm2);
			DebugLogRite ("Dut_Bt_GetBtXtal: 0x%08X\n", status); 
			DebugLogRite (" XTal: %s %02X\n", parm1?"ext":"int", parm2); 
			break;
			
		case 96:	// set RF crystal oscillator
			status = Dut_Bt_SetBtXtal ((BYTE)parm1, (BYTE)parm2);
			DebugLogRite ("Dut_Bt_SetBtXtal: 0x%08X\n", status); 
			break;
#endif // #if defined(_W8790_)

		case 111:  
			{	
 				DebugLogRite(" Dut_Bt_GetBt2ChHopping: 0x%08X\n", 
					Dut_Bt_GetBt2ChHopping((bool*)&parm1));
				DebugLogRite("FH between 2 Channel Mode %d\n", parm1);
			}
			break;
		case 112:  
			{	
 				DebugLogRite(" Dut_Bt_SetBt2ChHopping: 0x%08X\n", 
					Dut_Bt_SetBt2ChHopping(parm1?true:false));
			}
			break;
		case 113:  
			{	
 				DebugLogRite(" Dut_Bt_GetBtPwrControlClass: 0x%08X\n", 
					Dut_Bt_GetBtPwrControlClass(&parm1));
				DebugLogRite(" Power Control Class Mode %X\n", parm1);
			}
			break;
		case 114:  
			{	
 				DebugLogRite(" Dut_Bt_SetBtPwrControlClass: 0x%08X\n", 
					Dut_Bt_SetBtPwrControlClass(parm1));
			}
			break;
		case 115: // init RF
			{	
				DebugLogRite(" Dut_Bt_GetBtDisableBtuPwrCtl: 0x%08X\n", 
					Dut_Bt_GetBtDisableBtuPwrCtl((bool*)&parm1));
					DebugLogRite("Control : %d\n", parm1);
			}
			break;
		case 116: // init RF
			{	
				DebugLogRite(" Dut_Bt_SetBtDisableBtuPwrCtl: 0x%08X\n", 
					Dut_Bt_SetBtDisableBtuPwrCtl(parm1?true:false));
			}
			break;

 		case 117: // reset BT HW
			{	
				DebugLogRite(" Dut_Bt_SetBtThermalCtrlMode: 0x%08X\n", 
					Dut_Bt_SetBtThermalCtrlMode(parm1));
 			}
 			break;
			
			
 
#if defined (_W8688_) || defined (_W8787_) || defined(_W8782_)
/*
		case 329:  
			{	int Mode=0;
 
				DebugLogRite(" Dut_Bt_GetBcaMode: 0x%08X\n", 
					Dut_Bt_GetBcaMode(&Mode));
				DebugLogRite("Mode: 0x%02X\n", Mode);
			}
			break;
		case 330:  
			{	 
				DebugLogRite(" Dut_Bt_SetBcaMode: 0x%08X\n", 
					Dut_Bt_SetBcaMode(parm1));
			}
			break;
*/
#endif // #if defined (_W8688_) || defined (_W8787_)

#ifdef _HCI_PROTOCAL_
/*		case 231:
			{
  
				int ModeOnOff=0;
				bool PartnerLocalControlable=1;
				WORD MaxInterval=0x800, MinInterval=0x400;

 				sscanf (line, "%d %d %x %x %d",&cmd, &ModeOnOff, 
							&MaxInterval, &MinInterval, 
							&PartnerLocalControlable);
				DebugLogRite(" Dut_Bt_Test_Hold: 0x%08X\n", 
 					Dut_Bt_Test_Hold(ModeOnOff, 
									MaxInterval, 
									MinInterval, 
									PartnerLocalControlable));
			}
			break;
		case 232:
			{
 				int ModeOnOff=0;
				bool PartnerLocalControlable=1;
				WORD MaxInterval=0x800, MinInterval=0x400;
				WORD Timeout=1, Attempt=1;

 				sscanf (line, "%d %d %x %x %d %d %d",&cmd, &ModeOnOff, 
							&MaxInterval, &MinInterval, 
							&Attempt, &Timeout,  
							&PartnerLocalControlable);

				DebugLogRite(" Dut_Bt_Test_Sniff: 0x%08X\n", 
 					Dut_Bt_Test_Sniff
									(ModeOnOff, 
									MaxInterval, 
									MinInterval, 
									Attempt, 
									Timeout, 
									PartnerLocalControlable));
			}
			break;
		case 233:
			{
				int ModeOnOff=0, PartnerLocalControlable=1;
				WORD MaxInterval=0x800, MinInterval=0x400;

 				sscanf (line, "%d %d %x %x %d",&cmd, &ModeOnOff, 
							&MaxInterval, &MinInterval, 
							&PartnerLocalControlable);
				DebugLogRite(" Dut_Bt_Test_Park: 0x%08X\n", 
 					Dut_Bt_Test_Park(ModeOnOff, 
									MaxInterval, 
									MinInterval, 
									PartnerLocalControlable?true:false));
			}
			break;
*/

#endif //#ifdef _HCI_PROTOCAL_

 
 
 

#ifdef _HCI_PROTOCAL_
 /*
		case 296: // reset BT HW
			{	int SwPwrCtrlMode=2;
				WORD RefGain1 = 0x0278,	RefGain2 = 0x0178;
 				BYTE RefTemp = 0x65, TempGain = 0x05;
				
				sscanf (line, "%d %x %x %x %x",
							&cmd, &SwPwrCtrlMode, 
							&RefGain1, &RefGain2, &RefTemp, &TempGain);

 				DebugLogRite("Dut_Bt_SetMrvlTempCompModeHC: 0x%08X\n", 
								Dut_Bt_SetMrvlTempCompModeHC(
												SwPwrCtrlMode,
												RefGain1,
												RefGain2,
 												RefTemp,
 												TempGain));
 				DebugLogRite("Dut_Bt_ResetBt: 0x%08X\n", Dut_Bt_ResetBt());
			}
			break;

		case 333:
			{
				int PacketType=DEF_PACKETTYPE, PacketCount=100; 
				int PayloadLengthInByte=PAYLOADLENGTH_MAXIMUN_POSSIBLE;
				int PayloadPattern=DEF_PAYLOADPATTERN; 
				bool MasterAsTx= true;
				bool DutAsMaster= true;
				bool stress=false;
  
				int RxPacket=0;
				int MissingPacket=0;
				double Per=0,  Ber=0;
			
				cnt = sscanf (line, "%d  %x %x %d %d %d %d %d", 
					&cmd, &PacketCount, &PacketType, 
					&PayloadLengthInByte, &PayloadPattern, 
					&stress,
					&DutAsMaster, &MasterAsTx);
 
 				DebugLogRite(" Dut_Bt_AclLinkTest: 0x%08X\n", 
					Dut_Bt_AclLinkTest(PacketType, PacketCount, 
					  PayloadLengthInByte, PayloadPattern, 
					  MasterAsTx, DutAsMaster, 
					  stress, 
					  &RxPacket, &MissingPacket, &Per, &Ber));
				DebugLogRite("RxPacket %d\tMissingPacket %d\n"
					"Per %f\tBer %f\n", RxPacket, MissingPacket, Per, Ber);
			}
			break;

		case 1033:
			{	int i=0, j=0, status=0;
				int PacketType=DEF_PACKETTYPE, PacketCount=100; 
				int PayloadLengthInByte=PAYLOADLENGTH_MAXIMUN_POSSIBLE;
				int PayloadPattern=DEF_PAYLOADPATTERN; 
				bool MasterAsTx= true;
				bool DutAsMaster=true;
				bool stress=false;
				int RxPacket=0;
				int MissingPacket=0;
				double Per=0,  Ber=0;

				int PacketTypeArray[]={	0x01, 0x03, 0x05, 
										0x11, 0x13, 0x15, 
										0x21, 0x23, 0x25,
										0x31, 0x33, 0x35};

				cnt = sscanf (line, "%d  %d %d %d", 
					&cmd, &stress, &DutAsMaster,&MasterAsTx);  

				for (j=0; j<12; j++)
				{
				for (i=0; i<2; i++)
				{

					status =	Dut_Bt_AclLinkTest(PacketTypeArray[j], PacketCount, 
						  PayloadLengthInByte, PayloadPattern, 
						  MasterAsTx, DutAsMaster, 
						  stress, 
					  &RxPacket, &MissingPacket, &Per, &Ber);
 					DebugLogRite(" Dut_Bt_AclLinkTest: 0x%08X\n", status);
					if(status) break;
					DebugLogRite("RxPacket %d\tMissingPacket %d\n"
							"Per %f\tBer %f\n", RxPacket, MissingPacket, Per, Ber);

				}
				if(status) break;
				}
			}
			break;
*/
#endif //#ifdef _HCI_PROTOCAL_
 

// "BT Current Optimizations"
#ifdef _HCI_PROTOCAL_
/*
		case 340: // Dut_Bt_SetRfDynamicClkCtrl
			{ 
				int		DynClkTimer=0x190;
				int		MultiSlotProtect=1;
				int		AutoDetect=1;
				int		DynClkValue=1;
				int		DynClkEnable=1;
				int		DynClkStartTime=0x1000;
				int		DynClkFinishTime=0x137E;
				int		DynClkTimedEnable=1;

				cnt = sscanf (line, "%x %x %x %x %x %x %x %x %x",&DynClkTimer,
						 &cmd, 
						 &MultiSlotProtect,  
						 &AutoDetect,
						 &DynClkValue,
						 &DynClkEnable,
						 &DynClkStartTime,
						 &DynClkFinishTime,
						 &DynClkTimedEnable);
			
 				DebugLogRite(" Dut_Bt_SetRfDynamicClkCtrl: 0x%08X\n", 
					Dut_Bt_SetRfDynamicClkCtrl( DynClkTimer,
						 MultiSlotProtect,  
						 AutoDetect,
						 DynClkValue,
						 DynClkEnable,
						 DynClkStartTime,
						 DynClkFinishTime,
						 DynClkTimedEnable)); 
			}
			break;

		case 341:  // Dut_Bt_SetBtSleepMode
			{	int SleepMode=3;
				cnt = sscanf (line, " %x %d", &cmd, &SleepMode);
 				DebugLogRite(" Dut_Bt_SetBtSleepMode: 0x%08X\n", 
					Dut_Bt_SetBtSleepMode(SleepMode)); 
			}
			break;

		case 342: // Dut_Bt_SetWakeUpMethod
			{	
				int		HostWakeUpMethod=0;
				int		HostWakeUpGPIO=5;
				int		DeviceWakeUpMethod=0;
				int		DeviceWakeUpGPIO=4;

				cnt = sscanf (line, "%x %d %d %d %d",
						&cmd, 
						&HostWakeUpMethod,
						&HostWakeUpGPIO,
						&DeviceWakeUpMethod,
						&DeviceWakeUpGPIO);

 				DebugLogRite(" Dut_Bt_SetWakeUpMethod: 0x%08X\n", 
					Dut_Bt_SetWakeUpMethod(HostWakeUpMethod,
						HostWakeUpGPIO,
						DeviceWakeUpMethod,
						DeviceWakeUpGPIO)); 
			}
			break;


		case 343: // Dut_Bt_SetMrvlHiuModuleConfig
			{	int ModuleShutDownBringUp=0;
				int HostIfActive=0; 
				int HostIfType=0;

 				cnt = sscanf (line, "%x %d %d %d",
						&cmd, 
						&ModuleShutDownBringUp, 
						&HostIfActive, 
						&HostIfType);

				DebugLogRite(" Dut_Bt_SetMrvlHiuModuleConfig: 0x%08X\n", 
					Dut_Bt_SetMrvlHiuModuleConfig(ModuleShutDownBringUp, 
						   HostIfActive, 
						   HostIfType)); 
			}
			break;

		case 344: // Dut_Bt_SetMrvlRadioPerformanceMode
			{
				int		MrvlRxPowerMode=1;	 
 				int		PerfModeBdrRssiTh=0xB3;		
				int		PerfModeEdrRssiTh=0xB7;		 
				int		NormModeBdrRssiTh=0xB8;
				int		NormModeEdrRssiTh=0xBC;
				int		PerfModeBdrCrcTh=0x10;
				int		PerfModeEdrCrcTh=0x20;	 
				int		NormModeBdrCrcTh=0x20;	 
				int		NormModeEdrCrcTh=0x30;	 
				int		TxModeInTestMode=1;
				int		RxModeInTestMode=1;	 
				int		RxModeInOperationMode=1;	 
				int		RssiCrcSampleInterval=0xF0;
				int		LowPowerScanMode=0;

 				cnt = sscanf (line, "%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x",
						&cmd, 
						&MrvlRxPowerMode,	 
 						&PerfModeBdrRssiTh,		
						&PerfModeEdrRssiTh,		 
						&NormModeBdrRssiTh,	 
						&NormModeEdrRssiTh,	 
						&PerfModeBdrCrcTh,	 
						&PerfModeEdrCrcTh,	 
						&NormModeBdrCrcTh,		 
						&NormModeEdrCrcTh,		 
						&TxModeInTestMode,	 
						&RxModeInTestMode,	 
						&RxModeInOperationMode,	 
						&RssiCrcSampleInterval, 
						&LowPowerScanMode);

				DebugLogRite(" Dut_Bt_SetMrvlRadioPerformanceMode: 0x%08X\n", 
					Dut_Bt_SetMrvlRadioPerformanceMode(MrvlRxPowerMode,	 
 									PerfModeBdrRssiTh,		
									PerfModeEdrRssiTh,		 
									NormModeBdrRssiTh,	 
									NormModeEdrRssiTh,	 
									PerfModeBdrCrcTh,	 
									PerfModeEdrCrcTh,	 
									NormModeBdrCrcTh,		 
									NormModeEdrCrcTh,		 
									TxModeInTestMode,	 
									RxModeInTestMode,	 
									RxModeInOperationMode,	 
									RssiCrcSampleInterval, 
									LowPowerScanMode)); 
			}
			break;

		case 345:  // Dut_Bt_SetBtSleepMode
			{	
 				DebugLogRite(" Dut_Bt_SetMrvlArmDynamicClkCtrl: 0x%08X\n", 
					Dut_Bt_SetMrvlArmDynamicClkCtrl(parm1, parm2)); 
			}
			break;

// "Bt-Coex Mode"
		case 350: // Dut_Bt_SetBtXCompModeConfig
			{	int	BTCoexistenceMode=1;	 
 				int	PriorityMode_Wlan=0;
				int PriorityMode_UpdateDefInPta=0;
				int PriorityDelay=0;
				int HighPriority_BitMap=0xFFF;

 				cnt = sscanf (line, "%x %x %x %x %x %x",
						&cmd, 
						&BTCoexistenceMode,	 
 						&PriorityMode_Wlan,
						&PriorityMode_UpdateDefInPta,
						&PriorityDelay,
						&HighPriority_BitMap);

 				DebugLogRite(" Dut_Bt_SetBtXCompModeConfig: 0x%08X\n", 
					Dut_Bt_SetBtXCompModeConfig(BTCoexistenceMode,	 
 									PriorityMode_Wlan,
									PriorityMode_UpdateDefInPta,
									PriorityDelay,
									HighPriority_BitMap)); 
			}
			break;
 
		case 351: // Dut_Bt_SetBtMrvlModeConfig
			{	int	BTCoexistenceMode=1;	 
 				int	PriorityMode_Wlan=0;
				int PriorityMode_UpdateDefInPta=0;
				int PriorityDelay=0;
				int HighPriority_BitMap=0xFFF; 
				int MediumPriority_BitMap=0xFFF;
				int BtEndTimer=0;
				int BtFreqLowBand=0;
				int BtFreqHighBand=0;

 				cnt = sscanf (line, "%x %x %x %x %x %x %x %x %x %x",
						&cmd, 
						&BTCoexistenceMode,	 
 						&PriorityMode_Wlan,
						&PriorityMode_UpdateDefInPta,
						&PriorityDelay,
						&HighPriority_BitMap, 
						&MediumPriority_BitMap,
						&BtEndTimer,
						&BtFreqLowBand,
						&BtFreqHighBand);

 				DebugLogRite(" Dut_Bt_SetBtMrvlModeConfig: 0x%08X\n", 
					Dut_Bt_SetBtMrvlModeConfig(BTCoexistenceMode,	 
 									PriorityMode_Wlan,
									PriorityMode_UpdateDefInPta,
									PriorityDelay,
									HighPriority_BitMap, 
									MediumPriority_BitMap,
									BtEndTimer,
									BtFreqLowBand,
									BtFreqHighBand)); 
			}
			break;
 
		case 352: // Dut_Bt_SetBtMsiBtConf1ModeConfig
			{	int	BTCoexistenceMode=1;	 
 				int	NonPeriodicMode_Enable=0; 
				int NonPeriodicMode_BtTimerTlead=0xD;
				int NonPeriodicMode_BtTimerTtail=0x23;
				int PeriodicMode_BtSyncPeriod=0x12;
				int PeriodicMode_BtSyncPulseWidth=0x271;
				int PeriodicMode_BtTsyncLead=0x5;
				int PeriodicMode_BtSyncMode=1;

 				cnt = sscanf (line, "%x %x %x %x %x %x %x %x",
						&cmd, 
						&BTCoexistenceMode,	 
 						&NonPeriodicMode_Enable, 
						&NonPeriodicMode_BtTimerTlead,
						&NonPeriodicMode_BtTimerTtail,
						&PeriodicMode_BtSyncPeriod,
						&PeriodicMode_BtSyncPulseWidth,
						&PeriodicMode_BtTsyncLead );

 				DebugLogRite(" Dut_Bt_SetBtMrvlMode2Config: 0x%08X\n", 
					Dut_Bt_SetBtMrvlMode2Config(BTCoexistenceMode,	 
 									NonPeriodicMode_Enable, 
									NonPeriodicMode_BtTimerTlead,
									NonPeriodicMode_BtTimerTtail,
									PeriodicMode_BtSyncPeriod,
									PeriodicMode_BtSyncPulseWidth,
									PeriodicMode_BtTsyncLead,
									PeriodicMode_BtSyncMode)); 
			}
			break;
*/
#endif //#ifdef _HCI_PROTOCAL_
#ifdef _W8688_
		case 100:
		   DebugLogRite("Dut_Bt_SetPcmLoopBackMode: 0x%08X\n", 
			   Dut_Bt_SetPcmLoopBackMode(parm1, parm2));
			break;
#endif //#ifdef _W8688_

#if !defined(_NO_EEPROM_)
		case 989:
			{
//				short Mode=0;
				DebugLogRite("Dut_Bt_CheckCalDataSpace: 0x%08X\n", 
					Dut_Bt_CheckCalDataSpace((int*)&parm1));
				DebugLogRite("CheckCalDataSpace: 0x%08X\n",  parm1);
			 
			}
			break;
#endif // #if !defined(_NO_EEPROM_)
/* 
		case 992: // get accum
			{	
 				DebugLogRite(" Dut_Bt_GetAccum: 0x%08X\n", 
					Dut_Bt_GetAccum((int*)&parm2));
				DebugLogRite("Accum: %d\n",parm2);
			}
			break;


#ifdef _HCI_PROTOCAL_

		case 995:
			{
 
				DebugLogRite(" Dut_Bt_Hold: 0x%08X\n", 
					Dut_Bt_Hold(1, 
									0x8000, 
									0x4000));

			}
			break;
#endif //#ifdef _HCI_PROTOCAL_
*/
#ifndef _LINUX_
#if !defined(_NO_EEPROM_)
		case 999:
			DebugLogRite(" Dut_Bt_GetHeaderPresent: 0x%08X\n", 
					Dut_Bt_GetHeaderPresent((int*)&parm2));
			DebugLogRite("HeaderPresent: %d\n",parm2);
			break;
#endif // #if !defined(_NO_EEPROM_)
#endif   //#ifndef _LINUX_
		case 99:
			DebugLogRite("Exiting\n");
			break;

  		case -1:
			break;

		default:
			DebugLogRite("Unknown Option\n");
			break;
        }
	}


#ifndef _HIDE_MENU_
      if(99 != cmd )
		{
			DebugLogRite ("Press [Enter] key to continue ... ");
			fgets(line, sizeof(line), stdin);
		}
#endif //#ifndef _HIDE_MENU_


    } while (99 != cmd);



 



#if !defined(_BT_SDIO_) // && !defined(_WIFI_SDIO_) 
	status = Dut_Bt_CloseDevice(theObj);
#endif //#if !defined(_BT_SDIO_) // && !defined(_WIFI_SDIO_) 


	return (status);

}


 
 
int mainBT(int argc, char* argv[])
{
	int OptionMode=0;

	if(argc ==1) 
	{
		OptionMode = ALL; // basic basic RF 
	}
	else
	{
		if(!strcmp(argv[1], "-r")) OptionMode = EXT_RF; // extended RF 
		if(!strcmp(argv[1], "-m")) OptionMode = BASIC_SPI; // basic basic RF and Flash
		if(!strcmp(argv[1], "-a")) OptionMode = ALL; // basic basic RF and Flash
		if(!strcmp(argv[1], "-s")) OptionMode = SUPERUSER; // basic basic RF and Flash
	}
	
	BTMenu(OptionMode);

	return 0;
}
