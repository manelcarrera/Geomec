// opengltestview.cpp : implementation file
//

#include "opengltestview.h"
#include "2ddocument.h"
#include "2dsegment.h"
#include "MfcOpenglobjectobservertemp.h"
#include "TestExe.h"
#include "TestExeDoc.h"
#include "modelobjectscene.h"
#include "polylinecreator.h"
#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// COpenGLTestView

IMPLEMENT_DYNCREATE(COpenGLTestView, COpenGLView)

COpenGLTestView::COpenGLTestView() : m_pScene(0) {}

COpenGLTestView::~COpenGLTestView() { delete m_pScene; }

BEGIN_MESSAGE_MAP(COpenGLTestView, COpenGLView)
//{{AFX_MSG_MAP(COpenGLTestView)
ON_COMMAND(ID_DRAW_INSERT_POLYLINE, OnInsertPolyline)
ON_UPDATE_COMMAND_UI(ID_DRAW_INSERT_POLYLINE, OnUpdateInsertPolyline)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenGLTestView drawing

/////////////////////////////////////////////////////////////////////////////
// COpenGLTestView diagnostics

#ifdef _DEBUG
void COpenGLTestView::AssertValid() const { CView::AssertValid(); }

void COpenGLTestView::Dump(CDumpContext &dc) const { CView::Dump(dc); }
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COpenGLTestView message handlers

void COpenGLTestView::OnInitialUpdate() {
  delete m_pScene;
  m_pScene = new CModelObjectScene();
  m_pScene->ReParent(this);

  CTestExeDoc *pDoc = (CTestExeDoc *)GetDocument();
  if (pDoc->Model() == 0)
    return;

  // Set observers
  typedef CMfcOpenGLObjectObserverTemp<C2DVertex, COpenGLObjectObserver> T2DSegmentObserver;
  typedef CMfcOpenGLObjectObserverTemp<C2DSegment, T2DSegmentObserver> TPolyLineObserver;
  typedef CMfcOpenGLObjectObserverTemp<C2DPolyLine, TPolyLineObserver> TPolyLineContainerObserver;

  new TPolyLineContainerObserver(pDoc->Model()->PolyLines(), *m_pScene);

  typedef CMfcOpenGLObjectObserverTemp<C2DBody, COpenGLObjectObserver> TBodyContainerObserver;

  new TBodyContainerObserver(pDoc->Model()->Bodies(), *m_pScene);

  m_pScene->UpdateFrame();

  COpenGLView::OnInitialUpdate();
}

void COpenGLTestView::OnInsertPolyline() {
  CTestExeDoc *pDoc = (CTestExeDoc *)GetDocument();
  if (pDoc->Model() && m_pScene) {
    if (dynamic_cast<CPolyLineCreator *>(m_pScene->MouseListener()) == 0)
      new CPolyLineCreator(*m_pScene, *pDoc->Model());
    else
      MouseListener()->Cancel();
  }
}

void COpenGLTestView::OnUpdateInsertPolyline(CCmdUI *pCmdUI) {
  CTestExeDoc *pDoc = (CTestExeDoc *)GetDocument();
  if (pDoc->Model() && m_pScene) {
    pCmdUI->Enable();
    if (dynamic_cast<CPolyLineCreator *>(m_pScene->MouseListener()))
      pCmdUI->SetCheck(1);
    else
      pCmdUI->SetCheck(0);
  } else
    pCmdUI->Enable(FALSE);
}
