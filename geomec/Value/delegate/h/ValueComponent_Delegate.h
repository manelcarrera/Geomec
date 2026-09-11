#ifndef _ValueComponent_Delegate_h_
#define _ValueComponent_Delegate_h_

class CValueComponent;

#include "IValueComponent_Delegate.h"
#include "ValueComponent.h"

class CValueComponent_Delegate : public IValueComponent_Delegate {
public:
  CValueComponent_Delegate(CValueComponent *valueComponent);

  virtual bool Attributes();

private:
  CValueComponent_Delegate(const CValueComponent_Delegate &rhs);
  CValueComponent_Delegate &operator=(const CValueComponent_Delegate &rhs);

  CValueComponent *m_valueComponent;

  REGISTER_DELEGATE(CValueComponent, CValueComponent_Delegate);
};

template <unsigned int uComponentName, unsigned int uComponentIndex>
class CComponentTemp_Delegate : public CValueComponent_Delegate {
public:
  CComponentTemp_Delegate(CComponentTemp<uComponentName, uComponentIndex> *componentTemp);

private:
  CComponentTemp_Delegate(const CComponentTemp_Delegate &rhs);
  CComponentTemp_Delegate &operator=(const CComponentTemp_Delegate &rhs);

  CComponentTemp<uComponentName, uComponentIndex> *m_componentTemp;

  typedef CComponentTemp<uComponentName, uComponentIndex> CComponentTempTemplate;
  typedef CComponentTemp_Delegate<uComponentName, uComponentIndex> CComponentTemp_DelegateTemplate;

  REGISTER_DELEGATE(CComponentTempTemplate, CComponentTemp_DelegateTemplate);
};

template <unsigned int uComponentName, unsigned int uComponentIndex>
CComponentTemp_Delegate<uComponentName, uComponentIndex>::CComponentTemp_Delegate(
    CComponentTemp<uComponentName, uComponentIndex> *componentTemp)
    : CValueComponent_Delegate(componentTemp), m_componentTemp(componentTemp) {
  ACTIVATE_TEMPLATE_DELEGATE(CComponentTempTemplate, CComponentTemp_DelegateTemplate);
}

#endif // _ValueComponent_Delegate_h_
