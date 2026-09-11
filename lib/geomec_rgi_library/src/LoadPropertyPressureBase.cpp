
#include "LoadPropertyPressureBase.h"
#include "FormationBase.h"
#include "Pressure.h"
#include "FractureMatrixPressure.h"
#include "ElementValueSet.h"
#include "RGProperty.h"

namespace GeomecRGI
{

CLoadPropertyPressureBase::CLoadPropertyPressureBase(
  const RGProperty& rgProperty, RGInterface& rgi, CModelBase& modelBase,
  CRockMechProcessor& rmp)
: CLoadPropertyTemplate<IDT_VALUETYPE_PRESSURE>(rgProperty, rgi, modelBase, rmp)
{
}

CLoadPropertyPressureBase::~CLoadPropertyPressureBase()
{
}

// protected

void CLoadPropertyPressureBase::linkValueTypes(CFormationBase& formation,
  CValueType* pVT, const CDepletionStage& stage,
  bool fluidPressureFracDetected) const
{
  if (isFractureApertureModel(formation, stage))
  {
    assert(fluidPressureFracDetected);

    if (((m_RGProperty.getType() == RGPropertyType::propElementDeltaFluidPressure) ||
      (m_RGProperty.getType() == RGPropertyType::propElementFluidPressure)) &&
      !formation.FractureMatrixPressure(stage).IsLinkedTo(*pVT) &&
      (static_cast <CElementValueSet&> (
        static_cast <CValueComponent&> (
          pVT->Component()).ValueSet()).ValueSize() > 0))
    {
      formation.FractureMatrixPressure(stage).LinkTo(*pVT);
    }
    
    if (((m_RGProperty.getType() == RGPropertyType::propElementDeltaFluidPressureFrac) ||
      (m_RGProperty.getType() == RGPropertyType::propElementFluidPressureFrac)) &&
      !formation.Pressure(stage).IsLinkedTo(*pVT) &&
      (static_cast <CElementValueSet&> (
        static_cast <CValueComponent&> (
          pVT->Component()).ValueSet()).ValueSize() > 0))
    {
      formation.Pressure(stage).LinkTo(*pVT);
    }
  }
  else
  {
    if (((m_RGProperty.getType() == RGPropertyType::propElementDeltaFluidPressure) ||
      (m_RGProperty.getType() == RGPropertyType::propElementFluidPressure)) &&
      !formation.Pressure(stage).IsLinkedTo(*pVT) &&
      (static_cast <CElementValueSet&> (
        static_cast <CValueComponent&> (
          pVT->Component()).ValueSet()).ValueSize() > 0))
    {
      formation.Pressure(stage).LinkTo(*pVT);
    }
  }
}

void CLoadPropertyPressureBase::convertValue(geo::CValue& dValue) const
{
  if (dValue.Valid())
  {
    dValue.Value(dValue.Value() * 1e-6);  // from Pa to MPa
  }
}

} // namespace GeomecRGI
