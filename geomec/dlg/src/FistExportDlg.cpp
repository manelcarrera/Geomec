// FistExportDlg.cpp : implementation file
//
#include "stdafx.h"
#include "geomec.h"
#include "FistExportDlg.h"
#include "NewWellPath.h"
#include "modelbase.h"
#include "BaseEntryTypes.h"
#include "monthcaldlg.h"
#include "TNOFileDialog.h"
#include "fistexport.h"
#include "ResultRegister.h"
#include "FemAppMainWindow.h"
#include "GlobalMessage.h"

#include <qfile.h>

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CFistExportDlg dialog


CFistExportDlg::CFistExportDlg(CWnd* pParent /*=NULL*/)
: CDialog(CFistExportDlg::IDD, pParent)
{
//{{AFX_DATA_INIT(CFistExportDlg)
  // NOTE: the ClassWizard will add member initialization here
//}}AFX_DATA_INIT
}

CFistExportDlg::CFistExportDlg
( const CNewWellPath *pWellPath
, const CModelBase *pModel
, CWnd* pParent /*=NULL*/)
: CDialog(CFistExportDlg::IDD, pParent)
, m_pNewWellPath(pWellPath)
, m_pModel(pModel)
, m_bNewWellPaths(false) // don't care
{ // export all wells for which fist export points are selected
  // Construct
}

CFistExportDlg::CFistExportDlg
( bool bNewWellPaths
, const CModelBase *pModel
, CWnd* pParent /*=NULL*/)
: CDialog(CFistExportDlg::IDD, pParent)
, m_pNewWellPath(0)
, m_pModel(pModel)
, m_bNewWellPaths(bNewWellPaths)
{ // export all wells for which fist export points are selected
  // Construct
}

void CFistExportDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CFistExportDlg)
  DDX_Control(pDX, IDC_BTN_RMV_DATE, m_BtnRmvDate);
  DDX_Control(pDX, ID_EXPORT, m_BtnExport);
  DDX_Control(pDX, IDC_BTN_ADD_DATE, m_BtnAddDate);
  DDX_Control(pDX, IDC_DATE_LIST, m_ListDateCtrl);
  DDX_Control(pDX, IDC_POINT_LIST, m_ListPtCtrl);
  //}}AFX_DATA_MAP

  if(!pDX->m_bSaveAndValidate)
  {
  UpdateDateList();
  if ( 
    (m_pNewWellPath != 0 && m_NewDateList.size() == 0)
       )
      m_BtnExport.EnableWindow(FALSE);
  else
      m_BtnExport.EnableWindow(TRUE);

  if(m_ListDateCtrl.GetSelectedCount() == 0)
  m_BtnRmvDate.EnableWindow(FALSE);

  }
}


BEGIN_MESSAGE_MAP(CFistExportDlg, CDialog)
//{{AFX_MSG_MAP(CFistExportDlg)
  ON_BN_CLICKED(IDC_BTN_ADD_DATE, OnBtnAddDate)
  ON_BN_CLICKED(IDC_BTN_INC_STAGES, OnBtnIncStages)
  ON_BN_CLICKED(IDC_BTN_RMV_DATE, OnBtnRmvDate)
  ON_NOTIFY(LVN_ITEMCHANGED, IDC_DATE_LIST, OnItemchangedDateList)
  ON_BN_CLICKED(ID_EXPORT, OnExport)
  ON_BN_CLICKED(ID_CLOSE, OnClose)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFistExportDlg message handlers

BOOL CFistExportDlg::OnInitDialog() 
{
  //	m_well_list.setAutoDelete(true);
  //	m_date_list.AutoDelete(false);
  CDialog::OnInitDialog();

  CRect rect;
  m_ListPtCtrl.GetClientRect(rect);
  m_ListPtCtrl.InsertColumn(0, "TMD (m)", LVCFMT_LEFT, rect.Width() / 3);
  m_ListPtCtrl.InsertColumn(1, "Formation", LVCFMT_LEFT, rect.Width() / 3);
  m_ListPtCtrl.InsertColumn(2, "WellPath", LVCFMT_LEFT, rect.Width() / 3);

  m_ListDateCtrl.GetClientRect(rect);
  m_ListDateCtrl.InsertColumn(0, "Date", LVCFMT_CENTER, rect.Width());

  if(m_pNewWellPath)
  {
  m_NewWellList.push_front(m_pNewWellPath);
  const std::list<const QDate *> &fd= m_pNewWellPath->FistDates();
  std::list<const QDate *>::const_iterator it;
  for (it= fd.begin(); it != fd.end(); ++it)
  {
      m_NewDateList.push_front(new QDate(**it));
  }

  m_NewDateList.sort(::less);
  m_NewDateList.unique(::equal);
  }
  else if ( m_bNewWellPaths) // all new wellpaths
  {
  assert(m_pModel);

  CNewWellPathEntry::TNodeSet new_well_paths =
      ((CNewWellPathEntry*)m_pModel->GraphEntry(MD_NEW_WELLPATH))->EntryNodes();
  for(CNewWellPathEntry::TNodeSet::iterator it = new_well_paths.begin(); it != new_well_paths.end(); ++it)
  {
      m_NewWellList.push_front(*it);
      const std::list<const QDate *> &fd= (*it)->FistDates();
      std::list<const QDate *>::const_iterator it_date;
      for (it_date= fd.begin(); it_date != fd.end(); ++it_date)
      {
    m_NewDateList.push_front(new QDate(**it_date));
      }

      m_NewDateList.sort(::less);
      m_NewDateList.unique(::equal);
  }
  }
  else // all old wellpaths
  {
  assert(false);
  }

  UpdatePointList();
  UpdateDateList();
  UpdateData(FALSE);

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

void CFistExportDlg::OnBtnAddDate() 
{
  QDate init_stage;

  QDate first;
  QDate last;

  first.setDate(m_pModel->InitialDepletionStage().Time().GetYear(),
  m_pModel->InitialDepletionStage().Time().GetMonth(),
  1);

  const CDepletionStage* pLastStage = &m_pModel->LastDepletionStage();

  CAnalysisType antype(m_pModel->ResultRegister().NonLinear() ? CAnalysisType::AT_NONLIN : CAnalysisType::AT_LINEAR);
  while(pLastStage && !m_pModel->ResultRegister().ResultsAvailable(antype, *pLastStage))
  {
  if(pLastStage->Initial())
  pLastStage = 0;
  else
  pLastStage = &pLastStage->Previous();
  }

  if(pLastStage == 0)
  {
  _m()->msg("No results found, cannot add dates");
  return;
  }

  last.setDate(pLastStage->Time().GetYear(),
  pLastStage->Time().GetMonth(),
  1);

  CRect rect;
  m_BtnAddDate.GetWindowRect(&rect);
  CPoint ptScreen(rect.CenterPoint());
  CMonthCalDlg dlg(COleDateTime(first.year(), first.month(), first.day(), 0, 0, 0), ptScreen, this);

  QDate *date = 0;
  if(dlg.DoModal() == IDOK)
  {
  date = new QDate(dlg.DateTime().GetYear(), dlg.DateTime().GetMonth(), dlg.DateTime().GetDay());
  if(*date < first || *date > last)
  {
      _m()->msg(QString("New dates cannot be inserted before the initial depletion stage, " + first.toString("dd MM yyyy") + ",\n" + " or after the last calculated depletion stage, " + last.toString("dd MM yyyy") +"."), MB_OK|MB_ICONINFORMATION);
      delete date;
      return;
  }

  {
      m_NewDateList.push_front(date);
      m_NewDateList.sort(::less);
      m_NewDateList.unique(::equal);
      UpdateData(FALSE);
  }
  }
}

void CFistExportDlg::OnBtnIncStages() 
{
  CDepletionStage *pStage = const_cast<CDepletionStage*>(&m_pModel->InitialDepletionStage());
  QDate *date = 0;

  while(pStage)
  {
  CAnalysisType antype = (m_pModel->ResultRegister().NonLinear() ? CAnalysisType::AT_NONLIN : CAnalysisType::AT_LINEAR);
  if(m_pModel->ResultRegister().ResultsAvailable(antype, *pStage))
  {
      date = new QDate(pStage->Time().GetYear(), pStage->Time().GetMonth(), 1);
      {
    m_NewDateList.push_front(date);
    m_NewDateList.sort(::less);
    m_NewDateList.unique(::equal);
      }
  }

  if(!pStage->Last())
  pStage = &pStage->Next();
  else 
  pStage = 0;
  }

  UpdateData(FALSE);

}

void CFistExportDlg::OnBtnRmvDate() 
{

  POSITION pos = m_ListDateCtrl.GetFirstSelectedItemPosition();

  std::list<const QDate *> m_DatesToRemove; // NewWellPath

  while (pos)
  {
  int nItem = m_ListDateCtrl.GetNextSelectedItem(pos);
  {
      std::list<const QDate *>::iterator it= m_NewDateList.begin();

      for ( int jj= 0
          ; jj < m_NewDateList.size() && it!= m_NewDateList.end()
          ; ++jj
          )
      {
    if ( nItem == jj )
    {
          m_DatesToRemove.push_back( *it);
          break;
    }
    ++it;
      }
  }
  }

  {
  std::list<const QDate *>::iterator it;
  for ( it= m_DatesToRemove.begin()
    ; it != m_DatesToRemove.end()
    ; ++it
    )
  {
      m_NewDateList.remove( *it);
  }
  }


  UpdateData(FALSE);


}

void CFistExportDlg::OnCancel() 
{
  // TODO: Add extra cleanup here
  m_NewDateList.clear();

  CDialog::OnCancel();
}

void CFistExportDlg::UpdateDateList()
{
  QString str;
  int j = 0;
  int nIndex;
  m_ListDateCtrl.DeleteAllItems();
  {
  std::list<const QDate *>::iterator it;

  for ( it= m_NewDateList.begin()
    ; it != m_NewDateList.end()
    ; ++it
    )
  {
      const QDate *pDate = *it;
      nIndex = m_ListDateCtrl.InsertItem(j++, "");
      str = pDate->toString("dd MM yyyy");
      m_ListDateCtrl.SetItemText(nIndex, 0, str.toStdString().c_str());
  }
  }
}

void CFistExportDlg::UpdatePointList()
{
  int iColom, nIndex;
  CString strValue;
  std::list<const CNewWellPath*>::iterator it_wpath;

  for ( it_wpath= m_NewWellList.begin(); it_wpath != m_NewWellList.end(); ++it_wpath )
  {
  const CNewWellPath *pWellPath = *it_wpath;
  const std::list<CNewWellPoint> &list= pWellPath->FistExportPoints();
  std::list<CNewWellPoint>::const_iterator it_wpoint;
  int jj= 0;
  for ( it_wpoint= list.begin(); it_wpoint!=list.end(); ++jj, ++it_wpoint)
  {
      iColom = 0;
      nIndex = m_ListPtCtrl.InsertItem(jj, "");

      strValue.Format("%f", (*it_wpoint).TMD());
      m_ListPtCtrl.SetItemText(nIndex, iColom++, strValue);


      CNewGeoWellPoint gwp( *const_cast<CModelBase *>(m_pModel), *it_wpoint);
      const CFormationBase *fbase= gwp.Formation();
      if (fbase)
    strValue.Format( fbase->Name().toStdString().c_str());
      else
    strValue.Format( "-");

      m_ListPtCtrl.SetItemText(nIndex, iColom++, strValue);

      strValue.Format(pWellPath->Name().toStdString().c_str());
      m_ListPtCtrl.SetItemText(nIndex, iColom++, strValue);
  }
  }
}

void CFistExportDlg::OnItemchangedDateList(NMHDR* pNMHDR, LRESULT* pResult) 
{
  NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
  // TODO: Add your control notification handler code here
  //UpdateData(FALSE);
  if(m_ListDateCtrl.GetSelectedCount() != 0)
  m_BtnRmvDate.EnableWindow(TRUE);
  else
  m_BtnRmvDate.EnableWindow(FALSE);

  *pResult = 0;
}

void CFistExportDlg::AssignDatesToWellPath() const
{
  if ( 
       !( m_bNewWellPaths==false && m_pNewWellPath==0 )
     )
  {
  std::list<const CNewWellPath*>::const_iterator it;
  for ( it= m_NewWellList.begin(); it!= m_NewWellList.end(); ++it)
  {
      CNewWellPath *pPath= const_cast<CNewWellPath *>(*it);
      pPath->FistDates().clear();
      std::list<const QDate *>::const_iterator it2;
      for ( it2= m_NewDateList.begin()
          ; it2!= m_NewDateList.end()
          ; ++it2
          )
      {
    pPath->FistDates().push_back(new QDate(**it2));
    pPath->FistDates().sort(::less);
    pPath->FistDates().unique(::equal);
      }
  }
  }
}

void CFistExportDlg::OnExport() 
{
  CString file_types;
  file_types.Format("FIST (*.fxi)|*.fxi|All Files (*.*)|*.*");


  //CTnoFileDialog dlg(FALSE, "fxi", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ENABLETEMPLATE/*|OFN_EXPLORER *//*| OFN_ENABLETEMPLATEHANDLE*/, (LPCSTR)(file_types), FemAppGetMainWnd());
  CTnoFileDialog dlg(FALSE, "fxi", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, (LPCSTR)(file_types), FemAppGetMainWnd());

  dlg.m_ofn.lpstrTitle = "Export to FIST";
  /*dlg.m_ofn.lpTemplateName = MAKEINTRESOURCE(ID_EXPORT_DLG);//MAKEINTRESOURCE("EXPORT");//MAKEINTRESOURCE("NEWFILEOPENORD");
  dlg.m_ofn.hInstance = AfxGetInstanceHandle(); 
  */

  AssignDatesToWellPath();

  if(dlg.DoModal() == IDOK)
  {
  CWaitCursor wait;
  CString newName = dlg.GetPathName();
  CString Title = dlg.GetFileTitle();

  QFile file(QString((LPCSTR)(newName)));
  file.open(QIODevice::WriteOnly);
  QTextStream stream(&file);

  {
      CFistExport fexport((const CModelBase*)(m_pModel),0, m_pNewWellPath);	
      fexport.Save(stream);
  }
  file.close();

  }

  DWORD error = CommDlgExtendedError();

}

void CFistExportDlg::OnClose() 
{
  AssignDatesToWellPath();
  EndDialog(ID_CLOSE);

}
