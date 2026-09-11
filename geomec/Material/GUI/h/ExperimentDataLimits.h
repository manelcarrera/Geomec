#pragma once

class CTabExperiment;

// CExperimentDataLimits dialog

class CExperimentDataLimits : public CDialog
{
  DECLARE_DYNAMIC(CExperimentDataLimits)

public:
  CExperimentDataLimits
  ( CExperimentArray &experimentData
  , CTabExperiment *tabExperiment = NULL
  , CWnd* pParent = NULL
  );
  virtual ~CExperimentDataLimits();

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CExperimentDataLimits)
  //}}AFX_VIRTUAL

// Dialog Data
  enum { IDD = IDD_EXPERIMENTDATA_LIMITS };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  //{{AFX_MSG(CExperimentDataLimits)
  afx_msg void OnSliderReleased(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg void CExperimentDataLimits::OnSelchangeExperiments();
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()

private:
  int m_iLowerLimit;
  int m_iUpperLimit;
  const int m_iLowerRange;
  const int m_iUpperRange;
  CExperimentArray &m_vcExperimentData;
  CTabExperiment *m_pTabExperiment;
  int m_iExperimentDataIndex;
  std::vector <int> m_iInitialLowerLimit;
};
