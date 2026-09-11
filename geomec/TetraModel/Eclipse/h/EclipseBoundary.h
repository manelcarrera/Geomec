// EclipseBoundary.h: interface for the CEclipseBoundary class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ECLIPSEBOUNDARY_H__CEC50E28_AB51_4A89_8961_3DB5A06F10F3__INCLUDED_)
#define AFX_ECLIPSEBOUNDARY_H__CEC50E28_AB51_4A89_8961_3DB5A06F10F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Boundary.h"

class CEclipseBoundary : public CBoundary
{
public:
  CEclipseBoundary(const geo::IPoint& ptMin,
           const geo::IPoint& ptMax,
           CGraphModel& model,
           BOUNDARY_STATE state = DEFAULT_DEFINED);

  virtual TMinMax BestFit() const;

  // Events
  virtual void OnNewNeighbour(const CGraphNode &node);
  virtual void OnNeighbourModified(const CGraphNode &node);
};

#endif // !defined(AFX_ECLIPSEBOUNDARY_H__CEC50E28_AB51_4A89_8961_3DB5A06F10F3__INCLUDED_)
