// CasingMaterialLibDlg.h: interface for the CCasingMaterialLibDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CASINGMATERIALLIBDLG_H__C1664FF0_94CD_4602_8CDF_A9CC4C6BD76F__INCLUDED_)
#define AFX_CASINGMATERIALLIBDLG_H__C1664FF0_94CD_4602_8CDF_A9CC4C6BD76F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CasingMatListCtrl.h"
#include "CasingMaterialTable.h"
#include "MaterialDatabaseTemplate.h"
#include "CasingMaterialDatabase.h"

//class CCasingMaterial;

//##ModelId=3C60D57E01C6
class CCasingMaterialLibDlg : public CDialog
{
private:
	//##ModelId=3C8D9E2400CC
	void BuildMaterialList();
	//##ModelId=3C60D57E01D6
	CString m_strDatabasePath;
	//##ModelId=3CAAA670036F
	std::auto_ptr<CCasingMaterialDatabase> m_pModelDatabase;
	//##ModelId=3CAAA670039E
	std::auto_ptr<CCasingMaterial> m_pSelectedMaterial;
	//##ModelId=3CA011AC0149
	CDataModel *m_pDataModel;

	//##ModelId=3C60D57E0252
	void ShowMaterial(CString name);

	//##ModelId=3C8D9E240169
	CString strMatName;

	//typedef CMaterialDatabaseTemplate<CCasingMaterialTable> CCasingMatDatabase;

	
// Construction
public:
	//##ModelId=3C60D57E0263
	CCasingMaterialLibDlg(CString sDatabasePath,CDataModel *pDataModel,CWnd* pParent = NULL);   // standard constructor
	//##ModelId=3C60D57E0266
	CCasingMaterial *SelectedMaterial(){return (CCasingMaterial*)m_pSelectedMaterial.get();}

// Dialog Data
	//{{AFX_DATA(CCasingMaterialLibDlg)
	enum { IDD = IDD_CasingLibrary };
	//##ModelId=3CAAA67003CD
	CCasingMatListCtrl	m_lstMaterialParams;
	//##ModelId=3CAAA6710013
	CListCtrl	m_lstMaterialNames;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCasingMaterialLibDlg)
	protected:
	//##ModelId=3C60D57E02A0
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCasingMaterialLibDlg)
	//##ModelId=3C60D57E02B1
	virtual BOOL OnInitDialog();
	//##ModelId=3C60D57E02B3
	afx_msg void OnClicklstCasingLibrary(NMHDR* pNMHDR, LRESULT* pResult);
	//##ModelId=3C60D57E02C1
	afx_msg void OnItemchangedlstCasingLibrary(NMHDR* pNMHDR, LRESULT* pResult);
	//##ModelId=3C60D57E02D0
	afx_msg void OnItemchanginglstCasingLibrary(NMHDR* pNMHDR, LRESULT* pResult);
	//##ModelId=3C60D57E02DF
	afx_msg void OnItemchanginglstCasingLibraryParams(NMHDR* pNMHDR, LRESULT* pResult);
	//##ModelId=3C8D9E240263
	afx_msg void OnbtnDelete();
	//##ModelId=3C8D9E2402E0
	afx_msg void OnbtnEdit();
	//##ModelId=3C8D9E24038C
	afx_msg void OnbtnNew();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.



#endif // !defined(AFX_CASINGMATERIALLIBDLG_H__C1664FF0_94CD_4602_8CDF_A9CC4C6BD76F__INCLUDED_)
