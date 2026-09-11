#ifndef _OIDI_VOLUME_MESH_NODE_H__
#define _OIDI_VOLUME_MESH_NODE_H__

#include <MeshVizXLM/MbVec3.h>

#include <Inventor/SoInput.h>
#include <Inventor/actions/SoAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/fields/SoSFPlane.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <map>

#include "OIDIMeshNode.h"

class OIDIMeshVU;

class MoMesh;
class MoScalarSetI;
class MoPredefinedColorMapping;
class MoDrawStyle;
class MoMaterial;
class MoMeshSkin;
class MoMeshCellShape;
class MoMeshPlaneSlice;
class MoMeshVector;
class MoCellFilter;
class DecimatingCellFilterI;
class MoMeshAnnotatedIsoline;

class SoLightModel;
class SoPickStyle;
class SoClipPlaneManip;
class SoSwitch;
class SoSeparator;
class SoClipPlane;
class SoFieldSensor;

template <class T> class MiDataSetI;

class CCrossSection;

class OIDIVolumeMeshNode : public OIDIMeshNode {
  // Skin members
  SoSwitch *m_skinSwitch;
  SoSeparator *m_skinSeparator;
  SoGroup *m_skinAppearance;
  MoDrawStyle *m_skinDrawStyle;
  MoMaterial *m_skinMaterial;
  MoMeshSkin *m_skin;

  // Cell shape members
  SoSwitch *m_cellShapeSwitch;
  SoSeparator *m_cellShapeSeparator;
  SoGroup *m_cellShapeAppearance;
  MoDrawStyle *m_cellShapeDrawStyle;
  MoMaterial *m_cellShapeMaterial;
  MoMeshCellShape *m_cellShape;

  MoMeshAnnotatedIsoline *m_skinIsoline;
  SoSwitch *m_skinIsolineSwitch;

  void buildSkinAppearanceNode();
  void buildCellShapeAppearanceNode();
  void buildSkinNode();
  void BuildSkinIsolineNode();
  void buildCellShapeNode();
  void buildVolumeMeshNode();
  void buildNodes();

public:
  OIDIVolumeMeshNode(const OIDIMesh *meshData, OIDIMeshNodeManager *meshNodeManager);

  virtual void setIsoValues(int isoCount, double min, double max);
  virtual void setIsoGap(double gap);

  virtual void UpdateDisplaySettings(const MeshNodeSettings &meshNodeSettings, bool showColor);
  virtual void updateGeologyColors();

  virtual void updateColorScalarSet();
  virtual void updateIsoScalarSet();
};

#endif
