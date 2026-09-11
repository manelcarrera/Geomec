// FormationMatrialLibDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dsealect.h"
#include "Global.h"
#include "FormationMaterialDlg.h"
#include "FormationMatrialLibDlg.h"
#include "DoubleEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CFormationMatrialLibDlg dialog


//##ModelId=3C60D57501A6
CFormationMatrialLibDlg::CFormationMatrialLibDlg(CString sDatabasePath,CWnd* pParent /*=NULL*/ )
	: CDialog(CFormationMatrialLibDlg::IDD, pParent)
{

	m_strDatabasePath   = sDatabasePath;
	m_pModelDatabase	= std::auto_ptr<CFormationMaterialDatabase>(new CFormationMaterialDatabase);
	m_pModelDatabase->Database().Open(m_strDatabasePath,TRUE,FALSE,NULL);
	m_pSelectedMaterial =std::auto_ptr<CFormationMaterial>(new CFormationMaterial());

	//{{AFX_DATA_INIT(CFormationMatrialLibDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


//##ModelId=3C60D57501F4
void CFormationMatrialLibDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFormationMatrialLibDlg)
	DDX_Control(pDX, IDC_lstFormationLibraryParams, m_lstMaterialParams);
	DDX_Control(pDX, IDC_lstFormationLibrary, m_lstMaterialNames);
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CFormationMatrialLibDlg, CDialog)
	//{{AFX_MSG_MAP(CFormationMatrialLibDlg)
	ON_NOTIFY(NM_CLICK, IDC_lstFormationLibrary, OnClicklstFormationLibrary)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_lstFormationLibrary, OnItemchangedlstFormationLibrary)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_lstFormationLibrary, OnItemchanginglstFormationLibrary)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_lstFormationLibraryParams, OnItemchanginglstFormationLibraryParams)
	ON_BN_CLICKED(IDC_btnDelete, OnbtnDelete)
	ON_BN_CLICKED(IDC_btnEdit, OnbtnEdit)
	ON_BN_CLICKED(IDC_btnNew, OnbtnNew)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFormationMatrialLibDlg message handlers

//##ModelId=3C60D5750204
BOOL CFormationMatrialLibDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetWindowText(m_strDatabasePath);
	//Create Columns
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
	lvColumn.fmt= LVCFMT_LEFT;
	lvColumn.cx=90;
	CListCtrl* pList=0;

	pList=(CListCtrl*)GetDlgItem(IDC_lstFormationLibrary);

	ASSERT(pList);
	pList->SetExtendedStyle(LVS_EX_FULLROWSELECT);

	lvColumn.iSubItem=0;
	lvColumn.pszText="Material name";
	pList->InsertColumn(0, &lvColumn);

	//CFormationMaterialDatabase& database=*m_pModelDatabase;
	//CFormationMaterial&    material=*m_pSelectedMaterial;

	LV_ITEM lvItem;
	
	lvItem.mask= LVIF_TEXT|LVIF_STATE;
	lvItem.state=0;
	lvItem.stateMask=0;
	
	lvItem.iItem=0;
	lvItem.iSubItem=0;

	BuildMaterialList();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//##ModelId=3C60D5750197
void CFormationMatrialLibDlg::ShowMaterial(CString name)
{
	if(name.IsEmpty())
		return;
	CFormationMaterial fm;
	if(!m_pModelDatabase->GetMaterial(name,fm))
	{
		return;
	}
	m_lstMaterialParams.ShowMaterial(fm);
	*m_pSelectedMaterial=fm;
}

//##ModelId=3C60D5750206
void CFormationMatrialLibDlg::OnClicklstFormationLibrary(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	TRACE( "OnClicklstFormationLibrary\n");
	TRACE( "item:      %i \n" ,pNMListView->iItem);
	TRACE( "old state: %i \n" ,pNMListView->uOldState);
	TRACE( "new state: %i \n" ,pNMListView->uNewState);
	TRACE( "changed  : %i \n" ,pNMListView->uChanged);
	TRACE( "LVIS_SELECTED: %i \n" ,LVIS_SELECTED);
	TRACE("\n\n");
	
	int nItem = pNMListView->iItem;

	if (nItem<0)
	{
	    return ;
	}
	
	CListCtrl* pList=(CListCtrl*)GetDlgItem(IDC_lstFormationLibrary);
	strMatName=pList->GetItemText(nItem,0);
	ShowMaterial(strMatName);	
	*pResult = 0;

}

//##ModelId=3C60D5750233
void CFormationMatrialLibDlg::OnItemchangedlstFormationLibrary(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	*pResult = 0;
}

//##ModelId=3C60D5750252
void CFormationMatrialLibDlg::OnItemchanginglstFormationLibrary(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	*pResult = 0;
}


//##ModelId=3C60D5750281
void CFormationMatrialLibDlg::OnItemchanginglstFormationLibraryParams(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	*pResult = 0;
}

//##ModelId=3C8D9E1702FF
void CFormationMatrialLibDlg::OnbtnDelete() 
{
	if(strMatName.IsEmpty())
	{
		CListCtrl* pList=(CListCtrl*)GetDlgItem(IDC_lstFormationLibrary);
		strMatName=pList->GetItemText(0,0);
	}
	
	CFormationMaterial mat;
	m_pModelDatabase->GetMaterial(strMatName,mat);
	
	if(MessageBox("Are you sure you want to delete the material from the database?",NULL,MB_YESNO) == IDNO)
		return;
	else
	{
		m_pModelDatabase->RemoveMaterial(strMatName);
	}

	strMatName.Empty();
	UpdateData();
	BuildMaterialList();
	ShowMaterial(strMatName);

}

//##ModelId=3C8D9E17037C
void CFormationMatrialLibDlg::OnbtnEdit() 
{
	if(strMatName.IsEmpty())
	{
		CListCtrl* pList=(CListCtrl*)GetDlgItem(IDC_lstFormationLibrary);
		strMatName=pList->GetItemText(0,0);
	}
	
	CFormationMaterial mat;
	m_pModelDatabase->GetMaterial(strMatName,mat);
	CFormationMaterialDlg dlg(mat);

	if(dlg.DoModal()==IDOK)
	{
		mat=dlg.Material();
		if( m_pModelDatabase->MaterialExists(mat.Name()) && strMatName != mat.Name())
		{
			//name is changed AND already exists
			if(MessageBox("Material already exist in the database, replace?",NULL,MB_YESNO) == IDNO)
				return;
			else
			{
				//remove "old" mat with same new name
				m_pModelDatabase->RemoveMaterial(mat.Name());
			}
		}
		else //name is not changed OR doesn't exist
		{
			if(strMatName == mat.Name())
			{
				//remove "old" mat only when "old"name and "new"name are the same 
				m_pModelDatabase->RemoveMaterial(strMatName);
			}
		}

		m_pModelDatabase->AddMaterial(mat);
		UpdateData();
		//BuildMaterialList();
		ShowMaterial(mat.Name());
		strMatName = mat.Name(); // need to set material selected in list
		BuildMaterialList();
	}
	
}

//##ModelId=3C8D9E180011
void CFormationMatrialLibDlg::OnbtnNew() 
{
	CFormationMaterial mat;
	CFormationMaterialDlg dlg(mat);

	if(dlg.DoModal()==IDOK)
	{
		mat=dlg.Material();
		if( m_pModelDatabase->MaterialExists(mat.Name()) )
		{
			//name already exists
			if(MessageBox("Material already exist in the database, replace?",NULL,MB_YESNO) == IDNO)
				return;
			else
			{
				//remove "old" mat with same new name
				m_pModelDatabase->RemoveMaterial(mat.Name());
			}
		}
		m_pModelDatabase->AddMaterial(mat);
		UpdateData();
		ShowMaterial(mat.Name());
		strMatName = mat.Name(); // need to set material selected in list
		BuildMaterialList();
	}
	
}

//##ModelId=3C8D9E170188
void CFormationMatrialLibDlg::BuildMaterialList()
{
	CListCtrl* pList=0;
	pList=(CListCtrl*)GetDlgItem(IDC_lstFormationLibrary);
	pList->DeleteAllItems();
		
	CFormationMaterialDatabase& database=*m_pModelDatabase;
	CFormationMaterial&    material=*m_pSelectedMaterial;

	LV_ITEM lvItem;
	
	lvItem.mask= LVIF_TEXT|LVIF_STATE;
	lvItem.state=0;
	lvItem.stateMask=0;
	
	lvItem.iItem=0;
	lvItem.iSubItem=0;
				
	int i=0;
	database.MaterialTable().Open();
	database.MaterialTable().RS().MoveFirst();

	while(!database.MaterialTable().RS().IsEOF())
	{
		CString strName,strErr;
		database.MaterialTable().GetField(FORMATION_MATERIAL_NAME,strName);
		lvItem.iItem=i;
		lvItem.iSubItem=0;
		lvItem.pszText= strName.GetBufferSetLength(strName.GetLength());
		pList->InsertItem(&lvItem); //Formation Name
		database.MaterialTable().RS().MoveNext();
		i++;
	}

	database.MaterialTable().Close();

	LV_ITEM lv;
	lv.iItem=0;
	lv.iSubItem=0;
	lv.mask=LVIF_STATE;
	lv.state=LVIS_SELECTED;

	if(pList->GetItemCount()>0)
	{
		if(strMatName.IsEmpty())
		{
			pList->SetItem(0, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);
			strMatName=pList->GetItemText(0,0);
		}
		else
		{
			for(int i=0;i<pList->GetItemCount();i++)
			{
				if(pList->GetItemText(i,0)==strMatName)
				{
					pList->SetItem(i, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);
					pList->EnsureVisible(i,TRUE);
				}
			}
		}
	}
	ShowMaterial(strMatName);
	
}
