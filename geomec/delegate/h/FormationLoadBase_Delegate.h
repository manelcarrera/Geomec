#ifndef _FormationLoadBase_Delegate_h_
#define _FormationLoadBase_Delegate_h_

#include "FormationLoadBase.h"
#include "IValueComposite_Delegate.h"

template <class VALUE_TYPE>
  class CFormationLoadBase_Delegate : public IValueComposite_Delegate
{
public:
  CFormationLoadBase_Delegate(
    CFormationLoadBase <VALUE_TYPE> * formationLoadBase);

private:
  CFormationLoadBase_Delegate(const CFormationLoadBase_Delegate& rhs);
  CFormationLoadBase_Delegate& operator = (
    const CFormationLoadBase_Delegate& rhs);

  CFormationLoadBase <VALUE_TYPE> * m_formationLoadBase;

  REGISTER_DELEGATE(CFormationLoadBase <VALUE_TYPE>,
    CFormationLoadBase_Delegate <VALUE_TYPE>);
};

template <class VALUE_TYPE>
  CFormationLoadBase_Delegate <VALUE_TYPE> ::
    CFormationLoadBase_Delegate(
      CFormationLoadBase <VALUE_TYPE> * formationLoadBase)
: IValueComposite_Delegate(formationLoadBase)
, m_formationLoadBase(formationLoadBase)
{
  ACTIVATE_TEMPLATE_DELEGATE(CFormationLoadBase <VALUE_TYPE>,
    CFormationLoadBase_Delegate <VALUE_TYPE>);
}

#endif  // _FormationLoadBase_Delegate_h_
