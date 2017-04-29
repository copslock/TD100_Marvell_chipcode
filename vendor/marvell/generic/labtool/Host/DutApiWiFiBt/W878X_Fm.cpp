/** @file W878X_Fm.cpp
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
#include "DutFmApi878XDll.h"
#include "W878XApp.h"

#include "DutFuncs.h"
#include "DutAppVerNo.h"
#ifdef _HIDE_MENU_
#include "FmMenu.h"
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
#endif //#endif //#if defined(_W8780_) || defined (_W8688_) || defined (_W8689_) || defined (_W8787_) || defined(_W8790_)

 
int FmMenu(int OptionMode, void** theObj=NULL)
{
 	int				status=0;
 	unsigned long	cmd=0, parm1=0, parm2=0, cnt=0;
	char line[_MAX_PATH]="";

 	DebugLogRite("%s\n", Dut_Fm_About());
  	status = Dut_Fm_OpenDevice(theObj);
	DebugLogRite(" Dut_Fm_OpenDevice: 0x%08X\n", status);
	if (status)
	{
		status = Dut_Fm_CloseDevice(theObj);
		if(status) return (status);
		return (0);
	}

#ifdef _HIDE_MENU_
		DisplayMenu(FmCmdNames, 0);
#endif //#ifdef _HIDE_MENU_


	do
    {	
#ifndef _HIDE_MENU_

		DebugLogRite ("\n");
		DebugLogRite ("-----------------------------------------------------------------------\n"); 
		DebugLogRite ("\t\t87xx (FM) TEST MENU\n");
		DebugLogRite ("-----------------------------------------------------------------------\n"); 
  
#if !defined (_DEVELOPMENT_PHASE_)
		DebugLogRite (" 17. Tx Continous (Enable=0) (DataRate(0x)=11)\n"
			"\t(PayloadPattern=2) (PayloadOnly=1)\n"
			"\tEnable: 0: off; 1: on\n"
			"\tDataRate: 0x10: 1M, 0x20:2M, 0x30: 3M\n"
			"\tPayloadPattern: 0: all 0, 1: all 1, 2: PN9, 3: 0xAA, 4: 0xF0\n"
			"\tPayloadOnly: tx Payload only, no header (true, false)\n");

		DebugLogRite (" 18. Tx Cw (Enable=0) \n""\t\tEnable: 0: off; 1: on\n");
		DebugLogRite (" 19. Tx Carrier Suppression Test signal (Enable=0) \n""\t\tEnable: 0: off; 1: on\n");

		DebugLogRite (" 22. Set Caled power (pwr(double) mode(0=NormalMode 1=PerformanceMode) useFeLoss(=0))\n");
#endif // #if !defined (_DEVELOPMENT_PHASE_)

			if(!Dut_Shared_SupportNoE2PROM())
			{
#ifndef _FLASH_
				DebugLogRite (" 39. E2PROM download from binary file.\n");
#endif // #ifndef _FLASH_
				DebugLogRite (" 40. E2PROM Verify from binary file\n");
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

		DebugLogRite (" 53. Set Cal (from file:CalBtDataFile.txt)\n");
		DebugLogRite (" 54. Get Cal\n");
		if (SUPERUSER == OptionMode)
		{ 		
 			DebugLogRite (" 66. Read FMU Reg		(0x)(addr) (count =1)\n");
			DebugLogRite (" 67. Write FMU Reg		(0x 0x)(addr data)\n");
   
			DebugLogRite (" 68. Peek SOC Mem		(0x)(addr) (count =1)\n");
			DebugLogRite (" 69. Poke SOC Mem		(0x 0x)(addr data)\n");
 
			if(!Dut_Shared_SupportNoE2PROM())
			{
				DebugLogRite (" 70. Peek E2PROM		(0x)(addr) (count =1)\n");
				DebugLogRite (" 71. Poke E2PROM		(0x 0x)(addr data)\n");

#if defined (_W8689_) || defined (_W8787_) || defined(_W8782_)
			DebugLogRite (" 72. Read Efuse Data		(0x)(block) (WordCount =16)\n");

			DebugLogRite (" 73. Write Efuse Data	(0x 0x)(block WordCount=1)\n"
				"\t\tRequires 2.5V on VDD25_EFuse pin\n");
#endif //#if defined (_W8689_) || defined (_W8787_) 

			}
		}
		
		DebugLogRite (" 95. Get Rf XTAL control\n");
		DebugLogRite (" 96. Set Rf XTAL control (setting(8 bits))\n");

		DebugLogRite (" 99. Exit\n"); 
#else  //#ifndef _HIDE_MENU_
#endif //#ifndef _HIDE_MENU_

        DebugLogRite ("Enter option: ");

		fgets(line, sizeof(line), stdin);
		
		cmd=-1; 
        parm1=0;
		parm2=0;
		cnt = sscanf (line, "%d %x %x",&cmd, &parm1, &parm2);
		DebugLogRite("%s\n", line);

#ifdef _HIDE_MENU_
	if( strstr(line, "-h") || strstr(line, "?") )
	{
		DisplayMenu(FmCmdNames, cmd);
	}
	else
#endif //#ifdef _HIDE_MENU_
	{
        switch (cmd)
        {
#if !defined (_DEVELOPMENT_PHASE_)
		case 17: // read BTU
 			{	
				DWORD enable =0, packetType=DEF_PACKETTYPE;
				DWORD payloadPattern=DEF_PAYLOADPATTERN, payloadOnly=1;
				sscanf (line, "%d %d %x %x %x",
					&cmd, &enable, 
					&packetType, &payloadPattern, &payloadOnly);
    	
				DebugLogRite(" Dut_Fm_TxBtCont: 0x%08X\n", 
					Dut_Fm_TxBtCont(enable?true:false, 
									packetType, 
									payloadPattern, 
									payloadOnly?true:false));
			}
			break;
 
		case 18: // read BTU
 			{	
				DebugLogRite(" Dut_Fm_TxBtCw: 0x%08X\n", 
					Dut_Fm_TxBtCw(parm1?true:false));
			}
			break;
 
		case 19: // read BTU
 			{	    	

				cnt = sscanf (line, "%d %d %d",
					&cmd, &parm1, &parm2);
				if(cnt==3)
				{
					DebugLogRite(" Dut_Fm_TxBtCst: 0x%08X\n", 
						Dut_Fm_TxBtCst(	parm1?true:false, 
										parm2));
				}
				else
				{
					DebugLogRite(" Dut_Fm_TxBtCst: 0x%08X\n", 
						Dut_Fm_TxBtCst(parm1?true:false));
				}

			}
			break;
		case 22:
			{
				double pwr=0;
				int mode=0, useFeLoss=0;
				sscanf (line, "%d %lf %d %d",
					&cmd, &pwr, &mode, &useFeLoss);
				DebugLogRite(" Dut_Fm_SetRfPowerUseCal: 0x%08X\n", 
					Dut_Fm_SetRfPowerUseCal(pwr, mode, useFeLoss));
			}
			break;
#endif // #if !defined (_DEVELOPMENT_PHASE_)
 

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
				Dut_Fm_FlexSpiDL(true, "");
				DebugLogRite(" Dut_Fm_FlexSpiDL: 0x%08X\n", status);
 
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
				Dut_Fm_FlexSpiCompare(""); 
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

			DebugLogRite("Dut_Fm_DumpE2PData 0x%08X\n", 
				Dut_Fm_DumpE2PData( startingAddress,  Length, 
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

			DebugLogRite("Dut_Fm_ForceE2PromType 0x%08X\n", 
				Dut_Fm_ForceE2PromType(IfType, AddrWidth, DeviceType));
		} 
	
		case 42:
		{ 
			DWORD IfType=0, AddrWidth=-1, DeviceType=-1;
 
			DebugLogRite("Dut_Fm_GetE2PromType 0x%08X\n", 
				Dut_Fm_GetE2PromType(&IfType, &AddrWidth, &DeviceType));

			DebugLogRite("IfType %d\tAddrWidth %d\tDeviceType %d\n", IfType, AddrWidth, DeviceType);
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
				DebugLogRite(" Dut_Fm_GetUsbAttributes: 0x%08X\n", 
						Dut_Fm_GetUsbAttributes((BYTE*)&parm1, (BYTE*)&parm2));
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

				DebugLogRite(" Dut_Fm_SetUsbAttributes: 0x%08X\n", 
						Dut_Fm_SetUsbAttributes(Attribute, MaxPower));
			}
 			break;
#endif //#if defined(_IF_USB_) 

#if defined(_BT_UART_)  
		case 47:
			{
				int rate=115200; 

 				cnt = sscanf (line, "%d %d",&cmd, &rate);

				DebugLogRite(" ChangeUartRate: 0x%08X\n", ChangeUartRate(rate));
			}
			break;
#endif // #if defined(_BT_UART_)  

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
//					status = Dut_Fm_EraseEEPROM(parm1);
//					DebugLogRite(" Dut_Fm_EraseEEPROM: 0x%08X\n", status);
				}
				else
				{
					Dut_Fm_FlexSpiDL(true, "");
					DebugLogRite(" Dut_Fm_FlexSpiDL: 0x%08X\n", status);
				}
			}
			break;
#endif //#ifndef _FLASH_



		
		case 53:
			DebugLogRite("SaveCalDataBt:\t0x%08X\n", 
				LoadSaveCalData(1) );
			break;
		case 953:
			DebugLogRite("UpdateSaveBtCalData:\t0x%08X\n", 
				UpdateSaveBtCalData(1) );
			break;

		case 54:
			DebugLogRite("UpLoadSaveCalData:\t0x%08X\n", 
				UpLoadSaveCalData((int*) &parm1, 1) );
			
			DebugLogRite("CrC %d\n", parm1);
			break;
#endif // #if !defined (_WLAN_) && !defined (_NO_EEPROM_) 		

#if defined (_FM_) 
		case 66: // read FM
			{	
				DWORD	Address =0;
				DWORD	Value=0;
				int		i=0, cnt=0;
				int status=0;
 				cnt = parm2==0?1:parm2;
				for (i=0; i<cnt; i++)
				{
					Address = parm1+i*sizeof(DWORD); 
					status= Dut_Fm_ReadFmuRegister(Address, &Value);
					DebugLogRite(" Dut_Fm_ReadFmuRegister: 0x%08X\t", status);
//					DebugLogRite(" Dut_Fm_ReadBrfRegister: 0x%08X\t", 
//						status= Dut_Fm_ReadBrfRegister(Address, &Value));
					DebugLogRite("0x%08X\t0x%08X\n", Address, Value);
					if(status) break;
				}
			}
			break;
		case 67: // write FM
			{	
				DWORD Address =0;
				DWORD Value=0;
				Address = parm1;
				Value = parm2; 
				DebugLogRite(" Dut_Fm_WriteFmuRegister: 0x%08X\n", 
					Dut_Fm_WriteFmuRegister(Address, Value));
				DebugLogRite("0x%08X\t0x%08X\n", Address, Value);
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
					DebugLogRite(" Dut_Fm_ReadMem: 0x%08X\t", 
						Dut_Fm_ReadMem(Address, &Value));
					DebugLogRite("0x%08X\t0x%08X\n", Address, Value);
				}
			}
			break;
		case 69: // write mem
			{	
				DWORD Address =0, Value=0;
				Address = parm1; 
				Value = parm2;
				DebugLogRite(" Dut_Fm_WriteMem: 0x%08X\n", 
					Dut_Fm_WriteMem(Address, Value));
				DebugLogRite("0x%08X\t0x%08X\n", Address, Value);
			}
			break;
#endif //#if defined (_FM_) 

#if !defined (_WLAN_) && !defined (_NO_EEPROM_) 		

		case 70: // read mem
			{	
				DWORD Address =0, Value=0;
				int i=0, cnt=0;

				sscanf (line, "%d %x %x",&cmd, &parm1, &parm2);
				cnt = parm2==0?1:parm2;
				for (i=0; i<cnt; i++)
				{
					Address = parm1+i*sizeof(DWORD); 
					DebugLogRite(" Dut_Fm_SpiReadDword: 0x%08X\t", 
						Dut_Fm_SpiReadDword(Address, &Value));
					DebugLogRite("0x%08X\t0x%08X\n", Address, Value);
				}
			}
			break;
		case 71: // write mem
			{	
				DWORD Address =0, Value=0;
				Address = parm1; 
				Value = parm2;
				DebugLogRite(" Dut_Fm_SpiWriteDword: 0x%08X\n", 
					Dut_Fm_SpiWriteDword(Address, Value));
				DebugLogRite("0x%08X\t0x%08X\n", Address, Value);
			}
			break;
#endif // #if !defined (_WLAN_) && !defined (_NO_EEPROM_) 		


#if defined (_W8689_) || defined (_W8787_) || defined(_W8782_) 
		case 72: // read Efuse
			{	
				DWORD block =0, len=16;
				WORD Data[16]={0};
				DWORD i=0;

				sscanf (line, "%d %x %x",&cmd, &block, &len);
 					DebugLogRite(" Dut_Fm_GetEfuseData: 0x%08X\n", 
						Dut_Fm_GetEfuseData(block, len, Data));
 				for (i=0; i<len; i++)
				{
					DebugLogRite("0x%08X\t0x%08X\n", i, Data[i]);
				}
			}
			break;
		case 73: // write Efuse
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
				DebugLogRite(" Dut_Fm_SetEfuseData: 0x%08X\n", 
					Dut_Fm_SetEfuseData(block, len, Data));
			}
			break;
#endif // #if defined (_W8689_) || defined (_W8787_) 
	
         case 188: // Get Online Release Note
			 {	
				char  releaseNote[MAX_LENGTH]={0};
 				status = Dut_Fm_GetFwReleaseNote(releaseNote);
 				DebugLogRite(" Dut_Fm_GetFwReleaseNote: 0x%08X\n%s\n", status, releaseNote);
		 			
				DebugLogRite(" Dut_Fm_GetCurrentORVersion : 0x%08X\n",  
						Dut_Fm_GetCurrentORVersion((int*)&parm1, (int*)&parm2));
				DebugLogRite("OR Version for Soc: %X.%02X\nOR Version for RF : %X.%02X\n", 
					((parm1 & 0xFFFF0000)>>16), ((parm1 & 0xFFFF)), 
					((parm2 & 0xFFFF0000)>>16), ((parm2 & 0xFFFF)));

#ifndef _W8790_
				DebugLogRite(" Dut_Fm_GetCurrentAppMode : 0x%08X\n",  
						Dut_Fm_GetCurrentAppMode((int*)&parm1));
				DebugLogRite("Current App Mode: %X\n", 
					parm1);
#endif //#ifndef _W8790_

	 }
             break;

#ifdef _FM_
        case 89: // load configuration file
			{  
				char FileName[_MAX_PATH]="";
				cnt = sscanf (line, "%d %s",&cmd, FileName);
				if(strlen(FileName))
					DebugLogRite("LoadInitFmConfigFile [%s]: 0x%08X\n", FileName, 
						LoadInitFmConfigFile(FileName));
				else
					DebugLogRite("File Name not specified\n"); 

			}
            break;
#endif	//#ifdef _FM_

#if defined(_W8790_)
		case 95:
			status = Dut_Fm_GetXtal ((BYTE*)&parm1, (BYTE*)&parm2);
			DebugLogRite ("Dut_Fm_GetXtal: 0x%08X\n", status); 
			DebugLogRite (" XTal: %s %02X\n", parm1?"ext":"int", parm2); 
			break;
			
		case 96:	// set RF crystal oscillator
			status = Dut_Fm_SetXtal ((BYTE)parm1, (BYTE)parm2);
			DebugLogRite ("Dut_Fm_SetXtal: 0x%08X\n", status); 
			break;
#endif // #if defined(_W8790_)


// "BT Current Optimizations"
 
#if defined (_FM_) 

      case 88: // Fm Versions
			{  
				DWORD HwId=0, ManId=0, FwVer=0, HwVer=0; 
				int status = 0;
 				char VersionString[_MAX_PATH]="";
				DWORD version=Dut_Fm_Version();
 
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


				status = Dut_Fm_GetFmFwVersionStr(VersionString);
 				if (status)
					DebugLogRite("Dut_Fm_GetFmFwVersion : 0x%08X\n", status);
				else
					DebugLogRite("%s\n", VersionString);

/*
				status = Dut_Fm_GetFmFwVersion(&FwVer);
				if (status)
					DebugLogRite("Dut_Fm_GetFmFwVersion : 0x%08X\n", status);
				else
 					DebugLogRite("FwVersion : %04X\t\t", FwVer);
*/

				status = Dut_Fm_GetFmHwVersion(&HwVer);
				if (status)
					DebugLogRite("Dut_Fm_GetFmHwVersion : 0x%08X\n", status);
				else
 					DebugLogRite("HwVersion : %04X\t\t", HwVer);

				status = Dut_Fm_GetFmHwId(&HwId);
				if(status)
					DebugLogRite("Dut_Fm_GetFmHwId : 0x%08X\n", status);
				else
 					DebugLogRite("Hw ID : %04X\n", HwId);

				status = Dut_Fm_GetFmManId(&ManId);
				if (status)
 					DebugLogRite("Dut_Fm_GetFmManId : 0x%08X\n", status);
				else
 					DebugLogRite("Manufacture ID : %08X\n", ManId);
			}
            break;
/*
	   case 100: // Dut_Fm_FmReset
		   {   
				DebugLogRite("Dut_Fm_FmReset : 0x%08X\n",  
						Dut_Fm_FmReset());			 
			}
            break;
*/
	   case 100: // Dut_Fm_FmInitialization
			{  int FreqInKHz = 38400, FreqErrPpm=0;

				sscanf (line, " %x %d ", &cmd, &FreqInKHz);

				DebugLogRite("Dut_Fm_FmInitialization : 0x%08X\n",  
						Dut_Fm_FmInitialization(FreqInKHz));			 
			}
            break;
/*
	   case 101: // Dut_Fm_SetFmReferenceClk
			{  int FreqInKHz = 38400;

				sscanf (line, " %x %d", &cmd, &FreqInKHz );

				DebugLogRite("Dut_Fm_SetFmReferenceClk : 0x%08X\n",  
						Dut_Fm_SetFmReferenceClk(FreqInKHz));			 
			}
            break;
*/
	   case 102: // Dut_Fm_SetFmReferenceClkError
			{  int FreqError=10;

				sscanf (line, " %x %d", &cmd, &FreqError );

				DebugLogRite("Dut_Fm_SetFmReferenceClkError : 0x%08X\n",  
						Dut_Fm_SetFmReferenceClkError(FreqError));			 
			}
            break;
 
	   case 103: // Dut_Fm_FmPowerUpMode
			{  int mode=0;

				sscanf (line, "%x %x", &cmd, &mode );

				DebugLogRite("Dut_Fm_FmPowerUpMode : 0x%08X\n",  
						Dut_Fm_FmPowerUpMode(mode));			 
			}
            break;
	   case 104: // Dut_Fm_GetFmCurrentRssi
			{   
				DebugLogRite("Dut_Fm_GetFmCurrentRssi : 0x%08X\n",  
						Dut_Fm_GetFmCurrentRssi(&parm1));
				 
				DebugLogRite("RSSI : 0x%X (%d)\n", parm1, parm1);
			}
            break;
	   case 105: // Dut_Fm_GetFmCurrentCmi
			{   
				DebugLogRite("Dut_Fm_GetFmCurrentCmi : 0x%08X\n",  
						Dut_Fm_GetFmCurrentCmi(&parm1));
				 
				DebugLogRite("CMI : 0x%X (%d)\n", parm1, parm1);
			}
            break;
	   case 106: // Dut_Fm_SelectFmRxAntennaType
			{   
				DebugLogRite("Dut_Fm_SelectFmRxAntennaType : 0x%08X\n",  
						Dut_Fm_SelectFmRxAntennaType(parm1)); 
			}
            break;

	   case 107: // Dut_Fm_GetFmIrsMask(int *pRssiLow,		int *pNewRdsData,	int *pRssiIndicator,	int *pBandLimit, 
			//	 int *pSyncLost,	int *pSearchStop,	int *pBMatch,			int *pPiMatched,
			//	 int *pMonoStereoChanged,	int *pAudioPause)
			{   
				int RssiLow=0,	NewRdsData,	RssiIndicator=0,	BandLimit=0; 
				int SyncLost=0,	SearchStop, BMatch=0,			PiMatched=0;
				int MonoStereoChanged=0,	AudioPause=0,		CmiLow=0;

				DebugLogRite("Dut_Fm_GetFmIrsMask : 0x%08X\n",  
						Dut_Fm_GetFmIrsMask(&RssiLow,	&NewRdsData,	
											&RssiIndicator,	&BandLimit, 
											&SyncLost,	&SearchStop, 
											&BMatch,	&PiMatched,
											&MonoStereoChanged,	&AudioPause, 
											&CmiLow)); 
				DebugLogRite("IRS Mask : \n"
					" RssiLow: %d\t NewRdsData: %d\t RssiIndicator: %d\t BandLimit: %d\n" 
					" SyncLost: %d\t SearchStop: %d\t BMatch: %d\t\t PiMatched: %d\n"
					" MonoStereoChanged: %d\t\t AudioPause: %d\t CmiLow: %d\n", 
						RssiLow, NewRdsData,  RssiIndicator, BandLimit, 
						SyncLost, SearchStop, BMatch, PiMatched,
						MonoStereoChanged,	AudioPause , CmiLow);
			}
            break;
	   case 108: // Dut_Fm_SetFmIrsMask
			{   
				int RssiLow=0,	NewRdsData,	RssiIndicator=1,	BandLimit=0; 
				int SyncLost=0,	SearchStop, BMatch=0,			PiMatched=0;
				int MonoStereoChanged=0,	AudioPause=0,		CmiLow=0;

				sscanf (line, "%x %d %d %d %d %d %d %d %d %d %d %d", &cmd, 
											&RssiLow,	&NewRdsData,	
											&RssiIndicator,	&BandLimit, 
											&SyncLost,	&SearchStop, 
											&BMatch,	&PiMatched,
											&MonoStereoChanged,	&AudioPause, &CmiLow);

				DebugLogRite("Dut_Fm_SetFmIrsMask : 0x%08X\n",  
						Dut_Fm_SetFmIrsMask(RssiLow,		NewRdsData,	
											RssiIndicator,	BandLimit, 
											SyncLost,		SearchStop, 
											BMatch,			PiMatched,
											MonoStereoChanged,	AudioPause, 
											CmiLow)); 
			}
            break;
	   case 109: // Dut_Fm_GetFmCurrentFlags
			{   
				DebugLogRite("Dut_Fm_GetFmCurrentFlags : 0x%08X\n",  
						Dut_Fm_GetFmCurrentFlags(&parm1));
				 
				DebugLogRite("Current Flags : 0x%X\n", parm1);
			}
            break;
	   case 110: // Dut_Fm_FmPowerDown
			{   
				DebugLogRite("Dut_Fm_FmPowerDown : 0x%08X\n",  
						Dut_Fm_FmPowerDown()); 
			}
            break;
	   
	   
	   
	   
	   case 120: // Dut_Fm_GetFmChannel
			{   
				DebugLogRite("Dut_Fm_GetFmChannel : 0x%08X\n",  
						Dut_Fm_GetFmChannel(&parm1));
				 
				DebugLogRite("Channel : %d KHz\n", parm1);
			}
            break;
	   case 121: // Dut_Fm_SetFmChannel
			{  DWORD chInKHz=95600;

				sscanf (line, "%x %d", &cmd,  &chInKHz );

				DebugLogRite("Dut_Fm_SetFmChannel : 0x%08X\n",  
						Dut_Fm_SetFmChannel(&chInKHz));			 
				DebugLogRite("New Channel : %d KHz\n", chInKHz);
			}
            break;
/*	
	   case 122: // Dut_Fm_GetFmAfChannel
			{   
				DebugLogRite("Dut_Fm_GetFmAfChannel : 0x%08X\n",  
						Dut_Fm_GetFmAfChannel(&parm1));
				 
				DebugLogRite("AF Channel : %d KHz\n", parm1);
			}
            break;
*/
	   case 123: // Dut_Fm_SetFmAfChannel
			{  
			    DWORD chInKHz=95600;
			    BYTE  AfMode=0;

				sscanf (line, "%x %d %d", &cmd, &chInKHz , &AfMode);

				DebugLogRite("Dut_Fm_SetFmAfChannel : 0x%08X\n",  
						Dut_Fm_SetFmAfChannel(&chInKHz, AfMode));
				
				DebugLogRite("New AF Channel : %d KHz\n", chInKHz);
			}
            break;
            
	   case 124: // Dut_Fm_GetFmSearchDir
			{  
			 
				DebugLogRite("Dut_Fm_GetFmSearchDir : 0x%08X\n",  
						Dut_Fm_GetFmSearchDir(&parm1));
				 
				DebugLogRite("Direction : %d\n", parm1);
			}
            break;
            
	   case 125: // Dut_Fm_SetFmSearchDir
			{  int mode=0;

				sscanf (line, "%x %d", &cmd, &mode );

				DebugLogRite("Dut_Fm_SetFmSearchDir : 0x%08X\n",  
						Dut_Fm_SetFmSearchDir(mode));			 
			}
            break;

	   case 126: // Dut_Fm_GetFmAutoSearchMode
			{  
			 
				DebugLogRite("Dut_Fm_GetFmAutoSearchMode : 0x%08X\n",  
						Dut_Fm_GetFmAutoSearchMode(&parm1));
				 
				DebugLogRite("mode : %d\n", parm1);
			}
            break;
	   case 127: // Dut_Fm_SetFmAutoSearchMode
			{  int mode=0;

				sscanf (line, "%x %d", &cmd, &mode );

				DebugLogRite("Dut_Fm_SetFmAutoSearchMode : 0x%08X\n",  
						Dut_Fm_SetFmAutoSearchMode(mode));			 
			}
            break;
	   case 128: // Dut_Fm_StopFmSearch
			{  
				DebugLogRite("Dut_Fm_StopFmSearch : 0x%08X\n",  
						Dut_Fm_StopFmSearch());			 
			}
            break;
/*
	   case 129: // Dut_Fm_SetFmLoInjectionSide
			{  int mode=0;

				sscanf (line, "%x %d", &cmd, &mode );

				DebugLogRite("Dut_Fm_SetFmLoInjectionSide : 0x%08X\n",  
						Dut_Fm_SetFmLoInjectionSide(mode));			 
			}
            break;
*/
	   case 130: // Dut_Fm_GetFmRxForceMonoMode
			{  
			 
				DebugLogRite("Dut_Fm_GetFmRxForceMonoMode : 0x%08X\n",  
						Dut_Fm_GetFmRxForceMonoMode(&parm1));
				 
				DebugLogRite("mode : %d\n", parm1);
			}
            break;
	   case 131: // Dut_Fm_SetFmRxForceMonoMode
			{  int mode=0;

				sscanf (line, "%x %d", &cmd, &mode );

				DebugLogRite("Dut_Fm_SetFmRxForceMonoMode : 0x%08X\n",  
						Dut_Fm_SetFmRxForceMonoMode(mode));			 
			}
            break;
	   case 132: // Dut_Fm_GetFmRssiThreshold
			{  
			 
				DebugLogRite("Dut_Fm_GetFmRssiThreshold : 0x%08X\n",  
						Dut_Fm_GetFmRssiThreshold(&parm1));
				 
				DebugLogRite("RSSI Threshold : 0x%X\n", parm1);
			}
            break;
	   case 133: // Dut_Fm_SetFmRssiThreshold
			{  int threshold=0x30;

				sscanf (line, "%x %x", &cmd, &threshold );

				DebugLogRite("Dut_Fm_SetFmRssiThreshold : 0x%08X\n",  
						Dut_Fm_SetFmRssiThreshold(threshold));			 
			}
            break;
 
/* // TBRM
 	   case 134: // Dut_Fm_GetFmBand
			{  
			 
				DebugLogRite("Dut_Fm_GetFmBand : 0x%08X\n",  
						Dut_Fm_GetFmBand(&parm1));
				 
				DebugLogRite("Fm Band : %d\n", parm1);
			}
            break;
*/ 
	   case 135: // Dut_Fm_SetFmBand
			{  int band=0;

				sscanf (line, "%x %d", &cmd, &band );

				DebugLogRite("Dut_Fm_SetFmBand : 0x%08X\n",  
						Dut_Fm_SetFmBand(band));			 
			}
            break;
/*
       case 136: // Dut_Fm_GetFmChStepSizeKHz
			{  
			 
				DebugLogRite("Dut_Fm_GetFmChStepSizeKHz : 0x%08X\n",  
						Dut_Fm_GetFmChStepSizeKHz(&parm1));
				 
				DebugLogRite("StepSizeKHz : %d KHz\n", parm1);
			}
            break;
*/
	   case 137: // Dut_Fm_SetFmChStepSizeKHz
			{  int StepSizeKHz=0;

				sscanf (line, "%x %d", &cmd, &StepSizeKHz );

				DebugLogRite("Dut_Fm_SetFmChStepSizeKHz : 0x%08X\n",  
						Dut_Fm_SetFmChStepSizeKHz(StepSizeKHz));			 
			}
            break;
		
		case 138: // Fm Move Ch Up/Down 
			{  
				if(parm1)
				{
				DebugLogRite("Dut_Fm_FmMoveChDown : 0x%08X\n",  
						Dut_Fm_FmMoveChDown(&parm2));
				}
				else
				{
				DebugLogRite("Dut_Fm_FmMoveChUp : 0x%08X\n",  
						Dut_Fm_FmMoveChUp(&parm2));
				} 

				DebugLogRite("New Channel : %d KHz\n", parm2);
			}
            break;

	   case 139: // Dut_Fm_GetFmCmiThreshold
			{  
			 
				DebugLogRite("Dut_Fm_GetFmCmiThreshold : 0x%08X\n",  
						Dut_Fm_GetFmCmiThreshold(&parm1));
				 
				DebugLogRite("CMI Threshold : 0x%X\n", parm1);
			}
            break;
	   case 140: // Dut_Fm_SetFmCmiThreshold
			{  int threshold=0x30;

				sscanf (line, "%x %x", &cmd, &threshold );

				DebugLogRite("Dut_Fm_SetFmCmiThreshold : 0x%08X\n",  
						Dut_Fm_SetFmCmiThreshold(threshold));			 
			}
            break;

	   case 141: // Dut_Fm_GetFmStereoBlendingMode
			{  
			 
				DebugLogRite("Dut_Fm_GetFmStereoBlendingMode : 0x%08X\n",  
						Dut_Fm_GetFmStereoBlendingMode((int*)&parm1));
				 
				DebugLogRite("Stereo Blending Mode : 0x%X\n", parm1);
			}
            break;
	   case 142: // Dut_Fm_SetFmStereoBlendingMode
			{  int BlendingMode=0;

				sscanf (line, "%x %x", &cmd, &BlendingMode );

				DebugLogRite("Dut_Fm_SetFmStereoBlendingMode : 0x%08X\n",  
						Dut_Fm_SetFmStereoBlendingMode(BlendingMode));			 
			}
            break;
	   case 143: // Dut_Fm_SetFmStereoBlendingConfigration
			{  int TimeConstant=0;

				sscanf (line, "%x %x", &cmd, &TimeConstant );

				DebugLogRite("Dut_Fm_SetFmStereoBlendingConfigration : 0x%08X\n",  
						Dut_Fm_SetFmStereoBlendingConfigration(TimeConstant));			 
			}
            break;
	   case 144: // Dut_Fm_GetFmStereoStatus
			{  
			 
				DebugLogRite("Dut_Fm_GetFmStereoStatus : 0x%08X\n",  
						Dut_Fm_GetFmStereoStatus((int*)&parm1));
				 
				DebugLogRite("Stereo Status : 0x%X\n", parm1);
			}
            break;
	   case 145: // Dut_Fm_GetFmRssiThreshold
			{  
			 
				DebugLogRite("Dut_Fm_SetFmRxForceStereoMode : 0x%08X\n",  
						Dut_Fm_SetFmRxForceStereoMode(parm1));
				 
			}
            break;

/* // TBRM 
	   case 150: // Dut_Fm_GetFmRxAudioDeemphasis
			{  int Deemphasis=0;

				DebugLogRite("Dut_Fm_GetFmRxAudioDeemphasis : 0x%08X\n",  
						Dut_Fm_GetFmRxAudioDeemphasis(&Deemphasis));			 
				DebugLogRite("De-emphasis : %d\n", Deemphasis);
			}
            break;
*/
		case 151: // Dut_Fm_SetFmAudioEmphasis
			{  int Deemphasis=0;
				sscanf (line, "%x %d", &cmd, &Deemphasis );

				DebugLogRite("Dut_Fm_SetFmAudioEmphasis : 0x%08X\n",  
						Dut_Fm_SetFmAudioEmphasis(Deemphasis));			 
			}
            break;
 	   case 152: // Dut_Fm_GetFmRxMuteMode
			{  int MuteMode=0;

				DebugLogRite("Dut_Fm_GetFmRxMuteMode : 0x%08X\n",  
						Dut_Fm_GetFmRxMuteMode(&MuteMode));			 
				DebugLogRite("Mute Mode : %d\n", MuteMode);
			}
            break;
	   case 153: // Dut_Fm_SetFmRxMuteMode
			{  int mute=0;
				sscanf (line, "%x %d", &cmd, &mute );

				DebugLogRite("Dut_Fm_SetFmRxMuteMode : 0x%08X\n",  
						Dut_Fm_SetFmRxMuteMode(mute));			 
			}
            break;
 	   case 154: // Dut_Fm_GetFmRxMuteMode
			{  BYTE path=0;

				DebugLogRite("Dut_Fm_GetRxAudioPath : 0x%08X\n",  
						Dut_Fm_GetRxAudioPath(&path));			 
				DebugLogRite("Audio Path : %d\n", path);
			}
            break;
	   case 155: // Dut_Fm_SetFmRxMuteMode
			{	BYTE AudioPath=0,	I2sOperation=0, I2sMode=0;
				sscanf (line, "%x %d %d %d", &cmd, &AudioPath, &I2sOperation, &I2sMode );

				DebugLogRite("Dut_Fm_SetRxAudioPath : 0x%08X\n",  
						Dut_Fm_SetRxAudioPath(AudioPath, I2sOperation, I2sMode));			 
			}
            break;

 	   case 156: // Dut_Fm_GetFmRxMuteMode
		   {  
				DebugLogRite("Dut_Fm_SetFmRxAudioSamplingRate : 0x%08X\n",  
						Dut_Fm_SetFmRxAudioSamplingRate(parm1, parm2)); 
			}
            break;
	   case 157: // Dut_Fm_SetFmRxMuteMode
		   {	 
				DebugLogRite("Dut_Fm_EnableFmAudioPauseMode : 0x%08X\n",  
						Dut_Fm_EnableFmAudioPauseMode(parm1));			 
			}
            break;

/*
 	   case 158: // Dut_Fm_GetFmRxMuteMode
			{  
				WORD PauseDuration=0;

				DebugLogRite("Dut_Fm_GetFmAudioPauseDuration : 0x%08X\n",  
						Dut_Fm_GetFmAudioPauseDuration(&PauseDuration));			 
				DebugLogRite("Pause Duration : %d ms\n", PauseDuration);
			}
            break;
*/
	   case 159: // Dut_Fm_SetFmRxMuteMode
			{	
				WORD PauseDuration=0;
				sscanf (line, "%x %d", &cmd, &PauseDuration );

				DebugLogRite("Dut_Fm_SetFmAudioPauseDuration : 0x%08X\n",  
						Dut_Fm_SetFmAudioPauseDuration(PauseDuration));			 
			}
            break;

/*
		case 160: // Dut_Fm_GetFmRxMuteMode
			{  
				WORD PauseLevel=0;

				DebugLogRite("Dut_Fm_GetFmAudioPauseLevel : 0x%08X\n",  
						Dut_Fm_GetFmAudioPauseLevel(&PauseLevel));			 
				DebugLogRite("Pause Level : %d ms\n", PauseLevel);
			}
            break;
*/
	   case 161: // Dut_Fm_SetFmAudioPauseLevel
			{	
				WORD PauseLevel=0;
				sscanf (line, "%x %d", &cmd, &PauseLevel );

				DebugLogRite("Dut_Fm_SetFmAudioPauseLevel : 0x%08X\n",  
						Dut_Fm_SetFmAudioPauseLevel(PauseLevel));			 
			}
            break;
		case 162: // Dut_Fm_GetFmSoftMute
			{  
				int enable=0;

				DebugLogRite("Dut_Fm_GetFmSoftMute : 0x%08X\n",  
						Dut_Fm_GetFmSoftMute(&enable));			 
				DebugLogRite("Soft Mute Mode: %d\n", enable);
			}
            break;
		case 163: // Dut_Fm_SetFmSoftMute
			{  
				int Enable=0;
				WORD MuteThreshold=119;
				BYTE AudioAttenuation=0;

				sscanf (line, "%x %d %d %d", &cmd, 
					&Enable, &MuteThreshold, &AudioAttenuation );

				DebugLogRite("Dut_Fm_SetFmSoftMute : 0x%08X\n",  
						Dut_Fm_SetFmSoftMute(Enable, MuteThreshold,
											AudioAttenuation));		 
			}
            break;
		case 164: // Dut_Fm_GetPllLockStatus
			{  
				DebugLogRite("Dut_Fm_GetPllLockStatus : 0x%08X\n",  
						Dut_Fm_GetPllLockStatus((int*)&parm1));		 
				
				DebugLogRite("Pll Lock Status: %d\n", parm1);
			}
            break;
		case 165: // Dut_Fm_GetFmI2SStatus
			{  
				int ChFreqInKHz=95600, Sampling=0, BClk_LrClk=0;

				sscanf (line, "%x %d", &cmd, 
					&ChFreqInKHz);

				DebugLogRite("Dut_Fm_GetFmI2SParameter : 0x%08X\n",  
						Dut_Fm_GetFmI2SParameter(ChFreqInKHz, &Sampling,
											&BClk_LrClk));		 
				DebugLogRite("Sampling Rate: %d\nBClk_LrClk: %d\n", Sampling, BClk_LrClk);
			}
            break;

		case 166: // Dut_Fm_GetFmAudioVolume
			{  
				int volume=0;

				DebugLogRite("Dut_Fm_GetFmAudioVolume : 0x%08X\n",  
						Dut_Fm_GetFmAudioVolume(&volume));			 
				DebugLogRite("Audio Volume: %d\n", volume);
			}
            break;
		case 167: // Dut_Fm_SetFmAudioVolume
			{  
				int volume=0;

				sscanf (line, "%x %d", &cmd, &volume);

				DebugLogRite("Dut_Fm_SetFmAudioVolume : 0x%08X\n",  
						Dut_Fm_SetFmAudioVolume(volume));		 
			}
            break;
			
			
/*
	   case 170: // 
			{  
				int RdsDataPath=0;

				DebugLogRite("Dut_Fm_GetFmRxRdsDataPath : 0x%08X\n",  
						Dut_Fm_GetFmRxRdsDataPath(&RdsDataPath));			 
				DebugLogRite("Rds Data Path : %d\n", RdsDataPath);
			}
            break;
	   case 171: // 
			{	
				int RdsDataPath=0;
				sscanf (line, "%x %d", &cmd, &RdsDataPath );

				DebugLogRite("Dut_Fm_SetFmRxRdsDataPath : 0x%08X\n",  
						Dut_Fm_SetFmRxRdsDataPath(RdsDataPath));			 
			}
            break;
*/
	   case 172: // 
			{	
				BYTE SyncStatus =0; 

				DebugLogRite("Dut_Fm_GetFmRxRdsSyncStatus : 0x%08X\n",  
						Dut_Fm_GetFmRxRdsSyncStatus(&SyncStatus ));			 
				DebugLogRite("Sync Status : %d\n", SyncStatus);
			}
            break;
	   case 173: // 
			{	
				DebugLogRite("Dut_Fm_FmRxRdsFlush : 0x%08X\n",  
						Dut_Fm_FmRxRdsFlush());			 
			}
            break;

	   case 174: // 
			{	
				BYTE Depth =0; 

				DebugLogRite("Dut_Fm_GetFmRxRdsMemDepth : 0x%08X\n",  
						Dut_Fm_GetFmRxRdsMemDepth(&Depth ));			 
				DebugLogRite("Mem Depth: %d\n", Depth);
			}
            break;
	   case 175: // 
			{	
				BYTE Depth=4; 
				sscanf (line, "%x %d", &cmd, &Depth);
				DebugLogRite("Dut_Fm_SetFmRxRdsMemDepth : 0x%08X\n",  
						Dut_Fm_SetFmRxRdsMemDepth(Depth));			 
			}
            break;

	   case 176: // 
			{	
				WORD BlockB =0; 

				DebugLogRite("Dut_Fm_GetFmRxRdsBlockB : 0x%08X\n",  
						Dut_Fm_GetFmRxRdsBlockB(&BlockB ));			 
				DebugLogRite("Block B : 0x%04X\n", BlockB);
			}
            break;
	   case 177: // 
			{	
				WORD BlockB=0; 
				sscanf (line, "%x %x", &cmd, &BlockB);
				DebugLogRite("Dut_Fm_SetFmRxRdsBlockB : 0x%08X\n",  
						Dut_Fm_SetFmRxRdsBlockB(BlockB));			 
			}
            break;
	   case 178: // 
			{	
				WORD BlockB =0; 

				DebugLogRite("Dut_Fm_GetFmRxRdsBlockBMask : 0x%08X\n",  
						Dut_Fm_GetFmRxRdsBlockBMask(&BlockB ));			 
				DebugLogRite("Block B Mask: 0x%04X\n", BlockB);
			}
            break;
	   case 179: // 
			{	
				WORD BlockB=0; 
				sscanf (line, "%x %x", &cmd, &BlockB);
				DebugLogRite("Dut_Fm_SetFmRxRdsBlockBMask : 0x%08X\n",  
						Dut_Fm_SetFmRxRdsBlockBMask(BlockB));			 
			}
            break;
	   case 180: // 
			{	
				WORD PiCode =0; 

				DebugLogRite("Dut_Fm_GetFmRxRdsPiCode : 0x%08X\n", 
						Dut_Fm_GetFmRxRdsPiCode(&PiCode ));			 
				DebugLogRite("PI Code : 0x%04X\n", PiCode);
			}
            break;
	   case 181: // 
			{	
				WORD PiCode=0x1234; 
				sscanf (line, "%x %x", &cmd, &PiCode);
				DebugLogRite("Dut_Fm_SetFmRxRdsPiCode : 0x%08X\n",  
						Dut_Fm_SetFmRxRdsPiCode(PiCode));			 
			}
            break;
	   case 182: // 
			{	
				WORD PiCode =0; 

				DebugLogRite("Dut_Fm_GetFmRxRdsPiCodeMask : 0x%08X\n",  
						Dut_Fm_GetFmRxRdsPiCodeMask(&PiCode ));			 
				DebugLogRite("PI Code : 0x%04X\n", PiCode);
			}
            break;
	   case 183: // 
			{	
				WORD PiCode=0xFFFF; 
				sscanf (line, "%x %x", &cmd, &PiCode);
				DebugLogRite("Dut_Fm_SetFmRxRdsPiCodeMask : 0x%08X\n",  
						Dut_Fm_SetFmRxRdsPiCodeMask(PiCode));			 
			}
            break;

	   case 184: // 
			{	
				int Mode =0; 

				DebugLogRite("Dut_Fm_GetFmRxRdsMode : 0x%08X\n",  
						Dut_Fm_GetFmRxRdsMode(&Mode ));			 
				DebugLogRite("Rds Mode : %d\n", Mode);
			}
            break;
	   case 185: // 
			{	
				int Mode =0; 
				sscanf (line, "%x %x", &cmd, &Mode);
				DebugLogRite("Dut_Fm_SetFmRxRdsMode : 0x%08X\n",  
						Dut_Fm_SetFmRxRdsMode(Mode));			 
			}
            break;

	   case 190: // Dut_Fm_GetFmTxOutputPwr
			{  
				BYTE TargetPwr=119;
 
				DebugLogRite("Dut_Fm_GetFmTxOutputPwr : 0x%08X\n",  
						Dut_Fm_GetFmTxOutputPwr(&TargetPwr));			 
				DebugLogRite("Tx Output Power : %d\n", TargetPwr);			 
			}
            break;

	   case 191: // Dut_Fm_SetFmTxOutputPwr
			{  
				BYTE TargetPwr=119;
				sscanf (line, "%x %d", &cmd, &TargetPwr );

				DebugLogRite("Dut_Fm_SetTxFmOutputPwr : 0x%08X\n",  
						Dut_Fm_SetFmTxOutputPwr(TargetPwr));			 
			}
            break;

// TBRM 	   case 191: // Dut_Fm_SetFmRxAudioDeemphasis
// TBRM 			{  int Preemphasis=0;
// TBRM 				sscanf (line, "%x %d", &cmd, &Preemphasis );
// TBRM 
// TBRM 				DebugLogRite("Dut_Fm_SetFmAudioPreemphasis : 0x%08X\n",  
// TBRM 						Dut_Fm_SetFmAudioPreemphasis(Preemphasis));			 
// TBRM 			}
// TBRM             break;
 
	   case 192: // Dut_Fm_SetFmRxAudioDeemphasis
			{  int DeviationInHz=75000;
				sscanf (line, "%x %d", &cmd, &DeviationInHz );

				DebugLogRite("Dut_Fm_SetFmFreqDeviation : 0x%08X\n",  
						Dut_Fm_SetFmFreqDeviation(DeviationInHz));			 
			}
            break;
// TBRM 	   case 193: // Dut_Fm_SetFmRxAudioDeemphasis
// TBRM 			{  int FreqInKHz=95600;
// TBRM 				sscanf (line, "%x %d", &cmd, &FreqInKHz );
// TBRM 
// TBRM 				DebugLogRite("Dut_Fm_SearchFmTxFreeCh : 0x%08X\n",  
// TBRM 						Dut_Fm_SearchFmTxFreeCh(FreqInKHz));			 
// TBRM 			}
// TBRM             break;
	   case 194: // Dut_Fm_SetFmTxMonoStereoMode
			{	
				int Mode=0;
				sscanf (line, "%x %d", &cmd, &Mode );

				DebugLogRite("Dut_Fm_SetFmTxMonoStereoMode : 0x%08X\n",  
						Dut_Fm_SetFmTxMonoStereoMode(Mode));			 
			}
            break;

	   case 195: // Dut_Fm_SetFmMute
			{	int Mute=0 ;	
				sscanf (line, "%x %d ", &cmd, &Mute);

				DebugLogRite("Dut_Fm_SetFmTxMuteMode : 0x%08X\n",  
						Dut_Fm_SetFmTxMuteMode(Mute  ));			 
			}
            break;
	   case 196: // Dut_Fm_ConfigFmAudioInput
			{	int InputGain=1, ActiveAudioCtrl=3; 
				sscanf (line, "%x %d %d", &cmd, 
					&InputGain, &ActiveAudioCtrl);

				DebugLogRite("Dut_Fm_ConfigFmAudioInput : 0x%08X\n",  
						Dut_Fm_ConfigFmAudioInput(InputGain,	ActiveAudioCtrl));			 
			}
            break;
	   case 197: // Dut_Fm_EnableFmTxAudioAGC
			{	
				 
				DebugLogRite("Dut_Fm_EnableFmTxAudioAGC : 0x%08X\n",  
						Dut_Fm_EnableFmTxAudioAGC(parm1));			 
			}
            break;

	   case 198: // Dut_Fm_ForceFmTxAudioGain
			{					 
				DWORD LprDev=22500, LmrDev=22500, PilotDev=6750, RdsDev=2000;

				sscanf (line, "%x %d %d %d %d", &cmd, 
					&LprDev,  &LmrDev, &PilotDev, &RdsDev);

				DebugLogRite("Dut_Fm_SetFmFreqDeviationExtended : 0x%08X\n",  
						Dut_Fm_SetFmFreqDeviationExtended( LprDev,   LmrDev,  PilotDev, RdsDev));			 
			}
            break;
// TBRM  	   case 199: // DUT_FM_API int STDCALL Dut_Fm_GetFmTxPowerMode(int *pMode_HighLow)
// TBRM 			{	
// TBRM 				DebugLogRite("Dut_Fm_GetFmTxPowerMode : 0x%08X\n",  
// TBRM 						Dut_Fm_GetFmTxPowerMode((int *)&parm2));
// TBRM  
// TBRM 				DebugLogRite("Tx Power Mode : %d\n", parm2);			 
// TBRM 			}
// TBRM             break;
	   case 200: // Dut_Fm_SetFmTxPowerMode
			{					 
				DebugLogRite("Dut_Fm_SetFmTxPowerMode : 0x%08X\n",  
						Dut_Fm_SetFmTxPowerMode(parm1));			 
			}
            break;
 
	   case 201: // Dut_Fm_SetFmTxRdsPiCode
			{					 
				WORD PiCode=0x1234; 
				sscanf (line, "%x %x", &cmd, &PiCode);
				DebugLogRite("Dut_Fm_SetFmTxRdsPiCode : 0x%08X\n",  
						Dut_Fm_SetFmTxRdsPiCode(PiCode));			 
			}
            break;
	   case 202: // Dut_Fm_SetFmTxRdsGroup
			{					 
 				DebugLogRite("Dut_Fm_SetFmTxRdsGroup : 0x%08X\n",  
						Dut_Fm_SetFmTxRdsGroup(parm1));			 
			}
            break;
	   case 203: // Dut_Fm_SetFmTxRdsPty
			{					 
 				DebugLogRite("Dut_Fm_SetFmTxRdsPty : 0x%08X\n",  
						Dut_Fm_SetFmTxRdsPty((BYTE)parm1));			 
			}
            break;
	   case 204: // Dut_Fm_SetFmTxRdsAfCode
			{					 
				BYTE AfCode=202; 
				sscanf (line, "%x %d", &cmd, &AfCode);
				DebugLogRite("Dut_Fm_SetFmTxRdsAfCode : 0x%08X\n",  
						Dut_Fm_SetFmTxRdsAfCode(AfCode));			 
			}
            break;
	   case 205: // Dut_Fm_SetFmTxRdsMode
			{					 
 				DebugLogRite("Dut_Fm_SetFmTxRdsMode : 0x%08X\n",  
						Dut_Fm_SetFmTxRdsMode(parm1));			 
			}
            break;
	   case 206: // Dut_Fm_SetFmTxRdsScrolling
			{					 
 				DebugLogRite("Dut_Fm_SetFmTxRdsScrolling : 0x%08X\n",  
						Dut_Fm_SetFmTxRdsScrolling(parm1));			 
			}
            break;
	   case 207: // Dut_Fm_SetFmTxRdsScrollingRate
			{					 
				BYTE Rate_CharPerScroll=1; 
				sscanf (line, "%x %d", &cmd, &Rate_CharPerScroll);
				DebugLogRite("Dut_Fm_SetFmTxRdsScrollingRate : 0x%08X\n",  
						Dut_Fm_SetFmTxRdsScrollingRate(Rate_CharPerScroll));			 
			}
            break;
	   case 208: // Dut_Fm_SetFmTxRdsDisplayLength
			{					 
				BYTE Len=8; 
				sscanf (line, "%x %d", &cmd, &Len);
				DebugLogRite("Dut_Fm_SetFmTxRdsDisplayLength : 0x%08X\n",  
						Dut_Fm_SetFmTxRdsDisplayLength(Len));			 
			}
            break;
	   case 209: // Dut_Fm_SetFmTxRdsToggleAB
			{					 
 				DebugLogRite("Dut_Fm_SetFmTxRdsToggleAB : 0x%08X\n",  
						Dut_Fm_SetFmTxRdsToggleAB());			 
			}
            break;
	   case 210: // Dut_Fm_SetFmTxRdsRepository
			{					 
 				DebugLogRite("Dut_Fm_SetFmTxRdsRepository : 0x%08X\n",  
						Dut_Fm_SetFmTxRdsRepository(parm1));			 
			}
            break;

	   case 220: // Dut_Fm_SetFmTxRdsRepository
			{	
				int chInKHz=95600;

				sscanf (line, "%x %d", &cmd,  &chInKHz );				 
 				
				DebugLogRite("Dut_Fm_ResetFmRdsBLER : 0x%08X\n",  
						Dut_Fm_ResetFmRdsBLER(chInKHz));			 
			}
            break;
	   case 221: // Dut_Fm_SetFmTxRdsRepository
			{	int SeqGood=0, SeqNoSync=0, Expected=0, SuccRate=0;
			
 				DebugLogRite("Dut_Fm_GetFmRdsBLER : 0x%08X\n",  
						Dut_Fm_GetFmRdsBLER(&SeqGood, &SeqNoSync, 
									&Expected, &SuccRate));			 
 				DebugLogRite(	"Seq Good %d\nSeq No Sync %d\n"
								"Expected %d\nSucc Rate %d\n",
								SeqGood, SeqNoSync, 
								Expected, SuccRate);			 
			}
            break;

#endif //#if defined (_FM_) 




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
	status = Dut_Fm_CloseDevice(theObj);
#endif //#if !defined(_BT_SDIO_) // && !defined(_WIFI_SDIO_) 


	return (status);

}

int mainFm(int argc, char* argv[])
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
	
	FmMenu(OptionMode);

	return 0;
}
