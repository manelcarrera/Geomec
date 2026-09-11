#ifndef _TETRAMODEL_DELEGATE_H_
#define _TETRAMODEL_DELEGATE_H_

#include "3DModel_Delegate.h"

class CTetraModel;

class CTetraModel_Delegate : public C3DModel_Delegate {
public:
  CTetraModel_Delegate(CTetraModel *aTetraModel);

private:
  CTetraModel_Delegate(const CTetraModel_Delegate &rhs);
  CTetraModel_Delegate &operator=(const CTetraModel_Delegate &rhs);

  CTetraModel *m_TetraModel;

  REGISTER_DELEGATE(CTetraModel, CTetraModel_Delegate);
};

#endif // _TETRAMODEL_DELEGATE_H_
