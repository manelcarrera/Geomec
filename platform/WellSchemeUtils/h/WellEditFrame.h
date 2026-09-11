#if !defined(AFX_WELLEDITFRAME_H__0713101D_E71B_405A_93F4_6655FC6CC63C__INCLUDED_)
#define AFX_WELLEDITFRAME_H__0713101D_E71B_405A_93F4_6655FC6CC63C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WellEditFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWellEditFrame frame
class COpenGLView;
class COpenGLScene;
class CWellEditDlg;

class AFX_EXT_CLASS CWellEditFrame : public CFrameWnd
{
  DECLARE_DYNCREATE(CWellEditFrame)
protected:

public:
  CWellEditFrame();        
  void Init(COpenGLView* pView,CWellEditDlg* m_pDlg);

public:
  bool IsDirty();
// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CWellEditFrame)
  //}}AFX_VIRTUAL

// Implementation
protected:
  virtual ~CWellEditFrame();

  // Generated message map functions
  //{{AFX_MSG(CWellEditFrame)
  afx_msg void OnButtonSpreadOK();
  afx_msg void OnButtonSpreadCancel();
  afx_msg void OnCheckSpreadAzimuth();
  afx_msg void OnCloseupComboSpreadType();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
  COpenGLView* m_pView;
  CWellEditDlg* m_pDlg;
  bool m_Dirty;

  COpenGLScene*  CurrentScene();
  void		  On3dvLockDepth();
  void		  OnUpdate3DViewButtons(CCmdUI* pCmdUI);
  void		  On3dvLockEast();
  void		  On3dvLockNorth();
  void		  On3dvLockReset();
  void		  On3dvZoomReset();
  void		  On3dvZoomWindow();
  void		  On3dvZoomin();
  void		  On3dvZoomout();
  void		  On3dvCopypicture();
  void		  On3dvViewAxis();
  void		  On3dvViewBeDirec();
  void		  On3dvViewBoundary();
  void		  On3dvViewEDirec();
  void		  On3dvViewLighting();
  void		  On3dvViewNDirec();
  void		  On3dvViewPerspective();
  void		  On3dvViewZDirec();

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WELLEDITFRAME_H__0713101D_E71B_405A_93F4_6655FC6CC63C__INCLUDED_)
