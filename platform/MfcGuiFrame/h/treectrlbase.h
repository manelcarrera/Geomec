#if !defined(AFX_TREECTRLBASE_H__EBD66F88_F63B_11D5_B8F5_F2227CA4A730__INCLUDED_)
#define AFX_TREECTRLBASE_H__EBD66F88_F63B_11D5_B8F5_F2227CA4A730__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// treectrlbase.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlBase window

#include "CtrlDropTarget.h"
#include "IconImageList.h"
#include "ITreeObject.h"

class CTreeCtrlBase : public CTreeCtrl
{
  CCtrlDropTarget m_target;
  CIconImageList	m_ilState;
  CIconImageList	m_ilNormal;
  TCtrlObjectVec	m_vcClipBoard;
  BOOL m_bContextOperation;
  BOOL m_bTreeDeleted;
  INT m_timer;
  HTREEITEM m_expand;
  HTREEITEM m_hLastDragOverItem;

  void UpdateChildren(HTREEITEM hParent);

  static bool m_bAllowPaint;
public:
  static void AllowPaint(bool bAllowPaint);
  static bool AllowPaint(){ return m_bAllowPaint; };

  CTreeCtrlBase();

// Operations
  TTreeObjectVec GetSelection() const;
  void OnDeleteSelection();
  void UnSelectAll();
  void UnSelectStateless();
  CIconImageList* GetIconImageList(unsigned int nImage); //Need list before activated (state is not a callback)

public:		
  // Overridables ...
  virtual void OnCopy();
  virtual BOOL CanPaste();
  virtual void OnPaste();
  virtual void OnSelect(ITreeObject* pOld, ITreeObject* pNew);
  virtual void OnContext(const TTreeObjectVec &vec);
  virtual CWnd& GetContextWnd() const;
  virtual DROPEFFECT OnDragOver(TCtrlObjectVec vcDragged,
                  COleDataObject* pDataObject, 
                  DWORD dwKeyState, 
                  CPoint point);
  virtual void OnDragLeave(TCtrlObjectVec vcDragged);
  virtual BOOL OnDrop(TCtrlObjectVec vcDragged, 
            COleDataObject* pDataObject,
            DROPEFFECT dropEffect, 
            CPoint point);
  void UpdateTree();
  BOOL DeleteTree();

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CTreeCtrlBase)
  //}}AFX_VIRTUAL

// Implementation
public:
  void RegisterDropTarget();
  virtual ~CTreeCtrlBase();

  // Generated message map functions
protected:
  //{{AFX_MSG(CTreeCtrlBase)
  afx_msg void OnGetDisplayInfo(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
  afx_msg void OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnLButtonDown(unsigned int nFlags, CPoint point);
  //afx_msg void OnLButtonUp(unsigned int nFlags, CPoint point);
  //afx_msg void OnLButtonDblClk(unsigned int nFlags, CPoint point);
  afx_msg void OnRightClick(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnActivate(unsigned int nState, CWnd* pWndOther, BOOL bMinimized);
  afx_msg void OnPaint();
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREECTRLBASE_H__EBD66F88_F63B_11D5_B8F5_F2227CA4A730__INCLUDED_)
