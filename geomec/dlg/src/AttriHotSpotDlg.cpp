// AttriHotSpotDlg.cpp : implementation file
//

#include "AttriHotSpotDlg.h"
#include "GlobalMessage.h"
#include "OpenGLDlgView.h"
#include "geomec.h"
#include "stdafx.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CAttriHotSpotDlg dialog

CAttriHotSpotDlg::CAttriHotSpotDlg(CHotSpot &hot_spot, CWnd *pParent /*=NULL*/)
    : CAttributesTemplate<CHotSpot>(CAttriHotSpotDlg::IDD, hot_spot, pParent) {
  //{{AFX_DATA_INIT(CAttriHotSpotDlg)

  //}}AFX_DATA_INIT
}

void CAttriHotSpotDlg::DoDataExchange(CDataExchange *pDX) {
  CAttributesTemplate<CHotSpot>::DoDataExchange(pDX);

  std::pair<double, double> min_max;
  CString sName;
  int nState;

  if (!pDX->m_bSaveAndValidate) {
    sName = Copy().Name().toStdString().c_str();
    min_max = Copy().MinMaxValue();

    if (Copy().CenterIsTransparent())
      nState = 0;
    else
      nState = 1;
  }

  DDX_Text(pDX, IDC_ED_NAME, sName);
  DDX_Text(pDX, IDC_LEFT_VALUE, min_max.first);
  DDX_Text(pDX, IDC_RIGHT_VALUE, min_max.second);
  DDX_Radio(pDX, IDC_HS_RADIO_TOP, nState);

  if (pDX->m_bSaveAndValidate) {
    // Validate your data
    if (min_max.second < min_max.first) {
      EnterPopup();
      _m()->msg("Maximum value less than Minimum value");
      LeavePopup();
      pDX->Fail();
      return;
    }

    Copy().CenterIsTransparent(nState == 0);

    Copy().SetMinMaxValue(min_max.first, min_max.second);
    Copy().Name((LPCSTR)sName);
  }

  //{{AFX_DATA_MAP(CAttriHotSpotDlg)

  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAttriHotSpotDlg, CAttributesTemplate<CHotSpot>)
//{{AFX_MSG_MAP(CAttriHotSpotDlg)
ON_WM_PAINT()
ON_WM_CONTEXTMENU()
ON_COMMAND(ID_CHANGE_COLOR, OnChangeColor)
ON_WM_CREATE()
ON_BN_CLICKED(IDC_HS_RADIO_TOP, OnRadioTop)
ON_BN_CLICKED(IDC_HS_RADIO_BOTTOM, OnRadioBottom)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttriHotSpotDlg message handlers

void CAttriHotSpotDlg::OnPaint() {
  CPaintDC dc(this); // device context for painting
  CBrush brush1;
  CBrush brush2;
  CPen pen1;
  CPen pen2;
  CPen *pOldPen;
  CBrush *pOldBrush;

  ////Draw minimum color rectangle....
  COLORREF color = RGB(qRed(Copy().MinColor()), qGreen(Copy().MinColor()), qBlue(Copy().MinColor()));
  brush1.CreateSolidBrush(color);
  pOldBrush = dc.SelectObject(&brush1); // set new and save old brush....
  pen1.CreatePen(PS_SOLID, 1, color);
  pOldPen = dc.SelectObject(&pen1); // set new and save old pen...

  dc.Rectangle(m_rcMinRect);
  dc.SelectObject(pOldBrush); // put back old brush...
  dc.SelectObject(pOldPen);   // put back old pen...
  /////////////////////////////////////////////////////////

  ////Draw maximum color rectangle...
  color = RGB(qRed(Copy().MaxColor()), qGreen(Copy().MaxColor()), qBlue(Copy().MaxColor()));
  brush2.CreateSolidBrush(color);
  pOldBrush = dc.SelectObject(&brush2); // set new and save old brush....
  pen2.CreatePen(PS_SOLID, 1, color);
  pOldPen = dc.SelectObject(&pen2); // set new and save old pen...

  dc.Rectangle(m_rcMaxRect);
  dc.SelectObject(pOldBrush); // put back old brush...
  dc.SelectObject(pOldPen);   // put back old pen...
  /////////////////////////////////////////////////////////

  ////Draw the black lines...
  CRect rcLeftValue;
  CRect rcRightValue;
  GetDlgItem(IDC_LEFT_VALUE)->GetWindowRect(&rcLeftValue);
  ScreenToClient(&rcLeftValue);
  GetDlgItem(IDC_RIGHT_VALUE)->GetWindowRect(&rcRightValue);
  ScreenToClient(&rcRightValue);

  CPoint p1(m_rcMinRect.left + (m_rcMinRect.Width() / 2), m_rcTopView.bottom);
  dc.MoveTo(p1);
  p1.y = m_rcMinRect.top;
  dc.LineTo(p1);
  p1.y = m_rcMinRect.bottom;
  dc.MoveTo(p1);
  p1.y = m_rcBottomView.top;
  dc.LineTo(p1);
  p1.y = m_rcBottomView.bottom;
  dc.MoveTo(p1);
  p1.y = rcLeftValue.top;
  dc.LineTo(p1);

  p1.x = m_rcMaxRect.left + (m_rcMaxRect.Width() / 2);
  p1.y = m_rcTopView.bottom;
  dc.MoveTo(p1);
  p1.y = m_rcMaxRect.top;
  dc.LineTo(p1);
  p1.y = m_rcMaxRect.bottom;
  dc.MoveTo(p1);
  p1.y = m_rcBottomView.top;
  dc.LineTo(p1);
  p1.y = m_rcBottomView.bottom;
  dc.MoveTo(p1);
  p1.y = rcRightValue.top;
  dc.LineTo(p1);
}
BOOL CAttriHotSpotDlg::OnInitDialog() {
  CAttributesTemplate<CHotSpot>::OnInitDialog();

  CRect LeftRect;
  CRect RightRect;

  GetDlgItem(IDC_LEFT_COLOR)->GetWindowRect(&m_rcMinRect);
  ScreenToClient(&m_rcMinRect);
  GetDlgItem(IDC_RIGHT_COLOR)->GetWindowRect(&m_rcMaxRect);
  ScreenToClient(&m_rcMaxRect);
  GetDlgItem(IDC_HS_TOP_VIEW)->GetWindowRect(&m_rcTopView);
  ScreenToClient(&m_rcTopView);
  GetDlgItem(IDC_HS_BOTTOM_VIEW)->GetWindowRect(&m_rcBottomView);
  ScreenToClient(&m_rcBottomView);

  m_pTopOpenGLView->MoveWindow(m_rcTopView);
  m_pTopOpenGLView->UpdateWindow();

  LeftRect.SetRect(m_rcTopView.left, m_rcTopView.top, m_rcMinRect.left + (m_rcMinRect.Width() / 2), m_rcTopView.bottom);
  RightRect.SetRect(m_rcMaxRect.left + (m_rcMaxRect.Width() / 2), m_rcTopView.top, m_rcTopView.right,
                    m_rcTopView.bottom);
  LeftRect.OffsetRect(-m_rcTopView.left, 0);
  RightRect.OffsetRect(-m_rcTopView.left, 0);
  m_pTopOpenGLView->SetHotSpotRects(LeftRect, RightRect);
  m_pTopOpenGLView->ColorScale(Copy());

  m_pBottomOpenGLView->MoveWindow(m_rcBottomView);
  m_pBottomOpenGLView->UpdateWindow();

  LeftRect.SetRect(m_rcMinRect.left + (m_rcMinRect.Width() / 2), m_rcBottomView.top,
                   m_rcMaxRect.left + (m_rcMaxRect.Width() / 2), m_rcBottomView.bottom);
  RightRect.SetRect(m_rcMaxRect.left + (m_rcMaxRect.Width() / 2), m_rcBottomView.top, m_rcBottomView.right,
                    m_rcBottomView.bottom);
  LeftRect.OffsetRect(-m_rcBottomView.left, 0);
  RightRect.OffsetRect(-m_rcBottomView.left, 0);
  m_pBottomOpenGLView->SetHotSpotRects(LeftRect, RightRect);
  m_pBottomOpenGLView->ColorScale(Copy());

  m_pTopOpenGLView->ColorScale(Copy());
  m_pTopOpenGLView->CenterIsTransparent(true);
  m_pBottomOpenGLView->ColorScale(Copy());
  m_pBottomOpenGLView->CenterIsTransparent(false);

  // m_pToolTipCtrl->Create(this);

  UpdateData(FALSE);
  // TODO: Add extra initialization here

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CAttriHotSpotDlg::OnContextMenu(CWnd *pWnd, CPoint point) {
  m_ptClicked = point; // save the point...
  ScreenToClient(&m_ptClicked);

  // Create menu
  CMenu menu;
  menu.CreatePopupMenu();

  if (m_rcMinRect.PtInRect(m_ptClicked)) {
    menu.AppendMenu(MF_STRING, ID_CHANGE_COLOR, _T("&Change color"));
  }

  if (m_rcMaxRect.PtInRect(m_ptClicked))
    menu.AppendMenu(MF_STRING, ID_CHANGE_COLOR, _T("&Change color"));

  if (menu.GetMenuItemCount() > 0) {
    menu.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
  }
}

void CAttriHotSpotDlg::OnChangeColor() {
  EnterPopup();
  if (m_rcMinRect.PtInRect(m_ptClicked))
    Copy().MinColor(SelectColor(Copy().MinColor()));
  else
    Copy().MaxColor(SelectColor(Copy().MaxColor()));
  LeavePopup();
}

int CAttriHotSpotDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if (CAttributesTemplate<CHotSpot>::OnCreate(lpCreateStruct) == -1)
    return -1;

  //// Top view...
  m_pTopOpenGLView = new COpenGLDlgView;
  m_pTopOpenGLView->Create(0, 0, WS_CHILD | WS_VISIBLE, CRect(), this, 0);
  m_pTopOpenGLView->ShowWindow(SW_SHOW);
  m_pTopOpenGLView->UpdateWindow();
  //// Bottom view...
  m_pBottomOpenGLView = new COpenGLDlgView;
  m_pBottomOpenGLView->Create(0, 0, WS_CHILD | WS_VISIBLE, CRect(), this, 0);
  m_pBottomOpenGLView->ShowWindow(SW_SHOW);
  m_pBottomOpenGLView->UpdateWindow();

  return 0;
}

void CAttriHotSpotDlg::OnRadioTop() {
  if (!Copy().CenterIsTransparent())
    Copy().CenterIsTransparent(true);
  // TODO: Add your control notification handler code here
}

void CAttriHotSpotDlg::OnRadioBottom() {
  if (Copy().CenterIsTransparent())
    Copy().CenterIsTransparent(false);
  // TODO: Add your control notification handler code here
}
