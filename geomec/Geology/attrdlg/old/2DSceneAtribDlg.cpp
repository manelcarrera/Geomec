// 2DSceneAtribDlg.cpp : implementation file
//

#include "stdafx.h"
#include "geomec.h"
#include "2dsceneatribdlg.h"
#include "OpenGLSceneBase.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// C2DSceneAtribDlg dialog


C2DSceneAtribDlg::C2DSceneAtribDlg(COpenGLSceneBase &scene,CWnd* pParent /*=NULL*/)
  : CDialog(C2DSceneAtribDlg::IDD, pParent)
{
  m_pScene=&scene;
  m_Color=m_pScene->BackgroundColor();
  //{{AFX_DATA_INIT(C2DSceneAtribDlg)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}


void C2DSceneAtribDlg::FillFrame(const UINT uFrameId, const int nFillColor, CPaintDC &dc)
{
  CWnd *pColor = GetDlgItem(uFrameId);
  CBrush Brush(nFillColor);
  CRect rectClient;
  pColor->GetWindowRect( &rectClient );
  ScreenToClient( &rectClient );
  dc.FillRect( &rectClient, &Brush );
}


void C2DSceneAtribDlg::OnPaint() 
{
  CPaintDC dc(this); // device context for painting
  FillFrame(IDC_ST_BACKGROUND_COLOR, m_Color, dc);
}


void C2DSceneAtribDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(C2DSceneAtribDlg)
    // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(C2DSceneAtribDlg, CDialog)
  //{{AFX_MSG_MAP(C2DSceneAtribDlg)
  ON_BN_CLICKED(IDC_ST_BACKGROUND_COLOR, OnClickBackgroundColor)
  ON_WM_PAINT()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// C2DSceneAtribDlg message handlers

int SelectColor(const int nDefaultColor)
{
  CColorDialog dlg;
//	COLORREF aCustomColors[16];
  //GetCustomColors( aCustomColors );

  dlg.m_cc.Flags |= CC_RGBINIT;
  dlg.m_cc.rgbResult = nDefaultColor;
//	dlg.m_cc.lpCustColors = aCustomColors;

  if( dlg.DoModal() == IDOK )
  {
    int nColorRet = dlg.GetColor();
    //SetCustomColors( aCustomColors );
    return nColorRet;
  }

  return nDefaultColor;
}


void C2DSceneAtribDlg::OnClickBackgroundColor() 
{
  m_Color=SelectColor(m_Color);
  Invalidate(); //redraw
}

void C2DSceneAtribDlg::OnOK() 
{
  m_pScene->BackgroundColor(m_Color);
  m_pScene->InvalidateViews();
  m_pScene->Modified();
  CDialog::OnOK();
}
