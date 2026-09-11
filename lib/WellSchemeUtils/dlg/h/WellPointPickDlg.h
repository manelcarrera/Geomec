#if !defined(AFX_WELLPOINTPICKDLG_H__48659176_3BFE_4C57_A073_DCE39E54A1B2__INCLUDED_)
#define AFX_WELLPOINTPICKDLG_H__48659176_3BFE_4C57_A073_DCE39E54A1B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WellPointPickDlg.h : header file
//

//#include <qptrvector.h> 
#include "resource.h"
#include "WellPointListCtrl.h"
#include "NewWellSection.h"
#include "DoubleEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CWellPointPickDlg dialog


class AFX_EXT_CLASS CWellPointPickDlg : public CDialog
{
// Construction
public:
  CWellPointPickDlg
  ( std::list<CNewWellPoint> *pList
  , CNewWellPathBase& wellpath
  , std::list<std::list<INewWellSection *> > Sections
  , CDoubleQuantity::UNIT unit = CDoubleQuantity::SI_UNIT
  , CWnd* pParent =NULL
  );

	virtual CNewWellPoint OnCreateWellPoint(CNewWellPathBase& wellpath, double tmd, bool &bDefined);
	virtual void OnAddDefault(std::list<CNewWellPoint> &list)= 0;
	virtual CString OnOutOfBoundaryMessage();

	CWellPointListCtrl&  WellPointListCtrl() {return m_WellPointListCtrl;}
	virtual ~CWellPointPickDlg();

private:
// Dialog Data
	//{{AFX_DATA(CWellPointPickDlg)
	enum { IDD = UT_IDD_DIALOG_PICK_POINTS };
	CDoubleEdit	m_EditNrOfPoints;
	CDoubleEdit	m_EditRangeTo;
	CDoubleEdit	m_EditRangeFrom;
	CDoubleEdit	m_EditSingle;
	//}}AFX_DATA

	CButton *m_pRadioSingle;
	CButton *m_pRadioRange;
	CButton *m_pReset;

	std::list<CNewWellPoint> *m_pNewOrgList;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWellPointPickDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  CNewWellPathBase *m_pNewWellPath;

  std::list<CNewWellPoint> m_NewList;
  std::list<std::list<INewWellSection *> > m_NewSections;


	LRESULT OnValidateDoubleEdit(WPARAM id, LPARAM lparam);
	CDoubleQuantity::UNIT Unit();
	//void UpdateList();
	void SetButtonState();
	void OnRadioRange(); 
	void OnRadioSinglePoint(); 
	long    m_NrOfPoints;

	CLengthQuantity* m_pSingleValue;
	CLengthQuantity* m_pFrom;
	CLengthQuantity* m_pTo;
	CDependenciesManager* m_pDepManager;

	CWellPointListCtrl m_WellPointListCtrl;
	void  OnClickList(NMHDR* pNMHDR,LRESULT* pResult);
	void  OnItemchangingList(NMHDR* pNMHDR,LRESULT* pResult);

	// Generated message map functions
	//{{AFX_MSG(CWellPointPickDlg)
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonRemove();
	afx_msg void OnButtonAddDefault();

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WELLPOINTPICKDLG_H__48659176_3BFE_4C57_A073_DCE39E54A1B2__INCLUDED_)
