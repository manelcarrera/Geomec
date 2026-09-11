// InfoForm.cpp : implementation file
//

#include "stdafx.h"
#include "dsealect.h"
#include "InfoForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CInfoForm

IMPLEMENT_DYNCREATE(CInfoForm, CBaseFormView)

//##ModelId=3C0DC15A0159
CInfoForm::CInfoForm()
	: CBaseFormView(CInfoForm::IDD)
{
	//{{AFX_DATA_INIT(CInfoForm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//##ModelId=3C0DC15A016E
CInfoForm::~CInfoForm()
{
}

//##ModelId=3C0DC15A0168
void CInfoForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInfoForm)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInfoForm, CBaseFormView)
	//{{AFX_MSG_MAP(CInfoForm)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInfoForm diagnostics

#ifdef _DEBUG
//##ModelId=3C0DC15A0177
void CInfoForm::AssertValid() const
{
	CFormView::AssertValid();
}

//##ModelId=3C0DC15A0179
void CInfoForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CInfoForm message handlers

//##ModelId=3C0DC15A016B
void CInfoForm::OnDraw(CDC* pDC) 
{
	CBaseFormView::OnDraw(pDC);
}

//##ModelId=3DDA0C520236
int CInfoForm::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBaseFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_HelpButton.ShowWindow(SW_HIDE);
	// TODO: Add your specialized creation code here
	
	return 0;
}
