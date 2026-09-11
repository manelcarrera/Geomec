#include "CalibSettingsDlg.h"
#include "resource.h"
#include "stdafx.h"

CCalibSettingsDlg::CCalibSettingsDlg(double dConvCrit, int iMaxNumIter, CWnd *pParent)
    : CDialog(IDD_CALIBSETTINGS, pParent), m_dConvCrit(dConvCrit), m_iMaxNumIter(iMaxNumIter) {}

double CCalibSettingsDlg::ConvergenceCriterion() const { return m_dConvCrit; }

int CCalibSettingsDlg::MaxNumIterations() const { return m_iMaxNumIter; }

void CCalibSettingsDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_ED_CONVCRIT, m_dConvCrit);
  DDX_Text(pDX, IDC_ED_MAXNUMITER, m_iMaxNumIter);
}
