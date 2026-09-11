#if !defined(AFX_BASEFORMVIEW_H__03893C5D_14A9_4D67_BD95_5E1DA2290813__INCLUDED_)
#define AFX_BASEFORMVIEW_H__03893C5D_14A9_4D67_BD95_5E1DA2290813__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BaseFormView.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CBaseFormView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif


#include "DoubleEdit.h"
#include "DSealectDoc.h"
#include "OXToolTipCtrl.h"

class ITreeEntryBase;
class CTreeEntryBranch;
class CTreeEntryLeaf;
class CDSealectDrawView;
class CMainFrame;
class CDrawProxy;
class CDataModel;


//##ModelId=3C0DC160033C
class CBaseFormView : public CFormView
{

friend ITreeEntryBase;
friend CTreeEntryBranch;
friend CTreeEntryLeaf;
friend CMainFrame;
friend CDrawProxy;

private:
	//##ModelId=3DDA0C5F0199
	UINT m_nIDTemplate;

public:
	
	//##ModelId=3DDA0C5F01B9
	COXToolTipCtrl m_toolTip;
	
	//##ModelId=3C0DC160034F
	CBaseFormView(UINT nIDTemplate=0 );           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CBaseFormView)
	
	//##ModelId=3CAAA67200DF
	CButton m_PreviousButton;
	//##ModelId=3CAAA672010D
	CButton m_NextButton;

	//##ModelId=3C60D57F02C1
	CButton m_HelpButton;

	//##ModelId=3C0DC1600370
	virtual BOOL CheckComplete() const;
	//##ModelId=3C0DC1600372
	virtual BOOL CheckEntry() const;
	
	//##ModelId=3C0DC160037C
	CDataModel* GetModel() const
	{
		CDSealectDoc* pDoc=(CDSealectDoc*)GetDocument();
		return pDoc->DSealectModel();
	}

	//##ModelId=3C2044070177
	void ShowQuantity(CDoubleEdit* pEdit,CDoubleQuantity* pQuantity);

	//##ModelId=3C0DC160037E
	void ValidateQuantity(CDataExchange* pDX,CDoubleEdit* pDE,CDoubleQuantity *pQ,CString sQuantityName = "Value" );
	//##ModelId=3C0DC160038E
	void SetUnitlabel(int nID,CDoubleQuantity* pQ);


	//##ModelId=3C60D57F02DF
	CFont *GetFont(BOOL bBold);
	//##ModelId=3C20440701C6
	void UpdateButtons();
	//##ModelId=3C20440701D5
	void EnablePreviousButton(BOOL enable);
	//##ModelId=3C20440701F4
	void EnableNextButton(BOOL enable);
	//##ModelId=3C0DC1610023
	BOOL IsInfoForm() const;
	//##ModelId=3C0DC160039B
	void OnNext();
	//##ModelId=3C0DC160039C
	void OnPrevious();


	//##ModelId=3DDA0C5F01D7
	virtual void ShowHelp();

		
	//##ModelId=3C0DC16003A9
	ITreeEntryBase* TreeEntry() const
	{
		return m_pTreeEntry;
	}

	//##ModelId=3C0DC16003AB
	CDSealectDrawView* DrawView() const
	{
		return m_pDrawView;
	}

	//##ModelId=3C0DC16003AD
	void DrawView(CDSealectDrawView *newview) 
	{
		m_pDrawView=newview;
	}


	//{{AFX_DATA(CBaseFormView)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBaseFormView)
	public:
	//##ModelId=3DDA0C5F01E8
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//##ModelId=3DDA0C5F0226
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	//##ModelId=3C0DC16003BC
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//##ModelId=3C0DC16003C9
	virtual void OnDraw(CDC* pDC);
	//##ModelId=3C2044070271
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint); 
	//}}AFX_VIRTUAL

// Implementation
protected:
	//##ModelId=3C0DC16003D8
	virtual ~CBaseFormView();
	//##ModelId=3CAAA672014C
	ITreeEntryBase* m_pTreeEntry;

	//##ModelId=3C8D9E280021
	UINT m_iControlToValidate;

#ifdef _DEBUG
	//##ModelId=3C0DC1610005
	virtual void AssertValid() const;
	//##ModelId=3C0DC1610010
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CBaseFormView)
	//##ModelId=3C0DC1610013
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//##ModelId=3C0DC161001F
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	//##ModelId=3CA011AF034B
	virtual afx_msg LRESULT OnValidateDoubleEdit(WPARAM id, LPARAM lparam); 
	
	DECLARE_MESSAGE_MAP()
	
private:
	
	//##ModelId=3CAAA672017B
	CDSealectDrawView *m_pDrawView;

	//##ModelId=3CAAA672019A
	CFont m_font;
	//##ModelId=3C60D57F031E
	CFont m_fontBold;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BASEFORMVIEW_H__03893C5D_14A9_4D67_BD95_5E1DA2290813__INCLUDED_)
