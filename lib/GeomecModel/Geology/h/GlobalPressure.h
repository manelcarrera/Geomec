// GlobalPressure.h: interface for the CGlobalPressure class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLOBALPRESSURE_H__9569C199_6781_43AF_A867_A4F0DC804D76__INCLUDED_)
#define AFX_GLOBALPRESSURE_H__9569C199_6781_43AF_A867_A4F0DC804D76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PressureBase.h"
#include "GlobalFormationLoad.h"

class TPressure;

#include "ValueTypeFactory.h"

class CGlobalPressure : public CGlobalFormationLoad<TPressure, CComponentConstant <TPressure> >
{
public:
	CGlobalPressure(CFemAppModel& model,
					const double &dReferencePressure = 0,
					const double &dReferenceDepth = 0,
					const double &dPressureGradient = 0);
	CGlobalPressure(const CGlobalPressure& rhs);

	virtual unsigned int IconId() const;

  virtual void OnNewNeighbour(const CGraphNode& node);
  virtual void OnNeighbourDeleted(const CGraphNode& node);

  ACCEPT_GEOMECMODELVISITORS(VisitGlobalPressure);
};

#endif // !defined(AFX_GLOBALPRESSURE_H__9569C199_6781_43AF_A867_A4F0DC804D76__INCLUDED_)
