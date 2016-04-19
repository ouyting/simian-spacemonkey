# Microsoft Developer Studio Project File - Name="spaceMonkey" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=spaceMonkey - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "spaceMonkey.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "spaceMonkey.mak" CFG="spaceMonkey - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "spaceMonkey - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "spaceMonkey - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "spaceMonkey"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "spaceMonkey - Win32 Release"

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

!ELSEIF  "$(CFG)" == "spaceMonkey - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib glui32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "spaceMonkey - Win32 Release"
# Name "spaceMonkey - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "nrrd_c"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\nrrd\754.c
# End Source File
# Begin Source File

SOURCE=.\nrrd\cubic.c
# End Source File
# Begin Source File

SOURCE=.\nrrd\eigen.c
# End Source File
# Begin Source File

SOURCE=.\nrrd\misc.c
# End Source File
# Begin Source File

SOURCE=.\nrrd\vec.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\BaseWidget.cpp
# End Source File
# Begin Source File

SOURCE=.\CPWidgetRen.cpp
# End Source File
# Begin Source File

SOURCE=.\DPWidgetRen.cpp
# End Source File
# Begin Source File

SOURCE=.\glUE.cpp
# End Source File
# Begin Source File

SOURCE=.\gluvv.cpp
# End Source File
# Begin Source File

SOURCE=.\gluvvPrimitive.cpp
# End Source File
# Begin Source File

SOURCE=.\gluvvui.cpp
# End Source File
# Begin Source File

SOURCE=.\HSLPicker.cpp
# End Source File
# Begin Source File

SOURCE=.\HSLPickWidget.cpp
# End Source File
# Begin Source File

SOURCE=.\jmkTexure.cpp
# End Source File
# Begin Source File

SOURCE=.\LevWidget.cpp
# End Source File
# Begin Source File

SOURCE=.\LTWidgetRen.cpp
# End Source File
# Begin Source File

SOURCE=.\MetaVolume.cpp
# End Source File
# Begin Source File

SOURCE=.\NV20VolRen.cpp
# End Source File
# Begin Source File

SOURCE=.\NV20VolRen3D.cpp
# End Source File
# Begin Source File

SOURCE=.\NV20VolRen3D2.cpp
# End Source File
# Begin Source File

SOURCE=.\pbuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\PNoise.cpp
# End Source File
# Begin Source File

SOURCE=.\R8kVolRen3D.cpp
# End Source File
# Begin Source File

SOURCE=.\R8kVolRen3D_cpy.cpp
# End Source File
# Begin Source File

SOURCE=.\R8kVolRen3D_r2t.cpp
# End Source File
# Begin Source File

SOURCE=.\R8kVolRen3D_sav.cpp
# End Source File
# Begin Source File

SOURCE=.\testPBuff.cpp
# End Source File
# Begin Source File

SOURCE=.\testPert.cpp
# End Source File
# Begin Source File

SOURCE=.\TestRen.cpp
# End Source File
# Begin Source File

SOURCE=.\TFWidgetRen1.cpp
# End Source File
# Begin Source File

SOURCE=.\TFWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\TLUT.cpp
# End Source File
# Begin Source File

SOURCE=.\Trackball.cpp
# End Source File
# Begin Source File

SOURCE=.\VolRenAux.cpp
# End Source File
# Begin Source File

SOURCE=.\VolumeRenderable.cpp
# End Source File
# Begin Source File

SOURCE=.\VolumeRenderer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "nrrd_h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\air.h
# End Source File
# Begin Source File

SOURCE=.\nrrd\ell.h
# End Source File
# Begin Source File

SOURCE=.\nrrd\ellMacros.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\BaseWidget.h
# End Source File
# Begin Source File

SOURCE=.\CPWidgetRen.h
# End Source File
# Begin Source File

SOURCE=.\DPWidgetRen.h
# End Source File
# Begin Source File

SOURCE=.\glUE.h
# End Source File
# Begin Source File

SOURCE=.\gluvv.h
# End Source File
# Begin Source File

SOURCE=.\gluvvPrimitive.h
# End Source File
# Begin Source File

SOURCE=.\gluvvui.h
# End Source File
# Begin Source File

SOURCE=.\HSLPicker.h
# End Source File
# Begin Source File

SOURCE=.\HSLPickWidget.h
# End Source File
# Begin Source File

SOURCE=.\jmkTexure.h
# End Source File
# Begin Source File

SOURCE=.\LevWidget.h
# End Source File
# Begin Source File

SOURCE=.\LTWidgetRen.h
# End Source File
# Begin Source File

SOURCE=.\MetaVolume.h
# End Source File
# Begin Source File

SOURCE=.\NV20VolRen.h
# End Source File
# Begin Source File

SOURCE=.\NV20VolRen3D.h
# End Source File
# Begin Source File

SOURCE=.\NV20VolRen3D2.h
# End Source File
# Begin Source File

SOURCE=.\pbuffer.h
# End Source File
# Begin Source File

SOURCE=.\PNoise.h
# End Source File
# Begin Source File

SOURCE=.\R8kVolRen3D.h
# End Source File
# Begin Source File

SOURCE=.\R8kVolRen3D_cpy.h
# End Source File
# Begin Source File

SOURCE=.\R8kVolRen3D_r2t.h
# End Source File
# Begin Source File

SOURCE=.\R8kVolRen3D_sav.h
# End Source File
# Begin Source File

SOURCE=.\testPBuff.h
# End Source File
# Begin Source File

SOURCE=.\testPert.h
# End Source File
# Begin Source File

SOURCE=.\TestRen.h
# End Source File
# Begin Source File

SOURCE=.\TFWidgetRen1.h
# End Source File
# Begin Source File

SOURCE=.\TFWindow.h
# End Source File
# Begin Source File

SOURCE=.\TLUT.h
# End Source File
# Begin Source File

SOURCE=.\Trackball.h
# End Source File
# Begin Source File

SOURCE=.\VectorMath.h
# End Source File
# Begin Source File

SOURCE=.\VolRenAux.h
# End Source File
# Begin Source File

SOURCE=.\VolumeRenderable.h
# End Source File
# Begin Source File

SOURCE=.\VolumeRenderer.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\extensions.txt
# End Source File
# Begin Source File

SOURCE=.\RUNME_TFC.TXT
# End Source File
# Begin Source File

SOURCE=.\stuff.txt
# End Source File
# End Target
# End Project
