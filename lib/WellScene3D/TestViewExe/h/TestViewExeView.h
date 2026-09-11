// TestViewExeView.h : interface of the CTestViewExeView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTVIEWEXEVIEW_H__2212BF60_F3A3_4B7F_BE67_F78EFA810F29__INCLUDED_)
#define AFX_TESTVIEWEXEVIEW_H__2212BF60_F3A3_4B7F_BE67_F78EFA810F29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTestViewExeView : public COpenGLView {
protected: // create from serialization only
  CTestViewExeView();
  DECLARE_DYNCREATE(CTestViewExeView)

  // Attributes
public:
  CTestViewExeDoc *GetDocument();
  // Operations
public:
  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CTestViewExeView)
public:
  virtual BOOL PreCreateWindow(CREATESTRUCT &cs);

protected:
  virtual BOOL OnPreparePrinting(CPrintInfo *pInfo);
  virtual void OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo);
  virtual void OnEndPrinting(CDC *pDC, CPrintInfo *pInfo);
  virtual void OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint);
  //}}AFX_VIRTUAL

  // Implementation
public:
  virtual ~CTestViewExeView();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext &dc) const;
#endif

protected:
  // Generated message map functions
protected:
  //{{AFX_MSG(CTestViewExeView)
  // NOTE - the ClassWizard will add and remove member functions here.
  //    DO NOT EDIT what you see in these blocks of generated code !
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG // debug version in TestViewExeView.cpp
inline CTestViewExeDoc *CTestViewExeView::GetDocument() { return (CTestViewExeDoc *)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTVIEWEXEVIEW_H__2212BF60_F3A3_4B7F_BE67_F78EFA810F29__INCLUDED_)
