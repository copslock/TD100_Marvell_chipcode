#ifdef _HCI_PROTOCAL_

#include "../../DutCommon/Clss_os.h"
#include "../../DutCommon/utility.h"
 

#include GRUCLASS_BT_H_PATH		//	"GruBtApiClss.h"
//#include "DutBtApiClss.h"
#include DUTDLL_VER_H_PATH		//	"../DutApi878XDll/DutDllVerNo.h"	 
#include DUTCLASS_BT_CMDH_PATH	//	"../DutBtApi878XDll/BtMfgCmd.h"
 
//#include "../DutBtApi878XDll/DutBrf.hd"
#include DUTHCI_BT_LL_CTRL_H_PATH		//	"../DutBtApi878XDll/HciCtrl.h"

 
extern  WORD slotDur[4];
 
extern  int maxPayloadIbByte[NUM_BT_DATARATE][NUM_BT_HOPSLOT];
  
#ifndef _NO_CLASS_
GruBtApiClssHeader GruBtApiClss()
{
}

GruBtApiClssHeader ~GruBtApiClss()
{
}
#endif	//#ifndef _NO_CLASS_
 
int GruBtApiClssHeader Version(void)
{ 
 	return	MFG_VERSION(DUT_DLL_VERSION_MAJOR1, DUT_DLL_VERSION_MAJOR2,
						DUT_DLL_VERSION_MINOR1, DUT_DLL_VERSION_MINOR2);
}

char* GruBtApiClssHeader About(void)
{
	char tempDate[50]="", tempTime[50]="";
	char AboutFormat[512]=
			"Name:\t\tDutApiClass\n"
#ifdef _UDP_
			"Interface:\t""HCI_UART\n" 
#elif _BT_USB_
			"Interface:\t""USB\n"
#else 
			"Interface:\t""unknown\n"
#endif	
			"Version:\t%d.%d.%d.%02d\n" 
 			"Date:\t\t%s (%s)\n\n" 			
 			"Note:\t\t\n" 

#ifdef STATIC_OBJ
			"Static Object\n" 
#endif			
			;
 
	static char AboutString[512]="";
 
	sprintf(AboutString, AboutFormat, 
			DUT_DLL_VERSION_MAJOR1,	
			DUT_DLL_VERSION_MAJOR2,
			DUT_DLL_VERSION_MINOR1, 
			DUT_DLL_VERSION_MINOR2,
			__DATE__, __TIME__);

	return AboutString; 
}

 




int GruBtApiClssHeader OpenDevice( )
{
	int status=0;
/*
#ifdef _UART_
	char GruPortName[255]="";
 	char temp[_MAX_PATH]="";
	int Gru_BaudRate=0,Gru_RtsFlowControl=0, Gru_OverLapped=0;
 	int Gru_UartFwDl=0;
	char Gru_FwImageName[255]="";
	int Gru_BootBaudRate=0;
	int Gru_BootRtsFlowCtrl=0;
	int Gru_BootSignal=0;
	int Gru_BootSignalWait=0;
	int tempInt=0;
  
 	if(getcwd(temp, _MAX_PATH) != NULL)  
 	strcat(temp, "/setup.ini");
 	else
	strcpy(temp, "setup.ini");

 
  	GetPrivateProfileString("GoldenBT","UartPort","COM1",
		GruPortName, sizeof(GruPortName), temp); 
 	
	Gru_BaudRate	= GetPrivateProfileInt("GoldenBT","UartBaudRate", CBR_115200, temp); 
	Gru_RtsFlowControl	= GetPrivateProfileInt("GoldenBT","UartRtsFlowControl", RTS_CONTROL_HANDSHAKE, temp); 
	Gru_OverLapped 	= GetPrivateProfileInt("GoldenBT","UartOverLapped", false, temp); 

	if(GetDebugLogFlag())
	{
  		DebugLogRite( "GruPortName %s\n", GruPortName);
	}

	GetFwDlParm(&Gru_UartFwDl,	Gru_FwImageName,
			&Gru_BootBaudRate,	&Gru_BootRtsFlowCtrl,
			&Gru_BootSignal,	&Gru_BootSignalWait);

	Gru_UartFwDl			= GetPrivateProfileInt("GoldenBT","UartFwDl", 0, temp);
  	GetPrivateProfileString	("GoldenBT","FwImageName",Gru_FwImageName,Gru_FwImageName, 
							sizeof(Gru_FwImageName), temp); 
 	Gru_BootBaudRate		= GetPrivateProfileInt("GoldenBT","BootBaudRate", Gru_BootBaudRate, temp); 
	Gru_BootRtsFlowCtrl	= GetPrivateProfileInt("GoldenBT","BootRtsFlowCtrl", Gru_BootRtsFlowCtrl, temp); 
// 	Gru_BootSignal		= NAK; 
	tempInt	= GetPrivateProfileInt("GoldenBT","BootSignalWait", Gru_BootSignalWait, temp); 

	if(tempInt != Gru_BootSignalWait)
		Gru_BootSignalWait	=tempInt/1000;

	
	//DutBtIfClssHeader  
	SetUartPortName(GruPortName);
	SetUartParm(Gru_BaudRate, Gru_RtsFlowControl, Gru_OverLapped);

	SetFwDlParm(Gru_UartFwDl,	Gru_FwImageName,
			Gru_BootBaudRate,	Gru_BootRtsFlowCtrl,
			Gru_BootSignal,		Gru_BootSignalWait);

#endif // #ifdef _UART_
*/

#ifdef	 _IF_SDIO_
	status = DutBtApiClssHeader OpenDevice();
//	status = DutBtIfClssHeader OpenDevice();
#endif  //_IF_UDP_ 
	return status;
}


////////////////////////////////
#endif //#ifdef _HCI_PROTOCAL_
