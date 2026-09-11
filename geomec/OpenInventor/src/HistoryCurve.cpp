#include "stdafx.h"

#include "HistoryCurve.h"
#include "GeomecTime.h"
#include "DepletionStage.h"
#include "OIV2DHistoryGraph.h"

HistoryCurve::HistoryCurve( std::vector<DepletionStageValuePair> & depletionStageValueMap, const IValueComponentBase* valueComponent, const CAnalysisPoint * analysisPoint, CUnitNode::TUnitType unit, int colorIndex )
    : Curve(valueComponent, unit, colorIndex)
    , m_analysisPoint(analysisPoint)
{
    FillCurve(depletionStageValueMap);
}

void HistoryCurve::FillCurve(std::vector<DepletionStageValuePair> & depletionStageValueMap)
{
    point.deleteValues(0, -1);

    int s = 0;
    for (auto svpair : depletionStageValueMap)
    {
        const CDepletionStage * stage = svpair.first;
        const CGeomecTime & stageTime = stage->Time();
        int days = OIV2DHistoryGraph::s_baseTime.DaysTo(stageTime);
        const geo::CValue value = svpair.second;
        if (value.Valid())
        {
            SbVec2f v((float) days, value.Value());
            point.set1Value(s++, v);
        }
    }
}

void HistoryCurve::onUnitChanged()
{
    std::vector<DepletionStageValuePair> depletionStageValueMap;
    OIV2DHistoryGraph::PopulateDepletionStageValueMap(depletionStageValueMap, valueComponent(), analysisPoint(), unit());
    FillCurve(depletionStageValueMap);
}

QString HistoryCurve::getTextRepresentation() const
{
    return QString("");
}

const CAnalysisPoint * HistoryCurve::analysisPoint() const
{
    return m_analysisPoint;
}
