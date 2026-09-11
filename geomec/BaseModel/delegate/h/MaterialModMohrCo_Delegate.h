#ifndef _MaterialModMohrCo_Delegate_h_
#define _MaterialModMohrCo_Delegate_h_

class CMaterialModMohrCo;

#include "IMaterialRock_Delegate.h"

class CMaterialModMohrCo_Delegate : public IMaterialRock_Delegate
{
public:
  CMaterialModMohrCo_Delegate(CMaterialModMohrCo* materialModMohrCo);

private:
  CMaterialModMohrCo_Delegate(const CMaterialModMohrCo_Delegate& rhs);
  CMaterialModMohrCo_Delegate& operator = (
    const CMaterialModMohrCo_Delegate& rhs);

  CMaterialModMohrCo* m_materialModMohrCo;

  REGISTER_DELEGATE(CMaterialModMohrCo, CMaterialModMohrCo_Delegate);
};

#endif  // _MaterialModMohrCo_Delegate_h_
