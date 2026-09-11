// OpenGLDlgView.cpp: implementation of the COpenGLDlgView class.
//
//////////////////////////////////////////////////////////////////////

#include "OpenGLDlgView.h"
#include "ColorGradient.h"
#include "DrawDef.h"
#include "HotSpot.h"
#include "colorscaledrawdef.h"
#include "geomec.h"
#include "stdafx.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
// #define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(COpenGLDlgView, COpenGLView)
//{{AFX_MSG_MAP(CGraphView)
ON_WM_MOUSEACTIVATE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

COpenGLDlgView::COpenGLDlgView() : m_pColorScale(0) {}

COpenGLDlgView::~COpenGLDlgView() {}

void COpenGLDlgView::DrawScene() {
  COpenGLView::DrawScene();
  DrawColorScale(*m_pColorScale);
}

void COpenGLDlgView::DrawColorScale(IColorScaleNode &colorscale) {
  MakeCurrent();
  Invalidate();
  CColorGradient *colorgradient = dynamic_cast<CColorGradient *>(&colorscale);
  CHotSpot *hotspot = dynamic_cast<CHotSpot *>(&colorscale);

  if (colorgradient) {
    double dMin = ViewPortX();
    double dRange = ViewPortWidth();

    for (CColorGradient::color_iterator it = colorgradient->Begin(); it != colorgradient->End();) {
      CColorGradient::color_iterator it_first = it;
      it++;
      CColorGradient::color_iterator it_second = it;
      if ((it_first != colorgradient->End()) && (it_second != colorgradient->End())) {
        CColorScaleDrawDef DrawDef;
        DrawDef.ColorScaleDirection(CColorScaleDrawDef::CS_X_DIRECTION);
        DrawDef.AddPair(colorgradient->Fraction(it_first) * dRange, it_first->second);
        double dLeft = colorgradient->Fraction(it_first);

        DrawDef.AddPair(colorgradient->Fraction(it_second) * dRange, it_second->second);
        double dRight = colorgradient->Fraction(it_second);

        DrawObject(geo::CRectangle(geo::CPoint(ViewPortX() + ViewPortWidth() * dLeft, ViewPortY()),
                                   geo::CPoint(ViewPortX() + ViewPortWidth() * dRight, ViewPortY() + ViewPortHeight())),
                   DrawDef);
      }
    }
    return;
  } else if (hotspot) {
    double dMinLeft = m_rcLeftRect.left * 1.0;
    double dMaxLeft = m_rcLeftRect.right * 1.0;
    double dMinRight = m_rcRightRect.left * 1.0;
    double dMaxRight = m_rcRightRect.right * 1.0;

    if (CenterIsTransparent()) {
      geo::CVector vcHorizontal((dMaxLeft - dMinLeft), 0, 0);
      geo::CVector vcVertical(0, m_rcLeftRect.Height(), 0);
      geo::CPoint ptBegin(ViewPortX(), ViewPortY(), 0);
      DrawObject(geo::CRectangle(ptBegin, vcHorizontal, vcVertical), CDrawDef(hotspot->MinColor()));

      vcHorizontal.X(dMaxRight - dMinRight);
      vcVertical.Y(m_rcRightRect.Height());
      ptBegin.X(dMinRight);
      DrawObject(geo::CRectangle(ptBegin, vcHorizontal, vcVertical), CDrawDef(hotspot->MaxColor()));
    } else {

      CColorScaleDrawDef DrawDef;
      DrawDef.ColorScaleDirection(CColorScaleDrawDef::CS_X_DIRECTION);
      DrawDef.AddPair(dMinLeft, hotspot->MinColor());
      DrawDef.AddPair(dMaxLeft, hotspot->MaxColor());

      geo::CVector vcHorizontal((dMaxLeft - dMinLeft), 0, 0);
      geo::CVector vcVertical(0, m_rcLeftRect.Height(), 0);
      geo::CPoint ptBegin(dMinLeft, ViewPortY(), 0);
      DrawObject(geo::CRectangle(ptBegin, vcHorizontal, vcVertical), DrawDef);

      vcHorizontal.X(dMaxRight - dMinRight);
      vcVertical.Y(m_rcRightRect.Height());
      ptBegin.X(dMinRight);
      DrawObject(geo::CRectangle(ptBegin, vcHorizontal, vcVertical), CDrawDef(hotspot->MaxColor()));
    }
    return;
  }

  assert(false); // Never get here...
}

void COpenGLDlgView::OnCreate(LPCREATESTRUCT lpCreateStruct) { COpenGLView::OnCreate(lpCreateStruct); }

void COpenGLDlgView::SetHotSpotRects(CRect &LeftRect, CRect &RightRect) {
  m_rcLeftRect = LeftRect;
  m_rcRightRect = RightRect;
}

int COpenGLDlgView::OnMouseActivate(CWnd *pDesktopWnd, unsigned int nHitTest, unsigned int message) {
  int nResult = CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
  if (nResult == MA_NOACTIVATE || nResult == MA_NOACTIVATEANDEAT)
    return nResult; // frame does not want to activate

  return nResult;
}
