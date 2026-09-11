
#include "LoadPropertyBodyIndex.h"
#include "ModelBase.h"

namespace GeomecRGI
{

CLoadPropertyBodyIndex::CLoadPropertyBodyIndex(const RGProperty& rgProperty,
  RGInterface& rgi, CModelBase& modelBase, CRockMechProcessor& rmp)
: CLoadPropertyBase(rgProperty, rgi, modelBase, rmp)
{
}

CLoadPropertyBodyIndex::~CLoadPropertyBodyIndex()
{
}

bool CLoadPropertyBodyIndex::loadProperty()
{
  return true;
}

} // namespace GeomecRGI
