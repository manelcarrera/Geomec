#ifndef _MaterialLinear_Delegate_h_
#define _MaterialLinear_Delegate_h_

class CMaterialLinear;

#include "IMaterialRock_Delegate.h"

class CMaterialLinear_Delegate : public IMaterialRock_Delegate
{
public:
  CMaterialLinear_Delegate(CMaterialLinear* materialLinear);

private:
  CMaterialLinear_Delegate(const CMaterialLinear_Delegate& rhs);
  CMaterialLinear_Delegate& operator = (const CMaterialLinear_Delegate& rhs);

  CMaterialLinear* m_materialLinear;

  REGISTER_DELEGATE(CMaterialLinear, CMaterialLinear_Delegate);
};

#endif  // _MaterialLinear_Delegate_h_
