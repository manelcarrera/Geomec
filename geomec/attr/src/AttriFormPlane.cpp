// AttriFormPlane.cpp : implementation file
//

#include "stdafx.h"
#include "geomec.h"
#include "AttriFormPlane.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CAttriFormPlane dialog


CAttriFormPlane::CAttriFormPlane(CFormationPlane &plane, CWnd* pParent /*=NULL*/)
: CAttributesTemplate<CFormationPlane>(CAttriFormPlane::IDD, plane, pParent)
{
  //{{AFX_DATA_INIT(CAttriFormPlane)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}


void CAttriFormPlane::DoDataExchange(CDataExchange* pDX)
{
  CString	strName;

  CDialog::DoDataExchange(pDX);

  if(!pDX->m_bSaveAndValidate)
  {
    strName = Copy().Name().toStdString().c_str();
  }

  //{{AFX_DATA_MAP(CAttriFormPlane)
    // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP

  DDX_Text(pDX, IDC_ED_NAME, strName);

  if(pDX->m_bSaveAndValidate)
  {
    Copy().Name((LPCSTR) strName);
  }
}


BEGIN_MESSAGE_MAP(CAttriFormPlane, CDialog)
  //{{AFX_MSG_MAP(CAttriFormPlane)
  ON_BN_CLICKED(IDC_ST_COLOR, OnColor)
  ON_WM_PAINT()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttriFormPlane message handlers
void CAttriFormPlane::OnColor() 
{
  Copy().Color(SelectColor(Copy().Color()));
  Invalidate();
}

//##ModelId=3BE7AEC9019B
void CAttriFormPlane::OnPaint() 
{
  CPaintDC dc(this); // device context for painting
  FillFrame(IDC_ST_COLOR, Copy().Color(), dc);
}
