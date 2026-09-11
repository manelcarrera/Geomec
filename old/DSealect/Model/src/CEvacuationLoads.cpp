// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER

#include "CEvacuationLoads.h"

//##ModelId=3C0DDDAD0233
CEvacuationLoads::CEvacuationLoads()
:
m_qBottomHoleTreatingPressure			(1.0, 0.0, DOUBLE_MAX_, false, false),
m_qTreatingFluidTempEvac				(1.0, DOUBLE_MIN_, DOUBLE_MAX_, false, false),
m_qTimeOfExposureToTreatingFluidEvac	(1.0, 0.0, DOUBLE_MAX_, false, false)
{
	m_qBottomHoleTreatingPressure.Invalidate();
	m_qTreatingFluidTempEvac.Invalidate();
	m_qTimeOfExposureToTreatingFluidEvac.Invalidate();
}

//From: Extreme Operations
//Label: Bottom-hole treating pressure
//Descr: unloading pressure inside casing. Accessible if 
//bEvacuation=true
//##ModelId=3C0DDDAD0234
CSinglePressure *CEvacuationLoads::BottomHoleTreatingPressure() 
{
	return &m_qBottomHoleTreatingPressure;
}


//From: Extreme Operations
//Label: treating fluid temp.
//Descr: temperature of fluid inside casing.. Accessible 
//if bEvacuation=true
//##ModelId=3C0DDDAD0235
CTemperatureQuantity *CEvacuationLoads::TreatingFluidTempEvac() 
{
	return &m_qTreatingFluidTempEvac;
}


//From: Extreme Operations
//Label: Time of exposure to treating fluid
//Descr: Exposure time [h] (hours, not days). Accessible 
//if bEvacuation=true
//##ModelId=3C0DDDAD0243
CTimeHQuantity *CEvacuationLoads::TimeOfExposureToTreatingFluidEvac() 
{
	return &m_qTimeOfExposureToTreatingFluidEvac;
}

//##ModelId=3C3040AB032A
bool CEvacuationLoads::CheckComplete() const
{
	if (m_qBottomHoleTreatingPressure.Undefined())
	{
		return false;
	}
		
	if (m_qTimeOfExposureToTreatingFluidEvac.Undefined())
	{
		return false;
	}
		
	if (m_qTreatingFluidTempEvac.Undefined())
	{
		return false;
	}
	return true;
}
