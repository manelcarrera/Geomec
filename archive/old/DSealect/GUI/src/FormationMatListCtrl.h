#if !defined(AFX_FORMATIONMATLISTCTRL_H__976330DC_F9E5_46B0_8B9E_FAA712951FD7__INCLUDED_)
#define AFX_FORMATIONMATLISTCTRL_H__976330DC_F9E5_46B0_8B9E_FAA712951FD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FormationMatListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFormationMatListCtrl window

class CFormationMaterial;

//##ModelId=3C60D57502B0
class CFormationMatListCtrl : public CListCtrl
{
private:
	//##ModelId=3C60D57502CF
	void CreateColums();


// Construction
public:
	//##ModelId=3C60D57502D0
	CFormationMatListCtrl();
	//##ModelId=3C60D57502D1
	void ShowMaterial(CFormationMaterial& mat);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFormationMatListCtrl)
	protected:
	//##ModelId=3C60D57502D3
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	//##ModelId=3C60D57502DF
	virtual ~CFormationMatListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFormationMatListCtrl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMATIONMATLISTCTRL_H__976330DC_F9E5_46B0_8B9E_FAA712951FD7__INCLUDED_)
