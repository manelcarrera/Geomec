// InsituStressesDraw.cpp: implementation of the CInsituStressesDraw class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dsealect.h"
#include "DSealectGlobal.h"
#include "Global.h"
#include "DSModelDraw.h"
#include "InsituStressesDraw.h"
#include "CFormation.h"
#include "CFormationTopography.h"
#include "CDataModel.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

static WCHAR chart2dLicenseKey[] =
{
	0x0043,	0x006F,	0x0070,	0x0079,	0x0072,	0x0069,	
	0x0067,	0x0068,	0x0074,	0x0020,	0x0028,	0x0063,	
	0x0029,	0x0020,	0x0031,	0x0039,	0x0039,	0x0036,	
	0x0020,	0x004B,	0x004C,	0x0020,	0x0047,	0x0072,	
	0x006F,	0x0075,	0x0070,	0x0020,	0x0049,	0x006E,	
	0x0063,	0x002E
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CInsituStressesDraw, CDSealectDrawView)

//##ModelId=3C0DC15A00FC
CInsituStressesDraw::CInsituStressesDraw()
{
	m_bIsLineDrawn=false;
	m_bVerticalScroll=false;
}

//##ModelId=3C0DC15A0109
CInsituStressesDraw::~CInsituStressesDraw()
{	
	delete m_pChart;
	delete m_pAxis;
}

BEGIN_MESSAGE_MAP(CInsituStressesDraw, CDSealectDrawView)
	//{{AFX_MSG_MAP(CInsituStressesDraw)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CANCELMODE()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//##ModelId=3C0DC15A010B
void CInsituStressesDraw::OnDraw(CDC* pDC) 
{	
	TRACE("********* Ondraw\n");
	DrawFormations(pDC,SCRN_LEFT_PLOT, SCRN_TOP_PLOT , SCRN_BTTM_PLOT ,SCRN_WIDTH_PLOT,true);
	DrawStresses(pDC,SCRN_LEFT_PLOT,SCRN_WIDTH_PLOT,SCRN_TOP_PLOT, SCRN_BTTM_PLOT);
	m_bIsLineDrawn=false;
	if(!m_bVerticalScroll)
		DrawDepthLine();
	
}

//##ModelId=3DDA0C5201D8
void CInsituStressesDraw::DrawCoordSystem(CFormation *pFormation)
{
	double dAzi;
	if(pFormation)
	{
		if(pFormation->AzimuthOfSHWithNorth()->Undefined())
		{
			return;
		}
		else
		{
			dAzi= pFormation->AzimuthOfSHWithNorth()->Value();
		}
	}
	else
	{
		dAzi=0;
	}
	
	m_pAxis->Reset();
	m_pAxis->RotateZ(&dAzi);
	
}

//##ModelId=3C6A14C203A7
BOOL CInsituStressesDraw::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	BOOL ret= CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
	
	BOOL btemp;

	m_pAxis =new CAxisX;
	
	btemp = m_pAxis->Create(0,WS_CHILD || WS_VISIBLE ,CRect(0,0,100,100),this,0);
	m_pAxis->SetBorderStyle(0);
	m_pAxis->SetIsometricView(FALSE);
	m_pAxis->SetShowOrigin(TRUE);
	m_pAxis->SetDrawCircle(TRUE);
	m_pAxis->SetAxisColor(RGB(0,0,255));
	m_pAxis->SetTitle("");
	m_pAxis->SetFooter1("");
	m_pAxis->SetFooter2("");
	m_pAxis->SetFooter3("Analysis Coord System");
	m_pAxis->SetSigmaFooter("");
	m_pAxis->SetXLabel("H");
	m_pAxis->SetYLabel("h");
	m_pAxis->SetZLabel("");
	m_pAxis->SetXLabelOrigin("N");
	m_pAxis->SetYLabelOrigin("E");
	m_pAxis->SetZLabelOrigin("");
	m_pAxis->ShowWindow(SW_HIDE);

	
	BSTR bstrLicense = ::SysAllocStringLen(chart2dLicenseKey,
       sizeof(chart2dLicenseKey)/sizeof(WCHAR));

	m_pChart = new CChart2D;

	btemp = m_pChart->Create(0,WS_CHILD || WS_VISIBLE ,CRect(0,0,100,100),this,1,NULL,FALSE,bstrLicense);
	
	::SysFreeString(bstrLicense); 	

	//m_pChart->SetIsBatched(FALSE);
	CString ChartInitFile=GetAppPath() + "chart2.oc2";
	BOOL bLoad = m_pChart->Load(ChartInitFile);
	
	int top=500;
	int width=300;
	int height=300;
	m_pChart->MoveWindow(0,top,width,height);
	
	m_pAxis->MoveWindow(1.2*width,top,2*width/3,2*height/3);
//m_pAxis->ShowWindow(FALSE);
		
	return ret;

}

//##ModelId=3C6A14C3001D
void CInsituStressesDraw::OnInitialUpdate() 
{
	m_bMouseIsDown = false;
	m_lastY = SCRN_TOP_PLOT; // set line default on top of plot
	
	CDSealectDrawView::OnInitialUpdate();
	CRect rc;
	this->GetClientRect(&rc);
		
	long width,height;
	width=rc.right-rc.left;
	height=rc.bottom-rc.top;
	//width=width/2;
	//m_pAxis->MoveWindow(0,0,width,height);
	m_pChart->ShowWindow(FALSE);
	m_pAxis->ShowWindow(FALSE);

}

//##ModelId=3C6A14C3002D
void CInsituStressesDraw::OnSize(UINT nType, int cx, int cy) 
{
	//CDSealectDrawView::OnSize(nType, cx, cy);
	//CScrollView::OnSize(nType, cx, cy);
	SetScrollSizes(MM_TEXT,CSize(1000,1000));
}

//##ModelId=3C0DC15A012A
void CInsituStressesDraw::OnLButtonDown(UINT nFlags, CPoint point) 
{
	TRACE("LDown x,y %d %d\n",point.x,point.y);
	m_bMouseIsDown = true;
	Invalidate();			
	DrawDepthLine(point.y);
}

//##ModelId=3C0DC15A011C
void CInsituStressesDraw::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_bMouseIsDown = false;
		
}
//##ModelId=3CA011A10261
void CInsituStressesDraw::DrawDepthLine(double dYVal)
{
	//get the current unit system.
	CDSealectDoc* pDoc=(CDSealectDoc*)GetDoc();
	QU::UNIT us = pDoc->UnitSystem();
	CLengthQuantity tmpLQ;
	CString sUnit;
	sUnit = GetUnitString(&tmpLQ);

	TRACE("DrawLine\n");
	CPoint ptScrll =  GetScrollPosition(); 

	double dXS = 70 - ptScrll.x;
	double dXE = 600 - ptScrll.x;

	TRACE( "dY:      %f \n" ,dYVal);
	TRACE( "pointX:      %f \n" ,ptScrll.x);
	TRACE( "pointY:      %f \n" ,ptScrll.y);

	if(dYVal == -1.0)
	{
		//from Draw()
		dYVal = m_lastY;
	}

	double firstWorldY;
	double firstY = dYVal;
	double lastY = m_lastY;

	Screen2TVD(firstY + GetScrollPosition().y, firstWorldY,SCRN_TOP_PLOT,SCRN_BTTM_PLOT);
	
	CFormationTopography * pFT = GetDoc()->DSealectModel()->FormationTopography();
	CFormation * pFormation = pFT->GetFormationAtDepth(firstWorldY);
	DrawCoordSystem(pFormation);
		
	CClientDC dc(this);
	dc.SetROP2(R2_NOTXORPEN);

	if(!GetDoc()->DSealectModel()->FormationTopography()->CheckComplete())
	{
		if(m_bIsLineDrawn)
		{
			dc.MoveTo(dXS, lastY);
			dc.LineTo(dXE,lastY);
			m_bIsLineDrawn = false;
		}
		m_pChart->ShowWindow(FALSE);
		m_pAxis->ShowWindow(FALSE);
		return;
	}

	if(pFormation)
	{
		m_pChart->ShowWindow(SW_SHOW);
		m_pAxis->ShowWindow(SW_SHOW);
		DrawMohrChart(m_pChart,firstWorldY);
		
		if(m_bIsLineDrawn)
		{
			dc.MoveTo(dXS, lastY);
			dc.LineTo(dXE,lastY);
		}
		m_bIsLineDrawn = true;
		dc.MoveTo(dXS, firstY);
		dc.LineTo(dXE,firstY);
		m_lastY = firstY;
		
		double dRealYVal; // show real current depth 
		Screen2TVD(dYVal + GetScrollPosition().y,dRealYVal,SCRN_TOP_PLOT,SCRN_BTTM_PLOT); //convert from screen to real depth
		tmpLQ.Value(dRealYVal,CDoubleQuantity::SI_UNIT);
		CString sRealYVal;
		sRealYVal.Format("%#4.1f",tmpLQ.Value(us));
		dc.TextOut(150 - GetScrollPosition().x ,500 - GetScrollPosition().y, "Depth: " + sRealYVal + " " + sUnit);
	}
	else
	{
		if(m_bIsLineDrawn)
		{
			dc.MoveTo(dXS, lastY);
			dc.LineTo(dXE, lastY);
			m_bIsLineDrawn = false;
		}
	}

}		

//##ModelId=3C0DC15A010E
void CInsituStressesDraw::OnMouseMove(UINT nFlags, CPoint point) 
{
	TRACE("LMove x,y %d %d\n",point.x,point.y);
	if(m_bMouseIsDown)
	{
		TRACE("MouseMove\n");
		DrawDepthLine(point.y);
	
	}
	
}

//##ModelId=3DDA0C5201E7
void CInsituStressesDraw::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	m_bVerticalScroll = true;
	CDSealectDrawView::OnVScroll(nSBCode, nPos, pScrollBar);
	m_bVerticalScroll = false;
}
