// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CEVACUATIONLOADS_3B680BF202CF_INCLUDED
#define _INC_CEVACUATIONLOADS_3B680BF202CF_INCLUDED



#include "IComponent.h"

//##ModelId=3C0DDDAD01F5
class CEvacuationLoads 
: public IComponent
{
private:
	
	//##ModelId=3CAAA65902D3
	CSinglePressure m_qBottomHoleTreatingPressure;
	//##ModelId=3CAAA65902E2
	CTemperatureQuantity m_qTreatingFluidTempEvac;
	//##ModelId=3CAAA6590301
	CTimeHQuantity m_qTimeOfExposureToTreatingFluidEvac;

public:
	//##ModelId=3C3040AB032A
	bool CheckComplete() const;
	//##ModelId=3C0DDDAD0233
	CEvacuationLoads();

	//From: Extreme Operations
	//Label: Bottom-hole treating pressure
	//Descr: unloading pressure inside casing. Accessible if 
	//bEvacuation=true
	//##ModelId=3C0DDDAD0234
	CSinglePressure* BottomHoleTreatingPressure(); 
	
	//From: Extreme Operations
	//Label: treating fluid temp.
	//Descr: temperature of fluid inside casing.. Accessible 
	//if bEvacuation=true
	//##ModelId=3C0DDDAD0235
	CTemperatureQuantity* TreatingFluidTempEvac(); 
	
	//From: Extreme Operations
	//Label: Time of exposure to treating fluid
	//Descr: Exposure time [h] (hours, not days). Accessible 
	//if bEvacuation=true
	//##ModelId=3C0DDDAD0243
	CTimeHQuantity* TimeOfExposureToTreatingFluidEvac(); 
	

};

#endif /* _INC_CEVACUATIONLOADS_3B680BF202CF_INCLUDED */
