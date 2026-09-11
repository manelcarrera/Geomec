#if !defined(AFX_DATATREEVIEW_H__F96B09DC_9FF8_4544_B48C_A25766177F30__INCLUDED_)
#define AFX_DATATREEVIEW_H__F96B09DC_9FF8_4544_B48C_A25766177F30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DataTreeView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDataTreeView view
#include "PointSet.h"
#include "ValueCompositeObserver.h"

// ##ModelId=3B653D8E02D0
class CModelBase;

#include "GraphTreeView.h"
#include "IPointSet_Delegate.h"
#include "OpenGLNodeObserver_Delegate.h"
#include "StateBranch_Delegate.h"
#include "StateEnumerationBranch.h"
#include "UndefinedIconProvider.h"

class CDataTreeView : public CGraphTreeView {
  // Root branch

  // TODO AppendContextMenu
  // CDataBranch is not derived from CGraphNode, hence
  // the function AppendContextMenu does not ask for a delegate. However one
  // could verify that this function is never used!

  class CDataBranch : public IGraphTreeObject {
    CGraphTreeView *m_pTreeView;

  public:
    CDataBranch(CGraphTreeView &view);
    virtual QString Text() const;
    virtual unsigned int Icon() const;
    virtual void AppendContextMenu(CContextMenuInvoker &invoker);
    virtual void OnExpand() { Update(); }
    virtual void OnCollapse() { Update(); }
  };

  // PointSet branch
  typedef CGraphEntryTemp_Delegate<IPointSet> TPointSetEntry_Delegate;
  typedef CStateBranch_Delegate<TPointSetEntry, TPointSetEntry_Delegate, CDummyNode, CDummyObserver,
                                CUndefinedIconProvider, TRUE, FIXED_ITEM>
      TPointSetBranchBase;

  // TODO AppendContextMenu
  // CPointSetBranch is not derived from CGraphNode, hence
  // the function AppendContextMenu does not ask for a delegate. However one
  // could verify that this function is never used!

  class CPointSetBranch : public TPointSetBranchBase {
    typedef COpenGLNodeObserver_Delegate<IPointSet, IPointSet_Delegate, CDummyNode, CDummyObserver, TRUE, DELETE_ITEM>
        TPointSetObserver;
    typedef CEnumerationBranch<IValueComposite, CValueReferenceObserver, TRUE, FIXED_ITEM> TPropertyEnumerator;
    class CPropertyEnumerator : public TPropertyEnumerator {
    public:
      CPropertyEnumerator(TPointSetObserver &parent_observer, const CString &strName, const unsigned int uIcon,
                          HTREEITEM hInsertAfter = TVI_LAST);
      virtual BOOL OnFilter(const child_type &t) const;
    };

    typedef COpenGLNodeObserver_Delegate<IPointSet, IPointSet_Delegate, CDummyNode, CDummyObserver, TRUE, DELETE_ITEM>
        TPointSetObserver;
    typedef CStateEnumerationBranch<IPointSet, TPointSetObserver, TRUE, DELETE_ITEM, CUndefinedIconProvider>
        TStateEnumerationBranch;

    class CPointSetSubBranch : public TStateEnumerationBranch {
      const CPointSet::DIMENSION m_dim;
      const CPointSet::TPointSetType m_psType;

    public:
      CPointSetSubBranch(CPointSetBranch &parent_observer, const CPointSet::DIMENSION dim,
                         const CPointSet::TPointSetType psType, const CString &strName, const unsigned int uIcon,
                         HTREEITEM hInsertAfter = TVI_LAST);
      virtual BOOL OnFilter(const child_type &t) const;
      virtual CTreeNode *InsertChild(child_type &child);
      virtual void OnChildModified(CTreeNode &child);

      virtual DROPEFFECT CanDrop(TCtrlObjectVec &vcDragged, BOOL bMove) const;
      virtual void Drop(TCtrlObjectVec &vcDragged, BOOL bMove);
    };
    CModelBase &m_model;

  public:
    typedef COpenGLNodeObserver_Delegate<IPointSet, IPointSet_Delegate, CDummyNode, CDummyObserver, FALSE, FIXED_ITEM>
        TIPointSetObserver;
    typedef CStateBranch_Delegate<CPointSetEntry, TPointSetEntry_Delegate, IPointSet, TIPointSetObserver,
                                  CUndefinedIconProvider, TRUE, DELETE_ITEM>
        TPointSetEntryObserver;

    class CWellPathPointSetBranch : public TPointSetEntryObserver // waij TFS 92430
    {
    public:
      CWellPathPointSetBranch(CPointSetEntry &entry, CTreeCtrl &ctrl, HTREEITEM hInsertAfter = TVI_LAST);
      virtual BOOL OnFilter(const child_type &t) const;
      virtual QString Text() const;
      virtual void AppendContextMenu(CContextMenuInvoker &invoker);

    private:
      bool CanInsertNew() const;
      void InsertDeviatedWellPathByPointSet();
      bool CanCopyAllWellPaths() const;
      void CopyAllWellPaths();
      bool CanCopyRelevantWellPaths() const;
      void CopyRelevantWellPaths();
    };
    CPointSetBranch(TPointSetEntry &node, CModelBase &model, CTreeCtrl &view, HTREEITEM hParent = TVI_ROOT,
                    HTREEITEM hInsertAfter = TVI_LAST);
    virtual void AppendContextMenu(CContextMenuInvoker &invoker);
    void CreatePointSet();
  };

protected:
  // ##ModelId=3B653D8E037C
  CDataTreeView(); // protected constructor used by dynamic creation
  DECLARE_DYNCREATE(CDataTreeView)

  // Attributes
public:
  // Operations
public:
  virtual void OnSelect(ITreeObject &tree_object);
  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CDataTreeView)
public:
  // ##ModelId=3B653D8E03AE
  virtual void OnInitialUpdate();

protected:
  // ##ModelId=3B653D8E03B0
  virtual void OnDraw(CDC *pDC); // overridden to draw this view
  // ##ModelId=3B653D8E03BD
  virtual void OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint);
  //}}AFX_VIRTUAL

  // Implementation
protected:
  // ##ModelId=3B653D8E03CB
  virtual ~CDataTreeView();
#ifdef _DEBUG
  // ##ModelId=3B653D8E03CD
  virtual void AssertValid() const;
  // ##ModelId=3B653D8E03DB
  virtual void Dump(CDumpContext &dc) const;
#endif

  // Generated message map functions
protected:
  //{{AFX_MSG(CDataTreeView)

  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATATREEVIEW_H__F96B09DC_9FF8_4544_B48C_A25766177F30__INCLUDED_)
