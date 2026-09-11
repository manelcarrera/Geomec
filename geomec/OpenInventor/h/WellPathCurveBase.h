#ifndef _WellPathCurveBase_h_
#define _WellPathCurveBase_h_

#include "Curve.h"
#include "OIV2DWellPathGraph.h"

class WellPathCurveBase : public Curve
{
    OIV2DWellPathGraph::DepthAxis m_depthAxis;

public:

    WellPathCurveBase(const IValueComponentBase* valueComponent, CUnitNode::TUnitType unit, int colorIndex);

    OIV2DWellPathGraph::DepthAxis getDepthAxis() const;

    void setDepthAxis(OIV2DWellPathGraph::DepthAxis axis);

    virtual QString getWellPathName() const = 0;
};

#endif  // _WellPathCurveBase_h_
