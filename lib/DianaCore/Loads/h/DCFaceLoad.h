// DCFaceLoad.h: interface for the CFaceLoad class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IFACELOAD_H__1913B429_2A63_4FD1_A988_DFAC4AB7EB22__INCLUDED_)
#define AFX_IFACELOAD_H__1913B429_2A63_4FD1_A988_DFAC4AB7EB22__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IFace.h"

namespace dia {

class CLoadManager;

class CFaceLoad : public IDirectedLoad {
  const geo::IFace &m_Face;

public:
  CFaceLoad(CLoadCase &loadcase, double size, const geo::IVector &vecDirection, const geo::IFace &face);
  virtual ~CFaceLoad();

  const geo::IFace &Face() const;
};

} // namespace dia

#endif // !defined(AFX_IFACELOAD_H__1913B429_2A63_4FD1_A988_DFAC4AB7EB22__INCLUDED_)
