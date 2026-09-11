// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER

#include "CProductionLoads.h"
#include "CAnalysisPoint.h"
#include "CDataModel.h"
#include "CFormationTopography.h"

//##ModelId=3C0DDDAA0272
CProductionLoads::CProductionLoads(CAnalysisPoint& point)
: m_point(point),
  m_qBottomHoleFlowingPressure	(1.0, 0.0, DOUBLE_MAX_, false, false),
  m_qBottomHoleFlowingTemp		(1.0, DOUBLE_MIN_, DOUBLE_MAX_, false, false),
  m_qTimeOfContinuousProduction	(1.0, 0.0, DOUBLE_MAX_, false, false)
{
	m_qBottomHoleFlowingPressure.Invalidate();
	m_qBottomHoleFlowingTemp.Invalidate();
	m_qTimeOfContinuousProduction.Invalidate();

}

//From: Production&Injection
//Label: pore pressure
//Descr: Pore pressure. 
//Accessible if bConversion=true. Default value = insitu 
//pore pressure.
//##ModelId=3C0DDDAA0273
CSinglePressure CProductionLoads::ProductionPorePressure() const
{
  double dDepth = m_point.TMD()->Value();
  const CFormation* pFormation = Model().FormationTopography()->GetFormationAtDepth(dDepth);
  CSinglePressure qProductionPorePressure;

  if(pFormation)
    qProductionPorePressure.SetValue(pFormation->GetPorePressureAt(dDepth));

	return qProductionPorePressure;
}


//From: Production&Injection
//Label: Bottom hole flowing pressure
//Descr: flowing pressure at maximum draw-down.. 
//Accessible if bConversion=true. Default value = insitu 
//pore pressure.
//##ModelId=3C0DDDAA0281
CSinglePressure *CProductionLoads::BottomHoleFlowingPressure() 
{
	return &m_qBottomHoleFlowingPressure;
}

//##ModelId=3C0DDDAA0282
void CProductionLoads::BottomHoleFlowingPressure(const CSinglePressure& newval)
{
	m_qBottomHoleFlowingPressure= newval;
}

//From: Production&Injection
//Label: Bottom hole flowing temp.
//Descr: Flowing temperature at maximum draw-down.. 
//Accessible if bConversion=true. Default value = insitu 
//pore pressure.
//##ModelId=3C0DDDAA0291
CTemperatureQuantity *CProductionLoads::BottomHoleFlowingTemp()
{
	return &m_qBottomHoleFlowingTemp;
}


//From: Production&Injection
//Label: Time of continuous production.
//Descr: Time of continuous production.. 
//Accessible if bConversion=true. Default value = insitu 
//pore pressure..
//##ModelId=3C0DDDAA0292
CTimeDQuantity *CProductionLoads::TimeOfContinuousProduction()
{
	return &m_qTimeOfContinuousProduction;
}

//##ModelId=3C3040A70194
bool CProductionLoads::CheckComplete() const
{

	if (m_qBottomHoleFlowingPressure.Undefined())
	{
		return false;
	}
		
	if (m_qBottomHoleFlowingTemp.Undefined())
	{
		return false;
	}
	
	if (m_qTimeOfContinuousProduction.Undefined())
	{
		return false;
	}	
	return true;
}
