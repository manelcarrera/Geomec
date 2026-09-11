// attriHexaFormation.cpp : implementation file
//

#include "attrihexaformation.h"
#include "HexaEntryTypes.h"
#include "HexaHorizon.h"
#include "geomec.h"
#include "stdafx.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CAttriHexaFormation dialog

// ##ModelId=3BE7AEC9018B
CAttriHexaFormation::CAttriHexaFormation(CHexaFormation &formation, CWnd *pParent /*=NULL*/)
    : CAttriFormationTemplate<CHexaFormation>(CAttriHexaFormation::IDD, formation, pParent),
      m_elementOrderSubDialog(this, Source()) {
  //{{AFX_DATA_INIT(CAttriHexaFormation)
  m_bApplyToAll = FALSE;
  //}}AFX_DATA_INIT
}

BOOL CAttriHexaFormation::OnInitDialog() {
  CAttriFormationTemplate<CHexaFormation>::OnInitDialog();

  // Set the buddycontrol of the spincontrol so we can use the up and down key at the keyboard.
  ((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_LAYER))->SetBuddy(GetDlgItem(IDC_ED_LAYER_THICKNESS));
  ((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_ELEMENT))->SetBuddy(GetDlgItem(IDC_ED_NR_OF_ELEMENTS));

  if ((static_cast<const CModelBase &>(Copy().Model())).BranchState().IsBranch()) {
    GetDlgItem(IDC_ED_LAYER_THICKNESS)->EnableWindow(FALSE);
    GetDlgItem(IDC_SPIN_LAYER)->EnableWindow(FALSE);
    GetDlgItem(IDC_ED_NR_OF_ELEMENTS)->EnableWindow(FALSE);
    GetDlgItem(IDC_SPIN_ELEMENT)->EnableWindow(FALSE);
    GetDlgItem(IDC_CK_APPLY_TO_ALL)->EnableWindow(FALSE);
  }

  m_elementOrderSubDialog.onInitDialog();

  return TRUE;
}

// ##ModelId=3BE7AEC9018E
void CAttriHexaFormation::DoDataExchange(CDataExchange *pDX) {
  CString strBottomName;
  CString strTopName;
  CString strUnit;
  double dMinThickness;
  int nElements;

  CAttriFormationTemplate<CHexaFormation>::DoDataExchange(pDX);

  if (!pDX->m_bSaveAndValidate) {
    strUnit = CString(Copy().MinThickness().UnitName(UnitNode().Unit()).c_str());
    dMinThickness = Copy().MinThickness().Value(UnitNode().Unit());
    nElements = Copy().Elements();
    strBottomName = Copy().LowerHorizon().Name().toStdString().c_str();
    strTopName = Copy().UpperHorizon().Name().toStdString().c_str();
  }

  //{{AFX_DATA_MAP(CAttriHexaFormation)
  DDX_Check(pDX, IDC_CK_APPLY_TO_ALL, m_bApplyToAll);
  //}}AFX_DATA_MAP

  DDX_Text(pDX, IDC_ST_BOTTOM_HORIZON, strBottomName);
  DDX_Text(pDX, IDC_ST_TOP_HORIZON, strTopName);
  DDX_Text(pDX, IDC_ED_LAYER_THICKNESS, dMinThickness);
  DDV_MinMaxDouble(pDX, dMinThickness, Copy().MinThickness().MinValue(UnitNode().Unit()),
                   Copy().MinThickness().MaxValue(UnitNode().Unit()));
  DDX_Text(pDX, IDC_ED_NR_OF_ELEMENTS, nElements);
  DDV_MinMaxInt(pDX, nElements, 1, 1000000);
  DDX_Text(pDX, IDC_ST_UNIT, strUnit);

  if (pDX->m_bSaveAndValidate) {
    Copy().Elements(nElements);
    Copy().MinThickness(dMinThickness, UnitNode().Unit());

    // Apply to all
    if (m_bApplyToAll) {
      // Loop through all the formations to set the data.
      CHexaFormationEntry *pEntry = (CHexaFormationEntry *)(Copy().Model().GraphEntry(MD_HEXA_FORMATION));
      assert(pEntry);
      CHexaFormationEntry::TNodeSet stFormation = pEntry->EntryNodes();
      for (CHexaFormationEntry::TNodeSet::iterator it = stFormation.begin(); it != stFormation.end(); it++) {
        if (!(&Copy().Source() == (*it))) {
          // Set the formation
          (*it)->Elements(nElements);
          (*it)->MinThickness(dMinThickness, UnitNode().Unit());
        }
      }
    }
  }

  m_elementOrderSubDialog.doDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAttriHexaFormation, CDialog)
//{{AFX_MSG_MAP(CAttriHexaFormation)
ON_BN_CLICKED(IDC_ST_COLOR, OnColor)
ON_WM_PAINT()
ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LAYER, OnDeltaposSpinLayer)
ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ELEMENT, OnDeltaposSpinElement)
//}}AFX_MSG_MAP
ON_BN_CLICKED(IDC_INHERITFROMMODEL, OnBnClickedInheritFromModel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttriHexaFormation message handlers

// ##ModelId=3BE7AEC90191
void CAttriHexaFormation::OnColor() {
  Copy().Color(SelectColor(Copy().Color()));
  Invalidate();
}

// ##ModelId=3BE7AEC9019B
void CAttriHexaFormation::OnPaint() {
  CPaintDC dc(this); // device context for painting
  FillFrame(IDC_ST_COLOR, Copy().Color(), dc);
}

// The spincontrol code for the number of layers.
void CAttriHexaFormation::OnDeltaposSpinLayer(NMHDR *pNMHDR, LRESULT *pResult) {
  NM_UPDOWN *pNMUpDown = (NM_UPDOWN *)pNMHDR;

  CString str;
  ((CEdit *)GetDlgItem(IDC_ED_LAYER_THICKNESS))->GetWindowText(str);
  double dMinThickness = atof(str);
  if (pNMUpDown->iDelta > 0)
    dMinThickness--;
  if (pNMUpDown->iDelta < 0)
    dMinThickness++;
  if (dMinThickness < 1)
    dMinThickness = 1;
  str.Format("%.f", dMinThickness);
  ((CEdit *)GetDlgItem(IDC_ED_LAYER_THICKNESS))->SetWindowText(str);

  *pResult = 0;
}

// The spincontrol code for the number of elements.
void CAttriHexaFormation::OnDeltaposSpinElement(NMHDR *pNMHDR, LRESULT *pResult) {
  NM_UPDOWN *pNMUpDown = (NM_UPDOWN *)pNMHDR;

  CString str;
  ((CEdit *)GetDlgItem(IDC_ED_NR_OF_ELEMENTS))->GetWindowText(str);
  int nElements = atoi(str);
  if (pNMUpDown->iDelta > 0)
    nElements--;
  if (pNMUpDown->iDelta < 0)
    nElements++;
  if (nElements < 1)
    nElements = 1;
  str.Format("%d", nElements);
  ((CEdit *)GetDlgItem(IDC_ED_NR_OF_ELEMENTS))->SetWindowText(str);

  *pResult = 0;
}

void CAttriHexaFormation::OnBnClickedInheritFromModel() { m_elementOrderSubDialog.onBnClickedInheritFromModel(); }

void CAttriHexaFormation::OnOK() {
  double original = Copy().MinThickness().Value(UnitNode().Unit());
  CString string;

  ((CEdit *)GetDlgItem(IDC_ED_LAYER_THICKNESS))->GetWindowText(string);

  double modified = atof(string);

  if (std::abs(original - modified) > 1E-8) {
    dynamic_cast<CModelBase &>(Source().Model()).InvalidateMesh();
    Source().clearThicknessCache();
  }

  m_elementOrderSubDialog.onOK();

  CAttriFormationTemplate<CHexaFormation>::OnOK();
}
