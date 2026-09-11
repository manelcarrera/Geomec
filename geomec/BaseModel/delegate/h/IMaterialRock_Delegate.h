#ifndef _IMaterialRock_Delegate_h_
#define _IMaterialRock_Delegate_h_

class IMaterialRock;

#include "IMaterial_Delegate.h"

class IMaterialRock_Delegate : public IMaterial_Delegate {
public:
  IMaterialRock_Delegate(IMaterialRock *materialRock);

private:
  IMaterialRock_Delegate(const IMaterialRock_Delegate &rhs);
  IMaterialRock_Delegate &operator=(const IMaterialRock_Delegate &rhs);

  IMaterialRock *m_materialRock;

  REGISTER_DELEGATE(IMaterialRock, IMaterialRock_Delegate);
};

#endif // _IMaterialRock_Delegate_h_
