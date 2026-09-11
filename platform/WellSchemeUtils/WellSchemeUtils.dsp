# Microsoft Developer Studio Project File - Name="WellSchemeUtils" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=WellSchemeUtils - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WellSchemeUtils.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WellSchemeUtils.mak" CFG="WellSchemeUtils - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WellSchemeUtils - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "WellSchemeUtils - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
F90=df.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WellSchemeUtils - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "../WellFrame/Include" /I "$(LBSP)\FieldValueFile\include" /I "../OpenGLFrame/include" /I "../Geometry/include" /I "$(DEX_PATH)\include" /I "$(DEVELOP)\Lib\WellFrame\Include" /I "_$(QTDIR)\include" /I "$(QTDIR)\include" /I "$(LBSP)/Geometry/include" /I "..\Quantity\include" /I "$(LBSP)/OpenGLFrame/include" /I "../MfcGuiFrame/include" /I "..\WellScheme" /I "..\WellScene3D" /I "..\DoubleEditLib" /I "../ModelFrame/include" /I "$(LBSP)/ModelFrame/include" /I "$(DIAOBJ)\include30" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "USE_DEX" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x413 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x413 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 ModelObjectFactory.lib mfcguiframe.lib dexui.lib dex.lib geometry.lib openglframe.lib $(QTLIB) opengl32.lib glu32.lib Delayimp.lib ModelFrame.lib el30.lib FieldValueFile.lib /nologo /subsystem:windows /dll /pdb:none /machine:I386 /out:"../../UserDLLs/WellSchemeUtils.dll" /libpath:"../" /libpath:"../ModelObjectFactory/Release" /libpath:"../Geometry/release" /libpath:"../OpenGLFrame/release" /libpath:"../modelframe/release" /libpath:"../mfcguiframe/release" /libpath:"$(LBSP)/obj\lb\fw" /libpath:"$(DEX_PATH)\lib" /libpath:"$(LBSP)" /libpath:"$(LBOBJ)\lb" /libpath:"$(QTDIR)/lib"

!ELSEIF  "$(CFG)" == "WellSchemeUtils - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GR /GX /ZI /Od /I "../WellFrame/Include" /I "$(LBSP)\FieldValueFile\include" /I "../OpenGLFrame/include" /I "../Geometry/include" /I "$(DEX_PATH)\include" /I "$(DEVELOP)\Lib\WellFrame\Include" /I "_$(QTDIR)\include" /I "$(QTDIR)\include" /I "$(LBSP)/Geometry/include" /I "..\Quantity\include" /I "$(LBSP)/OpenGLFrame/include" /I "../MfcGuiFrame/include" /I "..\WellScheme" /I "..\WellScene3D" /I "..\DoubleEditLib" /I "../ModelFrame/include" /I "$(LBSP)/ModelFrame/include" /I "$(DIAOBJ)\include30" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "USE_DEX" /Yu"stdafx.h" /FD /GZ /c
# SUBTRACT CPP /u
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x413 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x413 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ModelObjectFactory.lib FieldValueFile.lib Quantity.lib dexuid.lib dexd.lib geometry.lib openglframe.lib $(QTLIB) opengl32.lib glu32.lib Delayimp.lib ModelFrame.lib mfcguiframe.lib el30.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /out:"../../UserDLLs/WellSchemeUtils.dll" /pdbtype:sept /libpath:"../FieldValueFile/Debug" /libpath:"../Quantity/Debug" /libpath:"../ModelObjectFactory/Debug" /libpath:"../Geometry/Debug" /libpath:"../OpenGLFrame/Debug" /libpath:"../modelframe/debug" /libpath:"../mfcguiframe/debug" /libpath:"$(DEX_PATH)\lib" /libpath:"$(LBSP)" /libpath:"$(LBOBJ)\lb" /libpath:"$(QTDIR)/lib"

!ENDIF 

# Begin Target

# Name "WellSchemeUtils - Win32 Release"
# Name "WellSchemeUtils - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DaoWellInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\dateeditdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DexCasingMatInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\dexdepthtypedlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DexWellInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\DSTable.cpp
# End Source File
# Begin Source File

SOURCE=.\font.cpp
# End Source File
# Begin Source File

SOURCE=.\IDexInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_WellEditView.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\picture.cpp
# End Source File
# Begin Source File

SOURCE=.\RadActTagDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SpreadView.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TableWellpathGeometry.cpp
# End Source File
# Begin Source File

SOURCE=.\VersionTable.cpp
# End Source File
# Begin Source File

SOURCE=.\WellEditDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\WellEditFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\WellEditView.cpp
# End Source File
# Begin Source File

SOURCE=.\wellinfodlg.cpp
# End Source File
# Begin Source File

SOURCE=.\WellLogDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\WellPointListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\WellPointListDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\WellPointPickDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\WellSchemeUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\WellSchemeUtils.def
# End Source File
# Begin Source File

SOURCE=.\WellSchemeUtils.odl
# End Source File
# Begin Source File

SOURCE=.\WellSchemeUtils.rc
# End Source File
# Begin Source File

SOURCE=.\wellspreadsheet.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\DaoWellInterface.h
# End Source File
# Begin Source File

SOURCE=.\dateeditdlg.h
# End Source File
# Begin Source File

SOURCE=.\DexCasingMatInterface.h
# End Source File
# Begin Source File

SOURCE=.\dexdepthtypedlg.h
# End Source File
# Begin Source File

SOURCE=.\DexWellInterface.h
# End Source File
# Begin Source File

SOURCE=.\DSTable.h
# End Source File
# Begin Source File

SOURCE=.\font.h
# End Source File
# Begin Source File

SOURCE=.\IDexInterface.h
# End Source File
# Begin Source File

SOURCE=.\picture.h
# End Source File
# Begin Source File

SOURCE=.\RadActTagDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SpreadView.h
# End Source File
# Begin Source File

SOURCE=.\SSOCX.H
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TableWellpathGeometry.h
# End Source File
# Begin Source File

SOURCE=.\VersionTable.h
# End Source File
# Begin Source File

SOURCE=.\WellEditDlg.h
# End Source File
# Begin Source File

SOURCE=.\WellEditFrame.h
# End Source File
# Begin Source File

SOURCE=.\WellEditView.h

!IF  "$(CFG)" == "WellSchemeUtils - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing WellEditView.h...
InputDir=.
InputPath=.\WellEditView.h
InputName=WellEditView

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "WellSchemeUtils - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing WellEditView.h...
InputDir=.
InputPath=.\WellEditView.h
InputName=WellEditView

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\wellinfodlg.h
# End Source File
# Begin Source File

SOURCE=.\WellLogDlg.h
# End Source File
# Begin Source File

SOURCE=.\WellPointListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\WellPointListDlg.h
# End Source File
# Begin Source File

SOURCE=.\WellPointPickDlg.h
# End Source File
# Begin Source File

SOURCE=.\wellspreadsheet.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\cur00001.cur
# End Source File
# Begin Source File

SOURCE=.\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\toolbar_.bmp
# End Source File
# Begin Source File

SOURCE=.\well_view_bar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\WellSchemeUtils.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\WellSchemeUtils.h
# End Source File
# End Target
# End Project
