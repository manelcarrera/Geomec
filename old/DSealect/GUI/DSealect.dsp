# Microsoft Developer Studio Project File - Name="DSealect" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=DSealect - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DSealect.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DSealect.mak" CFG="DSealect - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DSealect - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DSealect - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "DSealect - Win32 WellLife Release " (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/DSealect/GUI", KQJAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DSealect - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GR /GX /I "$(DSPINC)/Geometry" /I "$(DSPINC)/Quantity" /I "..\Model\SRC" /I "..\Utils" /I "..\GUI\SRC" /I "..\AxisX" /I "..\Chart2D" /I "..\DianaX" /I "..\spreadsheet" /I "..\FgvRun" /I "..\ToolTipEx" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Gf /Gy
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x413 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x413 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 Geometry.lib Quantity.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:"..\..\Geometry\Release" /libpath:"..\..\Quantity\Release"
# SUBTRACT LINK32 /incremental:yes

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\Quantity" /I "..\..\geometry" /I "..\Model\SRC" /I "..\..\Quantity\SRC" /I "..\Utils" /I "..\GUI\SRC" /I "..\AxisX" /I "..\Chart2D" /I "..\spreadsheet" /I "..\DianaX" /I "..\FgvRun" /I "..\ToolTipEx" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x413 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x413 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Geometry.lib Quantity.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"..\..\Geometry\Debug" /libpath:"..\..\Lib\Geometry\Debug" /libpath:"..\..\Quantity\Debug" /libpath:"..\..\Lib\Quantity\Debug"

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WellLife_Release"
# PROP BASE Intermediate_Dir "WellLife_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WellLife_Release"
# PROP Intermediate_Dir "WellLife_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GR /GX /I "$(DSPINC)\Geometry" /I "$(DSPINC)\Quantity" /I "..\Model\SRC" /I "..\Utils" /I "..\GUI\SRC" /I "..\AxisX" /I "..\Chart2D" /I "..\DianaX" /I "..\spreadsheet" /I "..\FgvRun" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /Gf /Gy
# ADD CPP /nologo /MD /W3 /GR /GX /I "$(DSPINC)\Geometry" /I "$(DSPINC)\Quantity" /I "..\Model\SRC" /I "..\Utils" /I "..\GUI\SRC" /I "..\AxisX" /I "..\Chart2D" /I "..\DianaX" /I "..\spreadsheet" /I "..\FgvRun" /I "..\ToolTipEx" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_WELLLIFE" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Gf /Gy
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x413 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x413 /d "NDEBUG" /d "_AFXDLL" /d "_WELLLIFE"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Geometry.lib Quantity.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Release/WellLife.exe" /libpath:"$(DSPLIB)"
# SUBTRACT BASE LINK32 /incremental:yes
# ADD LINK32 Geometry.lib Quantity.lib /nologo /subsystem:windows /debug /machine:I386 /out:"WellLife_Release/WellLife.exe" /libpath:"..\..\Lib\Geometry\Release" /libpath:"..\..\Lib\Quantity\Release" /libpath:"$(DSPLIB)"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "DSealect - Win32 Release"
# Name "DSealect - Win32 Debug"
# Name "DSealect - Win32 WellLife Release "
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\SRC\AddAnalysisPointDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\AddFormationDlg.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\AnalysisDraw.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\AnalysisForm.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\AnalysisPointList.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\BaseFormView.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\calculationwindowdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CasingMaterialDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CasingMaterialLibDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CasingMatListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\CasingOperationDraw.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\CasingOperationForm.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\CasingStressDraw.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\CasingStressForm.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\CementMaterialDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CementMaterialLibDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CementMatListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\CementOperationDraw.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\CementOperationForm.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\CementPropertiesDraw.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\CementPropertiesForm.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\CompletionAndTestingDraw.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\CompletionAndTestingForm.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\DDXQuantity.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\DepthsForAnalysisDraw.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\DepthsForAnalysisForm.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\DetailedResultsDraw.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\DetailedResultsForm.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\DoubleEdit.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\DrawProxy.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\DrillingOperationDraw.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\DrillingOperationForm.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\DSealect.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\DSealect.rc
# End Source File
# Begin Source File

SOURCE=.\SRC\DSealectDoc.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\DSealectDrawView.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\DSealectTreeView.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\DSModelDraw.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\DSResult.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\ExtremeOperationsDraw.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\ExtremeOperationsForm.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\fgvview.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\FormationDefinitionDraw.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\FormationDefinitionForm.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Model\SRC\FormationMaterialDatabase.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\FormationMaterialDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\FormationMaterialTable.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\FormationMatListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\FormationMatrialLibDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\FormationPropertiesDraw.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\FormationPropertiesForm.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\Global.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\InfoForm.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\InsituStressesDraw.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\InsituStressesForm.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\MainFrm.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\MeshRefinementDraw.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\MeshRefinementForm.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\MultiViewSplitter.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\optionsdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\OutputSelectionDraw.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\OutputSelectionForm.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\ProductionAndInjectionDraw.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\ProductionAndInjectionForm.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\registryxdll.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\ResultOverviewDraw.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\ResultOverviewForm.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SRC\treeobject.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\UnitDlg.cpp

!IF  "$(CFG)" == "DSealect - Win32 Release"

!ELSEIF  "$(CFG)" == "DSealect - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DSealect - Win32 WellLife Release "

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\SRC\AddAnalysisPointDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\AddFormationDlg.h
# End Source File
# Begin Source File

SOURCE=.\SRC\AnalysisDraw.h
# End Source File
# Begin Source File

SOURCE=.\SRC\AnalysisForm.h
# End Source File
# Begin Source File

SOURCE=.\SRC\AnalysisPointList.h
# End Source File
# Begin Source File

SOURCE=.\SRC\BaseFormView.h
# End Source File
# Begin Source File

SOURCE=.\src\calculationwindowdlg.h
# End Source File
# Begin Source File

SOURCE=.\src\CasingMaterialDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\CasingMaterialLibDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\CasingMatListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CasingOperationDraw.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CasingOperationForm.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CasingStressDraw.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CasingStressForm.h
# End Source File
# Begin Source File

SOURCE=.\src\CementMaterialDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\CementMaterialLibDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\CementMatListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CementOperationDraw.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CementOperationForm.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CementPropertiesDraw.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CementPropertiesForm.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CompletionAndTestingDraw.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CompletionAndTestingForm.h
# End Source File
# Begin Source File

SOURCE=.\SRC\DDXQuantity.h
# End Source File
# Begin Source File

SOURCE=.\SRC\DepthsForAnalysisDraw.h
# End Source File
# Begin Source File

SOURCE=.\SRC\DepthsForAnalysisForm.h
# End Source File
# Begin Source File

SOURCE=.\src\DetailedResultsDraw.h
# End Source File
# Begin Source File

SOURCE=.\src\DetailedResultsForm.h
# End Source File
# Begin Source File

SOURCE=.\SRC\DoubleEdit.h
# End Source File
# Begin Source File

SOURCE=.\SRC\DoubleFormat.h
# End Source File
# Begin Source File

SOURCE=.\SRC\DrawProxy.h
# End Source File
# Begin Source File

SOURCE=.\SRC\DrillingOperationDraw.h
# End Source File
# Begin Source File

SOURCE=.\SRC\DrillingOperationForm.h
# End Source File
# Begin Source File

SOURCE=.\SRC\DSealect.h
# End Source File
# Begin Source File

SOURCE=.\DSealect.hm
# End Source File
# Begin Source File

SOURCE=.\SRC\DSealectDoc.h
# End Source File
# Begin Source File

SOURCE=.\SRC\DSealectDrawView.h
# End Source File
# Begin Source File

SOURCE=.\SRC\DSealectTreeView.h
# End Source File
# Begin Source File

SOURCE=.\src\DSModelDraw.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\DSResult.h
# End Source File
# Begin Source File

SOURCE=.\SRC\ExtremeOperationsDraw.h
# End Source File
# Begin Source File

SOURCE=.\SRC\ExtremeOperationsForm.h
# End Source File
# Begin Source File

SOURCE=.\src\fgvview.h
# End Source File
# Begin Source File

SOURCE=.\SRC\FormationDefinitionDraw.h
# End Source File
# Begin Source File

SOURCE=.\SRC\FormationDefinitionForm.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\FormationMaterialDatabase.h
# End Source File
# Begin Source File

SOURCE=.\SRC\FormationMaterialDlg.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\FormationMaterialTable.h
# End Source File
# Begin Source File

SOURCE=.\SRC\FormationMatListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\SRC\FormationMatrialLibDlg.h
# End Source File
# Begin Source File

SOURCE=.\SRC\FormationPropertiesDraw.h
# End Source File
# Begin Source File

SOURCE=.\SRC\FormationPropertiesForm.h
# End Source File
# Begin Source File

SOURCE=.\SRC\Global.h
# End Source File
# Begin Source File

SOURCE=.\SRC\InfoForm.h
# End Source File
# Begin Source File

SOURCE=.\SRC\InsituStressesDraw.h
# End Source File
# Begin Source File

SOURCE=.\SRC\InsituStressesForm.h
# End Source File
# Begin Source File

SOURCE=.\SRC\MainFrm.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\MaterialDatabaseTemplate.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\MaterialListTemplate.h
# End Source File
# Begin Source File

SOURCE=.\SRC\MeshRefinementDraw.h
# End Source File
# Begin Source File

SOURCE=.\SRC\MeshRefinementForm.h
# End Source File
# Begin Source File

SOURCE=.\SRC\MultiViewSplitter.h
# End Source File
# Begin Source File

SOURCE=.\src\optionsdlg.h
# End Source File
# Begin Source File

SOURCE=.\SRC\OutputSelectionDraw.h
# End Source File
# Begin Source File

SOURCE=.\SRC\OutputSelectionForm.h
# End Source File
# Begin Source File

SOURCE=.\src\ProductionAndInjectionDraw.h
# End Source File
# Begin Source File

SOURCE=.\SRC\ProductionAndInjectionForm.h
# End Source File
# Begin Source File

SOURCE=.\SRC\registryxdll.h
# End Source File
# Begin Source File

SOURCE=.\SRC\resource.h
# End Source File
# Begin Source File

SOURCE=.\SRC\ResultOverviewDraw.h
# End Source File
# Begin Source File

SOURCE=.\SRC\ResultOverviewForm.h
# End Source File
# Begin Source File

SOURCE=.\SRC\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SRC\treeobject.h
# End Source File
# Begin Source File

SOURCE=.\SRC\UnitDlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=".\SRC\res\APPS Stop.ico"
# End Source File
# Begin Source File

SOURCE=".\SRC\res\Be Card Stack.ico"
# End Source File
# Begin Source File

SOURCE=".\SRC\res\Be Edit.ico"
# End Source File
# Begin Source File

SOURCE=".\SRC\res\Be Generic File.ico"
# End Source File
# Begin Source File

SOURCE=".\SRC\res\Be Kaleidoscope.ico"
# End Source File
# Begin Source File

SOURCE=".\SRC\res\Be Magnify.ico"
# End Source File
# Begin Source File

SOURCE=".\SRC\res\Be Script.ico"
# End Source File
# Begin Source File

SOURCE=.\SRC\res\BeBounce.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=.\src\res\bitmap4.bmp
# End Source File
# Begin Source File

SOURCE=".\SRC\res\Calculator 3D.ico"
# End Source File
# Begin Source File

SOURCE=.\src\res\CasingDebonding.bmp
# End Source File
# Begin Source File

SOURCE=".\SRC\res\Control Folders.ico"
# End Source File
# Begin Source File

SOURCE=.\src\res\diana.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\disable.ico
# End Source File
# Begin Source File

SOURCE=".\SRC\res\Drop Folder.ico"
# End Source File
# Begin Source File

SOURCE=.\SRC\res\DSealect.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\DSealect.rc2
# End Source File
# Begin Source File

SOURCE=.\SRC\res\DSealectDoc.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\expand.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\FormationDebonding.bmp
# End Source File
# Begin Source File

SOURCE=".\SRC\res\Graph Folder.ico"
# End Source File
# Begin Source File

SOURCE=.\SRC\res\HallIcon.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\icon4.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\iconenable.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\idr_casi.ico
# End Source File
# Begin Source File

SOURCE=".\SRC\res\Ink Jet.ico"
# End Source File
# Begin Source File

SOURCE=".\SRC\res\Purple Tab Folder.ico"
# End Source File
# Begin Source File

SOURCE=.\SRC\res\Recycling.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\sdi.ico
# End Source File
# Begin Source File

SOURCE=".\SRC\res\Shaded Reports.ico"
# End Source File
# Begin Source File

SOURCE=".\SRC\res\Shaded Utilities.ico"
# End Source File
# Begin Source File

SOURCE=.\SRC\res\Shell.bmp
# End Source File
# Begin Source File

SOURCE=.\SRC\res\ShellIcon.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\Shield.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\Sphere.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\splah2.bmp
# End Source File
# Begin Source File

SOURCE=.\SRC\res\Splash.bmp
# End Source File
# Begin Source File

SOURCE=.\SRC\res\Splash2.bmp
# End Source File
# Begin Source File

SOURCE=.\SRC\res\Splashwe.bmp
# End Source File
# Begin Source File

SOURCE=".\SRC\res\Stoned Folder.ico"
# End Source File
# Begin Source File

SOURCE=.\SRC\res\Sunburst.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\Time.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\TNOIcon.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\SRC\res\Tools.ico
# End Source File
# Begin Source File

SOURCE=".\SRC\res\Tube Empty_m.ico"
# End Source File
# Begin Source File

SOURCE=.\SRC\res\Weight.ico
# End Source File
# Begin Source File

SOURCE=".\SRC\res\Wild Folder 37.ico"
# End Source File
# End Group
# Begin Group "DSealect Model"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Model\SRC\CAnalysis.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CAnalysis.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CAnalysisPoint.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CAnalysisPoint.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CasingMaterial.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CasingMaterial.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CBatchTable.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CBatchTable.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CCasingOperation.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CCasingOperation.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CCementingOperation.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CCementingOperation.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CCementScenariosTable.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CCementScenariosTable.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CDataModel.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CDataModel.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CDependenciesManager.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CDependenciesManager.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CDrillingDirectionSection.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CDrillingDirectionSection.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CDrillingOperation.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CDrillingOperation.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CDrillingScheme.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CDrillingScheme.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CementMaterial.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CementMaterial.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CementScenario.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CementScenario.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CEvacuationLoads.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CEvacuationLoads.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CFormation.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CFormation.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CFormationTopography.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CFormationTopography.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CFracturingLoads.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CFracturingLoads.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CInjectionLoads.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CInjectionLoads.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\control.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\control.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\COverburden.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\COverburden.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CPerforationLoads.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CPerforationLoads.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CPriorToCompletionLoads.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CPriorToCompletionLoads.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CProductionLoads.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CProductionLoads.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CSeawater.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CSeawater.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CTestLoads.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CTestLoads.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\Dependency.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\Dependency.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\DianaXEventWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\DianaXEventWnd.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\DSMaterial.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\DSMaterial.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\FormationMaterial.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\FormationMaterial.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\IComponent.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\IComponent.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\InitialStress.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\InitialStress.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\MeshParam.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\MeshParam.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\RockRefinement.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\RockRefinement.h
# End Source File
# End Group
# Begin Group "Utils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Utils\CheckFrm.cpp
# End Source File
# Begin Source File

SOURCE=..\Utils\CheckFrm.h
# End Source File
# Begin Source File

SOURCE=..\Utils\CommandLineInfoEx.cpp
# End Source File
# Begin Source File

SOURCE=..\Utils\CommandLineInfoEx.h
# End Source File
# Begin Source File

SOURCE=..\Utils\ProgressCallBack.cpp
# End Source File
# Begin Source File

SOURCE=..\Utils\ProgressCallBack.h
# End Source File
# Begin Source File

SOURCE=..\Utils\ReadWriteBlob.cpp
# End Source File
# Begin Source File

SOURCE=..\Utils\ReadWriteBlob.h
# End Source File
# Begin Source File

SOURCE=..\Utils\Splah.cpp
# End Source File
# Begin Source File

SOURCE=..\Utils\Splah.h
# End Source File
# Begin Source File

SOURCE=..\Utils\StringUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\Utils\StringUtils.h
# End Source File
# End Group
# Begin Group "Database"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Model\src\AnalysisPointTable.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\src\AnalysisPointTable.h
# End Source File
# Begin Source File

SOURCE=..\Model\src\BatchTableTable.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\src\BatchTableTable.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CasingMaterialDatabase.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CasingMaterialDatabase.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CasingMaterialTable.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CasingMaterialTable.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CementMaterialDatabase.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CementMaterialDatabase.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CementMaterialTable.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CementMaterialTable.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CementTable.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\CementTable.h
# End Source File
# Begin Source File

SOURCE=..\Model\src\ControlTable.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\src\ControlTable.h
# End Source File
# Begin Source File

SOURCE=..\Model\src\DrillingDirectionTable.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\src\DrillingDirectionTable.h
# End Source File
# Begin Source File

SOURCE=..\Model\src\DrillingOperationTable.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\src\DrillingOperationTable.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\DSModelDatabase.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\DSModelDatabase.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\DSTable.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\DSTable.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\FormationTable.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\FormationTable.h
# End Source File
# Begin Source File

SOURCE=..\Model\src\ModelOverburdenTable.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\src\ModelOverburdenTable.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\VersionInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\VersionInfo.h
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\VersionTable.cpp
# End Source File
# Begin Source File

SOURCE=..\Model\SRC\VersionTable.h
# End Source File
# End Group
# Begin Group "AxisX"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\AxisX\axisx.cpp
# End Source File
# Begin Source File

SOURCE=..\AxisX\axisx.h
# End Source File
# Begin Source File

SOURCE=..\AxisX\picture.cpp
# End Source File
# Begin Source File

SOURCE=..\AxisX\picture.h
# End Source File
# End Group
# Begin Group "Chart2D"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Chart2D\2DCONST.H
# End Source File
# Begin Source File

SOURCE=..\Chart2D\accountingvf.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\accountingvf.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\actionmap.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\actionmap.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\actionmapcollection.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\actionmapcollection.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\axis.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\axis.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\axiscollection.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\axiscollection.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\axisstyle.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\axisstyle.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\bar.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\bar.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\border.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\border.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\bubble.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\bubble.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\candle.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\candle.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\chart2d.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\chart2d.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\chart2dchecknewpointsresult.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\chart2dchecknewpointsresult.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\chart2dcoordresult.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\chart2dcoordresult.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\chart2ddata.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\chart2ddata.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\chart2ddatacoordresult.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\chart2ddatacoordresult.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\chart2ddataindexresult.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\chart2ddataindexresult.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\chartarea.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\chartarea.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\chartgroup.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\chartgroup.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\chartgroupcollection.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\chartgroupcollection.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\chartlabel.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\chartlabel.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\chartlabelcollection.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\chartlabelcollection.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\chartstyle.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\chartstyle.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\chartstylecollection.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\chartstylecollection.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\coord.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\coord.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\currencyvf.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\currencyvf.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\datacoord.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\datacoord.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\dataindex.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\dataindex.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\derivedboolean.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\derivedboolean.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\deriveddouble.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\deriveddouble.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\derivedenum.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\derivedenum.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\derivedlong.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\derivedlong.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\derivedstring.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\derivedstring.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\fillstyle.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\fillstyle.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\font.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\font.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\fractionvf.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\fractionvf.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\gridlines.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\gridlines.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\hiloopenclose.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\hiloopenclose.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\image.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\image.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\interior.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\interior.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\label.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\label.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\labelcollection.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\labelcollection.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\legend.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\legend.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\linestyle.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\linestyle.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\location.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\location.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\marker.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\marker.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\markercollection.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\markercollection.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\numbervf.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\numbervf.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\percentagevf.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\percentagevf.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\pie.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\pie.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\plotarea.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\plotarea.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\pointstylecollection.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\pointstylecollection.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\polar.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\polar.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\scientificvf.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\scientificvf.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\slicestyle.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\slicestyle.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\stringcollection.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\stringcollection.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\symbolstyle.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\symbolstyle.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\timescale.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\timescale.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\title.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\title.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\valueformat.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\valueformat.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\valuelabel.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\valuelabel.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\valuelabelcollection.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\valuelabelcollection.h
# End Source File
# Begin Source File

SOURCE=..\Chart2D\view3d.cpp
# End Source File
# Begin Source File

SOURCE=..\Chart2D\view3d.h
# End Source File
# End Group
# Begin Group "DianaX"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\DianaX\dianax.cpp
# End Source File
# Begin Source File

SOURCE=..\DianaX\dianax.h
# End Source File
# End Group
# Begin Group "FgvRun"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\FgvRun\fgvrunctr.cpp
# End Source File
# Begin Source File

SOURCE=..\FgvRun\fgvrunctr.h
# End Source File
# End Group
# Begin Group "spreadsheet"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\spreadsheet\SpreadFunctions.cpp
# End Source File
# Begin Source File

SOURCE=..\spreadsheet\SpreadFunctions.h
# End Source File
# Begin Source File

SOURCE=..\spreadsheet\spreadsheet.cpp
# End Source File
# Begin Source File

SOURCE=..\spreadsheet\spreadsheet.h
# End Source File
# End Group
# Begin Group "ToolTipEx"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ToolTipEx\OXDllExt.h
# End Source File
# Begin Source File

SOURCE=..\ToolTipEx\OXToolTipCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\ToolTipEx\OXToolTipCtrl.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\SRC\DSealect.clw
# End Source File
# End Target
# End Project
# Section DSealect : {5BEA0BCB-5A64-4650-B59E-DD418A9D6154}
# 	2:21:DefaultSinkHeaderFile:axisx.h
# 	2:16:DefaultSinkClass:CAxisX
# End Section
# Section DSealect : {D86FB466-2743-11CF-A641-E996DF2C9318}
# 	2:5:Class:CChartStyle
# 	2:10:HeaderFile:chartstyle.h
# 	2:8:ImplFile:chartstyle.cpp
# End Section
# Section DSealect : {FC2C9F1A-26A2-11CF-A641-E996DF2C9318}
# 	2:5:Class:CPlotArea
# 	2:10:HeaderFile:plotarea.h
# 	2:8:ImplFile:plotarea.cpp
# End Section
# Section DSealect : {FC2C9F2A-26A2-11CF-A641-E996DF2C9318}
# 	2:5:Class:CAxisCollection
# 	2:10:HeaderFile:axiscollection.h
# 	2:8:ImplFile:axiscollection.cpp
# End Section
# Section DSealect : {2BB6A8B0-268F-11CF-A641-E996DF2C9318}
# 	2:5:Class:CLegend
# 	2:10:HeaderFile:legend.h
# 	2:8:ImplFile:legend.cpp
# End Section
# Section DSealect : {5DF9BF6B-59A9-11D0-B476-0020AFD59EF6}
# 	2:5:Class:CNumberVF
# 	2:10:HeaderFile:numbervf.h
# 	2:8:ImplFile:numbervf.cpp
# End Section
# Section DSealect : {DA683211-1FD4-11D2-AF1F-00A024C3C1C2}
# 	2:5:Class:CAxisStyle
# 	2:10:HeaderFile:axisstyle.h
# 	2:8:ImplFile:axisstyle.cpp
# End Section
# Section DSealect : {86E43510-1D29-11D0-B3FB-0020AFD59EF6}
# 	2:5:Class:CStringCollection
# 	2:10:HeaderFile:stringcollection.h
# 	2:8:ImplFile:stringcollection.cpp
# End Section
# Section DSealect : {3A01BC15-BA9F-11D2-B3B4-00104B2556D9}
# 	2:5:Class:CDianaX
# 	2:10:HeaderFile:dianax.h
# 	2:8:ImplFile:dianax.cpp
# End Section
# Section DSealect : {53609215-4A3F-11D2-BD2A-006097CF02D2}
# 	2:5:Class:CPointStyleCollection
# 	2:10:HeaderFile:pointstylecollection.h
# 	2:8:ImplFile:pointstylecollection.cpp
# End Section
# Section DSealect : {F4CC7DE0-9AE0-11CF-B9E2-00A024169263}
# 	2:5:Class:CActionMap
# 	2:10:HeaderFile:actionmap.h
# 	2:8:ImplFile:actionmap.cpp
# End Section
# Section DSealect : {5DF9BF6F-59A9-11D0-B476-0020AFD59EF6}
# 	2:5:Class:CAccountingVF
# 	2:10:HeaderFile:accountingvf.h
# 	2:8:ImplFile:accountingvf.cpp
# End Section
# Section DSealect : {C23B5F11-79F1-11D0-813F-0020AF19EE14}
# 	2:5:Class:CChart2DCoordResult
# 	2:10:HeaderFile:chart2dcoordresult.h
# 	2:8:ImplFile:chart2dcoordresult.cpp
# End Section
# Section DSealect : {FC2C9F12-26A2-11CF-A641-E996DF2C9318}
# 	2:5:Class:CBar
# 	2:10:HeaderFile:bar.h
# 	2:8:ImplFile:bar.cpp
# End Section
# Section DSealect : {FC2C9F22-26A2-11CF-A641-E996DF2C9318}
# 	2:5:Class:CAxis
# 	2:10:HeaderFile:axis.h
# 	2:8:ImplFile:axis.cpp
# End Section
# Section DSealect : {FC2C9F1E-26A2-11CF-A641-E996DF2C9318}
# 	2:5:Class:CDerivedString
# 	2:10:HeaderFile:derivedstring.h
# 	2:8:ImplFile:derivedstring.cpp
# End Section
# Section DSealect : {8027AB63-85DC-11D0-B4AF-0020AFD59EF6}
# 	2:5:Class:CDerivedEnum
# 	2:10:HeaderFile:derivedenum.h
# 	2:8:ImplFile:derivedenum.cpp
# End Section
# Section DSealect : {5DF9BF73-59A9-11D0-B476-0020AFD59EF6}
# 	2:5:Class:CFractionVF
# 	2:10:HeaderFile:fractionvf.h
# 	2:8:ImplFile:fractionvf.cpp
# End Section
# Section DSealect : {D86FB46C-2743-11CF-A641-E996DF2C9318}
# 	2:5:Class:CValueLabel
# 	2:10:HeaderFile:valuelabel.h
# 	2:8:ImplFile:valuelabel.cpp
# End Section
# Section DSealect : {05478A90-26E2-11CF-A641-E996DF2C9318}
# 	2:5:Class:CMarker
# 	2:10:HeaderFile:marker.h
# 	2:8:ImplFile:marker.cpp
# End Section
# Section DSealect : {D86FB460-2743-11CF-A641-E996DF2C9318}
# 	2:5:Class:CChart2DData
# 	2:10:HeaderFile:chart2ddata.h
# 	2:8:ImplFile:chart2ddata.cpp
# End Section
# Section DSealect : {D86FB470-2743-11CF-A641-E996DF2C9318}
# 	2:5:Class:CChartLabel
# 	2:10:HeaderFile:chartlabel.h
# 	2:8:ImplFile:chartlabel.cpp
# End Section
# Section DSealect : {8847CF32-2610-11CF-A640-9986B64D9618}
# 	2:5:Class:CDerivedLong
# 	2:10:HeaderFile:derivedlong.h
# 	2:8:ImplFile:derivedlong.cpp
# End Section
# Section DSealect : {C23B5F15-79F1-11D0-813F-0020AF19EE14}
# 	2:5:Class:CChart2DDataIndexResult
# 	2:10:HeaderFile:chart2ddataindexresult.h
# 	2:8:ImplFile:chart2ddataindexresult.cpp
# End Section
# Section DSealect : {FC2C9F26-26A2-11CF-A641-E996DF2C9318}
# 	2:5:Class:CSymbolStyle
# 	2:10:HeaderFile:symbolstyle.h
# 	2:8:ImplFile:symbolstyle.cpp
# End Section
# Section DSealect : {05478A94-26E2-11CF-A641-E996DF2C9318}
# 	2:5:Class:CDataIndex
# 	2:10:HeaderFile:dataindex.h
# 	2:8:ImplFile:dataindex.cpp
# End Section
# Section DSealect : {92D71E91-25A8-11CF-A640-9986B64D9618}
# 	2:5:Class:CChart2D
# 	2:10:HeaderFile:chart2d.h
# 	2:8:ImplFile:chart2d.cpp
# End Section
# Section DSealect : {FC2C9F16-26A2-11CF-A641-E996DF2C9318}
# 	2:5:Class:CPie
# 	2:10:HeaderFile:pie.h
# 	2:8:ImplFile:pie.cpp
# End Section
# Section DSealect : {6E5B3FD4-4C5D-11D0-813A-0020AF19EE14}
# 	2:5:Class:CImage
# 	2:10:HeaderFile:image.h
# 	2:8:ImplFile:image.cpp
# End Section
# Section DSealect : {D86FB464-2743-11CF-A641-E996DF2C9318}
# 	2:5:Class:CChartGroupCollection
# 	2:10:HeaderFile:chartgroupcollection.h
# 	2:8:ImplFile:chartgroupcollection.cpp
# End Section
# Section DSealect : {245631F2-F6C4-11CF-811C-0020AF19EE14}
# 	2:5:Class:CCandle
# 	2:10:HeaderFile:candle.h
# 	2:8:ImplFile:candle.cpp
# End Section
# Section DSealect : {5ECE0AB6-B52E-499D-8034-B5E0CD22746F}
# 	2:5:Class:CFgvRunCtr
# 	2:10:HeaderFile:fgvrunctr.h
# 	2:8:ImplFile:fgvrunctr.cpp
# End Section
# Section DSealect : {92D71E9C-25A8-11CF-A640-9986B64D9618}
# 	2:5:Class:CLabel
# 	2:10:HeaderFile:label.h
# 	2:8:ImplFile:label.cpp
# End Section
# Section DSealect : {05478A98-26E2-11CF-A641-E996DF2C9318}
# 	2:5:Class:CMarkerCollection
# 	2:10:HeaderFile:markercollection.h
# 	2:8:ImplFile:markercollection.cpp
# End Section
# Section DSealect : {5DF9BF71-59A9-11D0-B476-0020AFD59EF6}
# 	2:5:Class:CPercentageVF
# 	2:10:HeaderFile:percentagevf.h
# 	2:8:ImplFile:percentagevf.cpp
# End Section
# Section DSealect : {FC2C9F1C-26A2-11CF-A641-E996DF2C9318}
# 	2:5:Class:CTimeScale
# 	2:10:HeaderFile:timescale.h
# 	2:8:ImplFile:timescale.cpp
# End Section
# Section DSealect : {7BF6C920-333E-11D0-8126-0020AF19EE14}
# 	2:5:Class:CPolar
# 	2:10:HeaderFile:polar.h
# 	2:8:ImplFile:polar.cpp
# End Section
# Section DSealect : {FC2C9F10-26A2-11CF-A641-E996DF2C9318}
# 	2:5:Class:CChartArea
# 	2:10:HeaderFile:chartarea.h
# 	2:8:ImplFile:chartarea.cpp
# End Section
# Section DSealect : {5DF9BF6D-59A9-11D0-B476-0020AFD59EF6}
# 	2:5:Class:CCurrencyVF
# 	2:10:HeaderFile:currencyvf.h
# 	2:8:ImplFile:currencyvf.cpp
# End Section
# Section DSealect : {D86FB468-2743-11CF-A641-E996DF2C9318}
# 	2:5:Class:CChartStyleCollection
# 	2:10:HeaderFile:chartstylecollection.h
# 	2:8:ImplFile:chartstylecollection.cpp
# End Section
# Section DSealect : {5DF9BF61-59A9-11D0-B476-0020AFD59EF6}
# 	2:5:Class:CDerivedBoolean
# 	2:10:HeaderFile:derivedboolean.h
# 	2:8:ImplFile:derivedboolean.cpp
# End Section
# Section DSealect : {D86FB46A-2743-11CF-A641-E996DF2C9318}
# 	2:5:Class:CLabelCollection
# 	2:10:HeaderFile:labelcollection.h
# 	2:8:ImplFile:labelcollection.cpp
# End Section
# Section DSealect : {FC2C9F20-26A2-11CF-A641-E996DF2C9318}
# 	2:5:Class:CDerivedDouble
# 	2:10:HeaderFile:deriveddouble.h
# 	2:8:ImplFile:deriveddouble.cpp
# End Section
# Section DSealect : {3A01BC17-BA9F-11D2-B3B4-00104B2556D9}
# 	2:21:DefaultSinkHeaderFile:dianax.h
# 	2:16:DefaultSinkClass:CDianaX
# End Section
# Section DSealect : {C23B5F18-79F1-11D0-813F-0020AF19EE14}
# 	2:5:Class:CChart2DCheckNewPointsResult
# 	2:10:HeaderFile:chart2dchecknewpointsresult.h
# 	2:8:ImplFile:chart2dchecknewpointsresult.cpp
# End Section
# Section DSealect : {BEF6E003-A874-101A-8BBA-00AA00300CAB}
# 	2:5:Class:COleFont
# 	2:10:HeaderFile:font.h
# 	2:8:ImplFile:font.cpp
# End Section
# Section DSealect : {F4CC7DE2-9AE0-11CF-B9E2-00A024169263}
# 	2:5:Class:CActionMapCollection
# 	2:10:HeaderFile:actionmapcollection.h
# 	2:8:ImplFile:actionmapcollection.cpp
# End Section
# Section DSealect : {BCC503C6-2630-11D2-BD0B-006097CF02D2}
# 	2:5:Class:CBubble
# 	2:10:HeaderFile:bubble.h
# 	2:8:ImplFile:bubble.cpp
# End Section
# Section DSealect : {C23B5F13-79F1-11D0-813F-0020AF19EE14}
# 	2:5:Class:CChart2DDataCoordResult
# 	2:10:HeaderFile:chart2ddatacoordresult.h
# 	2:8:ImplFile:chart2ddatacoordresult.cpp
# End Section
# Section DSealect : {9DB2A901-616C-47B8-976F-BF1B836A5A33}
# 	2:5:Class:CAxisX
# 	2:10:HeaderFile:axisx.h
# 	2:8:ImplFile:axisx.cpp
# End Section
# Section DSealect : {FC2C9F14-26A2-11CF-A641-E996DF2C9318}
# 	2:5:Class:CView3D
# 	2:10:HeaderFile:view3d.h
# 	2:8:ImplFile:view3d.cpp
# End Section
# Section DSealect : {8847CF30-2610-11CF-A640-9986B64D9618}
# 	2:5:Class:CInterior
# 	2:10:HeaderFile:interior.h
# 	2:8:ImplFile:interior.cpp
# End Section
# Section DSealect : {FC2C9F24-26A2-11CF-A641-E996DF2C9318}
# 	2:5:Class:CLineStyle
# 	2:10:HeaderFile:linestyle.h
# 	2:8:ImplFile:linestyle.cpp
# End Section
# Section DSealect : {5DF9BF75-59A9-11D0-B476-0020AFD59EF6}
# 	2:5:Class:CScientificVF
# 	2:10:HeaderFile:scientificvf.h
# 	2:8:ImplFile:scientificvf.cpp
# End Section
# Section DSealect : {D86FB46E-2743-11CF-A641-E996DF2C9318}
# 	2:5:Class:CValueLabelCollection
# 	2:10:HeaderFile:valuelabelcollection.h
# 	2:8:ImplFile:valuelabelcollection.cpp
# End Section
# Section DSealect : {D86FB462-2743-11CF-A641-E996DF2C9318}
# 	2:5:Class:CChartGroup
# 	2:10:HeaderFile:chartgroup.h
# 	2:8:ImplFile:chartgroup.cpp
# End Section
# Section DSealect : {245631F0-F6C4-11CF-811C-0020AF19EE14}
# 	2:5:Class:CHiLoOpenClose
# 	2:10:HeaderFile:hiloopenclose.h
# 	2:8:ImplFile:hiloopenclose.cpp
# End Section
# Section DSealect : {05478A92-26E2-11CF-A641-E996DF2C9318}
# 	2:5:Class:CDataCoord
# 	2:10:HeaderFile:datacoord.h
# 	2:8:ImplFile:datacoord.cpp
# End Section
# Section DSealect : {781CFCA0-3165-11D2-AF2B-00A024C3C1C2}
# 	2:5:Class:CSliceStyle
# 	2:10:HeaderFile:slicestyle.h
# 	2:8:ImplFile:slicestyle.cpp
# End Section
# Section DSealect : {5DF9BF79-59A9-11D0-B476-0020AFD59EF6}
# 	2:5:Class:CValueFormat
# 	2:10:HeaderFile:valueformat.h
# 	2:8:ImplFile:valueformat.cpp
# End Section
# Section DSealect : {7BF80981-BF32-101A-8BBB-00AA00300CAB}
# 	2:5:Class:CPicture
# 	2:10:HeaderFile:picture.h
# 	2:8:ImplFile:picture.cpp
# End Section
# Section DSealect : {D86FB472-2743-11CF-A641-E996DF2C9318}
# 	2:5:Class:CChartLabelCollection
# 	2:10:HeaderFile:chartlabelcollection.h
# 	2:8:ImplFile:chartlabelcollection.cpp
# End Section
# Section DSealect : {92D71E98-25A8-11CF-A640-9986B64D9618}
# 	2:5:Class:CBorder
# 	2:10:HeaderFile:border.h
# 	2:8:ImplFile:border.cpp
# End Section
# Section DSealect : {8847CF34-2610-11CF-A640-9986B64D9618}
# 	2:5:Class:CLocation
# 	2:10:HeaderFile:location.h
# 	2:8:ImplFile:location.cpp
# End Section
# Section DSealect : {0575B1A9-7631-4484-94D5-7EBA922B1CBA}
# 	2:21:DefaultSinkHeaderFile:fgvrunctr.h
# 	2:16:DefaultSinkClass:CFgvRunCtr
# End Section
# Section DSealect : {92D71E9A-25A8-11CF-A640-9986B64D9618}
# 	2:5:Class:CTitle
# 	2:10:HeaderFile:title.h
# 	2:8:ImplFile:title.cpp
# End Section
# Section DSealect : {05478A96-26E2-11CF-A641-E996DF2C9318}
# 	2:5:Class:CCoord
# 	2:10:HeaderFile:coord.h
# 	2:8:ImplFile:coord.cpp
# End Section
# Section DSealect : {92D71E93-25A8-11CF-A640-9986B64D9618}
# 	2:21:DefaultSinkHeaderFile:chart2d.h
# 	2:16:DefaultSinkClass:CChart2D
# End Section
# Section DSealect : {FC2C9F18-26A2-11CF-A641-E996DF2C9318}
# 	2:5:Class:CFillStyle
# 	2:10:HeaderFile:fillstyle.h
# 	2:8:ImplFile:fillstyle.cpp
# End Section
# Section DSealect : {FC2C9F28-26A2-11CF-A641-E996DF2C9318}
# 	2:5:Class:CGridLines
# 	2:10:HeaderFile:gridlines.h
# 	2:8:ImplFile:gridlines.cpp
# End Section
