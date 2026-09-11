
#include "CalculateShearModulus.h"

namespace GeomecRGI
{

CCalculateShearModulus::CCalculateShearModulus(const RGProperty& rgProperty,
  RGInterface& rgi, CModelBase& modelBase, CRockMechProcessor& rmp)
: CLoadPropertyBase(rgProperty, rgi, modelBase, rmp)
{
}

CCalculateShearModulus::~CCalculateShearModulus()
{
}

bool CCalculateShearModulus::enforceUniqueness(unsigned int uValueType) const
{
  return true;
}

} // namespace GeomecRGI
