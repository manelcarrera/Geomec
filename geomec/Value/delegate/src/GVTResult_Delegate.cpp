#include "stdafx.h"

#include "GVTResult.h"
#include "GVTResult_Delegate.h"

CGVTResultGroup_Delegate::CGVTResultGroup_Delegate(CGVTResultGroup *gvtResultGroup)
    : CResultGroup_Delegate(gvtResultGroup), m_gvtResultGroup(gvtResultGroup) {}

void CGVTResultGroup_Delegate::AppendContextMenu(CContextMenuInvoker &invoker) {
  CResultGroup_Delegate::AppendContextMenu(invoker);
}
