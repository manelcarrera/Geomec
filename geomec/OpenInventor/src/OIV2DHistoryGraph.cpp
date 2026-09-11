#include "stdafx.h"

#include <MeshViz\nodes\PoDateFormatMapping.h>

#include "AnalysisPoint.h"
#include "HistoryCurve.h"
#include "ITensorGroup.h"
#include "IVectorResult.h"
#include "InfoLabel.h"
#include "Inventor\nodes\SoPickStyle.h"
#include "Inventor\nodes\SoTransparencyType.h"
#include "MeshVizXLM\MbVec3.h"
#include "MeshViz\graph\PoCartesianAxis.h"
#include "MeshViz\graph\PoLinearAxis.h"
#include "MeshViz\graph\PoTimeAxis.h"
#include "MeshViz\nodes\PoDomain.h"
#include "ModelBase.h"
#include "OIV2DHistoryGraph.h"
#include "OIV2DWellPathGraph.h"
#include "Result.h"
#include "SoGroupIterator.h"

const CGeomecTime OIV2DHistoryGraph::s_baseTime(1959, 3, 20);

void OIV2DHistoryGraph::PopulateDepletionStageValueMap(std::vector<DepletionStageValuePair> &depletionStageValueMap,
                                                       const IValueComponentBase *valueComponent,
                                                       const CAnalysisPoint *analysisPoint, CUnitNode::TUnitType unit) {
  depletionStageValueMap.clear();
  const IResultComponent *pComponent = dynamic_cast<const IResultComponent *>(valueComponent);
  assert(pComponent);

  IResult &result = (IResult &)(pComponent->Parent());
  const CDepletionStage *pStage = &((CModelBase &)result.Model()).InitialDepletionStage();

  while (pStage) {
    int nMode = result.Mode(pComponent->AnalysisType(), *pStage);
    if (nMode >= 0) {
      geo::CValue value;
      if (result.Component(pComponent->ComponentIndex(), nMode).Defined()) {
        bool bIsVectorComponent = valueComponent->Type() == IValueComponentBase::VECTOR;
        bool bIsTensorComponent = valueComponent->Type() == IValueComponentBase::TENSOR;

        if (!bIsVectorComponent && !bIsTensorComponent) {
          IResultComponent *resultComponent =
              dynamic_cast<IResultComponent *>(&result.Component(pComponent->ComponentIndex(), nMode));
          value = resultComponent->ValuePoint(analysisPoint->Point(), unit);
        } else if (bIsVectorComponent) {
          auto pVectorComponent = static_cast<const IVectorResult::CVectorComponent *>(
              &result.Component(pComponent->ComponentIndex(), nMode));
          assert(pVectorComponent);

          // different behavior between tensor eigenvectors and other vectors
          if (pVectorComponent->Parent().IsTensorVector()) {
            CTensor t = pVectorComponent->TensorData()->ValuePoint(analysisPoint->Point(), unit);

            assert(dynamic_cast<const ITensorGroup::CVectorComposite *>(&result) != 0);
            auto direction = (unsigned short)static_cast<const ITensorGroup::CVectorComposite *>(&result)
                                 ->Direction(); // min, med, or max

            geo::CMatrix eigenValueMatrix = t.GetPrincipalStr();
            auto component = 2 - direction; // reverse as smallest = first
            value = eigenValueMatrix.Value(component, component);
          } else {
            const IVectorResult *pResult = dynamic_cast<const IVectorResult *>(&result);
            const geo::IVector &v = pResult->VectorPoint(analysisPoint->Point(), *pVectorComponent);

            MbVec3d vector;
            if (unit == IQuantityDouble::FIELD_UNIT)
              vector = MbVec3d(pResult->ConvertToField(v.X()), pResult->ConvertToField(v.Y()),
                               pResult->ConvertToField(v.Z()));
            else
              vector = MbVec3d(v.X(), v.Y(), v.Z());

            value = vector.length();
          }
        } else /* bIsTensorComponent */
        {
          auto pTensorComponent = static_cast<const ITensorGroup::CComponentComposite::CFullTensorComponent *>(
              &result.Component(pComponent->ComponentIndex(), nMode));
          assert(pTensorComponent);
          CTensor t = pTensorComponent->TensorData()->ValuePoint(analysisPoint->Point(), unit);
          value = t.Norm();
        }
      }
      if (value.Valid())
        depletionStageValueMap.push_back(DepletionStageValuePair(pStage, value));
    }

    if (pStage->Last())
      pStage = 0;
    else {
      pStage = &pStage->Next();
    }
  }
}

PoTimeAxis *OIV2DHistoryGraph::createTimeAxis(float left, float right, const char *name, bool gridVisible) {
  SbVec3f start(left, 0.0f, 0.0f);
  PoTimeAxis *axis = new PoTimeAxis("1-1-2000", "1-1-2014", 0.0, PoTimeAxis::X);
  SetDefaultAxisParams(axis);
  axis->gridVisibility = gridVisible ? PoAxis::VISIBILITY_ON : PoAxis::VISIBILITY_OFF;

  axis->titleString = name;

  // axis->setMiscTextAttr(textAtt);
  // axis->timesPeriod = "2DAY";

  // axis->titleVisibility=PoAxis::VISIBILITY_ON;
  // axis->titleString.setValue("Time")

  // axis->gradStart = left;
  // axis->gradEnd = right;

  axis->titleDistAxis = titleDistAxisHorizontal;
  // axis->gridLengthGradOtherSide = 1000;

  return axis;
}

PoLinearAxis *OIV2DHistoryGraph::createVerticalAxis(float top, float bottom, const char *name, bool gridVisible) {
  SbVec3f start(top, 0.0f, 0.0f);
  PoLinearAxis *axis = new PoLinearAxis(start, bottom, PoCartesianAxis::YX);
  SetDefaultAxisParams(axis);
  axis->gridVisibility = gridVisible ? PoAxis::VISIBILITY_ON : PoAxis::VISIBILITY_OFF;

  axis->titleString = name;

  axis->gradStart = top;
  axis->gradEnd = bottom;

  axis->titlePosition = PoAxis::TITLE_MIDDLE;
  axis->titleDistAxis = titleDistAxisVerticalLarge;
  axis->gridLengthGradSide = 0;

  return axis;
}

CurveGroup *OIV2DHistoryGraph::createCurveGroup(CurveGroup::AxisPosition axisPosition, int linePattern) {
  const float ymin = 0.0f;
  const float ymax = 100.0f;
  const float xmin = 0.0f;
  const float xmax = 50.0f;

  CurveGroup *group = new CurveGroup;
  group->setName("HistoryCurveGroup");

  group->m_domain = createDomain(xmin, xmax, ymin, ymax);

  group->m_domain->transformType = PoDomain::TRANSFORM_01;
  group->m_axis = createVerticalAxis(ymin, ymax, "y", true);
  group->m_axisPosition = axisPosition;
  group->m_linePattern = linePattern;

  finishCreatingCurveGroup(group);

  return group;
}

void OIV2DHistoryGraph::createGraph() {
  SoTransparencyType *transparency = new SoTransparencyType;
  transparency->type = SoTransparencyType::BLEND;

  SoPickStyle *unpickable = new SoPickStyle;
  unpickable->style = SoPickStyle::UNPICKABLE;

  m_timeAxis = createTimeAxis(0, 50.0f, "Time", true);
  m_dateFormatMapping = new PoDateFormatMapping;
  m_dateFormatMapping->format = "dmY";

  // only domain for now ...
  m_domain = createDomain(0.0f, 50.0f, 0.0f, 100.0f);

  addChild(transparency);
  addChild(unpickable);
  addChild(m_domain);

  addChild(m_dateFormatMapping);
  addChild(m_timeAxis);

  m_curveGroups = new SoGroup;
  addChild(m_curveGroups);

  auto leftCurveGroup = createCurveGroup(CurveGroup::AxisPosition::AXIS_LEFT, 0xffff);
  auto rightCurveGroup = createCurveGroup(CurveGroup::AxisPosition::AXIS_RIGHT, 0xf0f0);

  m_curveGroups->addChild(leftCurveGroup);
  m_curveGroups->addChild(rightCurveGroup);

  addChild(unpickable);

  m_label = new InfoLabel;
  addChild(m_label);
}

void OIV2DHistoryGraph::removeAllCurves() {
  for (auto curveGroupNode : m_curveGroups) {
    static_cast<CurveGroup *>(curveGroupNode)->removeAllCurves();
    updateDomain(static_cast<CurveGroup *>(curveGroupNode));
  }
}

int OIV2DHistoryGraph::getNumCurves(int group) const {
  CurveGroup *curveGroup = static_cast<CurveGroup *>(m_curveGroups->getChild(group));
  return curveGroup->numCurves();
}

int OIV2DHistoryGraph::getNumCurves(CurveGroup *curveGroup) const { return curveGroup->numCurves(); }

int OIV2DHistoryGraph::getNumCurves() const {
  int numCurves = 0;
  for (auto curveGroupNode : m_curveGroups) {
    numCurves += static_cast<CurveGroup *>(curveGroupNode)->numCurves();
  }

  return numCurves;
}

Curve *OIV2DHistoryGraph::getCurve(CurveGroup *curveGroup, int index) const { return curveGroup->getCurve(index); }

bool OIV2DHistoryGraph::HandleMinMaxNearness(size_t coord, float &min, float &max) const {
  if (coord == 0) {
    if (min == max) {
      min -= 1.0f;
      max += 1.0f;

      return true;
    }

  } else {
    float offset = 1.0f;
    if (min == max) {
      if (max != 0) {
        float norm = std::abs(max);
        while (norm < 1.0f) {
          norm *= 10;
          offset /= 10;
        }
      }
      min -= offset;
      max += offset;

      return true;
    }
  }

  return false;
}

void OIV2DHistoryGraph::updateDomain(CurveGroup *curveGroup) {

  float rangeMinimum, rangeMaximum;
  getRangeInterval(curveGroup, rangeMinimum, rangeMaximum);

  // Keep the graph from exploding
  HandleMinMaxNearness(1, rangeMinimum, rangeMaximum);

  float yAxisMinimum, yAxisMaximum, yAxisTickSize;
  GetAxisBounds(rangeMinimum, rangeMaximum, 11, 1, yAxisMinimum, yAxisMaximum, yAxisTickSize);

  float domainMinimum, domainMaximum;
  getDomainInterval(domainMinimum, domainMaximum);

  // Keep the graph from exploding
  HandleMinMaxNearness(0, domainMinimum, domainMaximum);

  float xAxisMinimum, xAxisMaximum, xAxisTickSize;
  GetAxisBounds(domainMinimum, domainMaximum, 11, 1, xAxisMinimum, xAxisMaximum, xAxisTickSize);

  SbVec2f minVec(xAxisMinimum, yAxisMinimum);
  SbVec2f maxVec(xAxisMinimum + (xAxisMaximum - xAxisMinimum) / m_aspect, yAxisMaximum);
  m_domain->setValues(minVec, maxVec);
  curveGroup->m_domain->setValues(minVec, maxVec);

  char startDateString[16];
  char endDateString[16];

  GetDateString(xAxisMinimum, startDateString);
  GetDateString(xAxisMaximum, endDateString);

  m_dateFormatMapping->date1 = startDateString;
  m_dateFormatMapping->date2 = endDateString;
  m_dateFormatMapping->coord1 = xAxisMinimum;
  m_dateFormatMapping->coord2 = xAxisMaximum;

  m_timeAxis->coord = yAxisMinimum;
  m_timeAxis->startDate = startDateString;
  m_timeAxis->endDate = endDateString;
  m_timeAxis->gridLengthGradOtherSide = yAxisMaximum - yAxisMinimum;

  curveGroup->m_axis->gradPosition =
      curveGroup->m_axisPosition == CurveGroup::AxisPosition::AXIS_RIGHT ? PoAxis::GRAD_BELOW : PoAxis::GRAD_ABOVE;
  curveGroup->m_axis->start =
      SbVec3f(curveGroup->m_axisPosition == CurveGroup::AxisPosition::AXIS_RIGHT ? xAxisMaximum : xAxisMinimum,
              yAxisMinimum, 0.0f);
  curveGroup->m_axis->end = yAxisMaximum;
  curveGroup->m_axis->gradStart = yAxisMinimum;
  curveGroup->m_axis->gradEnd = yAxisMaximum;

  curveGroup->m_axis->step = yAxisTickSize;
  curveGroup->m_axis->gridLengthGradOtherSide = xAxisMaximum - xAxisMinimum;

  if (m_label)
    m_label->setup(m_domain, m_aspect);
}

void OIV2DHistoryGraph::getDomainInterval(float &globalMin, float &globalMax) const {
  if (getNumCurves() == 0) {
    globalMin = s_baseTime.DaysTo(CGeomecTime(1990, 1));
    globalMax = s_baseTime.DaysTo(CGeomecTime(2020, 1));
    return;
  }

  globalMin = std::numeric_limits<float>::max();
  globalMax = -std::numeric_limits<float>::max();

  for (auto curveGroupNode : m_curveGroups) {
    UpdateDomainInterval(static_cast<CurveGroup *>(curveGroupNode), globalMin, globalMax);
  }
}

void OIV2DHistoryGraph::getRangeInterval(CurveGroup *curveGroup, float &globalMin, float &globalMax) const {
  curveGroup->getMinMax(globalMin, globalMax);
}

OIV2DHistoryGraph::OIV2DHistoryGraph(float aspect, int preferredGroup)
    : OIV2DGraph(aspect, preferredGroup), m_markersVisible(true) {
  setName("HistoryGraph");
  createGraph();
}

OIV2DRangeInterface &OIV2DHistoryGraph::RangeInterface() {
  assert(false);
  return *(OIV2DRangeInterface *)0;
}

void OIV2DHistoryGraph::removeCurve(const IValueComponentBase *valueComponent, const CAnalysisPoint *analysisPoint) {
  auto predicate = [valueComponent, analysisPoint](const Curve *node) -> bool {
    const HistoryCurve *historyCurve = static_cast<const HistoryCurve *>(node);
    return (historyCurve->valueComponent() == valueComponent && historyCurve->analysisPoint() == analysisPoint);
  };

  removeCurvesByPredicate(predicate);
}

Curve *OIV2DHistoryGraph::findCurve(const IValueComponentBase *valueComponent, const CAnalysisPoint *analysisPoint) {
  for (auto curveGroupNode : m_curveGroups) {
    Curve *curve = static_cast<CurveGroup *>(curveGroupNode)->findCurve(valueComponent, analysisPoint);
    if (curve)
      return curve;
  }

  return 0;
}

bool OIV2DHistoryGraph::addCurve(const IValueComponentBase *valueComponent, const CAnalysisPoint *analysisPoint,
                                 CUnitNode *unitNode) {
  // Create curve
  std::vector<DepletionStageValuePair> depletionStageValueMap;
  PopulateDepletionStageValueMap(depletionStageValueMap, valueComponent, analysisPoint, unitNode->Unit());
  if (depletionStageValueMap.empty())
    return false;

  int colorIndex = colorManager().allocateColorIndex();
  auto newCurve = new HistoryCurve(depletionStageValueMap, valueComponent, analysisPoint, unitNode->Unit(), colorIndex);

  newCurve->setMarkerSize(10.0f);
  newCurve->markerFilterType = m_markersVisible ? PoCurve::ALL_POINTS : PoCurve::INACTIVE;

  CurveGroup *selectedCurveGroup = selectCurveGroup(m_preferredCurveGroup, valueComponent);

  if (selectedCurveGroup) {
    selectedCurveGroup->addCurve(newCurve);
    selectedCurveGroup->whichChild = SO_SWITCH_ALL;
    updateTitle(selectedCurveGroup);
  }
  return true;
}

void OIV2DHistoryGraph::setUnit(CUnitNode::TUnitType unit) { OIV2DGraph::setUnit(unit); }

QString OIV2DHistoryGraph::getGroupUnitName(int g) {
  CurveGroup *group = static_cast<CurveGroup *>(m_curveGroups->getChild(g));
  return getGroupUnitName(group);
}

QString OIV2DHistoryGraph::getGroupUnitName(CurveGroup *group) {
  if (group->numCurves() == 0)
    return "empty";
  return group->getCurve(0)->valueComponent()->UnitName();
}

Curve *OIV2DHistoryGraph::getCurve(int i) {
  for (auto curveGroupNode : m_curveGroups) {
    auto *curveGroup = static_cast<CurveGroup *>(curveGroupNode);
    if (i < getNumCurves(curveGroup)) {
      return getCurve(curveGroup, i);
    } else {
      i -= getNumCurves(curveGroup);
    }
  }
  return 0;
}

void OIV2DHistoryGraph::showMarkers(bool visible) {
  m_markersVisible = visible;

  for (int i = 0; i < getNumCurves(); ++i) {
    Curve *curve = getCurve(i);
    curve->markerFilterType = visible ? PoCurve::ALL_POINTS : PoCurve::INACTIVE;
  }
}

void OIV2DHistoryGraph::updateLabel(Curve *curve, PoDomain *domain, float x, float y) {
  m_label->setup(domain, m_aspect);

  m_label->setPosition(x, y);
  setNormalLabelColor();

  float xUsed, yUsed;
  char dateString[16];

  xUsed = x;
  yUsed = y;

  GetDateString((int)x, dateString);

  auto min = domain->min.getValue();
  auto max = domain->max.getValue();

  float xRange = max[0] - min[0];
  float yRange = max[1] - min[1];

  float tolerance = .01f;
  for (auto i = 0; i < curve->getNumPoints(); ++i) {
    const SbVec2f &point = curve->getPoint(i);

    float errorx = std::abs(point[0] - x) / xRange;
    float errory = std::abs(point[1] - y) / yRange;

    if (errorx < tolerance && errory < tolerance) {
      setHighlightLabelColor();

      m_label->setPosition(point[0], point[1]);
      GetDateString((int)point[0], dateString);
      xUsed = point[0];
      yUsed = point[1];
      tolerance = std::max(errorx, errory);
    }
  }

  auto label = curve->getLabel();
  std::string tmpString = label.toStdString();
  const char *propertyName = tmpString.c_str();

  char text[128];
  sprintf(text, "%s (%s, %f)", propertyName, dateString, yUsed);
  m_label->addText(text);
}

void OIV2DHistoryGraph::GetDateString(int daysPassed, char *dateString) {
  CGeomecTime geomecStartTime = s_baseTime + daysPassed;
  sprintf(dateString, "%d-%d-%d", geomecStartTime.GetDay(), geomecStartTime.GetMonth(), geomecStartTime.GetYear());
}

void OIV2DHistoryGraph::UpdateDomainInterval(CurveGroup *curveGroup, float &globalMin, float &globalMax) const {
  for (int i = 0; i < getNumCurves(curveGroup); ++i) {
    Curve *curve = getCurve(curveGroup, i);

    float minval, maxval;
    curve->getMinMax(minval, maxval, 0);
    globalMin = std::min(globalMin, minval);
    globalMax = std::max(globalMax, maxval);
  }
}

void OIV2DHistoryGraph::setForegroundColor(float foregroundColor[3]) {
  char diffuseColor[50];
  sprintf(diffuseColor, "diffuseColor [%.2f %.2f %.2f]", foregroundColor[0], foregroundColor[1], foregroundColor[2]);

  m_timeAxis->set(mainMaterial, diffuseColor);
  m_timeAxis->set(gridMaterial, diffuseColor);

  for (auto curveGroupNode : m_curveGroups) {
    auto *curveGroup = static_cast<CurveGroup *>(curveGroupNode);
    curveGroup->m_axis->set(mainMaterial, diffuseColor);
    curveGroup->m_axis->set(gridMaterial, diffuseColor);
  }

  OIV2DGraph::setForegroundColor(foregroundColor);
}

void OIV2DHistoryGraph::setLineThickness(int t) {
  for (auto c = 0; c < getNumCurves(); ++c) {
    Curve *curve = getCurve(c);
    curve->setLineWidth(t);
  }
}

void OIV2DHistoryGraph::collectComponents(int group, std::vector<const IValueComponentBase *> &components) {
  auto curveGroup = static_cast<CurveGroup *>(m_curveGroups->getChild(group));
  for (auto node : curveGroup->m_curves) {
    HistoryCurve *curve = static_cast<HistoryCurve *>(node);
    if (std::find(components.begin(), components.end(), curve->valueComponent()) == components.end()) {
      components.push_back(curve->valueComponent());
    }
  }
}
