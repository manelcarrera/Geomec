#ifndef _TetraSupportNode_Delegate_h_
#define _TetraSupportNode_Delegate_h_

class C3DSupportNode;

#include "BaseSupportNode_Delegate.h"

class C3DSupportNode_Delegate : public CBaseSupportNode_Delegate {
public:
  C3DSupportNode_Delegate(C3DSupportNode *tetraSupportNode);

  virtual double UnitFactor() const;
  virtual QString UnitName() const;

  bool CanEditTensor() const;
  void EditGlobalTensor();

private:
  C3DSupportNode_Delegate(const C3DSupportNode_Delegate &rhs);
  C3DSupportNode_Delegate &operator=(const C3DSupportNode_Delegate &rhs);

  C3DSupportNode *m_tetraSupportNode;

  REGISTER_DELEGATE(C3DSupportNode, C3DSupportNode_Delegate);
};

#endif // _TetraSupportNode_Delegate_h_
