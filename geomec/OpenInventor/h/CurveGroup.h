#ifndef _CurveGroup_h_
#define _CurveGroup_h_

class PoDomain;
class PoLinearAxis;
class Curve;
class CAnalysisPoint;

#include "Inventor\nodes\SoSwitch.h"

class CurveGroup : public SoSwitch
{
public:

  enum class AxisPosition
  {
    AXIS_LEFT,
    AXIS_RIGHT,
    AXIS_BOTTOM,
    AXIS_TOP
  };

  AxisPosition m_axisPosition;
  int m_linePattern;

  PoDomain*     m_domain;
  PoLinearAxis* m_axis;
  SoGroup*      m_curves;

  std::size_t   numCurves() const;

  Curve * getCurve(std::size_t i) const;

  void addCurve(Curve * curve);
  bool containsCurve( Curve * curve ) const;

  void getMinMax(float& globalMin, float& globalMax) const;
  void getMinMaxBound(float minX, float maxX, float& minVal, float& maxVal) const;

  Curve * findCurve( const IValueComponentBase* valueComponent, const CAnalysisPoint * analysisPoint );
  std::string getCommonDenominatorName() const;
  void removeAllCurves();
  std::string getTitle() const;
  void updateTitle() const;
};

#endif  // _CurveGroup_h_
