#include "stdafx.h"

#include "Checkfrm.h"
#include "GlobalMessage.h"

//////////////////////////////////////
// EnumChildWnds Callback
BOOL CALLBACK EnumChildWnds(HWND hwnd, CCheckFrame *pObj) {
  CRect R;
  GetWindowRect(hwnd, &R);
  pObj->m_pDialog->ScreenToClient(&R);

  if (hwnd != pObj->m_pFrame->m_hWnd) {
    if (IsRectContainedInRect(R, pObj->m_rFrm))
      pObj->m_adwWndHandles.Add(hwnd);
  }

  return TRUE;
}

//////////////////////////////////////
// Function name	: Set
// Description	    : Set the connection between the frame and
//					  the parent window holding the controls
// Return type		: void
// Argument         : CWnd *pParentWnd - WIndows with frame control
// Argument         : unsigned int nFrmCtl - ID of frame control
void CCheckFrame::Set(CWnd *pParentWnd, unsigned int nFrmCtl) {
  m_pDialog = pParentWnd;

  // Calculate the size of the rect that should contain the controls
  m_pFrame = m_pDialog->GetDlgItem(nFrmCtl);
  if (!m_pFrame) {
    _m()->msg("Unable to find frame control");
    return;
  }
  m_pFrame->GetWindowRect(&m_rFrm);
  m_pDialog->ScreenToClient(&m_rFrm);

  // Make sure the array is empty
  m_adwWndHandles.RemoveAll();

  // The (control) windows contained within the boundaries of the frame control
  // are enumerated and their associated window-handles collected in an array.
  EnumChildWindows(m_pDialog->m_hWnd, (WNDENUMPROC)EnumChildWnds, (LPARAM)(CCheckFrame *)this);
}

//////////////////////////////////////
// Function name	: CCheckFrame::Enable
// Description	    : Enable/Disable all the controls within the frame control window
// Return type		: void
// Argument         : BOOL bEnable
void CCheckFrame::Enable(BOOL bEnable) {
  for (int i = 0; i < m_adwWndHandles.GetSize(); i++)
    EnableWindow((HWND)m_adwWndHandles[i], bEnable);
}

void CCheckFrame::Visible(BOOL bShow, BOOL bDoFrame /*=FALSE*/) {
  for (int i = 0; i < m_adwWndHandles.GetSize(); i++) {
    if (bShow)
      ShowWindow((HWND)m_adwWndHandles[i], SW_SHOW);
    else
      ShowWindow((HWND)m_adwWndHandles[i], SW_HIDE);
  }

  if (bDoFrame) {
    if (bShow)
      ShowWindow(m_pFrame->m_hWnd, SW_SHOW);
    else
      ShowWindow(m_pFrame->m_hWnd, SW_HIDE);
  }
}

//////////////////////////////////////
// Function name	: IsRectContainedInRect
// Description	    : Utility function that calculates if one rectangle is completely
//					  within another.
// Return type		: BOOL - if rcChild is completely contained in rcMother returns true
// Argument         : CRect &rcChild
// Argument         : CRect &rcMother
/*
|---------------|
|   rcMother    |
| |----------|  |
| | rcChild  |  |
| |----------|  |
|---------------|
*/
BOOL IsRectContainedInRect(CRect &rcChild, CRect &rcMother) {

  if (rcMother.PtInRect(CPoint(rcChild.left, rcChild.top)) && rcMother.PtInRect(CPoint(rcChild.left, rcChild.bottom)) &&
      rcMother.PtInRect(CPoint(rcChild.right, rcChild.top)) && rcMother.PtInRect(CPoint(rcChild.right, rcChild.bottom)))
    return TRUE;

  return FALSE;
}
