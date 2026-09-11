
#include "LoadPropertyTemperature.h"

namespace GeomecRGI
{

CLoadPropertyTemperature::CLoadPropertyTemperature(const RGProperty& rgProperty,
  RGInterface& rgi, CModelBase& modelBase, CRockMechProcessor& rmp)
: CLoadPropertyTemperatureBase(rgProperty, rgi, modelBase, rmp)
{
}

CLoadPropertyTemperature::~CLoadPropertyTemperature()
{
}

} // namespace GeomecRGI
