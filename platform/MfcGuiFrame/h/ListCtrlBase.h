#if !defined(AFX_LISTCTRLBASE_H__00AB66A6_FC6F_11D5_B8F6_88B2EF8ABF79__INCLUDED_)
#define AFX_LISTCTRLBASE_H__00AB66A6_FC6F_11D5_B8F6_88B2EF8ABF79__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListCtrlBase.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListCtrlBase window

#include "CtrlDropTarget.h"
#include "IListObject.h"
#include "IconImageList.h"

class CListCtrlBase : public CListCtrl // public CListSupport
{
  CIconImageList m_ilState;  // State icon's
  CIconImageList m_ilNormal; // Normal sized icons ...
  CIconImageList m_ilSmall;  // Small icon's ...
  CCtrlDropTarget m_target;  // Drop target ...
  TCtrlObjectVec m_vcClipBoard;
  void ResizeCustomControl();

  DECLARE_DYNCREATE(CListCtrlBase)
public:
  CListCtrlBase();
  virtual ~CListCtrlBase();

  // Operations
  TListObjectVec GetSelection() const;
  void OnDeleteSelection();
  void UnSelectAll();
  void RegisterDropTarget();
  CEdit *EditSubLabel(std::pair<int, int> location);

public:
  // Overridables ...
  virtual void OnCopy();
  virtual BOOL CanPaste();
  virtual void OnPaste();
  virtual void OnSelect();
  virtual void OnContext(const TListObjectVec &vec);
  virtual CWnd &GetContextWnd() const;
  virtual DROPEFFECT OnDragOver(TCtrlObjectVec vcDragged, COleDataObject *pDataObject, DWORD dwKeyState, CPoint point);
  virtual BOOL OnDrop(TCtrlObjectVec vcDragged, COleDataObject *pDataObject, DROPEFFECT dropEffect, CPoint point);
  void Sort(int nColumn);
  CIconImageList &StateIconImageList() { return m_ilState; }
  IListObject *GetObject(int iRow);

  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CListCtrlBase)
  //}}AFX_VIRTUAL

  // Generated message map functions
protected:
  //{{AFX_MSG(CListCtrlBase)
  afx_msg void OnBeginLabelEdit(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnEndLabelEdit(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnHScroll(unsigned int nSBCode, unsigned int nPos, CScrollBar *pScrollBar);
  afx_msg void OnVScroll(unsigned int nSBCode, unsigned int nPos, CScrollBar *pScrollBar);
  afx_msg void OnLButtonDown(unsigned int nFlags, CPoint point);
  afx_msg void OnColumnClick(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnGetDisplayInfo(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnContextMenu(CWnd *pWnd, CPoint point);
  afx_msg void OnBeginDrag(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnDeleteItem(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnItemChanging(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnKeyDown(unsigned int nChar, unsigned int nRepCnt, unsigned int nFlags);
  afx_msg void OnDoubleClick(NMHDR *pNMHDR, LRESULT *pResult);
  //}}AFX_MSG

  afx_msg void OnSize(unsigned int nType, int cx, int cy);
  afx_msg void OnWindowPosChanged(WINDOWPOS FAR *lpwndpos);
  afx_msg BOOL OnHeaderTrack(unsigned int id, NMHDR *pNMHDR, LRESULT *pResult);
  DECLARE_MESSAGE_MAP()

  std::pair<int, int> HitTestEx(CPoint &point, unsigned int &uFlags) const;
};

int CALLBACK ListCompare(LPARAM iItem1, LPARAM iItem2, LPARAM lData);
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTCTRLBASE_H__00AB66A6_FC6F_11D5_B8F6_88B2EF8ABF79__INCLUDED_)
