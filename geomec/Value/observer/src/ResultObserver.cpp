// ResultObserver.cpp: implementation of the CResultObserver class.
//
//////////////////////////////////////////////////////////////////////

#include "ResultObserver.h"
#include "BaseEntryTypes.h"
#include "ContextMenuInvoker.h"
#include "DerivedResult.h"
#include "DerivedResult_Delegate.h"
#include "GVTResult.h"
#include "ISceneWrapper.h"
#include "MaterialResultTree.h"
#include "MaterialResultTree_Delegate.h"
#include "OIV2DHistoryScene.h"
#include "OIV2DSceneWrapper.h"
#include "ResultRegister.h"
#include "ResultTree.h"
#include "ResultTree_Delegate.h"
#include "Result_Delegate.h"
#include "SelectionObserver_Delegate.h"
#include "StaborResult.h"
#include "StaborResult_Delegate.h"
#include "geomec.h"
#include "openglscenebase.h"
#include "resourceIDI.h"
#include "resourceIDS.h"
#include "stdafx.h"
#include "treeviewbase.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
// #define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// IResultGroupObserver::CResultObserver::CResultComponentObserver Implementation
//////////////////////////////////////////////////////////////////////

IResultGroupObserver::CResultObserver::CResultComponentObserver::CResultComponentObserver(CResultObserver &parent,
                                                                                          int nComponent)
    : IGraphTreeObject(parent, TVI_LAST), m_nComponent(nComponent),
      m_pDelegate(static_cast<IResult_Delegate *>(ResultObserver().Result().getDelegate())) {
  Ctrl().SetItemData(Handle(), (DWORD_PTR)this);
}

IResultGroupObserver::CResultObserver::CResultComponentObserver::~CResultComponentObserver() {}

const IResultGroupObserver::CResultObserver &
IResultGroupObserver::CResultObserver::CResultComponentObserver::ResultObserver() const {
  return dynamic_cast<const CResultObserver &>(*Parent());
}

IResultGroupObserver::CResultObserver &
IResultGroupObserver::CResultObserver::CResultComponentObserver::ResultObserver() {
  return dynamic_cast<CResultObserver &>(*Parent());
}

QString IResultGroupObserver::CResultObserver::CResultComponentObserver::Text() const {
  assert(ResultObserver().Result().ModeSize() > 0);
  return ResultObserver().Result().Component(m_nComponent).Name();
}

unsigned int IResultGroupObserver::CResultObserver::CResultComponentObserver::Icon() const {
  assert(ResultObserver().Result().ModeSize() > 0);
  return ResultObserver().Result().Component(m_nComponent).IconId();
}

unsigned int IResultGroupObserver::CResultObserver::CResultComponentObserver::StateIcon() const {
  return ResultObserver().ResultComponentStateIconId(ResultObserver().Result(), m_nComponent);
}

void IResultGroupObserver::CResultObserver::CResultComponentObserver::ToggleState() {
  ResultObserver().ResultComponentToggleState(ResultObserver().Result(), m_nComponent);
}

/* NOT OBSOLETE */ void
IResultGroupObserver::CResultObserver::CResultComponentObserver::AppendContextMenu(CContextMenuInvoker &invoker) {
  ResultObserver().AppendResultComponentContextMenu(*m_pDelegate, m_nComponent, invoker);
}

//////////////////////////////////////////////////////////////////////
// IResultGroupObserver::CResultObserver Implementation
//////////////////////////////////////////////////////////////////////

IResultGroupObserver::CResultObserver::CResultObserver(IResult &result, IResultGroupObserver &parent)
    : CTreeNode(parent, TVI_LAST), m_result(result),
      m_pDelegate(static_cast<IResult_Delegate *>(result.getDelegate())) {
  Observer().reParent(&result);
  Ctrl().SetItemData(Handle(), (DWORD_PTR)this);

  // Create results component if nescesarry
  assert(result.ModeSize() > 0);
  if (result.ComponentSize() > 1) {
    for (int i = 0; i < result.ComponentSize(); i++) {
      new CResultComponentObserver(*this, i);
    }
  }
}

IResultGroupObserver::CResultObserver::~CResultObserver() {}

const IResult &IResultGroupObserver::CResultObserver::Result() const { return m_result; }

IResult &IResultGroupObserver::CResultObserver::Result() { return m_result; }

const IResultGroupObserver &IResultGroupObserver::CResultObserver::ResultGroupObserver() const {
  return dynamic_cast<const IResultGroupObserver &>(*Parent());
}

IResultGroupObserver &IResultGroupObserver::CResultObserver::ResultGroupObserver() {
  return dynamic_cast<IResultGroupObserver &>(*Parent());
}

unsigned int IResultGroupObserver::CResultObserver::ResultComponentStateIconId(const IValueComposite &result,
                                                                               int nComponent) const {
  return ResultGroupObserver().ResultComponentStateIconId(result, nComponent);
}

void IResultGroupObserver::CResultObserver::ResultComponentToggleState(IValueComposite &result, int nComponent) {
  ResultGroupObserver().ResultComponentToggleState(result, nComponent);
}

void IResultGroupObserver::CResultObserver::AppendResultComponentContextMenu(IResult_Delegate &result, int nComponent,
                                                                             CContextMenuInvoker &invoker) {
  ResultGroupObserver().AppendResultComponentContextMenu(result, nComponent, invoker);
}

unsigned int IResultGroupObserver::CResultObserver::StateIcon() const {
  assert(m_result.ModeSize() > 0);
  assert(m_result.ComponentSize() > 0);
  if (m_result.ComponentSize() > 1)
    return ResultGroupObserver().ResultStateIconId(m_result);

  return ResultGroupObserver().ResultComponentStateIconId(m_result, 0);
}

void IResultGroupObserver::CResultObserver::ToggleState() {
  assert(m_result.ModeSize() > 0);
  assert(m_result.ComponentSize() > 0);
  if (m_result.ComponentSize() > 1)
    ResultGroupObserver().ResultToggleState(m_result);
  else
    ResultGroupObserver().ResultComponentToggleState(m_result, 0);
  ResultGroupObserver().Update();
}

/* NOT OBSOLETE */ void IResultGroupObserver::CResultObserver::AppendContextMenu(CContextMenuInvoker &invoker) {
  assert(m_result.ModeSize() > 0);
  assert(m_result.ComponentSize() > 0);
  if (m_pDelegate) {
    if (m_result.ComponentSize() > 1)
      // Delegate().
      ResultGroupObserver().AppendResultContextMenu(*m_pDelegate, invoker);
    else
      // Delegate().
      ResultGroupObserver().AppendResultComponentContextMenu(*m_pDelegate, 0, invoker);
  }
}

const CGraphNode &IResultGroupObserver::CResultObserver::ObservedItem() const { return m_result; }

CGraphNode &IResultGroupObserver::CResultObserver::ObservedItem() { return m_result; }

const CGraphNode_Delegate &IResultGroupObserver::CResultObserver::Delegate() const { return *m_pDelegate; }

CGraphNode_Delegate &IResultGroupObserver::CResultObserver::Delegate() { return *m_pDelegate; }

//////////////////////////////////////////////////////////////////////
// IResultGroupObserver Implementation
//////////////////////////////////////////////////////////////////////

IResultGroupObserver::IResultGroupObserver(CResultGroup &group, IResultGroupObserver &parent)
    : CChildEnumerator<CGraphNode>(parent, TVI_LAST), m_group(group),
      m_pDelegate(static_cast<CResultGroup_Delegate *>(group.getDelegate())) {
  Observer().reParent(&group);
  Ctrl().SetItemData(Handle(), (DWORD_PTR)this);
  //	Update();
}

IResultGroupObserver::IResultGroupObserver(CResultGroup &group, CTreeCtrl &ctrl, HTREEITEM hParent,
                                           HTREEITEM hInsertAfter)
    : CChildEnumerator<CGraphNode>(ctrl, hParent, hInsertAfter), m_group(group),
      m_pDelegate(static_cast<CResultGroup_Delegate *>(group.getDelegate())) {
  Observer().reParent(&group);
  Ctrl().SetItemData(Handle(), (DWORD_PTR)this);
  //	Update();
}

IResultGroupObserver::~IResultGroupObserver() {}

const IResultGroupObserver *IResultGroupObserver::ResultGroupObserver() const {
  return dynamic_cast<const IResultGroupObserver *>(Parent());
}

IResultGroupObserver *IResultGroupObserver::ResultGroupObserver() {
  return dynamic_cast<IResultGroupObserver *>(Parent());
}

const CGraphNode &IResultGroupObserver::ObservedItem() const { return m_group; }

CGraphNode &IResultGroupObserver::ObservedItem() { return m_group; }

const CGraphNode_Delegate &IResultGroupObserver::Delegate() const { return *m_pDelegate; }

CGraphNode_Delegate &IResultGroupObserver::Delegate() { return *m_pDelegate; }

unsigned int IResultGroupObserver::ResultGroupStateIconId(const CResultGroup &group) const {
  if (ResultGroupObserver())
    return ResultGroupObserver()->ResultGroupStateIconId(group);
  return 0;
}

void IResultGroupObserver::ResultGroupToggleState(CResultGroup &group) {
  if (ResultGroupObserver())
    ResultGroupObserver()->ResultGroupToggleState(group);
}

void IResultGroupObserver::AppendResultGroupContextMenu(CResultGroup_Delegate &group_Delegate,
                                                        CContextMenuInvoker &invoker) {
  if (ResultGroupObserver())
    ResultGroupObserver()->AppendResultGroupContextMenu(group_Delegate, invoker);
}

unsigned int IResultGroupObserver::ResultStateIconId(const IValueComposite &result) const {
  if (ResultGroupObserver())
    return ResultGroupObserver()->ResultStateIconId(result);
  return 0;
}

void IResultGroupObserver::ResultToggleState(IValueComposite &result) {
  if (ResultGroupObserver())
    ResultGroupObserver()->ResultToggleState(result);
}

void IResultGroupObserver::AppendResultContextMenu(IResult_Delegate &result, CContextMenuInvoker &invoker) {
  if (ResultGroupObserver())
    ResultGroupObserver()->AppendResultContextMenu(result, invoker);
}

unsigned int IResultGroupObserver::ResultComponentStateIconId(const IValueComposite &result, int nComponent) const {
  if (ResultGroupObserver())
    return ResultGroupObserver()->ResultComponentStateIconId(result, nComponent);
  return 0;
}

void IResultGroupObserver::ResultComponentToggleState(IValueComposite &result, int nComponent) {
  if (ResultGroupObserver())
    ResultGroupObserver()->ResultComponentToggleState(result, nComponent);
}

void IResultGroupObserver::AppendResultComponentContextMenu(const IResult_Delegate &result, int nComponent,
                                                            CContextMenuInvoker &invoker) {
  if (ResultGroupObserver())
    ResultGroupObserver()->AppendResultComponentContextMenu(result, nComponent, invoker);
}

unsigned int IResultGroupObserver::StateIcon() const {
  if (ResultGroupObserver())
    return ResultGroupStateIconId(m_group);
  return ResultGroupStateIconId(m_group);
}

void IResultGroupObserver::ToggleState() {
  if (ResultGroupObserver())
    ResultGroupObserver()->ResultGroupToggleState(m_group);
  else
    ResultGroupToggleState(m_group);
}

/* NOT OBSOLETE */ void IResultGroupObserver::AppendContextMenu(CContextMenuInvoker &invoker) {
  if (ResultGroupObserver())
    ResultGroupObserver()->AppendResultGroupContextMenu(*m_pDelegate, invoker);
  else
    AppendResultGroupContextMenu(*m_pDelegate, invoker);
}

CTreeNode *IResultGroupObserver::InsertChild(CGraphNode &node) {
  CResultGroup *pGroup = dynamic_cast<CResultGroup *>(&node);
  if (pGroup)
    return new IResultGroupObserver(*pGroup, *this);
  IResult *pResult = dynamic_cast<IResult *>(&node);
  if (pResult)
    return new CResultObserver(*pResult, *this);
  assert(FALSE);
  return 0;
}

BOOL IResultGroupObserver::OnFilter(const CGraphNode &node) const {
  // Only insert children who are result groups or results
  const CResultGroup *pGroup = dynamic_cast<const CResultGroup *>(&node);
  const IResult *pResult = dynamic_cast<const IResult *>(&node);
  if (pResult || pGroup) {
    if (ObservedItem().isChild(node) > -1) {
      if (pGroup)
        return CanInsertResultGroup(*pGroup);
      return CanInsertResult(*pResult);
    }
  }
  return FALSE;
}

BOOL IResultGroupObserver::CanInsertResult(const IResult &result) const {
  if (ResultGroupObserver())
    return ResultGroupObserver()->CanInsertResult(result);
  return TRUE;
}

BOOL IResultGroupObserver::CanInsertResultGroup(const CResultGroup &group) const {
  if (ResultGroupObserver())
    return ResultGroupObserver()->CanInsertResultGroup(group);
  return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CResultGroupObserver Implementation
//////////////////////////////////////////////////////////////////////
CResultGroupObserver::CResultGroupObserver(CResultGroup &group, CTreeViewBase &view, HTREEITEM hParent,
                                           bool bShowAnalysisTypesAndDimensions, bool bShowDepletions)
    : IResultGroupObserver(group, view.GetTreeCtrl(), hParent, TVI_LAST), m_pDepletionStageBranch(0) {
  // This is the root constructor. Add linearity branch and depletion stage branches.
  if (bShowDepletions)
    m_pDepletionStageBranch = new CDepletionStageBranch(*this, ResultRegister().DepletionStageEntry());

  if (bShowAnalysisTypesAndDimensions) {
    new CLinearityBranch(*this);
    if (((CModelBase &)group.Model()).ResultRegisterSize() > 1)
      new CDimensionBranch(*this);
  }

  Update();
  SortChildren();
}

CResultGroupObserver::CResultGroupObserver(CResultGroup &group, CResultGroupObserver &parent)
    : IResultGroupObserver(group, parent), m_pDepletionStageBranch(0) {
  Update();
}

CResultRegister &CResultGroupObserver::ResultRegister() {
  int nRegister = PRIMARY_RESULT;
  CResultGroup *pGroup = dynamic_cast<CResultGroup *>(&ObservedItem());
  assert(pGroup);
  CGeomecDoc *pDoc = GetGeomecDoc();

  if (pDoc->CurrentScene()) {
    const IResultComponent *pResultComponent =
        dynamic_cast<const IResultComponent *>(pDoc->CurrentScene()->ValueComponent());
    if (pResultComponent)
      nRegister = pResultComponent->RegisterIndex();
  }

  return ((CModelBase &)pGroup->Model()).ResultRegister(nRegister);
}

void CResultGroupObserver::Update() {
  if (m_pDepletionStageBranch) {
    if (&ResultRegister().DepletionStageEntry() != &m_pDepletionStageBranch->ObservedItem()) {
      delete m_pDepletionStageBranch;
      m_pDepletionStageBranch = new CDepletionStageBranch(*this, ResultRegister().DepletionStageEntry());
    }
  }

  IResultGroupObserver::Update();
}

unsigned int CResultGroupObserver::ResultGroupStateIconId(const CResultGroup &group) const { return 0; }

void CResultGroupObserver::ResultGroupToggleState(CResultGroup &group) {}

/* NOT OBSOLETE */ void CResultGroupObserver::AppendResultGroupContextMenu(CResultGroup_Delegate &group_Delegate,
                                                                           CContextMenuInvoker &invoker) {
  group_Delegate.AppendContextMenu(invoker);
}

BOOL CResultGroupObserver::CanInsertResultGroup(const CResultGroup &group) const { return TRUE; }

unsigned int CResultGroupObserver::ResultStateIconId(const IValueComposite &result) const { return 0; }

void CResultGroupObserver::ResultToggleState(IValueComposite &result) {}

/* NOT OBSOLETE */ void CResultGroupObserver::AppendResultContextMenu(IResult_Delegate &result,
                                                                      CContextMenuInvoker &invoker) {
  result.AppendContextMenu(invoker);
}

BOOL CResultGroupObserver::CanInsertResult(const IResult &result) const { return TRUE; }

// Handlers result components
unsigned int CResultGroupObserver::ResultComponentStateIconId(const IValueComposite &result, int nComponent) const {
  // Fetch the scene from the document
  CGeomecDoc *pDoc = GetGeomecDoc();

  ISceneWrapper *pScene = pDoc->CurrentScene();
  if (pScene) {
    // We have a scene. Is it displaying our result component?
    const IResultComponent *pResultComponent = 0;
    for (size_t i = 0; i < pScene->NumberOfValueComponents(); ++i) {
      try {
        pResultComponent = dynamic_cast<const IResultComponent *>(pScene->ValueComponent(i));
      } catch (...) {
        pResultComponent = 0;
      }
      for (unsigned int nMode = 0; nMode < result.ModeSize(); nMode++) {
        if (pResultComponent == &result.Component(nComponent, nMode))
          return pScene->MultipleValueComponentsAllowed() ? IDI_CHECKED : IDI_RADIO_CHECKED;
      }
    }

    // Can one of our components connect to the currentscene?
    for (unsigned int nMode = 0; nMode < result.ModeSize(); nMode++) {
      // If we can connect then we're unchecked ..
      if (pDoc->CurrentScene()->CanConnectItem(result.Component(nComponent, nMode)))
        return pScene->MultipleValueComponentsAllowed() ? IDI_UNCHECKED : IDI_RADIO_UNCHECKED;
    }

    return pScene->MultipleValueComponentsAllowed() ? IDI_CHECK_DISABLED : IDI_RADIO_DISABLED;
  }

  // No component can connect to the scene. It seems that we're disabled..
  return IDI_CHECK_DISABLED;
}

void CResultGroupObserver::ResultComponentToggleState(IValueComposite &result, int nComponent) {
  IResult *pResult = dynamic_cast<IResult *>(&result);

  if (!pResult)
    return;

  // Make the think current
  CGeomecDoc *pDoc = GetGeomecDoc();

  if (!pDoc->CurrentScene())
    return;

  ISceneWrapper *pScene = pDoc->CurrentScene();
  if (pScene) {
    const IResultComponent *pResultComponent = 0;
    if (!pScene->IsOpenInventorScene()) // disabling the radiobutton's checkbox behavior; first for main view; TODO:
                                        // check History/Wells views
    {
      for (size_t i = 0; i < pScene->NumberOfValueComponents(); ++i) {
        pResultComponent = dynamic_cast<const IResultComponent *>(pScene->ValueComponent(i));

        for (unsigned int nMode = 0; nMode < result.ModeSize(); nMode++) {
          if (pResultComponent == &result.Component(nComponent, nMode)) {
            pDoc->CurrentScene()->UnLink(*pResultComponent);
            return;
          }
        }
      }
    }

    // Determine the current depletion stage and analysis type

    std::vector<IResultComponent *> toConnect;
    for (size_t i = 0; i < pScene->NumberOfValueComponents(); ++i) {
      CAnalysisType antype(determineInitialAnalysisType());
      int nRegister = PRIMARY_RESULT;

      const CDepletionStage *pStage = &((CModelBase &)result.Model()).InitialDepletionStage();

      pResultComponent = dynamic_cast<const IResultComponent *>(pScene->ValueComponent(i));
      if (pResultComponent) {
        antype = pResultComponent->AnalysisType();
        pStage = &pResultComponent->Stage();
        nRegister = pResultComponent->RegisterIndex();
      }

      // Try to connect
      IResultComponent *pNewResult = pResult->ResultComponent(*pStage, antype, nRegister, nComponent);
      if (pNewResult) {
        if (pResultComponent == pNewResult)
          return;
        if (pDoc->CurrentScene()->CanConnectItem(*pNewResult)) {
          toConnect.push_back(pNewResult);
        }
      }
    }

    if (!toConnect.empty()) {
      for_each(toConnect.begin(), toConnect.end(),
               [pDoc](IResultComponent *pResult) { pDoc->CurrentScene()->ConnectItem(*pResult); });
      return;
    }

    // Current mode is not suitable for the result component, but we must find another
    // which is valid and resembles the most to the current mode. First we try another depletion
    // stage.
    // Search for a suitable depletion stage and a suitable analysis type

    CAnalysisType antype(determineInitialAnalysisType());
    int nRegister = PRIMARY_RESULT;
    const CDepletionStage *pStage = &((CModelBase &)result.Model()).InitialDepletionStage();

    CModelBase &model = (CModelBase &)pScene->Model();
    for (int nRegCount = 0; nRegCount < model.ResultRegisterSize(); nRegCount++) {
      int nNewRegister = (nRegCount + nRegister) % model.ResultRegisterSize();
      const CDepletionStageEntry &dp_entry = model.ResultRegister(nRegister).DepletionStageEntry();

      for (int nDepCount = 0; nDepCount < dp_entry.EntryNodes().size(); nDepCount++) {
        int nNewTimeStep = (nDepCount + pStage->Index()) % dp_entry.EntryNodes().size();

        // Try current analysis type first
        if (pResult->ResultComponent(nNewTimeStep, antype, nNewRegister, nComponent)) {
          if (pScene->CanConnectItem(*pResult->ResultComponent(nNewTimeStep, antype, nNewRegister, nComponent))) {
            pScene->ConnectItem(*pResult->ResultComponent(nNewTimeStep, antype, nNewRegister, nComponent));
            return;
          }
        }

        CAnalysisType at = CAnalysisType::FirstType();
        while (at.Valid()) {
          if (at != antype) {
            if (pResult->ResultComponent(nNewTimeStep, at, nNewRegister, nComponent)) {
              if (pScene->CanConnectItem(*pResult->ResultComponent(nNewTimeStep, at, nNewRegister, nComponent))) {
                pScene->ConnectItem(*pResult->ResultComponent(nNewTimeStep, at, nNewRegister, nComponent));
                return;
              }
            }
          }

          at = at.NextType();
        }
      }
    }
  }
}

/* NOT OBSOLETE */ void CResultGroupObserver::AppendResultComponentContextMenu(const IResult_Delegate &result,
                                                                               int nComponent,
                                                                               CContextMenuInvoker &invoker) {
  (const_cast<IResult_Delegate &>(result)).AppendContextMenu(invoker);
}

CTreeNode *CResultGroupObserver::InsertChild(CGraphNode &node) {
  CGVTResultGroup *pGVTGroup = dynamic_cast<CGVTResultGroup *>(&node);
  if (pGVTGroup)
    return new CGVTResultGroupObserver(*pGVTGroup, *this);
  CResultGroup *pGroup = dynamic_cast<CResultGroup *>(&node);
  if (pGroup)
    return new CResultGroupObserver(*pGroup, *this);
  IResult *pResult = dynamic_cast<IResult *>(&node);
  if (pResult)
    return new CResultObserver(*pResult, *this);
  assert(FALSE);
  return 0;
}

BOOL CResultGroupObserver::operator<(const ICtrlObjectBase &object) const {
  // Default the less operator of the graph nodes handles the order of the
  // results and resultgroups. Only the linearity and depletionstage branches
  // are handled here.
  if (dynamic_cast<const CLinearityBranch *>(&object))
    return TRUE;

  if (dynamic_cast<const CDepletionStageBranch *>(&object))
    return TRUE;

  return FALSE;
}

CAnalysisType::TAnalysisType CResultGroupObserver::determineInitialAnalysisType() {
  if (ResultRegister().NonLinear()) {
    return CAnalysisType::AT_NONLIN;
  } else if (ResultRegister().Linear()) {
    return CAnalysisType::AT_LINEAR;
  } else if (ResultRegister().Mixture()) {
    return CAnalysisType::AT_MIXTURE;
  } else if (ResultRegister().Heat()) {
    return CAnalysisType::AT_HEAT;
  } else if (ResultRegister().MixtureContainment()) {
    return CAnalysisType::AT_MIXTURE_CONTAINMENT;
  }

  return CAnalysisType::AT_NONLIN;
}

CGVTResultGroupObserver::CGVTResultGroupObserver(CResultGroup &group, CTreeViewBase &view, HTREEITEM hParent,
                                                 bool bShowAnalysisTypesAndDimensions, bool bShowDepletions)
    : CResultGroupObserver(group, view, hParent, bShowAnalysisTypesAndDimensions, bShowDepletions) {}

CGVTResultGroupObserver::CGVTResultGroupObserver(CResultGroup &group, CResultGroupObserver &parent)
    : CResultGroupObserver(group, parent) {}

BOOL CGVTResultGroupObserver::OnFilter(const CGraphNode &node) const {
  if (dynamic_cast<const TDeltaT *>(&node) || dynamic_cast<const TDeltaV *>(&node))
    return TRUE;

  return CResultGroupObserver::OnFilter(node);
}

CTreeNode *CGVTResultGroupObserver::InsertChild(CGraphNode &node) {
  if (dynamic_cast<const TDeltaT *>(&node) || dynamic_cast<const TDeltaV *>(&node))
    return new CGVTOutputObserver(static_cast<IValueComposite &>(node), *this);
  else
    return CResultGroupObserver::InsertChild(node);
}

CGVTOutputObserver::CGVTOutputObserver(IValueComposite &node, CTreeNode &parent)
    : CValueCompositeObserver(node, parent, true, UNLINK_ITEM), m_selected(false) {}

unsigned int CGVTOutputObserver::StateIcon() const {
  CGeomecDoc *pDoc = GetGeomecDoc();
  if (pDoc->CurrentScene() && pDoc->CurrentScene()->IsOpenInventorScene()) {
    if (pDoc->CurrentScene()->IsLinkedTo(ObservedItem()))
      return IDI_RADIO_CHECKED;
    return IDI_RADIO_UNCHECKED;
  }
  return IDI_RADIO_DISABLED;
}

void CGVTOutputObserver::ToggleState() {
  CGeomecDoc *pDoc = GetGeomecDoc();
  ISceneWrapper *pScene = pDoc->CurrentScene();
  if (pScene && pScene->IsOpenInventorScene()) {
    CGraphNode &node = ObservedItem();
    if (pScene->CanConnectItem(node)) {
      for (size_t i = 0; i < pScene->NumberOfValueComponents(); ++i)
        pScene->UnLink(*pScene->ValueComponent(i));

      pScene->LinkTo(node);
    } else
      pScene->UnLink(node);
  }
}

/*
//////////////////////////////////////////////////////////////////////
// CLocalResultGroupObserver
//////////////////////////////////////////////////////////////////////
bool CLocalResultGroupObserver::AllSelected(const CResultGroup& group) const
{
  for(int i = 0; i < group.ChildSize(); i++)
  {
    const IResult *pResult = dynamic_cast<const IResult*>(&group.Child(i));
    if(pResult)
    {
      if(!AllSelected(*pResult))
        return false;
    }
    const CResultGroup *pResultGroup = dynamic_cast<const CResultGroup*>(&group.Child(i));
    if(pResultGroup)
    {
      if(!AllSelected(*pResultGroup))
        return false;
    }
  }

  return true;
}

bool CLocalResultGroupObserver::AllNotSelected(const CResultGroup& group) const
{
  for(int i = 0; i < group.ChildSize(); i++)
  {
    const IResult *pResult = dynamic_cast<const IResult*>(&group.Child(i));
    if(pResult)
    {
      if(!AllNotSelected(*pResult))
        return false;
    }
    const CResultGroup *pResultGroup = dynamic_cast<const CResultGroup*>(&group.Child(i));
    if(pResultGroup)
    {
      if(!AllNotSelected(*pResultGroup))
        return false;
    }
  }

  return true;
}

bool CLocalResultGroupObserver::AllSelected(const IResult& result) const
{
  assert(result.ModeSize() > 0);
  for(int i = 0; i < result. ComponentSize(); i++)
  {
    if(!ResultComponent(result, i))
      return false;
  }

  return true;
}

bool CLocalResultGroupObserver::AllNotSelected(const IResult& result) const
{
  assert(result.ModeSize() > 0);
  for(int i = 0; i < result.ComponentSize(); i++)
  {
    if(ResultComponent(result, i))
      return false;
  }

  return true;
}

void CLocalResultGroupObserver::SelectAll(CResultGroup& group)
{
  for(int i = 0; i < group.ChildSize(); i++)
  {
    IResult *pResult = dynamic_cast<IResult*>(&group.Child(i));
    if(pResult)
      SelectAll(*pResult);
    CResultGroup *pResultGroup = dynamic_cast<CResultGroup*>(&group.Child(i));
    if(pResultGroup)
      SelectAll(*pResultGroup);
  }
  Update();
}

void CLocalResultGroupObserver::DeselectAll(CResultGroup& group)
{
  for(int i = 0; i < group.ChildSize(); i++)
  {
    IResult *pResult = dynamic_cast<IResult*>(&group.Child(i));
    if(pResult)
      DeselectAll(*pResult);
    CResultGroup *pResultGroup = dynamic_cast<CResultGroup*>(&group.Child(i));
    if(pResultGroup)
      DeselectAll(*pResultGroup);
  }
  Update();
}

void CLocalResultGroupObserver::SelectAll(IResult& result)
{
  assert(result.ModeSize() > 0);
  for(int i = 0; i < result.ComponentSize(); i++)
    ResultComponent(result, i, true);
  Update();
}

void CLocalResultGroupObserver::DeselectAll(IResult& result)
{
  assert(result.ModeSize() > 0);
  for(int i = 0; i < result.ComponentSize(); i++)
    ResultComponent(result, i, false);
  Update();
}

BOOL CLocalResultGroupObserver::CanInsertResult(const IResult& result) const
{
  return Dlg().Valid(result);
}

BOOL CLocalResultGroupObserver::CanInsertResultGroup(const CResultGroup& group) const
{
  return Valid(group);
}

BOOL CLocalResultGroupObserver::Valid(const CResultGroup& group) const
{
  for(int i = 0; i < group.ChildSize(); i++)
  {
    const IResult *pResult = dynamic_cast<const IResult*>(&group.Child(i));
    if(pResult)
    {
      if(Dlg().Valid(*pResult))
        return TRUE;
    }
    const CResultGroup *pResultGroup = dynamic_cast<const CResultGroup*>(&group.Child(i));
    if(pResultGroup)
    {
      if(Valid(*pResultGroup))
        return TRUE;
    }
  }

  return FALSE;
}

CLocalResultGroupObserver::CLocalResultGroupObserver(CResultTree& tree, CTreeCtrlBase &m_tcResults)
: IResultGroupObserver(tree, m_tcResults, TVI_ROOT, TVI_LAST),
 // m_dlg(dlg)
{
  Ctrl().SetItemData(Handle(), (DWORD_PTR) this);
}

unsigned int CLocalResultGroupObserver::ResultGroupStateIconId(const CResultGroup& group) const
{
  if(AllNotSelected(group))
    return IDI_UNCHECKED;

  if(AllSelected(group))
    return IDI_CHECKED;

  return IDI_TRI_CHECKED;
}

void CLocalResultGroupObserver::ResultGroupToggleState(CResultGroup& group)
{
  if(AllNotSelected(group))
    SelectAll(group);
  else if(AllSelected(group))
    DeselectAll(group);
  else SelectAll(group);
  Update();
}

void CLocalResultGroupObserver::AppendResultGroupContextMenu(CResultGroup_Delegate& group_Delegate, CContextMenuInvoker
&invoker)
{
  // No context menu
}

unsigned int CLocalResultGroupObserver::ResultStateIconId(const IResult& result) const
{
  assert(result.ModeSize() > 0);
  assert(result.ComponentSize() > 1);	// If component size == 1 we have a scalar
  if(AllNotSelected(result))
    return IDI_UNCHECKED;

  if(AllSelected(result))
    return IDI_CHECKED;

  return IDI_TRI_CHECKED;
}

void CLocalResultGroupObserver::ResultToggleState(IResult& result)
{
  assert(result.ModeSize() > 0);
  assert(result.ComponentSize() > 1);	// If component size == 1 we have a scalar
  if(AllNotSelected(result))
    SelectAll(result);
  else if(AllSelected(result))
    DeselectAll(result);
  else SelectAll(result);
}

void CLocalResultGroupObserver::AppendResultContextMenu(IResult_Delegate& result, CContextMenuInvoker &invoker)
{
  // No context menu
}

unsigned int CLocalResultGroupObserver::ResultComponentStateIconId(const IResult& result, int nComponent) const
{
  if(m_dlg.ResultComponent(result, nComponent))
    return IDI_CHECKED;
  return IDI_UNCHECKED;
}

void CLocalResultGroupObserver::ResultComponentToggleState(IResult& result, int nComponent)
{
  ResultComponent(result, nComponent, !ResultComponent(result, nComponent));
  Update();
}

void CLocalResultGroupObserver::AppendResultComponentContextMenu(const IResult_Delegate& result, int nComponent,
CContextMenuInvoker &invoker)
{
  // No context menu
}
*/
//////////////////////////////////////////////////////////////////////
// DepletionStageBranch
//////////////////////////////////////////////////////////////////////
CDepletionStageBranch::CDepletionStageBranch(CResultGroupObserver &parent, CDepletionStageEntry &entry)
    : TDepletionStageBranchBase(entry, parent.Ctrl(), FALSE, FIXED_ITEM, parent.Handle(), TVI_LAST, FALSE, FALSE) {
  Update();
}

static int CALLBACK StageCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
  CDepletionStageBranch::CDepletionStageObserver *pObject1 =
      ((CDepletionStageBranch::CDepletionStageObserver *)lParam1);
  CDepletionStageBranch::CDepletionStageObserver *pObject2 =
      ((CDepletionStageBranch::CDepletionStageObserver *)lParam2);

  const CDepletionStage &stage1 = dynamic_cast<const CDepletionStage &>(pObject1->ObservedItem());
  const CDepletionStage &stage2 = dynamic_cast<const CDepletionStage &>(pObject2->ObservedItem());
  assert(&stage1 && &stage2);
  if (stage1.Index() < stage2.Index())
    return -1;

  if (stage2.Index() < stage1.Index())
    return 1;

  return 0;
}

void CDepletionStageBranch::SortChildren() {
  TVSORTCB tvs;
  tvs.hParent = Handle();
  tvs.lpfnCompare = StageCompare;
  tvs.lParam = (LPARAM)&Ctrl();
  Ctrl().SortChildrenCB(&tvs);
}

CTreeNode *CDepletionStageBranch::InsertChild(CDepletionStage &stage) {
  return new CDepletionStageObserver(*this, stage);
}

void CDepletionStageBranch::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint) { Update(); }

//////////////////////////////////////////////////////////////////////
// DepletionStage leaf
//////////////////////////////////////////////////////////////////////
CDepletionStageBranch::CDepletionStageObserver::CDepletionStageObserver(CDepletionStageBranch &branch,
                                                                        CDepletionStage &stage)
    : TDepletionStageLeafBase(stage, branch, FALSE, FIXED_ITEM, TVI_LAST, FALSE, TRUE) {
  Update();
}

/**
 *
 */
std::vector<IResultComponent *> CDepletionStageBranch::CDepletionStageObserver::SwitchTo() const {
  const CDepletionStage &dep_stage1 = (const CDepletionStage &)(ObservedItem());
  std::vector<IResultComponent *> results;

  ISceneWrapper *scene = GetGeomecDoc()->CurrentScene();
  for (size_t i = 0; i < scene->NumberOfValueComponents(); ++i) {
    IResultComponent *pResultComponent = dynamic_cast<IResultComponent *>(scene->ValueComponent(i));
    if (!pResultComponent)
      continue;

    const CDepletionStage &dep_stage2 = (const CDepletionStage &)(ObservedItem());
    if (&pResultComponent->Stage() == &dep_stage2)
      continue;

    IResult *pResult = (IResult *)(&pResultComponent->Parent());
    assert(pResult);

    IResultComponent *pRes =
        pResult->ResultComponent(dep_stage2, pResultComponent->AnalysisType(), pResultComponent->RegisterIndex(),
                                 pResultComponent->ComponentIndex());

    if (pRes != 0 && pRes->Defined())
      results.push_back(pRes);
  }

  return results;
}

/**
 * Determines whether any of the currently displayed components has a result for this
 * depletion stage.
 */
bool CDepletionStageBranch::CDepletionStageObserver::CanSwitchTo() const {
  ISceneWrapper *scene = GetGeomecDoc()->CurrentScene();
  if (scene == 0)
    return false;

  const CDepletionStage &dep_stage = (const CDepletionStage &)(ObservedItem());
  if (!scene->CanConnectItem(dep_stage))
    return false;

  for (size_t i = 0; i < scene->NumberOfValueComponents(); ++i) {
    // Do we have a result component?
    IResultComponent *pResultComponent = dynamic_cast<IResultComponent *>(scene->ValueComponent(i));
    if (!pResultComponent)
      continue; // return false;

    // Are we already looking at this depletion stage?
    if (&pResultComponent->Stage() == &dep_stage)
      return true;

    IResult *pResult = (IResult *)(&pResultComponent->Parent());
    assert(pResult);

    // Do we have a result component for this depletion stage?
    IResultComponent *pRes =
        pResult->ResultComponent(dep_stage, pResultComponent->AnalysisType(), pResultComponent->RegisterIndex(),
                                 pResultComponent->ComponentIndex());

    if (pRes != 0 && pRes->Defined())
      return true;
  }

  return false;
}

/**
 * Determines whether any value components are being displayed for this stage. If so,
 * the corresponding UI element (radio button or checkbox) should be checked.
 */
bool CDepletionStageBranch::CDepletionStageObserver::IsChecked() const {
  ISceneWrapper *scene = GetGeomecDoc()->CurrentScene();
  if (!scene)
    return false;

  for (size_t i = 0; i < scene->NumberOfValueComponents(); ++i) {
    IResultComponent *resultComp = dynamic_cast<IResultComponent *>(scene->ValueComponent(i));
    if (resultComp != 0 && &resultComp->Stage() == &ObservedItem())
      return true;
  }

  return false;
}

bool CDepletionStageBranch::CDepletionStageObserver::MultipleStagesAllowed() const {
  ISceneWrapper *scene = GetGeomecDoc()->CurrentScene();
  if (!scene)
    return false;

  return scene->MultipleStagesAllowed();
}

/**
 * Counts the number of different depletion stages for which value components are displayed
 */
int CDepletionStageBranch::CDepletionStageObserver::NumDisplayedStages() const {
  ISceneWrapper *scene = GetGeomecDoc()->CurrentScene();
  if (!scene)
    return 0;

  std::vector<const CDepletionStage *> stages;
  for (size_t i = 0; i < scene->NumberOfValueComponents(); ++i) {
    IResultComponent *resultComp = dynamic_cast<IResultComponent *>(scene->ValueComponent(i));
    if (resultComp != 0) {
      const CDepletionStage *stage = &resultComp->Stage();
      if (std::find(stages.begin(), stages.end(), stage) == stages.end())
        stages.push_back(stage);
    }
  }

  return (int)stages.size();
}

IResultComponent *CDepletionStageBranch::CDepletionStageObserver::Current() const {
  ISceneWrapper *scene = GetGeomecDoc()->CurrentScene();

  if (!scene)
    return 0;

  if (scene->IsOIV2DHistoryScene())
    return 0;

  return dynamic_cast<IResultComponent *>(scene->ValueComponent());
}

unsigned int CDepletionStageBranch::CDepletionStageObserver::StateIcon() const {
  ISceneWrapper *scene = GetGeomecDoc()->CurrentScene();
  if (!scene)
    return IDI_RADIO_DISABLED;

  if (!MultipleStagesAllowed()) { // radio button
    if (IsChecked())
      return CanSwitchTo() ? IDI_RADIO_CHECKED : IDI_RADIO_DISABLED;
    else
      return CanSwitchTo() ? IDI_RADIO_UNCHECKED : IDI_RADIO_DISABLED;
  } else { // checkbox
    if (IsChecked())
      return CanSwitchTo() ? IDI_CHECKED : IDI_CHECK_DISABLED;
    else
      return CanSwitchTo() ? IDI_UNCHECKED : IDI_CHECK_DISABLED;
  }
}

void CDepletionStageBranch::CDepletionStageObserver::ToggleState() {
  ISceneWrapper *scene = GetGeomecDoc()->CurrentScene();
  if (scene == 0)
    return;

  // First disconnect all results for this depletion stage
  std::vector<IResultComponent *> toUnlink;
  const CDepletionStage *depStage = &(const CDepletionStage &)(ObservedItem());
  for (size_t i = 0; i < scene->NumberOfValueComponents(); ++i) {
    auto p = dynamic_cast<IResultComponent *>(scene->ValueComponent(i));
    if (p != 0 && depStage == &p->Stage())
      toUnlink.push_back(p);
  }

  if (!toUnlink.empty()) {
    for (auto p : toUnlink)
      scene->UnLink(*p);
    return;
  }

  if (CanSwitchTo()) {
    // get the components that need to be added for this stage
    std::vector<IResultComponent *> resultComponents = SwitchTo();
    assert(!resultComponents.empty());

    // If multiple stages are not allowed, we first need to disconnect
    // the result components from other stages.
    if (!MultipleStagesAllowed()) {
      std::vector<const IValueComponentBase *> components;
      for (size_t i = 0; i < scene->NumberOfValueComponents(); ++i) {
        if (dynamic_cast<IResultComponent *>(scene->ValueComponent(i)))
          components.push_back(scene->ValueComponent(i));
      }

      for (auto vc : components)
        scene->UnLink(*vc);
    }

    // finally add the components for this stage
    for (auto p : resultComponents) {
      if (scene->CanConnectItem(*p))
        scene->ConnectItem(*p);
    }
  }
}

void CDepletionStageBranch::CDepletionStageObserver::OnMarkAsInitial() {
  CDepletionStage &dep_stage = (CDepletionStage &)(ObservedItem());
  dep_stage.MarkAsInitial();
}

bool CDepletionStageBranch::CDepletionStageObserver::IsMarkedAsInitial() const {
  const CDepletionStage &dep_stage = (const CDepletionStage &)(ObservedItem());
  return dep_stage.IsMarkedAsInitial();
}

/* NOT OBSOLETE */ void
CDepletionStageBranch::CDepletionStageObserver::AppendContextMenu(CContextMenuInvoker &invoker) {
  CDepletionStage &dep_stage = (CDepletionStage &)(ObservedItem());

  typedef CSingleCommandTemplate<CDepletionStage> TCommand;
  invoker.AddCommand(_T("&Mark as initial"),
                     *(new TCommand(dep_stage, &CDepletionStage::MarkAsInitial, &CDepletionStage::CanMarkAsInitial)));
}

//////////////////////////////////////////////////////////////////////
// LinearityBranch
//////////////////////////////////////////////////////////////////////
CLinearityBranch::CLinearityBranch(CResultGroupObserver &group_observer) : IGraphTreeObject(group_observer, TVI_LAST) {
  Ctrl().SetItemData(Handle(), (DWORD_PTR)this);
  new CLinearityLeaf(*this, CAnalysisType::AT_LINEAR);
  new CLinearityLeaf(*this, CAnalysisType::AT_NONLIN);
  new CLinearityLeaf(*this, CAnalysisType::AT_HEAT);
  new CLinearityLeaf(*this, CAnalysisType::AT_MIXTURE);
  new CLinearityLeaf(*this, CAnalysisType::AT_MIXTURE_CONTAINMENT);
}

QString CLinearityBranch::Text() const { return getStringTableEntry(IDS_ANALYSISTYPE); }

unsigned int CLinearityBranch::Icon() const { return IDI_NON_AND_LINEAR; }
/*
BOOL CLinearityBranch::operator<(const ICtrlObjectBase &object) const
{
  if(dynamic_cast<const CDepletionStageBranch*>(&object))
    return FALSE;
  if(this == &object)
    return FALSE;

  return FALSE;
}
*/
//////////////////////////////////////////////////////////////////////
// LinearityLeaf
//////////////////////////////////////////////////////////////////////
CLinearityBranch::CLinearityLeaf::CLinearityLeaf(CLinearityBranch &linear_branch, const CAnalysisType &antype)
    : IGraphTreeObject(linear_branch, TVI_LAST), m_antype(antype) {
  Ctrl().SetItemData(Handle(), (DWORD_PTR)this);
  Update();
}

QString CLinearityBranch::CLinearityLeaf::Text() const { return m_antype.Label(); }

IResultComponent *CLinearityBranch::CLinearityLeaf::Current(size_t index) const {
  // We retrieve the root resultgroup from parent's parent via the observed item interface.
  const CTreeNode *pNode = dynamic_cast<const CTreeNode *>(Parent()->Parent());
  assert(pNode);
  const CResultGroup *pGroup = dynamic_cast<const CResultGroup *>(&pNode->ObservedItem());
  assert(pGroup);

  // Retrieving the opengl scene from the document.

  CGeomecDoc *pDoc = GetGeomecDoc();

  ISceneWrapper *pScene = pDoc->CurrentScene();
  if (pScene) {
    const IResultComponent *pResultComponent = 0;

    pResultComponent = dynamic_cast<const IResultComponent *>(pScene->ValueComponent(index));
    return (IResultComponent *)pResultComponent;
  }

  return 0;
}

IResultComponent *CLinearityBranch::CLinearityLeaf::SwitchTo() const {
  IResultComponent *pResultComponent = Current();
  if (!pResultComponent)
    return 0;

  IResult *pResult = (IResult *)(&pResultComponent->Parent());
  assert(pResult);

  // 	if(pResultComponent->AnalysisType() == m_antype)
  // 		return 0;
  /*
    IResultComponent* pRes = 0;

    if(pResultComponent->AnalysisType() == CAnalysisType::AT_LINEAR)
    {
    pRes = pResult->ResultComponent(pResultComponent->Stage(),
                CAnalysisType::AT_NONLIN,
                              pResultComponent->RegisterIndex(),
                              pResultComponent->ComponentIndex());
    }
    else if(pResultComponent->AnalysisType() == CAnalysisType::AT_NONLIN)
    {
    pRes = pResult->ResultComponent(pResultComponent->Stage(),
                CAnalysisType::AT_LINEAR,
                              pResultComponent->RegisterIndex(),
                              pResultComponent->ComponentIndex());
    }
  */
  IResultComponent *pRes = pResult->ResultComponent(
      pResultComponent->Stage(), m_antype, pResultComponent->RegisterIndex(), pResultComponent->ComponentIndex());

  if (pRes) {
    if (!pRes->Defined())
      return 0;
  }

  return pRes;
}

bool CLinearityBranch::CLinearityLeaf::CanSwitchTo() const {
  size_t index = 0;

  size_t numValueComponents = NumValueComponents();

  for (index = 0; index < numValueComponents; ++index) {
    auto pResultComponent = Current(index);
    if (pResultComponent == 0)
      continue;

    IResult *pResult = (IResult *)(&pResultComponent->Parent());
    assert(pResult);

    if (pResultComponent->AnalysisType() == m_antype)
      return false;

    IResultComponent *pRes = pResult->ResultComponent(
        pResultComponent->Stage(), m_antype, pResultComponent->RegisterIndex(), pResultComponent->ComponentIndex());
    if (!pRes)
      return false;

    if (!pRes->Defined())
      return false;
  }

  return true;
}

unsigned int CLinearityBranch::CLinearityLeaf::StateIcon() const {
  CGeomecDoc *pDoc = GetGeomecDoc();

  // Loop because not all value components are IResultComponents
  for (size_t i = 0; i < NumValueComponents(); ++i) {
    if (Current(i) && (Current(i)->AnalysisType() == m_antype))
      return IDI_RADIO_CHECKED;
  }

  if (CanSwitchTo())
    return IDI_RADIO_UNCHECKED;

  return IDI_RADIO_DISABLED;
}

unsigned int CLinearityBranch::CLinearityLeaf::Icon() const { return m_antype.IconId(); }

void CLinearityBranch::CLinearityLeaf::ToggleState() {
  IResultComponent *pFirstNewResultComponent = 0;

  CGeomecDoc *pDoc = GetGeomecDoc();
  if (!pDoc->CurrentScene())
    return;
  while (true) {
    if (/* Current() != 0 && see below */ Current() == pFirstNewResultComponent)
      break;

    IResultComponent *pResultComponent = SwitchTo();
    if (!pFirstNewResultComponent)
      pFirstNewResultComponent = pResultComponent;

    if (Current() != pResultComponent) {
      pDoc->CurrentScene()->UnLink(*Current());
      if (pResultComponent)
        pDoc->CurrentScene()->ConnectItem(*pResultComponent);
    }
    /* Not sure why this was added, but if we're not showing result components, we obviously don't have to update the
    values Especially not if we enter an endless loop (Bug 113173) I'll leave it in as comments for now, in case we do
    find some reason to change this else if (Current() == 0)
    {
      auto valueComponent = pDoc->CurrentScene()->ValueComponent();
      if (valueComponent)
      {
        pDoc->CurrentScene()->UnLink(* valueComponent);
        pDoc->CurrentScene()->ConnectItem(*valueComponent);
      }
      else
        break;
    }
    */
  }
}

size_t CLinearityBranch::CLinearityLeaf::NumValueComponents() const {
  size_t numValueComponents = 0;
  CGeomecDoc *pDoc = GetGeomecDoc();

  ISceneWrapper *pScene = pDoc->CurrentScene();
  if (pScene) {
    numValueComponents = pScene->NumberOfValueComponents();
  }
  return numValueComponents;
}

//////////////////////////////////////////////////////////////////////
// DimensionBranch
//////////////////////////////////////////////////////////////////////
CDimensionBranch::CDimensionBranch(CResultGroupObserver &group_observer) : IGraphTreeObject(group_observer, TVI_LAST) {
  Ctrl().SetItemData(Handle(), (DWORD_PTR)this);
  CModelBase &model = (CModelBase &)((CResultGroup &)group_observer.ObservedItem()).Model();
  for (int nRegister = 0; nRegister < model.ResultRegisterSize(); nRegister++)
    new CDimensionLeaf(*this, nRegister);
}

QString CDimensionBranch::Text() const { return "Model"; }

unsigned int CDimensionBranch::Icon() const { return IDI_2D_3D_RESULT; }
/*
BOOL CDimensionBranch::operator<(const ICtrlObjectBase &object) const
{
  if(dynamic_cast<const CDepletionStageBranch*>(&object))
    return FALSE;
  if(dynamic_cast<const CLinearityBranch*>(&object))
    return FALSE;
  if(this == &object)
    return FALSE;

  return FALSE;
}
*/
//////////////////////////////////////////////////////////////////////
// LinearityLeaf
//////////////////////////////////////////////////////////////////////
CDimensionBranch::CDimensionLeaf::CDimensionLeaf(CDimensionBranch &dimension_branch, int nRegister)
    : IGraphTreeObject(dimension_branch, TVI_LAST), m_nRegister(nRegister) {
  Ctrl().SetItemData(Handle(), (DWORD_PTR)this);
  Update();
}

QString CDimensionBranch::CDimensionLeaf::Text() const {
  if (m_nRegister)
    return "3D";
  return "2D";
}

unsigned int CDimensionBranch::CDimensionLeaf::Icon() const {
  if (m_nRegister)
    return IDI_3D_RESULT;
  return IDI_2D_RESULT;
}

IResultComponent *CDimensionBranch::CDimensionLeaf::Current() const {
  // We retrieve the root resultgroup from parent's parent via the observed item interface.
  const CTreeNode *pNode = dynamic_cast<const CTreeNode *>(Parent()->Parent());
  assert(pNode);
  const CResultGroup *pGroup = dynamic_cast<const CResultGroup *>(&pNode->ObservedItem());
  assert(pGroup);

  // Retrieving the opengl scene from the document.

  CGeomecDoc *pDoc = GetGeomecDoc();

  ISceneWrapper *pScene = pDoc->CurrentScene();
  if (pScene) {
    const IResultComponent *pResultComponent = 0;

    pResultComponent = dynamic_cast<const IResultComponent *>(pScene->ValueComponent());
    return (IResultComponent *)pResultComponent;
  }

  return 0;
}

IResultComponent *CDimensionBranch::CDimensionLeaf::SwitchTo() const {
  IResultComponent *pResultComponent = Current();
  if (!pResultComponent)
    return 0;

  IResult *pResult = (IResult *)(&pResultComponent->Parent());
  assert(pResult);

  if (pResultComponent->RegisterIndex() == m_nRegister)
    return 0;

  IResultComponent *pRes = pResult->ResultComponent(pResultComponent->Stage(), pResultComponent->AnalysisType(),
                                                    m_nRegister, pResultComponent->ComponentIndex());
  if (pRes) {
    if (!pRes->Defined())
      return 0;
  }

  return pRes;
}

unsigned int CDimensionBranch::CDimensionLeaf::StateIcon() const {
  if (Current() && (Current()->RegisterIndex() == m_nRegister))
    return IDI_RADIO_CHECKED;

  if (SwitchTo())
    return IDI_RADIO_UNCHECKED;

  return IDI_RADIO_DISABLED;
}

void CDimensionBranch::CDimensionLeaf::ToggleState() {
  if (SwitchTo()) {
    const CTreeNode *pNode = dynamic_cast<const CTreeNode *>(Parent()->Parent());
    assert(pNode);
    const CResultGroup *pGroup = dynamic_cast<const CResultGroup *>(&pNode->ObservedItem());
    assert(pGroup);

    CGeomecDoc *pDoc = GetGeomecDoc();

    if (!pDoc->CurrentScene())
      return;

    pDoc->CurrentScene()->ConnectItem(*SwitchTo());
  }
}
