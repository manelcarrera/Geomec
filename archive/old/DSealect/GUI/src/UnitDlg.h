#if !defined(AFX_UNITDLG_H__60282629_A8C2_45F4_8387_389BC8BE8F6E__INCLUDED_)
#define AFX_UNITDLG_H__60282629_A8C2_45F4_8387_389BC8BE8F6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UnitDlg.h : header file
//
#include "DSealectDoc.h"
#include <vector>

/////////////////////////////////////////////////////////////////////////////
// CUnitDlg dialog

//##ModelId=3C0DC15600DE
class CUnitDlg : public CDialogBar
{
// Construction
public:
	//##ModelId=3C6A14BE036A
	std::vector<QU::UNIT> m_UnitVec;

	//##ModelId=3C2043F901A6
	BOOL Create(CWnd* pParentWnd, UINT nStyle);
	//##ModelId=3C0DC1560119
	CUnitDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUnitDlg)
	enum { IDD = IDD_DIALOGBAR_UNITS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUnitDlg)
	public:
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUnitDlg)
	//##ModelId=3C2043F901D5
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//##ModelId=3C2043F90223
	afx_msg void OnSelchangeComboUnits();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UNITDLG_H__60282629_A8C2_45F4_8387_389BC8BE8F6E__INCLUDED_)
