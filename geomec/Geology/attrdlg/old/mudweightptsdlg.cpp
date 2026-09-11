// wjrx mantis 3647 This class is not used!
//
// mudweightptsdlg.cpp : implementation file
//

#include "mudweightptsdlg.h"
#include "DsbXDlg.h"
#include "ResultRegister.h"
#include "StartDStaborDlg.h"
#include "geomec.h"
#include "modelbase.h"
#include "stdafx.h"
#include "wellpath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CMudWeightPtsDlg dialog

CMudWeightPtsDlg::CMudWeightPtsDlg(CWellPath *pWellPath, CWnd *pParent /*=NULL*/)
    : CDialog(CMudWeightPtsDlg::IDD, pParent), m_pWellPath(pWellPath) {

  //{{AFX_DATA_INIT(CMudWeightPtsDlg)
  m_dSingleValue = 0.0;
  m_dFrom = 0.0;
  m_dTo = 0.0;
  m_dNrOPts = 0.0;
  //}}AFX_DATA_INIT
}

const CUnitNode &CMudWeightPtsDlg::UnitNode() {
  CGeomecDoc &doc = ((CGeomecApp *)AfxGetApp())->GetDoc();
  return doc.UnitNode();
}

void CMudWeightPtsDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);

  CLengthQuantity qnLength;

  double dSingle = 0;
  double dFrom = 0;
  double dTo = 0;

  //{{AFX_DATA_MAP(CMudWeightPtsDlg)
  DDX_Text(pDX, IDC_EDIT_SINGLE, dSingle);
  DDX_Text(pDX, IDC_EDIT_RANGE_FROM, dFrom);
  DDX_Text(pDX, IDC_EDIT_RANGE_TO, dTo);
  DDX_Text(pDX, IDC_EDIT_RANGE_NROPTS, m_dNrOPts);
  //}}AFX_DATA_MAP

  m_dSingleValue = qnLength.Convert(dSingle, CQuantity::SI_UNIT, UnitNode().Unit());
  m_dFrom = qnLength.Convert(dFrom, CQuantity::SI_UNIT, UnitNode().Unit());
  m_dTo = qnLength.Convert(dTo, CQuantity::SI_UNIT, UnitNode().Unit());

  CString strUnit;

  if (!pDX->m_bSaveAndValidate) {
    switch (UnitNode().Unit()) {
    case CQuantity::SI_UNIT:
      strUnit.Format("m");
      break;
    case CQuantity::FIELD_UNIT:
      strUnit.Format("ft");
      break;
    }
  }

  CString name;
  name.Format(m_pWellPath->Name());

  DDX_Text(pDX, IDC_STATIC_WP_NAME, name);
  DDX_Text(pDX, IDC_STATIC_SINGLE_UNIT, strUnit);
  DDX_Text(pDX, IDC_ST_RNGE_FR_UNIT, strUnit);
  DDX_Text(pDX, IDC_ST_RNGE_TO_UNIT, strUnit);

  SetButtonState();
}

BEGIN_MESSAGE_MAP(CMudWeightPtsDlg, CDialog)
//{{AFX_MSG_MAP(CMudWeightPtsDlg)
ON_BN_CLICKED(IDC_RADIO_SINGLE_POINT, OnRadioSinglePoint)
ON_BN_CLICKED(IDC_RADIO_RANGE, OnRadioRange)
ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
ON_BN_CLICKED(IDC_BUTTON_REMOVE, OnButtonRemove)
ON_BN_CLICKED(IDC_BUTTON_RECALCDEF, OnButtonRecalcdef)
ON_BN_CLICKED(IDC_BUTTON_START_DSTABOR, OnButtonStartDStabor)
ON_BN_CLICKED(IDC_BUTTON_START_DSBBATCH, OnButtonStartDSbBatch)
ON_NOTIFY(NM_CLICK, IDC_LIST_DSBATCH_POINTS, OnClickListDsbatchPoints)
ON_NOTIFY(LVN_ITEMCHANGING, IDC_LIST_DSBATCH_POINTS, OnItemchangingListDsbatchPoints)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMudWeightPtsDlg message handlers

BOOL CMudWeightPtsDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  CRect rect;

  /*takes to long
  if(!(m_pWellPath->m_stDSBPoints.size() >0))
  {
    m_pWellPath->CalcDefDSBPoints(m_pWellPath->m_stDSBPoints);
    m_pWellPath->m_bDefaultOnly=true;
  }
  */

  m_stPts = m_pWellPath->m_stDSBPoints;
  m_bDefaultOnly = m_pWellPath->DefaultDSBOnly();

  m_pRadioSingle = (CButton *)GetDlgItem(IDC_RADIO_SINGLE_POINT);
  m_pRadioRange = (CButton *)GetDlgItem(IDC_RADIO_RANGE);
  m_pFrom = (CEdit *)GetDlgItem(IDC_EDIT_RANGE_FROM);
  m_pTo = (CEdit *)GetDlgItem(IDC_EDIT_RANGE_TO);
  m_pNrOPts = (CEdit *)GetDlgItem(IDC_EDIT_RANGE_NROPTS);
  m_pSinglePoint = (CEdit *)GetDlgItem(IDC_EDIT_SINGLE);

  m_pReset = (CButton *)GetDlgItem(IDC_BUTTON_RECALCDEF);
  if (m_pWellPath->DefaultDSBOnly())
    m_pReset->EnableWindow(FALSE);

  m_pRadioSingle->SetCheck(1);
  m_pFrom->EnableWindow(FALSE);
  m_pTo->EnableWindow(FALSE);
  m_pNrOPts->EnableWindow(FALSE);

  CListCtrl *pListCtrl;
  pListCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_DSBATCH_POINTS);
  pListCtrl->GetClientRect(&rect);
  double width = rect.right / 3;

  switch (UnitNode().Unit()) {
  case CQuantity::SI_UNIT: {
    pListCtrl->InsertColumn(0, "AHD (m)", LVCFMT_LEFT, width);
    pListCtrl->InsertColumn(1, "TVD (m)", LVCFMT_LEFT, width);
  } break;
  case CQuantity::FIELD_UNIT: {
    pListCtrl->InsertColumn(0, "AHD (ft)", LVCFMT_LEFT, width);
    pListCtrl->InsertColumn(1, "TVD (ft)", LVCFMT_LEFT, width);
  } break;
  }

  pListCtrl->InsertColumn(2, "Formation", LVCFMT_LEFT, rect.right - 2 * width);
  pListCtrl->SetExtendedStyle(LVS_EX_FULLROWSELECT);

  UpdateList();

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CMudWeightPtsDlg::OnRadioSinglePoint() {
  m_pFrom->EnableWindow(FALSE);
  m_pTo->EnableWindow(FALSE);
  m_pNrOPts->EnableWindow(FALSE);
  m_pSinglePoint->EnableWindow(TRUE);
}

void CMudWeightPtsDlg::OnRadioRange() {
  m_pFrom->EnableWindow(TRUE);
  m_pTo->EnableWindow(TRUE);
  m_pNrOPts->EnableWindow(TRUE);
  m_pSinglePoint->EnableWindow(FALSE);
}

void CMudWeightPtsDlg::OnButtonAdd() {
  CWaitCursor wait;

  UpdateData(TRUE);

  bool bOutOfBoundary = false;
  int beginsize = m_stPts.size();
  if (m_pRadioSingle->GetCheck()) {
    if (m_dSingleValue == 0) {
      AfxMessageBox("No zero allowed", 0, 0);
      return;
    }

    CGeoWellPoint pt(m_pWellPath->NewPoint(m_dSingleValue, CWellPath::AHD));
    if (pt.Empty())
      bOutOfBoundary = true;
    else {
      if (pt.Formation())
        m_stPts.insert(pt);
      else
        AfxMessageBox("No formation found for this point", 0, 0);
    }

  } else {

    if (m_dFrom == 0 || m_dTo == 0) {
      AfxMessageBox("No zero allowed", 0, 0);
      return;
    }
    double dInterval = (m_dTo - m_dFrom) / (double)(m_dNrOPts - 1);
    CGeoWellPoint ptFrom(m_pWellPath->NewPoint(m_dFrom, CWellPath::AHD));

    if (ptFrom.Empty() || !ptFrom.Formation())
      bOutOfBoundary = true;
    else
      m_stPts.insert(ptFrom);

    CGeoWellPoint ptTo(m_pWellPath->NewPoint(m_dTo, CWellPath::AHD));
    if (ptTo.Empty() || !ptTo.Formation())
      bOutOfBoundary = true;
    else
      m_stPts.insert(ptTo);

    for (int i = 1; i < m_dNrOPts - 1; i++) {
      CGeoWellPoint pt(m_pWellPath->NewPoint(m_dFrom + (dInterval * double(i)), CWellPath::AHD));
      if (pt.Empty() || !pt.Formation())
        bOutOfBoundary = true;
      else
        m_stPts.insert(pt);
    }
  }

  if (bOutOfBoundary)
    AfxMessageBox("Some points are outside the boundary\n and are not added", 0, 0);

  int endsize = m_stPts.size();
  if (beginsize != endsize)
    m_bDefaultOnly = false;

  UpdateList();
  m_pReset->EnableWindow(TRUE);
}

void CMudWeightPtsDlg::UpdateList() {
  CWaitCursor wait;

  CListCtrl *pListCtrl;
  pListCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_DSBATCH_POINTS);

  CLengthQuantity qnLength;

  CString strAHD;
  CString strTVD;
  CString strForm;
  std::vector<geo::IBody *> vcBody;
  double dAHD;
  double dTVD;

  CWellPath::TWellPointSet::iterator ite;
  int i = 0;
  int nIndex;

  pListCtrl->DeleteAllItems();
  m_mpIndexPoints.clear();
  for (ite = m_stPts.begin(); ite != m_stPts.end(); ite++) {
    if (ite->Empty())
      continue;

    dAHD = qnLength.Convert(ite->AlongHoleDepth(), UnitNode().Unit(), CQuantity::SI_UNIT);
    dTVD = qnLength.Convert(ite->Z(), UnitNode().Unit(), CQuantity::SI_UNIT);
    strAHD.Format("%f", dAHD);
    strTVD.Format("%f", dTVD);

    const CFormationBase *pFormation = ite->Formation();
    if (pFormation) {
      strForm.Format(pFormation->Name());
    } else {
      strForm.Format("No Formation found");
    }

    nIndex = pListCtrl->InsertItem(i, "");
    m_mpIndexPoints.insert(IndexPointPair(i, &(*ite)));

    pListCtrl->SetItemText(nIndex, 0, strAHD);
    pListCtrl->SetItemText(nIndex, 1, strTVD);
    pListCtrl->SetItemText(nIndex, 2, strForm);
    i++;
  }
}

void CMudWeightPtsDlg::OnButtonRemove() {
  CWaitCursor wait;

  CListCtrl *pListCtrl;
  pListCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_DSBATCH_POINTS);

  POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
  std::vector<int> vcIndex;
  std::vector<int>::iterator vcit;
  vcIndex.clear();

  int beginsize = m_stPts.size();
  while (pos) {
    int nItem = pListCtrl->GetNextSelectedItem(pos);
    vcIndex.push_back(nItem);
  }

  CGeoWellPoint *pTemp;
  CWellPath::TWellPointSet::iterator ite;
  for (vcit = vcIndex.begin(); vcit != vcIndex.end(); vcit++) {
    pListCtrl->DeleteItem(*vcit);
    pTemp = m_mpIndexPoints[*vcit];
    ite = m_stPts.find(*pTemp);
    m_stPts.erase(ite);
  }
  UpdateList();

  int endsize = m_stPts.size();
  if (beginsize != endsize)
    m_bDefaultOnly = false;

  if (vcIndex.size() > 0)
    m_pReset->EnableWindow(TRUE);

  SetButtonState();
}

void CMudWeightPtsDlg::OnCancel() { CDialog::OnCancel(); }

void CMudWeightPtsDlg::OnOK() {
  CWellPath::TWellPointSet::iterator it;

  // add new points to DsbPoints
  for (it = m_stPts.begin(); it != m_stPts.end(); it++) {
    m_pWellPath->AddDsbPoint(*it);
  }

  // look for points to remove from DsbPoints
  // and store the iterators
  std::vector<CWellPath::TWellPointSet::iterator> vecIt;
  for (it = m_pWellPath->m_stDSBPoints.begin(); it != m_pWellPath->m_stDSBPoints.end(); it++) {
    CWellPath::TWellPointSet::iterator find;
    find = m_stPts.find(*it);
    if (find == m_stPts.end())
      vecIt.push_back(it);
  }

  // remove the points with the stored iterators
  for (int i = 0; i < vecIt.size(); i++) {
    m_pWellPath->RemoveDsbPoint(*vecIt[i]);
  }

  m_pWellPath->m_bDefaultOnly = m_bDefaultOnly;
  CDialog::OnOK();
}

void CMudWeightPtsDlg::OnButtonRecalcdef() {
  CWaitCursor wait;
  m_pWellPath->CalcDefDSBPoints(m_stPts);
  UpdateList();
  m_bDefaultOnly = true;
  m_pReset->EnableWindow(FALSE);
}

void CMudWeightPtsDlg::OnButtonStartDSbBatch() {

  CModelBase *pModel = (CModelBase *)&m_pWellPath->Model();
  if (!pModel->ResultRegister().Linear()) {
    AfxMessageBox("First start a linear depletion analysis", 0, 0);
    return;
  }

  CListCtrl *pListCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_DSBATCH_POINTS);
  POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
  ASSERT(pos);

  std::vector<const CGeoWellPoint *> PointVec;
  while (pos) {
    int nItem = pListCtrl->GetNextSelectedItem(pos);
    CGeoWellPoint *pPoint = m_mpIndexPoints[nItem];
    PointVec.push_back(pPoint);
  }

  CStartDsbBatchDlg StartDlg(PointVec, pModel);
  StartDlg.DoModal();
}

void CMudWeightPtsDlg::OnButtonStartDStabor() {

  CModelBase *pModel = (CModelBase *)&m_pWellPath->Model();
  if (!pModel->ResultRegister().Linear()) {
    AfxMessageBox("First start a linear depletion analysis", 0, 0);
    return;
  }

  CListCtrl *pListCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_DSBATCH_POINTS);
  POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
  ASSERT(pos);
  int nItem = pListCtrl->GetNextSelectedItem(pos);
  CGeoWellPoint *pPoint = m_mpIndexPoints[nItem];

  CStartDStaborDlg StartDlg(pPoint, pModel);
  StartDlg.DoModal();
}

void CMudWeightPtsDlg::SetButtonState() {
  CButton *pRemoveButton = (CButton *)GetDlgItem(IDC_BUTTON_REMOVE);
  CButton *pButton = (CButton *)GetDlgItem(IDC_BUTTON_START_DSTABOR);
  CButton *pButtonBatch = (CButton *)GetDlgItem(IDC_BUTTON_START_DSBBATCH);
  CListCtrl *pListCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_DSBATCH_POINTS);
  POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
  if (pos == NULL) {
    // nothing is selected
    pRemoveButton->EnableWindow(FALSE);
    pButtonBatch->EnableWindow(FALSE);
    pButton->EnableWindow(FALSE);
    return;
  } else {
    pRemoveButton->EnableWindow(TRUE);

    pListCtrl->GetNextSelectedItem(pos);
    if (pos) {
      // more then one item is selected
      pButtonBatch->EnableWindow(TRUE);
      pButton->EnableWindow(FALSE);
    } else {
      // one item is selected
      pButtonBatch->EnableWindow(FALSE);
      pButton->EnableWindow(TRUE);
    }
  }
}

void CMudWeightPtsDlg::OnClickListDsbatchPoints(NMHDR *pNMHDR, LRESULT *pResult) {
  UpdateData(TRUE);
  *pResult = 0;
}

void CMudWeightPtsDlg::OnItemchangingListDsbatchPoints(NMHDR *pNMHDR, LRESULT *pResult) {
  NM_LISTVIEW *pNMListView = (NM_LISTVIEW *)pNMHDR;
  UpdateData(TRUE);
  *pResult = 0;
}
