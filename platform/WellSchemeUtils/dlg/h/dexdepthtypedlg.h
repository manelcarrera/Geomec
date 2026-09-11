#if !defined(AFX_DEXDEPTHTYPEDLG_H__DB49283B_39C2_4276_818A_0D9C36EDC87D__INCLUDED_)
#define AFX_DEXDEPTHTYPEDLG_H__DB49283B_39C2_4276_818A_0D9C36EDC87D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dexdepthtypedlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDexDepthTypeDlg dialog

class CDexDepthTypeDlg : public CDialog {
public:
  enum DEPTH_TYPE { PROGNOSED = 0, LOGGED };

private:
  DEPTH_TYPE m_depth_type;

  // Construction
public:
  CDexDepthTypeDlg(CWnd *pParent = NULL); // standard constructor

  DEPTH_TYPE DepthType() const { return m_depth_type; }

  // Dialog Data
  //{{AFX_DATA(CDexDepthTypeDlg)
  enum { IDD = IDD_DEXIMPORT_FORMATIONTYPE };
  int m_DepthType;
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CDexDepthTypeDlg)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CDexDepthTypeDlg)
  // NOTE: the ClassWizard will add member functions here
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEXDEPTHTYPEDLG_H__DB49283B_39C2_4276_818A_0D9C36EDC87D__INCLUDED_)
