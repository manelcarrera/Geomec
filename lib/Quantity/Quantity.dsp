# Microsoft Developer Studio Project File - Name="Quantity" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Quantity - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Quantity.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Quantity.mak" CFG="Quantity - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Quantity - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Quantity - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Quantity - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "../Geometry/include" /I "$(LBSP)/Geometry/include" /I "_$(QTDIR)\include" /I "$(QTDIR)\include" /I "./include" /I "../ModelFrame/include" /I "$(LBSP)/ModelFrame/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Quantity - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GR /GX /ZI /Od /I "_$(QTDIR)\include" /I "$(QTDIR)\include" /I "./include" /I "../ModelFrame/include" /I "$(LBSP)/ModelFrame/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /FD /GZ /c
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

# Name "Quantity - Win32 Release"
# Name "Quantity - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\SRC\Acceleration.cpp

!IF  "$(CFG)" == "Quantity - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "Quantity - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\AngleQuantity.cpp

!IF  "$(CFG)" == "Quantity - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "Quantity - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\CDependenciesManager.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\CohesionQuantity.cpp

!IF  "$(CFG)" == "Quantity - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "Quantity - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\CompressibilityQuantity.cpp

!IF  "$(CFG)" == "Quantity - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "Quantity - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\CrackOpeningQuantity.cpp

!IF  "$(CFG)" == "Quantity - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "Quantity - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\CurvatureQuantity.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\DensityQuantity.cpp

!IF  "$(CFG)" == "Quantity - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "Quantity - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\Dependency.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\ForceQuantity.cpp

!IF  "$(CFG)" == "Quantity - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "Quantity - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\FractureEnergy.cpp

!IF  "$(CFG)" == "Quantity - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "Quantity - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\GigaSinglePressureQuantity.cpp

!IF  "$(CFG)" == "Quantity - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "Quantity - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\LengthQuantity.cpp

!IF  "$(CFG)" == "Quantity - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "Quantity - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\LossCoefQuantity.cpp
# End Source File
# Begin Source File

SOURCE=.\include\moc_SingleQuantity.cpp

!IF  "$(CFG)" == "Quantity - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Quantity - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\NoneQuantity.cpp

!IF  "$(CFG)" == "Quantity - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "Quantity - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\PercentageQuantity.cpp

!IF  "$(CFG)" == "Quantity - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "Quantity - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\PressureGradient.cpp

!IF  "$(CFG)" == "Quantity - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "Quantity - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\SinglePressure.cpp

!IF  "$(CFG)" == "Quantity - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "Quantity - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\SingleQuantity.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\SmallLengthQuantity.cpp

!IF  "$(CFG)" == "Quantity - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "Quantity - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SRC\StrainQuantity.cpp

!IF  "$(CFG)" == "Quantity - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "Quantity - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\StressGradientQuantity.cpp

!IF  "$(CFG)" == "Quantity - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "Quantity - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\StressGradOrDensity.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\TemperatureGradientQuantity.cpp

!IF  "$(CFG)" == "Quantity - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "Quantity - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\TemperatureQuantity.cpp

!IF  "$(CFG)" == "Quantity - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "Quantity - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\ThermalConductivity.cpp

!IF  "$(CFG)" == "Quantity - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "Quantity - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\ThermalExpansion.cpp

!IF  "$(CFG)" == "Quantity - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "Quantity - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\TimeDQuantity.cpp

!IF  "$(CFG)" == "Quantity - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "Quantity - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\TimeHQuantity.cpp

!IF  "$(CFG)" == "Quantity - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "Quantity - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\VolumetricHeatQuantity.cpp

!IF  "$(CFG)" == "Quantity - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "Quantity - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\VolumetricSpecificHeat.cpp

!IF  "$(CFG)" == "Quantity - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "Quantity - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\WeightPerUnitLengthQuantity.cpp

!IF  "$(CFG)" == "Quantity - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "Quantity - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\include\Acceleration.h
# End Source File
# Begin Source File

SOURCE=.\include\AngleQuantity.h
# End Source File
# Begin Source File

SOURCE=.\include\CDependenciesManager.h
# End Source File
# Begin Source File

SOURCE=.\include\CohesionQuantity.h
# End Source File
# Begin Source File

SOURCE=.\include\CompressibilityQuantity.h
# End Source File
# Begin Source File

SOURCE=.\include\CrackOpeningQuantity.h
# End Source File
# Begin Source File

SOURCE=.\include\CurvatureQuantity.h
# End Source File
# Begin Source File

SOURCE=.\include\DensityQuantity.h
# End Source File
# Begin Source File

SOURCE=.\include\Dependency.h
# End Source File
# Begin Source File

SOURCE=.\include\DoubleSingleQuantity.h
# End Source File
# Begin Source File

SOURCE=.\include\ForceQuantity.h
# End Source File
# Begin Source File

SOURCE=.\include\FractureEnergy.h
# End Source File
# Begin Source File

SOURCE=.\include\GigaSinglePressureQuantity.h
# End Source File
# Begin Source File

SOURCE=.\include\LengthQuantity.h
# End Source File
# Begin Source File

SOURCE=.\include\LossCoefQuantity.h
# End Source File
# Begin Source File

SOURCE=.\include\NoneQuantity.h
# End Source File
# Begin Source File

SOURCE=.\include\PercentageQuantity.h
# End Source File
# Begin Source File

SOURCE=.\include\PressureGradient.h
# End Source File
# Begin Source File

SOURCE=.\include\SinglePressure.h
# End Source File
# Begin Source File

SOURCE=.\include\SingleQuantity.h

!IF  "$(CFG)" == "Quantity - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing SingleQuantity.h...
InputDir=.\include
InputPath=.\include\SingleQuantity.h
InputName=SingleQuantity

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Quantity - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing SingleQuantity.h...
InputDir=.\include
InputPath=.\include\SingleQuantity.h
InputName=SingleQuantity

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\include\SingleQuantityTemp.h
# End Source File
# Begin Source File

SOURCE=.\include\SmallLengthQuantity.h
# End Source File
# Begin Source File

SOURCE=.\SRC\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\include\StrainQuantity.h
# End Source File
# Begin Source File

SOURCE=.\include\StressGradientQuantity.h
# End Source File
# Begin Source File

SOURCE=.\include\StressGradOrDensity.h
# End Source File
# Begin Source File

SOURCE=.\include\TemperatureGradientQuantity.h
# End Source File
# Begin Source File

SOURCE=.\include\TemperatureQuantity.h
# End Source File
# Begin Source File

SOURCE=.\include\ThermalConductivity.h
# End Source File
# Begin Source File

SOURCE=.\include\ThermalExpansion.h
# End Source File
# Begin Source File

SOURCE=.\include\TimeDQuantity.h
# End Source File
# Begin Source File

SOURCE=.\include\TimeHQuantity.h
# End Source File
# Begin Source File

SOURCE=.\include\VolumetricHeatQuantity.h
# End Source File
# Begin Source File

SOURCE=.\include\VolumetricSpecificHeat.h
# End Source File
# Begin Source File

SOURCE=.\include\WeightPerUnitLengthQuantity.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\include\QuantityInclude.h
# End Source File
# End Target
# End Project
