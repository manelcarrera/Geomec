// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CPRIORTOCOMPLETIONLOADS_3B680CEB0000_INCLUDED
#define _INC_CPRIORTOCOMPLETIONLOADS_3B680CEB0000_INCLUDED



#include "IComponent.h"

//##ModelId=3C0DDDAA02A0
class CPriorToCompletionLoads 
: public IComponent
{
private:
	
	//##ModelId=3CAAA6560294
	CTemperatureQuantity m_qDrillingTemperature;
	//##ModelId=3CAAA65602B3
	CTimeDQuantity m_qTimeOfExposureToDrillingFluid;

public:
	//##ModelId=3C3040A7028E
	bool CheckComplete() const;
	//##ModelId=3C0DDDAA02E4
	CPriorToCompletionLoads();

	//From: Completion&Testing
	//Label: drilling fluid temperature
	//Descr: bottom hole temperature at drilling stage
	//##ModelId=3C0DDDAA02EF
	CTemperatureQuantity* DrillingTemperature();
	
	//From: Completion&Testing
	//Label: time of exposure to drilling fluid
	//Descr: time of exposion to drilling temperature
	//##ModelId=3C0DDDAA02F0
	CTimeDQuantity* TimeOfExposureToDrillingFluid();
	
};

#endif /* _INC_CPRIORTOCOMPLETIONLOADS_3B680CEB0000_INCLUDED */
