# Microsoft Developer Studio Project File - Name="WellScheme" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=WellScheme - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WellScheme.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WellScheme.mak" CFG="WellScheme - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WellScheme - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "WellScheme - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
F90=df.exe
RSC=rc.exe

!IF  "$(CFG)" == "WellScheme - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\Quantity\inlcude" /I "$(LBSP)/ModelFrame/include" /I "..\modelframe\include" /I "..\XMLLib" /I "$(LBSP)\modelframe\include" /I "..\Geometry\include" /I "..\OpenGL\Include" /I "..\Quantity\include" /I "_$(QTDIR)\include" /I "$(QTDIR)\include" /I "../Geometry/include" /I "$(LBSP)/Geometry/include" /I "../FieldValueFile/include" /I "$(LBSP)/FieldValueFile/include" /I "$(DIAOBJ)/include30" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /YX /FD /c
# ADD BASE RSC /l 0x413 /d "NDEBUG"
# ADD RSC /l 0x413 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "WellScheme - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GR /GX /ZI /Od /I "..\modelframe\include" /I "..\XMLLib" /I "$(LBSP)\modelframe\include" /I "..\Geometry\include" /I "..\OpenGL\Include" /I "..\Quantity\include" /I "_$(QTDIR)\include" /I "$(QTDIR)\include" /I "../Geometry/include" /I "$(LBSP)/Geometry/include" /I "../FieldValueFile/include" /I "$(LBSP)/FieldValueFile/include" /I "$(DIAOBJ)/include30" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /YX /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x413 /d "_DEBUG"
# ADD RSC /l 0x413 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "WellScheme - Win32 Release"
# Name "WellScheme - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\IWellSection.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_IWellSection.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_WellDefinitionPointList.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_wellpathbase.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_WellPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_WellPointList.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_WellSection.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_WellSectionList.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_WellSectionListBase.cpp
# End Source File
# Begin Source File

SOURCE=.\RTFile.cpp
# End Source File
# Begin Source File

SOURCE=.\WellDefinitionPointList.cpp
# End Source File
# Begin Source File

SOURCE=.\WellLog.cpp
# End Source File
# Begin Source File

SOURCE=.\WellLogFile.cpp
# End Source File
# Begin Source File

SOURCE=.\wellpathbase.cpp
# End Source File
# Begin Source File

SOURCE=.\WellPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\WellPointList.cpp
# End Source File
# Begin Source File

SOURCE=.\WellPointMapper.cpp
# End Source File
# Begin Source File

SOURCE=.\WellSection.cpp
# End Source File
# Begin Source File

SOURCE=.\WellSectionList.cpp
# End Source File
# Begin Source File

SOURCE=.\WellSectionListBase.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\IWellSection.h

!IF  "$(CFG)" == "WellScheme - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing IWellSection.h...
InputDir=.
InputPath=.\IWellSection.h
InputName=IWellSection

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "WellScheme - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing IWellSection.h...
InputDir=.
InputPath=.\IWellSection.h
InputName=IWellSection

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RTFile.h
# End Source File
# Begin Source File

SOURCE=.\WellDefinitionPointList.h

!IF  "$(CFG)" == "WellScheme - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing WellDefinitionPointList.h...
InputDir=.
InputPath=.\WellDefinitionPointList.h
InputName=WellDefinitionPointList

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "WellScheme - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing WellDefinitionPointList.h...
InputDir=.
InputPath=.\WellDefinitionPointList.h
InputName=WellDefinitionPointList

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WellLog.h
# End Source File
# Begin Source File

SOURCE=.\WellLogFile.h
# End Source File
# Begin Source File

SOURCE=.\wellpathbase.h

!IF  "$(CFG)" == "WellScheme - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\wellpathbase.h
InputName=wellpathbase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "WellScheme - Win32 Debug"

# Begin Custom Build - MOCing WellPathBase.h...
InputDir=.
InputPath=.\wellpathbase.h
InputName=wellpathbase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WellPoint.h

!IF  "$(CFG)" == "WellScheme - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing WellPoint.h...
InputDir=.
InputPath=.\WellPoint.h
InputName=WellPoint

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "WellScheme - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing WellPoint.h...
InputDir=.
InputPath=.\WellPoint.h
InputName=WellPoint

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WellPointList.h

!IF  "$(CFG)" == "WellScheme - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing WellPointList.h...
InputDir=.
InputPath=.\WellPointList.h
InputName=WellPointList

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "WellScheme - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing WellPointList.h...
InputDir=.
InputPath=.\WellPointList.h
InputName=WellPointList

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WellPointMapper.h
# End Source File
# Begin Source File

SOURCE=.\WellSchemeInclude.h
# End Source File
# Begin Source File

SOURCE=.\WellSection.h

!IF  "$(CFG)" == "WellScheme - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing WellSection.h...
InputDir=.
InputPath=.\WellSection.h
InputName=WellSection

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "WellScheme - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing WellSection.h...
InputDir=.
InputPath=.\WellSection.h
InputName=WellSection

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WellSectionList.h

!IF  "$(CFG)" == "WellScheme - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing WellSectionList.h...
InputDir=.
InputPath=.\WellSectionList.h
InputName=WellSectionList

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "WellScheme - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing WellSectionList.h...
InputDir=.
InputPath=.\WellSectionList.h
InputName=WellSectionList

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WellSectionListBase.h

!IF  "$(CFG)" == "WellScheme - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing WellSectionListBase.h...
InputDir=.
InputPath=.\WellSectionListBase.h
InputName=WellSectionListBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "WellScheme - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing WellSectionListBase.h...
InputDir=.
InputPath=.\WellSectionListBase.h
InputName=WellSectionListBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\WellScheme.h
# End Source File
# End Target
# End Project
