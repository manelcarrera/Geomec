# Microsoft Developer Studio Project File - Name="GmDatabase" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=GmDatabase - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GmDatabase.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GmDatabase.mak" CFG="GmDatabase - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GmDatabase - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "GmDatabase - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
F90=df.exe
RSC=rc.exe

!IF  "$(CFG)" == "GmDatabase - Win32 Release"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "include" /I "src" /I "../Geometry/include" /I "$(LBSP)/Geometry/include" /I "$(DIAOBJ)\include30" /I "$(QTDIR)\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "GmDatabase - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "include" /I "src" /I "../Geometry/include" /I "$(LBSP)/Geometry/include" /I "$(DIAOBJ)\include30" /I "$(QTDIR)\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "GmDatabase - Win32 Release"
# Name "GmDatabase - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\SRC\GM2Boundary.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\GM2Database.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\GM2Fault.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\GM2Formation.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\GM2Horizon.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\GM2Model.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\GM2PolyLine.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\GM2Pressure.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\GM2Timestep.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\GM2Vertex.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\GM2WellPath.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\GM3CancelException.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SRC\GM3CrossSection.cpp

!IF  "$(CFG)" == "GmDatabase - Win32 Release"

!ELSEIF  "$(CFG)" == "GmDatabase - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\GM3Database.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\GM3Formation.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SRC\GM3Horizon.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SRC\GM3IncompleteException.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SRC\GM3MeshZone.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SRC\GM3Model.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SRC\GM3Pressure.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SRC\GM3Timestep.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SRC\gm3WellPath.cpp

!IF  "$(CFG)" == "GmDatabase - Win32 Release"

!ELSEIF  "$(CFG)" == "GmDatabase - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\GM3WellPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\gmDatabase.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SRC\GMFormation.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\GMModel.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\GMnonlinprc.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\GMPressure.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\GMTimestep.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\include\GM2Boundary.h
# End Source File
# Begin Source File

SOURCE=.\SRC\GM2Database.h
# End Source File
# Begin Source File

SOURCE=.\include\GM2Fault.h
# End Source File
# Begin Source File

SOURCE=.\include\GM2Formation.h
# End Source File
# Begin Source File

SOURCE=.\include\GM2Horizon.h
# End Source File
# Begin Source File

SOURCE=.\include\GM2Model.h
# End Source File
# Begin Source File

SOURCE=.\include\GM2PolyLine.h
# End Source File
# Begin Source File

SOURCE=.\include\GM2Pressure.h
# End Source File
# Begin Source File

SOURCE=.\SRC\GM2TableDef.h
# End Source File
# Begin Source File

SOURCE=.\include\GM2Timestep.h
# End Source File
# Begin Source File

SOURCE=.\include\GM2Vertex.h
# End Source File
# Begin Source File

SOURCE=.\include\GM2WellPath.h
# End Source File
# Begin Source File

SOURCE=.\include\GM3CheckedValue.h
# End Source File
# Begin Source File

SOURCE=.\include\GM3CrossSection.h
# End Source File
# Begin Source File

SOURCE=.\SRC\GM3Database.h
# End Source File
# Begin Source File

SOURCE=.\include\GM3Formation.h
# End Source File
# Begin Source File

SOURCE=.\include\GM3Horizon.h
# End Source File
# Begin Source File

SOURCE=.\include\GM3IncompleteException.h
# End Source File
# Begin Source File

SOURCE=.\include\GM3MeshZone.h
# End Source File
# Begin Source File

SOURCE=.\include\GM3Model.h
# End Source File
# Begin Source File

SOURCE=.\include\GM3Pressure.h
# End Source File
# Begin Source File

SOURCE=.\SRC\GM3StringTable.h
# End Source File
# Begin Source File

SOURCE=.\include\GM3TableDef.h
# End Source File
# Begin Source File

SOURCE=.\include\GM3Timestep.h
# End Source File
# Begin Source File

SOURCE=.\include\gm3WellPath.h
# End Source File
# Begin Source File

SOURCE=.\include\GM3WellPoint.h
# End Source File
# Begin Source File

SOURCE=.\include\GMDatabase.h
# End Source File
# Begin Source File

SOURCE=.\include\GMFormation.h
# End Source File
# Begin Source File

SOURCE=.\include\GMModel.h
# End Source File
# Begin Source File

SOURCE=.\SRC\GMnonlinprc.h
# End Source File
# Begin Source File

SOURCE=.\include\GMPressure.h
# End Source File
# Begin Source File

SOURCE=.\include\GMTimestep.h
# End Source File
# Begin Source File

SOURCE=.\SRC\StdAfx.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Database_GM3.h
# End Source File
# End Target
# End Project
