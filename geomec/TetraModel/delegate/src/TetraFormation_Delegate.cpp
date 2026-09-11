#include "stdafx.h"

#include "TetraFormation_Delegate.h"
#include "TetraFormation.h"
#include "AttriFormation.h"
#include "AttriTetraVolume.h"
#include "TetraModel.h"

CTetraFormationVolume_Delegate::CTetraFormationVolume_Delegate(
  CTetraFormationVolume* tetraFormationVolume)
: CFormationVolume_Delegate(tetraFormationVolume)
, m_tetraFormationVolume(tetraFormationVolume)
{
}

bool CTetraFormationVolume_Delegate::Attributes() 
{
  CAttriTetraVolume dlg(*m_tetraFormationVolume);

  return dlg.DoModal() == IDOK;
}

bool CTetraFormationVolume_Delegate::CanDestroy() const
{
  return m_tetraFormationVolume->CanDestroy();
}

CTetraFormation_Delegate::CTetraFormation_Delegate(
  CTetraFormation* tetraFormation)
: C3DFormation_Delegate(tetraFormation)
, m_tetraFormation(tetraFormation)
{
}

void CTetraFormation_Delegate::AppendContextMenu(CContextMenuInvoker &invoker)
{
  invoker.AddCommand(_T("Move &up"),
    *(new CSingleCommandTemplate <CTetraFormation> (*m_tetraFormation,
      &CTetraFormation::moveUp, &CTetraFormation::canMoveUp)));
  invoker.AddCommand(_T("Move &down"),
    *(new CSingleCommandTemplate <CTetraFormation> (*m_tetraFormation,
      &CTetraFormation::moveDown, &CTetraFormation::canMoveDown)));
  invoker.AddSeparator();

  C3DFormation_Delegate::AppendContextMenu(invoker);
}

bool CTetraFormation_Delegate::Attributes()
{
  CAttriFormation dlg(*m_tetraFormation);

  return dlg.DoModal() == IDOK;
}

CTetraFormationEntry_Delegate::CTetraFormationEntry_Delegate(
  CTetraFormationEntry* tetraFormationEntry)
: CFormationEntryTempl_Delegate <CTetraFormation> (tetraFormationEntry)
, m_tetraFormationEntry(tetraFormationEntry)
{
}

void CTetraFormationEntry_Delegate::AppendContextMenu(
  CContextMenuInvoker &invoker)
{
  typedef CSingleCommandTemplate <CTetraFormationEntry> TForCommand;
  typedef CSingleCommandTemplate <CTetraFormationEntry_Delegate>
    TForCommand_Delegate;

  invoker.AddCommand("Create &formation", *(new TForCommand_Delegate(*this,
    &CTetraFormationEntry_Delegate::CreateFormation,
    &CTetraFormationEntry_Delegate::CanCreateFormation)));
  invoker.AddSeparator();

  typedef CSingleCommandTemplate <
    CFormationEntryTempl_Delegate <CTetraFormation> > TForBaseCommand;

  // allow gocad export

  invoker.AddCommand("Export to Gocad",
    *(new TForBaseCommand(*this,
      &CFormationEntryTempl_Delegate <CTetraFormation> ::OnExportGocad,
      &CFormationEntryTempl_Delegate <CTetraFormation> ::CanExportGocad)));
  invoker.AddSeparator();

  CFormationEntryTempl_Delegate <CTetraFormation> ::AppendContextMenu(invoker);

#if 0
  invoker.AddCommand("Show formation skin",
    *(new TForCommand(*m_tetraFormationEntry,
      &CTetraFormationEntry::OnShowSkin, 0, &CTetraFormationEntry::ShowSkin)));
#endif
}

void CTetraFormationEntry_Delegate::CreateFormation()
{
  CTetraFormation* pFormation = new CTetraFormation("New formation",
    (CTetraModel&) m_tetraFormationEntry->Model());
  CTetraFormation_Delegate* pFormation_Delegate =
    new CTetraFormation_Delegate(pFormation);

  if (!pFormation_Delegate->Attributes())
  {
    delete pFormation_Delegate;
    delete pFormation;
  }
}

bool CTetraFormationEntry_Delegate::CanCreateFormation() const
{
  return !(static_cast <const CModelBase&> (
    m_tetraFormationEntry->Model())).BranchState().IsBranch();
}
