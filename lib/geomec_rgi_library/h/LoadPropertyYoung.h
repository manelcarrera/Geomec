#ifndef _LoadPropertyYoung_h_
#define _LoadPropertyYoung_h_

#include "CalculateShearModulus.h"

namespace GeomecRGI {

class CLoadPropertyYoung : public CCalculateShearModulus {
public:
  using CLoadPropertyBase::loadProperty;

  CLoadPropertyYoung(const RGProperty &rgProperty, RGInterface &rgi, CModelBase &modelBase, CRockMechProcessor &rmp);
  virtual ~CLoadPropertyYoung();

  virtual bool loadProperty();

protected:
  virtual void convertValue(geo::CValue &dValue) const;

private:
  CLoadPropertyYoung(const CLoadPropertyYoung &rhs);
  CLoadPropertyYoung &operator=(const CLoadPropertyYoung &rhs);
};

} // namespace GeomecRGI

#endif // _LoadPropertyYoung_h_
