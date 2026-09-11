// CementOperationDraw.cpp: implementation of the CCementOperationDraw class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dsealect.h"
#include "DSModelDraw.h"
#include "CementOperationDraw.h"
#include "DSealectDoc.h"
#include <sstream>

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CCementOperationDraw, CDSealectDrawView)

//##ModelId=3C0DC15F01A6
CCementOperationDraw::CCementOperationDraw()
{

}

//##ModelId=3C0DC15F01A7
CCementOperationDraw::~CCementOperationDraw()
{
}

BEGIN_MESSAGE_MAP(CCementOperationDraw, CDSealectDrawView)
	//{{AFX_MSG_MAP(CCementOperationDraw)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//##ModelId=3C0DC15F01A9
void CCementOperationDraw::OnDraw(CDC* pDC) 
{
	DrawFormations(pDC,SCRN_LEFT_PLOT,SCRN_TOP_PLOT, SCRN_BTTM_PLOT,SCRN_WIDTH_PLOT,false);
	DrawWellPathLeadTail(pDC,SCRN_LEFT_PLOT,SCRN_TOP_PLOT, SCRN_BTTM_PLOT,SCRN_WIDTH_PLOT);

}

//##ModelId=3DDA0C5B02A3
void CCementOperationDraw::OnSize(UINT nType, int cx, int cy) 
{
	CDSealectDrawView::OnSize(nType, cx, cy);
	
	SetScrollSizes(MM_TEXT,CSize(700,700));
	
}
