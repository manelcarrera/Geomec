
#include "LoadPropertyDeltaFluidPressure.h"
#include "FaultPressure.h"
#include "FormationBase.h"
#include "FractureMatrixPressure.h"
#include "HorizonBase.h"
#include "Pressure.h"

namespace GeomecRGI {

CLoadPropertyDeltaFluidPressure::CLoadPropertyDeltaFluidPressure(const RGProperty &rgProperty, RGInterface &rgi,
                                                                 CModelBase &modelBase, CRockMechProcessor &rmp)
    : CLoadPropertyPressureBase(rgProperty, rgi, modelBase, rmp), m_useFractureMatrixPressure(false) {}

CLoadPropertyDeltaFluidPressure::~CLoadPropertyDeltaFluidPressure() {}

// protected

unsigned int CLoadPropertyDeltaFluidPressure::OverruleValueType(unsigned int uValueType) const {
  return m_useFractureMatrixPressure ? IDT_VALUETYPE_FRACTURE_MATRIX_PRESSURE : uValueType;
}

bool CLoadPropertyDeltaFluidPressure::addDelta(std::vector<geo::CValue> &vcNodalValues, const QString &strPropertyName,
                                               const CFormationBase *pFormation, const geo::IElement &elm,
                                               int nNod) const {
  return addDeltaBase(vcNodalValues, strPropertyName, pFormation, elm, nNod);
}

IValueDomainScalar::TValueVec CLoadPropertyDeltaFluidPressure::addDeltaSpecific(const CDepletionStage &prevstage,
                                                                                const CFormationBase *pFormation,
                                                                                const geo::IElement &elm,
                                                                                int nNod) const {
  if (pFormation) {
    if (isFractureApertureModel(*pFormation, prevstage, elm)) {
      m_useFractureMatrixPressure = true;
      return pFormation->FractureMatrixPressure(prevstage).Component().ScalarData().ValueElement(elm);
    } else {
      return pFormation->Pressure(prevstage).Component().ScalarData().ValueElement(elm);
    }
  } else {
    const CHorizonBase *pFault = m_ModelBase.Mesh().SlipHorizon(elm);

    if (pFault && pFault->Slip()) {
      return pFault->Pressure(prevstage).Component().ScalarData().ValueElement(elm);
    }
  }

  IValueDomainScalar::TValueVec vcPrevValues(nNod, 0);

  return vcPrevValues;
}

} // namespace GeomecRGI
