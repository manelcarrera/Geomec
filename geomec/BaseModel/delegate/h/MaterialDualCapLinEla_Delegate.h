#ifndef _MaterialDualCapLinEla_Delegate_h_
#define _MaterialDualCapLinEla_Delegate_h_

class CMaterialDualCapLinEla;

#include "IMaterialRock_Delegate.h"

class CMaterialDualCapLinEla_Delegate : public IMaterialRock_Delegate {
public:
  CMaterialDualCapLinEla_Delegate(CMaterialDualCapLinEla *materialDualCapLinEla);

private:
  CMaterialDualCapLinEla_Delegate(const CMaterialDualCapLinEla_Delegate &rhs);
  CMaterialDualCapLinEla_Delegate &operator=(const CMaterialDualCapLinEla_Delegate &rhs);

  CMaterialDualCapLinEla *m_materialDualCapLinEla;

  REGISTER_DELEGATE(CMaterialDualCapLinEla, CMaterialDualCapLinEla_Delegate);
};

#endif // _MaterialDualCapLinEla_Delegate_h_
