// attricolorgradientdlg.cpp : implementation file
//

#include "stdafx.h"

#include "geomec.h"
#include "attricolorgradientdlg.h"
#include "OpenGLDlgView.h"
#include <cfloat>
#include "GlobalMessage.h"
#include "ColorGradient_Delegate.h"

#include "DDX_Text_Patch.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

#define SMALLRECTWIDTH 5
#define HALFRECTWIDTH (SMALLRECTWIDTH - 1)/2
/////////////////////////////////////////////////////////////////////////////
// CAttriColorGradientDlg dialog


CAttriColorGradientDlg::CAttriColorGradientDlg(CColorGradient& color_gradient, CWnd* pParent /*=NULL*/)
  : CAttributesTemplate<CColorGradient>(CAttriColorGradientDlg::IDD, color_gradient, pParent), m_bInRect(FALSE), m_pToolTipCtrl(0)
{
  //{{AFX_DATA_INIT(CAttriColorGradientDlg)
  //}}AFX_DATA_INIT
}

CAttriColorGradientDlg::~CAttriColorGradientDlg()
{
  if(m_pToolTipCtrl)
    delete m_pToolTipCtrl;	
}

int CAttriColorGradientDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (CAttributesTemplate<CColorGradient>::OnCreate(lpCreateStruct) == -1)
    return -1;


  m_pOpenGLView = new COpenGLDlgView;
  m_pOpenGLView->Create(0, 0, WS_CHILD | WS_VISIBLE, CRect(), this, 0);
  m_pOpenGLView->ShowWindow(SW_SHOW);
  m_pOpenGLView->UpdateWindow();
    
//	m_pOpenGLView->m_pDlg = this;
  
  
  return 0;
}

BOOL CAttriColorGradientDlg::OnInitDialog() 
{
  CAttributesTemplate<CColorGradient>::OnInitDialog();

  CRect rcOpenGL;
  GetDlgItem(IDC_ST_OPENGL_VIEW)->GetWindowRect(&rcOpenGL);
  ScreenToClient(&rcOpenGL);

  m_pOpenGLView->Rect(rcOpenGL); // Tell view which rectangle it needs to draw in...
  m_pOpenGLView->MoveWindow(rcOpenGL); // Move the view to the right rectangle...
  m_pOpenGLView->UpdateWindow(); // Update..
  m_pOpenGLView->ColorScale(Copy()); // Tell view what kind of colorscale is going to be drawn...

  
  assert(m_pToolTipCtrl == 0);
  m_pToolTipCtrl = new CToolTipCtrl;
  m_pToolTipCtrl->Create(this);
  UpdateToolTip();

  UpdateData(FALSE);

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CAttriColorGradientDlg::DoDataExchange(CDataExchange* pDX)
{
  CAttributesTemplate<CColorGradient>::DoDataExchange(pDX);

  std::pair<double, double> min_max(0,0);
  CString sName;

  if(!pDX->m_bSaveAndValidate)
  {
    sName = Copy().Name().toStdString().c_str();
    min_max = Copy().MinMaxValue();
  
    int nShow;
    if(Copy().IsGlobal())
  {
      nShow = SW_SHOW;
      ((CButton*)GetDlgItem(IDC_RAD_AUTOEXTREMES))->SetCheck(Copy().AutoExtremes() ? 1 : 0);
      ((CButton*)GetDlgItem(IDC_RAD_MANEXTREMES ))->SetCheck(Copy().AutoExtremes() ? 0 : 1);
      UpdateRangeControls();
  }
    else 
    {
      CRect rect;
      GetWindowRect(rect);

      nShow = SW_HIDE;
      SetWindowText("Attributes for local color scale");
      SetWindowPos(0, rect.left, rect.top, rect.Width(), 4 * (rect.Height() / 5), 0); // Adjust size of dialog in case of local color scale...
    }

    GetDlgItem(IDC_ED_NAME)->EnableWindow(Copy().IsGlobal());
  GetDlgItem(IDC_FRA_MANEXTREMES)->ShowWindow(nShow);
  GetDlgItem(IDC_RAD_AUTOEXTREMES)->ShowWindow(nShow);
  GetDlgItem(IDC_RAD_MANEXTREMES)->ShowWindow(nShow);
    GetDlgItem(IDC_ED_MAX_VALUE)->ShowWindow(nShow);
    GetDlgItem(IDC_ED_MIN_VALUE)->ShowWindow(nShow);
    GetDlgItem(IDC_ST_MIN_LABEL)->ShowWindow(nShow);
    GetDlgItem(IDC_ST_MAX_LABEL)->ShowWindow(nShow);
  }

  //{{AFX_DATA_MAP(CAttriColorGradientDlg)
  //}}AFX_DATA_MAP

  DDX_Text(pDX, IDC_ED_NAME, sName);
  DDX_Text(pDX, IDC_ED_MAX_VALUE, min_max.second);
  DDX_Text(pDX, IDC_ED_MIN_VALUE, min_max.first);

  DDV_MinMaxDouble(pDX, min_max.first, -DBL_MAX, DBL_MAX);
  DDV_MinMaxDouble(pDX, min_max.second, -DBL_MAX, DBL_MAX);


  if(pDX->m_bSaveAndValidate)
  {
    Copy().Name((LPCSTR) sName);

    if(Copy().IsGlobal())
    {
      // Validate your data
      if(min_max.second <= min_max.first)
      {
    EnterPopup();
        _m()->msg("Maximum value less than Minimum value");
    LeavePopup();
        pDX->Fail();
        return;
      }

      Copy().SetMinMaxValue(min_max.first, min_max.second);
    }
  }
}


BEGIN_MESSAGE_MAP(CAttriColorGradientDlg, CAttributesTemplate<CColorGradient>)
  //{{AFX_MSG_MAP(CAttriColorGradientDlg)
  ON_BN_CLICKED(IDC_RAD_AUTOEXTREMES, OnAutoExtremes)
  ON_BN_CLICKED(IDC_RAD_MANEXTREMES, OnManualExtremes)
  ON_WM_PAINT()
  ON_WM_RBUTTONDOWN()
  ON_WM_CONTEXTMENU()
  ON_COMMAND(ID_INSERT_COLOR_INTERVAL, OnInsertColorInterval)
  ON_COMMAND(ID_CHANGE_COLOR, OnChangeColor)
  ON_COMMAND(ID_REMOVE_COLOR, OnRemoveColor)
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_CREATE()
  ON_WM_DESTROY()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttriColorGradientDlg message handlers

void CAttriColorGradientDlg::OnAutoExtremes()
{
  // Code to prevent resetting the name
  CString sName;
  GetDlgItem(IDC_ED_NAME)->GetWindowText(sName);
  Copy().Name((LPCSTR) sName);
  Copy().AutoExtremes(true);
  dynamic_cast <CColorGradient_Delegate*> (Copy().getDelegate())->FindExtents();
  UpdateData(FALSE);
  UpdateRangeControls();
}

void CAttriColorGradientDlg::OnManualExtremes()
{
  Copy().AutoExtremes(false);
  UpdateRangeControls();
}

void CAttriColorGradientDlg::UpdateRangeControls()
{
  GetDlgItem(IDC_ST_MIN_LABEL)->EnableWindow(!Copy().AutoExtremes());
  GetDlgItem(IDC_ED_MIN_VALUE)->EnableWindow(!Copy().AutoExtremes());
  GetDlgItem(IDC_ST_MAX_LABEL)->EnableWindow(!Copy().AutoExtremes());
  GetDlgItem(IDC_ED_MAX_VALUE)->EnableWindow(!Copy().AutoExtremes());
}

CRect CAttriColorGradientDlg::GetRect(CColorGradient::color_iterator it)
{
  double fraction = Copy().Fraction(it); //get relative position of new rectangle...

  CRect rect;
  GetDlgItem(IDC_ST_GDI_FRAME)->GetWindowRect(&rect);
  ScreenToClient(&rect);

  CRect retrect;

  double leftpoint = rect.left + (rect.Width() * fraction) - HALFRECTWIDTH; //define left of new rectangle...
  retrect.SetRect(leftpoint, rect.top, leftpoint + SMALLRECTWIDTH, rect.bottom); //set the dimensions of the rectangle...

  return retrect;
}

void CAttriColorGradientDlg::OnPaint() 
{
  CPaintDC dc(this); // device context for painting

  CRect rect;
  GetDlgItem(IDC_ST_GDI_FRAME)->GetWindowRect(&rect);
  ScreenToClient(&rect);
  CRect smallrect;

  for(CColorGradient::color_iterator it = Copy().Begin(); it != Copy().End(); it++)
  {
    COLORREF color = RGB(qRed((*it).second), qGreen((*it).second), qBlue((*it).second));
    CBrush brush(color);
    CBrush *pOldBrush = dc.SelectObject(&brush); //set new and save old brush....
    CPen pen(PS_SOLID, 1, color);
    CPen *pOldPen = dc.SelectObject(&pen); //set new and save old pen...

    smallrect = GetRect(it);
    dc.Rectangle(smallrect);
    dc.SelectObject(pOldBrush); //put back old brush...
    dc.SelectObject(pOldPen); //put back old pen...
  }	
  
  if(m_bInRect)
  {
    COLORREF color = RGB(qRed(m_nTempColor), qGreen(m_nTempColor), qBlue(m_nTempColor));
    CBrush brush(color);
    CBrush *pOldBrush = dc.SelectObject(&brush); // set new and save old brush... 
    CPen pen(PS_SOLID, 1, color);
    CPen *pOldPen = dc.SelectObject(&pen); //set new and save old pen...

    dc.Rectangle(m_rcTempRec);
    dc.SelectObject(pOldBrush); //put back old brush...
    dc.SelectObject(pOldPen); //put back old pen...
  }
}

void CAttriColorGradientDlg::ClearMapAndToolTip()
{
  TRectMap::iterator rectit;
  for(rectit = m_mpRectMap.begin(); rectit != m_mpRectMap.end(); rectit++)
  {
    m_pToolTipCtrl->DelTool(this, rectit->second);
  }
  m_mpRectMap.clear();
}

void CAttriColorGradientDlg::UpdateToolTip()
{
  ClearMapAndToolTip();

  CRect rect;
  GetDlgItem(IDC_ST_GDI_FRAME)->GetWindowRect(&rect);
  ScreenToClient(&rect);
  CRect smallrect;

  int nId = 1; // Id nrs. for use in the ToolTipCtrl...
  for(CColorGradient::color_iterator it = Copy().Begin(); it != Copy().End(); it++, nId++)
  {
    smallrect = GetRect(it);
    m_mpRectMap.insert(TRectIdPair(smallrect, nId));
  }

  TRectMap::iterator rectit;
  for(rectit = m_mpRectMap.begin(); rectit != m_mpRectMap.end(); rectit++)
  {
    double dValue;
    CString stvalue;

    // Calculate the value based on the middle of the rectangle....
    if(Copy().IsGlobal()) // Global color scale, real values...
    {
      dValue = Copy().MinMaxValue().first + (Copy().MinMaxValue().second - Copy().MinMaxValue().first) * (((rectit->first).left + HALFRECTWIDTH - rect.left) / ((rect.right - rect.left) * 1.0));
      stvalue.Format("%4.6G", dValue); 
    }
    else // Local color scale, only percentages....
    {
      dValue = (((rectit->first).left + HALFRECTWIDTH - rect.left) / ((rect.right - rect.left) * 1.0)) * 100;
      stvalue.Format("%.2f%%", dValue); // from 0% to 100%...
    }

    m_pToolTipCtrl->AddTool(this, stvalue, rectit->first, rectit->second);
  }

  m_pToolTipCtrl->Activate(TRUE);
}

void CAttriColorGradientDlg::OnRButtonDown(unsigned int nFlags, CPoint point) 
{
  // TODO: Add your message handler code here and/or call default
  
  
  CAttributesTemplate<CColorGradient>::OnRButtonDown(nFlags, point);
}

void CAttriColorGradientDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
  m_ptClicked = point; //save the point...
  ScreenToClient(&m_ptClicked);

  // Create menu
  CMenu menu;
  menu.CreatePopupMenu();

  // Loop over dingetjes
  BOOL bRectFound = FALSE;
  for(CColorGradient::color_iterator it = Copy().Begin(); it != Copy().End(); it++)
  {
    CRect test = GetRect(it);
    if(test.PtInRect(m_ptClicked))
    {
      menu.AppendMenu(MF_STRING, ID_CHANGE_COLOR, _T("&Change color"));
      if(!(it == Copy().Begin() || it == --Copy().End()))
        menu.AppendMenu(MF_STRING, ID_REMOVE_COLOR, _T("&Remove color value"));
      bRectFound = TRUE;
      break;
    }
  }

  // Test frames
  if(!bRectFound)
  {
    CRect rcGDI;
    GetDlgItem(IDC_ST_GDI_FRAME)->GetWindowRect(&rcGDI);
    CRect rcOpenGL;
    GetDlgItem(IDC_ST_OPENGL_VIEW)->GetWindowRect(&rcOpenGL);

    if(rcGDI.PtInRect(point) || rcOpenGL.PtInRect(point))
      menu.AppendMenu(MF_STRING, ID_INSERT_COLOR_INTERVAL, _T("&Insert color value"));
  }

  if(menu.GetMenuItemCount() > 0)
  {
    menu.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
  }
}

void CAttriColorGradientDlg::OnInsertColorInterval()
{
  CRect rcGDI;
  GetDlgItem(IDC_ST_GDI_FRAME)->GetWindowRect(&rcGDI);
  ScreenToClient(&rcGDI);
  
  if (!Copy().AddValue(m_ptClicked.x - rcGDI.left, rcGDI.Width(), qRgb(0, 255, 0))) //add interval and check if there are not too many already...
  {
  EnterPopup();
  _m()->msg("More than 10 sections are not allowed");
  LeavePopup();
  }

  UpdateToolTip();
  Invalidate();
  UpdateWindow();
}

void CAttriColorGradientDlg::OnChangeColor()
{
  CColorGradient::color_iterator it;
  for(it = Copy().Begin(); it != Copy().End(); it++)
  {
    CRect test = GetRect(it);
    if(test.PtInRect(m_ptClicked))
      break;
  }

  assert(it != Copy().End());

  EnterPopup();
  VERIFY(Copy().ChangeColor(it->first, SelectColor(it->second)));
  LeavePopup();
}

void CAttriColorGradientDlg::OnRemoveColor()
{
  CColorGradient::color_iterator it;
  for(it = Copy().Begin(); it != Copy().End(); it++)
  {
    CRect test = GetRect(it);
    if(test.PtInRect(m_ptClicked))
      break;
  }

  VERIFY(Copy().RemoveValue(it->first));

  Invalidate();
  UpdateWindow();
}


void CAttriColorGradientDlg::OnMouseMove(unsigned int nFlags, CPoint point) 
{
  if(m_bInRect)
  {
    CRect rcGDI;
    GetDlgItem(IDC_ST_GDI_FRAME)->GetWindowRect(&rcGDI);
    ScreenToClient(&rcGDI);

  
    int nOffset = point.x - m_rcTempRec.left; // calculate offset from last position of rectangle that is dragged...

    CRect invalirect; // construct a rectangle that will be invalidated (redrawn)....
    if(nOffset < 0)
      invalirect.SetRect(point.x - SMALLRECTWIDTH, rcGDI.top, m_rcTempRec.right + SMALLRECTWIDTH, rcGDI.bottom);
    else
      invalirect.SetRect(m_rcTempRec.left - SMALLRECTWIDTH, rcGDI.top, point.x + SMALLRECTWIDTH, rcGDI.bottom);

    if(m_rcTempRec.left + nOffset <= rcGDI.left + HALFRECTWIDTH) // check if new position of rectangle is too much to the left....
    {
      m_rcTempRec.SetRect(rcGDI.left + HALFRECTWIDTH, rcGDI.top, rcGDI.left + SMALLRECTWIDTH, rcGDI.bottom); // set explicitly...
      m_rcTempRec.OffsetRect(HALFRECTWIDTH, 0); // a little room bewtween the two rectangles..
    }
    else if(m_rcTempRec.right + nOffset >= rcGDI.right - 2) // check if new position of rectangle is too much to the right...
    {
      m_rcTempRec.SetRect(rcGDI.right - (SMALLRECTWIDTH + HALFRECTWIDTH), rcGDI.top, rcGDI.right - HALFRECTWIDTH, rcGDI.bottom); // set explicitly...
      m_rcTempRec.OffsetRect(-HALFRECTWIDTH, 0); // a little room bewtween the two rectangles..
    }
    else
      m_rcTempRec.OffsetRect(nOffset, 0); // offset the rectangle that is dragged...
    
    InvalidateRect(invalirect); // invalidate the rectangle...
  }
  
  CAttributesTemplate<CColorGradient>::OnMouseMove(nFlags, point);
}

void CAttriColorGradientDlg::OnLButtonDown(unsigned int nFlags, CPoint point) 
{
  SetCapture();
  m_pToolTipCtrl->Update();
  // Check if a rectangle is selected, excluding the first or the last...
  for(CColorGradient::color_iterator it = ++Copy().Begin(); it != --Copy().End(); it++)
  {
    CRect test = GetRect(it);
    
    if(test.PtInRect(point))
    {
      m_nTempColor = it->second; // save the color for use in other functions...
      VERIFY(Copy().RemoveValue(it->first)); // remove the iterator from the map, is put back in OnLButtonUp...
      m_bInRect = TRUE; // we are dragging a rectangle...
      m_rcTempRec = test; // make a copy of the rectangle for use in other functions...
      break;
    }
  }

  CAttributesTemplate<CColorGradient>::OnLButtonDown(nFlags, point);
}

void CAttriColorGradientDlg::OnLButtonUp(unsigned int nFlags, CPoint point) 
{
  if(m_bInRect)
  {
    CRect rcGDI;
    GetDlgItem(IDC_ST_GDI_FRAME)->GetWindowRect(&rcGDI);
    ScreenToClient(&rcGDI);
  
    // Calculate the new value based on the middle of the rectangle....
    double dValue = Copy().MinMaxValue().first + (Copy().MinMaxValue().second - Copy().MinMaxValue().first) * ((m_rcTempRec.left + HALFRECTWIDTH - rcGDI.left) / ((rcGDI.right - rcGDI.left) * 1.0));
    Copy().AddValue(dValue, m_nTempColor); // insert the new value and the color....
    m_bInRect = FALSE; // we stopped dragging a rectangle...
    
    InvalidateRect(rcGDI); // Redraw all the rectangles...
    UpdateToolTip();
  }
  CAttributesTemplate<CColorGradient>::OnLButtonUp(nFlags, point);
  ReleaseCapture();
}

void CAttriColorGradientDlg::OnDestroy() 
{
  
//	m_pOpenGLView->CurrentScene(0);
  CAttributesTemplate<CColorGradient>::OnDestroy();
  // TODO: Add your message handler code here
  
}

BOOL CAttriColorGradientDlg::PreTranslateMessage(MSG* pMsg) 
{
   if (NULL != m_pToolTipCtrl)
   {
      m_pToolTipCtrl->RelayEvent(pMsg);
    
   }
   
   return CAttributesTemplate<CColorGradient>::PreTranslateMessage(pMsg);
}

