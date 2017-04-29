/** @file w878X_Wifi.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#define _HIDE_MENU_
#define _DEVELOPMENT_PHASE_

#if defined (_WLAN_)
#include <time.h>
#include "DutWlanApi.hc"
#include "DutBtApi.hc"
#include "../DutCommon/Clss_os.h"
#include "../DutCommon/Dut_error.hc"
#include "../DutCommon/utilities.h"
#include "DutAppVerNo.h"
#include "DutApi878XDll.h"
#include "DutWlanApi878XDll.h"
#include "DutFuncs.h"
#include "W878XApp.h"

#ifdef _HIDE_MENU_
#include "WlanMenu.h"
#endif //#ifdef _HIDE_MENU_

extern int LoadSaveCalData(	int DeviceType=0,
 							char *FlexFileName=NULL,
							char *WlanCalFlexFileName=NULL
#if defined (_MIMO_)
							,
							char *WlanCalFlexPwrTrgtFileFormat=NULL					
#endif // #if defined (_MIMO_)
							);

extern int UpLoadSaveCalData(	int *CsC,
								int DeviceType=0,
								char *FlexFileName=NULL, 
 								char *WlanCalFlexFileName=NULL
#if defined (_MIMO_)
					,
					char *WlanCalFlexPwrTrgtFileFormat=NULL	
#endif // #if defined (_MIMO_)
					);

int DUTAPI_LoadSaveCalData()
{
	int status=0;
	status = LoadSaveCalData(0);
	if (status) return status;

	status = DutIf_ReloadCalData();

	return status;
}

int DUTAPI_UpLoadSaveCalData(int *CsC)
{  
	int status=0;

	status = UpLoadSaveCalData(CsC, 0) ;

	return status;
}

void SetSubPidVid(void) 
{
	char line[MAX_LENGTH]="";
	static DWORD SubPid=0x4500, SubVid=0x1385; 
	static DWORD ClassID=0x020000; 
	WORD ReadbackSubPid=0, ReadbackSubVid=0; 
	DWORD ReadbackClassID= 0; 

	DutIf_GetPIDVID(0x40, (WORD*)&SubPid, (WORD*)&SubVid);
	DutIf_GetClassId(0x34, &ClassID);

	DebugLogRite("^^ Please enter  new Class ID(0x%06X) ", ClassID);
	fgets(line, sizeof(line), stdin);
	sscanf (line, "%x",   &ClassID);

	DebugLogRite("^^ Please enter  new sub PID(0x%04X) ", SubPid);
	fgets(line, sizeof(line), stdin);
	sscanf (line, "%x",   &SubPid);

	DebugLogRite("^^ Please enter  new sub VID(0x%04X) ", SubVid);
	fgets(line, sizeof(line), stdin);
	sscanf (line, "%x",   &SubVid);

	DutIf_SetPIDVID(0x40, (WORD)SubPid, (WORD)SubVid);
	DutIf_SetClassId(0x34, ClassID);

	DutIf_GetPIDVID(0x40, &ReadbackSubPid, &ReadbackSubVid);
	DutIf_GetClassId(0x34, &ReadbackClassID);

	if(ClassID == ReadbackClassID )
		DebugLogRite(" ClassID programming verified\n"); 
	else
		DebugLogRite(" ClassID programming failed to verify\n"); 

	if(SubPid == ReadbackSubPid && SubVid == ReadbackSubVid )
		DebugLogRite(" SubPID and subVID programming verified\n"); 
	else
		DebugLogRite(" SubPID and subVID programming failed to verify\n"); 
	return;
}

void GetSubPidVid(void)
{
 	WORD ReadbackSubPid=0, ReadbackSubVid=0; 
	DWORD ClassId=0; 

 	DutIf_GetPIDVID(0x30, &ReadbackSubPid, &ReadbackSubVid);

 	DebugLogRite(" PID    0x%04X and VID    0x%04X \n", ReadbackSubPid, ReadbackSubVid); 
 
	DutIf_GetPIDVID(0x40, &ReadbackSubPid, &ReadbackSubVid);
 	DebugLogRite(" SubPID 0x%04X and SubVID 0x%04X \n", ReadbackSubPid, ReadbackSubVid); 

	DutIf_GetClassId(0x34, &ClassId);
 	DebugLogRite(" ClassId 0x%06X \n", ClassId, ReadbackSubVid); 
}

void itob(BYTE in, char *out)
{
    char temp[8]={0};
    int remain;
    int i = 0;
    remain = in;
    for(i=0; i<8; i++)
    {
        temp[i] = remain%2;
        remain /= 2;
        if(remain==0) break;
    }
    sprintf(out,"%d%d%d%d%d%d%d%d",temp[7],temp[6],temp[5],temp[4],temp[3],temp[2],temp[1],temp[0]);
}

int WifiMenu(int OptionMode, void** theObj=0)
{
 	int				status=0;
 	unsigned long	cmd=0, parm1=0, parm2=0, cnt=0;
 	unsigned long	dparm1=0, dparm2=0;
	BOOL			extern_LDO = 0;
	int				lnaMode = 0;

	char line[MAX_LENGTH]="";
	char INIFileName[_MAX_PATH]="";

	int vgcCtrl=0; 
	int BandAG=0;

   // find all network adapters in registry and fill them in device list
 	DebugLogRite("%s\n", DutIf_About());

 	status = DutIf_InitConnection(theObj);
	DebugLogRite(" DutIf_InitConnection: %d\n", status);
	if (status)
	{
        return (0);
	}
#if 0
	if(getcwd(INIFileName, _MAX_PATH) != NULL)  
		strcat(INIFileName, "//SetUp.ini");
	else
		strcpy(INIFileName, "SetUp.ini");
#else
	strcpy(INIFileName, "/etc/labtool/SetUp.ini");
#endif

	GetPrivateProfileString("HW_CONFIG", "LNA_MODE", "0x0", line, MAX_LENGTH, INIFileName);
	sscanf (line, "0x%x", &lnaMode);

	//DebugLogRite(" LNA mode: 0x%08x\n", lnaMode);

	DutIf_SetExtLnaNsettings(lnaMode);


#if defined (_INIT_FIX_)
	DutIf_WriteMACReg(0x300, 0xA); //enable receiver, disable BSSID filter
	DutIf_SetRfChannel(6); // workaround, force to use channel setting
#endif

#ifdef _HIDE_MENU_
		DisplayMenu(WlanCmdNames, 0);
#endif //#ifdef _HIDE_MENU_
 
	do
    {	
#ifndef _HIDE_MENU_
		status = DutIf_GetBandAG(&BandAG);

	   DebugLogRite ("------------------------------\n"); 
#if defined( _UDP_) || defined(_LINUX_)
#if defined(_W8782_)
 	   DebugLogRite ("8782 (802.11a/g/b/n) TEST MENU\n");
#else
       DebugLogRite ("8787 (802.11a/g/b/n) TEST MENU\n");
#endif //#ifdef _W8782
#elif defined(_IF_SDIO_)
       DebugLogRite ("SD8787 (802.11a/g/b/n) TEST MENU\n");
#elif defined(_IF_USB_)
	   DebugLogRite ("USB8787  TEST MENU\n");
#else
#if defined(_W8782_)
 	   DebugLogRite ("W8782 (802.11a/g/b/n) TEST MENU\n");
#else
       DebugLogRite ("W8787 (802.11a/g/b/n) TEST MENU\n");
#endif //#ifdef _W8782_
#endif	//#if defined( _UDP_) || defined(_LINUX_)

	   DebugLogRite ("------------------------------\n"); 
 
		if(BASIC_SPI !=OptionMode) 
		{
			DebugLogRite (" 11. Get RF Channel\n");
 			if(!status && BAND_802_11G == BandAG)
				DebugLogRite ("\t(2.4GHz Channels: 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, "
									"11, 12, 13, 14(JP))\n");
			else
			{
				int i, newline = CHANNEL_NUM_A/3+1;
				char line1[MAX_LENGTH]="";
				sprintf(line1, "\t(5GHz Channels: \n\t");
				for (i = 0; i<CHANNEL_NUM_A;i++)
				{					
					sprintf(line1, "%s %3d,", line1, aChannelList[i]);
					if ((newline-1)==i || (2*newline-1)==i || (3*newline-1)==i)
						sprintf(line1, "%s\n\t", line1);
				}
				line1[strlen(line1)-1]='\0';
				DebugLogRite("%s)\n", line1);
			}
			DebugLogRite (" 12. Set RF Channel (decimal)\n");
			DebugLogRite (" 13. Get RF DataRate (rate) \n"
				"\t(1 for  1M;  2 for  2M;  3 for 5.5M; 4 for 11M,  5 for 22M, \n"
#if defined  _CLIENT_BG_ || defined  _CLIENT_ABG_ 
				"\t 6 for  6M;  7 for  9M;  8 for 12M;  9 for 18M, 10 for 24M, \n" 
				"\t11 for 36M, 12 for 48M, 13 for 54M, 14 for 72M)\n"
#endif //#if defined  _CLIENT_BG_ || defined  _CLIENT_ABG_ 
			);
//		DebugLogRite (" 13. Get RF DataRate (1 for 1M; 2 for 2M; 3 for 5.5M; 4 for 11M, 5 for 22M)\n");
// hide		DebugLogRite (" 14. Set RF DataRate (1 for 1M; 2 for 2M; 3 for 5.5M; 4 for 11M, 5 for 22M)\n");
			DebugLogRite (" 14. Get RF DataRate (rate) \n"
				"\t(1 for  1M;  2 for  2M;  3 for 5.5M; 4 for 11M,  5 for 22M, \n"
#if defined  _CLIENT_BG_ || defined  _CLIENT_ABG_ 
				"\t 6 for  6M;  7 for  9M;  8 for 12M;  9 for 18M, 10 for 24M, \n" 
				"\t11 for 36M, 12 for 48M, 13 for 54M, 14 for 72M)\n"
#endif //#if defined  _CLIENT_BG_ || defined  _CLIENT_ABG_ 
			);
//		DebugLogRite (" 15. Get Tx Power setting at PA (targetPower)\n"
//			"If given targetPower, result will include correction from target to setting\n");
 		DebugLogRite (" 15. Get Tx Power setting\n");
//		if(vgcCtrl)
//		{
// 		DebugLogRite (" 16. Set Tx Power (pwr=0(dec) )\n");
		DebugLogRite (" 16. Set Tx Power at PA (hex) (rfregb8_value) (Use 15 to Get)\n");
//		}
//		else
//		{
//		DebugLogRite (" 16. Set Tx Power at PA (decimal)\n");
//		DebugLogRite (" 16. Set Tx Power at PA (decimal decimal decimal) (targetPower Corr1B/1C Corr1A) (Use 15 to Get)\n");
//		}
		DebugLogRite (" 17. Set Continuous Tx Mode \n\t(enable datarate (pattern=0) (carrier_supresssion_mode=0)) )\n");
 		DebugLogRite (" 18. Set CW Tx Mode (enable)\n");
 		DebugLogRite (" 19. Set Carrier Suppression Tx Mode(enable)\n");
#ifndef _DEVELOPMENT_PHASE_
  		DebugLogRite (" 20. Get Beacon period\n");
  		DebugLogRite (" 21. Set Beacon period (0x)\n");
#endif // #ifndef _DEVELOPMENT_PHASE_

#ifndef _DEVELOPMENT_PHASE_
		DebugLogRite (" 22. Set Power at Antenna Using Cal data (decimal decimal [modulation=0]) (ch pow [b =0, g=1])\n");
#endif // #ifndef _DEVELOPMENT_PHASE_
//  		DebugLogRite (" 23. Get Beacon Mode\n");
//  		DebugLogRite (" 24. Set Beacon Mode (enable)\n");
  		DebugLogRite (" 25. Set DutyCycle Tx Mode \n\t(enable dataRate "
						"((payloadweight =50)(pattern=0)(shortPreamble=0))\n");
  		DebugLogRite ("\tpattern: specify the byte length pattern."
			"or -1 as PN9\n");

#ifndef _DEVELOPMENT_PHASE_
   		DebugLogRite (" 28. enter Deep-Sleep mode\n");
#endif // #ifndef _DEVELOPMENT_PHASE_

#ifndef _DEVELOPMENT_PHASE_
		DebugLogRite (" 29. Get RF Band (0 for 2.4G, 1 for 5G)\n");
		DebugLogRite (" 30. Set RF Band (band (0 for 2.4G, 1 for 5G))\n");
#endif // #ifndef _DEVELOPMENT_PHASE_

		DebugLogRite (" 31. Clear received packet Count (Start Rx FER test)\n");
 		DebugLogRite (" 32. Get received packet Count (Stop Rx FER test)\n");
		DebugLogRite (" 33. Tx MultiCast Packet "
			"(0x)(len=400) (0x)(Count=64) (rate=4) \n"
#if !defined( _W8782_)
			"\t(pattern=0xAA) (shortPreamble=1) (bssid-xx.xx.xx.xx.xx.xx)\n");
#else
			"\t(pattern=0xAA) (shortPreamble=1) (bssid-xx.xx.xx.xx.xx.xx) (DPD: 1 for Ebable,0 for disable) (PowerID)\n");
#endif 
		DebugLogRite (" 34. Enable BSSID filter (enable bssid-xx.xx.xx.xx.xx.xx ssid-string)\n");
		}
 
		//if(BASIC_RF !=OptionMode && EXT_RF !=OptionMode ) 
		{
 //		DebugLogRite (" 37. SPI build-in header download (Use 38 to verify it)\n");
 //		DebugLogRite (" 38. SPI header Verify\n");

			if(!Dut_Shared_SupportNoE2PROM())
			{
				DebugLogRite (" 39. SPI download from sbin file.\n");
				DebugLogRite (" 40. SPI Verify from sbin file\n");
				DebugLogRite (" 41. Dump E2PROM content\n");
				DebugLogRite (" 42. Get E2P Type Info \n(Interface (1=SPI,2=I2C), Addrees Len(byte), Type (1=EEPROM,2=FLASH))\n");
				DebugLogRite (" 43. Set E2P Type Info \n(Interface (1=SPI,2=I2C), Addrees Len(byte), Type (1=EEPROM,2=FLASH))\n");
			}
				
			if (!Dut_Shared_SupportNoE2PROM())
			{
 			DebugLogRite (" 45. Read MACAddress \n");
 			DebugLogRite (" 46. Write MACAddress (xx.xx.xx.xx.xx.xx)\n");
			}

#ifndef _DEVELOPMENT_PHASE_
		if (!Dut_Shared_SupportNoE2PROM())
		{
#if defined(_IF_USB_) 
 			DebugLogRite (" 47. Get USB Attribute \n");
 			DebugLogRite (" 48. Set USB Attribute ((0x)(Attribute(8-bit)) (MaxPower=0(0 - 250)))\n");
#endif //#if defined(_IF_USB_) 

		}

		if (!Dut_Shared_SupportNoE2PROM())
		{
			DebugLogRite (" 49. Set PID/VID/ClassID.\n");
			DebugLogRite (" 50. Get PID/VID/ClassID.\n");

#ifdef _FLASH_
			DebugLogRite (" 51. Erase flash		(0x )(section)\n");
#endif //#ifdef _FLASH_
	//		DebugLogRite (" 52. about flash \n");
			DebugLogRite (" 53. Write CalTable (from file:CalDataFile.txt)\n");
			DebugLogRite (" 54. Read CalTable from EEPROM\n");
	//		DebugLogRite (" 57. Read HW Information \n");
		}
#endif // #ifndef _DEVELOPMENT_PHASE_

		}

#ifndef _DEVELOPMENT_PHASE_
		if(SUPERUSER ==OptionMode) 
#endif // #ifndef _DEVELOPMENT_PHASE_
		{
		DebugLogRite (" 60. Read MAC Reg		(0x)(addr)\n");
		DebugLogRite (" 61. Write MAC Reg	(0x 0x)(addr data)\n");
		DebugLogRite (" 62. Read BBP Reg		(0x)(addr)\n");
		DebugLogRite (" 63. Write BBP Reg	(0x 0x)(addr data)\n");
		DebugLogRite (" 64. Read RF Reg		(0x)(addr )\n");
		DebugLogRite (" 65. Write RF Reg		(0x 0x)(addr data)\n");
 	
 		DebugLogRite (" 66. Read CAU Reg		(0x)(addr )\n");
		DebugLogRite (" 67. Write CAU Reg		(0x 0x)(addr data)\n");
  
		DebugLogRite (" 68. Peek SOC Mem		(0x)(addr)\n");
		DebugLogRite (" 69. Poke SOC Mem		(0x 0x)(addr data)\n");
		if (!Dut_Shared_SupportNoE2PROM())
		{
	 		DebugLogRite (" 70. Peek SPI		(0x)(addr)\n");
	 		DebugLogRite (" 71. Poke SPI		(0x 0x)(addr data)\n");
		}
		DebugLogRite ("188. Get on Line Release Note\n");
		}

#ifndef _DEVELOPMENT_PHASE_
		DebugLogRite (" 72. Change Boot Mode to Normal Mode \n");
 		DebugLogRite (" 73. Change Boot Mode to DFU Mode\n");
// 		DebugLogRite (" 74. Set Power Range Table in FW (Use 73 to Get) \n");

// 		DebugLogRite (" 75. Set Configuration Settings from EEPROM to FW\n");
//		DebugLogRite (" 76. Get Power Table from FW\n");
//		DebugLogRite (" 77. Set Power Table in FW (Use 76 to Get)\n");
//		DebugLogRite (" 78. TestStart (password) \n");
//		DebugLogRite (" 79. TestCmplt (exit UDP mode) \n");
#endif // #ifndef _DEVELOPMENT_PHASE_
 
		DebugLogRite (" 88. Get FW and HW Versions\n");

#ifndef _DEVELOPMENT_PHASE_
		//DebugLogRite (" 89. Load Dut configuration file (FileName)\n");
//		DebugLogRite (" 91. Get Rf Vga control (extMode setting)\n");
//		DebugLogRite (" 92. Set Rf Vga control (extMode(0|1) setting(6 bits)) (Use 91  to Get)\n");
//		DebugLogRite (" 93. Get Rf Lc Cap control (extMode setting)\n");
//		DebugLogRite (" 94. Set Rf Lc Cap control (extMode setting(3 bits)) (Use 93 to Get)\n");
//		DebugLogRite (" 95. Start RssiNf Test\n");
//		DebugLogRite (" 96. Get RssiNf Test result (stopTest(0|1))\n");
		DebugLogRite (" 95. Get Rf XTAL control\n");
		DebugLogRite (" 96. Set Rf XTAL control (setting(8 bits))\n");
		DebugLogRite (" 97. HW SoftReset\n");
#endif // #ifndef _DEVELOPMENT_PHASE_

		DebugLogRite ("111. Get Channel BW (0: 20MHz, 1: 40MHz, 4: 10MHz, 5: 5MHz.\n\t\t2 and 3 are invalid for this chip)\n");
		DebugLogRite ("112. Set Channel BW\n");

#ifndef _DEVELOPMENT_PHASE_
		DebugLogRite ("117. Tx IQ Cal Mode(enable(1|0)))\n");
		DebugLogRite ("118. Get Tx IQ setting\n");
		DebugLogRite ("119. Set Tx IQ setting (extMode (0x)amp (0x)phase) amp and phase are 7 bits signed value)\n");

		if(SUPERUSER ==OptionMode)  // TBD
		{
//		DebugLogRite ("120. Get IQ Estimated value\n");
		DebugLogRite ("130. Perform Tx/Rx IQ Cal (avg(10))\n");
		DebugLogRite ("131. Get Rx IQ setting\n");
		DebugLogRite ("132. Set Rx IQ setting (extMode (0x)amp (0x)phase) amp and phase are 7 bits signed value)\n");
//		DebugLogRite ("133. Get Rx IQ Estimated value\n");
		}
#endif // #ifndef _DEVELOPMENT_PHASE_
		DebugLogRite ("121. Thermal Compensation (enable(0|1) [ SlopNum(26) SlopNum1(13) SlopDenom(5) TempAtCal(0x)(70) TempSlope(0x)(70) CalInMs(50) PTargetDelta(0x)(20)])\n");
		
		DebugLogRite ("140. Download RFU FW\n");
		DebugLogRite ("211. Get a Channel's Freq information (band chan)  \n"); 
		DebugLogRite ("212. Set Channel by Freq (in MHz)  \n"); 

#ifdef _ENGINEERING_ 
  		DebugLogRite ("970. RunBbpSqDiag internal only ((0x)sizeOfBufByte(F000))\n");
#endif //#ifdef _ENGINEERING_

		DebugLogRite (" 99. Exit\n"); 
#ifndef _DEVELOPMENT_PHASE_
		DebugLogRite ("101. Get Rf PA Control Mode (0= VGA|1 =ThermalOnly)\n"); 
		DebugLogRite ("102. Set Rf PA Control Mode (0= VGA|1 =ThermalOnly) (Use 101  to Get)\n"); 

		DebugLogRite ("122. Set BCA Config(mode(0-get|1-set), setting(0x))\n");

#endif // #ifndef _DEVELOPMENT_PHASE_
#endif //#ifndef _HIDE_MENU_

		cmd = -1;
		parm1 = 0;
        parm2 = 0;
		dparm1 = 0;
        dparm2 = 0;

        DebugLogRite ("Enter option: ");

		fgets(line, sizeof(line), stdin);

		// print the cmd in Test.txt file
		PrintOnScreen(0);
		DebugLogRite("%s\n", line);
        sscanf (line, "%d %x %x",&cmd, &parm1, &parm2);
        sscanf (line, "%d %d %d",&cmd, &dparm1, &dparm2);

// 		DebugLogRite("%s\n", line);
#ifdef _HIDE_MENU_
	if( strstr(line, "-h") || strstr(line, "?"))// || strlen(line)==1)
	{
		DisplayMenu(WlanCmdNames, cmd);
	}
	else
#endif //#ifdef _HIDE_MENU_
        switch (cmd)
        {
#ifndef _LINUX_
 		if(BASIC_SPI !=OptionMode) 
#endif
		{
#if defined(_W8782_)

		case 9: // Get Tx Ant
			{
				int antSelection = 0;
				status = DutIf_GetTxAntenna(&antSelection, 0);
 			    DebugLogRite(" DutIf_GetTxAntenna: 0x%08X\n", status);
			    DebugLogRite(" Tx Antenna: %d\n", antSelection);
			}
			break;
		case 10:  // Set Tx Ant
			{
				int antSelection = 0;
				sscanf (line, "%d %d",&cmd, &antSelection);
				status = DutIf_SetTxAntenna(antSelection, 0);
 				DebugLogRite(" DutIf_SetTxAntenna: 0x%08X\n", status);
			}
			break;
#endif //#if defined(_W8782_)

		case 11: // Get Tx Channel
			{
				double chFreqinMhz=0;
  		        status = DutIf_GetRfChannel((int*)&parm1, &chFreqinMhz);
 			    DebugLogRite(" DutIf_GetRfChannel: 0x%08X\n", status);
			    DebugLogRite(" RF Channel: %d (%.1lf MHz)\n", parm1, chFreqinMhz);
			}
            break;
            
        case 12: // Set Tx Channel
			sscanf (line, "%d %d",&cmd, &parm1);
			status = DutIf_SetRfChannel(parm1);
 			DebugLogRite(" DutIf_SetRfChannel: 0x%08X\n", status);
            break;
            
        case 13: // Set continuous Tx
			{
				int txDataRate=0; 
				status = DutIf_GetTxDataRate(&txDataRate);
				DebugLogRite(" DutIf_GetTxDataRate: 0x%08X\n", status);
				DebugLogRite(" DataRate:%d\n", (txDataRate>=0)? txDataRate+1:txDataRate);
			}
            break;
            
        case 14: // Set Tx Data Rate
			status = DutIf_SetTxDataRate(dparm1-1);
			DebugLogRite(" DutIf_SetTxDataRate: 0x%08X\n", status); 
            break;
/*
        case 15: // Get Tx Power
			{ 
				int pwr=0, Th_G=0, initP_G=0, Th_B=0, initP_B=0;
				BYTE PaConf1=0, PaConf2=0; 
				BYTE Vt_Hi_G=0, Vt_Lo_G=0, InitP_G=0;
				BYTE Vt_Hi_B=0, Vt_Lo_B=0, InitP_B=0;

				status = DutIf_GetRfPowerSetting(&PaConf1, &PaConf2, 
								&Vt_Hi_G, &Vt_Lo_G,	&InitP_G, 
								&Vt_Hi_B, &Vt_Lo_B,	&InitP_B);
				DebugLogRite(" DutIf_GetRfPowerSetting: 0x%08X\n", status); 
//  				DebugLogRite(" PaConf1 0x%02X\tPaConf2 0x%02X \n", PaConf1, PaConf2);
  				DebugLogRite(" InitP_G 0x%02X\tVt_Hi_G 0x%02X\tVt_Lo_G 0x%02X\n", 
										InitP_G, Vt_Hi_G, Vt_Lo_G);
			
				status = DutIf_GetRfPower(&pwr, &initP_G, &initP_B, &Th_G, &Th_B);
				DebugLogRite(" DutIf_GetRfPower: 0x%08X\n", status); 
				DebugLogRite(" pwr %d\tinitP_G 0x%02X\tTh_G 0x%02X\n", 
								pwr, initP_G, Th_G);
			}
          break;
*/
/*
		case 16: // Set Tx Power
			{
				int pwr=0, Th_G=0, initP_G=0, Th_B=0, initP_B=0;
				cnt = sscanf (line, "%d %d %x  %x ",
					&cmd, &pwr, &initP_G,  &initP_B, &Th_G,  &Th_B ); 

				status = DutIf_SetRfPower(&pwr, &Th_G, &initP_G);
				DebugLogRite(" DutIf_SetRfPower: 0x%08X\n", status); 
			}
            break;
*/    
		case 17: // Set Tx Cont Mode
			{
 				int pattern=0;
				int CSmode=0;
				cnt = sscanf (line, "%d %d %d  %x %d",
					&cmd, &parm1, &parm2,  &pattern, &CSmode); 
				if(0 == parm2) parm2 =4; 
				if(parm1) 
				DebugLogRite(" DutIf_SetTxDataRate: 0x%08X\n", 
						DutIf_SetTxDataRate(parm2-1)); 
				DebugLogRite(" DutIf_SetTxContMode: 0x%08X\n", 
						DutIf_SetTxContMode(parm1, 0, pattern, CSmode));
			}
            break;
        case 18: // Set Tx CW Mode
			status = DutIf_SetTxContMode(parm1,1);
 			DebugLogRite(" DutIf_SetTxContMode: 0x%08X\n", status);
            break;

        case 19: // Set Carrier Suppresion
			status = DutIf_SetTxCarrierSuppression(parm1);
 			DebugLogRite(" DutIf_SetTxCarrierSuppression: 0x%08X\n", status);
            break;

		case 20: // Set Carrier Suppresion
			status = DutIf_GetBeaconInterval(&parm1);
 			DebugLogRite(" DutIf_GetBeaconInterval: 0x%08X\n", status);
  			DebugLogRite(" Beacon Interval: 0x%X\n", parm1);
           break;

        case 21: // Set Carrier Suppresion
			status = DutIf_SetBeaconInterval(parm1);
 			DebugLogRite(" DutIf_SetBeaconInterval: 0x%08X\n", status);
            break;

 		case 111:	// Set WL_ACTIVE / TX_CONF line
			{
  				int bw=0;
 				DebugLogRite("DutIf_GetChannelBw: 0x%08X\n", 
					DutIf_GetChannelBw(&bw));
 				DebugLogRite("Channel Bw: %d\n", bw); 
			}
			break;

		case 112:	// Set WL_ACTIVE / TX_CONF line
			{
 				DebugLogRite("DutIf_SetChannelBw: 0x%08X\n", 
					DutIf_SetChannelBw(parm1));
			}
			break;

        case 211: 
        {
            double              FreqInMHz = 0.0;

            cnt = sscanf(line, "%*d %d %d", &parm1, &parm2);

            DebugLogRite("DutIf_GetRfChannelFreq: 0x%08X\n",
                         DutIf_GetRfChannelFreq(parm1, parm2, &FreqInMHz));

            DebugLogRite("Channel %d in Band %d is %.1lf MHz\n", parm2, parm1, FreqInMHz);
        }
            break;

        case 212: 
        {
            double      FreqInMHz = 0.0;

            cnt = sscanf (line, "%*d %lf", &FreqInMHz);

            DebugLogRite("DutIf_SetRfChannelByFreq: 0x%08X\n",
                         DutIf_SetRfChannelByFreq(FreqInMHz)); 
        }
            break;
      
      case 117: // Set Tx IQ Cal Mode
			status = DutIf_SetTxIQCalMode(parm1);
 			DebugLogRite(" DutIf_SetTxIQCalMode: 0x%08X\n", status);
            break;

	  
	  case 118: // Get Tx IQ setting
			{
				DWORD amp=0, phase=0;
				status = DutIf_GetTxIQValue(NULL, &amp, &phase);
 				DebugLogRite(" DutIf_GetTxIQValue: 0x%08X\n", status);
 				DebugLogRite(" Amp 0x%02X Phase 0x%02X\n", 
					amp, phase);
			}
            break;
       
		case 119: // Set Tx IQ Cal Mode
			{
				DWORD amp=0, phase=0;
				sscanf (line, "%d %x %x", 
					&cmd, &amp, &phase);

				status = DutIf_SetTxIQValue(0, amp, phase);
 				DebugLogRite(" DutIf_SetTxIQValue: 0x%08X\n", status);
			}
            break;

		case 130: // Set Tx IQ Cal Mode
			{
				int avg=10, skipRxIq=0;	//skipTxIq=0, 
 				DWORD TxAmp=0, TxPhase=0;
				DWORD RxAmp=0, RxPhase=0;
				int repeat=1;
		         
				sscanf (line, "%d %d %d %d",&cmd, &avg, &skipRxIq, &repeat);

//				if(skipTxIq==1) 
//				{
//					int mode=0;
//					status = DutIf_GetTxIQValue(&mode, &TxAmp, &TxPhase);
//				}
 				
				// skip happens only when the Tx IQ alpha and Beta both zero. 
				// use cmd 119 to set value. 
				if (avg <1) avg=1;
				if (avg > 10) avg=10;
#ifdef _DEBUG
				{
				int i=0;
				if(repeat<0) repeat=1;
				for(i=0;i<repeat;i++)
				{
					DebugLogRite("%d\t", i+1);
					DutIf_WriteBBReg(0x3be,0);
					DutIf_WriteBBReg(0x3bf,0);
					DutIf_WriteBBReg(0x3c0,0);
					DutIf_WriteBBReg(0x3c1,0);
					DutIf_WriteBBReg(0x3c4,0);
					DutIf_WriteBBReg(0x3c5,0);

#endif
				if (skipRxIq)
					status = DutIf_PerformTxRxIQCal(avg, 
												&TxAmp, &TxPhase, 
												NULL, NULL);
				else
				    status = DutIf_PerformTxRxIQCal(avg, 
												&TxAmp, &TxPhase, 
												&RxAmp, &RxPhase);

				if (status)
				    DebugLogRite(" DutIf_PerformRxIQCal: 0x%08X\n", status);
				else
				{
					DebugLogRite("Tx: Amp 0x%02X Phase 0x%02X\t", 
								TxAmp&0xff, TxPhase&0xff);
					if (skipRxIq)
						DebugLogRite("\n");
					else
	 					DebugLogRite("Rx: Amp 0x%02X Phase 0x%02X\n", 
								RxAmp&0xff, RxPhase&0xff);
				}
#ifdef _DEBUG
				}
				}
#endif
			}
            break;
        
		case 131: // Get Rx IQ setting
			{
				DWORD amp=0, phase=0;
				status = DutIf_GetRxIQ(NULL, &amp, &phase);
 				DebugLogRite(" DutIf_GetRxIQ: 0x%08X\n", status);
 				DebugLogRite(" Amp 0x%02X Phase 0x%02X\n", 
					amp, phase);
			}
            break;
       
		case 132: // Set Rx IQ Cal Mode
			{
				DWORD amp=0, phase=0;
				sscanf (line, "%d %x %x", 
					&cmd, &amp, &phase);

				status = DutIf_SetRxIQ(0, amp, phase);
 				DebugLogRite(" DutIf_SetRxIQ: 0x%08X\n", status);
			}
            break;

 /*       case 133: // Get Tx IQ estimated setting
			{
				DWORD amp=0, phase=0;
				status = DutIf_GetRxIQEstimated(&amp, &phase);
 				DebugLogRite(" DutIf_GetRxIQEstimated: 0x%08X\n", status);
 				DebugLogRite(" Amp 0x%08X Phase 0x%08X\n", 
								amp, phase);
			}
            break;
*/


		case 22:	//set rf power using cal data
			{
 				int Pwr4Pa=0;
				BOOL rateG=0;
				int totalLen=0;

				FILE* hFile;
				char temp[MAX_LENGTH]="WlanCalData_ext.conf";
				int i =0xf0;
				DWORD j=0;
				char temp1[4]="";

				cnt = sscanf (line, "%d %d %d %d %d",&cmd, &parm1, &Pwr4Pa, &rateG);
 				if(3 > cnt) Pwr4Pa= 15;
 		        status = DutIf_SetRfChannel(parm1);
 				DebugLogRite(" DutIf_SetRfChannel: 0x%X\n", status);
				if (Dut_Shared_SupportNoE2PROM())
				{
					//read the cal data content from file
					int CalDataLoaded=0;
					status = DutIf_GetCustomizedSettings(&CalDataLoaded);
					if(CalDataLoaded)
					{
						status = DutIf_SetRfPowerUseCal(&Pwr4Pa, rateG);
					}
					else
					{
						hFile = fopen(temp,"rb"); 
						if (!hFile)
						{
							DebugLogRite("File %s cannot be found!\n", temp);
							break;
						}

						fclose (hFile);
						status = DutIf_SetCustomizedSettings(temp);
						status = DutIf_SetRfPowerUseCal(&Pwr4Pa, rateG);
					}
			}
				else
				{
					status = DutIf_SetRfPowerUseCal(&Pwr4Pa, rateG);
				}
				DebugLogRite(" DutIf_SetRfPowerCal: 0x%X\n", status);
				break;
			}

		case 25: // Duty Cycle
			{
				int loss=0, tuning=0, payloadweight=50;
				int pattern=0, shortPreamble=0;
#if defined (_MIMO_)
				int ActSubCh=3, ShortGI=0, AdvCoding=0;
				int	TxBfOn = 0, GFMode=0, STBC=0;
				DWORD DPDEnable =0, PowerID=0;
				DWORD		SignalBw=-1;
#endif // #if defined (_MIMO_)
				cnt = sscanf (line, "%d %d %d %d %x %d"
#if defined (_MIMO_)
					" %d %d %d %d %d %d %d %d %d"
#endif // #if defined (_MIMO_)
					,
					&cmd, &parm1, &parm2, &payloadweight, &pattern, &shortPreamble
#if defined (_MIMO_)
					, 
					&ActSubCh, &ShortGI, &AdvCoding, &TxBfOn, &GFMode, &STBC, 
					&DPDEnable, &PowerID, &SignalBw 
#endif // #if defined (_MIMO_)
					); 
				if(0 == parm2) parm2 =4; 

 				status = DutIf_SetTxDutyCycle(parm1, parm2-1, payloadweight, 
									pattern, shortPreamble
#if defined (_MIMO_)	
									,
									ActSubCh, ShortGI, AdvCoding, TxBfOn, 
									GFMode, STBC, DPDEnable, PowerID, SignalBw
#endif // #if defined (_MIMO_)									
									);
 				DebugLogRite(" DutIf_SetTxDutyCycle: 0x%08X\n", status);
			}
			break;

		case 27:
			{ WORD calXosc=0;
				cnt = sscanf (line, "%d %d",&cmd, &parm1); 
				if(1 == cnt) parm1 =10;
				status = DutIf_XoscCal(&calXosc, parm1);
				DebugLogRite(" DutIf_XoscCal: 0x%08X \n", status);
				DebugLogRite("Calibration Data for %d TU: 0x%04X (%d)\n", 
				parm1, calXosc, calXosc);
			}
			break;       

		case 28:
			{   
				status = DutIf_EnterSleepMode();
				DebugLogRite(" DutIf_EnterSleepMode: 0x%X \n", status);
			}
			break;

		case 29:
			{   
				status = DutIf_GetBandAG((int*)&parm1);
				DebugLogRite(" DutIf_GetModeAG: 0x%X : %d (%s)\n", 
					status, parm1, parm1?"5GHz":"2.4GHz");
			}
			break;

		case 30:
			{   
				status = DutIf_SetBandAG( parm1);
				DebugLogRite(" DutIf_SetModeAG: 0x%X \n", status);
			}
			break;

		case 31: // Set continuous Tx
			status = DutIf_ClearRxPckts();
 			DebugLogRite(" DutIf_ClearRxPckts: 0x%08X\n", status);
            break;

		case 32: // Set continuous Tx
		   {
			DWORD CntOk=0, CntKo=0; 
 			status = DutIf_GetRxPckts(&cnt, &CntOk, &CntKo);
			DebugLogRite(" DutIf_GetRxPckt: 0x%08X\n", status);
			DebugLogRite(" GetRxPckt: \n \tRx Packet %d\n \tMulti Cast %d\n\tErr Count %d\n", 
				cnt, CntOk, CntKo);
			//if((CntKo+ CntOk) > 0) 
			//	DebugLogRite(" Rx FER based on detected packets: %8.2f%%\n", 100.0*CntKo/(CntKo+ CntOk));
		   }
            break;

		case 33: // Set continuous Tx
		   { 
		   		DWORD rate=4, pattern=0xAAAAAAAA, len=0x400, count=100, shortPreamble=1;
				int cnt=0; 
				char Bssid[6]="";
				int tempIntArray[6]={0}; 
#if defined (_MIMO_)
				DWORD DPDEnable =0, PowerID=0;
				int ActSubCh=3, ShortGI=0, AdvCoding=0;
				int	TxBfOn = 0, GFMode=0, STBC=0;
#endif // #if defined (_MIMO_)

					cnt = sscanf (line, "%d %x %x %d %x %d"
#if defined (_MIMO_)
					"%d %d %d %d %d %d %d %d"
#if defined (_W8787_) || 	defined (_W8782_)						
					"%d %d" 
#endif //#if defined (_W8787_) || 	defined (_W8782_)						
#endif // #if defined (_MIMO_)
					"%x.%x.%x.%x.%x.%x ",
					&cmd, &parm1, &parm2, &rate, &pattern, &shortPreamble,
#if defined (_MIMO_)
					 
					&ActSubCh, &ShortGI, &AdvCoding, &TxBfOn, &GFMode, &STBC, 
					&DPDEnable, &PowerID,  
#endif // #if defined (_MIMO_)
				&tempIntArray[0], &tempIntArray[1], &tempIntArray[2], 
				&tempIntArray[3], &tempIntArray[4], &tempIntArray[5]
				);


				if(cnt == 20) 
				{
				Bssid[0] = tempIntArray[0];
				Bssid[1] = tempIntArray[1];
				Bssid[2] = tempIntArray[2]; 
				Bssid[3] = tempIntArray[3];
				Bssid[4] = tempIntArray[4];
				Bssid[5] = tempIntArray[5];
				}
				else
				{
				Bssid[0] = (char)0xFF;
				Bssid[1] = (char)0xFF;
				Bssid[2] = (char)0xFF; 
				Bssid[3] = (char)0xFF;
				Bssid[4] = (char)0xFF;
				Bssid[5] = (char)0xFF;
				}
				len = (0 == parm1) ? 0x400:parm1; 
				count = (0 == parm2) ? 100:parm2; 

			status = DutIf_TxBrdCstPkts(rate-1, pattern, len, count, shortPreamble, Bssid
#if defined (_MIMO_)
										,
										ActSubCh, 
										ShortGI, 
										AdvCoding,  
										TxBfOn,  
										GFMode, 
										STBC,
										DPDEnable,//DPDEnable=0,
										PowerID //PowerID=0,
#endif // #if defined (_MIMO_)
										);
			DebugLogRite(" DutCtrl.TxBrdCstPkts: 0x%08X\n", status);
			}
            break;

#if defined (_W8787_) || defined (_W8782_)						
		case 35: // Set continuous Tx
		   { 
		   		DWORD rate=4, pattern=0xAAAAAAAA, len=0x400, enable=0, shortPreamble=1;
				int cnt=0; 
				char Bssid[6]="";
				int tempIntArray[6]={0}; 
#if defined (_MIMO_)
				DWORD DPDEnable =0, PowerID=0;
				int ActSubCh=3, ShortGI=0, AdvCoding=0;
				int	TxBfOn = 0, GFMode=0, STBC=0;
				DWORD	AdjustTxBurstGap=false,  BurstSifsInUs=0; 
				DWORD	SignalBw=-1;
#endif // #if defined (_MIMO_)

				cnt = sscanf (line, "%d %d %d"
#if defined (_W8787_) || defined (_W8782_)						
						"%d %d"
#endif //#if defined (_W8787_) || defined (_W8782_)						
					" %x %x %d"
#if defined (_MIMO_)
					"%d %d %d %d %d %d %d %d %d" 
#endif // #if defined (_MIMO_)
					"%x.%x.%x.%x.%x.%x ",
					&cmd, &enable, &rate, 
#if defined (_W8787_) || defined (_W8782_)						
					&AdjustTxBurstGap, &BurstSifsInUs, 
#endif //#if defined (_W8787_) || defined (_W8782_)						
					&len, &pattern, &shortPreamble,
#if defined (_MIMO_)
					&ActSubCh, &ShortGI, &AdvCoding, &TxBfOn, &GFMode, &STBC, 
					&DPDEnable, &PowerID,  &SignalBw,

#endif // #if defined (_MIMO_)
    				&tempIntArray[0], &tempIntArray[1], &tempIntArray[2], 
    				&tempIntArray[3], &tempIntArray[4], &tempIntArray[5]);

				if(cnt == 23) 
				{
    				Bssid[0] = tempIntArray[0];
	    			Bssid[1] = tempIntArray[1];
		    		Bssid[2] = tempIntArray[2]; 
			    	Bssid[3] = tempIntArray[3];
				    Bssid[4] = tempIntArray[4];
    				Bssid[5] = tempIntArray[5];
				}
				else
				{
	    			Bssid[0] = (char)0xFF;
		    		Bssid[1] = (char)0xFF;
			    	Bssid[2] = (char)0xFF; 
				    Bssid[3] = (char)0xFF;
				    Bssid[4] = (char)0xFF;
				    Bssid[5] = (char)0xFF;
				}
//				len = (0 == parm1) ? 0x400:parm1; 
//				count = (0 == parm2) ? 100:parm2; 

			    status = DutIf_AdjustPcktSifs(enable, rate-1, pattern, len,  shortPreamble, Bssid
#if defined (_MIMO_)
										,
										ActSubCh, 
										ShortGI, 
										AdvCoding,  
										TxBfOn,  
										GFMode, 
										STBC,
										DPDEnable,	//DPDEnable=0,
										PowerID,	//PowerID=0,
										SignalBw
									, 	AdjustTxBurstGap,	BurstSifsInUs
#endif // #if defined (_MIMO_)
										);
			    DebugLogRite(" DutIf_AdjustPcktSifs: 0x%08X\n", status);
			}
            break;
#endif // #if defined (_W8787_) || 	defined (_W8782_)						

		case 34: // enable BSSID filter
		   { 
				int cnt=0;
				char Ssid[50]="";
				BYTE Bssid[6]="";
				int tempIntArray[6]={0}; 
 				cnt = sscanf (line, "%d %x %x.%x.%x.%x.%x.%x %s",&cmd, &parm1,
				&tempIntArray[0], &tempIntArray[1], &tempIntArray[2], 
				&tempIntArray[3], &tempIntArray[4], &tempIntArray[5], 
				Ssid);

				if(cnt == 8) 
				{
					Bssid[0] = tempIntArray[0];
					Bssid[1] = tempIntArray[1];
					Bssid[2] = tempIntArray[2]; 
					Bssid[3] = tempIntArray[3];
					Bssid[4] = tempIntArray[4];
					Bssid[5] = tempIntArray[5];
				}
				else
				{
					Bssid[0] = 0xFF;
					Bssid[1] = 0xFF;
					Bssid[2] = 0xFF; 
					Bssid[3] = 0xFF;
					Bssid[4] = 0xFF;
					Bssid[5] = 0xFF;
				}
			status = DutIf_EnableBssidFilter(parm1, Bssid, Ssid);
			DebugLogRite("DutCtrl.EnableBssidFilter: 0x%08X\n", status);
			}
            break;
		}

#ifndef _FLASH_
		case 39:
		{
			if (Dut_Shared_SupportNoE2PROM())
			{
				DebugLogRite("Unknown Option\n");
			}
			else
			{			
				DutIf_FlexSpiDL(true, "");
				DebugLogRite(" DutIf_FlexSpiDL: 0x%08X\n", status);
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
				DutIf_FlexSpiCompare(""); 
			}
		}
			break;
 
		case 41: // Dump Flash
			{
				if (Dut_Shared_SupportNoE2PROM())
				{
					DebugLogRite("Unsupported Option\n");
				}
				else
				{
					int index=0, len=0;
					DWORD address=0,*addArray=NULL, *readback=0;
					len = (0 == parm2) ? 0x10:parm2;
					address = parm1;

					addArray=(DWORD*)malloc(len*sizeof(DWORD)); 
					readback=(DWORD*)malloc(len*sizeof(DWORD)); 
					memset(addArray,0,len*sizeof(DWORD));
					memset(readback,0,len*sizeof(DWORD));
					status = DutIf_DumpE2PData(address, len, addArray, readback);
					DebugLogRite(" DutIf_DumpE2PData 0x%08X\n",  status);
					for (index =0; index < len; index ++)
					{
 						DebugLogRite(" 0x%08X\t:\t0x%08X \n", 
									addArray[index], readback[index] );
					}	
					free(addArray);
					free(readback);
				}
			}
            break;

		case 42: // Get E2P Type
			{
				DWORD IfType;
				DWORD AddrWidth; 
				DWORD DeviceType;

				status = DutIf_GetE2PromType(&IfType, &AddrWidth, &DeviceType);
				DebugLogRite(" DutIf_GetE2PromType 0x%08X, Interface=%d, Address Len=%d, Device Type=%d \n", status, IfType, AddrWidth, DeviceType);
			}
			break;

		case 43: // Set E2P Type
			{
				DWORD temp;
				DWORD IfType;
				DWORD AddrWidth; 
				DWORD DeviceType;

				sscanf (line, "%d %d %d %d",&temp, &IfType, &AddrWidth, &DeviceType);

				status = DutIf_ForceE2PromType(IfType, AddrWidth, DeviceType);
				DebugLogRite(" DutIf_ForceE2PromType 0x%08X\n", status);
			}
			break;

		if(EXT_RF !=OptionMode) 
		{
		case 45: // Get Mac Address
			{
				if (Dut_Shared_SupportNoE2PROM())
				{
					DebugLogRite("Unknown Option\n");
				}
				else
				{
 
					BYTE tempMacAddr[6]={0,0,0,0,0,0}, fromMacReg=0;
					int cnt=0;
					cnt = sscanf (line, "%d", &cmd); 
 					status = DutIf_GetMACAddress(tempMacAddr);
					DebugLogRite(" DutIf_GetMACAddress: 0x%08X\n", status);
					DebugLogRite(" DutIf_GetMACAddress: %02x.%02x.%02x.%02x.%02x.%02x\n", 
						tempMacAddr[0], tempMacAddr[1], tempMacAddr[2], 
						tempMacAddr[3], tempMacAddr[4], tempMacAddr[5]);
				}
			}
            break;
            
		case 46: // Set Mac Address
			{
				if (Dut_Shared_SupportNoE2PROM())
				{
					DebugLogRite("Unknown Option\n");
				}
				else
				{
 					BYTE tempMacAddr[6]={0,0,0,0,0,0};
					int tempInt[6]={0}, cnt=0; 
					cnt = sscanf (line, "%d %x.%x.%x.%x.%x.%x",&cmd, 
						&tempInt[0], &tempInt[1], &tempInt[2],
						&tempInt[3], &tempInt[4], &tempInt[5]);
					if(cnt < 7) 
					{
						DebugLogRite("Invalid MAC Address\n"); 
						break;
					}
					tempMacAddr[0]= tempInt[0];
					tempMacAddr[1]= tempInt[1];
					tempMacAddr[2]= tempInt[2];
					tempMacAddr[3]= tempInt[3];
					tempMacAddr[4]= tempInt[4];
					tempMacAddr[5]= tempInt[5];

					status = DutIf_SetMACAddress(tempMacAddr);
					DebugLogRite(" DutIf_SetMACAddress: 0x%08X\n", status);
				}
			}
            break; 
#if defined(_IF_USB_) 
		case 47: // Set continuous Tx
			if (Dut_Shared_SupportNoE2PROM())
			{
				DebugLogRite("Unknown Option\n");
			}
			else
			{
				DebugLogRite(" DutIf_GetUsbAttributes: 0x%08X\n", 
						DutIf_GetUsbAttributes((BYTE*)&parm1, (BYTE*)&parm2));
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

				DebugLogRite(" DutIf_SetUsbAttributes: 0x%08X\n", 
						DutIf_SetUsbAttributes(Attribute, MaxPower));
			}
 			break;
#endif // #if defined (_IF_USB_) 
		
			case 49:
			{	
				GetSubPidVid();
				break;
			}
			case 50:
			{	
				SetSubPidVid();
				break;
			}

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
				}
				else
				{
					DutIf_FlexSpiDL(true, "");
					DebugLogRite(" DutIf_FlexSpiDL: 0x%08X\n", status);
				}
			}
			break;
#endif //#ifndef _FLASH_

			case 53: // Download Cal Data
 			{
				status = DUTAPI_LoadSaveCalData(); 
				DebugLogRite(" DUTAPI_LoadSaveCalData: 0x%08X\n", 
						status);
			}
            break;

       case 54: // Upload Cal Data
			{ 
				BOOL	 CsC=0;

				status = DUTAPI_UpLoadSaveCalData((int*)&CsC); 
				DebugLogRite(" DUTAPI_UpLoadSaveCalData: 0x%08X \tCsC:%x\n", 
						status, CsC);
			}
            break;
		}

		if(BASIC_SPI !=OptionMode && BASIC_RF !=OptionMode) 
		{	
		case 60:  
			{ 
				int index =0, cnt=0;
				DWORD address=0; 
				if (parm1 < 0x1000) parm1 =0xA000+parm1; 
				cnt = parm2 ? parm2:1;
				DebugLogRite(" DutIf_ReadMACReg:  \naddr\t\tdata\t\t status\n", status);
				for (index =0; index <cnt; index++)
				{
					address = parm1+index*sizeof(DWORD);
					status = DutIf_ReadMACReg( address, &parm2); 
					DebugLogRite("0x%08X\t0x%08X\t:0x%08X\n",address, parm2, status);
				}
			}
            break;
        
		case 61:  
			if (parm1 < 0x1000) parm1 =0xA000+parm1; 
			status = DutIf_WriteMACReg( parm1, parm2);
			DebugLogRite(" DutIf_WriteMACReg: 0x%08X\n", status);
            break;
		
		case 62: 
			{ 
				int index =0, cnt=0;
				int address=0;
				BYTE reading=0; 
				DebugLogRite(" DutIf_ReadBBReg:\naddr\tdata\t status\n");
				cnt = parm2 ? parm2:1;

				for (index =0; index <cnt; index++)
				{
					address = (parm1&0xFFFF)+index*sizeof(BYTE);
					status = DutIf_ReadBBReg(address, &reading);
 					DebugLogRite("0x%04X\t0x%02X \t:0x%08X\n",address, reading, status);
				}
			}
            break;

        case 63:  
			status = DutIf_WriteBBReg((WORD)(parm1&0xFFFF), (BYTE)(parm2&0xFF));
			DebugLogRite(" DutIf_WriteBBReg: 0x%08X\n", status);
            break;

		case 64:  
			{ 
				int index =0, cnt=0;
				int address=0; 
                BYTE reading=0; 
                BOOL binaryFlag=0;
                char binaryVal[10]={0};
                sscanf (line, "%d %x %x %d",&cmd, &parm1, &cnt, &binaryFlag);
                if (cnt==0) cnt=1;

                if (binaryFlag==1)
					DebugLogRite(" DutIf_ReadRfReg:\naddr\tdata\t\t status\n");
				else
					DebugLogRite(" DutIf_ReadRfReg:\naddr\tdata\t status\n");
				for (index =0; index <cnt; index++)
				{
					address = (int)((parm1)+index*sizeof(BYTE));
					status = DutIf_ReadRfReg(address, &reading);
                    if (binaryFlag==1)
                    {
                        itob(reading, binaryVal);
                        DebugLogRite("0x%04X\t0x%02X(%08s)\t:0x%08X\t\n",address, reading, binaryVal, status);
                    }
                    else
                    {
                        DebugLogRite("0x%04X\t0x%02X\t:0x%08X\t\n",address, reading, status);
                    }
				}
			}
            break;

        case 65:  
			status = DutIf_WriteRfReg((int)(parm1),(BYTE)(parm2&0xFF));
			DebugLogRite(" DutIf_WriteRfReg: 0x%08X\n", status);
            break;

		case 66:  
			{ 
				int index =0, cnt=0;
				int address=0; 
                BYTE reading=0; 
				DebugLogRite(" DutIf_ReadCAUReg:\naddr\tdata\t status\n");
				cnt = parm2 ? parm2:1;

				for (index =0; index <cnt; index++)
				{
					address = (int)((parm1)+index*sizeof(BYTE));
					status = DutIf_ReadCAUReg(address, &reading);
 					DebugLogRite("0x%04X\t0x%02X\t:0x%08X\t\n",address, reading, status);
				}
			}
            break;

        case 67:  
			status = DutIf_WriteCAUReg((int)(parm1),(BYTE)(parm2&0xFF));
			DebugLogRite(" DutIf_WriteCAUReg: 0x%08X\n", status);
            break;

		case 140:
			{
				char FileName[MAX_LENGTH]="";

				parm1 =0;
				cnt = sscanf (line, "%d %s %d",&cmd, FileName, &parm1);

				status = DutIf_LoadRFUFW(0, FileName);
				DebugLogRite("DutIf_LoadRFUFW: 0x%X\n", status);
			
				if (!status && parm1 == 0)
				{
 					status = DutIf_UpdateRFUFW(0);
					DebugLogRite("DutIf_UpdateRFUFW: 0x%X\n", status);
				}
				break;
			}

		case 141:
			{
				status = DutIf_UpdateRFUFW(0);
				DebugLogRite("DutIf_UpdateRFUFW: 0x%X\n", status);
			
				break;
			}
		}

		if(SUPERUSER ==OptionMode) 
		{
		case 68: // Set continuous Tx
		{ 
			int index =0, cnt=0, repeat=1, intervalInS=1, loop=0;
			DWORD address=0; 
 			cnt = parm2 ? parm2:1;
			sscanf(line, "%d %x %x %d %d", &cmd, &parm1, &parm2, &repeat, &intervalInS);
			DebugLogRite(" DutIf_PeekRegDword:  \naddr\t\tdata\t\t status\n", status);
			for(loop=0; loop<repeat;loop++)
			{
				for (index =0; index <cnt; index++)
				{
					address = parm1+index*sizeof(DWORD);
					status = DutIf_PeekRegDword( address, &parm2); 
					DebugLogRite("0x%08X\t0x%08X\t:0x%08X\n",address, parm2, status);
				}
				DebugLogRite("\n");
				if (repeat>1) Sleep(intervalInS*1000);
			}
			break;
		}
        
        case 69:  
 			status = DutIf_PokeRegDword(parm1, parm2);
			DebugLogRite(" DutIf_Poke: 0x%08X\n", status);
            break;
         
		case 70: // Read EEPROM 
 			status = DutIf_DumpE2PData(parm1, 1, &parm1, &parm2);
 			DebugLogRite(" DutIf_DumpE2PData: 0x%08X\n", status);
  			DebugLogRite(" Data: 0x%08X\n", parm2);
           break;
         
		case 71: // Write EEPROM
 			status = DutIf_WriteLumpedData(parm1, 1, &parm2);
 			DebugLogRite(" DutIf_WriteLumpedData: 0x%08X\n", status);
             break;
         
 		case 174: // pwr tbl
		   { 
			   			int i=0;  
			int HighFreqBondary=0, LowFreqBondary=0;
			int PpaGain1_Rng[RFPWRRANGE_NUM]={0}; 
			int PpaGain2_Rng[RFPWRRANGE_NUM]={0};  
			int MaxPwr_Rng[RFPWRRANGE_NUM]={0}; 
			int MinPwr_Rng[RFPWRRANGE_NUM]={0}; 
			int SizeOfPwrTblInByte=200;
			BYTE PwrTbl[200]; 

 //DutIf_API int STDCALL DutIf_UpLoadPwrTable( );

// TBD_KY 				status = Dut_Shared_ReadPwrTableFile(parm1,  parm2, 
// TBD_KY 					&HighFreqBondary, &LowFreqBondary, 
// TBD_KY 					PpaGain1_Rng, PpaGain2_Rng, MaxPwr_Rng, MinPwr_Rng,
// TBD_KY 					&SizeOfPwrTblInByte,  PwrTbl);
 				DebugLogRite(" Dut_Shared_ReadPwrTableFile: 0x%08X\n", status);
				if(status) break;
  
// TBD_KY 				status = DutIf_DownLoadPwrTable( 
// TBD_KY 						parm1, parm2,   
// TBD_KY 						HighFreqBondary,	LowFreqBondary,
// TBD_KY 						PpaGain1_Rng, 
// TBD_KY 						PpaGain2_Rng,  
// TBD_KY 						MaxPwr_Rng, 
// TBD_KY 						MinPwr_Rng, 
// TBD_KY 						SizeOfPwrTblInByte, PwrTbl);
 				DebugLogRite(" DutIf_DownLoadPwrTable: 0x%08X\n", status);

				DebugLogRite("SizeOfPwrTblInByte %d\n", 
				 SizeOfPwrTblInByte);

				DebugLogRite("HighFreqBondary %d\nLowFreqBondary %d\n", 
				 HighFreqBondary, LowFreqBondary);

				DebugLogRite("PpaGain1\t PpaGain2\t MaxPwr\t MinPwr\n" );
				for (i=0; i<RFPWRRANGE_NUM; i++)
				{ 
					DebugLogRite("0x%X\t 0x%X\t 0x%X\t 0x%X\n", 
						PpaGain1_Rng[i], PpaGain2_Rng[i], MaxPwr_Rng[i], MinPwr_Rng[i]);
				}
			
				{
					DebugLogRite("Power\t ThHi\t ThLo\t InitP\t\n" );
					for (i=0; i<SizeOfPwrTblInByte; i+=4)
					{ 
						DebugLogRite("0x%X\t 0x%X\t 0x%X\t 0x%X\n", 
							PwrTbl[i], PwrTbl[i+1], PwrTbl[i+2], PwrTbl[i+3]);
					}
				}
 
				DebugLogRite("\n\n"); 
		   }

		case 173: // Set continuous Tx
		   { 
 			    
			int i=0;  
			int HighFreqBondary=0, LowFreqBondary=0;
			int PpaGain1_Rng[RFPWRRANGE_NUM]={0}; 
			int PpaGain2_Rng[RFPWRRANGE_NUM]={0};  
			int MaxPwr_Rng[RFPWRRANGE_NUM]={0}; 
			int MinPwr_Rng[RFPWRRANGE_NUM]={0}; 
			int SizeOfPwrTblInByte=200;
			BYTE PwrTbl[200]; 

// TBD_KY 				status = DutIf_UpLoadPwrTable(parm1,  parm2, 
// TBD_KY 					&HighFreqBondary, &LowFreqBondary, 
// TBD_KY 					PpaGain1_Rng, PpaGain2_Rng, MaxPwr_Rng, MinPwr_Rng,
// TBD_KY 					&SizeOfPwrTblInByte,  PwrTbl);
				DebugLogRite(" DutIf_UpLoadPwrTable: 0x%08X\n", status);


// TBD_KY 				Dut_Shared_WritePwrTableFile( 
// TBD_KY 						parm1, parm2,   
// TBD_KY 						HighFreqBondary,	LowFreqBondary,
// TBD_KY 						PpaGain1_Rng, 
// TBD_KY 						PpaGain2_Rng,  
// TBD_KY 						MaxPwr_Rng, 
// TBD_KY 						MinPwr_Rng, 
// TBD_KY 					  &SizeOfPwrTblInByte, PwrTbl);
 
				DebugLogRite("SizeOfPwrTblInByte %d\n", 
								SizeOfPwrTblInByte);

				DebugLogRite("HighFreqBondary %d\nLowFreqBondary %d\n", 
				 HighFreqBondary, LowFreqBondary);

				DebugLogRite("PpaGain1\t PpaGain2\t MaxPwr\t MinPwr\n" );
				for (i=0; i<RFPWRRANGE_NUM; i++)
				{ 
					DebugLogRite("0x%X\t 0x%X\t 0x%X\t 0x%X\n", 
						PpaGain1_Rng[i], PpaGain2_Rng[i], MaxPwr_Rng[i], MinPwr_Rng[i]);
				}
			
				{
					DebugLogRite("Power\t ThHi\t ThLo\t InitP\t\n" );
					for (i=0; i<SizeOfPwrTblInByte; i+=4)
					{ 
						DebugLogRite("0x%X\t 0x%X\t 0x%X\t 0x%X\n", 
							PwrTbl[i], PwrTbl[i+1], PwrTbl[i+2], PwrTbl[i+3]);
					}
				}
 
				DebugLogRite("\n\n");
				 
			}
            break;
		 
 		 case 188: // Get Online Release Note
	        {
				char  releaseNote[2000]={0};
 			    status = DutIf_GetReleaseNote(releaseNote);
 			    DebugLogRite(" DutIf_GetReleaseNote: 0x%08X\n%s\n", status, releaseNote);
		    }
            break;
		}

        case 88: // Get FW & HW Version
			{  
			BYTE FwVersion[80]="";
			WORD socId=0;
			BYTE rfId=0, bbpId=0;
			BYTE socVersion=0, rfVersion=0, bbpVersion=0;
			BYTE OR_RF_Major, OR_RF_Minor, OR_RF_Cust;
			BYTE OR_SOC_Major, OR_SOC_Minor, OR_SOC_Cust;
			DWORD version=DutIf_DllVersion();
 
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

			status = DutIf_GetFWVersion(FwVersion);
			if(status)
				DebugLogRite(" DutIf_GetFWVersion: 0x%08X \n", status);
			else
				DebugLogRite("%s\n", FwVersion);

			status = DutIf_GetHWVersion(&socId, &socVersion, 
										&rfId, &rfVersion, 
										&bbpId, &bbpVersion);
			if (status) 
				DebugLogRite(" DutIf_GetHWVersion: 0x%08X \n",status);
			else
				DebugLogRite("SOC:\t%04X \t%02X\nBBP:\t%02X \t%02X\nRF:\t%02X \t%02X\n", 
					socId, socVersion, bbpId, bbpVersion, rfId, rfVersion);

			status = DutIf_GetORVersion(OR_ID_RF, &OR_RF_Major, &OR_RF_Minor, &OR_RF_Cust);
			status = DutIf_GetORVersion(OR_ID_SOC, &OR_SOC_Major, &OR_SOC_Minor, &OR_SOC_Cust);
			DebugLogRite("RF OR Version: \t%x.%x\t Customer ID:\t%x\n", OR_RF_Major, OR_RF_Minor, OR_RF_Cust);
			DebugLogRite("SOC OR Version:\t%x.%x\t Customer ID:\t%x\n", OR_SOC_Major, OR_SOC_Minor, OR_SOC_Cust);
			}
            break;

        case 89: // load configuration file
			{  
				char FileName[MAX_LENGTH]="";
				cnt = sscanf (line, "%d %s",&cmd, FileName);
				if(strlen(FileName))
				DebugLogRite("LoadInitConfigFile [%s]: %08X\n", FileName, LoadInitConfigFile(FileName));
				else
				DebugLogRite("File Name not specified\n"); 

			}
            break;           

		case 95:	// get RF crystal oscillator
			status = DutIf_GetRfXTal ((int *) &parm2, (int *) &parm1);
			DebugLogRite ("DutIf_GetRfXTal: %d\n", status); 
			DebugLogRite (" XTal: %02X\n", parm1); 
			break;			

		case 96:	// set RF crystal oscillator
			status = DutIf_SetRfXTal (1, parm1);
			DebugLogRite ("DutIf_SetRfXTal: %d\n", status); 
			break;			

		case 97:	// rf reset
			status = DutIf_HwResetRf ();
			DebugLogRite ("DutIf_HwResetRf: %d\n", status); 
			break;	 		

		case 101:	// get rf control mode
			status = DutIf_GetRfControlMode (&parm1);
			DebugLogRite ("DutIf_GetRfControlMode: %d\n", status); 
			DebugLogRite ("RfControlMode: %d\n", parm1); 
			break;			

		case 102:	// set rf control mode
			status = DutIf_SetRfControlMode (parm1);
			DebugLogRite ("DutIf_SetRfControlMode: %d\n", status); 
			break;			

		case 99:
			DebugLogRite("Exiting\n");
			break;

		case 296:
			{
			 int setting=0;
				DebugLogRite(" DutIf_StepRfPower: 0x%X\n", 
							DutIf_StepRfPower((int*)parm2
#if defined (_MIMO_)
							, parm1
#endif //#if defined (_MIMO_)
							)); 
			}
			break;	

		case 297:
			{ 	int loopIndex, count, rssi, SNR, NoiseFloor; 	 
				DebugLogRite("DutIf_SetRssiNf: %08X\n",	DutIf_SetRssiNf()); 
				
			for (loopIndex=0; loopIndex < (int)parm1; loopIndex++)
			{
#ifndef _LINUX_
				Sleep(50);
#endif
				DebugLogRite("DutIf_GetRssiNf: %08X\n",	DutIf_GetRssiNf(parm2,
					&count, &rssi, &SNR, &NoiseFloor)); 
				DebugLogRite("count\t: %d \t RSSI\t: %d \t SNR\t: %d \t NoiseFloor\t: %d \n",	 
					count, rssi, SNR, NoiseFloor ); 
			}
			}
			break;
#if defined(_W8787_)
		case 197:
			{
				char RSSI_Calc=0;
				status = DutIf_GetCalculatedRSSI(&RSSI_Calc);
				DebugLogRite("DutIf_GetCalculatedRSSI: %08X\n",	status); 
				DebugLogRite("DutIf_GetCalculatedRSSI: RSSI =%d\n", RSSI_Calc);
			}
			break;
		case 198:
		case 298:
			{
				int RSSI_ch=0, Band=0;
				char RSSI_val=0, NF_val=0;
				DWORD DeviceId=0;
				double chFreqinMhz=0;
  		        status = DutIf_GetRfChannel(&RSSI_ch, &chFreqinMhz);
				status = DutIf_GetBandAG((int*)&Band);
				status = DutIf_WlanRSSI_CalOneCh_CW(RSSI_ch, &RSSI_val, &NF_val, Band, DeviceId);
				DebugLogRite("DutIf_WlanRSSI_CalOneCh_CW: RSSI_val=0x%X, NF_val=0x%X\n", RSSI_val, NF_val);
			}
			break;
		case 199:
		case 299:
			{
				int RSSI_ch=0, Band=0;
				char RSSI_val=0, NF_val=0;
				DWORD DeviceId=0;
				double chFreqinMhz=0;
  		        status = DutIf_GetRfChannel(&RSSI_ch, &chFreqinMhz);
				status = DutIf_GetBandAG((int*)&Band);
				status = DutIf_WlanRSSI_CalOneCh_Pkt(RSSI_ch, &RSSI_val, &NF_val, Band, DeviceId);
				DebugLogRite("DutIf_WlanRSSI_CalOneCh_Pkt: RSSI_val=0x%X, NF_val=0x%X\n", RSSI_val, NF_val);
			}
			break;
#endif //#if defined(_W8787_)
		case 121:
			{
				int cnt;
				int enabled=0;

				DWORD tempRefAtCal=0x70, tempRefSlope=0x70;
				DWORD SlopeNumerator=26, SlopeNumerator1=13, SlopeDenominator=5;
				DWORD PTargetDelta=0x20, CalibrationIntervalInMS=500;

				cnt = sscanf (line, "%d %d %d %d %d %x %x %d %x",
					&cmd, &enabled, &SlopeNumerator, &SlopeNumerator1, &SlopeDenominator, 
					&tempRefAtCal, &tempRefSlope, &CalibrationIntervalInMS, &PTargetDelta);
				DebugLogRite(	"DutIf_ThermalComp: 0x%08X\n", 
								DutIf_ThermalComp(	enabled, 
													tempRefAtCal,
													tempRefSlope,
													SlopeNumerator, 
													SlopeNumerator1, 
													SlopeDenominator,
													CalibrationIntervalInMS, 
													PTargetDelta));
			}
			break;

		case 221:
			{
				int cnt;
				int enabled=0;
				char FileName[256] = "ThermTest.txt";
				DWORD PTarget=50, tempRef=0x70, SlopeNumerator=148, SlopeDenominator=30, CalibrationIntervalInMS=500;
				DWORD Temp=0x70, SlopeNumerator1=148, SlopeDenominator1=30;
			    cnt = sscanf (line, "%d %d %x %x %d %d %d %x %d %d",
								&cmd, &enabled, 
								&PTarget, &tempRef, 
								&SlopeNumerator, &SlopeDenominator, 
								&CalibrationIntervalInMS,
								&Temp, 
								&SlopeNumerator1, &SlopeDenominator1);

				if (cnt==1)
				{
					DebugLogRite("221 enable(1-start, 0 stop) pTargetInit(0x) TempRefAtCal(0x) Numerator Denominator interval TempRefSlope(0x) Numerator1 Denominator1\n");
					DebugLogRite("221 1 will get the parammeters from default file \"ThermTest.txt\"\n");
					DebugLogRite("221 1 filename will get the parammeters from default file specified\n");
					break;
				}

				if (cnt==2)
				{
					cnt = sscanf(line, "%d %d %s", &cmd, &enabled, FileName);

					if(!strlen(FileName))
					{
						DebugLogRite("File Name not specified\n"); 
						break;
					}

					LoadThermalTestFile(FileName, &PTarget, &tempRef,
							&SlopeNumerator, &SlopeDenominator,
							&CalibrationIntervalInMS,
							&Temp,
							&SlopeNumerator1, &SlopeDenominator1);
				}

				if (SlopeNumerator1==0) SlopeNumerator1=SlopeNumerator;
				if (SlopeDenominator1==0) SlopeDenominator1=SlopeDenominator;

				DebugLogRite("DutIf_ThermalTest: 0x%08X\n", DutIf_ThermalTest(enabled, PTarget, tempRef,
							SlopeNumerator, SlopeDenominator,
							CalibrationIntervalInMS,
							Temp,
							SlopeNumerator1, SlopeDenominator1));

			}
			break;

#if defined (_SUPPORT_LDO_)
		case 123:
			{
				if (extern_LDO)
				{
					DebugLogRite("DutIf_SetLDOConfig: 0x%08X\n", DutIf_SetLDOConfig(parm1));
				}
			}
			break;
		case 124:
			{
				if (extern_LDO)
				{
					int LDOSrc=0;
					DebugLogRite("DutIf_GetLDOConfig: 0x%08X\n", DutIf_GetLDOConfig(&LDOSrc));
					DebugLogRite("Current LDO Setting: 0x%X\n", LDOSrc);
				}
			}
			break;
#endif // #if defined (_SUPPORT_LDO_)

#if defined (_SUPPORT_PM_)
		case 125:	//set PM Reg
			{
				DebugLogRite("DutIf_WritePMReg: 0x%08X\n", DutIf_WritePMReg(parm1, parm2));
			}
			break;
 		case 126:	//get PM Reg
			{
				int regVal;

				if (parm2 <1) parm2=1;
				
				DebugLogRite("DutIf_ReadPMReg\n");
				for (int i=0; i<parm2;i++)
				{
					status = DutIf_ReadPMReg(parm1+i, &regVal);
					DebugLogRite(" 0x%02X:0x%08X 0x%02X\n", parm1+i, status, regVal);
				}
			}
			break;
		case 127:
			{
				int railId, railMode, railVolt, railPowerDownMode;
 				cnt = sscanf (line, "%d %d %d %d %d",
					&cmd, &railId, &railMode, &railVolt, &railPowerDownMode );
				DebugLogRite("DutIf_SetPMRail: 0x%08X\n", DutIf_SetPMRail(railId, railMode, railVolt, railPowerDownMode));
			}
			break;
#endif // #if defined (_SUPPORT_PM_)

		case 122:
			{
				int status = 0;
				DWORD FE_VER=0x00, FEMCap2G=0x01, FEMCap5G=0x0, FEM_INTERNAL_BT=0x01;
				DWORD FEM_ANT_DIV=0x0, FEM_EXTERNAL_2G_LNA=0x0,FEM_EXTERNAL_5G_LNA=0x0;
				DWORD FEM_SAMEANT_2G_BT=0x01, FEM_SAMEANT_5G_BT=0x0, CONCURRENT_CAP_2G_BT=0x01;
				DWORD CONCURRENT_CAP_5G_BT=0x00;
				DWORD AntDiversityInfo = 0x00;
				BYTE Front_End_2G[9], Front_End_5G[9];
				char FileName[_MAX_PATH]="setup.ini";
				char session[_MAX_PATH];
				int i=0;

				//setup.ini
				DutIf_GetFEMFileName(FileName);
				
				if (FALSE == IsFileExist(FileName))
				{
					DebugLogRite("\nCannot Find FEM FileName [%s]\n", FileName);
					DebugLogRite("Please make sure this file exist before continoue.\n\n");
					status = ERROR_ERROR;
					break;
				}
				else
					DebugLogRite("FEM FileName is \n  %s.\n", FileName);

				sprintf(session, "FEM_Setting");
				FE_VER=GetPrivateProfileInt (session, "FE_VER",		
								  FE_VER,	FileName); 
				FEMCap2G=GetPrivateProfileInt (session, "FEMCap2G",		
													  FEMCap2G,	FileName); 
				FEMCap5G=GetPrivateProfileInt (session, "FEMCap5G",		
													  FEMCap5G,	FileName);
				FEM_INTERNAL_BT=GetPrivateProfileInt (session, "FEM_INTERNAL_BT",		
													  FEM_INTERNAL_BT,	FileName);
				FEM_ANT_DIV=GetPrivateProfileInt (session, "FEM_ANT_DIV",		
													  FEM_ANT_DIV,	FileName);
				FEM_EXTERNAL_2G_LNA=GetPrivateProfileInt (session, "FEM_EXTERNAL_2G_LNA",		
													  FEM_EXTERNAL_2G_LNA,	FileName);
				FEM_EXTERNAL_5G_LNA=GetPrivateProfileInt (session, "FEM_EXTERNAL_5G_LNA",		
													  FEM_EXTERNAL_5G_LNA,	FileName);
				FEM_SAMEANT_2G_BT=GetPrivateProfileInt (session, "FEM_SAMEANT_2G_BT",		
													  FEM_SAMEANT_2G_BT,	FileName);
				FEM_SAMEANT_5G_BT=GetPrivateProfileInt (session, "FEM_SAMEANT_5G_BT",		
													  FEM_SAMEANT_5G_BT,	FileName);
				CONCURRENT_CAP_2G_BT=GetPrivateProfileInt (session, "CONCURRENT_CAP_2G_BT",		
													  CONCURRENT_CAP_2G_BT,	FileName);
				CONCURRENT_CAP_5G_BT=GetPrivateProfileInt (session, "CONCURRENT_CAP_5G_BT",		
													  CONCURRENT_CAP_5G_BT,	FileName);

				AntDiversityInfo = GetPrivateProfileInt (session, "AntDiversityInfo",
													  AntDiversityInfo, FileName);
				for(i=0; i<9; i++)
				{
					char key[_MAX_PATH];
					sprintf(key, "Front_End_2G_Byte_%d", i);
					Front_End_2G[i]=GetPrivateProfileInt (session, key,		
													  Front_End_2G[i],	FileName);
					sprintf(key, "Front_End_5G_Byte_%d", i);
					Front_End_5G[i]=GetPrivateProfileInt (session, key,		
													  Front_End_5G[i],	FileName);
				}

				//load parameters from setup.ini
				status = DutIf_SetFEM(FE_VER, FEMCap2G, FEMCap5G, FEM_INTERNAL_BT, 
					FEM_ANT_DIV, FEM_EXTERNAL_2G_LNA, FEM_EXTERNAL_5G_LNA,
					FEM_SAMEANT_2G_BT, FEM_SAMEANT_5G_BT, CONCURRENT_CAP_2G_BT, CONCURRENT_CAP_5G_BT, Front_End_2G, Front_End_5G, AntDiversityInfo);
				DebugLogRite("DutIf_SetFEM: 0x%08X\n",status);
			}
			break;

		case 997:
			{
				DWORD i=0;
                BYTE reading=0; 
				int sensorReading=0;

				if(parm1>0)
					DebugLogRite(" DutIf_SetTxDataRate 11M: 0x%08X\n", 
						DutIf_SetTxDataRate(3)); 

				for (i=0; i<parm1; i++)
				{
					DebugLogRite(" DutIf_SetTxContMode ON:  0x%08X\n", 
					DutIf_SetTxContMode(1, 0, 0xAA));  

					status = DutIf_GetThermalSensorReading(&sensorReading);
					DebugLogRite(" sensorReading: 0x%02X\n",  sensorReading);

					status = DutIf_ReadRfReg(0x4A, &reading);
 					DebugLogRite(" 0x%04X\t0x%02X :0x%08X\t\n", 0x4A, reading, status);
					status = DutIf_ReadRfReg(0x4B, &reading);
 					DebugLogRite(" 0x%04X\t0x%02X :0x%08X\t\n", 0x4B, reading, status);


					status = DutIf_ReadRfReg(0x3B, &reading);
 					DebugLogRite(" 0x%04X\t0x%02X :0x%08X\t\n", 0x3B, reading, status);

					if(0x0C != reading) 
					{
						if(parm2)
						{
							break;
						}
						else
						{
							DebugLogRite ("++++++++++++++++++++++++++>>>>>>>>\n");
							Sleep(1000);
						}
					}

					Sleep(1000);
					DebugLogRite(" DutIf_SetTxContMode OFF: 0x%08X\n", 
						DutIf_SetTxContMode(0, 0, 0xAA));  
				
					DebugLogRite ("\n");
				}
				
			}
			break;
#ifndef _LINUX_
#if defined(_FLASH_)
		case 998:
			{
				DebugLogRite("DutIf_CheckCalDataSpace: 0x%08X\n", 
					DutIf_CheckCalDataSpace((int*)&parm1));
				DebugLogRite("CheckCalDataSpace: 0x%08X\n",  parm1);
			 
				DebugLogRite("DutIf_CheckMacAddressSpace: 0x%08X\n", 
					DutIf_CheckMacAddressSpace((int*)&parm1));
				DebugLogRite("CheckMacAddressSpace: 0x%08X\n",  parm1);
				
			}
			break;
#endif // #if defined(_FLASH_)
#endif	//#ifndef _LINUX_
		case 921:
			{
				BOOL Enabled=0;
				int sensorReading=0;
				DWORD TempRefAtMinus40C=0;
				DWORD SlopeNumerator=0,  SlopeDenominator=0;
				DWORD SlopeNumerator1=0,  SlopeDenominator1=0;
				DWORD CalibrationIntervalInMS=0; 
				DWORD PPACoeff=0, PACoeff=0;
				DWORD Temp=0, PTarget=0;

				cnt = sscanf (line, "%d %d",&cmd, &sensorReading);

				DebugLogRite("DutIf_GetThermalSensorReading: 0x%08X\n", 
						DutIf_GetThermalSensorReading(&sensorReading));
				DebugLogRite("sensorReading: 0x%02X\n",  sensorReading);

				if(parm1)
				{
/*					DebugLogRite("\nDutIf_GetThermalComp: 0x%08X\n", 
						DutIf_GetThermalComp(&Enabled,
							&TempRefAtMinus40C, 
							&SlopeNumerator, &SlopeDenominator, 
							&CalibrationIntervalInMS, &PPACoeff, &PACoeff));
					DebugLogRite("Enabled = %d\nTempRef = 0x%02X\n" 
							"SlopeNumerator = %d \tSlopeDenominator = %d\n"
							"CalibrationIntervalInMS = %d\n"
							"PPACoeff = 0x%02X\nPACoeff = 0x%02X\n",  
							Enabled,
							TempRefAtMinus40C, 
							SlopeNumerator, SlopeDenominator, 
							CalibrationIntervalInMS, PPACoeff, PACoeff);
*/
					DebugLogRite("\nDutIf_GetThermalTest: 0x%08X\n", 
						DutIf_GetThermalTest(&Enabled,
							&PTarget,
							&TempRefAtMinus40C, 
							&SlopeNumerator, &SlopeDenominator, 
							&CalibrationIntervalInMS, 							
							&Temp, 
							&SlopeNumerator1, &SlopeDenominator1));

					DebugLogRite("Enabled = %d\nPTarget = 0x%02X\n" 
							"TempRef = 0x%02X\n"
							"SlopeNumerator = %d \nSlopeDenominator = %d\n"
							"CalibrationIntervalInMS = %d\n"
							"Temperature = 0x%2X  \nSlopeNumerator1 = %d\nSlopeDenominator1 = %d\n",  
							Enabled,
							PTarget,
							TempRefAtMinus40C, 
							SlopeNumerator, SlopeDenominator, 
							CalibrationIntervalInMS, 
							Temp,
							SlopeNumerator1, SlopeDenominator);
				}
			}
			break;

 		 case 923:
			 {
				 int lnaMode=0;

				 DebugLogRite("DutIf_GetExtLnaNsettings: 0x%08X\n", 
						DutIf_GetExtLnaNsettings(&lnaMode));
				 DebugLogRite("LNA mode: 0x%08X\n", lnaMode);
			 }
			break;

		 case 924:
			 {
				 int lnaMode=0;

				 cnt = sscanf (line, "%d %x",&cmd, &lnaMode);
				 DebugLogRite("DutIf_SetExtLnaNsettings: 0x%08X\n", 
						DutIf_SetExtLnaNsettings(lnaMode));
			 }
			break;

 		 case 999:
			 {
				 char FileName[MAX_LENGTH]="";
				 cnt = sscanf (line, "%d %s",&cmd, FileName);
				 int status =0;

				 if(strlen(FileName))
				 {
					 sprintf(FileName,"%s.txt",FileName);
					 status = SetOutputFileName(FileName);
					 DebugLogRite("SetOutputFileName [%s]: %08X\n", FileName, status);
				 }
			 }
			 break;
		
		  case 971:
			{
				BYTE Buf[64];

				DebugLogRite("DutIf_ArmReadUlongArray %X\n",
					DutIf_ArmReadUlongArray(parm1, (DWORD *)Buf, parm2));
				DebugLogRite("Buf[0]=0x%02X Buf[1]=0x%02X Buf[2]=0x%02X Buf[3]=0x%02X\n", 
							Buf[0], Buf[0], Buf[2], Buf[3]); 

			}
			break;
#if defined(_W8782_)
// DPD structures

			//DPD functions
		  case 200:	// Get DPD info
			{
				int i, num_of_entry;
				DWORD PowerID=0;
				DWORD Memory2Row=0;
				HAL_DPD_COEFF DPDTbl[HAL_DPD_MAX_NUM_COEFF]={0};

				cnt = sscanf (line, "%d %x %d",&cmd, &PowerID, &Memory2Row);
			
				DebugLogRite("DutIf_GetCompensation: 0x%08X\n", DutIf_GetCompensation(PowerID, Memory2Row, (DWORD *)DPDTbl));
				if(Memory2Row)
				{
					num_of_entry = 18;
				}
				else
				{
					num_of_entry = 9;
				}
				DebugLogRite("PowerID: 0x%04X\n", PowerID);
				if(Memory2Row)
				{
					DebugLogRite("Memory 2 Row\n");
				}
				else
				{
					DebugLogRite("Memory 1 Row\n");
				}
				for(i=0; i<num_of_entry; i++)
				{
					DebugLogRite("Entry[0x%02X] Real Hi=0x%02X, Real Lo=0x%02X, Image Hi=0x%02X, Image Lo=0x%02X\n",
						i, DPDTbl[i].real_hi, DPDTbl[i].real_lo, DPDTbl[i].imag_hi, DPDTbl[i].imag_lo);
				}
			}
			break;

		  case 201: // Set DPD info
			{
				DWORD PowerID=0;
				DWORD Memory2Row=0;
				HAL_DPD_COEFF DPDTbl[HAL_DPD_MAX_NUM_COEFF]={0};
				DWORD *ptr;

				ptr = (DWORD *)DPDTbl;

				cnt = sscanf (line, "%d %x %d %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x",
					&cmd, &PowerID, &Memory2Row,
					&ptr[0], &ptr[1], &ptr[2], &ptr[3], &ptr[4], &ptr[5], &ptr[6], &ptr[7], &ptr[8], 
					&ptr[9], &ptr[10], &ptr[11], &ptr[12], &ptr[13], &ptr[14], &ptr[15], &ptr[16], &ptr[17]
					);
				if(Memory2Row == 1)
				{
					if( cnt !=21)
					{
						DebugLogRite("Invalid input: Memory 2 row option needs 18 DPD entry input \n");
						break;
					}
				}
				else
				{
					if( cnt !=12)
					{
						DebugLogRite("Invalid input: Memory 1 row option needs 9 DPD entry input \n");
						break;
					}
				}

				DebugLogRite("DutIf_SetCompensation: 0x%08X\n", DutIf_SetCompensation(PowerID, Memory2Row, ptr));
			}
			break;

#endif //#if defined(_W8782_)

#ifdef _ENGINEERING_
		case 970:
        {	
		    int i=0; 
			DWORD sizeBytes=0xF000;
			DWORD SqBufAddr=0;

			DebugLogRite("DutIf_RunBbpSqDiag %X\n",
					DutIf_RunBbpSqDiag(sizeBytes, &SqBufAddr));
			DebugLogRite("sizeBytes = 0x%02X\nSqBufAddr = 0x%08X\n ", 
								sizeBytes, SqBufAddr ); 
		}
		break;
#endif //#ifdef _ENGINEERING_

 		default:
			DebugLogRite("Unknown Option\n");
			break;
        }

#ifndef _HIDE_MENU_
       if(99 != cmd )
		{
			DebugLogRite ("Press [Enter] key to continue ... ");
 			fgets(line, sizeof(line), stdin);
		}
#endif	//#ifndef _HIDE_MENU_
    } while (99 != cmd);

	DutIf_SetExtLnaNsettings(LNA_MODE_AUTO_CHECK);

#if !defined(_WIFI_SDIO_) && !defined(_BT_SDIO_)
 	status = DutIf_Disconnection(theObj);
	DebugLogRite(" DutIf_Disconnection: %d\n", status);
#endif // #if defined(_WIFI_SDIO_) && defined(_BT_SDIO_)

	return (status);
}


#endif //#if defined (_WLAN_)

