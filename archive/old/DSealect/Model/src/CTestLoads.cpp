// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER

#include "CTestLoads.h"

//##ModelId=3C0DDDAA017D
CTestLoads::CTestLoads()
: m_qTestPressure (0, 0.0, DOUBLE_MAX_, true, false),
  m_qFluidPressureGradient(0, 0, DOUBLE_MAX_, false, false)
{
  m_qTestPressure.Invalidate();
  m_qFluidPressureGradient.Invalidate();
}

//From: Completion&Testing
//Label: test pressure
//Descr: pressure in casing during pressure test
//##ModelId=3C0DDDAA017E
CSinglePressure *CTestLoads::TestPressure() 
{
	return &m_qTestPressure;
}

CPressureGradient2* CTestLoads::FluidPressureGradient()
{
  return &m_qFluidPressureGradient;
}

//##ModelId=3C3040A6030B
bool CTestLoads::CheckComplete() const
{
	if (m_qTestPressure.Undefined())
		return false;

  if(m_qFluidPressureGradient.Undefined())
    return false;
		
	return true;
}
