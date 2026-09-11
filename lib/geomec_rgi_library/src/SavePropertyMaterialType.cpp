
#include "SavePropertyMaterialType.h"
#include "GeomecMaterialModel2RGI.h"
#include "RGInterface.h"

namespace GeomecRGI
{

CSavePropertyMaterialType::CSavePropertyMaterialType(const RGProperty& rgProperty)
: CSavePropertyBase(rgProperty)
{
}

CSavePropertyMaterialType::~CSavePropertyMaterialType()
{
}

bool CSavePropertyMaterialType::saveProperty(RGInterface& rgi,
  CModelBase& modelBase, const CRockMechProcessor& /*rmp*/)
{
  CGeomecMaterialModel2RGI geomecMaterialModel2RGI(rgi, modelBase);
  std::vector <int> values = geomecMaterialModel2RGI.getMaterialTypes();

  rgi.saveProperty(m_RGProperty, values);

  return true;
}

} // namespace GeomecRGI
