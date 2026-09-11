#ifndef _LoadPropertyFluidPressure_h_
#define _LoadPropertyFluidPressure_h_

#include "LoadPropertyPressureBase.h"

namespace GeomecRGI
{

class CLoadPropertyFluidPressure : public CLoadPropertyPressureBase
{
  public:
    CLoadPropertyFluidPressure(const RGProperty& rgProperty,
      RGInterface& rgi, CModelBase& modelBase, CRockMechProcessor& rmp);
    virtual ~CLoadPropertyFluidPressure();

  private:
    CLoadPropertyFluidPressure(const CLoadPropertyFluidPressure& rhs);
    CLoadPropertyFluidPressure& operator = (
      const CLoadPropertyFluidPressure& rhs);
};

} // namespace GeomecRGI

#endif  // _LoadPropertyFluidPressure_h_
