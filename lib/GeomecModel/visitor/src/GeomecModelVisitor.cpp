#include "GeomecModelVisitor.h"

#include "3DFormation.h"
#include "3DHorizon.h"
#include "3DSupportNode.h"
#include "3dmodel.h"
#include "AnalysisPoint.h"
#include "BaseSupportNode.h"
#include "BoundaryBase.h"
#include "BoundaryInterfaceDef.h"
#include "CamClayUpscaling.h"
#include "ColorGradient.h"
#include "ColorScaleEntry.h"
#include "CrossSection.h"
#include "DepletionStage.h"
#include "DisplacementSupportNode.h"
#include "ElementSet.h"
#include "ExportResultData.h"
#include "ExportResultDataEntry.h"
#include "FaultParametersNode.h"
#include "FaultPressure.h"
#include "FormationBase.h"
#include "FormationPlane.h"
#include "FormationVolume.h"
#include "GoCadModel.h"
#include "GocadElementSet.h"
#include "HexaBoundary.h"
#include "HexaFormation.h"
#include "HexaHorizon.h"
#include "HexaMesh.h"
#include "HomogenizationBox.h"
#include "HorizonBase.h"
#include "HotSpot.h"
#include "IScalingBox.h"
#include "InterfaceBoundary.h"
#include "MeasuredTopDisplacementsNode.h"
#include "MeshObserver.h"
#include "MeshSampler.h"
#include "NonMeshedSurface.h"
#include "NonMeshedSurfaceParametersNode.h"
#include "NonMeshedSurfacePressure.h"
#include "RpnFormationOperand.h"
#include "StrainLoad.h"
#include "ThinLayerUpscaling.h"
#include "Upscaling.h"
#include "ValueComponent.h"
#include "VolumetricStrainLoad.h"
#include "WellFormation.h"
#include "WellFormationVolume.h"
#include "hexameshregion.h"
///// tot hier...
#include "CalculatedTemperature.h"
#include "DerivedResult.h"
#include "EclipseModel.h"
#include "ElementValueSet.h"
#include "FractTensor.h"
#include "FractureMatrixPressure.h"
#include "GVTResult.h"
#include "GVTSettings.h"
#include "GeoSurface.h"
#include "GeomecIValueSet.h"
#include "GlobalInitialStressNode.h"
#include "GlobalPressure.h"
#include "GlobalTemperature.h"
#include "HexaModel.h"
#include "HexaSurface.h"
#include "IMaterial.h"
#include "IMaterialBase.h"
#include "IMaterialRock.h"
#include "IMaterialServerBase.h"
#include "IPointSet.h"
#include "IStrainTensorGroup.h"
#include "IStressTensorGroup.h"
#include "ITensorGroup.h"
#include "IVectorResult.h"
#include "IWellMesh.h"
#include "IWellModel.h"
#include "InterfaceMaterial.h"
#include "LasPointSet.h"
#include "MatParamsTensor.h"
#include "MaterialAnisotropicCamClay.h"
#include "MaterialCamClay.h"
#include "MaterialCamClayCreep.h"
#include "MaterialCreep.h"
#include "MaterialDualCapLinEla.h"
#include "MaterialEntry.h"
#include "MaterialFractAnisotropy.h"
#include "MaterialFractureAperture.h"
#include "MaterialFractureAperture2.h"
#include "MaterialFractureApertureBase.h"
#include "MaterialLinear.h"
#include "MaterialModMohrCo.h"
#include "MaterialMohrCo.h"
#include "MaterialResultTree.h"
#include "MaterialRigidity.h"
#include "MaterialServer.h"
#include "MaterialUndrained.h"
#include "MaterialUpscaledAnisotropy.h"
#include "MeshBase.h"
#include "MeshRegionBase.h"
#include "MeshResultTree.h"
#include "ModelBase.h"
#include "NewWellPath.h"
#include "NodalValueSet.h"
#include "PointSet.h"
#include "Pressure.h"
#include "PressureSupportNode.h"
#include "QBSettings.h"
#include "Result.h"
#include "ResultRegister.h"
#include "ResultTree.h"
#include "RpnValueSet.h"
#include "StaborResult.h"
#include "Temperature.h"
#include "TetraBoundary.h"
#include "TetraFormation.h"
#include "TetraHorizonBase.h"
#include "TetraMesh.h"
#include "TetraModel.h"
#include "TetraSubBoundary.h"
#include "TetraSubHorizon.h"
#include "TetraSuperHorizon.h"
#include "TetraSurface.h"
#include "ValueStrain.h"
#include "ValueTensor.h"
#include "ValueType.h"
#include "ValueTypeFactory.h"
#include "ValueVector.h"
#include "WellCasingCement.h"
#include "WellCasingCementInterface.h"
#include "WellCasingInternalPressure.h"
#include "WellCasingInternalTemperature.h"
#include "WellCasingMesh.h"
#include "WellCasingModel.h"
#include "WellCasingNode.h"
#include "WellCasingRTCI.h"
#include "WellCasingSteel.h"
#include "WellCasingSteelMaterial.h"
#include "WellZoomInMesh.h"
#include "WellZoomInModel.h"
#include "ZoominModelEntry.h"
#include "ZoominModelPlaceHolder.h"
#include "materialmccohesionhard1.h"
#include "materialmccohesionhard2.h"
#include "materialmccohesionhard3.h"
#include "materialmcfrictionhard1.h"
#include "materialmcfrictionhard2.h"
#include "materialmcfrictionhard3.h"

IGeomecModelConstVisitor::IGeomecModelConstVisitor() {}

bool IGeomecModelConstVisitor::Visit3DFormation(const C3DFormation &formation) { return VisitFormationBase(formation); }

bool IGeomecModelConstVisitor::Visit3DHorizon(const C3DHorizon &horizon) { return VisitHorizonBase(horizon); }

bool IGeomecModelConstVisitor::Visit3DModel(const C3DModel &model) { return VisitModelBase(model); }

bool IGeomecModelConstVisitor::Visit3DSupportNode(const C3DSupportNode &node) { return VisitBaseSupportNode(node); }

bool IGeomecModelConstVisitor::VisitAnalysisPoint(const CAnalysisPoint &point) { return VisitColorNode(point); }

bool IGeomecModelConstVisitor::VisitAnalysisPointEntry(const CAnalysisPointEntry &entry) {
  return VisitGraphEntry(entry);
}

bool IGeomecModelConstVisitor::VisitSupportDepletion(const CSupportDepletion &supportdepletion) {
  return VisitStorageNode(supportdepletion);
}

bool IGeomecModelConstVisitor::VisitBaseSupportNode(const CBaseSupportNode &node) { return VisitOpenGLNode(node); }

bool IGeomecModelConstVisitor::VisitBoundaryBase(const CBoundaryBase &boundary) { return VisitColorNode(boundary); }

bool IGeomecModelConstVisitor::VisitBoundaryInterfaceDef(const CBoundaryInterfaceDef &def) {
  return VisitStorageNode(def);
}

bool IGeomecModelConstVisitor::VisitCamClayUpscalingNode(const CCamClayUpscalingNode &node) {
  return VisitIUpscalingNodeBase(node);
}

bool IGeomecModelConstVisitor::VisitColorGradient(const CColorGradient &gradient) {
  return VisitIColorScaleNode(gradient);
}

bool IGeomecModelConstVisitor::VisitColorScaleEntry(const CColorScaleEntry &entry) {
  return VisitIColorScaleEntry(entry);
}

bool IGeomecModelConstVisitor::VisitCrossSection(const CCrossSection &crosssection) {
  return VisitStorageNode(crosssection);
}

bool IGeomecModelConstVisitor::VisitCrossSectionEntry(const CCrossSectionEntry &entry) {
  return VisitGraphEntry(entry);
}

bool IGeomecModelConstVisitor::VisitDepletionStage(const CDepletionStage &stage) { return VisitStorageNode(stage); }

bool IGeomecModelConstVisitor::VisitDepletionStageEntry(const CDepletionStageEntry &entry) {
  return VisitGraphEntry(entry);
}

bool IGeomecModelConstVisitor::VisitDisplacementSupportNode(const CDisplacementSupportNode &node) {
  return Visit3DSupportNode(node);
}

bool IGeomecModelConstVisitor::VisitIElementSet(const IElementSet &elementset) { return VisitIPointSet(elementset); }

bool IGeomecModelConstVisitor::VisitElementSet(const CElementSet &elementset) { return VisitIElementSet(elementset); }

bool IGeomecModelConstVisitor::VisitExportResultData(const CExportResultData &data) { return VisitColorNode(data); }

bool IGeomecModelConstVisitor::VisitExportResultDataEntry(const CExportResultDataEntry &entry) {
  return VisitGraphEntry(entry);
}

bool IGeomecModelConstVisitor::VisitFaultParametersNode(const CFaultParametersNode &node) {
  return VisitStorageNode(node);
}

bool IGeomecModelConstVisitor::VisitFaultPressureComponent(const CFaultPressureComponent &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelConstVisitor::VisitFaultPressure(const CFaultPressure &pressure) {
  return VisitIValueComposite(pressure);
}

bool IGeomecModelConstVisitor::VisitFaultPressureGradientComponent(const CFaultPressureGradientComponent &component) {
  return VisitFaultPressureComponent(component);
}

bool IGeomecModelConstVisitor::VisitFaultPressureMinComponent(const CFaultPressureMinComponent &component) {
  return VisitFaultPressureComponent(component);
}

bool IGeomecModelConstVisitor::VisitFaultPressureMaxComponent(const CFaultPressureMaxComponent &component) {
  return VisitFaultPressureComponent(component);
}

bool IGeomecModelConstVisitor::VisitFaultPressureMaxGradientComponent(
    const CFaultPressureMaxGradientComponent &component) {
  return VisitFaultPressureComponent(component);
}

bool IGeomecModelConstVisitor::VisitGocadElementSet(const CGocadElementSet &elementset) {
  return VisitElementSet(elementset);
}

bool IGeomecModelConstVisitor::VisitHomogenizationBox(const CHomogenizationBox &box) { return VisitIScalingBox(box); }

bool IGeomecModelConstVisitor::VisitHomoBoxEntry(const CHomoBoxEntry &entry) { return VisitGraphEntry(entry); }

bool IGeomecModelConstVisitor::VisitHorizonBase(const CHorizonBase &horizon) { return VisitColorNode(horizon); }

bool IGeomecModelConstVisitor::VisitHotSpot(const CHotSpot &hotspot) { return VisitIColorScaleNode(hotspot); }

bool IGeomecModelConstVisitor::VisitInterfaceBoundary(const CInterfaceBoundary &boundary) {
  return VisitBoundaryBase(boundary);
}

bool IGeomecModelConstVisitor::VisitIScalingBox(const IScalingBox &box) { return VisitColorNode(box); }

bool IGeomecModelConstVisitor::VisitMeasuredTopDisplacementsNode(const CMeasuredTopDisplacementsNode &node) {
  return VisitStorageNode(node);
}

bool IGeomecModelConstVisitor::VisitMeshObserver(const CMeshObserver &observer) { return VisitGraphNode(observer); }

bool IGeomecModelConstVisitor::VisitPillarMap(const CPillarMap &map) { return VisitColorNode(map); }

bool IGeomecModelConstVisitor::VisitNonMeshedSurface(const CNonMeshedSurface &surface) {
  return VisitColorNode(surface);
}

bool IGeomecModelConstVisitor::VisitNonMeshedSurfaceEntry(const CNonMeshedSurfaceEntry &entry) {
  return VisitGraphEntry(entry);
}

bool IGeomecModelConstVisitor::VisitNonMeshedSurfaceParametersNode(const CNonMeshedSurfaceParametersNode &node) {
  return VisitStorageNode(node);
}

bool IGeomecModelConstVisitor::VisitNonMeshedSurfacePressureComponent(
    const CNonMeshedSurfacePressureComponent &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelConstVisitor::VisitNonMeshedSurfacePressure(const CNonMeshedSurfacePressure &pressure) {
  return VisitIValueComposite(pressure);
}

bool IGeomecModelConstVisitor::VisitFormationObserver(const CFormationObserver &observer) {
  return VisitGraphNode(observer);
}

bool IGeomecModelConstVisitor::VisitStrainLoadGradientComponent(const CStrainLoadGradientComponent &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelConstVisitor::VisitStrainLoadGWCDummyComponent(const CStrainLoadGWCDummyComponent &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelConstVisitor::VisitStrainLoadRepeaterComponent(const CStrainLoadRepeaterComponent &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelConstVisitor::VisitStrainLoad(const CStrainLoad &load) { return VisitIValueComposite(load); }

bool IGeomecModelConstVisitor::VisitThinLayerUpscalingNode(const CThinLayerUpscalingNode &node) {
  return VisitIUpscalingNodeBase(node);
}

bool IGeomecModelConstVisitor::VisitIUpscalingNodeBase(const IUpscalingNodeBase &node) {
  return VisitStorageNode(node);
}

bool IGeomecModelConstVisitor::VisitUpscalingEntry(const CUpscalingEntry &entry) { return VisitGraphEntry(entry); }

bool IGeomecModelConstVisitor::VisitUpscalingFormationsBranch(const CUpscalingFormationsBranch &branch) {
  return VisitOpenGLNode(branch);
}

bool IGeomecModelConstVisitor::VisitIUpscalingPointsetBranchBase(const IUpscalingPointsetBranchBase &branch) {
  return VisitOpenGLNode(branch);
}

bool IGeomecModelConstVisitor::VisitUpscalingPointsetRangesBranch(const CUpscalingPointsetRangesBranch &branch) {
  return VisitIUpscalingPointsetBranchBase(branch);
}

bool IGeomecModelConstVisitor::VisitUpscalingTargetPointsetBranch(const CUpscalingTargetPointsetBranch &branch) {
  return VisitIUpscalingPointsetBranchBase(branch);
}

bool IGeomecModelConstVisitor::VisitValueComponent(const CValueComponent &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelConstVisitor::VisitVolumetricStrainLoadGradientComponent(
    const CVolumetricStrainLoadGradientComponent &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelConstVisitor::VisitVolumetricStrainLoadGWCDummyComponent(
    const CVolumetricStrainLoadGWCDummyComponent &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelConstVisitor::VisitVolumetricStrainLoadRepeaterComponent(
    const CVolumetricStrainLoadRepeaterComponent &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelConstVisitor::VisitVolumetricStrainLoad(const CVolumetricStrainLoad &load) {
  return VisitIValueComposite(load);
}

bool IGeomecModelConstVisitor::VisitIFormationElementSet(const IFormationElementSet &elementset) {
  return VisitIElementSet(elementset);
}

bool IGeomecModelConstVisitor::VisitFormationBase(const CFormationBase &formation) { return VisitColorNode(formation); }

bool IGeomecModelConstVisitor::VisitFormationThickness(const CFormationThickness &formationthickness) {
  return VisitIValueComposite(formationthickness);
}

bool IGeomecModelConstVisitor::VisitFormationPlane(const CFormationPlane &plane) { return VisitColorNode(plane); }

bool IGeomecModelConstVisitor::VisitFormationVolume(const CFormationVolume &volume) {
  return VisitIFormationElementSet(volume);
}

bool IGeomecModelConstVisitor::VisitWellFormation(const CWellFormation &formation) {
  return Visit3DFormation(formation);
}

bool IGeomecModelConstVisitor::VisitWellFormationEntry(const CWellFormationEntry &entry) {
  return VisitGraphEntry(entry);
}

bool IGeomecModelConstVisitor::VisitWellFormationVolume(const CWellFormationVolume &volume) {
  return VisitFormationVolume(volume);
}

bool IGeomecModelConstVisitor::VisitGoCadModel(const CGoCadModel &model) { return VisitTetraModel(model); }

bool IGeomecModelConstVisitor::VisitGoCadSubModelEntry(const CGoCadSubModelEntry &entry) {
  return VisitGraphEntry(entry);
}

bool IGeomecModelConstVisitor::VisitHexaBoundary(const CHexaBoundary &boundary) {
  return VisitInterfaceBoundary(boundary);
}

bool IGeomecModelConstVisitor::VisitHexaFormationVolume(const CHexaFormationVolume &volume) {
  return VisitFormationVolume(volume);
}

bool IGeomecModelConstVisitor::VisitHexaFormation(const CHexaFormation &formation) {
  return Visit3DFormation(formation);
}

bool IGeomecModelConstVisitor::VisitHexaFormationEntry(const CHexaFormationEntry &entry) {
  return VisitGraphEntry(entry);
}

bool IGeomecModelConstVisitor::VisitHexaHorizon(const CHexaHorizon &horizon) { return Visit3DHorizon(horizon); }

bool IGeomecModelConstVisitor::VisitHexaHorizonEntry(const CHexaHorizonEntry &entry) { return VisitGraphEntry(entry); }

bool IGeomecModelConstVisitor::VisitHexaMesh(const CHexaMesh &mesh) { return VisitMeshBase(mesh); }

bool IGeomecModelConstVisitor::VisitHexaMeshRegionBase(const CHexaMeshRegionBase &region) {
  return VisitMeshRegionBase(region);
}

bool IGeomecModelConstVisitor::VisitHexaMainMeshRegion(const CHexaMainMeshRegion &region) {
  return VisitHexaMeshRegionBase(region);
}

bool IGeomecModelConstVisitor::VisitHexaSubMeshRegion(const CHexaSubMeshRegion &region) {
  return VisitHexaMeshRegionBase(region);
}

bool IGeomecModelConstVisitor::VisitHexaMeshRegionEntry(const CHexaMeshRegionEntry &entry) {
  return VisitGraphEntry(entry);
}

bool IGeomecModelConstVisitor::VisitHexaModel(const CHexaModel &model) { return Visit3DModel(model); }

bool IGeomecModelConstVisitor::VisitHexaSurface(const CHexaSurface &surface) { return VisitSurfaceBase(surface); }

bool IGeomecModelConstVisitor::VisitIMaterial(const IMaterial &material) { return VisitIMaterialBase(material); }

bool IGeomecModelConstVisitor::VisitIMaterialBase(const IMaterialBase &material) { return VisitStorageNode(material); }

bool IGeomecModelConstVisitor::VisitIMaterialRock(const IMaterialRock &material) { return VisitIMaterial(material); }

bool IGeomecModelConstVisitor::VisitIMaterialServerBase(const IMaterialServerBase &server) {
  return VisitStorageNode(server);
}

bool IGeomecModelConstVisitor::VisitInterfaceMaterial(const CInterfaceMaterial &material) {
  return VisitIMaterialBase(material);
}

bool IGeomecModelConstVisitor::VisitInterfaceMaterialEntry(const CInterfaceMaterialEntry &entry) {
  return VisitGraphEntry(entry);
}

bool IGeomecModelConstVisitor::VisitMaterialAnisotropicCamClay(const CMaterialAnisotropicCamClay &material) {
  return VisitIMaterialRock(material);
}

bool IGeomecModelConstVisitor::VisitMaterialCamClay(const CMaterialCamClay &material) {
  return VisitIMaterialRock(material);
}

bool IGeomecModelConstVisitor::VisitMaterialCamClayCreep(const CMaterialCamClayCreep &material) {
  return VisitIMaterialRock(material);
}

bool IGeomecModelConstVisitor::VisitMaterialCreep(const CMaterialCreep &material) {
  return VisitIMaterialRock(material);
}

bool IGeomecModelConstVisitor::VisitMaterialDualCapLinEla(const CMaterialDualCapLinEla &material) {
  return VisitIMaterialRock(material);
}

bool IGeomecModelConstVisitor::VisitMaterialEntry(const CMaterialEntry &entry) { return VisitGraphEntry(entry); }

bool IGeomecModelConstVisitor::VisitMaterialFractAnisotropy(const CMaterialFractAnisotropy &material) {
  return VisitMaterialFractureApertureBase(material);
}

bool IGeomecModelConstVisitor::VisitMaterialFractureAperture(const CMaterialFractureAperture &material) {
  return VisitMaterialFractureApertureBase(material);
}

bool IGeomecModelConstVisitor::VisitMaterialFractureAperture2(const CMaterialFractureAperture2 &material) {
  return VisitMaterialFractureApertureBase(material);
}

bool IGeomecModelConstVisitor::VisitMaterialFractureApertureBase(const CMaterialFractureApertureBase &material) {
  return VisitIMaterialRock(material);
}

bool IGeomecModelConstVisitor::VisitMaterialLinear(const CMaterialLinear &material) {
  return VisitIMaterialRock(material);
}

bool IGeomecModelConstVisitor::VisitMaterialMCCohesionHard1(const CMaterialMCCohesionHard1 &material) {
  return VisitMaterialMohrCo(material);
}

bool IGeomecModelConstVisitor::VisitMaterialMCCohesionHard2(const CMaterialMCCohesionHard2 &material) {
  return VisitMaterialMohrCo(material);
}

bool IGeomecModelConstVisitor::VisitMaterialMCCohesionHard3(const CMaterialMCCohesionHard3 &material) {
  return VisitMaterialMohrCo(material);
}

bool IGeomecModelConstVisitor::VisitMaterialMCFrictionHard1(const CMaterialMCFrictionHard1 &material) {
  return VisitMaterialMohrCo(material);
}

bool IGeomecModelConstVisitor::VisitMaterialMCFrictionHard2(const CMaterialMCFrictionHard2 &material) {
  return VisitMaterialMohrCo(material);
}

bool IGeomecModelConstVisitor::VisitMaterialMCFrictionHard3(const CMaterialMCFrictionHard3 &material) {
  return VisitMaterialMohrCo(material);
}

bool IGeomecModelConstVisitor::VisitMaterialModMohrCo(const CMaterialModMohrCo &material) {
  return VisitIMaterialRock(material);
}

bool IGeomecModelConstVisitor::VisitMaterialMohrCo(const CMaterialMohrCo &material) {
  return VisitIMaterialRock(material);
}

bool IGeomecModelConstVisitor::VisitMaterialResult(const CMaterialResult &matresult) { return VisitIResult(matresult); }

bool IGeomecModelConstVisitor::VisitMaterialResultTree(const CMaterialResultTree &tree) {
  return VisitResultGroup(tree);
}

bool IGeomecModelConstVisitor::VisitMaterialRigidity(const CMaterialRigidity &material) {
  return VisitIMaterialRock(material);
}

bool IGeomecModelConstVisitor::VisitMaterialServer(const CMaterialServer &server) {
  return VisitIMaterialServerBase(server);
}

bool IGeomecModelConstVisitor::VisitMaterialUndrained(const CMaterialUndrained &material) {
  return VisitIMaterialRock(material);
}

bool IGeomecModelConstVisitor::VisitMaterialUpscaledAnisotropy(const CMaterialUpscaledAnisotropy &material) {
  return VisitIMaterialRock(material);
}

bool IGeomecModelConstVisitor::VisitWellCasingCement(const CWellCasingCement &cement) {
  return VisitFormationBase(cement);
}

bool IGeomecModelConstVisitor::VisitWellCasingMaterialServer(const CWellCasingMaterialServer &server) {
  return VisitIMaterialServerBase(server);
}

bool IGeomecModelConstVisitor::VisitWellCasingSteel(const CWellCasingSteel &steel) { return VisitOpenGLNode(steel); }

bool IGeomecModelConstVisitor::VisitWellCasingSteelMaterial(const CWellCasingSteelMaterial &material) {
  return VisitIMaterialBase(material);
}

bool IGeomecModelConstVisitor::VisitWellCasingSteelMaterialEntry(const CWellCasingSteelMaterialEntry &entry) {
  return VisitGraphEntry(entry);
}

bool IGeomecModelConstVisitor::VisitIWellMesh(const IWellMesh &mesh) { return VisitMeshBase(mesh); }

bool IGeomecModelConstVisitor::VisitMeshBase(const CMeshBase &mesh) { return VisitIElementSet(mesh); }

bool IGeomecModelConstVisitor::VisitMeshRegionBase(const CMeshRegionBase &region) { return VisitColorNode(region); }

bool IGeomecModelConstVisitor::VisitMeshResult(const CMeshResult &result) { return VisitIValueComposite(result); }

bool IGeomecModelConstVisitor::VisitMeshResultTree(const CMeshResultTree &tree) { return VisitGraphEntry(tree); }

bool IGeomecModelConstVisitor::VisitWellCasingMesh(const CWellCasingMesh &mesh) { return VisitIWellMesh(mesh); }

bool IGeomecModelConstVisitor::VisitWellZoomInMesh(const CWellZoomInMesh &mesh) { return VisitIWellMesh(mesh); }

bool IGeomecModelConstVisitor::VisitEclipseModel(const CEclipseModel &model) { return VisitTetraModel(model); }

bool IGeomecModelConstVisitor::VisitEclipseSubModelEntry(const CEclipseSubModelEntry &entry) {
  return VisitGraphEntry(entry);
}

bool IGeomecModelConstVisitor::VisitIWellModel(const IWellModel &model) { return VisitModelBase(model); }

bool IGeomecModelConstVisitor::VisitModelBase(const CModelBase &model) { return VisitFemAppModel(model); }

bool IGeomecModelConstVisitor::VisitWellCasingModel(const CWellCasingModel &model) { return VisitIWellModel(model); }

bool IGeomecModelConstVisitor::VisitWellCasingFormationEntry(const CWellCasingFormationEntry &entry) {
  return VisitWellFormationEntry(entry);
}

bool IGeomecModelConstVisitor::VisitWellZoomInModel(const CWellZoomInModel &model) { return VisitIWellModel(model); }

bool IGeomecModelConstVisitor::VisitIPointSet(const IPointSet &pointset) { return VisitColorNode(pointset); }

bool IGeomecModelConstVisitor::VisitPointSetEntry(const CPointSetEntry &entry) { return VisitGraphEntry(entry); }

bool IGeomecModelConstVisitor::VisitLasPointSet(const CLasPointSet &pointset) { return VisitPointSet(pointset); }

bool IGeomecModelConstVisitor::VisitPointSet(const CPointSet &pointset) { return VisitIPointSet(pointset); }

bool IGeomecModelConstVisitor::VisitFractureMatrixComponentGWC(const CFractureMatrixComponentGWC &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelConstVisitor::VisitFractureMatrixComponentRepeater(const CFractureMatrixComponentRepeater &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelConstVisitor::VisitFractureMatrixPressure(const CFractureMatrixPressure &pressure) {
  return VisitIValueComposite(pressure);
}

bool IGeomecModelConstVisitor::VisitGlobalPressure(const CGlobalPressure &pressure) {
  return VisitIValueComposite(pressure);
}

bool IGeomecModelConstVisitor::VisitComponentGWC(const CComponentGWC &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelConstVisitor::VisitComponentRepeater(const CComponentRepeater &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelConstVisitor::VisitPressure(const CPressure &pressure) { return VisitIValueComposite(pressure); }

bool IGeomecModelConstVisitor::VisitPressureSupportNode(const CPressureSupportNode &node) {
  return Visit3DSupportNode(node);
}

bool IGeomecModelConstVisitor::VisitWellCasingInternalPressure(const CWellCasingInternalPressure &pressure) {
  return VisitIValueComposite(pressure);
}

bool IGeomecModelConstVisitor::VisitDerivedResult(const CDerivedResult &result) { return VisitIResult(result); }

bool IGeomecModelConstVisitor::VisitDerivedResultGroup(const CDerivedResultGroup &group) {
  return VisitResultGroup(group);
}

bool IGeomecModelConstVisitor::VisitIVectorResult(const IVectorResult &result) { return VisitIResult(result); }

bool IGeomecModelConstVisitor::VisitVectorResult(const CVectorResult &result) { return VisitIVectorResult(result); }

bool IGeomecModelConstVisitor::VisitResultGroup(const CResultGroup &group) { return VisitGraphNode(group); }

bool IGeomecModelConstVisitor::VisitIResult(const IResult &result) { return VisitIValueComposite(result); }

bool IGeomecModelConstVisitor::VisitIResultComponent(const IResultComponent &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelConstVisitor::VisitIScalarResultComponent(const IScalarResultComponent &component) {
  return VisitIResultComponent(component);
}

bool IGeomecModelConstVisitor::VisitResultTree(const CResultTree &tree) { return VisitResultGroup(tree); }

bool IGeomecModelConstVisitor::VisitStaborResult(const CStaborResult &result) { return VisitIResult(result); }

bool IGeomecModelConstVisitor::VisitGVTSettings(const CGVTSettings &settings) { return VisitStorageNode(settings); }

bool IGeomecModelConstVisitor::VisitQBSettings(const CQBSettings &settings) { return VisitStorageNode(settings); }

bool IGeomecModelConstVisitor::VisitSurfaceBase(const CSurfaceBase &surface) { return VisitIElementSet(surface); }

bool IGeomecModelConstVisitor::VisitSurfaceEntry(const CSurfaceEntry &entry) { return VisitGraphEntry(entry); }

bool IGeomecModelConstVisitor::VisitCalculatedTemperature(const CCalculatedTemperature &temperature) {
  return VisitIValueComposite(temperature);
}

bool IGeomecModelConstVisitor::VisitGlobalTemperature(const CGlobalTemperature &temperature) {
  return VisitIValueComposite(temperature);
}

bool IGeomecModelConstVisitor::VisitTemperatureGradientComponent(const CTemperatureGradientComponent &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelConstVisitor::VisitTemperatureGWCDummyComponent(const CTemperatureGWCDummyComponent &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelConstVisitor::VisitTemperatureRepeaterComponent(const CTemperatureRepeaterComponent &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelConstVisitor::VisitTemperature(const CTemperature &temperature) {
  return VisitIValueComposite(temperature);
}

bool IGeomecModelConstVisitor::VisitWellCasingInternalTemperature(const CWellCasingInternalTemperature &temperature) {
  return VisitIValueComposite(temperature);
}

bool IGeomecModelConstVisitor::VisitFractTensor(const CFractTensor &tensor) { return VisitMatParamsTensor(tensor); }

bool IGeomecModelConstVisitor::VisitIStrainTensorGroup(const IStrainTensorGroup &group) {
  return VisitITensorGroup(group);
}

bool IGeomecModelConstVisitor::VisitIStressTensorGroup(const IStressTensorGroup &group) {
  return VisitITensorGroup(group);
}

bool IGeomecModelConstVisitor::VisitITensorGroup(const ITensorGroup &group) { return VisitResultGroup(group); }

bool IGeomecModelConstVisitor::VisitMatParamsTensor(const CMatParamsTensor &tensor) { return VisitValueType(tensor); }

bool IGeomecModelConstVisitor::VisitTStressTensor(const TStressTensor &tensor) { return VisitValueType(tensor); }

bool IGeomecModelConstVisitor::VisitTStrainTensor(const TStrainTensor &tensor) { return VisitValueType(tensor); }

bool IGeomecModelConstVisitor::VisitTetraBoundary(const CTetraBoundary &boundary) {
  return VisitInterfaceBoundary(boundary);
}

bool IGeomecModelConstVisitor::VisitTetraFormationVolume(const CTetraFormationVolume &volume) {
  return VisitFormationVolume(volume);
}

bool IGeomecModelConstVisitor::VisitTetraFormation(const CTetraFormation &formation) {
  return Visit3DFormation(formation);
}

bool IGeomecModelConstVisitor::VisitTetraFormationEntry(const CTetraFormationEntry &entry) {
  return VisitGraphEntry(entry);
}

bool IGeomecModelConstVisitor::VisitTetraHorizonBase(const CTetraHorizonBase &horizon) {
  return Visit3DHorizon(horizon);
}

bool IGeomecModelConstVisitor::VisitTetraMesh(const CTetraMesh &mesh) { return VisitMeshBase(mesh); }

bool IGeomecModelConstVisitor::VisitTetraModel(const CTetraModel &model) { return Visit3DModel(model); }

bool IGeomecModelConstVisitor::VisitTetraSubBoundary(const CTetraSubBoundary &boundary) {
  return VisitColorNode(boundary);
}

bool IGeomecModelConstVisitor::VisitTetraSubHorizon(const CTetraSubHorizon &horizon) {
  return VisitTetraHorizonBase(horizon);
}

bool IGeomecModelConstVisitor::VisitTetraSubHorizonEntry(const CTetraSubHorizonEntry &entry) {
  return VisitGraphEntry(entry);
}

bool IGeomecModelConstVisitor::VisitTetraSuperHorizon(const CTetraSuperHorizon &horizon) {
  return VisitTetraSuperHorizon(horizon);
}

bool IGeomecModelConstVisitor::VisitTetraSuperHorizonEntry(const CTetraSuperHorizonEntry &entry) {
  return VisitGraphEntry(entry);
}

bool IGeomecModelConstVisitor::VisitTetraSurface(const CTetraSurface &surface) { return VisitSurfaceBase(surface); }

bool IGeomecModelConstVisitor::VisitGlobalInitialStressNode(const CGlobalInitialStressNode &node) {
  return VisitStorageNode(node);
}

bool IGeomecModelConstVisitor::VisitResultRegister(const CResultRegister &resultregister) {
  return VisitStorageNode(resultregister);
}

bool IGeomecModelConstVisitor::VisitTStrain(const TStrain &strain) { return VisitValueType(strain); }

bool IGeomecModelConstVisitor::VisitValueType(const CValueType &valuetype) { return VisitIValueComposite(valuetype); }

bool IGeomecModelConstVisitor::VisitTLengthComponent(const TLengthComponent &component) {
  return VisitValueComponent(component);
}

bool IGeomecModelConstVisitor::VisitSurfaceValueType(const CSurfaceValueType &valuetype) {
  return VisitValueType(valuetype);
}

bool IGeomecModelConstVisitor::VisitTPressure(const TPressure &pressure) { return VisitValueType(pressure); }

bool IGeomecModelConstVisitor::VisitTFractureMatrixPressure(const TFractureMatrixPressure &pressure) {
  return VisitValueType(pressure);
}

bool IGeomecModelConstVisitor::VisitTTemperature(const TTemperature &temperature) {
  return VisitValueType(temperature);
}

bool IGeomecModelConstVisitor::VisitTVolumetricStrain(const TVolumetricStrain &strain) {
  return VisitValueType(strain);
}

bool IGeomecModelConstVisitor::VisitTNormalStrain(const TNormalStrain &strain) { return VisitValueType(strain); }

bool IGeomecModelConstVisitor::VisitTLateralStrain(const TLateralStrain &strain) { return VisitValueType(strain); }

bool IGeomecModelConstVisitor::VisitTCohesion(const TCohesion &cohesion) { return VisitValueType(cohesion); }

bool IGeomecModelConstVisitor::VisitTBulkStiffness(const TBulkStiffness &stiffness) {
  return VisitValueType(stiffness);
}

bool IGeomecModelConstVisitor::VisitTShearModulus(const TShearModulus &modulus) { return VisitValueType(modulus); }

bool IGeomecModelConstVisitor::VisitTVelocityP(const TVelocityP &vp) { return VisitValueType(vp); }

bool IGeomecModelConstVisitor::VisitTVelocityS(const TVelocityS &vs) { return VisitValueType(vs); }

bool IGeomecModelConstVisitor::VisitTFrictionAngle(const TFrictionAngle &angle) { return VisitValueType(angle); }

bool IGeomecModelConstVisitor::VisitTPoissonsRatio(const TPoissonsRatio &ratio) { return VisitValueType(ratio); }

bool IGeomecModelConstVisitor::VisitTYoungsModulus(const TYoungsModulus &modulus) { return VisitValueType(modulus); }

bool IGeomecModelConstVisitor::VisitTDensity(const TDensity &density) { return VisitValueType(density); }

bool IGeomecModelConstVisitor::VisitTPorosity(const TPorosity &porosity) { return VisitValueType(porosity); }

bool IGeomecModelConstVisitor::VisitTInitFriction(const TInitFriction &friction) { return VisitValueType(friction); }

bool IGeomecModelConstVisitor::VisitTHardening(const THardening &hardening) { return VisitValueType(hardening); }

bool IGeomecModelConstVisitor::VisitTPreConsolidation(const TPreConsolidation &preconsolidation) {
  return VisitValueType(preconsolidation);
}

bool IGeomecModelConstVisitor::VisitTCapShape(const TCapShape &capshape) { return VisitValueType(capshape); }

bool IGeomecModelConstVisitor::VisitTDilatation(const TDilatation &dilatation) { return VisitValueType(dilatation); }

bool IGeomecModelConstVisitor::VisitTLatRatioMax(const TLatRatioMax &ratio) { return VisitValueType(ratio); }

bool IGeomecModelConstVisitor::VisitTLatRatioMin(const TLatRatioMin &ratio) { return VisitValueType(ratio); }

bool IGeomecModelConstVisitor::VisitTAzimuth(const TAzimuth &azimuth) { return VisitValueType(azimuth); }

bool IGeomecModelConstVisitor::VisitTCreepA1(const TCreepA1 &a1) { return VisitValueType(a1); }

bool IGeomecModelConstVisitor::VisitTCreepN1(const TCreepN1 &n1) { return VisitValueType(n1); }

bool IGeomecModelConstVisitor::VisitTCreepA2(const TCreepA2 &a2) { return VisitValueType(a2); }

bool IGeomecModelConstVisitor::VisitTCreepN2(const TCreepN2 &n2) { return VisitValueType(n2); }

bool IGeomecModelConstVisitor::VisitTFluidBulkModulus(const TFluidBulkModulus &modulus) {
  return VisitValueType(modulus);
}

bool IGeomecModelConstVisitor::VisitTHardCohesion1(const THardCohesion1 &cohesion) { return VisitValueType(cohesion); }

bool IGeomecModelConstVisitor::VisitTHardCohesion2(const THardCohesion2 &cohesion) { return VisitValueType(cohesion); }

bool IGeomecModelConstVisitor::VisitTHardCohesion3(const THardCohesion3 &cohesion) { return VisitValueType(cohesion); }

bool IGeomecModelConstVisitor::VisitTHardFriction1(const THardFriction1 &friction) { return VisitValueType(friction); }

bool IGeomecModelConstVisitor::VisitTHardFriction2(const THardFriction2 &friction) { return VisitValueType(friction); }

bool IGeomecModelConstVisitor::VisitTHardFriction3(const THardFriction3 &friction) { return VisitValueType(friction); }

bool IGeomecModelConstVisitor::VisitTEquivalentPlasticStrain1(const TEquivalentPlasticStrain1 &strain) {
  return VisitValueType(strain);
}

bool IGeomecModelConstVisitor::VisitTEquivalentPlasticStrain2(const TEquivalentPlasticStrain2 &strain) {
  return VisitValueType(strain);
}

bool IGeomecModelConstVisitor::VisitTEquivalentPlasticStrain3(const TEquivalentPlasticStrain3 &strain) {
  return VisitValueType(strain);
}

bool IGeomecModelConstVisitor::VisitTCreepQR1(const TCreepQR1 &qr1) { return VisitValueType(qr1); }

bool IGeomecModelConstVisitor::VisitTCreepQR2(const TCreepQR2 &qr2) { return VisitValueType(qr2); }

bool IGeomecModelConstVisitor::VisitTCreepAlpha(const TCreepAlpha &alpha) { return VisitValueType(alpha); }

bool IGeomecModelConstVisitor::VisitTTensileStretch(const TTensileStretch &tensilestretch) {
  return VisitValueType(tensilestretch);
}

bool IGeomecModelConstVisitor::VisitTThermalExpansion(const TThermalExpansion &expansion) {
  return VisitValueType(expansion);
}

bool IGeomecModelConstVisitor::VisitTRigidParam1(const TRigidParam1 &param) { return VisitValueType(param); }

bool IGeomecModelConstVisitor::VisitTRigidParam2(const TRigidParam2 &param) { return VisitValueType(param); }

bool IGeomecModelConstVisitor::VisitTRigidParam3(const TRigidParam3 &param) { return VisitValueType(param); }

bool IGeomecModelConstVisitor::VisitTRigidParam4(const TRigidParam4 &param) { return VisitValueType(param); }

bool IGeomecModelConstVisitor::VisitTRigidParam5(const TRigidParam5 &param) { return VisitValueType(param); }

bool IGeomecModelConstVisitor::VisitTRigidParam6(const TRigidParam6 &param) { return VisitValueType(param); }

bool IGeomecModelConstVisitor::VisitTRigidParam7(const TRigidParam7 &param) { return VisitValueType(param); }

bool IGeomecModelConstVisitor::VisitTRigidParam8(const TRigidParam8 &param) { return VisitValueType(param); }

bool IGeomecModelConstVisitor::VisitTRigidParam9(const TRigidParam9 &param) { return VisitValueType(param); }

bool IGeomecModelConstVisitor::VisitTRigidParam10(const TRigidParam10 &param) { return VisitValueType(param); }

bool IGeomecModelConstVisitor::VisitTRigidParam11(const TRigidParam11 &param) { return VisitValueType(param); }

bool IGeomecModelConstVisitor::VisitTRigidParam12(const TRigidParam12 &param) { return VisitValueType(param); }

bool IGeomecModelConstVisitor::VisitTRigidParam13(const TRigidParam13 &param) { return VisitValueType(param); }

bool IGeomecModelConstVisitor::VisitTRigidParam14(const TRigidParam14 &param) { return VisitValueType(param); }

bool IGeomecModelConstVisitor::VisitTRigidParam15(const TRigidParam15 &param) { return VisitValueType(param); }

bool IGeomecModelConstVisitor::VisitTRigidParam16(const TRigidParam16 &param) { return VisitValueType(param); }

bool IGeomecModelConstVisitor::VisitTRigidParam17(const TRigidParam17 &param) { return VisitValueType(param); }

bool IGeomecModelConstVisitor::VisitTRigidParam18(const TRigidParam18 &param) { return VisitValueType(param); }

bool IGeomecModelConstVisitor::VisitTRigidParam19(const TRigidParam19 &param) { return VisitValueType(param); }

bool IGeomecModelConstVisitor::VisitTRigidParam20(const TRigidParam20 &param) { return VisitValueType(param); }

bool IGeomecModelConstVisitor::VisitTRigidParam21(const TRigidParam21 &param) { return VisitValueType(param); }

bool IGeomecModelConstVisitor::VisitTElasticHardening(const TElasticHardening &hardening) {
  return VisitValueType(hardening);
}

bool IGeomecModelConstVisitor::VisitTSecondaryPreconsolidation(const TSecondaryPreconsolidation &precon) {
  return VisitValueType(precon);
}

bool IGeomecModelConstVisitor::VisitTSecondaryHardening(const TSecondaryHardening &hardening) {
  return VisitValueType(hardening);
}

bool IGeomecModelConstVisitor::VisitTUniaxialElasticCompressibility(const TUniaxialElasticCompressibility &comp) {
  return VisitValueType(comp);
}

bool IGeomecModelConstVisitor::VisitTUniaxialPlasticCompressibility(const TUniaxialPlasticCompressibility &comp) {
  return VisitValueType(comp);
}

bool IGeomecModelConstVisitor::VisitTYoungModulusNormal(const TYoungModulusNormal &modulus) {
  return VisitValueType(modulus);
}

bool IGeomecModelConstVisitor::VisitTYoungModulusTransverse(const TYoungModulusTransverse &modulus) {
  return VisitValueType(modulus);
}

bool IGeomecModelConstVisitor::VisitTPoissonRatioNormal(const TPoissonRatioNormal &ratio) {
  return VisitValueType(ratio);
}

bool IGeomecModelConstVisitor::VisitTPoissonRatioTransverse(const TPoissonRatioTransverse &ratio) {
  return VisitValueType(ratio);
}

bool IGeomecModelConstVisitor::VisitTLayerNormalIncl(const TLayerNormalIncl &incl) { return VisitValueType(incl); }

bool IGeomecModelConstVisitor::VisitTLayerNormalAzi(const TLayerNormalAzi &azi) { return VisitValueType(azi); }

bool IGeomecModelConstVisitor::VisitTFractStiffNormal(const TFractStiffNormal &stiff) { return VisitValueType(stiff); }

bool IGeomecModelConstVisitor::VisitTFractStiffShear(const TFractStiffShear &stiff) { return VisitValueType(stiff); }

bool IGeomecModelConstVisitor::VisitTThermLinExpNormal(const TThermLinExpNormal &exp) { return VisitValueType(exp); }

bool IGeomecModelConstVisitor::VisitTThermLinExpLateral(const TThermLinExpLateral &exp) { return VisitValueType(exp); }

bool IGeomecModelConstVisitor::VisitTThermalConductivity(const TThermalConductivity &conduc) {
  return VisitValueType(conduc);
}

bool IGeomecModelConstVisitor::VisitTThermalCapacity(const TThermalCapacity &capacity) {
  return VisitValueType(capacity);
}

bool IGeomecModelConstVisitor::VisitTHighFractDens(const THighFractDens &dens) { return VisitValueType(dens); }

bool IGeomecModelConstVisitor::VisitTHighFractDensIncl(const THighFractDensIncl &incl) { return VisitValueType(incl); }

bool IGeomecModelConstVisitor::VisitTHighFractDensAzi(const THighFractDensAzi &azi) { return VisitValueType(azi); }

bool IGeomecModelConstVisitor::VisitTLowFractDens(const TLowFractDens &dens) { return VisitValueType(dens); }

bool IGeomecModelConstVisitor::VisitTLowFractDensIncl(const TLowFractDensIncl &incl) { return VisitValueType(incl); }

bool IGeomecModelConstVisitor::VisitTLowFractDensAzi(const TLowFractDensAzi &azi) { return VisitValueType(azi); }

bool IGeomecModelConstVisitor::VisitTInterFractDens(const TInterFractDens &dens) { return VisitValueType(dens); }

bool IGeomecModelConstVisitor::VisitTPermeability(const TPermeability &perm) { return VisitValueType(perm); }

bool IGeomecModelConstVisitor::VisitTViscosity(const TViscosity &viscosity) { return VisitValueType(viscosity); }

bool IGeomecModelConstVisitor::VisitTYoungsModulusDecompaction(const TYoungsModulusDecompaction &modulus) {
  return VisitValueType(modulus);
}

bool IGeomecModelConstVisitor::VisitTPoissonsRatioDecompaction(const TPoissonsRatioDecompaction &ratio) {
  return VisitValueType(ratio);
}

bool IGeomecModelConstVisitor::VisitTFluidThermalExpansionCoefficient(
    const TFluidThermalExpansionCoefficient &coefficient) {
  return VisitValueType(coefficient);
}

bool IGeomecModelConstVisitor::VisitTFluidDensity(const TFluidDensity &density) { return VisitValueType(density); }

bool IGeomecModelConstVisitor::VisitTReferenceStress(const TReferenceStress &refstress) {
  return VisitValueType(refstress);
}

bool IGeomecModelConstVisitor::VisitTApertureParameter(const TApertureParameter &param) {
  return VisitValueType(param);
}

bool IGeomecModelConstVisitor::VisitTGrainStiffnessParameter(const TGrainStiffnessParameter &param) {
  return VisitValueType(param);
}

bool IGeomecModelConstVisitor::VisitTYoungsModulusNormalDecomp(const TYoungsModulusNormalDecomp &modulus) {
  return VisitValueType(modulus);
}

bool IGeomecModelConstVisitor::VisitTYoungModulusTransDecomp(const TYoungModulusTransDecomp &modulus) {
  return VisitValueType(modulus);
}

bool IGeomecModelConstVisitor::VisitTPoissonRatioNormalDecomp(const TPoissonRatioNormalDecomp &ratio) {
  return VisitValueType(ratio);
}

bool IGeomecModelConstVisitor::VisitTPoissonRatioTransDecomp(const TPoissonRatioTransDecomp &ratio) {
  return VisitValueType(ratio);
}

bool IGeomecModelConstVisitor::VisitTAnisotropicShearModulus(const TAnisotropicShearModulus &modulus) {
  return VisitValueType(modulus);
}

bool IGeomecModelConstVisitor::VisitTAnisotropicShearModulusDecomp(const TAnisotropicShearModulusDecomp &modulus) {
  return VisitValueType(modulus);
}

bool IGeomecModelConstVisitor::VisitTThomsenEpsilon(const TThomsenEpsilon &eps) { return VisitValueType(eps); }

bool IGeomecModelConstVisitor::VisitTThomsenGamma(const TThomsenGamma &gamma) { return VisitValueType(gamma); }

bool IGeomecModelConstVisitor::VisitTThomsenDelta(const TThomsenDelta &delta) { return VisitValueType(delta); }

bool IGeomecModelConstVisitor::VisitTInitialFractureApertureHigh(const TInitialFractureApertureHigh &ap) {
  return VisitValueType(ap);
}

bool IGeomecModelConstVisitor::VisitTInitialFractureApertureInter(const TInitialFractureApertureInter &ap) {
  return VisitValueType(ap);
}

bool IGeomecModelConstVisitor::VisitTInitialFractureApertureLow(const TInitialFractureApertureLow &ap) {
  return VisitValueType(ap);
}

bool IGeomecModelConstVisitor::VisitTReferenceAperture(const TReferenceAperture &ap) { return VisitValueType(ap); }

bool IGeomecModelConstVisitor::VisitTDynamicUniaxialStiffness(const TDynamicUniaxialStiffness &stiffness) {
  return VisitValueType(stiffness);
}

bool IGeomecModelConstVisitor::VisitTDynamicShearStiffness(const TDynamicShearStiffness &stiffness) {
  return VisitValueType(stiffness);
}

bool IGeomecModelConstVisitor::VisitTDisplacementVector(const TDisplacementVector &vector) {
  return VisitValueType(vector);
}

bool IGeomecModelConstVisitor::VisitTNormalStress(const TNormalStress &value) { return VisitValueType(value); }

bool IGeomecModelConstVisitor::VisitTMeanStress(const TMeanStress &value) { return VisitValueType(value); }

bool IGeomecModelConstVisitor::VisitTTimeStrain(const TTimeStrain &value) { return VisitValueType(value); }

bool IGeomecModelConstVisitor::VisitTDeltaV(const TDeltaV &value) { return VisitValueType(value); }

bool IGeomecModelConstVisitor::VisitTDeltaT(const TDeltaT &value) { return VisitValueType(value); }

bool IGeomecModelConstVisitor::VisitTDisplacementZ(const TDisplacementZ &value) { return VisitValueType(value); }

bool IGeomecModelConstVisitor::VisitTDummyMaterialParameter(const TDummyMaterialParameter &value) {
  return VisitValueType(value);
}

bool IGeomecModelConstVisitor::VisitTBoundaryKradMaterialParameter(const TBoundaryKradMaterialParameter &value) {
  return VisitValueType(value);
}

bool IGeomecModelConstVisitor::VisitTBoundaryKtanMaterialParameter(const TBoundaryKtanMaterialParameter &value) {
  return VisitValueType(value);
}

bool IGeomecModelConstVisitor::VisitElementValueSet(const CElementValueSet &valueset) {
  return VisitIValueSet(valueset);
}

bool IGeomecModelConstVisitor::VisitIValueSet(const IValueSet &valueset) { return VisitStorageNode(valueset); }

bool IGeomecModelConstVisitor::VisitNodalValueSet(const CNodalValueSet &valueset) { return VisitIValueSet(valueset); }

bool IGeomecModelConstVisitor::VisitRpnValueSet(const CRpnValueSet &valueset) { return VisitIValueSet(valueset); }

bool IGeomecModelConstVisitor::VisitNewWellPath(const CNewWellPath &wellpath) { return VisitColorNode(wellpath); }

bool IGeomecModelConstVisitor::VisitNewWellPathEntry(const CNewWellPathEntry &entry) { return VisitGraphEntry(entry); }

bool IGeomecModelConstVisitor::VisitWellCasingCementInterfaceMaterialServer(
    const CWellCasingCementInterfaceMaterialServer &server) {
  return VisitIMaterialServerBase(server);
}

bool IGeomecModelConstVisitor::VisitWellCasingCementInterface(const CWellCasingCementInterface &interf) {
  return VisitOpenGLNode(interf);
}

bool IGeomecModelConstVisitor::VisitWellCasingNode(const CWellCasingNode &node) { return VisitOpenGLNode(node); }

bool IGeomecModelConstVisitor::VisitWellCasingRTCI(const CWellCasingRTCI &rtci) { return VisitColorNode(rtci); }

bool IGeomecModelConstVisitor::VisitWellCasingRTCIEntry(const CWellCasingRTCIEntry &entry) {
  return VisitGraphEntry(entry);
}

bool IGeomecModelConstVisitor::VisitZoominModelEntry(const CZoominModelEntry &entry) { return VisitGraphEntry(entry); }

bool IGeomecModelConstVisitor::VisitZoominModelPlaceHolder(const CZoominModelPlaceHolder &placeholder) {
  return VisitOpenGLNode(placeholder);
}

bool IGeomecModelConstVisitor::VisitGVTResultTree(const CGVTResultTree &tree) { return VisitResultGroup(tree); }

bool IGeomecModelConstVisitor::VisitGVTResultGroup(const CGVTResultGroup &group) { return VisitResultGroup(group); }

bool IGeomecModelConstVisitor::VisitGVTFactor(const CGVTFactor &composite) { return VisitIResult(composite); }

/////

IGeomecModelVisitor::IGeomecModelVisitor() {}

bool IGeomecModelVisitor::Visit3DFormation(C3DFormation &formation) { return VisitFormationBase(formation); }

bool IGeomecModelVisitor::Visit3DHorizon(C3DHorizon &horizon) { return VisitHorizonBase(horizon); }

bool IGeomecModelVisitor::Visit3DModel(C3DModel &model) { return VisitModelBase(model); }

bool IGeomecModelVisitor::Visit3DSupportNode(C3DSupportNode &node) { return VisitBaseSupportNode(node); }

bool IGeomecModelVisitor::VisitAnalysisPoint(CAnalysisPoint &point) { return VisitColorNode(point); }

bool IGeomecModelVisitor::VisitAnalysisPointEntry(CAnalysisPointEntry &entry) { return VisitGraphEntry(entry); }

bool IGeomecModelVisitor::VisitSupportDepletion(CSupportDepletion &supportdepletion) {
  return VisitStorageNode(supportdepletion);
}

bool IGeomecModelVisitor::VisitBaseSupportNode(CBaseSupportNode &node) { return VisitOpenGLNode(node); }

bool IGeomecModelVisitor::VisitBoundaryBase(CBoundaryBase &boundary) { return VisitColorNode(boundary); }

bool IGeomecModelVisitor::VisitBoundaryInterfaceDef(CBoundaryInterfaceDef &def) { return VisitStorageNode(def); }

bool IGeomecModelVisitor::VisitCamClayUpscalingNode(CCamClayUpscalingNode &node) {
  return VisitIUpscalingNodeBase(node);
}

bool IGeomecModelVisitor::VisitColorGradient(CColorGradient &gradient) { return VisitIColorScaleNode(gradient); }

bool IGeomecModelVisitor::VisitColorScaleEntry(CColorScaleEntry &entry) { return VisitIColorScaleEntry(entry); }

bool IGeomecModelVisitor::VisitCrossSection(CCrossSection &crosssection) { return VisitStorageNode(crosssection); }

bool IGeomecModelVisitor::VisitCrossSectionEntry(CCrossSectionEntry &entry) { return VisitGraphEntry(entry); }

bool IGeomecModelVisitor::VisitDepletionStage(CDepletionStage &stage) { return VisitStorageNode(stage); }

bool IGeomecModelVisitor::VisitDepletionStageEntry(CDepletionStageEntry &entry) { return VisitGraphEntry(entry); }

bool IGeomecModelVisitor::VisitDisplacementSupportNode(CDisplacementSupportNode &node) {
  return Visit3DSupportNode(node);
}

bool IGeomecModelVisitor::VisitIElementSet(IElementSet &elementset) { return VisitIPointSet(elementset); }

bool IGeomecModelVisitor::VisitElementSet(CElementSet &elementset) { return VisitIElementSet(elementset); }

bool IGeomecModelVisitor::VisitExportResultData(CExportResultData &data) { return VisitColorNode(data); }

bool IGeomecModelVisitor::VisitExportResultDataEntry(CExportResultDataEntry &entry) { return VisitGraphEntry(entry); }

bool IGeomecModelVisitor::VisitFaultParametersNode(CFaultParametersNode &node) { return VisitStorageNode(node); }

bool IGeomecModelVisitor::VisitFaultPressureComponent(CFaultPressureComponent &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelVisitor::VisitFaultPressure(CFaultPressure &pressure) { return VisitIValueComposite(pressure); }

bool IGeomecModelVisitor::VisitFaultPressureGradientComponent(CFaultPressureGradientComponent &component) {
  return VisitFaultPressureComponent(component);
}

bool IGeomecModelVisitor::VisitFaultPressureMinComponent(CFaultPressureMinComponent &component) {
  return VisitFaultPressureComponent(component);
}

bool IGeomecModelVisitor::VisitFaultPressureMaxComponent(CFaultPressureMaxComponent &component) {
  return VisitFaultPressureComponent(component);
}

bool IGeomecModelVisitor::VisitFaultPressureMaxGradientComponent(CFaultPressureMaxGradientComponent &component) {
  return VisitFaultPressureComponent(component);
}

bool IGeomecModelVisitor::VisitGocadElementSet(CGocadElementSet &elementset) { return VisitElementSet(elementset); }

bool IGeomecModelVisitor::VisitHomogenizationBox(CHomogenizationBox &box) { return VisitIScalingBox(box); }

bool IGeomecModelVisitor::VisitHomoBoxEntry(CHomoBoxEntry &entry) { return VisitGraphEntry(entry); }

bool IGeomecModelVisitor::VisitHorizonBase(CHorizonBase &horizon) { return VisitColorNode(horizon); }

bool IGeomecModelVisitor::VisitHotSpot(CHotSpot &hotspot) { return VisitIColorScaleNode(hotspot); }

bool IGeomecModelVisitor::VisitInterfaceBoundary(CInterfaceBoundary &boundary) { return VisitBoundaryBase(boundary); }

bool IGeomecModelVisitor::VisitIScalingBox(IScalingBox &box) { return VisitColorNode(box); }

bool IGeomecModelVisitor::VisitMeasuredTopDisplacementsNode(CMeasuredTopDisplacementsNode &node) {
  return VisitStorageNode(node);
}

bool IGeomecModelVisitor::VisitMeshObserver(CMeshObserver &observer) { return VisitGraphNode(observer); }

bool IGeomecModelVisitor::VisitPillarMap(CPillarMap &map) { return VisitColorNode(map); }

bool IGeomecModelVisitor::VisitNonMeshedSurface(CNonMeshedSurface &surface) { return VisitColorNode(surface); }

bool IGeomecModelVisitor::VisitNonMeshedSurfaceEntry(CNonMeshedSurfaceEntry &entry) { return VisitGraphEntry(entry); }

bool IGeomecModelVisitor::VisitNonMeshedSurfaceParametersNode(CNonMeshedSurfaceParametersNode &node) {
  return VisitStorageNode(node);
}

bool IGeomecModelVisitor::VisitNonMeshedSurfacePressureComponent(CNonMeshedSurfacePressureComponent &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelVisitor::VisitNonMeshedSurfacePressure(CNonMeshedSurfacePressure &pressure) {
  return VisitIValueComposite(pressure);
}

bool IGeomecModelVisitor::VisitFormationObserver(CFormationObserver &observer) { return VisitGraphNode(observer); }

bool IGeomecModelVisitor::VisitStrainLoadGradientComponent(CStrainLoadGradientComponent &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelVisitor::VisitStrainLoadGWCDummyComponent(CStrainLoadGWCDummyComponent &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelVisitor::VisitStrainLoadRepeaterComponent(CStrainLoadRepeaterComponent &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelVisitor::VisitStrainLoad(CStrainLoad &load) { return VisitIValueComposite(load); }

bool IGeomecModelVisitor::VisitThinLayerUpscalingNode(CThinLayerUpscalingNode &node) {
  return VisitIUpscalingNodeBase(node);
}

bool IGeomecModelVisitor::VisitIUpscalingNodeBase(IUpscalingNodeBase &node) { return VisitStorageNode(node); }

bool IGeomecModelVisitor::VisitUpscalingEntry(CUpscalingEntry &entry) { return VisitGraphEntry(entry); }

bool IGeomecModelVisitor::VisitUpscalingFormationsBranch(CUpscalingFormationsBranch &branch) {
  return VisitOpenGLNode(branch);
}

bool IGeomecModelVisitor::VisitIUpscalingPointsetBranchBase(IUpscalingPointsetBranchBase &branch) {
  return VisitOpenGLNode(branch);
}

bool IGeomecModelVisitor::VisitUpscalingPointsetRangesBranch(CUpscalingPointsetRangesBranch &branch) {
  return VisitIUpscalingPointsetBranchBase(branch);
}

bool IGeomecModelVisitor::VisitUpscalingTargetPointsetBranch(CUpscalingTargetPointsetBranch &branch) {
  return VisitIUpscalingPointsetBranchBase(branch);
}

bool IGeomecModelVisitor::VisitValueComponent(CValueComponent &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelVisitor::VisitVolumetricStrainLoadGradientComponent(
    CVolumetricStrainLoadGradientComponent &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelVisitor::VisitVolumetricStrainLoadGWCDummyComponent(
    CVolumetricStrainLoadGWCDummyComponent &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelVisitor::VisitVolumetricStrainLoadRepeaterComponent(
    CVolumetricStrainLoadRepeaterComponent &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelVisitor::VisitVolumetricStrainLoad(CVolumetricStrainLoad &load) { return VisitIValueComposite(load); }

bool IGeomecModelVisitor::VisitIFormationElementSet(IFormationElementSet &elementset) {
  return VisitIElementSet(elementset);
}

bool IGeomecModelVisitor::VisitFormationBase(CFormationBase &formation) { return VisitColorNode(formation); }

bool IGeomecModelVisitor::VisitFormationThickness(CFormationThickness &formationthickness) {
  return VisitIValueComposite(formationthickness);
}

bool IGeomecModelVisitor::VisitFormationPlane(CFormationPlane &plane) { return VisitColorNode(plane); }

bool IGeomecModelVisitor::VisitFormationVolume(CFormationVolume &volume) { return VisitIFormationElementSet(volume); }

bool IGeomecModelVisitor::VisitWellFormation(CWellFormation &formation) { return Visit3DFormation(formation); }

bool IGeomecModelVisitor::VisitWellFormationEntry(CWellFormationEntry &entry) { return VisitGraphEntry(entry); }

bool IGeomecModelVisitor::VisitWellFormationVolume(CWellFormationVolume &volume) {
  return VisitFormationVolume(volume);
}

bool IGeomecModelVisitor::VisitGoCadModel(CGoCadModel &model) { return VisitTetraModel(model); }

bool IGeomecModelVisitor::VisitGoCadSubModelEntry(CGoCadSubModelEntry &entry) { return VisitGraphEntry(entry); }

bool IGeomecModelVisitor::VisitHexaBoundary(CHexaBoundary &boundary) { return VisitInterfaceBoundary(boundary); }

bool IGeomecModelVisitor::VisitHexaFormationVolume(CHexaFormationVolume &volume) {
  return VisitFormationVolume(volume);
}

bool IGeomecModelVisitor::VisitHexaFormation(CHexaFormation &formation) { return Visit3DFormation(formation); }

bool IGeomecModelVisitor::VisitHexaFormationEntry(CHexaFormationEntry &entry) { return VisitGraphEntry(entry); }

bool IGeomecModelVisitor::VisitHexaHorizon(CHexaHorizon &horizon) { return Visit3DHorizon(horizon); }

bool IGeomecModelVisitor::VisitHexaHorizonEntry(CHexaHorizonEntry &entry) { return VisitGraphEntry(entry); }

bool IGeomecModelVisitor::VisitHexaMesh(CHexaMesh &mesh) { return VisitMeshBase(mesh); }

bool IGeomecModelVisitor::VisitHexaMeshRegionBase(CHexaMeshRegionBase &region) { return VisitMeshRegionBase(region); }

bool IGeomecModelVisitor::VisitHexaMainMeshRegion(CHexaMainMeshRegion &region) {
  return VisitHexaMeshRegionBase(region);
}

bool IGeomecModelVisitor::VisitHexaSubMeshRegion(CHexaSubMeshRegion &region) { return VisitHexaMeshRegionBase(region); }

bool IGeomecModelVisitor::VisitHexaMeshRegionEntry(CHexaMeshRegionEntry &entry) { return VisitGraphEntry(entry); }

bool IGeomecModelVisitor::VisitHexaModel(CHexaModel &model) { return Visit3DModel(model); }

bool IGeomecModelVisitor::VisitHexaSurface(CHexaSurface &surface) { return VisitSurfaceBase(surface); }

bool IGeomecModelVisitor::VisitIMaterial(IMaterial &material) { return VisitIMaterialBase(material); }

bool IGeomecModelVisitor::VisitIMaterialBase(IMaterialBase &material) { return VisitStorageNode(material); }

bool IGeomecModelVisitor::VisitIMaterialRock(IMaterialRock &material) { return VisitIMaterial(material); }

bool IGeomecModelVisitor::VisitIMaterialServerBase(IMaterialServerBase &server) { return VisitStorageNode(server); }

bool IGeomecModelVisitor::VisitInterfaceMaterial(CInterfaceMaterial &material) { return VisitIMaterialBase(material); }

bool IGeomecModelVisitor::VisitInterfaceMaterialEntry(CInterfaceMaterialEntry &entry) { return VisitGraphEntry(entry); }

bool IGeomecModelVisitor::VisitMaterialAnisotropicCamClay(CMaterialAnisotropicCamClay &material) {
  return VisitIMaterialRock(material);
}

bool IGeomecModelVisitor::VisitMaterialCamClay(CMaterialCamClay &material) { return VisitIMaterialRock(material); }

bool IGeomecModelVisitor::VisitMaterialCamClayCreep(CMaterialCamClayCreep &material) {
  return VisitIMaterialRock(material);
}

bool IGeomecModelVisitor::VisitMaterialCreep(CMaterialCreep &material) { return VisitIMaterialRock(material); }

bool IGeomecModelVisitor::VisitMaterialDualCapLinEla(CMaterialDualCapLinEla &material) {
  return VisitIMaterialRock(material);
}

bool IGeomecModelVisitor::VisitMaterialEntry(CMaterialEntry &entry) { return VisitGraphEntry(entry); }

bool IGeomecModelVisitor::VisitMaterialFractAnisotropy(CMaterialFractAnisotropy &material) {
  return VisitMaterialFractureApertureBase(material);
}

bool IGeomecModelVisitor::VisitMaterialFractureAperture(CMaterialFractureAperture &material) {
  return VisitMaterialFractureApertureBase(material);
}

bool IGeomecModelVisitor::VisitMaterialFractureAperture2(CMaterialFractureAperture2 &material) {
  return VisitMaterialFractureApertureBase(material);
}

bool IGeomecModelVisitor::VisitMaterialFractureApertureBase(CMaterialFractureApertureBase &material) {
  return VisitIMaterialRock(material);
}

bool IGeomecModelVisitor::VisitMaterialLinear(CMaterialLinear &material) { return VisitIMaterialRock(material); }

bool IGeomecModelVisitor::VisitMaterialMCCohesionHard1(CMaterialMCCohesionHard1 &material) {
  return VisitMaterialMohrCo(material);
}

bool IGeomecModelVisitor::VisitMaterialMCCohesionHard2(CMaterialMCCohesionHard2 &material) {
  return VisitMaterialMohrCo(material);
}

bool IGeomecModelVisitor::VisitMaterialMCCohesionHard3(CMaterialMCCohesionHard3 &material) {
  return VisitMaterialMohrCo(material);
}

bool IGeomecModelVisitor::VisitMaterialMCFrictionHard1(CMaterialMCFrictionHard1 &material) {
  return VisitMaterialMohrCo(material);
}

bool IGeomecModelVisitor::VisitMaterialMCFrictionHard2(CMaterialMCFrictionHard2 &material) {
  return VisitMaterialMohrCo(material);
}

bool IGeomecModelVisitor::VisitMaterialMCFrictionHard3(CMaterialMCFrictionHard3 &material) {
  return VisitMaterialMohrCo(material);
}

bool IGeomecModelVisitor::VisitMaterialModMohrCo(CMaterialModMohrCo &material) { return VisitIMaterialRock(material); }

bool IGeomecModelVisitor::VisitMaterialMohrCo(CMaterialMohrCo &material) { return VisitIMaterialRock(material); }

bool IGeomecModelVisitor::VisitMaterialResult(CMaterialResult &matresult) { return VisitIResult(matresult); }

bool IGeomecModelVisitor::VisitMaterialResultTree(CMaterialResultTree &tree) { return VisitResultGroup(tree); }

bool IGeomecModelVisitor::VisitMaterialRigidity(CMaterialRigidity &material) { return VisitIMaterialRock(material); }

bool IGeomecModelVisitor::VisitMaterialServer(CMaterialServer &server) { return VisitIMaterialServerBase(server); }

bool IGeomecModelVisitor::VisitMaterialUndrained(CMaterialUndrained &material) { return VisitIMaterialRock(material); }

bool IGeomecModelVisitor::VisitMaterialUpscaledAnisotropy(CMaterialUpscaledAnisotropy &material) {
  return VisitIMaterialRock(material);
}

bool IGeomecModelVisitor::VisitWellCasingCement(CWellCasingCement &cement) { return VisitFormationBase(cement); }

bool IGeomecModelVisitor::VisitWellCasingMaterialServer(CWellCasingMaterialServer &server) {
  return VisitIMaterialServerBase(server);
}

bool IGeomecModelVisitor::VisitWellCasingSteel(CWellCasingSteel &steel) { return VisitOpenGLNode(steel); }

bool IGeomecModelVisitor::VisitWellCasingSteelMaterial(CWellCasingSteelMaterial &material) {
  return VisitIMaterialBase(material);
}

bool IGeomecModelVisitor::VisitWellCasingSteelMaterialEntry(CWellCasingSteelMaterialEntry &entry) {
  return VisitGraphEntry(entry);
}

bool IGeomecModelVisitor::VisitIWellMesh(IWellMesh &mesh) { return VisitMeshBase(mesh); }

bool IGeomecModelVisitor::VisitMeshBase(CMeshBase &mesh) { return VisitIElementSet(mesh); }

bool IGeomecModelVisitor::VisitMeshRegionBase(CMeshRegionBase &region) { return VisitColorNode(region); }

bool IGeomecModelVisitor::VisitMeshResult(CMeshResult &result) { return VisitIValueComposite(result); }

bool IGeomecModelVisitor::VisitMeshResultTree(CMeshResultTree &tree) { return VisitGraphEntry(tree); }

bool IGeomecModelVisitor::VisitWellCasingMesh(CWellCasingMesh &mesh) { return VisitIWellMesh(mesh); }

bool IGeomecModelVisitor::VisitWellZoomInMesh(CWellZoomInMesh &mesh) { return VisitIWellMesh(mesh); }

bool IGeomecModelVisitor::VisitEclipseModel(CEclipseModel &model) { return VisitTetraModel(model); }

bool IGeomecModelVisitor::VisitEclipseSubModelEntry(CEclipseSubModelEntry &entry) { return VisitGraphEntry(entry); }

bool IGeomecModelVisitor::VisitIWellModel(IWellModel &model) { return VisitModelBase(model); }

bool IGeomecModelVisitor::VisitModelBase(CModelBase &model) { return VisitFemAppModel(model); }

bool IGeomecModelVisitor::VisitWellCasingModel(CWellCasingModel &model) { return VisitIWellModel(model); }

bool IGeomecModelVisitor::VisitWellCasingFormationEntry(CWellCasingFormationEntry &entry) {
  return VisitWellFormationEntry(entry);
}

bool IGeomecModelVisitor::VisitWellZoomInModel(CWellZoomInModel &model) { return VisitIWellModel(model); }

bool IGeomecModelVisitor::VisitIPointSet(IPointSet &pointset) { return VisitColorNode(pointset); }

bool IGeomecModelVisitor::VisitPointSetEntry(CPointSetEntry &entry) { return VisitGraphEntry(entry); }

bool IGeomecModelVisitor::VisitLasPointSet(CLasPointSet &pointset) { return VisitPointSet(pointset); }

bool IGeomecModelVisitor::VisitPointSet(CPointSet &pointset) { return VisitIPointSet(pointset); }

bool IGeomecModelVisitor::VisitFractureMatrixComponentGWC(CFractureMatrixComponentGWC &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelVisitor::VisitFractureMatrixComponentRepeater(CFractureMatrixComponentRepeater &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelVisitor::VisitFractureMatrixPressure(CFractureMatrixPressure &pressure) {
  return VisitIValueComposite(pressure);
}

bool IGeomecModelVisitor::VisitGlobalPressure(CGlobalPressure &pressure) { return VisitIValueComposite(pressure); }

bool IGeomecModelVisitor::VisitComponentGWC(CComponentGWC &component) { return VisitIValueComponentBase(component); }

bool IGeomecModelVisitor::VisitComponentRepeater(CComponentRepeater &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelVisitor::VisitPressure(CPressure &pressure) { return VisitIValueComposite(pressure); }

bool IGeomecModelVisitor::VisitPressureSupportNode(CPressureSupportNode &node) { return Visit3DSupportNode(node); }

bool IGeomecModelVisitor::VisitWellCasingInternalPressure(CWellCasingInternalPressure &pressure) {
  return VisitIValueComposite(pressure);
}

bool IGeomecModelVisitor::VisitDerivedResult(CDerivedResult &result) { return VisitIResult(result); }

bool IGeomecModelVisitor::VisitDerivedResultGroup(CDerivedResultGroup &group) { return VisitResultGroup(group); }

bool IGeomecModelVisitor::VisitIVectorResult(IVectorResult &result) { return VisitIResult(result); }

bool IGeomecModelVisitor::VisitVectorResult(CVectorResult &result) { return VisitIVectorResult(result); }

bool IGeomecModelVisitor::VisitResultGroup(CResultGroup &group) { return VisitGraphNode(group); }

bool IGeomecModelVisitor::VisitIResult(IResult &result) { return VisitIValueComposite(result); }

bool IGeomecModelVisitor::VisitIResultComponent(IResultComponent &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelVisitor::VisitIScalarResultComponent(IScalarResultComponent &component) {
  return VisitIResultComponent(component);
}

bool IGeomecModelVisitor::VisitResultTree(CResultTree &tree) { return VisitResultGroup(tree); }

bool IGeomecModelVisitor::VisitStaborResult(CStaborResult &result) { return VisitIResult(result); }

bool IGeomecModelVisitor::VisitGVTSettings(CGVTSettings &settings) { return VisitStorageNode(settings); }

bool IGeomecModelVisitor::VisitQBSettings(CQBSettings &settings) { return VisitStorageNode(settings); }

bool IGeomecModelVisitor::VisitSurfaceBase(CSurfaceBase &surface) { return VisitIElementSet(surface); }

bool IGeomecModelVisitor::VisitSurfaceEntry(CSurfaceEntry &entry) { return VisitGraphEntry(entry); }

bool IGeomecModelVisitor::VisitCalculatedTemperature(CCalculatedTemperature &temperature) {
  return VisitIValueComposite(temperature);
}

bool IGeomecModelVisitor::VisitGlobalTemperature(CGlobalTemperature &temperature) {
  return VisitIValueComposite(temperature);
}

bool IGeomecModelVisitor::VisitTemperatureGradientComponent(CTemperatureGradientComponent &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelVisitor::VisitTemperatureGWCDummyComponent(CTemperatureGWCDummyComponent &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelVisitor::VisitTemperatureRepeaterComponent(CTemperatureRepeaterComponent &component) {
  return VisitIValueComponentBase(component);
}

bool IGeomecModelVisitor::VisitTemperature(CTemperature &temperature) { return VisitIValueComposite(temperature); }

bool IGeomecModelVisitor::VisitWellCasingInternalTemperature(CWellCasingInternalTemperature &temperature) {
  return VisitIValueComposite(temperature);
}

bool IGeomecModelVisitor::VisitFractTensor(CFractTensor &tensor) { return VisitMatParamsTensor(tensor); }

bool IGeomecModelVisitor::VisitIStrainTensorGroup(IStrainTensorGroup &group) { return VisitITensorGroup(group); }

bool IGeomecModelVisitor::VisitIStressTensorGroup(IStressTensorGroup &group) { return VisitITensorGroup(group); }

bool IGeomecModelVisitor::VisitITensorGroup(ITensorGroup &group) { return VisitResultGroup(group); }

bool IGeomecModelVisitor::VisitMatParamsTensor(CMatParamsTensor &tensor) { return VisitValueType(tensor); }

bool IGeomecModelVisitor::VisitTStressTensor(TStressTensor &tensor) { return VisitValueType(tensor); }

bool IGeomecModelVisitor::VisitTStrainTensor(TStrainTensor &tensor) { return VisitValueType(tensor); }

bool IGeomecModelVisitor::VisitTetraBoundary(CTetraBoundary &boundary) { return VisitInterfaceBoundary(boundary); }

bool IGeomecModelVisitor::VisitTetraFormationVolume(CTetraFormationVolume &volume) {
  return VisitFormationVolume(volume);
}

bool IGeomecModelVisitor::VisitTetraFormation(CTetraFormation &formation) { return Visit3DFormation(formation); }

bool IGeomecModelVisitor::VisitTetraFormationEntry(CTetraFormationEntry &entry) { return VisitGraphEntry(entry); }

bool IGeomecModelVisitor::VisitTetraHorizonBase(CTetraHorizonBase &horizon) { return Visit3DHorizon(horizon); }

bool IGeomecModelVisitor::VisitTetraMesh(CTetraMesh &mesh) { return VisitMeshBase(mesh); }

bool IGeomecModelVisitor::VisitTetraModel(CTetraModel &model) { return Visit3DModel(model); }

bool IGeomecModelVisitor::VisitTetraSubBoundary(CTetraSubBoundary &boundary) { return VisitColorNode(boundary); }

bool IGeomecModelVisitor::VisitTetraSubHorizon(CTetraSubHorizon &horizon) { return VisitTetraHorizonBase(horizon); }

bool IGeomecModelVisitor::VisitTetraSubHorizonEntry(CTetraSubHorizonEntry &entry) { return VisitGraphEntry(entry); }

bool IGeomecModelVisitor::VisitTetraSuperHorizon(CTetraSuperHorizon &horizon) {
  return VisitTetraSuperHorizon(horizon);
}

bool IGeomecModelVisitor::VisitTetraSuperHorizonEntry(CTetraSuperHorizonEntry &entry) { return VisitGraphEntry(entry); }

bool IGeomecModelVisitor::VisitTetraSurface(CTetraSurface &surface) { return VisitSurfaceBase(surface); }

bool IGeomecModelVisitor::VisitGlobalInitialStressNode(CGlobalInitialStressNode &node) {
  return VisitStorageNode(node);
}

bool IGeomecModelVisitor::VisitResultRegister(CResultRegister &resultregister) {
  return VisitStorageNode(resultregister);
}

bool IGeomecModelVisitor::VisitTStrain(TStrain &strain) { return VisitValueType(strain); }

bool IGeomecModelVisitor::VisitValueType(CValueType &valuetype) { return VisitIValueComposite(valuetype); }

bool IGeomecModelVisitor::VisitTLengthComponent(TLengthComponent &component) { return VisitValueComponent(component); }

bool IGeomecModelVisitor::VisitSurfaceValueType(CSurfaceValueType &valuetype) { return VisitValueType(valuetype); }

bool IGeomecModelVisitor::VisitTPressure(TPressure &pressure) { return VisitValueType(pressure); }

bool IGeomecModelVisitor::VisitTFractureMatrixPressure(TFractureMatrixPressure &pressure) {
  return VisitValueType(pressure);
}

bool IGeomecModelVisitor::VisitTTemperature(TTemperature &temperature) { return VisitValueType(temperature); }

bool IGeomecModelVisitor::VisitTVolumetricStrain(TVolumetricStrain &strain) { return VisitValueType(strain); }

bool IGeomecModelVisitor::VisitTNormalStrain(TNormalStrain &strain) { return VisitValueType(strain); }

bool IGeomecModelVisitor::VisitTLateralStrain(TLateralStrain &strain) { return VisitValueType(strain); }

bool IGeomecModelVisitor::VisitTCohesion(TCohesion &cohesion) { return VisitValueType(cohesion); }

bool IGeomecModelVisitor::VisitTBulkStiffness(TBulkStiffness &stiffness) { return VisitValueType(stiffness); }

bool IGeomecModelVisitor::VisitTShearModulus(TShearModulus &modulus) { return VisitValueType(modulus); }

bool IGeomecModelVisitor::VisitTVelocityP(TVelocityP &vp) { return VisitValueType(vp); }

bool IGeomecModelVisitor::VisitTVelocityS(TVelocityS &vs) { return VisitValueType(vs); }

bool IGeomecModelVisitor::VisitTFrictionAngle(TFrictionAngle &angle) { return VisitValueType(angle); }

bool IGeomecModelVisitor::VisitTPoissonsRatio(TPoissonsRatio &ratio) { return VisitValueType(ratio); }

bool IGeomecModelVisitor::VisitTYoungsModulus(TYoungsModulus &modulus) { return VisitValueType(modulus); }

bool IGeomecModelVisitor::VisitTDensity(TDensity &density) { return VisitValueType(density); }

bool IGeomecModelVisitor::VisitTPorosity(TPorosity &porosity) { return VisitValueType(porosity); }

bool IGeomecModelVisitor::VisitTInitFriction(TInitFriction &friction) { return VisitValueType(friction); }

bool IGeomecModelVisitor::VisitTHardening(THardening &hardening) { return VisitValueType(hardening); }

bool IGeomecModelVisitor::VisitTPreConsolidation(TPreConsolidation &preconsolidation) {
  return VisitValueType(preconsolidation);
}

bool IGeomecModelVisitor::VisitTCapShape(TCapShape &capshape) { return VisitValueType(capshape); }

bool IGeomecModelVisitor::VisitTDilatation(TDilatation &dilatation) { return VisitValueType(dilatation); }

bool IGeomecModelVisitor::VisitTLatRatioMax(TLatRatioMax &ratio) { return VisitValueType(ratio); }

bool IGeomecModelVisitor::VisitTLatRatioMin(TLatRatioMin &ratio) { return VisitValueType(ratio); }

bool IGeomecModelVisitor::VisitTAzimuth(TAzimuth &azimuth) { return VisitValueType(azimuth); }

bool IGeomecModelVisitor::VisitTCreepA1(TCreepA1 &a1) { return VisitValueType(a1); }

bool IGeomecModelVisitor::VisitTCreepN1(TCreepN1 &n1) { return VisitValueType(n1); }

bool IGeomecModelVisitor::VisitTCreepA2(TCreepA2 &a2) { return VisitValueType(a2); }

bool IGeomecModelVisitor::VisitTCreepN2(TCreepN2 &n2) { return VisitValueType(n2); }

bool IGeomecModelVisitor::VisitTFluidBulkModulus(TFluidBulkModulus &modulus) { return VisitValueType(modulus); }

bool IGeomecModelVisitor::VisitTHardCohesion1(THardCohesion1 &cohesion) { return VisitValueType(cohesion); }

bool IGeomecModelVisitor::VisitTHardCohesion2(THardCohesion2 &cohesion) { return VisitValueType(cohesion); }

bool IGeomecModelVisitor::VisitTHardCohesion3(THardCohesion3 &cohesion) { return VisitValueType(cohesion); }

bool IGeomecModelVisitor::VisitTHardFriction1(THardFriction1 &friction) { return VisitValueType(friction); }

bool IGeomecModelVisitor::VisitTHardFriction2(THardFriction2 &friction) { return VisitValueType(friction); }

bool IGeomecModelVisitor::VisitTHardFriction3(THardFriction3 &friction) { return VisitValueType(friction); }

bool IGeomecModelVisitor::VisitTEquivalentPlasticStrain1(TEquivalentPlasticStrain1 &strain) {
  return VisitValueType(strain);
}

bool IGeomecModelVisitor::VisitTEquivalentPlasticStrain2(TEquivalentPlasticStrain2 &strain) {
  return VisitValueType(strain);
}

bool IGeomecModelVisitor::VisitTEquivalentPlasticStrain3(TEquivalentPlasticStrain3 &strain) {
  return VisitValueType(strain);
}

bool IGeomecModelVisitor::VisitTCreepQR1(TCreepQR1 &qr1) { return VisitValueType(qr1); }

bool IGeomecModelVisitor::VisitTCreepQR2(TCreepQR2 &qr2) { return VisitValueType(qr2); }

bool IGeomecModelVisitor::VisitTCreepAlpha(TCreepAlpha &alpha) { return VisitValueType(alpha); }

bool IGeomecModelVisitor::VisitTTensileStretch(TTensileStretch &tensilestretch) {
  return VisitValueType(tensilestretch);
}

bool IGeomecModelVisitor::VisitTThermalExpansion(TThermalExpansion &expansion) { return VisitValueType(expansion); }

bool IGeomecModelVisitor::VisitTRigidParam1(TRigidParam1 &param) { return VisitValueType(param); }

bool IGeomecModelVisitor::VisitTRigidParam2(TRigidParam2 &param) { return VisitValueType(param); }

bool IGeomecModelVisitor::VisitTRigidParam3(TRigidParam3 &param) { return VisitValueType(param); }

bool IGeomecModelVisitor::VisitTRigidParam4(TRigidParam4 &param) { return VisitValueType(param); }

bool IGeomecModelVisitor::VisitTRigidParam5(TRigidParam5 &param) { return VisitValueType(param); }

bool IGeomecModelVisitor::VisitTRigidParam6(TRigidParam6 &param) { return VisitValueType(param); }

bool IGeomecModelVisitor::VisitTRigidParam7(TRigidParam7 &param) { return VisitValueType(param); }

bool IGeomecModelVisitor::VisitTRigidParam8(TRigidParam8 &param) { return VisitValueType(param); }

bool IGeomecModelVisitor::VisitTRigidParam9(TRigidParam9 &param) { return VisitValueType(param); }

bool IGeomecModelVisitor::VisitTRigidParam10(TRigidParam10 &param) { return VisitValueType(param); }

bool IGeomecModelVisitor::VisitTRigidParam11(TRigidParam11 &param) { return VisitValueType(param); }

bool IGeomecModelVisitor::VisitTRigidParam12(TRigidParam12 &param) { return VisitValueType(param); }

bool IGeomecModelVisitor::VisitTRigidParam13(TRigidParam13 &param) { return VisitValueType(param); }

bool IGeomecModelVisitor::VisitTRigidParam14(TRigidParam14 &param) { return VisitValueType(param); }

bool IGeomecModelVisitor::VisitTRigidParam15(TRigidParam15 &param) { return VisitValueType(param); }

bool IGeomecModelVisitor::VisitTRigidParam16(TRigidParam16 &param) { return VisitValueType(param); }

bool IGeomecModelVisitor::VisitTRigidParam17(TRigidParam17 &param) { return VisitValueType(param); }

bool IGeomecModelVisitor::VisitTRigidParam18(TRigidParam18 &param) { return VisitValueType(param); }

bool IGeomecModelVisitor::VisitTRigidParam19(TRigidParam19 &param) { return VisitValueType(param); }

bool IGeomecModelVisitor::VisitTRigidParam20(TRigidParam20 &param) { return VisitValueType(param); }

bool IGeomecModelVisitor::VisitTRigidParam21(TRigidParam21 &param) { return VisitValueType(param); }

bool IGeomecModelVisitor::VisitTElasticHardening(TElasticHardening &hardening) { return VisitValueType(hardening); }

bool IGeomecModelVisitor::VisitTSecondaryPreconsolidation(TSecondaryPreconsolidation &precon) {
  return VisitValueType(precon);
}

bool IGeomecModelVisitor::VisitTSecondaryHardening(TSecondaryHardening &hardening) { return VisitValueType(hardening); }

bool IGeomecModelVisitor::VisitTUniaxialElasticCompressibility(TUniaxialElasticCompressibility &comp) {
  return VisitValueType(comp);
}

bool IGeomecModelVisitor::VisitTUniaxialPlasticCompressibility(TUniaxialPlasticCompressibility &comp) {
  return VisitValueType(comp);
}

bool IGeomecModelVisitor::VisitTYoungModulusNormal(TYoungModulusNormal &modulus) { return VisitValueType(modulus); }

bool IGeomecModelVisitor::VisitTYoungModulusTransverse(TYoungModulusTransverse &modulus) {
  return VisitValueType(modulus);
}

bool IGeomecModelVisitor::VisitTPoissonRatioNormal(TPoissonRatioNormal &ratio) { return VisitValueType(ratio); }

bool IGeomecModelVisitor::VisitTPoissonRatioTransverse(TPoissonRatioTransverse &ratio) { return VisitValueType(ratio); }

bool IGeomecModelVisitor::VisitTLayerNormalIncl(TLayerNormalIncl &incl) { return VisitValueType(incl); }

bool IGeomecModelVisitor::VisitTLayerNormalAzi(TLayerNormalAzi &azi) { return VisitValueType(azi); }

bool IGeomecModelVisitor::VisitTFractStiffNormal(TFractStiffNormal &stiff) { return VisitValueType(stiff); }

bool IGeomecModelVisitor::VisitTFractStiffShear(TFractStiffShear &stiff) { return VisitValueType(stiff); }

bool IGeomecModelVisitor::VisitTThermLinExpNormal(TThermLinExpNormal &exp) { return VisitValueType(exp); }

bool IGeomecModelVisitor::VisitTThermLinExpLateral(TThermLinExpLateral &exp) { return VisitValueType(exp); }

bool IGeomecModelVisitor::VisitTThermalConductivity(TThermalConductivity &conduc) { return VisitValueType(conduc); }

bool IGeomecModelVisitor::VisitTThermalCapacity(TThermalCapacity &capacity) { return VisitValueType(capacity); }

bool IGeomecModelVisitor::VisitTHighFractDens(THighFractDens &dens) { return VisitValueType(dens); }

bool IGeomecModelVisitor::VisitTHighFractDensIncl(THighFractDensIncl &incl) { return VisitValueType(incl); }

bool IGeomecModelVisitor::VisitTHighFractDensAzi(THighFractDensAzi &azi) { return VisitValueType(azi); }

bool IGeomecModelVisitor::VisitTLowFractDens(TLowFractDens &dens) { return VisitValueType(dens); }

bool IGeomecModelVisitor::VisitTLowFractDensIncl(TLowFractDensIncl &incl) { return VisitValueType(incl); }

bool IGeomecModelVisitor::VisitTLowFractDensAzi(TLowFractDensAzi &azi) { return VisitValueType(azi); }

bool IGeomecModelVisitor::VisitTInterFractDens(TInterFractDens &dens) { return VisitValueType(dens); }

bool IGeomecModelVisitor::VisitTPermeability(TPermeability &perm) { return VisitValueType(perm); }

bool IGeomecModelVisitor::VisitTViscosity(TViscosity &viscosity) { return VisitValueType(viscosity); }

bool IGeomecModelVisitor::VisitTYoungsModulusDecompaction(TYoungsModulusDecompaction &modulus) {
  return VisitValueType(modulus);
}

bool IGeomecModelVisitor::VisitTPoissonsRatioDecompaction(TPoissonsRatioDecompaction &ratio) {
  return VisitValueType(ratio);
}

bool IGeomecModelVisitor::VisitTFluidThermalExpansionCoefficient(TFluidThermalExpansionCoefficient &coefficient) {
  return VisitValueType(coefficient);
}

bool IGeomecModelVisitor::VisitTFluidDensity(TFluidDensity &density) { return VisitValueType(density); }

bool IGeomecModelVisitor::VisitTReferenceStress(TReferenceStress &refstress) { return VisitValueType(refstress); }

bool IGeomecModelVisitor::VisitTApertureParameter(TApertureParameter &param) { return VisitValueType(param); }

bool IGeomecModelVisitor::VisitTGrainStiffnessParameter(TGrainStiffnessParameter &param) {
  return VisitValueType(param);
}

bool IGeomecModelVisitor::VisitTYoungsModulusNormalDecomp(TYoungsModulusNormalDecomp &modulus) {
  return VisitValueType(modulus);
}

bool IGeomecModelVisitor::VisitTYoungModulusTransDecomp(TYoungModulusTransDecomp &modulus) {
  return VisitValueType(modulus);
}

bool IGeomecModelVisitor::VisitTPoissonRatioNormalDecomp(TPoissonRatioNormalDecomp &ratio) {
  return VisitValueType(ratio);
}

bool IGeomecModelVisitor::VisitTPoissonRatioTransDecomp(TPoissonRatioTransDecomp &ratio) {
  return VisitValueType(ratio);
}

bool IGeomecModelVisitor::VisitTAnisotropicShearModulus(TAnisotropicShearModulus &modulus) {
  return VisitValueType(modulus);
}

bool IGeomecModelVisitor::VisitTAnisotropicShearModulusDecomp(TAnisotropicShearModulusDecomp &modulus) {
  return VisitValueType(modulus);
}

bool IGeomecModelVisitor::VisitTThomsenEpsilon(TThomsenEpsilon &eps) { return VisitValueType(eps); }

bool IGeomecModelVisitor::VisitTThomsenGamma(TThomsenGamma &gamma) { return VisitValueType(gamma); }

bool IGeomecModelVisitor::VisitTThomsenDelta(TThomsenDelta &delta) { return VisitValueType(delta); }

bool IGeomecModelVisitor::VisitTInitialFractureApertureHigh(TInitialFractureApertureHigh &ap) {
  return VisitValueType(ap);
}

bool IGeomecModelVisitor::VisitTInitialFractureApertureInter(TInitialFractureApertureInter &ap) {
  return VisitValueType(ap);
}

bool IGeomecModelVisitor::VisitTInitialFractureApertureLow(TInitialFractureApertureLow &ap) {
  return VisitValueType(ap);
}

bool IGeomecModelVisitor::VisitTReferenceAperture(TReferenceAperture &ap) { return VisitValueType(ap); }

bool IGeomecModelVisitor::VisitTDynamicUniaxialStiffness(TDynamicUniaxialStiffness &stiffness) {
  return VisitValueType(stiffness);
}

bool IGeomecModelVisitor::VisitTDynamicShearStiffness(TDynamicShearStiffness &stiffness) {
  return VisitValueType(stiffness);
}

bool IGeomecModelVisitor::VisitTDisplacementVector(TDisplacementVector &vector) { return VisitValueType(vector); }

bool IGeomecModelVisitor::VisitTNormalStress(TNormalStress &value) { return VisitValueType(value); }

bool IGeomecModelVisitor::VisitTMeanStress(TMeanStress &value) { return VisitValueType(value); }

bool IGeomecModelVisitor::VisitTTimeStrain(TTimeStrain &value) { return VisitValueType(value); }

bool IGeomecModelVisitor::VisitTDeltaV(TDeltaV &value) { return VisitValueType(value); }

bool IGeomecModelVisitor::VisitTDeltaT(TDeltaT &value) { return VisitValueType(value); }

bool IGeomecModelVisitor::VisitTDisplacementZ(TDisplacementZ &value) { return VisitValueType(value); }

bool IGeomecModelVisitor::VisitTDummyMaterialParameter(TDummyMaterialParameter &value) { return VisitValueType(value); }

bool IGeomecModelVisitor::VisitTBoundaryKradMaterialParameter(TBoundaryKradMaterialParameter &value) {
  return VisitValueType(value);
}

bool IGeomecModelVisitor::VisitTBoundaryKtanMaterialParameter(TBoundaryKtanMaterialParameter &value) {
  return VisitValueType(value);
}

bool IGeomecModelVisitor::VisitElementValueSet(CElementValueSet &valueset) { return VisitIValueSet(valueset); }

bool IGeomecModelVisitor::VisitIValueSet(IValueSet &valueset) { return VisitStorageNode(valueset); }

bool IGeomecModelVisitor::VisitNodalValueSet(CNodalValueSet &valueset) { return VisitIValueSet(valueset); }

bool IGeomecModelVisitor::VisitRpnValueSet(CRpnValueSet &valueset) { return VisitIValueSet(valueset); }

bool IGeomecModelVisitor::VisitNewWellPath(CNewWellPath &wellpath) { return VisitColorNode(wellpath); }

bool IGeomecModelVisitor::VisitNewWellPathEntry(CNewWellPathEntry &entry) { return VisitGraphEntry(entry); }

bool IGeomecModelVisitor::VisitWellCasingCementInterfaceMaterialServer(
    CWellCasingCementInterfaceMaterialServer &server) {
  return VisitIMaterialServerBase(server);
}

bool IGeomecModelVisitor::VisitWellCasingCementInterface(CWellCasingCementInterface &interf) {
  return VisitOpenGLNode(interf);
}

bool IGeomecModelVisitor::VisitWellCasingNode(CWellCasingNode &node) { return VisitOpenGLNode(node); }

bool IGeomecModelVisitor::VisitWellCasingRTCI(CWellCasingRTCI &rtci) { return VisitColorNode(rtci); }

bool IGeomecModelVisitor::VisitWellCasingRTCIEntry(CWellCasingRTCIEntry &entry) { return VisitGraphEntry(entry); }

bool IGeomecModelVisitor::VisitZoominModelEntry(CZoominModelEntry &entry) { return VisitGraphEntry(entry); }

bool IGeomecModelVisitor::VisitZoominModelPlaceHolder(CZoominModelPlaceHolder &placeholder) {
  return VisitOpenGLNode(placeholder);
}

bool IGeomecModelVisitor::VisitGVTResultTree(CGVTResultTree &tree) { return VisitResultGroup(tree); }

bool IGeomecModelVisitor::VisitGVTResultGroup(CGVTResultGroup &group) { return VisitResultGroup(group); }

bool IGeomecModelVisitor::VisitGVTFactor(CGVTFactor &composite) { return VisitIResult(composite); }
