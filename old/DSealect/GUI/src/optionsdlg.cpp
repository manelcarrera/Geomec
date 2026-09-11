// optionsdlg.cpp : implementation file
//

#include "stdafx.h"
#include "dsealect.h"
#include "DSealectDoc.h"
#include "DSealectGlobal.h"
#include "Global.h"
#include "optionsdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg dialog


//##ModelId=3DDA0C5002C3
COptionsDlg::COptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


//##ModelId=3DDA0C5002C5
void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
	//{{AFX_MSG_MAP(COptionsDlg)
	ON_BN_CLICKED(IDC_btnBrowseCasLib, OnbtnBrowseCasLib)
	ON_BN_CLICKED(IDC_btnBrowseCemLib, OnbtnBrowseCemLib)
	ON_BN_CLICKED(IDC_btnBrowseFormLib, OnbtnBrowseFormLib)
	ON_BN_CLICKED(IDC_btnBrowseTempDir, OnbtnBrowseTempDir)
	ON_BN_CLICKED(IDC_btnBrowseWorkDirOptions, OnbtnBrowseWorkDirOptions)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg message handlers

//##ModelId=3DDA0C5002D2
BOOL COptionsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	GetDlgItem(IDC_ebTempDir)->SetWindowText(GetDoc()->TempDir());
	GetDlgItem(IDC_ebWorkDir)->SetWindowText(GetDoc()->FileWorkDir());
	GetDlgItem(IDC_ebFormationLibrary)->SetWindowText(GetDoc()->FormationMatDatabasePath());
	GetDlgItem(IDC_ebCementLibrary)->SetWindowText(GetDoc()->CementMatDatabasePath());
	GetDlgItem(IDC_ebCasingLibrary)->SetWindowText(GetDoc()->CasingMatDatabasePath());

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//##ModelId=3DDA0C5002D4
void COptionsDlg::OnbtnBrowseCasLib() 
{
	CString sFilter;
	sFilter = "Mdb Files (*.mdb)|*.mdb|DSealect (*.dsl)|*.dsl|All Files (*.*)|*.*||"; 
	
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, sFilter, this);

	fileDlg.m_ofn.lpstrTitle = "Select Casing Library";

	CString sCaL;
	GetDlgItem(IDC_ebCasingLibrary)->GetWindowText(sCaL);
	fileDlg.m_ofn.lpstrInitialDir = sCaL;

	if ( fileDlg.DoModal() == IDOK)
	{
		GetDlgItem(IDC_ebCasingLibrary)->SetWindowText(fileDlg.GetPathName());
	}
}

//##ModelId=3DDA0C5002D6
void COptionsDlg::OnbtnBrowseCemLib() 
{
	CString sFilter;
	sFilter = "Mdb Files (*.mdb)|*.mdb|DSealect (*.dsl)|*.dsl|All Files (*.*)|*.*||"; 
	
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, sFilter, this);


	fileDlg.m_ofn.lpstrTitle = "Select Cement Library";

	CString sCeL;
	GetDlgItem(IDC_ebCementLibrary)->GetWindowText(sCeL);
	fileDlg.m_ofn.lpstrInitialDir = sCeL;

	if ( fileDlg.DoModal() == IDOK)
	{
		GetDlgItem(IDC_ebCementLibrary)->SetWindowText(fileDlg.GetPathName());
	}
	
}

//##ModelId=3DDA0C5002D8
void COptionsDlg::OnbtnBrowseFormLib() 
{
	CString sFilter;
	sFilter = "Mdb Files (*.mdb)|*.mdb|DSealect (*.dsl)|*.dsl|All Files (*.*)|*.*||"; 
	
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, sFilter, this);


	fileDlg.m_ofn.lpstrTitle = "Select Formation Library";
	
	CString sFL;
	GetDlgItem(IDC_ebFormationLibrary)->GetWindowText(sFL);
	fileDlg.m_ofn.lpstrInitialDir = sFL ;

	if ( fileDlg.DoModal() == IDOK)
	{
		GetDlgItem(IDC_ebFormationLibrary)->SetWindowText(fileDlg.GetPathName());
	}
}

//##ModelId=3DDA0C5002DA
void COptionsDlg::OnbtnBrowseTempDir() 
{
	CString sTmp;
	GetDlgItem(IDC_ebTempDir)->GetWindowText(sTmp);

	CString sNew = BrowseFolder(this->GetSafeHwnd(),"Select Temp Directory",sTmp);
	
	if(sNew.IsEmpty())
	{
		GetDlgItem(IDC_ebTempDir)->SetWindowText(sTmp);
	}
	else
	{
		GetDlgItem(IDC_ebTempDir)->SetWindowText(sNew);
	}
	
	
}

//##ModelId=3DDA0C5002E1
void COptionsDlg::OnOK() 
{
	CString sTD;
	GetDlgItem(IDC_ebTempDir)->GetWindowText(sTD);
	GetDoc()->SetTempDir(sTD);
	
	CString sFWD;
	GetDlgItem(IDC_ebWorkDir)->GetWindowText(sFWD);
	GetDoc()->SetFileWorkDir(sFWD);

	CString sFL;
	GetDlgItem(IDC_ebFormationLibrary)->GetWindowText(sFL);
	GetDoc()->SetFormationMatDatabasePath(sFL);

	CString sCeL;
	GetDlgItem(IDC_ebCementLibrary)->GetWindowText(sCeL);
	GetDoc()->SetCementMatDatabasePath(sCeL);

	CString sCaL;
	GetDlgItem(IDC_ebCasingLibrary)->GetWindowText(sCaL);
	GetDoc()->SetCasingMatDatabasePath(sCaL);
	
	CDialog::OnOK();
}

//##ModelId=3DDA0C5002E3
void COptionsDlg::OnbtnBrowseWorkDirOptions() 
{
	CString sTmp;
	GetDlgItem(IDC_ebWorkDir)->GetWindowText(sTmp);

	CString sNew = BrowseFolder(this->GetSafeHwnd(),"Select File Work Directory",sTmp);
	
	if(sNew.IsEmpty())
	{
		GetDlgItem(IDC_ebWorkDir)->SetWindowText(sTmp);
	}
	else
	{
		GetDlgItem(IDC_ebWorkDir)->SetWindowText(sNew);
	}
	
}
