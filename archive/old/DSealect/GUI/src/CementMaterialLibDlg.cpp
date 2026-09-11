// CementMaterialLibDlg.cpp: implementation of the CCementMaterialLibDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dsealect.h"
#include "Global.h"
#include "DoubleEdit.h"
#include "CementMaterial.h"
#include "CementMaterialDlg.h"
#include "CementMaterialLibDlg.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif


//##ModelId=3C60D57B03AA
CCementMaterialLibDlg::CCementMaterialLibDlg(CString sDatabasePath,CWnd* pParent /*=NULL*/ )
	: CDialog(CCementMaterialLibDlg::IDD, pParent)
{
	m_strDatabasePath   = sDatabasePath;
	m_pModelDatabase	= std::auto_ptr<CCementMaterialDatabase>(new CCementMaterialDatabase());
	m_pModelDatabase->Database().Open(m_strDatabasePath,TRUE,FALSE,NULL);
	m_pSelectedMaterial =std::auto_ptr<CCementMaterial>(new CCementMaterial());

	//{{AFX_DATA_INIT(CCementMaterialLibDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


//##ModelId=3C60D57C0020
void CCementMaterialLibDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCementMaterialLibDlg)
	DDX_Control(pDX, IDC_lstCementLibraryParams, m_lstMaterialParams);
	DDX_Control(pDX, IDC_lstCementLibrary, m_lstMaterialNames);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCementMaterialLibDlg, CDialog)
	//{{AFX_MSG_MAP(CCementMaterialLibDlg)
	ON_NOTIFY(NM_CLICK, IDC_lstCementLibrary, OnClicklstCementLibrary)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_lstCementLibrary, OnItemchangedlstCementLibrary)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_lstCementLibrary, OnItemchanginglstCementLibrary)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_lstCementLibraryParams, OnItemchanginglstCementLibraryParams)
	ON_BN_CLICKED(IDC_btnDelete, OnbtnDelete)
	ON_BN_CLICKED(IDC_btnEdit, OnbtnEdit)
	ON_BN_CLICKED(IDC_btnNew, OnbtnNew)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCementMaterialLibDlg message handlers

//##ModelId=3C60D57C003F
BOOL CCementMaterialLibDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//Create Columns
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
	lvColumn.fmt= LVCFMT_LEFT;
	lvColumn.cx=90;
	
	CListCtrl* pList=(CListCtrl*)GetDlgItem(IDC_lstCementLibrary);

	ASSERT(pList);

	pList->SetExtendedStyle(LVS_EX_FULLROWSELECT);

	lvColumn.iSubItem=0;
	lvColumn.pszText="Material name";
	pList->InsertColumn(0, &lvColumn);
	
	//CCementMaterialDatabase& database=*m_pModelDatabase;
	//CCementMaterial&    material=*m_pSelectedMaterial;

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

//##ModelId=3C60D57B039A
void CCementMaterialLibDlg::ShowMaterial(CString name)
{
	if(name.IsEmpty())
		return;
	CCementMaterial cam;
	if(!m_pModelDatabase->GetMaterial(name,cam))
	{
		return;
	}
	m_lstMaterialParams.ShowMaterial(cam);
	*m_pSelectedMaterial=cam;
}

//##ModelId=3C60D57C0041
void CCementMaterialLibDlg::OnClicklstCementLibrary(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	TRACE( "OnClicklstCementLibrary\n");
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
	
	CListCtrl* pList=(CListCtrl*)GetDlgItem(IDC_lstCementLibrary);
	//CString strMatName;
	strMatName=pList->GetItemText(nItem,0);
	ShowMaterial(strMatName);	
	*pResult = 0;
}

//##ModelId=3C60D57C005E
void CCementMaterialLibDlg::OnItemchangedlstCementLibrary(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	*pResult = 0;
}

//##ModelId=3C60D57C008D
void CCementMaterialLibDlg::OnItemchanginglstCementLibrary(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	*pResult = 0;
}


//##ModelId=3C60D57C00AC
void CCementMaterialLibDlg::OnItemchanginglstCementLibraryParams(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	*pResult = 0;
}

//##ModelId=3C8D9E2000AD
void CCementMaterialLibDlg::OnbtnDelete() 
{
	if(strMatName.IsEmpty())
	{
		CListCtrl* pList=(CListCtrl*)GetDlgItem(IDC_lstCementLibrary);
		strMatName=pList->GetItemText(0,0);
	}
	
	CCementMaterial mat;
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

//##ModelId=3C8D9E20013A
void CCementMaterialLibDlg::OnbtnEdit() 
{
	if(strMatName.IsEmpty())
	{
		CListCtrl* pList=(CListCtrl*)GetDlgItem(IDC_lstCementLibrary);
		strMatName=pList->GetItemText(0,0);
	}
	
	CCementMaterial mat;
	m_pModelDatabase->GetMaterial(strMatName,mat);
	CCementMaterialDlg dlg(mat);

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

//##ModelId=3C8D9E2001B7
void CCementMaterialLibDlg::OnbtnNew() 
{
	CCementMaterial mat;
	CCementMaterialDlg dlg(mat);

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

//##ModelId=3C8D9E1F031E
void CCementMaterialLibDlg::BuildMaterialList()
{
	CListCtrl* pList=0;
	pList=(CListCtrl*)GetDlgItem(IDC_lstCementLibrary);
	pList->DeleteAllItems();
		
	CCementMaterialDatabase& database=*m_pModelDatabase;
	CCementMaterial&    material=*m_pSelectedMaterial;

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
		database.MaterialTable().GetField(CEMENT_MATERIAL_NAME,strName);
		lvItem.iItem=i;
		lvItem.iSubItem=0;
		lvItem.pszText= strName.GetBufferSetLength(strName.GetLength());
		pList->InsertItem(&lvItem); //Cement Name
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
