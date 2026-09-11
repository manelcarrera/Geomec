// geomecstatusbar.cpp : implementation file
//

#include "stdafx.h"
#include "geomec.h"
#include "geomecstatusbar.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CGeomecStatusBar

//##ModelId=3BE7AEC202D4
CGeomecStatusBar::CGeomecStatusBar()
: CStatusBar()
{
}

BEGIN_MESSAGE_MAP(CGeomecStatusBar, CStatusBar)
	//{{AFX_MSG_MAP(CGeomecStatusBar)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGeomecStatusBar message handlers

//##ModelId=3BE7AEC202D5
void CGeomecStatusBar::OnLButtonDown(unsigned int nFlags, CPoint point) 
{
	CRect rect;
	GetItemRect(CommandToIndex(ID_INDICATOR_UNIT), rect);

	if(rect.PtInRect(point))
	{
		CGeomecDoc& doc = ((CGeomecApp*)AfxGetApp())->GetDoc();
	
		switch(doc.UnitNode().Unit())
		{
		case CQuantity::SI_UNIT:
			doc.UnitNode().Unit(CQuantity::FIELD_UNIT);
			break;
		case CQuantity::FIELD_UNIT:
			doc.UnitNode().Unit(CQuantity::SI_UNIT);
			break;
		default:
			break;
		}
	}
	
	CWnd::OnLButtonDown(nFlags, point);
}

int CGeomecStatusBar::GetNeededWidth(const CString& text)
{
	HFONT hFont = (HFONT)SendMessage(WM_GETFONT);
	CClientDC dcScreen(NULL);
	HGDIOBJ hOldFont = NULL;
	if (hFont != NULL)
		hOldFont = dcScreen.SelectObject(hFont);

	int nNeededWidth = dcScreen.GetTextExtent(text).cx;

	if (hOldFont != NULL)
		dcScreen.SelectObject(hOldFont);

	return nNeededWidth;
}

int CGeomecStatusBar::GetWidth(int nPaneIndex)
{
	unsigned int nID;
	unsigned int nStyle;
	int cxWidth;

	GetPaneInfo(nPaneIndex, nID, nStyle, cxWidth);

	return cxWidth;
}

void CGeomecStatusBar::SetWidth(int nPaneIndex, int nWidth)
{
	unsigned int nID;
	unsigned int nStyle;
	int cxWidth;

	GetPaneInfo(nPaneIndex, nID, nStyle, cxWidth);
	SetPaneInfo(nPaneIndex, nID, nStyle, nWidth);
}
