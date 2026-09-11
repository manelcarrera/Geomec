#include "stdafx.h"

#include "GVTResult_Delegate.h"
#include "GVTResult.h"


CGVTResultGroup_Delegate::CGVTResultGroup_Delegate(CGVTResultGroup* gvtResultGroup)
  : CResultGroup_Delegate(gvtResultGroup)
  , m_gvtResultGroup(gvtResultGroup)
{
}

void CGVTResultGroup_Delegate::AppendContextMenu(CContextMenuInvoker &invoker)
{
  CResultGroup_Delegate::AppendContextMenu(invoker);
}

