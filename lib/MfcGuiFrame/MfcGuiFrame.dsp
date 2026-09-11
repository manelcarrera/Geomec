# Microsoft Developer Studio Project File - Name="MfcGuiFrame" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=MfcGuiFrame - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MfcGuiFrame.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MfcGuiFrame.mak" CFG="MfcGuiFrame - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MfcGuiFrame - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "MfcGuiFrame - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
F90=df.exe
RSC=rc.exe

!IF  "$(CFG)" == "MfcGuiFrame - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\Geometry\include" /I "../GuiFrame/include" /I "$(LBSP)/GuiFrame/include" /I "../Geometry/include" /I "$(LBSP)/Geometry/include" /I "../OpenGLFrame/include" /I "$(LBSP)/OpenGLFrame/include" /I "../ModelFrame/include" /I "$(LBSP)/ModelFrame/include" /I "$(QTDIR)\include" /I "include" /I "..\Controls" /I "$(DIAOBJ)\include30" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x413 /d "NDEBUG"
# ADD RSC /l 0x413 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "MfcGuiFrame - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "../Geometry/include" /I "../GuiFrame/include" /I "$(LBSP)/GuiFrame/include" /I "$(LBSP)/Geometry/include" /I "../OpenGLFrame/include" /I "$(LBSP)/OpenGLFrame/include" /I "../ModelFrame/include" /I "$(LBSP)/ModelFrame/include" /I "$(QTDIR)\include" /I "include" /I "..\Controls" /I "$(DIAOBJ)\include30" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x413 /d "_DEBUG"
# ADD RSC /l 0x413 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "MfcGuiFrame - Win32 Release"
# Name "MfcGuiFrame - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\ContextMenuInvoker.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ControlManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CtrlDropTarget.cpp
# End Source File
# Begin Source File

SOURCE=.\src\FileCreator.cpp
# End Source File
# Begin Source File

SOURCE=.\src\IconImageList.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ICtrlObject.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ICtrlObjectBase.cpp
# End Source File
# Begin Source File

SOURCE=.\src\IListObject.cpp
# End Source File
# Begin Source File

SOURCE=.\src\InPlaceEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ISubListObject.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ITreeObject.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ListCtrlBase.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ListViewBase.cpp
# End Source File
# Begin Source File

SOURCE=.\src\MfcOpenGLObjectObserver.cpp

!IF  "$(CFG)" == "MfcGuiFrame - Win32 Release"

!ELSEIF  "$(CFG)" == "MfcGuiFrame - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\MfcTreeObjectObserver.cpp

!IF  "$(CFG)" == "MfcGuiFrame - Win32 Release"

!ELSEIF  "$(CFG)" == "MfcGuiFrame - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\OpenGLView.cpp
# End Source File
# Begin Source File

SOURCE=.\src\StaticFile.cpp
# End Source File
# Begin Source File

SOURCE=.\src\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\src\TnoFileDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TnoWinApp.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TreeCtrlBase.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TreeObject.cpp

!IF  "$(CFG)" == "MfcGuiFrame - Win32 Release"

!ELSEIF  "$(CFG)" == "MfcGuiFrame - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\TreeViewBase.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\include\ContextMenuInvoker.h
# End Source File
# Begin Source File

SOURCE=.\include\ControlManager.h
# End Source File
# Begin Source File

SOURCE=.\include\CtrlDropTarget.h
# End Source File
# Begin Source File

SOURCE=.\include\FileCreator.h
# End Source File
# Begin Source File

SOURCE=.\include\ICommand.h
# End Source File
# Begin Source File

SOURCE=.\include\IconImageList.h
# End Source File
# Begin Source File

SOURCE=.\include\ICtrlObject.h
# End Source File
# Begin Source File

SOURCE=.\include\ICtrlObjectBase.h
# End Source File
# Begin Source File

SOURCE=.\include\IListObject.h
# End Source File
# Begin Source File

SOURCE=.\src\inplaceedit.h
# End Source File
# Begin Source File

SOURCE=.\include\ISubListObject.h
# End Source File
# Begin Source File

SOURCE=.\include\ITreeObject.h
# End Source File
# Begin Source File

SOURCE=.\include\ListCtrlBase.h
# End Source File
# Begin Source File

SOURCE=.\include\listviewbase.h
# End Source File
# Begin Source File

SOURCE=.\include\MfcOpenGLObjectObserver.h
# End Source File
# Begin Source File

SOURCE=.\include\MfcOpenGLObjectObserverTemp.h
# End Source File
# Begin Source File

SOURCE=.\include\MfcTreeBranchObjectObserverTemp.h
# End Source File
# Begin Source File

SOURCE=.\include\MfcTreeLeafObserverTemp.h
# End Source File
# Begin Source File

SOURCE=.\include\MfcTreeObjectObserver.h
# End Source File
# Begin Source File

SOURCE=.\include\OpenGLView.h
# End Source File
# Begin Source File

SOURCE=.\include\StaticFile.h
# End Source File
# Begin Source File

SOURCE=.\src\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\include\TnoFileDialog.h
# End Source File
# Begin Source File

SOURCE=.\include\TnoWinApp.h
# End Source File
# Begin Source File

SOURCE=.\include\treectrlbase.h
# End Source File
# Begin Source File

SOURCE=.\src\TreeObject.h
# End Source File
# Begin Source File

SOURCE=.\include\treeviewbase.h
# End Source File
# End Group
# End Target
# End Project
