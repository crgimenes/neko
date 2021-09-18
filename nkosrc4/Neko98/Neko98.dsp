# Microsoft Developer Studio Project File - Name="Neko98" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Neko98 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Neko98.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Neko98.mak" CFG="Neko98 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Neko98 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Neko98 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Neko98 - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Neko98 - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Neko98 - Win32 Release"
# Name "Neko98 - Win32 Debug"
# Begin Group "Resources"

# PROP Default_Filter ".ico;.bmp;.rc"
# Begin Source File

SOURCE=.\Res\Awake.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Down1.ico
# End Source File
# Begin Source File

SOURCE=.\Res\down2.ico
# End Source File
# Begin Source File

SOURCE=.\Res\downclaw1.ico
# End Source File
# Begin Source File

SOURCE=.\Res\downclaw2.ico
# End Source File
# Begin Source File

SOURCE=.\Res\downleft1.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Downleft2.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Downright1.ico
# End Source File
# Begin Source File

SOURCE=.\Res\downright2.ico
# End Source File
# Begin Source File

SOURCE=.\Res\fp_down.ico
# End Source File
# Begin Source File

SOURCE=.\Res\fp_downleft.ico
# End Source File
# Begin Source File

SOURCE=.\Res\fp_downright.ico
# End Source File
# Begin Source File

SOURCE=.\Res\fp_left.ico
# End Source File
# Begin Source File

SOURCE=.\Res\fp_right.ico
# End Source File
# Begin Source File

SOURCE=.\Res\fp_up.ico
# End Source File
# Begin Source File

SOURCE=.\Res\fp_upleft.ico
# End Source File
# Begin Source File

SOURCE=.\Res\fp_upright.ico
# End Source File
# Begin Source File

SOURCE=.\Res\left1.ico
# End Source File
# Begin Source File

SOURCE=.\Res\left2.ico
# End Source File
# Begin Source File

SOURCE=.\Res\leftclaw1.ico
# End Source File
# Begin Source File

SOURCE=.\Res\leftclaw2.ico
# End Source File
# Begin Source File

SOURCE=.\Neko95.rc
# End Source File
# Begin Source File

SOURCE=.\Res\Right1.ico
# End Source File
# Begin Source File

SOURCE=.\Res\right2.ico
# End Source File
# Begin Source File

SOURCE=.\Res\rightclaw1.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Rightclaw2.ico
# End Source File
# Begin Source File

SOURCE=.\Res\scratch1.ico
# End Source File
# Begin Source File

SOURCE=.\Res\scratch2.ico
# End Source File
# Begin Source File

SOURCE=.\Res\sleep1.ico
# End Source File
# Begin Source File

SOURCE=.\Res\sleep2.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Up1.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Up2.ico
# End Source File
# Begin Source File

SOURCE=.\Res\upclaw1.ico
# End Source File
# Begin Source File

SOURCE=.\Res\upclaw2.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Upleft1.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Upleft2.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Upright1.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Upright2.ico
# End Source File
# Begin Source File

SOURCE=.\Res\wash2.ico
# End Source File
# Begin Source File

SOURCE=.\Res\yawn2.ico
# End Source File
# Begin Source File

SOURCE=.\Res\yawn3.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\AlwaysOnTopPet.cpp
# End Source File
# Begin Source File

SOURCE=.\AlwaysOnTopPet.h
# End Source File
# Begin Source File

SOURCE=.\DesktopHack.cpp
# End Source File
# Begin Source File

SOURCE=.\DesktopHack.h
# End Source File
# Begin Source File

SOURCE=.\DesktopPet.cpp
# End Source File
# Begin Source File

SOURCE=.\DesktopPet.h
# End Source File
# Begin Source File

SOURCE=.\Neko.cpp
# End Source File
# Begin Source File

SOURCE=.\Neko.h
# End Source File
# Begin Source File

SOURCE=.\Neko98.cpp
# End Source File
# Begin Source File

SOURCE=.\NekoCommon.h
# End Source File
# Begin Source File

SOURCE=.\NekoSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\NekoSettings.h
# End Source File
# Begin Source File

SOURCE=.\Pet.cpp
# End Source File
# Begin Source File

SOURCE=.\Pet.h
# End Source File
# Begin Source File

SOURCE=.\Tray.cpp
# End Source File
# Begin Source File

SOURCE=.\Tray.h
# End Source File
# End Target
# End Project
