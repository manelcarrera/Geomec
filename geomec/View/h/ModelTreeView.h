#if !defined(AFX_MODELTREEVIEW_H__7B615427_8C76_4C11_905B_F4C63A97CFBD__INCLUDED_)
#define AFX_MODELTREEVIEW_H__7B615427_8C76_4C11_905B_F4C63A97CFBD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModelTreeView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModelTreeView view
#include "GraphTreeView.h"
#include "TreeNode.h"

// ##ModelId=3B653D8D0198
class CModelTreeView : public CGraphTreeView {
  // TODO AppendContextMenu
  // CModelTreeObject is not derived from CGraphNode, hence
  // the function AppendContextMenu does not ask for a delegate. However one
  // could verify that this function is never used!

  class CModelTreeObject : public IGraphTreeObject {
    CFemAppDoc *m_pDoc;

  public:
    CModelTreeObject(CFemAppDoc *pDoc, CTreeCtrl &ctrl, HTREEITEM hParent, HTREEITEM hInsertAfter);
    virtual void AppendContextMenu(CContextMenuInvoker &invoker);
    virtual QString Text() const;
    virtual void Delete();
    virtual BOOL CanEditText() const;
    virtual BOOL CanDelete() const;
    virtual unsigned int Icon() const;
    virtual void SortChildren() {}; // Don't mesh up insertion order
  };

  class CViewButton {
  public:
    typedef void (CModelTreeView::*TClicked)();

  public:
    CViewButton(CModelTreeView &view, TClicked fnClicked, const CString &text, const CRect &rect, COLORREF color);

    void OnDraw(CDC *pDC);
    void setEnabled(bool bEnabled);
    bool isEnabled() const;
    void setVisible(bool bVisible);
    bool isVisible() const;

    void OnLButtonDown(UINT nFlags, CPoint point);
    void OnLButtonUp(UINT nFlags, CPoint point);
    void OnMouseMove(UINT nFlags, CPoint point);
    void OnMouseLeave();

  private:
    void Invalidate();
    void Clicked();
    void UpdateColorValue(BYTE &c);

  private:
    CModelTreeView &m_view;
    TClicked m_fnClicked;
    CString m_text;
    CRect m_rect;
    COLORREF m_color;
    bool m_bEnabled;
    bool m_bVisible;
    bool m_bDown;
  };

  CViewButton *m_viewbuttons[3];

protected:
  // ##ModelId=3B653D8D0244
  CModelTreeView(); // protected constructor used by dynamic creation
  DECLARE_DYNCREATE(CModelTreeView)

  // Attributes
public:
  // ##ModelId=3B653D8D02B1
  // Operations
public:
  virtual void OnSelect(ITreeObject &tree_object);

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CGeoModelView)
public:
  // ##ModelId=3B653D8D02C1
  virtual void OnInitialUpdate();
  // ##ModelId=3B653D8D02C3
  virtual BOOL DestroyWindow();

protected:
  // ##ModelId=3B653D8D02C5
  virtual void OnDraw(CDC *pDC); // overridden to draw this view
  // ##ModelId=3B653D8D02D0
  virtual void OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint);
  //}}AFX_VIRTUAL

  // Implementation
protected:
  // ##ModelId=3B653D8D02E2
  virtual ~CModelTreeView();
#ifdef _DEBUG
  // ##ModelId=3B653D8D02F0
  virtual void AssertValid() const;
  // ##ModelId=3B653D8D02F2
  virtual void Dump(CDumpContext &dc) const;
#endif

  // Generated message map functions
protected:
  //{{AFX_MSG(CGeoModelView)

  //}}AFX_MSG
  afx_msg void OnSize(unsigned int nType, int cx, int cy);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnMouseLeave();
  DECLARE_MESSAGE_MAP()

private:
  void OnChangeClicked();
  void OnDeleteClicked();
  void OnCancelClicked();

private:
  int m_ButtonHeight;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODELTREEVIEW_H__7B615427_8C76_4C11_905B_F4C63A97CFBD__INCLUDED_)
