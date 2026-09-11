// FemAppDoc.cpp : implementation of the CFemAppDoc class
//

#include "FemAppDoc.h"
#include "FemAppGUI.h"
#include "FemAppModel.h"
#include "GraphNode_Delegate.h"
#include "ISceneWrapper.h"
#include "OpenGLSceneNode.h"
#include "StorageNode.h"
#include "TnoWinApp.h"
#include "TreeNode.h"
#include "stdafx.h"
#include "unitnode.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CFemAppDoc

IMPLEMENT_DYNCREATE(CFemAppDoc, CDocument)

BEGIN_MESSAGE_MAP(CFemAppDoc, CDocument)
//{{AFX_MSG_MAP(CFemAppDoc)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFemAppDoc construction/destruction

CFemAppDoc::CFemAppDoc()
    : m_pCurrent(0), m_pModel(0), m_pGUI(0), m_pContext(0), m_bIsDeleting(false), m_bStickToScene(false) {
  // m_pCurrent = new CScenePtr(*this);
}

CFemAppDoc::~CFemAppDoc() {
  // The model ptr MUST be zero
  assert(m_pModel == 0);
  assert(m_pGUI == 0);

  if (m_pCurrent)
    delete m_pCurrent;
  m_pCurrent = 0;
}

/////////////////////////////////////////////////////////////////////////////
// CFemAppDoc diagnostics

#ifdef _DEBUG
// ##ModelId=3BC55D6300B3
void CFemAppDoc::AssertValid() const { CDocument::AssertValid(); }

// ##ModelId=3BC55D6300B5
void CFemAppDoc::Dump(CDumpContext &dc) const { CDocument::Dump(dc); }
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFemAppDoc commands

void CFemAppDoc::OnSelect(CGraphNode *pNode) {
  // Let's do the base it's work
  CGraphNode *pPreviousNode = SelectedNode();

  if (pNode == 0)
    m_pSelection.Clear();
  else {
    if (m_pSelection.Ptr() != pNode) // Bug 487
      m_pSelection.LinkTo(*pNode);
  }

  assert(m_pSelection.Ptr() == pNode);

  // inform prev selected node of deselecting
  if (pNode != pPreviousNode) {
    if (pPreviousNode)
      pPreviousNode->OnDeselect(pNode);

    if (pNode) {
      CGraphNode_Delegate *pNode_Delegate = dynamic_cast<CGraphNode_Delegate *>(pNode->getDelegate());

      if (pNode_Delegate)
        pNode_Delegate->OnSelect(pPreviousNode);
    }
  }

  // We need at least a selected node and an scene
  if (SelectedNode() == 0)
    return;

  // Can we view the current selection with the current scene
  if (CurrentScene()) {
    CurrentScene()->OnSelect(*SelectedNode());
  }
}

void CFemAppDoc::OnAttachModel(CFemAppModel &model, CFemAppGUI &gui) {
  // OK ... delete old model ...
  DeleteContents();

  assert(!Model());

  // Insert model in document ...
  m_pModel = &model;
  m_pGUI = &gui;
}

void CFemAppDoc::OnDetachModel(CFemAppModel &model, CFemAppGUI &gui) {
  if (&model == Model()) {
    // Make pointer zero and delete existing selections
    m_pModel = 0;
    m_pGUI = 0;
    m_pSelection.Clear();

    CurrentScene((ISceneWrapper *)0);
  }
}

// Switch between existing models (don't delete anything, just switch the current context)
void CFemAppDoc::OnSwitchModel(CFemAppModel &model, CFemAppGUI &gui) {
  if (&model == Model())
    return; // nothing to do

  m_pModel = &model;
  m_pGUI = &gui;
}

const ISceneWrapper *CFemAppDoc::CurrentScene() const {
  return m_pCurrent;
  /*
  if (m_pCurrent)
    return m_pCurrent->Ptr();
  else
    return 0;\
  */
}

ISceneWrapper *CFemAppDoc::CurrentScene() {
  return m_pCurrent;
  /*
  if (m_pCurrent)
    return m_pCurrent->Ptr();
  else
    return 0;
  */
}

void CFemAppDoc::OnViewPerspective() {
  assert(CurrentScene());
  CurrentScene()->DisplayPerspective(!CurrentScene()->DisplayPerspective());
}

void CFemAppDoc::OnViewRotate() {
  assert(CurrentScene());
  CurrentScene()->AllowDynamicRotate(!CurrentScene()->AllowDynamicRotate());
}

void CFemAppDoc::OnViewAxis() {
  assert(CurrentScene());
  CurrentScene()->DisplayGnomon(!CurrentScene()->DisplayGnomon());
}

void CFemAppDoc::OnViewFill() {
  assert(CurrentScene());
  CurrentScene()->Fill(!CurrentScene()->Fill());
}

void CFemAppDoc::DeleteContents() {
  // Reset Current view ...
  ISceneWrapper *pTemp = m_pCurrent;
  m_pCurrent = 0;

  // Broadcast to view that there no scene any more
  UpdateAllViews(0, NEW_CURRENT_SCENE, (CObject *)(CurrentScene()));

  // Close model enz...
  // If we have model close it and destroy ....
  if (Model()) {
    CTnoWinApp *app = (CTnoWinApp *)AfxGetApp();
    if (app->IsExiting()) {
      // Just forget about the model, don't take (lots of) time destroying: process will release all memory

      // Clear the scene first
      if (pTemp)
        pTemp->Clear();
      // Clear selection
      OnSelect(0);

      m_pModel->OnExit(); // we do need to delete any temporary disk data in case result cache is used
      m_pModel = 0;
      m_pGUI = 0;
      m_bIsDeleting = true;
    } else {
      // Decouple model by closing it and destroy ...
      m_bIsDeleting = true;
      CFemAppModel *pModel = Model();
      pModel->setDeleting(m_bIsDeleting);
      pModel->DestroyCaches();
      pModel->CloseModel();
      pTemp = 0; // no more valid scene
      assert(!Model());
      assert(!m_pSelection.Ptr());
    }
    UpdateAllViews(0, NEW_MODEL, (CObject *)((CurrentScene())));
  }

  // Reset modifiedflag
  SetModifiedFlag(FALSE);
  m_bIsDeleting = false;

  CDocument::DeleteContents();

  m_pCurrent = pTemp;
  // Broadcast to the view that this is the new scene
  UpdateAllViews(0, NEW_CURRENT_SCENE, (CObject *)(m_pCurrent));
}

void CFemAppDoc::OnZoomIn() { CurrentScene()->ScaleVector(CurrentScene()->ScaleVector() * 1.2); }

void CFemAppDoc::OnZoomOut() { CurrentScene()->ScaleVector(CurrentScene()->ScaleVector() * 0.8); }

void CFemAppDoc::OnZoomReset() {
  assert(CurrentScene());
  CurrentScene()->ResetPan();
  CurrentScene()->ResetScale();
}

void CFemAppDoc::CurrentScene(ISceneWrapper *pScene) {
  if ((!m_pCurrent || m_pCurrent != pScene) && !m_bStickToScene) {
    if (m_pCurrent) {
      m_pCurrent->Deactivate();
      delete m_pCurrent;
    }
    m_pCurrent = pScene;

    // we need to update views first, as Activate might visit them and otherwise they may follow the pointer we just
    // deleted
    UpdateAllViews(0, NEW_CURRENT_SCENE, (CObject *)(m_pCurrent));
    if (m_pCurrent)
      m_pCurrent->Activate();
  }
}

bool CFemAppDoc::StickToScene() const { return m_bStickToScene; }

void CFemAppDoc::StickToScene(bool bStick) { m_bStickToScene = bStick; }

void CFemAppDoc::OnLockX() {
  assert(CurrentScene());
  CurrentScene()->RotateLockVector(geo::CVector::Yaxis);
}

void CFemAppDoc::OnLockY() {
  assert(CurrentScene());
  CurrentScene()->RotateLockVector(geo::CVector::Xaxis);
}

void CFemAppDoc::OnLockZ() {
  assert(CurrentScene());
  CurrentScene()->RotateLockVector(geo::CVector::Zaxis);
}

void CFemAppDoc::OnUnlockAxes() {
  assert(CurrentScene());
  CurrentScene()->RotateLockVector(geo::CVector::NullVector);
}

void CFemAppDoc::OnViewLighting() {
  assert(CurrentScene());
  CurrentScene()->EnableLighting(!CurrentScene()->LightingEnabled());
}

void CFemAppDoc::OnZoomWindow() { UpdateAllViews(0, ON_ZOOMWINDOW_BUTTON); }

CUnitNode &CFemAppDoc::UnitNode() { return m_unit; }

const CUnitNode &CFemAppDoc::UnitNode() const { return m_unit; }

/////////////////////////////////////////////////////////////////////////////
void CFemAppDoc::OnInvalidateStorage() { SetModifiedFlag(); }
/*
void CFemAppDoc::OnInvalidateCalculation(int nType)
{
}
*/
// ##ModelId=3B653D030198
CFemAppModel *CFemAppDoc::Model() { return m_pModel; }

const CFemAppModel *CFemAppDoc::Model() const { return m_pModel; }

CFemAppGUI *CFemAppDoc::GUI() { return m_pGUI; }

const CFemAppGUI *CFemAppDoc::GUI() const { return m_pGUI; }

CFemAppGUI *CFemAppDoc::retrieveMatchingGUI(CFemAppGUI *femAppGUI, CFemAppModel *femAppModel) const {
  CFemAppGUI *matchingGUI = 0;

  for (int i = 0; i < femAppGUI->childGUISize(); ++i) {
    matchingGUI = retrieveMatchingGUI(&femAppGUI->childGUI(i), femAppModel);

    if (matchingGUI != 0) {
      break;
    }
  }

  if (matchingGUI == 0) {
    if (femAppGUI->model() == femAppModel) {
      matchingGUI = femAppGUI;
    }
  }

  return matchingGUI;
}

// ##ModelId=3B653D030179
void CFemAppDoc::OnModelProperties() {
  // TODO: Add your command handler code here
}

// ##ModelId=3B653D03018B
void CFemAppDoc::OnCloseDocument() {
  // OnCloseDocument will also call DeleteContents, but after destruction of the views...
  DeleteContents();
  CDocument::OnCloseDocument();
}

// ##ModelId=3B653D03018D

void CFemAppDoc::OnContextMenu(ICtrlObject &ctrl_object) { m_pContext = &ctrl_object; }

CGraphNode &CFemAppDoc::ContextMenuNode() {
  CTreeNode *pNode = dynamic_cast<CTreeNode *>(m_pContext);
  assert(pNode);
  return pNode->ObservedItem();
}

ICtrlObject &CFemAppDoc::ContextMenuCtrlObject() { return *m_pContext; }

const ICtrlObject &CFemAppDoc::ContextMenuCtrlObject() const { return *m_pContext; }

const CGraphNode *CFemAppDoc::SelectedNode() const { return m_pSelection.Ptr(); }

CGraphNode *CFemAppDoc::SelectedNode() { return m_pSelection.Ptr(); }

bool CFemAppDoc::OnContextCanDelete() const {
  assert(Model());
  return (ContextMenuCtrlObject().CanDelete() != FALSE);
}

void CFemAppDoc::OnContextDelete() {
  assert(Model());
  assert(ContextMenuCtrlObject().CanDelete());
  ContextMenuCtrlObject().Delete();
}
