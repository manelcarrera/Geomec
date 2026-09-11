#ifndef _MaterialUndrained_Delegate_h_
#define _MaterialUndrained_Delegate_h_

class CMaterialUndrained;

#include "IMaterialRock_Delegate.h"

class CMaterialUndrained_Delegate : public IMaterialRock_Delegate
{
public:
  CMaterialUndrained_Delegate(CMaterialUndrained* materialUndrained);

private:
  CMaterialUndrained_Delegate(const CMaterialUndrained_Delegate& rhs);
  CMaterialUndrained_Delegate& operator = (
    const CMaterialUndrained_Delegate& rhs);

  CMaterialUndrained* m_materialUndrained;

  REGISTER_DELEGATE(CMaterialUndrained, CMaterialUndrained_Delegate);
};

#endif  // _MaterialUndrained_Delegate_h_
