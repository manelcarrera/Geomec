#ifndef _3DModel_Delegate_h_
#define _3DModel_Delegate_h_

class C3DModel;

#include "ModelBase_Delegate.h"

class C3DModel_Delegate : public CModelBase_Delegate
{
public:
  C3DModel_Delegate(C3DModel* a3DModel);

private:
  C3DModel_Delegate(const C3DModel_Delegate& rhs);
  C3DModel_Delegate& operator = (const C3DModel_Delegate& rhs);

  C3DModel* m_3DModel;

  REGISTER_DELEGATE(C3DModel, C3DModel_Delegate);
};

#endif  // _3DModel_Delegate_h_
