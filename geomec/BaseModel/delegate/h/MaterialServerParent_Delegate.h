#ifndef _MaterialServerParent_Delegate_h_
#define _MaterialServerParent_Delegate_h_

#include "Delegate.h"
#include "MaterialServerParent.h"

template <class DELEGATEBASE, class BASE, class MATERIALSERVER>
class CMaterialServerParent_Delegate : public DELEGATEBASE {
public:
  CMaterialServerParent_Delegate(CMaterialServerParent<BASE, MATERIALSERVER> *materialServerParent);

private:
  CMaterialServerParent_Delegate(const CMaterialServerParent_Delegate &rhs);
  CMaterialServerParent_Delegate &operator=(const CMaterialServerParent_Delegate &rhs);

  CMaterialServerParent<BASE, MATERIALSERVER> *m_materialServerParent;

  typedef CMaterialServerParent<BASE, MATERIALSERVER> CMaterialServerParentTemplate;
  typedef CMaterialServerParent_Delegate<DELEGATEBASE, BASE, MATERIALSERVER> CMaterialServerParent_DelegateTemplate;

  REGISTER_DELEGATE(CMaterialServerParentTemplate, CMaterialServerParent_DelegateTemplate);
};

template <class DELEGATEBASE, class BASE, class MATERIALSERVER>
CMaterialServerParent_Delegate<DELEGATEBASE, BASE, MATERIALSERVER>::CMaterialServerParent_Delegate(
    CMaterialServerParent<BASE, MATERIALSERVER> *materialServerParent)
    : DELEGATEBASE(materialServerParent), m_materialServerParent(materialServerParent) {
  ACTIVATE_TEMPLATE_DELEGATE(CMaterialServerParentTemplate, CMaterialServerParent_DelegateTemplate);
}

#endif // _MaterialServerParent_Delegate_h_
