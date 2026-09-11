// BaseFormView.cpp : implementation file
//

#include "stdafx.h"
#include "dsealect.h"
#include "treeobject.h"
#include "BaseFormView.h"
#include "InfoForm.h"
#include "MultiViewSplitter.h"
#include "DSealectDrawView.h"
#include "StringUtils.h"
#include "CDependenciesManager.h"
#include "DoubleFormat.h"
#include "DSealectGlobal.h"
#include "MainFrm.h"
#include "DDXQuantity.h"
#include "CDataModel.h"

#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

#define FONTHEIGH 8
#define TOPOFBUTTONS 630


/////////////////////////////////////////////////////////////////////////////
// CBaseFormView

IMPLEMENT_DYNCREATE(CBaseFormView, CFormView)



//##ModelId=3C0DC160034F
CBaseFormView::CBaseFormView(UINT nIDTemplate )
	: CFormView(nIDTemplate),m_pTreeEntry(0)
{
	m_nIDTemplate=nIDTemplate;
	m_pDrawView=NULL;
	m_pTreeEntry=NULL;
	m_iControlToValidate=0;
	//{{AFX_DATA_INIT(CBaseFormView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//##ModelId=3C0DC16003D8
CBaseFormView::~CBaseFormView()
{
}

/*
void CBaseFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBaseFormView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}
*/


BEGIN_MESSAGE_MAP(CBaseFormView, CFormView)
	//{{AFX_MSG_MAP(CBaseFormView)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnNext)
	ON_BN_CLICKED(IDC_BUTTON_PREVIOUS, OnPrevious)
	ON_BN_CLICKED(IDC_BUTTON_HELP, ShowHelp)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_VALIDATE_DOUBLE_EDIT, OnValidateDoubleEdit)

END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBaseFormView diagnostics

#ifdef _DEBUG
//##ModelId=3C0DC1610005
void CBaseFormView::AssertValid() const
{
	CFormView::AssertValid();
}

//##ModelId=3C0DC1610010
void CBaseFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CBaseFormView message handlers
//##ModelId=3C0DC16003BC
void CBaseFormView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	CFormView::OnActivateView(bActivate, pActivateView, pDeactiveView);

}



//##ModelId=3C0DC1610013
int CBaseFormView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{

	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CClientDC dc(this);
	int nHeight =((dc.GetDeviceCaps(LOGPIXELSY)*FONTHEIGH)/72);

	
	m_font.CreateFont(nHeight,0,0,0,FW_NORMAL,0,0,0,
		DEFAULT_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE , "MS Sans Serif");

	m_fontBold.CreateFont(nHeight,0,0,0,FW_BOLD,0,0,0,
		DEFAULT_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE , "MS Sans Serif");
	
	int iTop = TOPOFBUTTONS;
	int iWidth=80;
	int iHeight=25;

	CRect rPrev(10,iTop,10+iWidth,iTop+iHeight);
	CRect rNext(200,iTop,200+iWidth,iTop+iHeight);
	CRect rHelp(200,iTop-(1.2*iHeight),200+iWidth,iTop+iHeight-(1.2*iHeight));
	
	m_PreviousButton.Create("<- Previous",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON | WS_TABSTOP,rPrev,this,IDC_BUTTON_PREVIOUS);
	m_NextButton.Create("Next ->",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON | WS_TABSTOP,rNext,this,IDC_BUTTON_NEXT);
	
	m_HelpButton.Create("Help",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON | WS_TABSTOP,rHelp,this,IDC_BUTTON_HELP);

	m_NextButton.SetFont(GetFont(TRUE));
	m_PreviousButton.SetFont(GetFont(TRUE));
	m_HelpButton.SetFont(GetFont(FALSE));

	return 0;
}

//##ModelId=3C0DC160039B
void CBaseFormView::OnNext() 
{
	if(TreeEntry()->Next())
	{
		TreeEntry()->Next()->Activate(TreeEntry());
	}
}

//##ModelId=3C0DC160039C
void CBaseFormView::OnPrevious() 
{
	if(TreeEntry()->Previous())
	{
		TreeEntry()->Previous()->Activate(TreeEntry());
	}
}


//##ModelId=3DDA0C5F01D7
void CBaseFormView::ShowHelp()
{
	WinHelp(m_nIDTemplate);
}

//##ModelId=3C0DC161001F
void CBaseFormView::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CFormView::OnShowWindow(bShow, nStatus);
	
	if(bShow)
	{
		if(DrawView())
		{
			DrawView()->Invalidate();
		}
		UpdateButtons();
	}
}

//##ModelId=3C0DC16003C9
void CBaseFormView::OnDraw(CDC* pDC) 
{
	CFont *POldFont = pDC->SelectObject(&m_font);
	if(IsInfoForm())
	{
		long nHeight = ((pDC->GetDeviceCaps(LOGPIXELSY)*FONTHEIGH)/72);
	
		CTreeEntryBranch* pEntry=(CTreeEntryBranch*)TreeEntry();
		if(pEntry)
		{
			std::vector<CString> vec;
			if(StringUtils::SplitString(pEntry->Text(),CString("\r\n"),vec) > 0)
			{
				for(int i=0 ;i<vec.size();i++)
				{
					pDC->TextOut(10,10+ i*(nHeight+3),vec[i]);
				}
			}
			else
			{
				pDC->TextOut(10,10,pEntry->Text());
			}
		}
	}
	pDC->SelectObject(POldFont);
}


//##ModelId=3C0DC1600370
BOOL CBaseFormView::CheckComplete() const
{
	ASSERT(FALSE); 
	return TRUE;
}



//##ModelId=3C0DC1600372
BOOL CBaseFormView::CheckEntry() const
{ 
	if(TreeEntry()->Previous())
	{
		//is previus a info form
		if(TreeEntry()->Previous()->FormView()->IsInfoForm())
		{
			if(TreeEntry()->Previous()->Previous())
			{
				return TreeEntry()->Previous()->Previous()->FormView()->CheckComplete();
			}
			else
			{
				//very first item;
				return TRUE;
			}
		}
		else
		{
			//only enter if previus is complete
			return TreeEntry()->Previous()->FormView()->CheckComplete();
		}
	}
	else
	{
		//very first item;
		return TRUE;
	}

}

//##ModelId=3C2044070177
void CBaseFormView::ShowQuantity(CDoubleEdit* pEdit,CDoubleQuantity* pQuantity)
{
	
	if(!pQuantity->Undefined())
	{
		//show the value quantity in user units.
		pEdit->ShowValue(pQuantity->Value(GetDoc()->UnitSystem()));
	}
	else
	{
		//undifined quantities are shown as a
		//blank edit box.
		pEdit->MakeEmpty();
	}

}

//##ModelId=3C0DC160037E
void CBaseFormView::ValidateQuantity(CDataExchange* pDX,CDoubleEdit* pDE,CDoubleQuantity *pQ,CString sQuantityName )
{
	int nIDC=pDE->GetDlgCtrlID();
	DDV_Quantity(pDX,nIDC,pQ,&GetModel()->DependenciesManager,sQuantityName);
}

//##ModelId=3C0DC160038E
void CBaseFormView::SetUnitlabel(int nID,CDoubleQuantity* pQ)
{
	CStatic *pUnitLabel=0;
	pUnitLabel = (CStatic*) GetDlgItem(nID);
	ASSERT(pUnitLabel!=0);
	::SetUnitlabel(pUnitLabel,pQ);
	
}



//##ModelId=3C0DC1610023
BOOL CBaseFormView::IsInfoForm() const
{
	return this->IsKindOf( RUNTIME_CLASS( CInfoForm ) ); 
}

//##ModelId=3C2044070271
void CBaseFormView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	CView::OnUpdate(pSender,lHint,pHint);

	if(lHint==UPDATE_UNITS || lHint==UPDATE_DOC_SAVED)
		UpdateData(FALSE);
}


//##ModelId=3C20440701F4
void CBaseFormView::EnableNextButton(BOOL enable)
{
	m_NextButton.EnableWindow(enable);
}

//##ModelId=3C20440701D5
void CBaseFormView::EnablePreviousButton(BOOL enable)
{
	m_PreviousButton.EnableWindow(enable);
}


//##ModelId=3C20440701C6
void CBaseFormView::UpdateButtons()
{
	if(TreeEntry())
	{
		if(TreeEntry()->Next())
		{
			EnableNextButton(TreeEntry()->Next()->Enable());
		}
		else
		{
			EnableNextButton(FALSE);
		}

		if(TreeEntry()->Previous())
		{
			EnablePreviousButton(TreeEntry()->Previous()->Enable());
		}
		else
		{
			EnablePreviousButton(FALSE);
		}
	}
}

//##ModelId=3C60D57F02DF
CFont *CBaseFormView::GetFont(BOOL bBold)
{
	if(bBold)
	{
		return &m_fontBold;
	}
	else
	{
		return &m_font;
	}
}

//##ModelId=3CA011AF034B
LRESULT CBaseFormView::OnValidateDoubleEdit(WPARAM id, LPARAM lparam)
{
   m_iControlToValidate = id;
   UpdateData();
   m_iControlToValidate = 0;
   return TRUE;
}

//##ModelId=3DDA0C5F01E8
BOOL CBaseFormView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	
	TRACE("CBaseFormView::OnCmdMsg\n");
	if(AfxGetApp()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) return TRUE;
	
	return CFormView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

//##ModelId=3DDA0C5F0226
BOOL CBaseFormView::PreTranslateMessage(MSG* pMsg) 
{
	
	if(GetDoc()->m_bShowToolTip)
	{
		if(m_toolTip.m_pParentWnd)
			m_toolTip.RelayEvent(pMsg);
	}		
		
	return CFormView::PreTranslateMessage(pMsg);
}
