#if !defined(AFX_TREESPLITVIEW_H__10B94ADD_3A04_4182_A4B0_F06473CBE5E7__INCLUDED_)
#define AFX_TREESPLITVIEW_H__10B94ADD_3A04_4182_A4B0_F06473CBE5E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreeSplitView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTreeSplitView view

//##ModelId=3B497D7A0022
class CTreeSplitView : public CView
{
	//##ModelId=3B497D7A008F
	CSplitterWnd	m_wndSplitter;
protected:
	//##ModelId=3B497D7A009D
	CTreeSplitView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CTreeSplitView)

// Attributes
public:

// Operations
public:
	void GetRowInfo(int row, int& cyCur, int& cyMin) const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeSplitView)
	public:
	//##ModelId=3B497D7A009E
	virtual void OnInitialUpdate();
	protected:
	//##ModelId=3B497D7A00AD
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	//##ModelId=3B497D7A00B0
	virtual ~CTreeSplitView();
#ifdef _DEBUG
	//##ModelId=3B497D7A00B2
	virtual void AssertValid() const;
	//##ModelId=3B497D7A00BD
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeSplitView)
	//##ModelId=3B497D7A00C0
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//##ModelId=3B497D7A00C3
	afx_msg void OnSize(unsigned int nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREESPLITVIEW_H__10B94ADD_3A04_4182_A4B0_F06473CBE5E7__INCLUDED_)
