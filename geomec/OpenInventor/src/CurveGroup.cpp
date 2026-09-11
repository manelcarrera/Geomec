#include "stdafx.h"

#include "CurveGroup.h"
#include "Curve.h"
#include "MeshViz\graph\PoLinearAxis.h"
#include "HistoryCurve.h"
#include "SoGroupIterator.h"

std::size_t CurveGroup::numCurves() const
{
  return m_curves->getNumChildren();
}

Curve * CurveGroup::getCurve( std::size_t i ) const
{
  return static_cast<Curve *> (m_curves->getChild(i));
}

void CurveGroup::addCurve(Curve * curve )
{  
  m_curves->addChild(curve);
  curve->setLinePattern(m_linePattern);
}

bool CurveGroup::containsCurve( Curve * curve ) const
{
  return m_curves->findChild(curve) >= 0;
}

void CurveGroup::updateTitle() const
{
  std::string title = getTitle();
  m_axis->titleString = title;
  if (m_axisPosition == CurveGroup::AxisPosition::AXIS_LEFT || m_axisPosition == CurveGroup::AxisPosition::AXIS_RIGHT)
  {
    m_axis->titlePath = title.length() < 8 ? PoAxis::PATH_DOWN : PoAxis::PATH_RIGHT;
  }
}

Curve * CurveGroup::findCurve( const IValueComponentBase* valueComponent, const CAnalysisPoint * analysisPoint )
{
  auto predicate = [&valueComponent, & analysisPoint](SoNode * curveNode)->bool
  {
    assert (dynamic_cast<HistoryCurve *> (curveNode) != 0);
    HistoryCurve * curve = static_cast<HistoryCurve *> (curveNode);
    return (curve->valueComponent() == valueComponent && curve->analysisPoint() == analysisPoint);
  };

  decltype (begin (m_curves)) curveIter;
  if ((curveIter = std::find_if(begin (m_curves), end(m_curves), predicate)) != end(m_curves))
  {
    return static_cast<Curve *>(* curveIter);
  }
  else
    return 0;
}

std::string CurveGroup::getTitle() const
{
  std::string title = "";
  if (numCurves() != 0)
  {
    std::string unitName = getCurve(0)->getUnitName().toStdString();
    std::string commonDenominatorName = getCommonDenominatorName();
    title = commonDenominatorName + " [" + unitName +"]";

  }

  return title;
}
std::string CurveGroup::getCommonDenominatorName() const
{
  if (numCurves() == 0)
    return "";

  std::string firstCurveName = getCurve(0)->getResultPath(0).toStdString();
  std::string commonDenominator = "";

  int lastDepth = false;
  for (int depth = 1; !lastDepth; ++depth) // how far we look into the firstCurveName
  {
    size_t pos = 0;
    for (int s = 0; s < depth; ++s) // look into firstCurveName with specified depth
    {
      pos = firstCurveName.find ("::", pos);
      if (pos == std::string::npos)
      {
        lastDepth = true;
        pos = firstCurveName.length();
      }
      else
        pos += 2;
    }

    std::string denominator = firstCurveName.substr(0, lastDepth ? pos : pos - 2); // found a substring with specified depth

    // now see if all other curves start with same substring
    bool isCommon = true;
    for (int c = 0; c < numCurves(); ++c)
    {
      std::string curveName = getCurve(c)->getResultPath(0).toStdString();
      if (curveName.find(denominator, 0) == std::string::npos)
      {
        isCommon = false;
        break;
      }
    }
    if (isCommon)
      commonDenominator = denominator;
    else
      break;   
  }
  return commonDenominator;
}

void CurveGroup::removeAllCurves()
{
  m_curves->removeAllChildren();
}

void CurveGroup::getMinMax( float& globalMin, float& globalMax ) const
{
  if(numCurves() == 0)
  {
    globalMin = 0;
    globalMax = 100;
    return;
  }

  globalMin = FLT_MAX;
  globalMax = -FLT_MAX;

  int valueCoordinate = (m_axisPosition == AxisPosition::AXIS_LEFT || m_axisPosition == AxisPosition::AXIS_RIGHT) ? 1 : 0;

  for(int i=0; i < numCurves(); ++i)
  {
    auto curve = getCurve(i);

    float minval, maxval;
    curve->getMinMax(minval, maxval, valueCoordinate);
    globalMin = std::min(globalMin, minval);
    globalMax = std::max(globalMax, maxval);
  }
}


void CurveGroup::getMinMaxBound(float minX, float maxX, float& minVal, float& maxVal) const
{
  minVal = FLT_MAX;
  maxVal = -FLT_MAX;

  for (int i = 0; i < numCurves(); ++i)
  {
  auto curve = getCurve(i);

  float minval, maxval;
  curve->getMinMaxBound(minX, maxX, minval, maxval);
  minVal = std::min(minVal, minval);
  maxVal = std::max(maxVal, maxval);
  }
}

