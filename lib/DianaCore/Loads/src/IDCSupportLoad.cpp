
#include "IDCLoad.h"
#include "IDCDirectedLoad.h"
#include "IDCSupportLoad.h"

#include "DCLoadManager.h"
#include "IDCDianaRunner.h"

#include "ISupport.h"

#include "lbcx.h"
#include "lbfl.h"

namespace dia {

ISupportLoad::ISupportLoad(CLoadCase &loadcase, double size, const geo::IVector &vecDirection, const geo::ISupport &support)
: IDirectedLoad(loadcase, size, vecDirection),
  m_support(support)
{
}

ISupportLoad::~ISupportLoad()
{
}

const geo::ISupport &ISupportLoad::Support() const
{
  return m_support;
}

bool ISupportLoad::WriteFilos(const std::string& sType) const
{
  for(size_t i = 0; i < m_support.DirectionSize(); ++i)
  {
  const geo::IVector& vecComponent = m_support.Direction(i);
  assert(fabs(vecComponent.Length() - 1) < EPS);

  // project the load vector onto the component
  assert(ValueSize() == 1);
  geo::CVector vecLoad = Direction().UnitVector() * double(Values()[0]);
  geo::CVector vecLoadComponent = vecComponent * vecLoad.DotProduct(vecComponent);

  if(vecLoadComponent.Length() > EPS)
  {
    ftn_int_t idxdir = (ftn_int_t) Manager().Runner().AddDirection(vecLoadComponent);

    ftn_int_t idx = Inquire("DEFORM", "DIM");
    if(idx < 0) idx = 0;
    ++idx;

    assert(!XistIndexed("DEFORM/", &idx));

    PushDir();

    ChangeIndexedDir("DEFORM/", &idx);

    PutItem("DIR", &idxdir);

    ftn_int_t nodenr = (ftn_int_t) Support().NodeIndex() + 1;
    PutItemLength("NODES", &nodenr, 1);

    PutCharItem("TYPE", sType.c_str());

      ftn_double_t dValue = ftn_double_t(vecLoadComponent.Length());
    PutItemLength("VALUES", &dValue, 1);

    PopDir();
  }
  }

  return true;
}

} // namespace dia
