
#include "LoadPropertyDeltaTemperature.h"
#include "FormationBase.h"
#include "Temperature.h"

namespace GeomecRGI
{

CLoadPropertyDeltaTemperature::CLoadPropertyDeltaTemperature(
  const RGProperty& rgProperty, RGInterface& rgi, CModelBase& modelBase,
  CRockMechProcessor& rmp)
: CLoadPropertyTemperatureBase(rgProperty, rgi, modelBase, rmp)
{
}

CLoadPropertyDeltaTemperature::~CLoadPropertyDeltaTemperature()
{
}

// protected

void CLoadPropertyDeltaTemperature::convertValue(geo::CValue& /*dValue*/) const
{
}

bool CLoadPropertyDeltaTemperature::addDelta(
  std::vector <geo::CValue>& vcNodalValues, const QString& strPropertyName,
  const CFormationBase* pFormation, const geo::IElement& elm, int nNod) const
{
  return addDeltaBase(vcNodalValues, strPropertyName, pFormation, elm, nNod);
}

IValueDomainScalar::TValueVec CLoadPropertyDeltaTemperature::addDeltaSpecific(
  const CDepletionStage& prevstage, const CFormationBase* pFormation,
  const geo::IElement& elm, int nNod) const
{
  if (pFormation)
  {
  return pFormation->UserTemperature(prevstage).Component().ScalarData().ValueElement(elm);
  }

  IValueDomainScalar::TValueVec vcPrevValues(nNod, 0);

  return vcPrevValues;
}

} // namespace GeomecRGI
