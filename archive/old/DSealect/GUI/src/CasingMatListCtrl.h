// CasingMatListCtrl.h: interface for the CCasingMatListCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CASINGMATLISTCTRL_H__DD8A6E83_9457_466E_BC42_81FB903C31AF__INCLUDED_)
#define AFX_CASINGMATLISTCTRL_H__DD8A6E83_9457_466E_BC42_81FB903C31AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// CCasingMatListCtrl window

class CCasingMaterial;

//##ModelId=3C60D57E0177
class CCasingMatListCtrl : public CListCtrl
{
private:
	//##ModelId=3C60D57E0188
	void CreateColums();


// Construction
public:
	//##ModelId=3C60D57E0189
	CCasingMatListCtrl();
	//##ModelId=3C60D57E0197
	void ShowMaterial(CCasingMaterial& mat);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCasingMatListCtrl)
	protected:
	//##ModelId=3C60D57E0199
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	//##ModelId=3C60D57E019B
	virtual ~CCasingMatListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCasingMatListCtrl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_CASINGMATLISTCTRL_H__DD8A6E83_9457_466E_BC42_81FB903C31AF__INCLUDED_)
