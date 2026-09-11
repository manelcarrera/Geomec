// WellPointListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WellPointListDlg.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CWellPointListDlg dialog


CWellPointListDlg::CWellPointListDlg
( well::CWellPathBase& wellpath
, CDoubleQuantity::UNIT unit
, CWnd* pParent /*=NULL*/
, QVector<well::CWellSectionList*>* pSections /*NULL*/
)
: CDialog(CWellPointListDlg::IDD, pParent)
, m_WellPointListCtrl(wellpath)
{
  if(pSections)
    m_Sections = *pSections;

  m_pWellPath=&wellpath;
  m_Unit=unit;
  //{{AFX_DATA_INIT(CWellPointListDlg)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}


void CWellPointListDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);

  DDX_Control(pDX, UT_IDC_LIST_WELLLIST, m_WellPointListCtrl);

  //{{AFX_DATA_MAP(CWellPointListDlg)
    // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWellPointListDlg, CDialog)
  //{{AFX_MSG_MAP(CWellPointListDlg)
  ON_CBN_EDITCHANGE(UT_IDC_COMBO_LISTS, OnEditchangeComboLists)
  ON_CBN_SELCHANGE(UT_IDC_COMBO_LISTS, OnSelchangeComboLists)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWellPointListDlg message handlers

BOOL CWellPointListDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();
  CRect rect;

  CComboBox* pCombo =(CComboBox*)GetDlgItem(UT_IDC_COMBO_LISTS);
  

  QList<int> list = m_pWellPath->GetAllPointListID(); 

  if(list.count() < 1)
    return TRUE;

  pCombo->ResetContent();
  for(int i=0; i< list.count();i++)
  {
    pCombo->InsertString(i,m_pWellPath->GetWellPointList(list[i]).GetDescription().toStdString().c_str());
    pCombo->SetItemDataPtr(i,(void*)&m_pWellPath->GetWellPointList(list[i]));
    
  }

  pCombo->InsertString(list.count(),m_pWellPath->DefPointList().GetDescription().toStdString().c_str());
  pCombo->SetItemDataPtr(list.count(),&m_pWellPath->DefPointList());
  

  m_pList = &m_pWellPath->GetWellPointList(list[0]);
  pCombo->SetCurSel(0);
  m_WellPointListCtrl.OnInitList(m_pList,m_Sections,m_Unit);
  m_WellPointListCtrl.UpdateList();
  
  
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CWellPointListDlg::OnEditchangeComboLists() 
{
  // TODO: Add your control notification handler code here
  
}

void CWellPointListDlg::OnSelchangeComboLists() 
{
  CComboBox* pCombo =(CComboBox*)GetDlgItem(UT_IDC_COMBO_LISTS);
  int nIndex = pCombo->GetCurSel();
  m_pList = (well::CWellPointList*)pCombo->GetItemDataPtr(nIndex);
  m_WellPointListCtrl.OnInitList(m_pList,m_Sections,m_Unit);
  m_WellPointListCtrl.UpdateList();
}
