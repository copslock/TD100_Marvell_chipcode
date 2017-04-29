/** @file Clss_os.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#if defined(WIN32) || defined(NDIS51_MINIPORT) || defined(NDIS50_MINIPORT) || defined(_LINUX_)
#pragma pack(1) 

#ifdef _LINUX_
#define PACK __attribute__ ((packed))
#else // #ifdef _LINUX_
#define PACK
#endif // #ifdef _LINUX_

#else //#if defined(WIN32) || defined(NDIS51_MINIPORT) || defined(NDIS50_MINIPORT) || defined(_LINUX_)
#include "win.h"  
#endif //#if defined(WIN32) || defined(NDIS51_MINIPORT) || defined(NDIS50_MINIPORT) || defined(_LINUX_)


#define DUTDLL_VER_H_PATH			"../DutApi878XDll/DutDllVerNo.h"
#define DUTDLL_SHARED_H_PATH		"../DutApi878XDll.h"
#define DUTCLASS_SHARED_H_PATH		"../DutShareApi878XDll/DutSharedClss.h"
#define DUTCLASS_SHARED_STDH_PATH	"../DutShareApi878XDll/stdAfx.h"
 
#define DUTDLL_WLAN_HC_PATH			"../DutWlanApi.hc"
#define DUTCLASS_WLAN_HD_PATH		"../DutWlanApi878XDll/DutWlanApiClss.hd"

//#ifdef _WLAN_ 

#define DUTDLL_WLAN_H_PATH			"../DutWlanApi878XDll.h"
#define DUTCLASS_WLAN_H_PATH		"../DutWlanApi878XDll/DutWlanApiClss.h"


#define DUTIF_WLAN_H_PATH			"../DutWlanApi878XDll/DutIf_IfClss.h" 
//#endif  //#ifdef _WLAN_


#ifdef _BT_
#define DUTDLL_BT_H_PATH			"../DutBtApi878XDll.h"
#define DUTDLL_BT_HC_PATH			"../DutBtApi.hc"
#define DUTCLASS_BT_H_PATH			"../DutBtApi878XDll/DutBtApiClss.h"
#define DUTCLASS_BT_CMDH_PATH		"../DutBtApi878XDll/BtMfgCmd.h"

#define DUTHCI_BT_LL_STRUCT_H_PATH		"../DutBtApi878XDll/HciStruct.h"
#define DUTHCI_BT_LL_OPCODE_H_PATH		"../DutBtApi878XDll/hci_opcodes.h"
#define DUTHCI_BT_LL_UARTERR_H_PATH		"../DutBtApi878XDll/UartErrCode.h"
#define DUTHCI_BT_LL_CTRL_H_PATH		"../DutBtApi878XDll/HciCtrl.h"
#define DUTHCI_BT_LL_XMODEM_H_PATH		"../DutBtApi878XDll/xmodem.h"
#define DUTHCI_BT_LL_UARTCHECK_H_PATH	"../UartPortCheck.h"

#define GRUCLASS_BT_H_PATH				"../GruBtApi878xDll/GruBtApiClss.h"

#endif  //#ifdef _BT_


#ifndef _LINUX_
//////////////////////////////////////////
//		NOT LINUX
//////////////////////////////////////////
 
#include "stdafx.h" 
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <direct.h>
#include <stdio.h>
#include <math.h>
#include <tchar.h> 
#include <conio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "Dut_error.hc"

//#ifndef STDCALL
#define STDCALL //__stdcall
//#endif  //#ifndef STDCALL

#ifdef _WIN_NDIS_
#include "../../windis5/include/WINDIS.h"
#endif // #ifdef _WIN_NDIS_
 
#ifdef _WLAN_
#define DutWlanApiClssHeader		DutWlanApiClss::  
#define DutIf_SdioAdptHeader		DutIf_SdioAdpt::	
#define DutIf_UDPAdptHeader			DutIf_UdpIp::	

#endif  //#ifdef _WLAN_

#ifdef _BT_
#define DutBtApiClssHeader			DutBtApiClss::
#define DutBtIfClssHeader			DutBtIf_If::
#define DutBtLowLevelClssHeader		DutBtIf_HciUart::
#define GruBtApiClssHeader			GruBtApiClss::
#define BtLinkTestClssHeader		BtLinkTestBtApiClss::
#define DutSpiApiClssHeader			DutBtApiClssHeader //DutSpiApiClss::


#endif  //#ifdef _BT_
#define DutSharedClssHeader 			DutSharedClss::
#define DutSharedClssObj 			DutSharedClss


//////////////////////////////////////////
//		NOT LINUX
//////////////////////////////////////////



#else	//_LINUX_
//////////////////////////////////////////
//		LINUX
//////////////////////////////////////////
#define STDCALL 
#ifdef _WLAN_
#define DutApiClssHeader 
#define DUTDLL_WLAN_H_PATH			"../DutWlanApi878XDll.h"
#define DUTDLL_WLAN_HC_PATH			"../DutWlanApi.hc"
#define DUTCLASS_WLAN_H_PATH		"../DutWlanApi878XDll/DutWlanApiClss.h"
#define DUTCLASS_WLAN_HD_PATH		"../DutWlanApi878XDll/DutWlanApiClss.hd"


#define DUTIF_WLAN_H_PATH			"../DutWlanApi878XDll/DutIf_IfClss.h" 
#endif  //#ifdef _WLAN_

#ifdef _BT_

#define DUTDLL_BT_H_PATH			"../DutBtApi878XDll.h"
#define DUTDLL_BT_HC_PATH			"../DutBtApi.hc"
#define DUTCLASS_BT_H_PATH			"../DutBtApi878XDll/DutBtApiClss.h"
#define DUTCLASS_BT_CMDH_PATH		"../DutBtApi878XDll/BtMfgCmd.h"

#define DUTHCI_BT_LL_STRUCT_H_PATH		"../DutBtApi878XDll/HciStruct.h"
#define DUTHCI_BT_LL_OPCODE_H_PATH		"../DutBtApi878XDll/hci_opcodes.h"
#define DUTHCI_BT_LL_UARTERR_H_PATH		"../DutBtApi878XDll/UartErrCode.h"
#define DUTHCI_BT_LL_CTRL_H_PATH		"../DutBtApi878XDll/HciCtrl.h"

#define GRUCLASS_BT_H_PATH				"../GruBtApi878xDll/GruBtApiClss.h"

#endif // #ifdef _BT_
 
#ifdef _NO_CLASS_

#ifdef _WLAN_
#define DutWlanApiClssHeader		  

#endif  //#ifdef _WLAN_

#ifdef _BT_
#define DutBtApiClssHeader		
#define DutBtIfClssHeader		
#define DutBtLowLevelClssHeader 
#define GruBtApiClssHeader		
#define BtLinkTestClssHeader	
#define DutSpiApiClssHeader	

#define DutIf_SdioAdptHeader	
#endif //#ifdef _BT_

#define DutSharedClssHeader

#define DutSharedClssObj 			void
#else //_NO_CLASS_

#ifdef _WLAN_
#define DutWlanApiClssHeader		DutWlanApiClss::  

#endif  //#ifdef _WLAN_

#ifdef _BT_
#define DutBtApiClssHeader		DutBtApiClss::
#define DutBtIfClssHeader		DutBtIf_If::
#define DutBtLowLevelClssHeader 	DutBtIf_HciUart::
#define GruBtApiClssHeader		GruBtApiClss::
#define BtLinkTestClssHeader		BtLinkTestBtApiClss::
#define DutSpiApiClssHeader		DutBtApiClssHeader //DutSpiApiClss::

#define DutIf_SdioAdptHeader		DutIf_SdioAdpt::	

#endif //#ifdef _BT_

#define DutSharedClssHeader DutSharedClss::

#define DutSharedClssObj 			DutSharedClss
//#define DUT_BTDll_H_PATH  "../DutBtApi878XDll/DutWlanApiClss.h"

#endif //_NO_CLASS_
 
//#endif  //#ifdef _BT_
#include <stdarg.h>
#ifndef _LINUX_FEDORA_
#include <malloc/malloc.h>
#else //#ifndef _LINUX_FEDORA_
#include <sys/resource.h> 
#include <malloc.h> 
#endif //#ifndef _LINUX_FEDORA_

#include <stdarg.h>
#include <unistd.h> 
#include <stdlib.h>
#include <stdio.h>
#include <math.h>  
#include <sys/stat.h> 
#include <sys/types.h>

#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h> 
#include <sys/ioctl.h>
#include <termios.h>
#include <limits.h>
#include <sys/times.h> 
#include <pthread.h>
#include <net/if.h>
#include "Dut_error.hc"
#include "utilities_os.h"
  

#define ERROR_SUCCESS			(0)
#define NO_ERROR				(0)
#define INVALID_HANDLE_VALUE	(-1)

#define ERROR_INVALID_HANDLE	(6)  
#define ERROR_INVALID_DATA		(0x8001000FL)  
#define ERROR_WRITE_FAULT		(88L)  
#define ERROR_GEN_FAILURE		(31L)  


#define TRUE			(1)
#define FALSE			(0)
#ifndef _MAX_PATH
#define _MAX_PATH		(256)
#endif // #ifndef _MAX_PATH
#ifndef _MAX_FNAME
#define _MAX_FNAME		(256)
#endif // #ifndef _MAX_FNAME

typedef unsigned long HANDLE;
typedef unsigned char* PUCHAR;
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef unsigned long ULONG;
typedef unsigned int UINT;
typedef unsigned int BOOL;
typedef unsigned char UCHAR;
typedef unsigned short USHORT;
typedef unsigned int BOOLEAN;


//#define PACK_START
//#define PACK_END __attribute__ ((packed))


#define Sleep(a) (sleep(a/1000)) // a in mili-Second

//////////////////////////////////////////
//		LINUX
//////////////////////////////////////////

#endif //_LINUX_

