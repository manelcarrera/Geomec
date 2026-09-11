// TestViewExeDoc.h : interface of the CTestViewExeDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTVIEWEXEDOC_H__DAA38FD2_168C_42D1_9E69_13AF8859706A__INCLUDED_)
#define AFX_TESTVIEWEXEDOC_H__DAA38FD2_168C_42D1_9E69_13AF8859706A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "wellpathbase.h"

#define LST_DSTABORPOINTS 1

class CTestViewExeDoc : public CDocument
{
protected: // create from serialization only
  CTestViewExeDoc();
  DECLARE_DYNCREATE(CTestViewExeDoc)

// Attributes
public:
  well::CWellPathBase* m_pWellPath;
  CWellSceneInterMed* m_pSceneInterMed;
  COpenGLScene* m_pScene;

  COpenGLScene* Scene();
  void InitWellPath();
  void InitWellPath2();
// Operations
public:

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CTestViewExeDoc)
  public:
  virtual BOOL OnNewDocument();
  virtual void Serialize(CArchive& ar);
  //}}AFX_VIRTUAL

// Implementation
public:
  virtual ~CTestViewExeDoc();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
  //{{AFX_MSG(CTestViewExeDoc)
  afx_msg void OnTestmenuDeletesection();
  afx_msg void OnTestmenuAtachscene();
  afx_msg void OnTestmenuDetachscene();
  afx_msg void OnTestmenuShowdialog();
  afx_msg void OnTestmenuShowListdlg();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTVIEWEXEDOC_H__DAA38FD2_168C_42D1_9E69_13AF8859706A__INCLUDED_)
