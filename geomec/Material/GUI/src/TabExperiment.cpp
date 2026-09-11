#include "stdafx.h"
#include "geomec.h"

#include "TabExperiment.h"
#include "excel8.h"
#include "SelectWorksheetDlg.h"
#include "MatParamDlg.h"
#include "GlobalMessage.h"


BEGIN_MESSAGE_MAP(CTabExperiment, CDialog)
  //{{AFX_MSG_MAP(CTabExperiment)
  ON_BN_CLICKED(IDC_INSERT, OnInsert)
  ON_BN_CLICKED(IDC_DELETE, OnDelete)
  ON_LBN_SELCHANGE(IDC_LIST_FILES, OnSelchangeListFiles)
  ON_BN_CLICKED(IDC_GAMMASALL, OnGammasall)
  ON_BN_CLICKED(IDC_GAMMASSELECTED, OnGammasselected)
  //}}AFX_MSG_MAP
//	ON_MESSAGE( WM_NOTIFYPARENT, OnFileListUpdated )
//	ON_MESSAGE( WM_EDITLOSTFOCUS, OnEditLostFocus )
END_MESSAGE_MAP()


CTabExperiment::CTabExperiment(CMatParamDlg& dlg, CWnd* pParent)
: CDialog(IDD, pParent),
  m_iShowInPicture(0),
  m_Dlg(dlg),
  m_iCurrentSel(-1)
, m_bUseInitialLowerLimit(false)
, m_iInitialLowerLimit(0)
{
}

unsigned int CTabExperiment::ID()
{
  return IDD;
}

CMatParamDlg& CTabExperiment::Dlg()
{
  return m_Dlg;
}

bool CTabExperiment::ShowAllExperiments() const
{
  return (m_iShowInPicture == 0);
}

CExperimentArray& CTabExperiment::ExperimentData()
{
  return m_vcExperimentData;
}

const CExperimentArray& CTabExperiment::ExperimentData() const
{
  return m_vcExperimentData;
}

void CTabExperiment::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);

  DDX_Radio(pDX, IDC_GAMMASALL, m_iShowInPicture);
}

BOOL CTabExperiment::OnInitDialog() 
{
  CDialog::OnInitDialog();

  // initially disable delete button (list is empty)
  GetDlgItem( IDC_DELETE )->EnableWindow( FALSE );

  m_iShowInPicture = 0;
//	UpdatePictureRadios();
//	UpdateWeightFactor();

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CTabExperiment::OnInsert() 
{
  // show open dialog, with multiple selection option
  CString strExtension, strFilter;
  strExtension = "ges";
  strFilter = "Excel files (*.xls*)|*.xls*|All files (*.*)|*.*||";

  // Optional parameters which are not passed 
  // can be passed a VARIANT containing VT_ERROR/DISP_E_PARAMNOTFOUND.
  VARIANT vNotPassed;
  V_VT( &vNotPassed ) = VT_ERROR;
  V_ERROR( &vNotPassed ) = DISP_E_PARAMNOTFOUND;

  VARIANT vFALSE;
  V_VT( &vFALSE ) = VT_BOOL;
  V_BOOL( &vFALSE ) = FALSE;

  CFileDialog dlg1( TRUE, strExtension, NULL, OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_HIDEREADONLY, strFilter, this );

  if( dlg1.DoModal() == IDOK )
  {
    POSITION pos = dlg1.GetStartPosition();
    while( pos )
    {
      CString PathName = dlg1.GetNextPathName( pos );
      _Application ap;
      COleException e;

      if( !ap.CreateDispatch( "Excel.Application", &e ) )
      {
        //Excel not available
        _m()->msg( "Excel needs to be installed in order to be able to import Excel format files.", MB_OK | MB_HELP );
        return;
      }

      Workbooks wbs;
      wbs.AttachDispatch( ap.GetWorkbooks() );

      // non-filled in optional parameters should have following format (according to MSDN)
      VARIANT vOptional;
      vOptional.vt = VT_ERROR;
      vOptional.scode = DISP_E_PARAMNOTFOUND;

      _Workbook wb;

      wb.AttachDispatch( wbs.Open( PathName, vOptional, vOptional, vOptional, vOptional, vOptional,
        vOptional, vOptional, vOptional, vOptional, vOptional, vOptional, vOptional ) );

      Worksheets wss;
      wss.AttachDispatch( wb.GetWorksheets() );

      int nWssCount = wss.GetCount();
      if( nWssCount < 1 )
      {
        _m()->msg( "Could not find any worksheets in the selected Excel file", MB_OK | MB_HELP );
        ap.Quit();
        return;
      }

      int *pSelectedIndices;
      int nSelectedIndices;

      if( nWssCount > 1 )
      {
        // let user select sheet
        CSelectWorksheetDlg dlg2;
    std::vector<CString> Strings;

    int i;
        for( i=0; i<nWssCount; i++ )
        {
          _Worksheet ws;
          ws.AttachDispatch( wss.GetItem( COleVariant( (short) ( i+1 ) ) ) );
          Strings.push_back( ws.GetName() );
        }

        dlg2.SetStrings( Strings );
        dlg2.SetTitle( FileNameFromPathName( PathName ) );

        if( dlg2.DoModal() != IDOK )
        {
          // user cancelled import
          ap.Quit();
          continue;
        }

        int *pSelected;

        dlg2.GetSelectedIndices( &pSelected, &nSelectedIndices );

        pSelectedIndices = (int *)malloc( nSelectedIndices * sizeof( int ) );
        for( i=0; i<nSelectedIndices; i++ ) pSelectedIndices[i] = pSelected[i];
      }
      else
      {
        pSelectedIndices = (int *)malloc( sizeof( int ) );
        nSelectedIndices = 1;
        pSelectedIndices[0] = 0;
      }

      for( int i=0; i<nSelectedIndices; i++ )
      {
        _Worksheet ws;
        ws.AttachDispatch( wss.GetItem( COleVariant( (short) ( pSelectedIndices[i] + 1 ) ) ) );
        CExperimentData ExpData;
        if( ExpData.ReadFromFile( ws, FileNameFromPathName( PathName ) + ", " + ws.GetName(), &m_Dlg ) )
        {
          m_vcExperimentFileName.push_back( FileNameFromPathName( PathName ) + ", " + ws.GetName() );
          m_vcExperimentData.push_back( ExpData );
        }
        else
        {
          // no valid file, notify user
          CString msg;
          msg.Format( "The file %s is not a valid experiment file. The file can not be used.", FileNameFromPathName( PathName ) );
          _m()->msg( msg, MB_OK | MB_HELP );
        }
      }
      free( pSelectedIndices );
      wb.Close(vFALSE, vNotPassed, vNotPassed);
      wbs.Close();
      ap.Quit();
    }
  }

  UpdateFileList();
//	SendMessageToDialog( WM_CLEARHISTORY );
  OnSelchangeListFiles();
  OnFileListUpdated();
}

void CTabExperiment::OnDelete() 
{
  int total = 0;
  int nSelected = 0;
  int* piSelected = 0;

  getSelectedItems(total, nSelected, &piSelected);

  // sort the items in piSelected top down
  int i;
  for( i=0; i<nSelected-1; ++i )
  {
    for( int j=i+1; j<nSelected; ++j )
    {
      if( piSelected[i] < piSelected[j] )
      {
        // swap
        int tmp = piSelected[i];
        piSelected[i] = piSelected[j];
        piSelected[j] = tmp;
      }
    }
  }

  // delete the items
  for( i=0; i<nSelected; ++i )
  {
    m_vcExperimentFileName.erase( m_vcExperimentFileName.begin() + piSelected[i] );
    m_vcExperimentData.erase( m_vcExperimentData.begin() + piSelected[i] );
  }

  m_Dlg.adjustCalculatedData(nSelected, piSelected);

  delete [] piSelected;

  UpdateFileList();
//	SendMessageToDialog( WM_CLEARHISTORY );
  OnSelchangeListFiles();
  OnFileListUpdated();
}

void CTabExperiment::OnSelchangeListFiles() 
{
  int total = 0;
  int nSelected = 0;
  int* piSelected = 0;

  getSelectedItems(total, nSelected, &piSelected);

  // enable delete button only if one or more items selected
  GetDlgItem( IDC_DELETE )->EnableWindow( nSelected > 0 );

  UpdatePictureRadios();
  UpdateWeightFactor();
  OnFileListUpdated();

  delete [] piSelected;
}

void CTabExperiment::OnGammasall() 
{
  m_iShowInPicture = 0;
  OnFileListUpdated();
}

void CTabExperiment::OnGammasselected() 
{
  m_iShowInPicture = 1;
  OnFileListUpdated();
}

void CTabExperiment::OnFileListUpdated()
{
  int nItems = 0;
  int nSelected = 0;
  int* piSelected = 0;

  getSelectedItems(nItems, nSelected, &piSelected);

  m_Dlg.OnExperimentFileListUpdated(nItems, nSelected, piSelected,
  m_bUseInitialLowerLimit, m_iInitialLowerLimit);

  delete [] piSelected;
}

void CTabExperiment::setInitialLowerLimit(
  bool useInitialLowerLimit, const std::vector <int>& initialLowerLimit)
{
  m_bUseInitialLowerLimit = useInitialLowerLimit;
  m_iInitialLowerLimit = initialLowerLimit;
}

void CTabExperiment::getSelectedItems(int& total, int& selected,
  int* items[]) const
{
  // A 'dynamic_cast' resolves to NULL, hence the 'old' C-style cast!

  CListBox* listBox = (CListBox*) GetDlgItem(IDC_LIST_FILES);

  total = listBox->GetCount();
  selected = listBox->GetSelCount();
  *items = new int[selected];

  listBox->GetSelItems(selected, *items);
}

CString CTabExperiment::FileNameFromPathName( const CString &strPathName ) const
{
  int len = strPathName.GetLength();
  for( int i=0; i<len; ++i )
  {
    if( strPathName[len-i-1] == '\\' )
    {
      return strPathName.Right( i );
    }
  }

  // no backslash found, return whole name
  return strPathName;
}

void CTabExperiment::UpdateFileList()
{
  CListBox *pListBox = (CListBox *) GetDlgItem( IDC_LIST_FILES );
  pListBox->ResetContent();

  for( int i=0; i<m_vcExperimentFileName.size(); ++i )
  {
    pListBox->AddString( m_vcExperimentFileName[i] );
  }
}

void CTabExperiment::UpdatePictureRadios()
{
  CListBox *pListBox = (CListBox *) GetDlgItem( IDC_LIST_FILES );

  int nItems = pListBox->GetCount();
  int nSelected = pListBox->GetSelCount();

  GetDlgItem( IDC_FRA_PICTURE )->EnableWindow( nItems > 0 );
  GetDlgItem( IDC_GAMMASALL )->EnableWindow( nItems > 0 );
  GetDlgItem( IDC_GAMMASSELECTED )->EnableWindow( nItems > 0 && nSelected > 0 );

  if( m_iShowInPicture == -1 && nItems > 0 ) m_iShowInPicture = 0;
  if( m_iShowInPicture == 1 && nSelected == 0 && nItems > 0 ) m_iShowInPicture = 0;
  if( ( m_iShowInPicture == 0 || m_iShowInPicture == 1 ) && nItems == 0 ) m_iShowInPicture = -1;

  m_Dlg.UpdatePQRadios(nItems > 0);
  UpdateData( FALSE );
}

void CTabExperiment::UpdateWeightFactor()
{
  int nItems = 0;
  int nSelected = 0;
  int* piSelected = 0;

  getSelectedItems(nItems, nSelected, &piSelected);

  CEdit *pEdWeight = (CEdit *) GetDlgItem( IDC_EDX_WEIGHT );
  CWnd *pLblWeight = GetDlgItem( IDC_LBL_WEIGHT );

  pLblWeight->EnableWindow( nSelected == 1 );
  BOOL bDisableEditX = ( nSelected != 1 );

    pEdWeight->SetReadOnly( bDisableEditX );

  if( nSelected == 1 )
  {
    assert( piSelected[0] < m_vcExperimentData.size() );
    char number[100];
    sprintf_s(number,"%f",
            m_vcExperimentData[piSelected[0]].WeightFactor());

    pEdWeight->SetWindowText( number );
    m_iCurrentSel = piSelected[0];
  }
  else
  {
    pEdWeight->SetWindowText( "" );
    m_iCurrentSel = -1;
  }

  delete [] piSelected;
}

