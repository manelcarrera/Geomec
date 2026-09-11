#ifndef _MaterialUpscaledAnisotropy_Delegate_h_
#define _MaterialUpscaledAnisotropy_Delegate_h_

class CMaterialUpscaledAnisotropy;

#include "IMaterialRock_Delegate.h"

class CMaterialUpscaledAnisotropy_Delegate : public IMaterialRock_Delegate
{
public:
  CMaterialUpscaledAnisotropy_Delegate(
  CMaterialUpscaledAnisotropy* materialUpscaledAnisotropy);

private:
  CMaterialUpscaledAnisotropy_Delegate(
  const CMaterialUpscaledAnisotropy_Delegate& rhs);
  CMaterialUpscaledAnisotropy_Delegate& operator = (
  const CMaterialUpscaledAnisotropy_Delegate& rhs);

  CMaterialUpscaledAnisotropy* m_materialUpscaledAnisotropy;

  REGISTER_DELEGATE(CMaterialUpscaledAnisotropy,
  CMaterialUpscaledAnisotropy_Delegate);
};

#endif  // _MaterialUpscaledAnisotropy_Delegate_h_
