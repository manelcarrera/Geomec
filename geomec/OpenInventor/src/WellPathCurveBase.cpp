#include "stdafx.h"

#include "WellPathCurveBase.h"

WellPathCurveBase::WellPathCurveBase(const IValueComponentBase *valueComponent, CUnitNode::TUnitType unit,
                                     int colorIndex)
    : Curve(valueComponent, unit, colorIndex), m_depthAxis(OIV2DWellPathGraph::DEPTH_AXIS_AHD) {}

OIV2DWellPathGraph::DepthAxis WellPathCurveBase::getDepthAxis() const { return m_depthAxis; }

void WellPathCurveBase::setDepthAxis(OIV2DWellPathGraph::DepthAxis axis) {
  if (m_depthAxis != axis) {
    m_depthAxis = axis;
    refresh();
  }
}
