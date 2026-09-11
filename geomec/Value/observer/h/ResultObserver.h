// ResultObserver.h: interface for the CResultObserver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESULTOBSERVER_H__94619492_8F1E_4AA9_AC2F_283B2E3999EA__INCLUDED_)
#define AFX_RESULTOBSERVER_H__94619492_8F1E_4AA9_AC2F_283B2E3999EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CResultGroup_Delegate;
class IResult_Delegate;
class CTreeViewBase;

#include "DepletionStage.h"
#include "DepletionStage_Delegate.h"
#include "NodeObserver_Delegate.h"
#include "Result.h"
#include "ValueCompositeObserver.h"

// TODO AppendContextMenu
// IResultGroupObserver is not derived from CGraphNode, hence
// the function AppendContextMenu does not ask for a delegate. However one
// could verify that this function is never used!

class IResultGroupObserver : public CChildEnumerator<CGraphNode> {
public:
  // TODO AppendContextMenu
  // CResultObserver is not derived from CGraphNode, hence
  // the function AppendContextMenu does not ask for a delegate. However one
  // could verify that this function is never used!

  class CResultObserver : public CTreeNode {
  public:
    // If a result has more than one component the result component observers
    // represent the different components of the result.

    // TODO AppendContextMenu
    // CResultComponentObserver is not derived from CGraphNode, hence
    // the function AppendContextMenu does not ask for a delegate. However one
    // could verify that this function is never used!

    class CResultComponentObserver : public IGraphTreeObject {
    private:
      int m_nComponent;

    public:
      // Construction
      CResultComponentObserver(CResultObserver &parent, int nComponent);

      // Destruction
      ~CResultComponentObserver();

      // Parent access
      const CResultObserver &ResultObserver() const;
      CResultObserver &ResultObserver();

      // Treeobject overrides
      virtual QString Text() const;
      virtual unsigned int Icon() const;
      virtual unsigned int StateIcon() const;
      virtual void ToggleState();
      virtual void AppendContextMenu(CContextMenuInvoker &invoker);

    private:
      IResult_Delegate *m_pDelegate;
    };

  private:
    IResult &m_result;

  public:
    // Construction
    CResultObserver(IResult &result, IResultGroupObserver &parent);

    // Destruction
    ~CResultObserver();

    // Result access
    const IResult &Result() const;
    IResult &Result();

    // Result Group observer
    const IResultGroupObserver &ResultGroupObserver() const;
    IResultGroupObserver &ResultGroupObserver();

    virtual const CGraphNode &ObservedItem() const;
    virtual CGraphNode &ObservedItem();

    virtual const CGraphNode_Delegate &Delegate() const;
    virtual CGraphNode_Delegate &Delegate();

    // Handlers result components
    virtual unsigned int ResultComponentStateIconId(const IValueComposite &result, int nComponent) const;
    virtual void ResultComponentToggleState(IValueComposite &result, int nComponent);
    virtual void AppendResultComponentContextMenu(IResult_Delegate &result, int nComponent,
                                                  CContextMenuInvoker &invoker);

    // Treeobject overrides
    virtual unsigned int StateIcon() const;
    virtual void ToggleState();
    virtual void AppendContextMenu(CContextMenuInvoker &invoker);

  private:
    IResult_Delegate *m_pDelegate;
  };

private:
  CResultGroup &m_group;

public:
  // Construction
  IResultGroupObserver(CResultGroup &group, IResultGroupObserver &parent);
  IResultGroupObserver(CResultGroup &group, CTreeCtrl &ctrl, HTREEITEM hParent, HTREEITEM hInsertAfter);

  // Destruction
  ~IResultGroupObserver();

  // Parent
  const IResultGroupObserver *ResultGroupObserver() const;
  IResultGroupObserver *ResultGroupObserver();

  virtual const CGraphNode &ObservedItem() const;
  virtual CGraphNode &ObservedItem();

  virtual const CGraphNode_Delegate &Delegate() const;
  virtual CGraphNode_Delegate &Delegate();

  // Handlers for result group events
  virtual unsigned int ResultGroupStateIconId(const CResultGroup &group) const;
  virtual void ResultGroupToggleState(CResultGroup &group);
  virtual void AppendResultGroupContextMenu(CResultGroup_Delegate &group_Delegate, CContextMenuInvoker &invoker);
  virtual BOOL CanInsertResultGroup(const CResultGroup &group) const;

  // Handlers for results
  virtual unsigned int ResultStateIconId(const IValueComposite &result) const;
  virtual void ResultToggleState(IValueComposite &result);
  virtual void AppendResultContextMenu(IResult_Delegate &result, CContextMenuInvoker &invoker);
  virtual BOOL CanInsertResult(const IResult &result) const;

  // Handlers result components
  virtual unsigned int ResultComponentStateIconId(const IValueComposite &result, int nComponent) const;
  virtual void ResultComponentToggleState(IValueComposite &result, int nComponent);
  virtual void AppendResultComponentContextMenu(const IResult_Delegate &result, int nComponent,
                                                CContextMenuInvoker &invoker);

  // Treeobject overrides
  virtual unsigned int StateIcon() const;
  virtual void ToggleState();
  virtual void AppendContextMenu(CContextMenuInvoker &invoker);
  virtual CTreeNode *InsertChild(CGraphNode &node);
  virtual BOOL OnFilter(const CGraphNode &node) const;
  virtual void SortChildren() {}; // Don't mesh up insertion order

private:
  CResultGroup_Delegate *m_pDelegate;
};

// The resultgroup observer observes a result group. It automatically generates the children of
// IResult and CResultGroup types attached to the resultgroup.
class CDepletionStageBranch;
class CResultGroupObserver : public IResultGroupObserver {
  CDepletionStageBranch *m_pDepletionStageBranch;
  CResultRegister &ResultRegister();

public:
  // Construction
  CResultGroupObserver(CResultGroup &group, CTreeViewBase &view, HTREEITEM hParent,
                       bool bShowAnalysisTypesAndDimensions, bool bShowDepletions = true);
  CResultGroupObserver(CResultGroup &group, CResultGroupObserver &parent);

  // Handlers for result group events
  virtual unsigned int ResultGroupStateIconId(const CResultGroup &group) const;
  virtual void ResultGroupToggleState(CResultGroup &group);
  virtual void AppendResultGroupContextMenu(CResultGroup_Delegate &group_Delegate, CContextMenuInvoker &invoker);
  virtual BOOL CanInsertResultGroup(const CResultGroup &group) const;

  // Handlers for results
  virtual unsigned int ResultStateIconId(const IValueComposite &result) const;
  virtual void ResultToggleState(IValueComposite &result);
  virtual void AppendResultContextMenu(IResult_Delegate &result, CContextMenuInvoker &invoker);
  virtual BOOL CanInsertResult(const IResult &result) const;

  // Handlers result components
  virtual unsigned int ResultComponentStateIconId(const IValueComposite &result, int nComponent) const;
  virtual void ResultComponentToggleState(IValueComposite &result, int nComponent);
  virtual void AppendResultComponentContextMenu(const IResult_Delegate &result, int nComponent,
                                                CContextMenuInvoker &invoker);

  // Insert
  virtual CTreeNode *InsertChild(CGraphNode &node);
  BOOL operator<(const ICtrlObjectBase &object) const;
  virtual void SortChildren() {}; // Don't mesh up insertion order
  virtual void Update();

private:
  CAnalysisType::TAnalysisType determineInitialAnalysisType();
};

class CGVTResultGroupObserver : public CResultGroupObserver {
public:
  CGVTResultGroupObserver(CResultGroup &group, CTreeViewBase &view, HTREEITEM hParent,
                          bool bShowAnalysisTypesAndDimensions, bool bShowDepletions);
  CGVTResultGroupObserver(CResultGroup &group, CResultGroupObserver &parent);
  virtual BOOL OnFilter(const CGraphNode &node) const;
  virtual CTreeNode *InsertChild(CGraphNode &node);
};

class CGVTOutputObserver : public CValueCompositeObserver {
  bool m_selected;

public:
  CGVTOutputObserver(IValueComposite &node, CTreeNode &parent);
  virtual unsigned int StateIcon() const;
  virtual void ToggleState();
};

// The depletion stage branch shows all depletionstages and shows the selected depletionstage for the
// current result.
typedef CNodeObserver_Delegate<CDepletionStageEntry, CDepletionStageEntry_Delegate, CDepletionStage, CDummyObserver,
                               FALSE, FIXED_ITEM>
    TDepletionStageBranchBase;
class CDepletionStageBranch : public TDepletionStageBranchBase {
public:
  // The depletionstage observer observes the depletionstage. When no result is selected
  // the depletionstage is grayed out.
  typedef CNodeObserver_Delegate<CDepletionStage, CDepletionStage_Delegate, CDummyNode, CDummyObserver, FALSE,
                                 FIXED_ITEM>
      TDepletionStageLeafBase;

  // TODO AppendContextMenu
  // CDepletionStageObserver is not derived from CGraphNode, hence
  // the function AppendContextMenu does not ask for a delegate. However one
  // could verify that this function is never used!

  class CDepletionStageObserver : public TDepletionStageLeafBase {
    std::vector<IResultComponent *> SwitchTo() const;
    bool CanSwitchTo() const;
    bool IsChecked() const;
    bool MultipleStagesAllowed() const;
    int NumDisplayedStages() const;
    IResultComponent *Current() const;

    bool IsMarkedAsInitial() const;
    void OnMarkAsInitial();

  public:
    CDepletionStageObserver(CDepletionStageBranch &branch, CDepletionStage &stage);
    virtual unsigned int StateIcon() const;
    virtual void ToggleState();
    virtual void AppendContextMenu(CContextMenuInvoker &invoker);
  };

public:
  CDepletionStageBranch(CResultGroupObserver &parent, CDepletionStageEntry &entry);
  virtual CTreeNode *InsertChild(CDepletionStage &stage);
  virtual void SortChildren(); // Don't mesh up insertion order

protected:
  virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
};

// The linearitybranch shows all posible analysis types (Linear and Non-Linear for this moment).
//
class CLinearityBranch : public IGraphTreeObject {
  class CLinearityLeaf : public IGraphTreeObject {
    CAnalysisType m_antype;
    IResultComponent *SwitchTo() const;
    bool CanSwitchTo() const;

    size_t NumValueComponents() const;

    IResultComponent *Current(size_t index = 0) const;

  public:
    CLinearityLeaf(CLinearityBranch &linear_branch, const CAnalysisType &antype);
    virtual QString Text() const;
    virtual unsigned int StateIcon() const;
    virtual unsigned int Icon() const;
    virtual void ToggleState();
  };

public:
  CLinearityBranch(CResultGroupObserver &root_observer);
  virtual QString Text() const;
  virtual unsigned int Icon() const;
  virtual void SortChildren() {}; // Don't mesh up insertion order
};

// The linearitybranch shows all posible analysis types (Linear and Non-Linear for this moment).
//
class CDimensionBranch : public IGraphTreeObject {
  class CDimensionLeaf : public IGraphTreeObject {
    int m_nRegister;
    IResultComponent *SwitchTo() const;
    IResultComponent *Current() const;

  public:
    CDimensionLeaf(CDimensionBranch &dimension_branch, int nRegister);
    virtual QString Text() const;
    virtual unsigned int StateIcon() const;
    virtual unsigned int Icon() const;
    virtual void ToggleState();
  };

public:
  CDimensionBranch(CResultGroupObserver &root_observer);
  virtual QString Text() const;
  virtual unsigned int Icon() const;
  virtual void SortChildren() {}; // Don't mesh up insertion order
};

#endif // !defined(AFX_RESULTOBSERVER_H__94619492_8F1E_4AA9_AC2F_283B2E3999EA__INCLUDED_)
