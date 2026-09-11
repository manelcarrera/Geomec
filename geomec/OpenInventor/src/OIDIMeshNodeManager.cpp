#include "stdafx.h"

#include <memory>
#include <typeinfo>

#include "OIDIMeshNodeManager.h"
#include "OpenInventorDataInterface.h"
#include "OpenInventorEventsHandler.h"
#include "SoGroupIterator.h"

#include "CrossSection.h"
#include "CrossSectionManipulator.h"
#include "LegendScene.h"
#include "ValueTracker.h"

#include "LegendView.h"

#include "CrossSectionGroup.h"
#include "MeshCrossSection.h"
#include "MeshPointProbeGroup.h"
#include "OIVWellPath.h"

#include "OIDILineMeshNode.h"
#include "OIDISurfaceMeshNode.h"
#include "OIDISurfaceMeshOutlineNode.h"
#include "OIDIVolumeMeshNode.h"

#include "HexaMesh.h"
#include "IColorScaleNode.h"
#include "ITensorGroup.h"
#include "IVectorResult.h"
#include "OpenGLNode_Delegate.h"
#include "TetraMesh.h"

#include "ColorGradient.h"
#include "ColorScaleEntry.h"
#include "GeomecDoc.h"
#include "HotSpot.h"
#include "MeshResultTree.h"

#include "ModifiedHint.h"
// View/TensorViz
#include "MoTensorSet.h"

#include <MeshVizXLM/mesh/data/MiDataSetI.h>

#include <MeshVizXLM/mapping/nodes/MoDrawStyle.h>
#include <MeshVizXLM/mapping/nodes/MoLevelColorMapping.h>
#include <MeshVizXLM/mapping/nodes/MoLinearColorMapping.h>
#include <MeshVizXLM/mapping/nodes/MoMaterial.h>
#include <MeshVizXLM/mapping/nodes/MoMeshPlaneSlice.h>
#include <MeshVizXLM/mapping/nodes/MoScalarSetI.h>
#include <MeshVizXLM/mapping/nodes/MoVec3SetI.h>

#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoSwitch.h>

#include "IProgressBase.h"
#include "IProgressFactory.h"
#include "Inventor/nodes/SoMaterial.h"
#include "MeshNodeSettings.h"
#include "MeshVizXLM/extractors/MiPointProbeUnstructured.h"
#include "ModelView.h"
#include "NewWellPath.h"
#include "ValueMapper.h"
#include "WellCasingCementInterface.h"
#include "WellCasingModel.h"
#include "hexameshregion.h"

#include "Algebra.h"
#include "Printer.h" //'print_bounding_box' TODO: remove
#include "RBox.h"

#include "Global.h"
#include "IProgressFactory.h"

void print_bounding_box(char *cp, SbBox3f box) {
  return; // FIXME
  SbVec3f min = box.getMin();
  SbVec3f max = box.getMax();
  Printer::instance()->debug("bbox: %s: min(%f,%f,%f) max(%f,%f,%f)", cp, min[0], min[1], min[2], max[0], max[1],
                             max[2]);
}

MoColorMapping *OIDIMeshNodeManager::s_tensorVectorColorMapping = 0;

OIDIScalarSetI *CollectValues(const OIDIMesh *mesh, const IValueComponentBase *pValueComponent,
                              IQuantityDouble::UNIT unit, bool collectByTopology, IProgressBase &progress);
OIDIVectorSetI *CollectVectors(const OIDIMesh *mesh, const IVectorResult::IVectorComponent *pVectorComponent,
                               IQuantityDouble::UNIT unit, bool collectByTopology, IProgressBase &progress);
OIDITensorSetI *CollectTensors(const OIDIMesh *mesh, const IValueComponentBase *pTensorComponent,
                               IQuantityDouble::UNIT unit, bool collectByTopology, IProgressBase &progress);

MoColorMapping *CreateColorMapping(double min, double max, const CColorScaleEntry *colorScaleEntry);

void DecomposeQColor(QRgb color, float &red, float &green, float &blue) {
  red = qRed(color) / 255.0f;
  green = qGreen(color) / 255.0f;
  blue = qBlue(color) / 255.0f;
}

OIDIMeshNodeManager::OIDIMeshNodeManager(COpenInventorEventsHandler *eventsHandler)
    : m_openInventorEventsHandler(eventsHandler), m_inView(false), m_showTracker(false), m_crossSectionManipSwitch(0),
      m_crossSectionManip(0), m_currentCrossSection(0), m_valueTracker(0), m_elementTrackerSelected(false),
      m_draggerTrackerSelected(false), m_mouseTrackerSelected(false), m_legendScene(0), m_legendSceneSwitch(0),
      m_clipPlaneModeSelected(false), m_scale(0), m_lightModel(0), m_valueColorMap(0), m_geologyColorMap(0),
      m_geologyOrValueSwitch(0), m_valueScalarSetGroup(0), m_vectorSetGroup(0), m_tensorSetGroup(0),
      m_displayedMeshes(0), m_meshes(0), m_wellPaths(0), m_wellPathFont(0), m_pValueComponentDisplayed(0),
      m_pIsoValueComponentDisplayed(0), m_pickStyle(0), m_meshNodeSettings(new MeshNodeSettings), m_showTopView(false),
      m_pointSetCount(0), m_highlightedNode(0), m_highlightedMesh(0), m_translation(0, 0, 0) {
  renderCaching = false; // renderCaching interferes with MoMeshTensor rendering
  setName("OIDIMeshNodeManager");

  m_scale = new SoScale;
  m_scale->scaleFactor.setValue(1, 1, 1);
  addChild(m_scale);

  m_lightModel = new SoLightModel;
  addChild(m_lightModel);

  // Improve the lighting of planes (backside not black).
  SoShapeHints *shapeHints = new SoShapeHints;
  addChild(shapeHints);
  shapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;

  m_geologyOrValueSwitch = new SoSwitch;
  m_geologyOrValueSwitch->setName("ColorMapGeologyOrValueSwitch");
  addChild(m_geologyOrValueSwitch);

  m_geologyOrValueSwitch->whichChild = 0;

  m_geologyGroup = new SoGroup;
  m_geologyGroup->setName("GeologyGroup");
  m_geologyOrValueSwitch->addChild(m_geologyGroup);

  m_valueGroup = new SoGroup;
  m_valueGroup->setName("ValueGroup");
  m_geologyOrValueSwitch->addChild(m_valueGroup);

  m_geologyColorMap = new MoLevelColorMapping;
  m_geologyColorMap->setName("GeologyColorMap");
  m_geologyGroup->addChild(m_geologyColorMap);

  m_valueColorMap = new MoLinearColorMapping;
  m_valueColorMap->setName("ValueColorMap");
  m_valueGroup->addChild(m_valueColorMap);

  m_valueScalarSetGroup = new SoGroup;
  m_valueScalarSetGroup->setName("ScalarSetGroup");

  m_valueGroup->addChild(m_valueScalarSetGroup);

  m_vectorSetGroup = new SoGroup;
  m_vectorSetGroup->setName("VectorSetGroup");
  m_valueGroup->addChild(m_vectorSetGroup);

  m_tensorSetGroup = new SoGroup;
  m_tensorSetGroup->setName("TensorSetGroup");
  m_valueGroup->addChild(m_tensorSetGroup);

  initCrossSectionManip(); // sets m_crossSectionManip which is used by the value tracker as well.

  // We want everything but our draggers/manipulators to be unpickable
  m_pickStyle = new SoPickStyle;
  m_pickStyle->style = SoPickStyle::UNPICKABLE;
  addChild(m_pickStyle);

  // Put mesh nodes under a separator, so the clipping planes don't clip anything
  // other than meshes.
  m_meshes = new SoSeparator;
  m_meshes->renderCaching = false; // renderCaching interferes with MoMeshTensor rendering
  m_meshes->setName("meshes");

  m_xsecGroup = new CrossSectionGroup(this);
  m_meshes->addChild(m_xsecGroup);

  m_meshPointProbeGroup = new MeshPointProbeGroup;
  m_meshes->addChild(m_meshPointProbeGroup);

  m_displayedMeshes = new SoGroup;
  m_displayedMeshes->setName("displayedMeshes");
  m_meshes->addChild(m_displayedMeshes);
  addChild(m_meshes);

  SoPickStyle *pickableStyle = new SoPickStyle;
  pickableStyle->style = SoPickStyle::SHAPE;
  addChild(pickableStyle);

  m_wellPaths = new SoSeparator;
  m_wellPaths->setName("WellPaths");

  m_wellPathFont = new SoFont;
  m_wellPathFont->name = "Arial";
  m_wellPathFont->size = 18;
  m_wellPaths->addChild(m_wellPathFont);

  addChild(m_wellPaths);

  // keep these at the end as they clear the opengl depth buffer ...
  m_valueTracker = new ValueTracker(this, m_crossSectionManip);
  addChild(m_valueTracker);

  m_legendSceneSwitch = new SoSwitch;
  addChild(m_legendSceneSwitch);
  m_legendSceneSwitch->whichChild = SO_SWITCH_ALL;

  m_legendScene = new LegendScene;
  m_legendSceneSwitch->addChild(m_legendScene);
}

OIDIMeshNodeManager::~OIDIMeshNodeManager() { delete m_meshNodeSettings; }

void OIDIMeshNodeManager::EnterView() {
  m_inView = true;
  if (m_showTracker)
    m_crossSectionManipSwitch->whichChild = SO_SWITCH_ALL;
}

void OIDIMeshNodeManager::LeaveView() {
  m_inView = false;
  if (m_showTracker)
    m_crossSectionManipSwitch->whichChild = SO_SWITCH_NONE;
}

SoScale *OIDIMeshNodeManager::getScale() { return m_scale; }

const MiDataSetI<MbVec3d> *OIDIMeshNodeManager::getVectorDataSet(int index) {
  if (vectorSetsAvailable() && index >= 0) {
    MoVec3SetI *vectorSet = dynamic_cast<MoVec3SetI *>(m_vectorSetGroup->getChild(index));
    assert(vectorSet);

    const MiDataSetI<MbVec3d> *dataSet = vectorSet->getVec3Set();
    assert(dataSet);
    return dataSet;
  } else
    return 0;
}

const MiDataSetI<CTensor> *OIDIMeshNodeManager::getTensorDataSet(int index) {
  if (tensorSetsAvailable() && index >= 0) {
    MoTensorSet *tensorSet = dynamic_cast<MoTensorSet *>(m_tensorSetGroup->getChild(index));
    assert(tensorSet);

    const MiDataSetI<CTensor> *dataSet = tensorSet->getTensorSet();
    assert(dataSet);
    return dataSet;
  } else
    return 0;
}

const MiDataSetI<double> *OIDIMeshNodeManager::getFirstScalarDataSet() { return getScalarDataSet(0); }

const MiDataSetI<double> *OIDIMeshNodeManager::getScalarDataSet(int index) {
  auto numChildren = getActiveScalarSetGroup()->getNumChildren();

  if (index >= 0 && scalarSetsAvailable() && numChildren > index) {
    SoNode *child = getActiveScalarSetGroup()->getChild(index);
    MoScalarSetI *scalarSet = dynamic_cast<MoScalarSetI *>(child);
    assert(scalarSet);

    const MiDataSetI<double> *dataSet = scalarSet->getScalarSet();
    assert(dataSet);
    return dataSet;
  } else
    return 0;
}

const MiDataSetI<double> *OIDIMeshNodeManager::getLastScalarDataSet() {
  return getScalarDataSet(getActiveScalarSetGroup()->getNumChildren() - 1);
}

SoGroup *OIDIMeshNodeManager::getActiveScalarSetGroup() { return m_valueScalarSetGroup; }

void OIDIMeshNodeManager::initCrossSectionManip() {
  m_crossSectionManip = new CrossSectionManipulator(); // used by the value tracker as well.
  m_crossSectionManip->setMeshNodeManager(this);
  m_crossSectionManipSwitch = new SoSwitch;
  m_crossSectionManipSwitch->setName("crossSectionManipSwitch");
  m_crossSectionManipSwitch->addChild(m_crossSectionManip);
  m_crossSectionManipSwitch->whichChild = SO_SWITCH_NONE;

  m_showTracker = false;

  addChild(m_crossSectionManipSwitch);
}

SoSwitch *OIDIMeshNodeManager::createWellPathNode(const CNewWellPath &wellPath) {
  SoSwitch *wellPathSwitch = new SoSwitch;
  wellPathSwitch->addChild(new OIVWellPath(wellPath, m_translation));
  wellPathSwitch->whichChild = SO_SWITCH_ALL;

  return wellPathSwitch;
}

void OIDIMeshNodeManager::usePhongLightModel(bool mode) {
  if (m_lightModel)
    m_lightModel->model = mode ? SoLightModel::PHONG : SoLightModel::BASE_COLOR;
}

bool OIDIMeshNodeManager::usingPhongLightModel() const {
  return m_lightModel && m_lightModel->model.getValue() == SoLightModel::PHONG;
}

void OIDIMeshNodeManager::OnShowCrossSection(const CCrossSection &xsec) {
  CGeomecDoc *doc = GetGeomecDoc();
  IWellModel *model = dynamic_cast<IWellModel *>(doc->Model());

  m_xsecGroup->enableCrossSection(&xsec);
  m_xsecGroup->setActiveCrossSection(&xsec);

  // disconnect the previous current cross section
  if (m_currentCrossSection != 0) {
    m_currentCrossSection->disconnect();
    if (!model)
      m_currentCrossSection->clip = false;
  }

  m_showTracker = true;
  m_crossSectionManipSwitch->whichChild = m_inView ? SO_SWITCH_ALL : SO_SWITCH_NONE;

  // connect the cross section manipulator to the new current cross section
  m_currentCrossSection = m_xsecGroup->getCrossSectionNode(&xsec);

  UpdateCrossSectionManip(xsec);

  // link the current cross section to the cross section manipulator
  m_currentCrossSection->connectFrom(m_crossSectionManip);
  m_currentCrossSection->clip = m_clipPlaneModeSelected;

  UpdateValueTrackerDisplay();
  OnColorScaleChanged();
  UpdateDisplaySettings();
}

void OIDIMeshNodeManager::OnHideCrossSection(const CCrossSection &xsec) {
  MeshCrossSection *cs = m_xsecGroup->getCrossSectionNode(&xsec);
  assert(cs);
  cs->ref();

  m_xsecGroup->disableCrossSection(&xsec);

  // If we're hiding the current cross section, then also switch off the manipulator
  if (m_currentCrossSection && cs == m_currentCrossSection) {
    m_currentCrossSection->disconnect();
    m_currentCrossSection = 0;

    m_showTracker = false;
    m_crossSectionManipSwitch->whichChild = SO_SWITCH_NONE;
    m_crossSectionManip->removeConstraints();

    // try to make another cross section that is still visible the active one.
    if (m_xsecGroup->getNumChildren() > 0) {
      const CCrossSection *activeCrossSection = static_cast<MeshCrossSection *>(m_xsecGroup->getChild(0))->getParent();
      OnShowCrossSection(*activeCrossSection);
    }

    UpdateValueTrackerDisplay();
  }

  cs->unref();
  OnColorScaleChanged();

  ActUponNoValuesDisplayed(Deleted);
}

void OIDIMeshNodeManager::OnCrossSectionChanged(const CCrossSection &xsec) {
  MeshCrossSection *mcs = m_xsecGroup->getCrossSectionNode(&xsec);
  if (mcs != 0) {
    mcs->updateFromParent();
    if (mcs == m_currentCrossSection) {
      UpdateCrossSectionManip(xsec);
    }
  }
}

void OIDIMeshNodeManager::OnShowWellPath(const CNewWellPath &wellPath) {
  if (wellPath.PolyLine().PointSize() == 0)
    return;

  // see if well path is present in map
  TNewWellPathMap::iterator iter = m_newWellPathMap.find(&wellPath);
  if (iter != m_newWellPathMap.end()) {
    iter->second->whichChild = SO_SWITCH_ALL;
  } else {
    SoSwitch *wellPathNode = createWellPathNode(wellPath);

    m_wellPaths->addChild(wellPathNode);

    m_newWellPathMap[&wellPath] = wellPathNode;
  }
  addToGeologyLegend(wellPath);
}

void OIDIMeshNodeManager::OnHideWellPath(const CNewWellPath &wellPath) {
  TNewWellPathMap::iterator iter = m_newWellPathMap.find(&wellPath);
  if (iter != m_newWellPathMap.end()) {
    m_wellPaths->removeChild(iter->second);
    m_newWellPathMap.erase(iter);
  }
  removeFromGeologyLegend(wellPath);
}

void OIDIMeshNodeManager::OnWellPathChanged(const CNewWellPath &wellPath) {
  TNewWellPathMap::iterator iter = m_newWellPathMap.find(&wellPath);
  if (iter != m_newWellPathMap.end()) {
    OnWellPathChanged(iter);
  }
  ModifyGeologyLegend(wellPath);
}

void OIDIMeshNodeManager::OnWellPathChanged(TNewWellPathMap::iterator iter) {
  assert(iter != m_newWellPathMap.end());

  SoSwitch *wellPathSwitch = iter->second;

  SoSwitch *newWellPathSwitch = createWellPathNode(*iter->first);
  m_wellPaths->replaceChild(wellPathSwitch, newWellPathSwitch);

  iter->second = newWellPathSwitch;
}

void OIDIMeshNodeManager::OnDeformation(Deformation::Data::Input input) {
  // Printer::instance()->debug("OIDIMeshNodeManager::OnDeformation( %d, %d, %f, %f )", input.active, input.stage,
  // input.scale.gen, input.scale.def );
  CWellCasingModel *model = dynamic_cast<CWellCasingModel *>(GetGeomecDoc()->Model());
  if (model) {
    if (!model->m_deformation)
      model->m_deformation = new Deformation();
    Deformation *d = static_cast<Deformation *>(model->m_deformation);
    Deformation::eStatus status = d->get(input, m_meshSet);
    if (status == Deformation::eStatus::Valid)
      d->apply();
  }
  // Printer::instance()->debug("OIDIMeshNodeManager::OnDeformation : END");
}
Deformation::Data::Input OIDIMeshNodeManager::Deformation_() {
  CWellCasingModel *model = dynamic_cast<CWellCasingModel *>(GetGeomecDoc()->Model());
  if (model) {
    Deformation *d = static_cast<Deformation *>(model->m_deformation);
    if (d)
      return d->input();
  }
  return Deformation::INVALID_INPUT;
}

SbBox3f OIDIMeshNodeManager::GetGroupBoundingBox(SoGroup *group) {
  SbBox3f meshBoundingBox;

  if (group != 0) {
    SbViewportRegion vpregion(m_openInventorEventsHandler->Width(), m_openInventorEventsHandler->Height());
    SoGetBoundingBoxAction bboxAction(vpregion);
    bboxAction.apply(group);

    meshBoundingBox = bboxAction.getBoundingBox();
  } else {
    meshBoundingBox = SbBox3f(-1, -1, -1, 1, 1, 1);
  }

  return meshBoundingBox;
}

SbBox3f OIDIMeshNodeManager::get_xsec_manipulator_casing_mesh_bounding_box() {
  SbBox3f bbox;

  if (m_meshSet.isEmpty())
    return SbBox3f(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f);

  for (auto mesh : m_meshSet) {
    if (mesh != 0) {
      std::string name = mesh->getOpenGLNode()->Name().toStdString();

      const CWellCasingModel *model_casing = dynamic_cast<const CWellCasingModel *>(&mesh->getOpenGLNode()->Model());

      if (model_casing) {
        SbBox3f box = GetMeshBoundingBox(mesh);

        SbVec3f min = box.getMin();
        SbVec3f max = box.getMax();

        SbVec3f min_mod = SbVec3f(min[0] / 10, min[1] / 10, -5);
        SbVec3f max_mod = SbVec3f(max[0] / 10, max[1] / 10, +5);
        // SbVec3f max_mod = SbVec3f( max[0], max[1], min[2] + max[0]);

        // SbBox3f box_mod( min_mod, max_mod );
        SbBox3f box_mod(min, max);

        return box_mod;
      }
    }
  }

  return bbox;
}

SbBox3f OIDIMeshNodeManager::GetAllMeshesBoundingBox() {
  SbBox3f bbox;

  if (m_meshSet.isEmpty())
    return SbBox3f(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f);

  for (auto mesh : m_meshSet) {
    if (mesh != 0) {
      SbBox3f meshBbox = GetMeshBoundingBox(mesh);
      std::string name = mesh->getOpenGLNode()->Name().toStdString();
      print_bounding_box((char *)name.c_str(), meshBbox);
      bbox.extendBy(meshBbox);
    }
  }

  return bbox;
}

SbBox3f OIDIMeshNodeManager::GetDisplayedVolumesBoundingBox() {
  SbBox3f bbox;

  if (m_meshSet.isEmpty())
    return SbBox3f(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f);

  for (auto mesh : m_meshSet) {
    if (mesh != 0 && mesh->meshType() == OIDIMesh::VolumeMesh && FindMeshNode(m_displayedMeshes, mesh) != 0) {
      SbBox3f meshBbox = GetMeshBoundingBox(mesh);
      bbox.extendBy(meshBbox);
    }
  }

  return bbox;
}

bool OIDIMeshNodeManager::DisplayedVolumesContains(const SbVec3f &point) {
  if (m_meshSet.isEmpty())
    return true;

  for (const OIDIMesh *mesh : m_meshSet) {
    if (mesh != 0 && mesh->meshType() == OIDIMesh::VolumeMesh && FindMeshNode(m_displayedMeshes, mesh) != 0) {
      SbBox3f meshBbox = GetMeshBoundingBox(mesh);
      if (meshBbox.intersect(point)) {
        MiPointProbeUnstructured *probe = static_cast<const OIDIMeshVU *>(mesh)->getProbe();
        probe->setLocation(MbVec3d(point));
        if (probe->isFound()) {
          return true;
        }
      }
    }
  }
  return false;
}

SbVec3f OIDIMeshNodeManager::getDisplayedVolumesPoint() {
  for (const OIDIMesh *mesh : m_meshSet) {
    if (mesh != 0 && mesh->meshType() == OIDIMesh::VolumeMesh && FindMeshNode(m_displayedMeshes, mesh) != 0) {
      SbBox3f meshBbox = GetMeshBoundingBox(mesh);
      SbVec3f increment = (meshBbox.getMax() - meshBbox.getMin()) / 100.0f;

      std::unique_ptr<MiPointProbeUnstructured> probe(
          MiPointProbeUnstructured::getNewInstance(*static_cast<const OIDIMeshVU *>(mesh)));

      int i = -1;
      for (SbVec3f point = meshBbox.getMin() + 50 * increment; ++i < 50; point += increment) {
        probe->setLocation(MbVec3d(point));
        if (probe->isFound()) {
          return point;
        }
      }

      i = -1;
      for (SbVec3f point = meshBbox.getMin() + 49 * increment; ++i < 50; point -= increment) {
        probe->setLocation(MbVec3d(point));
        if (probe->isFound()) {
          return point;
        }
      }
    }
  }

  SbBox3f meshBbox = GetAllMeshesBoundingBox();
  return meshBbox.getCenter();
}

size_t OIDIMeshNodeManager::GetNumVolumeMeshCells() {
  size_t numCells = 0;
  for (auto mesh : m_meshSet) {
    if (mesh != 0 && mesh->meshType() == OIDIMesh::VolumeMesh) {
      numCells += mesh->getNumCells();
    }
  }
  return numCells;
}

size_t OIDIMeshNodeManager::GetNumDisplayedVolumeCells() {
  size_t numCells = 0;
  for (auto mesh : m_meshSet) {
    if (mesh != 0 && mesh->meshType() == OIDIMesh::VolumeMesh && FindMeshNode(m_displayedMeshes, mesh) != 0) {
      numCells += mesh->getNumCells();
    }
  }
  return numCells;
}

SbBox3f OIDIMeshNodeManager::GetMeshBoundingBox(const OIDIMesh *mesh) {
  if (mesh != 0) {
    MbVec3d localMinVec = mesh->getMin();
    MbVec3d localMaxVec = mesh->getMax();
    return SbBox3f(localMinVec[0] - m_translation[0], localMinVec[1] - m_translation[1],
                   localMinVec[2] - m_translation[2], localMaxVec[0] - m_translation[0],
                   localMaxVec[1] - m_translation[1], localMaxVec[2] - m_translation[2]);
  } else
    return SbBox3f();
}

OIDIMeshNode *OIDIMeshNodeManager::FindMeshNode(SoGroup *group, const OIDIMesh *mesh) {
  for (auto node : group) {
    OIDIMeshNode *meshNode = dynamic_cast<OIDIMeshNode *>(node);
    if (meshNode && meshNode->getMesh() == mesh)
      return meshNode;
  }
  return 0;
}

int OIDIMeshNodeManager::addScalarDataSet(MiDataSetI<double> *dataSet) {
  // Create a new MoScalarSetI node, and make it reference the data set
  MoScalarSetI *scalarSet = new MoScalarSetI;
  scalarSet->setScalarSet(dataSet);

  m_valueScalarSetGroup->addChild(scalarSet);

  return m_valueScalarSetGroup->getNumChildren() - 1;
}

int OIDIMeshNodeManager::addVectorDataSet(MiDataSetI<MbVec3d> *dataSet) {
  MoVec3SetI *newVectorSet = new MoVec3SetI;
  newVectorSet->setVec3Set(dataSet);

  m_vectorSetGroup->addChild(newVectorSet);
  return m_vectorSetGroup->getNumChildren() - 1;
}

int OIDIMeshNodeManager::addTensorDataSet(MiDataSetI<CTensor> *dataSet) {
  MoTensorSet *newTensorSet = new MoTensorSet;
  newTensorSet->setTensorSet(dataSet);

  m_tensorSetGroup->addChild(newTensorSet);
  return m_tensorSetGroup->getNumChildren() - 1;
}

void OIDIMeshNodeManager::ShowValuesPerElement(bool perElement) {
  m_meshNodeSettings->showValuesPerElement = perElement;
  UpdateDisplaySettings();
}

bool OIDIMeshNodeManager::ShowValuesPerElement() const { return m_meshNodeSettings->showValuesPerElement; }

bool OIDIMeshNodeManager::CanShowValuesPerElement() { return true; }

void OIDIMeshNodeManager::ViewBeachBalls(bool view) {
  m_meshNodeSettings->viewBeachBalls = view;
  UpdateDisplaySettings();
}

bool OIDIMeshNodeManager::BeachBallsOnView() const { return m_meshNodeSettings->viewBeachBalls; }

bool OIDIMeshNodeManager::CanViewBeachBalls() const { return true; }

void OIDIMeshNodeManager::removeDataSets() {
  // Clean up scalar data sets
  for (auto node : m_valueScalarSetGroup) {
    if (node->getTypeId() == MoScalarSetI::getClassTypeId())
      delete reinterpret_cast<MoScalarSetI *>(node)->getScalarSet();
  }

  m_valueScalarSetGroup->removeAllChildren();

  // Clean up vector data sets
  for (auto node : m_vectorSetGroup) {
    if (node->getTypeId() == MoVec3SetI::getClassTypeId())
      delete reinterpret_cast<MoVec3SetI *>(node)->getVec3Set();
  }

  m_vectorSetGroup->removeAllChildren();

  // Don't delete the tensor data sets here, they are managed elsewhere via smart pointers
  m_tensorSetGroup->removeAllChildren();

  for (auto node : m_displayedMeshes) {
    assert(dynamic_cast<OIDIMeshNode *>(node) != nullptr);
    OIDIMeshNode *meshNode = static_cast<OIDIMeshNode *>(node);

    meshNode->setVectorSetId(-1);
    meshNode->setDisplayVectors(false);
    meshNode->setDisplayBeachBalls(false);
    meshNode->setColorScalarSetId(-1);
    meshNode->setIsoScalarSetId(-1);
    meshNode->setTensorSetId(-1);
    meshNode->setTensorVectorSetId(-1);
    meshNode->setDisplayTensorVectors(false);
    meshNode->UpdateDisplaySettings(*m_meshNodeSettings, ShowColor());
  }

  m_meshInfoMap.clear();
}

void OIDIMeshNodeManager::removeIsoDataSets() {
  for (auto node : m_displayedMeshes) {
    assert(dynamic_cast<OIDIMeshNode *>(node) != nullptr);
    OIDIMeshNode *meshNode = static_cast<OIDIMeshNode *>(node);

    meshNode->setIsoScalarSetId(-1);
    meshNode->UpdateDisplaySettings(*m_meshNodeSettings, ShowColor());
  }

  // Reset all isoScalarSetIds in the map to -1
  std::for_each(std::begin(m_meshInfoMap), std::end(m_meshInfoMap),
                [](MeshInfoMap::value_type &item) { item.second.isoScalarSetId = -1; });
}

void OIDIMeshNodeManager::OnValueComponentChanged(const IValueComponentBase *pValueComponent,
                                                  const CDoubleQuantity::UNIT unit, bool settingsChanged) {
  // No idea whether this should be cached (valuecomponent caches, and we might not be able to tell
  // when we would need to invalidate, or it might be such a hassle that it won't be worth it)
  //

  // No idea yet if this is the right place to do this either:
  // when we select an OpenGLNode its default view mode will be "geology," not "value" --TODO: needs to be checked--
  // so viewing the valuecomponent will always go through this event handler after the meshes were previously activated.

  // Update to the above: it looks like we don't always receive this event. It needs further investigation, but the
  // first time a result tree, or part thereof, is selected, it seems that we only get an OnNew event. Only after making
  // other selections do we get this event. This should probably be remedied in the OnNew event.

  bool valueComponentChanged = (pValueComponent != m_pValueComponentDisplayed);
  m_pValueComponentDisplayed = pValueComponent;
  m_pIsoValueComponentDisplayed = pValueComponent;

  if (settingsChanged && !scalarSetsAvailable())
    return; // no need to do anything

  if (m_meshSet.isEmpty())
    return; // no meshes to apply to

  bool doRemoveDataSets = valueComponentChanged || settingsChanged;

  if (!pValueComponent) {
    // valuecomponent has been reset to 0
    if (doRemoveDataSets)
      removeDataSets();
    DisplayGeologyLegend();
    return;
  }

  // Can throw CProgressCancel
  ActUponValueComponentChanged(pValueComponent, unit, ColorComponent, valueComponentChanged || settingsChanged);

  /* We're handling this in the update of the legend
  OnColorScaleChanged();
  UpdateDisplaySettings();

  if (valueComponentChanged || !ShowColor())
    DisplayValuesLegend();

  UpdateValueTrackerDisplay();
  */
}

void OIDIMeshNodeManager::OnIsoValueComponentChanged(const IValueComponentBase *pValueComponent,
                                                     CDoubleQuantity::UNIT unit) {
  bool doRemoveDataSets = pValueComponent != m_pIsoValueComponentDisplayed;

  try {
    ActUponValueComponentChanged(pValueComponent, unit, IsoComponent, doRemoveDataSets);
  } catch (CProgressCancel *p) {
    throw(p);
  }

  m_pIsoValueComponentDisplayed = pValueComponent;

  GetIsoDataMinMax(m_meshNodeSettings->dataMin, m_meshNodeSettings->dataMax);

  /* We're handling this in the update of the legend
  UpdateDisplaySettings();
  UpdateValuesLegendAdornments();
  */
}

/**
 * @param pValueComponent
 * @param unit
 * @param ComponentType Indicates which component changed (color or iso value)
 * @param createNewDataSets If true, delete all data sets and creae
 te them again. This happens when the
 *            value component has actually changed, or when the settings have changed (for instance,
 *            when going from PER_CELL to PER_NODE properties).
 */
void OIDIMeshNodeManager::ActUponValueComponentChanged(const IValueComponentBase *pValueComponent,
                                                       CDoubleQuantity::UNIT unit, ComponentType whichComponentChanged,
                                                       bool createNewDataSets) {
  bool bIsVectorComponent = pValueComponent && pValueComponent->Type() == IValueComponentBase::VECTOR;
  bool bIsTensorComponent = pValueComponent && pValueComponent->Type() == IValueComponentBase::TENSOR;

  if (createNewDataSets) {
    // remove the old ones
    switch (whichComponentChanged) {
    case ColorComponent:
      removeDataSets();
      break;
    case IsoComponent:
      removeIsoDataSets();
      break;
    default:
      assert(false);
    }
  }

  CValueMapper *vm = CValueMapper::instance();

  std::unique_ptr<IProgressBase> prog(_g->prog()->create(eProgress::Geo, "Collecting values", !vm->UseLegacyCode()));

  for (std::size_t i = 0; i < m_meshSet.NrOfMesh(); ++i) {
    const OIDIMesh *mesh = m_meshSet.getMesh(i);

    if (!mesh)
      continue;

    // Printer::instance()->debug( "mesh: %s", mesh->getOpenGLNode()->Name().toStdString().c_str() );

    // Check if we have an OIDIMeshNode for this mesh
    bool meshNodeIsDisplayed;
    OIDIMeshNode *meshNode = FindMeshNode(mesh, meshNodeIsDisplayed);

    // Even if there's no mesh node, the mesh can still be used in a cross section
    bool meshUsedInCrossSection = (mesh->meshType() == OIDIMesh::VolumeMesh && m_xsecGroup->getNumChildren() > 0);

    if (meshNode == 0 && !meshUsedInCrossSection)
      continue; // mesh not used

    MeshInfo meshInfo;

    // This function is also called when a new mesh is added. In that case, createNewDataSets is false, and some of the
    // meshes might already have valid data set ids.
    if (!createNewDataSets)
      meshInfo = m_meshInfoMap[mesh];

    int scalarSetId = (whichComponentChanged == ColorComponent || meshInfo.colorScalarSetId >= 0)
                          ? meshInfo.colorScalarSetId
                          : meshInfo.isoScalarSetId;

    // If the current mesh does not have a valid data set id, collect the values from the value component
    if (scalarSetId == -1) // relies on scalarSetId >=0 if property was assigned to the meshNode
    {
      // if we have point sets on display, only (try to) map value components onto point sets
      if (pValueComponent->CanMap(*(mesh->getOpenGLNode()))) {
        prog->AddSteps(mesh->meshType() == OIDIMesh::PointSetMesh ? mesh->getSize() : mesh->getNumCells());
      }
    }
  }

  vm->EnterSections();

  bool bCanceled = false;

  try {

    for (std::size_t i = 0; i < m_meshSet.NrOfMesh(); ++i) {
      const OIDIMesh *mesh = m_meshSet.getMesh(i);
      if (!mesh)
        continue;

      // IT_Printer::instance()->debug( "mesh (collecting values): %s",
      // mesh->getOpenGLNode()->Name().toStdString().c_str() );

      prog->StatusMessage("Collecting values for " + mesh->getOpenGLNode()->Name());

      // Check if we have an OIDIMeshNode for this mesh
      bool meshNodeIsDisplayed;
      OIDIMeshNode *meshNode = FindMeshNode(mesh, meshNodeIsDisplayed);

      // Even if there's no mesh node, the mesh can still be used in a cross section
      bool meshUsedInCrossSection = (mesh->meshType() == OIDIMesh::VolumeMesh && m_xsecGroup->getNumChildren() > 0);

      if (meshNode == 0 && !meshUsedInCrossSection)
        continue; // mesh not used

      MeshInfo meshInfo;

      // This function is also called when a new mesh is added. In that case, createNewDataSets is false, and some of
      // the meshes might already have valid data set ids.
      if (!createNewDataSets)
        meshInfo = m_meshInfoMap[mesh];

      int scalarSetId = (whichComponentChanged == ColorComponent || meshInfo.colorScalarSetId >= 0)
                            ? meshInfo.colorScalarSetId
                            : meshInfo.isoScalarSetId;

      // If the current mesh does not have a valid data set id, collect the values from the value component
      if (scalarSetId == -1) // relies on scalarSetId >=0 if property was assigned to the meshNode
      {
        OIDIScalarSetI *values = 0;
        OIDIVectorSetI *vectors = 0;
        OIDIScalarSetI *eigenValues = 0;
        OIDITensorSetI *tensors = 0;
        OIDIScalarSetI *beachballScalars1 = 0;
        OIDIScalarSetI *beachballScalars2 = 0;

        bool computeByTopology = ShowValuesPerElement() && mesh->meshType() != OIDIMesh::PointSetMesh &&
                                 mesh->meshType() != OIDIMesh::PointMesh;

        // if we have point sets on display, only (try to) map value components onto point sets
        if (pValueComponent->CanMap(*(mesh->getOpenGLNode()))) {
          if (whichComponentChanged == IsoComponent || (!bIsVectorComponent && !bIsTensorComponent)) {
            values = CollectValues(mesh, pValueComponent, unit, computeByTopology, *prog);

            // getMesh()->GetGeometry()->setOffset( std::vector<SbVec3d>);
          } else if (bIsVectorComponent) {
            auto pVectorComponent = static_cast<const IVectorResult::IVectorComponent *>(pValueComponent);
            assert(pVectorComponent);

            // different behavior between tensor eigenvectors and other vectors
            if (pVectorComponent->Parent().IsTensorVector()) {
              meshInfo.tensorSet.reset(CollectTensors(mesh, pVectorComponent, unit, computeByTopology, *prog));

              const ITensorGroup::CVectorComposite *pResult =
                  dynamic_cast<const ITensorGroup::CVectorComposite *>(&pVectorComponent->Parent());

              GenerateEigenVectorsAndValuesFromTensors(*meshInfo.tensorSet, (unsigned short)pResult->Direction(),
                                                       vectors, eigenValues);
              values = GenerateScalarsFromScalars(*eigenValues); // makes a copy
            } else {
              vectors = CollectVectors(mesh, pVectorComponent, unit, computeByTopology, *prog);
              if (vectors)
                values = GenerateScalarsFromVectors(*vectors, 3);
            }
          } else if (bIsTensorComponent) {
            if (dynamic_cast<const ITensorGroup::CComponentComposite::CFullTensorComponent *>(pValueComponent)) {
              auto pTensorComponent =
                  static_cast<const ITensorGroup::CComponentComposite::CFullTensorComponent *>(pValueComponent);

              pTensorComponent->ResetMinMax();
            } else {
              pValueComponent->TensorData()->ResetMinMax();
            }

            meshInfo.tensorSet.reset(CollectTensors(mesh, pValueComponent, unit, computeByTopology, *prog));
            tensors = meshInfo.tensorSet.get(); // dangerous, but whatever...

            // generate the default scalar sets that show tensile / compressive stresses by color
            std::array<OIDIScalarSetI *, 2> beachballScalarSets =
                generateDefaultBeachballScalarSets(*meshInfo.tensorSet);
            beachballScalars1 = beachballScalarSets[0];
            beachballScalars2 = beachballScalarSets[1];

            // returns the Frobenius norm
            values = GenerateScalarsFromTensors(*meshInfo.tensorSet, 6);
          }

          if (values != nullptr)
            scalarSetId = addScalarDataSet(values);

          if (vectors != nullptr && vectors->dataSize() != 0) {
            int id = addVectorDataSet(vectors);
            if (eigenValues != 0)
              meshInfo.tensorVectorSetId = id;
            else
              meshInfo.vectorSetId = id;
          } else {
            delete vectors;
            vectors = nullptr;
          }

          if (eigenValues != nullptr && eigenValues->dataSize() != 0)
            meshInfo.eigenValueSignSetId = addScalarDataSet(eigenValues);
          else {
            delete eigenValues;
            eigenValues = nullptr;
          }

          if (tensors != nullptr && tensors->dataSize() != 0)
            meshInfo.tensorSetId = addTensorDataSet(tensors);
          else
            // tensors has a shared_ptr to it, so we can't just delete it
            tensors = nullptr;

          if (beachballScalars1 != nullptr && beachballScalars1->dataSize() != 0)
            meshInfo.beachballScalarSetId1 = addScalarDataSet(beachballScalars1);
          else {
            delete beachballScalars1;
            beachballScalars1 = nullptr;
          }

          if (beachballScalars2 != nullptr && beachballScalars2->dataSize() != 0)
            meshInfo.beachballScalarSetId2 = addScalarDataSet(beachballScalars2);
          else {
            delete beachballScalars2;
            beachballScalars2 = 0;
          }
        } else {
          values = new OIDIScalarSetI(0, computeByTopology ? MiDataSet::PER_CELL : MiDataSet::PER_NODE);
          scalarSetId = addScalarDataSet(values);
        }
      }

      if (whichComponentChanged == ColorComponent) {
        meshInfo.colorScalarSetId = scalarSetId;

        if (meshNode != 0) {
          meshNode->setColorScalarSetId(meshInfo.colorScalarSetId);
          meshNode->setVectorSetId(meshInfo.vectorSetId);
          meshNode->setTensorVectorSetId(meshInfo.tensorVectorSetId);
          meshNode->setTensorSetId(meshInfo.tensorSetId);
          meshNode->setVectorScalarSetId(meshInfo.eigenValueSignSetId);
          meshNode->setBeachballScalarSetIds(meshInfo.beachballScalarSetId1, meshInfo.beachballScalarSetId2);

          if (meshInfo.vectorSetId != -1)
            meshNode->setDisplayVectors(true);
          if (meshInfo.tensorVectorSetId != -1)
            meshNode->setDisplayTensorVectors(true);
          if (meshInfo.tensorSetId != -1)
            meshNode->setDisplayBeachBalls(true);
        }
      } else {
        meshInfo.isoScalarSetId = scalarSetId;

        if (meshNode != 0)
          meshNode->setIsoScalarSetId(scalarSetId);
      }

      if (mesh->meshType() == OIDIMesh::VolumeMesh) {
        const OIDIMeshVU *meshVU = static_cast<const OIDIMeshVU *>(mesh);

        if (whichComponentChanged == ColorComponent) {
          m_meshPointProbeGroup->setScalarSetId(meshVU, scalarSetId);
          m_meshPointProbeGroup->setVectorSetId(meshVU, -1);
          if (meshInfo.vectorSetId >= 0)
            m_meshPointProbeGroup->setVectorSetId(meshVU, meshInfo.vectorSetId);
          if (meshInfo.tensorVectorSetId >= 0)
            m_meshPointProbeGroup->setVectorSetId(meshVU, meshInfo.tensorVectorSetId);

          m_meshPointProbeGroup->setTensorSetId(meshVU, meshInfo.tensorSetId);
        }

        for (auto node : m_xsecGroup) {
          MeshCrossSection *mcs = static_cast<MeshCrossSection *>(node);

          if (whichComponentChanged == ColorComponent) {
            // we always have a scalarsetid
            mcs->setScalarSetId(meshVU, scalarSetId);

            bool showVectors = meshInfo.vectorSetId != -1; // plain vectors
            bool showTensorVectors = meshInfo.tensorVectorSetId != -1 &&
                                     meshInfo.eigenValueSignSetId != -1; // eigenvectors and eigenvalues of tensors
            bool showTensors = meshInfo.tensorSetId != -1;               // projections of tensors on cross sections

            mcs->showVectors = false;
            mcs->showTensorVectors = false;

            if (showVectors) {
              mcs->setVectorSetId(meshVU, meshInfo.vectorSetId);
              mcs->showVectors = true;
            } else if (showTensorVectors) {
              mcs->setTensorVectorSetId(meshVU, meshInfo.tensorVectorSetId, meshInfo.eigenValueSignSetId);
              mcs->showTensorVectors = true;
            } else if (showTensors) {
              int tensorVectorSetId, eigenValueSignSetId;
              mcs->getTensorVectorSetIds(meshVU, tensorVectorSetId, eigenValueSignSetId);
              // Only create new datasets if we don't already have them, or when a new property is set
              if (createNewDataSets || (tensorVectorSetId == -1 && eigenValueSignSetId == -1)) {
                // Generate tensor vector set from tensors and normal vector.
                tensorVectorSetId = createTensorVectorDataSet(meshInfo.tensorSet);
                // Derive eigenvalue sign set from tensors, for coloring the vectors green / red
                eigenValueSignSetId = createTensorVectorSignDataSet(meshInfo.tensorSet);
              }

              mcs->setTensorVectorSetId(meshVU, tensorVectorSetId, eigenValueSignSetId);
#if 1
              if (mcs == m_currentCrossSection) {
                m_meshPointProbeGroup->setVectorSetId(meshVU, tensorVectorSetId); // does not work as it should yet ...
              }
#endif
              mcs->showTensorVectors = true;
            }
          } else {
            mcs->setIsoScalarSetId(meshVU, scalarSetId);
          }
        }
      }

      m_meshInfoMap[mesh] = meshInfo;

      vm->NextSection();
    }
  } catch (CProgressCancel *e) {
    delete e;
    bCanceled = true;
  }

  vm->LeaveSections();

  if (bCanceled) {
    throw new CProgressCancel();
  }
}

int OIDIMeshNodeManager::createTensorVectorDataSet(std::shared_ptr<OIDITensorSetI> tensors) {
  OIDITensorVectorSetI *vectorDataSet = new OIDITensorVectorSetI(
      tensors->dataSize(), ShowValuesPerElement() ? MiDataSet::PER_CELL : MiDataSet::PER_NODE, tensors);

  return addVectorDataSet(vectorDataSet);
}

int OIDIMeshNodeManager::createTensorVectorSignDataSet(std::shared_ptr<OIDITensorSetI> tensors) {
  OIDITensorVectorSignSetI *scalarDataSet = new OIDITensorVectorSignSetI(
      tensors->dataSize(), ShowValuesPerElement() ? MiDataSet::PER_CELL : MiDataSet::PER_NODE, tensors);

  return addScalarDataSet(scalarDataSet);
}

OIDIMeshNode *OIDIMeshNodeManager::FindMeshNode(const OIDIMesh *mesh, bool &displayed) {
  displayed = true;
  OIDIMeshNode *meshNode = 0;

  meshNode = FindMeshNode(m_displayedMeshes, mesh);

  return meshNode;
}

void OIDIMeshNodeManager::OnColorScaleChanged() {
  if (!scalarSetsAvailable())
    return;
  UpdateColorMapping();
  RefreshVectorBaseScale();
  RefreshTensorBaseScale();
  UpdateValuesLegendAdornments();
}

void OIDIMeshNodeManager::OnNewFormationNode(const CFormationBase &node) {
  m_meshSet.handleNodeNew(node);
  std::vector<const OIDIMesh *> meshes;
  m_meshSet.collectMeshes(node, meshes);

  for (size_t i = 0; i < meshes.size(); ++i) {
    assert(dynamic_cast<const OIDIMeshVU *>(meshes[i]) != 0);
    const OIDIMeshVU *mesh = static_cast<const OIDIMeshVU *>(meshes[i]);

    m_meshPointProbeGroup->addProbe(GetName(node), mesh, m_valueTracker);
  }
}

void OIDIMeshNodeManager::OnFormationNodeModified(const CFormationBase &node, ModifiedHint hint) {
  // At the moment, this function is only used to change the color of cross sections based on the
  // formation color. If there are no cross sections, we can exit immediately.
  if (m_xsecGroup->getNumChildren() == 0)
    return;

  float red, green, blue;
  DecomposeQColor(node.Color(), red, green, blue);

  std::vector<const OIDIMesh *> meshes;
  m_meshSet.collectMeshes(node, meshes);

  for (size_t i = 0; i < meshes.size(); ++i) {
    const OIDIMesh *mesh = meshes[i];

    if (mesh->meshType() == OIDIMesh::VolumeMesh) {
      const MiMesh *mimesh = dynamic_cast<const MiMesh *>(mesh);
      for (auto xsnode : m_xsecGroup) {
        MeshCrossSection *xsec = static_cast<MeshCrossSection *>(xsnode);
        xsec->setColor(mimesh, SbColor(red, green, blue));
      }
    }
  }
  ModifyGeologyLegend(node);
}

void OIDIMeshNodeManager::OnFormationNodeDeleted(const CFormationBase &node) {
  std::vector<const OIDIMesh *> meshes;
  m_meshSet.collectMeshes(node, meshes);

  for (size_t i = 0; i < meshes.size(); ++i) {
    assert(dynamic_cast<const OIDIMeshVU *>(meshes[i]) != 0);
    const OIDIMeshVU *mesh = static_cast<const OIDIMeshVU *>(meshes[i]);

    m_meshPointProbeGroup->removeProbe(mesh);

    // clean up the meshInfoMap ...
    m_meshInfoMap.erase(mesh);
  }
  HandleOpenGLNodeDeleted(node);
  m_meshSet.handleNodeDeleted(node);
}

void OIDIMeshNodeManager::OnNewOpenGLNode(const COpenGLNode &node) { HandleNewOpenGLNode(node); }

void OIDIMeshNodeManager::ActUponNoValuesDisplayedWhenAdded() { ActUponNoValuesDisplayed(Added); }

void OIDIMeshNodeManager::OnOpenGLNodeModified(const COpenGLNode &node, enum ModifiedHint uHint) {
  if (uHint == SelectionChanged) // not so important for OIV
    return;

  // brute force, but maybe sometimes not brutal enough, e.g. in case of un- or re-meshing?
  if (!HandleOpenGLNodeDeleted(node) || uHint == MeshCleared)
    return;

  HandleNewOpenGLNode(node);

  ActUponNoValuesDisplayed(Added);

  if (m_legendScene->isDisplayingValueLegend())
    MakeValuedMeshesVisible();
  else
    MakeAllMeshesVisible();

  OnColorScaleChanged();
  UpdateDisplaySettings();
  UpdateValueTrackerDisplay();
}

void OIDIMeshNodeManager::OnOpenGLNodeDeleted(const COpenGLNode &node) {
  if (!HandleOpenGLNodeDeleted(node))
    return;

  ActUponNoValuesDisplayed(Deleted);

  OnColorScaleChanged();
  UpdateValueTrackerDisplay();
}

void OIDIMeshNodeManager::HandleNewOpenGLNode(const COpenGLNode &node) {
  if (!CanDisplay(&node))
    return;

  PointSetWasSelected(dynamic_cast<const CPointSet *>(&node) != 0 ||
                      dynamic_cast<const CHexaMeshRegionBase *>(&node) != 0);

  bool meshAdded = false;
  bool bCreated = m_meshSet.handleNodeNew(node); // creates the node's meshes if necessary

  std::vector<const OIDIMesh *> meshes;
  m_meshSet.collectMeshes(node, meshes);

  // Printer::instance()->debug( "openGL_Node : NEW : %s -> OIVD Node : %s - meshes:%d - mesh_set: %d",
  // node.Name().toStdString().c_str(), bCreated ? "NEW" : "Existant", meshes.size(), m_meshSet.NrOfMesh() );

  for (std::size_t i = 0; i < meshes.size(); ++i) {
    const OIDIMesh *mesh = meshes[i];
    OIDIMeshNode *meshNode = FindOrCreateMeshNode(mesh); // already added to m_displayedMeshes

    float red, green, blue;
    DecomposeQColor(node.Color(), red, green, blue);
    meshNode->setGeologyColor(red, green, blue);

    meshNode->setVisibility(true);
    meshAdded = true;

    meshNode->UpdateDisplaySettings(*m_meshNodeSettings, ShowColor());
  }

  CWellCasingModel *model = dynamic_cast<CWellCasingModel *>(GetGeomecDoc()->Model());
  if (model) {
    Deformation *d = dynamic_cast<Deformation *>(model->m_deformation);
    if (d && d->status() == Deformation::eStatus::Valid) {
      // FIXME: consider the case: uncheck - check -> no need to re-calculate values for all the meshes

      bool bCanApply = false; // but we do need to be sure that our meshes and geometries are exactly the same,
                              // otherwise we can follow dead pointers

      if (d->num_meshes() == m_meshSet.NrOfMesh()) // new openGL_Node
      {
        bCanApply = true;

        for (size_t i = 0; i < d->num_meshes(); ++i) {
          const Deformation::Data::Result &r = d->result(i);

          const OIDIMesh *found = nullptr;
          for (size_t j = 0; j < m_meshSet.NrOfMesh(); ++j) {
            if (m_meshSet.getMesh(j) == r.info.mesh) {
              found = r.info.mesh;
            }
          }

          if (!found || &found->getGeometry() != r.info.geo) {
            bCanApply = false;
            break;
          }
        }
      }

      if (bCanApply) {
        d->apply();
      } else {
        refresh_xsec();

        Deformation::eStatus status = d->refresh(m_meshSet);
        if (status == Deformation::eStatus::Valid)
          d->apply();
      }
    }
  }

  if (meshAdded) {
    addToGeologyLegend(node);
  }
}

bool OIDIMeshNodeManager::refresh_xsec() {
  bool res = false;

  for (auto node : m_xsecGroup) // FIXME : only if it's linked
  {
    MeshCrossSection *mcs = static_cast<MeshCrossSection *>(node);
    const CCrossSection *xsec = mcs->getParent();

    if (m_openInventorEventsHandler->m_sceneNode.IsLinkedTo(*xsec)) {
      m_openInventorEventsHandler->m_sceneNode.UnLink(*(CGraphNode *)xsec);
      m_openInventorEventsHandler->m_sceneNode.LinkTo(*(CGraphNode *)xsec);
      res = true;
    }
  }
  return res;
}

float OIDIMeshNodeManager::GetHeight(const COpenGLNode &node) {
  SbBox3f bbox;

  m_meshSet.handleNodeNew(node); // creates the node's meshes if necessary

  std::vector<const OIDIMesh *> meshes;
  m_meshSet.collectMeshes(node, meshes);

  for (auto mesh : meshes) {
    bbox.extendBy(GetMeshBoundingBox(mesh));
  }

  return (bbox.getMin()[2] + bbox.getMax()[2]) / 2;
}

bool OIDIMeshNodeManager::HandleOpenGLNodeDeleted(const COpenGLNode &node) {
  PointSetWasDeselected(dynamic_cast<const CPointSet *>(&node) != 0 ||
                        dynamic_cast<const CHexaMeshRegionBase *>(&node) != 0);

  std::vector<const OIDIMesh *> meshes;
  m_meshSet.collectMeshes(node, meshes);

  bool meshesFound = false;
  for (std::size_t i = 0; i < meshes.size(); ++i) {
    const OIDIMesh *mesh = meshes[i];

    OIDIMeshNode *meshNode = FindMeshNode(m_displayedMeshes, mesh);

    if (meshNode) {
      meshNode->setVisibility(false);
      m_displayedMeshes->removeChild(meshNode);
      meshesFound = true;
    }

    // clean up the meshInfoMap ...
    if (!dynamic_cast<const CFormationBase *>(&node))
      m_meshInfoMap.erase(mesh);
  }

  CWellCasingModel *model = dynamic_cast<CWellCasingModel *>(GetGeomecDoc()->Model());
  if (model) {
    Deformation *d = dynamic_cast<Deformation *>(model->m_deformation);
    if (d && d->status() == Deformation::eStatus::Valid)
      refresh_xsec();
  }

  // Don't delete the node if it's a formation node. These should always be available because
  // they're also used by cross sections. Deletion of formation nodes is handled by the
  // CFormationNodeAssistant
  if (!dynamic_cast<const CFormationBase *>(&node))
    m_meshSet.handleNodeDeleted(node); // deletes the node from the mesh set

  removeFromGeologyLegend(node);
  UpdateGeologyLegend();

  return true;
}

bool OIDIMeshNodeManager::ValuesBeingDisplayed(AddedOrDeleted action) const {
  for (size_t index = 0; index < m_displayedMeshes->getNumChildren(); ++index) {
    OIDIMeshNode *meshNode = dynamic_cast<OIDIMeshNode *>(m_displayedMeshes->getChild(index));
    if (action == Added && meshNode->getValueColorScalarSetId() < 0)
      return false;
    if (action == Deleted && meshNode->getValueColorScalarSetId() >= 0)
      return true;
  }

  return (action == Added ? true : true);
}

bool OIDIMeshNodeManager::DraggerTrackerSelected() const { return (m_draggerTrackerSelected == true); }

bool OIDIMeshNodeManager::CanSelectDraggerTracker() const { return true; }

void OIDIMeshNodeManager::SelectDraggerTracker(bool selected) {
  updateValueTracker(0, 0, MbVec3d(-1e-8), (size_t)-1);
  invalidateValueTrackerCellMarkers();

  m_draggerTrackerSelected = selected;
  if (selected)
    m_mouseTrackerSelected = false;

  m_valueTracker->TrackElement(TrackElement() || MouseTrackerSelected());
  UpdateValueTrackerDisplay();
}

bool OIDIMeshNodeManager::MouseTrackerSelected() const { return (m_mouseTrackerSelected == true); }

bool OIDIMeshNodeManager::CanSelectMouseTracker() const { return true; }

void OIDIMeshNodeManager::SelectMouseTracker(bool selected) {
  updateValueTracker(0, 0, MbVec3d(-1e-8), (size_t)-1);
  invalidateValueTrackerCellMarkers();

  m_mouseTrackerSelected = selected;
  if (selected) {
    m_draggerTrackerSelected = false;
  }
  m_valueTracker->TrackElement(TrackElement() || MouseTrackerSelected());
  UpdateValueTrackerDisplay();
}

bool OIDIMeshNodeManager::DraggerConfined() const { return m_crossSectionManip->isConfined(); }

bool OIDIMeshNodeManager::CanSetDraggerConfined() const { return m_crossSectionManip != 0; }

void OIDIMeshNodeManager::SetDraggerConfined(bool yesOrNo) { m_crossSectionManip->setConfined(yesOrNo); }

bool OIDIMeshNodeManager::LegendHidden() const {
  return (m_legendSceneSwitch->whichChild.getValue() == SO_SWITCH_NONE);
}

bool OIDIMeshNodeManager::CanSelectHideLegend() const { return true; }

void OIDIMeshNodeManager::HideLegend(bool yesOrNo) {
  m_legendSceneSwitch->whichChild = yesOrNo ? SO_SWITCH_NONE : SO_SWITCH_ALL;
}

void OIDIMeshNodeManager::SelectMeshMode(MeshMode mode) {
  m_meshNodeSettings->meshMode = mode;
  UpdateDisplaySettings();
}

MeshMode OIDIMeshNodeManager::MeshModeSelected() { return m_meshNodeSettings->meshMode; }

bool OIDIMeshNodeManager::CanSelectMeshMode() { return true; }

int OIDIMeshNodeManager::Transparency() const { return m_meshNodeSettings->transparency; }

void OIDIMeshNodeManager::Transparency(int transparency) {
  transparency = std::min(transparency, 100);
  transparency = std::max(transparency, 0);

  m_meshNodeSettings->transparency = transparency;
  UpdateDisplaySettings();
}

void OIDIMeshNodeManager::SetPickStyle(SoPickStyle::Style style) { m_pickStyle->style = style; }

SoPickStyle::Style OIDIMeshNodeManager::GetPickStyle() { return (SoPickStyle::Style)m_pickStyle->style.getValue(); }

int OIDIMeshNodeManager::IsoCount() const { return m_meshNodeSettings->isoCount; }

void OIDIMeshNodeManager::IsoCount(int isoCount) {
  isoCount = std::min(isoCount, 50);
  isoCount = std::max(isoCount, 0);

  m_meshNodeSettings->isoCount = isoCount;
  UpdateDisplaySettings();
}

double OIDIMeshNodeManager::IsoGap() const { return m_meshNodeSettings->isoGap; }

void OIDIMeshNodeManager::IsoGap(double gap) {
  m_meshNodeSettings->isoGap = gap;
  UpdateDisplaySettings();
}

bool OIDIMeshNodeManager::ClipPlaneModeSelected() const { return m_clipPlaneModeSelected; }

bool OIDIMeshNodeManager::CanSelectClipPlaneMode() const { return true; }

void OIDIMeshNodeManager::TrackElement(bool enable) {
  m_elementTrackerSelected = enable;
  m_valueTracker->TrackElement(TrackElement() || MouseTrackerSelected());
  dehighlightMesh();
}

bool OIDIMeshNodeManager::TrackElement() const { return m_elementTrackerSelected; }

bool OIDIMeshNodeManager::CanTrackElement() const { return true; }

void OIDIMeshNodeManager::SelectClipPlaneMode(bool enable) {
  m_clipPlaneModeSelected = enable;

  CGeomecDoc *doc = GetGeomecDoc();
  IWellModel *model = dynamic_cast<IWellModel *>(doc->Model());

  if (model)
    m_xsecGroup->SelectClipPlaneMode(enable);
  else if (m_currentCrossSection)
    m_currentCrossSection->clip = enable;
}

bool OIDIMeshNodeManager::ContourLineModeSelected() const {
  return m_meshNodeSettings->contourLineModeSelected && CanSelectContourLineMode();
}

bool OIDIMeshNodeManager::CanSelectContourLineMode() const {
  return m_meshNodeSettings->meshMode == SkinMesh && CanShowValue() /* && !PointSetSelected() */;
}

void OIDIMeshNodeManager::SelectContourLineMode(bool enable) {
  m_meshNodeSettings->contourLineModeSelected = enable;
  UpdateDisplaySettings();
  UpdateValuesLegendAdornments();
}

bool OIDIMeshNodeManager::MeshLinesDisplayed() const {
  return m_meshNodeSettings->meshLinesDisplayed && CanSelectMeshLineDisplayMode();
}

bool OIDIMeshNodeManager::CanSelectMeshLineDisplayMode() const { return true; }

void OIDIMeshNodeManager::SelectMeshLinesDisplayed(bool enable) {
  m_meshNodeSettings->meshLinesDisplayed = enable;
  UpdateDisplaySettings();
}

bool OIDIMeshNodeManager::FormationOutlinesDisplayed() const {
  return m_meshNodeSettings->formationOutlinesDisplayed && CanSelectFormationOutlineDisplayMode();
}

bool OIDIMeshNodeManager::CanSelectFormationOutlineDisplayMode() const { return true; }

void OIDIMeshNodeManager::SelectFormationOutlinesDisplayed(bool enable) {
  m_meshNodeSettings->formationOutlinesDisplayed = enable;
  UpdateDisplaySettings();
}

bool OIDIMeshNodeManager::MeshesBeingDisplayed() const { return m_displayedMeshes->getNumChildren() != 0; }

void OIDIMeshNodeManager::Fill(bool bFill) {
  m_meshNodeSettings->meshFilled = bFill;
  UpdateDisplaySettings();
}

// true -> currently filled
// false -> currently line
bool OIDIMeshNodeManager::Fill() const { return m_meshNodeSettings->meshFilled; }

bool OIDIMeshNodeManager::CanFill() const { return MeshesBeingDisplayed(); }

void OIDIMeshNodeManager::ViewTensorVector(TensorVectorMode mode) {
  m_meshNodeSettings->tensorVectorMode = mode;
  UpdateDisplaySettings();
}

TensorVectorMode OIDIMeshNodeManager::ViewTensorVector() { return m_meshNodeSettings->tensorVectorMode; }

void OIDIMeshNodeManager::ViewProportional(bool bProportional) {
  if (bProportional) {
    m_scale->scaleFactor.setValue(1, 1, 1);
  } else {
    m_scale->scaleFactor.setValue(1, 1, 2);
  }
}

// true -> currently filled
// false -> currently line
bool OIDIMeshNodeManager::ViewProportional() const {
  const SbVec3f &scale = m_scale->scaleFactor.getValue();
  return (scale[2] == 1);
}

void OIDIMeshNodeManager::ShowTopView(bool show) {
  m_showTopView = show;
  m_openInventorEventsHandler->CheckCamera();
}

bool OIDIMeshNodeManager::ShowTopView() { return m_showTopView; }

/**
 * Display the legend
 */
void OIDIMeshNodeManager::DisplayValuesLegend() {
  if (!scalarSetsAvailable())
    return;

  MakeValuedMeshesVisible();

  ShowColor(false);
  switchToValueColors();
  UpdateDisplaySettings();
  // m_legendScene->displayValuesLegend();
}

void OIDIMeshNodeManager::DisplayGeologyLegend() {
  MakeAllMeshesVisible();

  ShowColor(true);
  switchToGeologyColors();
  UpdateDisplaySettings();
  // m_legendScene->displayGeologyLegend();
}

void OIDIMeshNodeManager::UpdateValuesLegend() {
  if (!scalarSetsAvailable())
    return;

  OnColorScaleChanged();
  DisplayValuesLegend();
  UpdateValueTrackerDisplay();

  // m_legendScene->updateValuesLegend(m_openInventorEventsHandler->Width(), m_openInventorEventsHandler->Height(), min,
  // max);
};

void OIDIMeshNodeManager::UpdateValuesLegendAdornments() {
  // UpdateValuesLegendAdornments(m_pValueComponentDisplayed);
}

void OIDIMeshNodeManager::UpdateValuesLegendAdornments(const IValueComponentBase *valueComponent) {
  /*
    // structure and content inspired by COpenGLSceneBase::OnUpdateValueLegend()
    if (!m_legendScene) return;
    m_legendScene->clearValuesAdornments();

    if (valueComponent == 0) return;

    CreateColorScaleAdornment();

    AddValuesLegendAdornment("");
    AddValuesLegendAdornment("-- Coloring property --");


    if(getFirstScalarDataSet())
    {
      std::string valueBinding = "Value binding: ";
      valueBinding += (getFirstScalarDataSet()->getBinding() == MiDataSet::PER_CELL) ? "per Cell" : "per Node";
      AddValuesLegendAdornment(valueBinding);
    }

    CreatePropertyAdornments(valueComponent);

    if (ContourLineModeSelected() && m_pIsoValueComponentDisplayed)
    {
      AddValuesLegendAdornment("");
      AddValuesLegendAdornment("-- Contour line property --");


      if(getLastScalarDataSet())
      {
        std::string valueBinding = "Value binding: ";
        valueBinding += (getLastScalarDataSet()->getBinding() == MiDataSet::PER_CELL) ? "per Cell" : "per Node";
        AddValuesLegendAdornment(valueBinding);
      }

      CreatePropertyAdornments(m_pIsoValueComponentDisplayed);
    }

    UpdateValuesLegend();
  */
}

const std::string OIDIMeshNodeManager::GetLegendTitle() const { return m_legendScene->getTitle(); }

void OIDIMeshNodeManager::SetTextColor(float *rgb) {
  m_legendScene->SetTextColor(rgb);
  m_valueTracker->SetTextColor(rgb);
}

void OIDIMeshNodeManager::GetTextColor(float *rgb) { m_legendScene->GetTextColor(rgb); }

void OIDIMeshNodeManager::UpdateValueTrackerDisplay() {
  m_crossSectionManip->updateDraggerPosition();

  m_valueTracker->Display(m_mouseTrackerSelected || m_draggerTrackerSelected);

  m_valueTracker->showDragger(m_draggerTrackerSelected);
  m_meshPointProbeGroup->enable(m_draggerTrackerSelected);
}

std::string OIDIMeshNodeManager::GetName(const CGraphNode &node) const { return node.Name().toStdString(); }

void OIDIMeshNodeManager::GetColorScaleRange(double &minimum, double &maximum) {
  const CColorScaleEntry *colorScaleEntry = m_openInventorEventsHandler->GetColorScaleEntry();
  if (colorScaleEntry->ColorScaleType() == CColorScaleEntry::LOCAL) {
    const CColorGradient *colorGradient = colorScaleEntry->LocalColorGradient();

    double gradientMin = colorGradient->MinMaxValue().first;
    double gradientMax = colorGradient->MinMaxValue().second;

    if (!scalarSetsAvailable()) {
      minimum = gradientMin;
      maximum = gradientMax;
    } else {
      GetMeshDataMinMax(minimum, maximum);
      if (fabs(maximum - minimum) < 1E-11) {
        double epsilon = 1.E-11 * (std::abs(minimum) < 1.E-4 ? 1 : std::abs(minimum));
        minimum -= epsilon;
        maximum += epsilon;
      }
    }
  } else if (colorScaleEntry->ColorScaleType() == CColorScaleEntry::GLOBAL) {
    CColorGradient *colorGradient = const_cast<CColorGradient *>(colorScaleEntry->GlobalColorGradient());

    double gradientMin = colorGradient->MinMaxValue().first;
    double gradientMax = colorGradient->MinMaxValue().second;

    if (!scalarSetsAvailable() || !colorGradient->AutoExtremes()) {
      minimum = gradientMin;
      maximum = gradientMax;
    } else {
      GetMeshDataMinMax(minimum, maximum);
      if (fabs(maximum - minimum) < 1E-11) {
        double epsilon = 1.E-11 * (std::abs(minimum) < 1.E-4 ? 1 : std::abs(minimum));
        minimum -= epsilon;
        maximum += epsilon;
      }
    }
  } else if (colorScaleEntry->ColorScaleType() == CColorScaleEntry::HOTSPOT) {
    const CHotSpot *pHotSpot = colorScaleEntry->HotSpot();
    assert(pHotSpot); // has to be a hotspot...

    double hotspotMin = pHotSpot->MinMaxValue().first;
    double hotspotMax = pHotSpot->MinMaxValue().second;

    if (!scalarSetsAvailable()) {
      minimum = hotspotMin;
      maximum = hotspotMax;
    } else {
      GetMeshDataMinMax(minimum, maximum);
      if (fabs(maximum - minimum) < 1E-11) {
        double epsilon = 1.E-11 * (std::abs(minimum) < 1.E-4 ? 1 : std::abs(minimum));
        minimum -= epsilon;
        maximum += epsilon;
      }
    }
  } else
    assert(false);

  assert(minimum < maximum);
}

void OIDIMeshNodeManager::UpdateColorMapping() {
  const CColorScaleEntry *colorScaleEntry = m_openInventorEventsHandler->GetColorScaleEntry();
  double min, max;
  GetColorScaleRange(min, max);

  MoColorMapping *colorMapping = CreateColorMapping(min, max, colorScaleEntry);

  assert(m_valueColorMap != 0);
  m_valueGroup->replaceChild(m_valueColorMap, colorMapping);

  m_valueColorMap = colorMapping;
}

void OIDIMeshNodeManager::AddValuesLegendAdornment(std::string adornment) {
  m_legendScene->addValuesAdornment(adornment);
}

void OIDIMeshNodeManager::AddLegendPropertyContextAdornments(const IResultComponent *pResultComponent) {
  AddValuesLegendAdornment("Property:");

  std::vector<std::string> context;
  const IResult &result = dynamic_cast<const IResult &>(pResultComponent->Parent());
  const std::string resultName = result.Name().toStdString();

  const std::string componentName = pResultComponent->Name().toStdString();
  context.push_back(componentName);

  if (componentName != resultName)
    context.push_back(resultName);

  const CResultGroup *pGroup = result.Parent();
  while (pGroup) {
    context.push_back(pGroup->Name().toStdString());
    pGroup = pGroup->Parent();
  }

  std::vector<std::string>::reverse_iterator contextIter;

  int contextIndex;

  for (contextIter = context.rbegin(), contextIndex = 0; contextIter != context.rend(); ++contextIter, ++contextIndex) {
    std::string contextAdornment;
    if (*contextIter == "")
      continue;
    for (int i = 0; i < contextIndex; ++i) // indenting
    {
      contextAdornment += "  ";
    }
    contextAdornment += "\xBB ";
    contextAdornment += *contextIter;
    AddValuesLegendAdornment(contextAdornment);
  }
}

void OIDIMeshNodeManager::GetMeshDataMinMax(double &minimum, double &maximum) {
  GetDataMinMax(ColorComponent, minimum, maximum);
}

void OIDIMeshNodeManager::GetIsoDataMinMax(double &minimum, double &maximum) {
  GetDataMinMax(IsoComponent, minimum, maximum);
}

void OIDIMeshNodeManager::GetDataMinMax(ComponentType componentType, double &minimum, double &maximum) {
  minimum = DBL_MAX;
  maximum = -DBL_MAX;

  if (scalarSetsAvailable()) {
    if (GetNumberOfCrossSectionsDisplayed() > 0 || m_displayedMeshes->getNumChildren() == 0) {
      // Get min-max values for all volume meshes
      for (auto i : m_meshInfoMap) {
        int id = (componentType == ColorComponent) ? i.second.colorScalarSetId : i.second.isoScalarSetId;

        if (id == -1)
          continue;

        MoScalarSetI *scalarSet = dynamic_cast<MoScalarSetI *>(m_valueScalarSetGroup->getChild(id));
        const MiDataSetI<double> *dataSet = scalarSet->getScalarSet();

        updateMinMax(dataSet, minimum, maximum);
      }
    } else {
      for (auto node : m_displayedMeshes) {
        assert(dynamic_cast<OIDIMeshNode *>(node) != nullptr);
        OIDIMeshNode *meshNode = static_cast<OIDIMeshNode *>(node);
        UpdateDataMinMaxForNode(meshNode, componentType, minimum, maximum);
      }
    }
  }

  if (minimum > maximum) {
    minimum = DBL_UNDEFINED;
    maximum = DBL_UNDEFINED;
  }
}

void OIDIMeshNodeManager::UpdateDataMinMaxForNode(OIDIMeshNode *meshNode, ComponentType componentType, double &minimum,
                                                  double &maximum) {
  if (!meshNode)
    return;

  int scalarSetId =
      (componentType == ColorComponent) ? meshNode->getValueColorScalarSetId() : meshNode->getIsoScalarSetId();

  if (scalarSetId == -1)
    return;

  MoScalarSetI *scalarSet = dynamic_cast<MoScalarSetI *>(m_valueScalarSetGroup->getChild(scalarSetId));
  assert(scalarSet);

  const MiDataSetI<double> *dataSet = scalarSet->getScalarSet();
  assert(dataSet);

  updateMinMax(dataSet, minimum, maximum);
}

bool OIDIMeshNodeManager::scalarSetsAvailable() {
  bool scalarSetsAvailable = getActiveScalarSetGroup() != 0 && getActiveScalarSetGroup()->getNumChildren() > 0;
  return scalarSetsAvailable;
}

bool OIDIMeshNodeManager::vectorSetsAvailable() {
  bool vectorSetsAvailable = m_vectorSetGroup != 0 && m_vectorSetGroup->getNumChildren() > 0;
  return vectorSetsAvailable;
}

bool OIDIMeshNodeManager::tensorSetsAvailable() {
  bool tensorSetsAvailable = m_tensorSetGroup != 0 && m_tensorSetGroup->getNumChildren() > 0;
  return tensorSetsAvailable;
}

void OIDIMeshNodeManager::RemoveAllMeshes() {
  m_displayedMeshes->removeAllChildren();
  m_meshSet.clear();
}

OIDIMeshNode *OIDIMeshNodeManager::CreateMeshNode(const OIDIMesh *mesh) {
  switch (mesh->meshType()) {
  case OIDIMesh::VolumeMesh:
    return new OIDIVolumeMeshNode(mesh, this);
  case OIDIMesh::SurfaceMesh:
  case OIDIMesh::PointSetMesh:
  case OIDIMesh::PointMesh:
    if (dynamic_cast<const CPressureSupportNode *>(mesh->getOpenGLNode()) != 0)
      return new OIDISurfaceMeshOutlineNode(mesh, this);
    else
      return new OIDISurfaceMeshNode(mesh, this);
  case OIDIMesh::LineMesh:
    return new OIDILineMeshNode(mesh, this);
  default:
    assert(false);
    return 0;
  }
}

void OIDIMeshNodeManager::ShowGeologyLegend() {
  // Hide value legend and show geology legend
  DisplayGeologyLegend();
}

// determines if this is the mesh to contain all meshes ...
bool OIDIMeshNodeManager::IsMainMesh(const COpenGLNode &node) {
  return (dynamic_cast<const CTetraMesh *>(&node) != 0 || dynamic_cast<const CHexaMesh *>(&node) != 0);
}

// type, value and name are  used for sorting the legend items
void OIDIMeshNodeManager::addToGeologyLegend(const COpenGLNode &node) {
  // m_legendScene->addGeologyLegendItem(& node);
  UpdateGeologyLegend();
}

void OIDIMeshNodeManager::removeFromGeologyLegend(const COpenGLNode &node) {
  // m_legendScene->removeGeologyLegendItem(& node);
  UpdateGeologyLegend();
}

void OIDIMeshNodeManager::ModifyGeologyLegend(const COpenGLNode &node) {
  // m_legendScene->refreshGeologyLegendItem(& node);
  UpdateGeologyLegend();
}

void OIDIMeshNodeManager::UpdateGeologyLegend() {
  // m_legendScene->updateGeologyLegend();
}

void OIDIMeshNodeManager::UpdateBoundingBox(OIDIMeshNode *meshNode, SbBox3f &bboxOverall) {
  SbBox3f bbox = meshNode->getBoundingBox();
  bboxOverall.extendBy(bbox);
}

void OIDIMeshNodeManager::SetTranslation(bool on) {
  CGeomecDoc *doc = GetGeomecDoc();
  CModelBase *model = static_cast<CModelBase *>(doc->Model());
  CMeshBase *mesh = model ? &model->Mesh() : 0;

  SbVec3d old_translation(m_translation[0], m_translation[1], m_translation[2]);

  if (on && mesh && mesh->IsMesh()) {
    geo::CPoint min = mesh->Mesh().Min();
    geo::CPoint max = mesh->Mesh().Max();

    m_translation.setValue((min.X() + max.X()) / 2, (min.Y() + max.Y()) / 2, (min.Z() + max.Z()) / 2);
    RefinementBox::m_translation = {m_translation[0], m_translation[1], m_translation[2]};
  } else {
    m_translation.setValue(0, 0, 0);
  }

  if (m_translation[0] != old_translation[0] || m_translation[1] != old_translation[1] ||
      m_translation[2] != old_translation[2]) {
    m_meshSet.Translate(m_translation);
    for (TNewWellPathMap::iterator it = m_newWellPathMap.begin(); it != m_newWellPathMap.end(); ++it) {
      OnWellPathChanged(it);
    }
  }
}

const SbVec3d &OIDIMeshNodeManager::GetTranslation() { return m_translation; }

// whether geology colors are being displayed
bool OIDIMeshNodeManager::ShowColor() {
  bool showingColor =
      m_geologyOrValueSwitch->whichChild.getValue() == 0 || m_valueScalarSetGroup->getNumChildren() == 0;
  return showingColor;
}

// set whether geology colors are to be displayed
void OIDIMeshNodeManager::ShowColor(bool show) {
  if (show) {
    m_geologyOrValueSwitch->whichChild = 0; // geology
  } else if (m_valueScalarSetGroup->getNumChildren() != 0) {
    m_geologyOrValueSwitch->whichChild = 1; // value
  }
}

bool OIDIMeshNodeManager::CanShowValue() const { return m_pValueComponentDisplayed != 0; }

void OIDIMeshNodeManager::PointSize(double m_pointSize) { m_wellPathFont->size = (float)m_pointSize; }

void OIDIMeshNodeManager::SetVectorScale(float scale) {
  m_meshNodeSettings->vectorScale = scale;
  UpdateDisplaySettings();
}

void OIDIMeshNodeManager::SetVectorBaseScale(float scale) {
  m_meshNodeSettings->vectorBaseScale = scale;
  UpdateDisplaySettings();
}

void OIDIMeshNodeManager::SetBeachBallBaseScale(float scale) {
  m_meshNodeSettings->beachBallBaseScale = scale;
  UpdateDisplaySettings();
}

void OIDIMeshNodeManager::SetVectorVisibleFraction(float factor) {
  m_meshNodeSettings->vectorVisibleFraction = factor;
  UpdateDisplaySettings();
}

void OIDIMeshNodeManager::UpdateDisplaySettings() {
  for (auto node : m_displayedMeshes) {
    assert(dynamic_cast<OIDIMeshNode *>(node) != nullptr);
    OIDIMeshNode *meshNode = static_cast<OIDIMeshNode *>(node);
    meshNode->UpdateDisplaySettings(*m_meshNodeSettings, ShowColor());
  }

  for (auto node : m_xsecGroup) {
    MeshCrossSection *mcs = static_cast<MeshCrossSection *>(node);
    mcs->updateDisplaySettings(*m_meshNodeSettings);
  }
}

void OIDIMeshNodeManager::unlinkValueComponent() { m_openInventorEventsHandler->unlinkValueComponent(); }

void OIDIMeshNodeManager::MakeAllMeshesVisible() {
  for (auto node : m_displayedMeshes) {
    assert(dynamic_cast<OIDIMeshNode *>(node) != nullptr);
    OIDIMeshNode *meshNode = static_cast<OIDIMeshNode *>(node);
    meshNode->setVisibility(true);
  }
}

void OIDIMeshNodeManager::MakeValuedMeshesVisible() {
  for (auto node : m_displayedMeshes) {
    assert(dynamic_cast<OIDIMeshNode *>(node) != nullptr);
    OIDIMeshNode *meshNode = static_cast<OIDIMeshNode *>(node);
    meshNode->setVisibility(meshNode->getValueColorScalarSetId() >= 0);
  }
}

OIDIMeshNode *OIDIMeshNodeManager::FindOrCreateMeshNode(const OIDIMesh *mesh) {
  OIDIMeshNode *meshNode;
  if ((meshNode = FindMeshNode(m_displayedMeshes, mesh)) == 0) {
    meshNode = CreateMeshNode(mesh);
    m_displayedMeshes->addChild(meshNode);
  }
  return meshNode;
}

COpenGLNode *OIDIMeshNodeManager::getOpenGLNode(const OIDIMesh *mesh) {
  const COpenGLNode *node = mesh->getOpenGLNode();
  return const_cast<COpenGLNode *>(node);
}

void OIDIMeshNodeManager::setVectorDataSetNormal(int vectorSetId, SbVec3f normal) {
  if (vectorSetId < 0 || vectorSetId >= m_vectorSetGroup->getNumChildren())
    return;

  SoNode *child = m_vectorSetGroup->getChild(vectorSetId);
  MoVec3SetI *vectorSet = dynamic_cast<MoVec3SetI *>(child);
  if (!vectorSet)
    return;

  OIDIVectorSetI *vectorDataSet =
      const_cast<OIDIVectorSetI *>(dynamic_cast<const OIDIVectorSetI *>(vectorSet->getVec3Set()));
  assert(vectorDataSet);

  vectorDataSet->setNormal(normal, m_meshNodeSettings->tensorVectorMode);
}

void OIDIMeshNodeManager::setVectorSignDataSetNormal(int scalarSetId, SbVec3f normal) {
  if (scalarSetId < 0)
    return;
  SoNode *child = m_valueScalarSetGroup->getChild(scalarSetId);
  MoScalarSetI *scalarSet = dynamic_cast<MoScalarSetI *>(child);
  if (!scalarSet)
    return;

  OIDIScalarSetI *scalarDataSet =
      const_cast<OIDIScalarSetI *>(dynamic_cast<const OIDIScalarSetI *>(scalarSet->getScalarSet()));
  assert(scalarDataSet);

  scalarDataSet->setNormal(normal, m_meshNodeSettings->tensorVectorMode);
}

bool OIDIMeshNodeManager::isTensorVectorSignDataSet(int scalarSetId) {
  if (scalarSetId < 0)
    return false;
  SoNode *child = m_valueScalarSetGroup->getChild(scalarSetId);
  MoScalarSetI *scalarSet = dynamic_cast<MoScalarSetI *>(child);
  if (!scalarSet)
    return false;

  OIDIScalarSetI *scalarDataSet =
      const_cast<OIDIScalarSetI *>(dynamic_cast<const OIDIScalarSetI *>(scalarSet->getScalarSet()));
  assert(scalarDataSet);

  return scalarDataSet->isTensorVectorSignDataSet();
}

void OIDIMeshNodeManager::CreatePropertyAdornments(const IValueComponentBase *valueComponent) {
  if (!valueComponent)
    return;
  const IResultComponent *resultComponent = dynamic_cast<const IResultComponent *>(valueComponent);

  if (resultComponent) {
    AddLegendPropertyContextAdornments(resultComponent);

    std::string depletionStage = "Depletion stage: ";
    depletionStage += resultComponent->Stage().Name().toStdString();
    AddValuesLegendAdornment(depletionStage);

    if (resultComponent->AnalysisType().Valid()) {
      std::string analysis = "Analysis: ";
      analysis += resultComponent->AnalysisType().Label().toStdString();
      AddValuesLegendAdornment(analysis);

#if 0
      if (resultComponent->AnalysisType() == CAnalysisType::AT_NONLIN)
      {
        AddValuesLegendAdornment("Step type: Time");
      }
#endif
    }
  } else {
    std::string valueName = "Property: ";
    if (valueComponent->Parent().TypeId() && !dynamic_cast<const CMeshResult *>(&valueComponent->Parent()))
      valueName += valueComponent->Parent().TypeName().toStdString();
    else
      valueName += valueComponent->Parent().Name().toStdString(); // Derived result

    AddValuesLegendAdornment(valueName);

    if (valueComponent->Parent().ComponentSize() != 1) {
      std::string component = "Component: ";
      component += valueComponent->Name().toStdString();
      AddValuesLegendAdornment(component);
    }
  }

  CGeomecDoc &doc = *GetGeomecDoc();

#if 0
  std::string unitType = "Unit type: ";
  unitType += doc.UnitNode().Name().toStdString();
  AddValuesLegendAdornment(unitType);
#endif
  std::string unit = "Unit: ";
  unit += valueComponent->UnitName(doc.UnitNode().Unit()).toStdString();
  AddValuesLegendAdornment(unit);
}

void OIDIMeshNodeManager::CreateColorScaleAdornment() {
  const CColorScaleEntry *colorScaleEntry = m_openInventorEventsHandler->GetColorScaleEntry();

  std::string colorScale = "Color Scale: ";

  switch (colorScaleEntry->ColorScaleType()) {
  case CColorScaleEntry::LOCAL:
    colorScale += "Local";
    break;
  case CColorScaleEntry::GLOBAL:
    colorScale += "Global (";
    colorScale += colorScaleEntry->CurrentColorScale().Name().toStdString();
    colorScale += ")";
    break;
  case CColorScaleEntry::HOTSPOT:
    colorScale += "Hotspot (";
    colorScale += colorScaleEntry->CurrentColorScale().Name().toStdString();
    colorScale += ")";
    break;
  default:
    assert(false);
    break;
  }
  AddValuesLegendAdornment(colorScale);
}

#if 0
void OIDIMeshNodeManager::UpdateMeshBoundingBox( SoGroup * group, SbBox3f & bboxOverall )
{
  SbBox3f bbox = GetGroupBoundingBox(group);
  bboxOverall.extendBy(bbox);
}
#endif

MoColorMapping *OIDIMeshNodeManager::GetTensorVectorColorMapping() {
  if (!s_tensorVectorColorMapping) {
    MoLevelColorMapping *colorMapping = new MoLevelColorMapping;

    float zeroThreshold = 1e-20f;
    float oneThreshold = 1.0f;
    colorMapping->values.set1Value(0, -oneThreshold);
    colorMapping->colors.set1Value(0, 1.0f, 0.0f, 0.0f, 1.0f);
    colorMapping->values.set1Value(1, -zeroThreshold);
    colorMapping->colors.set1Value(1, 1.0f, 1.0f, 0.0f, 1.0f);
    colorMapping->values.set1Value(2, zeroThreshold);
    colorMapping->colors.set1Value(2, 0.0f, 1.0f, 0.0f, 1.0f);
    colorMapping->values.set1Value(3, oneThreshold);

    colorMapping->ref();

    s_tensorVectorColorMapping = colorMapping;
  }

  return s_tensorVectorColorMapping;
}

void OIDIMeshNodeManager::ActUponNoValuesDisplayed(AddedOrDeleted action) {
  if (!ValuesBeingDisplayed(action)) {
    removeDataSets();
    unlinkValueComponent();
    DisplayGeologyLegend();
  }
}

void OIDIMeshNodeManager::RefreshVectorBaseScale() {
  // Get maximum vector length in all data sets
  double maxLen = 0.0;
  double min, max;
  GetMeshDataMinMax(min, max); // TODO: write a GetMeshVectorMinMax()
  if (min != DBL_UNDEFINED && max != DBL_UNDEFINED)
    maxLen = std::max(std::abs(min), std::abs(max));
  //     for(int i=0; i < m_vectorSetGroup->getNumChildren(); ++i)
  //     {
  //         MoVec3SetI* child = reinterpret_cast<MoVec3SetI*>(m_vectorSetGroup->getChild(i));
  //         MbVec3d maxVec = child->getVec3Set()->getMax();
  //         if(maxVec[0] != -std::numeric_limits<double>::max() && maxVec[0] != DBL_UNDEFINED
  //             && maxVec.length() > maxLen)
  //             maxLen = maxVec.length();
  //     }

  // Compute a scale factor so that the largest vector is 10% of total mesh size
  if (maxLen == 0)
    maxLen = 1;
  SbBox3f bbox = GetAllMeshesBoundingBox();
  float scale = .1f * bbox.getSize().length() / maxLen;

  SetVectorBaseScale(scale);
}

void OIDIMeshNodeManager::RefreshTensorBaseScale() {
  SbBox3f bbox = GetAllMeshesBoundingBox();
  float bboxDiagonal = bbox.getSize().length();

  float numCells = GetNumVolumeMeshCells();
  float numCellsPerLength = std::powf((float)numCells, 0.333f);

  if (numCells < 1000)
    numCells = 1000;

  float scale = 0.1f * bboxDiagonal / numCellsPerLength;

  SetBeachBallBaseScale(scale);
}

bool OIDIMeshNodeManager::CanRefreshVectorBaseScale() {
  return true; // for now ...
}

double OIDIMeshNodeManager::computeDistanceClosestNode(const MiMeshUnstructuredI *mesh, const MbVec3d &coordinates,
                                                       size_t cellId, size_t &cniClosest) {
  return m_valueTracker->computeDistanceClosestNode(mesh, coordinates, cellId, cniClosest);
}

void OIDIMeshNodeManager::invalidateValueTrackerCellMarkers() { m_valueTracker->invalidateCellMarkers(); }

void OIDIMeshNodeManager::computeValueTrackerCellMarkerSet(const MiMeshUnstructuredI *mesh, const MbVec3d &coordinates,
                                                           size_t cellId, size_t &meshNodeIndexClosest,
                                                           MbVec3d &meshNodeCoordinatesClosest) {
  if (!mesh)
    return;
  m_valueTracker->computeCellMarkerSet(mesh, coordinates, cellId, meshNodeIndexClosest, meshNodeCoordinatesClosest);
}

void OIDIMeshNodeManager::computeValueTrackerCellFacetSet(const OIDIMeshVU *mesh, size_t cellId) {
  if (!mesh)
    return;
  m_valueTracker->computeCellFacetSet(mesh, cellId);
}

void OIDIMeshNodeManager::computeValueTrackerCellFacetSet(const OIDIMeshSU *mesh, size_t cellId) {
  if (!mesh)
    return;
  m_valueTracker->computeCellFacetSet(mesh, cellId);
}

void OIDIMeshNodeManager::updateValueTracker(const MiMeshUnstructuredI *mesh, const OIDIMesh *oidiMesh,
                                             const MbVec3d &coordinates, size_t cellId) {
  if (!mesh) {
    m_valueTracker->updateValueTracker(0, "", coordinates, cellId, -1, -1, -1);
  }
  if (!oidiMesh)
    return;

  COpenGLNode *oglNode = getOpenGLNode(oidiMesh);
  if (!oglNode)
    return;

  auto meshInfo = m_meshInfoMap[oidiMesh];

  int vectorSetId = meshInfo.eigenValueSignSetId < 0 ? meshInfo.vectorSetId : meshInfo.tensorVectorSetId;
  m_valueTracker->updateValueTracker(mesh, GetName(*oglNode), coordinates, cellId, meshInfo.colorScalarSetId,
                                     vectorSetId, meshInfo.tensorSetId);
}

void OIDIMeshNodeManager::handleContext(const OIDIMesh *mesh, size_t cellId, QPoint &point,
                                        const MbVec3d &coordinates) {
  if (!mesh)
    return;
  const COpenGLNode *oglNode = mesh->getOpenGLNode();

  std::vector<const geo::IObject *> vcHit;

  const geo::IObject *objectSet = mesh->getObjectSet();

  geo::CPoint geoCoordinates(coordinates[0], coordinates[1], coordinates[2]);

  const geo::CArray<geo::CLine> *lineArray1 = dynamic_cast<const geo::CArray<geo::CLine> *>(objectSet);
  if (lineArray1) {
    const geo::IPoint *nearestPoint;
    const geo::CLine &iLine1 = lineArray1->Object(cellId);

    const geo::IPoint &first1 = iLine1.First();
    const geo::IPoint &second1 = iLine1.Second();

    nearestPoint =
        (geoCoordinates.SquareDistance(first1) < geoCoordinates.SquareDistance(second1)) ? &first1 : &second1;

    // find and select all points in the meshes of oglnode with same coordinates.
    std::vector<const OIDIMesh *> meshes;
    m_meshSet.collectMeshes(*oglNode, meshes);

    for (std::size_t i = 0; i < meshes.size(); ++i) {
      const geo::CArray<geo::CLine> *lineArray2 =
          dynamic_cast<const geo::CArray<geo::CLine> *>(meshes[i]->getObjectSet());
      if (lineArray2) {
        for (size_t l = 0; l < lineArray2->Size(); ++l) {
          const geo::CLine &iLine2 = lineArray2->Object(l);

          const geo::IPoint &first2 = iLine2.First();
          const geo::IPoint &second2 = iLine2.Second();

          double firstDistance = nearestPoint->SquareDistance(first2);
          double secondDistance = nearestPoint->SquareDistance(second2);

          if (firstDistance < 1)
            vcHit.push_back(&first2);
          if (secondDistance < 1)
            vcHit.push_back(&second2);
        }
      }
    }
  } else
    vcHit.push_back(objectSet);

  CGeomecDoc *pDoc = GetGeomecDoc();
  if (!pDoc)
    return;
  CModelView *mv = pDoc->GetModelView();

  Qt::MouseButton button = Qt::RightButton;

  IOpenGLFrame::TKeyboardModifiers state = 0;

  COpenGLNode_Delegate *pNode =
      dynamic_cast<COpenGLNode_Delegate *>((const_cast<COpenGLNode *>(oglNode))->getDelegate());
  pNode->MouseRelease(*mv, state, button, point, vcHit);
}

void OIDIMeshNodeManager::computeValueTrackerCellFacetSet(const OIDIPointSetMesh *mesh, size_t cellId) {
  m_valueTracker->computeCellFacetSet(mesh, cellId);
}

void OIDIMeshNodeManager::clearValueTrackerCellFacetSet() { m_valueTracker->clearCellFacetSet(); }

bool OIDIMeshNodeManager::PointSetSelected() const { return m_pointSetCount != 0; }

void OIDIMeshNodeManager::PointSetWasDeselected(bool wasIt) {
  if (wasIt)
    --m_pointSetCount;
}

void OIDIMeshNodeManager::PointSetWasSelected(bool wasIt) {
  if (wasIt) {
    ++m_pointSetCount;
    m_openInventorEventsHandler->CheckLighting();
  }
}

void OIDIMeshNodeManager::switchToGeologyColors() {
  for (auto node : m_displayedMeshes) {
    OIDIMeshNode *meshNode = static_cast<OIDIMeshNode *>(node);
    meshNode->switchToGeologyColors();
  }

  for (auto node : m_xsecGroup) {
    MeshCrossSection *mcs = static_cast<MeshCrossSection *>(node);
    mcs->coloring = MoMaterial::COLOR;
  }
}

void OIDIMeshNodeManager::switchToValueColors() {
  for (auto node : m_displayedMeshes) {
    OIDIMeshNode *meshNode = static_cast<OIDIMeshNode *>(node);
    meshNode->switchToValueColors();
  }

  for (auto node : m_xsecGroup) {
    MeshCrossSection *mcs = static_cast<MeshCrossSection *>(node);
    mcs->coloring = MoMaterial::CONTOURING;
  }
}

bool OIDIMeshNodeManager::highlightMesh(const OIDIMesh *mesh) {
  if (!mesh)
    return false;

  OIDIMeshNode *meshNode = FindMeshNode(m_displayedMeshes, mesh);
  if (meshNode) {
    if (meshNode->highlight()) {
      m_highlightedNode = mesh->getOpenGLNode();
      m_highlightedMesh = mesh;
      return true;
    }
  }

  return false;
}

void OIDIMeshNodeManager::dehighlightMesh() {
  std::vector<const OIDIMesh *> meshes;
  if (!m_highlightedMesh)
    return;
  m_meshSet.collectMeshes(*m_highlightedNode, meshes);

  for (std::size_t i = 0; i < meshes.size(); ++i) {
    const OIDIMesh *mesh = meshes[i];
    if (mesh == m_highlightedMesh) {
      OIDIMeshNode *meshNode = FindMeshNode(m_displayedMeshes, mesh);
      if (meshNode)
        meshNode->dehighlight();
    }
  }
  m_highlightedMesh = 0;
  m_highlightedNode = 0;
}

bool OIDIMeshNodeManager::CanDisplay(const COpenGLNode *pOpenGLNode) {
  if (!m_meshSet.isEmpty())
    return true;

  // otherwise we need to be more careful?
  OIDISet meshSet;
  meshSet.handleNodeNew(*pOpenGLNode); // creates the node's meshes if necessary

  return !meshSet.isEmpty();
}

bool OIDIMeshNodeManager::IsVisible(const COpenGLNode *pOpenGLNode) {
  std::vector<const OIDIMesh *> meshes;
  m_meshSet.collectMeshes(*pOpenGLNode, meshes);

  for (std::size_t i = 0; i < meshes.size(); ++i) {
    const OIDIMesh *mesh = meshes[i];

    OIDIMeshNode *meshNode = FindMeshNode(m_displayedMeshes, mesh);
    if (meshNode && meshNode->isVisible())
      return true;
  }

  const CNewWellPath *pNewWellPath = dynamic_cast<const CNewWellPath *>(pOpenGLNode);
  if (pNewWellPath) {
    TNewWellPathMap::const_iterator it = m_newWellPathMap.find(pNewWellPath);
    return it != m_newWellPathMap.end() && it->second->whichChild.getValue() == SO_SWITCH_ALL;
  }

  return false;
}

bool OIDIMeshNodeManager::handleRightMousePress(const QPoint &point) {
  if (ShowColor())
    return false;

  CGeomecDoc &doc = *GetGeomecDoc();
  CModelView *mv = doc.GetModelView();

  SbBox2i32 legendBBox = m_legendScene->getValuesLegendBoundingBox();

  // y coordinates of bounding box are from the top down
  CRect rect;
  mv->GetClientRect(&rect);
  SbVec2i32 vec(point.x(), rect.Height() - point.y());
  if (!legendBBox.intersect(vec))
    return false;

  QPoint global = mv->GlobalScreenPoint(point);

  CPoint cp(global.x(), global.y());
  CLegendView::DoOnRButtonUp(cp, doc);

  return true;
}

namespace {

// the initial state of the CrossSectionManipulator object is active

class CCrossSectionManipulatorState {
public:
  CCrossSectionManipulatorState(CrossSectionManipulator *crossSectionManipulator);

  void activate(MeshCrossSection *currentCrossSection);
  void deactivate();

private:
  CCrossSectionManipulatorState(const CCrossSectionManipulatorState &rhs);
  CCrossSectionManipulatorState &operator=(CCrossSectionManipulatorState rhs);

  CrossSectionManipulator *m_crossSectionManipulator;
  SoSFBool m_enableCallbacks;
  SoNode *m_rotator;
  SoNode *m_scaler;
  SoNode *m_yTranslator;
  SoNode *m_xzTranslator;
  bool m_active;
};

const SbName ROTATOR = "rotator.rotator";
const SbName SCALER = "scaler.scaler";
const SbName YTRANSLATOR = "translator.yTranslator.translator";
const SbName XZTRANSLATOR = "translator.xzTranslator.translator";

CCrossSectionManipulatorState::CCrossSectionManipulatorState(CrossSectionManipulator *crossSectionManipulator)
    : m_crossSectionManipulator(crossSectionManipulator), m_enableCallbacks(m_crossSectionManipulator->enableCallbacks),
      m_rotator(m_crossSectionManipulator->getPart(ROTATOR, FALSE)),
      m_scaler(m_crossSectionManipulator->getPart(SCALER, FALSE)),
      m_yTranslator(m_crossSectionManipulator->getPart(YTRANSLATOR, FALSE)),
      m_xzTranslator(m_crossSectionManipulator->getPart(XZTRANSLATOR, FALSE)), m_active(true) {}

void CCrossSectionManipulatorState::activate(MeshCrossSection *currentCrossSection) {
  if (!m_active) {
    m_crossSectionManipulator->enableCallbacks = m_enableCallbacks;
    m_crossSectionManipulator->setPart(ROTATOR, m_rotator);
    m_crossSectionManipulator->setPart(SCALER, m_scaler);
    m_crossSectionManipulator->setPart(YTRANSLATOR, m_yTranslator);
    m_crossSectionManipulator->setPart(XZTRANSLATOR, m_xzTranslator);

    CCrossSection::OrientationType orientation = currentCrossSection->getOrientation();

    m_crossSectionManipulator->limitRotation(orientation);

    m_active = !m_active;
  }
}

void CCrossSectionManipulatorState::deactivate() {
  if (m_active) {
    // motion, start and finish callbacks are disabled
    m_crossSectionManipulator->enableCallbacks = FALSE;
    // The 'star' of the jack. Set of 3 white axes (lines)
    m_crossSectionManipulator->setPart(ROTATOR, new SoSeparator);
    // Six small white cubes, the tips of the rotator star
    m_crossSectionManipulator->setPart(SCALER, new SoSeparator);
    // Skinny white cylinder aligned with axes of motion
    m_crossSectionManipulator->setPart(YTRANSLATOR, new SoSeparator);
    // White flattened cube, drawn in wireframe
    m_crossSectionManipulator->setPart(XZTRANSLATOR, new SoSeparator);

    m_active = !m_active;
  }
}

} // anonymous namespace

void OIDIMeshNodeManager::UpdateCrossSectionManip(const CCrossSection &xsec) {
  static CCrossSectionManipulatorState crossSectionManipulatorState(m_crossSectionManip);

  // initialize the cross section manipulator with the values of the current cross section
  SbBox3f bbox = GetAllMeshesBoundingBox();
  // print_bounding_box( "all   ", bbox_orig );

  // SbBox3f bbox = get_xsec_manipulator_casing_mesh_bounding_box();
  // print_bounding_box( "casing", bbox );

  m_crossSectionManip->setBoundingBox(bbox);

  const geo::IPoint &basepoint = xsec.IntersectPlaneScreen().BasePoint();
  SbVec3f basepointVec(basepoint.X() - m_translation[0], basepoint.Y() - m_translation[1],
                       basepoint.Z() - m_translation[2]);

  const geo::IVector &normal = xsec.IntersectPlaneScreen().Normal();
  SbVec3f normalVec = SbVec3f(normal.X(), normal.Y(), normal.Z());

  crossSectionManipulatorState.activate(m_currentCrossSection);

  m_crossSectionManip->setPlane(basepointVec, normalVec);
  m_crossSectionManip->limitRotation(xsec.Orientation());
}

OIDIMeshNode *OIDIMeshNodeManager::getMainMeshGroupMeshNode() const { return 0; }

// Note: this time-out mechanism works only within the OIV view; if it were implemented across views, it might be useful
// enough to replace the Enter/LeaveView functionality
void OIDIMeshNodeManager::timeOut() {
  if (!empty(m_xsecGroup)) {
    m_crossSectionManipSwitch->whichChild = SO_SWITCH_NONE;
  }

  m_valueTracker->showDragger(false);
}

void OIDIMeshNodeManager::resetTimeOut(bool force) {
  if (force) {
    m_crossSectionManipSwitch->whichChild = SO_SWITCH_ALL;
  } else {
    if (!empty(m_xsecGroup)) {
      m_crossSectionManipSwitch->whichChild = (m_inView && m_showTracker) ? SO_SWITCH_ALL : SO_SWITCH_NONE;
    }

    m_valueTracker->showDragger(m_draggerTrackerSelected);
  }
}

void OIDIMeshNodeManager::restoreValueComponent(IValueComponentBase *pValueComponent) {
  m_pValueComponentDisplayed = pValueComponent;
  m_pIsoValueComponentDisplayed = pValueComponent;
}

const OIDISet &OIDIMeshNodeManager::getMeshSet() const { return m_meshSet; }

SoGroup *OIDIMeshNodeManager::getMeshes() { return m_meshes; }
SoGroup *OIDIMeshNodeManager::getDisplayedMeshes() { return m_displayedMeshes; }

SoGroup *OIDIMeshNodeManager::getMeshPointProbeGroup() { return m_meshPointProbeGroup; }

std::size_t OIDIMeshNodeManager::GetNumberOfCrossSectionsDisplayed() { return m_xsecGroup->getNumChildren(); }

int OIDIMeshNodeManager::NumberOfOpenGLNodesLinked() {
  return m_openInventorEventsHandler->NumberOfOpenGLNodesLinked();
}

void OIDIMeshNodeManager::updateMinMax(const MiDataSetI<double> *dataSet, double &minimum, double &maximum) {
  double dataSetMinimum = dataSet->getMin();
  double dataSetMaximum = dataSet->getMax();

  if (dataSetMinimum != DBL_UNDEFINED && dataSetMaximum != DBL_UNDEFINED && dataSetMaximum >= dataSetMinimum) {
    minimum = std::min(minimum, dataSetMinimum);
    maximum = std::max(maximum, dataSetMaximum);
  }
}

// adjust them so they are not too close
void OIDIMeshNodeManager::adjustMinMax(double &minimum, double &maximum) {
  if (maximum == DBL_UNDEFINED || minimum == DBL_UNDEFINED)
    return;

  double maxTmp = maximum;
  double minTmp = minimum;

  if (minTmp == maxTmp) {
    minTmp -= std::abs(minTmp) < 1e-9 ? 1e-10 : 0.1 * std::abs(minTmp);
    maxTmp += std::abs(maxTmp) < 1e-9 ? 1e-10 : 0.1 * std::abs(maxTmp);
  }

  double range = maxTmp - minTmp;
  if (minTmp != 0) {
    if (range / std::abs(minTmp) < 0.01) {
      minTmp -= 0.1 * std::abs(minTmp);
    }
  }

  if (maxTmp != 0) {
    if (range / std::abs(maxTmp) < 0.01) {
      maxTmp += 0.1 * std::abs(maxTmp);
    }
  }

  minimum = minTmp;
  maximum = maxTmp;
}

OIDIScalarSetI *CollectValues(const OIDIMesh *mesh, const IValueComponentBase *pValueComponent,
                              IQuantityDouble::UNIT unit, bool collectByTopology, IProgressBase &progress) {
  if (collectByTopology)
    return mesh->collectValuesByTopology(pValueComponent, unit, progress);
  else
    return mesh->collectValuesByGeometry(pValueComponent, unit, progress);
}

OIDIVectorSetI *CollectVectors(const OIDIMesh *mesh, const IVectorResult::IVectorComponent *pVectorComponent,
                               IQuantityDouble::UNIT unit, bool collectByTopology, IProgressBase &progress) {
  if (collectByTopology)
    return mesh->collectVectorsByTopology(pVectorComponent, unit, progress);
  else
    return mesh->collectVectorsByGeometry(pVectorComponent, unit, progress);
}

OIDITensorSetI *CollectTensors(const OIDIMesh *mesh, const IValueComponentBase *pTensorComponent,
                               IQuantityDouble::UNIT unit, bool collectByTopology, IProgressBase &progress) {
  if (collectByTopology)
    return mesh->collectTensorsByTopology(pTensorComponent, unit, progress);
  else
    return mesh->collectTensorsByGeometry(pTensorComponent, unit, progress);
}

MoColorMapping *CreateColorMapping(double min, double max, const CColorScaleEntry *colorScaleEntry) {
  bool defined = (min != DBL_UNDEFINED && max != DBL_UNDEFINED);

  if (!defined || colorScaleEntry->ColorScaleType() == CColorScaleEntry::LOCAL ||
      colorScaleEntry->ColorScaleType() == CColorScaleEntry::GLOBAL) {
    const SbVec4d undefinedColor(0.6f, 0.5f, 0.5f, 0.1f);
    MoLinearColorMapping *colorMapping = new MoLinearColorMapping;
    colorMapping->setName("ValueColorMap");

    if (defined) {
      colorMapping->maxThresholdEnable = true;
      colorMapping->maxThresholdValue = (float)(max + std::abs(DBL_UNDEFINED) / 2);
      colorMapping->maxThresholdColor.setValue(undefinedColor);

      colorMapping->minThresholdEnable = true;
      colorMapping->minThresholdValue = (float)(min - std::abs(DBL_UNDEFINED) / 2);
      colorMapping->minThresholdColor.setValue(undefinedColor);

      CColorGradient *colorGradient = const_cast<CColorGradient *>(colorScaleEntry->CurrentColorGradient());
      assert(colorGradient);
      if (colorGradient->AutoExtremes()) {
        colorGradient->SetMinMaxValue(min, max);
      }

      int index = 0;
      for (CColorGradient::color_iterator colorIterator = colorGradient->Begin(); colorIterator != colorGradient->End();
           ++colorIterator, ++index) {
        double gradientValue = (*colorIterator).first;

        float red, green, blue;
        DecomposeQColor((*colorIterator).second, red, green, blue);
        colorMapping->colors.set1Value(index, red, green, blue, 1.0f);

        float fGradientValue = (float)gradientValue;
        colorMapping->values.set1Value(index, fGradientValue);
      }
    } else {
      // all colors around the undefined value are greyish
      int numColors = 3;
      double finalValue = min + numColors / 2.0;
      for (int index = 0; index < numColors; ++index) {
        colorMapping->colors.set1Value(index, undefinedColor);
        colorMapping->values.set1Value(index, finalValue - index);
      }
    }

    return colorMapping;
  } else if (colorScaleEntry->ColorScaleType() == CColorScaleEntry::HOTSPOT) {
    const CHotSpot *pHotSpot = colorScaleEntry->HotSpot();
    assert(pHotSpot); // has to be a hotspot...

    double hotspotMin = pHotSpot->MinMaxValue().first;
    double hotspotMax = pHotSpot->MinMaxValue().second;

    float redMin = qRed(pHotSpot->MinColor()) / 255.0f;
    float greenMin = qGreen(pHotSpot->MinColor()) / 255.0f;
    float blueMin = qBlue(pHotSpot->MinColor()) / 255.0f;

    float redMax = qRed(pHotSpot->MaxColor()) / 255.0f;
    float greenMax = qGreen(pHotSpot->MaxColor()) / 255.0f;
    float blueMax = qBlue(pHotSpot->MaxColor()) / 255.0f;

    const float opacity = 0.1f; // does not seem to work very well ...
    double fudge =
        (max - min) / 10000; // discontinuity correction, OIV does not like coinciding points in a color mapping

    // keep min and max outside the hotspot interval
    min = std::min(min, hotspotMin - 2 * fudge);
    max = std::max(max, hotspotMax + 2 * fudge);

    if (pHotSpot->CenterIsTransparent()) {
      MoLevelColorMapping *colorMapping = new MoLevelColorMapping;
      colorMapping->setName("ValueColorMap");

      const SbVec4d undefinedColor(0.6f, 0.5f, 0.5f, 0.1f);

      if (defined) {
        colorMapping->maxThresholdEnable = true;
        colorMapping->maxThresholdValue = (float)(max + 100 + std::abs(max));
        colorMapping->maxThresholdColor.setValue(undefinedColor);

        colorMapping->minThresholdEnable = true;
        colorMapping->minThresholdValue = (float)(min - 100 - std::abs(min));
        colorMapping->minThresholdColor.setValue(undefinedColor);
      }

      colorMapping->values.set1Value(0, min);
      colorMapping->colors.set1Value(0, redMin, greenMin, blueMin, 1.0f);
      colorMapping->values.set1Value(1, hotspotMin);
      colorMapping->colors.set1Value(1, 0.1f, 0.1f, 0.1f, opacity);
      colorMapping->values.set1Value(2, hotspotMax);
      colorMapping->colors.set1Value(2, redMax, greenMax, blueMax, 1.0f);
      colorMapping->values.set1Value(3, max);

      return colorMapping;
    } else {
      MoLinearColorMapping *colorMapping = new MoLinearColorMapping;
      colorMapping->setName("ValueColorMap");

      const SbVec4d undefinedColor(0.6f, 0.5f, 0.5f, 0.1f);

      if (defined) {
        colorMapping->maxThresholdEnable = true;
        colorMapping->maxThresholdValue = (float)(max + 100 + std::abs(max));
        colorMapping->maxThresholdColor.setValue(undefinedColor);

        colorMapping->minThresholdEnable = true;
        colorMapping->minThresholdValue = (float)(hotspotMin);
        colorMapping->minThresholdColor.setValue(SbVec4d(0.0f, 0.0f, 0.0f, opacity));
      }

      colorMapping->values.set1Value(0, hotspotMin);
      colorMapping->colors.set1Value(0, redMin, greenMin, blueMin, 1.0f);
      colorMapping->values.set1Value(1, hotspotMax);
      colorMapping->colors.set1Value(1, redMax, greenMax, blueMax, 1.0f);
      colorMapping->values.set1Value(2, max);
      colorMapping->colors.set1Value(2, redMax, greenMax, blueMax, 1.0f);

      return colorMapping;
    }
  } else {
    assert(false);
    return 0;
  }
}

// OIDIMeshNodeManager::MeshInfo

OIDIMeshNodeManager::MeshInfo::MeshInfo()
    : colorScalarSetId(-1), isoScalarSetId(-1), vectorSetId(-1), tensorSetId(-1), tensorVectorSetId(-1),
      eigenValueSignSetId(-1), beachballScalarSetId1(-1), beachballScalarSetId2(-1), tensorSet(0) {}
