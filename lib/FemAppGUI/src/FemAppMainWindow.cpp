#include "StdAfx.h"
#include "FemAppMainWindow.h"

#include <cassert>


CWnd *FemAppGetMainWnd()
{
  static CWnd *pMainWindow = AfxGetMainWnd(); // we need to be called from the main thread to initialize
  assert(pMainWindow);

  CWnd *pWindow = AfxGetMainWnd(); // if our thread has a window, return it, otherwise return main thread's main window

  return pWindow ? pWindow : pMainWindow;
}
