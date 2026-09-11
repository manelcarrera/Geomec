#include "stdafx.h"
#include "GraphLimitsDlg.h"

#include <QString>


namespace {
  const QString ERROR_DEPTH_INVALID = "min depth is equal to or larger than max depth";
  const QString ERROR_VALUE_INVALID = "min value is equal to or larger than max value";
  const QString ERROR_RANGE_INVALID = "range too small to render";
};


BEGIN_MESSAGE_MAP(CGraphLimitsDlg, CDialog)
  //{{AFX_MSG_MAP(CGraphLimitsDlg)
  ON_BN_CLICKED(IDC_CHK_AUTOMATIC_SCALING_LEFT, OnChkAutoLeft)
  ON_BN_CLICKED(IDC_CHK_AUTOMATIC_SCALING_TOP, OnChkAutoTop)
  ON_BN_CLICKED(IDC_CHK_AUTOMATIC_SCALING_BOTTOM, OnChkAutoBottom)
  // waij tfs 90989
  ON_EN_SETFOCUS(IDC_SHALLOW_DEPTH, OnSetFocusEditLeft)
  ON_EN_SETFOCUS(IDC_DEEP_DEPTH, OnSetFocusEditLeft)
  ON_EN_KILLFOCUS(IDC_SHALLOW_DEPTH, OnKillFocusEditLeft)
  ON_EN_KILLFOCUS(IDC_DEEP_DEPTH, OnKillFocusEditLeft)
  ON_EN_SETFOCUS(IDC_LOW_VALUE_TOP, OnSetFocusEditTop)
  ON_EN_SETFOCUS(IDC_HIGH_VALUE_TOP, OnSetFocusEditTop)
  ON_EN_SETFOCUS(IDC_LOW_VALUE_BOTTOM, OnSetFocusEditBottom)
  ON_EN_SETFOCUS(IDC_HIGH_VALUE_BOTTOM, OnSetFocusEditBottom)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


CGraphLimitsDlg::CGraphLimitsDlg(OIV2DRangeInterface& rangeInterface, CWnd* pParent /*=NULL*/)
  : CDialog(CGraphLimitsDlg::IDD, pParent)
  , m_rangeInterface(rangeInterface)
  , m_iAutoLeft(true)
  , m_iAutoTop(true)
  , m_iAutoBottom(true)
{
  //{{AFX_DATA_INIT(CGraphLimitsDlg)
  //}}AFX_DATA_INIT
}

BOOL CGraphLimitsDlg::OnInitDialog()
{
  CDialog::OnInitDialog();
  if (!m_rangeInterface.HaveAxis(OIV2DRangeInterface::AXIS_TOP))
  {
  GetDlgItem(IDC_LOW_VALUE_TOP)->EnableWindow(false);
  GetDlgItem(IDC_HIGH_VALUE_TOP)->EnableWindow(false);
  GetDlgItem(IDC_CHK_AUTOMATIC_SCALING_TOP)->EnableWindow(false);
  GetDlgItem(IDC_TICKMARKS_TOP)->EnableWindow(false);
  }
  if (!m_rangeInterface.HaveAxis(OIV2DRangeInterface::AXIS_BOTTOM))
  {
  GetDlgItem(IDC_LOW_VALUE_BOTTOM)->EnableWindow(false);
  GetDlgItem(IDC_HIGH_VALUE_BOTTOM)->EnableWindow(false);
  GetDlgItem(IDC_CHK_AUTOMATIC_SCALING_BOTTOM)->EnableWindow(false);
  GetDlgItem(IDC_TICKMARKS_BOTTOM)->EnableWindow(false);
  }

  GetDlgItem(IDOK)->SetFocus();

  return FALSE;
}

void CGraphLimitsDlg::DoDataExchange(CDataExchange* pDX)
{
  float dMinLeft; //wjrx mantis 2559
  float dMaxLeft;
  float dMinTop;
  float dMaxTop;
  float dMinBottom;
  float dMaxBottom;
  unsigned int iTickLeft;
  unsigned int iTickTop;
  unsigned int iTickBottom;

  CDialog::DoDataExchange(pDX);

  if (!pDX->m_bSaveAndValidate)
  {
  m_iAutoLeft = m_rangeInterface.Auto(OIV2DRangeInterface::AXIS_LEFT);
  m_rangeInterface.GetRangeMinMax(OIV2DRangeInterface::AXIS_LEFT, dMinLeft, dMaxLeft);
  iTickLeft = m_rangeInterface.Tick(OIV2DRangeInterface::AXIS_LEFT);

  if (m_rangeInterface.HaveAxis(OIV2DRangeInterface::AXIS_TOP))
  {
      m_iAutoTop = m_rangeInterface.Auto(OIV2DRangeInterface::AXIS_TOP);
      m_rangeInterface.GetRangeMinMax(OIV2DRangeInterface::AXIS_TOP, dMinTop, dMaxTop);
      iTickTop = m_rangeInterface.Tick(OIV2DRangeInterface::AXIS_TOP);
  }

  if (m_rangeInterface.HaveAxis(OIV2DRangeInterface::AXIS_BOTTOM))
  {
      m_iAutoBottom = m_rangeInterface.Auto(OIV2DRangeInterface::AXIS_BOTTOM);
      m_rangeInterface.GetRangeMinMax(OIV2DRangeInterface::AXIS_BOTTOM, dMinBottom, dMaxBottom);
      iTickBottom = m_rangeInterface.Tick(OIV2DRangeInterface::AXIS_BOTTOM);
  }
  }

  //{{AFX_DATA_MAP(CGraphLimitsDlg)
  DDX_Text(pDX, IDC_SHALLOW_DEPTH, dMinLeft);
  DDX_Text(pDX, IDC_DEEP_DEPTH, dMaxLeft); //deepest depth
  DDX_Text(pDX, IDC_TICKMARKS_LEFT, iTickLeft); // vertical: depth

  if (m_rangeInterface.HaveAxis(OIV2DRangeInterface::AXIS_TOP))
  {
  DDX_Check(pDX, IDC_CHK_AUTOMATIC_SCALING_TOP, m_iAutoTop);
  DDX_Text(pDX, IDC_LOW_VALUE_TOP, dMinTop);
  DDX_Text(pDX, IDC_HIGH_VALUE_TOP, dMaxTop);
  DDX_Text(pDX, IDC_TICKMARKS_TOP, iTickTop); // horizontal top: value
  }

  if (m_rangeInterface.HaveAxis(OIV2DRangeInterface::AXIS_BOTTOM))
  {
  DDX_Check(pDX, IDC_CHK_AUTOMATIC_SCALING_BOTTOM, m_iAutoBottom);
  DDX_Text(pDX, IDC_LOW_VALUE_BOTTOM, dMinBottom);
  DDX_Text(pDX, IDC_HIGH_VALUE_BOTTOM, dMaxBottom);
  DDX_Text(pDX, IDC_TICKMARKS_BOTTOM, iTickBottom); // horizontal bottom: value
  }

  DDX_Check(pDX, IDC_CHK_AUTOMATIC_SCALING_LEFT, m_iAutoLeft);

  //}}AFX_DATA_MAP

  if (pDX->m_bSaveAndValidate)
  {
  QString error;

  if (dMinLeft >= dMaxLeft)
  {
      error += "Left axis: " + ERROR_DEPTH_INVALID;
  }
  else if (!m_iAutoLeft && !m_rangeInterface.RangeLargeEnough(OIV2DRangeInterface::AXIS_LEFT, dMinLeft, dMaxLeft))
  {
      error += "Left axis: " + ERROR_RANGE_INVALID;
  }

  if (m_rangeInterface.HaveAxis(OIV2DRangeInterface::AXIS_TOP))
  {
      if (dMinTop >= dMaxTop)
      {
    if (!error.isEmpty())
          error += "\r\n";
    error += "Top axis: " + ERROR_VALUE_INVALID;
      }
      else if (!m_iAutoTop && !m_rangeInterface.RangeLargeEnough(OIV2DRangeInterface::AXIS_TOP, dMinTop, dMaxTop))
      {
    if (!error.isEmpty())
          error += "\r\n";
    error += "Top axis: " + ERROR_RANGE_INVALID;
      }
  }

  if (m_rangeInterface.HaveAxis(OIV2DRangeInterface::AXIS_BOTTOM))
  {
      if (dMinBottom >= dMaxBottom)
      {
    if (!error.isEmpty())
          error += "\r\n";
    error += "Bottom axis: " + ERROR_VALUE_INVALID;
      }
      else if (!m_iAutoBottom && !m_rangeInterface.RangeLargeEnough(OIV2DRangeInterface::AXIS_BOTTOM, dMinBottom, dMaxBottom))
      {
    if (!error.isEmpty())
          error += "\r\n";
    error += "Bottom axis: " + ERROR_RANGE_INVALID;
      }
  }

  if (!error.isEmpty())
  {
      _m()->msg(error);
      pDX->Fail();
  }

  m_rangeInterface.SetMinMax(OIV2DRangeInterface::AXIS_LEFT, dMinLeft, dMaxLeft);
  m_rangeInterface.Auto(OIV2DRangeInterface::AXIS_LEFT, m_iAutoLeft);
  // wjrx mantis 3383
  //
  if (iTickLeft < 2) iTickLeft = 2;
  m_rangeInterface.Tick(OIV2DRangeInterface::AXIS_LEFT, iTickLeft);

  if (m_rangeInterface.HaveAxis(OIV2DRangeInterface::AXIS_TOP))
  {
      m_rangeInterface.SetMinMax(OIV2DRangeInterface::AXIS_TOP, dMinTop, dMaxTop);
      m_rangeInterface.Auto(OIV2DRangeInterface::AXIS_TOP, m_iAutoTop);
      if (iTickTop < 2) iTickTop = 2;
      m_rangeInterface.Tick(OIV2DRangeInterface::AXIS_TOP, iTickTop);
  }

  if (m_rangeInterface.HaveAxis(OIV2DRangeInterface::AXIS_BOTTOM))
  {
      m_rangeInterface.SetMinMax(OIV2DRangeInterface::AXIS_BOTTOM, dMinBottom, dMaxBottom);
      m_rangeInterface.Auto(OIV2DRangeInterface::AXIS_BOTTOM, m_iAutoBottom);
      if (iTickBottom < 2) iTickBottom = 2;
      m_rangeInterface.Tick(OIV2DRangeInterface::AXIS_BOTTOM, iTickBottom);
  }

  m_rangeInterface.UpdateGraph();
  }
  else
  {
  // apply correct formatting
  DisplayLeft(dMinLeft, dMaxLeft);
  if (m_rangeInterface.HaveAxis(OIV2DRangeInterface::AXIS_TOP))
      DisplayTop(dMinTop, dMaxTop);
  if (m_rangeInterface.HaveAxis(OIV2DRangeInterface::AXIS_BOTTOM))
      DisplayBottom(dMinBottom, dMaxBottom);
  }
}

/// \brief Perform actions when check box Automatic for X changes
/// When the check box is 'on', recalculate the extreme values for Depth (=X).
/// When the check box for Value (=Y) is on, also recalculate the extreme
/// values for Y.
//
void CGraphLimitsDlg::OnChkAutoLeft()
{
  m_iAutoLeft = !m_iAutoLeft;
  if (m_iAutoLeft)
  {
  FindExtremesLeft();
  // Extreme Y values are calculated with respect to the 
  // current X values.
  if (m_iAutoTop)
      FindExtremesTop();

  if (m_iAutoBottom)
      FindExtremesBottom();
  }
}

/// \brief Perform actions when check box Automatic for Y changes
/// When the check box is 'on', recalculate the extreme values for Value (=Y).
//
void CGraphLimitsDlg::OnChkAutoTop()
{
  m_iAutoTop = !m_iAutoTop;
  if (m_iAutoTop)
  FindExtremesTop();
}

void CGraphLimitsDlg::OnChkAutoBottom()
{
  m_iAutoBottom = !m_iAutoBottom;
  if (m_iAutoBottom)
  FindExtremesBottom();
}

/// \brief Perform actions when edit box for X gets focus
/// Check box Automatic is switched off.
//
void CGraphLimitsDlg::OnSetFocusEditLeft()
{
  m_iAutoLeft = false;
  ((CButton*)GetDlgItem(IDC_CHK_AUTOMATIC_SCALING_LEFT))->SetCheck(false);
}

/// \brief Perform actions when edit box for Y loses focus
/// When the check box for Value (=Y) is on, recalculate the extreme
/// values for Y.
/// Extreme Y values are calculated with respect to the current X values, and
/// the X value has just been changed.
//
void CGraphLimitsDlg::OnKillFocusEditLeft()
{
  // Extreme Y values are calculated with respect to the 
  // current X values.
  if (m_iAutoTop)
  FindExtremesTop();
}

/// \brief Perform actions when edit box for Y gets focus
/// Check box Automatic is switched off.
void CGraphLimitsDlg::OnSetFocusEditTop()
{
  m_iAutoTop = false;
  ((CButton*)GetDlgItem(IDC_CHK_AUTOMATIC_SCALING_TOP))->SetCheck(false);
}

void CGraphLimitsDlg::OnSetFocusEditBottom()
{
  m_iAutoBottom = false;
  ((CButton*)GetDlgItem(IDC_CHK_AUTOMATIC_SCALING_BOTTOM))->SetCheck(false);
}

/// \brief format and display the two X-values.
void CGraphLimitsDlg::DisplayLeft(float MinLeft, float MaxLeft)
{
  char number[100];
  sprintf(number, "%.2f", MinLeft);
  ((CEdit*)GetDlgItem(IDC_SHALLOW_DEPTH))->SetWindowText(number);
  sprintf(number, "%.2f", MaxLeft);
  ((CEdit*)GetDlgItem(IDC_DEEP_DEPTH))->SetWindowText(number);
}

/// \brief Format and display the two X-values.
void CGraphLimitsDlg::DisplayTop(float MinTop, float MaxTop)
{
  // wjrx mantis 2643 .2 => .3
  char number[100];
  sprintf(number, "%.3g", MinTop);
  ((CEdit*)GetDlgItem(IDC_LOW_VALUE_TOP))->SetWindowText(number);
  sprintf(number, "%.3g", MaxTop);
  ((CEdit*)GetDlgItem(IDC_HIGH_VALUE_TOP))->SetWindowText(number);
}

void CGraphLimitsDlg::DisplayBottom(float MinBottom, float MaxBottom)
{
  // wjrx mantis 2643 .2 => .3
  char number[100];
  sprintf(number, "%.3g", MinBottom);
  ((CEdit*)GetDlgItem(IDC_LOW_VALUE_BOTTOM))->SetWindowText(number);
  sprintf(number, "%.3g", MaxBottom);
  ((CEdit*)GetDlgItem(IDC_HIGH_VALUE_BOTTOM))->SetWindowText(number);
}

/// \brief Find the extreme X-values
void CGraphLimitsDlg::FindExtremesLeft()
{
  m_rangeInterface.ResetMinMax(OIV2DRangeInterface::AXIS_LEFT);
  m_rangeInterface.UpdateGraph();

  float min, max;
  m_rangeInterface.GetRangeMinMax(OIV2DRangeInterface::AXIS_LEFT, min, max);
  DisplayLeft(min, max);

  // Extreme Y values are calculated with respect to the 
  // current X values.
  if (m_iAutoTop)
  FindExtremesTop();

  if (m_iAutoBottom)
  FindExtremesBottom();
}

/// \brief Find the extreme Y-values
void CGraphLimitsDlg::FindExtremesTop()
{
  if (m_rangeInterface.HaveAxis(OIV2DRangeInterface::AXIS_TOP))
  {
  m_rangeInterface.Auto(OIV2DRangeInterface::AXIS_TOP, m_iAutoTop);
  m_rangeInterface.ResetMinMax(OIV2DRangeInterface::AXIS_TOP);
  m_rangeInterface.UpdateGraph();

  float min, max;
  m_rangeInterface.GetRangeMinMax(OIV2DRangeInterface::AXIS_TOP, min, max);
  DisplayTop(min, max);
  }
}

void CGraphLimitsDlg::FindExtremesBottom()
{
  if (m_rangeInterface.HaveAxis(OIV2DRangeInterface::AXIS_BOTTOM))
  {
  m_rangeInterface.Auto(OIV2DRangeInterface::AXIS_BOTTOM, m_iAutoBottom);
  m_rangeInterface.ResetMinMax(OIV2DRangeInterface::AXIS_BOTTOM);
  m_rangeInterface.UpdateGraph();

  float min, max;
  m_rangeInterface.GetRangeMinMax(OIV2DRangeInterface::AXIS_BOTTOM, min, max);
  DisplayBottom(min, max);
  }
}

