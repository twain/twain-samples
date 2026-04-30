# Microsoft Developer Studio Project File - Name="TWAIN_APP_VS6_mfc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=TWAIN_APP_VS6_mfc - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TWAIN_APP_VS6_mfc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TWAIN_APP_VS6_mfc.mak" CFG="TWAIN_APP_VS6_mfc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TWAIN_APP_VS6_mfc - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "TWAIN_APP_VS6_mfc - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/TWG/Twain_App_sample01/visual_studio_mfc", NPBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TWAIN_APP_VS6_mfc - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseVS6"
# PROP Intermediate_Dir "ReleaseVS6"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "." /I ".." /I "..\..\pub\external\include\\" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 FreeImage.lib /nologo /subsystem:windows /machine:I386 /libpath:"..\..\pub\external\lib" /libpath:"..\..\pub\lib"
# Begin Special Build Tool
TargetPath=.\ReleaseVS6\TWAIN_APP_VS6_mfc.exe
SOURCE="$(InputPath)"
PreLink_Desc=create pub folder if do not exist
PreLink_Cmds=if NOT EXIST $(ProjectDir)\..\pub\bin mkdir $(ProjectDir)\..\pub\bin
PostBuild_Desc=Copy Application to pub\bin directory
PostBuild_Cmds=copy $(TargetPath)  $(ProjectDir)\..\pub\bin & copy $(ProjectDir)\..\..\pub\external\bin\FreeImage.dll $(ProjectDir)\..\pub\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "TWAIN_APP_VS6_mfc - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugVS6"
# PROP Intermediate_Dir "DebugVS6"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "." /I ".." /I "..\..\pub\external\include\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /FR /YX"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 FreeImage.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"..\..\pub\external\lib" /libpath:"..\..\pub\lib"
# Begin Special Build Tool
TargetPath=.\DebugVS6\TWAIN_APP_VS6_mfc.exe
SOURCE="$(InputPath)"
PreLink_Desc=create pub folder if do not exist
PreLink_Cmds=if NOT EXIST $(ProjectDir)\..\pub\bin mkdir $(ProjectDir)\..\pub\bin
PostBuild_Desc=Copy Application to pub\bin directory
PostBuild_Cmds=copy $(TargetPath)  $(ProjectDir)\..\pub\bin & copy $(ProjectDir)\..\..\pub\external\bin\FreeImage.dll $(ProjectDir)\..\pub\bin
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "TWAIN_APP_VS6_mfc - Win32 Release"
# Name "TWAIN_APP_VS6_mfc - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\src\CTiffWriter.cpp
# End Source File
# Begin Source File

SOURCE=..\src\DSMInterface.cpp
# End Source File
# Begin Source File

SOURCE=mfc.cpp
# End Source File
# Begin Source File

SOURCE=mfcDlgConfigure.cpp
# End Source File
# Begin Source File

SOURCE=mfcDlgMain.cpp
# End Source File
# Begin Source File

SOURCE=stdafx.cpp

!IF  "$(CFG)" == "TWAIN_APP_VS6_mfc - Win32 Release"

# ADD CPP /YX"stdafx.h"

!ELSEIF  "$(CFG)" == "TWAIN_APP_VS6_mfc - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=TW_Array_Dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\TwainApp.cpp
# End Source File
# Begin Source File

SOURCE=..\src\TwainApp_ui.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\src\CTiffWriter.h
# End Source File
# Begin Source File

SOURCE=..\src\DSMInterface.h
# End Source File
# Begin Source File

SOURCE=mfc.h
# End Source File
# Begin Source File

SOURCE=mfcDlgConfigure.h
# End Source File
# Begin Source File

SOURCE=mfcDlgMain.h
# End Source File
# Begin Source File

SOURCE=resource.h
# End Source File
# Begin Source File

SOURCE=stdafx.h
# End Source File
# Begin Source File

SOURCE=TW_Array_Dlg.h
# End Source File
# Begin Source File

SOURCE=..\src\TwainApp.h
# End Source File
# Begin Source File

SOURCE=..\src\TwainApp_ui.h
# End Source File
# Begin Source File

SOURCE=..\src\TwainStructs.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=res\mfc.ico
# End Source File
# Begin Source File

SOURCE=mfc.rc
# End Source File
# Begin Source File

SOURCE=res\mfc.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\pub\external\lib\FreeImage.lib
# End Source File
# End Target
# End Project
