#include "stdafx.h"

#include "3DGUI.h"
#include "3dmodel.h"
#include "BoundaryBase.h"
#include "BoundaryBase_Delegate.h"
#include "BranchState.h"
#include "ContextMenuInvoker.h"
#include "GeomecDoc.h"
#include "ModelBase.h"

CBoundaryBase_Delegate::CBoundaryBase_Delegate(CBoundaryBase *boundaryBase)
    : CColorNode_Delegate(boundaryBase), m_boundaryBase(boundaryBase) {}

void CBoundaryBase_Delegate::AppendContextMenu(CContextMenuInvoker &invoker) {
  typedef CSingleCommandTemplate<CBoundaryBase_Delegate> TBoundaryBase_DelegateCommand;

  // Add own item

  invoker.AddCommand(_T("&Modify"), *(new TBoundaryBase_DelegateCommand(*this, &CBoundaryBase_Delegate::Edit,
                                                                        &CBoundaryBase_Delegate::CanModify)));

  invoker.AddSeparator();

  // Append default menu ..

  CColorNode_Delegate::AppendContextMenu(invoker);
}

bool CBoundaryBase_Delegate::CanEdit() const {
  return !(static_cast<const CModelBase &>(m_boundaryBase->Model())).BranchState().IsBranch();
}

bool CBoundaryBase_Delegate::Edit() { return true; }

bool CBoundaryBase_Delegate::CanModify() const {
  return !(static_cast<const CModelBase &>(m_boundaryBase->Model())).BranchState().IsBranch() &&
         m_boundaryBase->State() != CBoundaryBase::ROTATED;
}
