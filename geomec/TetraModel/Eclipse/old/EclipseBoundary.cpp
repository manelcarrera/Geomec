// EclipseBoundary.cpp: implementation of the CEclipseBoundary class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER
#include "geomec.h"
#include "EclipseBoundary.h"
#include "EclipseFormation.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEclipseBoundary::CEclipseBoundary(const geo::IPoint& ptMin,
                   const geo::IPoint& ptMax,
                   CGraphModel& model,
                   BOUNDARY_STATE state)
: CBoundary(ptMin, ptMax, model, state)
{

}


CEclipseBoundary::TMinMax CEclipseBoundary::BestFit() const
{
  TMinMax ret;

  // Calculate formation of the eclipse  ....
  for(CGraphNode::const_iterator it = CGraphNode::begin(); it != CGraphNode::end(); it++)
  {
    const CEclipseFormation* pFormation = dynamic_cast<const CEclipseFormation*> (*it);
    if(pFormation)
    {
        ret.first = ret.first.Min(pFormation->DisplayList().Min());
        ret.second = ret.second.Max(pFormation->DisplayList().Max());
    }
  }

  return ret;
}

void CEclipseBoundary::OnNewNeighbour(const CGraphNode &node)
{
  // Update boundaries ...
  const CEclipseFormation* pFormation = dynamic_cast<const CEclipseFormation*> (&node);

  CBoundary::OnNewNeighbour(node);

  if(pFormation && (State() == USER_DEFINED))
    return;		// Does not effect state

  if(pFormation && (State() == DEFAULT_DEFINED))
  {
    State(BEST_FIT);
    return;
  }

  if(pFormation && (State() == BEST_FIT))
  {
    TMinMax bestfit = BestFit();
    ASSERT(!bestfit.first.Empty() && !bestfit.second.Empty());
    Set(bestfit.first, bestfit.second);
  }
}
  
 
void CEclipseBoundary::OnNeighbourModified(const CGraphNode &node)
{
  const CEclipseFormation* pFormation = dynamic_cast<const CEclipseFormation*> (&node);
  
  if(pFormation && (State() == USER_DEFINED))
    return;		// Does not effect state

  if(pFormation && (State() == DEFAULT_DEFINED))
  {
    State(BEST_FIT);
    return;
  }

  if(pFormation && (State() == BEST_FIT))
  {
    TMinMax bestfit = BestFit();
    ASSERT(!bestfit.first.Empty() && !bestfit.second.Empty());
    Set(bestfit.first, bestfit.second);
  }

  CBoundary::OnNeighbourModified(node);
}