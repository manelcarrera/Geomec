#ifndef _MaterialAnisotropicCamClay_Delegate_h_
#define _MaterialAnisotropicCamClay_Delegate_h_

class CMaterialAnisotropicCamClay;

#include "IMaterialRock_Delegate.h"

class CMaterialAnisotropicCamClay_Delegate : public IMaterialRock_Delegate
{
public:
  CMaterialAnisotropicCamClay_Delegate(
  CMaterialAnisotropicCamClay* materialAnisotropicCamClay);

private:
  CMaterialAnisotropicCamClay_Delegate(
  const CMaterialAnisotropicCamClay_Delegate& rhs);
  CMaterialAnisotropicCamClay_Delegate& operator = (
  const CMaterialAnisotropicCamClay_Delegate& rhs);

  CMaterialAnisotropicCamClay* m_materialAnisotropicCamClay;

  REGISTER_DELEGATE(CMaterialAnisotropicCamClay,
  CMaterialAnisotropicCamClay_Delegate);
};

#endif  // _MaterialAnisotropicCamClay_Delegate_h_
