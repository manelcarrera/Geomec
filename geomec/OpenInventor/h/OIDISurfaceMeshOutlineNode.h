#ifndef _OIDI_SURFACE_MESHOUTLINE_NODE_H__
#define _OIDI_SURFACE_MESHOUTLINE_NODE_H__

#include <Inventor/actions/SoAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoSeparator.h>
#include <MeshVizXLM/MbVec3.h>

#include <map>

#include "OIDIMeshNode.h"

class OIDIMeshSU;

class MoMesh;
class MoScalarSetI;
class MoPredefinedColorMapping;
class MoDrawStyle;
class MoMaterial;
class MoMeshSurface;
class MoMeshOutline;

class SoLightModel;
class SoPickStyle;
class SoClipPlaneManip;
class SoSwitch;
class SoSeparator;
class SoDrawStyle;

class MoCellFilter;
class DecimatingCellFilterI;
class MoMeshVector;

template <class T> class MiDataSetI;

class OIDISurfaceMeshOutlineNode : public OIDIMeshNode {
  // Cell shape members
  SoSwitch *m_surfaceSwitch;
  SoSeparator *m_surfaceSeparator;
  SoGroup *m_surfaceAppearance;
  MoDrawStyle *m_surfaceDrawStyle;
  MoMaterial *m_surfaceMaterial;
  MoMeshSurface *m_surface;

  SoSeparator *m_outlineSeparator;
  SoGroup *m_outlineAppearance;
  MoDrawStyle *m_outlineDrawStyle;
  MoMaterial *m_outlineMaterial;
  SoDrawStyle *m_lineDrawStyle;
  MoMeshOutline *m_outline;

  void buildSurfaceAppearanceNode();
  void buildSurfaceNode();

  void buildSurfaceMeshNode();
  void buildNodes();

  virtual bool highlight();
  virtual bool dehighlight();

public:
  explicit OIDISurfaceMeshOutlineNode(const OIDIMesh *meshData, OIDIMeshNodeManager *meshNodeManager);

  virtual void updateColorScalarSet();

  virtual void updateIsoScalarSet();

  virtual void setIsoValues(int isoCount, double min, double max);
  virtual void setIsoGap(double gap);

  virtual void UpdateDisplaySettings(const MeshNodeSettings &meshNodeSettings, bool showColor);
  virtual void updateGeologyColors();
  void buildOutlineAppearanceNode();
};

#endif
