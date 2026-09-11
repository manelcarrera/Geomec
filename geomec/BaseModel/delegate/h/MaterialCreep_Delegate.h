#ifndef _MaterialCreep_Delegate_h_
#define _MaterialCreep_Delegate_h_

class CMaterialCreep;

#include "IMaterialRock_Delegate.h"

class CMaterialCreep_Delegate : public IMaterialRock_Delegate
{
public:
  CMaterialCreep_Delegate(CMaterialCreep* materialCreep);

private:
  CMaterialCreep_Delegate(const CMaterialCreep_Delegate& rhs);
  CMaterialCreep_Delegate& operator = (const CMaterialCreep_Delegate& rhs);

  CMaterialCreep* m_materialCreep;

  REGISTER_DELEGATE(CMaterialCreep, CMaterialCreep_Delegate);
};

#endif  // _MaterialCreep_Delegate_h_
