#ifndef _GlobalFormationLoad_Delegate_h_
#define _GlobalFormationLoad_Delegate_h_

#include "FormationLoadBase_Delegate.h"
#include "GlobalFormationLoad.h"

template <class VALUE_TYPE, class CONSTANT_TYPE>
  class CGlobalFormationLoad_Delegate :
  public CFormationLoadBase_Delegate <VALUE_TYPE>
{
public:
  CGlobalFormationLoad_Delegate(
  CGlobalFormationLoad <VALUE_TYPE, CONSTANT_TYPE> * globalFormationLoad);

private:
  CGlobalFormationLoad_Delegate(const CGlobalFormationLoad_Delegate& rhs);
  CGlobalFormationLoad_Delegate& operator = (
  const CGlobalFormationLoad_Delegate& rhs);

  CGlobalFormationLoad <VALUE_TYPE, CONSTANT_TYPE> * m_globalFormationLoad;

  typedef CGlobalFormationLoad <VALUE_TYPE, CONSTANT_TYPE>
  CGlobalFormationLoadTemplate;
  typedef CGlobalFormationLoad_Delegate <VALUE_TYPE, CONSTANT_TYPE>
  CGlobalFormationLoad_DelegateTemplate;

  REGISTER_DELEGATE(CGlobalFormationLoadTemplate,
  CGlobalFormationLoad_DelegateTemplate);
};

template <class VALUE_TYPE, class CONSTANT_TYPE>
  CGlobalFormationLoad_Delegate <VALUE_TYPE, CONSTANT_TYPE> ::
  CGlobalFormationLoad_Delegate(
      CGlobalFormationLoad <VALUE_TYPE, CONSTANT_TYPE> * globalFormationLoad)
: CFormationLoadBase_Delegate <VALUE_TYPE> (globalFormationLoad)
, m_globalFormationLoad(globalFormationLoad)
{
  ACTIVATE_TEMPLATE_DELEGATE(CGlobalFormationLoadTemplate,
  CGlobalFormationLoad_DelegateTemplate);
}

#endif  // _GlobalFormationLoad_Delegate_h_
