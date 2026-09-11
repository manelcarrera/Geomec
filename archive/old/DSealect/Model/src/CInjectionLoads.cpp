// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER

#include "CInjectionLoads.h"

//##ModelId=3C0DDDAC015D
CInjectionLoads::CInjectionLoads()
:
m_qMaximumInjectionPressure		(1.0, 0.0, DOUBLE_MAX_, false, false),
m_qInjectionTemperature			(1.0, DOUBLE_MIN_, DOUBLE_MAX_, false, false),
m_qTimeOfContinuousInjection	(1.0, 0.0, DOUBLE_MAX_, false, false)
{
	m_qMaximumInjectionPressure.Invalidate();
	m_qInjectionTemperature.Invalidate();
	m_qTimeOfContinuousInjection.Invalidate();
}

//From: Production&Injection
//Label: Maximum injection pressure.
//Descr: Maximum pressure of production fluid. Accessible 
//if bConversion=true
//##ModelId=3C0DDDAC0168
CSinglePressure *CInjectionLoads::MaximumInjectionPressure()
{
	return &m_qMaximumInjectionPressure;
}


//From: Production&Injection
//Label: Injection temperature
//Descr: Temperature of fluid inside casing. Accessible 
//if bConversion=true
//##ModelId=3C0DDDAC0169
CTemperatureQuantity *CInjectionLoads::InjectionTemperature()
{
	return &m_qInjectionTemperature;
}


//From: Production&Injection
//Label: Time of continuous injection
//Descr: Time of continuous injection. Accessible if 
//bConversion=true
//##ModelId=3C0DDDAC016A
CTimeDQuantity *CInjectionLoads::TimeOfContinuousInjection()
{
	return &m_qTimeOfContinuousInjection;
}

//##ModelId=3C3040A9027E
bool CInjectionLoads::CheckComplete() const
{
	if (m_qInjectionTemperature.Undefined())
	{
		return false;
	}
		
	if (m_qMaximumInjectionPressure.Undefined())
	{
		return false;
	}
	
	if (m_qTimeOfContinuousInjection.Undefined())
	{
		return false;
	}
		
	return true;
}
