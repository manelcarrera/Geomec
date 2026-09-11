// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CFRACTURINGLOADS_3B67CC380264_INCLUDED
#define _INC_CFRACTURINGLOADS_3B67CC380264_INCLUDED



#include "IComponent.h"


//##ModelId=3C0DDDAC0187
class CFracturingLoads 
: public IComponent
{
private:
	
	//##ModelId=3CAAA65800A0
	CSinglePressure m_qMaxInjectingFracPressure;
	//##ModelId=3CAAA65800BF
	CTemperatureQuantity m_qTreatingFluidTemp;
	//##ModelId=3CAAA65800DF
	CTimeHQuantity m_qTimeOfExposureToTreatingFluid;

public:
	//##ModelId=3C3040A90398
	bool CheckComplete() const;

	//##ModelId=3C0DDDAC01BB
	CFracturingLoads();

	//From: Extreme Operations
	//Label: Maximum injection pressure.
	//Descr: Injecting pressure of fracturing fluid. 
	//Accessible if bFracturing=true
	//##ModelId=3C0DDDAC01BC
	CSinglePressure* MaxInjectingFracPressure();
	
	//From: Extreme Operations
	//Label: treating fluid temp.
	//Descr: temperature of fluid inside casing.. Accessible 
	//if bFracturing=true
	//##ModelId=3C0DDDAC01C6
	CTemperatureQuantity* TreatingFluidTemp();
	
	//From: Extreme Operations
	//Label: Time of exposure to treating fluid
	//Descr: Exposure time [h] (hours, not days). Accessible 
	//if bFracturing=true
	//##ModelId=3C0DDDAC01C7
	CTimeHQuantity* TimeOfExposureToTreatingFluid();
	
};

#endif /* _INC_CFRACTURINGLOADS_3B67CC380264_INCLUDED */
