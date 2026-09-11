#ifndef _BoundaryBase_Delegate_h_
#define _BoundaryBase_Delegate_h_

class CBoundaryBase;

#include "ColorNode_Delegate.h"

class CBoundaryBase_Delegate : public CColorNode_Delegate {
public:
  CBoundaryBase_Delegate(CBoundaryBase *boundaryBase);

  virtual void AppendContextMenu(CContextMenuInvoker &invoker);

  virtual bool CanEdit() const;
  virtual bool Edit();

  virtual bool CanModify() const;

private:
  CBoundaryBase_Delegate(const CBoundaryBase_Delegate &rhs);
  CBoundaryBase_Delegate &operator=(const CBoundaryBase_Delegate &rhs);

  CBoundaryBase *m_boundaryBase;

  REGISTER_DELEGATE(CBoundaryBase, CBoundaryBase_Delegate);
};

#endif // _BoundaryBase_Delegate_h_
