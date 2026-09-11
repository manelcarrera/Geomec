// SelectWorksheetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "SelectWorksheetDlg.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectWorksheetDlg dialog


CSelectWorksheetDlg::CSelectWorksheetDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CSelectWorksheetDlg::IDD, pParent)
{
  //{{AFX_DATA_INIT(CSelectWorksheetDlg)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
  m_piIndices = NULL;
  m_nIndices = 0;
}

CSelectWorksheetDlg::~CSelectWorksheetDlg()
{
  if( m_piIndices ) free( m_piIndices );
}

void CSelectWorksheetDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CSelectWorksheetDlg)
    // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectWorksheetDlg, CDialog)
  //{{AFX_MSG_MAP(CSelectWorksheetDlg)
  ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectWorksheetDlg message handlers
void CSelectWorksheetDlg::SetStrings( const std::vector<CString>& vcStrings )
{
  m_vcStrings = vcStrings;
}

void CSelectWorksheetDlg::GetSelectedIndices( int **piIndices, int *nIndices )
{
  *piIndices = m_piIndices;
  *nIndices = m_nIndices;
}

BOOL CSelectWorksheetDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();

  CListBox *pList = (CListBox *) GetDlgItem( IDC_LIST );
  for( size_t i=0; i<m_vcStrings.size(); i++ )
  {
    pList->AddString( m_vcStrings[i] );
  }

  SetWindowText( m_strTitle );

  pList->SetSel( -1, TRUE );

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectWorksheetDlg::OnOK() 
{
  CListBox *pList = (CListBox *) GetDlgItem( IDC_LIST );

  m_nIndices = pList->GetSelCount();
  m_piIndices = (int *)malloc( m_nIndices * sizeof( int ) );

  pList->GetSelItems( m_nIndices, m_piIndices );

  CDialog::OnOK();
}

void CSelectWorksheetDlg::OnSelchangeList() 
{
  CListBox *pList = (CListBox *) GetDlgItem( IDC_LIST );
  GetDlgItem( IDOK )->EnableWindow( pList->GetSelCount() > 0 );
}
