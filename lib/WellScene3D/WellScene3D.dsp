# Microsoft Developer Studio Project File - Name="WellScene3D" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=WellScene3D - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WellScene3D.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WellScene3D.mak" CFG="WellScene3D - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WellScene3D - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "WellScene3D - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
F90=df.exe
RSC=rc.exe

!IF  "$(CFG)" == "WellScene3D - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\modelframe\include" /I "..\Geometry\include" /I "..\OpenGLFrame\Include" /I "$(LBSP)/Geometry/include" /I "..\Quantity\include" /I "$(LBSP)\modelframe\include" /I "$(LBSP)/OpenGLFrame/include" /I "..\WellScheme" /I "$(QTDIR)\include" /I "$(DIAOBJ)\include30" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /YX /FD /c
# ADD BASE RSC /l 0x413 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x413 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "WellScene3D - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GR /GX /ZI /Od /I "..\modelframe\include" /I "..\Geometry\include" /I "..\OpenGLFrame\Include" /I "$(LBSP)/Geometry/include" /I "..\Quantity\include" /I "$(LBSP)\modelframe\include" /I "$(LBSP)/OpenGLFrame/include" /I "..\WellScheme" /I "$(QTDIR)\include" /I "$(DIAOBJ)\include30" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /YX /FD /GZ /c
# ADD BASE RSC /l 0x413 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x413 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "WellScene3D - Win32 Release"
# Name "WellScene3D - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ArrowLabelSymbol.cpp
# End Source File
# Begin Source File

SOURCE=.\DrillingTowerSymbol.cpp
# End Source File
# Begin Source File

SOURCE=.\GroundLevelSymbol.cpp
# End Source File
# Begin Source File

SOURCE=.\LevelSymbol.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\WaterLevelSymbol.cpp
# End Source File
# Begin Source File

SOURCE=.\WellDrawSpecBase.cpp
# End Source File
# Begin Source File

SOURCE=.\WellPointDrawSpec.cpp
# End Source File
# Begin Source File

SOURCE=.\WellSceneInterMed.cpp
# End Source File
# Begin Source File

SOURCE=.\WellSectionDrawSpec.cpp
# End Source File
# Begin Source File

SOURCE=.\WellSymbolBase.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ArrowLabelSymbol.h
# End Source File
# Begin Source File

SOURCE=.\DrillingTowerSymbol.h
# End Source File
# Begin Source File

SOURCE=.\GroundLevelSymbol.h
# End Source File
# Begin Source File

SOURCE=.\LevelSymbol.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\WaterLevelSymbol.h
# End Source File
# Begin Source File

SOURCE=.\WellDrawSpecBase.h

!IF  "$(CFG)" == "WellScene3D - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing WellDrawSpecBase.h...
InputDir=.
InputPath=.\WellDrawSpecBase.h
InputName=WellDrawSpecBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "WellScene3D - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing WellDrawSpecBase.h...
InputDir=.
InputPath=.\WellDrawSpecBase.h
InputName=WellDrawSpecBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WellPointDrawSpec.h

!IF  "$(CFG)" == "WellScene3D - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing WellPointDrawSpec.h...
InputDir=.
InputPath=.\WellPointDrawSpec.h
InputName=WellPointDrawSpec

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "WellScene3D - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing WellPointDrawSpec.h...
InputDir=.
InputPath=.\WellPointDrawSpec.h
InputName=WellPointDrawSpec

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WellSceneInterMed.h

!IF  "$(CFG)" == "WellScene3D - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing WellSceneInterMed.h...
InputDir=.
InputPath=.\WellSceneInterMed.h
InputName=WellSceneInterMed

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "WellScene3D - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing WellSceneInterMed.h...
InputDir=.
InputPath=.\WellSceneInterMed.h
InputName=WellSceneInterMed

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WellSectionDrawSpec.h

!IF  "$(CFG)" == "WellScene3D - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing WellSectionDrawSpec.h...
InputDir=.
InputPath=.\WellSectionDrawSpec.h
InputName=WellSectionDrawSpec

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "WellScene3D - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing WellSectionDrawSpec.h...
InputDir=.
InputPath=.\WellSectionDrawSpec.h
InputName=WellSectionDrawSpec

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WellSymbolBase.h
# End Source File
# End Group
# Begin Group "moc_files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\moc_WellDrawSpecBase.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_WellPointDrawSpec.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_WellSceneInterMed.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_WellSectionDrawSpec.cpp
# End Source File
# End Group
# End Target
# End Project
