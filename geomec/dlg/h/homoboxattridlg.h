#if !defined(AFX_SCALINBOXATTRIDLG_H__3AAF4B41_2ECE_424B_8FB0_273626F7E958__INCLUDED_)
#define AFX_SCALINBOXATTRIDLG_H__3AAF4B41_2ECE_424B_8FB0_273626F7E958__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// scalinboxattridlg.h : header file
//

#include "HomogenizationBox.h"
#include "AttributeTempl.h"



class CHomoBoxAttriDlg : public CAttributesTemplate<CHomogenizationBox>
{
	IQuantityDouble::UNIT m_unit;
	//CString m_Name;
// Construction
public:
	CHomoBoxAttriDlg(const CFemAppModel &model, CHomogenizationBox &homo_box, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHomoBoxAttriDlg)
	enum { IDD = IDD_ATTRI_HOMOGENIZATION };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHomoBoxAttriDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	virtual BOOL OnInitDialog();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHomoBoxAttriDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void configureDivisionCombo(CComboBox& combo, int division);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCALINBOXATTRIDLG_H__3AAF4B41_2ECE_424B_8FB0_273626F7E958__INCLUDED_)
