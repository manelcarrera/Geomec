#include "stdafx.h"

#include "TetraSubHorizon_Delegate.h"
#include "TetraSubHorizon.h"
#include "AttriHorizon.h"
#include "TetraEntryTypes.h"
#include "ContextMenuInvoker.h"

CTetraSubHorizon_Delegate::CTetraSubHorizon_Delegate(
  CTetraSubHorizon* tetraSubHorizon)
: CTetraHorizonBase_Delegate(tetraSubHorizon)
, m_tetraSubHorizon(tetraSubHorizon)
{
}

bool CTetraSubHorizon_Delegate::Attributes()
{
  typedef CAttriHorizon <CTetraSubHorizon> TAttriHorizon;

  if (m_tetraSubHorizon->Slip())
  {
    // Do fault dialog

    TAttriHorizon dlg(IDD_ATTRI_FAULT, *m_tetraSubHorizon, false);

    if (dlg.DoModal() == IDOK)
    {
      if (!m_tetraSubHorizon->IsLinkedTo(
        *m_tetraSubHorizon->Model().GraphEntry(MD_TETRA_SUB_ALL)))
      {
        m_tetraSubHorizon->AttachToEntry();
        assert(m_tetraSubHorizon->Model().GraphEntry(MD_BASE_HORIZON));
        m_tetraSubHorizon->LinkTo(
          *m_tetraSubHorizon->Model().GraphEntry(MD_BASE_HORIZON));

        CModelBase& model =
          dynamic_cast <CModelBase&> (m_tetraSubHorizon->Model());

        model.InvalidateMesh();
      }

      return TRUE;
    }
  }
  else
  {
    // Do horizon dialog

    TAttriHorizon dlg(IDD_ATTRI_NON_SLIP_HORIZON, *m_tetraSubHorizon, false);

    if (dlg.DoModal() == IDOK)
    {
      if (!m_tetraSubHorizon->IsLinkedTo(
        *m_tetraSubHorizon->Model().GraphEntry(MD_TETRA_SUB_ALL)))
      {
        m_tetraSubHorizon->AttachToEntry();
        assert(m_tetraSubHorizon->Model().GraphEntry(MD_BASE_HORIZON));
        m_tetraSubHorizon->LinkTo(
          *m_tetraSubHorizon->Model().GraphEntry(MD_BASE_HORIZON));

        CModelBase& model =
          dynamic_cast <CModelBase&> (m_tetraSubHorizon->Model());

        model.InvalidateMesh();
      }
    }
  }

  return FALSE;
}

void CTetraSubHorizon_Delegate::AppendContextMenu(CContextMenuInvoker &invoker)
{
#ifdef TETRA_FAULTS_KEYFILE
  const CKeyFile& keyfile = (dynamic_cast <const CGeomecApp*> (
    AfxGetApp()))->KeyFile();

  if (m_tetraSubHorizon->keyfile.Unlocked())
  {
#endif
    typedef CSingleCommandTemplate <CTetraSubHorizon> THorCommand;

    if (m_tetraSubHorizon->Slip())
    {
      invoker.AddCommand("Change to &horizon",
        *(new THorCommand(*m_tetraSubHorizon, &CTetraSubHorizon::ToggleSlip,
          &CTetraSubHorizon::CanToggleSlip)));
    }
    else
    {
      invoker.AddCommand("Change to &fault/fracture",
        *(new THorCommand(*m_tetraSubHorizon, &CTetraSubHorizon::ToggleSlip,
          &CTetraSubHorizon::CanToggleSlip)));
    }

    invoker.AddSeparator();
#ifdef TETRA_FAULTS_KEYFILE
  }
#endif

  CTetraHorizonBase_Delegate::AppendContextMenu(invoker);
}

CTetraSubHorizonEntry_Delegate::CTetraSubHorizonEntry_Delegate(
  CTetraSubHorizonEntry* tetraSubHorizonEntry)
: CStorageNodeEntry_Delegate <CTetraSubHorizon> (tetraSubHorizonEntry)
, m_tetraSubHorizonEntry(tetraSubHorizonEntry)
{
}
