#if !defined(AFX_2DSCENEATRIBDLG_H__490B5B4C_41FD_4F64_86F4_D16168C6464E__INCLUDED_)
#define AFX_2DSCENEATRIBDLG_H__490B5B4C_41FD_4F64_86F4_D16168C6464E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// 2dsceneatribdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// C2DSceneAtribDlg dialog

class COpenGLSceneBase;

class C2DSceneAtribDlg : public CDialog
{
// Construction
public:
  C2DSceneAtribDlg(COpenGLSceneBase &scene, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
  //{{AFX_DATA(C2DSceneAtribDlg)
  enum { IDD = IDD_2DSCENE_PROPS };
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(C2DSceneAtribDlg)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  COpenGLSceneBase* m_pScene;
  COLORREF m_Color;
  void FillFrame(const UINT uFrameId, const int nFillColor, CPaintDC &dc);
  // Generated message map functions
  //{{AFX_MSG(C2DSceneAtribDlg)
  afx_msg void OnClickBackgroundColor();
  afx_msg void OnPaint();
  virtual void OnOK();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_2DSCENEATRIBDLG_H__490B5B4C_41FD_4F64_86F4_D16168C6464E__INCLUDED_)
