#if !defined(AFX_CALCULATIONPROPERTIES_H__F7020B63_F35B_431B_B215_444C0E3F8210__INCLUDED_)
#define AFX_CALCULATIONPROPERTIES_H__F7020B63_F35B_431B_B215_444C0E3F8210__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CalculationProperties.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCalculationProperties dialog
#include "ModelBase.h"

class CTempPath;

class CCalculationProperties : public CDialog
{
private:
  CModelBase &m_Model;
  CTempPath &m_OriginalTempPath;
  CTempPath *m_pTempPathCopy;
  int m_iElementOrder;
  BOOL m_bUseParallelSolver;
  int m_nSolverThreads;
  int m_iUseDecompactionParams;
  int m_iGenerateStreamFile;

  void UpdateMethodCheckBox();
  void UpdateParallelSolverControls();

public:
  CCalculationProperties(CModelBase &model, CTempPath &temppath, CWnd* pParent = NULL);   // standard constructor

  CTempPath &TempPath();

// Dialog Data
  //{{AFX_DATA(CCalculationProperties)
  enum { IDD = IDD_CAL_PROPERTIES };
  BOOL	m_bUseMethod;
  BOOL	m_bSaltCheck;
  BOOL m_bEnableResultCache;
  int m_iResultCacheSize;
  //}}AFX_DATA

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CCalculationProperties)
  void OnBrowse(int nID, CTempPath::Category category);

  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CCalculationProperties)
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnAnalysisProperties();
  afx_msg void OnUpdateAnalysisProperties(CCmdUI* pCmdUI);
  afx_msg void OnBrowseUser();
  afx_msg void OnBrowseCalc();
  afx_msg void OnEnv();
  afx_msg void OnUser();
  afx_msg void OnOrderlinear();
  afx_msg void OnOrderquadratic();
  afx_msg void OnUsemethod();
  afx_msg void OnParallel();
  afx_msg void OnEnableResultCache();
  afx_msg void OnConvergenceOptions();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALCULATIONPROPERTIES_H__F7020B63_F35B_431B_B215_444C0E3F8210__INCLUDED_)
