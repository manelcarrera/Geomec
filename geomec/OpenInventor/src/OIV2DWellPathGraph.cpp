#include "stdafx.h"

#include <MeshViz/graph/PoGenAxis.h>
#include <MeshViz/graph/PoLinearAxis.h>
#include <MeshViz/graph/PoRectangle.h>
#include <MeshViz/nodes/PoDomain.h>

#include "Inventor/nodes/SoPickStyle.h"
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoMatrixTransform.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoTransparencyType.h>

#include "Curve.h"
#include "InfoLabel.h"
#include "NewWellPath.h"
#include "NewWellPathCurve.h"
#include "OIV2DWellPathGraph.h"
#include "SoGroupIterator.h"

namespace {
const char *getDepthAxisName(OIV2DWellPathGraph::DepthAxis depthAxis) {
  switch (depthAxis) {
  case OIV2DWellPathGraph::DEPTH_AXIS_AHD:
    return "AHD";
  case OIV2DWellPathGraph::DEPTH_AXIS_TVD:
    return "TVD";
  case OIV2DWellPathGraph::DEPTH_AXIS_Z:
    return "Depth";
  default:
    return "unknown";
  }
}

// poor man's TVD2TMD
double TVD2TMD(const CNewWellPath &newWellPath, double value) {
  auto predicate = [value](const CNewWellPoint &p) -> bool { return p.TVD() >= value; };

  auto begin = newWellPath.GraphPoints().begin();
  auto end = newWellPath.GraphPoints().end();
  auto iter = std::find_if(begin, end, predicate);
  if (iter == end) {
    return 0;
  }
  auto &above = *iter;
  double aboveTMD = above.TMD();
  double aboveTVD = above.TVD();
  if (iter == begin) {
    return aboveTMD;
  }

  auto &below = *--iter;
  double belowTMD = below.TMD();
  double belowTVD = below.TVD();

  assert(belowTVD < value);

  double tmd = belowTMD + (value - belowTVD) * ((aboveTMD - belowTMD) / (aboveTVD - belowTVD));

  return tmd;
}
} // namespace

const int OIV2DWellPathGraph::numGraduationsYAxis = 6;
const float OIV2DWellPathGraph::yAxisOffset = .07f;

/**
 * Set a bunch of default parameters for an axis, in addition to name and grid visibility
 */

PoGenAxis *OIV2DWellPathGraph::createVerticalAxis(const char *name, bool gridVisible) {
  PoGenAxis *axis = new PoGenAxis;
  SetDefaultAxisParams(axis);

  axis->gridVisibility = gridVisible ? PoAxis::VISIBILITY_ON : PoAxis::VISIBILITY_OFF;

  axis->titleString = name;
  axis->titlePosition = PoAxis::TITLE_MIDDLE;
  axis->titleDistAxis = titleDistAxisVertical;
  axis->type = PoCartesianAxis::YX;

  return axis;
}

PoLinearAxis *OIV2DWellPathGraph::createHorizontalAxis(CurveGroup::AxisPosition pos, bool gridVisible) {
  float left = 0.0f;
  float right = 1.0f;

  float y = (pos == CurveGroup::AxisPosition::AXIS_TOP) ? m_ranges.m_top : m_ranges.m_bottom;

  SbVec3f start(left, y, 0.0f);
  PoLinearAxis *axis = new PoLinearAxis(start, right, PoCartesianAxis::XY);
  SetDefaultAxisParams(axis);

  axis->titleString = "-";

  axis->gradPosition = (pos == CurveGroup::AxisPosition::AXIS_BOTTOM) ? PoAxis::GRAD_BELOW : PoAxis::GRAD_ABOVE;

  axis->titleDistAxis = titleDistAxisHorizontal;
  axis->gridLengthGradOtherSide = m_ranges.m_top - m_ranges.m_bottom;
  axis->gridVisibility = gridVisible ? PoAxis::VISIBILITY_ON : PoAxis::VISIBILITY_OFF;

  return axis;
}

void OIV2DWellPathGraph::buildFormationNodes() {
  int visibility = (getDepthAxis() == DEPTH_AXIS_Z) ? SO_SWITCH_NONE : SO_SWITCH_ALL;

  m_formationSections = new SoGroup;
  m_formationSections->setName("FormationSections");
  m_formationSectionsTranslation = new SoTranslation;

  SoSeparator *sep = new SoSeparator;
  sep->addChild(m_formationSectionsTranslation);
  sep->addChild(m_formationSections);

  m_formationSectionsSwitch = new SoSwitch;
  m_formationSectionsSwitch->addChild(sep);
  m_formationSectionsSwitch->whichChild = visibility;

  m_formationLinesSeparator = new SoSeparator;
  m_formationLinesSeparator->setName("FormationLines");
  m_formationLinesTransform = new SoMatrixTransform;
  m_formationLinesSwitch = new SoSwitch;
  m_formationLinesSwitch->whichChild = visibility;
  m_formationLines = new SoIndexedLineSet;

  m_formationLinesSeparator->addChild(m_formationLinesTransform);
  m_formationLinesSeparator->addChild(m_formationLines);
  m_formationLinesSwitch->addChild(m_formationLinesSeparator);

  addChild(m_formationSectionsSwitch);
  addChild(m_formationLinesSwitch);
}

void OIV2DWellPathGraph::setupFormationSections(const std::vector<FormationSectionParams> &params) {
  const float width = .05f;
  const float offset = 0.0f;

  float x0 = offset;
  float x1 = offset + width;

  SoVertexProperty *vertexProp = new SoVertexProperty;
  vertexProp->materialBinding = SoVertexProperty::PER_VERTEX;

  auto formationLines = new SoIndexedLineSet;
  m_formationLinesSeparator->replaceChild(m_formationLines, formationLines);
  m_formationLines = formationLines;

  int formationIndex = 0;
  int i = 0, j = 0;

  m_formationSections->removeAllChildren();
  for (auto section : params) {
    float top = (float)section.top;
    float bottom = (float)section.bottom;

    // clipping; watch out for the signs, as depth is negative
    if (bottom >= m_ranges.m_top)
      continue;
    if (top <= m_ranges.m_bottom)
      continue;

    if (top > m_ranges.m_top && bottom <= m_ranges.m_top)
      top = m_ranges.m_top;

    if (top >= m_ranges.m_bottom && bottom < m_ranges.m_bottom)
      bottom = m_ranges.m_bottom;

    if (top == bottom)
      continue;

    SbVec2f p1(x0, top);
    SbVec2f p2(x1, bottom);
    PoRectangle *rect = new PoRectangle(p1, p2);

    char colorStr[128];
    sprintf(colorStr, "shininess 0.9 ambientColor %f %f %f diffuseColor %f %f %f", section.color.redF(),
            section.color.greenF(), section.color.blueF(), section.color.redF(), section.color.greenF(),
            section.color.blueF());
    rect->set(mainMaterial, (const char *)colorStr);
    m_formationSections->addChild(rect);

    if (formationIndex > 0) {
      m_formationLines->coordIndex.set1Value(j++, i);
      m_formationLines->coordIndex.set1Value(j++, i + 1);
      m_formationLines->coordIndex.set1Value(j++, SO_END_LINE_INDEX);

      uint32_t alpha = 0x40; // make lines semi-transparent
      uint32_t colorval = (section.color.rgba() << 8) | alpha;
      vertexProp->orderedRGBA.set1Value(i, colorval);
      vertexProp->vertex.set1Value(i++, SbVec3f(0.0f, top, 0.0f));
      vertexProp->orderedRGBA.set1Value(i, colorval);
      vertexProp->vertex.set1Value(i++, SbVec3f(1.0f, top, 0.0f));
    }

    formationIndex++;
  }

  m_formationLines->vertexProperty = vertexProp;

  updateFormationSectionsTransforms();
}

void OIV2DWellPathGraph::setupFormationSections() {
  if (m_newWellPaths.size() == 1) {
    {
      setupFormationSections(*m_newWellPaths[0]);
    }
  }
}

/**
 * Create the colored formation bar on the left-hand side of the graph, including
 * the horizontal lines for the formation boundaries.
 */
void OIV2DWellPathGraph::setupFormationSections(const CNewWellPath &wellPath) {
  std::vector<FormationSectionParams> paramsVec;

  if (getDepthAxis() == DEPTH_AXIS_AHD) {
    for (auto section : wellPath.FormationSections()) {
      FormationSectionParams params;
      params.top = -CLengthQuantity(section.Top().TMD()).Value(m_unit);
      params.top = std::min(params.top, (double)m_ranges.m_topAHD);

      params.bottom = -CLengthQuantity(section.Bottom().TMD()).Value(m_unit);
      params.bottom = std::max(params.bottom, (double)m_ranges.m_bottomAHD);

      params.color = section.Formation()->Color();

      paramsVec.push_back(params);
    }
  } else {
    for (auto section : wellPath.FormationSections()) {
      FormationSectionParams params;
      params.top = -CLengthQuantity(section.Top().TVD()).Value(m_unit);
      params.top = std::min(params.top, (double)m_ranges.m_topTVD);

      params.bottom = -CLengthQuantity(section.Bottom().TVD()).Value(m_unit);
      params.bottom = std::max(params.bottom, (double)m_ranges.m_bottomTVD);

      params.color = section.Formation()->Color();

      paramsVec.push_back(params);
    }
  }

  setupFormationSections(paramsVec);
}

CurveGroup *OIV2DWellPathGraph::createCurveGroup(CurveGroup::AxisPosition axisPosition, int linePattern) {
  const float xmin = 0.0f;
  const float xmax = 1.0f;

  CurveGroup *group = new CurveGroup;
  group->setName("CurveGroup");
  group->m_axisPosition = axisPosition;
  group->m_domain = new PoDomain;
  group->m_domain->setValues(SbVec2f(xmin, m_ranges.m_bottom), SbVec2f(xmax / m_aspect, m_ranges.m_top));
  group->m_domain->transformType = PoDomain::TRANSFORM_01;
  group->m_axis = createHorizontalAxis(axisPosition, true);
  group->m_linePattern = linePattern;

  finishCreatingCurveGroup(group);

  return group;
}

bool OIV2DWellPathGraph::setupCurve(Curve *newCurve) {
  int numPoints = newCurve->point.getNum();
  if (numPoints == 0) {
    delete newCurve;
    return false;
  }

  newCurve->setLineWidth(m_lineWidth);
  newCurve->setMarkerSize(5.0f);
  newCurve->markerFilterType = m_markersVisible ? PoCurve::ALL_POINTS : PoCurve::INACTIVE;

  // Find a group to put the curve into
  CurveGroup *group = selectCurveGroup(m_preferredCurveGroup, newCurve->valueComponent());
  assert(group != 0);

  group->addCurve(newCurve);
  group->whichChild = SO_SWITCH_ALL;

  updateDomain(group);
  updateTitle(group);

  return true;
}

void OIV2DWellPathGraph::createGraph() {
  SoTransparencyType *transparency = new SoTransparencyType;
  transparency->type = SoTransparencyType::BLEND;

  SoPickStyle *unpickable = new SoPickStyle;
  unpickable->style = SoPickStyle::UNPICKABLE;

  m_yAxisLeft = createVerticalAxis("AHD", true);
  m_yAxisRight = createVerticalAxis("TVD", false);
  m_yAxisRight->gradPosition = PoAxis::GRAD_BELOW;

  m_yAxisLeftSwitch = new SoSwitch;
  m_yAxisLeftSwitch->addChild(m_yAxisLeft);
  m_yAxisLeftSwitch->whichChild = SO_SWITCH_ALL;

  m_yAxisRightSwitch = new SoSwitch;
  m_yAxisRightSwitch->addChild(m_yAxisRight);
  m_yAxisRightSwitch->whichChild = SO_SWITCH_ALL;

  m_verticalDomain = createDomain(0.0f, 1.0f, m_ranges.m_bottom, m_ranges.m_top);

  addChild(transparency);
  addChild(unpickable);
  addChild(m_verticalDomain);
  addChild(m_yAxisLeftSwitch);
  addChild(m_yAxisRightSwitch);

  buildFormationNodes();

  m_curveGroups = new SoGroup;
  addChild(m_curveGroups);

  auto topCurveGroup = createCurveGroup(CurveGroup::AxisPosition::AXIS_TOP, 0xffff);
  auto bottomCurveGroup = createCurveGroup(CurveGroup::AxisPosition::AXIS_BOTTOM, 0xf0f0);

  m_curveGroups->addChild(topCurveGroup);
  m_curveGroups->addChild(bottomCurveGroup);

  addChild(unpickable);

  m_label = new InfoLabel;
  addChild(m_label);
}

void OIV2DWellPathGraph::updateVerticalAxes() {
  SbVec3f start(-yAxisOffset * m_aspect, m_ranges.m_top, 0.0f);
  float end = m_ranges.m_bottom;

  const char *unitName = (m_unit == CUnitNode::TUnitType::SI_UNIT) ? "m" : "ft";
  const char *leftAxisName = getDepthAxisName(getDepthAxis());
  const char *rightAxisName = "TVD";

  SbString axisTitle;
  const char *axisTitleFormat = "%s [%s]";

  // AHD axis
  m_yAxisLeft->start = start;
  m_yAxisLeft->end = end;
  m_yAxisLeft->gridLengthGradOtherSide = (1.0f + yAxisOffset) * m_aspect;

  axisTitle.sprintf(axisTitleFormat, leftAxisName, unitName);
  m_yAxisLeft->titleString = axisTitle;

  // TVD axis
  // The tvd axis is placed in the same domain as the ahd axis, so it gets the same start and end values.
  // The graduations however are custom strings, based on the conversion from AHD -> TVD
  m_yAxisRight->start = start;
  m_yAxisRight->end = end;

  rightAxisName = strncmp(leftAxisName, "AHD", 3) == 0 ? "TVD" : "AHD";
  axisTitle.sprintf(axisTitleFormat, rightAxisName, unitName);
  m_yAxisRight->titleString = axisTitle;

  // If we're showing wells in AHD, then both axes are shown: left = AHD, right = TVD. Otherwise,
  // the left axis will show TVD or Z, and the right axis is hidden.
  m_yAxisRightSwitch->whichChild = (m_newWellPaths.size() == 1) ? SO_SWITCH_ALL : SO_SWITCH_NONE;

  int iStep = (m_ranges.m_bottom - m_ranges.m_top) / m_ranges.m_tick[OIV2DRangeInterface::AXIS_LEFT];
  int iGraduations = 1 + m_ranges.m_tick[OIV2DRangeInterface::AXIS_LEFT];

  for (int i = 0; i < iGraduations; ++i) {
    double depthValue = -(m_ranges.m_top + i * iStep);

    SbString s;
    const char *formatStr = "%.0f";
    s.sprintf(formatStr, depthValue);
    m_yAxisLeft->gradList.set1Value(i, s);

    if (getDepthAxis() == DEPTH_AXIS_AHD) // then also show TVD
    {
      double tvd = 0.0f;

      // Convert AHD to TVD
      CLengthQuantity tmp(depthValue, m_unit);
      if (!m_newWellPaths.empty())
        tvd = tmp.Convert(m_newWellPaths[0]->TMD2TVD(tmp.Value()), m_unit, CDoubleQuantity::SI_UNIT);

      s.sprintf(formatStr, tvd);
      m_yAxisRight->gradList.set1Value(i, s);
    }

    if (getDepthAxis() == DEPTH_AXIS_TVD) // then also show AHD
    {
      double tmd = 0.0f;

      // Convert TVD to AHD
      CLengthQuantity tmp(depthValue, m_unit);
      if (!m_newWellPaths.empty())
        tmd = tmp.Convert(::TVD2TMD(*m_newWellPaths[0], tmp.Value()), m_unit,
                          CDoubleQuantity::SI_UNIT); // poor man's TVD2TMD

      s.sprintf(formatStr, tmd);
      m_yAxisRight->gradList.set1Value(i, s);
    }
  }
}

void OIV2DWellPathGraph::refreshVisualization() {
  updateVerticalDomain();
  updateVerticalAxes();
  updateFormationSectionsTransforms();
  updateDomains();
}

void OIV2DWellPathGraph::updateVerticalDomain() {
  m_verticalDomain->setValues(SbVec2f(0.0f, m_ranges.m_bottom), SbVec2f(1.0f, m_ranges.m_top));

  setupFormationSections();
  updateFormationSectionsTransforms();
}

void OIV2DWellPathGraph::updateFormationSectionsTransforms() {
  m_formationSectionsTranslation->translation = SbVec3f(m_aspect * 1.05f, 0.0f, 0.0f);

  SoMatrixTransform *transform = m_verticalDomain->getMatrixTransform();
  SbMatrix matrix = transform->matrix.getValue();
  matrix.scale(SbVec3f(m_aspect, 1.0f, 1.0f));
  m_formationLinesTransform->matrix = matrix;
}

bool OIV2DWellPathGraph::HandleMinMaxNearness(size_t coord, float &min, float &max) const {
  // determine whether min and max are near
  float absMin = std::abs(min);
  float absMax = std::abs(max);
  float absDiff = std::abs(max - min);

  bool areNear = min == max;

  if (!areNear) {
    if (absMin == 0 || absMax == 0 || absDiff < FLT_EPSILON) // values around 0 or too close for precision
      areNear = absDiff < 1;
    else
      areNear = (absDiff / (absMin + absMax)) < 0.1;
  }

  if (areNear) {
    // reasonably crude heuristics for range

    if (min < 0) {
      min *= 1.3f;
      max *= 0.7f;
    } else if (min > 0) {
      min *= 0.7f;
      max *= 1.3f;
    } else // min == 0
    {
      min = -1;
      if (max > 0)
        max *= 1.3f;
      else
        max = 1;
    }

    // return false;
  }

  return false;
}

void OIV2DWellPathGraph::updateDomain(CurveGroup *group) {
  OIV2DRangeInterface::TAxis axis = (group->m_axisPosition == CurveGroup::AxisPosition::AXIS_TOP)
                                        ? OIV2DRangeInterface::AXIS_TOP
                                        : OIV2DRangeInterface::AXIS_BOTTOM;

  if (m_ranges.m_auto[axis]) {
    float rangeMinimum = 0.0f;
    float rangeMaximum = 0.0f;

    if (!m_ranges.m_auto[OIV2DRangeInterface::AXIS_LEFT])
      group->getMinMaxBound(-m_ranges.m_top, -m_ranges.m_bottom, rangeMinimum, rangeMaximum);

    if (m_ranges.m_auto[OIV2DRangeInterface::AXIS_LEFT] || rangeMinimum == FLT_MAX)
      group->getMinMax(rangeMinimum, rangeMaximum);

    // Keep the graph from exploding
    // if min and max are near, we need a decent range, otherwise the tracker will get confused
    HandleMinMaxNearness(0, rangeMinimum, rangeMaximum);

    GetAxisBounds(rangeMinimum, rangeMaximum, m_ranges.m_tick[axis], m_ranges.m_subtick[axis] / 2,
                  m_ranges.m_horizontalMin[axis - 1], m_ranges.m_horizontalMax[axis - 1],
                  m_ranges.m_horizontalStp[axis - 1]);
  } else {
    m_ranges.m_horizontalStp[axis - 1] =
        (m_ranges.m_horizontalMax[axis - 1] - m_ranges.m_horizontalMin[axis - 1]) / m_ranges.m_tick[axis];

    float y = (group->m_axisPosition == CurveGroup::AxisPosition::AXIS_TOP) ? m_ranges.m_top : m_ranges.m_bottom;

    float min = m_ranges.m_horizontalMin[axis - 1];
    min *= (min < 0) ? 1.1 : 0.9;

    float max = m_ranges.m_horizontalMax[axis - 1] + std::abs(m_ranges.m_horizontalMin[axis - 1] - min);

    float domainGlobalMax = min + (max - min) / m_aspect;

    SbVec2f minVec(min, m_ranges.m_bottom);
    SbVec2f maxVec(domainGlobalMax, m_ranges.m_top);
    group->m_domain->setValues(minVec, maxVec);

    group->m_axis->start = SbVec3f(m_ranges.m_horizontalMin[axis - 1], y, 0.0f);
    group->m_axis->end = m_ranges.m_horizontalMax[axis - 1];

    float gridLength = m_ranges.m_top - m_ranges.m_bottom;
    group->m_axis->gridLengthGradOtherSide = gridLength;

    group->m_axis->tickSubDef = PoCartesianAxis::NUM_SUB_TICK;
    group->m_axis->tickNumOrPeriod = m_ranges.m_horizontalStp[axis - 1] / m_ranges.m_subtick[axis];

    group->m_axis->step = m_ranges.m_horizontalStp[axis - 1];

    if (group->numCurves() > 0)
      setRightClip(m_aspect);

    return;
  }

  float domainGlobalMax = m_ranges.m_horizontalMin[axis - 1] +
                          (m_ranges.m_horizontalMax[axis - 1] - m_ranges.m_horizontalMin[axis - 1]) / m_aspect;

  SbVec2f minVec(m_ranges.m_horizontalMin[axis - 1], m_ranges.m_bottom);
  SbVec2f maxVec(domainGlobalMax, m_ranges.m_top);
  group->m_domain->setValues(minVec, maxVec);

  float y = (group->m_axisPosition == CurveGroup::AxisPosition::AXIS_TOP) ? m_ranges.m_top : m_ranges.m_bottom;
  group->m_axis->start = SbVec3f(m_ranges.m_horizontalMin[axis - 1], y, 0.0f);
  group->m_axis->end = m_ranges.m_horizontalMax[axis - 1];

  group->m_axis->step = m_ranges.m_horizontalStp[axis - 1];

  if (group->numCurves() > 0)
    setRightClip(m_aspect);

#if 1
  // to be used with large tick numbers
  group->m_axis->tickNumOrPeriod = 3;
#endif

  float gridLength = m_ranges.m_top - m_ranges.m_bottom;
  group->m_axis->gridLengthGradOtherSide = gridLength;
}

void OIV2DWellPathGraph::setupTopAndBottom() {
  if (m_newWellPaths.empty()) {
    m_ranges.m_topAHD = 0.0f;
    m_ranges.m_bottomAHD = -1000.0f;
    m_ranges.m_topTVD = 0.0f;
    m_ranges.m_bottomTVD = -1000.0f;
    m_ranges.m_topZ = 0.0f;
    m_ranges.m_bottomZ = -1000.0f;
  } else {
    m_ranges.m_bottomAHD = std::numeric_limits<float>::max();
    m_ranges.m_topAHD = -m_ranges.m_bottomAHD;

    m_ranges.m_bottomTVD = std::numeric_limits<float>::max();
    ;
    m_ranges.m_topTVD = -m_ranges.m_bottomTVD;

    m_ranges.m_bottomZ = std::numeric_limits<float>::max();
    ;
    m_ranges.m_topZ = -m_ranges.m_bottomZ;

    for (auto wellPath : m_newWellPaths) {
      const CNewWellPoint &first = *wellPath->GraphPoints().begin();
      const CNewWellPoint &last = *wellPath->GraphPoints().rbegin();

      float topAHD = (float)-CLengthQuantity(first.TMD()).Value(m_unit);
      float bottomAHD = (float)-CLengthQuantity(last.TMD()).Value(m_unit);

      float topTVD = (float)-CLengthQuantity(first.TVD()).Value(m_unit);
      float bottomTVD = (float)-CLengthQuantity(last.TVD()).Value(m_unit);

      float topZ = (float)-CLengthQuantity(first.Z()).Value(m_unit);
      float bottomZ = (float)-CLengthQuantity(last.Z()).Value(m_unit);

      m_ranges.m_topAHD = std::max(m_ranges.m_topAHD, topAHD);
      m_ranges.m_bottomAHD = std::min(m_ranges.m_bottomAHD, bottomAHD);
      m_ranges.m_topTVD = std::max(m_ranges.m_topTVD, topTVD);
      m_ranges.m_bottomTVD = std::min(m_ranges.m_bottomTVD, bottomTVD);
      m_ranges.m_topZ = std::max(m_ranges.m_topZ, topZ);
      m_ranges.m_bottomZ = std::min(m_ranges.m_bottomZ, bottomZ);
    }
  }

  switch (getDepthAxis()) {
  case DEPTH_AXIS_AHD:
    m_ranges.m_top = m_ranges.m_topAHD;
    m_ranges.m_bottom = m_ranges.m_bottomAHD;
    break;
  case DEPTH_AXIS_TVD:
    m_ranges.m_top = m_ranges.m_topTVD;
    m_ranges.m_bottom = m_ranges.m_bottomTVD;
    break;
  case DEPTH_AXIS_Z:
    m_ranges.m_top = m_ranges.m_topZ;
    m_ranges.m_bottom = m_ranges.m_bottomZ;
    break;
  }

  updateVerticalDomain();
  updateVerticalAxes();
  updateDomains();
}

OIV2DWellPathGraph::OIV2DWellPathGraph(CUnitNode::TUnitType unit, float aspect, int preferredGroup)
    : OIV2DGraph(aspect, preferredGroup), m_unit(unit), m_depthAxis(DEPTH_AXIS_AHD), m_formationLinesVisible(true),
      m_markersVisible(true), m_lineWidth(1), m_ranges(*this, 0, -1000) {
  createGraph();
  setupTopAndBottom();
}

OIV2DRangeInterface &OIV2DWellPathGraph::RangeInterface() { return m_ranges; }

void OIV2DWellPathGraph::addWellPath(const CNewWellPath *wellPath) {
  if (std::find(m_newWellPaths.begin(), m_newWellPaths.end(), wellPath) == m_newWellPaths.end()) {
    m_newWellPaths.push_back(wellPath);
    setupTopAndBottom();

    bool is_first_vc = true;
    for (auto vc : m_valueComponents) {
      int colorIndex = colorManager().allocateColorIndex();
      Curve *newCurve = new NewWellPathCurve(wellPath, vc, m_unit, colorIndex);
      if (is_first_vc) {
        COpenGLNode::TColor color_ = wellPath->Color();
        newCurve->setColor(color_, color_); // mcr 2018-12-02: don't do in 'NewWellPathCurve' as all the classes chain
                                            // expects 'int' instead of 'unsigned in'
        is_first_vc = false;
      }
      setupCurve(newCurve);
    }

    if (m_newWellPaths.size() == 1) {
      setupFormationSections(*wellPath);
    }

    UpdateDepthAxes();
  }
}

void OIV2DWellPathGraph::removeWellPath(const CNewWellPath *wellPath) {
  auto iter = std::find(m_newWellPaths.begin(), m_newWellPaths.end(), wellPath);
  if (iter != m_newWellPaths.end()) {
    m_newWellPaths.erase(iter);

    removeCurvesByPredicate([wellPath](const Curve *curve) -> bool {
      const NewWellPathCurve *nwpCurve = dynamic_cast<const NewWellPathCurve *>(curve);
      return nwpCurve != 0 && nwpCurve->getWellPath() == wellPath;
    });

    // If there's only 1 wellpath left, then setup the formation sections for that one
    int wellPathCount = m_newWellPaths.size();
    if (wellPathCount == 1) {
      setupFormationSections(*m_newWellPaths[0]);
    }

    UpdateDepthAxes();

    setupTopAndBottom();
  }
}

bool OIV2DWellPathGraph::canAddValueComponent(const IValueComponentBase *component) const {
  return selectCurveGroup(m_preferredCurveGroup, component) != 0;
}

bool OIV2DWellPathGraph::addValueComponent(const IValueComponentBase *component) {
  assert(std::find(m_valueComponents.begin(), m_valueComponents.end(), component) == m_valueComponents.end());
  m_valueComponents.push_back(component);

  int wellPathCount = m_newWellPaths.size();

  bool ok = true;

  // Create curves
  for (auto wellPath : m_newWellPaths) {
    int colorIndex = colorManager().allocateColorIndex();
    Curve *newCurve = new NewWellPathCurve(wellPath, component, m_unit, colorIndex);
    if (m_valueComponents.size() == 1) {
      COpenGLNode::TColor color_ = wellPath->Color();
      newCurve->setColor(color_, color_);
    }
    if (!setupCurve(newCurve))
      ok = false;
    else {
      assert(wellPathCount <= 1 || getDepthAxis() == DEPTH_AXIS_Z);
      static_cast<WellPathCurveBase *>(newCurve)->setDepthAxis(getDepthAxis());
    }
  }
  return ok;
}

void OIV2DWellPathGraph::updateCurves(const IValueComponentBase *component) {
  for (auto g : m_curveGroups) {
    CurveGroup *group = static_cast<CurveGroup *>(g);
    for (auto c : group->m_curves) {
      Curve *curve = static_cast<Curve *>(c);
      if (curve->valueComponent() == component) {
        curve->refresh();
        updateDomain(group);
      }
    }
  }
}

void OIV2DWellPathGraph::removeCurves(const IValueComponentBase *component) {
  m_valueComponents.erase(std::remove(m_valueComponents.begin(), m_valueComponents.end(), component),
                          m_valueComponents.end());

  removeCurvesByPredicate([component](const Curve *curve) -> bool { return curve->valueComponent() == component; });

  if (m_valueComponents.size() == 1 && m_newWellPaths.size() == 1) {
    NewWellPathCurve *curve = (NewWellPathCurve *)getCurve(m_newWellPaths[0], m_valueComponents[0]);
    COpenGLNode::TColor color_ = m_newWellPaths[0]->Color();
    curve->setColor(color_, color_);
    updateCurves(m_valueComponents[0]);
  }
}

void OIV2DWellPathGraph::removeAllCurves() {
  m_valueComponents.clear();

  for (auto g : m_curveGroups) {
    CurveGroup *group = static_cast<CurveGroup *>(g);
    group->m_curves->removeAllChildren();

    updateDomain(group);
    updateTitle(group);
  }
}

const NewWellPathCurve *OIV2DWellPathGraph::getCurve(const CNewWellPath *wellPath,
                                                     const IValueComponentBase *component) const {
  for (auto g : m_curveGroups) {
    CurveGroup *group = static_cast<CurveGroup *>(g);
    for (auto c : group->m_curves) {
      NewWellPathCurve *curve = dynamic_cast<NewWellPathCurve *>(c);
      if (curve != 0 && curve->valueComponent() == component && curve->getWellPath() == wellPath)
        return curve;
    }
  }

  return 0;
}

void OIV2DWellPathGraph::collectComponents(int groupindex, std::vector<const IValueComponentBase *> &components) {
  CurveGroup *curveGroup = static_cast<CurveGroup *>(m_curveGroups->getChild(groupindex));
  if (!curveGroup)
    return;
  for (auto node : curveGroup->m_curves) {
    Curve *curve = static_cast<Curve *>(node);
    if (std::find(components.begin(), components.end(), curve->valueComponent()) == components.end()) {
      components.push_back(curve->valueComponent());
    }
  }
}

void OIV2DWellPathGraph::updateLabel(Curve *curve, PoDomain *domain, float x, float y) {
  m_label->setup(domain, m_aspect);
  m_label->setPosition(x, y);
  setNormalLabelColor();

  auto min = domain->min.getValue();
  auto max = domain->max.getValue();

  float xRange = max[0] - min[0];
  float yRange = max[1] - min[1];
  float xUsed = x, yUsed = y;
  float tolerance = .01f;

  int numCurvePoints = curve->getNumPoints();
  for (auto i = 0; i < numCurvePoints; ++i) {
    const SbVec2f &point = curve->getPoint(i);

    float errorx = std::abs(point[0] - x) / xRange;
    float errory = std::abs(point[1] - y) / yRange;
    if (errorx < tolerance && errory < tolerance) {
      setHighlightLabelColor();
      xUsed = point[0];
      yUsed = point[1];
      tolerance = std::max(errorx, errory);
    }
  }

  QString wpName = static_cast<WellPathCurveBase *>(curve)->getWellPathName();
  QString name = curve->getLabel();
  QString xUnit = curve->getUnitName();
  const char *yUnit = m_unit == (CUnitNode::TUnitType::SI_UNIT) ? "m" : "ft";
  const char *depthAxisName = getDepthAxisName(getDepthAxis());

  char buffer[256];

#if 0
  sprintf(buffer, "%s: %s %.4g %s, %s %.0f %s", 
    wpName.toStdString().c_str(),
    name.toStdString().c_str(),
    xUsed, 
    xUnit.toStdString().c_str(),
    depthAxisName,
    -yUsed,
    yUnit);
#endif

  sprintf(buffer, "%s (%.4g, %.0f)", name.toStdString().c_str(), xUsed, -yUsed);

  m_label->addText(buffer);
}

void OIV2DWellPathGraph::showFormationLines(bool visible) {
  m_formationLinesVisible = visible;
  m_formationLinesSwitch->whichChild = visible ? SO_SWITCH_ALL : SO_SWITCH_NONE;
}

void OIV2DWellPathGraph::showMarkers(bool visible) {
  m_markersVisible = visible;

  for (auto g : m_curveGroups) {
    CurveGroup *group = static_cast<CurveGroup *>(g);
    for (auto c : group->m_curves) {
      Curve *curve = static_cast<Curve *>(c);
      curve->markerFilterType = visible ? PoCurve::ALL_POINTS : PoCurve::INACTIVE;
    }
  }
}

void OIV2DWellPathGraph::setUnit(CUnitNode::TUnitType unit) {
  OIV2DGraph::setUnit(unit);

  if (unit != m_unit) {
    m_unit = unit;

    setupTopAndBottom();

    // Only setup formation sections if we have exactly one wellpath
    if (m_newWellPaths.size() == 1)
      setupFormationSections(*m_newWellPaths[0]);
  }
}

void OIV2DWellPathGraph::toggleDepthAxis() {
  setDepthAxis((m_depthAxis == OIV2DWellPathGraph::DEPTH_AXIS_AHD) ? OIV2DWellPathGraph::DEPTH_AXIS_TVD
                                                                   : OIV2DWellPathGraph::DEPTH_AXIS_AHD);
}

void OIV2DWellPathGraph::setDepthAxis(DepthAxis axis) {
  assert(axis != DEPTH_AXIS_Z);
  m_depthAxis = axis;

  UpdateDepthAxes();
}

OIV2DWellPathGraph::DepthAxis OIV2DWellPathGraph::getDepthAxis() const {
  int wellPathCount = m_newWellPaths.size();

  if (wellPathCount == 0)
    return DEPTH_AXIS_TVD;
  else if (wellPathCount > 1)
    return DEPTH_AXIS_Z;
  else
    return m_depthAxis;
}

void OIV2DWellPathGraph::setLineThickness(int t) {
  m_lineWidth = t;

  for (auto g : m_curveGroups) {
    CurveGroup *group = static_cast<CurveGroup *>(g);
    for (auto c : group->m_curves) {
      Curve *curve = static_cast<Curve *>(c);
      curve->setLineWidth(t);
    }
  }
}

void OIV2DWellPathGraph::setForegroundColor(float foregroundColor[3]) {
  char diffuseColor[50];
  sprintf(diffuseColor, "diffuseColor [%.2f %.2f %.2f]", foregroundColor[0], foregroundColor[1], foregroundColor[2]);

  for (auto g : m_curveGroups) {
    CurveGroup *group = static_cast<CurveGroup *>(g);
    group->m_axis->set(mainMaterial, diffuseColor);
    group->m_axis->set(gridMaterial, diffuseColor);
  }

  m_yAxisLeft->set(mainMaterial, diffuseColor);
  m_yAxisLeft->set(gridMaterial, diffuseColor);

  m_yAxisRight->set(mainMaterial, diffuseColor);
  m_yAxisRight->set(gridMaterial, diffuseColor);

  OIV2DGraph::setForegroundColor(foregroundColor);
}

int OIV2DWellPathGraph::getNumCurves() const {
  int numCurves = 0;
  for (auto g : m_curveGroups) {
    CurveGroup *group = static_cast<CurveGroup *>(g);
    numCurves += group->numCurves();
  }
  return numCurves;
}

int OIV2DWellPathGraph::getNumCurves(int group) const {
  CurveGroup *curveGroup = static_cast<CurveGroup *>(m_curveGroups->getChild(group));
  if (!curveGroup)
    return 0;

  return curveGroup->numCurves();
}

void OIV2DWellPathGraph::UpdateDepthAxes() {
  for (int i = 0; i < m_curveGroups->getNumChildren(); ++i) {
    CurveGroup *group = static_cast<CurveGroup *>(m_curveGroups->getChild(i));
    for (int j = 0; j < group->m_curves->getNumChildren(); ++j)
      static_cast<WellPathCurveBase *>(group->m_curves->getChild(j))->setDepthAxis(getDepthAxis());
  }

  setupTopAndBottom();

  bool formationsVisible = (m_newWellPaths.size() == 1);
  bool linesVisible = formationsVisible && m_formationLinesVisible;

  m_formationSectionsSwitch->whichChild = formationsVisible ? SO_SWITCH_ALL : SO_SWITCH_NONE;
  m_formationLinesSwitch->whichChild = linesVisible ? SO_SWITCH_ALL : SO_SWITCH_NONE;
}

OIV2DWellPathGraph::RangeInterface::RangeInterface(OIV2DWellPathGraph &graph, float top, float bottom)
    : m_graph(graph), m_top(top), m_bottom(bottom) {
  for (size_t i = AXIS_LEFT; i < _AXIS_TOTAL; ++i) {
    m_auto[i] = true;
    m_tick[i] = 5;
    m_subtick[i] = 10;
  }
}

OIV2DWellPathGraph::RangeInterface::~RangeInterface() {}

void OIV2DWellPathGraph::RangeInterface::Enter() {
  m_savedTop = m_top;
  m_savedBottom = m_bottom;
}

void OIV2DWellPathGraph::RangeInterface::Leave(bool revert) {
  if (revert) {
    SetMinMax(AXIS_LEFT, -m_savedTop, -m_savedBottom);
    UpdateGraph();
  }
}

bool OIV2DWellPathGraph::RangeInterface::HaveAxis(TAxis axis) const {
  return axis == AXIS_LEFT || m_graph.getCurveGroup(axis);
}

void OIV2DWellPathGraph::RangeInterface::GetRangeMinMax(TAxis axis, float &min, float &max) const {
  switch (axis) {
  case AXIS_LEFT:
    switch (m_graph.getDepthAxis()) {
    case DEPTH_AXIS_TVD:
      /* Fall-through */
    case DEPTH_AXIS_AHD:
      min = -m_topTVD;
      max = -m_bottomTVD;
      break;
    case DEPTH_AXIS_Z:
      min = -m_topZ;
      max = -m_bottomZ;
      break;
    }
    break;
  case AXIS_TOP:
  /* Fall-through */
  case AXIS_BOTTOM:
    min = m_horizontalMin[axis - 1];
    max = m_horizontalMax[axis - 1];
  }
}

bool OIV2DWellPathGraph::RangeInterface::RangeLargeEnough(TAxis axis, float min, float max) const {
  return !m_graph.HandleMinMaxNearness(0, min, max);
}

void OIV2DWellPathGraph::RangeInterface::SetMinMax(TAxis axis, float min, float max) {
  switch (axis) {
  case AXIS_LEFT:
    switch (m_graph.getDepthAxis()) {
    case DEPTH_AXIS_TVD:
      /* Fall-through */
    case DEPTH_AXIS_AHD:
      m_top = m_topTVD = -min;
      m_bottom = m_bottomTVD = -max;
      break;
    case DEPTH_AXIS_Z:
      m_top = m_topZ = -min;
      m_bottom = m_bottomZ = -max;
      break;
    }
    break;
  case AXIS_TOP:
  /* Fall-through */
  case AXIS_BOTTOM:
    m_horizontalMin[axis - 1] = min;
    m_horizontalMax[axis - 1] = max;
    break;
  }
}

void OIV2DWellPathGraph::RangeInterface::ResetMinMax(TAxis axis) {
  switch (axis) {
  case AXIS_LEFT:
    m_graph.setupTopAndBottom();
    break;
  case AXIS_TOP:
  /* Fall-through */
  case AXIS_BOTTOM:
    if (CurveGroup *group = m_graph.getCurveGroup(axis)) {
      float min, max;

      if (!m_auto[AXIS_LEFT])
        group->getMinMaxBound(-m_top, -m_bottom, min, max);
      else
        group->getMinMax(min, max);

      m_graph.HandleMinMaxNearness(0, min, max);

      GetAxisBounds(min, max, m_tick[axis], m_subtick[axis], m_horizontalMin[axis - 1], m_horizontalMax[axis - 1],
                    m_horizontalStp[axis - 1]);
    }
    break;
  }
}

int OIV2DWellPathGraph::RangeInterface::Tick(TAxis axis) const { return m_tick[axis]; }

void OIV2DWellPathGraph::RangeInterface::Tick(TAxis axis, int value) { m_tick[axis] = value; }

int OIV2DWellPathGraph::RangeInterface::SubTick(TAxis axis) const { return m_subtick[axis]; }

void OIV2DWellPathGraph::RangeInterface::SubTick(TAxis axis, int value) { m_subtick[axis] = value; }

bool OIV2DWellPathGraph::RangeInterface::Auto(TAxis axis) const { return m_auto[axis]; }

void OIV2DWellPathGraph::RangeInterface::Auto(TAxis axis, bool value) { m_auto[axis] = value; }

void OIV2DWellPathGraph::RangeInterface::UpdateGraph() { m_graph.refreshVisualization(); }
