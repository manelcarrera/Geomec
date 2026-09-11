# Microsoft Developer Studio Project File - Name="CutSurfaceUtil" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=CutSurfaceUtil - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CutSurfaceUtil.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CutSurfaceUtil.mak" CFG="CutSurfaceUtil - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CutSurfaceUtil - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CutSurfaceUtil - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
F90=df.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CutSurfaceUtil - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "../triangle/include" /I "$(LBSP)/triangle/include" /I "../OpenGLFrame/include" /I "../WellFrame/Include" /I "../Geometry/include" /I "$(DEX_PATH)\include" /I "$(DEVELOP)\Lib\WellFrame\Include" /I "_$(QTDIR)\include" /I "$(QTDIR)\include" /I "$(LBSP)/Geometry/include" /I "..\Quantity\include" /I "$(LBSP)/OpenGLFrame/include" /I "../MfcGuiFrame/include" /I "..\WellScheme" /I "..\WellScene3D" /I "..\DoubleEditLib" /I "../ModelFrame/include" /I "$(LBSP)/ModelFrame/include" /I "$(DIAOBJ)\include30" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "EXP_CUT_SURFACE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 triangle.lib quantity.lib doubleeditlib.lib geometry.lib openglframe.lib $(QTLIB) opengl32.lib glu32.lib Delayimp.lib ModelFrame.lib mfcguiframe.lib el30.lib /nologo /subsystem:windows /dll /machine:I386 /def:".\CutSurfaceUtil.def" /out:"../../UserDLLs/CutSurfaceUtil.dll" /implib:"./Release/CutSurfaceUtil.lib" /libpath:"../triangle/release" /libpath:"../quantity/release" /libpath:"../doubleeditlib/release" /libpath:"../Geometry/Release" /libpath:"../OpenGLFrame/Release" /libpath:"../mfcguiframe/release" /libpath:"$(LBSP)/obj\lb\fw" /libpath:"$(LBOBJ)\lb" /libpath:"$(QTDIR)/lib" /libpath:"$(LBSP)"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "CutSurfaceUtil - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "../triangle/include" /I "$(LBSP)/triangle/include" /I "../OpenGLFrame/include" /I "../WellFrame/Include" /I "../Geometry/include" /I "$(DEX_PATH)\include" /I "$(DEVELOP)\Lib\WellFrame\Include" /I "_$(QTDIR)\include" /I "$(QTDIR)\include" /I "$(LBSP)/Geometry/include" /I "..\Quantity\include" /I "$(LBSP)/OpenGLFrame/include" /I "../MfcGuiFrame/include" /I "..\WellScheme" /I "..\WellScene3D" /I "..\DoubleEditLib" /I "../ModelFrame/include" /I "$(LBSP)/ModelFrame/include" /I "$(DIAOBJ)\include30" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "EXP_CUT_SURFACE" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 triangle.lib quantity.lib doubleeditlib.lib geometry.lib openglframe.lib $(QTLIB) opengl32.lib glu32.lib Delayimp.lib ModelFrame.lib mfcguiframe.lib el30.lib /nologo /subsystem:windows /dll /debug /machine:I386 /def:".\CutSurfaceUtil.def" /out:"../../UserDLLs/CutSurfaceUtil.dll" /implib:"./Debug/CutSurfaceUtil.lib" /pdbtype:sept /libpath:"../triangle/debug" /libpath:"../quantity/debug" /libpath:"../doubleeditlib/debug" /libpath:"../Geometry/Debug" /libpath:"../OpenGLFrame/Debug" /libpath:"../modelframe/debug" /libpath:"../mfcguiframe/debug" /libpath:"$(LBSP)/obj\lb\fw" /libpath:"$(LBOBJ)\lb" /libpath:"$(QTDIR)/lib" /libpath:"$(LBSP)"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "CutSurfaceUtil - Win32 Release"
# Name "CutSurfaceUtil - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CheckFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\CutSurfaceDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CutSurfaceFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\CutsurfaceTreeView.cpp
# End Source File
# Begin Source File

SOURCE=.\CutSurfaceUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\CutSurfaceUtil.def
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\CutSurfaceUtil.rc
# End Source File
# Begin Source File

SOURCE=.\EditForm.cpp
# End Source File
# Begin Source File

SOURCE=.\GLView.cpp
# End Source File
# Begin Source File

SOURCE=.\NamedDrawDef.cpp
# End Source File
# Begin Source File

SOURCE=.\SelSurfDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SelSurfFormView.cpp
# End Source File
# Begin Source File

SOURCE=.\SelSurfFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\SelSurfGLView.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TriaIntersecBox.cpp

!IF  "$(CFG)" == "CutSurfaceUtil - Win32 Release"

!ELSEIF  "$(CFG)" == "CutSurfaceUtil - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CheckFrm.h
# End Source File
# Begin Source File

SOURCE=.\CutSurfaceDlg.h
# End Source File
# Begin Source File

SOURCE=.\CutSurfaceFrame.h
# End Source File
# Begin Source File

SOURCE=.\CutsurfaceTreeView.h
# End Source File
# Begin Source File

SOURCE=.\EditForm.h
# End Source File
# Begin Source File

SOURCE=.\GLView.h
# End Source File
# Begin Source File

SOURCE=.\NamedDrawDef.h
# End Source File
# Begin Source File

SOURCE=.\NamedSurfaceMap.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SelSurfDlg.h
# End Source File
# Begin Source File

SOURCE=.\SelSurfFormView.h
# End Source File
# Begin Source File

SOURCE=.\SelSurfFrame.h
# End Source File
# Begin Source File

SOURCE=.\SelSurfGLView.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TriaIntersecBox.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\CutSurfaceUtil.rc2
# End Source File
# Begin Source File

SOURCE=.\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\well_view_bar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
