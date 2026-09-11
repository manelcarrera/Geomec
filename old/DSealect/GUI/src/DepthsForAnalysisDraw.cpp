// DepthsForAnalysisDraw.cpp: implementation of the CDepthsForAnalysisDraw class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DSModelDraw.h"
#include "dsealect.h"
#include "DepthsForAnalysisDraw.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CDepthsForAnalysisDraw, CDSealectDrawView)

//##ModelId=3C0DC15E0040
CDepthsForAnalysisDraw::CDepthsForAnalysisDraw()
{

}

//##ModelId=3C0DC15E0041
CDepthsForAnalysisDraw::~CDepthsForAnalysisDraw()
{

}

BEGIN_MESSAGE_MAP(CDepthsForAnalysisDraw, CDSealectDrawView)
	//{{AFX_MSG_MAP(CDepthsForAnalysisDraw)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//##ModelId=3C0DC15E004E
void CDepthsForAnalysisDraw::OnDraw(CDC* pDC) 
{
	DrawFormations(pDC,100.0,SCRN_TOP_PLOT , SCRN_BTTM_PLOT,SCRN_WIDTH_PLOT,false);
	DrawWellPathLeadTail(pDC,100.0,SCRN_TOP_PLOT, SCRN_BTTM_PLOT,SCRN_WIDTH_PLOT);
	DrawAnalysisPoints(pDC,100.0,SCRN_TOP_PLOT, SCRN_BTTM_PLOT,SCRN_WIDTH_PLOT);
		
}

//##ModelId=3DDA0C5A0003
void CDepthsForAnalysisDraw::OnSize(UINT nType, int cx, int cy) 
{
	CDSealectDrawView::OnSize(nType, cx, cy);
	
	SetScrollSizes(MM_TEXT,CSize(700,700));
	
}
