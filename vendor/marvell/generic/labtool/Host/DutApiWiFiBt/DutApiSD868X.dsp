# Microsoft Developer Studio Project File - Name="DutApiSD868X" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=DutApiSD868X - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DutApiSD868X.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DutApiSD868X.mak" CFG="DutApiSD868X - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DutApiSD868X - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "DutApiSD868X - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "DutApiSD868X - Win32 Release_Engineering" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "_CLIENT_BG_" /D "_BT_" /D "_WLAN_" /D "BIG_ENDIAN" /D "_FLASH__" /D "_IF_SDIO_" /D "_WIFI_SDIO_" /D "_BT_SDIO_" /D "_W8688_" /D "_HCI_PROTOCAL__" /D "_SUPPORT_LDO__" /D "_SUPPORT_PM__" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Release\*.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /w /W0 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "_CLIENT_BG_" /D "_BT_" /D "_WLAN_" /D "BIG_ENDIAN" /D "_FLASH__" /D "_IF_SDIO_" /D "_WIFI_SDIO_" /D "_BT_SDIO_" /D "_W8688_" /D "_HCI_PROTOCAL__" /D "_SUPPORT_LDO__" /D "_SUPPORT_PM__" /D "_ENGINEERING_" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib nafxcwd.lib libcmtd.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"nafxcwd.lib" /nodefaultlib:"libcmtd.lib" /pdbtype:sept
# SUBTRACT LINK32 /nodefaultlib
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Debug\*.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DutApiSD868X___Win32_Release_Engineering"
# PROP BASE Intermediate_Dir "DutApiSD868X___Win32_Release_Engineering"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_Engineering"
# PROP Intermediate_Dir "Release_Engineering"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "_CLIENT_BG_" /D "_BT_" /D "_WLAN_" /D "BIG_ENDIAN" /D "_FLASH__" /D "_IF_SDIO_" /D "_WIFI_SDIO_" /D "_BT_SDIO_" /D "_W8688_" /D "_HCI_PROTOCAL__" /D "_SUPPORT_LDO__" /D "_SUPPORT_PM__" /FR /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "_CLIENT_BG_" /D "_BT_" /D "_WLAN_" /D "BIG_ENDIAN" /D "_FLASH__" /D "_IF_SDIO_" /D "_WIFI_SDIO_" /D "_BT_SDIO_" /D "_W8688_" /D "_HCI_PROTOCAL__" /D "_SUPPORT_LDO__" /D "_SUPPORT_PM__" /D "_ENGINEERING_" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Release_Engineering\*.exe
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "DutApiSD868X - Win32 Release"
# Name "DutApiSD868X - Win32 Debug"
# Name "DutApiSD868X - Win32 Release_Engineering"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DutCalFileHandle.cpp
# End Source File
# Begin Source File

SOURCE=.\DutFuncs.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\w8385p.cpp
# End Source File
# Begin Source File

SOURCE=.\W86xx.cpp
# End Source File
# Begin Source File

SOURCE=.\W8xxxApp.cpp
# End Source File
# Begin Source File

SOURCE=.\DutApiSD8688Dll.lib

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

!ENDIF 

# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\DutApiApp.rc
# End Source File
# Begin Source File

SOURCE=.\DutAppVerNo.h
# End Source File
# End Group
# Begin Group "DutBt"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DutBtApi8xxxDll\BtMfgCmd.h

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutBtApi8xxxDll\DutBrf.cpp

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutBtApi8xxxDll\DutBrf.hd

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutBtApi8xxxDll\DutBtApi8xxxDll.cpp

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutBtApi8xxxDll\DutBtApiClss.cpp

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutBtApi8xxxDll\DutBtApiClss.h

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutBtApi8xxxDll\DutBtApiCombCmds.cpp

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutBtApi8xxxDll\DutBtCalFunc.cpp

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutBtApi8xxxDll\DutBtIf_IfClss.cpp

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutBtApi8xxxDll\DutBtIf_IfClss.h

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutBtApi8xxxDll\DutBtu.cpp

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutBtApi8xxxDll\DutBtu.hd

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutBtApi8xxxDll\DutEfuse.cpp

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutBtApi8xxxDll\hci_opcodes.h

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutBtApi8xxxDll\HciCtrl.h

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutBtApi8xxxDll\HciStruct.h

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutBtApi8xxxDll\UartErrCode.h

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "DutCommon"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\DutCommon\Clss_os.h
# End Source File
# Begin Source File

SOURCE=..\DutCommon\Dut_error.hc
# End Source File
# Begin Source File

SOURCE=..\DutCommon\mathUtility.c
# End Source File
# Begin Source File

SOURCE=..\DutCommon\mathUtility.h
# End Source File
# Begin Source File

SOURCE=..\DutCommon\utilities.cpp
# End Source File
# Begin Source File

SOURCE=..\DutCommon\utilities.h
# End Source File
# Begin Source File

SOURCE=..\DutCommon\utilities_os.cpp
# End Source File
# Begin Source File

SOURCE=..\DutCommon\utilities_os.h
# End Source File
# Begin Source File

SOURCE=..\DutCommon\utility.cpp
# End Source File
# Begin Source File

SOURCE=..\DutCommon\utility.h
# End Source File
# End Group
# Begin Group "GruBt"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GruBtApi86xxDll\DutBtApi86xxLinkTest.cpp

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GruBtApi86xxDll\GruBtApiClss.cpp

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GruBtApi86xxDll\GruBtApiClss.h

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GruBtApi86xxDll\GruDllVerNo.h

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "DutWlan"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DutFuncs.h

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutWlanApi8xxxDll\DutIf_IfClss.cpp

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutWlanApi8xxxDll\DutIf_IfClss.h

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutWlanApi8xxxDll\DutWlanApi.cpp

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutWlanApi8xxxDll\DutWlanApi8xxxDll.cpp

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutWlanApi8xxxDll\DutWlanApiClss.cpp

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutWlanApi8xxxDll\DutWlanApiClss.h

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutWlanApi8xxxDll\DutWlanApiClss.hd

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutWlanApi8xxxDll\WlanMfgCmd.h

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\DutApi8xxxDll.h
# End Source File
# Begin Source File

SOURCE=.\DutBtApi.hc
# End Source File
# Begin Source File

SOURCE=.\DutBtApi8xxxDll.h
# End Source File
# Begin Source File

SOURCE=.\DutWlanApi.hc
# End Source File
# Begin Source File

SOURCE=.\DutWlanApi8xxxDll.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\W8xxxApp.h
# End Source File
# End Group
# Begin Group "Dut"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DutApi8xxxDll\DutApi8xxxDll.cpp

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutShareApi8xxxDll\DutShared_Caldata.hd

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutShareApi8xxxDll\DutShared_spiAccess.cpp

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutShareApi8xxxDll\DutSharedCal.cpp

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutShareApi8xxxDll\DutSharedCalData.cpp

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutShareApi8xxxDll\DutSharedCalFileAccess.cpp

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutShareApi8xxxDll\DutSharedCalFilehandle.cpp

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutShareApi8xxxDll\DutSharedCalTxt.cpp

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutShareApi8xxxDll\DutSharedClss.h

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutShareApi8xxxDll\DutSpiImage.cpp

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "DutIf"

# PROP Default_Filter ""
# Begin Group "ndis"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DutIf\DutIf_Ndis.cpp

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutIf\DutIf_Ndis.h

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutIf\DutNdis.h

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "udp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DutIf\DutIf_UdpIpClss.cpp

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutIf\DutIf_UdpIpClss.h

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "SDIO"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DutIf\DutIf_SdioAdapter.cpp

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutIf\DutIf_SdioAdapter.h

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutIf\public.h

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE="c:\Program Files\Microsoft Visual Studio\VC98\Lib\SETUPAPI.LIB"

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "USB"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DutIf\DutIf_DvcIoCtrl.cpp

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutIf\DutIf_DvcIoCtrl.h

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutIf\MrvlMfg.h

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DutIf\MrvlMfg.lib

!IF  "$(CFG)" == "DutApiSD868X - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DutApiSD868X - Win32 Release_Engineering"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# End Group
# End Target
# End Project
