
#include "SavePropertyFormation.h"
#include "GeomecFormationIDs2RGI.h"
#include "RGInterface.h"
#include "propertyValues.h"

namespace GeomecRGI
{

CSavePropertyFormation::CSavePropertyFormation(const RGProperty& rgProperty)
: CSavePropertyBase(rgProperty)
{
}

CSavePropertyFormation::~CSavePropertyFormation()
{
}

bool CSavePropertyFormation::saveProperty(RGInterface& rgi,
  CModelBase& modelBase, const CRockMechProcessor& /*rmp*/)
{
  CGeomecFormationIDs2RGI geomecFormationIDs2RGI(rgi, modelBase);
  std::vector <int> values = geomecFormationIDs2RGI.getFormationIDs();

  propertyValuesMayNotContainNull(m_RGProperty.getType(), values);
  rgi.saveProperty(m_RGProperty, values);

  return true;
}

} // namespace GeomecRGI
