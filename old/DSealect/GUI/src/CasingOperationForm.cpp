// CasingOperationForm.cpp : implementation file
//

#include "stdafx.h"
#include "dsealect.h"
#include "CasingOperationForm.h"
#include "CasingMaterial.h"
#include "CasingMaterialDlg.h"
#include "DoubleFormat.h"
#include "DSealectGlobal.h"
#include "MaterialDatabaseTemplate.h"
#include "CasingMaterialLibDlg.h"
#include "treeobject.h"
#include "CCasingOperation.h"
#include "CDataModel.h"
#include "CFormationTopography.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CCasingOperationForm

IMPLEMENT_DYNCREATE(CCasingOperationForm, CBaseFormView)

//##ModelId=3C0DC16001D4
CCasingOperationForm::CCasingOperationForm()
	: CBaseFormView(CCasingOperationForm::IDD)
{
	//{{AFX_DATA_INIT(CCasingOperationForm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//##ModelId=3C0DC1600206
CCasingOperationForm::~CCasingOperationForm()
{
	if(m_toolTip)
		delete m_toolTip;
}

//##ModelId=3C0DC1600203
void CCasingOperationForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCasingOperationForm)
	DDX_Control(pDX, IDC_ebPercentageEccentricity,	m_debPercentageEccentricity);
	DDX_Control(pDX, IDC_ebGuideShoeDepth,			m_debGuideShoeDepth);
	DDX_Control(pDX, IDC_lstCasingParams,	m_lstCasingMaterialParams);
	//}}AFX_DATA_MAP

	if(!CheckEntry())
		return;

	ValidateQuantity(pDX, &m_debPercentageEccentricity,	GetCasingOperation()->PercentageEccentricity(),	CString("PercentageEccentricity"));
	ValidateQuantity(pDX, &m_debGuideShoeDepth,			GetCasingOperation()->GuideShoeTMD(),			CString("GuideShoeDepth"));

	if (!pDX->m_bSaveAndValidate)
	{
		SetUnitlabel(IDC_lbUPercentageEccentricity, GetCasingOperation()->PercentageEccentricity());
		SetUnitlabel(IDC_lbUGuideShoeDepth, GetCasingOperation()->GuideShoeTMD());
	}
	
	CString MatName=GetModel()->DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->CasingMaterialName();

	if(GetModel()->DrillingScheme()->GetDrillingOperation(0)->CasingOperation())
	{
		GetDlgItem(IDC_btnEdit)->EnableWindow(!MatName.IsEmpty());
		GetDlgItem(IDC_btnToLibrary)->EnableWindow(!MatName.IsEmpty());
		GetDlgItem(IDC_btnFromLibrary)->EnableWindow(TRUE);
		GetDlgItem(IDC_btnNew)->EnableWindow(TRUE);
		if(!MatName.IsEmpty())
		{
			m_lstCasingMaterialParams.ShowMaterial(*GetModel()->CasingMatList()->GetMaterial(MatName));
			GetDlgItem(IDC_Warning)->ShowWindow(SW_HIDE);
		}
		else
		{
			m_lstCasingMaterialParams.DeleteAllItems();
			GetDlgItem(IDC_Warning)->ShowWindow(SW_SHOW);
		}
	}
	else
	{
		GetDlgItem(IDC_btnNew)->EnableWindow(FALSE);
		GetDlgItem(IDC_btnEdit)->EnableWindow(FALSE);
		GetDlgItem(IDC_btnToLibrary)->EnableWindow(FALSE);
		GetDlgItem(IDC_btnFromLibrary)->EnableWindow(FALSE);
	}

	GetModel()->RemoveUnusedMaterial();
	DrawView()->Invalidate();
	TreeEntry()->UpdateEnable();
    UpdateButtons();
}


BEGIN_MESSAGE_MAP(CCasingOperationForm, CBaseFormView)
	//{{AFX_MSG_MAP(CCasingOperationForm)
	ON_EN_KILLFOCUS(IDC_ebPercentageEccentricity,	OnKillfocusPercentageEccentricity)
	ON_EN_KILLFOCUS(IDC_ebGuideShoeDepth,			OnKillfocusGuideShoeDepth)
	ON_BN_CLICKED(IDC_btnNew, OnbtnNew)
	ON_BN_CLICKED(IDC_btnEdit, OnbtnEdit)
	ON_BN_CLICKED(IDC_btnFromLibrary, OnbtnFromLibrary)
	ON_BN_CLICKED(IDC_btnToLibrary, OnbtnToLibrary)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCasingOperationForm diagnostics

#ifdef _DEBUG
//##ModelId=3C0DC1600213
void CCasingOperationForm::AssertValid() const
{
	CFormView::AssertValid();
}

//##ModelId=3C0DC1600215
void CCasingOperationForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCasingOperationForm message handlers

//##ModelId=3C0DC1600222
void CCasingOperationForm::OnKillfocusPercentageEccentricity() 
{
	UpdateData();
	
}

//##ModelId=3C0DC1600224
void CCasingOperationForm::OnKillfocusGuideShoeDepth() 
{
	UpdateData();
	
}

//##ModelId=3C0DC16001B6
CCasingOperation* CCasingOperationForm::GetCasingOperation()
{
	return GetModel()->DrillingScheme()->GetDrillingOperation(0)->CasingOperation();
}

//##ModelId=3C0DC16001D5
BOOL CCasingOperationForm::CheckComplete() const
{

	if(!TreeEntry()->Previous()->FormView()->CheckComplete())
	{
		return FALSE;
	}

	if(GetModel()->FormationTopography()->NrOfFormations()<1)
	{
		return FALSE;
	}
	
	if(GetModel()->DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->PercentageEccentricity()->Undefined())
		return FALSE;

	if(GetModel()->DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->GuideShoeTMD()->Undefined())
		return FALSE;

  int nSections = GetModel()->DrillingScheme()->NrOfDrillingDirections();
  CDrillingDirectionSection* pLastSection = GetModel()->DrillingScheme()->GetDrillingDirectionSection(nSections - 1);
  double dBottomTMD = pLastSection->DrillingSectionBottomTMD()->Value();
	if(GetModel()->DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->GuideShoeTMD()->Value() > dBottomTMD)
    return FALSE;

	if(GetModel()->DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->CasingMaterialName().IsEmpty())
		return FALSE;

	return TRUE;

}

//##ModelId=3C60D57E00EB
void CCasingOperationForm::OnInitialUpdate() 
{
	CBaseFormView::OnInitialUpdate();
	
	GetDlgItem(IDC_fraCasingPlacement)->SetFont(GetFont(TRUE));

	// TOOLTIP
	static short bInit=0;
	if(!bInit)
	{
		//One time initialization!!
		bInit=1;
		
		m_toolTip.Create(this);
		CRect rect;
		CString sTmp;

		sTmp.LoadString(IDTT_CAO_ebGuideShoeDepth);
		m_toolTip.AddTool( GetDlgItem(IDC_ebGuideShoeDepth), sTmp);
		GetDlgItem(IDC_ebGuideShoeDepth)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_CAO_ebPercentageEccentricity);
		m_toolTip.AddTool( GetDlgItem(IDC_ebPercentageEccentricity), sTmp);
		GetDlgItem(IDC_ebPercentageEccentricity)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_CAO_lstCasingParams);
		m_toolTip.AddTool( GetDlgItem(IDC_lstCasingParams), sTmp);
		GetDlgItem(IDC_lstCasingParams)->GetWindowRect(&rect);	
		ScreenToClient(rect);
		
		//IDC_ebGuideShoeDepth
		//IDC_ebPercentageEccentricity
		//IDC_lstCasingParams

		//IDTT_CAO_ebGuideShoeDepth
		//IDTT_CAO_ebPercentageEccentricity
		//IDTT_CAO_lstCasingParams

		CRect rectMargin(TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE); //left, top, right, bottom
		m_toolTip.SetMargin(rectMargin);
		m_toolTip.Activate(TRUE);
	}
	
	
}

//##ModelId=3C60D57E00FA
void CCasingOperationForm::OnbtnNew() 
{
	//ASSERT(m_pCurrentFormation);
	CDoubleQuantity* pQ = GetModel()->GetBoreholeDep()->GetCasingOuter();

	CCasingMaterial cam;

	CCasingMaterialDlg dlg(cam,GetModel(),TRUE);
	if(dlg.DoModal()==IDOK)
	{
		cam=dlg.Material();
		if(GetModel()->CasingMatList()->Exists(cam.Name()))
		{
			if(MessageBox("Material already exist in the model, replace?",NULL,MB_YESNO) == IDNO)
			{
				GetModel()->GetBoreholeDep()->SetCasingOuter(pQ);
				return;
			}
			else
			{
				GetModel()->CasingMatList()->Remove(cam.Name());
			}
		}

		GetModel()->GetBoreholeDep()->SetCasingOuter(GetModel()->CasingMatList()->GetMaterial(cam.Name())->CasingOuterDiameter());
		GetModel()->DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->CasingMaterialName(cam.Name());
		GetModel()->CasingMatList()->Add(cam);
		GetDocument()->SetModifiedFlag(TRUE);
		UpdateData();
	}
	
}

//##ModelId=3C60D57E010A
void CCasingOperationForm::OnbtnEdit() 
{
	//ASSERT(m_pCurrentFormation);
	CDoubleQuantity* pQ = GetModel()->GetBoreholeDep()->GetCasingOuter();

	CCasingMaterialDlg dlg(*GetModel()->CasingMatList()->GetMaterial(GetModel()->DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->CasingMaterialName()),GetModel(),TRUE);

	if(dlg.DoModal()==IDOK)
	{
		CCasingMaterial cam;
		cam=dlg.Material();
		if(GetModel()->CasingMatList()->Exists(cam.Name()))
		{
			if(MessageBox("Material already exist in the model, replace?",NULL,MB_YESNO) == IDNO)
			{
				GetModel()->GetBoreholeDep()->SetCasingOuter(pQ);
				return;
			}
			else
			{
				GetModel()->CasingMatList()->Remove(cam.Name());
			}
		}
		GetModel()->CasingMatList()->Add(cam);
		GetModel()->DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->CasingMaterialName(cam.Name());
		GetModel()->GetBoreholeDep()->SetCasingOuter(GetModel()->CasingMatList()->GetMaterial(cam.Name())->CasingOuterDiameter());
		GetDocument()->SetModifiedFlag(TRUE);
		UpdateData();
	}
	
}

//##ModelId=3C60D57E010C
void CCasingOperationForm::OnbtnFromLibrary() 
{

	CCasingMaterialDatabase db;
	db.Database().Open(GetDoc()->CasingMatDatabasePath(),TRUE,FALSE);
	if(!db.HasMaterials())
	{
		CString msg;
		msg.LoadString(IDS_MATLIB_NO_MATERIAL);
		db.Database().Close();
		MessageBox(msg);
		return;
	}
	db.Database().Close();

	//ASSERT(m_pCurrentFormation);
	CCasingMaterialLibDlg casld(GetDoc()->CasingMatDatabasePath(),GetModel());

	if(casld.DoModal()==IDOK)
	{
		if(casld.SelectedMaterial())
		{
			CCasingMaterial cas=*casld.SelectedMaterial();
			CDoubleQuantity* pQ = GetModel()->GetBoreholeDep()->GetCasingOuter();
			GetModel()->GetBoreholeDep()->SetCasingOuter(cas.CasingOuterDiameter());
			std::string strtemp;
			if(!GetModel()->DependenciesManager.CheckQuantity(cas.CasingOuterDiameter(),strtemp))
			{
				GetModel()->GetBoreholeDep()->SetCasingOuter(pQ);
				MessageBox(strtemp.c_str());
				return;
			}


			if(GetModel()->CasingMatList()->Exists(cas.Name()))
			{
				if(MessageBox("Material already exist in the model, replace?",NULL,MB_YESNO) == IDNO)
				{
					GetModel()->GetBoreholeDep()->SetCasingOuter(pQ);
					return;
				}
				else
				{
					GetModel()->CasingMatList()->Remove(cas.Name());
				}
			}
			
			GetModel()->CasingMatList()->Add(cas);
			GetModel()->DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->CasingMaterialName(cas.Name());
			GetModel()->GetBoreholeDep()->SetCasingOuter(GetModel()->CasingMatList()->GetMaterial(cas.Name())->CasingOuterDiameter());
			GetDocument()->SetModifiedFlag(TRUE);
			MessageBox("Material imported from database"); 
			UpdateData();
		}
	}
	
}

//##ModelId=3C60D57E010E
void CCasingOperationForm::OnbtnToLibrary() 
{

//	ASSERT(m_pCurrentFormation);
	ASSERT(!GetModel()->DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->CasingMaterialName().IsEmpty());


	CCasingMaterialDatabase db;
	db.Database().Open(GetDoc()->CasingMatDatabasePath(),TRUE,FALSE);
	
	ASSERT(GetModel()->CasingMatList()->Exists(GetModel()->DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->CasingMaterialName()));

	CCasingMaterial *cas=GetModel()->CasingMatList()->GetMaterial(GetModel()->DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->CasingMaterialName());

	if(db.MaterialExists(cas->Name()))
	{
		if(MessageBox("Material already exists in the database, replace?",NULL,MB_YESNO)==IDNO)
		{
			db.Database().Close();
			return;
		}
		db.RemoveMaterial(cas->Name());
	}

	db.AddMaterial(*cas);
	db.Database().Close();

	MessageBox("Material added to database"); 
	
}


