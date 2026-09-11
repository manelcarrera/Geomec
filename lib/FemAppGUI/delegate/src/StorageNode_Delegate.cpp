#include "stdafx.h"

#include "StorageNode_Delegate.h"
#include "FemAppDoc.h"
#include "FemAppModel.h"
#include "GeomecDoc.h"

CStorageNode_Delegate::CStorageNode_Delegate(CStorageNode* storageNode)
: CGraphNode_Delegate(storageNode)
, m_storageNode(storageNode)
{
}

void CStorageNode_Delegate::AppendContextMenu(CContextMenuInvoker& invoker)
{
  typedef CSingleCommandTemplate <CFemAppDoc> TFemAppDocCommand;

  invoker.AddCommand(_T("&Copy"), *(new TGraphNode_DelegateCommand(*this,
    &CStorageNode_Delegate::OnCopy)));
  invoker.AddCommand(_T("&Paste"), *(new TGraphNode_DelegateCommand(*this,
    &CStorageNode_Delegate::OnPaste, &CStorageNode_Delegate::OnCanPaste)));
  invoker.AddCommand(_T("&Delete"), *(new TFemAppDocCommand(
    *GetGeomecDoc(), &CFemAppDoc::OnContextDelete,
    &CFemAppDoc::OnContextCanDelete)));
  invoker.AddSeparator();
  invoker.AddCommand(_T("&Attributes"), *(new TGraphNode_DelegateCommand(
    *this, &CGraphNode_Delegate::Attributes)));
}
