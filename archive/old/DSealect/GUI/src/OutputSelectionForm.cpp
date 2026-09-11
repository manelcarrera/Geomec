// OutputSelectionForm.cpp : implementation file
//

#include "stdafx.h"
#include "dsealect.h"
#include "OutputSelectionForm.h"
#include "treeobject.h"
#include "DSealectGlobal.h"
#include "CDataModel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputSelectionForm

IMPLEMENT_DYNCREATE(COutputSelectionForm, CBaseFormView)

//##ModelId=3C0DC15801E9
COutputSelectionForm::COutputSelectionForm()
	: CBaseFormView(COutputSelectionForm::IDD)
{
	//{{AFX_DATA_INIT(COutputSelectionForm)
	m_iOverviewLevel = 1;
	//}}AFX_DATA_INIT
}

//##ModelId=3C0DC1580203
COutputSelectionForm::~COutputSelectionForm()
{
}

//##ModelId=3C0DC15801F7
void COutputSelectionForm::DoDataExchange(CDataExchange* pDX)
{
	if(!pDX->m_bSaveAndValidate)
	{
		//update gui
		m_iOverviewLevel=GetDoc()->DSealectModel()->Control()->OutputLevel();	
	}

	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COutputSelectionForm)
	DDX_Radio(pDX, IDC_optOverview1, m_iOverviewLevel);
	//}}AFX_DATA_MAP

	if(!CheckEntry())
		return;

	if (pDX->m_bSaveAndValidate)
	{
		//update data
		GetModel()->Control()->OutputLevel(m_iOverviewLevel);
	}

	DrawView()->Invalidate();
	TreeEntry()->UpdateEnable();
    UpdateButtons();

}


BEGIN_MESSAGE_MAP(COutputSelectionForm, CBaseFormView)
	//{{AFX_MSG_MAP(COutputSelectionForm)
	ON_BN_CLICKED(IDC_optOverview1, OnoptOverview1)
	ON_BN_CLICKED(IDC_optOverview2, OnoptOverview2)
	ON_BN_CLICKED(IDC_optOverview3, OnoptOverview3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutputSelectionForm diagnostics

#ifdef _DEBUG
//##ModelId=3C0DC1580205
void COutputSelectionForm::AssertValid() const
{
	CFormView::AssertValid();
}

//##ModelId=3C0DC1580207
void COutputSelectionForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COutputSelectionForm message handlers

//##ModelId=3C60D57201E7
void COutputSelectionForm::OnoptOverview1() 
{
	UpdateData();	
	UpdateData(FALSE);
	GetDocument()->SetModifiedFlag(TRUE);
}

//##ModelId=3C60D57201F5
void COutputSelectionForm::OnoptOverview2() 
{
	UpdateData();	
	UpdateData(FALSE);
	GetDocument()->SetModifiedFlag(TRUE);
}

//##ModelId=3C60D57201F7
void COutputSelectionForm::OnoptOverview3() 
{
	UpdateData();	
	UpdateData(FALSE);
	GetDocument()->SetModifiedFlag(TRUE);
}

//##ModelId=3C60D57201E5
void COutputSelectionForm::OnInitialUpdate() 
{
	m_iOverviewLevel = 1;
	CBaseFormView::OnInitialUpdate();
	GetDlgItem(IDC_fraSelectOutputLevel)->SetFont(GetFont(TRUE));	
}

//##ModelId=3C0DC15801EA
BOOL COutputSelectionForm::CheckComplete() const
{
	if(!TreeEntry()->Previous()->FormView()->CheckComplete())
	{
		return FALSE;
	}

	return TRUE;	
}
