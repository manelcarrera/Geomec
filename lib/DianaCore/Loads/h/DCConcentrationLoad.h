#ifndef _DCCONCENTRATIONLOAD_H_
#define _DCCONCENTRATIONLOAD_H_

#include "IDCStructuralLoad.h"

namespace dia {

class CConcentrationLoad : public IStructuralLoad {
public:
  // same value in all nodes
  CConcentrationLoad(CLoadCase &loadcase, double size, const geo::IElement &element);

  // vector of values for each node (vcValues.size() must equal to element.NrOfPoints())
  CConcentrationLoad(CLoadCase &loadcase, const std::vector<double> &vcValues, const geo::IElement &element);

  virtual bool WriteFilos() const;
  const geo::IElement &Element() const;

private:
  const geo::IElement &m_Element;
};

} // namespace dia

#endif // _DCCONCENTRATIONLOAD_H_
