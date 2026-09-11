// WellPathFileUnitDlg.cpp : implementation file
//

#include "stdafx.h"
#include "geomec.h"
#include "WellPathFileUnitDlg.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CWellPathFileUnitDlg dialog


CWellPathFileUnitDlg::CWellPathFileUnitDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CWellPathFileUnitDlg::IDD, pParent)
  , m_Unit(CQuantity::FIELD_UNIT)
{
  //{{AFX_DATA_INIT(CWellPathFileUnitDlg)
  //}}AFX_DATA_INIT
}

CQuantity::UNIT CWellPathFileUnitDlg::Unit()
{
  if(m_Unit==0)
    return CQuantity::FIELD_UNIT;
  else
    return CQuantity::SI_UNIT;
}

void CWellPathFileUnitDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CWellPathFileUnitDlg)
  DDX_Radio(pDX, IDC_RADIO_FIELD, m_Unit);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWellPathFileUnitDlg, CDialog)
  //{{AFX_MSG_MAP(CWellPathFileUnitDlg)
    // NOTE: the ClassWizard will add message map macros here
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWellPathFileUnitDlg message handlers
