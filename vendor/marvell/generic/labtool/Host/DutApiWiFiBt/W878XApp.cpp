/** @file W878XApp.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#include <time.h>
#include "../DutCommon/Clss_os.h"
#include "../DutCommon/Dut_error.hc"
#include "../DutCommon/utilities.h"
#include "../DutCommon/utilities_os.h"
 
#include "W878XApp.h"
#if defined (_WLAN_)
#include "DutWlanApi878XDll.h"
#endif // #if defined(_WLAN_)

#if defined(_BT_)
#include "DutBtApi878XDll.h"
#endif // #if defined(_BT_)

#include "DutAppVerNo.h"



#if defined (_WLAN_)
extern int WifiMenu(int OptionMode, void **theObj=NULL);
#endif // #if defined(_WLAN_)

#if defined(_BT_)
extern int BTMenu(int OptionMode, void **theObj=NULL);
#endif // #if defined(_BT_)

#if defined(_FM_)
extern int FmMenu(int OptionMode, void **theObj=NULL);
#endif
char* labtool_About(void)
{
	char tempDate[50]="", tempTime[50]="";
	char AboutFormat[512]=
			"Name:\t\tDut labtool\n"
			"Interface:\t"
#if defined (_BT_UART_)
			"HCI_UART\n" 
#else //#if defined (_BT_UART_)
			"Unknown\n" 
#endif //#if defined (_BT_UART_)

			
			"Version:\t%d.%d.%d.%02d\n" 
 			"Date:\t\t%s (%s)\n\n" 			
 			"Note:\t\t\n" 

#ifdef STATIC_OBJ
			"Static Object\n" 
#endif			
			;
 
	static char AboutString[512]="";
 
	sprintf(AboutString, AboutFormat, 
			DUTCTRL_VERSION_MAJOR1,	
			DUTCTRL_VERSION_MAJOR2,
			DUTCTRL_VERSION_MINOR1, 
			DUTCTRL_VERSION_MINOR2,
			__DATE__, __TIME__);

	return AboutString; 
}
 

int main(int argc, char **argv)
{		
enum  
{
	MENU_BLANK, 

#if defined(_WLAN_)
	MENU_WIFI, 
#endif	//#if defined(_WLAN_)

#if defined(_BT_)
	MENU_BT, 
#endif	//#if defined(_BT_)

#if defined(_FM_)
	MENU_FM, 
#endif	//#if defined(_FM_)
	
};


	unsigned long	cmd=0, parm1=0, parm2=0, cnt=0;
	int OptionMode=SUPERUSER;
	void *oneObj=NULL;
#if !defined (_BT_SDIO_)
	void *otherObjs=NULL;
#endif // #if defined (_BT_SDIO_)
    int status=0;

	char line[MAX_LENGTH]="";

	if(argc ==1) 
	{
		OptionMode = ALL; // basic basic RF 
	}
	else
	{
		if(1)
		{
		if(!strcmp(argv[1], "-r")) OptionMode = EXT_RF; // externded RF 
		if(!strcmp(argv[1], "-m")) OptionMode = BASIC_SPI; // basic basic RF and Flash
		if(!strcmp(argv[1], "-a")) OptionMode = ALL; // basic basic RF and Flash
		if(!strcmp(argv[1], "-s")) OptionMode = SUPERUSER; // basic basic RF and Flash
#if !defined (_BT_SDIO_)
		if(!strcmp(argv[1], "-u")) OptionMode = UART_ONLY; // No FW DL by open SDIO port
#endif // #if !defined (_BT_SDIO_)
		}
		else
		{
			OptionMode = ALL;
		}
	}

#if defined(_WLAN_)
#if defined(_WIFI_SDIO_) //&& defined(_BT_SDIO_)
	if(UART_ONLY !=	OptionMode) 
		status = DutIf_InitConnection(&oneObj);
	if (status) 
	{
		DebugLogRite("Fail to initialized the sdio interface: 0x%x\n",status);
		return (status);
	}
#endif
#else //#if defined(_WLAN_)
	status =   	status = Dut_Bt_OpenDevice(&otherObjs);
	if (status) 
	{
		DebugLogRite("Fail to initialized the UART interface: 0x%x\n",status);
		return (status);
	}
#if defined (_BT_UART_)
	if(UART_ONLY !=	OptionMode) 
		Dut_Bt_UartFwDl();
#endif // #if defined (_BT_UART_)


#endif	//#if defined(_WLAN_)

	do
	{
		DebugLogRite ("%s\n",labtool_About());

#if defined(_WLAN_)
		DebugLogRite ("%d. =========WiFi tool=============\n", MENU_WIFI);
#endif	//#if defined(_WLAN_)

#if !defined(_W8782_)
#if defined(_BT_)
		DebugLogRite ("%d. =========BT   tool============= \n", MENU_BT);
#endif // #if defined(_BT_)

#if defined(_FM_)
		DebugLogRite ("%d. =========FM   tool============= \n", MENU_FM);
#endif
#endif //#if !defined(_W8782_)
		DebugLogRite ("99.Exit \n");
		cmd = 0;
		parm1 = 0;
        parm2 = 0;

        DebugLogRite ("Enter option: ");

		fgets(line, sizeof(line), stdin);

		// print the cmd in Test.txt file
		PrintOnScreen(0);
		DebugLogRite("%s\n", line);
        sscanf (line, "%d %x %x",&cmd, &parm1, &parm2);
        switch (cmd)
		{
#if defined (_WLAN_)
		case MENU_WIFI:
			WifiMenu(OptionMode, &oneObj);
			break;
#endif // #if defined(_WLAN_)

#if defined(_BT_)
		case MENU_BT:
#if defined (_BT_SDIO_)
            BTMenu(OptionMode, &oneObj);
#else // #if defined (_BT_SDIO_)
            BTMenu(OptionMode, &otherObjs);
#endif //#if defined (_BT_SDIO_)
            break;
#endif // #if defined(_BT_)

#if defined(_FM_)
		case MENU_FM:
#if defined (_BT_SDIO_)
            FmMenu(OptionMode, &oneObj);
#else // #if defined (_BT_SDIO_)
            FmMenu(OptionMode, &otherObjs);
#endif //#if defined (_BT_SDIO_)
			break;
#endif // #if defined(_FM_)

		case 99:
			DebugLogRite("Exiting\n");
			break;		
		default:
			DebugLogRite("Unknown Option\n");
			break;
        }

//       if(99 != cmd )
//		{
//			DebugLogRite ("Press [Enter] key to continue ... ");
// 			fgets(line, sizeof(line), stdin);
//		}


    } while (99 != cmd);

#if defined(_WLAN_)

#if defined(_WIFI_SDIO_) // && defined(_BT_SDIO_)
	if (oneObj)
	{
 		status = DutIf_Disconnection(&oneObj);
		DebugLogRite(" DutIf_Disconnection: %d\n", status);
	}
#endif

#else //#if defined(_WLAN_)
	if (otherObjs)
	{
 		status = Dut_Bt_CloseDevice(&otherObjs);
		DebugLogRite(" Dut_Bt_CloseDevice: %d\n", status);
	}

#endif	//#if defined(_WLAN_)

	return 0;

}

