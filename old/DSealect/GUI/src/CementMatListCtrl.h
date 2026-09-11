#if !defined(AFX_CEMENTMATLISTCTRL_H__1680A09B_3A7C_482D_9F23_52A3AF4F10C7__INCLUDED_)
#define AFX_CEMENTMATLISTCTRL_H__1680A09B_3A7C_482D_9F23_52A3AF4F10C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CementMatListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCementMatListCtrl window
class CCementMaterial;

//##ModelId=3C60D57B02A0
class CCementMatListCtrl : public CListCtrl
{
private:
	//##ModelId=3C60D57B02B1
	void CreateColums();

// Construction
public:
	//##ModelId=3C60D57B02B2
	CCementMatListCtrl();
	//##ModelId=3C60D57B02B3
	void ShowMaterial(CCementMaterial& mat);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCementMatListCtrl)
	protected:
	//##ModelId=3C60D57B02C0
		virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	//##ModelId=3C60D57B02C2
	virtual ~CCementMatListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCementMatListCtrl)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CEMENTMATLISTCTRL_H__1680A09B_3A7C_482D_9F23_52A3AF4F10C7__INCLUDED_)
