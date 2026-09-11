#if !defined(AFX_DSEALECTTREEVIEW_H__E0BD8D52_041B_45CB_BBAB_030A54C48F05__INCLUDED_)
#define AFX_DSEALECTTREEVIEW_H__E0BD8D52_041B_45CB_BBAB_030A54C48F05__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DSealectTreeView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDSealectTreeView view

#include "treeobject.h"
#include "afxcview.h"
#include <afxtempl.h>



//##ModelId=3C0DC15B034B
class CDSealectTreeView : public CTreeView
{
private:
	
	//##ModelId=3CAAA66B013C
	CImageList m_ilTreeImages;
	//##ModelId=3CAAA66B014C
	CImageList m_ilTreeImagesState;

protected:
	//##ModelId=3C0DC15C00CB
	struct Color_Font
	{
		//##ModelId=3C0DC15C00CD
		COLORREF color;
		//##ModelId=3CAAA66B01B9
		LOGFONT  logfont;
	};
	//##ModelId=3CAAA66B017B
	CMap< void*, void*, Color_Font, Color_Font& > m_mapColorFont ;
	//##ModelId=3C0DC15B03A9
	CDSealectTreeView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDSealectTreeView)


// Attributes
public:

	//##ModelId=3C0DC15B03AA
	COLORREF GetItemColor(HTREEITEM hItem);
	//##ModelId=3C0DC15B03AC
	BOOL GetItemBold(HTREEITEM hItem);
	//##ModelId=3C0DC15B03BA
	BOOL GetItemFont(HTREEITEM hItem, LOGFONT * plogfont);
	//##ModelId=3C0DC15B03BD
	void SetItemColor(HTREEITEM hItem, COLORREF color);
	//##ModelId=3C0DC15B03CA
	void SetItemBold(HTREEITEM hItem, BOOL bBold);
	//##ModelId=3C0DC15B03DA
	void SetItemFont(HTREEITEM hItem, LOGFONT& logfont);


	//##ModelId=3C0DC15C0001
void ReleaseTreeItem(const HTREEITEM hti);
	//##ModelId=3C0DC15C0003
BOOL InsertItem(ITreeObject* Item ,ITreeObject* pParent=NULL,ITreeObject* pInsertAfter=NULL);	


// Operations
public:
	//##ModelId=3C0DC15C0011
	void OnInitialUpdate();
	//##ModelId=3C0DC15C0012
	BOOL CreateImageList();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDSealectTreeView)
	protected:
	//##ModelId=3C0DC15C0013
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//##ModelId=3C0DC15C0021
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//##ModelId=3C0DC15C002F
	virtual ~CDSealectTreeView();
#ifdef _DEBUG
	//##ModelId=3C0DC15C003E
	virtual void AssertValid() const;
	//##ModelId=3C0DC15C0040
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CDSealectTreeView)
	//##ModelId=3C0DC15C004E
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	//##ModelId=3C0DC15C0052
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	//##ModelId=3C0DC15C005F
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	//##ModelId=3C0DC15C006D
	afx_msg void OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
	//##ModelId=3C0DC15C007D
	afx_msg void OnPaint();
	//##ModelId=3C0DC15C007F
	afx_msg void OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DSEALECTTREEVIEW_H__E0BD8D52_041B_45CB_BBAB_030A54C48F05__INCLUDED_)
