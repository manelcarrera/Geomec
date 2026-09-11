#if !defined(AFX_WELLPOINTLISTDLG_H__DD143546_8CC9_48B8_86F6_0F466B0DABDB__INCLUDED_)
#define AFX_WELLPOINTLISTDLG_H__DD143546_8CC9_48B8_86F6_0F466B0DABDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WellPointListDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWellPointListDlg dialog
//#include <qptrvector.h> 
#include "resource.h"
#include "WellPointListCtrl.h"

class AFX_EXT_CLASS CWellPointListDlg : public CDialog
{
// Construction
public:
	CWellPointListDlg(well::CWellPathBase& wellpath,
						CDoubleQuantity::UNIT unit = CDoubleQuantity::SI_UNIT,
						CWnd* pParent = NULL,
						QVector<well::CWellSectionList*>* pSections=NULL);   // standard constructor

	virtual ~CWellPointListDlg(){}
// Dialog Data
	//{{AFX_DATA(CWellPointListDlg)
	enum { IDD = UT_IDD_WELLLIST_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CWellPointListCtrl&  WellPointListCtrl() {return m_WellPointListCtrl;}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWellPointListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWellPointListDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnEditchangeComboLists();
	afx_msg void OnSelchangeComboLists();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	

private:
	well::CWellPathBase* m_pWellPath;
	CWellPointListCtrl m_WellPointListCtrl;
	well::CWellPointList* m_pList;
	IndexPointMap* m_pIndexPoints;
	CDoubleQuantity::UNIT m_Unit;
	QVector<well::CWellSectionList*> m_Sections;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WELLPOINTLISTDLG_H__DD143546_8CC9_48B8_86F6_0F466B0DABDB__INCLUDED_)
