#include "StdAfx.h"

#include "ExportResultData_Delegate.h"
#include "ExportResultData.h"
#include "resultexportdlg.h"
#include "ContextMenuInvoker.h"

CExportResultData_Delegate::CExportResultData_Delegate(
  CExportResultData* exportResultData)
: CColorNode_Delegate(exportResultData)
, m_exportResultData(exportResultData)
{
}

bool CExportResultData_Delegate::Attributes()
{
  TResultExportConfiguration resultExportConfiguration(!m_exportResultData->ReservoirMacro());

  if (m_exportResultData->ReservoirMacro())
  resultExportConfiguration.enableFormationSelection = true;

  CResultExportDlg resultExportDlg((CModelBase&) m_exportResultData->Model(),
  resultExportConfiguration, "Cancel", true);

  resultExportDlg.setExportResultData(m_exportResultData, true);

  return (resultExportDlg.DoModal() == IDOK);
}

void CExportResultData_Delegate::AppendContextMenu(CContextMenuInvoker& invoker)
{
  typedef CSingleCommandTemplate <CExportResultData_Delegate>
  CExportResultData_DelegateCommand;

  invoker.AddCommand(_T("&Attributes"), *(new CExportResultData_DelegateCommand(
  *this, &CExportResultData_Delegate::Attributes)));
}
