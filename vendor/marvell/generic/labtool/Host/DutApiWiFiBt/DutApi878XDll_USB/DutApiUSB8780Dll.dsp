# Microsoft Developer Studio Project File - Name="DutApiUSB8780Dll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=DutApiUSB8780Dll - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DutApiUSB8780Dll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DutApiUSB8780Dll.mak" CFG="DutApiUSB8780Dll - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DutApiUSB8780Dll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DutApiUSB8780Dll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DutApiUSB8780Dll - Win32 Release_Engineering" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DutApi83xxpDLL_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_USB_" /D "_CLIENT_BG_" /D "_W8780_" /D "DUTAPI8XXXDLL_EXPORTS" /D "BIG_ENDIAN" /D "_BT_" /D "_WLAN_" /D "_BT_USB_" /D "_FLASH_" /D "_BTRX_RSSIONLY_" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib MrvlMfg.lib /nologo /dll /profile /machine:I386 /libpath:"../"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy                                                    Release\*.dll                                                    ..\                                                   	copy                                                    Release\*.lib                                                    ..\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DutApiUSB8780Dll___Win32_Debug"
# PROP BASE Intermediate_Dir "DutApiUSB8780Dll___Win32_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_USB_" /D "_CLIENT_BG_" /D "_W8780_" /D "DUTAPI8XXXDLL_EXPORTS" /D "BIG_ENDIAN" /D "_BT_" /D "_BT_USB_" /D "_FLASH_" /FR /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_USB_" /D "_CLIENT_BG_" /D "_W8780_" /D "DUTAPI8XXXDLL_EXPORTS" /D "BIG_ENDIAN" /D "_BT_" /D "_WLAN_" /D "_BT_USB_" /D "_FLASH_" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib MrvlMfg.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib MrvlMfg.lib /nologo /dll /machine:I386 /libpath:"../"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy                                                    Debug\*.dll                                                    ..\                                                   	copy                                                    Debug\*.lib                                                    ..\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release_Engineering"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DutApiUSB8780Dll___Win32_Release_Engineering"
# PROP BASE Intermediate_Dir "DutApiUSB8780Dll___Win32_Release_Engineering"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_Engineering"
# PROP Intermediate_Dir "Release_Engineering"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_USB_" /D "_CLIENT_BG_" /D "_W8780_" /D "DUTAPI8XXXDLL_EXPORTS" /D "BIG_ENDIAN" /D "_BT_" /D "_WLAN_" /D "_BT_USB_" /D "_FLASH_" /D "_BTRX_RSSIONLY_" /FR /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_USB_" /D "_CLIENT_BG_" /D "_W8780_" /D "DUTAPI8XXXDLL_EXPORTS" /D "BIG_ENDIAN" /D "_BT_" /D "_WLAN_" /D "_BT_USB_" /D "_FLASH_" /D "_ENGINEERING_" /D "_BTRX_RSSIONLY_" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib MrvlMfg.lib /nologo /dll /profile /machine:I386 /libpath:"../"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib MrvlMfg.lib /nologo /dll /profile /machine:I386 /libpath:"../"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy                                                    Release_Engineering\*.dll                                                    ..\                                                   	copy                                                    Release_Engineering\*.lib                                                    ..\ 
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "DutApiUSB8780Dll - Win32 Release"
# Name "DutApiUSB8780Dll - Win32 Debug"
# Name "DutApiUSB8780Dll - Win32 Release_Engineering"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\DutApi8xxxDll\DutApi8xxxDll.cpp
# End Source File
# Begin Source File

SOURCE=..\DutBtApi8xxxDll\DutBtApi8xxxDll.cpp

!IF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\DutWlanApi8xxxDll\DutWlanApi8xxxDll.cpp

!IF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\DutShareApi8xxxDll\StdAfx.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\DutApi.hc
# End Source File
# Begin Source File

SOURCE=..\DutApi8xxxDll.h
# End Source File
# Begin Source File

SOURCE=..\DutBtApi8xxxDll.h
# End Source File
# Begin Source File

SOURCE=..\DutWlanApi8xxxDll.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\DutApi8xxxDll\DutApiDll.rc
# End Source File
# Begin Source File

SOURCE=..\DutApi8xxxDll\DutDllVerNo.h
# End Source File
# End Group
# Begin Group "DutWlan"

# PROP Default_Filter ""
# Begin Group "UDP"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\DutIf\DutIf_UdpIpClss.cpp

!IF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\DutIf\DutIf_UdpIpClss.h

!IF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Lib\WS2_32.LIB"

!IF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "NDIS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\DutIf\DutIf_Ndis.cpp

!IF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\DutIf\DutIf_Ndis.h

!IF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "SDIO"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\DutIf\DutIf_SdioAdapter.cpp

!IF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\DutIf\DutIf_SdioAdapter.h

!IF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\DutIf\public.h

!IF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE="c:\Program Files\Microsoft Visual Studio\VC98\Lib\SETUPAPI.LIB"

!IF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "USB"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\DutIf\Dsdef.h
# End Source File
# Begin Source File

SOURCE=..\DutIf\DutIf_DvcIoCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\DutIf\DutIf_DvcIoCtrl.h
# End Source File
# Begin Source File

SOURCE=..\DutIf\MrvlMfg.lib
# End Source File
# End Group
# Begin Source File

SOURCE=..\DutWlanApi8xxxDll\DutIf_IfClss.cpp
# End Source File
# Begin Source File

SOURCE=..\DutWlanApi8xxxDll\DutIf_IfClss.h
# End Source File
# Begin Source File

SOURCE=..\DutWlanApi8xxxDll\DutWlanApi.cpp
# End Source File
# Begin Source File

SOURCE=..\DutWlanApi.hc
# End Source File
# Begin Source File

SOURCE=..\DutWlanApi8xxxDll\DutWlanApiClss.cpp
# End Source File
# Begin Source File

SOURCE=..\DutWlanApi8xxxDll\DutWlanApiClss.h
# End Source File
# Begin Source File

SOURCE=..\DutWlanApi8xxxDll\DutWlanApiClss.hd
# End Source File
# Begin Source File

SOURCE=..\DutWlanApi8xxxDll\mfgcmd.h
# End Source File
# Begin Source File

SOURCE=..\DutIf\MrvlMfg.h
# End Source File
# Begin Source File

SOURCE=..\DutWlanApi8xxxDll\WlanMfgCmd.h
# End Source File
# End Group
# Begin Group "DutBt"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\DutBtApi8xxxDll\BtMfgCmd.h
# End Source File
# Begin Source File

SOURCE=..\DutBtApi8xxxDll\DutBrf.cpp
# End Source File
# Begin Source File

SOURCE=..\DutBtApi8xxxDll\DutBrf.hd
# End Source File
# Begin Source File

SOURCE=..\DutBtApi.hc
# End Source File
# Begin Source File

SOURCE=..\DutBtApi8xxxDll\DutBtApiClss.cpp
# End Source File
# Begin Source File

SOURCE=..\DutBtApi8xxxDll\DutBtApiClss.h
# End Source File
# Begin Source File

SOURCE=..\DutBtApi8xxxDll\DutBtApiCombCmds.cpp

!IF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\DutBtApi8xxxDll\DutBtCalFunc.cpp

!IF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\DutBtApi8xxxDll\DutBtIf_IfClss.cpp
# End Source File
# Begin Source File

SOURCE=..\DutBtApi8xxxDll\DutBtu.cpp
# End Source File
# Begin Source File

SOURCE=..\DutBtApi8xxxDll\DutBtu.hd
# End Source File
# Begin Source File

SOURCE=..\DutBtApi8xxxDll\DutEfuse.cpp

!IF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\DutBtApi8xxxDll\hci_opcodes.h
# End Source File
# Begin Source File

SOURCE=..\DutBtApi8xxxDll\HciCtrl.h
# End Source File
# Begin Source File

SOURCE=..\DutBtApi8xxxDll\HciStruct.h
# End Source File
# Begin Source File

SOURCE=..\DutBtApi8xxxDll\UartErrCode.h
# End Source File
# End Group
# Begin Group "DutCommon"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\DutCommon\Clss_os.h
# End Source File
# Begin Source File

SOURCE=..\..\DutCommon\Dut_error.hc
# End Source File
# Begin Source File

SOURCE=..\..\DutCommon\mathUtility.c
# End Source File
# Begin Source File

SOURCE=..\..\DutCommon\mathUtility.h
# End Source File
# Begin Source File

SOURCE=..\..\DutCommon\utilities.cpp
# End Source File
# Begin Source File

SOURCE=..\..\DutCommon\utilities.h
# End Source File
# Begin Source File

SOURCE=..\..\DutCommon\utilities_os.cpp

!IF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release"

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Debug"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release_Engineering"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\DutCommon\utilities_os.h
# End Source File
# Begin Source File

SOURCE=..\..\DutCommon\utility.cpp
# End Source File
# Begin Source File

SOURCE=..\..\DutCommon\utility.h
# End Source File
# Begin Source File

SOURCE=.\xmodem.h
# End Source File
# End Group
# Begin Group "GruBt"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\GruBtApi86xxDll\DutBtApi86xxLinkTest.cpp
# End Source File
# Begin Source File

SOURCE=..\GruBtApi86xxDll\GruBtApiClss.cpp
# End Source File
# Begin Source File

SOURCE=..\GruBtApi86xxDll\GruBtApiClss.h
# End Source File
# Begin Source File

SOURCE=..\GruBtApi86xxDll\GruDllVerNo.h
# End Source File
# End Group
# Begin Group "DutSharedApi"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\DutShareApi8xxxDll\DutShared_Caldata.hd
# End Source File
# Begin Source File

SOURCE=..\DutShareApi8xxxDll\DutShared_spiAccess.cpp
# End Source File
# Begin Source File

SOURCE=..\DutShareApi8xxxDll\DutSharedCal.cpp
# End Source File
# Begin Source File

SOURCE=..\DutShareApi8xxxDll\DutSharedCalData.cpp
# End Source File
# Begin Source File

SOURCE=..\DutShareApi8xxxDll\DutSharedCalFileAccess.cpp

!IF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiUSB8780Dll - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\DutShareApi8xxxDll\DutSharedCalFilehandle.cpp
# End Source File
# Begin Source File

SOURCE=..\DutShareApi8xxxDll\DutSharedCalTxt.cpp
# End Source File
# Begin Source File

SOURCE=..\DutShareApi8xxxDll\DutSharedClss.h
# End Source File
# Begin Source File

SOURCE=..\DutShareApi8xxxDll\DutSpiImage.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\DutBtApi8xxxDll\DutBtIf_IfClss.h
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
