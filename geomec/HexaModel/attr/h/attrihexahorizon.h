#if !defined(AFX_ATTRIHEXAHORIZON_H__3372E412_310B_4F47_BA33_6D108CC45106__INCLUDED_)
#define AFX_ATTRIHEXAHORIZON_H__3372E412_310B_4F47_BA33_6D108CC45106__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HexaHorizon.h"
#include "AttriHorizon.h"
#include "IListObject.h"
#include "ListCtrlBase.h"

class CAttri3DHorizonDlg : public CAttriHorizon<C3DHorizon>
{
// Construction
	class CSurfaceListObject : public IListObject
	{
		CSurfaceBase* m_pSurface;
	public:
		CSurfaceListObject(CListCtrl &ctrl, CSurfaceBase& surface, BOOL bSelected);
		~CSurfaceListObject();
		virtual QString Text() const;
		virtual unsigned int Icon() const;
		const CSurfaceBase& Surface() const;
		CSurfaceBase& Surface();
	};
	BOOL m_bInitialUpdate;
public:
	CAttri3DHorizonDlg(C3DHorizon &horizon, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAttriHorizon)
	enum { IDD = IDD_ATTRI_3DHORIZON };
	CListCtrlBase	m_lbSurface;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAttriHorizon)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void OnUpdateLb();
	// Generated message map functions
	//{{AFX_MSG(CAttriHorizon)
	virtual BOOL OnInitDialog();
	afx_msg void OnConstantDepth();
	afx_msg void OnSurface();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATTRIHEXAHORIZON_H__3372E412_310B_4F47_BA33_6D108CC45106__INCLUDED_)
