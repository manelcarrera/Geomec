#if !defined(AFX_FORMATIONDEFINITIONFORM_H__96235DDF_3DF7_4542_A084_A05416B60EE3__INCLUDED_)
#define AFX_FORMATIONDEFINITIONFORM_H__96235DDF_3DF7_4542_A084_A05416B60EE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FormationDefinitionForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFormationDefinitionForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "DoubleEdit.h"
#include "BaseFormView.h"
#include "FormationDefinitionDraw.h"
#include "CheckFrm.h"

class CFormation;

//##ModelId=3C0DC15A032C
class CFormationDefinitionForm : public CBaseFormView
{
private:
	
	//##ModelId=3C20440001A7
	CFormation* m_pCurrentFormation;
	//##ModelId=3C2044000204
	void BuildFormationDefinitionList();
	//##ModelId=3C2044000243
	void AskForRemovingAllFormations(CDoubleQuantity* pQuantity,CDoubleEdit* pEdit);

	//##ModelId=3CAAA66802C3
	CFormationDefinitionDraw m_FormationDefinitionDraw;
protected:
	//##ModelId=3C0DC15A0341
	CFormationDefinitionForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CFormationDefinitionForm)

	//##ModelId=3CAAA66802E2
	CCheckFrame m_optOffShore;
	//##ModelId=3CAAA66802F2
	CCheckFrame m_optOnShore;

// Form Data
public:
	//##ModelId=3C0DC15A0360
	virtual BOOL CheckComplete() const;
	//##ModelId=3C0DC15A0362
	virtual BOOL CheckEntry() const;


private:
	//{{AFX_DATA(CFormationDefinitionForm)
	enum { IDD = IDD_FormationDefinition };
	//##ModelId=3CAAA6680311
	CListCtrl	m_lstFormationName;
	//##ModelId=3C0DC15A037A
	int m_intOnShoreDefinition;
	//##ModelId=3CAAA6680321
	CDoubleEdit	m_debOverburdenBottomTVD;
	//##ModelId=3CAAA6680330
	CDoubleEdit	m_debModelTopTVD;
	//##ModelId=3CAAA6680350
	CDoubleEdit	m_debModelBottomTVD;
	//##ModelId=3CAAA668035F
	CDoubleEdit	m_debMeanGroundLevel;
	//##ModelId=3CAAA668037E
	CDoubleEdit	m_deSeaLevel;
	//##ModelId=3CAAA668038E
	CDoubleEdit	m_deWaterDepth;
	//}}AFX_DATA



	

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFormationDefinitionForm)
	public:
	//##ModelId=3C0DC15B0000
	virtual void OnInitialUpdate();
	protected:
	//##ModelId=3C0DC15B0002
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	

// Implementation
protected:
	//##ModelId=3C0DC15B0011
	virtual ~CFormationDefinitionForm();
#ifdef _DEBUG
	//##ModelId=3C0DC15B001F
	virtual void AssertValid() const;
	//##ModelId=3C0DC15B0021
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CFormationDefinitionForm)
	//##ModelId=3C0DC15B002E
	afx_msg void OnKillfocusMeanSeaLevel();
	//##ModelId=3C0DC15B0030
	afx_msg void OnKillfocusWaterDepth();
	//##ModelId=3C0DC15B0032
	afx_msg void OnKillfocusMeanGroundLevel();
	//##ModelId=3C0DC15B003E
	afx_msg void OnKillfocusModelBottomTVD();
	//##ModelId=3C0DC15B0040
	afx_msg void OnKillfocusModelTopTVD();
	//##ModelId=3C0DC15B0042
	afx_msg void OnKillfocusOverburdenBottomTVD();
	//##ModelId=3C0DC15B004E
	afx_msg void OnoptOffshoreDefinition();
	//##ModelId=3C0DC15B0050
	afx_msg void OnoptOnshoreDefinition();
	//##ModelId=3C0DC15B0052
	afx_msg void OnAddFormation();
	//##ModelId=3C0DC15B005D
	afx_msg void OnRemoveFormation();
	//##ModelId=3C0DC15B005F
	afx_msg void OnClicklstFormationDefinition(NMHDR* pNMHDR, LRESULT* pResult);
	//##ModelId=3C8D9E1A038C
	afx_msg void OnKeydownlstFormationDefinition(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	//##ModelId=3CA011A40232
	virtual afx_msg LRESULT OnValidateDoubleEdit(WPARAM id, LPARAM lparam); 
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMATIONDEFINITIONFORM_H__96235DDF_3DF7_4542_A084_A05416B60EE3__INCLUDED_)
