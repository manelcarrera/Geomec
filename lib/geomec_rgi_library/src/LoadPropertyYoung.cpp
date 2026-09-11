
#include "LoadPropertyYoung.h"

namespace GeomecRGI {

CLoadPropertyYoung::CLoadPropertyYoung(const RGProperty &rgProperty, RGInterface &rgi, CModelBase &modelBase,
                                       CRockMechProcessor &rmp)
    : CCalculateShearModulus(rgProperty, rgi, modelBase, rmp) {}

CLoadPropertyYoung::~CLoadPropertyYoung() {}

bool CLoadPropertyYoung::loadProperty() {
  return CCalculateShearModulus::loadProperty<CLoadPropertyYoung, IDT_VALUETYPE_YOUNGS_MODULUS,
                                              IDT_VALUETYPE_YOUNGMODULUS_NORM, IDT_VALUETYPE_YOUNGMODULUS_TRANS>();
}

// protected

void CLoadPropertyYoung::convertValue(geo::CValue &dValue) const {
  if (dValue.Valid()) {
    dValue.Value(dValue.Value() * 1e-6); // from Pa to MPa
  }
}

} // namespace GeomecRGI
