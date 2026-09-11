#ifndef _PressureSupportNode_Delegate_h_
#define _PressureSupportNode_Delegate_h_

class CPressureSupportNode;

#include "TetraSupportNode_Delegate.h"

class CPressureSupportNode_Delegate : public C3DSupportNode_Delegate {
public:
  CPressureSupportNode_Delegate(CPressureSupportNode *pressureSupportNode);

  virtual bool MouseRelease(TFrame &frame, TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point,
                            const TObjectVec &vcHit);

  virtual void AppendContextMenu(CContextMenuInvoker &invoker);

  void OnShowDialog();

protected:
  virtual double UnitFactor() const;
  virtual QString UnitName() const;

private:
  CPressureSupportNode_Delegate(const CPressureSupportNode_Delegate &rhs);
  CPressureSupportNode_Delegate &operator=(const CPressureSupportNode_Delegate &rhs);

  CPressureSupportNode *m_pressureSupportNode;

  REGISTER_DELEGATE(CPressureSupportNode, CPressureSupportNode_Delegate);
};

#endif // _PressureSupportNode_Delegate_h_
