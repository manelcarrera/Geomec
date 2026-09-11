#include "DCConcentrationLoad.h"

#include "lbcx.h"
#include "lbfl.h"

#include "IElement.h"

namespace dia {

CConcentrationLoad::CConcentrationLoad(CLoadCase &loadcase, double size, const geo::IElement &element)
    : IStructuralLoad(loadcase, size), m_Element(element) {}

CConcentrationLoad::CConcentrationLoad(CLoadCase &loadcase, const std::vector<double> &vcValues,
                                       const geo::IElement &element)
    : IStructuralLoad(loadcase, vcValues), m_Element(element) {}

bool CConcentrationLoad::WriteFilos() const {
  ftn_int_t idx = Inquire("ELEMEN", "DIM");
  if (idx < 0)
    idx = 0;
  ++idx;

  assert(!XistIndexed("ELEMEN/", &idx));

  PushDir();

  ChangeIndexedDir("ELEMEN/", &idx);

  ftn_int_t idxelm = (ftn_int_t)(Element().Index() + 1);
  PutItemLength("ELEMEN", &idxelm, 1);

  ftn_int_t idxcase = (ftn_int_t)(LoadCase().Index() + 1);
  PutItem("CASE", &idxcase);

  PutItemLength("CONCEN", Values(), ValueSize());

  PopDir();

  return true;
}

const geo::IElement &CConcentrationLoad::Element() const { return m_Element; }

} // namespace dia