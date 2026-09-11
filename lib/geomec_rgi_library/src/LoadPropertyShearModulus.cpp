
#include "LoadPropertyShearModulus.h"
#include "ModelBase.h"

namespace GeomecRGI {

CLoadPropertyShearModulus::CLoadPropertyShearModulus(const RGProperty &rgProperty, RGInterface &rgi,
                                                     CModelBase &modelBase, CRockMechProcessor &rmp)
    : CLoadPropertyTemplate<IDT_VALUETYPE_SHEARMODULUS>(rgProperty, rgi, modelBase, rmp), m_NormalYoungsModulus(0),
      m_TransverseYoungsModulus(0), m_NormalPoissonsRatio(0), m_TransversePoissonsRatio(0) {
  retrieveValueTypes(&m_NormalYoungsModulus, &m_TransverseYoungsModulus, &m_NormalPoissonsRatio,
                     &m_TransversePoissonsRatio);
}

CLoadPropertyShearModulus::~CLoadPropertyShearModulus() {}

bool CLoadPropertyShearModulus::canCalculatePropertyValues() const {
  return (m_NormalYoungsModulus != 0) && (m_TransverseYoungsModulus != 0) && (m_NormalPoissonsRatio != 0) &&
         (m_TransversePoissonsRatio != 0);
}

std::vector<std::vector<geo::CValue>> CLoadPropertyShearModulus::calculatePropertyValues() const {
  std::vector<std::vector<geo::CValue>> vcValues;

  for (int e = 0; e < m_ModelBase.Mesh().ElementSet().ElementSize(); ++e) {
    const geo::IElement &element = m_ModelBase.Mesh().ElementSet().Element(e);
    const IValueDomainScalar::TValueVec normalYoungsModulusValueVector =
        m_NormalYoungsModulus->Component().ScalarData().ValueElement(element);
    const IValueDomainScalar::TValueVec transverseYoungsModulusValueVector =
        m_TransverseYoungsModulus->Component().ScalarData().ValueElement(element);
    const IValueDomainScalar::TValueVec normalPoissonsRatioValueVector =
        m_NormalPoissonsRatio->Component().ScalarData().ValueElement(element);
    const IValueDomainScalar::TValueVec transversePoissonsRatioValueVector =
        m_TransversePoissonsRatio->Component().ScalarData().ValueElement(element);

    assert(normalYoungsModulusValueVector.size() == transverseYoungsModulusValueVector.size());
    assert(transverseYoungsModulusValueVector.size() == normalPoissonsRatioValueVector.size());
    assert(normalPoissonsRatioValueVector.size() == transversePoissonsRatioValueVector.size());

    std::vector<geo::CValue> shearModulusValueVector(normalYoungsModulusValueVector.size());

    for (std::vector<geo::CValue>::size_type s = 0; s < shearModulusValueVector.size(); ++s) {
      shearModulusValueVector[s] =
          (normalYoungsModulusValueVector[s].Value() / (2 * (1 + normalPoissonsRatioValueVector[s].Value())));
    }

    vcValues.push_back(shearModulusValueVector);
  }

  return vcValues;
}

bool CLoadPropertyShearModulus::enforceUniqueness(unsigned int uValueType) const { return true; }

// private

void CLoadPropertyShearModulus::convertValue(geo::CValue &dValue) const {
  if (dValue.Valid()) {
    dValue.Value(dValue.Value() * 1e-6); // from Pa to MPa
  }
}

void CLoadPropertyShearModulus::retrieveValueTypes(CValueType **normalYoungsModulus,
                                                   CValueType **transverseYoungsModulus,
                                                   CValueType **normalPoissonsRatio,
                                                   CValueType **transversePoissonsRatio) const {
  std::set<CValueType *> links = m_ModelBase.Mesh().Links<CValueType>();
  std::set<CValueType *>::const_iterator iterator;

  for (iterator = links.begin(); iterator != links.end(); ++iterator) {
    switch ((*iterator)->TypeId()) {
    case IDT_VALUETYPE_YOUNGMODULUS_NORM:
      *normalYoungsModulus = *iterator;
      break;
    case IDT_VALUETYPE_YOUNGMODULUS_TRANS:
      *transverseYoungsModulus = *iterator;
      break;
    case IDT_VALUETYPE_POISSONRATIO_NORM:
      *normalPoissonsRatio = *iterator;
      break;
    case IDT_VALUETYPE_POISSONRATIO_TRANS:
      *transversePoissonsRatio = *iterator;
      break;
    }
  }
}

} // namespace GeomecRGI
