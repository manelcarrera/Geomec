// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CTESTLOADS_3B680D3D008D_INCLUDED
#define _INC_CTESTLOADS_3B680D3D008D_INCLUDED



#include "IComponent.h"


//##ModelId=3C0DDDAA0158
class CTestLoads 
: public IComponent
{
private:
	
	//##ModelId=3CAAA656013C
	CSinglePressure m_qTestPressure;
  CPressureGradient2 m_qFluidPressureGradient;

public:
	//##ModelId=3C3040A6030B
	bool CheckComplete() const;

	//##ModelId=3C0DDDAA017D
	CTestLoads();

	//From: Completion&Testing
	//Label: test pressure
	//Descr: pressure in casing during pressure test
	//##ModelId=3C0DDDAA017E
	CSinglePressure* TestPressure();
  CPressureGradient2* FluidPressureGradient();
	
};

#endif /* _INC_CTESTLOADS_3B680D3D008D_INCLUDED */
