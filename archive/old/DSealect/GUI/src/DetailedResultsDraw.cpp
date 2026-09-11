// DetailedResultsDraw.cpp: implementation of the CDetailedResultsDraw class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dsealect.h"
#include "Axis.h"
#include "DSModelDraw.h"
#include "2DCONST.h"
#include "Chart2DData.h"
#include "ChartArea.h"
#include "ChartGroup.h"
#include "ChartLabel.h"
#include "ChartLabelCollection.h"
#include "AxisCollection.h"
#include "DerivedDouble.h"
#include "Label.h"
#include "LabelCollection.h"
#include "ValueLabelCollection.h"
#include "ValueLabel.h"
#include "Title.h"
#include "ChartGroupCollection.h"
#include "Legend.h"
#include "DSealectGlobal.h"
#include "Global.h"
#include "DetailedResultsForm.h"
#include "DetailedResultsDraw.h"
#include "SpreadFunctions.h"
#include "CBatchTable.h"
#include "CDataModel.h"
#include "CFormationTopography.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//Showmode
#define SHOWNONE	-1
#define BARCHART	0
#define SPREADSHEET	1
#define LINECHART	2
#define FGVVIEW		3

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// DetailedResultsDraw.cpp : implementation file
//

static WCHAR SpreadSheetLicenseKey[] =
{
	0x0043,	0x006F,	0x0070,	0x0079,	0x0072,	0x0069,	
	0x0067,	0x0068,	0x0074,	0x0020,	0x0028,	0x0063,	
	0x0029,	0x0020,	0x0031,	0x0039,	0x0039,	0x0035,	
	0x0020,	0x0046,	0x0061,	0x0072,	0x0050,	0x006F,	
	0x0069,	0x006E,	0x0074
};


static WCHAR chart2dLicenseKey[] =
{
	0x0043,	0x006F,	0x0070,	0x0079,	0x0072,	0x0069,	
	0x0067,	0x0068,	0x0074,	0x0020,	0x0028,	0x0063,	
	0x0029,	0x0020,	0x0031,	0x0039,	0x0039,	0x0036,	
	0x0020,	0x004B,	0x004C,	0x0020,	0x0047,	0x0072,	
	0x006F,	0x0075,	0x0070,	0x0020,	0x0049,	0x006E,	
	0x0063,	0x002E
};


/////////////////////////////////////////////////////////////////////////////
// CResultOverviewDraw

IMPLEMENT_DYNCREATE(CDetailedResultsDraw, CDSealectDrawView)

//##ModelId=3DDA0C59023A
CDetailedResultsDraw::CDetailedResultsDraw()
: m_pSpreadSheet(0),
	m_pBarChart(0),
	m_pLineChart(0),
	m_pFgvView(0),
	m_pFgvLegend(0),
	m_iShowMode(BARCHART),
	m_pAxis(0),
  m_pAnalysisPoint(0),
  m_pBatchTableRow(0),
  m_iResultType(-1),
  m_iScenario(-1),
  m_iTypeOfDamage(0),
  m_iPlotSpace(-1),
  m_iPresent(-1)
{
}

//##ModelId=3DDA0C590300
CDetailedResultsDraw::~CDetailedResultsDraw()
{
	if(m_pBarChart)
		delete m_pBarChart;

	if(m_pSpreadSheet)
		delete m_pSpreadSheet;

	if(m_pLineChart)
		delete m_pLineChart;

	if(m_pFgvView)
		delete m_pFgvView;

	if(m_pFgvLegend)
		delete m_pFgvLegend;

	if(m_pAxis)
		delete m_pAxis;
}


BEGIN_MESSAGE_MAP(CDetailedResultsDraw, CDSealectDrawView)
	//{{AFX_MSG_MAP(CDetailedResultsDraw)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_POPUP_COPY, OnPopupCopy)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//##ModelId=3DDA0C59033F
void CDetailedResultsDraw::OnMouseDownControl1(short Button, short Shift, long X, long Y) 
{

	if(Button == 2)
	{
		this->SetFocus();
		this->SetActiveWindow();
		CMenu menu;
		CMenu *submenu;
		// Load the menu.
		menu.LoadMenu(IDR_POPUPMENU_COPY);
		// Get the pop-up menu.
		submenu = menu.GetSubMenu(0);
		// Convert to screen coordinates.
		CPoint point(X,Y);
		ClientToScreen(&point);
		point.x = point.x - GetScrollPosition().x;
		point.y = point.y - GetScrollPosition().y;
		// Post the menu.
		submenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,
				point.x,    point.y,
				this,NULL); //AfxGetApp()->m_pMainWnd
	}
}

BEGIN_EVENTSINK_MAP(CDetailedResultsDraw, CDSealectDrawView)
    //{{AFX_EVENTSINK_MAP(CChartFormView)
	ON_EVENT(CDetailedResultsDraw, 1, -605 /* MouseDown */, OnMouseDownControl1, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CDetailedResultsDraw, 2, -605 /* MouseDown */, OnMouseDownControl1, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CDetailedResultsDraw, 3, -605 /* MouseDown */, OnMouseDownControl1, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


#define SS_ACTION_CLIPBOARD_COPY 22
#define SS_ACTION_SELECT_BLOCK   2
#define SS_ACTION_SEL_MODE_CLEAR   29


//##ModelId=3DDA0C59035E
void CDetailedResultsDraw::OnPopupCopy() 
{

	if(m_iShowMode == SPREADSHEET)
	{	
		CopyToClipboard(*m_pSpreadSheet);
		//m_pSpreadSheet->GetMaxCols();
		//m_pSpreadSheet->GetMaxRows();
		//m_pSpreadSheet->SetTypeTextWordWrap(TRUE);
		//m_pSpreadSheet->SetRow(0);
		//m_pSpreadSheet->SetCol(0);
		//m_pSpreadSheet->SetRow2(8);
		//m_pSpreadSheet->SetCol2(8);
		//m_pSpreadSheet->SetAction(SS_ACTION_SELECT_BLOCK);
		//m_pSpreadSheet->SetAction(SS_ACTION_CLIPBOARD_COPY);
		//m_pSpreadSheet->SetAction(SS_ACTION_SEL_MODE_CLEAR);
	}	
	else if(m_iShowMode == BARCHART)
	{
		m_pBarChart->CopyToClipboard(oc2dFormatBitmap);
	}
	else if(m_iShowMode == LINECHART)
	{
		m_pLineChart->CopyToClipboard(oc2dFormatBitmap);
	}
	else if(m_iShowMode == FGVVIEW)
	{
	}
}

//##ModelId=3DDA0C5902F1
void CDetailedResultsDraw::OnDraw(CDC* pDC)
{
	if(m_iPresent == 0)
	{
		if(m_iResultType == 0)
		{
			m_iShowMode = BARCHART;
		}
		else if(m_iResultType == 1)
		{
			m_iShowMode=SPREADSHEET;
		}
	}
	else if(m_iPresent == 1)
	{
		if(m_iPlotSpace == 0)
		{
			m_iShowMode=LINECHART;
		}
		else if(m_iPlotSpace == 1)
		{
			m_iShowMode= FGVVIEW;
		}
	}
	
	OnEraseBkgnd(pDC);

	if(m_pBatchTableRow->HasResults()) 
	{
		int iLoadPhase = m_pBatchTableRow->DSResult()->Space()->GetSpaceTable(m_iTypeOfDamage)->GetFirstLoadPhaseItemIndex();

		if(iLoadPhase < 0 && m_iShowMode == LINECHART)
		{
			m_iShowMode = -1;
			//pDC->TextOut(100,100,"Load not applied");
		}

		m_pBarChart->ShowWindow(m_iShowMode == BARCHART ? SW_SHOW: SW_HIDE);
		m_pLineChart->ShowWindow(m_iShowMode == LINECHART ? SW_SHOW: SW_HIDE);
		m_pSpreadSheet->ShowWindow(m_iShowMode == SPREADSHEET ? SW_SHOW: SW_HIDE);
				
		if(m_iShowMode == FGVVIEW)
		{
			if(m_pBatchTableRow->GetContourFileName(m_iTypeOfDamage).IsEmpty())
			{
				if(m_pFgvView->IsWindowVisible())
					m_pFgvView->ShowWindow(SW_HIDE);
				if(m_pFgvLegend->IsWindowVisible())
					m_pFgvLegend->ShowWindow(SW_HIDE);
				if(m_pAxis->IsWindowVisible())
					m_pAxis->ShowWindow(SW_HIDE);

				if(!m_pBatchTableRow->DSResult()->Space()->GetSpaceTable(m_iTypeOfDamage)->HasResults())
				{
					pDC->TextOut(100,100,"Intact");
				}
				else
				{
					pDC->TextOut(100,100,"No results available");
				}
			}
			else
			{
				if(!m_pFgvView->IsWindowVisible())
					m_pFgvView->ShowWindow(SW_SHOW);
				if(!m_pFgvLegend->IsWindowVisible())
					m_pFgvLegend->ShowWindow(SW_SHOW);
								
			}
		}
		else
		{
			//pDC->TextOut(100,100,"No results available");
			if(m_pFgvView->IsWindowVisible())
				m_pFgvView->ShowWindow(SW_HIDE);
			if(m_pFgvLegend->IsWindowVisible())
				m_pFgvLegend->ShowWindow(SW_HIDE);
			if(m_pAxis->IsWindowVisible())
				m_pAxis->ShowWindow(SW_HIDE);
			if(!m_pBatchTableRow->DSResult()->Space()->GetSpaceTable(m_iTypeOfDamage)->HasResults())
			{
				pDC->TextOut(100,100,"Intact");
			}
			else
			{
				pDC->TextOut(100,100,"No results available"); ///should never happen
			}
		}

		
		if(m_pBarChart->IsWindowVisible() || m_pLineChart->IsWindowVisible() || m_pSpreadSheet->IsWindowVisible() || m_pFgvView->IsWindowVisible())
		{
			if(!m_pBatchTableRow->CalculationSucceeded())
			{
				pDC->TextOut(0,0,"WARNING: Numerical analysis did not converge. Distrust result accuracy.");
			}
		}

		StartDraw();
		
	}
	else
	{
		m_iShowMode=SHOWNONE;
		m_pBarChart->ShowWindow(SW_HIDE);
		m_pLineChart->ShowWindow(SW_HIDE);
		m_pSpreadSheet->ShowWindow(SW_HIDE);

		if(m_pFgvView->IsWindowVisible())
			m_pFgvView->ShowWindow(SW_HIDE);
		if(m_pFgvLegend->IsWindowVisible())
			m_pFgvLegend->ShowWindow(SW_HIDE);
		if(m_pAxis->IsWindowVisible())
			m_pAxis->ShowWindow(SW_HIDE);
		pDC->TextOut(100,100,"No results available");
	
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDetailedResultsDraw diagnostics

#ifdef _DEBUG
//##ModelId=3DDA0C590302
void CDetailedResultsDraw::AssertValid() const
{
	CView::AssertValid();
}

//##ModelId=3DDA0C590304
void CDetailedResultsDraw::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDetailedResultsDraw message handlers

//##ModelId=3DDA0C5902B3
BOOL CDetailedResultsDraw::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	
	BOOL ret = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
	BSTR bstrLicense;
	BOOL btemp;

	bstrLicense = ::SysAllocStringLen(SpreadSheetLicenseKey,
       sizeof(SpreadSheetLicenseKey)/sizeof(WCHAR));
	m_pSpreadSheet = new CSpreadSheet;
	btemp = m_pSpreadSheet->Create(0,WS_CHILD || WS_VISIBLE ,CRect(0,0,100,100),this,1,NULL,FALSE,bstrLicense);
	::SysFreeString(bstrLicense); 

	//m_pSpreadSheet->SetEnabled(TRUE);
	m_pSpreadSheet->SetEditMode(FALSE);
	m_pSpreadSheet->SetLock(TRUE);
	m_pSpreadSheet->SetTypeEditLen(120); // set maximum string-length in cell to 120
	m_pSpreadSheet->SetGrayAreaBackColor(RGB(255,255,255));

	bstrLicense = ::SysAllocStringLen(chart2dLicenseKey,
       sizeof(chart2dLicenseKey)/sizeof(WCHAR));
	m_pBarChart = new CChart2D;
	btemp = m_pBarChart->Create(0,WS_CHILD || WS_VISIBLE ,CRect(0,0,100,100),this,2,NULL,FALSE,bstrLicense);
	::SysFreeString(bstrLicense); 	

	CString ChartInitFile=GetAppPath() + "Barchart.oc2"; //TODO temporary hard path
	BOOL bLoad = m_pBarChart->Load(ChartInitFile);

	bstrLicense = ::SysAllocStringLen(chart2dLicenseKey,
       sizeof(chart2dLicenseKey)/sizeof(WCHAR));
	m_pLineChart = new CChart2D;
	btemp = m_pLineChart->Create(0,WS_CHILD || WS_VISIBLE ,CRect(0,0,100,100),this,3,NULL,FALSE,bstrLicense);
	::SysFreeString(bstrLicense); 	

	m_pFgvView = new CFgvView;
	btemp = m_pFgvView->Create(0,WS_CHILD || WS_VISIBLE ,CRect(0,0,100,100),this,4,NULL);

	m_pFgvLegend = new CFgvView;
	btemp = m_pFgvLegend->Create(0,WS_CHILD || WS_VISIBLE ,CRect(0,0,100,100),this,5,NULL);

	m_pAxis = new CAxisX;
	btemp = m_pAxis->Create(0,WS_CHILD || WS_VISIBLE ,CRect(150,1000,150,150),this,6,NULL);

	m_pAxis->SetIsometricView(FALSE);
	m_pAxis->SetDrawCircle(TRUE);
	m_pAxis->SetTitle("");
	m_pAxis->SetFooter1("");
	m_pAxis->SetFooter2("");
	m_pAxis->SetFooter3("Analysis Coord System");
	m_pAxis->SetSigmaFooter("");
	m_pAxis->SetBorderStyle(0);
	m_pAxis->SetXLabel("N");
	m_pAxis->SetYLabel("E");
	m_pAxis->SetZLabel("g");
	m_pAxis->SetXLabelOrigin("X");
	m_pAxis->SetYLabelOrigin("Y");
	m_pAxis->SetZLabelOrigin("W");
	m_pAxis->SetAxisColor(RGB(0,0,255));
	m_pAxis->ShowWindow(SW_HIDE);

	//m_pFgvView->SetFgvWhiteBackground(TRUE);
	m_pFgvView->SetShowFrame(FALSE);
	m_pFgvView->SetShowContourLegend(FALSE);
	m_pFgvView->SetShowStatusMonitor(FALSE);
	m_pFgvView->SetShowResultsMonitor(FALSE);
	m_pFgvView->SetShowAxes(FALSE);
	m_pFgvView->SetFgvWhiteBackground(TRUE);
	
	//m_pFgvLegend->SetFgvWhiteBackground(TRUE);
	m_pFgvLegend->SetShowFrame(FALSE);
	m_pFgvLegend->SetShowResults(FALSE);
	m_pFgvLegend->SetShowAxes(FALSE);
	m_pFgvLegend->SetShowModel(FALSE);
	m_pFgvLegend->SetShowStatusMonitor(FALSE);
	m_pFgvLegend->SetShowResultsMonitor(FALSE);
	m_pFgvLegend->SetFgvWhiteBackground(TRUE);

	ChartInitFile=GetAppPath() + "Linechart.oc2"; //TODO temporary hard path
	bLoad = m_pLineChart->Load(ChartInitFile);

	m_pBarChart->ShowWindow(m_iShowMode == BARCHART ? SW_SHOW : SW_HIDE);
	m_pLineChart->ShowWindow(m_iShowMode == LINECHART ? SW_SHOW : SW_HIDE);
	m_pSpreadSheet->ShowWindow(m_iShowMode == SPREADSHEET ? SW_SHOW : SW_HIDE);
	m_pFgvView->ShowWindow(m_iShowMode == FGVVIEW ? SW_SHOW : SW_HIDE);
	m_pFgvLegend->ShowWindow(m_iShowMode == FGVVIEW ? SW_SHOW : SW_HIDE);
	m_pAxis->ShowWindow(m_iShowMode == FGVVIEW ? SW_SHOW : SW_HIDE);
	
	m_pSpreadSheet->MoveWindow(0,0,10000,10000);
	m_pBarChart->MoveWindow(0,0,700,700);
	m_pLineChart->MoveWindow(0,0,700,700);
	m_pFgvView->MoveWindow(0,0,600,600);
	m_pFgvLegend->MoveWindow(0,600,150,150);
	m_pAxis->MoveWindow(175,605,150,150);
	m_pLineChart->SetAllowUserChanges(FALSE);
	m_pBarChart->SetAllowUserChanges(FALSE);

	return ret;
}

//##ModelId=3DDA0C590310
void CDetailedResultsDraw::OnSize(UINT nType, int cx, int cy) 
{

	TRACE("cx = %d , cy = %d\n",cx,cy);
	int iMinimumSize = 500;
	if(cx > iMinimumSize && cy > iMinimumSize)
	{
		m_iScreenWidth = cx;
		m_iScreenHeight = cy;
	}
	else
	{
		m_iScreenWidth = iMinimumSize;
		m_iScreenHeight = iMinimumSize;
	}
	
	if(m_pFgvView)
	{
		SetScrollSizes(MM_TEXT,CSize(m_iScreenWidth, m_iScreenHeight));
		m_pFgvView->MoveWindow(0, 15, m_iScreenWidth - 170, m_iScreenHeight - 170);
		m_pFgvLegend->MoveWindow(0, m_iScreenHeight - 155, 150, 150);
		m_pAxis->MoveWindow(175, m_iScreenHeight - 150, 150, 150);
	}

	/*
	if(m_pFgvView)
		m_pFgvView->MoveWindow(0,0,cx,cy-150);
	if(m_pFgvLegend)
		m_pFgvLegend->MoveWindow(0,cy-150,150,150);
	if(m_pAxis)
		m_pAxis->MoveWindow(150,cy-150,150,150);
	*/				
	

	if(m_iShowMode = SPREADSHEET)
		SetScrollSizes(MM_TEXT,CSize(8000,8000));
	//if(m_iShowMode == FGVVIEW)
	//	SetScrollSizes(MM_TEXT,CSize(cx,cy));
	if(m_iShowMode == BARCHART)
		SetScrollSizes(MM_TEXT,CSize(2000,2000));
	if(m_iShowMode == LINECHART)
		SetScrollSizes(MM_TEXT,CSize(700,700));
		
	
}

//##ModelId=3DDA0C5902A3
void CDetailedResultsDraw::StartDraw()
{
  if(m_pAnalysisPoint)
  {
	  //m_vecRows = GetDoc()->DSealectModel()->BatchTable()->GetSucceededRows(m_pAnalysisPoint);
	  FillValidLoadPhaseVectors();
	  FillValidTablesVector();
	  FillPlotPointsVector();

	  //copy ValidLoadPhaseVector to temporary vector
	  m_vecTempLPV = m_vecValidLoadPhasesSealant;
  			
  	
	  if(m_iShowMode == SPREADSHEET)
	  {	
		  DrawTable();
	  }	
	  else if(m_iShowMode == BARCHART)
	  {
		  DrawBarChart();
	  }
	  else if(m_iShowMode == LINECHART)
	  {
		  DrawLineChart();
	  }
	  else if(m_iShowMode == FGVVIEW)
	  {
		  DrawFgvView();
	  }
  }
}

//##ModelId=3DDA0C5902B2
void CDetailedResultsDraw::DrawFgvView()
{
	CDSealectDoc* pDoc=(CDSealectDoc*)GetDoc();
	QU::UNIT us = pDoc->UnitSystem();

	SetScrollSizes(MM_TEXT,CSize(1000,1000)); //TODO
	
	CString sFileName = m_pBatchTableRow->GetContourFileName(m_iTypeOfDamage);

	if(sFileName.IsEmpty())		
	{
		if(m_pFgvView->IsWindowVisible())
			m_pFgvView->ShowWindow(SW_HIDE);
		if(m_pFgvLegend->IsWindowVisible())
			m_pFgvLegend->ShowWindow(SW_HIDE);
		if(m_pAxis->IsWindowVisible())
			m_pAxis->ShowWindow(SW_HIDE);

		//clears the prev. picture
		//m_pFgvView->SetFgvFileName(sFileName);
		//m_pFgvLegend->SetFgvFileName(sFileName);
	}
	else
	{
		m_pFgvView->SetFgvFileName(sFileName);
		m_pFgvLegend->SetFgvFileName(sFileName);

		if(m_iTypeOfDamage!=FORMATION_DEBONDING_TABLE && m_iTypeOfDamage!=CASING_DEBONDING_TABLE)
		{
			DrawAnalysisCoordSystem(m_pAxis, m_pAnalysisPoint->TMD()->Value());
			m_pFgvView->SetUseDefaultPallette(TRUE);
			
		}
		else
		{
			if(m_pAxis->IsWindowVisible())
				m_pAxis->ShowWindow(SW_HIDE);

			m_pFgvView->SetUseDefaultPallette(FALSE);
			m_pFgvView->ColorPalletteToUse(5); //this pallette has black io white so the grid has a different color than the background and will be visible

		}
		OnEraseBkgnd(GetDC());
		
		CBitmap bmp;
		double dLength;
		bool bFilled = false;
		if(m_iTypeOfDamage ==CASING_DEBONDING_TABLE)//3
		{
			bmp.LoadBitmap(IDB_CASING_DEBONDING);
			dLength = CircularCasingLength();
			bFilled = true;
		}
		else if(m_iTypeOfDamage == FORMATION_DEBONDING_TABLE )//2
		{
			bmp.LoadBitmap(IDB_FORMATION_DEBONDING);
			dLength = CircularBoreholeLength();
			bFilled = true;
		}
		
		CString sLength;		//to show the circular length around interface
		CString sHalfLength;	//to show half of the circular length around interface
		CRect rec;
		
		m_pFgvView->GetClientRect(rec);

		CClientDC dc(this);

		if(bFilled && !sFileName.IsEmpty())
		{
			DrawBitmap(&dc, bmp, 0, 150 - GetScrollPosition().x, rec.Height() - GetScrollPosition().y +20);
			sLength.Format("%#4.2f", dLength);
			sHalfLength.Format("%#4.2f", dLength / 2);

			GetDC()->TextOut(290 - GetScrollPosition().x, rec.Height() - GetScrollPosition().y +85,"0.0");
			GetDC()->TextOut(150 - GetScrollPosition().x, rec.Height() - GetScrollPosition().y +95, sHalfLength);
			GetDC()->TextOut(288 - GetScrollPosition().x, rec.Height() - GetScrollPosition().y +107, sLength);
		}

		CFont* pFont = dc.GetCurrentFont();

		int mode = dc.SetBkMode(OPAQUE );  

		CPoint p(350,m_iScreenHeight - 165);
		CPoint pHead(0,0);

		COLORREF oldbkcolor = dc.SetBkColor(RGB(255,255,255));//(RGB(0,0,0));
		COLORREF oldcolor = dc.SetTextColor(RGB(0,0,0));//(RGB(255,255,255));
		
		CString txt="";
		CString txtHead = CString(TypeOfDamage[m_iTypeOfDamage]);

		CString frmt;
		
		int rowcount=0;

		if(m_iTypeOfDamage == FORMATION_DEBONDING_TABLE || m_iTypeOfDamage == CASING_DEBONDING_TABLE )
		{
			txtHead = txtHead + ", in meters";
		}
		else
		{
			txtHead = txtHead + ", in strain unit";
		}

		frmt.Format("\nDepth along well = %#1.0f ",m_pAnalysisPoint->TMD()->Value(us));
		//frmt = frmt + m_pAnalysisPoint->TMD()->UnitName(us).;
		txt = txt + frmt;
		txt = txt + CString("\nCement material = ") + m_pBatchTableRow->CementMaterial()->Name();

		int LoadPhase = m_pBatchTableRow->DSResult()->Space()->GetSpaceTable(m_iTypeOfDamage)->GetFirstLoadPhaseItemIndex();
		if(LoadPhase>=0)
		{
			txt = txt + CString("\nLoad phase = ") + LoadPhaseNames[LoadPhase];
		}

		rec = CRect(0,0,1,1); // info bottom right
		dc.DrawText(txt, rec, DT_CALCRECT);
		int width = rec.Width();
		int height = rec.Height();
		rec.top = p.y - GetScrollPosition().y;
		rec.left = p.x - GetScrollPosition().x;
		rec.right = rec.left + width;
		rec.bottom = rec.top + height;
		dc.DrawText(txt, rec, DT_LEFT  );
		
		
		CRect recHead; //Header, top left
		recHead = CRect(0,0,1,1); 
		dc.DrawText(txtHead, recHead, DT_CALCRECT);
		int widthHead = recHead.Width();
		int heightHead= recHead.Height();
		recHead.top = pHead.y - GetScrollPosition().y;
		recHead.left = pHead.x - GetScrollPosition().x;
		recHead.right = recHead.left + width + 200;
		recHead.bottom = recHead.top + height;
		dc.DrawText(txtHead, recHead, DT_LEFT  );
		
		if(!m_pBatchTableRow->CalculationSucceeded())
		{
			CPoint pDist(350,m_iScreenHeight - 75);
			CString txtDist="Distrust result accuracy. Calculation diverged.";
			COLORREF redcolor = dc.SetTextColor(RGB(255,0,0));

			CRect recDist;
			recDist = CRect(0,0,1,1); // distrust warning bottom right
			dc.DrawText(txtDist, recDist, DT_CALCRECT);
			int widthDist = recDist.Width();
			int heightDist = recDist.Height();
			recDist.top = pDist.y - GetScrollPosition().y;
			recDist.left = pDist.x - GetScrollPosition().x;
			recDist.right = recDist.left + widthDist;
			recDist.bottom = recDist.top + heightDist;
			dc.DrawText(txtDist, recDist, DT_LEFT  );
		}

		dc.SetTextColor(oldcolor);
		dc.SetBkColor(oldbkcolor);
		dc.SetBkMode(mode);  
	}
	
	m_pFgvView->LoadPicture();
	m_pFgvLegend->LoadPicture();


}

void CDetailedResultsDraw::SetAnalysisPoint(CAnalysisPoint* pAnalysisPoint)
{
  m_pAnalysisPoint = pAnalysisPoint;
}

//##ModelId=3DDA0C5902A6
void CDetailedResultsDraw::DrawTable()
{	
	CDSealectDoc* pDoc=(CDSealectDoc*)GetDoc();
	QU::UNIT us = pDoc->UnitSystem();
	
	SetScrollSizes(MM_TEXT,CSize(8000,8000));

	CString sTmp;
	CString sAllFailedFalse = "Intact";
	CString sNoResults = "No result";
	CString sDistrust = "";

	BOOL bFailed;
	
	m_pSpreadSheet->SetReDraw(FALSE);

	m_pSpreadSheet->SetMaxCols(m_vecValidTables.size() + 2); //Nr of valid tables +2
	m_pSpreadSheet->SetMaxRows(m_vecTempLPV.size() + 2); // (NR_OF_LOADPHASES + 2);
	m_pSpreadSheet->SetColWidth(-1,16);
	m_pSpreadSheet->SetRowHeight(-1,50);
	m_pSpreadSheet->SetCol(0);
	m_pSpreadSheet->SetColHidden(TRUE);
	m_pSpreadSheet->SetRow(0);
	m_pSpreadSheet->SetRowHidden(TRUE);

	m_pSpreadSheet->SetCol(-1);
	m_pSpreadSheet->SetRow(-1);
	m_pSpreadSheet->SetTypeEditMultiLine(TRUE);

	if(!m_pBatchTableRow->CalculationSucceeded())
	{
		SetRedBackColorColRow(-1,-1,true);
		sDistrust = "Distrust result accuracy.\nCalculation diverged.";
	}
	else
	{
		SetRedBackColorColRow(-1,-1,false);
	}

	SetBackColorColRow(1,-1);
	SetBackColorColRow(2,-1);
	SetBackColorColRow(-1,1);
	SetBackColorColRow(-1,2);
	
	SetBorder(1,-1,1,15);		
	SetBorder(2,-1,1,15);
	SetBorder(-1,1,1,15);
	SetBorder(-1,2,1,15);

	m_pSpreadSheet->SetText(1,2,COleVariant(CString("Load type")));
	m_pSpreadSheet->SetText(2,1,COleVariant(CString("Damage type ->")));
	m_pSpreadSheet->SetText(2,2,COleVariant(CString("Sequence")));

	CCrackOpeningQuantity tmpCOQ;
	CString sUnitCrack;
	sUnitCrack = GetUnitString(&tmpCOQ);

	CStrainQuantity tmpSQ;  
	CString sUnitStrain;
	sUnitStrain = GetUnitString(&tmpSQ);
	
	CString sSequence = "";
	int iRememberShutIn = -1;

	for(int i = 0 ; i < m_vecTempLPV.size() ; i++) // for each valid loadphase
	{
		m_pSpreadSheet->SetText(1,i+3,COleVariant(CString(LoadPhaseNames[m_vecTempLPV[i]])));

		sSequence.Format("%i",i+1);
		//sequence number (starts with 1 and) is added with 1 until after shut-in, then unchanged  
		if(iRememberShutIn > -1) 
		{
			sSequence.Format("%i",iRememberShutIn+2);
		}
		if(m_vecTempLPV[i] == SHUT_IN)
		{
			iRememberShutIn = i;
		}
		m_pSpreadSheet->SetText(2,i+3,COleVariant(CString(sSequence)));
		
		for(int j = 0 ; j < m_vecValidTables.size() ; j++) // for each valid table
		{
			if(m_vecValidTables[j] == FORMATION_DEBONDING_TABLE || m_vecValidTables[j] == CASING_DEBONDING_TABLE || m_vecValidTables[j] == CEMENT_RADIAL_CRACKING_TABLE)
			{
				sTmp = TypeOfDamage[m_vecValidTables[j]] + CString("\n") + sUnitCrack;
			}
			else
			{
				sTmp = TypeOfDamage[m_vecValidTables[j]] + CString("\n") + sUnitStrain;
			}
			//m_pSpreadSheet->SetText(j+3,1,COleVariant(CString(TypeOfDamage[m_vecValidTables[j]])));
			m_pSpreadSheet->SetText(j+3,1,COleVariant(CString(sTmp)));

			m_pSpreadSheet->SetText(j+3,2,COleVariant(CString(sDistrust))); //could be empty or distrust_warning
			

			sTmp = "";
			bFailed = m_pBatchTableRow->DSResult()->GetFailedAtTableAndLoadphase(m_vecValidTables[j],m_vecTempLPV[i]);
			if(!bFailed)
			{	
				
				if(m_pBatchTableRow->DSResult()->GetCapacityAtTableAndLoadphase(m_vecValidTables[j],m_vecTempLPV[i])== -1)
				{	
					sTmp = sNoResults;
				}
				else
				{
					sTmp = sAllFailedFalse;
				}
			}
			else
			{	
				double dTmpDum = m_pBatchTableRow->DSResult()->GetDamageLevelAtTableAndLoadphase(m_vecValidTables[j],m_vecTempLPV[i]);
				
				if(m_vecValidTables[j] == FORMATION_DEBONDING_TABLE || m_vecValidTables[j] == CASING_DEBONDING_TABLE || m_vecValidTables[j] == CEMENT_RADIAL_CRACKING_TABLE)
				{
					//sTmp = TypeOfDamage[m_vecValidTables[j]] + CString("\n") + sUnitCrack;
					tmpCOQ.Value(dTmpDum,CDoubleQuantity::SI_UNIT);
					sTmp.Format("%#1.4g",tmpCOQ.Value(us));
				}
				else
				{
					//sTmp = TypeOfDamage[m_vecValidTables[j]] + CString("\n") + sUnitStrain;
					tmpSQ.Value(dTmpDum,CDoubleQuantity::SI_UNIT);
					sTmp.Format("%#1.4g",tmpSQ.Value(us));
				}
				
				//sTmp.Format("%#1.4g",m_pBatchTableRow->DSResult()->GetDamageLevelAtTableAndLoadphase(m_vecValidTables[j],m_vecTempLPV[i]));
			}
			
			m_pSpreadSheet->SetText(j+3,i+3,COleVariant(CString(sTmp)));
		}
	}
	
	m_pSpreadSheet->SetCol(0);
	m_pSpreadSheet->SetRow(0);
	m_pSpreadSheet->SetAction(29); //(SS_ACTION_SEL_MODE_CLEAR);
	//m_pSpreadSheet->EnableWindow(FALSE);
	
	m_pSpreadSheet->SetReDraw(TRUE);
		
}

//##ModelId=3DDA0C5902A5
void CDetailedResultsDraw::DrawLineChart()
{
	//get the current unit system.
	CDSealectDoc* pDoc=(CDSealectDoc*)GetDoc();
	QU::UNIT us = pDoc->UnitSystem();

	CCrackOpeningQuantity tmpCOQ;
	CString sUnitCrack;
	sUnitCrack = GetUnitString(&tmpCOQ);

	CStrainQuantity tmpSQ;  
	CString sUnitStrain;
	sUnitStrain = GetUnitString(&tmpSQ);
	
	m_pLineChart->SetIsBatched(TRUE);

	CChartLabelCollection labcol = m_pLineChart->GetChartLabels();
	CChartLabel distLabel =  labcol.GetItem(COleVariant(long (1)));
	distLabel.SetIsShowing(m_pBatchTableRow->CalculationSucceeded()==FALSE);

	CAxis xa;
	CAxis ya;
	CChart2DData dat;

	xa = m_pLineChart->GetChartArea().GetAxes().GetItem(COleVariant("X"));
	ya = m_pLineChart->GetChartArea().GetAxes().GetItem(COleVariant("Y"));

	CString sTmpLabel= "";
	for(int k=0 ; k< m_vecLoadLabelsToPlot.size() ; k++)
	{
		if(k!=0)
		{
			sTmpLabel = sTmpLabel + "                             "; // need space between two loadlabels
		}
		sTmpLabel = sTmpLabel + m_vecLoadLabelsToPlot[k]; //write LoadLabels as x-as title
	}
	//set axis scale
	double dMaxVal = 100.0; 
	xa.GetMax().SetValue(dMaxVal);
	xa.GetDataMax().SetValue(dMaxVal);
	xa.GetMin().SetValue(0.0);
	xa.GetDataMin().SetValue(0.0);
	xa.GetTitle().SetText(sTmpLabel);

	ya.GetMin().SetValue(0.0);
	ya.GetDataMin().SetValue(0.0);

	double dMaxPlotDam = FindMaxPlotDamageLevel();
	CString sTmp;
	if(m_iTypeOfDamage == FORMATION_DEBONDING_TABLE || m_iTypeOfDamage == CASING_DEBONDING_TABLE || m_iTypeOfDamage == CEMENT_RADIAL_CRACKING_TABLE)
	{
		sTmp = sUnitCrack;
		tmpCOQ.Value(dMaxPlotDam,CDoubleQuantity::SI_UNIT);
		ya.GetMax().SetValue(tmpCOQ.Value(us));
		ya.GetDataMax().SetValue(tmpCOQ.Value(us));
	}
	else
	{
		sTmp = sUnitStrain;
		tmpSQ.Value(dMaxPlotDam,CDoubleQuantity::SI_UNIT);
		ya.GetMax().SetValue(tmpSQ.Value(us));
		ya.GetDataMax().SetValue(tmpSQ.Value(us));
	}
	ya.GetTitle().SetText("Damage level " + sTmp);

	CChartGroup group= m_pLineChart->GetChartGroups().GetItem(COleVariant(long (1)));
	
	dat = group.GetData();
	
	long lSerie;
	int iNrOfPoints = m_vecPointsToPlot.size(); // nr of points 

	dat.SetNumSeries(1); 
	
	CLabel lb;
	CLabelCollection lc = group.GetSeriesLabels();
	lc.RemoveAll();

	// start fill header string
	CString sInfo = CString(TypeOfDamage[m_iTypeOfDamage]);
	sInfo = sInfo + "\nEvolution of maximum damage over load level/time\nwithin ";

	int LoadPhase = m_pBatchTableRow->DSResult()->Space()->GetSpaceTable(m_iTypeOfDamage)->GetFirstLoadPhaseItemIndex();
	if(LoadPhase>=0)
	{
		sInfo = sInfo + CString(LoadPhaseNames[LoadPhase]);
	}

	sInfo.Format(sInfo + "\nDepth along well = %#1.0f",m_pAnalysisPoint->TMD()->Value(us));
	sInfo = sInfo + "\nCement material = " + CString(m_pBatchTableRow->CementMaterial()->Name());
	m_pLineChart->GetHeader().GetText().SetText(sInfo); //show header string
	
	xa.SetAnnotationMethod(2);
	xa.GetValueLabels().RemoveAll();
	xa.SetAnnotationRotationAngle(0.0);

	int j = 0;
	lSerie = j + 1;
		
	dat.SetNumPoints(lSerie,iNrOfPoints);
	
	double dSetY;
	for(int i = 0; i < iNrOfPoints ; i++) // for each point in vecPointsToPlot
	{
		CString sTmp;
		sTmp.Format("%#4.1f",m_vecPointsToPlot[i][2]);
		//xa.GetValueLabels().Add(double(i+1), sTmp);  
		xa.GetValueLabels().Add(double(m_vecPointsToPlot[i][0]), sTmp);  
		
		dat.SetX(lSerie, i + 1,m_vecPointsToPlot[i][0]);//.first);

		dSetY = m_vecPointsToPlot[i][1];
		//dat.SetY(lSerie,i+1,m_vecPointsToPlot[i][1]);//.second);
		if(m_iTypeOfDamage == FORMATION_DEBONDING_TABLE || m_iTypeOfDamage == CASING_DEBONDING_TABLE || m_iTypeOfDamage == CEMENT_RADIAL_CRACKING_TABLE)
		{
			tmpCOQ.Value(dSetY,CDoubleQuantity::SI_UNIT);
			dat.SetY(lSerie, i + 1,tmpCOQ.Value(us));//.second);
		}
		else
		{
			tmpSQ.Value(dSetY,CDoubleQuantity::SI_UNIT);
			dat.SetY(lSerie, i + 1,tmpSQ.Value(us));//.second);
		}
	}
	
	m_pLineChart->SetIsBatched(FALSE);
	m_pLineChart->Refresh();
}

//##ModelId=3DDA0C5902A4
void CDetailedResultsDraw::DrawBarChart()
{
	//get the current unit system.
	CDSealectDoc* pDoc=(CDSealectDoc*)GetDoc();
	QU::UNIT us = pDoc->UnitSystem();
	
	m_pBarChart->SetIsBatched(TRUE);

	CAxis xa;
	CAxis ya;
	CChart2DData dat;

	xa = m_pBarChart->GetChartArea().GetAxes().GetItem(COleVariant("X"));
	ya = m_pBarChart->GetChartArea().GetAxes().GetItem(COleVariant("Y"));

	//set axis scale
	double dMaxVal = m_vecValidTables.size() + 0.5; //Nr of valid tables + 0.5 
	xa.GetMax().SetValue(dMaxVal);
	xa.GetDataMax().SetValue(dMaxVal);
	xa.GetMin().SetValue(0.0);
	xa.GetDataMin().SetValue(0.0);
	xa.GetTitle().SetText("Damage types");

	ya.GetMax().SetValue(100.0);
	ya.GetDataMax().SetValue(100.0);
	ya.GetMin().SetValue(0.0);
	ya.GetDataMin().SetValue(0.0);
	ya.GetTitle().SetText("Remaining Capacity (%)");

	CChartGroup group= m_pBarChart->GetChartGroups().GetItem(COleVariant(long (1)));

	CChartLabelCollection labcol = m_pBarChart->GetChartLabels();
	CChartLabel distLabel =  labcol.GetItem(COleVariant(long (1)));
	
			
	distLabel.SetIsShowing(m_pBatchTableRow->CalculationSucceeded()==FALSE);
	
	dat = group.GetData();
	
	long lSerie;
	double dCapacity;
	int iNrOfTables = m_vecValidTables.size(); // nr of valid tables 

	dat.SetNumSeries(m_vecTempLPV.size()); //sum of valid loadphases

	CLegend legend = m_pBarChart->GetLegend();
	legend.SetIsShowing(TRUE); // temp False
	
	CLabel lb;
	CLabelCollection lc = group.GetSeriesLabels();
	lc.RemoveAll();

	xa.SetAnnotationMethod(2);
	xa.GetValueLabels().RemoveAll();
	xa.SetAnnotationRotationAngle(45.0);

	//fill info string to put in header
	CString sInfo;
	sInfo.Format("\nDepth along well = %#1.0f",m_pAnalysisPoint->TMD()->Value(us));
	sInfo = sInfo + "\nCement material = " + CString(m_pBatchTableRow->CementMaterial()->Name());

	for(int j = 0 ; j < dat.GetNumSeries() ; j++) // for each valid loadphase
	{
		lSerie = j + 1;
		
		dat.SetNumPoints(lSerie,iNrOfTables);
		
		lb = lc.Add(LoadPhaseNames[m_vecTempLPV[j]],COleVariant()); //ColeVariant is empty to add at end of collection
		
		for(int i = 0; i < iNrOfTables ; i++) // for each table in vecValidTables
		{
			if(j == 0)
			{
				CString sTmp = (TypeOfDamage[m_vecValidTables[i]]);
				xa.GetValueLabels().Add(double(i + 1), sTmp);  
			}
			
			m_pBarChart->GetHeader().GetText().SetText("Risk of damage over load phases" + sInfo);
			
			dCapacity = m_pBatchTableRow->DSResult()->GetCapacityAtTableAndLoadphase(m_vecValidTables[i],m_vecTempLPV[j]);
					
			dat.SetY(lSerie, i + 1,(GetReverseCapacity(dCapacity))); 
		}
	}

	m_pBarChart->SetIsBatched(FALSE);
	m_pBarChart->Refresh();
  
}

//##ModelId=3DDA0C59014E
double CDetailedResultsDraw::GetReverseCapacity(double dCapacity)
{
	if(dCapacity == -1) // means there is no result
		return 0.0;

	return 100.0 - dCapacity;
}

//##ModelId=3DDA0C59014D
double CDetailedResultsDraw::FindMaxPlotDamageLevel()
{
	if(m_vecPointsToPlot.size() <= 0)
		return 0.0;

	double dMax = 0.0;
	for(int i = 0 ; i < m_vecPointsToPlot.size() ; i++)
	{
		if(m_vecPointsToPlot[i][1] > dMax)
			dMax = m_vecPointsToPlot[i][1];
	}
	return dMax;
}

//##ModelId=3DDA0C59014B
void CDetailedResultsDraw::FillValidTablesVector()
{
	m_vecValidTables.clear();

  assert(GetDoc()->DSealectModel()->TMDtoTVD(m_pAnalysisPoint->TMD()->Value()).second);
	double dDepth = GetDoc()->DSealectModel()->TMDtoTVD(m_pAnalysisPoint->TMD()->Value()).first;
	CString name = GetDoc()->DSealectModel()->FormationTopography()->GetFormationAtDepth(dDepth)->MaterialName();
	CFormationMaterial* pFM = GetDoc()->DSealectModel()->FormationMatList()->GetMaterial(name);
	
	name = GetDoc()->DSealectModel()->DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->CasingMaterialName();
	CCasingMaterial* pCM = GetDoc()->DSealectModel()->CasingMatList()->GetMaterial(name);
	
	for(int i = 0 ; i <= CEMENT_RADIAL_CRACKING_TABLE ; i++) // for all tables
	{
		if(i == FORMATION_PLASTICITY_TABLE)
		{	
			if(pFM->Plasticity() && GetDoc()->DSealectModel()->Control()->OutputLevel() == 2) 
				m_vecValidTables.push_back(i); // only if FormationPlasticity = true && Outputlevel = 3
		}
		else if(i == CASING_PLASTICITY_TABLE)
		{
			if(pCM->Plasticity() && GetDoc()->DSealectModel()->Control()->OutputLevel() == 2)
				m_vecValidTables.push_back(i); // only if CasingMaterialPlasticity = true && Outputlevel = 3
		}
		else
		{
			m_vecValidTables.push_back(i);
		}
	}
}

//##ModelId=3DDA0C59014C
void CDetailedResultsDraw::FillPlotPointsVector()
{
	//get the current unit system.
	CDSealectDoc* pDoc=(CDSealectDoc*)GetDoc();
	QU::UNIT us = pDoc->UnitSystem();
	
	CSinglePressure tmpSPQ;
	CString sUnit;
	sUnit = GetUnitString(&tmpSPQ);

	m_vecPointsToPlot.clear();
	m_vecLoadLabelsToPlot.clear();

	if(!m_pBatchTableRow->DSResult()->Space()->GetSpaceTable(m_iTypeOfDamage)->HasResults())
		return;

	CLoadPhaseItems* pLPI = m_pBatchTableRow->DSResult()->Space()->GetSpaceTable(m_iTypeOfDamage)->GetLoadPhaseItem(0);
	int iTNR = pLPI->GetNrOfLoadsToPlot(); // Total Nr of Loads to plot == 100 in real x-coordinates
	int iNR = 0; //Nr of Load to plot

	double dMin;	// minimum load/time for a load
	double dMax;	// maximum load/time for a load
	double dDumX;	// load/time between min and max
	int c;			// c = (iNR - 1) * (100 / iTNR)
	int d;			// d = iNR * (100 / iTNR)
	double dRico;	// dRico = (d - c) / (dMax - dMin) 
	double dPlotX;	// real x-coordinate in plot calculated with: dPlotX = dRico * dDumX + c - dRico * dMin
	double dPlotY;	// damage level
	std::vector<double> vecXYDum;
		
	bool bOneStepBefore = false;

	for(int i = 0 ; i < pLPI->GetNrOfAppliedLoads() ; i++)
	{
		vecXYDum.clear();
		if(pLPI->GetAppliedLoad(i)->IsOneStepLoad())  //One step loads are added to the begin of the next load
		{
			bOneStepBefore = true;
			dPlotX = iNR * (100 / iTNR); 
			dPlotY = pLPI->GetAppliedLoad(i)->GetAppliedLoadItem(0)->DamageLevel();
			vecXYDum.push_back(dPlotX);
			vecXYDum.push_back(dPlotY);
			vecXYDum.push_back(pLPI->GetAppliedLoad(i)->GetMaxValPlotX());
			m_vecPointsToPlot.push_back(vecXYDum);
		}
		else
		{
			CString sTmp = pLPI->GetAppliedLoad(i)->GetLoadLabel();
			if(pLPI->GetAppliedLoad(i)->IsAppliedTime())
			{
				sTmp = sTmp + " [s]";
				//m_vecLoadLabelsToPlot.push_back(pLPI->GetAppliedLoad(i)->GetLoadLabel() + "[s]"); //add loadlabel for load to plot 
			}
			else
			{
				sTmp = sTmp + " " + sUnit;
			}
			m_vecLoadLabelsToPlot.push_back(sTmp); //add loadlabel for load to plot 
			iNR++;
			if(bOneStepBefore)
			{
				dMin = pLPI->GetAppliedLoad(i - 1)->GetMaxValPlotX();
				bOneStepBefore = false;
			}
			else
			{
				dMin = pLPI->GetAppliedLoad(i)->GetMinValPlotX();
			}
			dMax = pLPI->GetAppliedLoad(i)->GetMaxValPlotX();
			c = (iNR - 1) * (100 / iTNR);
			d = iNR * (100 / iTNR);
			dRico = (d - c) / (dMax - dMin); 

			for(int j = 0 ; j < pLPI->GetAppliedLoad(i)->GetNrOfItems() ; j++)
			{
				if(pLPI->GetAppliedLoad(i)->IsAppliedTime())
				{
					dDumX = pLPI->GetAppliedLoad(i)->GetAppliedLoadItem(j)->Time();
				}
				else
				{
					dDumX = pLPI->GetAppliedLoad(i)->GetAppliedLoadItem(j)->LoadLevel();
				}
				dPlotX = dRico * dDumX + c - dRico * dMin;
				dPlotY = pLPI->GetAppliedLoad(i)->GetAppliedLoadItem(j)->DamageLevel();
				vecXYDum.clear();
				
				if(!pLPI->GetAppliedLoad(i)->IsAppliedTime()) // only for loads, time-units are the same (seconds)
				{
					tmpSPQ.Value(dDumX,CDoubleQuantity::SI_UNIT);
					dDumX = tmpSPQ.Value(us);
				}

				vecXYDum.push_back(dPlotX);
				vecXYDum.push_back(dPlotY);
				vecXYDum.push_back(dDumX);
				m_vecPointsToPlot.push_back(vecXYDum);
			}
		}
	}
	
}

//##ModelId=3DDA0C590144
void CDetailedResultsDraw::FillValidLoadPhaseVectors()
{
	
	//fill vectors with integers corresponding to loadphases
	m_vecValidLoadPhasesFormation.clear();
	m_vecValidLoadPhasesCasing.clear();
	m_vecValidLoadPhasesSealant.clear();

  assert(GetDoc()->DSealectModel()->TMDtoTVD(m_pAnalysisPoint->TMD()->Value()).second);
	double dDepth = GetDoc()->DSealectModel()->TMDtoTVD(m_pAnalysisPoint->TMD()->Value()).first;
	CString name = GetDoc()->DSealectModel()->FormationTopography()->GetFormationAtDepth(dDepth)->MaterialName();
	CFormationMaterial* pFM = GetDoc()->DSealectModel()->FormationMatList()->GetMaterial(name);
	
	for(int i = DRILLING ; i < NR_OF_LOADPHASES ; i++)
	{
		switch(i)
		{
			case DRILLING:
				if(pFM->Plasticity())
					m_vecValidLoadPhasesFormation.push_back(DRILLING);
				break;
			
			case CURING:
			case PRESSURE_TEST:
				{
					m_vecValidLoadPhasesFormation.push_back(i);
					m_vecValidLoadPhasesCasing.push_back(i);
					m_vecValidLoadPhasesSealant.push_back(i);
				}
				break;

			case COMPLETION:
				//if(!GetDoc()->DSealectModel()->Control()->BalancedCompletion())
				if(GetDoc()->DSealectModel()->Control()->DoCompletion())
				{
					m_vecValidLoadPhasesFormation.push_back(COMPLETION);
					m_vecValidLoadPhasesCasing.push_back(COMPLETION);
					m_vecValidLoadPhasesSealant.push_back(COMPLETION);
				}
				break;

			case SHUT_IN:
				if(GetDoc()->DSealectModel()->Control()->ShutIn())
				{
					m_vecValidLoadPhasesFormation.push_back(SHUT_IN);
					m_vecValidLoadPhasesCasing.push_back(SHUT_IN);
					m_vecValidLoadPhasesSealant.push_back(SHUT_IN);
				}
				break;

			case PRODUCTION:
				if(GetDoc()->DSealectModel()->Control()->IsProduction())
				{
					m_vecValidLoadPhasesFormation.push_back(PRODUCTION);
					m_vecValidLoadPhasesCasing.push_back(PRODUCTION);
					m_vecValidLoadPhasesSealant.push_back(PRODUCTION);
				}
				break;

			case INJECTION:
				if(GetDoc()->DSealectModel()->Control()->IsInjection())
				{
					m_vecValidLoadPhasesFormation.push_back(INJECTION);
					m_vecValidLoadPhasesCasing.push_back(INJECTION);
					m_vecValidLoadPhasesSealant.push_back(INJECTION);
				}
				break;

			case FRAC_KILLING:
				if(GetDoc()->DSealectModel()->Control()->Fracturing())
				{
					m_vecValidLoadPhasesFormation.push_back(FRAC_KILLING);
					m_vecValidLoadPhasesCasing.push_back(FRAC_KILLING);
					m_vecValidLoadPhasesSealant.push_back(FRAC_KILLING);
				}
				break;

			case EVAC_UNLOAD:
				if(GetDoc()->DSealectModel()->Control()->Evacuation())
				{
					m_vecValidLoadPhasesFormation.push_back(EVAC_UNLOAD);
					m_vecValidLoadPhasesCasing.push_back(EVAC_UNLOAD);
					m_vecValidLoadPhasesSealant.push_back(EVAC_UNLOAD);
				}
				break;
		}
	}
	
}

//##ModelId=3DDA0C59013F
void CDetailedResultsDraw::SetBorder(long lCol, long lRow, long lStyle, long lType)
{
	
	m_pSpreadSheet->SetCol(lCol);
	m_pSpreadSheet->SetRow(lRow);
	m_pSpreadSheet->SetCellBorderStyle(lStyle);
	m_pSpreadSheet->SetCellBorderType(lType);
	m_pSpreadSheet->SetAction(16) ;//(SS_ACTION_SET_CELL_BORDER);
	
}

//##ModelId=3DDA0C59013C
void CDetailedResultsDraw::SetBackColorColRow(long lCol, long lRow)
{
	m_pSpreadSheet->SetCol(lCol);
	m_pSpreadSheet->SetRow(lRow);
	m_pSpreadSheet->SetBackColor(0x8000000F);
	
}

void CDetailedResultsDraw::SetRedBackColorColRow(long lCol, long lRow, bool bRedYesWhiteNo)
{
	m_pSpreadSheet->SetCol(lCol);
	m_pSpreadSheet->SetRow(lRow);
	if(bRedYesWhiteNo)
	{
		m_pSpreadSheet->SetBackColor(RGB(255,200,200));
	} 
	else
	{
		m_pSpreadSheet->SetBackColor(RGB(255,255,255));
	}
	
}

//##ModelId=3DDA0C59032F
BOOL CDetailedResultsDraw::OnEraseBkgnd(CDC* pDC) 
{
// Set brush to desired background color
	CBrush* pBackBrush; 


	if(m_iShowMode == FGVVIEW)
	{
		//if(m_pBatchTableRow->GetContourFileName(m_iTypeOfDamage).IsEmpty())
		//{
			pBackBrush=new CBrush(RGB(255,255,255));
		//}
		//else
		//{
			//pBackBrush=new CBrush(RGB(0,0,0));
							
		//}
	}
	else
	{
		pBackBrush=new CBrush(RGB(255,255,255));
	}

	// Save old brush
	CBrush* pOldBrush = pDC->SelectObject(pBackBrush);

	CRect rect;
	pDC->GetClipBox(&rect);     // Erase the area needed

	pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(),PATCOPY);
	pDC->SelectObject(pOldBrush);
	delete pBackBrush;
    return TRUE;
  
}
