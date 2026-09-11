#ifndef _OIV2DHistoryGraph_h_
#define _OIV2DHistoryGraph_h_

class PoDateFormatMapping;
class PoTimeAxis;
class CAnalysisPoint;
class PoLinearAxis;

#include "OIV2DGraph.h"
#include "GeomecTime.h"
#include "CurveGroup.h"
#include "DepletionStageValuePair.h"

class OIV2DHistoryGraph : public OIV2DGraph
{
  PoDomain*            m_domain;

  PoDateFormatMapping * m_dateFormatMapping;
  PoTimeAxis *         m_timeAxis;

  bool m_markersVisible;

protected:

  virtual bool HandleMinMaxNearness(size_t coord, float& min, float& max) const;

public:
  static const CGeomecTime s_baseTime;

  static void GetDateString( int days, char * dateString );

  OIV2DHistoryGraph (float aspect, int preferredGroup);

  virtual OIV2DRangeInterface& RangeInterface();

  bool addCurve( const IValueComponentBase* valueComponent, const CAnalysisPoint * analysisPoint, CUnitNode * unitNode );
  void removeCurve( const IValueComponentBase* valueComponent, const CAnalysisPoint * analysisPoint);
  Curve * findCurve( const IValueComponentBase* valueComponent, const CAnalysisPoint * analysisPoint);
  Curve * getCurve (int i);
  
  PoTimeAxis * createTimeAxis(float left, float right, const char* name, bool gridVisible);
  PoLinearAxis* createVerticalAxis(float bottom, float top, const char* name, bool gridVisible);
  CurveGroup* createCurveGroup(CurveGroup::AxisPosition position, int linePattern);
  void createGraph();
  void updateDomain(CurveGroup * curveGroup); 
  virtual void setUnit(CUnitNode::TUnitType unit);

  void removeAllCurves();
  Curve* getCurve(CurveGroup * curveGroup, int index) const;

  int getNumCurves() const;
  int getNumCurves(CurveGroup * curveGroup) const;
  int getNumCurves(int group) const;
  void getDomainInterval(float& globalMin, float& globalMax) const;
  void UpdateDomainInterval( CurveGroup * curveGroup, float &globalMin, float &globalMax ) const;

  void getRangeInterval(CurveGroup * curveGroup, float& globalMin, float& globalMax) const;
  void showMarkers(bool visible);
  virtual void updateLabel(Curve * curve, PoDomain* domain, float x, float y) override;
  void setForegroundColor( float textColor[3] );

  static void PopulateDepletionStageValueMap(
    std::vector<DepletionStageValuePair> & depletionStageValueMap, 
    const IValueComponentBase* valueComponent, 
    const CAnalysisPoint * analysisPoint, 
    CUnitNode::TUnitType unit );

  virtual void setLineThickness( int t );
  void collectComponents( int group, std::vector<const IValueComponentBase *> & components );
  QString getGroupUnitName(CurveGroup * group);
  QString getGroupUnitName(int group);

};

#endif  // _OIV2DHistoryGraph_h_
