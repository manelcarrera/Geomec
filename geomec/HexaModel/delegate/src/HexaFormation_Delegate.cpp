#include "stdafx.h"

#include "HexaFormation_Delegate.h"
#include "HexaFormation.h"
#include "AttriHexaFormation.h"

CHexaFormation_Delegate::CHexaFormation_Delegate(CHexaFormation* hexaFormation)
: C3DFormation_Delegate(hexaFormation)
, m_hexaFormation(hexaFormation)
{
}

bool CHexaFormation_Delegate::Attributes()
{
  CAttriHexaFormation dlg(*m_hexaFormation);

  return dlg.DoModal() == IDOK;
}

CHexaFormationEntry_Delegate::CHexaFormationEntry_Delegate(
  CHexaFormationEntry* hexaFormationEntry)
: CFormationEntryTempl_Delegate <CHexaFormation> (
  hexaFormationEntry)
, m_hexaFormationEntry(hexaFormationEntry)
{
}

void CHexaFormationEntry_Delegate::AppendContextMenu(
  CContextMenuInvoker& invoker)
{
  typedef CSingleCommandTemplate <
  CFormationEntryTempl_Delegate <CHexaFormation> > TForBaseCommand;

  // allow gocad export

  invoker.AddCommand("Export to Gocad",
  *(new TForBaseCommand(*this,
      &CFormationEntryTempl_Delegate <CHexaFormation> ::OnExportGocad,
      &CFormationEntryTempl_Delegate <CHexaFormation> ::CanExportGocad)));
  invoker.AddSeparator();

  CFormationEntryTempl_Delegate <CHexaFormation> ::AppendContextMenu(
  invoker);

  typedef CSingleCommandTemplate <CHexaFormationEntry> TForCommand;

#if 0
  invoker.AddCommand("Show formation skin",
    *(new TForCommand(*m_hexaFormationEntry, &CHexaFormationEntry::OnShowSkin,
      0, &CHexaFormationEntry::ShowSkin)));
#endif
}
