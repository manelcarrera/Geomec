#include "stdafx.h"

#include "GroupNode_Delegate.h"
#include "GroupNode.h"
#include "ContextMenuInvoker.h"
#include "TreeNode.h"
#include "TreeCtrlBase.h"
#include "FemAppDoc.h"
#include "GeomecDoc.h"
#include "AttriGroupNode.h"

CGroupNode_Delegate::CGroupNode_Delegate(CGroupNode *groupNode)
  : COpenGLNode_Delegate(groupNode)
{
}

CGroupNode_Delegate::~CGroupNode_Delegate()
{
}

void CGroupNode_Delegate::AppendContextMenu(CContextMenuInvoker& invoker)
{
  typedef CSingleCommandTemplate<CFemAppDoc> TModelCommand;
  typedef CSingleCommandTemplate<CGroupNode_Delegate> TGroupCommand;

  invoker.AddCommand(_T("&Delete"), *(new TModelCommand(*GetGeomecDoc(), &CFemAppDoc::OnContextDelete, &CFemAppDoc::OnContextCanDelete)));
  invoker.AddSeparator();
  invoker.AddCommand(_T("&Attributes"), *(new TGroupCommand(*this, &CGroupNode_Delegate::Attributes)));
}

bool CGroupNode_Delegate::Attributes()
{
  CAttriGroupNode dlg(*static_cast<CGroupNode *>(getGraphNode()));
  return (dlg.DoModal() == IDOK);
}


CGroupNodeEntry_Delegate::CGroupNodeEntry_Delegate(CGroupNodeEntry *groupNodeEntry)
  : CStorageNodeEntry_Delegate<CGroupNode>(groupNodeEntry)
{
}

CGroupNodeEntry_Delegate::~CGroupNodeEntry_Delegate()
{
}

