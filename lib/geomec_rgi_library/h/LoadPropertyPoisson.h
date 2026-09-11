#ifndef _LoadPropertyPoisson_h_
#define _LoadPropertyPoisson_h_

#include "CalculateShearModulus.h"

namespace GeomecRGI
{

class CLoadPropertyPoisson : public CCalculateShearModulus
{
  public:
    using CLoadPropertyBase::loadProperty;

    CLoadPropertyPoisson(const RGProperty& rgProperty, RGInterface& rgi,
      CModelBase& modelBase, CRockMechProcessor& rmp);
    virtual ~CLoadPropertyPoisson();

    virtual bool loadProperty();

  private:
    CLoadPropertyPoisson(const CLoadPropertyPoisson& rhs);
    CLoadPropertyPoisson& operator = (const CLoadPropertyPoisson& rhs);
};

} // namespace GeomecRGI

#endif  // _LoadPropertyPoisson_h_
