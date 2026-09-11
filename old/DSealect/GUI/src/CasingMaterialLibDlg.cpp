// CasingMaterialLibDlg.cpp: implementation of the CCasingMaterialLibDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dsealect.h"
#include "Global.h"
#include "DoubleEdit.h"
#include "CasingMaterial.h"
#include "CasingMaterialDlg.h"
#include "CasingMaterialLibDlg.h"


#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif


//##ModelId=3C60D57E0263
CCasingMaterialLibDlg::CCasingMaterialLibDlg(CString sDatabasePath,CDataModel *pDataModel,CWnd* pParent /*=NULL*/ )
	: CDialog(CCasingMaterialLibDlg::IDD, pParent)
{
	m_pDataModel        = pDataModel;
	m_strDatabasePath   = sDatabasePath;
	m_pModelDatabase	= std::auto_ptr<CCasingMaterialDatabase>(new CCasingMaterialDatabase());
	m_pModelDatabase->Database().Open(m_strDatabasePath,TRUE,FALSE,NULL);
	m_pSelectedMaterial =std::auto_ptr<CCasingMaterial>(new CCasingMaterial());

	//{{AFX_DATA_INIT(CCasingMaterialLibDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


//##ModelId=3C60D57E02A0
void CCasingMaterialLibDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCasingMaterialLibDlg)
	DDX_Control(pDX, IDC_lstCasingLibraryParams, m_lstMaterialParams);
	DDX_Control(pDX, IDC_lstCasingLibrary, m_lstMaterialNames);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCasingMaterialLibDlg, CDialog)
	//{{AFX_MSG_MAP(CCasingMaterialLibDlg)
	ON_NOTIFY(NM_CLICK, IDC_lstCasingLibrary, OnClicklstCasingLibrary)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_lstCasingLibrary, OnItemchangedlstCasingLibrary)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_lstCasingLibrary, OnItemchanginglstCasingLibrary)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_lstCasingLibraryParams, OnItemchanginglstCasingLibraryParams)
	ON_BN_CLICKED(IDC_btnDelete, OnbtnDelete)
	ON_BN_CLICKED(IDC_btnEdit, OnbtnEdit)
	ON_BN_CLICKED(IDC_btnNew, OnbtnNew)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCasingMaterialLibDlg message handlers

//##ModelId=3C60D57E02B1
BOOL CCasingMaterialLibDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//Create Columns
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
	lvColumn.fmt= LVCFMT_LEFT;
	lvColumn.cx=90;
	
	CListCtrl* pList=(CListCtrl*)GetDlgItem(IDC_lstCasingLibrary);

	ASSERT(pList);

	pList->SetExtendedStyle(LVS_EX_FULLROWSELECT);

	lvColumn.iSubItem=0;
	lvColumn.pszText="Material name";
	pList->InsertColumn(0, &lvColumn);
	
	//CCasingMaterialDatabase& database=*m_pModelDatabase;
	//CCasingMaterial&    material=*m_pSelectedMaterial;

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

//##ModelId=3C60D57E0252
void CCasingMaterialLibDlg::ShowMaterial(CString name)
{
	if(name.IsEmpty())
		return;
	CCasingMaterial cam;
	if(!m_pModelDatabase->GetMaterial(name,cam))
	{
		return;
	}
	m_lstMaterialParams.ShowMaterial(cam);
	*m_pSelectedMaterial=cam;
}

//##ModelId=3C60D57E02B3
void CCasingMaterialLibDlg::OnClicklstCasingLibrary(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	TRACE( "OnClicklstCasingLibrary\n");
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
	
	CListCtrl* pList=(CListCtrl*)GetDlgItem(IDC_lstCasingLibrary);
	//CString strMatName;
	strMatName=pList->GetItemText(nItem,0);
	ShowMaterial(strMatName);	
	*pResult = 0;
}

//##ModelId=3C60D57E02C1
void CCasingMaterialLibDlg::OnItemchangedlstCasingLibrary(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	*pResult = 0;
}

//##ModelId=3C60D57E02D0
void CCasingMaterialLibDlg::OnItemchanginglstCasingLibrary(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	*pResult = 0;
}


//##ModelId=3C60D57E02DF
void CCasingMaterialLibDlg::OnItemchanginglstCasingLibraryParams(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	*pResult = 0;
}

//##ModelId=3C8D9E240263
void CCasingMaterialLibDlg::OnbtnDelete() 
{
	if(strMatName.IsEmpty())
	{
		CListCtrl* pList=(CListCtrl*)GetDlgItem(IDC_lstCasingLibrary);
		strMatName=pList->GetItemText(0,0);
	}
	
	CCasingMaterial mat;
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

//##ModelId=3C8D9E2402E0
void CCasingMaterialLibDlg::OnbtnEdit() 
{
	if(strMatName.IsEmpty())
	{
		CListCtrl* pList=(CListCtrl*)GetDlgItem(IDC_lstCasingLibrary);
		strMatName=pList->GetItemText(0,0);
	}
	
	CCasingMaterial mat;
	m_pModelDatabase->GetMaterial(strMatName,mat);
	CCasingMaterialDlg dlg(mat,m_pDataModel);

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

//##ModelId=3C8D9E24038C
void CCasingMaterialLibDlg::OnbtnNew() 
{
	CCasingMaterial mat;
	CCasingMaterialDlg dlg(mat,m_pDataModel);

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

//##ModelId=3C8D9E2400CC
void CCasingMaterialLibDlg::BuildMaterialList()
{
	CListCtrl* pList=0;
	pList=(CListCtrl*)GetDlgItem(IDC_lstCasingLibrary);
	pList->DeleteAllItems();
		
	CCasingMaterialDatabase& database=*m_pModelDatabase;
	CCasingMaterial&    material=*m_pSelectedMaterial;

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
		database.MaterialTable().GetField(CASING_MATERIAL_NAME,strName);
		lvItem.iItem=i;
		lvItem.iSubItem=0;
		lvItem.pszText= strName.GetBufferSetLength(strName.GetLength());
		pList->InsertItem(&lvItem); //Casing Name
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
