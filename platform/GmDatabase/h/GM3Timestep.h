// Timestep.h: interface for the CTimestep class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMESTEP_H__CD712A2B_6BEC_4AC5_9CF3_5640D9ED592C__INCLUDED_)
#define AFX_TIMESTEP_H__CD712A2B_6BEC_4AC5_9CF3_5640D9ED592C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GMTimestep.h"

namespace gm {
#ifdef _WIN64
class CDaoRecordset;
#endif

class C3DTimestep : public CTimestep 
{
public:
  // Construction
  C3DTimestep();
  C3DTimestep(const CString& strName);
  C3DTimestep(const CString& strName, const int nYear);

  void ReadRecord(CDaoRecordset &rs, const double &dVersion);
  void ReadNonLinRecord(CDaoRecordset &rs);
};

}

#endif // !defined(AFX_TIMESTEP_H__CD712A2B_6BEC_4AC5_9CF3_5640D9ED592C__INCLUDED_)
