// wizzardpagebase.cpp : implementation file
//

#include "wizzardpagebase.h"
#include "GeomecStringTable.h"
#include "geomec.h"
#include "stdafx.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

CWizzardPageBase::CPageTreeItem::CPageTreeItem(CPropertyPage &page, CTreeCtrl &ctrl)
    : ITreeObject(ctrl, TVI_ROOT, TVI_LAST), m_page(page) {
  Ctrl().SetItemData(Handle(), (DWORD_PTR)this);
}

QString CWizzardPageBase::CPageTreeItem::Text() const {
  const CWizzardPageBase *pPage = dynamic_cast<const CWizzardPageBase *>(&m_page);
  assert(pPage);
  return (LPCSTR)pPage->Text();
}

unsigned int CWizzardPageBase::CPageTreeItem::Icon() const { return 0; }

BOOL CWizzardPageBase::CPageTreeItem::OnSelect() {
  CWizzardPageBase *pPage = dynamic_cast<CWizzardPageBase *>(&m_page);
  assert(pPage);
  if (pPage->Sheet().GetActivePage() == &m_page)
    return TRUE;

  pPage->Sheet().SetActivePage(&m_page);
  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CWizzardPageBase dialog
CWizzardPageBase::CWizzardPageBase(unsigned int uTemplateID, unsigned int uCaption)
    : CPropertyPage(uTemplateID, uCaption), m_uCaption(uCaption) {
  //	sheet.AddPage(this);
  //{{AFX_DATA_INIT(CWizzardPageBase)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}

void CWizzardPageBase::DoDataExchange(CDataExchange *pDX) {
  CPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CWizzardPageBase)
  DDX_Control(pDX, IDC_TC_OVERVIEW, m_lcOverview);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWizzardPageBase, CPropertyPage)
//{{AFX_MSG_MAP(CWizzardPageBase)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWizzardPageBase message handlers

BOOL CWizzardPageBase::OnSetActive() {
  BOOL bIsLast = FALSE;
  BOOL bIsFirst = FALSE;
  /*
    m_lcOverview.DeleteAllItems();
    for(int i = 0; i < Sheet().GetPageCount(); i++)
    {
      CPageTreeItem *pItem = new CPageTreeItem(*Sheet().GetPage(i), m_lcOverview);
      if(Sheet().GetPage(i) == this)
        m_lcOverview.SelectItem(pItem->Handle());

      if(i == 0 && (Sheet().GetPage(i) == this))
        bIsFirst = TRUE;

      if(i == (Sheet().GetPageCount() - 1) && (Sheet().GetPage(i) == this))
        bIsLast = TRUE;
    }
  */
  if (bIsFirst)
    Sheet().SetWizardButtons(PSWIZB_NEXT);
  else if (bIsLast)
    Sheet().SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
  else
    Sheet().SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

  return CPropertyPage::OnSetActive();
}

const CPropertySheet &CWizzardPageBase::Sheet() const { return *((CPropertySheet *)GetParent()); }

CPropertySheet &CWizzardPageBase::Sheet() { return *((CPropertySheet *)GetParent()); }

BOOL CWizzardPageBase::Enable() const { return TRUE; }

CString CWizzardPageBase::Text() const { return getStringTableEntry(m_uCaption); }