// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__052C5A4A_B09E_11D4_8301_0020C5E20F56__INCLUDED_)
#define AFX_MAINFRM_H__052C5A4A_B09E_11D4_8301_0020C5E20F56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GeomecSplitterWnd.h"
#include "GeomecStatusBar.h"
#include "GraphMainFrm.h"
#include "VectorDialogBar.h"

std::string ExePath();

// ##ModelId=3BC55D6102D4
class CMainFrame : public CGraphMainFrm {
private:
  static const CRect s_rectDefault;

public:
  static const char s_profileHeading[];

private:
  static const char s_profileRect[];
  static const char s_profileIcon[];
  static const char s_profileMax[];
  static const char s_profileMainFrameSplitter[];

public:
  static const char s_profileOpenGLSplitViewSplitter[];
  static const char s_profileTreeSplitViewSplitter[];

private:
  BOOL m_bFirstTime;
  BOOL m_bDoSaveParameters;

protected: // create from serialization only
  // ##ModelId=3BC55D6102E4
  CMainFrame();
  DECLARE_DYNCREATE(CMainFrame)
  void DockControlBarLeftOf(CControlBar *Bar, CControlBar *LeftOf);
  // void DockControlBarLeftOf(CToolBar* Bar,CToolBar* LeftOf);

private:
  BOOL PreTranslateMessage(MSG *pMsg);

  // Attributes
public:
  // Operations
public:
  void CanSaveParametersOnExit() { m_bDoSaveParameters = TRUE; }

  virtual void HtmlHelp(DWORD_PTR dwData, unsigned int nCmd);

  void OnModelChanged(const CFemAppModel *pModel);

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CMainFrame)
public:
  // ##ModelId=3BC55D6102E5
  virtual BOOL PreCreateWindow(CREATESTRUCT &cs);

protected:
  // ##ModelId=3BC55D6102E8
  virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext);
  virtual void ActivateFrame(int nCmdShow = -1);
  //}}AFX_VIRTUAL

  virtual void GetMessageString(UINT nID, CString &rMessage) const;

  // private:
public:
  void OpenPdfDocument(const char *fileName, const char *path);

  // Implementation
public:
  // ##ModelId=3BC55D6102F6
  virtual ~CMainFrame();
  virtual CStatusBar &StatusBar() { return m_wndStatusBar; }

  void SetIcon(CToolBar &toolbar, int icon, int resource);
  CToolBar &GetConsistencyToolbar();

#ifdef _DEBUG
  // ##ModelId=3BC55D6102F8
  virtual void AssertValid() const;
  // ##ModelId=3BC55D6102FA
  virtual void Dump(CDumpContext &dc) const;
#endif
  CGeomecStatusBar m_wndStatusBar;

protected: // control bar embedded members
  CToolBar m_wndToolBar;
  CToolBar m_wndViewBar;
  CToolBar m_wndCalcBar;
  CToolBar m_wndTopViewBar;
  CToolBar m_wndTensorBar;

  CGeomecSplitterWnd m_wndSplitter;
  CVectorDialogBar m_vectorDialogBar;

  // Generated message map functions
protected:
  //{{AFX_MSG(CMainFrame)
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
  afx_msg void OnUpdateUnit(CCmdUI *pCmdUI);
  afx_msg void OnUpdateLin(CCmdUI *pCmdUI);
  afx_msg void OnUpdateNonLin(CCmdUI *pCmdUI);
  afx_msg void OnUpdateHeat(CCmdUI *pCmdUI);
  afx_msg void OnUpdateMixture(CCmdUI *pCmdUI);
  afx_msg void OnUpdate_ElapsedTime(CCmdUI *pCmdUI);
  afx_msg void OnUpdateMesh(CCmdUI *pCmdUI);
  afx_msg void OnViewToolbarGeneral();
  afx_msg void OnUpdateViewToolbarGeneral(CCmdUI *pCmdUI);
  afx_msg void OnViewToolbarView();
  afx_msg void OnUpdateViewToolbarView(CCmdUI *pCmdUI);
  afx_msg void OnDropFiles(HDROP hDropInfo);
  afx_msg void OnDestroy();
  afx_msg void OnViewToolbarTopview();
  afx_msg void OnUpdateViewToolbarTopview(CCmdUI *pCmdUI);
  //}}AFX_MSG
  afx_msg void OnViewToolbarCalc();
  afx_msg void OnUpdateViewToolbarCalc(CCmdUI *pCmdUI);
  afx_msg void OnViewTensorToolBar();
  afx_msg void OnUpdateViewTensorToolBar(CCmdUI *pCmdUI);
  afx_msg void OnClose();
  afx_msg void OnHelpFinder();
  afx_msg void OnHelpFinder_Custom(UINT nID);
  afx_msg void OnNcRButtonUp(UINT nHitTest, CPoint point);
  afx_msg LRESULT on_status(WPARAM p1, LPARAM p2);
  afx_msg LRESULT on_event(WPARAM p1, LPARAM p2);
  DECLARE_MESSAGE_MAP()

public:
  afx_msg void OnOIVTutorial();
  afx_msg void OnMeshRefinementManual();
  afx_msg void OnLicenseBorrowingHelp();
  afx_msg void OnWellZoominHelp();
  afx_msg void OnGVTQuickTourHelp();
  afx_msg void OnMeshRefinementHelp();
  afx_msg void OnViewToolbarVectorDisplay();
  afx_msg void OnUpdateViewToolbarVectorDisplay(CCmdUI *pCmdUI);
  afx_msg void OnContextMenu(CWnd *pWnd, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__052C5A4A_B09E_11D4_8301_0020C5E20F56__INCLUDED_)
