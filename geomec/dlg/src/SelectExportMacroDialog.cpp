// D:\users\skpx\model-view\geomec3\SRC\SelectExportMacroDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Geomec.h"
#include "SelectExportMacroDialog.h"

// CSelectExportMacroDialog dialog

IMPLEMENT_DYNAMIC(CSelectExportMacroDialog, CDialog)

CSelectExportMacroDialog::CSelectExportMacroDialog(
  CExportResultData** exportResultData,
  const CGraphEntry::TEntryNodeSet& entryNodeSet, CWnd* pParent /*=NULL*/)
  : CDialog(CSelectExportMacroDialog::IDD, pParent)
  , m_exportResultData(exportResultData)
  , m_entryNodeSet(entryNodeSet)
{
}

CSelectExportMacroDialog::~CSelectExportMacroDialog()
{
}

void CSelectExportMacroDialog::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_SELECT_EXPORT_MACRO_LIST, m_selectExportMacrosList);
}


BEGIN_MESSAGE_MAP(CSelectExportMacroDialog, CDialog)
  ON_LBN_DBLCLK(IDC_SELECT_EXPORT_MACRO_LIST, &CSelectExportMacroDialog::OnLbnDblclkSelectExportMacroList)
  ON_LBN_SELCHANGE(IDC_SELECT_EXPORT_MACRO_LIST, &CSelectExportMacroDialog::OnLbnSelchangeSelectExportMacroList)
  ON_LBN_SELCANCEL(IDC_SELECT_EXPORT_MACRO_LIST, &CSelectExportMacroDialog::OnLbnSelcancelSelectExportMacroList)
END_MESSAGE_MAP()


// CSelectExportMacroDialog message handlers

BOOL CSelectExportMacroDialog::OnInitDialog()
{
  CDialog::OnInitDialog();

  for (CGraphEntry::TEntryNodeSet::const_iterator iterator =
    m_entryNodeSet.begin(); iterator != m_entryNodeSet.end(); ++iterator)
  {
    int index = m_selectExportMacrosList.AddString(
      (*iterator)->Name().toStdString().c_str());

    m_selectExportMacrosList.SetItemData(index, (DWORD_PTR) *iterator);
  }

  OnLbnSelcancelSelectExportMacroList();

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectExportMacroDialog::OnLbnDblclkSelectExportMacroList()
{
  retrieveExportResultData();

  EndDialog(IDOK);
}

void CSelectExportMacroDialog::OnLbnSelchangeSelectExportMacroList()
{
  retrieveExportResultData();

  GetDlgItem(IDOK)->EnableWindow(TRUE);
}

void CSelectExportMacroDialog::OnLbnSelcancelSelectExportMacroList()
{
  *m_exportResultData = 0;

  GetDlgItem(IDOK)->EnableWindow(FALSE);
}

// private

void CSelectExportMacroDialog::retrieveExportResultData()
{
  int item = m_selectExportMacrosList.GetCurSel();

  assert(item != LB_ERR);

  DWORD_PTR itemData = m_selectExportMacrosList.GetItemData(item);

  *m_exportResultData = (CExportResultData*) itemData;

  assert(m_exportResultData != 0);
}
