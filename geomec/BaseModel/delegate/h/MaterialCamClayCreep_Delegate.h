#ifndef _MaterialCamClayCreep_Delegate_h_
#define _MaterialCamClayCreep_Delegate_h_

class CMaterialCamClayCreep;

#include "IMaterialRock_Delegate.h"

class CMaterialCamClayCreep_Delegate : public IMaterialRock_Delegate {
public:
  CMaterialCamClayCreep_Delegate(CMaterialCamClayCreep *materialCamClay);

private:
  CMaterialCamClayCreep_Delegate(const CMaterialCamClayCreep_Delegate &rhs);
  CMaterialCamClayCreep_Delegate &operator=(const CMaterialCamClayCreep_Delegate &rhs);

  CMaterialCamClayCreep *m_materialCamClayCreep;

  REGISTER_DELEGATE(CMaterialCamClayCreep, CMaterialCamClayCreep_Delegate);
};

#endif // _MaterialCamClayCreep_Delegate_h_
