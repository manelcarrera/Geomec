// wjrx mantis 3186
//
#include "Geomec.h"
#include "stdafx.h"
// #include "resource.h"
#include "LengthQuantity.h"
#include "PointSet.h"
#include "SampledPointSetDlg.h"

// CSampledPointSetDlg dialog

IMPLEMENT_DYNAMIC(CSampledPointSetDlg, CDialog)

CSampledPointSetDlg::CSampledPointSetDlg(const CPointSet &pointSet, const CUnitNode::TUnitType unit,
                                         CWnd *pParent /*= NULL*/)
    : CDialog(CSampledPointSetDlg::IDD, pParent), m_pointSet(pointSet), m_unit(unit), m_dSizeNorth(100),
      m_dSizeEast(100), m_dSizeDepth(20), m_sName("_sampled") {
  CLengthQuantity q;

  // for GUI display
  m_dSizeNorth = q.Convert(m_dSizeNorth, m_unit, IQuantityDouble::SI_UNIT);
  m_dSizeEast = q.Convert(m_dSizeEast, m_unit, IQuantityDouble::SI_UNIT);
  m_dSizeDepth = q.Convert(m_dSizeDepth, m_unit, IQuantityDouble::SI_UNIT);
}

CSampledPointSetDlg::~CSampledPointSetDlg() {}

void CSampledPointSetDlg::DoDataExchange(CDataExchange *pDX) { CDialog::DoDataExchange(pDX); }

void CSampledPointSetDlg::OnNValueChange() {
  m_dSizeNorth = GetValue(IDC_SAMPLEDPOINTSET_SIZE_NORTH);
  EnableOK();
}

void CSampledPointSetDlg::OnEValueChange() {
  m_dSizeEast = GetValue(IDC_SAMPLEDPOINTSET_SIZE_EAST);
  EnableOK();
}

void CSampledPointSetDlg::OnDValueChange() {
  m_dSizeDepth = GetValue(IDC_SAMPLEDPOINTSET_SIZE_DEPTH);
  EnableOK();
}

double CSampledPointSetDlg::GetValue(int fieldId) {
  CEdit *edit = 0;
  CString field;

  edit = (CEdit *)GetDlgItem(fieldId);
  edit->GetWindowText(field);
  return atof(field);
}

void CSampledPointSetDlg::EnableOK() {
  if (m_sName.size() > 0 && m_dSizeNorth > 0 && m_dSizeEast > 0 &&
      (m_dSizeDepth > 0 || m_pointSet.Dimension() != CPointSet::DIM_3D))
    GetDlgItem(IDOK)->EnableWindow(TRUE);
  else
    GetDlgItem(IDOK)->EnableWindow(FALSE);
}

void CSampledPointSetDlg::OnNameChange() {
  CEdit *edit = 0;
  CString field;

  edit = (CEdit *)GetDlgItem(IDC_SAMPLEDPOINTSET_NAME);
  edit->GetWindowText(field);
  m_sName = field;

  EnableOK();
}

BOOL CSampledPointSetDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  CEdit *nameEdit = (CEdit *)GetDlgItem(IDC_SAMPLEDPOINTSET_NAME);
  CEdit *sizeNorthEdit = (CEdit *)GetDlgItem(IDC_SAMPLEDPOINTSET_SIZE_NORTH);
  CEdit *sizeEastEdit = (CEdit *)GetDlgItem(IDC_SAMPLEDPOINTSET_SIZE_EAST);
  CEdit *sizeDepthEdit = (CEdit *)GetDlgItem(IDC_SAMPLEDPOINTSET_SIZE_DEPTH);

  m_sName = m_pointSet.Name().toStdString() + m_sName;
  nameEdit->SetWindowText((LPCSTR)m_sName.c_str());
  CString strText;
  strText.Format("%.0f", m_dSizeNorth);
  sizeNorthEdit->SetWindowText(strText);
  strText.Format("%.0f", m_dSizeEast);
  sizeEastEdit->SetWindowText(strText);

  if (m_pointSet.Dimension() == CPointSet::DIM_3D) {
    strText.Format("%.0f", m_dSizeDepth);
    sizeDepthEdit->SetWindowText(strText);
  } else {
    strText = "";
    sizeDepthEdit->SetWindowText(strText);
    sizeDepthEdit->EnableWindow(FALSE);
  }

  CLengthQuantity q;
  GetDlgItem(IDC_BLOCK_EDGE_UNIT)->SetWindowText((LPCSTR)(q.UnitName(m_unit).c_str()));

  return TRUE;
}

double CSampledPointSetDlg::getSizeNorth() const {
  CLengthQuantity q;
  return q.Convert(m_dSizeNorth, IQuantityDouble::SI_UNIT, m_unit);
}

double CSampledPointSetDlg::getSizeEast() const {
  CLengthQuantity q;
  return q.Convert(m_dSizeEast, IQuantityDouble::SI_UNIT, m_unit);
}

double CSampledPointSetDlg::getSizeDepth() const {
  if (m_pointSet.Dimension() == CPointSet::DIM_3D) {
    CLengthQuantity q;
    return q.Convert(m_dSizeDepth, IQuantityDouble::SI_UNIT, m_unit);
  } else
    return 0.0;
}

CString CSampledPointSetDlg::getName() const { return (LPCSTR)m_sName.c_str(); }

BEGIN_MESSAGE_MAP(CSampledPointSetDlg, CDialog)
ON_EN_CHANGE(IDC_SAMPLEDPOINTSET_SIZE_NORTH, OnNValueChange)
ON_EN_CHANGE(IDC_SAMPLEDPOINTSET_SIZE_EAST, OnEValueChange)
ON_EN_CHANGE(IDC_SAMPLEDPOINTSET_SIZE_DEPTH, OnDValueChange)
ON_EN_CHANGE(IDC_SAMPLEDPOINTSET_NAME, OnNameChange)
END_MESSAGE_MAP()

// CSampledPointSetDlg message handlers
