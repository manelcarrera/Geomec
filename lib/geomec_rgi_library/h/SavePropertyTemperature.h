#ifndef _SavePropertyTemperature_h_
#define _SavePropertyTemperature_h_

#include "SavePropertyBase.h"

namespace GeomecRGI
{

class CSavePropertyTemperature : public CSavePropertyBase
{
  public:
  CSavePropertyTemperature(const RGProperty& rgProperty);
  virtual ~CSavePropertyTemperature();

  virtual bool saveProperty(RGInterface& rgi, CModelBase& modelBase,
      const CRockMechProcessor& rmp);

  private:
  CSavePropertyTemperature(const CSavePropertyTemperature& rhs);
  CSavePropertyTemperature& operator = (
      const CSavePropertyTemperature& rhs);
};

} // namespace GeomecRGI

#endif  // _SavePropertyTemperature_h_
