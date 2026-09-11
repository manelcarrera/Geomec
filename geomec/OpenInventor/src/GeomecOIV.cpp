#include "GeomecOIV.h"
#include "stdafx.h"

// View/TensorViz
#include "MoMeshTensor.h"
#include "MoMeshTensorVector.h"
#include "MoTensorSet.h"
#include "MoTensorSetElement.h"

#include "CrossSectionManipulator.h"
#include "FormationCrossSection.h"
#include "GeomecPolygonScreenDrawer.h"
#include "MeshCrossSection.h"
#include "SoSwitchBool.h"

void GeomecOIV::init() {
  // Init custom OIV classes
  MoMeshTensor::initClass();
  MoMeshTensorVector::initClass();
  MoTensorSet::initClass();
  MoTensorSetElement::initClass();
  CrossSectionManipulator::initClass();
  GeomecPolygonScreenDrawer::initClass();

  FormationCrossSection::initClass();
  MeshCrossSection::initClass();
  SoSwitchBool::initClass();
}

void GeomecOIV::finish() {
  MoMeshTensor::exitClass();
  MoMeshTensorVector::exitClass();
  MoTensorSet::exitClass();
  MoTensorSetElement::exitClass();
  CrossSectionManipulator::exitClass();
  GeomecPolygonScreenDrawer::exitClass();

  FormationCrossSection::exitClass();
  MeshCrossSection::exitClass();
}
