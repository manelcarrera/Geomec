#include "stdafx.h"

#include "HexaHorizon_Delegate.h"
#include "HexaHorizon.h"
#include "HexaEntryTypes.h"
#include "BranchState.h"
#include "ContextMenuInvoker.h"
#include "HorizonDelDlg.h"
#include "ModelBase.h"

CHexaHorizon_Delegate::CHexaHorizon_Delegate(CHexaHorizon* hexaHorizon)
: C3DHorizon_Delegate(hexaHorizon)
, m_hexaHorizon(hexaHorizon)
{
}

bool CHexaHorizon_Delegate::Attributes()
{
  if (C3DHorizon_Delegate::Attributes())
  {
  if (!m_hexaHorizon->IsLinkedTo(
      *m_hexaHorizon->Model().GraphEntry(MD_HEXA_HORIZON)))
  {
      m_hexaHorizon->LinkTo(
    *m_hexaHorizon->Model().GraphEntry(MD_HEXA_HORIZON));
  }

  return true;
  }

  return false;
}

void CHexaHorizon_Delegate::AppendContextMenu(CContextMenuInvoker& invoker)
{
  if (m_hexaHorizon->UpperFormation())
  {
  invoker.AddCommand(_T("Move &up"),
      *(new CSingleCommandTemplate <CHexaHorizon> (*m_hexaHorizon,
    &CHexaHorizon::MoveUp, &CHexaHorizon::CanMove)));
  }

  if (m_hexaHorizon->LowerFormation())
  {
  invoker.AddCommand(_T("Move &down"),
      *(new CSingleCommandTemplate <CHexaHorizon> (*m_hexaHorizon,
    &CHexaHorizon::MoveDown, &CHexaHorizon::CanMove)));
  }

  if (m_hexaHorizon->UpperFormation() || m_hexaHorizon->LowerFormation())
  {
  invoker.AddSeparator();
  }

  C3DHorizon_Delegate::AppendContextMenu(invoker);
}

bool CHexaHorizon_Delegate::CanDestroy() const
{
  return m_hexaHorizon->CanDestroy();
}

bool CHexaHorizon_Delegate::Destroy()
{
  CHexaFormation* pUpper = m_hexaHorizon->UpperFormation();
  CHexaFormation* pLower = m_hexaHorizon->LowerFormation();
  const CHexaFormation* selection = 0;

  if (pUpper || pLower)
  {
  CHorizonDelDlg dlg(pLower, pUpper);

  if (dlg.DoModal() == IDOK)
  {
      selection = &dlg.Selection();
  }
  else
  {
      return true;
  }
  }

  m_hexaHorizon->LocalDestroy(pUpper, pLower, selection);

  return true;
}

CHexaHorizonEntry_Delegate::CHexaHorizonEntry_Delegate(
  CHexaHorizonEntry* hexaHorizonEntry)
: CStorageNodeEntry_Delegate <CHexaHorizon> (hexaHorizonEntry)
, m_hexaHorizonEntry(hexaHorizonEntry)
{
}

void CHexaHorizonEntry_Delegate::AppendContextMenu(
  CContextMenuInvoker& invoker)
{
  // Export all ...

  typedef CSingleCommandTemplate <CHexaHorizonEntry_Delegate>
  THexaHorizonEntryCommand_Delegate;

  invoker.AddCommand(_T("Create horizon"),
  *(new THexaHorizonEntryCommand_Delegate(*this,
      &CHexaHorizonEntry_Delegate::CreateHorizon,
      &CHexaHorizonEntry_Delegate::CanCreateHorizon)));
}

void CHexaHorizonEntry_Delegate::CreateHorizon()
{
  QString name;

  name = QString("New Horizon %1").arg(
  m_hexaHorizonEntry->GraphEntryNodes().size() + 1);

  CHexaHorizon *pHorizon =
  new CHexaHorizon(name, 0, m_hexaHorizonEntry->Model(), FALSE);
  CHexaHorizon_Delegate *pHorizon_Delegate =
  new CHexaHorizon_Delegate(pHorizon);

  if (!pHorizon_Delegate->Attributes())
  {
  delete pHorizon_Delegate;
  delete pHorizon;
  }
  else
  {
    pHorizon->LinkToEntry();
    pHorizon->AttachToEntry();

  if (pHorizon->SurfaceSize() == 1 && pHorizon->Name() == name)
  {
      // Take over more meaningfull name than default name
      pHorizon->Name(pHorizon->Surface(0).Name());
  }

  m_hexaHorizonEntry->InsertHorizon(*pHorizon);
  }
}

bool CHexaHorizonEntry_Delegate::CanCreateHorizon() const
{
  return !(static_cast <const CModelBase&> (
  m_hexaHorizonEntry->Model())).BranchState().IsBranch();
}
