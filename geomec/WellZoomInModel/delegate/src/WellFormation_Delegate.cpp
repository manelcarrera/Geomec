#include "stdafx.h"

#include "AttriNameColorDlg.h"
#include "WellFormation.h"
#include "WellFormation_Delegate.h"

CWellFormation_Delegate::CWellFormation_Delegate(CWellFormation *wellFormation)
    : C3DFormation_Delegate(wellFormation), m_wellFormation(wellFormation) {}

bool CWellFormation_Delegate::Attributes() {
  CAttriNameColorDlg<CWellFormation> dlg(*m_wellFormation);

  return dlg.DoModal() == IDOK;
}

CWellFormationEntry_Delegate::CWellFormationEntry_Delegate(CWellFormationEntry *wellFormationEntry)
    : CFormationEntryTempl_Delegate<CWellFormation>(wellFormationEntry), m_wellFormationEntry(wellFormationEntry) {}

void CWellFormationEntry_Delegate::AppendContextMenu(CContextMenuInvoker &invoker) {
  CFormationEntryTempl_Delegate<CWellFormation>::AppendContextMenu(invoker);

  typedef CSingleCommandTemplate<CWellFormationEntry> TForCommand;

#if 0
  invoker.AddCommand("Show formation skin",
  *(new TForCommand(*m_wellFormationEntry,
      &CWellFormationEntry::OnShowSkin, 0, &CWellFormationEntry::ShowSkin)));
#endif
}
