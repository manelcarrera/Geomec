#ifndef _DCSTRAINLOAD_H_
#define _DCSTRAINLOAD_H_

#include "IDCStructuralLoad.h"
#include "mfTensor.h"

namespace dia {

class CStrainLoad : public IStructuralLoad {
public:
  // same value in all nodes
  CStrainLoad(CLoadCase &loadcase, const CTensor &values, const geo::IElement &element, bool writePrinciples);

  // different values in nodes
  CStrainLoad(CLoadCase &loadcase, const std::vector<CTensor> &vcValues, const geo::IElement &element,
              bool writePrinciples);

  virtual bool WriteFilos() const;
  const geo::IElement &Element() const;

private:
  const geo::IElement &m_Element;
  std::vector<CTensor> m_vcTensors;

  bool m_WritePrinciples;
};

} // namespace dia

#endif // _DCSTRAINLOAD_H_
