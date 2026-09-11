// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER

#include "CSeawater.h"
#include "DSealectGlobal.h"
#include "DSealectDoc.h"
#include "CDataModel.h"
#include "CFormationTopography.h"

//##ModelId=3C0DDDAA01E5
CSeawater::CSeawater()
:
m_qWaterDepth	(1.0, 0.0, DOUBLE_MAX_, false, false),
m_qWaterdensity	(1.0, 0.0, DOUBLE_MAX_, false, false)
{
	m_qWaterDepth.Invalidate();
	m_qWaterdensity.Invalidate();
}

//From: Formation Definition
//Label: Sea bed TVD
//Descr: True vertical depth of sea bed measured from 
//derrick floor.
//##ModelId=3C0DDDAA01E6
CLengthQuantity *CSeawater::WaterDepth()
{
	return &m_qWaterDepth;
}

const CLengthQuantity *CSeawater::WaterDepth() const
{
	return &m_qWaterDepth;
}

CLengthQuantity CSeawater::SeaBedTVD() const
{
  if(m_qWaterDepth.Undefined())
    return CLengthQuantity();

  return CLengthQuantity(m_qWaterDepth.Value() + GetDoc()->DSealectModel()->FormationTopography()->ElevationTVD()->Value());
}


//From: 
//Label: 
//Descr:
//##ModelId=3C0DDDAA01E7
CDensityQuantity *CSeawater::Waterdensity()
{
	return &m_qWaterdensity;
}

//##ModelId=3CA0118700AB
bool CSeawater::CheckComplete() const
{
	if(m_qWaterDepth.Undefined())
		return FALSE;
		
	return true;
}
