#ifndef _TetraSubBoundary_Delegate_h_
#define _TetraSubBoundary_Delegate_h_

#include "ColorNode_Delegate.h"
#include "TetraSubBoundary.h"

class CTetraSubBoundary_Delegate : public CColorNode_Delegate {
public:
  class CHorizonPlaceHolder_Delegate : public COpenGLNode_Delegate {
  public:
    CHorizonPlaceHolder_Delegate(CTetraSubBoundary::CHorizonPlaceHolder *horizonPlaceHolder);

    virtual void AppendContextMenu(CContextMenuInvoker &invoker);

  private:
    CHorizonPlaceHolder_Delegate(const CHorizonPlaceHolder_Delegate &rhs);
    CHorizonPlaceHolder_Delegate &operator=(const CHorizonPlaceHolder_Delegate &rhs);

    CTetraSubBoundary::CHorizonPlaceHolder *m_horizonPlaceHolder;

    REGISTER_DELEGATE(CTetraSubBoundary::CHorizonPlaceHolder, CHorizonPlaceHolder_Delegate);
  };

  CTetraSubBoundary_Delegate(CTetraSubBoundary *tetraSubBoundary);

  virtual void AppendContextMenu(CContextMenuInvoker &invoker);

private:
  CTetraSubBoundary_Delegate(const CTetraSubBoundary_Delegate &rhs);
  CTetraSubBoundary_Delegate &operator=(const CTetraSubBoundary_Delegate &rhs);

  CTetraSubBoundary *m_tetraSubBoundary;

  REGISTER_DELEGATE(CTetraSubBoundary, CTetraSubBoundary_Delegate);
};

#endif // _TetraSubBoundary_Delegate_h_
