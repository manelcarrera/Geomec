#ifndef _DisplacementSupportNode_Delegate_h_
#define _DisplacementSupportNode_Delegate_h_

class CDisplacementSupportNode;

#include "TetraSupportNode_Delegate.h"

class CDisplacementSupportNode_Delegate : public C3DSupportNode_Delegate {
public:
  CDisplacementSupportNode_Delegate(CDisplacementSupportNode *displacementSupportNode);

  virtual bool MouseRelease(TFrame &frame, TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point,
                            const TObjectVec &vcHit);

  virtual void AppendContextMenu(CContextMenuInvoker &invoker);

  void OnShowDialog();

protected:
  virtual double UnitFactor() const;
  virtual QString UnitName() const;

private:
  CDisplacementSupportNode_Delegate(const CDisplacementSupportNode_Delegate &rhs);
  CDisplacementSupportNode_Delegate &operator=(const CDisplacementSupportNode_Delegate &rhs);

  CDisplacementSupportNode *m_displacementSupportNode;

  REGISTER_DELEGATE(CDisplacementSupportNode, CDisplacementSupportNode_Delegate);
};

#endif // _DisplacementSupportNode_Delegate_h_
