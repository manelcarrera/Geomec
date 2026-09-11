# Microsoft Developer Studio Project File - Name="DCasing" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=DCasing - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DCasing.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DCasing.mak" CFG="DCasing - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DCasing - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "DCasing - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DCasing - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\..\QDianaStartUp" /I "_$(QTDIR)\include" /I "$(QTDIR)\include" /I "..\..\quantity\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /YX /FD /c
# ADD BASE RSC /l 0x413 /d "NDEBUG"
# ADD RSC /l 0x413 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "DCasing - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\QDianaStartUp" /I "_$(QTDIR)\include" /I "$(QTDIR)\include" /I "..\..\quantity\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /FD /GZ /c
# SUBTRACT CPP /YX
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

# Name "DCasing - Win32 Release"
# Name "DCasing - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DCasing.cpp
# End Source File
# Begin Source File

SOURCE=.\DCasingdefines.cpp
# End Source File
# Begin Source File

SOURCE=.\DCasinggeneralParameters.cpp
# End Source File
# Begin Source File

SOURCE=.\DCasingPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\DCasingPointResult.cpp
# End Source File
# Begin Source File

SOURCE=.\DCasintSlotHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_DCasing.cpp

!IF  "$(CFG)" == "DCasing - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "DCasing - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\moc_DCasinggeneralParameters.cpp

!IF  "$(CFG)" == "DCasing - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "DCasing - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\moc_DCasintSlotHandler.cpp

!IF  "$(CFG)" == "DCasing - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "DCasing - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\DCasing.h

!IF  "$(CFG)" == "DCasing - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing DCasing.h...
InputDir=.
InputPath=.\DCasing.h
InputName=DCasing

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "DCasing - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing DCasing.h...
InputDir=.
InputPath=.\DCasing.h
InputName=DCasing

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DCasingDefines.h

!IF  "$(CFG)" == "DCasing - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing DCasingDefines.h...
InputDir=.
InputPath=.\DCasingDefines.h
InputName=DCasingDefines

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "DCasing - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DCasinggeneralParameters.h

!IF  "$(CFG)" == "DCasing - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing DCasinggeneralParameters.h...
InputDir=.
InputPath=.\DCasinggeneralParameters.h
InputName=DCasinggeneralParameters

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "DCasing - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing DCasinggeneralParameters.h...
InputDir=.
InputPath=.\DCasinggeneralParameters.h
InputName=DCasinggeneralParameters

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DCasingPoint.h

!IF  "$(CFG)" == "DCasing - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing DCasingPoint.h...
InputDir=.
InputPath=.\DCasingPoint.h
InputName=DCasingPoint

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "DCasing - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DCasingPointResult.h
# End Source File
# Begin Source File

SOURCE=.\DCasingPointResultData.h
# End Source File
# Begin Source File

SOURCE=.\DCasintSlotHandler.h

!IF  "$(CFG)" == "DCasing - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing DCasintSlotHandler.h...
InputDir=.
InputPath=.\DCasintSlotHandler.h
InputName=DCasintSlotHandler

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "DCasing - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing DCasintSlotHandler.h...
InputDir=.
InputPath=.\DCasintSlotHandler.h
InputName=DCasintSlotHandler

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\DCasintSlotHandler.moc

!IF  "$(CFG)" == "DCasing - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing DCasintSlotHandler.cpp...
InputDir=.
InputPath=.\DCasintSlotHandler.moc
InputName=DCasintSlotHandler

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).cpp -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "DCasing - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing DCasintSlotHandler.cpp...
InputDir=.
InputPath=.\DCasintSlotHandler.moc
InputName=DCasintSlotHandler

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).cpp -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
