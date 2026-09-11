#include "stdafx.h"

#include "WellCasingRTCI_Delegate.h"
#include "WellCasingRTCI.h"
#include "AttriWellCasingRTCI.h"
#include "ContextMenuInvoker.h"

CWellCasingRTCI_Delegate::CWellCasingRTCI_Delegate(
  CWellCasingRTCI* wellCasingRTCI)
: CColorNode_Delegate(wellCasingRTCI)
, m_wellCasingRTCI(wellCasingRTCI)
{
}

bool CWellCasingRTCI_Delegate::Attributes()
{
  CAttriWellCasingRTCI dlg(*m_wellCasingRTCI);

  return dlg.DoModal() == IDOK;
}

CWellCasingRTCIEntry_Delegate::CWellCasingRTCIEntry_Delegate(
  CWellCasingRTCIEntry* wellCasingRTCIEntry)
: CStorageNodeEntry_Delegate <CWellCasingRTCI> (wellCasingRTCIEntry)
, m_wellCasingRTCIEntry(wellCasingRTCIEntry)
{
}

void CWellCasingRTCIEntry_Delegate::AppendContextMenu(
  CContextMenuInvoker &invoker)
{
  typedef CSingleCommandTemplate <CWellCasingRTCIEntry> TCommand;

  invoker.AddCommand("Create RTCI Wire",
  *new TCommand(*m_wellCasingRTCIEntry, &CWellCasingRTCIEntry::CreateRTCI));
}
