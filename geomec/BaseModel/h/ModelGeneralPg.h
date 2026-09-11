#if !defined(AFX_MODELGENERALPG_H__FC1838FC_956B_4E02_88D6_6E0615B93403__INCLUDED_)
#define AFX_MODELGENERALPG_H__FC1838FC_956B_4E02_88D6_6E0615B93403__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModelGeneralPg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModelGeneralPg dialog

//##ModelId=3BC55D610276
#include "Geomecdoc.h"
#include "StaticFile.h"

class CModelGeneralPg : public CPropertyPage
{
// Construction
	CGeomecDoc& m_doc;
public:
	//##ModelId=3BC55D610278
	CModelGeneralPg(CGeomecDoc &doc);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CModelGeneralPg)
	enum { IDD = IDD_ATTRI_MODEL_GENERAL };
	CStaticFile	m_stPath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModelGeneralPg)
	protected:
	//##ModelId=3BC55D610279
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModelGeneralPg)
	afx_msg void OnChangeProjectInfo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODELGENERALPG_H__FC1838FC_956B_4E02_88D6_6E0615B93403__INCLUDED_)
