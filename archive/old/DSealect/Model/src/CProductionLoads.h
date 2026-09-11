// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CPRODUCTIONLOADS_3B67C019033F_INCLUDED
#define _INC_CPRODUCTIONLOADS_3B67C019033F_INCLUDED



#include "IComponent.h"

class CAnalysisPoint;

//##ModelId=3C0DDDAA0214
class CProductionLoads 
: public IComponent
{

private:
  CAnalysisPoint& m_point;

	//##ModelId=3CAAA6560217
	CSinglePressure m_qBottomHoleFlowingPressure;
	//##ModelId=3CAAA6560236
	CTemperatureQuantity m_qBottomHoleFlowingTemp;
	//##ModelId=3CAAA6560256
	CTimeDQuantity m_qTimeOfContinuousProduction;

public:
	//##ModelId=3C3040A70194
	bool CheckComplete() const;

	//##ModelId=3C0DDDAA0272
	CProductionLoads(CAnalysisPoint& point);

	//From: Production&Injection
	//Label: pore pressure
	//Descr: Pore pressure. 
	//Accessible if bConversion=true. Default value = insitu 
	//pore pressure.
	//##ModelId=3C0DDDAA0273
	CSinglePressure ProductionPorePressure() const;
	
	//From: Production&Injection
	//Label: Bottom hole flowing pressure
	//Descr: flowing pressure at maximum draw-down.. 
	//Accessible if bConversion=true. Default value = insitu 
	//pore pressure.
	//##ModelId=3C0DDDAA0281
	CSinglePressure* BottomHoleFlowingPressure(); 
	//##ModelId=3C0DDDAA0282
	void BottomHoleFlowingPressure(const CSinglePressure& newval);
	
	//From: Production&Injection
	//Label: Bottom hole flowing temp.
	//Descr: Flowing temperature at maximum draw-down.. 
	//Accessible if bConversion=true. Default value = insitu 
	//pore pressure.
	//##ModelId=3C0DDDAA0291
	CTemperatureQuantity* BottomHoleFlowingTemp();
	
	//From: Production&Injection
	//Label: Time of continuous production.
	//Descr: Time of continuous production.. 
	//Accessible if bConversion=true. Default value = insitu 
	//pore pressure..
	//##ModelId=3C0DDDAA0292
	CTimeDQuantity* TimeOfContinuousProduction();
	

};

#endif /* _INC_CPRODUCTIONLOADS_3B67C019033F_INCLUDED */
