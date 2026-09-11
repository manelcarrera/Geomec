#include "stdafx.h"
#include "geomec.h"

#include "MatLibDlg.h"

#include "mlMaterialLibrary.h"
#include "mlMatParam.h"
#include "Materials.h"
#include "MatParamDlg.h"
#include "LibraryMaterial.h"
#include "MaterialHelperFactory.h"
#include "RenameMaterialDlg.h"
#include "GlobalMessage.h"
#include "resourceIDI.h"
#include "MatLibListCtrl.h"

bool CMatLibDlg::m_bMatListAscending = true;
bool CMatLibDlg::m_bParamListAscending = true;
int CMatLibDlg::m_iSortColumn = 0;

CMatLibDlg::CMatLibDlg(ml::CMaterialLibrary& matlib, int modelfilter, CAnalysisLogger& logger, bool bImportMode, UINT nIDTemplate, CWnd* pParent)
: CDialog(nIDTemplate, pParent),
  m_matlib(matlib),
  m_nModelFilter(modelfilter),
  m_logger(logger),
  m_bImportMode(bImportMode)
{
  m_nUnitDef = (GetGeomecDoc()->UnitNode().Unit() == IQuantityDouble::SI_UNIT ? 0 : 1);
}

BEGIN_MESSAGE_MAP(CMatLibDlg, CDialog)
	//{{AFX_MSG_MAP(CMatLibDlg)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTMATERIALS, OnItemchangedListmaterials)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LISTMATERIALS, OnEndlabeleditListmaterials)
	ON_BN_CLICKED(IDC_BUT_DELETE, OnButDelete)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LISTMATERIALS, OnColumnclickListmaterials)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LISTPARAMETERS, OnColumnclickListparameters)
	ON_BN_CLICKED(IDC_BUT_NEW, OnButNew)
	ON_BN_CLICKED(IDC_BUT_EDIT, OnButEdit)
	ON_NOTIFY(NM_DBLCLK, IDC_LISTMATERIALS, OnDblclkListmaterials)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

std::vector<ml::CMaterial*> CMatLibDlg::SelectedMaterials()
{
  return m_vcSelection;
}

BOOL CMatLibDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	HICON hIcon = AfxGetApp()->LoadIcon(IDI_MATLIB);
	SetIcon(hIcon, TRUE );
	SetIcon(hIcon, FALSE);

	CString strError;

	InitListControls();
	FillListControls();
	GetDlgItem(IDC_BUT_DELETE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUT_EDIT)->EnableWindow(FALSE);

	DWORD dwStyleAdd = 0;
	DWORD dwStyleRem = LVS_SINGLESEL;

	CListCtrl *pListCtrl = (CListCtrl *) GetDlgItem(IDC_LISTMATERIALS);
	pListCtrl->ModifyStyle(dwStyleRem, dwStyleAdd);

  GetDlgItem(IDOK)->ShowWindow(m_bImportMode ? SW_SHOW : SW_HIDE);
  GetDlgItem(IDOK)->EnableWindow(FALSE);

  if(m_bImportMode)
    SetWindowText("Import From Material Library");

  m_ImageList.Create(16, 16, ILC_COLOR, 0, 2); // waij TFS 80780

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMatLibDlg::OnOK()
{
  m_vcSelection = CurrentSelection();
  CDialog::OnOK();
}

void CMatLibDlg::InitListControls()
{
	CString str;
	CListCtrl *pListCtrl;
	// Initialize the Materials list control
	// Get a pointer to the list control
	pListCtrl = (CListCtrl *)GetDlgItem(IDC_LISTMATERIALS);

	// By default, item selection in a list control is only possible in the 1st column,
	// so add full row selection to the list control style:
	pListCtrl->SetExtendedStyle(pListCtrl->GetExtendedStyle() | LVS_EX_FULLROWSELECT);


	// Create the 1st column for the list control
  str = QObject::tr("Name").toStdString().c_str();
	LVCOLUMN lvc;
	lvc.mask      = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
	lvc.pszText   = str.GetBuffer( str.GetLength() );        // header
	lvc.cx        = 140;           // width in pixels
	lvc.fmt       = LVCFMT_LEFT;   // format left align
	pListCtrl->InsertColumn(1, &lvc);
	str.ReleaseBuffer();

	// Create the 2nd column for the list control
  str = QObject::tr("Model").toStdString().c_str();
	lvc.mask     |= LVCF_SUBITEM;
	lvc.iSubItem  = 1;             // assign column to subitem nr 1
	lvc.pszText   = str.GetBuffer( str.GetLength() );
	lvc.cx        = 113;
	pListCtrl->InsertColumn(2, &lvc);
	str.ReleaseBuffer();
	
	// Initialize the Parameters list control
	// Get a pointer to the list control
	pListCtrl = (CListCtrl *)GetDlgItem(IDC_LISTPARAMETERS);

	// By default, item selection in a list control is only possible in the 1st column,
	// so add full row selection to the list control style:
//	pListCtrl->SetExtendedStyle(pListCtrl->GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	// Create the 1st column for the list control
  str = QObject::tr("Name").toStdString().c_str();
	lvc.mask      = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
	lvc.pszText   = str.GetBuffer( str.GetLength() );        // header
	lvc.cx        = 190;           // width in pixels
	lvc.fmt       = LVCFMT_LEFT;   // format left align
	pListCtrl->InsertColumn(1, &lvc);
	str.ReleaseBuffer();

	// Create the 2nd column for the list control
  str = QObject::tr("Value").toStdString().c_str();
	lvc.mask     |= LVCF_SUBITEM | LVCF_FMT;
	lvc.iSubItem  = 1;             // assign column to subitem nr 1
	lvc.pszText   = str.GetBuffer( str.GetLength() );
	lvc.cx        = 97;
	lvc.fmt		  = LVCFMT_RIGHT;
	pListCtrl->InsertColumn(2, &lvc);
	str.ReleaseBuffer();
	
	// Create the 3rd column for the list control
  str = QObject::tr("Unit").toStdString().c_str();
	lvc.mask     ^= LVCF_FMT;
	lvc.iSubItem  = 2;             // assign column to subitem nr 1
	lvc.pszText   = str.GetBuffer( str.GetLength() );
	lvc.cx        = 50;
	pListCtrl->InsertColumn(3, &lvc);
	str.ReleaseBuffer();
}

void CMatLibDlg::FillListControls()
{
	CListCtrl *pListCtrl = (CListCtrl *)GetDlgItem(IDC_LISTMATERIALS);
	pListCtrl->DeleteAllItems();

	LVITEM lvi;
	lvi.mask = LVIF_TEXT;
	lvi.iItem = 0;

	for(int i = 0; i < m_matlib.MaterialSize(); ++i)
	{
		CString strName = m_matlib.Material(i).Name().toStdString().c_str();

    lvi.iSubItem = 0;
	  lvi.pszText = strName.GetBuffer(strName.GetLength());
	  lvi.iItem = pListCtrl->InsertItem(&lvi);
	  pListCtrl->SetItemData(lvi.iItem, (DWORD_PTR)(i));
	  strName.ReleaseBuffer();

	  lvi.iSubItem = 1;
	  CString strModelName = m_matlib.Material(i).MaterialModelName().toStdString().c_str();
	  lvi.pszText = strModelName.GetBuffer(strModelName.GetLength());
	  pListCtrl->SetItem(&lvi);
	  strModelName.ReleaseBuffer();
	}

	pListCtrl->SortItems(CompareFuncMatList, (DWORD_PTR)&m_matlib);
}

int CMatLibDlg::CompareFuncMatList(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
  ml::CMaterialLibrary& matlib = *((ml::CMaterialLibrary*)lParamSort);
  ml::CMaterial& mat1 = matlib.Material(lParam1);
  ml::CMaterial& mat2 = matlib.Material(lParam2);
	bool bUseAscending = true;

	int icmp;

	if (m_iSortColumn == 0)
	{
		// sort 1st column
		icmp = mat1.Name().compare(mat2.Name());
		// if equal, sort on 2nd column!
	}

	if(m_iSortColumn == 1 || !icmp)
	{
		if(m_iSortColumn != 1 )
      bUseAscending = false;
		// sort 2nd column
		icmp = mat1.MaterialModelName().compare(mat2.MaterialModelName());
		// if equal, sort on 1st column
		if (!icmp && m_iSortColumn != 0)
		{
			icmp = mat1.Name().compare(mat2.Name());
			bUseAscending = false;
		}
	}

	if(bUseAscending)
	{
		if(icmp < 0)
		{
			// 1st item less than 2nd
			if(m_bMatListAscending) return -1; // 1st should precede 2nd
			else                    return  1; // 1st should follow 2nd
		}
		else
		{
			// 1st item more than 2nd
			if(m_bMatListAscending) return  1; // 1st should follow 2nd
			else                    return -1; // 1st should precede 2nd
		}
	}
	else
	{
		return(icmp > 0 ? 1 : -1);
	}
}

int CALLBACK CMatLibDlg::CompareFuncParamList( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort )
{
  const ml::CMatParam& param1 = *((const ml::CMatParam*)lParam1);
  const ml::CMatParam& param2 = (*(const ml::CMatParam*)lParam2);
	int nUnitDef = (int)lParamSort;
	bool bUseAscending = true;

	int icmp = 1;

	if(m_iSortColumn == 0)
	{
		// sort 1st column
		icmp = param1.Name().compare(param2.Name());
		// if equal, sort on 2nd column!
	}

	double dComp1 = param1.ValueToUserUnit(nUnitDef);
	double dComp2 = param2.ValueToUserUnit(nUnitDef);

	if(m_iSortColumn == 1 || !icmp)
	{
		if(m_iSortColumn != 1)
      bUseAscending = false;
		// sort 2nd column
		icmp = (dComp1 < dComp2 ? -1 : 1);
		if(dComp1 == dComp2)
      icmp = 0;

		// if equal, sort on 1st column
		if(!icmp && m_iSortColumn == 1)
		{
			icmp = param1.Name().compare(param2.Name());
			bUseAscending = false;
		}
	}

	if(m_iSortColumn == 2 || !icmp)
	{
		if(m_iSortColumn != 2)
      bUseAscending = false;
		// sort 3rd column
		icmp = strcmp(param1.UnitName(nUnitDef), param2.UnitName(nUnitDef));
		// if equal sort on 1st column
		if(!icmp && (m_iSortColumn == 1 || m_iSortColumn == 2))
		{
			icmp = param1.Name().compare(param2.Name());
			bUseAscending = false;
		}
	}

	if(bUseAscending)
	{
		if(icmp < 0)
		{
			// 1st item less than 2nd
			if(m_bParamListAscending) return -1; // 1st should precede 2nd
			else                      return  1; // 1st should follow 2nd
		}
		else
		{
			// 1st item more than 2nd
			if(m_bParamListAscending) return  1; // 1st should follow 2nd
			else                      return -1; // 1st should precede 2nd
		}
	}
	else
	{
		return(icmp > 0 ? 1 : -1);
	}
}

void CMatLibDlg::UpdateParameterList()
{
	CListCtrl* pListCtrl1 = (CListCtrl*)GetDlgItem(IDC_LISTMATERIALS);
	POSITION pos = pListCtrl1->GetFirstSelectedItemPosition();
	if(pos)
	{
		int nIndex = pListCtrl1->GetNextSelectedItem(pos);
		if(pos)
		{
			// multiple selected
			CListCtrl *pListCtrl2 = (CListCtrl*) GetDlgItem(IDC_LISTPARAMETERS);
			pListCtrl2->DeleteAllItems();
			GetDlgItem(IDC_BUT_DELETE)->EnableWindow(TRUE); // can delete multiple
			GetDlgItem(IDC_BUT_EDIT)->EnableWindow(FALSE); //can't edit multiple
      GetDlgItem(IDOK)->EnableWindow(TRUE); // can import multiple
		}
		else
		{
			// single selection, show material parameters
      const ml::CMaterial& mat = m_matlib.Material(pListCtrl1->GetItemData(nIndex));
			DisplayMaterial(mat);
			GetDlgItem(IDC_BUT_DELETE)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUT_EDIT)->EnableWindow(TRUE);
      GetDlgItem(IDOK)->EnableWindow(TRUE);
		}
	}
	else
	{
		// none selected
		CListCtrl* pListCtrl2 = (CListCtrl*)GetDlgItem(IDC_LISTPARAMETERS);
		pListCtrl2->DeleteAllItems();
		GetDlgItem(IDC_BUT_DELETE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUT_EDIT)->EnableWindow(FALSE);
    GetDlgItem(IDOK)->EnableWindow(FALSE);
	}
}

void CMatLibDlg::DisplayMaterial(const ml::CMaterial& mat)
{
  const CLibraryMaterial& libMat = static_cast<const CLibraryMaterial&>(mat);

	CListCtrl* pListCtrl = (CListCtrl*)GetDlgItem(IDC_LISTPARAMETERS);

	pListCtrl->DeleteAllItems();

  // waij TFS 80780
  // clear list of images
  while (m_ImageList.GetImageCount()>0)
  {
    m_ImageList.Remove(m_ImageList.GetImageCount()-1);
  }
  
  // fill list of images
  for(size_t i = 0; i < libMat.ParameterSize(); ++i)
  {
    m_ImageList.Add(AfxGetApp()->LoadIcon(libMat.Parameter(i).IconID()));
  }

  // assign list of images to CListCtrl instance
  pListCtrl->SetImageList(&m_ImageList, LVSIL_SMALL);

  // show list ctrl items (text + icon) and their
  // subitems (='items' in the next columns)
  for(int i = 0; i < libMat.ParameterSize(); ++i)
  {
    const CLibraryMaterialParameter& param = libMat.Parameter(i);
    CString strParamName = param.Name().toStdString().c_str();
    int nIndex = pListCtrl->InsertItem(i, strParamName, i);
    assert(nIndex==i);
    pListCtrl->SetItemData(i, (DWORD_PTR)&param);

    CString str1;
    str1.Format("%g", param.ValueToUserUnit(m_nUnitDef));
    pListCtrl->SetItemText( i, 1, str1);

    CString str2 = param.UnitName(m_nUnitDef);
    pListCtrl->SetItemText( i, 2, str2);
  }

  // waij TFS 81579
  AdjustColumnWidth(this, pListCtrl);

  pListCtrl->SortItems(CompareFuncParamList, (DWORD)m_nUnitDef);

  // force redraw
  Invalidate();
  UpdateWindow();
  
}

void CMatLibDlg::SelectMaterial(const ml::CMaterial& mat)
{
	CListCtrl* pListCtrl = (CListCtrl*)GetDlgItem(IDC_LISTMATERIALS);
  int n = pListCtrl->GetItemCount();
  for(int i = 0; i < n; ++i)
  {
    ml::CMaterial& m = m_matlib.Material(pListCtrl->GetItemData(i));
    if(&mat == &m)
    {
      pListCtrl->SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
      break;
    }
  }
}

std::vector<ml::CMaterial*> CMatLibDlg::CurrentSelection()
{
  std::vector<ml::CMaterial*> vcSelected;

	CListCtrl *pListCtrl = (CListCtrl*)GetDlgItem(IDC_LISTMATERIALS);
	POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
	while(pos)
	{
		int nIndex = pListCtrl->GetNextSelectedItem(pos);
    ml::CMaterial& mat = m_matlib.Material(pListCtrl->GetItemData(nIndex));
    vcSelected.push_back(&mat);
  }

  return vcSelected;
}

void CMatLibDlg::OnItemchangedListmaterials(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here

	UpdateParameterList();
	
	*pResult = 0;
}

void CMatLibDlg::OnEndlabeleditListmaterials(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	CListCtrl *pListCtrl = (CListCtrl *) GetDlgItem( IDC_LISTMATERIALS );
	if( pDispInfo->item.pszText == NULL ) return; // user cancelled rename

	CString strNewName = pDispInfo->item.pszText;

  ml::CMaterial& mat = m_matlib.Material(pListCtrl->GetItemData(pDispInfo->item.iItem));
  mat.Name((LPCSTR)strNewName);

  pListCtrl->SetItemText(pDispInfo->item.iItem, 0, (LPCSTR)strNewName);
  //FillListControls();
	
	*pResult = 0;
}

void CMatLibDlg::OnButDelete()
{
  std::vector<ml::CMaterial*> vcDelete = CurrentSelection();

  assert(!vcDelete.empty());

  CString msg;
  msg.Format("Are you sure you want to remove %s?", (vcDelete.size() == 1 ? "this material" : "these materials"));
  if(_m()->msg(msg, MB_YESNO | MB_ICONQUESTION) == IDNO)
    return;

  for(size_t i = 0; i < vcDelete.size(); ++i)
    m_matlib.DeleteMaterial(*vcDelete[i]);

	FillListControls();
	UpdateParameterList();
	CListCtrl *pListCtrl = (CListCtrl*)GetDlgItem(IDC_LISTMATERIALS);
	pListCtrl->SetFocus();
}

void CMatLibDlg::OnColumnclickListmaterials(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iSubItem == m_iSortColumn)
    m_bMatListAscending = !m_bMatListAscending;

	CListCtrl* pListCtrl = (CListCtrl*)GetDlgItem(IDC_LISTMATERIALS);
	m_iSortColumn = pNMListView->iSubItem;

	pListCtrl->SortItems(CompareFuncMatList, (DWORD_PTR)this);
	
	*pResult = 0;
}

void CMatLibDlg::OnColumnclickListparameters(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iSubItem == m_iSortColumn)
    m_bParamListAscending = !m_bParamListAscending;

	CListCtrl* pListCtrl = (CListCtrl*)GetDlgItem(IDC_LISTPARAMETERS);
	m_iSortColumn = pNMListView->iSubItem;

	pListCtrl->SortItems(CompareFuncParamList, m_nUnitDef);
	
	*pResult = 0;
}

void CMatLibDlg::OnButNew()
{
  CRenameMaterialDlg dlg(QString(), MM_LINEAR, true, m_nModelFilter, this);
  if(dlg.DoModal() == IDOK)
  {
    const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();

    ml::CMaterial::CCreator* pCreator = f->getMatCreator(int(dlg.MaterialModel()));
    ml::CMaterial& mat = m_matlib.AddMaterial(dlg.Name(), *pCreator);
    FillListControls();
    SelectMaterial(mat);
    DisplayMaterial(mat);
    OnButEdit();
  }
}

void CMatLibDlg::OnButEdit()
{
	CListCtrl* pListCtrl = (CListCtrl*)GetDlgItem(IDC_LISTMATERIALS);
	POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
	int nIndex = pListCtrl->GetNextSelectedItem(pos);

  if(!pos)
  {
    ml::CMaterial& mat = m_matlib.Material(pListCtrl->GetItemData(nIndex));
    assert(dynamic_cast<CLibraryMaterial*>(&mat));
    CMatParamDlg dlg(static_cast<CLibraryMaterial&>(mat), m_matlib, m_nModelFilter, m_logger, this);
    dlg.DoModal();
    FillListControls();
    SelectMaterial(mat);
    DisplayMaterial(mat);
  }
}

void CMatLibDlg::OnDblclkListmaterials(NMHDR* pNMHDR, LRESULT* pResult)
{
  OnButEdit(); // edit selected material
}
