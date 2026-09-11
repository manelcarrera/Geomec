#if !defined(AFX_FEMAPPOPENGLVIEW_H__B8F2E84D_081F_49B1_BA92_9E03AD8B078A__INCLUDED_)
#define AFX_FEMAPPOPENGLVIEW_H__B8F2E84D_081F_49B1_BA92_9E03AD8B078A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "openglview.h"

class COpenGLNode;
class COpenGLSceneBase;
class CFemAppOpenGLView : public COpenGLView {
  mutable bool m_bStep;

protected:
  CFemAppOpenGLView(); // protected constructor used by dynamic creation
  DECLARE_DYNCREATE(CFemAppOpenGLView)
  virtual ~CFemAppOpenGLView();

#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext &dc) const;
#endif
public:
  BOOL CopyBitmapToFile(const CString &sPathname);
  void CopyClipboard();
  virtual void OnStartDraw(int nSteps) const;
  virtual void OnDrawStep() const;
  virtual void OnEndDraw() const;
  // Overrides
  virtual void CurrentScene(COpenGLSceneBase *pCurrentScene);
  virtual COpenGLSceneBase *CurrentScene();

  virtual COpenGLView *LegendView() { return 0; }
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CModelView)
protected:
  virtual void OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint);
  //}}AFX_VIRTUAL

  // Generated message map functions
protected:
  //{{AFX_MSG(CModelView)
  afx_msg void OnDestroy();
  afx_msg void OnEditDelete();
  afx_msg void OnUpdateEditDelete(CCmdUI *pCmdUI);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
private:
  BOOL CopyBitmap(const CString &sPathname = "");
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FEMAPPOPENGLVIEW_H__B8F2E84D_081F_49B1_BA92_9E03AD8B078A__INCLUDED_)
