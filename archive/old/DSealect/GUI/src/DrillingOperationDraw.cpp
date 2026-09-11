// DrillingOperationDraw.cpp: implementation of the CDrillingOperationDraw class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dsealect.h"
#include "DSealectGlobal.h"
#include "DSModelDraw.h"
#include "DrillingOperationDraw.h"
#include <sstream>
#include "CDataModel.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CDrillingOperationDraw, CDSealectDrawView)

//##ModelId=3C0DC15D0205
CDrillingOperationDraw::CDrillingOperationDraw()
{
	m_pAxisXInclination = 0;
	m_pAxisXAzimuthWNorth = 0;
	m_pAxisX3D = 0;
	
}

//##ModelId=3C0DC15D0206
CDrillingOperationDraw::~CDrillingOperationDraw()
{
	if(m_pAxisXInclination)
		delete m_pAxisXInclination;
	if(m_pAxisXAzimuthWNorth)
		delete m_pAxisXAzimuthWNorth;
	if(m_pAxisX3D)
		delete m_pAxisX3D;
		
}

BEGIN_MESSAGE_MAP(CDrillingOperationDraw, CDSealectDrawView)
	//{{AFX_MSG_MAP(CDrillingOperationDraw)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//##ModelId=3C0DC15D0208
void CDrillingOperationDraw::OnDraw(CDC* pDC) 
{
//#if 0
	 if (GetDoc()->DSealectModel()->DrillingScheme()->GetDrillingDirectionSection(0)->WellInclination()->Undefined())
		 return;

	 if (GetDoc()->DSealectModel()->DrillingScheme()->GetDrillingDirectionSection(0)->AzimuthWithNorth()->Undefined())
		 return;

	double dIncl = GetDoc()->DSealectModel()->DrillingScheme()->GetDrillingDirectionSection(0)->WellInclination()->Value();
	m_pAxisXInclination->RotateOriginView(180,0,0);
	m_pAxisXInclination->Reset();
	m_pAxisXInclination->RotateZ(&dIncl);
	m_pAxisXInclination->ShowWindow(SW_SHOW);

	double dAzi = GetDoc()->DSealectModel()->DrillingScheme()->GetDrillingDirectionSection(0)->AzimuthWithNorth()->Value();
	m_pAxisXAzimuthWNorth->Reset();
	m_pAxisXAzimuthWNorth->RotateZ(&dAzi);
	m_pAxisXAzimuthWNorth->ShowWindow(SW_SHOW);

	/*
	geo::CMatrix mNegRot;
	GetDoc()->DSealectModel()->CalculateNEgMatrix(mNegRot);

	
	SAFEARRAYBOUND rgsabound[1];
	rgsabound[0].lLbound = 0;
	rgsabound[0].cElements = 3;
	
	VARIANT Var[3];
	SAFEARRAY * psa[3];
	int i;
	

	for(i=0;i<3;i++)
	{
		VariantInit(&Var[i]);
		psa[i] = SafeArrayCreate(VT_R8, 1, rgsabound);

		for(long j=0;j<3;j++)
		{
			double val=mNegRot[j][i];
			SafeArrayPutElement(psa[i],&j,&val);
		}
		Var[i].vt= VT_ARRAY | VT_R8;
		Var[i].parray=psa[i];
	}

	m_pAxisX3D->SetAxisExt(Var[0],Var[1],Var[2]);
	
	for(i=0;i<3;i++)
	{
		SafeArrayDestroy(psa[i]);
	}

	m_pAxisX3D->ShowWindow(SW_SHOW);
*/
	
	//DrawAnalysisCoordSystem(pDC,4*SCRN_WIDTH_PLOT,SCRN_BTTM_PLOT+20,SCRN_BTTM_PLOT + 220,2*SCRN_WIDTH_PLOT,m_pAxisX3D);
//	DrawAnalysisCoordSystem(m_pAxisX3D);
	DrawFormations(pDC,200.0,SCRN_TOP_PLOT , SCRN_BTTM_PLOT,SCRN_WIDTH_PLOT,false);
	DrawWellPathPlasticElastic(pDC,200.0,SCRN_TOP_PLOT , SCRN_BTTM_PLOT,SCRN_WIDTH_PLOT);	
//#endif
}


//##ModelId=3DDA0C580090
BOOL CDrillingOperationDraw::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	BOOL ret =  CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

	BOOL btemp;
	m_pAxisXInclination = new CAxisX;
	btemp = m_pAxisXInclination->Create(0,WS_CHILD || WS_VISIBLE ,CRect(0,SCRN_BTTM_PLOT+20,2*SCRN_WIDTH_PLOT,SCRN_BTTM_PLOT + 250),this,2,NULL);
	m_pAxisXAzimuthWNorth = new CAxisX;
	btemp = m_pAxisXAzimuthWNorth->Create(0,WS_CHILD || WS_VISIBLE ,CRect(2*SCRN_WIDTH_PLOT,SCRN_BTTM_PLOT+20,4*SCRN_WIDTH_PLOT,SCRN_BTTM_PLOT + 250),this,2,NULL);
	m_pAxisX3D = new CAxisX;
	btemp = m_pAxisX3D->Create(0,WS_CHILD || WS_VISIBLE ,CRect(4*SCRN_WIDTH_PLOT,SCRN_BTTM_PLOT+20,6*SCRN_WIDTH_PLOT,SCRN_BTTM_PLOT + 250),this,2,NULL);

	m_pAxisXInclination->SetIsometricView(FALSE);
	m_pAxisXInclination->SetTitle("");
	m_pAxisXInclination->SetFooter1("");
	m_pAxisXInclination->SetFooter2("");
	m_pAxisXInclination->SetFooter3("      Inclination");
	m_pAxisXInclination->SetSigmaFooter("");
	m_pAxisXInclination->SetHideY(TRUE);
	m_pAxisXInclination->SetHideZ(TRUE);
	m_pAxisXInclination->SetHideZOrigin(TRUE);
	m_pAxisXInclination->SetDrawCircle(FALSE);
	m_pAxisXInclination->SetBorderStyle(0);
	m_pAxisXInclination->SetXLabel(" Well");
	m_pAxisXInclination->SetXLabelOrigin("\nVert.");
	m_pAxisXInclination->SetYLabelOrigin("Hor.");
	m_pAxisXInclination->SetAxisColor(RGB(0,0,255));
	m_pAxisXInclination->ShowWindow(SW_HIDE);

	m_pAxisXAzimuthWNorth->SetIsometricView(FALSE);      
	m_pAxisXAzimuthWNorth->SetTitle("");
	m_pAxisXAzimuthWNorth->SetFooter1("");
	m_pAxisXAzimuthWNorth->SetFooter2("");
	m_pAxisXAzimuthWNorth->SetFooter3("      Azimuth with North");
	m_pAxisXAzimuthWNorth->SetSigmaFooter("");
	m_pAxisXAzimuthWNorth->SetHideY(TRUE);
	m_pAxisXAzimuthWNorth->SetHideZ(TRUE);
	m_pAxisXAzimuthWNorth->SetDrawCircle(FALSE);
	m_pAxisXAzimuthWNorth->SetBorderStyle(0);
	m_pAxisXAzimuthWNorth->SetHideZOrigin(TRUE);
	m_pAxisXAzimuthWNorth->SetXLabel("  Well");
	m_pAxisXAzimuthWNorth->SetXLabelOrigin("  N");
	m_pAxisXAzimuthWNorth->SetYLabelOrigin("E");
	m_pAxisXAzimuthWNorth->SetAxisColor(RGB(0,0,255));
	m_pAxisXAzimuthWNorth->ShowWindow(SW_HIDE);

	m_pAxisX3D->SetIsometricView(FALSE);
	m_pAxisX3D->SetDrawCircle(TRUE);
	m_pAxisX3D->SetTitle("");
	m_pAxisX3D->SetFooter1("");
	m_pAxisX3D->SetFooter2("");
	m_pAxisX3D->SetFooter3("Analysis Coord System");
	m_pAxisX3D->SetSigmaFooter("");
	m_pAxisX3D->SetBorderStyle(0);
	m_pAxisX3D->SetXLabel("N");
	m_pAxisX3D->SetYLabel("E");
	m_pAxisX3D->SetZLabel("g");
	m_pAxisX3D->SetXLabelOrigin("X");
	m_pAxisX3D->SetYLabelOrigin("Y");
	m_pAxisX3D->SetZLabelOrigin("Well");
	m_pAxisX3D->SetAxisColor(RGB(0,0,255));
	m_pAxisX3D->ShowWindow(SW_HIDE);
	
	return ret;
}

//##ModelId=3DDA0C5800BF
void CDrillingOperationDraw::OnSize(UINT nType, int cx, int cy) 
{
	CDSealectDrawView::OnSize(nType, cx, cy);
	
	SetScrollSizes(MM_TEXT,CSize(700,700));
	
}
