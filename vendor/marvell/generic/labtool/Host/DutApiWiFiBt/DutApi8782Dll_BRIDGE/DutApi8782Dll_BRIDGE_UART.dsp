# Microsoft Developer Studio Project File - Name="DutApi8782Dll_BRIDGE_UART" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=DutApi8782Dll_BRIDGE_UART - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DutApi8782Dll_BRIDGE_UART.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DutApi8782Dll_BRIDGE_UART.mak" CFG="DutApi8782Dll_BRIDGE_UART - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DutApi8782Dll_BRIDGE_UART - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DutApi8782Dll_BRIDGE_UART - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DutApi8782Dll_BRIDGE_UART - Win32 Release_Engineering" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DUTAPI8787DLL_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "_ENGINEERING__" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DUTAPI8787DLL_EXPORTS" /D "_CLIENT_BG_" /D "_MIMO_" /D "_WLAN_" /D "BIG_ENDIAN" /D "_FLASH__" /D "_FM_" /D "_IF_UART_" /D "_BT_REMOTE_UART_" /D "_W8782_" /D "_HCI_PROTOCAL_" /D "_BT_" /D "_CAL_REV_D_" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /profile /machine:I386 /libpath:"../"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy                                                                            Release\*.dll                                                                            ..\                                                                           	copy                                                                            Release\*.lib                                                                            ..\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DutApiSD8688Dll___Win32_Debug"
# PROP BASE Intermediate_Dir "DutApiSD8688Dll___Win32_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_USB_" /D "_CLIENT_BG_" /D "_MIMO_" /D "_W8782_" /D "DUTAPI8787DLL_EXPORTS" /D "BIG_ENDIAN" /D "_BT_" /D "_BT_UART_" /D "_FLASH_" /FR /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "_DEBUG" /D "_ENGINEERING_" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DUTAPI8787DLL_EXPORTS" /D "_CLIENT_BG_" /D "_MIMO_" /D "_WLAN_" /D "BIG_ENDIAN" /D "_FLASH__" /D "_FM_" /D "_IF_UART_" /D "_BT_REMOTE_UART_" /D "_W8782_" /D "_HCI_PROTOCAL_" /D "_BT_" /D "_CAL_REV_D_" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib MrvlMfg.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /libpath:"../"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy                                                                            Debug\*.dll                                                                            ..\                                                                           	copy                                                                            Debug\*.lib                                                                            ..\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release_Engineering"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DutApiSD8688Dll___Win32_Release_Engineering"
# PROP BASE Intermediate_Dir "DutApiSD8688Dll___Win32_Release_Engineering"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_Engineering"
# PROP Intermediate_Dir "Release_Engineering"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_USB_" /D "_CLIENT_BG_" /D "_W8782_" /D "DUTAPI8787DLL_EXPORTS" /D "BIG_ENDIAN" /D "_BT_" /D "_WLAN_" /D "_BT_USB_" /D "_FLASH_" /D "_BTRX_RSSIONLY_" /FR /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "_ENGINEERING_" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DUTAPI8787DLL_EXPORTS" /D "_CLIENT_BG_" /D "_MIMO_" /D "_WLAN_" /D "BIG_ENDIAN" /D "_FLASH__" /D "_FM_" /D "_IF_UART_" /D "_BT_REMOTE_UART_" /D "_W8782_" /D "_HCI_PROTOCAL_" /D "_BT_" /D "_CAL_REV_D_" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib MrvlMfg.lib /nologo /dll /profile /machine:I386 /libpath:"../"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /profile /machine:I386 /libpath:"../"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy                                                               Release_Engineering\*.dll                                                               ..\                                                              	copy   Release_Engineering\*.lib    ..\	copy  Release_Engineering\*.lib  ..\..\MfgSuiteWiFiBt	copy  Release_Engineering\*.dll  ..\..\MfgSuiteWiFiBt
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "DutApi8782Dll_BRIDGE_UART - Win32 Release"
# Name "DutApi8782Dll_BRIDGE_UART - Win32 Debug"
# Name "DutApi8782Dll_BRIDGE_UART - Win32 Release_Engineering"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\DutApi878XDll\DutApi878XDll.cpp
# End Source File
# Begin Source File

SOURCE=..\DutBtApi878XDll\DutBtApi878XDll.cpp

!IF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\DutFmApi878XDll\DutFmApi878XDll.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\DutWlanApi878XDll\DutWlanApi878XDll.cpp

!IF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\DutShareApi878XDll\StdAfx.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\DutApi.hc
# End Source File
# Begin Source File

SOURCE=..\DutApi878XDll.h
# End Source File
# Begin Source File

SOURCE=..\DutBtApi878XDll.h
# End Source File
# Begin Source File

SOURCE=..\DutWlanApi878XDll.h
# End Source File
# Begin Source File

SOURCE=..\W878XApp.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\DutApi878XDll\DutApiDll.rc
# End Source File
# Begin Source File

SOURCE=..\DutApi878XDll\DutDllVerNo.h
# End Source File
# End Group
# Begin Group "DutWlan"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\DutWlanApi878XDll\DutIf_IfClss.cpp
# End Source File
# Begin Source File

SOURCE=..\DutWlanApi878XDll\DutIf_IfClss.h
# End Source File
# Begin Source File

SOURCE=..\DutWlanApi878XDll\DutWlanApi.cpp
# End Source File
# Begin Source File

SOURCE=..\DutWlanApi.hc
# End Source File
# Begin Source File

SOURCE=..\DutWlanApi878XDll\DutWlanApiClss.cpp
# End Source File
# Begin Source File

SOURCE=..\DutWlanApi878XDll\DutWlanApiClss.h
# End Source File
# Begin Source File

SOURCE=..\DutWlanApi878XDll\DutWlanApiClss.hd
# End Source File
# Begin Source File

SOURCE=..\DutWlanApi878XDll\DutXpartFileHandle.cpp
# End Source File
# Begin Source File

SOURCE=..\DutWlanApi878XDll\DutXpartFileHandle.h
# End Source File
# Begin Source File

SOURCE=..\DutIf\MrvlMfg.h
# End Source File
# Begin Source File

SOURCE=..\DutWlanApi878XDll\WlanMfgCmd.h
# End Source File
# End Group
# Begin Group "DutBt"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\DutBtApi878XDll\BtMfgCmd.h
# End Source File
# Begin Source File

SOURCE=..\DutBtApi878XDll\DutBrf.cpp
# End Source File
# Begin Source File

SOURCE=..\DutBtApi878XDll\DutBrf.hd
# End Source File
# Begin Source File

SOURCE=..\DutBtApi.hc
# End Source File
# Begin Source File

SOURCE=..\DutBtApi878XDll\DutBtApiClss.cpp
# End Source File
# Begin Source File

SOURCE=..\DutBtApi878XDll\DutBtApiClss.h
# End Source File
# Begin Source File

SOURCE=..\DutBtApi878XDll\DutBtApiCombCmds.cpp

!IF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\DutBtApi878XDll\DutBtCalFunc.cpp

!IF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\DutBtApi878XDll\DutBtIf_IfClss.cpp
# End Source File
# Begin Source File

SOURCE=..\DutBtApi878XDll\DutBtIf_IfClss.h
# End Source File
# Begin Source File

SOURCE=..\DutBtApi878XDll\DutBtu.cpp
# End Source File
# Begin Source File

SOURCE=..\DutBtApi878XDll\DutBtu.hd
# End Source File
# Begin Source File

SOURCE=..\DutBtApi878XDll\DutEfuse.cpp

!IF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\DutBtApi878XDll\hci_opcodes.h
# End Source File
# Begin Source File

SOURCE=..\DutBtApi878XDll\HciCtrl.h
# End Source File
# Begin Source File

SOURCE=..\DutBtApi878XDll\HciStruct.h
# End Source File
# Begin Source File

SOURCE=..\DutBtApi878XDll\UartErrCode.h
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

SOURCE=..\..\DutCommon\MenuDisplay.cpp
# End Source File
# Begin Source File

SOURCE=..\..\DutCommon\utilities.cpp
# End Source File
# Begin Source File

SOURCE=..\..\DutCommon\utilities.h
# End Source File
# Begin Source File

SOURCE=..\..\DutCommon\utilities_os.cpp

!IF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release"

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Debug"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release_Engineering"

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
# Begin Group "DutSharedApi"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\DutShareApi878XDll\DutShared_Caldata.hd
# End Source File
# Begin Source File

SOURCE=..\DutShareApi878XDll\DutShared_spiAccess.cpp
# End Source File
# Begin Source File

SOURCE=..\DutShareApi878XDll\DutSharedCal.cpp
# End Source File
# Begin Source File

SOURCE=..\DutShareApi878XDll\DutSharedCalData.cpp
# End Source File
# Begin Source File

SOURCE=..\DutShareApi878XDll\DutSharedCalFileAccess.cpp

!IF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\DutShareApi878XDll\DutSharedCalFilehandle.cpp
# End Source File
# Begin Source File

SOURCE=..\DutShareApi878XDll\DutSharedCalTxt.cpp
# End Source File
# Begin Source File

SOURCE=..\DutShareApi878XDll\DutSharedClss.h
# End Source File
# Begin Source File

SOURCE=..\DutShareApi878XDll\DutSpiImage.cpp
# End Source File
# End Group
# Begin Group "DutIf"

# PROP Default_Filter ""
# Begin Group "UDP"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\DutIf\DutIf_UdpIpClss.cpp

!IF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Debug"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\DutIf\DutIf_UdpIpClss.h

!IF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Debug"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Lib\WS2_32.LIB"

!IF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Debug"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "NDIS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\DutIf\DutIf_Ndis.cpp

!IF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\DutIf\DutIf_Ndis.h

!IF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "SDIO"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\DutIf\DutIf_SdioAdapter.cpp

!IF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\DutIf\DutIf_SdioAdapter.h

!IF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\DutIf\public.h

!IF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE="c:\Program Files\Microsoft Visual Studio\VC98\Lib\SETUPAPI.LIB"

!IF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApi8782Dll_BRIDGE_UART - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Uart"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\DutIf\DutBtIf_HciUart.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\DutIf\DutBtIf_HciUart.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\DutBtApi878XDll\xmodem.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\xmodem.lib
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\HciCtrl.lib
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "USB"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\DutIf\Dsdef.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\DutIf\DutIf_DvcIoCtrl.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\DutIf\DutIf_DvcIoCtrl.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\DutIf\MrvlMfg.lib
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Remote_UART"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\DutIf\DutIf_UartComm.cpp
# End Source File
# Begin Source File

SOURCE=..\DutIf\DutIf_UartComm.h
# End Source File
# Begin Source File

SOURCE=..\DutIf\SerialCommHelper.cpp
# End Source File
# Begin Source File

SOURCE=..\DutIf\SerialCommHelper.h
# End Source File
# End Group
# End Group
# Begin Group "DutFm"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\DutFmApi878XDll.h
# End Source File
# Begin Source File

SOURCE=..\DutFmApi878XDll\DutFmu.cpp
# End Source File
# Begin Source File

SOURCE=..\DutFmApi878XDll\FmMfgCmd.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
