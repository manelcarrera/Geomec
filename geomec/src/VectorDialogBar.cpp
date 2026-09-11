// VectorDialogBar.cpp : implementation file
//

#include "stdafx.h"
#include "Geomec.h"
#include "VectorDialogBar.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "ModelView.h"

IMPLEMENT_DYNAMIC(CVectorDialogBar, CDialogBar)

CVectorDialogBar::CVectorDialogBar()
{

}

CVectorDialogBar::~CVectorDialogBar()
{
}

void CVectorDialogBar::DoDataExchange(CDataExchange* pDX)
{
  CDialogBar::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_SCALE_SLIDER, m_scaleSlider);
  DDX_Control(pDX, IDC_DECIMATE_SLIDER, m_decimateSlider);
}


BEGIN_MESSAGE_MAP(CVectorDialogBar, CDialogBar)
  ON_WM_HSCROLL()
  ON_MESSAGE(WM_INITDIALOG, OnInitDialog ) 
END_MESSAGE_MAP()


// CVectorDialogBar message handlers

LRESULT CVectorDialogBar::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
  BOOL bRet = HandleInitDialog(wParam, lParam);

  if (!UpdateData(FALSE))
  {
      TRACE0("Warning: UpdateData failed during dialog init.\n");
  }

  return bRet;
}


void CVectorDialogBar::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  CWnd* wnd = GetParent();
  CRuntimeClass* rtclass = wnd->GetRuntimeClass();

  while(wnd && !wnd->IsKindOf(RUNTIME_CLASS(CMainFrame)))
  wnd = wnd->GetParent();

  if(wnd)
  {
  CSliderCtrl* slider = reinterpret_cast<CSliderCtrl*>(pScrollBar);
  int pos = slider->GetPos();

  CGeomecDoc* pDoc = &((CGeomecApp*)AfxGetApp())->GetDoc();
  CModelView* modelView = pDoc->GetModelView();

  // Convert the slider position to a value between 0.0f and 1.0f
  int maxval = slider->GetRangeMax();
  int minval = slider->GetRangeMin();
  float normalizedPos = (float)(pos - minval) / (float)(maxval - minval);

  if(slider == &m_scaleSlider)
  {
      // Map slider position to range [.25, 4.0]
      float scale = powf(2.0f, 4.0f * normalizedPos - 2.0f);
      modelView->SendMessage(WM_SCALE_VECTORS, 0U, *((LPARAM*)&scale));
  }
  else // decimate slider
  {
      // Map slider position to range [1, .01]
      float factor = powf(10.0f, -2.0f * normalizedPos);

      modelView->SendMessage(WM_DECIMATE_VECTORS, 0U, *((LPARAM*)&factor));
  }
  }

  CDialogBar::OnHScroll(nSBCode, nPos, pScrollBar);
}
