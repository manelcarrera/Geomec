// IDirectedLoad.h: interface for the IDirectedLoad class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IDIRECTEDLOAD_H__284919A7_96EB_4658_8360_BCB4B25637F0__INCLUDED_)
#define AFX_IDIRECTEDLOAD_H__284919A7_96EB_4658_8360_BCB4B25637F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Vector.h"
#include "IDCStructuralLoad.h"

namespace dia {

class CLoadManager;

class IDirectedLoad : public IStructuralLoad  
{
  const geo::CVector m_vecDirection;

public:
  IDirectedLoad(CLoadCase &loadcase, double size, const geo::IVector &vecDirection);
  IDirectedLoad(CLoadCase &loadcase, const std::vector<double>& vcValue, const geo::IVector &vecDirection);
  virtual ~IDirectedLoad();

  const geo::IVector &Direction() const;
};

}

#endif // !defined(AFX_IDIRECTEDLOAD_H__284919A7_96EB_4658_8360_BCB4B25637F0__INCLUDED_)
