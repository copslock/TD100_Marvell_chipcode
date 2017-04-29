/** @file xModemVer.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

//#include "XMODEMCtrlVer.h"


#define XMODEMCTRL_VERSION_MAJOR1		1
#define XMODEMCTRL_VERSION_MAJOR2		0
#define XMODEMCTRL_VERSION_MINOR1		0
#define XMODEMCTRL_VERSION_MINOR2		0

//#define VERSTRING (0x30|XMODEMCTRL_VERSION_MAJOR1) "," "2" "," "0" ","  "," 0x0
//#define VERSTRING					0x31","0x32","0x30","0x30"\0"
#define XMODEMCTRL_VERSTRING	"1,0,0,0\0"

#define XMODEM_DLL_FILEVER				(XMODEMCTRL_VERSION_MAJOR1),(XMODEMCTRL_VERSION_MAJOR2),(XMODEMCTRL_VERSION_MINOR1),(XMODEMCTRL_VERSION_MINOR2) //123,45,67,89//1,2,1,0
#define XMODEM_DLL_PRODUCTVER			(XMODEMCTRL_VERSION_MAJOR1),(XMODEMCTRL_VERSION_MAJOR2),(XMODEMCTRL_VERSION_MINOR1),(XMODEMCTRL_VERSION_MINOR2) //123,45,67,89//1,2,1,0
#define XMODEM_DLL_FILEVER_STRING		XMODEMCTRL_VERSTRING
#define XMODEM_DLL_PRODUCTVER_STRING	XMODEMCTRL_VERSTRING  

#define XMODEM_DLL_COMMENTSTRING			"XMODEM Control Low Level Lib\0"
#define XMODEM_DLL_SPECIALBUILDSTRING		"\0"  
#define XMODEM_DLL_PRODUCTNAMESTRING		"BT Product\0"
#define XMODEM_DLL_PRIVATEBUILDSTRING		"Preliminary\0"
#define XMODEM_DLL_FILEDESCRIPTIONSTRING	"\0"
