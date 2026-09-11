#if !defined(AFX_ATTRIHEXAFORMATION_H__0987A5D9_A5EC_4DB1_A6B2_7190F7560F97__INCLUDED_)
#define AFX_ATTRIHEXAFORMATION_H__0987A5D9_A5EC_4DB1_A6B2_7190F7560F97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// attriHexaFormation.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAttriHexaFormation dialog
#include "HexaFormation.h"
#include "AttriFormationTemplate.h"

//##ModelId=3BE7AEC9010E
class CAttriHexaFormation : public CAttriFormationTemplate<CHexaFormation>
{
// Construction
public:
	//##ModelId=3BE7AEC9018B
	CAttriHexaFormation(CHexaFormation& formation, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAttriHexaFormation)
	enum { IDD = IDD_ATRI_HOR_FORMATION };
	BOOL	m_bApplyToAll;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAttriHexaFormation)
	protected:
	//##ModelId=3BE7AEC9018E
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAttriHexaFormation)
	virtual BOOL OnInitDialog();
	//##ModelId=3BE7AEC90191
	afx_msg void OnColor();
	//##ModelId=3BE7AEC9019B
	afx_msg void OnPaint();

	afx_msg void OnDeltaposSpinLayer(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinElement(NMHDR* pNMHDR, LRESULT* pResult);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATTRIHEXAFORMATION_H__0987A5D9_A5EC_4DB1_A6B2_7190F7560F97__INCLUDED_)
