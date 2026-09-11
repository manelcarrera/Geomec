#ifndef _TetraHorizonBase_Delegate_h_
#define _TetraHorizonBase_Delegate_h_

class CTetraHorizonBase;

#include "3DHorizon_Delegate.h"

class CTetraHorizonBase_Delegate : public C3DHorizon_Delegate
{
public:
  CTetraHorizonBase_Delegate(CTetraHorizonBase* tetraHorizonBase);

private:
  CTetraHorizonBase_Delegate(const CTetraHorizonBase_Delegate& rhs);
  CTetraHorizonBase_Delegate& operator = (
  const CTetraHorizonBase_Delegate& rhs);

  CTetraHorizonBase* m_tetraHorizonBase;

  REGISTER_DELEGATE(CTetraHorizonBase, CTetraHorizonBase_Delegate);
};

#endif  // _TetraHorizonBase_Delegate_h_
