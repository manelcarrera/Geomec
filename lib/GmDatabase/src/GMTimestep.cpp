// GMTimestep.cpp: implementation of the CTimestep class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "GMTimestep.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace gm { 

CTimestep::CTimestep(const QString& strName, const int nYear)
: m_strName(strName), 
  m_nYear(nYear),
  m_convergence_type(FORCE),
  m_iteration_scheme(REGULAR),
  m_dConvNormVal(0.00001),
  m_nMaxIterStep(5),
  m_nLoadSteps(10)
{
}

CTimestep::CTimestep(const QString& strName)
: m_strName(strName),
  m_convergence_type(FORCE),
  m_iteration_scheme(REGULAR),
  m_dConvNormVal(0.00001),
  m_nMaxIterStep(5),
  m_nLoadSteps(10)
{
}

CTimestep::CTimestep()
: m_strName(_T("")),
  m_convergence_type(FORCE),
  m_iteration_scheme(REGULAR),
  m_dConvNormVal(0.00001),
  m_nMaxIterStep(5),
  m_nLoadSteps(10)
{
}

const QString& CTimestep::Name() const
{
	return m_strName;
}

const TCheckedInt& CTimestep::Year() const
{
	return m_nYear;
}

CTimestep::eIterationScheme CTimestep::IterationScheme() const
{
	return m_iteration_scheme;
}

CTimestep::eConvergenceType CTimestep::ConvergenceType() const
{
	return m_convergence_type;
}

double CTimestep::ConvergenceNormValue() const
{
	return m_dConvNormVal;
}

int CTimestep::LoadSteps() const
{
	return m_nLoadSteps;
}

int CTimestep::MaxIterationSteps() const
{
	return m_nMaxIterStep;
}

}
