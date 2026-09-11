#include "StdAfx.h"
#include "Geomec.h"

#include "GeomecSplitterWnd.h"

BEGIN_MESSAGE_MAP(CGeomecSplitterWnd, CSplitterWnd)
	ON_WM_MOUSEWHEEL()
    ON_WM_KEYDOWN()
    ON_WM_KEYUP()
END_MESSAGE_MAP()

CGeomecSplitterWnd::CGeomecSplitterWnd()
: m_bRecursionLock(false)
{
}

BOOL CGeomecSplitterWnd::OnMouseWheel(unsigned int nFlags, short zDelta, CPoint pt)
{
	if(m_bRecursionLock)
		return FALSE;

	m_bRecursionLock = true;

	BOOL bRet = FALSE;

	CWnd* pPane = GetPaneFromPoint(pt);
	if(pPane)
		bRet = (pPane->SendMessage(WM_MOUSEWHEEL, MAKEWPARAM(nFlags, zDelta), MAKELPARAM(pt.x, pt.y)) != 0);

	m_bRecursionLock = false;

	return bRet;
}

CWnd* CGeomecSplitterWnd::GetPaneFromPoint(const CPoint& pt)
{
	int row;
	int col;

	for(row = 0; row < GetRowCount(); ++row)
	{
		for(col = 0; col < GetColumnCount(); ++col)
		{
			CWnd* pPane = GetPane(row, col);
			if(pPane)
			{
				CRect rect;
				pPane->GetWindowRect(&rect);
				if(rect.left <= pt.x && rect.right  >= pt.x &&
				   rect.top  <= pt.y && rect.bottom >= pt.y)
				{
					return pPane;
				}
			}
		}
	}

	return 0;
}

afx_msg void CGeomecSplitterWnd::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{ 
    CWnd* pPane =GetPane(0, 1);
    pPane->SendMessage(WM_KEYDOWN, nChar, MAKEWPARAM(nRepCnt, nFlags));
}

afx_msg void CGeomecSplitterWnd::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags )
{ 
    CWnd* pPane =GetPane(0, 1);
    pPane->SendMessage(WM_KEYUP, nChar, MAKEWPARAM(nRepCnt, nFlags));
}