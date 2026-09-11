
#include "LoadPropertyPoisson.h"

namespace GeomecRGI
{

CLoadPropertyPoisson::CLoadPropertyPoisson(const RGProperty& rgProperty,
  RGInterface& rgi, CModelBase& modelBase, CRockMechProcessor& rmp)
: CCalculateShearModulus(rgProperty, rgi, modelBase, rmp)
{
}

CLoadPropertyPoisson::~CLoadPropertyPoisson()
{
}

bool CLoadPropertyPoisson::loadProperty()
{
  return CCalculateShearModulus::loadProperty <CLoadPropertyPoisson,
  IDT_VALUETYPE_POISSONS_RATIO, IDT_VALUETYPE_POISSONRATIO_NORM,
  IDT_VALUETYPE_POISSONRATIO_TRANS> ();
}

} // namespace GeomecRGI
