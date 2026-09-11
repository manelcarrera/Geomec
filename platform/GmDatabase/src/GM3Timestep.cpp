// Timestep.cpp: implementation of the C3DTimestep class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GM3Timestep.h"
#include "GM3TableDef.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace gm {

C3DTimestep::C3DTimestep(const CString& strName, const int nYear)
: CTimestep((LPCSTR) strName, nYear)
{
}

C3DTimestep::C3DTimestep(const CString& strName)
: CTimestep((LPCSTR) strName)
{
}

C3DTimestep::C3DTimestep()
{
}

void C3DTimestep::ReadRecord(CDaoRecordset &rs, const double &dVersion)
{
#ifndef _WIN64
  assert(!rs.IsEOF());

  // By default we invalidate the year
  m_nYear.Invalidate();
  
  // Read time step ...
  if(dVersion >= 2.6) 
  {
    if(rs.GetFieldValue(FD_YEAROK).boolVal) 
    {
      m_nYear.Value( rs.GetFieldValue(FD_YEAR).iVal );
    }
  }

  // Always read the name
  m_strName = rs.GetFieldValue(FD_TIMESTEP_NAME).pcVal;
#endif
}

void C3DTimestep::ReadNonLinRecord(CDaoRecordset &rs)
{
#ifndef _WIN64
  m_convergence_type		= (eConvergenceType) rs.GetFieldValue(FD_NONLIN_CONVTYPE).lVal;
  m_iteration_scheme		= (eIterationScheme) rs.GetFieldValue(FD_NONLIN_ITERSCHEME).lVal;
  m_nLoadSteps			= rs.GetFieldValue(FD_NONLIN_LOADSTEPS).lVal;
  m_nMaxIterStep			= rs.GetFieldValue(FD_NONLIN_MAXNUMITER).lVal;
  m_dConvNormVal			= rs.GetFieldValue(FD_NONLIN_CONVNORMVAL).dblVal;
#endif
}

}
