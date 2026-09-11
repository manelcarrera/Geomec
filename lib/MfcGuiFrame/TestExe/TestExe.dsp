# Microsoft Developer Studio Project File - Name="TestExe" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=TestExe - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TestExe.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TestExe.mak" CFG="TestExe - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TestExe - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "TestExe - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TestExe - Win32 Release"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "model_src" /I "$(LBSP)\Geometry\include" /I "..\..\Triangle\include" /I "$(LBSP)\OpenGL\include" /I "include" /I "$(QTDIR)\include" /I "$(LBSP)\ModelFrame\include" /I "..\..\MfcGuiFrame\include" /I "$(LBSP)\GuiFrame\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x413 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x413 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386 /libpath:"$(LBSP)"

!ELSEIF  "$(CFG)" == "TestExe - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "model_src" /I "$(LBSP)\Geometry\include" /I "..\..\Triangle\include" /I "$(LBSP)\OpenGL\include" /I "include" /I "$(QTDIR)\include" /I "$(LBSP)\ModelFrame\include" /I "..\..\MfcGuiFrame\include" /I "$(LBSP)\GuiFrame\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x413 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x413 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 modelframe.lib triangle.lib opengl.lib geometry.lib el30.lib qt-mt320.lib glu32.lib opengl32.lib mfcguiframe.lib guiframe.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"../../MfcGuiFrame/debug" /libpath:"\\tsv-diana\diana\obj\ci" /libpath:"\\tsv-diana\diana\obj\lb" /libpath:"\\tsv-diana\diana\obj\lib" /libpath:"$(DSPLIB)" /libpath:"$(QTDIR)/lib" /libpath:"\\tsv-dsp\users\all\psapi" /libpath:"../../triangle/Debug" /libpath:"$(LBSP)"

!ENDIF 

# Begin Target

# Name "TestExe - Win32 Release"
# Name "TestExe - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\model_src\2dbody.cpp

!IF  "$(CFG)" == "TestExe - Win32 Release"

!ELSEIF  "$(CFG)" == "TestExe - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\model_src\2DDocument.cpp

!IF  "$(CFG)" == "TestExe - Win32 Release"

!ELSEIF  "$(CFG)" == "TestExe - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\model_src\2dpolyline.cpp

!IF  "$(CFG)" == "TestExe - Win32 Release"

!ELSEIF  "$(CFG)" == "TestExe - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\model_src\2dsegment.cpp

!IF  "$(CFG)" == "TestExe - Win32 Release"

!ELSEIF  "$(CFG)" == "TestExe - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\model_src\2dvertex.cpp

!IF  "$(CFG)" == "TestExe - Win32 Release"

!ELSEIF  "$(CFG)" == "TestExe - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\opengltestview.cpp
# End Source File
# Begin Source File

SOURCE=.\PolyLineCreator.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TestExe.cpp
# End Source File
# Begin Source File

SOURCE=.\TestExe.rc
# End Source File
# Begin Source File

SOURCE=.\TestExeDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\treetestview.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\model_src\2dbody.h
# End Source File
# Begin Source File

SOURCE=.\model_src\2DDocument.h
# End Source File
# Begin Source File

SOURCE=.\model_src\2dpolyline.h
# End Source File
# Begin Source File

SOURCE=.\model_src\2dsegment.h
# End Source File
# Begin Source File

SOURCE=.\model_src\2dvertex.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\opengltestview.h
# End Source File
# Begin Source File

SOURCE=.\PolyLineCreator.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TestExe.h
# End Source File
# Begin Source File

SOURCE=.\TestExeDoc.h
# End Source File
# Begin Source File

SOURCE=.\treetestview.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bodies.ico
# End Source File
# Begin Source File

SOURCE=.\res\draw.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00002.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\TestExe.ico
# End Source File
# Begin Source File

SOURCE=.\res\TestExe.rc2
# End Source File
# Begin Source File

SOURCE=.\res\TestExeDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\GuiFrame\src\OpenGLObjectObserver.moc

!IF  "$(CFG)" == "TestExe - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing OpenGLObjectObserver.cpp...
InputDir=\users\lnb\Development\lib\GuiFrame\src
InputPath=..\..\GuiFrame\src\OpenGLObjectObserver.moc
InputName=OpenGLObjectObserver

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).cpp -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "TestExe - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing OpenGLObjectObserver.cpp...
InputDir=\users\lnb\Development\lib\GuiFrame\src
InputPath=..\..\GuiFrame\src\OpenGLObjectObserver.moc
InputName=OpenGLObjectObserver

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).cpp -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
