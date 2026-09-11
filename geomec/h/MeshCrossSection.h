#ifndef MESHCROSSSECTION_H_INCLUDED
#define MESHCROSSSECTION_H_INCLUDED

class FormationCrossSection;
class CrossSectionManipulator;
class OIDIMeshNodeManager;
class SoClipPlane;
class SoFieldSensor;
class MiMesh;
struct MeshNodeSettings;

#include "CrossSection.h"

#include "SoSwitchBool.h"
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/fields/SoSFPlane.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFFloat.h>
#include "MeshMode.h"

/**
 * 
 */
class MeshCrossSection : public SoGroup
{
  SO_NODE_HEADER(MeshCrossSection);

  static void basePointSensorCallback(void* data, SoSensor* sensor);
  static void normalSensorCallback(void* data, SoSensor* sensor);

  static SbPlane getDefaultPlane();
  static SbVec3f getDefaultBasePoint();
  static SbVec3f getDefaultNormal();

  OIDIMeshNodeManager* m_manager;

  CCrossSection* m_parent;

  SoGroup*       m_formationCrossSections;
  SoClipPlane*   m_clipPlane;
  SoFieldSensor* m_basepointSensor;
  SoFieldSensor* m_normalSensor;

  bool m_updatingFromParent;

  void basepointChanged();
  void normalChanged();
  void planeChanged();

  void add(FormationCrossSection* fmtXSec);

  TensorVectorMode  m_tensorVectorMode;

public:

  static void initClass();
  static void exitClass();

  enum ColoringType
  {
      COLOR,
      CONTOURING
  };

  static MeshCrossSection* build(OIDIMeshNodeManager& mgr, CCrossSection* parent);

  MeshCrossSection();

  virtual ~MeshCrossSection();

  const CCrossSection* getParent() const;

  void setColor(const MiMesh* formationMesh, SbColor color);

  void setScalarSetId(const MiMesh* formationMesh, int scalarSetId);

  int getScalarSetId(const MiMesh* formationMesh) const;

  void setVectorSetId(const MiMesh* formationMesh, int vectorSetId);

  int getVectorSetId(const MiMesh* formationMesh) const;

  void setTensorVectorSetId(const MiMesh* formationMesh, int tensorVectorSetId, int colorScalarSetId);

  void getTensorVectorSetIds(const MiMesh* formationMesh, int& vectorSetId, int& scalarSetId) const;

  void setIsoScalarSetId(const MiMesh* formationMesh, int isoScalarSetId);

  void setIsoValues(int count, float minval, float maxval);

  void updateFromParent();

  CCrossSection::OrientationType getOrientation() const;

  void updateDisplaySettings(const MeshNodeSettings& meshNodeSettings);

  void connectFrom(CrossSectionManipulator* manip);

  void disconnect();

  SoSFFloat vectorScale;
  SoSFFloat vectorFilter;

  SoSFBool showEdges;
  SoSFBool showVectors;
  SoSFBool showTensorVectors;
  SoSFBool showOutline;
  SoSFBool showIsoline;

  SoSFPlane plane;
  SoSFVec3f basepoint;
  SoSFVec3f normal;

  /** 
   * Turn clipping on (true) or off (false)
   */
  SoSFBool clip;

  /**
   * Uses ColoringType:
   *   - COLOR for a single solid color
   *   - CONTOURING to use a scalar set and color map
   */
  SoSFEnum coloring;
};

#endif
