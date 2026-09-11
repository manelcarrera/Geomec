// dimensiondlg.cpp : implementation file
//

#include "stdafx.h"
#include "geomec.h"
#include "dimensiondlg.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CDimensionDlg dialog


CDimensionDlg::CDimensionDlg(CPointSet::DIMENSION dim, CWnd* pParent /*=NULL*/)
  : CDialog(CDimensionDlg::IDD, pParent), m_dim(dim)
{
  //{{AFX_DATA_INIT(CDimensionDlg)
  //}}AFX_DATA_INIT
}


void CDimensionDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  int nDim;

  if(!pDX->m_bSaveAndValidate)
  {
    if(m_dim == CPointSet::DIM_2D)
      nDim = 0;
    else
      nDim = 1;
  }

  //{{AFX_DATA_MAP(CDimensionDlg)
  DDX_Radio(pDX, IDC_RD_DIM_2D, nDim);
  //}}AFX_DATA_MAP

  if(pDX->m_bSaveAndValidate)
  {
    switch(nDim)
    {
    case 0:
      m_dim = CPointSet::DIM_2D;
      break;
    case 1:
      m_dim = CPointSet::DIM_3D;
      break;
    default:
      break;
    }
  }
}

CPointSet::DIMENSION CDimensionDlg::Dimension() const
{
  return m_dim;
}

BEGIN_MESSAGE_MAP(CDimensionDlg, CDialog)
  //{{AFX_MSG_MAP(CDimensionDlg)
    // NOTE: the ClassWizard will add message map macros here
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDimensionDlg message handlers
