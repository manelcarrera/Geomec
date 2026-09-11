#if !defined(AFX_OPENGLTESTVIEW_H__9DCF6E57_49EE_42CC_A388_D6A951B6AAE9__INCLUDED_)
#define AFX_OPENGLTESTVIEW_H__9DCF6E57_49EE_42CC_A388_D6A951B6AAE9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// opengltestview.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COpenGLTestView view

#include "OpenGLView.h"

class CModelObjectScene;
class COpenGLTestView : public COpenGLView
{
  CModelObjectScene* m_pScene;
protected:
  COpenGLTestView();           // protected constructor used by dynamic creation
  DECLARE_DYNCREATE(COpenGLTestView)

// Attributes
public:

// Operations
public:

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(COpenGLTestView)
  public:
  virtual void OnInitialUpdate();
  //}}AFX_VIRTUAL

// Implementation
protected:
  virtual ~COpenGLTestView();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

  // Generated message map functions
protected:
  //{{AFX_MSG(COpenGLTestView)
  afx_msg void OnInsertPolyline();
  afx_msg void OnUpdateInsertPolyline(CCmdUI* pCmdUI);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPENGLTESTVIEW_H__9DCF6E57_49EE_42CC_A388_D6A951B6AAE9__INCLUDED_)
