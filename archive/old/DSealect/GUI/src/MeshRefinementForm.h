#if !defined(AFX_MESHREFINEMENTFORM_H__F85AF2DD_A0FF_4DE7_9115_21171E47ADDB__INCLUDED_)
#define AFX_MESHREFINEMENTFORM_H__F85AF2DD_A0FF_4DE7_9115_21171E47ADDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MeshRefinementForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMeshRefinementForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "BaseFormView.h"
#include "MeshRefinementDraw.h"
#include "CheckFrm.h"


//##ModelId=3C0DC15803B9
class CMeshRefinementForm : public CBaseFormView
{
private:

protected:
	//##ModelId=3C0DC15803CD
	CMeshRefinementForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMeshRefinementForm)

	//##ModelId=3CAAA66402D3
	CCheckFrame m_optAdvancedMeshRefinement;

// Form Data
public:

	//{{AFX_DATA(CMeshRefinementForm)
	enum { IDD = IDD_MeshRefinement };
	//##ModelId=3C60D572038C
	CSliderCtrl	m_sldRockRefinement;
	//##ModelId=3C60D57203AB
	CSliderCtrl	m_sldCementRefinement;
	//##ModelId=3C60D57203CA
	CSliderCtrl	m_sldCasingRefinement;
	//##ModelId=3C60D57203D9
	int		m_iCasingRefLevel;
	//##ModelId=3C60D5730000
	int		m_iCementRefLevel;
	//##ModelId=3C60D5730010
	int		m_iRockRefLevel;
	//##ModelId=3C8D9E14032E
	int m_intDefaultMeshRefinement;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	//##ModelId=3C0DC15803DD
	BOOL CheckComplete() const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMeshRefinementForm)
	public:
	//##ModelId=3C0DC159000F
	virtual void OnInitialUpdate();
	protected:
	//##ModelId=3C0DC1590011
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//##ModelId=3C0DC159001F
	virtual ~CMeshRefinementForm();
#ifdef _DEBUG
	//##ModelId=3C0DC1590021
	virtual void AssertValid() const;
	//##ModelId=3C0DC1590023
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CMeshRefinementForm)
	//##ModelId=3C0DC159002F
	afx_msg void OnoptAdvancedTuningOfMeshRefinement();
	//##ModelId=3C0DC1590031
	afx_msg void OnoptDefaultMeshRefinement();
	//##ModelId=3C60D5730020
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//##ModelId=3CA0119F0290
	afx_msg void OnSetfocusebCasingRefinementLevel();
	//##ModelId=3CA0119F02BF
	afx_msg void OnSetfocusebCementRefinementLevel();
	//##ModelId=3CA0119F02CE
	afx_msg void OnSetfocusebRockRefinementLevel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MESHREFINEMENTFORM_H__F85AF2DD_A0FF_4DE7_9115_21171E47ADDB__INCLUDED_)
