// dexdepthtypedlg.cpp : implementation file
//

#include "stdafx.h"
#include "wellschemeutils.h"
#include "dexdepthtypedlg.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CDexDepthTypeDlg dialog


CDexDepthTypeDlg::CDexDepthTypeDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CDexDepthTypeDlg::IDD, pParent)
{
  //{{AFX_DATA_INIT(CDexDepthTypeDlg)
  m_DepthType = 0;
  //}}AFX_DATA_INIT
}


void CDexDepthTypeDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CDexDepthTypeDlg)
  DDX_Radio(pDX, IDC_PROGNOSED, m_DepthType);
  //}}AFX_DATA_MAP
  if(m_DepthType == 0)
    m_depth_type = PROGNOSED;
  else
    m_depth_type = LOGGED;
}


BEGIN_MESSAGE_MAP(CDexDepthTypeDlg, CDialog)
  //{{AFX_MSG_MAP(CDexDepthTypeDlg)
    // NOTE: the ClassWizard will add message map macros here
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDexDepthTypeDlg message handlers
