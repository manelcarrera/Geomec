
#include "LoadPropertyIsReservoir.h"

namespace GeomecRGI
{

  CLoadPropertyIsReservoir::CLoadPropertyIsReservoir(const RGProperty& rgProperty,
    RGInterface& rgi, CModelBase& modelBase, CRockMechProcessor& rmp)
    : CLoadPropertyBase(rgProperty, rgi, modelBase, rmp)
  {
  }

  CLoadPropertyIsReservoir::~CLoadPropertyIsReservoir()
  {
  }

  bool CLoadPropertyIsReservoir::loadProperty()
  {
    return true;
  }

} // namespace GeomecRGI
