// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__919DDFB7_0F2B_4639_84AC_AF966B1782F1__INCLUDED_)
#define AFX_MAINFRM_H__919DDFB7_0F2B_4639_84AC_AF966B1782F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "treeobject.h"
#include "UnitDlg.h"

class CDSealectView;
class CSealectTreeView;
class CMultiViewSplitter;
class CDSealectDrawView;


//##ModelId=3C0DC1590128
class CMainFrame : public CFrameWnd
{

	//##ModelId=3CAAA6650284
	CMultiViewSplitter m_wndSplitter;

protected: // create from serialization only
	//##ModelId=3C0DC1590157
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	
// Operations
public:

private:
	//##ModelId=3C0DC1590158
	void GetInfoString(const CString &sFrom,CString &sInfo);
	//##ModelId=3C0DC159015B
	BOOL CreateViews(CCreateContext* pContext);
	//##ModelId=3CA011A00261
	CTreeEntryLeaf* InsertTreeEntryLeaf(CRuntimeClass* FormView,CRuntimeClass* DrawView,CString text,UINT IconID,CCreateContext* pContext, ITreeEntryBase* pParentEntry,CTreeEntryBranch* pBranch);
	//##ModelId=3CA011A002CE
	CTreeEntryBranch* InsertTreeEntryBranch(CRuntimeClass* FormView,CRuntimeClass* DrawView,CString LabelText,CString InfoText,UINT IconID,CCreateContext* pContext, ITreeEntryBase* pParentEntry);

	//##ModelId=3CAAA66502A4
	CDSealectTreeView* m_pTreeView;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	//##ModelId=3C0DC159017C
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//##ModelId=3C2043FD0129
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	//##ModelId=3C0DC1590188
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	//##ModelId=3C0DC1590197
	virtual ~CMainFrame();
#ifdef _DEBUG
	//##ModelId=3C0DC1590199
	virtual void AssertValid() const;
	//##ModelId=3C0DC15901A5
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	//##ModelId=3CAAA66502C3
	CStatusBar  m_wndStatusBar;
	//##ModelId=3CAAA66502E2
	CToolBar    m_wndToolBar;
	//##ModelId=3C2043FD01E5
	CUnitDlg    m_wndUnitBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	//##ModelId=3C0DC15901D4
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//##ModelId=3C0DC15901E6
	afx_msg void OnDestroy();
	//##ModelId=3C8D9E15037C
	afx_msg void OnActionsCasinglibrary();
	//##ModelId=3C8D9E15038D
	afx_msg void OnActionsCementlibrary();
	//##ModelId=3C8D9E15039B
	afx_msg void OnActionsFormationlibrary();
	//##ModelId=3DDA0C510293
	afx_msg void OnUpdateAppRun(CCmdUI* pCmdUI);
	//##ModelId=3DDA0C5102A3
	afx_msg void OnActionsOptions();
	//##ModelId=3DDA0C5102A5
	afx_msg void OnAppTooltip();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__919DDFB7_0F2B_4639_84AC_AF966B1782F1__INCLUDED_)
