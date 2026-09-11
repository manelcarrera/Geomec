// OpenGLSceneBase.cpp: implementation of the COpenGLSceneNode class.
//
//////////////////////////////////////////////////////////////////////

#include "OpenGLSceneNode.h"
#include "FemAppEntryTypes.h"
#include "FemAppMainWindow.h"
#include "FemAppModel.h"
#include "FemAppOpenGLView.h"
#include "GraphMainFrm.h"
#include "OpenGLComposite.h"
#include "OpenGLNodeBase.h"
#include "OpenGLNode_Delegate.h"
#include "SceneDrawDef.h"
#include "StorageNode.h"
#include "legendframebase.h"
#include "openglnode.h"
#include "stdafx.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
// #define new DEBUG_NEW
#endif

COpenGLSceneNode::CDefaultListener::CDefaultListener(COpenGLSceneNode &scene)
    : CMouseListener(scene), m_bDestroy(FALSE), m_scene(scene) {}

bool COpenGLSceneNode::CDefaultListener::DispatchSelection(TKeyboardModifiers state, TMouseButton button,
                                                           const TScreenPoint &point, pfHandler handler) {
  // Get Selection
  IOpenGLFrame::TObjectVec vcSelection = Frame().ProcessSelection(point);
  // All node in current scene

  COpenGLSceneNode::TOpenGLNodeSet stOpenGLNode = m_scene.NodesInScene();

  // Walk over nodes
  COpenGLSceneNode::TOpenGLNodeSet::iterator itNode;
  for (itNode = stOpenGLNode.begin(); itNode != stOpenGLNode.end(); itNode++) {
    std::vector<const geo::IObject *> vcHit;

    // Is the list selected?
    for (size_t i = 0; i < vcSelection.size(); i++) {
      // Walk over display list
      for (int nList = 0; nList < (*itNode)->DisplayListSize(); nList++) {
        if (&(*itNode)->DisplayList(nList) == vcSelection[i])
          vcHit.push_back(vcSelection[i]);
      }
    }

    if (vcHit.size() > 0) {
      COpenGLNode_Delegate *pNode = static_cast<COpenGLNode_Delegate *>(((COpenGLNode *)(*itNode))->getDelegate());
      if ((pNode->*handler)(Frame(), state, button, point, vcHit))
        return true; // Handled ...
    }
  }

  return false;
}

bool COpenGLSceneNode::CDefaultListener::MousePress(TKeyboardModifiers state, TMouseButton button,
                                                    const TScreenPoint &point) {
  return DispatchSelection(state, button, point, &COpenGLNode_Delegate::MousePress);
}

bool COpenGLSceneNode::CDefaultListener::MouseRelease(TKeyboardModifiers state, TMouseButton button,
                                                      const TScreenPoint &point) {
  return DispatchSelection(state, button, point, &COpenGLNode_Delegate::MouseRelease);
}

bool COpenGLSceneNode::CDefaultListener::MouseDblClk(TKeyboardModifiers state, TMouseButton button,
                                                     const TScreenPoint &point) {
  return DispatchSelection(state, button, point, &COpenGLNode_Delegate::MouseDblClk);
}

bool COpenGLSceneNode::CDefaultListener::MouseMove(TKeyboardModifiers state, TMouseButton button,
                                                   const TScreenPoint &point) {
  return DispatchSelection(state, button, point, &COpenGLNode_Delegate::MouseMove);
}

bool COpenGLSceneNode::CDefaultListener::OnOK() { return m_bDestroy; }

bool COpenGLSceneNode::CDefaultListener::OnDelete() { return m_bDestroy; }

bool COpenGLSceneNode::CDefaultListener::OnCancel() { return m_bDestroy; }

void COpenGLSceneNode::CDefaultListener::Destroy() { m_bDestroy = TRUE; }

COpenGLSceneNode::CSceneOperationReceiver::CSceneOperationReceiver(COpenGLSceneNode &scene)
    : COperationReceiver(scene.Model()), m_scene(scene) {}

void COpenGLSceneNode::CSceneOperationReceiver::OnBeginOperation() { m_scene.OnBeginOperation(); }

void COpenGLSceneNode::CSceneOperationReceiver::OnEndOperation() { m_scene.OnEndOperation(); }
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
COpenGLSceneNode::COpenGLSceneNode(CFemAppModel &model)
    : CStorageNode(model), m_bFill(FALSE), m_pSelected(0), m_bDirty(false), m_bProgress(false) {
  // TODO TO GUI due to CDefaultListener->CMouseListener

  m_pDefaultListener = new CDefaultListener(*this);
  m_pReceiver = new CSceneOperationReceiver(*this);
}

COpenGLSceneNode::COpenGLSceneNode(const QString &strName, CFemAppModel &model)
    : CStorageNode(strName, model), m_bFill(FALSE), m_pSelected(0), m_bDirty(false), m_bProgress(false) {
  // TODO TO GUI due to CDefaultListener->CMouseListener

  m_pReceiver = new CSceneOperationReceiver(*this);

  // Init vector
  COpenGLScene::ViewVector(geo::CVector::Yaxis * -1.0);
  COpenGLScene::UpVector(geo::CVector::Zaxis);

  // Switch off bounding box viewing
  DisplayBoundingBox(FALSE);

  // Register
  assert(Model().GraphEntry(MD_BASE_SCENE_NODE));

  create(Model().GraphEntry(MD_BASE_SCENE_NODE));

  m_pDefaultListener = new CDefaultListener(*this);
}

COpenGLSceneNode::COpenGLSceneNode(unsigned int uName, CFemAppModel &model)
    : CStorageNode(uName, model), m_bFill(FALSE), m_pSelected(0), m_bDirty(false), m_bProgress(false) {
  // TODO TO GUI due to CDefaultListener->CMouseListener

  m_pReceiver = new CSceneOperationReceiver(*this);

  // Init vector
  COpenGLScene::ViewVector(geo::CVector::Yaxis * -1.0);
  COpenGLScene::UpVector(geo::CVector::Zaxis);

  // Switch off bounding box viewing
  DisplayBoundingBox(FALSE);

  // Register
  assert(Model().GraphEntry(MD_BASE_SCENE_NODE));
  create(Model().GraphEntry(MD_BASE_SCENE_NODE));

  m_pDefaultListener = new CDefaultListener(*this);
}

COpenGLSceneNode::COpenGLSceneNode(const COpenGLSceneNode &rhs)
    : CStorageNode(rhs), COpenGLScene(rhs), m_bFill(rhs.m_bFill), m_pSelected(0), m_bDirty(false), m_bProgress(false) {
  // TODO TO GUI due to CDefaultListener->CMouseListener

  m_pReceiver = new CSceneOperationReceiver(*this);

  // Init vector
  //	COpenGLScene::ViewVector(geo::CVector::Yaxis*-1.0);
  //	COpenGLScene::UpVector(geo::CVector::Zaxis);

  // Switch off bounding box viewing
  //	DisplayBoundingBox(FALSE);

  // Register
  assert(Model().GraphEntry(MD_BASE_SCENE_NODE));
  //	LinkTo(*Model().GraphEntry(MD_BASE_SCENE_NODE));

  m_pDefaultListener = new CDefaultListener(*this);
}

COpenGLSceneNode::~COpenGLSceneNode() {
  if (IsLegendFrame())
    LegendFrame().ClearLegendFrames();

  if (!IsCopy()) {
    // Delete all draw defs...
    for (TViewMap::iterator it = m_mpViewMap.begin(); it != m_mpViewMap.end(); it++) {
      const TDisplayVec &vcDisplay = it->second;
      for (size_t i = 0; i < vcDisplay.size(); i++) {
        IDrawDef *pDrawDef = const_cast<IDrawDef *>(&DrawDef(*vcDisplay[i]));
        delete pDrawDef;
      }
    }
  }

  m_pDefaultListener->Destroy();

  delete m_pReceiver;
}

void COpenGLSceneNode::OnBeginOperation() {}

void COpenGLSceneNode::OnEndOperation() {
  if (m_bDirty)
    UpdateScene();
  m_bDirty = false;
}

bool COpenGLSceneNode::IsOperation() const { return m_pReceiver->IsOperation(); }

void COpenGLSceneNode::UpdateScene() {
  if (IsOperation())
    m_bDirty = true;
  else {
    UpdateLegend();
    UpdateFrame();
    Modified();
  }
}

COpenGLSceneNode::TOpenGLNodeSet COpenGLSceneNode::NodesInScene() const {
  TOpenGLNodeSet stNode;

  for (TViewMap::const_iterator it = m_mpViewMap.begin(); it != m_mpViewMap.end(); it++) {
    stNode.insert(it->first);
  }

  return stNode;
}

COpenGLSceneNode::TOpenGLNodeSet COpenGLSceneNode::NodesInSceneForLegend() const { return NodesInScene(); }

std::vector<COpenGLSceneNode::TColor> COpenGLSceneNode::OnColor(const COpenGLNode &node,
                                                                const geo::IObject &object) const {
  return node.OnColor(object);
}

void COpenGLSceneNode::OnNewNeighbour(const CGraphNode &node) {
  // Let the graphnode do it's work ...
  CGraphNode::OnNewNeighbour(node);

  const COpenGLNode *pViewNode = dynamic_cast<const COpenGLNode *>(&node);

  if (pViewNode) {
    TDisplayVec vcDisplay;

    // Walk over objects
    for (int i = 0; i < pViewNode->DisplayListSize(); i++) {
      // Create a new draw def ...
      IDrawDef *pDrawDef = OnCreateDrawDef(*pViewNode, pViewNode->DisplayList(i));
      assert(pDrawDef);

      // Try to insert in the scene ...
      if (COpenGLScene::insert(pViewNode->DisplayList(i), *pDrawDef)) {
        // Inserted in the scene add to vector ...
        assert(pDrawDef == &DrawDef(pViewNode->DisplayList(i)));
        vcDisplay.push_back(&pViewNode->DisplayList(i));
      } else {
        // Insertion failed. Destroy create drawdef
        delete pDrawDef;
      }
    }

    VERIFY(m_mpViewMap.insert(TViewMap::value_type(pViewNode, vcDisplay)).second);

    // Inform framework
    OnNewOpenGLNode(const_cast<COpenGLNode &>(*pViewNode));

    UpdateScene();
  }
}

void COpenGLSceneNode::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint) {
  assert(IsLinkedTo(node));

  // Is our selected modified? -> Render
  if (m_pSelected == &node) {
    // Reselect
    OnSelect(*m_pSelected);
  }

  // Displaylist modified
  const COpenGLNode *pViewNode = dynamic_cast<const COpenGLNode *>(&node);
  if (pViewNode) {
    // Get our display vector. When we're still in the OnNearestNeighbour routine
    // we possibly can get in the modified routine. In that case the node is node
    // not found in the view map
    TViewMap::iterator it = m_mpViewMap.find(pViewNode);
    if (it != m_mpViewMap.end()) {
      const TDisplayVec &vcDisplay = it->second;

      // Walk over display vector and remove the shit invalidate the shit
      for (size_t i = 0; i < vcDisplay.size(); i++) {
        // Invalidate the displaylist and get the drawdef
        COpenGLScene::invalidate(*vcDisplay[i], false);
        IDrawDef *pDrawDef = const_cast<IDrawDef *>(&DrawDef(*vcDisplay[i]));

        // Delete from list and view map
        erase(*vcDisplay[i], false);

        // Delete the drawdef
        delete pDrawDef;
      }

      it->second.clear();

      // Reinsert the shit
      for (int i = 0; i < pViewNode->DisplayListSize(); i++) {
        // Create a new draw def ...
        IDrawDef *pDrawDef = OnCreateDrawDef(*pViewNode, pViewNode->DisplayList(i));
        assert(pDrawDef);

        // Assign to scene ...
        if (COpenGLScene::insert(pViewNode->DisplayList(i), *pDrawDef)) {
          assert(pDrawDef == &DrawDef(pViewNode->DisplayList(i)));
          it->second.push_back(&pViewNode->DisplayList(i));
        } else
          delete pDrawDef;
      }
    }

    OnOpenGLNodeModified(const_cast<COpenGLNode &>(*pViewNode), uHint);

    UpdateScene();
  }
}

void COpenGLSceneNode::OnNeighbourDeleted(const CGraphNode &node) {
  // Is our selected deleted?
  if (m_pSelected == &node)
    m_pSelected = 0;

  // Look up node in view map ...
  if (m_mpViewMap.find((COpenGLNode *)&node) != m_mpViewMap.end()) {
    // Get the deleted view node from map and it's displaylist
    COpenGLNode *pViewNode = (COpenGLNode *)(&node);
    TViewMap::iterator it = m_mpViewMap.find(pViewNode);
    assert(it != m_mpViewMap.end());
    const TDisplayVec &vcDisplay = it->second;

    for (size_t i = 0; i < vcDisplay.size(); i++) {
      // Invalidate the displaylist and get the drawdef
      //			COpenGLScene::invalidate(*vcDisplay[i]);
      IDrawDef *pDrawDef = const_cast<IDrawDef *>(&DrawDef(*vcDisplay[i]));

      // Delete from list and view map
      erase(*vcDisplay[i], false);

      // Delete the drawdef
      delete pDrawDef;
    }

    m_mpViewMap.erase(pViewNode);

    // Inform the frame work that the opengl node is deleted
    OnOpenGLNodeDeleted((COpenGLNode &)(node));

    UpdateScene();
  }

  CStorageNode::OnNeighbourDeleted(node);
}

void COpenGLSceneNode::InvalidateAll() {
  for (TViewMap::iterator it = m_mpViewMap.begin(); it != m_mpViewMap.end(); it++) {
    const TDisplayVec &vcDisplay = it->second;
    for (size_t i = 0; i < vcDisplay.size(); i++) {
      invalidate(*vcDisplay[i], false);
      UpdateScene();
    }
  }
}

void COpenGLSceneNode::Clear() {
  // If we have an selected, unlink him
  if (m_pSelected)
    UnLink(*m_pSelected);

  assert(m_pSelected == 0);

  // Get all attached viewable nodes in a set ...
  typedef std::set<const COpenGLNode *, CGraphNode::CPtrLess> TViewNodeSet;
  TViewNodeSet stViewNode;
  for (TViewMap::iterator it = m_mpViewMap.begin(); it != m_mpViewMap.end(); it++)
    VERIFY(stViewNode.insert(it->first).second);

  if (stViewNode.size() == 0)
    return;

  // Unlink them ....
  {
    COperation oper(Model());

    for (TViewNodeSet::iterator itt = stViewNode.begin(); itt != stViewNode.end(); itt++) {
      assert(IsLinkedTo(**itt));
      UnLink(const_cast<COpenGLNode &>(**itt));
    }
  }

  UpdateFrame();
}

unsigned int COpenGLSceneNode::IconId() const { return 0; }

unsigned int COpenGLSceneNode::TypeId() const { return 0; }

bool COpenGLSceneNode::Fill() const { return m_bFill; }

void COpenGLSceneNode::Fill(bool bFill, bool bUpdate) {
  if (m_bFill != bFill) {
    m_bFill = bFill;
    if (bUpdate) {
      UpdateFrame();
      // InvalidateAll();
      // Modified();
    }
  }
}

void COpenGLSceneNode::Render(const COpenGLNodeBase &node) {
  const COpenGLNode *pViewNode = dynamic_cast<const COpenGLNode *>(&node);
  if (pViewNode) {
    if (!IsLinkedTo(*pViewNode))
      LinkTo(const_cast<COpenGLNode &>(*pViewNode));
  }

  const COpenGLComposite *pComposite = dynamic_cast<const COpenGLComposite *>(&node);
  if (pComposite) {
    COpenGLComposite::TOpenGLNodeSet stNode = pComposite->OpenGLNodes();
    for (COpenGLComposite::TOpenGLNodeSet::const_iterator it = stNode.begin(); it != stNode.end(); it++) {
      Render(**it);
    }
  }
}

bool COpenGLSceneNode::operator==(const COpenGLSceneNode &rhs) const {
  if (!CGraphNode::operator==(rhs))
    return false;

  if (!COpenGLScene::operator==(rhs))
    return false;

  if (m_bFill != rhs.m_bFill)
    return false;

  return true;
}

// ##ModelId=3BE7AEBE016C
COpenGLSceneNode &COpenGLSceneNode::operator=(const COpenGLSceneNode &rhs) {
  CGraphNode::operator=(rhs);
  COpenGLScene::operator=(rhs);
  m_bFill = rhs.m_bFill;
  UpdateFrame();
  return *this;
}

void COpenGLSceneNode::OnOpenGLNodeDeleted(COpenGLNode &node) {}

void COpenGLSceneNode::OnOpenGLNodeModified(COpenGLNode &node, enum ModifiedHint uHint) {}

void COpenGLSceneNode::OnNewOpenGLNode(COpenGLNode &node) {}

void COpenGLSceneNode::OnSelect(CGraphNode &node) {
  if (CanConnectItem(node)) {
    Clear();
    m_pSelected = &node;
    // LinkTo(node);
    ConnectItem(node);
  } else
    Clear();
}

IDrawDef *COpenGLSceneNode::OnCreateDrawDef(const COpenGLNode &node, const geo::IObject &object) {
  CSceneDrawDef *sceneDrawDef = new CSceneDrawDef(*this, *node.OnCreateDrawDef(object));

  return sceneDrawDef;
}

bool COpenGLSceneNode::Empty() const { return true; }

long COpenGLSceneNode::SavedItems() const {
  // TODO : Create generic save load code ...
  return 0;
}

bool COpenGLSceneNode::ConnectItem(const CGraphNode &item) {
  const COpenGLNodeBase *pNode = dynamic_cast<const COpenGLNodeBase *>(&item);
  if (pNode)
    Render(*pNode);

  return true;
}

bool COpenGLSceneNode::CanConnectItem(const CGraphNode &item) const {
  const COpenGLNodeBase *pNode = dynamic_cast<const COpenGLNodeBase *>(&item);
  return pNode != 0;
}

void COpenGLSceneNode::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  CStorageNode::LoadStream(stream, version, progress);

  // Check that the opengl scene entry is linked to the scene
  assert(Model().GraphEntry(MD_BASE_SCENE_NODE));
  if (!CGraphNode::parent())
    create(Model().GraphEntry(MD_BASE_SCENE_NODE));
}

void COpenGLSceneNode::SaveStream(TSTREAM &stream, TPROGRESS &progress) { CStorageNode::SaveStream(stream, progress); }

COpenGLSceneNode::eSelectMode COpenGLSceneNode::SelectMode() const { return SM_SELECT; }

void COpenGLSceneNode::UpdateLegend() {
  if (IsLegendFrame()) {
    LegendFrame().ClearLegendFrames();
    OnUpdateLegend();
    LegendFrame().UpdateFrame();
  }
}

bool COpenGLSceneNode::ReParent(IOpenGLFrame *pParent) {
  if (IsLegendFrame()) {
    // wjrx mantis 2550 start
    CFemAppOpenGLView *pView = dynamic_cast<CFemAppOpenGLView *>(Parent());

    if (pView && pView->LegendView())
      pView->LegendView()->BackgroundColor(BackgroundColor());
    // wjrx mantis 2550 end

    LegendFrame().ClearLegendFrames();
  }

  bool bRet = COpenGLScene::ReParent(pParent);

  if (Parent()) {
    UpdateFrame();
    UpdateLegend();
  }

  return bRet;
}

/*!
  Return the CLegendFrame attached to the scene. The client is able to create the legend in the OnUpdateLegend() call.

  \sa OnUpdateLegend()
*/
CLegendFrame &COpenGLSceneNode::LegendFrame() {
  CFemAppOpenGLView *pView = dynamic_cast<CFemAppOpenGLView *>(Parent());
  assert(pView);
  assert(pView->LegendView());
  assert(pView->LegendView()->ChildSize() == 1);
  CLegendFrame *pFrame = dynamic_cast<CLegendFrame *>(&pView->LegendView()->Child(0));
  assert(pFrame);
  return *pFrame;
}

/*!
  Return the CLegendFrame attached to the scene. The client is able to create the legend in the OnUpdateLegend() call.

  \sa OnUpdateLegend()
*/
bool COpenGLSceneNode::IsLegendFrame() const {
  const CFemAppOpenGLView *pView = dynamic_cast<const CFemAppOpenGLView *>(Parent());
  if (pView) {
    if (const_cast<CFemAppOpenGLView *>(pView)->LegendView()) {
      return const_cast<CFemAppOpenGLView *>(pView)->LegendView()->ChildSize() == 1;
    }
  }

  return false;
}

/*!
  The function is overrides by the client to customize the legend.
  The client can acces the legend frame by LegendFrame() function to create LegendSquare deratives.

  \sa LegendFrame()
*/
void COpenGLSceneNode::OnUpdateLegend() { InsertLinkedNodesInLegend(); }

/*!
  This function inserts node linked to the opengl scene in the legend when called during OnUpdateLegend().
  The linked nodes to the scene are attached to the groups they belong to.
  The OnInsertNodeInLegend() call back function is called so that the client can overide this function to filter the
  node. Default this function returns true. The function return the number of rejected nodes by OnInsertNodeInLegend()
  function.

  \sa OnInsertNodeInLegend(), OnUpdateLegend()
*/
int COpenGLSceneNode::InsertLinkedNodesInLegend() {
  // We fill the node map and divide the nodes in groups based on the TypeName()
  typedef std::set<const COpenGLNode *, CGraphNode::CLess> TNodeSet;
  typedef std::map<QString, TNodeSet> TColorMap;
  TColorMap mpNode;
  int nRejectedNodes = 0;

  TOpenGLNodeSet stNode = NodesInSceneForLegend();
  TOpenGLNodeSet::iterator itNode;
  for (itNode = stNode.begin(); itNode != stNode.end(); itNode++) {
    if (OnInsertNodeInLegend(**itNode)) {
      QString sTypeName = (*itNode)->TypeName();
      if ((sTypeName.length() > 0) && ((*itNode)->DisplayListSize() > 0)) {
        TColorMap::iterator it = mpNode.insert(TColorMap::value_type(sTypeName, TNodeSet())).first;
        it->second.insert(*itNode);
      }

    } else
      nRejectedNodes++;
  }

  // Insert the nodes in the legend
  for (TColorMap::iterator it = mpNode.begin(); it != mpNode.end(); it++) {
    TNodeSet &node_set = it->second;
    QString sName = it->first;

    // Create caption
    new CLegendFrame::legend_separator(LegendFrame(), 20);
    new CLegendFrame::legend_label(LegendFrame(), sName);
    new CLegendFrame::legend_separator(LegendFrame(), 10);
    for (TNodeSet::iterator ob_it = node_set.begin(); ob_it != node_set.end(); ob_it++) {
      new CLegendFrame::legend_index(LegendFrame(), 15, 15, (*ob_it)->Color(), (*ob_it)->Name());
      new CLegendFrame::legend_separator(LegendFrame(), 3);
    }
  }

  if (mpNode.size() > 0)
    new CLegendFrame::legend_separator(LegendFrame(), 20);

  return nRejectedNodes;
}

/*!
  Callback function of InsertLinkedNodesInLegend(). Default all the node are placed in the legend.

  \sa InsertLinkedNodesInLegend()
*/
bool COpenGLSceneNode::OnInsertNodeInLegend(const COpenGLNode &node) const { return true; }

bool COpenGLSceneNode::COpenGLNodePriorityLess::operator()(const COpenGLNode *p1, const COpenGLNode *p2) const {
  int prio1 = p1->SelectionPriority();
  int prio2 = p2->SelectionPriority();

  if (prio1 != prio2)
    return prio2 < prio1; // invert so higher priority becomes first

  return p1 < p2; // pointer comparison
}

void COpenGLSceneNode::OnBeginDraw(int nSteps) {
  if (nSteps > 100) {
    CGraphMainFrm *pFrame = dynamic_cast<CGraphMainFrm *>(FemAppGetMainWnd());
    assert(pFrame != 0);
    pFrame->StartProgress(_T("Drawing ..."), nSteps);
    m_bProgress = true;
  }
}

void COpenGLSceneNode::OnDrawStep(int nSteps) {
  if (m_bProgress) {
    CGraphMainFrm *pFrame = dynamic_cast<CGraphMainFrm *>(FemAppGetMainWnd());
    assert(pFrame != 0);
    pFrame->ProgressStep();
  }
}

void COpenGLSceneNode::OnEndDraw() {
  if (m_bProgress) {
    CGraphMainFrm *pFrame = dynamic_cast<CGraphMainFrm *>(FemAppGetMainWnd());
    assert(pFrame != 0);
    pFrame->EndProgress();
    m_bProgress = false;
  }
}
