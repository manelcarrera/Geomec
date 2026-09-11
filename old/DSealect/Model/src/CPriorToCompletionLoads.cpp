// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER

#include "CPriorToCompletionLoads.h"

//##ModelId=3C0DDDAA02E4
CPriorToCompletionLoads::CPriorToCompletionLoads()
:
m_qDrillingTemperature				(1.0, DOUBLE_MIN_, DOUBLE_MAX_, false, false),
m_qTimeOfExposureToDrillingFluid	(1.0, 0.0, DOUBLE_MAX_, false, false)
{
	m_qDrillingTemperature.Invalidate();
	m_qTimeOfExposureToDrillingFluid.Invalidate();
}

//From: Completion&Testing
//Label: drilling fluid temperature
//Descr: bottom hole temperature at drilling stage
//##ModelId=3C0DDDAA02EF
CTemperatureQuantity *CPriorToCompletionLoads::DrillingTemperature()
{
	return &m_qDrillingTemperature;
}


//From: Completion&Testing
//Label: time of exposure to drilling fluid
//Descr: time of exposion to drilling temperature
//##ModelId=3C0DDDAA02F0
CTimeDQuantity *CPriorToCompletionLoads::TimeOfExposureToDrillingFluid()
{
	return &m_qTimeOfExposureToDrillingFluid;
}

//##ModelId=3C3040A7028E
bool CPriorToCompletionLoads::CheckComplete() const
{
	if (m_qDrillingTemperature.Undefined())
	{
		return false;
	}
		
	if (m_qTimeOfExposureToDrillingFluid.Undefined())
	{
		return false;
	}
		
	return true;
}
