// DCTemperatureLoad.cpp: implementation of the CTemperatureLoad class.
//
//////////////////////////////////////////////////////////////////////

#include "DCTemperatureLoad.h"

#include "DCLoadManager.h"
#include "IDCDianaRunner.h"

#include "DCLoadCase.h"

#include "IElement.h"

#include "lbcx.h"
#include "lbfl.h"

namespace dia {

CTemperatureLoad::CTemperatureLoad(CLoadCase &loadcase, double size, const geo::IElement &element)
: IStructuralLoad(loadcase, size),
  m_Element(element)
{
}

CTemperatureLoad::CTemperatureLoad(CLoadCase &loadcase, const std::vector<double> &vcValues, const geo::IElement &element)
: IStructuralLoad(loadcase, vcValues),
  m_Element(element)
{
}

CTemperatureLoad::~CTemperatureLoad()
{
}

const geo::IElement &CTemperatureLoad::Element() const
{
  return m_Element;
}

bool CTemperatureLoad::WriteFilos() const
{
  ftn_int_t idx = Inquire("ELEMEN", "DIM");
  if(idx < 0) idx = 0;
  ++idx;

  assert(!XistIndexed("ELEMEN/", &idx));

  PushDir();

  ChangeIndexedDir("ELEMEN/", &idx);

  ftn_int_t idxelm = (ftn_int_t) (Element().Index() + 1);
  PutItemLength("ELEMEN", &idxelm, 1);

  ftn_int_t idxcase = (ftn_int_t) (LoadCase().Index() + 1);
  PutItem("CASE", &idxcase);

  PutItemLength("TEMPER", Values(), ValueSize());

  PopDir();

  return true;
}

} // namespace dia
