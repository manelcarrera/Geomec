// D:\users\wedx\svn_trunk\DSealect\GUI\src\ImportSurveyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DSealect.h"
#include "ImportSurveyDlg.h"

#include "SurveyReader.h"
#include "PreviewListObject.h"

IMPLEMENT_DYNAMIC(CImportSurveyDlg, CDialog)

CImportSurveyDlg::CImportSurveyDlg(CImportInfo& ImportInfo, const CString& strFileName, CWnd* pParent)
: CDialog(CImportSurveyDlg::IDD, pParent),
  m_ImportInfo(ImportInfo),
  m_strFileName(strFileName)
{
  Init();
}

CImportSurveyDlg::~CImportSurveyDlg()
{
}

void CImportSurveyDlg::Init()
{
  m_iMDUnit = (m_ImportInfo.GetUnit(CSurveyReader::TAG_MD) == CSurveyReader::UN_M ? 0 : 1);
  m_iStartRow = m_ImportInfo.DataStart() + 1;
  m_iEndRow = m_ImportInfo.DataEnd() + 1;
  m_iMDColumn = m_ImportInfo.GetColumnNumber(CSurveyReader::TAG_MD) + 1;
  m_iInclinationColumn = m_ImportInfo.GetColumnNumber(CSurveyReader::TAG_INC) + 1;
  m_iAzimuthColumn = m_ImportInfo.GetColumnNumber(CSurveyReader::TAG_AZI) + 1;
}

BOOL CImportSurveyDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  // fill unit combo
  CComboBox* pUnitCombo = (CComboBox*)GetDlgItem(IDC_ComboMDUnit);
  pUnitCombo->AddString("m");
  pUnitCombo->AddString("ft");

  // list control
  m_lcPreview.InsertColumn(0, "Row #", LVCFMT_LEFT, 50);
  for(int i = 0; i < m_ImportInfo.MaxNumColumns() && i < MAX_COLUMNS; ++i)
    m_lcPreview.InsertColumn(i + 1, QString("Col. %1").arg(i + 1).toAscii().data(), LVCFMT_LEFT, 60);

  for(int i = 0; i < m_ImportInfo.LineSize(); ++i)
    new CPreviewListObject(m_lcPreview, i+1, m_ImportInfo.LineVec(i), m_ImportInfo.MaxNumColumns());

  if(m_ImportInfo.MaxNumColumns() > MAX_COLUMNS)
  {
    CString s;
    GetDlgItem(IDC_FramePreview)->GetWindowText(s);
    s += QString(" (Displaying maximum of %1 columns)").arg(MAX_COLUMNS).toAscii().data();
    GetDlgItem(IDC_FramePreview)->SetWindowText(s);
  }

  CSpinButtonCtrl* pSpin;
  pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SpinStartRow);
  pSpin->SetRange32(1, m_ImportInfo.LineSize());
  pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SpinEndRow);
  pSpin->SetRange32(1, m_ImportInfo.LineSize());
  pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SpinMDColumn);
  pSpin->SetRange32(1, m_ImportInfo.MaxNumColumns());
  pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SpinInclinationColumn);
  pSpin->SetRange32(1, m_ImportInfo.MaxNumColumns());
  pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SpinAzimuthColumn);
  pSpin->SetRange32(1, m_ImportInfo.MaxNumColumns());

  UpdateData(FALSE);

  return TRUE;
}

void CImportSurveyDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_CBIndex(pDX, IDC_ComboMDUnit, m_iMDUnit);
  DDX_Text(pDX, IDC_EditStartRow, m_iStartRow);
  DDX_Text(pDX, IDC_EditEndRow, m_iEndRow);
  DDX_Text(pDX, IDC_EditMDColumn, m_iMDColumn);
  DDX_Text(pDX, IDC_EditInclinationColumn, m_iInclinationColumn);
  DDX_Text(pDX, IDC_EditAzimuthColumn, m_iAzimuthColumn);
  DDX_Text(pDX, IDC_EditFileName, m_strFileName);
  DDX_Control(pDX, IDC_ListPreview, m_lcPreview);

  if(pDX->m_bSaveAndValidate)
  {
    if(m_iStartRow <= 0 || m_iStartRow >= m_iEndRow)
    {
      AfxMessageBox("Please set a valid start row");
      pDX->PrepareEditCtrl(IDC_EditStartRow);
      pDX->Fail();
    }

    if(m_iEndRow <= m_iStartRow || m_iEndRow > m_ImportInfo.LineSize())
    {
      AfxMessageBox("Please set a valid end row");
      pDX->PrepareEditCtrl(IDC_EditEndRow);
      pDX->Fail();
    }

    if(m_iMDColumn <= 0 || m_iMDColumn > m_ImportInfo.MaxNumColumns())
    {
      AfxMessageBox("Please set a valid column");
      pDX->PrepareEditCtrl(IDC_EditMDColumn);
      pDX->Fail();
    }

    if(m_iMDUnit == -1)
    {
      AfxMessageBox("Please select a unit");
      pDX->PrepareCtrl(IDC_ComboMDUnit);
      pDX->Fail();
    }

    if(m_iInclinationColumn <= 0 || m_iInclinationColumn > m_ImportInfo.MaxNumColumns())
    {
      AfxMessageBox("Please set a valid column");
      pDX->PrepareEditCtrl(IDC_EditInclinationColumn);
      pDX->Fail();
    }

    if(m_iAzimuthColumn <= 0 || m_iAzimuthColumn > m_ImportInfo.MaxNumColumns())
    {
      AfxMessageBox("Please set a valid column");
      pDX->PrepareEditCtrl(IDC_EditAzimuthColumn);
      pDX->Fail();
    }

    if(m_iMDColumn          == m_iAzimuthColumn     ||
       m_iAzimuthColumn     == m_iInclinationColumn ||
       m_iInclinationColumn == m_iMDColumn)
    {
      AfxMessageBox("Cannot use the same column more than once");
      pDX->Fail();
    }

    int mincols = m_ImportInfo.LineVec(m_iStartRow - 1).size();
    for(int i = m_iStartRow; i < m_iEndRow; ++i)
      mincols = min(mincols, m_ImportInfo.LineVec(i).size());

    if(m_iMDColumn > mincols)
    {
      AfxMessageBox("The selected MD column is not available in all rows between start and end");
      pDX->PrepareEditCtrl(IDC_EditMDColumn);
      pDX->Fail();
    }

    if(m_iInclinationColumn > mincols)
    {
      AfxMessageBox("The selected inclination column is not available in all rows between start and end");
      pDX->PrepareEditCtrl(IDC_EditInclinationColumn);
      pDX->Fail();
    }

    if(m_iAzimuthColumn > mincols)
    {
      AfxMessageBox("The selected azimuth column is not available in all rows between start and end");
      pDX->PrepareEditCtrl(IDC_EditAzimuthColumn);
      pDX->Fail();
    }
  }
}

void CImportSurveyDlg::OnOK()
{
  if(UpdateData())
  {
    CImportInfo::TUnitID unitID = (m_iMDUnit == 0 ? CSurveyReader::UN_M : CSurveyReader::UN_FT);
    m_ImportInfo.SetUnit(CSurveyReader::TAG_MD, unitID);
    m_ImportInfo.DataStart(m_iStartRow - 1);
    m_ImportInfo.DataEnd(m_iEndRow - 1);
    m_ImportInfo.SetColumnNumber(CSurveyReader::TAG_MD, m_iMDColumn - 1);
    m_ImportInfo.SetColumnNumber(CSurveyReader::TAG_INC, m_iInclinationColumn - 1);
    m_ImportInfo.SetColumnNumber(CSurveyReader::TAG_AZI, m_iAzimuthColumn - 1);

  	EndDialog(IDOK);
  }
}


BEGIN_MESSAGE_MAP(CImportSurveyDlg, CDialog)
  ON_BN_CLICKED(IDC_ButtonReset, &CImportSurveyDlg::OnReset)
END_MESSAGE_MAP()


// CImportSurveyDlg message handlers

void CImportSurveyDlg::OnReset()
{
  Init();
  UpdateData(FALSE);
}
