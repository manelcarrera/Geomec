#ifndef _MaterialFractAnisotropy_Delegate_h_
#define _MaterialFractAnisotropy_Delegate_h_

class CMaterialFractAnisotropy;

#include "IMaterialRock_Delegate.h"

class CMaterialFractAnisotropy_Delegate : public IMaterialRock_Delegate {
public:
  CMaterialFractAnisotropy_Delegate(CMaterialFractAnisotropy *materialFractAnisotropy);

private:
  CMaterialFractAnisotropy_Delegate(const CMaterialFractAnisotropy_Delegate &rhs);
  CMaterialFractAnisotropy_Delegate &operator=(const CMaterialFractAnisotropy_Delegate &rhs);

  CMaterialFractAnisotropy *m_materialFractAnisotropy;

  REGISTER_DELEGATE(CMaterialFractAnisotropy, CMaterialFractAnisotropy_Delegate);
};

#endif // _MaterialFractAnisotropy_Delegate_h_
