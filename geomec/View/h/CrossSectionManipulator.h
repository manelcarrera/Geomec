#ifndef _CROSSSECTIONMANIPULATOR_H_
#define _CROSSSECTIONMANIPULATOR_H_

#include <Inventor/draggers/SoJackDragger.h>
#include <Inventor/fields/SoSFPlane.h>

#include "CrossSection.h"
class OIDIMeshNodeManager;

/**
 * Manipulator to control a plane or cross section. Movement of the
 * manipulator is constrained, so the plane can never leave the
 * given bounding box.
 */
class CrossSectionManipulator : public SoJackDragger {
  SO_NODE_HEADER(CrossSectionManipulator);

  SbBox3f m_bbox;
  SbBox3f m_bboxReduced;
  SbVec3f m_savedTranslation;
  CCrossSection::OrientationType m_orientationConstraint;

  SoNode *m_rotator;
  SoNode *m_separator;

  OIDIMeshNodeManager *m_meshNodeManager;

  bool m_confined;

  static void draggerStartCallback(void *userData, SoDragger *dragger);
  static void draggerMotionCallback(void *userData, SoDragger *dragger);
  static void draggerFinishCallback(void *userData, SoDragger *dragger);

  void onDraggerStart(SoDragger *dragger);
  void onDraggerMotion(SoDragger *dragger);
  void onDraggerFinish(SoDragger *dragger);

public:
  CrossSectionManipulator();
  virtual ~CrossSectionManipulator();

  void setMeshNodeManager(OIDIMeshNodeManager *meshNodeManager);

  void updateDraggerPosition();

  void setBoundingBox(const SbBox3f &bbox);

  void setPlane(const SbVec3f &point_, const SbVec3f &normal_);

  void touch();

  SoSFPlane plane;

  SoSFVec3f normal;

  void limitRotation(CCrossSection::OrientationType orientation);

  static void initClass();

  static SbVec3f getDefaultBasePoint();

  static SbVec3f getDefaultNormal();

  static SbPlane getDefaultPlane();
  void removeConstraints();
  void setTranslation(const SbVec3f &basepointVec);
  void setNormal(const SbVec3f &normalVec);
  void confineTranslation();

  bool isConfined() const;

  void setConfined(bool isConfined);
};

#endif
