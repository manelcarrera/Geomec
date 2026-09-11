// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER

#include "CDrillingDirectionSection.h"

//##ModelId=3C0DDDAE009D
CDrillingDirectionSection::CDrillingDirectionSection()
:
m_qWellInclination			(1.0, 0.0, 90, true, true),
m_qAzimuthWithNorth			(0.0, 0.0, 360, true, false),
m_qDrillingSectionTopTMD	(1.0, 0.0, DOUBLE_MAX_, false, false),
m_qDrillingSectionBottomTMD	(1.0, 0.0, DOUBLE_MAX_, false, false)
{
	m_qWellInclination.Invalidate();
	m_qAzimuthWithNorth.Invalidate();
  m_qDrillingSectionTopTMD.Invalidate();
	m_qDrillingSectionBottomTMD.Invalidate();
}

//From: Drilling Operation
//label: Well Inclination
//Descr: Well Inclination
//##ModelId=3C0DDDAE009E
CAngleQuantity *CDrillingDirectionSection::WellInclination()
{
	return &m_qWellInclination;
}

//From: Drilling Operation
//label: Azimuth with North
//descr:Azimuth with North
//##ModelId=3C0DDDAE009F
CAngleQuantity *CDrillingDirectionSection::AzimuthWithNorth() 
{
	return &m_qAzimuthWithNorth;
}

//Top and bottom of the DrillingSection.
//##ModelId=3C0DDDAE00A0
CLengthQuantity* CDrillingDirectionSection::DrillingSectionTopTMD()
{
  return &m_qDrillingSectionTopTMD;
}

CLengthQuantity *CDrillingDirectionSection::DrillingSectionBottomTMD()
{
	return &m_qDrillingSectionBottomTMD;
}

//##ModelId=3CA0118C03D8
bool CDrillingDirectionSection::CheckComplete() const
{
	if(m_qAzimuthWithNorth.Undefined())
		return false;

	if(m_qWellInclination.Undefined())
		return false;

	return true;
}
