// TestExeDoc.cpp : implementation of the CTestExeDoc class
//

#include "TestExeDoc.h"
#include "2DDocument.h"
#include "TestExe.h"
#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CTestExeDoc

IMPLEMENT_DYNCREATE(CTestExeDoc, CDocument)

BEGIN_MESSAGE_MAP(CTestExeDoc, CDocument)
//{{AFX_MSG_MAP(CTestExeDoc)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestExeDoc construction/destruction

CTestExeDoc::CTestExeDoc() : m_pModel(0) {
  // TODO: add one-time construction code here
}

CTestExeDoc::~CTestExeDoc() {}

BOOL CTestExeDoc::OnNewDocument() {
  if (!CDocument::OnNewDocument())
    return FALSE;

  delete m_pModel;
  m_pModel = new C2DDocument();
  m_pModel->newDocument();

  UpdateAllViews(0);

  // TODO: add reinitialization code here
  // (SDI documents will reuse this document)

  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CTestExeDoc serialization

void CTestExeDoc::Serialize(CArchive &ar) {
  if (ar.IsStoring()) {
    // TODO: add storing code here
  } else {
    // TODO: add loading code here
  }
}

/////////////////////////////////////////////////////////////////////////////
// CTestExeDoc diagnostics

#ifdef _DEBUG
void CTestExeDoc::AssertValid() const { CDocument::AssertValid(); }

void CTestExeDoc::Dump(CDumpContext &dc) const { CDocument::Dump(dc); }
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTestExeDoc commands

void CTestExeDoc::DeleteContents() {
  if (m_pModel)
    m_pModel->closeDocument();
  delete m_pModel;
  m_pModel = 0;

  CDocument::DeleteContents();
}
