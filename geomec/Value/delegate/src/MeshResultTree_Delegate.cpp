#include "stdafx.h"

#include "MeshResultTree_Delegate.h"
#include "Geomec.h" // Mandatory order, before "MeshResultTree.h"
#include "MeshResultTree.h"
#include "ContextMenuInvoker.h"
#include "ResultExportDlg.h"

CMeshResult_Delegate::CMeshResult_Delegate(CMeshResult* meshResult)
: IValueComposite_Delegate(meshResult)
, m_meshResult(meshResult)
{
}

void CMeshResult_Delegate::AppendContextMenu(CContextMenuInvoker &invoker)
{
  typedef CSingleCommandTemplate <CMeshResult_Delegate> TCommand;

  invoker.AddCommand("&Export results",
    *(new TCommand(*this, &CMeshResult_Delegate::Export,
      &CMeshResult_Delegate::CanExport)));
}

bool CMeshResult_Delegate::CanExport() const
{
  return m_meshResult->CanExport();
}

void CMeshResult_Delegate::Export()
{
  CResultExportDlg dlg(*m_meshResult, -1);

  dlg.DoModal();
}

CMeshResultTree_Delegate::CMeshResultTree_Delegate(
  CMeshResultTree* meshResultTree)
: CGraphEntryTemp_Delegate <CMeshResult> (meshResultTree)
, m_meshResultTree(meshResultTree)
{
}

void CMeshResultTree_Delegate::AppendContextMenu(CContextMenuInvoker &invoker)
{
  typedef CSingleCommandTemplate <CMeshResultTree_Delegate> TCommand;

  invoker.AddCommand("&Export results",
    *(new TCommand(*this, &CMeshResultTree_Delegate::Export,
      &CMeshResultTree_Delegate::CanExport)));
}

bool CMeshResultTree_Delegate::CanExport() const
{
  return m_meshResultTree->CanExport();
}

void CMeshResultTree_Delegate::Export()
{
  CResultExportDlg dlg(**m_meshResultTree->EntryNodes().begin(), -1);

  dlg.DoModal();
}
