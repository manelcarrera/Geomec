#ifndef _LoadPropertyTemplate_h_
#define _LoadPropertyTemplate_h_

#include "LoadPropertyBase.h"

namespace GeomecRGI {

template <unsigned int VALUETYPE> class CLoadPropertyTemplate : public CLoadPropertyBase {
public:
  CLoadPropertyTemplate(const RGProperty &rgProperty, RGInterface &rgi, CModelBase &modelBase, CRockMechProcessor &rmp);
  virtual ~CLoadPropertyTemplate();

  virtual bool loadProperty();

private:
  CLoadPropertyTemplate(const CLoadPropertyTemplate &rhs);
  CLoadPropertyTemplate &operator=(const CLoadPropertyTemplate &rhs);
};

template <unsigned int VALUETYPE>
CLoadPropertyTemplate<VALUETYPE>::CLoadPropertyTemplate(const RGProperty &rgProperty, RGInterface &rgi,
                                                        CModelBase &modelBase, CRockMechProcessor &rmp)
    : CLoadPropertyBase(rgProperty, rgi, modelBase, rmp) {}

template <unsigned int VALUETYPE> CLoadPropertyTemplate<VALUETYPE>::~CLoadPropertyTemplate() {}

template <unsigned int VALUETYPE> bool CLoadPropertyTemplate<VALUETYPE>::loadProperty() {
  return CLoadPropertyBase::loadProperty(VALUETYPE);
}

} // namespace GeomecRGI

#endif // _LoadPropertyTemplate_h_
