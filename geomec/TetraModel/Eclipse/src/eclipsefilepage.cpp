// eclipsefilepage.cpp : implementation file
//

#include "stdafx.h"
#include "geomec.h"
#include "eclipsefilepage.h"
#include "TNOFileDialog.h"
#include "resourceIDS.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CEclipseFilePage property page

IMPLEMENT_DYNCREATE(CEclipseFilePage, CPropertyPage)

CEclipseFilePage::CEclipseFilePage()
: CWizzardPageBase(IDD_ECLIPSE_FILE, IDS_ECLIPSE_FILE)
{
}

CEclipseFilePage::CEclipseFilePage(CEclipseModel& model) : CWizzardPageBase(IDD_ECLIPSE_FILE, IDS_ECLIPSE_FILE)
{
  //{{AFX_DATA_INIT(CEclipseFilePage)
  m_sPath = _T("");
  //}}AFX_DATA_INIT
}

CEclipseFilePage::~CEclipseFilePage()
{
}

void CEclipseFilePage::DoDataExchange(CDataExchange* pDX)
{
  CWizzardPageBase::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CEclipseFilePage)
  DDX_Text(pDX, IDC_ED_PATH, m_sPath);
  DDV_MaxChars(pDX, m_sPath, 256);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEclipseFilePage, CWizzardPageBase)
  //{{AFX_MSG_MAP(CEclipseFilePage)
  ON_BN_CLICKED(IDC_BT_BROWSE, OnBrowse)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEclipseFilePage message handlers

void CEclipseFilePage::OnBrowse() 
{
  // Start the file browser and load the eclipse file
  CTnoFileDialog file_dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Eclipse Files (*.grid;*.ecl)|*.grid; *.ecl|All Files (*.*)|*.*||");
  if(file_dlg.DoModal() == IDOK)
  {
    // Make path equal to path
    m_sPath = file_dlg.GetPathName();
    UpdateData(FALSE);
  }
  
}
