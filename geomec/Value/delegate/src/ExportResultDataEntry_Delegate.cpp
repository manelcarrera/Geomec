#include "StdAfx.h"

#include "ExportResultDataEntry_Delegate.h"
#include "ExportResultDataEntry.h"
#include "ExportResultData.h"
#include "ContextMenuInvoker.h"
#include "ExportResultData_Delegate.h"
#include "ManageExportMacrosDialog.h"
#include "TNOFileDialog.h"


CExportResultDataEntry_Delegate::CExportResultDataEntry_Delegate(
  CExportResultDataEntry* exportResultDataEntry)
: CStorageNodeEntry_Delegate <CExportResultData> (exportResultDataEntry)
, m_exportResultDataEntry(exportResultDataEntry)
{
}

bool CExportResultDataEntry_Delegate::Attributes()
{
  CExportResultDataEntry* exportResultDataEntry =
  (CExportResultDataEntry*) getGraphNode();

  if ((exportResultDataEntry != 0) &&
  (exportResultDataEntry->EntryNodes().size() > 0))
  {
  CManageExportMacrosDialog
      manageExportMacrosDialog(exportResultDataEntry->GraphEntryNodes(), NULL);

  bool result = (manageExportMacrosDialog.DoModal() == IDOK);

  return result;
  }

  return false;
}

void CExportResultDataEntry_Delegate::
  AppendContextMenu(CContextMenuInvoker& invoker)
{
  typedef CSingleCommandTemplate <CExportResultDataEntry_Delegate>
  CExportResultDataEntry_DelegateCommand;

  invoker.AddCommand(_T("&Create export macro"),
  *(new CExportResultDataEntry_DelegateCommand(*this,
      &CExportResultDataEntry_Delegate::createExportMacro)));
  invoker.AddSeparator();
  invoker.AddCommand(_T("&Import library"), *(new CExportResultDataEntry_DelegateCommand(*this, &CExportResultDataEntry_Delegate::Import)));
  invoker.AddCommand(_T("&Export library"), *(new CExportResultDataEntry_DelegateCommand(*this, &CExportResultDataEntry_Delegate::Export)));
  invoker.AddSeparator();
  invoker.AddCommand(_T("&Attributes"),
  *(new CExportResultDataEntry_DelegateCommand(*this,
      &CExportResultDataEntry_Delegate::Attributes)));
}

void CExportResultDataEntry_Delegate::createExportMacro()
{
  CExportResultData* exportResultData =
  new CExportResultData("Result export macro",
      m_exportResultDataEntry->Model());
  CExportResultData_Delegate* exportResultData_Delegate =
  new CExportResultData_Delegate(exportResultData);

  if (!exportResultData_Delegate->Attributes())
  {
  delete exportResultData_Delegate;
  delete exportResultData;
  }
}

void CExportResultDataEntry_Delegate::Import()
{
  CTnoFileDialog dlg(TRUE, "rmx", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "XML Result Macros (*.rmx)|*.rmx||");
  if (dlg.DoModal() != IDOK)
  return;

  m_exportResultDataEntry->Import((const char *)dlg.GetPathName());
}

void CExportResultDataEntry_Delegate::Export()
{
  CTnoFileDialog dlg(FALSE, "rmx", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "XML Result Macros (*.rmx)|*.rmx||");
  if (dlg.DoModal() != IDOK)
  return;

  //AfxGetApp()->WriteProfileString("Settings", "XMLMatLib", dlg.GetPathName());
  m_exportResultDataEntry->Export((const char *)dlg.GetPathName());
}

