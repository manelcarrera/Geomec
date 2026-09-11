// FormationDefinitionDraw.cpp: implementation of the CFormationDefinitionDraw class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "dsealect.h"
#include "DSealectDoc.h"
#include "Global.h"
#include "DSModelDraw.h"
#include "FormationDefinitionDraw.h"
#include <sstream>

#define getrandom( min, max ) ((rand() % (int)(((max) + 1) - (min))) + (min))

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CFormationDefinitionDraw, CDSealectDrawView)

//##ModelId=3C0DC15B009D
CFormationDefinitionDraw::CFormationDefinitionDraw()
{

}

//##ModelId=3C0DC15B009E
CFormationDefinitionDraw::~CFormationDefinitionDraw()
{

}

BEGIN_MESSAGE_MAP(CFormationDefinitionDraw, CDSealectDrawView)
	//{{AFX_MSG_MAP(CFormationDefinitionDraw)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//##ModelId=3C0DC15B00AC
void CFormationDefinitionDraw::OnDraw(CDC* pDC) 
{
	DrawFormations(pDC,SCRN_LEFT_PLOT,SCRN_TOP_PLOT , SCRN_BTTM_PLOT,SCRN_WIDTH_PLOT,true);
	DrawModelRangeLine(pDC,SCRN_LEFT_PLOT,SCRN_TOP_PLOT , SCRN_BTTM_PLOT,SCRN_WIDTH_PLOT);
		
}

//##ModelId=3C6A14C501C3
void CFormationDefinitionDraw::OnInitialUpdate() 
{
	m_bMouseIsDown=false;
	CDSealectDrawView::OnInitialUpdate();

}

//##ModelId=3C6A14C501C5
BOOL CFormationDefinitionDraw::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

}


//##ModelId=3C0DC15B00BD
void CFormationDefinitionDraw::OnLButtonDown(UINT nFlags, CPoint point) 
{

}

//##ModelId=3C0DC15B00AF
void CFormationDefinitionDraw::OnMouseMove(UINT nFlags, CPoint point) 
{
	CDSealectDrawView::OnMouseMove(nFlags, point);
}

//##ModelId=3C0DC15B00B3
void CFormationDefinitionDraw::OnLButtonUp(UINT nFlags, CPoint point) 
{
	
	CDSealectDrawView::OnLButtonUp(nFlags, point);
}

//##ModelId=3DDA0C5400AF
void CFormationDefinitionDraw::OnSize(UINT nType, int cx, int cy) 
{
	CDSealectDrawView::OnSize(nType, cx, cy);
	
	SetScrollSizes(MM_TEXT,CSize(700,700));
	
	
}
