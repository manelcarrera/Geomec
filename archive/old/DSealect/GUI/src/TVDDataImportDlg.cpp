// D:\users\wedx\svn_trunk_dsealect\DSealect\GUI\src\TVDDataImportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DSealect.h"
#include "TVDDataImportDlg.h"

#include "ITVDDataReader.h"
#include "PreviewListObject.h"

// CTVDDataImportDlg dialog

IMPLEMENT_DYNAMIC(CTVDDataImportDlg, CDialog)

CTVDDataImportDlg::CTVDDataImportDlg(ITVDDataReader& reader, const CString& strFileName, CWnd* pParent)
: CDialog(CTVDDataImportDlg::IDD, pParent),
  m_reader(reader),
  m_strFileName(strFileName)
{
  Init();
}

CTVDDataImportDlg::~CTVDDataImportDlg()
{
}

BOOL CTVDDataImportDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  SetWindowText(m_reader.DialogCaption());
  GetDlgItem(IDC_LabelDataColumn)->SetWindowText(m_reader.DataColumnCaption());

  // fill unit combo
  CComboBox* pUnitCombo = (CComboBox*)GetDlgItem(IDC_ComboTVDUnit);
  pUnitCombo->AddString("m");
  pUnitCombo->AddString("ft");

  CComboBox* pDataUnitCombo = (CComboBox*)GetDlgItem(IDC_ComboDataUnit);
  std::vector<std::pair<int, CString> > vcUnits = m_reader.DataUnits();
  for(int i = 0; i < vcUnits.size(); ++i)
    pDataUnitCombo->AddString(vcUnits[i].second);

  // list control
  m_lcPreview.InsertColumn(0, "Row #", LVCFMT_LEFT, 50);
  for(int i = 0; i < m_reader.ImportInfo().MaxNumColumns() && i < MAX_COLUMNS; ++i)
    m_lcPreview.InsertColumn(i + 1, QString("Col. %1").arg(i + 1).toAscii().data(), LVCFMT_LEFT, 60);

  for(int i = 0; i < m_reader.ImportInfo().LineSize(); ++i)
    new CPreviewListObject(m_lcPreview, i+1, m_reader.ImportInfo().LineVec(i), m_reader.ImportInfo().MaxNumColumns());

  if(m_reader.ImportInfo().MaxNumColumns() > MAX_COLUMNS)
  {
    CString s;
    GetDlgItem(IDC_FramePreview)->GetWindowText(s);
    s += QString(" (Displaying maximum of %1 columns)").arg(MAX_COLUMNS).toAscii().data();
    GetDlgItem(IDC_FramePreview)->SetWindowText(s);
  }

  CSpinButtonCtrl* pSpin;
  pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SpinStartRow);
  pSpin->SetRange32(1, m_reader.ImportInfo().LineSize());
  pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SpinEndRow);
  pSpin->SetRange32(1, m_reader.ImportInfo().LineSize());
  pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SpinTVDColumn);
  pSpin->SetRange32(1, m_reader.ImportInfo().MaxNumColumns());
  pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SpinDataColumn);
  pSpin->SetRange32(1, m_reader.ImportInfo().MaxNumColumns());

  UpdateData(FALSE);

  return TRUE;
}

void CTVDDataImportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
  DDX_CBIndex(pDX, IDC_ComboTVDUnit, m_iTVDUnit);
  DDX_CBIndex(pDX, IDC_ComboDataUnit, m_iDataUnit);
  DDX_Text(pDX, IDC_EditStartRow, m_iStartRow);
  DDX_Text(pDX, IDC_EditEndRow, m_iEndRow);
  DDX_Text(pDX, IDC_EditTVDColumn, m_iTVDColumn);
  DDX_Text(pDX, IDC_EditDataColumn, m_iDataColumn);
  DDX_Text(pDX, IDC_EditFileName, m_strFileName);
  DDX_Control(pDX, IDC_ListPreview, m_lcPreview);

  if(pDX->m_bSaveAndValidate)
  {
    if(m_iStartRow <= 0 || m_iStartRow > m_iEndRow)
    {
      AfxMessageBox("Please set a valid start row");
      pDX->PrepareEditCtrl(IDC_EditStartRow);
      pDX->Fail();
    }

    if(m_iEndRow < m_iStartRow || m_iEndRow > m_reader.ImportInfo().LineSize())
    {
      AfxMessageBox("Please set a valid end row");
      pDX->PrepareEditCtrl(IDC_EditEndRow);
      pDX->Fail();
    }

    if(m_iTVDColumn <= 0 || m_iTVDColumn > m_reader.ImportInfo().MaxNumColumns())
    {
      AfxMessageBox("Please set a valid column");
      pDX->PrepareEditCtrl(IDC_EditTVDColumn);
      pDX->Fail();
    }

    if(m_iTVDUnit == -1)
    {
      AfxMessageBox("Please select a unit");
      pDX->PrepareCtrl(IDC_ComboTVDUnit);
      pDX->Fail();
    }

    if(m_iDataUnit == -1)
    {
      AfxMessageBox("Please select a unit");
      pDX->PrepareEditCtrl(IDC_ComboDataUnit);
      pDX->Fail();
    }

    if(m_iDataColumn <= 0 || m_iDataColumn > m_reader.ImportInfo().MaxNumColumns())
    {
      AfxMessageBox("Please set a valid column");
      pDX->PrepareEditCtrl(IDC_EditDataColumn);
      pDX->Fail();
    }

    if(m_iTVDColumn == m_iDataColumn)
    {
      AfxMessageBox("Cannot use the same column more than once");
      pDX->Fail();
    }

    int mincols = m_reader.ImportInfo().LineVec(m_iStartRow - 1).size();
    for(int i = m_iStartRow; i < m_iEndRow; ++i)
      mincols = min(mincols, m_reader.ImportInfo().LineVec(i).size());

    if(m_iTVDColumn > mincols)
    {
      AfxMessageBox("The selected TVD column is not available in all rows between start and end");
      pDX->PrepareEditCtrl(IDC_EditMDColumn);
      pDX->Fail();
    }

    if(m_iDataColumn > mincols)
    {
      CString msg;
      msg.Format("The selected %s column is not available in all rows between start and end", m_reader.DataColumnCaption());
      AfxMessageBox(msg);
      pDX->PrepareEditCtrl(IDC_EditInclinationColumn);
      pDX->Fail();
    }
  }
}

void CTVDDataImportDlg::OnOK()
{
  if(UpdateData())
  {
    CImportInfo::TUnitID unitID = (m_iTVDUnit == 0 ? ITVDDataReader::UN_M : ITVDDataReader::UN_FT);
    m_reader.ImportInfo().SetUnit(ITVDDataReader::TAG_TVD, unitID);

    std::vector<std::pair<int, CString> > vcUnits = m_reader.DataUnits();
    assert(m_iDataUnit >= 0 && m_iDataUnit < vcUnits.size());
    m_reader.ImportInfo().SetUnit(ITVDDataReader::TAG_DATA, vcUnits[m_iDataUnit].first);

    m_reader.ImportInfo().DataStart(m_iStartRow - 1);
    m_reader.ImportInfo().DataEnd(m_iEndRow - 1);
    m_reader.ImportInfo().SetColumnNumber(ITVDDataReader::TAG_TVD, m_iTVDColumn - 1);
    m_reader.ImportInfo().SetColumnNumber(ITVDDataReader::TAG_DATA, m_iDataColumn - 1);

  	EndDialog(IDOK);
  }
}

void CTVDDataImportDlg::Init()
{
  m_iTVDUnit = (m_reader.ImportInfo().GetUnit(ITVDDataReader::TAG_TVD) == ITVDDataReader::UN_M ? 0 : 1);

  std::vector<std::pair<int, CString> > vcUnits = m_reader.DataUnits();
  m_iDataUnit = 0;
  for(int i = 0; i < vcUnits.size(); ++i)
  {
    if(m_reader.ImportInfo().GetUnit(ITVDDataReader::TAG_DATA) == vcUnits[i].first)
    {
      m_iDataUnit = i;
      break;
    }
  }

  m_iStartRow = m_reader.ImportInfo().DataStart() + 1;
  m_iEndRow = m_reader.ImportInfo().DataEnd() + 1;
  m_iTVDColumn = m_reader.ImportInfo().GetColumnNumber(ITVDDataReader::TAG_TVD) + 1;
  m_iDataColumn = m_reader.ImportInfo().GetColumnNumber(ITVDDataReader::TAG_DATA) + 1;
}

BEGIN_MESSAGE_MAP(CTVDDataImportDlg, CDialog)
  ON_BN_CLICKED(IDC_ButtonReset, &CTVDDataImportDlg::OnReset)
END_MESSAGE_MAP()

void CTVDDataImportDlg::OnReset()
{
  Init();
  UpdateData(FALSE);
}
