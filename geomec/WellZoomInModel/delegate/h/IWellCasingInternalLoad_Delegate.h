#ifndef _IWellCasingInternalLoad_Delegate_h_
#define _IWellCasingInternalLoad_Delegate_h_

#include "IValueComposite_Delegate.h"
#include "IWellCasingInternalLoad.h"

template <class VALUETYPE> class IWellCasingInternalLoad_Delegate : public IValueComposite_Delegate {
public:
  IWellCasingInternalLoad_Delegate(IWellCasingInternalLoad<VALUETYPE> *wellCasingInternalLoad);

private:
  IWellCasingInternalLoad_Delegate(const IWellCasingInternalLoad_Delegate &rhs);
  IWellCasingInternalLoad_Delegate &operator=(const IWellCasingInternalLoad_Delegate &rhs);

  IWellCasingInternalLoad<VALUETYPE> *m_wellCasingInternalLoad;

  REGISTER_DELEGATE(IWellCasingInternalLoad<VALUETYPE>, IWellCasingInternalLoad_Delegate<VALUETYPE>);
};

template <class VALUETYPE>
IWellCasingInternalLoad_Delegate<VALUETYPE>::IWellCasingInternalLoad_Delegate(
    IWellCasingInternalLoad<VALUETYPE> *wellCasingInternalLoad)
    : IValueComposite_Delegate(wellCasingInternalLoad), m_wellCasingInternalLoad(wellCasingInternalLoad) {
  ACTIVATE_TEMPLATE_DELEGATE(IWellCasingInternalLoad<VALUETYPE>, IWellCasingInternalLoad_Delegate<VALUETYPE>);
}

#endif // _IWellCasingInternalLoad_Delegate_h_
