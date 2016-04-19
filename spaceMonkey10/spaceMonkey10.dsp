# Microsoft Developer Studio Project File - Name="spaceMonkey10" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=spaceMonkey10 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "spaceMonkey10.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "spaceMonkey10.mak" CFG="spaceMonkey10 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "spaceMonkey10 - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "spaceMonkey10 - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "spaceMonkey10"
# PROP Scc_LocalPath "..\.."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "spaceMonkey10 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "spaceMonkey10 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "spaceMonkey10 - Win32 Release"
# Name "spaceMonkey10 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\BaseWidget.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\Copy of DPWidgetRen.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\CPWidgetRen.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\DPWidgetRen.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\glUE.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\gluvv.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\gluvvPrimitive.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\gluvvui.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\HSLPicker.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\HSLPickWidget.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\LevWidget.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\LTWidgetRen.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\MetaVolume.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\NV20VolRen.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\NV20VolRen3D.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\NV20VolRen3D2.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\NV20VolRen3D3.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\pbuffer.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\sampleRenderer.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\TestRen.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\TFWidgetRen.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\TFWidgetRen1.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\TFWindow.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\TLUT.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\Trackball.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\VolumeRenderable.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\VolumeRenderer.cpp"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\BaseWidget.h"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\CPWidgetRen.h"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\DPWidgetRen.h"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\glUE.h"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\gluvv.h"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\gluvvPrimitive.h"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\gluvvui.h"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\HSLPicker.h"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\HSLPickWidget.h"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\LevWidget.h"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\LTWidgetRen.h"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\MetaVolume.h"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\NV20VolRen.h"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\NV20VolRen3D.h"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\NV20VolRen3D2.h"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\NV20VolRen3D3.h"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\pbuffer.h"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\sampleRenderer.h"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\TestRen.h"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\TFWidgetRen.h"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\TFWidgetRen1.h"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\TFWindow.h"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\TLUT.h"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\Trackball.h"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\VectorMath.h"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\VolumeRenderable.h"
# End Source File
# Begin Source File

SOURCE="..\..\spaceMonkey-1.0\VolumeRenderer.h"
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
