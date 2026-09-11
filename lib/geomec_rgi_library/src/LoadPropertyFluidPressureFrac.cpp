
#include "LoadPropertyFluidPressureFrac.h"

namespace GeomecRGI
{

CLoadPropertyFluidPressureFrac::CLoadPropertyFluidPressureFrac(
  const RGProperty& rgProperty, RGInterface& rgi, CModelBase& modelBase,
  CRockMechProcessor& rmp)
: CLoadPropertyPressureBase(rgProperty, rgi, modelBase, rmp)
{
}

CLoadPropertyFluidPressureFrac::~CLoadPropertyFluidPressureFrac()
{
}

} // namespace GeomecRGI
