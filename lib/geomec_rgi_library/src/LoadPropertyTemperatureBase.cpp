
#include "LoadPropertyTemperatureBase.h"
#include "FormationBase.h"
#include "Temperature.h"

namespace GeomecRGI
{

CLoadPropertyTemperatureBase::CLoadPropertyTemperatureBase(
  const RGProperty& rgProperty, RGInterface& rgi, CModelBase& modelBase,
  CRockMechProcessor& rmp)
: CLoadPropertyTemplate<IDT_VALUETYPE_TEMPERATURE>(rgProperty, rgi, modelBase, rmp)
{
}

CLoadPropertyTemperatureBase::~CLoadPropertyTemperatureBase()
{
}

// protected

void CLoadPropertyTemperatureBase::linkValueTypes(CFormationBase& formation,
  CValueType* pVT, const CDepletionStage& stage,
  bool /*fluidPressureFracDetected*/) const
{
  formation.UserTemperature(stage).LinkTo(*pVT);
}

void CLoadPropertyTemperatureBase::convertValue(geo::CValue& dValue) const
{
  if (dValue.Valid())
  {
  dValue.Value(dValue.Value() - 273.15);  // from K to C
  }
}

} // namespace GeomecRGI
