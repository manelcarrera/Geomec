// DCLoadCombination.h: interface for the CLoadCombination class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DCLOADCOMBINATION_H__2DF4A594_FDD6_44A8_8047_280D61744AE1__INCLUDED_)
#define AFX_DCLOADCOMBINATION_H__2DF4A594_FDD6_44A8_8047_280D61744AE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>

namespace dia {

class CLoadManager;
class CLoadCase;

class CLoadCombination  
{
  friend class CLoadManager;

  CLoadManager &m_Manager;
  typedef std::map<const CLoadCase *, double> TFactorMap;
  TFactorMap m_Factors;
  int m_Index;

  // only called by (friend) CLoadManager
  CLoadCombination(CLoadManager &manager, int index);

protected:
  CLoadManager &Manager();

public:
  virtual ~CLoadCombination();

  int Index() const;

  const CLoadManager &Manager() const;

  int LoadCaseSize() const;
  const CLoadCase &LoadCase(int nIndex) const;
  CLoadCase &LoadCase(int nIndex);

  const double &Factor(const CLoadCase &LoadCase) const;

  void InsertLoadCase(const CLoadCase &LoadCase, double dFactor = 1.0);

  bool WriteFilos() const;
};

}

#endif // !defined(AFX_DCLOADCOMBINATION_H__2DF4A594_FDD6_44A8_8047_280D61744AE1__INCLUDED_)
