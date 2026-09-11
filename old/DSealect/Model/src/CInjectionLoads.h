// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CINJECTIONLOADS_3B680B5500BC_INCLUDED
#define _INC_CINJECTIONLOADS_3B680B5500BC_INCLUDED



#include "IComponent.h"


//##ModelId=3C0DDDAC0129
class CInjectionLoads 
: public IComponent
{
private:
	
	//##ModelId=3CAAA6580013
	CSinglePressure m_qMaximumInjectionPressure;
	//##ModelId=3CAAA6580041
	CTemperatureQuantity m_qInjectionTemperature;
	//##ModelId=3CAAA6580052
	CTimeDQuantity m_qTimeOfContinuousInjection;

public:
	//##ModelId=3C3040A9027E
	bool CheckComplete() const;
	//##ModelId=3C0DDDAC015D
	CInjectionLoads();

	//From: Production&Injection
	//Label: Maximum injection pressure.
	//Descr: Maximum pressure of production fluid. Accessible 
	//if bConversion=true
	//##ModelId=3C0DDDAC0168
	CSinglePressure* MaximumInjectionPressure();
	
	//From: Production&Injection
	//Label: Injection temperature
	//Descr: Temperature of fluid inside casing. Accessible 
	//if bConversion=true
	//##ModelId=3C0DDDAC0169
	CTemperatureQuantity* InjectionTemperature();
	
	//From: Production&Injection
	//Label: Time of continuous injection
	//Descr: Time of continuous injection. Accessible if 
	//bConversion=true
	//##ModelId=3C0DDDAC016A
	CTimeDQuantity* TimeOfContinuousInjection();
	
};

#endif /* _INC_CINJECTIONLOADS_3B680B5500BC_INCLUDED */
