#ifndef _LoadPropertyTemperature_h_
#define _LoadPropertyTemperature_h_

#include "LoadPropertyTemperatureBase.h"

namespace GeomecRGI
{

class CLoadPropertyTemperature : public CLoadPropertyTemperatureBase
{
  public:
    CLoadPropertyTemperature(const RGProperty& rgProperty, RGInterface& rgi,
      CModelBase& modelBase, CRockMechProcessor& rmp);
    virtual ~CLoadPropertyTemperature();

  private:
    CLoadPropertyTemperature(const CLoadPropertyTemperature& rhs);
    CLoadPropertyTemperature& operator = (const CLoadPropertyTemperature& rhs);
};

} // namespace GeomecRGI

#endif  // _LoadPropertyTemperature_h_
