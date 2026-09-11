
#include "LoadPropertyFormation.h"

namespace GeomecRGI {

CLoadPropertyFormation::CLoadPropertyFormation(const RGProperty &rgProperty, RGInterface &rgi, CModelBase &modelBase,
                                               CRockMechProcessor &rmp)
    : CLoadPropertyBase(rgProperty, rgi, modelBase, rmp) {}

CLoadPropertyFormation::~CLoadPropertyFormation() {}

bool CLoadPropertyFormation::loadProperty() { return true; }

} // namespace GeomecRGI
