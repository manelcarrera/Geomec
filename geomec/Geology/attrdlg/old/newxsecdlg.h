#if !defined(AFX_NEWXSECDLG_H__3AF48C1D_BAFB_41AB_9DAF_FF477D0E50B2__INCLUDED_)
#define AFX_NEWXSECDLG_H__3AF48C1D_BAFB_41AB_9DAF_FF477D0E50B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// newxsecdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewXSecDlg dialog

class CNewXSecDlg : public CDialog
{
  double m_dFirstNorth;
  double m_dFirstEast;
  double m_dSecondNorth;
  double m_dSecondEast;
  CGraphModel &m_model;
// Construction
public:
  CNewXSecDlg(CGraphModel &model, CWnd* pParent = NULL);   // standard constructor

  double FirstNorth() {return m_dFirstNorth;}
  void FirstNorth(double dValue) {m_dFirstNorth = dValue;}
  double FirstEast() {return m_dFirstEast;}
  void FirstEast(double dValue) {m_dFirstEast = dValue;}
  double SecondNorth() {return m_dSecondNorth;}
  void SecondNorth(double dValue) {m_dSecondNorth = dValue;}
  double SecondEast() {return m_dSecondEast;}
  void SecondEast(double dValue) {m_dSecondEast = dValue;}

// Dialog Data
  //{{AFX_DATA(CNewXSecDlg)
  enum { IDD = IDD_NEW_XSEC };
    // NOTE: the ClassWizard will add data members here
  //}}AFX_DATA

  const CUnitNode& UnitNode();


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CNewXSecDlg)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CNewXSecDlg)
    // NOTE: the ClassWizard will add member functions here
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWXSECDLG_H__3AF48C1D_BAFB_41AB_9DAF_FF477D0E50B2__INCLUDED_)
