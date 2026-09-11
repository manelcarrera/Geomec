// FormationPropertiesForm.cpp : implementation file
//

#include "stdafx.h"
#include "dsealect.h"
#include "treeobject.h"
#include "FormationPropertiesForm.h"
#include "FormationMaterial.h"
#include "FormationMaterialDlg.h"
#include "DoubleFormat.h"
#include "DSealectGlobal.h"
#include "MaterialDatabaseTemplate.h"
#include "FormationMatrialLibDlg.h"
#include "CFormation.h"
#include "CDataModel.h"
#include "CFormationTopography.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CFormationPropertiesForm

IMPLEMENT_DYNCREATE(CFormationPropertiesForm, CBaseFormView)

//##ModelId=3C0DC15A01B8
CFormationPropertiesForm::CFormationPropertiesForm()
	: CBaseFormView(CFormationPropertiesForm::IDD)
{
	//{{AFX_DATA_INIT(CFormationPropertiesForm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//##ModelId=3C0DC15A01D5
CFormationPropertiesForm::~CFormationPropertiesForm()
{
}

//##ModelId=3C0DC15A01C9
void CFormationPropertiesForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFormationPropertiesForm)
	DDX_Control(pDX, IDC_lstFormationMaterialParams, m_lstFormationMateriaParams);
	DDX_Control(pDX, IDC_lstFormationMaterial, m_lstFormationMaterial);
	//}}AFX_DATA_MAP

	if(!CheckEntry())
		return;

	BuildFormationMaterialList();

	if(m_pCurrentFormation)
	{
		GetDlgItem(IDC_btnEdit)->EnableWindow(!m_pCurrentFormation->MaterialName().IsEmpty());
		GetDlgItem(IDC_btnToLibrary)->EnableWindow(!m_pCurrentFormation->MaterialName().IsEmpty());
		GetDlgItem(IDC_btnFromLibrary)->EnableWindow(TRUE);
		GetDlgItem(IDC_btnNew)->EnableWindow(TRUE);
		if(!m_pCurrentFormation->MaterialName().IsEmpty())
		{
			m_lstFormationMateriaParams.ShowMaterial(*GetModel()->FormationMatList()->GetMaterial(m_pCurrentFormation->MaterialName()));
			long Index = GetDoc()->DSealectModel()->FormationTopography()->GetFormationIndex(m_pCurrentFormation);
			m_lstFormationMaterial.SetItemText(Index,1,m_pCurrentFormation->MaterialName());
			GetDlgItem(IDC_Warning)->ShowWindow(SW_HIDE);
		}
		else
		{
			m_lstFormationMateriaParams.DeleteAllItems();
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


BEGIN_MESSAGE_MAP(CFormationPropertiesForm, CBaseFormView)
	//{{AFX_MSG_MAP(CFormationPropertiesForm)
	ON_BN_CLICKED(IDC_btnFromLibrary, OnbtnFromLibrary)
	ON_NOTIFY(NM_CLICK, IDC_lstFormationMaterial, OnClicklstFormationMaterial)
	ON_NOTIFY(LVN_KEYDOWN, IDC_lstFormationMaterial, OnKeydownlstFormationMaterial)
	ON_BN_CLICKED(IDC_btnEdit, OnEdit)
	ON_BN_CLICKED(IDC_btnNew, OnNew)
	ON_BN_CLICKED(IDC_btnToLibrary, OnToLibrary)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFormationPropertiesForm diagnostics

#ifdef _DEBUG
//##ModelId=3C0DC15A01E4
void CFormationPropertiesForm::AssertValid() const
{
	CFormView::AssertValid();
}

//##ModelId=3C0DC15A01E6
void CFormationPropertiesForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFormationPropertiesForm message handlers

//##ModelId=3C0DC15A01C7
void CFormationPropertiesForm::OnInitialUpdate() 
{
	CBaseFormView::OnInitialUpdate();
	
	static short bInit=0;
	if(!bInit)
	{
		//One time initialization!!
		bInit=1;

		//Create Columns
		LV_COLUMN lvColumn;
		lvColumn.mask = LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
		lvColumn.fmt= LVCFMT_LEFT;
		lvColumn.cx=68;
		
		CListCtrl* pList=(CListCtrl*)GetDlgItem(IDC_lstFormationMaterial);

		ASSERT(pList);

		pList->SetExtendedStyle(LVS_EX_FULLROWSELECT);

		lvColumn.iSubItem=0;
		lvColumn.pszText="Formation";
		pList->InsertColumn(0, &lvColumn);
		lvColumn.iSubItem=1;
		lvColumn.pszText="Material";
		pList->InsertColumn(1, &lvColumn);
		lvColumn.iSubItem=2;
		lvColumn.pszText="Top ";
		pList->InsertColumn(2, &lvColumn);
		lvColumn.iSubItem=3;
		lvColumn.pszText="Bottom ";
		pList->InsertColumn(3, &lvColumn);
	}
		

}


//##ModelId=3C2043FF0281
void CFormationPropertiesForm::BuildFormationMaterialList()
{
	//get the current unit system:
	CDSealectDoc* pDoc=(CDSealectDoc*)GetDocument();
	QU::UNIT us = pDoc->UnitSystem();
		
	CListCtrl* pList=(CListCtrl*)GetDlgItem(IDC_lstFormationMaterial);

	ASSERT(pList);
	pList->DeleteAllItems();
		
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_TEXT;
	
	CLengthQuantity temp; //temporary Quantity, to get Length-unit-string
	
	CString strUnit("Top ");
	strUnit+= GetUnitString(&temp);
	lvColumn.pszText = (LPSTR)(LPCSTR)strUnit;
	pList->SetColumn(2, &lvColumn); //set header text 3rd column  
		
	strUnit=("Bottom ");
	strUnit+= GetUnitString(&temp);
	lvColumn.pszText=strUnit.GetBufferSetLength(strUnit.GetLength());
	pList->SetColumn(3, &lvColumn); //set header text 4th column  

	if(GetModel()->FormationTopography()->NrOfFormations()<1)
	{
		m_pCurrentFormation=0;
		return;
	}

	//Create items
	LV_ITEM lvItem;
	
	lvItem.mask= LVIF_TEXT|LVIF_STATE;
	lvItem.state=0;
	lvItem.stateMask=0;
	
	CString sTmp;
	lvItem.iItem=0;
	lvItem.iSubItem=0;
				
	for(int i =0 ;i<GetModel()->FormationTopography()->NrOfFormations();i++)
	{
		CString sTmp;
		lvItem.iItem=(i);
		
		lvItem.iSubItem=0;
		sTmp=GetModel()->FormationTopography()->GetFormationAt(i)->FormationName();
		lvItem.pszText= sTmp.GetBufferSetLength(sTmp.GetLength());
		pList->InsertItem(&lvItem); //Formation Name

		sTmp=GetModel()->FormationTopography()->GetFormationAt(i)->MaterialName();
		pList->SetItemText(i,1,sTmp.GetBufferSetLength(sTmp.GetLength())); //Material Name
	
		if(i==0) 
		{
			//set overburden-bottom-TVD as Top-TVD for 1st formation
			sTmp=CDoubleEdit::Format(GetModel()->FormationTopography()->Overburden()->OverburdenBottomTVD()->Value(us));
			pList->SetItemText(0,2,sTmp.GetBufferSetLength(sTmp.GetLength()));
		}
		else
		{
			//(Formation topTVD) in col3 is equal to (previous formation bottomTVD) in col4:
			pList->SetItemText(i,2,m_lstFormationMaterial.GetItemText((i-1),3));
		}
				
		sTmp=CDoubleEdit::Format(GetModel()->FormationTopography()->GetFormationAt(i)->BottomTVD()->Value(us));
		pList->SetItemText(i,3,sTmp.GetBufferSetLength(sTmp.GetLength()));
		
	}
	
	if(m_pCurrentFormation)
	{	
		//select current formation:
		long Index=GetModel()->FormationTopography()->GetFormationIndex(m_pCurrentFormation);
		if(Index<0)
		{
			m_pCurrentFormation=GetModel()->FormationTopography()->GetFormationAt(0);
			Index=0;
		}
		pList->SetItem(Index, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);
		pList->EnsureVisible(Index,true);
	}
	else
	{
		//if there is no CurrentFormation the first formation is selected(ListItem 0)
		pList->SetItem(0, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);
		m_pCurrentFormation=GetModel()->FormationTopography()->GetFormationAt(0);
		pList->EnsureVisible(0,true);
	}
}


//##ModelId=3C2043FF0177
void CFormationPropertiesForm::OnClicklstFormationMaterial(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	if (pNMListView->iItem<0)
	{
		m_pCurrentFormation	=0; //first formation as default
		UpdateData(FALSE);
		return ;
	}
	else
	{
		m_pCurrentFormation=GetModel()->FormationTopography()->GetFormationAt(pNMListView->iItem);
	}

	UpdateData(FALSE);	
	*pResult = 0;
}

//##ModelId=3C2043FF01D5
void CFormationPropertiesForm::OnKeydownlstFormationMaterial(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
		
	// set from 0 to 1 to lock the use of arrow-keys 	
	*pResult = 1;
}

//##ModelId=3C60D575005E
void CFormationPropertiesForm::OnEdit() 
{
	ASSERT(m_pCurrentFormation);

	CFormationMaterialDlg dlg(*GetModel()->FormationMatList()->GetMaterial(m_pCurrentFormation->MaterialName()));

	if(dlg.DoModal()==IDOK)
	{
		CFormationMaterial fm;
		fm=dlg.Material();
		if(GetModel()->FormationMatList()->Exists(fm.Name()))
		{
			if(MessageBox("Material already exist in the model, replace?",NULL,MB_YESNO) == IDNO)
				return;
			else
			{
				GetModel()->FormationMatList()->Remove(fm.Name());
			}
		}
		m_pCurrentFormation->MaterialName(fm.Name());
		GetDocument()->SetModifiedFlag(TRUE);
		GetModel()->FormationMatList()->Add(fm);
		UpdateData();
	}
}



//##ModelId=3C60D575006F
void CFormationPropertiesForm::OnNew() 
{
	ASSERT(m_pCurrentFormation);
	CFormationMaterial fm;

	CFormationMaterialDlg dlg(fm);
	if(dlg.DoModal()==IDOK)
	{
		fm=dlg.Material();
		if(GetModel()->FormationMatList()->Exists(fm.Name()))
		{
			if(MessageBox("Material already exist in the model, replace?",NULL,MB_YESNO) == IDNO)
				return;
			else
			{
				GetModel()->FormationMatList()->Remove(fm.Name());
			}
		}
		m_pCurrentFormation->MaterialName(fm.Name());
		GetDocument()->SetModifiedFlag(TRUE);
		GetModel()->FormationMatList()->Add(fm);
		UpdateData();
	}
}

//##ModelId=3C60D5750071
void CFormationPropertiesForm::OnToLibrary() 
{
	ASSERT(m_pCurrentFormation);
	ASSERT(!m_pCurrentFormation->MaterialName().IsEmpty());

	CFormationMaterialDatabase db;
	db.Database().Open(GetDoc()->FormationMatDatabasePath(),TRUE,FALSE);
	
	ASSERT(GetModel()->FormationMatList()->Exists(m_pCurrentFormation->MaterialName()));

	CFormationMaterial *fm=GetModel()->FormationMatList()->GetMaterial(m_pCurrentFormation->MaterialName());

	if(db.MaterialExists(fm->Name()))
	{
		if(MessageBox("Material already exists in the database, replace?",NULL,MB_YESNO)==IDNO)
		{
			db.Database().Close();
			return;
		}
		db.RemoveMaterial(fm->Name());
	}

	db.AddMaterial(*fm);
	db.Database().Close();

	MessageBox("Material added to database"); 
}


//##ModelId=3C2043FF0149
void CFormationPropertiesForm::OnbtnFromLibrary() 
{
	CFormationMaterialDatabase db;
	db.Database().Open(GetDoc()->FormationMatDatabasePath(),TRUE,FALSE);
	if(!db.HasMaterials())
	{
		CString msg;
		msg.LoadString(IDS_MATLIB_NO_MATERIAL);
		db.Database().Close();
		MessageBox(msg);
		return;
	}
	db.Database().Close();

	ASSERT(m_pCurrentFormation);
	CFormationMatrialLibDlg fmld(GetDoc()->FormationMatDatabasePath());


	if(fmld.DoModal()==IDOK)
	{
		if(fmld.SelectedMaterial())
		{
			CFormationMaterial fm=*fmld.SelectedMaterial();
			if(GetModel()->FormationMatList()->Exists(fm.Name()))
			{
				if(MessageBox("Material already exist in the model, replace?",NULL,MB_YESNO) == IDNO)
					return;
				else
				{
					GetModel()->FormationMatList()->Remove(fm.Name());
				}
			}
			
			GetModel()->FormationMatList()->Add(fm);
			m_pCurrentFormation->MaterialName(fm.Name());
			GetDocument()->SetModifiedFlag(TRUE);
			MessageBox("Material imported from database"); 
			UpdateData();
		}
	}
}

//##ModelId=3C0DC15A01B9
BOOL CFormationPropertiesForm::CheckComplete() const
{
	if(!TreeEntry()->Previous()->FormView()->CheckComplete())
	{
		return FALSE;
	}

	if(GetModel()->FormationTopography()->NrOfFormations()<1)
	{
		return FALSE;
	}

	CString sTmp;
	if(GetModel()->FormationTopography()->NrOfFormations()<1)
		return FALSE;

	for(int i =0 ;i<GetModel()->FormationTopography()->NrOfFormations();i++)
	{
		sTmp=GetModel()->FormationTopography()->GetFormationAt(i)->MaterialName();
		if(sTmp.IsEmpty())
		{
			return FALSE;
		}

    CFormationMaterial* pMat = GetModel()->FormationMatList()->GetMaterial(sTmp);
    if(!pMat || !pMat->CheckComplete())
      return FALSE;

    // must have either defined densities in formations or a valid density profile
    if(pMat->Density()->Undefined() && GetModel()->DensityProfile().empty())
      return FALSE;
	}
	
	return TRUE;
}

