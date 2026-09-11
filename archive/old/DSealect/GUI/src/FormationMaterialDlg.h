#if !defined(AFX_FORMATIONMATERIALDLG_H__9E115B6D_6FE1_40C9_81A2_360C191D3BEC__INCLUDED_)
#define AFX_FORMATIONMATERIALDLG_H__9E115B6D_6FE1_40C9_81A2_360C191D3BEC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FormationMaterialDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFormationMaterialDlg dialog

//#include "OXToolTipCtrl.h"
#include "BaseFormView.h"
#include "CheckFrm.h"
#include "FormationMaterial.h"

//##ModelId=3C60D575036B
class CFormationMaterialDlg : public CDialog
{
private:

	//##ModelId=3DDA0C5301D8
	COXToolTipCtrl m_toolTip;

	//##ModelId=3C8D9E18037C
	int m_iControlToValidate;

	//##ModelId=3CAAA6670330
	CCheckFrame m_fraPlasticity;
	//##ModelId=3CAAA6670350
	CFormationMaterial m_Material;

	//##ModelId=3C6A14C4025F
	BOOL PreTranslateMessage(MSG* pMsg);
	//##ModelId=3C8D9E19009E
	void SetUnitlabel(int nID,CDoubleQuantity* pQ);

// Construction
public:
	//##ModelId=3C60D57503C9
	CFormationMaterialDlg(const CFormationMaterial& Material,CWnd* pParent = NULL);   // standard constructor
	//##ModelId=3C60D5760000
	CFormationMaterial Material();

	
// Dialog Data
	//{{AFX_DATA(CFormationMaterialDlg)
	enum { IDD = IDD_EditFormationMaterial };
	//##ModelId=3CAAA667038E
	CComboBox	m_HardeningType;
	//##ModelId=3CAAA66703AD
	CButton		m_fraElasticity;
	//##ModelId=3CAAA66703CD
	CDoubleEdit	m_dbYoungsModulus;
	//##ModelId=3CAAA6680004
	CDoubleEdit	m_dbVolumetricSpecificHeat;
	//##ModelId=3CAAA6680023
	CDoubleEdit	m_dbThermalExpansion;
	//##ModelId=3CAAA6680042
	CDoubleEdit	m_dbThermalConductivity;
	//##ModelId=3CAAA6680062
	CDoubleEdit	m_dbPoissonRatio;
	//##ModelId=3CAAA6680081
	CEdit		m_edName;
	//##ModelId=3CAAA66800A0
	CDoubleEdit	m_dbHardeningGradient;
	//##ModelId=3CAAA66800BF
	CDoubleEdit	m_dbFrictionAngle;
	//##ModelId=3CAAA66800DF
	CDoubleEdit	m_FractureEnergy;
	//##ModelId=3CAAA66800FE
	CDoubleEdit	m_dbCohesion;
	//##ModelId=3C60D57601D5
	CString	m_sMatName;
	//##ModelId=3C60D5760214
	BOOL	m_bPlasticity;
	CDoubleEdit m_dbDensity;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFormationMaterialDlg)
	protected:
	//##ModelId=3C60D5760262
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFormationMaterialDlg)
	//##ModelId=3C60D57602A5
	afx_msg void OnKillfocusName();
	//##ModelId=3C60D57602C5
	virtual BOOL OnInitDialog();
	//##ModelId=3C60D57602D0
	afx_msg void OnchkPlasticity();
	//##ModelId=3C6A14C402EC
	afx_msg void OnCloseupcmbHardeningType();
	//}}AFX_MSG
	//##ModelId=3CA011A30109
	virtual afx_msg LRESULT OnValidateDoubleEdit(WPARAM id, LPARAM lparam); 
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMATIONMATERIALDLG_H__9E115B6D_6FE1_40C9_81A2_360C191D3BEC__INCLUDED_)
