#ifndef _SavePropertyFluidPressure_h_
#define _SavePropertyFluidPressure_h_

#include "SavePropertyBase.h"

namespace GeomecRGI
{

class CSavePropertyFluidPressure : public CSavePropertyBase
{
  public:
  CSavePropertyFluidPressure(const RGProperty& rgProperty);
  virtual ~CSavePropertyFluidPressure();

  virtual bool saveProperty(RGInterface& rgi, CModelBase& modelBase,
      const CRockMechProcessor& rmp);

  private:
  CSavePropertyFluidPressure(const CSavePropertyFluidPressure& rhs);
  CSavePropertyFluidPressure& operator = (
      const CSavePropertyFluidPressure& rhs);
};

} // namespace GeomecRGI

#endif  // _SavePropertyFluidPressure_h_
