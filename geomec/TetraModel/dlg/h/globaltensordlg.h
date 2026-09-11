#if !defined(AFX_GLOBALTENSORDLG_H__C59F3BA3_A205_40BE_9CE3_6AC019F4603A__INCLUDED_)
#define AFX_GLOBALTENSORDLG_H__C59F3BA3_A205_40BE_9CE3_6AC019F4603A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// globaltensordlg.h : header file
//
#include "3DSupportNode.h"
#include "AttributesTemplate.h"

/////////////////////////////////////////////////////////////////////////////
// CGlobalTensorDlg dialog

class CGlobalTensorDlg : public CAttributesTemplate<C3DSupportNode>
{
// Construction
public:
	CGlobalTensorDlg(C3DSupportNode &supnode, CWnd* pParent = NULL);

	void SetInitialDepletionStage(int nStage);

// Dialog Data
	//{{AFX_DATA(CGlobalTensorDlg)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGlobalTensorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGlobalTensorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnGeneral();
	afx_msg void OnUndefined();
	afx_msg void OnVertical();
	afx_msg void OnSelchangeStage();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	int m_nDeplStage;
	bool m_bInitialized;
	C3DSupportNode &m_SourceNode;
	std::vector<const CDepletionStage*> m_vcDepletionStages;

	void UpdateControls();
	void EmptyEditBoxes();
	BOOL CheckInput();
	void FillDepletionStageVector();
	void FillDepletionStageCombo();
	void SetRadioButton();
	void GetRadioButton();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLOBALTENSORDLG_H__C59F3BA3_A205_40BE_9CE3_6AC019F4603A__INCLUDED_)
