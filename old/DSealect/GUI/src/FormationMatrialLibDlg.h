#if !defined(AFX_FORMATIONMATRIALLIBDLG_H__C7F89E09_262F_4C9A_B2E5_F26096C56902__INCLUDED_)
#define AFX_FORMATIONMATRIALLIBDLG_H__C7F89E09_262F_4C9A_B2E5_F26096C56902__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FormationMatrialLibDlg.h : header file
//

#include "FormationMatListCtrl.h"
#include "FormationMaterialTable.h"
#include "MaterialDatabaseTemplate.h"
#include "FormationMaterialDatabase.h"

/////////////////////////////////////////////////////////////////////////////
// CFormationMatrialLibDlg dialog

//##ModelId=3C60D57500FA
class CFormationMatrialLibDlg : public CDialog
{
private:
	//##ModelId=3C8D9E170188
	void BuildMaterialList();
	//##ModelId=3C60D575011A
	CString m_strDatabasePath;
	//##ModelId=3CAAA66700B0
	std::auto_ptr<CFormationMaterialDatabase> m_pModelDatabase;
	//##ModelId=3CAAA66700EE
	std::auto_ptr<CFormationMaterial> m_pSelectedMaterial;
	//##ModelId=3C60D5750197
	void ShowMaterial(CString name);

	//##ModelId=3C8D9E170215
	CString strMatName;


// Construction
public:
	//##ModelId=3C60D57501A6
	CFormationMatrialLibDlg(CString sDatabasePath,CWnd* pParent = NULL);   // standard constructor
	//##ModelId=3C60D57501B6
	CFormationMaterial *SelectedMaterial(){return (CFormationMaterial*)m_pSelectedMaterial.get();}


// Dialog Data
	//{{AFX_DATA(CFormationMatrialLibDlg)
	enum { IDD = IDD_FormationLibrary };
	//##ModelId=3CAAA667012D
	CFormationMatListCtrl	m_lstMaterialParams;
	//##ModelId=3CAAA667015C
	CListCtrl	m_lstMaterialNames;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFormationMatrialLibDlg)
	protected:
	//##ModelId=3C60D57501F4
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFormationMatrialLibDlg)
	//##ModelId=3C60D5750204
	virtual BOOL OnInitDialog();
	//##ModelId=3C60D5750206
	afx_msg void OnClicklstFormationLibrary(NMHDR* pNMHDR, LRESULT* pResult);
	//##ModelId=3C60D5750233
	afx_msg void OnItemchangedlstFormationLibrary(NMHDR* pNMHDR, LRESULT* pResult);
	//##ModelId=3C60D5750252
	afx_msg void OnItemchanginglstFormationLibrary(NMHDR* pNMHDR, LRESULT* pResult);
	//##ModelId=3C60D5750281
	afx_msg void OnItemchanginglstFormationLibraryParams(NMHDR* pNMHDR, LRESULT* pResult);
	//##ModelId=3C8D9E1702FF
	afx_msg void OnbtnDelete();
	//##ModelId=3C8D9E17037C
	afx_msg void OnbtnEdit();
	//##ModelId=3C8D9E180011
	afx_msg void OnbtnNew();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMATIONMATRIALLIBDLG_H__C7F89E09_262F_4C9A_B2E5_F26096C56902__INCLUDED_)
