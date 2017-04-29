# Microsoft Developer Studio Project File - Name="DutBtApi86xxDll_USB" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=DutBtApi86xxDll_USB - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DutBtApi86xxDll_USB.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DutBtApi86xxDll_USB.mak" CFG="DutBtApi86xxDll_USB - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DutBtApi86xxDll_USB - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DutBtApi86xxDll_USB - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DutBtApi86xxDll_USB - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DUTBTAPI86XXDLL_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DUTBTAPI86XXDLL_EXPORTS" /D "_BT_" /D "_HCI_UART_" /D "BIG_ENDIAN" /D "BT_WLAN_EEPROM" /D "_WLAN_COEXIST" /D "_ENGINEERING_" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"Release/DutBtApi86xxDll.dll"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Release\*.lib    ..	copy Release\*.dll    ..
# End Special Build Tool

!ELSEIF  "$(CFG)" == "DutBtApi86xxDll_USB - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DUTBTAPI86XXDLL_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DUTBTAPI86XXDLL_EXPORTS" /D "_BT_" /D "_USB_" /D "BIG_ENDIAN" /D "BT_WLAN_EEPROM" /D "_WLAN_COEXIST" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"Debug/DutBtApi86xxDll.dll" /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Debug\*.lib    ..	copy Debug\*.dll    ..
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "DutBtApi86xxDll_USB - Win32 Release"
# Name "DutBtApi86xxDll_USB - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\DutBtApi86xxDll\DutBtApi86xxDll.cpp
# End Source File
# Begin Source File

SOURCE=..\DutBtApi86xxDll\DutWlanApi86xxDll.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\DutBtApi86xxDll\StdAfx.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\DutBtApi86xxDll.h
# End Source File
# Begin Source File

SOURCE=..\DutWlanApi86xxDll.h
# End Source File
# Begin Source File

SOURCE=..\DutBtApi86xxDll\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\DutBtApi86xxDll\DutApiDll.rc
# End Source File
# Begin Source File

SOURCE=..\DutBtApi86xxDll\DutDllVerNo.h
# End Source File
# End Group
# Begin Group "Dut"

# PROP Default_Filter ""
# Begin Group "USB"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\DutBtApi86xxDll\Dsdef.h
# End Source File
# Begin Source File

SOURCE=..\DutBtApi86xxDll\DutIf_DvcIoCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\DutBtApi86xxDll\DutIf_DvcIoCtrl.h
# End Source File
# Begin Source File

SOURCE=..\DutBtApi86xxDll\MrvlMfg.h
# End Source File
# Begin Source File

SOURCE=..\DutBtApi86xxDll\MrvlMfg.lib
# End Source File
# End Group
# Begin Source File

SOURCE=..\DutBtApi86xxDll\BtMfgCmd.h
# End Source File
# Begin Source File

SOURCE=..\DutApi.hc
# End Source File
# Begin Source File

SOURCE=..\DutBtApi86xxDll\DutApiCalData.cpp
# End Source File
# Begin Source File

SOURCE=..\DutBtApi86xxDll\DutApiClss.hd
# End Source File
# Begin Source File

SOURCE=..\DutBtApi86xxDll\DutApiClss_Caldata.hd
# End Source File
# Begin Source File

SOURCE=..\DutBtApi86xxDll\DutApiCombCmds.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\DutBtApi86xxDll\DutBrf.cpp
# End Source File
# Begin Source File

SOURCE=..\DutBtApi86xxDll\DutBrf.hd
# End Source File
# Begin Source File

SOURCE=..\DutBtApi.hc
# End Source File
# Begin Source File

SOURCE=..\DutBtApi86xxDll\DutBtApi_spiAccess.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\DutBtApi86xxDll\DutBtApiClss.cpp
# End Source File
# Begin Source File

SOURCE=..\DutBtApi86xxDll\DutBtApiClss.h
# End Source File
# Begin Source File

SOURCE=..\DutBtApi86xxDll\DutBtCalFunc.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\DutBtApi86xxDll\DutBtIf_HciUart.cpp
# End Source File
# Begin Source File

SOURCE=..\DutBtApi86xxDll\DutBtIf_HciUart.h
# End Source File
# Begin Source File

SOURCE=..\DutBtApi86xxDll\DutBtIf_IfClss.cpp
# End Source File
# Begin Source File

SOURCE=..\DutBtApi86xxDll\DutBtIf_IfClss.h
# End Source File
# Begin Source File

SOURCE=..\DutBtApi86xxDll\DutBtu.cpp
# End Source File
# Begin Source File

SOURCE=..\DutBtApi86xxDll\DutBtu.hd
# End Source File
# Begin Source File

SOURCE=..\DutBtApi86xxDll\hci_opcodes.h
# End Source File
# Begin Source File

SOURCE=..\DutBtApi86xxDll\HciCtrl.h
# End Source File
# Begin Source File

SOURCE=..\DutBtApi86xxDll\HciStruct.h
# End Source File
# Begin Source File

SOURCE=..\DutBtApi86xxDll\UartErrCode.h
# End Source File
# Begin Source File

SOURCE=..\DutBtApi86xxDll\HciCtrl.lib
# End Source File
# Begin Source File

SOURCE=..\DutBtApi86xxDll\xmodem.lib
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

SOURCE=..\..\DutCommon\utilities_os.h
# End Source File
# Begin Source File

SOURCE=..\..\DutCommon\utility.cpp
# End Source File
# Begin Source File

SOURCE=..\..\DutCommon\utility.h
# End Source File
# Begin Source File

SOURCE=..\DutBtApi86xxDll\xmodem.h
# End Source File
# End Group
# Begin Group "Gru"

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
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
