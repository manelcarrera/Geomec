// MeshRefinementDraw.cpp: implementation of the CMeshRefinementDraw class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dsealect.h"
#include "DSModelDraw.h"
#include "MeshRefinementDraw.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CMeshRefinementDraw, CDSealectDrawView)

//##ModelId=3C0DC1590050
CMeshRefinementDraw::CMeshRefinementDraw()
{

}

//##ModelId=3C0DC1590051
CMeshRefinementDraw::~CMeshRefinementDraw()
{

}

BEGIN_MESSAGE_MAP(CMeshRefinementDraw, CDSealectDrawView)
	//{{AFX_MSG_MAP(CMeshRefinementDraw)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//##ModelId=3C0DC1590053
void CMeshRefinementDraw::OnDraw(CDC* pDC) 
{
	/*
	CRect lpR;
	GetClientRect(&lpR);
	
	CString strH;
	CString strW;
		
	strH.Format("%#4.1f",lpR.Size().cx);
	strW.Format("%#4.1f",lpR.Size().cy);
		
	pDC->TextOut(100,100,strH + ", " + strW);
	*/
	
	DrawMesh(pDC);	
}

//##ModelId=3DDA0C510070
void CMeshRefinementDraw::OnSize(UINT nType, int cx, int cy) 
{
	CDSealectDrawView::OnSize(nType, cx, cy);
	
	SetScrollSizes(MM_TEXT,CSize(700,700));
	
}
