// DCFaceMoment.h: interface for the CFaceMoment class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IFACEMOMENT_H__6FB24D7B_38D2_45BB_B9A2_A172707D0C6E__INCLUDED_)
#define AFX_IFACEMOMENT_H__6FB24D7B_38D2_45BB_B9A2_A172707D0C6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IFace.h"

namespace dia {

class CLoadManager;

class CFaceMoment : public IDirectedLoad {
  const geo::IFace &m_Face;

public:
  CFaceMoment(CLoadCase &loadcase, double size, const geo::IVector &vecDirection, const geo::IFace &face);
  virtual ~CFaceMoment();

  const geo::IFace &Face() const;
};

} // namespace dia

#endif // !defined(AFX_IFACEMOMENT_H__6FB24D7B_38D2_45BB_B9A2_A172707D0C6E__INCLUDED_)
