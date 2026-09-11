// CementMaterialLibDlg.h: interface for the CCementMaterialLibDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CEMENTMATERIALLIBDLG_H__7F48C2A8_AC24_40DF_AD5A_81319658FF0E__INCLUDED_)
#define AFX_CEMENTMATERIALLIBDLG_H__7F48C2A8_AC24_40DF_AD5A_81319658FF0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// CementMatrialLibDlg.h : header file
//
#include "CementMatListCtrl.h"
#include "CementMaterialTable.h"
#include "MaterialDatabaseTemplate.h"
#include "CementMaterialDatabase.h"

//class CCementMaterial;

//##ModelId=3C60D57B02EE
class CCementMaterialLibDlg : public CDialog
{
private:
	//##ModelId=3C8D9E1F031E
	void BuildMaterialList();
	//##ModelId=3C60D57B02FF
	CString m_strDatabasePath;
	//##ModelId=3CAAA66E0275
	std::auto_ptr<CCementMaterialDatabase> m_pModelDatabase;
	//##ModelId=3CAAA66E02A4
	std::auto_ptr<CCementMaterial> m_pSelectedMaterial;
	//##ModelId=3C60D57B039A
	void ShowMaterial(CString name);

	//##ModelId=3C8D9E1F03BA
	CString strMatName;

	//typedef CMaterialDatabaseTemplate<CCementMaterialTable> CCementMatDatabase;

	
// Construction
public:
	//##ModelId=3C60D57B03AA
	CCementMaterialLibDlg(CString sDatabasePath,CWnd* pParent = NULL);   // standard constructor
	//##ModelId=3C60D57B03C9
	CCementMaterial *SelectedMaterial(){return (CCementMaterial*)m_pSelectedMaterial.get();}

// Dialog Data
	//{{AFX_DATA(CCementMaterialLibDlg)
	enum { IDD = IDD_CementLibrary };
	//##ModelId=3CAAA66E02E2
	CCementMatListCtrl	m_lstMaterialParams;
	//##ModelId=3CAAA66E0311
	CListCtrl	m_lstMaterialNames;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCementMaterialLibDlg)
	protected:
	//##ModelId=3C60D57C0020
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCementMaterialLibDlg)
	//##ModelId=3C60D57C003F
	virtual BOOL OnInitDialog();
	//##ModelId=3C60D57C0041
	afx_msg void OnClicklstCementLibrary(NMHDR* pNMHDR, LRESULT* pResult);
	//##ModelId=3C60D57C005E
	afx_msg void OnItemchangedlstCementLibrary(NMHDR* pNMHDR, LRESULT* pResult);
	//##ModelId=3C60D57C008D
	afx_msg void OnItemchanginglstCementLibrary(NMHDR* pNMHDR, LRESULT* pResult);
	//##ModelId=3C60D57C00AC
	afx_msg void OnItemchanginglstCementLibraryParams(NMHDR* pNMHDR, LRESULT* pResult);
	//##ModelId=3C8D9E2000AD
	afx_msg void OnbtnDelete();
	//##ModelId=3C8D9E20013A
	afx_msg void OnbtnEdit();
	//##ModelId=3C8D9E2001B7
	afx_msg void OnbtnNew();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif // !defined(AFX_CEMENTMATERIALLIBDLG_H__7F48C2A8_AC24_40DF_AD5A_81319658FF0E__INCLUDED_)
