// DCWeightLoad.h: interface for the CWeightLoad class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IDCWEIGHTLOAD_H__BC83A16C_ED1D_4A7B_AFBB_A605247E41DA__INCLUDED_)
#define AFX_IDCWEIGHTLOAD_H__BC83A16C_ED1D_4A7B_AFBB_A605247E41DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IDCDirectedLoad.h"

namespace dia {

class CWeightLoad : public IDirectedLoad  
{
public:
  // size is gravity acceleration g
  CWeightLoad(CLoadCase &loadcase, double size, const geo::IVector &vecDirection);
  virtual ~CWeightLoad();

  virtual bool WriteFilos() const;
};

}

#endif // !defined(AFX_IDCWEIGHTLOAD_H__BC83A16C_ED1D_4A7B_AFBB_A605247E41DA__INCLUDED_)
