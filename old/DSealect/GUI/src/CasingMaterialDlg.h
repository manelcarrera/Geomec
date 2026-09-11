// CasingMaterialDlg.h: interface for the CCasingMaterialDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CASINGMATERIALDLG_H__90905849_83F6_41EC_8B44_07E934DAC6C9__INCLUDED_)
#define AFX_CASINGMATERIALDLG_H__90905849_83F6_41EC_8B44_07E934DAC6C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CheckFrm.h"
#include "Dependency.h"
#include "OXToolTipCtrl.h"

class CCasingMaterial;
class CDataModel;

//##ModelId=3C60D57E033D
class CCasingMaterialDlg : public CDialog
{
private:
	//##ModelId=3CA011AD01F3
	BOOL m_bDependencyWithModel;
	//##ModelId=3C8D9E250292
	UINT m_iControlToValidate;
	//##ModelId=3CA011AD032D
	CDoubleQuantity* m_pCasingOuter;
	//##ModelId=3CAAA671014C
	CCheckFrame m_fraPlasticity;
	//##ModelId=3CAAA671016B
	CCasingMaterial m_Material;

	//##ModelId=3CA011AD035C
	CDataModel *m_pDataModel;

	//##ModelId=3C6A14CD02FB
	//BOOL PreTranslateMessage(MSG* pMsg);

	//##ModelId=3C8D9E2503CA
	void SetUnitlabel(int nID,CDoubleQuantity* pQ);

	//##ModelId=3DDA0C5E017B
	COXToolTipCtrl m_toolTip;

	//##ModelId=3C8D9E270188
	class CDep1: public CDependency
	{
	public:
		//##ModelId=3C8D9E270206
		CDep1(CCasingMaterial* pCM);
		//##ModelId=3C8D9E270216
		virtual bool CheckQuantity(const CDoubleQuantity* pQuantity) const;
		//##ModelId=3C8D9E270219
		std::string GetDependencyString() const;
	};
	
		
// Construction
public:
	
	//##ModelId=3C60D57E037B
	CCasingMaterialDlg( CCasingMaterial& material,CDataModel* pDataModel,BOOL bDependencyWithModel=FALSE ,CWnd* pParent = NULL);   // standard constructor
	
	//##ModelId=3C8D9E26030F
	~CCasingMaterialDlg();

	//##ModelId=3C60D57E038C
	CCasingMaterial Material();


	

// Dialog Data
	//{{AFX_DATA(CCasingMaterialDlg)
	enum { IDD = IDD_EditCasingMaterial };
	//##ModelId=3CAAA671019A
	CButton		m_chkPlasticity;
	//##ModelId=3CAAA67101B9
	CDoubleEdit	m_dbYoungsModulus;
	//##ModelId=3CAAA67101D9
	CDoubleEdit	m_dbVolumetricSpecificHeat;
	//##ModelId=3CAAA67101F8
	CDoubleEdit	m_dbThermalExpansion;
	//##ModelId=3CAAA6710217
	CDoubleEdit	m_dbThermalConductivity;
	//##ModelId=3CAAA6710246
	CDoubleEdit	m_dbPoissonRatio;
	//##ModelId=3CAAA6710265
	CEdit		m_CasingType;
	//##ModelId=3CAAA6710275
	CDoubleEdit	m_dbCasingOuterDiameter;
	//##ModelId=3CAAA67102A4
	CDoubleEdit	m_CasingInnerDiameter;
	//##ModelId=3CAAA67102C3
	CDoubleEdit	m_edWeightPerUnitLength;
	//##ModelId=3CAAA67102E2
	CDoubleEdit	m_dbYieldStrength;
	//##ModelId=3C60D57F00AC
	CString m_sMatName;
	//##ModelId=3C60D57F00CC
	BOOL m_bPlasticity;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCasingMaterialDlg)
	public:
	//##ModelId=3C6A14CD02FB
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	//##ModelId=3C60D57F00DB
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCasingMaterialDlg)
	//##ModelId=3C60D57F00EB
	afx_msg void OnKillfocusName();
	//##ModelId=3C6A14CD0359
	afx_msg void OnchkPlasticity();
	//##ModelId=3C6A14CD035B
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	//##ModelId=3CA011AE0242
	virtual afx_msg LRESULT OnValidateDoubleEdit(WPARAM id, LPARAM lparam); 
	DECLARE_MESSAGE_MAP()

private:

	//##ModelId=3C8D9E260283
	CDep1 *m_pDep1;
		

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif // !defined(AFX_CASINGMATERIALDLG_H__90905849_83F6_41EC_8B44_07E934DAC6C9__INCLUDED_)
