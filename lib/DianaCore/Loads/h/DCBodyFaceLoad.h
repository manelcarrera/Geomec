// DCBodyFaceLoad.h: interface for the CBodyFaceLoad class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DCBODYFACELOAD_H__C8160D61_F94F_4822_87FA_34DE7285E684__INCLUDED_)
#define AFX_DCBODYFACELOAD_H__C8160D61_F94F_4822_87FA_34DE7285E684__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IBody.h"
#include "IDCDirectedLoad.h"

namespace dia {

class CBodyFaceLoad : public IDirectedLoad {
  const geo::IBody &m_Body;
  int m_nFaceIndex;
  std::vector<double> m_vcValues;

public:
  // same value for all nodes
  CBodyFaceLoad(CLoadCase &loadcase, double size, const geo::IVector &vecDirection, const geo::IBody &body,
                int nFaceIndex);
  // different values for each node
  CBodyFaceLoad(CLoadCase &loadcase, const std::vector<double> &vcValues, const geo::IVector &vecDirection,
                const geo::IBody &body, int nFaceIndex);
  // a normal load
  CBodyFaceLoad(CLoadCase &loadcase, const std::vector<double> &vcValues, const geo::IBody &body, int nFaceIndex);
  // a normal load
  CBodyFaceLoad(CLoadCase &loadcase, double size, const geo::IBody &body, int nFaceIndex);

  virtual ~CBodyFaceLoad();

  virtual bool WriteFilos() const;
  const geo::IBody &Body() const;
  int FaceIndex() const;
};

} // namespace dia

#endif // !defined(AFX_DCBODYFACELOAD_H__C8160D61_F94F_4822_87FA_34DE7285E684__INCLUDED_)
