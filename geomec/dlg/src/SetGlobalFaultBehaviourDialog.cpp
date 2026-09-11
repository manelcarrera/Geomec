#include "stdafx.h"

#include "afxdialogex.h"

#include "Geomec.h"
#include "SetGlobalFaultBehaviourDialog.h"
#include "GlobalUnitNode.h"
#include "ConvertModeType.h"

IMPLEMENT_DYNAMIC(CSetGlobalFaultBehaviourDialog, CDialogEx)

CSetGlobalFaultBehaviourDialog::CSetGlobalFaultBehaviourDialog(
  CFemAppModel* femAppModel, CWnd* pParent)
: CDialogEx(CSetGlobalFaultBehaviourDialog::IDD, pParent)
, m_setGlobalFaultBehaviour(femAppModel)
, m_editReferenceDepth(0)
, m_editReferencePressure(0)
, m_editGradient(0)
, m_firstTime(true)
, m_valueGlobalFaultBehaviour(0)
{
}

CSetGlobalFaultBehaviourDialog::~CSetGlobalFaultBehaviourDialog()
{
}

BOOL CSetGlobalFaultBehaviourDialog::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  enableControls();

  UpdateData(FALSE);

  return TRUE;
}

void CSetGlobalFaultBehaviourDialog::OnBnClickedRadioButton()
{
  enableControls();
}

// protected

void CSetGlobalFaultBehaviourDialog::DoDataExchange(CDataExchange* pDX)
{
  setUnitNames(pDX);

  CDialogEx::DoDataExchange(pDX);

  DDX_Control(pDX, IDC_RADIO_MAXIMUM, m_maximum);
  DDX_Control(pDX, IDC_RADIO_MINIMUM, m_minimum);
  DDX_Control(pDX, IDC_RADIO_GRADIENT, m_gradient);
  DDX_Control(pDX, IDC_RADIO_MAXGRADIENT, m_maximumGradient);

  DoDataExchangeModeType(pDX);
  DoDataExchangeGradient(pDX);
}

BEGIN_MESSAGE_MAP(CSetGlobalFaultBehaviourDialog, CDialogEx)
  ON_BN_CLICKED(IDC_RADIO_MAXIMUM,
  &CSetGlobalFaultBehaviourDialog::OnBnClickedRadioButton)
  ON_BN_CLICKED(IDC_RADIO_MINIMUM,
  &CSetGlobalFaultBehaviourDialog::OnBnClickedRadioButton)
  ON_BN_CLICKED(IDC_RADIO_GRADIENT,
  &CSetGlobalFaultBehaviourDialog::OnBnClickedRadioButton)
  ON_BN_CLICKED(IDC_RADIO_MAXGRADIENT,
  &CSetGlobalFaultBehaviourDialog::OnBnClickedRadioButton)
END_MESSAGE_MAP()

// private

void CSetGlobalFaultBehaviourDialog::OnOK()
{
  UpdateData(TRUE);

  if ((m_maximum.GetCheck() == BST_CHECKED) ||
  (m_minimum.GetCheck() == BST_CHECKED) ||
  (m_gradient.GetCheck() == BST_CHECKED) ||
  (m_maximumGradient.GetCheck() == BST_CHECKED))
  {
  CGlobalUnitNode globalUnitNode;
  CConvertModeType convertModeType;
  CFaultPressure::TModeType
      modeType = convertModeType(m_valueGlobalFaultBehaviour);
  std::vector <double> values;

  if (m_gradient.GetCheck() == BST_CHECKED)
  {
      values = getValues();
  }

  m_setGlobalFaultBehaviour.
      setGlobalFaultBehaviour(values, globalUnitNode.unitNode(), modeType);
  }

  CDialogEx::OnOK();
}

void CSetGlobalFaultBehaviourDialog::enableControls()
{
  bool canModifyPressure = m_setGlobalFaultBehaviour.canModifyPressure();

  // m_gradient is used because the pointer returned by GetDlgItem(...) was
  // not castable to a CButton pointer, see also
  // http://www.flounder.com/getdlgitem.htm

  bool isGradientChecked = (m_gradient.GetCheck() == BST_CHECKED);

  GetDlgItem(IDC_RADIO_MAXIMUM)->EnableWindow(canModifyPressure);
  GetDlgItem(IDC_RADIO_MINIMUM)->EnableWindow(canModifyPressure);
  
  m_gradient.EnableWindow(canModifyPressure);

  GetDlgItem(IDC_LBL_DEPTH)->
  EnableWindow(canModifyPressure && isGradientChecked);
  GetDlgItem(IDC_ED_DEPTH)->
  EnableWindow(canModifyPressure && isGradientChecked);
  GetDlgItem(IDC_UN_DEPTH)->
  EnableWindow(canModifyPressure && isGradientChecked);

  GetDlgItem(IDC_LBL_PRESSURE)->
  EnableWindow(canModifyPressure && isGradientChecked);
  GetDlgItem(IDC_ED_PRESSURE)->
  EnableWindow(canModifyPressure && isGradientChecked);
  GetDlgItem(IDC_UN_PRESSURE)->
  EnableWindow(canModifyPressure && isGradientChecked);

  GetDlgItem(IDC_LBL_GRADIENT)->
  EnableWindow(canModifyPressure && isGradientChecked);
  GetDlgItem(IDC_ED_GRADIENT)->
  EnableWindow(canModifyPressure && isGradientChecked);
  GetDlgItem(IDC_UN_GRADIENT)->
  EnableWindow(canModifyPressure && isGradientChecked);
}

namespace
{

CString QString2CString(const QString& q)
{
  return q.toStdString().c_str();
}

} // anonymous namespace

void CSetGlobalFaultBehaviourDialog::setUnitNames(CDataExchange* pDX) const
{
  CGlobalUnitNode globalUnitNode;
  std::vector <QString> unitNames =
  m_setGlobalFaultBehaviour.getUnitNames(globalUnitNode.unitNode());
  std::vector <CString> newUnitNames;

  newUnitNames.resize(unitNames.size());

  std::transform(unitNames.begin(), unitNames.end(), newUnitNames.begin(),
  QString2CString);

  DDX_Text(pDX, IDC_UN_DEPTH,
  newUnitNames[CSetGlobalFaultBehaviour::DEPTH]);
  DDX_Text(pDX, IDC_UN_PRESSURE,
  newUnitNames[CSetGlobalFaultBehaviour::PRESSURE]);
  DDX_Text(pDX, IDC_UN_GRADIENT,
  newUnitNames[CSetGlobalFaultBehaviour::GRADIENT]);
}

std::vector <double> CSetGlobalFaultBehaviourDialog::getValues() const
{
  std::vector <double> values;

  values.push_back(m_editReferenceDepth);
  values.push_back(m_editReferencePressure);
  values.push_back(m_editGradient);

  return values;
}

void CSetGlobalFaultBehaviourDialog::DoDataExchangeModeType(CDataExchange* pDX)
{
  CFaultPressure::TModeType modeType;

  if (m_setGlobalFaultBehaviour.getModeType(modeType))
  {
  if (m_firstTime)
  {
      m_firstTime = false;

      CConvertModeType convertModeType;

      m_valueGlobalFaultBehaviour = convertModeType(modeType);
  }
  }

  DDX_Radio(pDX, IDC_RADIO_MAXIMUM, m_valueGlobalFaultBehaviour);
}

void CSetGlobalFaultBehaviourDialog::DoDataExchangeGradient(CDataExchange* pDX)
{
  if (m_gradient.GetCheck() != BST_CHECKED)
  {
  CString empty;

  DDX_Text(pDX, IDC_ED_DEPTH, empty);
  DDX_Text(pDX, IDC_ED_PRESSURE, empty);
  DDX_Text(pDX, IDC_ED_GRADIENT, empty);
  }
  else
  {
  CGlobalUnitNode globalUnitNode;
  std::vector <double> values = getValues();

  if (m_setGlobalFaultBehaviour.getValues(values, globalUnitNode.unitNode()))
  {
      m_editReferenceDepth = values[CSetGlobalFaultBehaviour::DEPTH];
      m_editReferencePressure = values[CSetGlobalFaultBehaviour::PRESSURE];
      m_editGradient = values[CSetGlobalFaultBehaviour::GRADIENT];
  }

  DDX_Text(pDX, IDC_ED_DEPTH, m_editReferenceDepth);
  DDX_Text(pDX, IDC_ED_PRESSURE, m_editReferencePressure);
  DDX_Text(pDX, IDC_ED_GRADIENT, m_editGradient);
  }
}
