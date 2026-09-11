// DCLineLoad.h: interface for the CLineLoad class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ILINELOAD_H__8D537144_4396_424A_BB39_131181526D30__INCLUDED_)
#define AFX_ILINELOAD_H__8D537144_4396_424A_BB39_131181526D30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IDCDirectedLoad.h"
#include "ILine.h"

namespace dia {

class CLoadManager;

class CLineLoad : public IDirectedLoad {
  const geo::IObject &m_Object;
  int m_nSetIndex;

public:
  CLineLoad(CLoadCase &loadcase, double size, const geo::IVector &vecDirection, const geo::ILine &line);
  CLineLoad(CLoadCase &loadcase, double size, const geo::IVector &vecDirection, const geo::IElementSet &group,
            int nSetIndex);
  virtual ~CLineLoad();

  virtual bool WriteFilos() const;
};

} // namespace dia

#endif // !defined(AFX_ILINELOAD_H__8D537144_4396_424A_BB39_131181526D30__INCLUDED_)
