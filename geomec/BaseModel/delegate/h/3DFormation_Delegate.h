#ifndef _3DFormation_Delegate_h_
#define _3DFormation_Delegate_h_

class C3DFormation;

#include "FormationBase_Delegate.h"

class C3DFormation_Delegate : public CFormationBase_Delegate
{
public:
  C3DFormation_Delegate(C3DFormation* a3DFormation);

private:
  C3DFormation_Delegate(const C3DFormation_Delegate& rhs);
  C3DFormation_Delegate& operator = (const C3DFormation_Delegate& rhs);

  C3DFormation* m_3DFormation;

  REGISTER_DELEGATE(C3DFormation, C3DFormation_Delegate);
};

#endif  // _3DFormation_Delegate_h_
