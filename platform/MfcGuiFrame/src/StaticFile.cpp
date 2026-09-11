// StaticFile.cpp: implementation of the CStaticFile class.
//
//////////////////////////////////////////////////////////////////////
#include "StaticFile.h"
#include "stdafx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStaticFile::CStaticFile(DWORD dwFormat, BOOL bPathEllipsis) : m_bPathEllipsis(bPathEllipsis), m_dwFormat(dwFormat) {}

BEGIN_MESSAGE_MAP(CStaticFile, CStatic)
//{{AFX_MSG_MAP(CStaticFile)
ON_MESSAGE(WM_SETTEXT, OnSetText)
ON_WM_ERASEBKGND()
ON_WM_PAINT()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStaticFile message handlers

BOOL CStaticFile::OnEraseBkgnd(CDC *pDC) {
  RECT rectWnd; // window rectangle

  // Erase background
  GetClientRect(&rectWnd);
  pDC->FillSolidRect(&rectWnd, ::GetSysColor(COLOR_3DFACE));
  return (TRUE);
}

void CStaticFile::OnPaint() {
  CPaintDC dc(this);       // device context for painting
  RECT rectWnd;            // window rectangle
  CString strText;         // window text
  CWnd *pWndParent = NULL; // parent window

  // Set default font
  pWndParent = GetParent();
  if (pWndParent)
    dc.SelectObject(pWndParent->GetFont());

  // Draw text
  GetWindowText(strText);
  GetClientRect(&rectWnd);
  DWORD dwFormat = m_dwFormat | (m_bPathEllipsis ? DT_PATH_ELLIPSIS : DT_END_ELLIPSIS);
  ::DrawTextEx(dc.m_hDC, strText.GetBuffer(0), strText.GetLength(), &rectWnd, dwFormat, NULL);
  strText.ReleaseBuffer();

  // Do not call CWnd::OnPaint() for painting messages
}

LRESULT CStaticFile::OnSetText(WPARAM wParam, LPARAM lParam) {
  DefWindowProc(WM_SETTEXT, wParam, lParam);
  Invalidate();
  return (TRUE);
}

BOOL CStaticFile::PathEllipsis() const { return m_bPathEllipsis; }

void CStaticFile::PathEllipsis(const BOOL bEnable) { m_bPathEllipsis = bEnable; }

DWORD CStaticFile::TextFormat() const { return m_dwFormat; }

void CStaticFile::TextFormat(const DWORD dwFormat) { m_dwFormat = dwFormat; }