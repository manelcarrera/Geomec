#ifndef _LoadPropertyFluidPressureFrac_h_
#define _LoadPropertyFluidPressureFrac_h_

#include "LoadPropertyPressureBase.h"

namespace GeomecRGI
{

class CLoadPropertyFluidPressureFrac : public CLoadPropertyPressureBase
{
  public:
  CLoadPropertyFluidPressureFrac(const RGProperty& rgProperty,
      RGInterface& rgi, CModelBase& modelBase, CRockMechProcessor& rmp);
  virtual ~CLoadPropertyFluidPressureFrac();

  private:
  CLoadPropertyFluidPressureFrac(const CLoadPropertyFluidPressureFrac& rhs);
  CLoadPropertyFluidPressureFrac& operator = (
      const CLoadPropertyFluidPressureFrac& rhs);
};

} // namespace GeomecRGI

#endif  // _LoadPropertyFluidPressureFrac_h_
