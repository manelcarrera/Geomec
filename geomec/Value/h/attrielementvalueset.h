#if !defined(AFX_ATTRIVALUESET_H__3F0921D6_37B5_4C19_9075_2C666B9A5150__INCLUDED_)
#define AFX_ATTRIVALUESET_H__3F0921D6_37B5_4C19_9075_2C666B9A5150__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// attrivalueset.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAttriValueSet dialog
class CElementValueSet;
class CAttriElementValueSet : public CDialog
{
// Construction
	CElementValueSet& m_value_set;
public:
	CAttriElementValueSet(CElementValueSet& value_set, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAttriElementValueSet)
	enum { IDD = IDD_ATTRI_ELEMENT_VALUE_SET };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAttriElementValueSet)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAttriElementValueSet)
		// NOTE: the ClassWizard will add member functions here
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATTRIVALUESET_H__3F0921D6_37B5_4C19_9075_2C666B9A5150__INCLUDED_)
