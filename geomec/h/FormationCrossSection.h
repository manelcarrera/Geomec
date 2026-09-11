#ifndef FORMATIONCROSSSECTION_H_INCLUDED
#define FORMATIONCROSSSECTION_H_INCLUDED

class SoSwitch;
class MoMesh;
class MoMaterial;
class MoDrawStyle;
class SoDrawStyle;
class MoMeshPlaneSlice;
class MoCellFilter;
class MoMeshVector;
class MoMeshTensorVector;
class MoMeshOutline;
class MoMeshIsoline;
class DecimatingCellFilterI;
class ExtractorCallback;
class OIDIMeshVU;

#include "SoSwitchBool.h"
#include <Inventor/fields/SoMFFloat.h>
#include <Inventor/fields/SoSFColor.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoSFPlane.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/sensors/SoFieldSensor.h>

class FormationCrossSection : public SoGroup {
  SO_NODE_HEADER(FormationCrossSection);

  MoMesh *m_mesh;
  MoMaterial *m_crossSectionMaterial;
  MoDrawStyle *m_crossSectionDrawStyle;
  MoMeshPlaneSlice *m_slice;

  SoSwitch *m_cellFilterSwitch;
  MoCellFilter *m_cellFilter;
  DecimatingCellFilterI *m_decimatingCellFilter;
  ExtractorCallback *m_extractorCallback;

  SoSwitch *m_vectorSwitch;
  SoSwitch *m_tensorVectorSwitch;
  SoSwitch *m_outlineSwitch;
  SoSwitch *m_isolineSwitch;
  MoMeshVector *m_vectors;
  MoMeshTensorVector *m_tensorVectors;
  SoSeparator *m_vectorGroup;
  MoMesh *m_planeMesh;
  MoMaterial *m_outlineMaterial;
  MoDrawStyle *m_outlineDrawStyle;
  SoDrawStyle *m_soOutlineDrawStyle;
  MoMeshOutline *m_outline;
  MoMeshIsoline *m_isoline;

  // These are used to convert the corresponding boolean fields
  // to valid inputs for the SoSwitch::whichChild field
  SoSwitchBool *m_showVectors;
  SoSwitchBool *m_showTensorVectors;
  SoSwitchBool *m_showOutline;
  SoSwitchBool *m_showIsoline;

  SoFieldSensor m_vectorFilterSensor;

  static SbPlane getDefaultPlane();

  static void sensorCallback(void *data, SoSensor *sensor);

  void onSensorTriggered(SoSensor *sensor);

  void buildNode();

  virtual ~FormationCrossSection();

public:
  static void initClass();
  static void exitClass();

  enum ColoringType { COLOR, CONTOURING };

  SoSFPlane plane;

  SoSFEnum coloring;
  SoSFColor color;

  SoSFInt32 colorScalarSetId;        // scalar properties
  SoSFInt32 isoScalarSetId;          // scalar set for drawing isolines
  SoSFInt32 vectorSetId;             // MoMeshVector set
  SoSFInt32 tensorVectorSetId;       // MoMeshTensorVector set
  SoSFInt32 tensorVectorScalarSetId; // for coloring the tensor vectors

  SoMFFloat isovalues;

  SoSFBool showEdges;
  SoSFBool showVectors;
  SoSFBool showTensorVectors;
  SoSFBool showOutline;
  SoSFBool showIsoline;

  SoSFFloat vectorScale;
  SoSFFloat vectorFilter;

  FormationCrossSection();

  void setMesh(const OIDIMeshVU *mesh);

  const OIDIMeshVU *getMesh() const;
};

#endif
