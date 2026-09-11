// ResultOverviewDraw.cpp : implementation file
//

#include "stdafx.h"
#include "dsealect.h"
#include "2DCONST.h"
#include "Chart2D.h"
#include "Axis.h"
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
#include "ResultOverviewForm.h"
#include "ResultOverviewDraw.h"
#include "SpreadFunctions.h"
#include "CDataModel.h"
#include "CFormationTopography.h"

#define new DEBUG_NEW
#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

#define SEALANT		0
#define FORMATION	1
#define CASING		2

#define PRESENTATION_TABLE		0
#define PRESENTATION_BAR_CHART	1

#define ID_SPREADSHEET 2000
#define ID_CHART       2001


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




IMPLEMENT_DYNCREATE(CResultOverviewDraw, CDSealectDrawView)

//##ModelId=3CA0119D03D8
CResultOverviewDraw::CResultOverviewDraw()
: m_pSpreadSheet(0),
  m_pChart(0),
  m_pAnalysisPoint(0),
  m_iWellComponent(-1)
{
}

//##ModelId=3CA0119E005D
CResultOverviewDraw::~CResultOverviewDraw()
{
	if(m_pChart)
		delete m_pChart;

	if(m_pSpreadSheet)
		delete m_pSpreadSheet;
}


BEGIN_EVENTSINK_MAP(CResultOverviewDraw, CDSealectDrawView)
    //{{AFX_EVENTSINK_MAP(CChartFormView)
	ON_EVENT(CResultOverviewDraw, ID_SPREADSHEET, -605 /* MouseDown */, OnMouseDownControl1, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CResultOverviewDraw, ID_CHART, -605 /* MouseDown */, OnMouseDownControl1, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

#define SS_ACTION_CLIPBOARD_COPY 22
#define SS_ACTION_SELECT_BLOCK   2


//##ModelId=3DDA0C4F035E
void CResultOverviewDraw::OnPopupCopy() 
{
	if(m_iPresentationDraw == PRESENTATION_TABLE)
	{	
		CopyToClipboard(*m_pSpreadSheet);
		/*
		m_pSpreadSheet->SetRow(0);
		m_pSpreadSheet->SetCol(0);
		m_pSpreadSheet->SetRow2(8);
		m_pSpreadSheet->SetCol2(8);
		m_pSpreadSheet->SetAction(SS_ACTION_SELECT_BLOCK);
		m_pSpreadSheet->SetAction(SS_ACTION_CLIPBOARD_COPY);
		*/
	}
	else
	{
		m_pChart->CopyToClipboard(oc2dFormatBitmap);
	}

}



//##ModelId=3DDA0C4F0321
void CResultOverviewDraw::OnMouseDownControl1(short Button, short Shift, long X, long Y) 
{

	if(Button==2)
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
		point.x=point.x - GetScrollPosition().x;
		point.y=point.y - GetScrollPosition().y;
		// Post the menu.
		submenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,
				point.x,    point.y,
				this,NULL); //AfxGetApp()->m_pMainWnd
	}
}



BEGIN_MESSAGE_MAP(CThisChart, CChart2D)
	//{{AFX_MSG_MAP(CThisChart)
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//##ModelId=3DDA0C4F012F
void CThisChart::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CChart2D::OnRButtonDown(nFlags, point);
}



BEGIN_MESSAGE_MAP(CResultOverviewDraw, CDSealectDrawView)
	//{{AFX_MSG_MAP(CResultOverviewDraw)
	ON_WM_SIZE()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_POPUP_COPY, OnPopupCopy)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResultOverviewDraw drawing

//##ModelId=3DDA0C4F02E1
void CResultOverviewDraw::SetPresentationDraw(int iPresentationDraw)
{
	m_iPresentationDraw = iPresentationDraw;
	m_pSpreadSheet->ShowWindow(m_iPresentationDraw == PRESENTATION_TABLE);
	m_pChart->ShowWindow(m_iPresentationDraw == PRESENTATION_BAR_CHART);
}

void CResultOverviewDraw::SetAnalysisPoint(CAnalysisPoint* pAnalysisPoint)
{
  m_pAnalysisPoint = pAnalysisPoint;
}

void CResultOverviewDraw::SetWellComponent(int iWellComponent)
{
  m_iWellComponent = iWellComponent;
}

//##ModelId=3CA0119E004E
void CResultOverviewDraw::OnDraw(CDC* pDC)
{
	pDC->TextOut(100,100,"No results available");
	StartDraw();
}

/////////////////////////////////////////////////////////////////////////////
// CResultOverviewDraw diagnostics

#ifdef _DEBUG
//##ModelId=3CA0119E005F
void CResultOverviewDraw::AssertValid() const
{
	CView::AssertValid();
}

//##ModelId=3CA0119E006D
void CResultOverviewDraw::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CResultOverviewDraw message handlers

//##ModelId=3CA0119D03E7
BOOL CResultOverviewDraw::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	BOOL ret = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
	BSTR bstrLicense;
	BOOL btemp;

//#if 0
  bstrLicense = ::SysAllocString(L"Copyright (c) 1995 FarPoint");
	//bstrLicense = ::SysAllocStringLen(SpreadSheetLicenseKey,
  //     sizeof(SpreadSheetLicenseKey)/sizeof(WCHAR));
	m_pSpreadSheet = new CSpreadSheet;
	btemp = m_pSpreadSheet->Create(0,WS_CHILD | WS_VISIBLE ,CRect(0,0,100,100),this,ID_SPREADSHEET,NULL,FALSE,bstrLicense);
	::SysFreeString(bstrLicense); 	
//#endif

	bstrLicense = ::SysAllocStringLen(chart2dLicenseKey,
       sizeof(chart2dLicenseKey)/sizeof(WCHAR));
	m_pChart = new CChart2D;
	btemp = m_pChart->Create(0,WS_CHILD | WS_VISIBLE ,CRect(0,0,100,100),this,ID_CHART,NULL,FALSE,bstrLicense);
	::SysFreeString(bstrLicense); 	

	m_pSpreadSheet->SetGrayAreaBackColor(RGB(255,255,255));
	m_pSpreadSheet->SetTypeEditLen(120);
	m_pSpreadSheet->SetEnabled(TRUE);
	m_pSpreadSheet->SetEditMode(FALSE);
	m_pSpreadSheet->SetLock(TRUE);

	CString ChartInitFile=GetAppPath() + "Barchart.oc2"; //TODO temporary hard path
	BOOL bLoad = m_pChart->Load(ChartInitFile);

	m_pChart->ShowWindow(m_iPresentationDraw == PRESENTATION_BAR_CHART);
	m_pSpreadSheet->ShowWindow(m_iPresentationDraw == PRESENTATION_TABLE);

	m_pSpreadSheet->MoveWindow(0,0,3000,3000);
	m_pChart->MoveWindow(0,0,700,700);
	m_pChart->SetAllowUserChanges(FALSE);

	return ret;
}

//##ModelId=3CA0119E007C
void CResultOverviewDraw::OnSize(UINT nType, int cx, int cy) 
{
	
	if(m_iPresentationDraw == PRESENTATION_BAR_CHART)
	{
		SetScrollSizes(MM_TEXT,CSize(700,700));
	}
	else
	{
		SetScrollSizes(MM_TEXT,CSize(2500,2500));
	}
	
	
}


//##ModelId=3DDA0C4F02F1
void CResultOverviewDraw::StartDraw()
{
  if(m_pAnalysisPoint)
  {
	  m_vecRows = GetDoc()->DSealectModel()->BatchTable()->GetResultRows(m_pAnalysisPoint);
	  FillValidLoadPhaseVectors();

	  //copy ValidLoadPhaseVector to temporary vector
	  if(m_iWellComponent == FORMATION)
	  {
		  m_vecTempLPV = m_vecValidLoadPhasesFormation;
	  }
	  else if(m_iWellComponent == CASING)
	  {
		  m_vecTempLPV = m_vecValidLoadPhasesCasing;
	  }
	  else if(m_iWellComponent == SEALANT)
	  {
		  m_vecTempLPV = m_vecValidLoadPhasesSealant;
	  }
	  else
	  {
		  ASSERT(FALSE);
	  }
  		
  	
	  if(m_vecTempLPV.size()>0)
	  {
		  if(m_iPresentationDraw == PRESENTATION_TABLE)
		  {
			  m_pSpreadSheet->SetFocus();
			  m_pSpreadSheet->SetActiveWindow();
			  DrawTable();

		  }
		  else
		  {
			  DrawBarChart();
		  }
	  }
	  else
	  {
		  m_pSpreadSheet->ShowWindow(FALSE);
		  m_pChart->ShowWindow(FALSE);

	  }
  }
}

//##ModelId=3DDA0C4F0301
void CResultOverviewDraw::DrawTable()
{	
	CString sTmp;
	CString sAllFailedFalse = "Intact";
	CString sNoResults = "No result";
	CString sDistrust = "Distrust result accuracy.\nCalculation diverged.";

	BOOL bFailed;
	m_pSpreadSheet->SetReDraw(FALSE);

	m_pSpreadSheet->SetMaxCols(m_vecRows.size() + 2); //Nr of valid cementscenario's +2
	m_pSpreadSheet->SetMaxRows(m_vecTempLPV.size() + 2); // (NR_OF_LOADPHASES + 2);
	m_pSpreadSheet->SetColWidth(-1,18);
	m_pSpreadSheet->SetRowHeight(-1,50);
	m_pSpreadSheet->SetCol(0);
	m_pSpreadSheet->SetColHidden(TRUE);
	m_pSpreadSheet->SetRow(0);
	m_pSpreadSheet->SetRowHidden(TRUE);

	m_pSpreadSheet->SetCol(-1);
	m_pSpreadSheet->SetRow(-1);
	m_pSpreadSheet->SetTypeEditMultiLine(TRUE);

	SetBackColorColRow(1,-1);
	SetBackColorColRow(2,-1);
	SetBackColorColRow(-1,1);
	SetBackColorColRow(-1,2);
	
	SetBorder(1,-1,1,15);		
	SetBorder(2,-1,1,15);
	SetBorder(-1,1,1,15);
	SetBorder(-1,2,1,15);

	m_pSpreadSheet->SetText(1,2,COleVariant(CString("Load type")));
	m_pSpreadSheet->SetText(2,1,COleVariant(CString("Cement material ->")));
	m_pSpreadSheet->SetText(2,2,COleVariant(CString("Sequence")));

	CString sSequence = "";
	int iRememberShutIn = -1;

	//for(int i = 0 ; i < NR_OF_LOADPHASES ; i++)
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
				

		for(int j = 0 ; j < m_vecRows.size() ; j++) // for each valid cementscenario
		{
			m_pSpreadSheet->SetText(j+3,1,COleVariant(CString(m_vecRows[j]->CementMaterial()->Name())));

			if(!m_vecRows[j]->CalculationSucceeded())
			{
				m_pSpreadSheet->SetText(j+3,2,COleVariant(CString(sDistrust)));
				SetRedBackColorColRow(j+3,-1);
			}
			

			sTmp = "";
			if(m_iWellComponent == FORMATION)
			{
				bFailed = m_vecRows[j]->DSResult()->GetFailedAtTableAndLoadphase(FORMATION_PLASTICITY_TABLE,m_vecTempLPV[i]);
				if(!bFailed)
				{	
					if(m_vecRows[j]->DSResult()->GetCapacityAtTableAndLoadphase(FORMATION_PLASTICITY_TABLE,m_vecTempLPV[i])== -1)
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
					sTmp = TableDamageType[FORMATION_PLASTICITY_TABLE];
				}
			}
			if(m_iWellComponent == CASING)
			{
				bFailed = m_vecRows[j]->DSResult()->GetFailedAtTableAndLoadphase(CASING_PLASTICITY_TABLE,m_vecTempLPV[i]);
				if(!bFailed)
				{
					if(m_vecRows[j]->DSResult()->GetCapacityAtTableAndLoadphase(CASING_PLASTICITY_TABLE,m_vecTempLPV[i])== -1)
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
					sTmp = TableDamageType[CASING_PLASTICITY_TABLE];
				}
			}
			if(m_iWellComponent == SEALANT)
			{
				for(int k = FORMATION_DEBONDING_TABLE ; k < NR_OF_TABLES ; k++)
				{
					bFailed = m_vecRows[j]->DSResult()->GetFailedAtTableAndLoadphase(k,m_vecTempLPV[i]);
					if(bFailed)
					{
						sTmp = sTmp + TableDamageType[k] + "\n"; //add all damagetypes to string
					}
				}
				if(sTmp.IsEmpty()) 
				{
					if(m_vecRows[j]->DSResult()->GetSealantRiskCapacityAtLoadPhase(m_vecTempLPV[i])== -1)
					{	
						sTmp = sNoResults;
					}
					else
					{
						sTmp = sAllFailedFalse;
					}
				}
			}

			//GetDoc()->DSealectModel()->BatchTable()->GetRow(j)->DSResult()->GetFailedAtTableAndLoadphase(iTable,i)
						
			m_pSpreadSheet->SetText(j+3,i+3,COleVariant(CString(sTmp)));
		}
	}
	
	m_pSpreadSheet->SetCol(0);
	m_pSpreadSheet->SetRow(0);
	m_pSpreadSheet->SetAction(29); //(SS_ACTION_SEL_MODE_CLEAR);
	m_pSpreadSheet->SetReDraw(TRUE);
	
	

}

//##ModelId=3DDA0C4F02F2
void CResultOverviewDraw::DrawBarChart()
{
	//get the current unit system.
	CDSealectDoc* pDoc=(CDSealectDoc*)GetDoc();
	QU::UNIT us = pDoc->UnitSystem();

	m_pChart->SetIsBatched(TRUE);

	CAxis xa;
	CAxis ya;
	CChart2DData dat;

	xa = m_pChart->GetChartArea().GetAxes().GetItem(COleVariant("X"));
	ya = m_pChart->GetChartArea().GetAxes().GetItem(COleVariant("Y"));

	
	//set axis scale
	double dMaxVal = m_vecRows.size() + 0.5; //Nr of cement scenario's + 0.5 
	xa.GetMax().SetValue(dMaxVal);
	xa.GetDataMax().SetValue(dMaxVal);
	xa.GetMin().SetValue(0.0);
	xa.GetDataMin().SetValue(0.0);
	xa.GetTitle().SetText("Sealants");
	xa.SetAnnotationRotationAngle(45.0);

	ya.GetMax().SetValue(100.0);
	ya.GetDataMax().SetValue(100.0);
	ya.GetMin().SetValue(0.0);
	ya.GetDataMin().SetValue(0.0);
	ya.GetTitle().SetText("Remaining Capacity (%)");

	CChartGroup group= m_pChart->GetChartGroups().GetItem(COleVariant(long (1)));
	
	dat = group.GetData();
	
	long lSerie;
	double dCapacity;
	int iNrOfCements = m_vecRows.size(); // nr of valid cementscenario's 

	dat.SetNumSeries(m_vecTempLPV.size()); //sum of valid loadphases

	CLegend legend = m_pChart->GetLegend();
	legend.SetIsShowing(TRUE); // temp False
	
	CChartLabelCollection labcol = m_pChart->GetChartLabels();
	CChartLabel distLabel =  labcol.GetItem(COleVariant(long (1)));
	distLabel.SetIsShowing(FALSE);	

	CLabel lb;
	CLabel lb2;
	CLabelCollection lc = group.GetSeriesLabels();
	lc.RemoveAll();

	
	CString sInfo;
	sInfo.Format("\nDepth along well = %#1.0f",m_pAnalysisPoint->TMD()->Value(us));
	sInfo = sInfo + "\nWell component = " + CString(WellComponent[m_iWellComponent]);

	xa.SetAnnotationMethod(2);
	xa.GetValueLabels().RemoveAll();

	for(int j = 0 ; j < dat.GetNumSeries() ; j++) // for each valid loadphase
	{
		lSerie = j+1;
		
		dat.SetNumPoints(lSerie,iNrOfCements);
		
		lb = lc.Add(LoadPhaseNames[m_vecTempLPV[j]],COleVariant()); //ColeVariant is empty to add at end of collection
		
		for(int i = 0; i < iNrOfCements ; i++) // for each cement in vecRows
		{
			CString sStar = "";
			if(!m_vecRows[i]->CalculationSucceeded())
			{
				sStar = " ***";
				distLabel.SetIsShowing(TRUE);	
				
			}
			
			if(j==0)					
				xa.GetValueLabels().Add(double(i+1),m_vecRows[i]->CementMaterial()->Name() + sStar); 
						
			if(m_iWellComponent == SEALANT)
			{
				m_pChart->GetHeader().GetText().SetText("Sealant risk of failure" + sInfo);
				dCapacity = m_vecRows[i]->DSResult()->GetSealantRiskCapacityAtLoadPhase(m_vecTempLPV[j]);
			}
			if(m_iWellComponent == FORMATION)
			{
				m_pChart->GetHeader().GetText().SetText("Formation risk of failure" + sInfo);
				//dCapacity = m_vecRows[i]->DSResult()->GetFormationRiskCapacityAtLoadPhase(m_vecTempLPV[j]);
				dCapacity = m_vecRows[i]->DSResult()->GetCapacityAtTableAndLoadphase(FORMATION_PLASTICITY_TABLE,m_vecTempLPV[j]);
			}
			if(m_iWellComponent == CASING)
			{
				m_pChart->GetHeader().GetText().SetText("Casing risk of failure" + sInfo);
				//GetDoc()->DSealectModel()->BatchTable()->GetRow(i)->DSResult()->GetCasingRiskCapacityAtLoadPhase(j)
				//dCapacity = m_vecRows[i]->DSResult()->GetCasingRiskCapacityAtLoadPhase(m_vecTempLPV[j]);
				dCapacity = m_vecRows[i]->DSResult()->GetCapacityAtTableAndLoadphase(CASING_PLASTICITY_TABLE,m_vecTempLPV[j]);
			}
			
			dat.SetY(lSerie,i+1,(GetReverseCapacity(dCapacity))); 
		}
	}
	
	
	m_pChart->SetIsBatched(FALSE);
	m_pChart->Refresh();
		
}

//##ModelId=3DDA0C4F0245
double CResultOverviewDraw::GetReverseCapacity(double dCapacity)
{
	if(dCapacity == -1) // means there is no result
		return 0.0;

	return 100.0 - dCapacity;
}

//##ModelId=3DDA0C4F0237
void CResultOverviewDraw::FillValidLoadPhaseVectors()
{
	//fill vectors with integers corresponding to loadphases
	m_vecValidLoadPhasesFormation.clear();
	m_vecValidLoadPhasesCasing.clear();
	m_vecValidLoadPhasesSealant.clear();

	
	for(int i = DRILLING ; i < NR_OF_LOADPHASES ; i++)
	{
		switch(i)
		{
			case DRILLING:
				// drilling is valid for FormationComponent only
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

	double dDepth = GetDoc()->DSealectModel()->TMDtoTVD(m_pAnalysisPoint->TMD()->Value()).first;
	CString name = GetDoc()->DSealectModel()->FormationTopography()->GetFormationAtDepth(dDepth)->MaterialName();
	CFormationMaterial* pFM = GetDoc()->DSealectModel()->FormationMatList()->GetMaterial(name);


	if(!pFM->Plasticity()) // if formationplasticity is false, there are no valid loadphases
		m_vecValidLoadPhasesFormation.clear();

	name = GetDoc()->DSealectModel()->DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->CasingMaterialName();
	CCasingMaterial* pCM = GetDoc()->DSealectModel()->CasingMatList()->GetMaterial(name);

	if(!pCM->Plasticity()) // if casingplasticity is false, there are no valid loadphases
		m_vecValidLoadPhasesCasing.clear();
}



//##ModelId=3DDA0C4F0217
void CResultOverviewDraw::SetBorder(long lCol, long lRow, long lStyle, long lType)
{
	m_pSpreadSheet->SetCol(lCol);
	m_pSpreadSheet->SetRow(lRow);
	m_pSpreadSheet->SetCellBorderStyle(lStyle);
	m_pSpreadSheet->SetCellBorderType(lType);
	m_pSpreadSheet->SetAction(16) ;//(SS_ACTION_SET_CELL_BORDER);
}

//##ModelId=3DDA0C4F0207
void CResultOverviewDraw::SetBackColorColRow(long lCol, long lRow)
{
	m_pSpreadSheet->SetCol(lCol);
	m_pSpreadSheet->SetRow(lRow);
	m_pSpreadSheet->SetBackColor(0x8000000F);
}

void CResultOverviewDraw::SetRedBackColorColRow(long lCol, long lRow)
{
	m_pSpreadSheet->SetCol(lCol);
	m_pSpreadSheet->SetRow(lRow);
	m_pSpreadSheet->SetBackColor(RGB(255,200,200));
}


//##ModelId=3DDA0C4F0310
void CResultOverviewDraw::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDSealectDrawView::OnRButtonDown(nFlags, point);
}

