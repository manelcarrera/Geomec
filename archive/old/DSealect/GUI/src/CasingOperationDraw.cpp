// CasingOperationDraw.cpp: implementation of the CCasingOperationDraw class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dsealect.h"
#include "DSealectDoc.h"
#include "DSModelDraw.h"
#include "CasingOperationDraw.h"
#include <sstream>

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CCasingOperationDraw, CDSealectDrawView)

//##ModelId=3C0DC1600244
CCasingOperationDraw::CCasingOperationDraw()
{

}

//##ModelId=3C0DC1600245
CCasingOperationDraw::~CCasingOperationDraw()
{

}

BEGIN_MESSAGE_MAP(CCasingOperationDraw, CDSealectDrawView)
	//{{AFX_MSG_MAP(CCasingOperationDraw)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//##ModelId=3C0DC1600251
void CCasingOperationDraw::OnDraw(CDC* pDC) 
{
	DrawCrossSectGeom(pDC);
	
}

//##ModelId=3DDA0C5D0274
void CCasingOperationDraw::OnSize(UINT nType, int cx, int cy) 
{
	CDSealectDrawView::OnSize(nType, cx, cy);
	
	SetScrollSizes(MM_TEXT,CSize(700,700));
	
}
