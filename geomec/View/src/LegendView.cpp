// LegendView.cpp : implementation file
//
#include "stdafx.h"
#include <cfloat>
#include "geomec.h"
#include "LegendView.h"
#include "OpenGLSceneBase.h"
#include "ColorScaleEntry.h"
#include "GeomecDoc.h"
#include "IColorScaleNode_Delegate.h"
#include "ColorScaleEntry_Delegate.h"
#include "ColorGradient_Delegate.h"
#include "HotSpot_Delegate.h"
#include "ColorGradient.h"
#include "HotSpot.h"
#include "FemAppModel.h"
#include "FemAppEntryTypes.h"
#include "ISceneWrapper.h"
#include "FemAppMainWindow.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif




/////////////////////////////////////////////////////////////////////////////
// CLegendView

IMPLEMENT_DYNCREATE(CLegendView, CScrollView)

//##ModelId=3B6549E202B1
CLegendView::CLegendView()
{
}

//##ModelId=3B6549E202B2
CLegendView::~CLegendView()
{
}

BEGIN_MESSAGE_MAP(CLegendView, COpenGLView)
	ON_WM_RBUTTONUP()
  ON_WM_SIZE()
	ON_WM_CREATE()
  ON_WM_HSCROLL()
  ON_WM_VSCROLL()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLegendView diagnostics


#ifdef _DEBUG
//##ModelId=3B6549E202B4
void CLegendView::AssertValid() const
{
	COpenGLView::AssertValid();
}

//##ModelId=3B6549E202C2
void CLegendView::Dump(CDumpContext& dc) const
{
	COpenGLView::Dump(dc);
}
#endif //_DEBUG

#define ID_CHECK_GLOBAL		1
#define ID_CHECK_LOCAL		2
#define ID_CHECK_HOTSPOT	3
#define ID_ATTRIBUTES		4
#define ID_CREATE_HOTSPOT	5
#define ID_CREATE_GLOBAL	6
#define ID_CS_ATTRIB		7


void CLegendView::OnRButtonUp(unsigned int nFlags, CPoint screenpoint) 
{
    ClientToScreen(&screenpoint);
    CGeomecDoc& doc = dynamic_cast<CGeomecDoc&> (*GetDocument());

    return DoOnRButtonUp(screenpoint, doc);

}

void CLegendView::DoOnRButtonUp( CPoint &screenpoint, CGeomecDoc& doc )
{
    if(doc.CurrentScene())
    {
        // If we're showing color return (No menu is needed)
        if(doc.CurrentScene()->ShowColor())
            return;

        // Get the entry
        CColorScaleEntry *pEntry = dynamic_cast<CColorScaleEntry*>(doc.CurrentScene()->Model().GraphEntry(MD_BASE_COLOR_SCALE));
        CColorScaleEntry_Delegate *pEntry_Delegate =
            dynamic_cast <CColorScaleEntry_Delegate*> (pEntry->getDelegate());
        assert(pEntry_Delegate);
        assert(pEntry);

        // Now we build a menu
        CMenu menu;
        menu.CreatePopupMenu();

        // do create menu
        //wjrx mantis 2584 menu.AppendMenu(MF_UNCHECKED, ID_CREATE_GLOBAL, _T("Create global gradient"));
        menu.AppendMenu(MF_UNCHECKED, ID_CREATE_HOTSPOT, _T("Create hotspot"));		
        menu.AppendMenu(MF_SEPARATOR);
        menu.AppendMenu(MF_UNCHECKED, ID_CS_ATTRIB, _T("Color scales..."));		
        menu.AppendMenu(MF_SEPARATOR);
        menu.AppendMenu(MF_STRING, ID_CHECK_LOCAL, _T("&Local"));
        menu.AppendMenu(MF_STRING, ID_CHECK_GLOBAL, _T("&Global"));
        menu.AppendMenu(MF_STRING, ID_CHECK_HOTSPOT, _T("&Hotspot"));

        // Config the memu
        switch(pEntry->ColorScaleType())
        {
        case CColorScaleEntry::GLOBAL:
            menu.CheckMenuItem(ID_CHECK_GLOBAL, MF_CHECKED | MF_BYCOMMAND);
            break;
        case CColorScaleEntry::LOCAL:
            menu.CheckMenuItem(ID_CHECK_LOCAL, MF_CHECKED | MF_BYCOMMAND);
            break;

        case CColorScaleEntry::HOTSPOT:
            menu.CheckMenuItem(ID_CHECK_HOTSPOT, MF_CHECKED | MF_BYCOMMAND);
            break;

        default:
            assert(false);
        }

        if(pEntry->GlobalColorGradient() == 0)
            menu.EnableMenuItem(ID_CHECK_GLOBAL, MF_GRAYED | MF_BYCOMMAND);

        if(pEntry->HotSpot() == 0)
            menu.EnableMenuItem(ID_CHECK_HOTSPOT, MF_GRAYED | MF_BYCOMMAND);

        menu.AppendMenu(MF_SEPARATOR);
        menu.AppendMenu(MF_STRING, ID_ATTRIBUTES, _T("&Attributes"));

        switch(menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_NONOTIFY | TPM_RETURNCMD, screenpoint.x, screenpoint.y, FemAppGetMainWnd()))
        {
        case ID_CHECK_GLOBAL:
            pEntry->ColorScaleType(CColorScaleEntry::GLOBAL);
            break;
        case ID_CHECK_LOCAL:
            pEntry->ColorScaleType(CColorScaleEntry::LOCAL);
            break;
        case ID_CHECK_HOTSPOT:
            pEntry->ColorScaleType(CColorScaleEntry::HOTSPOT);
            break;
        case ID_ATTRIBUTES:
            {
                IColorScaleNode_Delegate* colorScaleNode_Delegate = 0;

                switch (pEntry->ColorScaleType())
                {
                case CColorScaleEntry::LOCAL:
                    colorScaleNode_Delegate =
                        new CColorGradient_Delegate(pEntry->LocalColorGradient());
                    break;
                case CColorScaleEntry::GLOBAL:
                    colorScaleNode_Delegate =
                        new CColorGradient_Delegate(pEntry->GlobalColorGradient());
                    break;
                case CColorScaleEntry::HOTSPOT:
                    colorScaleNode_Delegate =
                        new CHotSpot_Delegate(pEntry->HotSpot());
                    break;
                default:
                    assert(FALSE);
                    break;
                }

                colorScaleNode_Delegate->Attributes();
                delete colorScaleNode_Delegate;
            }
            break;
        case ID_CREATE_GLOBAL:
            pEntry_Delegate->OnNewColorGradient();
            break;
        case ID_CREATE_HOTSPOT:
            pEntry_Delegate->OnNewHotSpot();
            break;
        case ID_CS_ATTRIB:
            pEntry_Delegate->Attributes();
            break;
        default:	
            // Do nothing
            break;
        }
    }
}

int CLegendView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (COpenGLView::OnCreate(lpCreateStruct) == -1)
		return -1;

  EnableScrollBarCtrl(SB_BOTH);

	return 0;
}

void CLegendView::OnSize(unsigned int nType, int cx, int cy) 
{
	OnResizeParent();
  UpdateScrollBars();
}

void CLegendView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  if(ChildSize() == 1)
  {
    CLegendFrame& lf = LegendFrame();
    int pos = lf.ScrollX();
    int rh = HorizontalScrollRange();

    switch(nSBCode)
    {
    case SB_LEFT:
      pos = 0;
      break;
    case SB_ENDSCROLL:
//      pos = nPos;
      break;
    case SB_LINELEFT:
      pos = (pos > 0 ? pos - 1 : pos);
      break;
    case SB_LINERIGHT:
      pos = (pos < rh ? pos + 1 : rh);
      break;
    case SB_PAGELEFT:
      pos = (pos > rh/10 ? pos - rh/10 : pos);
      break;
    case SB_PAGERIGHT:
      pos = (pos < rh - rh/10 ? rh + rh/10 : pos);
      break;
    case SB_RIGHT:
      pos = rh;
      break;
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
      pos = nPos;
      break;
    }

    SetScrollPos(SB_HORZ, pos);
    lf.setScrollX(pos);
    lf.UpdateFrame();
  }

  COpenGLView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CLegendView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  if(ChildSize() == 1)
  {
    CLegendFrame& lf = LegendFrame();
    int pos = lf.ScrollY();
    int rv = VerticalScrollRange();

    switch(nSBCode)
    {
    case SB_TOP:
      pos = 0;
      break;
    case SB_ENDSCROLL:
//      pos = nPos;
      break;
    case SB_LINEUP:
      pos = (pos > 0 ? pos - 1 : pos);
      break;
    case SB_LINEDOWN:
      pos = (pos < rv ? pos + 1 : rv);
      break;
    case SB_PAGEUP:
      pos = (pos > rv/10 ? pos - rv/10 : pos);
      break;
    case SB_PAGEDOWN:
      pos = (pos < rv - rv/10 ? rv + rv/10 : pos);
      break;
    case SB_BOTTOM:
      pos = rv;
      break;
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
      pos = nPos;
      break;
    }

    SetScrollPos(SB_VERT, pos);
    lf.setScrollY(pos);
    lf.UpdateFrame();
  }

  COpenGLView::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CLegendView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
  UpdateScrollBars();
}

void CLegendView::UpdateScrollBars()
{
  if(ChildSize() == 1)
  {
    // scroll ranges
    int rh = HorizontalScrollRange();
    int rv = VerticalScrollRange();

    // current scroll position
    CLegendFrame& lf = LegendFrame();
    int sh = lf.ScrollX();
    int sv = lf.ScrollY();

    // reset scroll if no range
    if(!rv)
      lf.setScrollY(sv = 0);

    if(!rh)
      lf.setScrollX(sh = 0);

    SCROLLINFO si;

    // horizontal bar...
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE /*| SIF_DISABLENOSCROLL*/;
    si.nMin = 0;
    si.nMax = rh;
    si.nPage = rh / 10;
    si.nPos = sh;
    SetScrollInfo(SB_HORZ, &si);

    // vertical bar...
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE /*| SIF_DISABLENOSCROLL*/;
    si.nMin = 0;
    si.nMax = rv;
    si.nPage = rv / 10;
    si.nPos = sv;
    SetScrollInfo(SB_VERT, &si);
  }
}

CLegendFrame& CLegendView::LegendFrame()
{
  assert(ChildSize() == 1);
  CLegendFrame& lf = static_cast<CLegendFrame&>(Child(0));
  return lf;
}

int CLegendView::HorizontalScrollRange()
{
    CLegendFrame& lf = LegendFrame();
    int w = lf.LegendWidth();

    CRect rect;
    GetClientRect(&rect);
    int cw = rect.Width();

    // horizontal scroll range
    int rw = (w - cw < 0 ? 0 : w - cw);

    return rw;
}

int CLegendView::VerticalScrollRange()
{
    CLegendFrame& lf = LegendFrame();
    int h = lf.LegendHeight();

    CRect rect;
    GetClientRect(&rect);
    int ch = rect.Height();

    // vertical scroll range
    int rh = (h - ch < 0 ? 0 : h - ch);

    return rh;
}
