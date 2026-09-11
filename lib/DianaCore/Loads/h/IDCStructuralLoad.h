#ifndef _IDCSTRUCTURALLOAD_H_
#define _IDCSTRUCTURALLOAD_H_

#include "DCLoadCase.h"
#include "IDCLoad.h"

namespace dia {

class IStructuralLoad : public ILoad {
  CLoadCase &m_LoadCase;

public:
  IStructuralLoad(CLoadCase &loadcase, double size);
  IStructuralLoad(CLoadCase &loadcase, const std::vector<double> &vcValue);

  virtual const CLoadCase &Case() const;
  const CLoadCase &LoadCase() const;
};

} // namespace dia

#endif // _IDCSTRUCTURALLOAD_H_
