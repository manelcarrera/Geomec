// MeshRefinementForm.cpp : implementation file
//

#include "stdafx.h"
#include "dsealect.h"
#include "treeobject.h"
#include "MeshRefinementForm.h"
#include "CDataModel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CMeshRefinementForm

IMPLEMENT_DYNCREATE(CMeshRefinementForm, CBaseFormView)

//##ModelId=3C0DC15803CD
CMeshRefinementForm::CMeshRefinementForm()
	: CBaseFormView(CMeshRefinementForm::IDD)
{
	//{{AFX_DATA_INIT(CMeshRefinementForm)
	m_iCasingRefLevel = 3;
	m_iCementRefLevel = 5;
	m_iRockRefLevel = 2;
	//}}AFX_DATA_INIT
}

//##ModelId=3C0DC159001F
CMeshRefinementForm::~CMeshRefinementForm()
{
}

//##ModelId=3C0DC1590011
void CMeshRefinementForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);

	if (!pDX->m_bSaveAndValidate)
	{
		//update gui
		CSliderCtrl* pSlider=(CSliderCtrl*)GetDlgItem(IDC_sldRockRefinementLevel);
		m_iRockRefLevel=GetModel()->Control()->RockRefinement();
		pSlider->SetPos(m_iRockRefLevel);
						
		CSliderCtrl* pSlider1=(CSliderCtrl*)GetDlgItem(IDC_sldCasingRefinementLevel);
		m_iCasingRefLevel=GetModel()->Control()->CasingRefinement();
		pSlider1->SetPos(m_iCasingRefLevel);
				
		CSliderCtrl* pSlider2=(CSliderCtrl*)GetDlgItem(IDC_sldCementRefinementLevel);
		m_iCementRefLevel=GetModel()->Control()->CementRefinement();
		pSlider2->SetPos(m_iCementRefLevel);


		if(GetModel()->Control()->DefaultMeshRefinement())
			m_intDefaultMeshRefinement = 0;
		else
			m_intDefaultMeshRefinement = 1;
	}
	
	//{{AFX_DATA_MAP(CMeshRefinementForm)
	DDX_Radio(pDX, IDC_optDefaultMeshRefinement,	m_intDefaultMeshRefinement);
	DDX_Control(pDX, IDC_sldRockRefinementLevel, m_sldRockRefinement);
	DDX_Control(pDX, IDC_sldCementRefinementLevel, m_sldCementRefinement);
	DDX_Control(pDX, IDC_sldCasingRefinementLevel, m_sldCasingRefinement);
	DDX_Text(pDX, IDC_ebCasingRefinementLevel, m_iCasingRefLevel);
	DDX_Text(pDX, IDC_ebCementRefinementLevel, m_iCementRefLevel);
	DDX_Text(pDX, IDC_ebRockRefinementLevel, m_iRockRefLevel);
	//}}AFX_DATA_MAP
	
	if(!CheckEntry())
		return;


	if(pDX->m_bSaveAndValidate)
	{
		//update data
		//if default then data is set internal
		GetModel()->Control()->DefaultMeshRefinement(m_intDefaultMeshRefinement==0?true:false);

		if(GetModel()->Control()->DefaultMeshRefinement())
		{
			m_iCasingRefLevel=GetModel()->Control()->CasingRefinement();
			m_iCementRefLevel=GetModel()->Control()->CementRefinement();
			m_iRockRefLevel=GetModel()->Control()->RockRefinement();
		}
		else
		{
			GetModel()->Control()->CasingRefinement(m_iCasingRefLevel);
			GetModel()->Control()->CementRefinement(m_iCementRefLevel);
			GetModel()->Control()->RockRefinement(m_iRockRefLevel);
		}
	}


	m_optAdvancedMeshRefinement.Enable(m_intDefaultMeshRefinement==1?TRUE:FALSE);	//dis-/enable default-mesh-refinement frame

	
	DrawView()->Invalidate();
	TreeEntry()->UpdateEnable();
	UpdateButtons();
}


BEGIN_MESSAGE_MAP(CMeshRefinementForm, CBaseFormView)
	//{{AFX_MSG_MAP(CMeshRefinementForm)
	ON_BN_CLICKED(IDC_optAdvancedTuningOfMeshRefinement, OnoptAdvancedTuningOfMeshRefinement)
	ON_BN_CLICKED(IDC_optDefaultMeshRefinement, OnoptDefaultMeshRefinement)
	ON_WM_HSCROLL()
	ON_EN_SETFOCUS(IDC_ebCasingRefinementLevel, OnSetfocusebCasingRefinementLevel)
	ON_WM_CTLCOLOR()
	ON_EN_SETFOCUS(IDC_ebCementRefinementLevel, OnSetfocusebCementRefinementLevel)
	ON_EN_SETFOCUS(IDC_ebRockRefinementLevel, OnSetfocusebRockRefinementLevel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMeshRefinementForm diagnostics

#ifdef _DEBUG
//##ModelId=3C0DC1590021
void CMeshRefinementForm::AssertValid() const
{
	CFormView::AssertValid();
}

//##ModelId=3C0DC1590023
void CMeshRefinementForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMeshRefinementForm message handlers

//##ModelId=3C0DC159000F
void CMeshRefinementForm::OnInitialUpdate() 
{
	m_optAdvancedMeshRefinement.Set(this, IDC_fraAdvancedTuningOfMeshRefinement);
	CBaseFormView::OnInitialUpdate();
	
	((CSliderCtrl*)GetDlgItem(IDC_sldRockRefinementLevel))->SetPos(m_iRockRefLevel);
	((CSliderCtrl*)GetDlgItem(IDC_sldRockRefinementLevel))->SetRangeMin(1,FALSE);
	((CSliderCtrl*)GetDlgItem(IDC_sldRockRefinementLevel))->SetRangeMax(3,TRUE);
	
	((CSliderCtrl*)GetDlgItem(IDC_sldCasingRefinementLevel))->SetPos(m_iCasingRefLevel);
	((CSliderCtrl*)GetDlgItem(IDC_sldCasingRefinementLevel))->SetRangeMin(2,FALSE);
	((CSliderCtrl*)GetDlgItem(IDC_sldCasingRefinementLevel))->SetRangeMax(10,TRUE);
	

	((CSliderCtrl*)GetDlgItem(IDC_sldCementRefinementLevel))->SetPos(m_iCementRefLevel);
	((CSliderCtrl*)GetDlgItem(IDC_sldCementRefinementLevel))->SetRangeMin(3,FALSE);
	((CSliderCtrl*)GetDlgItem(IDC_sldCementRefinementLevel))->SetRangeMax(10,TRUE);
	
	GetDlgItem(IDC_optDefaultMeshRefinement)->SetFont(GetFont(TRUE));
	GetDlgItem(IDC_optAdvancedTuningOfMeshRefinement)->SetFont(GetFont(TRUE));
	
}

//##ModelId=3C0DC159002F
void CMeshRefinementForm::OnoptAdvancedTuningOfMeshRefinement() 
{
	UpdateData();
	UpdateData(FALSE);
	GetDocument()->SetModifiedFlag(TRUE);
}

//##ModelId=3C0DC1590031
void CMeshRefinementForm::OnoptDefaultMeshRefinement() 
{
	UpdateData();
	UpdateData(FALSE);
	GetDocument()->SetModifiedFlag(TRUE);

}


//##ModelId=3C60D5730020
void CMeshRefinementForm::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CSliderCtrl* pSlide=(CSliderCtrl*)pScrollBar;

	CString tmp;
	
	switch(pScrollBar->GetDlgCtrlID())
	{
		
		case IDC_sldRockRefinementLevel:
			tmp.Format("%i",pSlide->GetPos());
			GetDlgItem(IDC_ebRockRefinementLevel)->SetWindowText(tmp);
			break;

		case IDC_sldCasingRefinementLevel:
			tmp.Format("%i",pSlide->GetPos());
			GetDlgItem(IDC_ebCasingRefinementLevel)->SetWindowText(tmp);
			break;

		case IDC_sldCementRefinementLevel:
			tmp.Format("%i",pSlide->GetPos());
			GetDlgItem(IDC_ebCementRefinementLevel)->SetWindowText(tmp);
			//curCempos = pScrollBar->GetScrollPos();
			//m_iCementRefLevel=curCempos;
			break;
	}

	UpdateData();

	CBaseFormView::OnHScroll(nSBCode, nPos, pScrollBar);
	GetDocument()->SetModifiedFlag(TRUE);
}

//##ModelId=3C0DC15803DD
BOOL CMeshRefinementForm::CheckComplete() const
{
	if(!TreeEntry()->Previous()->Previous()->FormView()->CheckComplete())
	{
		return FALSE;
	}

	return TRUE;
}



//##ModelId=3CA0119F0290
void CMeshRefinementForm::OnSetfocusebCasingRefinementLevel() 
{
	GetDlgItem(IDC_sldCasingRefinementLevel)->SetFocus();
	
}

//##ModelId=3CA0119F02BF
void CMeshRefinementForm::OnSetfocusebCementRefinementLevel() 
{
	GetDlgItem(IDC_sldCementRefinementLevel)->SetFocus();
	
}

//##ModelId=3CA0119F02CE
void CMeshRefinementForm::OnSetfocusebRockRefinementLevel() 
{
	GetDlgItem(IDC_sldRockRefinementLevel)->SetFocus();
	
}
