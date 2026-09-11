// CementMaterialDlg.h: interface for the CCementMaterialDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CEMENTMATERIALDLG_H__F64EF320_8708_4E99_9087_3BBEB424F7CA__INCLUDED_)
#define AFX_CEMENTMATERIALDLG_H__F64EF320_8708_4E99_9087_3BBEB424F7CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// CementMaterialDlg.h : header file
//
#include "BaseFormView.h"
//#include "OXToolTipCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CCementMaterialDlg dialog

class CCementMaterial;

//##ModelId=3C60D57C0149
class CCementMaterialDlg : public CDialog
{
private:

	//##ModelId=3DDA0C5C01B9
	COXToolTipCtrl m_toolTip;

	//##ModelId=3C8D9E210198
	int m_iControlToValidate;
	//##ModelId=3CAAA66F0033
	CCementMaterial m_Material;

	//##ModelId=3C6A14CB0194
	//BOOL PreTranslateMessage(MSG* pMsg);

	//##ModelId=3C8D9E2102C0
	void SetUnitlabel(int nID,CDoubleQuantity* pQ);
// Construction
public:
	//##ModelId=3C60D57C0177
	CCementMaterialDlg(const CCementMaterial& material,CWnd* pParent = NULL);   // standard constructor
	//##ModelId=3C60D57C0189
	CCementMaterial Material();


// Dialog Data
	//{{AFX_DATA(CCementMaterialDlg)
	enum { IDD = IDD_EditCementMaterial };
	//##ModelId=3CAAA66F0062
	CButton		m_fraElasticity;
	//##ModelId=3CAAA66F0081
	CDoubleEdit	m_dbYoungsModulus;
	//##ModelId=3CAAA66F0090
	CDoubleEdit	m_dbVolumetricSpecificHeat;
	//##ModelId=3CAAA66F00B0
	CDoubleEdit	m_dbThermalExpansion;
	//##ModelId=3CAAA66F00CF
	CDoubleEdit	m_dbThermalConductivity;
	//##ModelId=3CAAA66F00EE
	CDoubleEdit	m_dbPoissonRatio;
	//##ModelId=3CAAA66F010D
	CEdit		m_edName;
	//##ModelId=3CAAA66F012D
	CDoubleEdit	m_dbCompHardeningGradient;
	//##ModelId=3CAAA66F014B
	CDoubleEdit	m_dbFrictionAngle;
	//##ModelId=3CAAA66F016B
	CDoubleEdit	m_CompFractureEnergy;
	//##ModelId=3CAAA66F018A
	CDoubleEdit	m_dbCohesion;
	//##ModelId=3CAAA66F01AA
	CDoubleEdit	m_dbTensileStrength;
	//##ModelId=3CAAA66F01C9
	CDoubleEdit	m_dbTensFractureEnergy;
	//##ModelId=3CAAA66F01F8
	CComboBox	m_HardeningType;
	//##ModelId=3CAAA66F0217
	CDoubleEdit	m_VolumetricShrinkage;
	//##ModelId=3CAAA66F0236
	CDoubleEdit	m_HydrationHeat;
	//##ModelId=3C60D57D003F
	CString m_sMatName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCementMaterialDlg)
	public:
	//##ModelId=3C6A14CB0194
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	//##ModelId=3C60D57D008D
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCementMaterialDlg)
	//##ModelId=3C60D57D00FB
	afx_msg void OnKillfocusName();
	//##ModelId=3C60D57D012D
	virtual BOOL OnInitDialog();
	//##ModelId=3C6A14CB0211
	afx_msg void OnCloseupcmbCompHardeningType();
	//}}AFX_MSG
	//##ModelId=3CA011AA00DA
	virtual afx_msg LRESULT OnValidateDoubleEdit(WPARAM id, LPARAM lparam); 

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CEMENTMATERIALDLG_H__F64EF320_8708_4E99_9087_3BBEB424F7CA__INCLUDED_)
