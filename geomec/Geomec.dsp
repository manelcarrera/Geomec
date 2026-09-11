# Microsoft Developer Studio Project File - Name="Geomec" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Geomec - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Geomec.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Geomec.mak" CFG="Geomec - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Geomec - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Geomec - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
F90=df.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Geomec - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "../lib/CutSurfaceUtil" /I "../lib/Tensor/include" /I "$(LBSP)/Tensor/include" /I "../lib/FieldValueFile/include" /I "$(LBSP)/FieldValueFile/include" /I "../lib/Tensor/Include" /I "$(LBOBJ)\include30" /I "../Lib/TetMesh" /I "d:\Users\all\psapi" /I "../lib/GuiFrame/include" /I "$(LBSP)/GuiFrame/include" /I "../lib/MfcGuiFrame/include" /I "../lib/ModelFrame/include" /I "$(LBSP)/ModelFrame/include" /I "../Lib/OpenGLFrame/include" /I "$(LBSP)/OpenGLFrame/include" /I "src" /I "2D\src" /I "hexamodel\src" /I "tetramodel\src" /I "tetramodel\eclipse\src" /I "tetramodel\gocad" /I "basemodel\src" /I "value\src" /I "geology\src" /I "view\src" /I "geology\attridlg\src" /I "dstaborlink\src" /I "excel\src" /I "../lib/DoubleEditLib" /I "../lib/wellscheme" /I "../lib/wellscene3D" /I "../lib/wellschemeutils" /I "../Lib/Geometry/include" /I "$(LBSP)/Geometry/include" /I "../Lib/FemApp" /I "../Lib/Quantity/include" /I "../Lib/TSMesh" /I "../Lib/gmdatabase/include" /I "../Lib/RPN" /I "../Lib/DianaCore" /I "../WellScheme" /I "../WellSchemeUtils" /I "d:\users\all\HtmlHelpAPI" /I "_$(QTDIR)\include" /I "$(QTDIR)\include" /I "../lib/DCasingLib/DCasing" /I "../lib/2DWellViewLib" /I "../lib/MeshDataCacher" /I "../lib/xmllib" /I "../lib/triangle/include" /I "$(LBSP)/triangle/include" /I "../Lib/TetMesh/include" /I "$(LBSP)/TetMesh/include" /I "$(LBOBJ)\lb\fl44" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /Yu"stdafx.h" /FD /Zm400 /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 $(QTLIB) fl44.lib cp40.lib dc32.lib opengl32.lib glu32.lib glib-1.3.lib cx40.lib ds30.lib HtmlHelp.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imm32.lib wsock32.lib winspool.lib winmm.lib ci30.lib psapi.lib el30.lib gs30.lib /nologo /subsystem:windows /incremental:yes /map /machine:I386 /libpath:"../lib/ModelObjectFactory/Release" /libpath:"../lib/CutsurfaceUtil/Release" /libpath:"../lib/Triangle/Release" /libpath:"../lib/Geometry/Release" /libpath:"../lib/modelframe/release" /libpath:"../lib/2DwellViewLib/Release" /libpath:"../lib/QDianaStartUp/Release" /libpath:"../lib/DCasingLib/DCasing/Release" /libpath:"../lib/xmllib/Release" /libpath:"../lib/wellschemeutils/Release" /libpath:"../lib/wellscheme/Release" /libpath:"../Lib/rpn/Release" /libpath:"../Lib/tetmesh/Release" /libpath:"../Lib/OpenGLFrame/Release" /libpath:"../Lib/FieldValueFile/Release" /libpath:"../Lib/gmdatabase/Release" /libpath:"../Lib/dag/Release" /libpath:"../Lib/FemApp/Release" /libpath:"../Lib/Quantity/Release" /libpath:"../Lib/TSMesh/Release" /libpath:"../Lib/DianaCore/Release" /libpath:"../Lib/MfcGuiFrame/Release" /libpath:"$(LBOBJ)\ci" /libpath:"$(LBOBJ)\lb" /libpath:"$(LBOBJ)\..\lib" /libpath:"$(DSPLIB)" /libpath:"d:\users\all\HtmlHelpAPI" /libpath:"$(QTDIR)/lib" /libpath:"d:\users\all\psapi" /libpath:"../lib/doubleeditlib/Release" /libpath:"../lib/meshdatacacher/Release" /libpath:"../lib/wellscene3d/Release" /libpath:"../lib/Tensor/Release" /libpath:"$(LBSP)"

!ELSEIF  "$(CFG)" == "Geomec - Win32 Debug"

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
# ADD F90 /browser
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GR /GX /ZI /Od /I "../lib/CutSurfaceUtil" /I "../lib/Tensor/include" /I "$(LBSP)/Tensor/include" /I "../lib/FieldValueFile/include" /I "$(LBSP)/FieldValueFile/include" /I "$(LBOBJ)\include30" /I "d:\Users\all\psapi" /I "../lib/GuiFrame/include" /I "$(LBSP)/GuiFrame/include" /I "../lib/MfcGuiFrame/include" /I "../lib/ModelFrame/include" /I "$(LBSP)/ModelFrame/include" /I "../Lib/OpenGLFrame/include" /I "$(LBSP)/OpenGLFrame/include" /I "src" /I "2D\src" /I "hexamodel\src" /I "tetramodel\src" /I "tetramodel\eclipse\src" /I "tetramodel\gocad" /I "basemodel\src" /I "value\src" /I "geology\src" /I "view\src" /I "geology\attridlg\src" /I "dstaborlink\src" /I "excel\src" /I "../lib/DoubleEditLib" /I "../lib/wellscheme" /I "../lib/wellscene3D" /I "../lib/wellschemeutils" /I "../Lib/Geometry/include" /I "$(LBSP)/Geometry/include" /I "../Lib/FemApp" /I "../Lib/Quantity/include" /I "../Lib/TSMesh" /I "../Lib/gmdatabase/include" /I "../Lib/RPN" /I "../Lib/DianaCore" /I "../WellScheme" /I "../WellSchemeUtils" /I "d:\users\all\HtmlHelpAPI" /I "_$(QTDIR)\include" /I "$(QTDIR)\include" /I "../lib/DCasingLib/DCasing" /I "../lib/2DWellViewLib" /I "../lib/MeshDataCacher" /I "../lib/xmllib" /I "../lib/triangle/include" /I "$(LBSP)/triangle/include" /I "../Lib/TetMesh/include" /I "$(LBSP)/TetMesh/include" /I "$(LBOBJ)\lb\fl44" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "TRACEINCLUDES" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /Yu"stdafx.h" /FD /Zm400 /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 $(QTLIB) fl44.lib cp40.lib dc32.lib opengl32.lib glu32.lib glib-1.3.lib cx40.lib ds30.lib HtmlHelp.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imm32.lib wsock32.lib winspool.lib winmm.lib ci30.lib psapi.lib el30.lib gs30.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:"../lib/ModelObjectFactory/Debug" /libpath:"../lib/CutsurfaceUtil/Debug" /libpath:"../lib/triangle/Debug" /libpath:"../lib/Geometry/Debug" /libpath:"../lib/modelframe/debug" /libpath:"../lib/2DwellViewLib/Debug" /libpath:"../lib/QDianaStartUp/Debug" /libpath:"../lib/DCasingLib/DCasing/Debug" /libpath:"../lib/xmllib/debug" /libpath:"../lib/wellschemeutils/debug" /libpath:"../lib/wellscheme/debug" /libpath:"../Lib/rpn/debug" /libpath:"../Lib/tetmesh/debug" /libpath:"../Lib/OpenGLFrame/debug" /libpath:"../Lib/FieldValueFile/debug" /libpath:"../Lib/gmdatabase/debug" /libpath:"../Lib/FemApp/debug" /libpath:"../Lib/Quantity/debug" /libpath:"../Lib/TSMesh/debug" /libpath:"../Lib/DianaCore/debug" /libpath:"../Lib/MfcGuiFrame/Debug" /libpath:"../lib/doubleeditlib/Debug" /libpath:"../lib/meshdatacacher/Debug" /libpath:"../lib/wellscene3d/Debug" /libpath:"../lib/Tensor/Debug" /libpath:"$(LBSP)" /libpath:"$(LBOBJ)\lb" /libpath:"$(LBOBJ)\ci" /libpath:"$(LBOBJ)\..\lib" /libpath:"$(LBOBJ)\lib" /libpath:"$(DSPLIB)" /libpath:"d:\users\all\HtmlHelpAPI" /libpath:"$(QTDIR)/lib" /libpath:"d:\users\all\psapi"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Geomec - Win32 Release"
# Name "Geomec - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\2D\src\2DAnalysisPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DBackgroundNode.cpp
# End Source File
# Begin Source File

SOURCE=.\2D\src\2dboundary.cpp
# End Source File
# Begin Source File

SOURCE=.\2d\src\2dboundarydlg.cpp
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DCompartment.cpp
# End Source File
# Begin Source File

SOURCE=.\2D\src\2dcreatemodeldlg.cpp
# ADD CPP /I "..\..\SRC"
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DCrossSection.cpp
# End Source File
# Begin Source File

SOURCE=.\2d\src\2DDataScene.cpp
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DFault.cpp
# End Source File
# Begin Source File

SOURCE=.\2d\src\2DFaultCreator.cpp
# End Source File
# Begin Source File

SOURCE=.\2d\src\2DFaultOffsetCreator.cpp
# End Source File
# Begin Source File

SOURCE=.\2d\src\2DFaultOffsetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DFormation.cpp
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DGeologyScene.cpp
# End Source File
# Begin Source File

SOURCE=.\2d\src\2DGraphScene.cpp
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DHorizon.cpp
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DHorizonBase.cpp
# End Source File
# Begin Source File

SOURCE=.\2d\src\2DHorizonMerger.cpp
# End Source File
# Begin Source File

SOURCE=.\2d\src\2DIntersectedMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DMesher.cpp
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DMeshScene.cpp
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DModel.cpp
# End Source File
# Begin Source File

SOURCE=.\2d\src\2DModelAnalyser.cpp
# End Source File
# Begin Source File

SOURCE=.\2d\src\2DPointModifier.cpp
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DPolyline.cpp
# End Source File
# Begin Source File

SOURCE=.\2d\src\2DPreviewDialogBase.cpp
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DScene.cpp
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DSegment.cpp
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DVertex.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\2DWellpathGraphScene.cpp
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\3DFormation.cpp
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\3DHorizon.cpp
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\3dmodel.cpp
# End Source File
# Begin Source File

SOURCE=.\src\analysislogdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\AnalysisPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\2D\src\attri2danalysispoint.cpp
# ADD CPP /I "..\..\SRC"
# End Source File
# Begin Source File

SOURCE=.\2D\src\attri2dboundary.cpp
# End Source File
# Begin Source File

SOURCE=.\2D\src\attri2dcompartment.cpp
# ADD CPP /I "..\..\SRC"
# End Source File
# Begin Source File

SOURCE=.\2D\src\attri2dcrosssection.cpp
# End Source File
# Begin Source File

SOURCE=.\2D\src\attri2dvertex.cpp
# ADD CPP /I "..\..\SRC"
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\attrianalysispoint.cpp
# ADD CPP /I "..\..\src"
# End Source File
# Begin Source File

SOURCE=.\geology\attridlg\src\attriboundarydlg.cpp
# End Source File
# Begin Source File

SOURCE=.\geology\attridlg\src\attricolorgradientdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\view\src\attricolorscaleentrydlg.cpp
# ADD CPP /I "..\..\src"
# End Source File
# Begin Source File

SOURCE=.\tetramodel\eclipse\src\attrieclipsesubmodel.cpp
# End Source File
# Begin Source File

SOURCE=.\value\src\attrielementvalueset.cpp
# End Source File
# Begin Source File

SOURCE=.\src\AttriFaultPressureDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\attriformation.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\AttriFormPlane.cpp
# End Source File
# Begin Source File

SOURCE=.\geology\attridlg\src\attriglobalpressuredlg.cpp
# End Source File
# Begin Source File

SOURCE=.\geology\attridlg\src\attriglobalstressdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\HexaModel\SRC\attrihexaformation.cpp
# End Source File
# Begin Source File

SOURCE=.\HexaModel\SRC\attrihexahorizon.cpp
# End Source File
# Begin Source File

SOURCE=.\HexaModel\SRC\attrihexameshregiondlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\AttriHotSpotDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\attrimesh.cpp
# ADD CPP /I "..\..\SRC"
# End Source File
# Begin Source File

SOURCE=.\geology\attridlg\src\attrinodalvalueset.cpp
# End Source File
# Begin Source File

SOURCE=.\geology\attridlg\src\AttriPointSetUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\geology\attridlg\src\attripressuredlg.cpp
# End Source File
# Begin Source File

SOURCE=.\view\src\attriscene.cpp
# End Source File
# Begin Source File

SOURCE=.\geology\attridlg\src\attrisurfacedlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\AttriTemperatureDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\AttriTetMeshZone.cpp
# End Source File
# Begin Source File

SOURCE=.\src\AttriTetraVolume.cpp
# End Source File
# Begin Source File

SOURCE=.\geology\attridlg\src\attrivaluetypedlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Geology\AttriDlg\SRC\attriwellpathdlg.cpp
# ADD CPP /I "..\..\..\SRC"
# End Source File
# Begin Source File

SOURCE=.\Geology\AttriDlg\SRC\attrixsecdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\AxisTranslator.cpp
# End Source File
# Begin Source File

SOURCE=.\value\src\BaseResultSet.cpp
# End Source File
# Begin Source File

SOURCE=.\view\src\BirdEyeScene.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\BoundaryBase.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\BoundaryInterfaceMaterial.cpp
# End Source File
# Begin Source File

SOURCE=.\src\BranchState.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CalculationProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\casinggrade.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\Clipboard.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ColorGradient.cpp
# End Source File
# Begin Source File

SOURCE=.\view\src\ColorScaleEntry.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ComboListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\CrossSection.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CrossSectionFrom2D.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CrossSectionFrom2DEntry.cpp
# End Source File
# Begin Source File

SOURCE=.\view\src\CrossSectionScene.cpp
# End Source File
# Begin Source File

SOURCE=.\View\SRC\DataTreeView.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\DCasingLogRatDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DCasingResult.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DCasintAdvancedDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DCasintCalculation.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DCasintCalculationDlg.cpp

!IF  "$(CFG)" == "Geomec - Win32 Release"

!ELSEIF  "$(CFG)" == "Geomec - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\DCasintDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DCasintPointPickDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\DCasintView.cpp
# End Source File
# Begin Source File

SOURCE=.\view\src\DefaultScene.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\DepletionStage.cpp
# End Source File
# Begin Source File

SOURCE=.\value\src\DerivedResult.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DianaEnv.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DianaRunController.cpp
# End Source File
# Begin Source File

SOURCE=.\value\src\dimensiondlg.cpp
# End Source File
# Begin Source File

SOURCE=.\tetramodel\src\DisplacementSupportNode.cpp
# End Source File
# Begin Source File

SOURCE=.\tetramodel\eclipse\src\EclipseBody.cpp
# End Source File
# Begin Source File

SOURCE=.\TetraModel\Eclipse\SRC\eclipsefilepage.cpp
# ADD CPP /I "..\..\..\SRC"
# End Source File
# Begin Source File

SOURCE=.\TetraModel\Eclipse\SRC\eclipsehorizonpage.cpp
# ADD CPP /I "..\..\..\src"
# End Source File
# Begin Source File

SOURCE=.\TetraModel\Eclipse\SRC\EclipseModel.cpp
# End Source File
# Begin Source File

SOURCE=.\value\src\ElementSet.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ElementValueSet.cpp
# End Source File
# Begin Source File

SOURCE=.\Excel\src\excel8.cpp
# End Source File
# Begin Source File

SOURCE=.\src\exportdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ExportFormat.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\ExportGocadDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\tetramodel\src\facesupportdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\geology\src\FaultPressure.cpp
# End Source File
# Begin Source File

SOURCE=.\src\FileCopier.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\FistExport.cpp

!IF  "$(CFG)" == "Geomec - Win32 Release"

!ELSEIF  "$(CFG)" == "Geomec - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\FistExportDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\FistPointPickDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\FormationBase.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\FormationPlane.cpp
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\FormationVolume.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\Geomec.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\Geomec.rc
# End Source File
# Begin Source File

SOURCE=.\2D\src\Geomec2DDianaRunner.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Geomec3DDianaRunner.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GeomecDianaRunner.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GeomecDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GeomecSplitterWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\src\geomecstatusbar.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\GeomecTime.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GeomecUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GeoProgress.cpp
# End Source File
# Begin Source File

SOURCE=.\geology\src\geosurface.cpp
# End Source File
# Begin Source File

SOURCE=.\value\src\GlobalInitialStessNode.cpp
# End Source File
# Begin Source File

SOURCE=.\geology\src\GlobalPressure.cpp
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\GlobalTemperature.cpp
# End Source File
# Begin Source File

SOURCE=.\tetramodel\src\globaltensordlg.cpp
# ADD CPP /I "..\..\SRC"
# End Source File
# Begin Source File

SOURCE=.\HexaModel\SRC\Gm3HexaModel.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\GocadExport.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\GocadExportWrapper.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\GocadImport.cpp
# End Source File
# Begin Source File

SOURCE=.\TetraModel\GoCad\GocadMeshImporter.cpp
# End Source File
# Begin Source File

SOURCE=.\TetraModel\GoCad\GoCadModel.cpp
# End Source File
# Begin Source File

SOURCE=.\HexaModel\SRC\HexaBoundary.cpp
# End Source File
# Begin Source File

SOURCE=.\HexaModel\SRC\HexaFormation.cpp
# End Source File
# Begin Source File

SOURCE=.\HexaModel\SRC\HexaHorizon.cpp
# End Source File
# Begin Source File

SOURCE=.\HexaModel\SRC\HexaMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\HexaModel\SRC\hexameshdepthdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\HexaModel\SRC\hexameshregion.cpp
# End Source File
# Begin Source File

SOURCE=.\HexaModel\SRC\HexaModel.cpp
# End Source File
# Begin Source File

SOURCE=.\HexaModel\SRC\hexamodelsaveload.cpp
# End Source File
# Begin Source File

SOURCE=.\HexaModel\SRC\HexaSurface.cpp
# End Source File
# Begin Source File

SOURCE=.\view\src\historyrangedlg.cpp
# ADD CPP /I "..\..\SRC"
# End Source File
# Begin Source File

SOURCE=.\view\src\HistoryScene.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\homoboxattridlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\HomogenizationBox.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\homogenizationresultsdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\HomogenizationRunner.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\HorizonBase.cpp
# End Source File
# Begin Source File

SOURCE=.\geology\attridlg\src\horizondeldlg.cpp
# End Source File
# Begin Source File

SOURCE=.\view\src\HotSpot.cpp
# End Source File
# Begin Source File

SOURCE=.\value\src\IElementSet.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ifaceelmmat.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\IMaterial.cpp
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\IMaterialRock.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ImportFormat.cpp
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\InitialTemperatureDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\InPlaceCombo.cpp
# End Source File
# Begin Source File

SOURCE=.\TetraModel\SRC\intfaceattrdlg.cpp
# ADD CPP /I "..\..\SRC"
# End Source File
# Begin Source File

SOURCE=.\value\src\IPointSet.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\IScalingBox.cpp
# End Source File
# Begin Source File

SOURCE=.\value\src\IStrainTensorGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\value\src\IStressTensorGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\value\src\ITensorGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\value\src\IValueSet.cpp
# End Source File
# Begin Source File

SOURCE=.\value\src\IVectorResult.cpp
# End Source File
# Begin Source File

SOURCE=.\src\KeyFile.cpp
# End Source File
# Begin Source File

SOURCE=.\view\src\legendview.cpp
# End Source File
# Begin Source File

SOURCE=.\src\linstaparamsdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ListCtrlEx.cpp
# End Source File
# Begin Source File

SOURCE=.\src\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\Material.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\MaterialCamClay.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\MaterialCreep.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\MaterialEntry.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\MaterialLinear.cpp
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\materialmccohesionhard1.cpp
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\materialmccohesionhard2.cpp
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\materialmccohesionhard3.cpp
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\materialmcfrictionhard1.cpp
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\materialmcfrictionhard2.cpp
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\materialmcfrictionhard3.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\MaterialModMohrCo.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\MaterialMohrCo.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\MaterialParameter.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\MaterialRigidity.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\MaterialServer.cpp
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\MaterialUndrained.cpp
# End Source File
# Begin Source File

SOURCE=.\src\matlibx5.cpp
# End Source File
# Begin Source File

SOURCE=.\src\matlibx5param.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\MeshBase.cpp
# End Source File
# Begin Source File

SOURCE=.\src\MeshMessageDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\MeshRegionBase.cpp
# End Source File
# Begin Source File

SOURCE=.\value\src\MeshResultTree.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\MeshSurface.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\meshtolerancedlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\moc_DCasintCalculation.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\src\moc_DCasintCalculationDlg.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\geology\src\moc_WellPathProgress.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\ModelBase.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\ModelGeneralPg.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\modelgeologypg.cpp
# End Source File
# Begin Source File

SOURCE=.\view\src\ModelObserver.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\modeltimesteppg.cpp
# End Source File
# Begin Source File

SOURCE=.\View\SRC\ModelTreeView.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\ModelValidationDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\View\SRC\ModelView.cpp
# End Source File
# Begin Source File

SOURCE=.\src\MonthCalDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\NewModelDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\tetramodel\src\nodalsupportdlg.cpp
# ADD CPP /I "..\..\SRC"
# End Source File
# Begin Source File

SOURCE=.\value\src\NodalValueSet.cpp
# End Source File
# Begin Source File

SOURCE=.\view\src\NodeLocationHighlighter.cpp
# End Source File
# Begin Source File

SOURCE=.\src\NonlinParams.cpp
# End Source File
# Begin Source File

SOURCE=.\view\src\NormalOpenGLScene.cpp
# End Source File
# Begin Source File

SOURCE=.\view\src\OpenGL2DScene.cpp
# End Source File
# Begin Source File

SOURCE=.\src\OpenGLDlgView.cpp
# End Source File
# Begin Source File

SOURCE=.\view\src\OpenGLSceneBase.cpp
# End Source File
# Begin Source File

SOURCE=.\View\SRC\OpenGLSplitView.cpp
# End Source File
# Begin Source File

SOURCE=.\geology\src\PointSet.cpp
# End Source File
# Begin Source File

SOURCE=.\src\pointsetcreatedlg.cpp
# End Source File
# Begin Source File

SOURCE=.\view\src\PointsetEntryObserver.cpp
# End Source File
# Begin Source File

SOURCE=.\geology\src\Pressure.cpp
# End Source File
# Begin Source File

SOURCE=.\geology\src\PressureBase.cpp
# End Source File
# Begin Source File

SOURCE=.\tetramodel\src\PressureSupportNode.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ProgressDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\PropertyExcel.cpp
# End Source File
# Begin Source File

SOURCE=.\src\PropertyFile.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\registryxdll.cpp
# End Source File
# Begin Source File

SOURCE=.\value\src\Result.cpp
# End Source File
# Begin Source File

SOURCE=.\Value\SRC\ResultCache.cpp
# End Source File
# Begin Source File

SOURCE=.\value\src\resultexportdlg.cpp
# ADD CPP /I "..\..\src"
# End Source File
# Begin Source File

SOURCE=.\value\src\ResultObserver.cpp
# End Source File
# Begin Source File

SOURCE=.\value\src\ResultRegister.cpp
# End Source File
# Begin Source File

SOURCE=.\value\src\resultselectdlg.cpp
# ADD CPP /I "..\..\SRC"
# End Source File
# Begin Source File

SOURCE=.\value\src\ResultTree.cpp
# End Source File
# Begin Source File

SOURCE=.\value\src\RPNDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\value\src\RpnEditFormulaDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\value\src\RpnFormationOperand.cpp
# End Source File
# Begin Source File

SOURCE=.\value\src\RpnValueSet.cpp
# End Source File
# Begin Source File

SOURCE=.\value\src\ScalarValueSet.cpp
# End Source File
# Begin Source File

SOURCE=.\TetraModel\SRC\SelectGocadSolidDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\ShellGoCadFile.cpp
# End Source File
# Begin Source File

SOURCE=.\src\splah.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\SpringElement.cpp
# End Source File
# Begin Source File

SOURCE=.\value\src\StaborResult.cpp
# End Source File
# Begin Source File

SOURCE=.\geology\src\StatusValue.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\StdAfx.cpp

!IF  "$(CFG)" == "Geomec - Win32 Release"

# ADD CPP /Yc

!ELSEIF  "$(CFG)" == "Geomec - Win32 Debug"

# ADD CPP /Yc""

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\value\src\stress.cpp
# End Source File
# Begin Source File

SOURCE=.\value\src\StressComponentProxy.cpp
# End Source File
# Begin Source File

SOURCE=.\tetramodel\src\SupportDlgBase.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\surfacemergedefdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Temperature.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TempPath.cpp
# End Source File
# Begin Source File

SOURCE=.\TetraModel\SRC\TetraBoundary.cpp
# End Source File
# Begin Source File

SOURCE=.\TetraModel\SRC\TetraFormation.cpp
# End Source File
# Begin Source File

SOURCE=.\TetraModel\SRC\TetraHorizonBase.cpp
# End Source File
# Begin Source File

SOURCE=.\TetraModel\SRC\TetraMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\TetraModel\SRC\TetraModel.cpp
# End Source File
# Begin Source File

SOURCE=.\TetraModel\SRC\TetraSubBoundary.cpp
# End Source File
# Begin Source File

SOURCE=.\TetraModel\SRC\TetraSubHorizon.cpp
# End Source File
# Begin Source File

SOURCE=.\TetraModel\SRC\TetraSuperHorizon.cpp
# End Source File
# Begin Source File

SOURCE=.\tetramodel\src\TetraSupportNode.cpp
# End Source File
# Begin Source File

SOURCE=.\TetraModel\SRC\TetraSurface.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TipDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\View\SRC\TopviewScene.cpp
# End Source File
# Begin Source File

SOURCE=.\View\SRC\TreeSplitView.cpp
# End Source File
# Begin Source File

SOURCE=.\geology\src\TSSurfaceProgress.cpp
# End Source File
# Begin Source File

SOURCE=.\value\src\ValueComponent.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ValueCompositeObserver.cpp
# End Source File
# Begin Source File

SOURCE=.\src\valuetype.cpp
# End Source File
# Begin Source File

SOURCE=.\value\src\ValueTypeFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\Value\SRC\ValueVector.cpp
# End Source File
# Begin Source File

SOURCE=.\value\src\VectorValueSet.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\versioninfo.cpp
# End Source File
# Begin Source File

SOURCE=.\geology\src\WellPath.cpp
# End Source File
# Begin Source File

SOURCE=.\Geology\SRC\WellPathFile.cpp
# End Source File
# Begin Source File

SOURCE=.\geology\src\WellPathFileUnitDlg.cpp
# ADD CPP /I "..\..\SRC"
# End Source File
# Begin Source File

SOURCE=.\src\wellpathgraphlimitsdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\geology\src\WellPathProgress.cpp
# End Source File
# Begin Source File

SOURCE=.\src\wizzardpagebase.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\2D\src\2DAnalysisPoint.h
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DBackgroundNode.h
# End Source File
# Begin Source File

SOURCE=.\2D\src\2dboundary.h
# End Source File
# Begin Source File

SOURCE=.\2d\src\2DBoundaryDlg.h
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DCompartment.h
# End Source File
# Begin Source File

SOURCE=.\2D\src\2dcreatemodeldlg.h
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DCrossSection.h
# End Source File
# Begin Source File

SOURCE=.\2d\src\2DDataScene.h
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DEntryTypes.h
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DFault.h
# End Source File
# Begin Source File

SOURCE=.\2d\src\2DFaultCreator.h
# End Source File
# Begin Source File

SOURCE=.\2d\src\2DFaultOffsetCreator.h
# End Source File
# Begin Source File

SOURCE=.\2d\src\2DFaultOffsetDlg.h
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DFormation.h
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DGeologyScene.h
# End Source File
# Begin Source File

SOURCE=.\2d\src\2DGraphScene.h
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DHorizon.h
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DHorizonBase.h
# End Source File
# Begin Source File

SOURCE=.\2d\src\2DHorizonMerger.h
# End Source File
# Begin Source File

SOURCE=.\2d\src\2DIntersectedMesh.h
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DMesh.h
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DMesher.h
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DMeshScene.h
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DModel.h
# End Source File
# Begin Source File

SOURCE=.\2d\src\2DModelAnalyser.h
# End Source File
# Begin Source File

SOURCE=.\2d\src\2DPointModifier.h
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DPolyline.h
# End Source File
# Begin Source File

SOURCE=.\2d\src\2DPreviewDialogBase.h
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DScene.h
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DSegment.h
# End Source File
# Begin Source File

SOURCE=.\2D\src\2DVertex.h
# End Source File
# Begin Source File

SOURCE=.\SRC\2DWellpathGraphScene.h
# PROP Ignore_Default_Tool 1
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\3DFormation.h
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\3DHorizon.h
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\3dmodel.h
# End Source File
# Begin Source File

SOURCE=.\src\analysislogdlg.h
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\AnalysisPoint.h
# End Source File
# Begin Source File

SOURCE=.\value\src\AngleComponentProxy.h
# End Source File
# Begin Source File

SOURCE=.\2D\src\attri2danalysispoint.h
# End Source File
# Begin Source File

SOURCE=.\2D\src\attri2dboundary.h
# End Source File
# Begin Source File

SOURCE=.\2D\src\attri2dcompartment.h
# End Source File
# Begin Source File

SOURCE=.\2D\src\attri2dcrosssection.h
# End Source File
# Begin Source File

SOURCE=.\2D\src\attri2dvertex.h
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\attrianalysispoint.h
# End Source File
# Begin Source File

SOURCE=.\geology\attridlg\src\attriboundarydlg.h
# End Source File
# Begin Source File

SOURCE=.\Geology\AttriDlg\SRC\AttributesTemplate.h
# End Source File
# Begin Source File

SOURCE=.\geology\attridlg\src\attricolorgradientdlg.h
# End Source File
# Begin Source File

SOURCE=.\view\src\attricolorscaleentrydlg.h
# End Source File
# Begin Source File

SOURCE=.\tetramodel\eclipse\src\attrieclipsesubmodel.h
# End Source File
# Begin Source File

SOURCE=.\value\src\attrielementvalueset.h
# End Source File
# Begin Source File

SOURCE=.\src\AttriFaultPressureDlg.h
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\attriformation.h
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\attriformationelementset.h
# End Source File
# Begin Source File

SOURCE=.\src\AttriFormationLoadDlg.h
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\AttriFormationTemplate.h
# End Source File
# Begin Source File

SOURCE=.\SRC\AttriFormPlane.h
# End Source File
# Begin Source File

SOURCE=.\src\AttriGlobalFormationLoadDlg.h
# End Source File
# Begin Source File

SOURCE=.\geology\attridlg\src\attriglobalpressuredlg.h
# End Source File
# Begin Source File

SOURCE=.\geology\attridlg\src\attriglobalstressdlg.h
# End Source File
# Begin Source File

SOURCE=.\HexaModel\SRC\attrihexaformation.h
# End Source File
# Begin Source File

SOURCE=.\HexaModel\SRC\attrihexahorizon.h
# End Source File
# Begin Source File

SOURCE=.\HexaModel\SRC\attrihexameshregiondlg.h
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\AttriHorizon.h
# End Source File
# Begin Source File

SOURCE=.\src\AttriHotSpotDlg.h
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\attrimesh.h
# End Source File
# Begin Source File

SOURCE=.\geology\attridlg\src\attrinodalvalueset.h
# End Source File
# Begin Source File

SOURCE=.\geology\attridlg\src\AttriPointSetDlg.h
# End Source File
# Begin Source File

SOURCE=.\geology\attridlg\src\AttriPointSetUtil.h
# End Source File
# Begin Source File

SOURCE=.\geology\attridlg\src\attripressuredlg.h
# End Source File
# Begin Source File

SOURCE=.\view\src\attriscene.h
# End Source File
# Begin Source File

SOURCE=.\Geology\AttriDlg\SRC\attrisurfacedlg.h
# End Source File
# Begin Source File

SOURCE=.\src\AttriTemperatureDlg.h
# End Source File
# Begin Source File

SOURCE=.\SRC\AttriTetMeshZone.h
# End Source File
# Begin Source File

SOURCE=.\src\AttriTetraVolume.h
# End Source File
# Begin Source File

SOURCE=.\geology\attridlg\src\attrivaluetypedlg.h
# End Source File
# Begin Source File

SOURCE=.\Geology\AttriDlg\SRC\attriwellpathdlg.h
# End Source File
# Begin Source File

SOURCE=.\Geology\AttriDlg\SRC\attrixsecdlg.h
# End Source File
# Begin Source File

SOURCE=.\src\AxisTranslator.h
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\BaseEntryTypes.h
# End Source File
# Begin Source File

SOURCE=.\value\src\BaseResultSet.h
# End Source File
# Begin Source File

SOURCE=.\view\src\BirdEyeScene.h
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\BoundaryBase.h
# End Source File
# Begin Source File

SOURCE=.\SRC\BoundaryInterfaceMaterial.h
# End Source File
# Begin Source File

SOURCE=.\src\BranchState.h
# End Source File
# Begin Source File

SOURCE=.\src\CalculationProperties.h
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\casinggrade.h
# End Source File
# Begin Source File

SOURCE=.\SRC\Clipboard.h
# End Source File
# Begin Source File

SOURCE=.\src\ColorGradient.h
# End Source File
# Begin Source File

SOURCE=.\view\src\ColorScaleEntry.h
# End Source File
# Begin Source File

SOURCE=.\src\ComboListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\value\src\ComponentProxyTemp.h
# End Source File
# Begin Source File

SOURCE=.\SRC\CrossSection.h
# End Source File
# Begin Source File

SOURCE=.\src\CrossSectionFrom2D.h
# End Source File
# Begin Source File

SOURCE=.\src\CrossSectionFrom2DEntry.h
# End Source File
# Begin Source File

SOURCE=.\view\src\CrossSectionScene.h
# End Source File
# Begin Source File

SOURCE=.\View\SRC\DataTreeView.h
# End Source File
# Begin Source File

SOURCE=.\SRC\DCasingLogRatDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\DCasingResult.h
# End Source File
# Begin Source File

SOURCE=.\src\DCasintAdvancedDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\DCasintCalculation.h

!IF  "$(CFG)" == "Geomec - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing DCasintCalculation.h...
InputDir=.\src
InputPath=.\src\DCasintCalculation.h
InputName=DCasintCalculation

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Geomec - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing DCasintCalculation.h...
InputDir=.\src
InputPath=.\src\DCasintCalculation.h
InputName=DCasintCalculation

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\DCasintCalculationDlg.h

!IF  "$(CFG)" == "Geomec - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing DCasintCalculationDlg.h...
InputDir=.\src
InputPath=.\src\DCasintCalculationDlg.h
InputName=DCasintCalculationDlg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Geomec - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing DCasintCalculationDlg.h...
InputDir=.\src
InputPath=.\src\DCasintCalculationDlg.h
InputName=DCasintCalculationDlg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\DCasintDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\DCasintPointPickDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\DCasintView.h
# End Source File
# Begin Source File

SOURCE=.\view\src\DefaultScene.h
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\DepletionStage.h
# End Source File
# Begin Source File

SOURCE=.\value\src\depthcomponent.h
# End Source File
# Begin Source File

SOURCE=.\value\src\DerivedResult.h
# End Source File
# Begin Source File

SOURCE=.\src\DianaEnv.h
# End Source File
# Begin Source File

SOURCE=.\src\DianaRunController.h
# End Source File
# Begin Source File

SOURCE=.\value\src\dimensiondlg.h
# End Source File
# Begin Source File

SOURCE=.\tetramodel\src\DisplacementSupportNode.h
# End Source File
# Begin Source File

SOURCE=.\tetramodel\eclipse\src\EclipseBody.h
# End Source File
# Begin Source File

SOURCE=.\TetraModel\Eclipse\SRC\eclipsefilepage.h
# End Source File
# Begin Source File

SOURCE=.\TetraModel\Eclipse\SRC\eclipsehorizonpage.h
# End Source File
# Begin Source File

SOURCE=.\TetraModel\Eclipse\SRC\EclipseModel.h
# End Source File
# Begin Source File

SOURCE=.\value\src\ElementSet.h
# End Source File
# Begin Source File

SOURCE=.\src\ElementValueSet.h
# End Source File
# Begin Source File

SOURCE=.\Excel\src\excel8.h
# End Source File
# Begin Source File

SOURCE=.\src\exportdlg.h
# End Source File
# Begin Source File

SOURCE=.\src\ExportFormat.h
# End Source File
# Begin Source File

SOURCE=.\SRC\ExportGocadDlg.h
# End Source File
# Begin Source File

SOURCE=.\tetramodel\src\facesupportdlg.h
# End Source File
# Begin Source File

SOURCE=.\geology\src\FaultPressure.h
# End Source File
# Begin Source File

SOURCE=.\value\src\FieldFactor.h
# End Source File
# Begin Source File

SOURCE=.\src\FileCopier.h
# End Source File
# Begin Source File

SOURCE=.\SRC\FistExport.h
# End Source File
# Begin Source File

SOURCE=.\SRC\FistExportDlg.h
# End Source File
# Begin Source File

SOURCE=.\SRC\FistPointPickDlg.h
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\FormationBase.h
# End Source File
# Begin Source File

SOURCE=.\src\FormationLoad.h
# End Source File
# Begin Source File

SOURCE=.\src\FormationLoadBase.h
# End Source File
# Begin Source File

SOURCE=.\src\FormationLoadComponent.h
# End Source File
# Begin Source File

SOURCE=.\src\FormationLoadGradientComponent.h
# End Source File
# Begin Source File

SOURCE=.\src\FormationLoadRepeaterComponent.h
# End Source File
# Begin Source File

SOURCE=.\SRC\FormationPlane.h
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\FormationVolume.h
# End Source File
# Begin Source File

SOURCE=.\SRC\Geomec.h
# End Source File
# Begin Source File

SOURCE=.\2D\src\Geomec2DDianaRunner.h
# End Source File
# Begin Source File

SOURCE=.\src\Geomec3DDianaRunner.h
# End Source File
# Begin Source File

SOURCE=.\src\GeomecDianaRunner.h
# End Source File
# Begin Source File

SOURCE=.\src\GeomecDoc.h
# End Source File
# Begin Source File

SOURCE=.\src\GeomecSplitterWnd.h
# End Source File
# Begin Source File

SOURCE=.\src\geomecstatusbar.h
# End Source File
# Begin Source File

SOURCE=.\SRC\GeomecTime.h
# End Source File
# Begin Source File

SOURCE=.\src\GeomecUtils.h
# End Source File
# Begin Source File

SOURCE=.\src\GeoProgress.h
# End Source File
# Begin Source File

SOURCE=.\geology\src\geosurface.h
# End Source File
# Begin Source File

SOURCE=.\src\GlobalFormationLoad.h
# End Source File
# Begin Source File

SOURCE=.\value\src\GlobalInitialStessNode.h
# End Source File
# Begin Source File

SOURCE=.\geology\src\GlobalPressure.h
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\GlobalTemperature.h
# End Source File
# Begin Source File

SOURCE=.\tetramodel\src\globaltensordlg.h
# End Source File
# Begin Source File

SOURCE=.\HexaModel\SRC\Gm3HexaModel.h
# End Source File
# Begin Source File

SOURCE=.\SRC\GocadExport.h
# End Source File
# Begin Source File

SOURCE=.\SRC\GocadExportWrapper.h
# End Source File
# Begin Source File

SOURCE=.\SRC\GocadImport.h
# End Source File
# Begin Source File

SOURCE=.\TetraModel\GoCad\GocadMeshImporter.h
# End Source File
# Begin Source File

SOURCE=.\TetraModel\GoCad\GoCadModel.h
# End Source File
# Begin Source File

SOURCE=.\HexaModel\SRC\HexaBoundary.h
# End Source File
# Begin Source File

SOURCE=.\HexaModel\SRC\HexaEntryTypes.h
# End Source File
# Begin Source File

SOURCE=.\HexaModel\SRC\HexaFormation.h
# End Source File
# Begin Source File

SOURCE=.\HexaModel\SRC\HexaHorizon.h
# End Source File
# Begin Source File

SOURCE=.\HexaModel\SRC\HexaMesh.h
# End Source File
# Begin Source File

SOURCE=.\HexaModel\SRC\hexameshdepthdlg.h
# End Source File
# Begin Source File

SOURCE=.\HexaModel\SRC\hexameshregion.h
# End Source File
# Begin Source File

SOURCE=.\HexaModel\SRC\HexaModel.h
# End Source File
# Begin Source File

SOURCE=.\HexaModel\SRC\HexaSurface.h
# End Source File
# Begin Source File

SOURCE=.\view\src\historyrangedlg.h
# End Source File
# Begin Source File

SOURCE=.\view\src\HistoryScene.h
# PROP Ignore_Default_Tool 1
# End Source File
# Begin Source File

SOURCE=.\SRC\homoboxattridlg.h
# End Source File
# Begin Source File

SOURCE=.\SRC\HomogenizationBox.h
# End Source File
# Begin Source File

SOURCE=.\SRC\homogenizationresultsdlg.h
# End Source File
# Begin Source File

SOURCE=.\SRC\HomogenizationRunner.h
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\HorizonBase.h
# End Source File
# Begin Source File

SOURCE=.\geology\attridlg\src\horizondeldlg.h
# End Source File
# Begin Source File

SOURCE=.\view\src\HotSpot.h
# End Source File
# Begin Source File

SOURCE=.\src\ifaceelmmat.h
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\IMaterial.h
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\IMaterialRock.h
# End Source File
# Begin Source File

SOURCE=.\src\ImportFormat.h
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\InitialTemperatureDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\InPlaceCombo.h
# End Source File
# Begin Source File

SOURCE=.\TetraModel\SRC\intfaceattrdlg.h
# End Source File
# Begin Source File

SOURCE=.\value\src\IPointSet.h
# End Source File
# Begin Source File

SOURCE=.\value\src\IProperty.h
# End Source File
# Begin Source File

SOURCE=.\SRC\IScalingBox.h
# End Source File
# Begin Source File

SOURCE=.\value\src\IStrainTensorGroup.h
# End Source File
# Begin Source File

SOURCE=.\value\src\IStressTensorGroup.h
# End Source File
# Begin Source File

SOURCE=.\value\src\ITensorGroup.h
# End Source File
# Begin Source File

SOURCE=.\value\src\ITensorGroupTemplate.h
# End Source File
# Begin Source File

SOURCE=.\value\src\IValueSet.h
# End Source File
# Begin Source File

SOURCE=.\value\src\IVectorResult.h
# End Source File
# Begin Source File

SOURCE=.\src\KeyFile.h
# End Source File
# Begin Source File

SOURCE=.\view\src\legendview.h
# End Source File
# Begin Source File

SOURCE=.\src\linstaparamsdlg.h
# End Source File
# Begin Source File

SOURCE=.\src\ListCtrlEx.h
# End Source File
# Begin Source File

SOURCE=.\src\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\Material.h
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\MaterialCamClay.h
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\MaterialCreep.h
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\MaterialEntry.h
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\MaterialLinear.h
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\materialmccohesionhard1.h
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\materialmccohesionhard2.h
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\materialmccohesionhard3.h
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\materialmcfrictionhard1.h
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\materialmcfrictionhard2.h
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\materialmcfrictionhard3.h
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\MaterialModMohrCo.h
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\MaterialMohrCo.h
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\MaterialParameter.h
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\MaterialRigidity.h
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\MaterialServer.h
# End Source File
# Begin Source File

SOURCE=.\basemodel\src\MaterialUndrained.h
# End Source File
# Begin Source File

SOURCE=.\src\matlibx5.h
# End Source File
# Begin Source File

SOURCE=.\src\MatLibX5Defines.h
# End Source File
# Begin Source File

SOURCE=.\src\matlibx5param.h
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\MeshBase.h
# End Source File
# Begin Source File

SOURCE=.\src\MeshMessageDlg.h
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\MeshRegionBase.h
# End Source File
# Begin Source File

SOURCE=.\value\src\MeshResultTree.h
# End Source File
# Begin Source File

SOURCE=.\SRC\MeshSurface.h
# End Source File
# Begin Source File

SOURCE=.\SRC\meshtolerancedlg.h
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\ModelBase.h
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\ModelGeneralPg.h
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\modelgeologypg.h
# End Source File
# Begin Source File

SOURCE=.\view\src\ModelObserver.h
# End Source File
# Begin Source File

SOURCE=.\BaseModel\SRC\modeltimesteppg.h
# End Source File
# Begin Source File

SOURCE=.\View\SRC\ModelTreeView.h
# End Source File
# Begin Source File

SOURCE=.\SRC\ModelValidationDlg.h
# End Source File
# Begin Source File

SOURCE=.\View\SRC\ModelView.h
# End Source File
# Begin Source File

SOURCE=.\src\MonthCalDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\NewModelDlg.h
# End Source File
# Begin Source File

SOURCE=.\tetramodel\src\nodalsupportdlg.h
# End Source File
# Begin Source File

SOURCE=.\value\src\NodalValueSet.h
# End Source File
# Begin Source File

SOURCE=.\view\src\NodeLocationHighlighter.h
# End Source File
# Begin Source File

SOURCE=.\src\NonlinParams.h
# End Source File
# Begin Source File

SOURCE=.\view\src\NormalOpenGLScene.h
# End Source File
# Begin Source File

SOURCE=.\view\src\OpenGL2DScene.h
# End Source File
# Begin Source File

SOURCE=.\src\OpenGLDlgView.h
# End Source File
# Begin Source File

SOURCE=.\view\src\OpenGLNodeObserver.h
# End Source File
# Begin Source File

SOURCE=.\view\src\OpenGLSceneBase.h
# End Source File
# Begin Source File

SOURCE=.\View\SRC\OpenGLSplitView.h
# End Source File
# Begin Source File

SOURCE=.\geology\src\PointSet.h
# End Source File
# Begin Source File

SOURCE=.\src\pointsetcreatedlg.h
# End Source File
# Begin Source File

SOURCE=.\view\src\PointsetEntryObserver.h
# End Source File
# Begin Source File

SOURCE=.\geology\src\Pressure.h
# End Source File
# Begin Source File

SOURCE=.\geology\src\PressureBase.h
# End Source File
# Begin Source File

SOURCE=.\tetramodel\src\PressureSupportNode.h
# End Source File
# Begin Source File

SOURCE=.\src\ProgressDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\PropertyExcel.h
# End Source File
# Begin Source File

SOURCE=.\src\PropertyFile.h
# End Source File
# Begin Source File

SOURCE=.\view\src\RadioEntryObserver.h
# End Source File
# Begin Source File

SOURCE=.\SRC\registryxdll.h
# End Source File
# Begin Source File

SOURCE=.\SRC\resource.h

!IF  "$(CFG)" == "Geomec - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Making HTML Help Include File...
InputDir=.\SRC
TargetName=Geomec
InputPath=.\SRC\resource.h

"Help\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	makehm ID_,IDH_,0x10000 IDM_,IDH_,0x10000 $(InputDir)\resource.h >"Help\$(TargetName).hm" 
	makehm IDP_,IDH_,0x30000 $(InputDir)\resource.h >>"Help\$(TargetName).hm" 
	makehm IDR_,IDH_,0x20000 $(InputDir)\resource.h >>"Help\$(TargetName).hm" 
	makehm IDD_,IDH_,0x20000 $(InputDir)\resource.h >>"Help\$(TargetName).hm" 
	makehm IDW_,IDH_,0x50000 $(InputDir)\resource.h >>"Help\$(TargetName).hm" 
	echo. >>"Help\$(TargetName).hm" 
	"c:\program files\HTML Help Workshop\MakeIDH.exe" "Help\$(TargetName).hm" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Geomec - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Making HTML Help Include File...
InputDir=.\SRC
TargetName=Geomec
InputPath=.\SRC\resource.h

"Help\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	makehm ID_,IDH_,0x10000 IDM_,IDH_,0x10000 $(InputDir)\resource.h >"Help\$(TargetName).hm" 
	makehm IDP_,IDH_,0x30000 $(InputDir)\resource.h >>"Help\$(TargetName).hm" 
	makehm IDR_,IDH_,0x20000 $(InputDir)\resource.h >>"Help\$(TargetName).hm" 
	makehm IDD_,IDH_,0x20000 $(InputDir)\resource.h >>"Help\$(TargetName).hm" 
	makehm IDW_,IDH_,0x50000 $(InputDir)\resource.h >>"Help\$(TargetName).hm" 
	echo. >>"Help\$(TargetName).hm" 
	"c:\program files\HTML Help Workshop\MakeIDH.exe" "Help\$(TargetName).hm" 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\value\src\Result.h
# End Source File
# Begin Source File

SOURCE=.\Value\SRC\ResultCache.h
# End Source File
# Begin Source File

SOURCE=.\value\src\resultexportdlg.h
# End Source File
# Begin Source File

SOURCE=.\value\src\ResultObserver.h
# End Source File
# Begin Source File

SOURCE=.\value\src\ResultRegister.h
# End Source File
# Begin Source File

SOURCE=.\value\src\resultselectdlg.h
# End Source File
# Begin Source File

SOURCE=.\value\src\ResultTree.h
# End Source File
# Begin Source File

SOURCE=.\value\src\rpn_version.h
# End Source File
# Begin Source File

SOURCE=.\value\src\RPNDlg.h
# End Source File
# Begin Source File

SOURCE=.\value\src\RpnEditFormulaDlg.h
# End Source File
# Begin Source File

SOURCE=.\value\src\RpnFormationOperand.h
# End Source File
# Begin Source File

SOURCE=.\value\src\RpnMaterialParameterProxy.h
# End Source File
# Begin Source File

SOURCE=.\value\src\RpnValueSet.h
# End Source File
# Begin Source File

SOURCE=.\value\src\ScalarValueSet.h
# End Source File
# Begin Source File

SOURCE=.\TetraModel\SRC\SelectGocadSolidDlg.h
# End Source File
# Begin Source File

SOURCE=.\value\src\SelectionObserver.h
# End Source File
# Begin Source File

SOURCE=.\SRC\ShellGoCadFile.h
# End Source File
# Begin Source File

SOURCE=.\src\splah.h
# End Source File
# Begin Source File

SOURCE=.\SRC\SpringElement.h
# End Source File
# Begin Source File

SOURCE=.\value\src\StaborResult.h
# End Source File
# Begin Source File

SOURCE=.\geology\src\StatusValue.h
# End Source File
# Begin Source File

SOURCE=.\SRC\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\value\src\stress.h
# End Source File
# Begin Source File

SOURCE=.\value\src\StressComponentProxy.h
# End Source File
# Begin Source File

SOURCE=.\tetramodel\src\SupportDlgBase.h
# End Source File
# Begin Source File

SOURCE=.\SRC\surfacemergedefdlg.h
# End Source File
# Begin Source File

SOURCE=.\src\Temperature.h
# End Source File
# Begin Source File

SOURCE=.\src\TempPath.h
# End Source File
# Begin Source File

SOURCE=.\value\src\TensorValueSetTemplate.h
# End Source File
# Begin Source File

SOURCE=.\TetraModel\SRC\TetraBoundary.h
# End Source File
# Begin Source File

SOURCE=.\TetraModel\SRC\TetraEntryTypes.h
# End Source File
# Begin Source File

SOURCE=.\TetraModel\SRC\TetraFormation.h
# End Source File
# Begin Source File

SOURCE=.\TetraModel\SRC\TetraHorizonBase.h
# End Source File
# Begin Source File

SOURCE=.\TetraModel\SRC\TetraMesh.h
# End Source File
# Begin Source File

SOURCE=.\TetraModel\SRC\TetraModel.h
# End Source File
# Begin Source File

SOURCE=.\TetraModel\SRC\TetraSubBoundary.h
# End Source File
# Begin Source File

SOURCE=.\TetraModel\SRC\TetraSubHorizon.h
# End Source File
# Begin Source File

SOURCE=.\TetraModel\SRC\TetraSuperHorizon.h
# End Source File
# Begin Source File

SOURCE=.\tetramodel\src\TetraSupportNode.h
# End Source File
# Begin Source File

SOURCE=.\TetraModel\SRC\TetraSurface.h
# End Source File
# Begin Source File

SOURCE=.\src\TipDlg.h
# End Source File
# Begin Source File

SOURCE=.\View\SRC\TopviewScene.h
# End Source File
# Begin Source File

SOURCE=.\View\SRC\TreeSplitView.h
# End Source File
# Begin Source File

SOURCE=.\geology\src\TSSurfaceProgress.h
# End Source File
# Begin Source File

SOURCE=.\value\src\ValueComponent.h
# End Source File
# Begin Source File

SOURCE=.\src\ValueCompositeObserver.h
# End Source File
# Begin Source File

SOURCE=.\value\src\ValueTensor.h
# End Source File
# Begin Source File

SOURCE=.\value\src\ValueType.h
# End Source File
# Begin Source File

SOURCE=.\value\src\ValueTypeFactory.h
# End Source File
# Begin Source File

SOURCE=.\value\src\ValueTypes.h
# End Source File
# Begin Source File

SOURCE=.\Value\SRC\ValueVector.h
# End Source File
# Begin Source File

SOURCE=.\value\src\VectorValueSet.h
# End Source File
# Begin Source File

SOURCE=.\SRC\versioninfo.h
# End Source File
# Begin Source File

SOURCE=.\geology\src\WellPath.h

!IF  "$(CFG)" == "Geomec - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing wellpath.h...
InputDir=.\geology\src
InputPath=.\geology\src\WellPath.h
InputName=WellPath

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Geomec - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Geology\SRC\WellPathFile.h
# End Source File
# Begin Source File

SOURCE=.\geology\src\WellPathFileUnitDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\wellpathgraphlimitsdlg.h
# End Source File
# Begin Source File

SOURCE=.\geology\src\WellPathProgress.h

!IF  "$(CFG)" == "Geomec - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing WellPathProgress.h...
InputDir=.\geology\src
InputPath=.\geology\src\WellPathProgress.h
InputName=WellPathProgress

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "Geomec - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing WellPathProgress.h...
InputDir=.\geology\src
InputPath=.\geology\src\WellPathProgress.h
InputName=WellPathProgress

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\wizzardpagebase.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\src\res\analysis.ico
# End Source File
# Begin Source File

SOURCE=.\hlp\AppExit.bmp
# End Source File
# Begin Source File

SOURCE=.\src\res\arrow.bmp
# End Source File
# Begin Source File

SOURCE=.\src\res\arrow.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\birdeye.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\bmp2d_to.bmp
# End Source File
# Begin Source File

SOURCE=.\SRC\res\body.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\bottom_h.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\boundary.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\bulk_den.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\bulkmodulus.ico
# End Source File
# Begin Source File

SOURCE=.\hlp\Bullet.bmp
# End Source File
# Begin Source File

SOURCE=.\src\res\calc_bar.bmp
# End Source File
# Begin Source File

SOURCE=.\SRC\res\check_re.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\checked.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\RES\closemesh.cur
# End Source File
# Begin Source File

SOURCE=.\src\res\cohesion.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\colorrange.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\compartm.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\componen.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\compress.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\crossection.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\crossection2d.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\crossections.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\crossections2d.ico
# End Source File
# Begin Source File

SOURCE=.\hlp\CurArw2.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\CurArw4.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\CurHelp.bmp
# End Source File
# Begin Source File

SOURCE=.\src\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\SRC\res\curve.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\curves.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\data_sto.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\depletin.ico
# End Source File
# Begin Source File

SOURCE=".\src\res\depletion stage branch.ico"
# End Source File
# Begin Source File

SOURCE=".\src\res\depletion stage initial.ico"
# End Source File
# Begin Source File

SOURCE=".\src\res\depletion stage initialdef.ico"
# End Source File
# Begin Source File

SOURCE=".\src\res\Depletion Stage.ico"
# End Source File
# Begin Source File

SOURCE=.\src\res\depletionstage_initial_branch.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\depletionstage_initial_none.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\depletionstage_none.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\Diana.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\discs.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\RES\draw.cur
# End Source File
# Begin Source File

SOURCE=.\hlp\EditCopy.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditCut.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditPast.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditUndo.bmp
# End Source File
# Begin Source File

SOURCE=.\src\res\element_.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\fault.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\faultld.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\faultlu.ico
# End Source File
# Begin Source File

SOURCE=.\hlp\FileNew.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FileOpen.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FilePrnt.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FileSave.bmp
# End Source File
# Begin Source File

SOURCE=.\SRC\res\formatio.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\formation.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\formations.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\FormPlanes.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\formula.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\friction.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\gen_pres.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\general.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\Geomec.rc2
# End Source File
# Begin Source File

SOURCE=.\SRC\res\GeomecDoc.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\global_i.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\graphs.ico
# End Source File
# Begin Source File

SOURCE=.\hlp\HlpSBar.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\HlpTBar.bmp
# End Source File
# Begin Source File

SOURCE=.\SRC\res\homgen.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\horizon.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\horizon_.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\horizont.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\hotspots.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\ico00002.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\ico00003.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\ico00004.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\ico00005.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\ico00006.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00007.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00008.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00009.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00010.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00011.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00012.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00013.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00014.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00015.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00016.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00017.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00018.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00019.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00020.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00021.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00022.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00023.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00024.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00025.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00026.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00027.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00028.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00029.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00030.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00031.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00032.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00033.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\ico00034.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00035.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00036.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00037.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00038.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00039.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\ico00040.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00041.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00042.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00043.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00044.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00045.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00046.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00047.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00048.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00049.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00050.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00051.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00052.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00053.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00054.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00055.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00056.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\ico00057.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\ico00058.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\ico00059.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\ico00060.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\ico00061.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\ico00062.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\ico00063.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\ico00064.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\ico00065.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\ico00066.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\ico00067.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\ico00068.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\ico00069.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\ico00070.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\ico00071.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\ico00072.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\ico00073.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\ico00074.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\ico00075.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00076.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00077.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico00078.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico2d_3d.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\ico2d_mo.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico2d_re.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\ico2d_xs.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ico3d_mo.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\icon3.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\icon_dis.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\linear.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\litebulb.bmp
# End Source File
# Begin Source File

SOURCE=.\src\res\mainfram.bmp
# End Source File
# Begin Source File

SOURCE=.\SRC\res\map.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\material.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\maximal_.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\mesh.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\mesh_mas.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\mesh_reg.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\minimal_.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\Model1.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\nodrop.cur
# End Source File
# Begin Source File

SOURCE=.\src\res\non_and_.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\non_line.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\pointset.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\pointsets.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\poissons.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\porosity.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\pres_con.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\pres_dis.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\pres_gwc.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\pres_und.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\pressure.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\pressure_change.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\pressure_maximum.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\pressure_minimum.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\pressures.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\property.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\radio_di.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\radiobutton_cheked.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\radiobutton_unchecked.ico
# End Source File
# Begin Source File

SOURCE=.\hlp\RecFirst.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\RecLast.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\RecNext.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\RecPrev.bmp
# End Source File
# Begin Source File

SOURCE=.\SRC\res\report.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\result.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\result_m.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\rpn_valueset_used.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\scalingb.ico
# End Source File
# Begin Source File

SOURCE=.\hlp\Scmax.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\ScMenu.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Scmin.bmp
# End Source File
# Begin Source File

SOURCE=.\src\res\shear_ve.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\side_sur.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\slip_con.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\slip_hor.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\snapsegm.cur
# End Source File
# Begin Source File

SOURCE=.\src\res\splash.bmp
# End Source File
# Begin Source File

SOURCE=.\src\res\splashnotext.bmp
# End Source File
# Begin Source File

SOURCE=.\src\res\splashnotextyel.bmp
# End Source File
# Begin Source File

SOURCE=.\src\res\splashtext.bmp
# End Source File
# Begin Source File

SOURCE=".\src\res\splashtextyel .bmp"
# End Source File
# Begin Source File

SOURCE=.\src\res\stress.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\stress_distr.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\stress_t.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\surface.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\surface_.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\surfaces.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\temperature.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\temperature_distributed.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\temperature_gradient.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\temperature_repeater.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\temperature_repeater_change.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\temperatures.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\thermal_expansion.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\top_hori.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\topview_.bmp
# End Source File
# Begin Source File

SOURCE=.\src\res\tri_chec.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\ursa21.bmp
# End Source File
# Begin Source File

SOURCE=.\src\res\value_se.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\value_ty.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\valuetyp.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\vertical.ico
# End Source File
# Begin Source File

SOURCE=.\SRC\res\view.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\view_bar.bmp
# End Source File
# Begin Source File

SOURCE=.\src\res\xsection_horizontal.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\youngs_m.ico
# End Source File
# End Group
# Begin Group "Help Files"

# PROP Default_Filter "cnt;rtf"
# Begin Group "Topics"

# PROP Default_Filter ""
# Begin Group "context"

# PROP Default_Filter ""
# Begin Group "attributes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\help\context\attributes\analysis_point_attributes.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\attributes_distributed_element_value_dialog.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\attributes_distributed_value_dialog.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\attributes_property_dialog.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\azimuth_inputs.GIF
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\bounda7.gif
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\boundary_attributes.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\cross_section_attributes.htm
# End Source File
# Begin Source File

SOURCE=.\help\context\attributes\fault_attributes.htm
# End Source File
# Begin Source File

SOURCE=.\help\context\attributes\fault_pressure.gif
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\formation_attributes.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\formation_plane_attributes.htm
# End Source File
# Begin Source File

SOURCE=.\help\context\attributes\formation_tetra_attributes.htm
# End Source File
# Begin Source File

SOURCE=.\help\context\attributes\formation_volume_attributes.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\global_color_scale.htm
# End Source File
# Begin Source File

SOURCE=.\help\context\attributes\gwc1.gif
# End Source File
# Begin Source File

SOURCE=.\help\context\attributes\gwc2.GIF
# End Source File
# Begin Source File

SOURCE=.\help\context\attributes\gwc3.GIF
# End Source File
# Begin Source File

SOURCE=.\help\context\attributes\gwc4.GIF
# End Source File
# Begin Source File

SOURCE=.\help\context\attributes\gwc5.GIF
# End Source File
# Begin Source File

SOURCE=.\help\context\attributes\gwc6.GIF
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\horizon_attributes.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\hotspots.htm
# End Source File
# Begin Source File

SOURCE=.\help\context\attributes\init_stress_attributes.png
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\initia10.gif
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\initia5.gif
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\initia7.gif
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\initia8.gif
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\initia9.gif
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\initial_pore_pressure.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\initial_stress.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\initial_temperature_field.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\local_color_scale.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\main_mesh_attributes.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\mesh_attributes.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\mesh_attributes_datastorage.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\mesh_region_attributes.htm
# End Source File
# Begin Source File

SOURCE=.\help\context\attributes\model_attributes.gif
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\model_attributes.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\model_depletion_stage.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\model_general.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\model_global_stresses.htm
# End Source File
# Begin Source File

SOURCE=.\help\context\attributes\point_elementset_attributes.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\pointset_attributes.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\pressu1.gif
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\pressu2.gif
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\pressu5.gif
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\pressu6.gif
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\pressure_attributes.htm
# End Source File
# Begin Source File

SOURCE=.\help\context\attributes\quantity_attributes.htm
# End Source File
# Begin Source File

SOURCE=.\help\context\attributes\restart.htm
# End Source File
# Begin Source File

SOURCE=.\help\context\attributes\restart_attributes.png
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\rpn_calculator.htm
# End Source File
# Begin Source File

SOURCE=.\help\context\attributes\scaling_box_attributes.htm
# End Source File
# Begin Source File

SOURCE=.\help\context\attributes\sub_horizon_attributes.htm
# End Source File
# Begin Source File

SOURCE=.\help\context\attributes\surface_attributes.htm
# End Source File
# Begin Source File

SOURCE=.\help\context\attributes\temperature_attributes.htm
# End Source File
# Begin Source File

SOURCE=.\help\context\attributes\weight_of_seawater.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\attributes\wellpath_attributes.htm
# End Source File
# End Group
# Begin Group "dialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Help\context\dialogs\analysis_options.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\dialogs\bounda7.gif
# End Source File
# Begin Source File

SOURCE=.\Help\context\dialogs\calculation_criteria.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\dialogs\color_scale.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\dialogs\create_pointset.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\dialogs\cut_surfaces.htm
# End Source File
# Begin Source File

SOURCE=".\Help\context\dialogs\far-field.GIF"
# End Source File
# Begin Source File

SOURCE=.\Help\context\dialogs\far_field.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\dialogs\fist_export.htm
# End Source File
# Begin Source File

SOURCE=.\help\context\dialogs\global_strain_tensor.htm
# End Source File
# Begin Source File

SOURCE=.\help\context\dialogs\global_stress_tensor.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\dialogs\graph_limits.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\dialogs\interface.htm
# End Source File
# Begin Source File

SOURCE=.\help\context\dialogs\linear_analysis_settings.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\dialogs\new_model.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\dialogs\nonlinear_calculation_parameter.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\dialogs\open_model.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\dialogs\points_fist_export.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\dialogs\save_model.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\dialogs\save_model_as.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\dialogs\select_result.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\dialogs\select_surfaces.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\dialogs\view_settings.htm
# End Source File
# End Group
# Begin Source File

SOURCE=.\Help\context\along_vertical_depth.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\calculate_optimum_mudweight.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\change_position_cross_section.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\change_position_wellpath.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\convert_to_tetramodel.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\create_global_gradient.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\create_horizon.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\create_hotspot.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\delete_cross_section.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\delete_horizon.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\delete_wellpath.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\export_results.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\formation_depth_lines.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\insert_cross_section.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\insert_cross_section_by_value.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\insert_cross_section_graphical.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\insert_deviated_wellpath.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\insert_vertical_wellpath.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\insert_vertical_wellpath_graphical.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\insert_vertical_wellpath_value.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\legend_attributes.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\markers.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\material_library_formation.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\modify_boundary.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\move_horizon.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\remove_material_formation.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\reset_limits.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\select_points_dstabor.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\track_coordinates.htm
# End Source File
# Begin Source File

SOURCE=.\Help\context\true_vertical_depth.htm
# End Source File
# End Group
# Begin Group "datastorage"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Help\datastorage\2d_pointset.htm
# End Source File
# Begin Source File

SOURCE=.\Help\datastorage\3d_pointset.htm
# End Source File
# Begin Source File

SOURCE=.\help\datastorage\data_storage_collapsed.bmp
# End Source File
# Begin Source File

SOURCE=.\Help\datastorage\datastorage_general.htm
# End Source File
# Begin Source File

SOURCE=.\Help\datastorage\datastorage_tree.htm
# End Source File
# Begin Source File

SOURCE=.\help\datastorage\dragging_property.bmp
# End Source File
# Begin Source File

SOURCE=.\Help\datastorage\element_pointset.htm
# End Source File
# Begin Source File

SOURCE=.\Help\datastorage\list_material_models.htm
# End Source File
# Begin Source File

SOURCE=.\Help\datastorage\list_of_pointsets.htm
# End Source File
# Begin Source File

SOURCE=.\Help\datastorage\list_surfaces_tree.htm
# End Source File
# Begin Source File

SOURCE=.\Help\datastorage\mat_lib.jpg
# End Source File
# Begin Source File

SOURCE=.\Help\datastorage\materials_tree.htm
# End Source File
# Begin Source File

SOURCE=.\Help\datastorage\mesh_tree.htm
# End Source File
# Begin Source File

SOURCE=.\Help\datastorage\new_mat.jpg
# End Source File
# Begin Source File

SOURCE=.\help\datastorage\point_set_2d.bmp
# End Source File
# Begin Source File

SOURCE=.\Help\datastorage\point_sets_tree.htm
# End Source File
# Begin Source File

SOURCE=.\Help\datastorage\pointset.htm
# End Source File
# Begin Source File

SOURCE=.\Help\datastorage\rock_materials_in_datastorage.htm
# End Source File
# Begin Source File

SOURCE=.\Help\datastorage\surfaces_tree.htm
# End Source File
# End Group
# Begin Group "general"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Help\general\context_sensitive_menu.htm
# End Source File
# Begin Source File

SOURCE=.\Help\general\distributed_quantities.htm
# End Source File
# Begin Source File

SOURCE=.\Help\general\distributed_value.htm
# End Source File
# Begin Source File

SOURCE=.\Help\general\extrap3.gif
# End Source File
# Begin Source File

SOURCE=.\Help\general\extrapolated_value.htm
# End Source File
# Begin Source File

SOURCE=.\Help\general\general.htm
# End Source File
# Begin Source File

SOURCE=.\Help\general\generic_tags.htm
# End Source File
# Begin Source File

SOURCE=.\Help\general\geomec_limits.htm
# End Source File
# Begin Source File

SOURCE=.\Help\general\material_tags.htm
# End Source File
# Begin Source File

SOURCE=.\Help\general\pressure_temperature_pointset_values.htm
# End Source File
# Begin Source File

SOURCE=.\Help\general\result_tags.htm
# End Source File
# Begin Source File

SOURCE=.\Help\general\tags.htm
# End Source File
# Begin Source File

SOURCE=.\Help\general\units.htm
# End Source File
# Begin Source File

SOURCE=.\Help\general\values.htm
# End Source File
# End Group
# Begin Group "main window"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\Help\main window\drawing_area.htm"
# End Source File
# Begin Source File

SOURCE=".\Help\main window\dynamic.htm"
# End Source File
# Begin Source File

SOURCE=".\Help\main window\legend_area.htm"
# End Source File
# Begin Source File

SOURCE=".\Help\main window\legend_geology.htm"
# End Source File
# Begin Source File

SOURCE=".\Help\main window\legend_value.htm"
# End Source File
# Begin Source File

SOURCE=".\Help\main window\main_window.htm"
# End Source File
# Begin Source File

SOURCE=".\Help\main window\value_mesh_results.htm"
# End Source File
# Begin Source File

SOURCE=".\Help\main window\value_results.htm"
# End Source File
# End Group
# Begin Group "menus"

# PROP Default_Filter ""
# Begin Group "analysis"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Help\menus\analysis\analysis_calculation_criteria.htm
# End Source File
# Begin Source File

SOURCE=.\Help\menus\analysis\analysis_log.htm
# End Source File
# Begin Source File

SOURCE=.\Help\menus\analysis\analysis_menu.htm
# End Source File
# Begin Source File

SOURCE=.\Help\menus\analysis\linear_depletion.htm
# End Source File
# Begin Source File

SOURCE=.\Help\menus\analysis\nonlinear_depletion.htm
# End Source File
# End Group
# Begin Group "edit"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Help\menus\Edit\copy.htm
# End Source File
# Begin Source File

SOURCE=.\Help\menus\Edit\delete.htm
# End Source File
# Begin Source File

SOURCE=.\Help\menus\Edit\edit_menu.htm
# End Source File
# End Group
# Begin Group "file"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Help\menus\File\exit.htm
# End Source File
# Begin Source File

SOURCE=.\Help\menus\File\file_menu.htm
# End Source File
# Begin Source File

SOURCE=.\Help\menus\File\recent_file.htm
# End Source File
# End Group
# Begin Group "help"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Help\menus\Help\about_geomec.htm
# End Source File
# Begin Source File

SOURCE=.\Help\menus\Help\help_menu.htm
# End Source File
# Begin Source File

SOURCE=.\Help\menus\Help\help_topics.htm
# End Source File
# End Group
# Begin Group "insert"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Help\menus\Insert\insert_menu.htm
# End Source File
# End Group
# Begin Group "view"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Help\menus\View\axis.htm
# End Source File
# Begin Source File

SOURCE=.\Help\menus\View\boundary.htm
# End Source File
# Begin Source File

SOURCE=.\Help\menus\View\fill.htm
# End Source File
# Begin Source File

SOURCE=.\Help\menus\View\geology.htm
# End Source File
# Begin Source File

SOURCE=.\Help\menus\View\global_colorscale.htm
# End Source File
# Begin Source File

SOURCE=.\Help\menus\View\hotspot.htm
# End Source File
# Begin Source File

SOURCE=.\Help\menus\View\local_colorscale.htm
# End Source File
# Begin Source File

SOURCE=.\Help\menus\View\perspective.htm
# End Source File
# Begin Source File

SOURCE=.\Help\menus\View\rotate.htm
# End Source File
# Begin Source File

SOURCE=.\Help\menus\View\status_bar.htm
# End Source File
# Begin Source File

SOURCE=.\Help\menus\View\value.htm
# End Source File
# Begin Source File

SOURCE=.\Help\menus\View\view_menu.htm
# End Source File
# End Group
# Begin Source File

SOURCE=.\Help\menus\geomec_menus.htm
# End Source File
# End Group
# Begin Group "toolbars"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Help\toolbars\analysis_create_mesh.htm
# End Source File
# Begin Source File

SOURCE=.\Help\toolbars\analysis_invalidate_mesh.htm
# End Source File
# Begin Source File

SOURCE=.\Help\toolbars\toolbar_analysis.htm
# End Source File
# Begin Source File

SOURCE=.\Help\toolbars\toolbar_general.htm
# End Source File
# Begin Source File

SOURCE=.\Help\toolbars\toolbar_topview.htm
# End Source File
# Begin Source File

SOURCE=.\Help\toolbars\toolbar_view.htm
# End Source File
# Begin Source File

SOURCE=.\Help\toolbars\toolbars.htm
# End Source File
# Begin Source File

SOURCE=.\Help\toolbars\topview_polygonal_mesh_region.htm
# End Source File
# Begin Source File

SOURCE=.\Help\toolbars\view_birdeye_view.htm
# End Source File
# Begin Source File

SOURCE=.\Help\toolbars\view_depth_direction.htm
# End Source File
# Begin Source File

SOURCE=.\Help\toolbars\view_easting_direction.htm
# End Source File
# Begin Source File

SOURCE=.\Help\toolbars\view_lighting.htm
# End Source File
# Begin Source File

SOURCE=.\Help\toolbars\view_lock_depth.htm
# End Source File
# Begin Source File

SOURCE=.\Help\toolbars\view_lock_easting.htm
# End Source File
# Begin Source File

SOURCE=.\Help\toolbars\view_lock_northing.htm
# End Source File
# Begin Source File

SOURCE=.\Help\toolbars\view_northing_direction.htm
# End Source File
# Begin Source File

SOURCE=.\Help\toolbars\view_unlock.htm
# End Source File
# Begin Source File

SOURCE=.\Help\toolbars\view_zoom_all.htm
# End Source File
# Begin Source File

SOURCE=.\Help\toolbars\view_zoom_in.htm
# End Source File
# Begin Source File

SOURCE=.\Help\toolbars\view_zoom_in_cursor.htm
# End Source File
# Begin Source File

SOURCE=.\Help\toolbars\view_zoom_out.htm
# End Source File
# End Group
# Begin Group "tree"

# PROP Default_Filter ""
# Begin Group "mesh_results"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Help\tree\Mesh_results\depth.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Mesh_results\max_angle.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Mesh_results\Max_edge.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Mesh_results\mean_edge.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Mesh_results\mesh_results_tree.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Mesh_results\min_angle.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Mesh_results\min_max_edge_ratio.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Mesh_results\shape_factor.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Mesh_results\surface.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Mesh_results\volume.htm
# End Source File
# End Group
# Begin Group "model"

# PROP Default_Filter ""
# Begin Group "hexa"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Help\tree\Model\hexa\hexahedron_tree.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Model\hexa\horizon_tree.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Model\hexa\list_horizons_tree.htm
# End Source File
# End Group
# Begin Group "tetra"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\help\tree\model\tetra\gocad_sub_model_tree.htm
# End Source File
# Begin Source File

SOURCE=.\help\tree\model\tetra\tetrahedron_tree.htm
# End Source File
# End Group
# Begin Source File

SOURCE=.\Help\tree\Model\2D_results_tree.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Model\analysis_points_tree.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Model\boundary_tree.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Model\cross_sections_tree.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Model\formation_plane_tree.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Model\formations_tree.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Model\horizons_formation_tree.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Model\initial_pore_pressure_tree.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Model\initial_temperature_tree.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Model\list_cross_sections_tree.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Model\list_depletion_formation_tree.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Model\list_formations_tree.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Model\list_of_meshregions_tree.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Model\list_wellpaths_tree.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Model\material_formation_tree.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Model\mesh_tree.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Model\model_tree.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Model\model_types.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Model\planes_formations_tree.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Model\pressure_formation_tree.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Model\scaling_boxes_tree.htm
# End Source File
# Begin Source File

SOURCE=.\help\tree\model\temperature_formation_tree.htm
# End Source File
# Begin Source File

SOURCE=.\help\tree\model\weight_of_seawater.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Model\wellpaths_tree.htm
# End Source File
# End Group
# Begin Group "results"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Help\tree\Results\depletion_stages.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Results\displacements.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Results\fault.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Results\linear_nonlinear.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Results\optimum.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Results\pore_pressure.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Results\results_tree.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Results\strains.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Results\stresses.htm
# End Source File
# End Group
# Begin Group "views"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Help\tree\Views\2D_line_graph.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Views\Birdeye_view.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Views\cross_sections.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Views\default.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Views\top_view.htm
# End Source File
# Begin Source File

SOURCE=.\Help\tree\Views\views_tree.htm
# End Source File
# End Group
# Begin Group "defined results"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\Help\tree\Defined results\user_defined_results_tree.htm"
# End Source File
# End Group
# Begin Group "derived results"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\Help\tree\Derived result\derived_result_tree.htm"
# End Source File
# End Group
# Begin Source File

SOURCE=.\Help\tree\geomec_tree.htm
# End Source File
# End Group
# Begin Group "background"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\help\background\anisotr1.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\anisotr2.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\anisotropy.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\averaging_methods.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\calibr1.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\calibr10.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\calibr11.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\calibr12.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\calibr13.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\calibr14.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\calibr15.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\calibr16.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\calibr17.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\calibr18.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\calibr19.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\calibr2.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\calibr20.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\calibr3.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\calibr4.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\calibr5.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\calibr6.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\calibr7.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\calibr8.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\calibr9.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\calibration_algorithm.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\cam_clay.htm
# End Source File
# Begin Source File

SOURCE=.\help\background\cam_clay_01.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\cam_clay_02.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\cam_clay_03.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\cam_clay_04.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\cam_clay_05.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\cam_clay_06.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\cam_clay_07.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\cam_clay_08.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\cam_clay_09.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\cam_clay_10.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\cam_clay_11.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\cam_clay_12.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\cam_clay_13.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\cam_clay_14.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\cam_clay_15.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\cam_clay_16.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\cam_clay_17.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\cam_clay_18.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\cam_clay_19.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\cam_clay_20.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\cam_clay_21.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\cam_clay_22.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\cam_clay_23.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\cam_clay_24.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\constant_stiffness.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\construction_and_constrain.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\convergence_norm_type.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\default_values_iteration.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\displa18.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\displacement_norm.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\element_to_element_mapping.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\elements.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\energy19.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\energy_norm.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\fault_63.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\fault_64.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\fault_65.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\fault_66.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\fault_67.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\fault_68.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\fault_69.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\fault_70.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\fault_71.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\fault_frictional.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\force_20.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\force_norm.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\geomec_theory_description.htm
# End Source File
# Begin Source File

SOURCE=.\help\background\homog1.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\homog2.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\homog3.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\homogenization.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\initia10.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\initia11.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\initia12.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\initia13.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\initia6.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\initia7.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\initia8.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\initia9.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\initial_stresses.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\invers16.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\invers20.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\invers21.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\invers22.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\invers23.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\invers24.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\invers25.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\invers26.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\invers27.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\invers28.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\invers29.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\invers30.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\invers31.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\invers32.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\invers33.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\invers34.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\invers35.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\invers36.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\invers37.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\invers38.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\inverse_averaging.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\inverse_volume_based_mapping.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\iterat2.jpg
# End Source File
# Begin Source File

SOURCE=.\Help\background\iterat21.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\iterat22.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\iterat23.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\iterat24.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\iterat25.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\iterat26.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\iterat27.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\iterat28.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\iterat29.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\iterat30.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\iteration_schemes.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\iteration_schemes_theory.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\Krad.GIF
# End Source File
# Begin Source File

SOURCE=.\Help\background\Ktan.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\linear40.jpg
# End Source File
# Begin Source File

SOURCE=.\Help\background\linear5.jpg
# End Source File
# Begin Source File

SOURCE=.\Help\background\linear_material.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\linear_stiffness.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\mapping_routines.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\material_models.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\modifi31.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\modifi32.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\modifi33.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\modifi34.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\modifi35.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\modifi40.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\modifi41.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\modifi42.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\modifi43.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\modifi43a.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\modifi43b.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\modifi44.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\modifi45.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\modifi45a.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\modifi45b.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\modifi46.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\modifi47.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\modifi48.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\modifi49.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\modifi50.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\modifi50a.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\modifi50b.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\modifi50c.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\modifi51.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\modifi51a.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\modifi51b.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\modifi51c.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\modifi52.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\modifi53.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\modifi54.GIF
# End Source File
# Begin Source File

SOURCE=.\Help\background\modified_mohr_coulomb.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\mohr_c36.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\mohr_c37.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\mohr_c38.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\mohr_c39.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\mohr_c40.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\mohr_c41.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\mohr_c42.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\mohr_c43.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\mohr_c44.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\mohr_c45.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\mohr_c46.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\mohr_ch1.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\mohr_ch2.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\mohr_ch3.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\mohr_ch4.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\mohr_ch5.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\mohr_ch6.GIF
# End Source File
# Begin Source File

SOURCE=.\Help\background\mohr_coulomb.htm
# End Source File
# Begin Source File

SOURCE=.\help\background\mohr_coulomb_hardening.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\newton3.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\newton4.jpg
# End Source File
# Begin Source File

SOURCE=.\Help\background\newton_modified.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\newton_regular.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\nodal_17.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\nodal_interpolation.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\non_smoothed.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\non_we14.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\non_weighted_averaging.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\nonlin41.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\nonlin42.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\nonlin43.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\nonlin44.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\nonlin45.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\nonlin46.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\nonlin47.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\nonlin48.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\nonlinear_material.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\point_interpolation.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\result1.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result10.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result101.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result102.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result103.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result104.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result105.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result106.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result107.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result108.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result109.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result11.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result110.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result111.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result112.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result113.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result114.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result12.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result13.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result14.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result15.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result16.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result17.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result18.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result19.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result2.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result20.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result21.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result22.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result23.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result24.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result25.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result26.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result27.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\result28.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\result29.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result3.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\result30.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\result31.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result4.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result5.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result6.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result7.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result8.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\result9.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\results_theory.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\rules.htm
# End Source File
# Begin Source File

SOURCE=.\help\background\salt1.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\salt10.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\salt11.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\salt12.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\salt13.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\salt14.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\salt15.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\salt16.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\salt17.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\salt18.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\salt19.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\salt2.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\salt20.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\salt21.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\salt22.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\salt3.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\salt4.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\salt4b.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\salt4c.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\salt4d.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\salt4e.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\salt5.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\salt6.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\salt7.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\salt8.gif
# End Source File
# Begin Source File

SOURCE=.\help\background\salt9.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\salt_c1.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\salt_c10.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\salt_c11.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\salt_c2.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\salt_c3.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\salt_c4.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\salt_c5.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\salt_c56.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\salt_c57.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\salt_c58.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\salt_c59.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\salt_c6.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\salt_c60.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\salt_c61.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\salt_c62.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\salt_c7.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\salt_c8.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\salt_c9.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\salt_creep.htm
# End Source File
# Begin Source File

SOURCE=.\help\background\salt_n1.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\salt_n10.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\salt_n11.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\salt_n12.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\salt_n13.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\salt_n2.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\salt_n3.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\salt_n4.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\salt_n5.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\salt_n6.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\salt_n7.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\salt_n8.GIF
# End Source File
# Begin Source File

SOURCE=.\help\background\salt_n9.GIF
# End Source File
# Begin Source File

SOURCE=.\Help\background\smoothed_point_interpolation.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\strain28.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\strain29.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\strain30.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\strain31.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\strain32.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\strain_methods.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\undrai1.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\undrai10.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\undrai2.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\undrai25.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\undrai26.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\undrai27.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\undrai28.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\undrai29.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\undrai3.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\undrai30.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\undrai31.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\undrai4.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\undrai5.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\undrai6.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\undrai7.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\undrai8.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\undrai9.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\undrained.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\volume15.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\volume39.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\volume40.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\volume41.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\volume42.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\volume43.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\volume44.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\volume_based_mapping.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\volume_weighted_averaging.htm
# End Source File
# Begin Source File

SOURCE=.\Help\background\weight18.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\weight19.gif
# End Source File
# Begin Source File

SOURCE=.\Help\background\weighted_mapping.htm
# End Source File
# Begin Source File

SOURCE=".\Help\background\workflow-zoom-in.htm"
# End Source File
# Begin Source File

SOURCE=".\Help\background\zoom-in-technique.htm"
# End Source File
# Begin Source File

SOURCE=".\Help\background\zoom-in.GIF"
# End Source File
# End Group
# Begin Group "calculate"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Help\calculate\calculating_in_geomec.htm
# End Source File
# Begin Source File

SOURCE=.\Help\calculate\troubleshooting.htm
# End Source File
# End Group
# Begin Group "file formats"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\Help\file formats\ascii.htm"
# End Source File
# Begin Source File

SOURCE=".\Help\file formats\eclipse_grid.htm"
# End Source File
# Begin Source File

SOURCE=".\Help\file formats\elemen1.gif"
# End Source File
# Begin Source File

SOURCE=".\Help\file formats\element_section.htm"
# End Source File
# Begin Source File

SOURCE=".\Help\file formats\element_set_file.htm"
# End Source File
# Begin Source File

SOURCE=".\Help\file formats\excel.htm"
# End Source File
# Begin Source File

SOURCE=".\Help\file formats\experiment_file_format.htm"
# End Source File
# Begin Source File

SOURCE=".\Help\file formats\file_formats.htm"
# End Source File
# Begin Source File

SOURCE=".\help\file formats\gocad.gif"
# End Source File
# Begin Source File

SOURCE=".\Help\file formats\gocad_file.htm"
# End Source File
# Begin Source File

SOURCE=".\Help\file formats\hotspot.gif"
# End Source File
# Begin Source File

SOURCE=".\Help\file formats\hotspot_file_format.htm"
# End Source File
# Begin Source File

SOURCE=".\Help\file formats\point_section.htm"
# End Source File
# Begin Source File

SOURCE=".\Help\file formats\point_set_file.htm"
# End Source File
# Begin Source File

SOURCE=".\Help\file formats\tagged_format.htm"
# End Source File
# Begin Source File

SOURCE=".\Help\file formats\untagged_format.htm"
# End Source File
# Begin Source File

SOURCE=".\Help\file formats\well_path_format.htm"
# End Source File
# End Group
# Begin Group "material library"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\Help\material library\mat_lib.JPG"
# End Source File
# Begin Source File

SOURCE=".\Help\material library\working_mat_lib.htm"
# End Source File
# End Group
# Begin Group "boundary"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Help\boundary\bounda1.gif
# End Source File
# Begin Source File

SOURCE=.\Help\boundary\bounda10.gif
# End Source File
# Begin Source File

SOURCE=.\Help\boundary\bounda11.gif
# End Source File
# Begin Source File

SOURCE=.\Help\boundary\bounda12.gif
# End Source File
# Begin Source File

SOURCE=.\Help\boundary\bounda13.gif
# End Source File
# Begin Source File

SOURCE=.\Help\boundary\bounda14.gif
# End Source File
# Begin Source File

SOURCE=.\Help\boundary\bounda15.gif
# End Source File
# Begin Source File

SOURCE=.\Help\boundary\bounda2.gif
# End Source File
# Begin Source File

SOURCE=.\Help\boundary\bounda3.gif
# End Source File
# Begin Source File

SOURCE=.\Help\boundary\bounda4.gif
# End Source File
# Begin Source File

SOURCE=.\Help\boundary\bounda5.gif
# End Source File
# Begin Source File

SOURCE=.\Help\boundary\bounda6.gif
# End Source File
# Begin Source File

SOURCE=.\Help\boundary\bounda7.gif
# End Source File
# Begin Source File

SOURCE=.\Help\boundary\bounda8.gif
# End Source File
# Begin Source File

SOURCE=.\Help\boundary\bounda9.gif
# End Source File
# Begin Source File

SOURCE=.\Help\boundary\boundary_conditions.htm
# End Source File
# Begin Source File

SOURCE=.\Help\boundary\boundary_displacement_from_tensor.htm
# End Source File
# Begin Source File

SOURCE=.\Help\boundary\boundary_face_attributes_dialog.htm
# End Source File
# Begin Source File

SOURCE=.\Help\boundary\boundary_general.htm
# End Source File
# Begin Source File

SOURCE=.\Help\boundary\boundary_global_strain_tensor.htm
# End Source File
# Begin Source File

SOURCE=.\Help\boundary\boundary_global_stress_tensor.htm
# End Source File
# Begin Source File

SOURCE=.\Help\boundary\boundary_node_attributes_dialog.htm
# End Source File
# Begin Source File

SOURCE=.\Help\boundary\boundary_prescribed_displacements.htm
# End Source File
# Begin Source File

SOURCE=.\Help\boundary\boundary_tensor_specification.htm
# End Source File
# End Group
# Begin Group "models"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Help\models\eclipse_model.htm
# End Source File
# End Group
# Begin Group "2D_help"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\help\2D_help\2D_index.htm
# End Source File
# Begin Source File

SOURCE=.\help\2D_help\create_2D.htm
# End Source File
# Begin Source File

SOURCE=.\help\2D_help\create_2D_model.gif
# End Source File
# End Group
# Begin Source File

SOURCE=.\Help\contents.htm
# End Source File
# End Group
# Begin Source File

SOURCE=.\Help\geomec.hhp

!IF  "$(CFG)" == "Geomec - Win32 Release"

# PROP Ignore_Default_Tool 1
USERDEP__GEOME="Help\geomec.hm"	
# Begin Custom Build - Making HTML help file...
OutDir=.\Release
InputPath=.\Help\geomec.hhp
InputName=geomec

"$(OutDir)\$(InputName).chm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"c:\program files\HTML Help Workshop\hhc.exe" Help\$(InputName).hhp 
	echo. 
	copy Help\$(InputName).chm $(OutDir)\$(InputName).chm 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Geomec - Win32 Debug"

# PROP Ignore_Default_Tool 1
USERDEP__GEOME="Help\geomec.hm"	
# Begin Custom Build - Making HTML help file...
OutDir=.\Debug
InputPath=.\Help\geomec.hhp
InputName=geomec

"$(OutDir)\$(InputName).chm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"c:\program files\HTML Help Workshop\hhc.exe" Help\$(InputName).hhp 
	echo. 
	copy Help\$(InputName).chm $(OutDir)\$(InputName).chm 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Help\Index.hhk
# End Source File
# Begin Source File

SOURCE=.\Help\TOC.hhc
# End Source File
# End Group
# Begin Group "Test Files"

# PROP Default_Filter ".dat .mx"
# End Group
# Begin Group "DstaborLink"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\dstaborlink\src\DsbPickPointsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\dstaborlink\src\DsbPickPointsDlg.h
# End Source File
# Begin Source File

SOURCE=.\dstaborlink\src\dsbtsctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\dstaborlink\src\dsbtsctrl.h
# End Source File
# Begin Source File

SOURCE=.\DStaborLink\src\dsbx.cpp
# End Source File
# Begin Source File

SOURCE=.\DStaborLink\src\dsbx.h
# End Source File
# Begin Source File

SOURCE=.\dstaborlink\src\DsbXDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\dstaborlink\src\DsbXDlg.h
# End Source File
# Begin Source File

SOURCE=.\DStaborLink\src\insitu.cpp
# End Source File
# Begin Source File

SOURCE=.\DStaborLink\src\insitu.h
# End Source File
# Begin Source File

SOURCE=.\DStaborLink\src\mesh.cpp
# End Source File
# Begin Source File

SOURCE=.\DStaborLink\src\mesh.h
# End Source File
# Begin Source File

SOURCE=.\DStaborLink\src\mudweight.cpp
# End Source File
# Begin Source File

SOURCE=.\DStaborLink\src\mudweight.h
# End Source File
# Begin Source File

SOURCE=.\DStaborLink\src\OptimumMudDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DStaborLink\src\OptimumMudDlg.h
# End Source File
# Begin Source File

SOURCE=.\DStaborLink\src\output.cpp
# End Source File
# Begin Source File

SOURCE=.\DStaborLink\src\output.h
# End Source File
# Begin Source File

SOURCE=.\DStaborLink\src\proc.cpp
# End Source File
# Begin Source File

SOURCE=.\DStaborLink\src\proc.h
# End Source File
# Begin Source File

SOURCE=.\DStaborLink\src\rock.cpp
# End Source File
# Begin Source File

SOURCE=.\DStaborLink\src\rock.h
# End Source File
# Begin Source File

SOURCE=.\dstaborlink\src\StartDstaborDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\dstaborlink\src\StartDstaborDlg.h
# End Source File
# Begin Source File

SOURCE=.\dstaborlink\src\StartDstaborGuiDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\dstaborlink\src\StartDstaborGuiDlg.h
# End Source File
# Begin Source File

SOURCE=.\DStaborLink\src\timepoints.cpp
# End Source File
# Begin Source File

SOURCE=.\DStaborLink\src\timepoints.h
# End Source File
# Begin Source File

SOURCE=.\DStaborLink\src\well.cpp
# End Source File
# Begin Source File

SOURCE=.\DStaborLink\src\well.h
# End Source File
# End Group
# Begin Group "keyfile"

# PROP Default_Filter "k70"
# Begin Source File

SOURCE=.\keyfile\diafgv.k70
# End Source File
# End Group
# Begin Group "in40"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\in40\in40.exe
# End Source File
# End Group
# Begin Source File

SOURCE=..\lib\DCasingLib\DCasing\DCasintSlotHandler.moc

!IF  "$(CFG)" == "Geomec - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing DCasintSlotHandler.cpp...
InputDir=\users\wedx\development\lib\DCasingLib\DCasing
InputPath=..\lib\DCasingLib\DCasing\DCasintSlotHandler.moc
InputName=DCasintSlotHandler

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).cpp -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "Geomec - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing DCasintSlotHandler.cpp...
InputDir=\users\wedx\development\lib\DCasingLib\DCasing
InputPath=..\lib\DCasingLib\DCasing\DCasintSlotHandler.moc
InputName=DCasintSlotHandler

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).cpp -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\view\src\HistoryScene.moc

!IF  "$(CFG)" == "Geomec - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing HistoryScene.cpp...
InputDir=.\view\src
InputPath=.\view\src\HistoryScene.moc
InputName=HistoryScene

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).cpp -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "Geomec - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing HistoryScene.cpp...
InputDir=.\view\src
InputPath=.\view\src\HistoryScene.moc
InputName=HistoryScene

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).cpp -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
# Section Geomec : {73EDCE94-CCD6-11D3-B3AC-00A0C941C60B}
# 	2:5:Class:CMatLibX
# 	2:10:HeaderFile:matlibx.h
# 	2:8:ImplFile:matlibx.cpp
# End Section
# Section Geomec : {AE07426B-3E6B-11D3-A889-0060977A7366}
# 	2:5:Class:CInSitu
# 	2:10:HeaderFile:insitu.h
# 	2:8:ImplFile:insitu.cpp
# End Section
# Section Geomec : {AE074274-3E6B-11D3-A889-0060977A7366}
# 	2:5:Class:CTimePoints
# 	2:10:HeaderFile:timepoints.h
# 	2:8:ImplFile:timepoints.cpp
# End Section
# Section Geomec : {AE074268-3E6B-11D3-A889-0060977A7366}
# 	2:5:Class:CRock
# 	2:10:HeaderFile:rock.h
# 	2:8:ImplFile:rock.cpp
# End Section
# Section Geomec : {4E2C43CE-38F8-11D3-B38E-00A0C941C60B}
# 	2:21:DefaultSinkHeaderFile:dsbx.h
# 	2:16:DefaultSinkClass:CDsbX
# End Section
# Section Geomec : {AE07426E-3E6B-11D3-A889-0060977A7366}
# 	2:5:Class:CProc
# 	2:10:HeaderFile:proc.h
# 	2:8:ImplFile:proc.cpp
# End Section
# Section Geomec : {9CBFA32C-39B5-11D3-B38E-00A0C941C60B}
# 	2:5:Class:CDsbX
# 	2:10:HeaderFile:dsbx.h
# 	2:8:ImplFile:dsbx.cpp
# End Section
# Section Geomec : {72ADFD54-2C39-11D0-9903-00A0C91BC942}
# 	1:17:CG_IDS_DIDYOUKNOW:123
# 	1:22:CG_IDS_TIPOFTHEDAYMENU:122
# 	1:18:CG_IDS_TIPOFTHEDAY:121
# 	1:22:CG_IDS_TIPOFTHEDAYHELP:126
# 	1:19:CG_IDP_FILE_CORRUPT:125
# 	1:7:IDD_TIP:120
# 	1:13:IDB_LIGHTBULB:119
# 	1:18:CG_IDS_FILE_ABSENT:124
# 	2:17:CG_IDS_DIDYOUKNOW:CG_IDS_DIDYOUKNOW
# 	2:7:CTipDlg:CTipDlg
# 	2:22:CG_IDS_TIPOFTHEDAYMENU:CG_IDS_TIPOFTHEDAYMENU
# 	2:18:CG_IDS_TIPOFTHEDAY:CG_IDS_TIPOFTHEDAY
# 	2:12:CTIP_Written:OK
# 	2:22:CG_IDS_TIPOFTHEDAYHELP:CG_IDS_TIPOFTHEDAYHELP
# 	2:2:BH:
# 	2:19:CG_IDP_FILE_CORRUPT:CG_IDP_FILE_CORRUPT
# 	2:7:IDD_TIP:IDD_TIP
# 	2:8:TipDlg.h:TipDlg.h
# 	2:13:IDB_LIGHTBULB:IDB_LIGHTBULB
# 	2:18:CG_IDS_FILE_ABSENT:CG_IDS_FILE_ABSENT
# 	2:10:TipDlg.cpp:TipDlg.cpp
# End Section
# Section Geomec : {7218FAF0-3F30-11D3-A88A-0060977A7366}
# 	2:5:Class:CMesh
# 	2:10:HeaderFile:mesh.h
# 	2:8:ImplFile:mesh.cpp
# End Section
# Section Geomec : {73EDCE96-CCD6-11D3-B3AC-00A0C941C60B}
# 	2:21:DefaultSinkHeaderFile:matlibx.h
# 	2:16:DefaultSinkClass:CMatLibX
# End Section
# Section Geomec : {7218FAF4-3F30-11D3-A88A-0060977A7366}
# 	2:5:Class:COutput
# 	2:10:HeaderFile:output.h
# 	2:8:ImplFile:output.cpp
# End Section
# Section Geomec : {AE074271-3E6B-11D3-A889-0060977A7366}
# 	2:5:Class:CMudWeight
# 	2:10:HeaderFile:mudweight.h
# 	2:8:ImplFile:mudweight.cpp
# End Section
# Section Geomec : {B68435BF-EF44-11D3-B3AE-00A0C941C60B}
# 	2:5:Class:CMatLibXParam
# 	2:10:HeaderFile:matlibxparam.h
# 	2:8:ImplFile:matlibxparam.cpp
# End Section
# Section Geomec : {AE074265-3E6B-11D3-A889-0060977A7366}
# 	2:5:Class:CWell
# 	2:10:HeaderFile:well.h
# 	2:8:ImplFile:well.cpp
# End Section
