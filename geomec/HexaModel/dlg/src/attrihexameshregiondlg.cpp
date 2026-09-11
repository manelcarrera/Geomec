// attrihexameshregiondlg.cpp : implementation file
//

#include "attrihexameshregiondlg.h"
#include "BranchState.h"
#include "GlobalMessage.h"
#include "HexaBoundary.h"
#include "HexaModel.h"
#include "geomec.h"
#include "hexaentrytypes.h"
#include "modelbase.h"
#include "stdafx.h"
#include <cmath>

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CAttriHexaMainMeshRegionDlg dialog
/////////////////////////////////////////////////////////////////////////////

CAttriHexaMainMeshRegionDlg::CAttriHexaMainMeshRegionDlg(CHexaMainMeshRegion &region, CWnd *pParent /*=NULL*/)
    : CAttributesTemplate<CHexaMainMeshRegion>(CAttriHexaMainMeshRegionDlg::IDD, region, pParent) {
  //{{AFX_DATA_INIT(CAttriHexaMainMeshRegionDlg)
  //}}AFX_DATA_INIT
}

void CAttriHexaMainMeshRegionDlg::DoDataExchange(CDataExchange *pDX) {
  CAttributesTemplate<CHexaMainMeshRegion>::DoDataExchange(pDX);

  double dGridX, dGridY;
  CString sUnit;
  CLengthQuantity l;

  if (!pDX->m_bSaveAndValidate) {
    sUnit = CString(l.UnitName(UnitNode().Unit()).c_str());
    dGridX = l.Convert(Copy().GridSizeX(), UnitNode().Unit(), CQuantity::SI_UNIT);
    dGridY = l.Convert(Copy().GridSizeY(), UnitNode().Unit(), CQuantity::SI_UNIT);
  }

  //{{AFX_DATA_MAP(CAttriHexaMainMeshRegionDlg)
  //}}AFX_DATA_MAP

  DDX_Text(pDX, IDC_ST_UNIT_I, sUnit);
  DDX_Text(pDX, IDC_ST_UNIT_II, sUnit);
  DDX_Text(pDX, IDC_ED_GRID_X, dGridX);
  DDX_Text(pDX, IDC_ED_GRID_Y, dGridY);

  // Do some minmax validation ...
  CModelBase *pModel = (CModelBase *)(&Copy().Model());

  // Save the values.
  if (pDX->m_bSaveAndValidate) {
    if ((fabs(dGridX - l.Convert(Copy().GridSizeX(), UnitNode().Unit(), CQuantity::SI_UNIT)) > EPS) ||
        (fabs(dGridY - l.Convert(Copy().GridSizeY(), UnitNode().Unit(), CQuantity::SI_UNIT)) > EPS)) {
      if (dGridX <= 0 || dGridY <= 0) {
        AfxMessageBox("Grid size must be greater than 0");
        pDX->PrepareEditCtrl(dGridX <= 0 ? IDC_ED_GRID_X : IDC_ED_GRID_Y);
        pDX->Fail();
      }

      double dMaxX = pModel->Boundary().Max().X() - pModel->Boundary().Min().X(); // minimum of 1 element in x-direction
      double dMaxY = pModel->Boundary().Max().Y() - pModel->Boundary().Min().Y(); // minimum of 1 element in y-direction
      if (dGridX > l.Convert(dMaxX, UnitNode().Unit(), CQuantity::SI_UNIT) ||
          dGridY > l.Convert(dMaxY, UnitNode().Unit(), CQuantity::SI_UNIT)) {
        CString msg;
        msg.Format("Grid size can be at most %g",
                   dGridX > l.Convert(dMaxX, UnitNode().Unit(), CQuantity::SI_UNIT) ? dMaxX : dMaxY);
        AfxMessageBox(msg);
        pDX->PrepareEditCtrl(dGridX > l.Convert(dMaxX, UnitNode().Unit(), CQuantity::SI_UNIT) ? IDC_ED_GRID_X
                                                                                              : IDC_ED_GRID_Y);
        pDX->Fail();
      }
      Copy().SetMainGrid(l.Convert(dGridX, CQuantity::SI_UNIT, UnitNode().Unit()),
                         l.Convert(dGridY, CQuantity::SI_UNIT, UnitNode().Unit()));
    }
  }
}

BEGIN_MESSAGE_MAP(CAttriHexaMainMeshRegionDlg, CAttributesTemplate<CHexaMainMeshRegion>)
//{{AFX_MSG_MAP(CAttriHexaMainMeshRegionDlg)
ON_BN_CLICKED(IDC_ST_COLOR, OnColor)
ON_WM_PAINT()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttriHexaMeshRegionDlg message handlers

BOOL CAttriHexaMainMeshRegionDlg::OnInitDialog() {
  CAttributesTemplate<CHexaMainMeshRegion>::OnInitDialog();

  if ((static_cast<const CModelBase &>(Copy().Model())).BranchState().IsBranch()) {
    GetDlgItem(IDC_ED_GRID_X)->EnableWindow(FALSE);
    GetDlgItem(IDC_ED_GRID_Y)->EnableWindow(FALSE);
  }

  return FALSE;
}

void CAttriHexaMainMeshRegionDlg::OnColor() {
  Copy().Color(SelectColor(Copy().Color()));
  Invalidate();
}

// ##ModelId=3BE7AEC9019B
void CAttriHexaMainMeshRegionDlg::OnPaint() {
  CPaintDC dc(this); // device context for painting
  FillFrame(IDC_ST_COLOR, Copy().Color(), dc);
}

/////////////////////////////////////////////////////////////////////////////
// CAttriHexaSubMeshRegionDlg dialog
/////////////////////////////////////////////////////////////////////////////

CAttriHexaSubMeshRegionDlg::CAttriHexaSubMeshRegionDlg(CHexaSubMeshRegion &region, CWnd *pParent /*=NULL*/)
    : CAttributesTemplate<CHexaSubMeshRegion>(CAttriHexaSubMeshRegionDlg::IDD, region, pParent),
      m_model(region.Model()) {
  //{{AFX_DATA_INIT(CAttriHexaSubMeshRegionDlg)
  //}}AFX_DATA_INIT
}

void CAttriHexaSubMeshRegionDlg::DoDataExchange(CDataExchange *pDX) {
  CAttributesTemplate<CHexaSubMeshRegion>::DoDataExchange(pDX);

  //	int nGridX, nGridY;
  CString sName, sUnit;
  CLengthQuantity l;
  sUnit = CString(l.UnitName(UnitNode().Unit()).c_str());

  DDX_Text(pDX, IDC_ST_UNIT_I, sUnit);
  DDX_Text(pDX, IDC_ST_UNIT_II, sUnit);

  //{{AFX_DATA_MAP(CAttriHexaSubMeshRegionDlg)
  DDX_Control(pDX, IDC_CB_GRID_X, m_cbExpX);
  DDX_Control(pDX, IDC_CB_GRID_Y, m_cbExpY);
  //}}AFX_DATA_MAP
  DDX_Control(pDX, IDC_LC_FORMATION, m_lcFormation);

  if (!pDX->m_bSaveAndValidate) {
    sName = Copy().Name().toStdString().c_str();
    OnUpdateCombo();
  }

  DDX_Text(pDX, IDC_ED_NAME, sName);

  // Save the values.
  if (pDX->m_bSaveAndValidate) {
    Copy().Name((LPCSTR)sName);
    if ((m_cbExpX.GetCurSel() > -1) && (m_cbExpY.GetCurSel() > -1))
      Copy().SetSubGrid(m_cbExpX.GetCurSel(), m_cbExpY.GetCurSel());
  }
}

void CAttriHexaSubMeshRegionDlg::OnUpdateCombo() {
  // Clear combo's first
  m_cbExpX.Clear();
  m_cbExpY.Clear();

  // Put in new shit
  for (int i = 0; i < 10; i++) {
    CLengthQuantity l;
    // Fill the refinement factors
    CString strX, strY;
    strX.Format("%.2f",
                l.Convert(Copy().Main().GridSizeX() / pow((double)2, i), UnitNode().Unit(), CQuantity::SI_UNIT));
    strY.Format("%.2f",
                l.Convert(Copy().Main().GridSizeY() / pow((double)2, i), UnitNode().Unit(), CQuantity::SI_UNIT));
    m_cbExpX.AddString(strX);
    m_cbExpY.AddString(strY);
  }

  // set it to the first refinement.
  if (Copy().ExpX() == -1) {
    assert(Copy().ExpY() == -1);
    m_cbExpX.SetCurSel(1);
    m_cbExpY.SetCurSel(1);
  } else {
    assert(Copy().ExpX() >= 0 && Copy().ExpX() < 10);
    assert(Copy().ExpY() >= 0 && Copy().ExpY() < 10);
    m_cbExpX.SetCurSel(Copy().ExpX());
    m_cbExpY.SetCurSel(Copy().ExpY());
  }
}

BEGIN_MESSAGE_MAP(CAttriHexaSubMeshRegionDlg, CAttributesTemplate<CHexaSubMeshRegion>)
//{{AFX_MSG_MAP(CAttriHexaSubMeshRegionDlg)
ON_BN_CLICKED(IDC_ST_COLOR, OnColor)
ON_WM_PAINT()
//}}AFX_MSG_MAP
ON_NOTIFY(LVN_ITEMCHANGED, IDC_LC_FORMATION, &CAttriHexaSubMeshRegionDlg::OnLvnItemchangedLcFormation)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttriHexaMeshRegionDlg message handlers

BOOL CAttriHexaSubMeshRegionDlg::OnInitDialog() {
  CAttributesTemplate<CHexaSubMeshRegion>::OnInitDialog();

  if ((static_cast<const CModelBase &>(Copy().Model())).BranchState().IsBranch()) {
    GetDlgItem(IDC_CB_GRID_X)->EnableWindow(FALSE);
    GetDlgItem(IDC_CB_GRID_Y)->EnableWindow(FALSE);
  }

  CRect rect;
  m_lcFormation.GetWindowRect(&rect);

  m_lcFormation.InsertColumn(0, _T("Formation"), LVCFMT_LEFT, (rect.Width() - 3) * 0.7, 0);
  m_lcFormation.InsertColumn(1, _T("#Elements"), LVCFMT_LEFT, (rect.Width() - 3) * 0.3, 1);

  std::vector<CHexaFormation *> vcSortedFormations;
  CHexaFormationEntry *pEntry = (CHexaFormationEntry *)m_model.GraphEntry(MD_HEXA_FORMATION);
  CHexaFormationEntry::TNodeSet stFormation = pEntry->EntryNodes();
  CHexaFormationEntry::TNodeSet::iterator it;
  if (!stFormation.empty()) {
    CHexaFormation *pForm = *stFormation.begin();
    while (pForm->UpperFormation())
      pForm = const_cast<CHexaFormation *>(pForm->UpperFormation());
    assert(pForm);
    while (pForm) {
      vcSortedFormations.push_back(pForm);
      pForm = const_cast<CHexaFormation *>(pForm->LowerFormation());
    }

    for (size_t i = 0; i < vcSortedFormations.size(); i++) {
      new CFormationListObject(m_lcFormation, *vcSortedFormations[i], Copy().Formation(*vcSortedFormations[i]));
    }
  }

  return TRUE;
}

void CAttriHexaSubMeshRegionDlg::OnColor() {
  Copy().Color(SelectColor(Copy().Color()));
  Invalidate();
}

// ##ModelId=3BE7AEC9019B
void CAttriHexaSubMeshRegionDlg::OnPaint() {
  CPaintDC dc(this); // device context for painting
  FillFrame(IDC_ST_COLOR, Copy().Color(), dc);
}

void CAttriHexaSubMeshRegionDlg::OnOK() {
  BOOL bModified = FALSE;
  Copy().ClearFormations();

  for (int i = 0; i < m_lcFormation.GetItemCount(); i++) {
    CFormationListObject *pObject = (CFormationListObject *)(m_lcFormation.GetItemData(i));
    assert(pObject);
    CMeshDepthObject *pSubObject = (CMeshDepthObject *)(pObject->SubObject(1));
    // if (pObject->Modified())
    // bModified = TRUE;

    // pObject->Apply();

    if (Copy().Formation(pObject->Formation()) != pSubObject->Elements()) {
      Copy().Formation(pObject->Formation(), pSubObject->Elements());
      bModified = TRUE;
    }
  }

  if (bModified)
    ((CHexaModel &)m_model).InvalidateMesh();

  CAttributesTemplate<CHexaSubMeshRegion>::OnOK();
}

void CAttriHexaSubMeshRegionDlg::OnLvnItemchangedLcFormation(NMHDR *pNMHDR, LRESULT *pResult) {
  LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
  // TODO: Add your control notification handler code here
  *pResult = 0;
}

CAttriHexaSubMeshRegionDlg::CFormationListObject::CFormationListObject(CListCtrl &ctrl, CHexaFormation &formation,
                                                                       int elements)
    : IListObject(ctrl), m_formation(formation) {
  Ctrl().SetItemData(Ctrl().GetItemCount() - 1, (DWORD_PTR)this);
  new CMeshDepthObject(*this, elements);
}

QString CAttriHexaSubMeshRegionDlg::CFormationListObject::Text() const { return m_formation.Name(); }

unsigned int CAttriHexaSubMeshRegionDlg::CFormationListObject::Icon() const { return m_formation.IconId(); }

const CHexaFormation &CAttriHexaSubMeshRegionDlg::CFormationListObject::Formation() const { return m_formation; }

CHexaFormation &CAttriHexaSubMeshRegionDlg::CFormationListObject::Formation() { return m_formation; }

void CAttriHexaSubMeshRegionDlg::CFormationListObject::Apply() {
  CMeshDepthObject *pObject = (CMeshDepthObject *)(SubObject(1));
  Formation().Elements(pObject->Elements());
}

BOOL CAttriHexaSubMeshRegionDlg::CFormationListObject::Modified() const {
  CMeshDepthObject *pObject = (CMeshDepthObject *)(SubObject(1));
  return Formation().Elements() != pObject->Elements();
}

BOOL CAttriHexaSubMeshRegionDlg::CFormationListObject::operator<(const ICtrlObjectBase &object) const {
  const CFormationListObject *pObject = (const CFormationListObject *)(&object);
  return pObject->Formation().Less(Formation());
}

CAttriHexaSubMeshRegionDlg::CMeshDepthObject::CMeshDepthObject(CFormationListObject &list_object, int elements)
    : ISubListObject(list_object, 1) {
  m_nElement = elements < 0 ? list_object.Formation().Elements() : elements;
}

int CAttriHexaSubMeshRegionDlg::CMeshDepthObject::Elements() const { return m_nElement; }

QString CAttriHexaSubMeshRegionDlg::CMeshDepthObject::Text() const {
  QString sRet;
  sRet = QString("%1").arg(m_nElement);
  return sRet;
}

BOOL CAttriHexaSubMeshRegionDlg::CMeshDepthObject::CanEditText() const {
  return !(static_cast<const CModelBase &>(
               (static_cast<const CFormationListObject &>(ListObject()).Formation().Model())))
              .BranchState()
              .IsBranch();
}

BOOL CAttriHexaSubMeshRegionDlg::CMeshDepthObject::EditText(const QString &strText) {
  if (!IsInteger(strText.toStdString().c_str())) {
    _m()->msg("Mesh Depth is not an integer");
    return FALSE;
  }

  if (0 > atoi(strText.toStdString().c_str())) {
    _m()->msg("Mesh depth > 0");
    return FALSE;
  }

  m_nElement = atoi(strText.toStdString().c_str());

  return TRUE;
}

BOOL CAttriHexaSubMeshRegionDlg::CMeshDepthObject::IsInteger(const CString &sString) const {
  // Integer
  for (int i = 0; i < sString.GetLength(); i++) {
    if (!isdigit(sString[i]))
      return FALSE;
  }

  return TRUE;
}
