#include "stdafx.h"

#include "OIV2DGraph.h"
#include "MeshViz\graph\PoAxis.h"
#include "MeshViz\nodes\PoDomain.h"
#include "OIV2DWellPathGraph.h"
#include "SoGroupIterator.h"
#include "MeshViz\graph\PoLinearAxis.h"
#include "InfoLabel.h"
#include "Curve.h"
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoClipPlane.h>


#include <algorithm>

const char* const OIV2DGraph::fontName = "arial.ttf";
const float OIV2DGraph::titleFontSize = .02f;
const float OIV2DGraph::titleDistAxisHorizontal = .07f; // distance to horizontal axis
const float OIV2DGraph::titleDistAxisVertical = .02f; // distance to vertical axis
const float OIV2DGraph::titleDistAxisVerticalLarge = .11f; // distance to vertical axis
const float OIV2DGraph::gradFontSize = .02f;
const float OIV2DGraph::arrowHeight = .0f;
const float OIV2DGraph::arrowLength = .0f;

const char* const OIV2DGraph::mainMaterial    = "appearance.material";
const char* const OIV2DGraph::gridDrawStyle   = "mainGradGridApp.drawStyle";
const char* const OIV2DGraph::subGridDrawStyle   = "subGradGridApp.drawStyle";
const char* const OIV2DGraph::gridMaterial    = "mainGradGridApp.material";
const char* const OIV2DGraph::gridLineStyle   = "style LINES";
const char* const OIV2DGraph::gridLineWidth   = "lineWidth 0.2";
const char* const OIV2DGraph::gridLinePattern = "linePattern 0xf0f0";
const char* const OIV2DGraph::subGridLinePattern = "linePattern 0x0000";

OIV2DGraph::OIV2DGraph( float aspect, int preferredGroup)
  : m_aspect(aspect)
  , m_preferredCurveGroup(preferredGroup)
  , m_curveGroups(0)
  , m_label (0)
  , m_clipPlaneRight(0)
{
  m_labelTextColor[0] = m_labelTextColor[1] = m_labelTextColor[2] = 1.0f;
}

void OIV2DGraph::SetDefaultAxisParams(PoAxis* axis)
{
  axis->gradFontName = fontName;
  axis->gradFontSize = gradFontSize;

  axis->titleFontName = fontName;
  axis->titleFontSize = titleFontSize;
  axis->titleVisibility = PoAxis::VISIBILITY_ON;

  axis->arrowVisibility = PoAxis::VISIBILITY_OFF;
  axis->arrowHeight = arrowHeight;
  axis->arrowLength = arrowLength;

  axis->set(gridDrawStyle, gridLineStyle);
  axis->set(gridDrawStyle, gridLineWidth);
  axis->set(gridDrawStyle, gridLinePattern);

  axis->tickSubLength = 0.0000001f; // nigh invisible
  axis->set(subGridDrawStyle, subGridLinePattern); // invisible

  axis->set(mainMaterial, "diffuseColor [1 1 1]");
  axis->set(mainMaterial, "transparency .2");
  axis->set(gridMaterial, "diffuseColor [1 1 1]");
  axis->set(gridMaterial, "transparency .6");
}

PoDomain* OIV2DGraph::createDomain(float horMin, float horMax, float vertMin, float vertMax)
{
  SbVec2f minVec(horMin, vertMin);
  SbVec2f maxVec(horMax, vertMax);

  PoDomain* domain = new PoDomain;
  domain->transformType = PoDomain::TRANSFORM_01;
  domain->setValues(minVec, maxVec);

  return domain;
}

void OIV2DGraph::finishCreatingCurveGroup(CurveGroup *group)
{
  SoPickStyle* pickable = new SoPickStyle;
  pickable->style = SoPickStyle::SHAPE;

  group->m_curves = new SoGroup;

  group->addChild(group->m_domain);
  group->addChild(group->m_axis);

  group->addChild(pickable);

  SoSeparator *separator = new SoSeparator;

  m_clipping = new SoSwitch;

  SoClipPlane *clipPlaneTop = new SoClipPlane;
  clipPlaneTop->on = true;
  clipPlaneTop->plane.setValue(SbPlane(SbVec3f(0, -1, 0), -1));

  SoClipPlane *clipPlaneBottom = new SoClipPlane;
  clipPlaneBottom->on = true;
  clipPlaneBottom->plane.setValue(SbPlane(SbVec3f(0, 1, 0), 0));

  SoClipPlane *clipPlaneLeft = new SoClipPlane;
  clipPlaneLeft->on = true;
  clipPlaneLeft->plane.setValue(SbPlane(SbVec3f(1, 0, 0), 0));

  m_planeRight = SbPlane(SbVec3f(-1, 0, 0), -1 * m_aspect);
  m_clipPlaneRight = new SoClipPlane;
  m_clipPlaneRight->on = true;
  m_clipPlaneRight->plane.connectFrom(&m_planeRight);

  m_clipping->addChild(clipPlaneTop);
  m_clipping->addChild(clipPlaneBottom);
  m_clipping->addChild(clipPlaneLeft);
  m_clipping->addChild(m_clipPlaneRight);
  m_clipping->whichChild = SO_SWITCH_ALL;

  separator->addChild(m_clipping);
  separator->addChild(group->m_curves);

  group->addChild(separator);
}

void OIV2DGraph::GetAxisBounds(float minimum, float maximum, int ticks, int subticks, float & lowerBound, float & upperBound, float & tickSize)
{
  if (minimum == maximum) // just in case, otherwise the while-loop below will be endless
  {
  minimum -= 1;
  maximum += 1;
  }
  double range = maximum - minimum;
  double unroundedTickSize = range / (ticks * subticks - 1);
  double x = std::ceil(std::log10(unroundedTickSize) -1);
  double pow10x = std::pow(10, x);
  double roundedTickSize = std::ceil(unroundedTickSize / pow10x) * pow10x;
  double dbl_lowerBound = roundedTickSize * std::floor(double (minimum)/roundedTickSize);
  // upperBound = roundedTickSize * std::ceil(1 + double (maximum)/roundedTickSize);
  double dbl_upperBound = dbl_lowerBound + ticks * subticks * roundedTickSize; // one tickSize more to make sure

  // center the curves
  while (true)
  {
    double newUpperBound = dbl_upperBound - roundedTickSize;
    double newLowerBound = dbl_lowerBound - roundedTickSize;

    if (newUpperBound - maximum < minimum - newLowerBound)
      break;
    else
    {
      dbl_upperBound = newUpperBound;
      dbl_lowerBound = newLowerBound;
    }
  }

  while (true)
  {
    double newUpperBound = dbl_upperBound + roundedTickSize;
    double newLowerBound = dbl_lowerBound + roundedTickSize;

    if (newUpperBound - maximum > minimum - newLowerBound)
      break;
    else
    {
      dbl_upperBound = newUpperBound;
      dbl_lowerBound = newLowerBound;
    }
  }

  lowerBound = float (dbl_lowerBound);
  lowerBound -= 1e-7 * std::abs(lowerBound); // to make sure lowerBound < dbl_lowerBound, to get the right ticks in meshviz
  tickSize = float (roundedTickSize);

  upperBound = lowerBound + ticks * subticks * tickSize;
}


void OIV2DGraph::setAspect( float aspect )
{
  m_aspect = aspect;
  setRightClip(aspect);
}

void OIV2DGraph::setRightClip(float distance)
{
  if (m_clipPlaneRight)
  {
  m_planeRight = SbPlane(SbVec3f(-1, 0, 0), -distance);
  }
}

void OIV2DGraph::clearLabel()
{
  if (m_label) m_label->clearText();
}

void OIV2DGraph::showLabel(bool visible)
{
  if (m_label) m_label->show(visible);
}

void OIV2DGraph::setNormalLabelColor()
{
  if (m_label) m_label->setLabelColor(m_labelTextColor[0], m_labelTextColor[1], m_labelTextColor[2]);
}

void OIV2DGraph::setHighlightLabelColor()
{
  if (m_label) m_label->setLabelColor(m_labelTextColor[0], m_labelTextColor[1], 1.0f - m_labelTextColor[2]);
}

void OIV2DGraph::setForegroundColor( float color[3] )
{
  memcpy(m_labelTextColor, color, 3 * sizeof(float));
  m_label->setCrossHairColor(color);
}

void OIV2DGraph::updateTitle(const CurveGroup* group)
{
  group->updateTitle();
}
  
ColorManager & OIV2DGraph::colorManager()
{
  return m_colorManager;
}

void OIV2DGraph::removeCurvesByPredicate(std::function<bool(const Curve *)> predicate )
{
  for (auto g : m_curveGroups)
  {
    CurveGroup* group = static_cast<CurveGroup*>(g);

    // This code should move to CurveGroup
    bool removed = false;
    auto iter = begin(group->m_curves);
    auto endCurves = end(group->m_curves);
    // std::find_if expects a SoNode * -> bool predicate, which the predicate supplied essentially is as a Curve is a SoNode and group only contains Curves
    while ((iter = std::find_if(iter, endCurves, * reinterpret_cast<std::function<bool(const SoNode *)> *> (& predicate))) != endCurves)
    {
      Curve* curve =static_cast<Curve*>(*iter);          
      colorManager().releaseColorIndex(curve->getColorIndex());
      iter = erase(iter);
      removed = true;
    }

    if(removed)
    {
      if(group->numCurves() == 0)
        group->whichChild = SO_SWITCH_NONE;               
    }
  }
}

void OIV2DGraph::setUnit( CUnitNode::TUnitType unit )
{
  for (auto curveGroupNode : m_curveGroups)
  {
    for (auto curveNode : static_cast<CurveGroup *>(curveGroupNode)->m_curves)
      static_cast<Curve*>(curveNode)->setUnit(unit);
  }
}

void OIV2DGraph::updateDomains()
{
  bool clearGrid = false;

  for (auto curveGroupNode : m_curveGroups)
  {
    auto * curveGroup = static_cast<CurveGroup *> (curveGroupNode);
    updateTitle(curveGroup);
    updateDomain(curveGroup);
    

    if (curveGroup->numCurves() != 0)
    {
      if (clearGrid)
      {
        curveGroup->m_axis->gridLengthGradOtherSide = 0;
      }

      clearGrid = true;
    }
  }
}

CurveGroup * OIV2DGraph::selectCurveGroup( int preferredCurveGroupIndex, const IValueComponentBase* valueComponent ) const
{
  CurveGroup * selectedCurveGroup = 0;
  CurveGroup * preferredCurveGroup = static_cast<CurveGroup *>(m_curveGroups->getChild (preferredCurveGroupIndex));
  if (preferredCurveGroup->numCurves() == 0 || preferredCurveGroup->getCurve(0)->valueComponent()->UnitName() == valueComponent->UnitName())
  {
    selectedCurveGroup = preferredCurveGroup;
  }
  else
  {
    for (auto curveGroupNode : m_curveGroups)
    {
      auto * curveGroup = static_cast<CurveGroup *> (curveGroupNode);
      if (curveGroup->numCurves() == 0 ||
        curveGroup->getCurve(0)->valueComponent()->UnitName() == valueComponent->UnitName())
      {
        selectedCurveGroup = curveGroup;
        break;
      }  
    }
  }
  return selectedCurveGroup;
}

CurveGroup * OIV2DGraph::getCurveGroup(OIV2DRangeInterface::TAxis axis) const
{
  CurveGroup::AxisPosition curveAxis = axis == OIV2DRangeInterface::AXIS_TOP ? CurveGroup::AxisPosition::AXIS_TOP : CurveGroup::AxisPosition::AXIS_BOTTOM;

  for (auto curveGroupNode : m_curveGroups)
  {
  auto * curveGroup = static_cast<CurveGroup *> (curveGroupNode);
  if (curveGroup->m_axisPosition == curveAxis)
      return curveGroup->numCurves() > 0 ? curveGroup : 0;
  }
  return 0;
}

void OIV2DGraph::setPreferredGroup( int preferredGroup )
{
  m_preferredCurveGroup = preferredGroup;
}


