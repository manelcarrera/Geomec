#ifndef _MaterialCamClay_Delegate_h_
#define _MaterialCamClay_Delegate_h_

class CMaterialCamClay;

#include "IMaterialRock_Delegate.h"

class CMaterialCamClay_Delegate : public IMaterialRock_Delegate
{
public:
  CMaterialCamClay_Delegate(CMaterialCamClay* materialCamClay);

private:
  CMaterialCamClay_Delegate(const CMaterialCamClay_Delegate& rhs);
  CMaterialCamClay_Delegate& operator = (const CMaterialCamClay_Delegate& rhs);

  CMaterialCamClay* m_materialCamClay;

  REGISTER_DELEGATE(CMaterialCamClay, CMaterialCamClay_Delegate);
};

#endif  // _MaterialCamClay_Delegate_h_
