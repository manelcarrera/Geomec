#if !defined(AFX_WELLPATHGRAPHLIMITSDLG_H__60572A82_773D_4649_9B1D_A45B1A2D9230__INCLUDED_)
#define AFX_WELLPATHGRAPHLIMITSDLG_H__60572A82_773D_4649_9B1D_A45B1A2D9230__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// wellpathgraphlimitsdlg.h : header file
//

#include "GlobalMessage.h"
#include "resource.h"

#include "OIV2DRangeInterface.h"

/////////////////////////////////////////////////////////////////////////////
// CGraphLimitsDlg dialog

class CGraphLimitsDlg : public CDialog
{
  // Construction
  OIV2DRangeInterface& m_rangeInterface;

  int m_iAutoLeft, m_iAutoTop, m_iAutoBottom; // bool; wjrx mantis 2559, tfs 90989

public:
  CGraphLimitsDlg(OIV2DRangeInterface& rangeInterface, CWnd* pParent = NULL);   // standard constructor
  void FindExtremesLeft();
  void FindExtremesTop();
  void FindExtremesBottom();
  void DisplayLeft(float MinLeft, float MaxLeft);
  void DisplayTop(float MinTop, float MaxTop);
  void DisplayBottom(float MinBottom, float MaxBottom);

  // Dialog Data
  //{{AFX_DATA(CGraphLimitsDlg)
  enum { IDD = IDD_GRAPHLIMITS };
  //}}AFX_DATA


  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CGraphLimitsDlg)
protected:
  BOOL OnInitDialog();
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

  // Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CGraphLimitsDlg)
  void OnChkAutoLeft();
  void OnChkAutoTop();
  void OnChkAutoBottom();
  void OnSetFocusEditLeft();
  void OnKillFocusEditLeft();
  void OnSetFocusEditTop();
  void OnSetFocusEditBottom();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};



#endif // !defined(AFX_WELLPATHGRAPHLIMITSDLG_H__60572A82_773D_4649_9B1D_A45B1A2D9230__INCLUDED_