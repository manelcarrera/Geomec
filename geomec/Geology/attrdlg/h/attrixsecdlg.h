#if !defined(AFX_ATTRIXSECDLG_H__4F048AF5_6977_44CA_996C_957C9FD93E2C__INCLUDED_)
#define AFX_ATTRIXSECDLG_H__4F048AF5_6977_44CA_996C_957C9FD93E2C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// attrixsecdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAttriXSecDlg dialog

#include "CrossSection.h"

class CUnitNode;

class CAttriXSecDlg : public CDialog {

  // Construction
public:
  CAttriXSecDlg(CCrossSection &Xsec, CWnd *pParent = NULL); // standard constructor

  // Dialog Data
  //{{AFX_DATA(CAttriXSecDlg)
  enum { IDD = IDD_ATTRI_XSEC };
  // NOTE: the ClassWizard will add data members here
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CAttriXSecDlg)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support

  //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CAttriXSecDlg)
  virtual BOOL OnInitDialog();
  //}}AFX_MSG
  virtual void OnOK();

  DECLARE_MESSAGE_MAP()

private:
  void UpdateControls();

  bool TryApply();

private:
  CCrossSection &m_xsec;
  CCrossSection::OrientationType m_constraintScreen;
  CCrossSection::OrientationType m_constraintBookmarked;

  geo::CPoint m_basePointScreen;
  geo::CVector m_normalScreen;
  geo::CVector m_normalScreenSaved;

  geo::CPoint m_basePointBookmarked;
  geo::CVector m_normalBookmarked;
  geo::CVector m_normalBookmarkedSaved;

  CString m_strName;

  bool m_bOk;
  geo::CPoint m_ptFirst;
  geo::CPoint m_ptSecond;
  double m_dDepth;
  int m_crossSectionType;

public:
  afx_msg void OnCopyFromBookmarked();
  afx_msg void OnCopyFromScreen();
  afx_msg void OnBnClickedRadioScreenDepth();
  afx_msg void OnBnClickedRadioScreenNe();
  afx_msg void OnBnClickedRadioScreenNorthing();
  afx_msg void OnBnClickedRadioScreenEasting();
  afx_msg void OnBnClickedRadioScreenCurrent();
  afx_msg void OnBnClickedRadioScreenAnydir();
  afx_msg void OnBnClickedRadioBmDepth();
  afx_msg void OnBnClickedRadioBmNe();
  afx_msg void OnBnClickedRadioBmNorthing();
  afx_msg void OnBnClickedRadioBmEasting();
  afx_msg void OnBnClickedRadioBmCurrent();
  afx_msg void OnBnClickedRadioBmAnydir();
  afx_msg void OnApply();
  afx_msg void OnRadioVertical();
  afx_msg void OnRadioHorizontal();
  afx_msg void OnRadioManual();
  afx_msg void OnBnClickedOk();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATTRIXSECDLG_H__4F048AF5_6977_44CA_996C_957C9FD93E2C__INCLUDED_)
