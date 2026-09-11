// DCPressureLoad.h: interface for the CPressureLoad class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPRESSURELOAD_H__103BD489_2102_4ADA_BD0A_BBF1E8431AE4__INCLUDED_)
#define AFX_IPRESSURELOAD_H__103BD489_2102_4ADA_BD0A_BBF1E8431AE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IDCStructuralLoad.h"

namespace dia {

class CLoadManager;

class CPressureLoad : public IStructuralLoad {
  const geo::IElement &m_Element;

public:
  // same value in all nodes
  CPressureLoad(CLoadCase &loadcase, double size, const geo::IElement &element);

  // vector of values for each node (vcValues.size() must equal to body.NrOfPoints())
  CPressureLoad(CLoadCase &loadcase, const std::vector<double> &vcValues, const geo::IElement &element);
  virtual ~CPressureLoad();

  virtual bool WriteFilos() const;
  const geo::IElement &Element() const;
};

} // namespace dia

#endif // !defined(AFX_IPRESSURELOAD_H__103BD489_2102_4ADA_BD0A_BBF1E8431AE4__INCLUDED_)
