#ifndef _COORDINATE_AXIS_H__
#define _COORDINATE_AXIS_H__

#include <Inventor/nodes/SoSwitch.h>

class SoSeparator;
class SbBox3f;
class PoLinearAxis;
class PoGroup3Axis3;
class SoMaterial;
class SoAnnoText3Property;
class SoTranslation;

class OIDIMeshNodeManager;

class CoordinateAxis : public SoSwitch
{
public:
  CoordinateAxis (OIDIMeshNodeManager * meshNodeManager);
  void reset(const SbBox3f & bbox, const SbVec3d& translation);

  void adjustAxis( PoLinearAxis * axis, float &size, float &origin );

  void SetAxisColor( float * rgb );
  void GetAxisColor( float * rgb ) const;
  void PointSize( const double & size );

private:
  PoGroup3Axis3*         m_coordAxisGroup;
  SoMaterial *           m_coordAxisMaterial;
  SoAnnoText3Property *  m_textProperty;
  SoTranslation *        m_translation;
};

#endif