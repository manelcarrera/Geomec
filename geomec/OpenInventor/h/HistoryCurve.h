#ifndef _HistoryCurve_h_
#define _HistoryCurve_h_

class CAnalysisPoint;

#include "Curve.h"
#include "DepletionStageValuePair.h"

class HistoryCurve : public Curve
{
    const CAnalysisPoint * m_analysisPoint;

    QString m_resultPath;

    void CreateDepletionStageValueMap();
    void FillCurve(std::vector<DepletionStageValuePair> & depletionStageValueMap);
  
public:
    HistoryCurve(std::vector<DepletionStageValuePair> & depletionStageValueMap, const IValueComponentBase* valueComponent, const CAnalysisPoint * analysisPoint, CUnitNode::TUnitType unit, int colorIndex);
    const CAnalysisPoint * analysisPoint() const;

    virtual QString getTextRepresentation() const;
    virtual void onUnitChanged();
};

#endif  // _HistoryCurve_h_
