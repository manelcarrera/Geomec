#include "stdafx.h"

#include "MeshPointProbeGroup.h"
#include "OIDIMesh.h"
#include "OIDIMeshNodeManager.h"
#include "OIDIVolumeMeshNode.h"
#include "ValueTextScene.h"
#include "ValueTracker.h"

#include <Inventor/draggers/SoJackDragger.h>
#include <Inventor/nodes/SoResetTransform.h>
#include <Inventor/nodes/SoSwitch.h>

#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoFaceSet.h>
#include <Inventor/nodes/SoMarkerSet.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterialBinding.h>

#include <Inventor/sensors/SoFieldSensor.h>

#include "CrossSectionManipulator.h"
#include <MeshVizXLM/MbVec3.h>
#include <MeshVizXLM/extractors/MiPointProbeUnstructured.h>
#include <MeshVizXLM/extractors/MxQuadrangleCellExtract.h>
#include <MeshVizXLM/extractors/MxTriangleCellExtract.h>
#include <MeshVizXLM/mapping/nodes/MoMeshPointProbe.h>

void localNormalSensorCallback(void *data, SoSensor *sensor) {
  ValueTracker *vt = (ValueTracker *)data;
  vt->normalSensorCallback(sensor);
}

ValueTracker::ValueTracker(OIDIMeshNodeManager *meshNodeManager, CrossSectionManipulator *dragger)
    : m_meshNodeManager(meshNodeManager), m_dragger(dragger), m_draggerSwitch(0), m_firstTime(true),
      m_cellMarkerSwitch(0), m_validCellMarkerSwitch(0), m_numCellNodes((size_t)-1), m_vertexProperty(0),
      m_cellMarkerMaterial(0), m_cellMarkerSet(0), m_cellFacetSetCoordinates(0), m_cellFacetSet(0),
      m_volumeCellFacetSetInitialized(0), m_surfaceCellFacetSetInitialized(0) {
  setName("ValueTracker");

  assert(dragger != 0);

  // We do not want this tracker stuff as part of the bounding box
  SoResetTransform *pReset = new SoResetTransform;
  addChild(pReset);
  pReset->whatToReset = SoResetTransform::BBOX;

  // before m_switch, which contains the text scene ...
  m_validCellMarkerSwitch = new SoSwitch;
  m_validCellMarkerSwitch->setName("ValidCellMarkerSwitch");
  addChild(m_validCellMarkerSwitch);
  m_cellMarkerSwitch = new SoSwitch;
  m_cellMarkerSwitch->setName("cellMarkerSwitch");
  m_validCellMarkerSwitch->addChild(m_cellMarkerSwitch);

  m_switch = new SoSwitch;
  addChild(m_switch);

  m_draggerSwitch = new SoSwitch;
  m_switch->addChild(m_draggerSwitch);

  m_draggerSwitch->whichChild = SO_SWITCH_ALL;
  m_draggerSwitch->addChild(m_dragger);

  SoPickStyle *unpickableStyle = new SoPickStyle;
  unpickableStyle->style = SoPickStyle::UNPICKABLE;
  m_switch->addChild(unpickableStyle);

  m_cellMarkerMaterial = new SoMaterial;
  m_cellMarkerSet = new SoMarkerSet;
  m_vertexProperty = new SoVertexProperty;
  m_cellMarkerSet->vertexProperty = m_vertexProperty;
  SoMaterialBinding *markerMaterialBinding = new SoMaterialBinding;
  markerMaterialBinding->value = SoMaterialBinding::PER_VERTEX;

  // m_cellMarkerSwitch->addChild(m_cellMarkerCoordinates);
  m_cellMarkerSwitch->addChild(markerMaterialBinding);
  m_cellMarkerSwitch->addChild(m_cellMarkerMaterial);
  m_cellMarkerSwitch->addChild(m_cellMarkerSet);

  SoDrawStyle *facetDrawStyle = new SoDrawStyle;
  facetDrawStyle->style = SoDrawStyleElement::LINES;
  facetDrawStyle->lineWidth = 3;
  m_cellMarkerSwitch->addChild(facetDrawStyle);

  SoMaterialBinding *facetMaterialBinding = new SoMaterialBinding;
  facetMaterialBinding->value = SoMaterialBinding::OVERALL;
  m_cellMarkerSwitch->addChild(facetMaterialBinding);

  SoMaterial *facetMaterial = new SoMaterial;
  facetMaterial->ambientColor.setValue(0.0f, 0.0f, 1.0f);
  facetMaterial->diffuseColor.setValue(1.0f, 0.0f, 1.0f);
  facetMaterial->emissiveColor.setValue(0.0f, 1.0f, 0.0f);

  m_cellMarkerSwitch->addChild(facetMaterial);

  m_cellFacetSetCoordinates = new SoCoordinate3;
  m_cellFacetSet = new SoFaceSet;

  m_cellMarkerSwitch->addChild(m_cellFacetSetCoordinates);
  m_cellMarkerSwitch->addChild(m_cellFacetSet);

  m_draggerTextScene = new ValueTextScene;
  m_switch->addChild(m_draggerTextScene);
  m_draggerTextScene->display();

#if 0
  m_normalSensor = new SoFieldSensor(::localNormalSensorCallback, this);
  m_normalSensor->setPriority(0);
  m_normalSensor->attach(&m_dragger->normal);
#endif
}

void ValueTracker::UpdateDisplay() {
  if (IsDisplayed()) {
    SbBox3f meshBoundingBox = m_meshNodeManager->GetAllMeshesBoundingBox();
    SbVec3f translation = m_dragger->translation.getValue();
    if (!meshBoundingBox.intersect(translation)) {
      m_dragger->setTranslation(meshBoundingBox.getCenter());
      m_dragger->setNormal(SbVec3f(0.0f, -1.0f, 0.0f));
      m_firstTime = false;
    }
    m_draggerTextScene->setTranslation(m_meshNodeManager->GetTranslation());
  }
  updatePropertyName();
}

void ValueTracker::Display(bool mode) {
  if (!m_switch)
    return;
  m_switch->whichChild = mode ? SO_SWITCH_ALL : SO_SWITCH_NONE;
  if (!mode)
    return;

  UpdateDisplay();

  SbBox3f meshBoundingBox = m_meshNodeManager->GetAllMeshesBoundingBox();
  m_dragger->setBoundingBox(meshBoundingBox);
}

bool ValueTracker::IsDisplayed() { return m_switch && m_switch->whichChild.getValue() == SO_SWITCH_ALL; }

void ValueTracker::motionCallback(GMoMeshPointProbe *probe, const MiVolumeMeshUnstructured *mesh,
                                  const std::string &meshName, size_t cellId, const MeXScalardSetI &scalars,
                                  const MeXVec3dSetI &vectors) {
  if (!probe)
    return;
  if (m_switch->whichChild.getValue() == SO_SWITCH_NONE)
    return;

  SbVec3f probePosition = probe->position.getValue(); // m_probePosition also used in computeCellMarkerSet(...)oooo

  size_t meshNodeIndexClosest = (size_t)-1;
  MbVec3d meshNodeCoordinatesClosest(DBL_UNDEFINED);

  MbVec3d coordinates(probePosition[0], probePosition[1], probePosition[2]);

  computeCellMarkerSet(mesh, coordinates, cellId, meshNodeIndexClosest, meshNodeCoordinatesClosest);

  computeCellFacetSet(mesh, cellId);

  updatePropertyName();

  // Compute the normal by rotating the default normal according to the manips orientation
  SbVec3f normal;
  m_dragger->rotation.getValue().multVec(SbVec3f(0.0f, 1.0f, 0.0f), normal);
  // m_meshNodeManager->setVectorDataSetNormal(0, normal);

  m_draggerTextScene->setNormalVector(normal);

  m_draggerTextScene->setCellId(meshName, cellId);
  m_draggerTextScene->setCellCoordinates(probePosition);

  m_draggerTextScene->setNodeId(meshNodeIndexClosest);
  m_draggerTextScene->setNodeCoordinates(meshNodeCoordinatesClosest);

  int scalarSetId = probe->scalarSetIds.getValues(0)[0];
  int vectorSetId = probe->vectorSetIds.getValues(0)[0];
  int tensorSetId = probe->getTensorSetId();

  setTextSceneValues(meshNodeIndexClosest, scalarSetId, vectorSetId, tensorSetId, cellId);

  double scalar = scalars.getSize() > 0 ? scalars.get(0) : DBL_UNDEFINED;
  MbVec3d vector = vectors.getSize() > 0 ? vectors.get(0) : MbVec3d(DBL_UNDEFINED);
  m_draggerTextScene->setCellPropertyValue(scalar);
  m_draggerTextScene->setCellPropertyVector(vector);
}

double ValueTracker::computeDistanceClosestNode(const MiMeshUnstructuredI *mesh, const MbVec3d &coordinates,
                                                size_t cellId, size_t &cniClosest) {
  double distanceClosest = DBL_MAX;
  const MiCell *cell = mesh->getTopology().getCell(cellId);
  if (!cell)
    return distanceClosest;

  size_t numCellNodes = cell->getNumNodes();

  for (size_t cni = 0; cni < numCellNodes; ++cni) {
    size_t meshNodeIndex = cell->getNodeIndex(cni);
    MbVec3d meshNodeCoordinates = mesh->getGeometry().getCoord(meshNodeIndex);
    MbVec3d diff = coordinates - meshNodeCoordinates;
    double diffLength = diff.length();
    if (diffLength < distanceClosest) {
      cniClosest = cni;
      distanceClosest = diffLength;
    }
  }
  return distanceClosest;
}

void ValueTracker::invalidateCellMarkers() { m_validCellMarkerSwitch->whichChild = SO_SWITCH_NONE; }

void ValueTracker::computeCellMarkerSet(const MiMeshUnstructuredI *mesh, const MbVec3d &coordinates, size_t cellId,
                                        size_t &meshNodeIndexClosest, MbVec3d &meshNodeCoordinatesClosest) {
  m_validCellMarkerSwitch->whichChild = SO_SWITCH_NONE;
  meshNodeIndexClosest = (size_t)-1;

  const MiCell *cell = mesh->getTopology().getCell(cellId);
  if (!cell)
    return;

  size_t numCellNodes = cell->getNumNodes();
  double distanceClosest = -1;
  size_t cniClosest;

  int32_t markerIndex = SoMarkerSet::SQUARE_FILLED_9_9;
  SbColor markerGreyColor(1.0f, 1.0f, 0.0f);
  SbColor markerHighlightColor(1.0f, 0.1f, 0.1f);

  if (numCellNodes != m_numCellNodes) {
    auto cellMarkerSet = new SoMarkerSet;
    m_cellMarkerSwitch->replaceChild(m_cellMarkerSet, cellMarkerSet);
    m_cellMarkerSet = cellMarkerSet;

    m_vertexProperty = new SoVertexProperty;
    m_vertexProperty->vertex.setNum(numCellNodes);
    m_cellMarkerSet->vertexProperty = m_vertexProperty;

    m_numCellNodes = numCellNodes;
  }

  m_cellMarkerSet->markerIndex.setNum(numCellNodes);
  m_cellMarkerMaterial->diffuseColor.setNum(numCellNodes);

  for (size_t cni = 0; cni < numCellNodes; ++cni) {
    size_t meshNodeIndex = cell->getNodeIndex(cni);
    MbVec3d meshNodeCoordinates = mesh->getGeometry().getCoord(meshNodeIndex);
    MbVec3d diff = coordinates - meshNodeCoordinates;
    double diffLength = diff.length();
    if (diffLength < distanceClosest || distanceClosest < 0) {
      meshNodeIndexClosest = meshNodeIndex;
      cniClosest = cni;
      distanceClosest = diffLength;
      meshNodeCoordinatesClosest = meshNodeCoordinates;
    }

    m_vertexProperty->vertex.set1Value(cni, meshNodeCoordinates[0], meshNodeCoordinates[1], meshNodeCoordinates[2]);

    m_cellMarkerSet->markerIndex.set1Value(cni, markerIndex);
    m_cellMarkerMaterial->diffuseColor.set1Value(cni, markerGreyColor);
    m_cellMarkerMaterial->specularColor.set1Value(cni, markerGreyColor);
    m_cellMarkerMaterial->ambientColor.set1Value(cni, markerGreyColor);
    m_cellMarkerMaterial->emissiveColor.set1Value(cni, markerGreyColor);
  }

  m_cellMarkerMaterial->diffuseColor.setValues(cniClosest, 1, &markerHighlightColor);
  m_cellMarkerMaterial->specularColor.setValues(cniClosest, 1, &markerHighlightColor);
  m_cellMarkerMaterial->ambientColor.setValues(cniClosest, 1, &markerHighlightColor);
  m_cellMarkerMaterial->emissiveColor.setValues(cniClosest, 1, &markerHighlightColor);

  m_validCellMarkerSwitch->whichChild = SO_SWITCH_ALL;
}

void ValueTracker::updateValueTracker(const MiMeshUnstructuredI *mesh, const std::string &meshName,
                                      const MbVec3d &coordinates, size_t cellId, int scalarSetId, int vectorSetId,
                                      int tensorSetId) {
  m_draggerTextScene->clearNormalVector();

  size_t meshNodeIndexClosest = (size_t)-1;
  MbVec3d meshNodeCoordinatesClosest = coordinates;

  if (cellId != (size_t)-1) {
    const MiCell *cell = mesh->getTopology().getCell(cellId);
    size_t numCellNodes = cell->getNumNodes();
    double distanceClosest = DBL_MAX;
    size_t cniClosest;

    for (size_t cni = 0; cni < numCellNodes; ++cni) {
      size_t meshNodeIndex = cell->getNodeIndex(cni);
      MbVec3d meshNodeCoordinates = mesh->getGeometry().getCoord(meshNodeIndex);
      MbVec3d diff = coordinates - meshNodeCoordinates;
      double meshNodeDistance = diff.length();
      if (meshNodeDistance < distanceClosest) {
        meshNodeIndexClosest = meshNodeIndex;
        cniClosest = cni;
        distanceClosest = meshNodeDistance;
        meshNodeCoordinatesClosest = meshNodeCoordinates;
      }
    }
  }

  const MiDataSetI<double> *scalarDataSet = m_meshNodeManager->getScalarDataSet(scalarSetId);
  const MiDataSetI<MbVec3d> *vectorDataSet = m_meshNodeManager->getVectorDataSet(vectorSetId);

  m_draggerTextScene->setCellId(meshName, cellId);
  m_draggerTextScene->setCellCoordinates(SbVec3f(coordinates[0], coordinates[1], coordinates[2]));
  m_draggerTextScene->setCellPropertyValue(DBL_UNDEFINED);
  m_draggerTextScene->setCellPropertyVector(MbVec3d(DBL_UNDEFINED));

  m_draggerTextScene->setNodeId(meshNodeIndexClosest);
  m_draggerTextScene->setNodeCoordinates(meshNodeCoordinatesClosest);

  setTextSceneValues(meshNodeIndexClosest, scalarSetId, vectorSetId, tensorSetId, cellId);

  // if node values are given interpolate the values at given coordinates.
  if (cellId != (size_t)-1 && ((scalarDataSet && scalarDataSet->getBinding() == MiDataSet::PER_NODE) ||
                               (vectorDataSet && vectorDataSet->getBinding() == MiDataSet::PER_NODE))) {
    const MiCell *cell = mesh->getTopology().getCell(cellId);
    std::vector<double> weights;

    GetCellNodeWeights(cell, mesh, coordinates, weights);
    if (weights.empty())
      return;

    size_t numCellNodes = cell->getNumNodes();
    assert(weights.size() == numCellNodes);

    double scalarValue = 0;
    MbVec3d vectorValue(0.0);

    for (size_t cni = 0; cni < numCellNodes; ++cni) {
      size_t meshNodeIndex = cell->getNodeIndex(cni);
      if (scalarDataSet) {
        double nodeValue = scalarDataSet->get(meshNodeIndex);
        scalarValue += weights[cni] * nodeValue;
      }
      if (vectorDataSet) {
        MbVec3d nodeValue = vectorDataSet->get(meshNodeIndex);
        vectorValue += weights[cni] * nodeValue;
      }
    }
    if (scalarDataSet)
      m_draggerTextScene->setCellPropertyValue(scalarValue);
    if (vectorDataSet)
      m_draggerTextScene->setCellPropertyVector(vectorValue);
  }
}

void ValueTracker::computeCellFacetSet(const MiVolumeMeshUnstructured *mesh, size_t cellId) {
  if (!mesh)
    return;
  const auto *cell = mesh->getTopology().getCell(cellId);
  if (!cell)
    return;
  size_t numFacets = cell->getNumFacets();
  std::vector<size_t> facetNodeIndices;

  // calculate the list of node indices for the cell facets

  if (!m_volumeCellFacetSetInitialized) {
    // first time, we need to initialize m_cellfacetset, won't change afterward.
    size_t fniSizePrev = 0;
    for (size_t f = 0; f < numFacets; ++f) {
      cell->appendNodesIndexOfFacet(f, facetNodeIndices);
      int32_t numFacetNodes = facetNodeIndices.size() - fniSizePrev;
      fniSizePrev = facetNodeIndices.size();
      m_cellFacetSet->numVertices.set1Value(f, numFacetNodes);
    }
    m_volumeCellFacetSetInitialized = true;
    m_surfaceCellFacetSetInitialized = false;
  } else {
    // less to do
    for (size_t f = 0; f < numFacets; ++f) {
      cell->appendNodesIndexOfFacet(f, facetNodeIndices);
    }
  }

  for (int n = 0; n < facetNodeIndices.size(); ++n) {
    MbVec3d meshNodeCoordinates = mesh->getGeometry().getCoord(facetNodeIndices[n]);
    m_cellFacetSetCoordinates->point.set1Value(n, meshNodeCoordinates[0], meshNodeCoordinates[1],
                                               meshNodeCoordinates[2]);
  }
}

void ValueTracker::computeCellFacetSet(const OIDIMeshSU *mesh, size_t cellId) {
  if (!mesh)
    return;
  const OIDISurfaceCell *cell = mesh->getCell(cellId);
  std::vector<size_t> facetNodeIndices;

  // calculate the list of node indices for the cell facets

  if (!m_surfaceCellFacetSetInitialized) {
    // first time, we need to initialize m_cellfacetset, won't change afterward.

    cell->appendNodeIndices(facetNodeIndices);
    m_cellFacetSet->numVertices.setValue(facetNodeIndices.size());

    m_surfaceCellFacetSetInitialized = true;
    m_volumeCellFacetSetInitialized = false;
  } else {
    cell->appendNodeIndices(facetNodeIndices);
  }

  for (int n = 0; n < facetNodeIndices.size(); ++n) {
    MbVec3d meshNodeCoordinates = mesh->getCoord(facetNodeIndices[n]);
    m_cellFacetSetCoordinates->point.set1Value(n, meshNodeCoordinates[0], meshNodeCoordinates[1],
                                               meshNodeCoordinates[2]);
  }
}

void ValueTracker::computeCellFacetSet(const OIDIPointSetMesh *mesh, size_t cellId) {
  if (!mesh)
    return;
  clearCellFacetSet();
}

void ValueTracker::clearCellFacetSet() {
  m_surfaceCellFacetSetInitialized = false;
  m_volumeCellFacetSetInitialized = false;

  m_cellFacetSet->numVertices.setValue(0);
  m_cellFacetSetCoordinates->point.deleteValues(0);
  m_vertexProperty->vertex.deleteValues(0);
  m_cellMarkerSet->markerIndex.deleteValues(0);
}

void ValueTracker::SetTextColor(float *rgb) { m_draggerTextScene->SetTextColor(rgb); }

void ValueTracker::GetTextColor(float *rgb) { m_draggerTextScene->GetTextColor(rgb); }

void ValueTracker::updatePropertyName() { m_draggerTextScene->setPropertyName("Value"); }

void ValueTracker::normalSensorCallback(SoSensor *sensor) {
  SbVec3f normal;
  m_dragger->rotation.getValue().multVec(SbVec3f(0.0f, 1.0f, 0.0f), normal);
  // m_meshNodeManager->setVectorDataSetNormal(0, normal);

  m_draggerTextScene->setNormalVector(normal);
}

void ValueTracker::updateTensorText(size_t index) {
  // m_draggerTextScene->setTensor (m_meshNodeManager->getTensor(index));
}

void ValueTracker::TrackElement(bool track) { m_cellMarkerSwitch->whichChild = track ? SO_SWITCH_ALL : SO_SWITCH_NONE; }

bool ValueTracker::TrackElement(void) { return m_cellMarkerSwitch->whichChild.getValue() == SO_SWITCH_ALL; }

void ValueTracker::showDragger(bool show) { m_draggerSwitch->whichChild = show ? SO_SWITCH_ALL : SO_SWITCH_NONE; }

CrossSectionManipulator *ValueTracker::getDragger() { return m_dragger; }

void ValueTracker::setTextSceneValues(size_t nodeIndex, int scalarSetId, int vectorSetId, int tensorSetId,
                                      size_t cellId) {
  const MiDataSetI<double> *scalarDataset = m_meshNodeManager->getScalarDataSet(scalarSetId);

  if (scalarDataset != 0) {
    if (scalarDataset->getBinding() == MiDataSet::PER_CELL) {
      m_draggerTextScene->setNodePropertyValueUnknown();
      m_draggerTextScene->setCellPropertyValue(scalarDataset->get(cellId));
    } else {
      m_draggerTextScene->setNodePropertyValue(scalarDataset->get(nodeIndex));
      m_draggerTextScene->setCellPropertyValueUnknown();
    }
  } else {
    m_draggerTextScene->setNodePropertyValueUnknown();
    m_draggerTextScene->setCellPropertyValueUnknown();
  }

  const MiDataSetI<MbVec3d> *vectorDataset = m_meshNodeManager->getVectorDataSet(vectorSetId);

  if (vectorDataset != 0) {
    if (vectorDataset->getBinding() == MiDataSet::PER_CELL) {
      m_draggerTextScene->setNodePropertyVectorUnknown();
      m_draggerTextScene->setCellPropertyVector(vectorDataset->get(cellId));
    } else {
      m_draggerTextScene->setNodePropertyVector(vectorDataset->get(nodeIndex));
      m_draggerTextScene->setCellPropertyVectorUnknown();
    }
  } else {
    m_draggerTextScene->setNodePropertyVectorUnknown();
    m_draggerTextScene->setCellPropertyVectorUnknown();
  }

  const MiDataSetI<CTensor> *tensorDataset = m_meshNodeManager->getTensorDataSet(tensorSetId);

  if (tensorDataset != 0) {
    if (tensorDataset->getBinding() == MiDataSet::PER_CELL) {
      CTensor tensor = tensorDataset->get(cellId);
      m_draggerTextScene->setTensor(&tensor);
      updateTensorText(cellId);
    } else {
      CTensor tensor = tensorDataset->get(nodeIndex);
      m_draggerTextScene->setTensor(&tensor);
      updateTensorText(nodeIndex);
    }
  } else {
    m_draggerTextScene->setTensor(0);
  }
}

void ValueTracker::GetCellNodeWeights(const MiCell *cell, const MiMeshUnstructuredI *mesh, const MbVec3d &coordinates,
                                      std::vector<double> &weights) {
  const MiVolumeCell *volumeCell = dynamic_cast<const MiVolumeCell *>(cell);
  const MiSurfaceCell *surfaceCell = dynamic_cast<const MiSurfaceCell *>(cell);

  size_t numCellNodes = cell->getNumNodes();
  weights.resize(numCellNodes);

  if (volumeCell) {
    if (numCellNodes == 4) {
      MxTetrahedronCellExtract::getWeight(mesh->getGeometry(), volumeCell, coordinates, weights);
    } else {
      assert(numCellNodes == 8);
      MxTetrahedronCellExtract::getWeight(mesh->getGeometry(), volumeCell, coordinates, weights);
    }
  } else if (surfaceCell) {
    if (numCellNodes == 3) {
      MxTriangleCellExtract::getWeight(mesh->getGeometry(), surfaceCell, coordinates, weights);
    } else {
      assert(numCellNodes == 4);
      MxQuadrangleCellExtract::getWeight(mesh->getGeometry(), surfaceCell, coordinates, weights);
    }
  }
}
