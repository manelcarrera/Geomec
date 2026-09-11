// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER

#include "CFracturingLoads.h"

//##ModelId=3C0DDDAC01BB
CFracturingLoads::CFracturingLoads()
:
m_qMaxInjectingFracPressure				(1.0, 0.0, DOUBLE_MAX_, false, false),
	m_qTreatingFluidTemp				(1.0, DOUBLE_MIN_, DOUBLE_MAX_, false, false),
	m_qTimeOfExposureToTreatingFluid	(1.0, 0.0, DOUBLE_MAX_, false, false)
{
	m_qMaxInjectingFracPressure.Invalidate();
	m_qTreatingFluidTemp.Invalidate();
	m_qTimeOfExposureToTreatingFluid.Invalidate();
}

//From: Extreme Operations
//Label: Maximum injection pressure.
//Descr: Injecting pressure of fracturing fluid. 
//Accessible if bFracturing=true
//##ModelId=3C0DDDAC01BC
CSinglePressure *CFracturingLoads::MaxInjectingFracPressure()
{
	return &m_qMaxInjectingFracPressure;
}


//From: Extreme Operations
//Label: treating fluid temp.
//Descr: temperature of fluid inside casing.. Accessible 
//if bFracturing=true
//##ModelId=3C0DDDAC01C6
CTemperatureQuantity *CFracturingLoads::TreatingFluidTemp() 
{
	return &m_qTreatingFluidTemp;
}


//From: Extreme Operations
//Label: Time of exposure to treating fluid
//Descr: Exposure time [h] (hours, not days). Accessible 
//if bFracturing=true
//##ModelId=3C0DDDAC01C7
CTimeHQuantity *CFracturingLoads::TimeOfExposureToTreatingFluid()
{
	return &m_qTimeOfExposureToTreatingFluid;
}

//##ModelId=3C3040A90398
bool CFracturingLoads::CheckComplete() const
{
	if (m_qMaxInjectingFracPressure.Undefined())
	{
		return false;
	}
		
	if (m_qTimeOfExposureToTreatingFluid.Undefined())
	{
		return false;
	}
		
	if (m_qTreatingFluidTemp.Undefined())
	{
		return false;
	}
	return true;
}
