// eclipsehorizonpage.cpp : implementation file
//

#include "eclipsehorizonpage.h"
#include "geomec.h"
#include "resourceIDS.h"
#include "stdafx.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CEclipseHorizonPage dialog
IMPLEMENT_DYNCREATE(CEclipseHorizonPage, CPropertyPage)

CEclipseHorizonPage::CEclipseHorizonPage() : CWizzardPageBase(IDD_ECLIPSE_HORIZON, IDS_ECLIPSE_HORIZON) {}

CEclipseHorizonPage::CEclipseHorizonPage(CEclipseModel &model)
    : CWizzardPageBase(IDD_ECLIPSE_HORIZON, IDS_ECLIPSE_HORIZON) {
  //{{AFX_DATA_INIT(CEclipseHorizonPage)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}

void CEclipseHorizonPage::DoDataExchange(CDataExchange *pDX) {
  CWizzardPageBase::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CEclipseHorizonPage)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEclipseHorizonPage, CWizzardPageBase)
//{{AFX_MSG_MAP(CEclipseHorizonPage)
// NOTE: the ClassWizard will add message map macros here
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEclipseHorizonPage message handlers
