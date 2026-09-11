
#include "SavePropertyBodyIndex.h"
#include "GeomecFormationVolumeIDs2RGI.h"
#include "RGInterface.h"
#include "propertyValues.h"

namespace GeomecRGI
{

CSavePropertyBodyIndex::CSavePropertyBodyIndex(const RGProperty& rgProperty)
: CSavePropertyBase(rgProperty)
{
}

CSavePropertyBodyIndex::~CSavePropertyBodyIndex()
{
}

bool CSavePropertyBodyIndex::saveProperty(RGInterface& rgi,
  CModelBase& modelBase, const CRockMechProcessor& /*rmp*/)
{
  CGeomecFormationVolumeIDs2RGI geomecFormationVolumeIDs2RGI(rgi, modelBase);
  std::vector <int> values =
  geomecFormationVolumeIDs2RGI.getFormationVolumeIDs();

  propertyValuesMayNotContainNull(m_RGProperty.getType(), values);
  rgi.saveProperty(m_RGProperty, values);

  return true;
}

} // namespace GeomecRGI
