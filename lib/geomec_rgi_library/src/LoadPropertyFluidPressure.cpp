
#include "LoadPropertyFluidPressure.h"

namespace GeomecRGI {

CLoadPropertyFluidPressure::CLoadPropertyFluidPressure(const RGProperty &rgProperty, RGInterface &rgi,
                                                       CModelBase &modelBase, CRockMechProcessor &rmp)
    : CLoadPropertyPressureBase(rgProperty, rgi, modelBase, rmp) {}

CLoadPropertyFluidPressure::~CLoadPropertyFluidPressure() {}

} // namespace GeomecRGI
