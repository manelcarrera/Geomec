
#include "LoadPropertyDeltaFluidPressureFrac.h"
#include "FormationBase.h"
#include "Pressure.h"
#include "HorizonBase.h"
#include "FaultPressure.h"

namespace GeomecRGI
{

CLoadPropertyDeltaFluidPressureFrac::CLoadPropertyDeltaFluidPressureFrac(
  const RGProperty& rgProperty, RGInterface& rgi, CModelBase& modelBase,
  CRockMechProcessor& rmp)
: CLoadPropertyPressureBase(rgProperty, rgi, modelBase, rmp)
{
}

CLoadPropertyDeltaFluidPressureFrac::~CLoadPropertyDeltaFluidPressureFrac()
{
}

// protected

bool CLoadPropertyDeltaFluidPressureFrac::addDelta(
  std::vector <geo::CValue>& vcNodalValues, const QString& strPropertyName,
  const CFormationBase* pFormation, const geo::IElement& elm, int nNod) const
{
  return addDeltaBase(vcNodalValues, strPropertyName, pFormation, elm, nNod);
}

IValueDomainScalar::TValueVec
  CLoadPropertyDeltaFluidPressureFrac::addDeltaSpecific(
  const CDepletionStage& prevstage, const CFormationBase* pFormation,
  const geo::IElement& elm, int nNod) const
{
  if (pFormation)
  {
    if (isFractureApertureModel(*pFormation, prevstage, elm))
    {
      return pFormation->Pressure(prevstage).Component().ScalarData().ValueElement(elm);
    }
  }
  else
  {
    const CHorizonBase* pFault = m_ModelBase.Mesh().SlipHorizon(elm);

    if (pFault && pFault->Slip())
    {
      return pFault->Pressure(prevstage).Component().ScalarData().ValueElement(elm);
    }
  }

  IValueDomainScalar::TValueVec vcPrevValues(nNod, 0);

  return vcPrevValues;
}

} // namespace GeomecRGI
