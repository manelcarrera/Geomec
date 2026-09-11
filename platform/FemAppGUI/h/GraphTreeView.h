// GraphTreeView.h: interface for the CGraphTreeView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHTREEVIEW_H__0B0EC0EF_07B9_470A_AB43_3A9763781BB0__INCLUDED_)
#define AFX_GRAPHTREEVIEW_H__0B0EC0EF_07B9_470A_AB43_3A9763781BB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTreeNode;

#include "TreeViewBase.h"

// #include <map>

class CIconImageList;
class IGraphTreeObject;
class CFemAppDoc;
class CGraphTreeView : public CTreeViewBase {
  friend class IGraphTreeObject;
  ITreeObject *m_pSelection;

private:
  class CRootObserver : public CGraphNode {
  public:
    CRootObserver();
    void DeleteRoots();
    ~CRootObserver();
    virtual unsigned int IconId() const;
    virtual unsigned int TypeId() const;
  };

  CRootObserver m_root;

  CBitmap *GetEmptyBitmap(int Size, int bpp);

protected:
  // ##ModelId=3B653D000287
  CGraphTreeView(); // protected constructor used by dynamic creation
                    //	DECLARE_DYNCREATE(CGraphTreeView)
  void DeleteTree();
  virtual CTreeCtrlBase *OnCreateCtrl();
  // Attributes
public:
  // Operations
public:
  //	void OnContextMenu(CWnd* pWnd, CPoint point) ;
  // ##ModelId=3B653D000292
  CFemAppDoc *Document() const;
  // ##ModelId=3B653D000294
  CTreeNode *SelectedItem();

  // Function for selection
  virtual void OnSelect(ITreeObject &tree_object);
  ICtrlObject *SelectedCtrlObject();
  CGraphNode *SelectedNode();
  const CGraphNode *SelectedNode() const;

  // Operations on the selection
  void OnEditCopy();
  void OnEditPaste();
  void OnEditDelete();

  // ##ModelId=3B653D0002A2
  void OnUpdateEditPaste(CCmdUI *pCmdUI);
  // ##ModelId=3B653D0002A4
  // ##ModelId=3B653D0002B4

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CGraphTreeView)
public:
protected:
  // ##ModelId=3B653D0002C4
  virtual void OnDraw(CDC *pDC); // overridden to draw this view
  // ##ModelId=3B653D0002D0
  virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
  //}}AFX_VIRTUAL

  // Implementation
protected:
  // ##ModelId=3B653D0002D3
  virtual ~CGraphTreeView();
#ifdef _DEBUG
  // ##ModelId=3B653D0002E1
  virtual void AssertValid() const;
  // ##ModelId=3B653D0002E3
  virtual void Dump(CDumpContext &dc) const;
#endif

  // Generated message map functions
protected:
  void AddRoot(CTreeNode &obs);
  //{{AFX_MSG(CGraphTreeView)
  afx_msg void OnDeleteSelection();
  afx_msg void OnSelectionChanged(NMHDR *pNMHDR, LRESULT *pResult);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
private:
  // ##ModelId=3B653D00035F
};

#endif // !defined(AFX_GRAPHTREEVIEW_H__0B0EC0EF_07B9_470A_AB43_3A9763781BB0__INCLUDED_)
