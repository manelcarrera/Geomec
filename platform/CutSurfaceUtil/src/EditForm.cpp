// EditForm.cpp : implementation file
//

#include "stdafx.h"

#include "Box.h"
#include "CheckFrm.h"
#include "CutSurfaceDlg.h"
#include "CutSurfaceFrame.h"
#include "DDXQuantity.h"
#include "EditForm.h"
#include "ISurface.h"
#include "Point.h"
#include "resource.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditForm

IMPLEMENT_DYNCREATE(CEditForm, CFormView)

CEditForm::CEditForm()
    : CFormView(IDD_FORMVIEW), m_pBox(0)

{
  m_NrOfNodeX.MinValue(3, CDoubleQuantity::SI_UNIT);
  m_NrOfNodeY.MinValue(3, CDoubleQuantity::SI_UNIT);
  m_NrOfNodeZ.MinValue(3, CDoubleQuantity::SI_UNIT);

  m_BaseName = "new_";

  m_unit = CDoubleQuantity::SI_UNIT;
  m_IsGenerated = false;
  m_NrOfNodeX.SetValue(10);
  m_NrOfNodeY.SetValue(10);
  m_NrOfNodeZ.SetValue(10);
  //{{AFX_DATA_INIT(CEditForm)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}

CEditForm::~CEditForm() {}

/*virtual*/ void CEditForm::OnInitialUpdate() { CFormView::OnInitialUpdate(); }

void CEditForm::DoDataExchange(CDataExchange *pDX) {
  CFormView::DoDataExchange(pDX);

  DDX_Control(pDX, IDC_EDIT_MIN_X, m_MinX);
  DDX_Control(pDX, IDC_EDIT_MIN_Y, m_MinY);
  DDX_Control(pDX, IDC_EDIT_MIN_Z, m_MinZ);

  DDX_Control(pDX, IDC_EDIT_MAX_X, m_MaxX);
  DDX_Control(pDX, IDC_EDIT_MAX_Y, m_MaxY);
  DDX_Control(pDX, IDC_EDIT_MAX_Z, m_MaxZ);

  DDX_Control(pDX, IDC_EDIT_NODE_X, m_NodeNrX);
  DDX_Control(pDX, IDC_EDIT_NODE_Y, m_NodeNrY);
  DDX_Control(pDX, IDC_EDIT_NODE_Z, m_NodeNrZ);

  DDX_Text(pDX, IDC_EDIT_BASE_NAME, m_BaseName);

  m_NodeNrX.UseAsIntegerInput();
  m_NodeNrY.UseAsIntegerInput();
  m_NodeNrZ.UseAsIntegerInput();

  DDV_Quantity(pDX, IDC_EDIT_MIN_X, &m_qMinX, m_unit, "Min. X");
  DDV_Quantity(pDX, IDC_EDIT_MIN_Y, &m_qMinY, m_unit, "Min. Y");
  DDV_Quantity(pDX, IDC_EDIT_MIN_Z, &m_qMinZ, m_unit, "Min. Z");

  DDV_Quantity(pDX, IDC_EDIT_MAX_X, &m_qMaxX, m_unit, "Max. X");
  DDV_Quantity(pDX, IDC_EDIT_MAX_Y, &m_qMaxY, m_unit, "Max. Y");
  DDV_Quantity(pDX, IDC_EDIT_MAX_Z, &m_qMaxZ, m_unit, "Max. Z");

  DDV_Quantity(pDX, IDC_EDIT_NODE_X, &m_NrOfNodeX, CDoubleQuantity::SI_UNIT, "NrNodesX");
  DDV_Quantity(pDX, IDC_EDIT_NODE_Y, &m_NrOfNodeY, CDoubleQuantity::SI_UNIT, "NrNodesY");
  DDV_Quantity(pDX, IDC_EDIT_NODE_Z, &m_NrOfNodeZ, CDoubleQuantity::SI_UNIT, "NrNodesZ");

  CButton *pBut = (CButton *)GetDlgItem(IDC_BUTTON_GEN_SURFACES);
  pBut->EnableWindow(BoundingBoxDefined() && NodesNrDefined());

  pBut = (CButton *)GetDlgItem(ID_BUTTON_OK);
  pBut->EnableWindow(m_IsGenerated);

  pBut = (CButton *)GetDlgItem(IDC_BUTTON_CLEAR_SURFACES);
  pBut->EnableWindow(m_IsGenerated);

  //{{AFX_DATA_MAP(CEditForm)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEditForm, CFormView)
//{{AFX_MSG_MAP(CEditForm)
ON_BN_CLICKED(IDC_BUTTON_GEN_SURFACES, OnButtonGenSurfaces)
ON_BN_CLICKED(IDC_BUTTON_CLEAR_SURFACES, OnButtonClearSurfaces)
//}}AFX_MSG_MAP
ON_MESSAGE(WM_VALIDATE_DOUBLE_EDIT, OnValidateDoubleEdit)
ON_BN_CLICKED(ID_BUTTON_OK, OnOK)
ON_BN_CLICKED(ID_BUTTON_CANCEL, OnCancel)
END_MESSAGE_MAP()

LRESULT CEditForm::OnValidateDoubleEdit(WPARAM id, LPARAM lparam) {

  m_iControlToValidate = id;
  UpdateData(TRUE);
  m_iControlToValidate = 0;

  if (BoundingBoxDefined()) {
    if (NodesNrDefined())
      m_pFrame->OnNodesNrChanged(GetBoundingBox(), int(m_NrOfNodeX), int(m_NrOfNodeY), int(m_NrOfNodeZ));
    else
      m_pFrame->OnBBChanged(GetBoundingBox());

  } else {
    m_pFrame->OnBBChanged(0);
  }

  return TRUE;
}
void CEditForm::OnOK() {

  UpdateData(TRUE);
  m_pDlg->m_BaseName = m_BaseName;
  m_pDlg->OnOK();
}

void CEditForm::OnCancel() { m_pDlg->OnCancel(); }

/////////////////////////////////////////////////////////////////////////////
// CEditForm diagnostics

#ifdef _DEBUG
void CEditForm::AssertValid() const { CFormView::AssertValid(); }

void CEditForm::Dump(CDumpContext &dc) const { CFormView::Dump(dc); }
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEditForm message handlers

BOOL CEditForm::NodesNrDefined() const {
  if (!m_NrOfNodeX.Undefined() && !m_NrOfNodeY.Undefined() && !m_NrOfNodeZ.Undefined()) {
    return TRUE;
  }

  return FALSE;
}

BOOL CEditForm::BoundingBoxDefined() const {
  if (!m_qMinX.Undefined() && !m_qMinY.Undefined() && !m_qMinZ.Undefined() && !m_qMaxX.Undefined() &&
      !m_qMaxY.Undefined() && !m_qMaxZ.Undefined()) {
    return TRUE;
  }

  return m_pBox != 0;
  //
}

const geo::CBox *CEditForm::GetBoundingBox() const {
  if (m_pBox)
    return new geo::CBox(*m_pBox);

  assert(BoundingBoxDefined());
  geo::CPoint min_p(m_qMinX, m_qMinY, m_qMinZ);
  geo::CPoint max_p(m_qMaxX, m_qMaxY, m_qMaxZ);

  geo::CPoint min_pp = min_p.Min(max_p);
  geo::CPoint max_pp = max_p.Max(min_p);
  return new geo::CBox(min_pp, max_pp);
}

void CEditForm::OnButtonGenSurfaces() {
  assert(BoundingBoxDefined() && NodesNrDefined());

  CWnd *wnd = GetDlgItem(IDC_EDIT_BASE_NAME);
  assert(wnd);
  wnd->GetWindowText(m_BaseName);
  m_pFrame->OnCutSurface(GetBoundingBox(), int(m_NrOfNodeX), int(m_NrOfNodeY), int(m_NrOfNodeZ), (m_pBox == 0),
                         m_BaseName);
  m_IsGenerated = true;
  UpdateData(FALSE);
}

void CEditForm::OnButtonClearSurfaces() {
  m_pFrame->OnCutSurface(0, 0, 0, 0, m_pBox == 0);
  if (BoundingBoxDefined()) {
    if (NodesNrDefined())
      m_pFrame->OnNodesNrChanged(GetBoundingBox(), int(m_NrOfNodeX), int(m_NrOfNodeY), int(m_NrOfNodeZ));
    else
      m_pFrame->OnBBChanged(GetBoundingBox());
  }

  m_IsGenerated = false;
  UpdateData(FALSE);
}

void CEditForm::Init(CCutSurfaceFrame *frame, CCutSurfaceDlg *dlg, CDoubleQuantity::UNIT unit,
                     BOOL calculateDefaultZoomBox, const geo::CBox *pBox) {
  m_unit = unit;
  m_pDlg = dlg;
  m_pFrame = frame;

  if (calculateDefaultZoomBox) {
    CalculateDefaultZoomBox();
  } else if (pBox) {
    m_pBox = pBox;
    if (NodesNrDefined())
      m_pFrame->OnNodesNrChanged(GetBoundingBox(), int(m_NrOfNodeX), int(m_NrOfNodeY), int(m_NrOfNodeZ));
    else
      m_pFrame->OnBBChanged(GetBoundingBox());

    CButton *pBut = (CButton *)GetDlgItem(IDC_BUTTON_GEN_SURFACES);
    pBut->EnableWindow(BoundingBoxDefined() && NodesNrDefined());
  }

  if (!m_pDlg->m_allow_edit_box) {
    CCheckFrame cf;
    cf.Set(this, IDC_FRAME_BOX_DEF);
    cf.Enable(FALSE);
  }
}

void CEditForm::CalculateDefaultZoomBox() {

  geo::CPoint pmin;
  geo::CPoint pmax;

  int i;
  for (i = 0; i < m_pDlg->m_OrgSurfaces->size(); i++) {
    STNamedSurface &ns = m_pDlg->m_OrgSurfaces->at(i);

    const geo::ISurface *surf = ns.Surface;

    if (i == 0) {
      pmin = surf->Min();
      pmax = surf->Max();
    } else {
      pmin = pmin.Min(surf->Min());
      pmax = pmax.Max(surf->Max());
    }
  }

  double fac = 0.333;

  geo::CPoint d = pmax - pmin;

  m_qMinX.SetValue(pmin.X() + 0.5 * d.X() * (1.0 - fac));
  m_qMaxX.SetValue(m_qMinX + d.X() * fac);

  m_qMinY.SetValue(pmin.Y() + 0.5 * d.Y() * (1.0 - fac));
  m_qMaxY.SetValue(m_qMinY + d.Y() * fac);

  m_qMinZ.SetValue(pmin.Z() + 0.5 * d.Z() * (1.0 - fac));
  m_qMaxZ.SetValue(m_qMinZ + d.Z() * fac);

  UpdateData(FALSE);

  if (BoundingBoxDefined()) {
    if (NodesNrDefined())
      m_pFrame->OnNodesNrChanged(GetBoundingBox(), int(m_NrOfNodeX), int(m_NrOfNodeY), int(m_NrOfNodeZ));
    else
      m_pFrame->OnBBChanged(GetBoundingBox());
  }
}
