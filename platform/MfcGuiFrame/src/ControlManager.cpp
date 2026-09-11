// ControlManager.cpp: implementation of the CControlManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ControlManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//// CControlManager implementation
CControlManager::CControlManager(CWnd& control, CWnd& parent, THorizontal horbehaviour, TVertical verbehaviour)
: m_control(control),
  m_parent(parent),
  m_horbehaviour(horbehaviour),
  m_verbehaviour(verbehaviour)
{
  GetOffsets();
}

CControlManager::~CControlManager()
{
}

void CControlManager::ResizeParams(int beh, int newsiz, int left, int right, int size, int& x, int& width)
{
  // determine resize parameters based on behaviour, size and offsets
  // in vertical sense left is top and right is bottom
  switch(beh)
  {
  case 0:
    x = left;
    width = size;
    break;
  case 1:
    x = newsiz - right - size;
    width = size;
    break;
  case 2:
    x = left;
    width = newsiz - right - left;
    break;
  default:
    assert(false);
  }
}

void CControlManager::OnResize()
{
  int x;
  int y;
  int nWidth;
  int nHeight;

  CRect rectClient;
  m_parent.GetClientRect(&rectClient);

  int cx = rectClient.Width();
  int cy = rectClient.Height();

  ResizeParams(int(m_horbehaviour), cx, m_offset[2], m_offset[3], m_size[0], x, nWidth);
  ResizeParams(int(m_verbehaviour), cy, m_offset[0], m_offset[1], m_size[1], y, nHeight);

  // invalidate original position
  m_control.Invalidate();

  // move the control
  m_control.MoveWindow(x, y, nWidth, nHeight);

  // invalidate new position
  m_control.Invalidate();
}

void CControlManager::GetOffsets()
{
  CRect rectControl;
  m_control.GetWindowRect(&rectControl);

  m_parent.ScreenToClient(&rectControl);

  CRect rectDialog;
  m_parent.GetClientRect(&rectDialog);

  m_offset[0] = rectControl.top - rectDialog.top;
  assert(m_offset[0] >= 0);
  m_offset[1] = rectDialog.bottom - rectControl.bottom;
  assert(m_offset[1] >= 0);
  m_offset[2] = rectControl.left - rectDialog.left;
  assert(m_offset[2] >= 0);
  m_offset[3] = rectDialog.right - rectControl.right;
  assert(m_offset[3] >= 0);

  m_size[0] = rectControl.Width();
  assert(m_size[0] >= 0);
  m_size[1] = rectControl.Height();
  assert(m_size[1] >= 0);
}
