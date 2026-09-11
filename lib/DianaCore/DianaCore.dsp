# Microsoft Developer Studio Project File - Name="DianaCore" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=DianaCore - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DianaCore.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DianaCore.mak" CFG="DianaCore - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DianaCore - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "DianaCore - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
F90=df.exe
RSC=rc.exe

!IF  "$(CFG)" == "DianaCore - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "src" /I "Loads/src" /I "../Geometry/include" /I "$(LBSP)/Geometry/include" /I "../Tensor/include" /I "$(LBSP)/Tensor/include" /I "$(LBOBJ)\include30" /I "$(QTDIR)\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "DianaCore - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "src" /I "Loads/src" /I "../Geometry/include" /I "$(LBSP)/Geometry/include" /I "../Tensor/include" /I "$(LBSP)/Tensor/include" /I "$(LBOBJ)\include30" /I "$(LBOBJ)\lb/fl44" /I "$(QTDIR)\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "DianaCore - Win32 Release"
# Name "DianaCore - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\comfile.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCBodyFaceLoad.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCFaceEdgeLoad.cpp
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCFaceLoad.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCFaceMoment.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCInitialStressLoad.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCLineLoad.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCLineMoment.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCLoadCase.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCLoadCombination.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCLoadManager.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCPointLoad.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCPointMoment.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCPrescribedDisplacement.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCPrescribedRotation.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCPressureLoad.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\src\DCRangeBuilder.cpp
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCTemperatureLoad.cpp
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCWeightLoad.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\src\FilosReplacements.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\src\IDCAnalysisLogger.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\src\IDCDianaRunner.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Loads\src\IDCDirectedLoad.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\src\IDCElementProperty.cpp
# End Source File
# Begin Source File

SOURCE=.\Loads\src\IDCLoad.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\src\IDCTemperatureTable.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\comfile.h
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCBodyFaceLoad.h
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCFaceEdgeLoad.h
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCFaceLoad.h
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCFaceMoment.h
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCInitialStressLoad.h
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCLineLoad.h
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCLineMoment.h
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCLoadCase.h
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCLoadCombination.h
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCLoadManager.h
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCPointLoad.h
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCPointMoment.h
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCPrescribedDisplacement.h
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCPrescribedRotation.h
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCPressureLoad.h
# End Source File
# Begin Source File

SOURCE=.\src\DCRangeBuilder.h
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCTemperatureLoad.h
# End Source File
# Begin Source File

SOURCE=.\Loads\src\DCWeightLoad.h
# End Source File
# Begin Source File

SOURCE=.\src\IDCAnalysisLogger.h
# End Source File
# Begin Source File

SOURCE=.\src\IDCDianaRunner.h
# End Source File
# Begin Source File

SOURCE=.\Loads\src\IDCDirectedLoad.h
# End Source File
# Begin Source File

SOURCE=.\src\IDCElementProperty.h
# End Source File
# Begin Source File

SOURCE=.\Loads\src\IDCLoad.h
# End Source File
# Begin Source File

SOURCE=.\src\IDCTemperatureTable.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\DianaCore.h
# End Source File
# End Target
# End Project
