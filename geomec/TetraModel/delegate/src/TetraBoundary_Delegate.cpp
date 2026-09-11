#include "stdafx.h"

#include "TetraBoundary_Delegate.h"
#include "TetraBoundary.h"
#include "BoundaryInterfaceDef_Delegate.h"
#include "ContextMenuInvoker.h"
#include "TNOFileDialog.h"
#include "Geomec.h"

CTetraBoundary_Delegate::CTetraBoundary_Delegate(CTetraBoundary* tetraBoundary)
: CBoundaryBase_Delegate(tetraBoundary)
, m_tetraBoundary(tetraBoundary)
{
}

bool CTetraBoundary_Delegate::Attributes() 
{
  CAttriTetraBoundaryDlg dlg(*m_tetraBoundary);

  return dlg.DoModal() == IDOK;
}

void CTetraBoundary_Delegate::AppendContextMenu(CContextMenuInvoker &invoker)
{
  CGeomecApp* pApp = (CGeomecApp*) (AfxGetApp());

  typedef CSingleCommandTemplate <CTetraBoundary> TBoundaryCommand;
  typedef CSingleCommandTemplate <CTetraBoundary_Delegate>
  TBoundaryCommand_Delegate;

  invoker.AddCommand(_T("&Export generated surfaces"),
  *(new TBoundaryCommand_Delegate(*this,
      &CTetraBoundary_Delegate::ExportSurfaces,
      &CTetraBoundary_Delegate::CanExportSurface)));

  if(!m_tetraBoundary->Model().parentModel())
  {
  invoker.AddCommand(_T("&Apply boundary interfaces"),
      *(new TBoundaryCommand(*m_tetraBoundary, &CTetraBoundary::ToggleInterfaces,
    0, &CTetraBoundary::CreateInterfaces)));
  }

  invoker.AddCommand(_T("&Interface attributes"),
  *(new TBoundaryCommand_Delegate(*this,
      &CTetraBoundary_Delegate::InterfaceAttributes,
      &CTetraBoundary_Delegate::CanCreateInterfaces)));
  invoker.AddSeparator();

  CBoundaryBase_Delegate::AppendContextMenu(invoker);
}

bool CTetraBoundary_Delegate::CanModify() const
{
  return m_tetraBoundary->IsSuperModel() &&
  !(static_cast <const CModelBase&> (m_tetraBoundary->Model())).
      BranchState().IsBranch();
}

bool CTetraBoundary_Delegate::CanCreateInterfaces() const
{
  const CModelBase& model = (const CModelBase&) (m_tetraBoundary->Model());

  return (model.IsMesh() != FALSE) && m_tetraBoundary->CreateInterfaces();
}

void CTetraBoundary_Delegate::InterfaceAttributes()
{
  assert(typeid(m_tetraBoundary->InterfaceDefinition()).name() ==
  typeid(CBoundaryInterfaceDef).name());

  CBoundaryInterfaceDef_Delegate* boundaryInterfaceDef_Delegate =
  static_cast <CBoundaryInterfaceDef_Delegate*> (
      m_tetraBoundary->InterfaceDefinition().getDelegate());

  boundaryInterfaceDef_Delegate->Attributes();
}

bool CTetraBoundary_Delegate::CanExportSurface() const
{
  return m_tetraBoundary->CanExportSurface();
}

void CTetraBoundary_Delegate::ExportSurfaces()
{
  // Export boundary surfaces

  CTnoFileDialog dlg(FALSE, "mx", "Boundary.mx", OFN_OVERWRITEPROMPT,
  "GoCad surface (*.mx)|*.mx|All Files (*.*)|*.*||");

  if (dlg.DoModal() == IDOK)
  {
  CFemAppDoc& doc = *GetGeomecDoc();

  m_tetraBoundary->ExportSurfaces((LPCSTR)dlg.GetPathName(), doc.UnitNode());
  }
}

////////////////////////////////////////////////////////////////////////////////
// CAttriTetraBoundaryDlg implementation
////////////////////////////////////////////////////////////////////////////////

CAttriTetraBoundaryDlg::CAttriTetraBoundaryDlg(CTetraBoundary& boundary,
  CWnd* pParent)
: CAttriBoundaryDlg <CTetraBoundary> (boundary, IDD_ATTRI_TETRA_BOUNDARY,
  pParent)
{
}

void CAttriTetraBoundaryDlg::DoDataExchange(CDataExchange* pDX)
{
  CString sUnit;
  double dDistance;
  bool bBranch = (static_cast <const CModelBase&> (
  Copy().Model())).BranchState().IsBranch();

  if (!pDX->m_bSaveAndValidate)
  {
  dDistance = Copy().DistanceToSubBoundary().Value(UnitNode().Unit());
  sUnit = CString(Copy().DistanceToSubBoundary().UnitName(UnitNode().Unit()).
      c_str());
  }

  DDX_Text(pDX, IDC_ED_DISTANCE, dDistance);
  DDX_Text(pDX, IDC_ST_DISTANCE_UNIT, sUnit);
  DDV_MinMaxDouble(pDX, dDistance,
  Copy().DistanceToSubBoundary().MinValue(UnitNode().Unit()),
  Copy().DistanceToSubBoundary().MaxValue(UnitNode().Unit()));

  if (pDX->m_bSaveAndValidate)
  {
  Copy().DistanceToSubBoundary(dDistance, UnitNode().Unit());
  }

  CAttriBoundaryDlg <CTetraBoundary> ::DoDataExchange(pDX);

  GetDlgItem(IDC_RD_BEST_FIT)->EnableWindow(Copy().IsSuperModel() && !bBranch);
  GetDlgItem(IDC_RD_USER_DEFINED)->EnableWindow(Copy().IsSuperModel() &&
  !bBranch);
  GetDlgItem(IDC_ST_DISTANCE)->EnableWindow(Copy().IsSuperModel() && !bBranch);
  GetDlgItem(IDC_ED_DISTANCE)->EnableWindow(Copy().IsSuperModel() && !bBranch);
  GetDlgItem(IDC_ST_DISTANCE_UNIT)->EnableWindow(Copy().IsSuperModel() &&
  !bBranch);
}

BOOL CAttriTetraBoundaryDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
  DWORD nNotification = HIWORD(wParam);
  DWORD nControlID = LOWORD(wParam);

  if (nNotification == EN_KILLFOCUS)
  {
  if (nControlID == IDC_ED_DISTANCE)
  {
      OnKillFocusCoordinate();

      return TRUE;
  }
  }

  return CAttriBoundaryDlg <CTetraBoundary> ::OnCommand(wParam, lParam);
}
