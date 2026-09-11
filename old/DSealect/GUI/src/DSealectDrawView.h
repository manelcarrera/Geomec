#if !defined(AFX_DSEALECTDRAWVIEW_H__CB55CFF1_B064_4C7C_A64F_4C59A8DC2461__INCLUDED_)
#define AFX_DSEALECTDRAWVIEW_H__CB55CFF1_B064_4C7C_A64F_4C59A8DC2461__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DSealectDrawView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDSealectDrawView view

#include "DSealectDoc.h"
class CMainFrame;
class CBaseFormView;
class CDrawProxy;
class CDataModel;

//##ModelId=3C0DC15C0148
class CDSealectDrawView : public CScrollView
{

friend CMainFrame;

private:

	

protected:
	//##ModelId=3C0DC15C016C
	CDSealectDrawView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDSealectDrawView)


// Attributes
public:

// Operations
public:
	//##ModelId=3C0DC15C0177
	UINT m_ViewID;
	//##ModelId=3C0DC15C0178
	int ViewID();
	
	//##ModelId=3C8D9E1C01B7
	CDataModel* GetModel() const
	{
		CDSealectDoc* pDoc=(CDSealectDoc*)GetDocument();
		return pDoc->DSealectModel();
	}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDSealectDrawView)
	protected:
	//##ModelId=3C0DC15C0186
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//##ModelId=3C0DC15C0189
	virtual ~CDSealectDrawView();
#ifdef _DEBUG
	//##ModelId=3C0DC15C0197
	virtual void AssertValid() const;
	//##ModelId=3C0DC15C0199
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CDSealectDrawView)
	//##ModelId=3C6A14C603A7
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DSEALECTDRAWVIEW_H__CB55CFF1_B064_4C7C_A64F_4C59A8DC2461__INCLUDED_)
