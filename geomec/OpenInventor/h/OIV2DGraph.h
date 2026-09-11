#ifndef _OIV2DGraph_h_
#define _OIV2DGraph_h_

class InfoLabel;
class PoAxis;
class PoDomain;
class CurveGroup;
class Curve;
class SoSwitch;
class SoClipPlane;


#include "Inventor\nodes\SoGroup.h"
#include "Inventor\fields\SoSFPlane.h"
#include "ColorManager.h"
#include "unitnode.h"
#include "OIV2DRangeInterface.h"


class OIV2DGraph : public SoGroup
{
  static const char* const fontName;
  static const float titleFontSize;
  
  static const float gradFontSize;
  static const float arrowHeight;
  static const float arrowLength;

  static const char* const gridDrawStyle;
  static const char* const subGridDrawStyle;
  static const char* const gridLineStyle;
  static const char* const gridLineWidth;
  static const char* const gridLinePattern;
  static const char* const subGridLinePattern;

  ColorManager m_colorManager;

protected:

  static const float titleDistAxisHorizontal;
  static const float titleDistAxisVertical;
  static const float titleDistAxisVerticalLarge;

     static const char* const mainMaterial;
     static const char* const gridMaterial;

     float m_aspect;

     InfoLabel * m_label;
     float m_labelTextColor[3];

     SoGroup*      m_curveGroups;
     int m_preferredCurveGroup;

     SoSwitch*     m_clipping;
     SoSFPlane     m_planeRight;
     SoClipPlane*  m_clipPlaneRight;

  static void SetDefaultAxisParams(PoAxis* axis);

  static void GetAxisBounds(float minimum, float maximum, int ticks, int subticks, float & lowerBound, float & upperBound, float & tickSize);

  static PoDomain* createDomain(float horMin, float horMax, float vertMin, float vertMax);
  void finishCreatingCurveGroup(CurveGroup* curveGroup);

  virtual bool HandleMinMaxNearness(size_t coord, float& min, float& max) const = 0;

public:
  OIV2DGraph( float aspect, int preferredGroup);

  virtual OIV2DRangeInterface& RangeInterface() = 0;

  void setAspect( float aspect );
  void setRightClip(float distance);
  void clearLabel();
  void showLabel(bool visible);
  void setNormalLabelColor();
  void setHighlightLabelColor();

  CurveGroup * selectCurveGroup( int preferredCurveGroupIndex, const IValueComponentBase* valueComponent ) const;
  CurveGroup * getCurveGroup(OIV2DRangeInterface::TAxis axis) const;
  
  void removeCurvesByPredicate(std::function<bool(const Curve *)> predicate);
  virtual void updateLabel(Curve * curve, PoDomain* domain, float x, float y) = 0;


  ColorManager & colorManager ();
  virtual void setLineThickness( int t ) = 0;
  virtual void showMarkers(bool visible) = 0;
  virtual void setForegroundColor( float color[3] );
  virtual void removeAllCurves() = 0;
  virtual int getNumCurves() const = 0;
  virtual int getNumCurves(int group) const = 0;
  virtual void updateDomain(CurveGroup* group) = 0;
  void updateTitle(const CurveGroup* group);
  virtual void setUnit(CUnitNode::TUnitType unit);
  void updateDomains();
  void setPreferredGroup( int preferredGroup );

};

#endif  // _OIV2DGraph_h_
