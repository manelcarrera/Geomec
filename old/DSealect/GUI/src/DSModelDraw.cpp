// DSModelDraw.cpp: 
//
////////////////////////////////

#include "stdafx.h"
#include "dsealect.h"
#include "MeshParam.h"
#include "Axis.h"
#include "Chart2DData.h"
#include "ChartArea.h"
#include "ChartGroup.h"
#include "ChartLabel.h"
#include "ChartGroupCollection.h"
#include "ChartLabelCollection.h"
#include "AxisCollection.h"
#include "DerivedDouble.h"
#include "Label.h"
#include "LabelCollection.h"
#include "Legend.h"
#include "2DCONST.H"
#include "DataIndex.h"
#include "DoubleEdit.h"
#include "DSealectDoc.h"
#include "DSealectGlobal.h"
#include "DSModelDraw.h"
#include "CDataModel.h"
#include "CFormationTopography.h"
#include "CAnalysis.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

#include <vector>




//static const double dY1	=  20.0;	// upperleft y coordinate of whole formation drawing area
//static const double dX1	=  20.0;	// upperleft x coordinate of whole formation drawing area
//static const double dY2		= 400.0;	// lowerright y coordinate of whole formation drawing area
//static const double dWidth		= 100.0;	// width of whole formation drawing area
static double dScreenYhi;						// highest y coordinate of single formation	
static double dScreenYlo;						// lowest y coordinate of single formation	

#undef max


void DrawFormations(CDC* pDC, double dX1, double dY1, double dY2, double dWidth, bool bColor)
{
	//get the current unit system.
	CDSealectDoc* pDoc=(CDSealectDoc*)GetDoc();
	QU::UNIT us = pDoc->UnitSystem();
	CLengthQuantity tmpLQ;
		
	if(GetDoc()->DSealectModel()->FormationTopography()->ElevationTVD()->Undefined())
		return ;
		
	if (GetDoc()->DSealectModel()->FormationTopography()->Offshore()) //offshore
	{
		if(GetDoc()->DSealectModel()->FormationTopography()->Seawater()->SeaBedTVD().Undefined())
			return ;
	}
	
	if(GetDoc()->DSealectModel()->FormationTopography()->NrOfFormations()<1)
		return ;
		
	if(GetDoc()->DSealectModel()->FormationTopography()->Overburden()->OverburdenBottomTVD()->Undefined())
		return ;
	
	LOGFONT logFont;
	logFont.lfHeight =18;
	logFont.lfWidth = 0;
	logFont.lfEscapement = 0;
	logFont.lfOrientation=0;
	logFont.lfStrikeOut=0;
	logFont.lfUnderline=0;
	logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;
	strcpy(logFont.lfFaceName ,"Courier");
	
	double dTVDhi;
	double dTVDlo;
	CString sName;

	CBrush brushColor(RGB(0,0,255)); 
	CBrush* oldBrush = pDC->SelectObject(&brushColor);
		
	CFormationTopography * pFT = GetDoc()->DSealectModel()->FormationTopography();

	if((!pFT->Overburden()->OverburdenBottomTVD()->Undefined()) && pFT->NrOfFormations()>0) 
	{
		CString sOverbBttmTVD;
		sOverbBttmTVD.Format("%#4.1f",pFT->Overburden()->OverburdenBottomTVD()->Value(us));
		pDC->TextOut(dX1+dWidth + 10,dY1 - 10,sOverbBttmTVD);
			
		for(int i = 0 ;i<pFT->NrOfFormations();i++)
		{	
			if(i==0)
			{
				dTVDhi = pFT->Overburden()->OverburdenBottomTVD()->Value();
			}
			else
			{
				dTVDhi = pFT->GetFormationAt(i - 1)->BottomTVD()->Value();
			}
			
			dTVDlo = pFT->GetFormationAt(i)->BottomTVD()->Value();
			sName = pFT->GetFormationAt(i)->FormationName();
			
			TVD2Screen(dTVDhi,dScreenYhi,dY1,dY2);
			TVD2Screen(dTVDlo,dScreenYlo,dY1,dY2);
				
			CBrush brushColor(RGB(255*(sin(2.0*i)), 255*(cos(i*2.0)), 255-255*(sin(i*2.0)))); //color dependens on i 
			CBrush brushWhite(RGB(255, 255, 255)); //white
			if(bColor)
			{
				pDC->SelectObject(&brushColor);	
			}
			else
			{
				pDC->SelectObject(&brushWhite);
			}
			
			pDC->Rectangle(dX1,dScreenYhi,dX1+dWidth,dScreenYlo); //draw formation as rectangle
			
			//CFont font;
			//font.CreateFontIndirect(&logFont);
			//pDC->SelectObject(&font);

			CString strVal;
			tmpLQ.Value(dTVDlo,CDoubleQuantity::SI_UNIT);
			strVal.Format("%#4.1f",tmpLQ.Value(us));
			pDC->TextOut(dX1+dWidth + 10,dScreenYlo - 10,strVal);
			
			pDC->SetBkMode(TRANSPARENT);
			pDC->TextOut(dX1 + 10,dScreenYlo - 15,sName);
		}
	}

	pDC->SelectObject(oldBrush);

}
void DrawModelRangeLine(CDC* pDC, double dX1, double dY1, double dY2, double dWidthFormations)
{
	if(GetDoc()->DSealectModel()->FormationTopography()->NrOfFormations()<1)
		return ;
	
	CFont fontTest;
	LOGFONT lFont;
	lFont.lfHeight =18;
	lFont.lfWidth = 0;
	lFont.lfWeight = FW_BOLD;
	lFont.lfEscapement = -900; // needed to turn text 90 degrees
	lFont.lfOrientation=0;
	lFont.lfStrikeOut=0;
	lFont.lfUnderline=0;
	lFont.lfPitchAndFamily = VARIABLE_PITCH | FF_MODERN;
	strcpy(lFont.lfFaceName ,"Courier");
	fontTest.CreateFontIndirect(&lFont);
	
	CPen penRed(PS_SOLID, 2, RGB(255,0,0));
	CPen* oldPen = pDC->SelectObject(&penRed);

	CDataModel * pDSM = GetDoc()->DSealectModel();
	
	pDC->Ellipse(dX1 + dWidthFormations + 100 - 2,dY1 - 2, dX1 + dWidthFormations + 2 + 100, dY1 + 2); // dots at begin and end of line
	pDC->Ellipse(dX1 + dWidthFormations + 100 - 2,dY2 - 2, dX1 + dWidthFormations + 2 + 100, dY2 + 2); // dots at begin and end of line

	pDC->MoveTo(dX1 + dWidthFormations + 100,dY1); 
	pDC->LineTo(dX1 + dWidthFormations + 100,dY2); //draw model range line
	
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255,0,0));
	pDC->TextOut(dX1 + dWidthFormations + 110, dY1 - 10, "Model Top TVD = Overburden bottom TVD");
	pDC->TextOut(dX1 + dWidthFormations + 110, dY2 - 10, "Model Bottom TVD");
	
	pDC->SelectObject(&fontTest); //use 90 degrees turned font 
	pDC->TextOut(dX1 + dWidthFormations + 125,(dY1 + dY2) / 2 - 50,"Model Range");

	pDC->SelectObject(oldPen);
	
}

CString GetDepthAsString(double dScrnY)
{
	//get the current unit system.
	CDSealectDoc* pDoc=(CDSealectDoc*)GetDoc();
	QU::UNIT us = pDoc->UnitSystem();

	CLengthQuantity tmpLQ;
	double dDepth;
	CString sDepth;

	Screen2TVD(dScrnY,dDepth,20.0,400.0);
	tmpLQ.Value(dDepth,CDoubleQuantity::SI_UNIT);
	sDepth.Format("%#4.1f",tmpLQ.Value(us));
	return sDepth;
	
}

void TVD2Screen(double dTVD, double &dScrnY, double dY1, double dY2)
{
	double dOverbBttmTVD;
	double dTVDMaxFormationNr;
	double dRico;
	
	CFormationTopography* pFT = GetDoc()->DSealectModel()->FormationTopography();
	dOverbBttmTVD = pFT->Overburden()->OverburdenBottomTVD()->Value();
	
	double k = pFT->NrOfFormations();
	dTVDMaxFormationNr = pFT->GetFormationAt(k - 1)->BottomTVD()->Value();

	dRico = ((dY2 - dY1) / (dTVDMaxFormationNr - dOverbBttmTVD));
	dScrnY = dRico * dTVD + dY1 - dOverbBttmTVD * dRico;
	 
}

void Screen2TVD(double dScrnY, double &dTVD, double dY1, double dY2)
{
	double dOverbBttmTVD;
	double dTVDMaxFormationNr;
	double dRico;

	CFormationTopography* pFT = GetDoc()->DSealectModel()->FormationTopography();
	dOverbBttmTVD = pFT->Overburden()->OverburdenBottomTVD()->Value();
	
	double k = pFT->NrOfFormations();
	dTVDMaxFormationNr = pFT->GetFormationAt(k - 1)->BottomTVD()->Value();

	dRico = ((dY2 - dY1) / (dTVDMaxFormationNr - dOverbBttmTVD));
	//dScrnY = dRico * dTVD + dY1 - dOverbBttmTVD * dRico;

	dTVD=((dOverbBttmTVD * dRico) + dScrnY - dY1)/dRico;
	 
}

void DrawWellPathLeadTail(CDC* pDC, double dX1, double dY1, double dY2, double dWidth)
{
	if(GetDoc()->DSealectModel()->FormationTopography()->NrOfFormations()<1)
		return ;

	if(!GetDoc()->DSealectModel()->DrillingScheme()->GetDrillingOperation(0)->CementingOperation()->CheckComplete())
		return;
	
	//draw wellpath through formations drawn with function DrawFormations
	double dTopTailX;
	double dTopTailY;
	double dTopLeadX;
	double dTopLeadY;
	CCementingOperation *pCO = GetDoc()->DSealectModel()->DrillingScheme()->GetDrillingOperation(0)->CementingOperation();
	
	if(pCO->TopOfTailCement()->Undefined())
		return;
  if(!GetDoc()->DSealectModel()->TMDtoTVD(pCO->TopOfCementTMD()->Value()).second)
    return;
  if(!GetDoc()->DSealectModel()->TMDtoTVD(pCO->TopOfTailCement()->Value()).second)
    return;

	CPen penLead(PS_SOLID, 3, RGB(255,0,0));
	CPen* oldPen = pDC->SelectObject(&penLead);
	CPen penTail(PS_SOLID, 3, RGB(0,255,0));
	CPen penAbove(PS_SOLID, 3, RGB(0,0,255));

	double dXbegin; 
	double dXend;
	if(GetDoc()->DSealectModel()->DrillingScheme()->NrOfDrillingDirections() == 1 &&
     GetDoc()->DSealectModel()->DrillingScheme()->GetDrillingDirectionSection(0)->WellInclination()->Value() == 0.0)
	{
		dXbegin = dX1 + dWidth/2; //draw vertical well
		dXend = dX1 + dWidth/2;
	}
	else
	{
		dXbegin = dX1 +10; //draw diagonal line
		dXend = dX1+dWidth-10;
	}

	pDC->MoveTo(dXbegin,dY1);
	
	pDC->SelectObject(penAbove); //select blue line
  assert(GetDoc()->DSealectModel()->TMDtoTVD(pCO->TopOfCementTMD()->Value()).second);
	TVD2Screen(GetDoc()->DSealectModel()->TMDtoTVD(pCO->TopOfCementTMD()->Value()).first,dTopLeadY,dY1,dY2);
	if(dTopLeadY > dY1)
	{
		dTopLeadX=(dXend-dXbegin)*((dTopLeadY-dY1)/(dY2-dY1))+dXbegin; 
		pDC->LineTo(dTopLeadX,dTopLeadY); //blue line from modeltop to lead-cement-top 
	}

	pDC->SelectObject(penLead); //select red line
	if(pCO->LeadTailCements())
	{
    assert(GetDoc()->DSealectModel()->TMDtoTVD(pCO->TopOfTailCement()->Value()).second);
		TVD2Screen(GetDoc()->DSealectModel()->TMDtoTVD(pCO->TopOfTailCement()->Value()).first,dTopTailY,dY1,dY2);
		dTopTailX=(dXend-dXbegin)*((dTopTailY-dY1)/(dY2-dY1))+dXbegin;
		
		pDC->LineTo(dTopTailX,dTopTailY); //red line from lead-cement-top to tail-cement-top (if tail exists)
		pDC->SelectObject(penTail); //sellect green line
	}
	
	pDC->LineTo(dXend,dY2); //red or green line to model-bottom 

	pDC->SelectObject(&penAbove);
	pDC->MoveTo(dX1+dWidth+110,dY2-60);
	pDC->LineTo(dX1+dWidth+115,dY2-50);
	pDC->TextOut(dX1+dWidth+125,dY2-60,"Mud");

	pDC->SelectObject(&penLead);
	pDC->MoveTo(dX1+dWidth+110,dY2-40);
	pDC->LineTo(dX1+dWidth+115,dY2-30);
	pDC->TextOut(dX1+dWidth+125,dY2-40,"Lead");

	pDC->SelectObject(&penTail);
	pDC->MoveTo(dX1+dWidth+110,dY2-20);
	pDC->LineTo(dX1+dWidth+115,dY2-10);
	pDC->TextOut(dX1+dWidth+125,dY2-20,"Tail");

	pDC->SelectObject(oldPen);

}

void DrawWellPathPlasticElastic(CDC* pDC, double dX1, double dY1, double dY2, double dWidth)
{
	if(GetDoc()->DSealectModel()->FormationTopography()->NrOfFormations()<1)
		return ;

	if(!GetDoc()->DSealectModel()->DrillingScheme()->GetDrillingOperation(0)->CheckComplete())
		return;

	if(!GetDoc()->DSealectModel()->IsPolyLineCreated())
		return;

	//draw wellpath points through formations drawn with function DrawFormations

	double dWX;
	double dWY;
	
	CPen pen(PS_SOLID, 3, RGB(0,0,0));
	CPen* oldPen = pDC->SelectObject(&pen);

	double dXbegin;
	double dXend;
	
	if(GetDoc()->DSealectModel()->DrillingScheme()->NrOfDrillingDirections() == 1 &&
     GetDoc()->DSealectModel()->DrillingScheme()->GetDrillingDirectionSection(0)->WellInclination()->Value() == 0.0)
	{
		dXbegin = dX1 + dWidth/2; //draw vertical well
		dXend = dX1 + dWidth/2;
	}
	else
	{
		dXbegin = dX1 +10; 
		dXend = dX1+dWidth-10;
	}

  CDataModel& model = *GetDoc()->DSealectModel();
  if(!model.IsPolyLineCreated())
    return;

  const geo::IPoint& ptLast = model.PolyLine().Point(model.PolyLine().PointSize() - 1);
  if(ptLast.Z() <= model.FormationTopography()->Overburden()->OverburdenBottomTVD()->Value())
    return;

	double dTVD = GetDoc()->DSealectModel()->GetModelTopTVD().Value();
	double NrOfSteps = 100.0;
	double dDelta = (GetDoc()->DSealectModel()->GetModelBottomTVD().Value() - dTVD) / NrOfSteps;
	double dDeltaScreenY = (dY2-dY1)/(NrOfSteps-1.0);
	double dDeltaScreenX = (dXend-dXbegin)/(NrOfSteps-1.0);

	CPen penP(PS_SOLID, 3, RGB(255,0,0));
	CPen penE(PS_SOLID, 3, RGB(0,0,0));

	pDC->SelectObject(&penE);
	pDC->MoveTo(dX1+dWidth+110,dY2-40);
	pDC->LineTo(dX1+dWidth+115,dY2-30);
	pDC->TextOut(dX1+dWidth+125,dY2-40,"Elastic");

	pDC->SelectObject(&penP);
	pDC->MoveTo(dX1+dWidth+110,dY2-20);
	pDC->LineTo(dX1+dWidth+115,dY2-10);
	pDC->TextOut(dX1+dWidth+125,dY2-20,"Plastic");

	for(int i=0 ;i< NrOfSteps;i++)
	{
    if(dTVD > ptLast.Z())
      break;

		TVD2Screen(dTVD,dWY,dY1,dY2);

		dWX=(dXend-dXbegin)*((dWY-dY1)/(dY2-dY1))+dXbegin;
		
		if(GetDoc()->DSealectModel()->GetTauRatioAtDepth(dTVD) > 1.0)
		{
			pDC->SelectObject(&penP);
		}
		else
		{
			pDC->SelectObject(&penE);
		}
		pDC->MoveTo(dWX,dWY);
		pDC->LineTo(dWX+dDeltaScreenX,dWY+dDeltaScreenY);
		dTVD = dTVD + dDelta;
	}
	pDC->SelectObject(oldPen);	

	

}

void DrawAnalysisPoints(CDC* pDC, double dX1, double dY1, double dY2, double dWidth)
{
	//get the current unit system.
	CDSealectDoc* pDoc=(CDSealectDoc*)GetDoc();
	QU::UNIT us = pDoc->UnitSystem();
	CLengthQuantity tmpLQ;

	if(GetDoc()->DSealectModel()->FormationTopography()->NrOfFormations()<1)
		return;

	if(GetDoc()->DSealectModel()->Analysis()->NrOfAnalysisPoints()<1)
		return;
	
	//draw analysis points on wellpath drawn with function DrawWellPathLeadTail
	double dAPX;
	double dAPY;
	
	CCementingOperation *pCO = GetDoc()->DSealectModel()->DrillingScheme()->GetDrillingOperation(0)->CementingOperation();
	if(pCO->TopOfTailCement()->Undefined() && pCO->LeadTailCements())
	{
		return;
	}

	CBrush brushColorAP(RGB(0,0,255)); 
	pDC->SelectObject(&brushColorAP);

	CPen pen(PS_DOT, 1, RGB(0,0,255));
	CPen* oldPen = pDC->SelectObject(&pen);

	double dXbegin ; 
	double dXend ;
	if(GetDoc()->DSealectModel()->DrillingScheme()->NrOfDrillingDirections() == 1 &&
     GetDoc()->DSealectModel()->DrillingScheme()->GetDrillingDirectionSection(0)->WellInclination()->Value() == 0.0)
	{
		dXbegin = dX1 + dWidth/2; //draw vertical well
		dXend = dX1 + dWidth/2;
	}
	else
	{
		dXbegin = dX1 +10; 
		dXend = dX1+dWidth-10;
	}


	for(int i=0 ;i< GetDoc()->DSealectModel()->Analysis()->NrOfAnalysisPoints();i++)
	{
		CAnalysisPoint *pAP=GetDoc()->DSealectModel()->Analysis()->GetAnalysisPointAt(i);	

    if(GetDoc()->DSealectModel()->TMDtoTVD(pAP->TMD()->Value()).second)
    {
		  TVD2Screen(GetDoc()->DSealectModel()->TMDtoTVD(pAP->TMD()->Value()).first,dAPY,dY1,dY2);

		  dAPX=(dXend-dXbegin)*((dAPY-dY1)/(dY2-dY1))+dXbegin;
  		
		  pDC->Ellipse(dAPX-4,dAPY-4,dAPX+4,dAPY+4); 
		  pDC->MoveTo(dX1,dAPY);
		  pDC->LineTo(dX1+dWidth, dAPY);

      assert(GetDoc()->DSealectModel()->TMDtoTVD(pAP->TMD()->Value()).second);
		  tmpLQ.Value(GetDoc()->DSealectModel()->TMDtoTVD(pAP->TMD()->Value()).first,CDoubleQuantity::SI_UNIT);
		  CString strI;
		  strI.Format("%#4.1f",tmpLQ.Value(us));
  		
		  pDC->SetBkMode(TRANSPARENT);
		  pDC->SetTextColor(RGB(0,0,255));
		  pDC->TextOut(dX1-55,dAPY-10,strI);
    }

	}
	pDC->SelectObject(oldPen);	
		
}

void DrawPorePress(CDC* pDC, double dScrnXBegin , double dPlotFactor, double dY1, double dY2)
{
	CPen GreenPen(PS_SOLID, 1, RGB(0,255,0));
	pDC->SelectObject(&GreenPen);

	CFormationTopography * pFT = GetDoc()->DSealectModel()->FormationTopography();

	double dTVD;
	double dScreenTVD;

  std::vector<std::pair<double, double> > vcValues = pFT->GetFirstFormation()->PorePressureValues();
  assert(vcValues.size() >= 2);
	double dTopPorePres = vcValues[0].second;
	double dBotPorePres;

	pDC->MoveTo(dScrnXBegin + dTopPorePres * dPlotFactor, dY1); //start position plotline
	for(int i = 0 ;i<pFT->NrOfFormations();i++)
	{	
		if(i!=0)
		{
			CFormation *pFormation=pFT->GetFormationAt(i);
      vcValues = pFormation->PorePressureValues();

      dTVD = vcValues[0].first;
			dTopPorePres = vcValues[0].second;
			
			TVD2Screen(dTVD,dScreenTVD,dY1,dY2);
			pDC->LineTo(dScrnXBegin + dTopPorePres * dPlotFactor,dScreenTVD);
		}

    for(int j = 1; j < vcValues.size(); ++j)
    {
      dTVD = vcValues[j].first;
      dBotPorePres = vcValues[j].second;

		  TVD2Screen(dTVD,dScreenTVD,dY1,dY2);
		  pDC->LineTo(dScrnXBegin + dBotPorePres * dPlotFactor,dScreenTVD);
    }
	}
}

void DrawStresses(CDC* pDC, double dX1FormPlot , double dWidthFormPlot, double dY1, double dY2)
{
	//get the current unit system.
	CDSealectDoc* pDoc=(CDSealectDoc*)GetDoc();
	QU::UNIT us = pDoc->UnitSystem();
	CSinglePressure tmpSPQ;
	CString sUnit;
	sUnit = GetUnitString(&tmpSPQ);

	if(GetDoc()->DSealectModel()->FormationTopography()->NrOfFormations()<1)
		return;
	
	for(int i =0 ;i<GetDoc()->DSealectModel()->FormationTopography()->NrOfFormations();i++)
	{
		if(!GetDoc()->DSealectModel()->FormationTopography()->GetFormationAt(i)->CheckInsituStressesComplete())
			return;
	}

	if(!GetDoc()->DSealectModel()->FormationTopography()->Overburden()->CheckComplete())
		return;
			
	LOGFONT logFontS;
	logFontS.lfHeight =18;
	logFontS.lfWidth = 0;
	logFontS.lfWeight = FW_BOLD;
	logFontS.lfEscapement = 0;
	logFontS.lfOrientation=0;
	logFontS.lfStrikeOut=0;
	logFontS.lfUnderline=0;
	logFontS.lfPitchAndFamily = VARIABLE_PITCH | FF_MODERN;
	strcpy(logFontS.lfFaceName ,"Symbol");
	
	CFont fontS;
	fontS.CreateFontIndirect(&logFontS); //fontS = Symbol font
	CFont* oldFont = pDC->SelectObject(&fontS); // current font
	
	pDC->SelectObject(oldFont);

	CPen pen(PS_SOLID, 3, RGB(0,0,255)); //blue axis lines
	CPen* oldPen = pDC->SelectObject(&pen);

	double dVSScrnWidth = 100.0; //width of vertical stress plot
	double dVSScrnXbegin; // upperleft x coordinate of Vertical Stress plot drawing area
	dVSScrnXbegin=dX1FormPlot+dWidthFormPlot+70;
	
	pDC->MoveTo(dVSScrnXbegin,dY1);			//start pos
	pDC->LineTo(dVSScrnXbegin,dY2);				//draw y-axis
	pDC->LineTo(dVSScrnXbegin+dVSScrnWidth,dY2);	//draw x-axis
	pDC->TextOut(dVSScrnXbegin, dY1-20, "Vertical Stress");
	
	pDC->SelectObject(fontS);
	pDC->TextOut(dVSScrnXbegin+dVSScrnWidth/2,dY2+14,"s");
	pDC->SelectObject(oldFont);
	pDC->TextOut(dVSScrnXbegin+dVSScrnWidth/2+12,dY2+17,"z " + sUnit);

	double dXHSScrnWidth = 100.0; //width of maX Horizontal stress plot
	double dXHSScrnXbegin; // upperleft x coordinate of maX Horizontal Stress plot drawing area
	dXHSScrnXbegin=dVSScrnXbegin+dVSScrnWidth+50;

	pDC->MoveTo(dXHSScrnXbegin,dY1);			//start pos
	pDC->LineTo(dXHSScrnXbegin,dY2);				//draw y-axis
	pDC->LineTo(dXHSScrnXbegin+dXHSScrnWidth,dY2);	//draw x-axis
	pDC->TextOut(dXHSScrnXbegin, dY1-20, "Max.Hor.Stress");

	pDC->SelectObject(fontS);
	pDC->TextOut(dXHSScrnXbegin+dXHSScrnWidth/2,dY2+14,"s");
	pDC->SelectObject(oldFont);
	pDC->TextOut(dXHSScrnXbegin+dXHSScrnWidth/2+12,dY2+17,"H " + sUnit);

	double dNHSScrnWidth = 100.0; //width of miN Horizontal stress plot
	double dNHSScrnXbegin; // upperleft x coordinate of miN Horizontal Stress plot drawing area
	dNHSScrnXbegin=dXHSScrnXbegin+dXHSScrnWidth+50;

	pDC->MoveTo(dNHSScrnXbegin,dY1);			//start pos
	pDC->LineTo(dNHSScrnXbegin,dY2);				//draw y-axis
	pDC->LineTo(dNHSScrnXbegin+dVSScrnWidth,dY2);	//draw x-axis
	pDC->TextOut(dNHSScrnXbegin, dY1-20, "Min.Hor.Stress");

	pDC->SelectObject(fontS);
	pDC->TextOut(dNHSScrnXbegin+dVSScrnWidth/2,dY2+14,"s");
	pDC->SelectObject(oldFont);
	pDC->TextOut(dNHSScrnXbegin+dVSScrnWidth/2+12,dY2+17,"h " + sUnit);

	pDC->SelectObject(oldPen);

	CFormationTopography * pFT = GetDoc()->DSealectModel()->FormationTopography();

	double dVertStress;
	double dMaxHorStress;
	double dMinHorStress;
	double dScreenTVD;
	
	//double k = pFT->NrOfFormations();
	double dOBVStress= pFT->GetFirstFormation()->VerticalStresses().begin()->second;
//****** draw vertical stress plot
	double dPlotFactor = dVSScrnWidth / pFT->GetLastFormation()->VerticalStresses().rbegin()->second;
		
	pDC->MoveTo(dVSScrnXbegin + dOBVStress * dPlotFactor, dY1); //start position plotline
	for(int i = 0 ;i<pFT->NrOfFormations();i++)
	{	
		CFormation *pFormation=pFT->GetFormationAt(i);
    CFormation::TDepthValueVector vcStresses = pFormation->VerticalStresses();

    for(int j = 1; j < vcStresses.size(); ++j)
    {
      TVD2Screen(vcStresses[j].first, dScreenTVD, dY1, dY2);
      pDC->LineTo(dVSScrnXbegin + vcStresses[j].second * dPlotFactor, dScreenTVD);
    }

    dVertStress = vcStresses[vcStresses.size() - 1].second;
	}

	DrawPorePress(pDC, dVSScrnXbegin , dPlotFactor, dY1, dY2);
	pDC->SelectObject(oldPen);

	CString strVert;
	tmpSPQ.Value(dVertStress,CDoubleQuantity::SI_UNIT);
	strVert.Format("%#1.4g",tmpSPQ.Value(us));
	//strVert.Format("%#4.2f",dVertStress);
	pDC->SetBkMode(TRANSPARENT);
	pDC->TextOut(dVSScrnXbegin + dVertStress * dPlotFactor-20,dScreenTVD +5,strVert);


//*****	draw max horizontal stress plot	
	dOBVStress = pFT->GetFirstFormation()->MaxHorStresses().begin()->second;
	dPlotFactor = dXHSScrnWidth / pFT->GetLastFormation()->MaxHorStresses().rbegin()->second;
	pDC->MoveTo(dXHSScrnXbegin + dOBVStress * dPlotFactor, dY1); //start position plotline
	for(int i = 0; i < pFT->NrOfFormations(); i++)
	{
    CFormation* pFormation = pFT->GetFormationAt(i);
    CFormation::TDepthValueVector vcMHStresses = pFormation->MaxHorStresses();

		if(i)
		{
			TVD2Screen(vcMHStresses[0].first,dScreenTVD,dY1,dY2);
			pDC->LineTo(dXHSScrnXbegin + vcMHStresses[0].second * dPlotFactor,dScreenTVD);
		}

    for(int j = 1; j < vcMHStresses.size(); ++j)
    {
      TVD2Screen(vcMHStresses[j].first, dScreenTVD, dY1, dY2);
  		pDC->LineTo(dXHSScrnXbegin + vcMHStresses[j].second * dPlotFactor, dScreenTVD);
    }

    dMaxHorStress = vcMHStresses[vcMHStresses.size() - 1].second;
	}

	DrawPorePress(pDC, dXHSScrnXbegin , dPlotFactor, dY1, dY2);
	pDC->SelectObject(oldPen);


	CString strMax;
	tmpSPQ.Value(dMaxHorStress,CDoubleQuantity::SI_UNIT);
	strMax.Format("%#1.4g",tmpSPQ.Value(us));
	//strMax.Format("%#4.2f",dMaxHorStress);
	pDC->SetBkMode(TRANSPARENT);
	pDC->TextOut(dXHSScrnXbegin + dMaxHorStress * dPlotFactor-20,dScreenTVD +5,strMax);

//***** draw min horizontal stress plot
	dOBVStress = pFT->GetFirstFormation()->MinHorStresses().begin()->second;
	dPlotFactor = dNHSScrnWidth / pFT->GetLastFormation()->MinHorStresses().rbegin()->second;
	pDC->MoveTo(dNHSScrnXbegin + dOBVStress * dPlotFactor, dY1); //start position plotline
	for(int i = 0 ; i < pFT->NrOfFormations(); i++)
	{	
    CFormation* pFormation = pFT->GetFormationAt(i);
    CFormation::TDepthValueVector vcMHStresses = pFormation->MinHorStresses();

		if(i)
		{
			TVD2Screen(vcMHStresses[0].first,dScreenTVD,dY1,dY2);
			pDC->LineTo(dNHSScrnXbegin + vcMHStresses[0].second * dPlotFactor,dScreenTVD);
		}

    for(int j = 1; j < vcMHStresses.size(); ++j)
    {
      TVD2Screen(vcMHStresses[j].first, dScreenTVD, dY1, dY2);
  		pDC->LineTo(dNHSScrnXbegin + vcMHStresses[j].second * dPlotFactor, dScreenTVD);
    }

    dMinHorStress = vcMHStresses[vcMHStresses.size() - 1].second;
	}
	
	DrawPorePress(pDC, dNHSScrnXbegin , dPlotFactor, dY1, dY2);
	pDC->SelectObject(oldPen);


	CString strMin;
	tmpSPQ.Value(dMinHorStress,CDoubleQuantity::SI_UNIT);
	strMin.Format("%#1.4g",tmpSPQ.Value(us));
	pDC->SetBkMode(TRANSPARENT);
	pDC->TextOut(dNHSScrnXbegin + dMinHorStress * dPlotFactor-20,dScreenTVD +5,strMin);

	CPen GreenPen(PS_SOLID, 1, RGB(0,255,0));
	pDC->SelectObject(&GreenPen);
	pDC->MoveTo(dNHSScrnXbegin ,dScreenTVD +60);
	pDC->LineTo(dNHSScrnXbegin +30,dScreenTVD +80);
	pDC->TextOut(dNHSScrnXbegin + 35,dScreenTVD +60,"Pore Pressure");

	pDC->SelectObject(oldPen);
	pDC->MoveTo(dNHSScrnXbegin ,dScreenTVD +80);
	pDC->LineTo(dNHSScrnXbegin +30,dScreenTVD +100);
	pDC->TextOut(dNHSScrnXbegin + 35,dScreenTVD +80,"Total Stress");

}

void DrawCrossSectGeom(CDC* pDC)
{
	double dCScrnXCenter = 200.0;
	double dCScrnYCenter = 200.0;
	double dCScrnWidth = 300.0; //equal to screen-borehole

	CDrillingOperation* pDO = GetDoc()->DSealectModel()->DrillingScheme()->GetDrillingOperation(0);
	CString Name = pDO->CasingOperation()->CasingMaterialName();

	if(Name.IsEmpty())
		return;

	CCasingMaterial* pCM= GetDoc()->DSealectModel()->CasingMatList()->GetMaterial(Name);
	
	if(pDO->BoreholeDiameter()->Undefined() || pDO->CasingOperation()->PercentageEccentricity()->Undefined()) //TODO: check inner and outer
	{
		return;
	}

	double dBorehole = pDO->BoreholeDiameter()->Value();
	double dCasingOuter = pCM->CasingOuterDiameter()->Value();
	double dCasingInner = pCM->CasingInnerDiameter()->Value();
	double dPercEccent = pDO->CasingOperation()->PercentageEccentricity()->Value();
	
	double dEccentrShift = ((dBorehole - dCasingOuter) / 2 * dPercEccent); //formula on page 11/59
	double dESB = dEccentrShift / dBorehole;	//factor needed in screen draw
	double dFCOB = dCasingOuter / dBorehole;	//factor needed in screen draw
	double dFCIB = dCasingInner / dBorehole;	//factor needed in screen draw

	CBrush brushColorRect(RGB(255,200,150)); 
	pDC->SelectObject(&brushColorRect);
	double dScrnY1Rect = dCScrnYCenter - dCScrnWidth / 1.9; 
	pDC->Rectangle(dCScrnXCenter - dCScrnWidth / 1.9,
					dScrnY1Rect,
					dCScrnXCenter + dCScrnWidth / 1.9,
					dCScrnYCenter + dCScrnWidth / 1.9); //draw rectangle
	
	CBrush brushColorSeal(RGB(200,200,200)); 
	pDC->SelectObject(&brushColorSeal);
	double dScrnY1Seal = dCScrnYCenter - dCScrnWidth / 2;
	pDC->Ellipse(dCScrnXCenter - dCScrnWidth / 2,
					dScrnY1Seal,
					dCScrnXCenter + dCScrnWidth / 2,
					dCScrnYCenter + dCScrnWidth / 2); //draw sealant cirkel

	CBrush brushColorCasingOut(RGB(10,10,10)); 
	pDC->SelectObject(&brushColorCasingOut);
	double dScrnY1CasOut = dCScrnYCenter + dESB * dCScrnWidth - dFCOB * dCScrnWidth / 2;
	pDC->Ellipse(dCScrnXCenter - dFCOB * dCScrnWidth / 2,
					dScrnY1CasOut,
					dCScrnXCenter + dFCOB * dCScrnWidth / 2,
					dCScrnYCenter + dESB * dCScrnWidth + dFCOB * dCScrnWidth / 2); //draw casing outer cirkel

	CBrush brushColorCasingIn(RGB(255,255,255)); 
	pDC->SelectObject(&brushColorCasingIn);
	double dScrnY1CasIn = dCScrnYCenter + dESB * dCScrnWidth - dFCIB * dCScrnWidth / 2;
	pDC->Ellipse(dCScrnXCenter - dFCIB * dCScrnWidth / 2,
					dScrnY1CasIn,
					dCScrnXCenter + dFCIB * dCScrnWidth / 2,
					dCScrnYCenter + dESB * dCScrnWidth + dFCIB * dCScrnWidth / 2); //draw casing inner cirkel
	
	CPen penRed(PS_SOLID, 2, RGB(255,0,0));
	CPen* oldPen = pDC->SelectObject(&penRed);

	pDC->MoveTo(dCScrnXCenter, (dScrnY1Rect + dScrnY1Seal) / 2);
	pDC->LineTo(dCScrnXCenter + (dCScrnWidth / 2) + 10,	(dScrnY1Rect + dScrnY1Seal) / 2);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255,0,0));
	pDC->TextOut(dCScrnXCenter + (dCScrnWidth / 2) + 20 ,(dScrnY1Rect + dScrnY1Seal) / 2 - 10, "Rock");
	//pDC->TextOut(dCScrnXCenter ,(dScrnY1Rect + dScrnY1Seal) / 2-10,"Rock");

	pDC->MoveTo(dCScrnXCenter, (dScrnY1Seal+dScrnY1CasOut) / 2);
	pDC->LineTo(dCScrnXCenter + (dCScrnWidth / 2) + 10,	(dScrnY1Seal + dScrnY1CasOut) / 2);
	pDC->TextOut(dCScrnXCenter + (dCScrnWidth / 2) + 20,(dScrnY1Seal + dScrnY1CasOut) / 2 - 10, "Cement");
	//pDC->TextOut(dCScrnXCenter ,(dScrnY1Seal + dScrnY1CasOut) / 2-10,"Cement");

	pDC->MoveTo(dCScrnXCenter, (dScrnY1CasOut+dScrnY1CasIn) / 2);
	pDC->LineTo(dCScrnXCenter + (dCScrnWidth / 2) + 10,	(dScrnY1CasOut + dScrnY1CasIn) / 2);
	pDC->TextOut(dCScrnXCenter + (dCScrnWidth / 2) + 20,(dScrnY1CasOut + dScrnY1CasIn) / 2 - 10, "Casing");
	//pDC->TextOut(dCScrnXCenter ,(dScrnY1CasOut + dScrnY1CasIn) / 2-10,"Casing");

	pDC->SelectObject(oldPen);
}


void DrawMesh(CDC* pDC)
{
	double dCScrnXCenter = 300.0;
	double dCScrnYCenter = 300.0;
	double dCScrnWidth = 300.0; 
	double dNrOfTangent = 2 * GetDoc()->DSealectModel()->MeshParam()->NDivTangent();
	
	CDrillingOperation* pDO = GetDoc()->DSealectModel()->DrillingScheme()->GetDrillingOperation(0);	
	double dPlotFactor = 40.0 * (dCScrnWidth / (50.0 * pDO->BoreholeDiameter()->Value())); // todo :40 is temp
	
	double dTangentAngle = 360.0 / dNrOfTangent;
	double dSumAngle = 0.0;
	double R1;
	double R2;
	double X1;
	double X2;
	double Y1;
	double Y2;

	CPen penBlue(PS_SOLID, 1, RGB(0,0,255));
	CPen* oldPen = pDC->SelectObject(&penBlue);
	
	
	CPen penRed(PS_SOLID, 1, RGB(255,0,0));
	CPen penGreen(PS_SOLID, 1, RGB(0,255,0));
	
	std::vector<double> Vec;
	
	pDC->MoveTo(dCScrnXCenter,dCScrnYCenter);

	pDC->SetROP2(R2_MASKPEN);

	for(int i = 1 ; i<4; i++) //1=casing, 2=cement 3=rock
	{
		if(i == 1)
		{
			GetDoc()->DSealectModel()->MeshParam()->GetMeshRadiCas(Vec);
			pDC->SelectObject(&penBlue);
		}
		if(i == 2)
		{
			GetDoc()->DSealectModel()->MeshParam()->GetMeshRadiCem(Vec);
			pDC->SelectObject(&penRed);
		}
		if(i == 3)
		{
			GetDoc()->DSealectModel()->MeshParam()->GetMeshRadiRock(Vec);
			pDC->SelectObject(&penGreen);
		}

		dSumAngle=0.0;
		for(int j = 0 ; j < dNrOfTangent ; j++)
		{
			R1 = Vec[0] ;
			R2 = Vec[Vec.size()-1] ;
			X1 = R1 * cos(dSumAngle * PI /180);
			Y1 = R1 * sin(dSumAngle * PI /180);
			X2 = R2 * cos(dSumAngle * PI /180);
			Y2 = R2 * sin(dSumAngle * PI /180);
			pDC->MoveTo(X1 * dPlotFactor+dCScrnXCenter,Y1 * dPlotFactor+dCScrnYCenter);
			pDC->LineTo(X2 * dPlotFactor+dCScrnXCenter,Y2 * dPlotFactor+dCScrnYCenter);
			dSumAngle= dSumAngle+dTangentAngle;
		}
		
		for(int c = 0 ; c < Vec.size() ; c++)
		{
			R1 = Vec[c];
								
			pDC->Ellipse(	dCScrnXCenter - R1 * dPlotFactor, 
							dCScrnYCenter - R1 * dPlotFactor, 
							dCScrnXCenter + R1 * dPlotFactor,  
							dCScrnYCenter + R1 * dPlotFactor);
		}
		
	}
	
	pDC->SelectObject(oldPen);
}

CChartLabel AddLabel(CChart2D* ch, CString text ,int serie ,long point, long offset, long anchor ,long rotation) 
{        
    //Add a label to a Chart
    CChartLabel lb;
    lb = ch->GetChartLabels().Add();
    
	lb.SetAttachMethod(oc2dAttachDataIndex);
    lb.GetText().SetText(text);
    lb.GetAttachDataIndex().SetSeries(serie);
    lb.GetAttachDataIndex().SetPoint(point);
    lb.SetOffset(offset);
    lb.SetAnchor(anchor);
    lb.SetRotation(rotation);
    return lb;

}



void DrawMohrChart(CChart2D* m_pChart, double dDepth)
{
	//get the current unit system.
	CDSealectDoc* pDoc=(CDSealectDoc*)GetDoc();
	QU::UNIT us = pDoc->UnitSystem();
	CSinglePressure tmpSPQ;
	CString sUnit;
	sUnit = GetUnitString(&tmpSPQ);

	if(!GetDoc()->DSealectModel()->FormationTopography()->CheckComplete())
	{
		//m_pChart->ShowWindow(FALSE);	
		return;
	}
	//m_pChart->ShowWindow(TRUE);
	

	tmpSPQ.Value(GetDoc()->DSealectModel()->GetEffectiveVertStressAtDepth(dDepth),CDoubleQuantity::SI_UNIT);
	double dVert = tmpSPQ.Value(us);
	tmpSPQ.Value(GetDoc()->DSealectModel()->GetEffectiveMaxHorizontalStressAtDepth(dDepth),CDoubleQuantity::SI_UNIT);
	double dMaxHor = tmpSPQ.Value(us);
	tmpSPQ.Value(GetDoc()->DSealectModel()->GetEffectiveMinHorizontalStressAtDepth(dDepth),CDoubleQuantity::SI_UNIT);
	double dMinHor = tmpSPQ.Value(us);
	
	double dSmax;
	double dSmin;
	double dSmid;
	CString strmin;
	CString strmid;
	CString strmax;

	CAxis xa;
	CAxis ya;
	CChart2DData dat;

	if(dVert > dMaxHor)
	{
		dSmax = dVert;
		strmax = "Sv";
		dSmid = dMaxHor;
		strmid = "SH";
		dSmin = dMinHor;
		strmin = "Sh";
	}
	else
	{
		dSmax = dMaxHor;
		strmax = "SH";
		if(dVert < dMinHor)
		{
			dSmid = dMinHor;
			strmid = "Sh";
			dSmin = dVert;
			strmin = "Sv";

		}
		else
		{
			dSmid = dVert;
			strmid = "Sv";
			dSmin = dMinHor;
			strmin = "Sh";
		}
	}

	CFormationTopography* pFT = GetDoc()->DSealectModel()->FormationTopography();
	CString Name = pFT->GetFormationAtDepth(dDepth)->MaterialName();
	
	double dCoh;
	double dFriction;

	bool bAllDataDefined = false;
	if(!Name.IsEmpty())
	{
		CFormationMaterial* pFM= GetDoc()->DSealectModel()->FormationMatList()->GetMaterial(Name);
		if(pFM->Plasticity())
		{
			bAllDataDefined = true;
		}
	}

	if(bAllDataDefined)
	{
		CFormationMaterial* pFM= GetDoc()->DSealectModel()->FormationMatList()->GetMaterial(Name);
		//tmpSPQ.Value(pFM->Cohesion()->Value(),CDoubleQuantity::SI_UNIT);
		tmpSPQ.Value(GetDoc()->DSealectModel()->GetCohesionAtDepth(dDepth),CDoubleQuantity::SI_UNIT);
		dCoh = tmpSPQ.Value(us);

		tmpSPQ.Value(GetDoc()->DSealectModel()->GetFrictionAtDepth(dDepth),CDoubleQuantity::SI_UNIT);
		dFriction = tmpSPQ.Value(us);
		//dFriction=	pFM->FrictionAngle()->Value(us);
	}

	m_pChart->SetIsBatched(TRUE);
	
	xa = m_pChart->GetChartArea().GetAxes().GetItem(COleVariant("X"));
	ya = m_pChart->GetChartArea().GetAxes().GetItem(COleVariant("Y"));

	double dMaxX;
	double dMaxY;
	double dMinX;
	double dMinY;
	double dMaxVal;
		
	//Max, min for the chart  
	
	/*
	UITGECOMMENT OM EVEN MEE TE TESTEN.....
	
	dMaxX = dSmax;
	if(bAllDataDefined)
	{
		dMinX=0.0;
		//dMinX = std::_MAX(-(dCoh / tan(PI * dFriction / 180.0)), -dSmax);
		//dMaxY = dCoh + dMaxX * tan(PI * dFriction / 180.0);
		dMaxY = (dMaxX-dMinX)/2.0;
	}
	else
	{
		dMinX = dSmin ;//0.0; //-dSmax;
		dMaxY = (dMaxX-dMinX)/2.0;
	}
	
	dMinY = 0.0;
	*/

	dMaxX = 3.0/2.0 * dSmax - 0.5 * dSmin;
	dMinX = 3.0/2.0 * dSmin - 0.5 * dSmax;
	dMaxY = dSmax - dSmin;
	dMinY = 0.0;


	dMaxVal = std::max(dMaxX, dMaxY);

	//set axis scale
	xa.GetMax().SetValue(dMaxVal);
	xa.GetDataMax().SetValue(dMaxVal);
	xa.GetMin().SetValue(dMinX);
	xa.GetDataMin().SetValue(dMinX);
	xa.GetTitle().SetText("Sigma " + sUnit);

	ya.GetMax().SetValue((dMaxVal - dMinX)/2);
	ya.GetDataMax().SetValue((dMaxVal - dMinX)/2);
	ya.GetMin().SetValue(dMinY);
	ya.GetDataMin().SetValue(dMinY);
	ya.GetTitle().SetText("Tau " + sUnit);

	CChartGroup group= m_pChart->GetChartGroups().GetItem(COleVariant(long (1)));
	
	dat = group.GetData();
	if(bAllDataDefined)
	{
		dat.SetNumSeries(5);
	}
	else
	{
		dat.SetNumSeries(3);
	}
	
	long lNrOfPoints = 36;
	long lSerie;
	double dRad;
	double dMid;
	double da;
	double dAA;

	
	//Draw circle mid & min
	lSerie = 1;
	dRad = (dSmid - dSmin) / 2.0;
	dMid = (dSmid + dSmin) / 2.0;
	dat.SetNumPoints(lSerie,lNrOfPoints);
	da = PI / (lNrOfPoints - 1);
	for(int i = 0; i<lNrOfPoints ; i++)
	{
		dAA = i * da;
		dat.SetX(1,i+1,(dMid + dRad * cos(dAA)));
		dat.SetY(lSerie,i+1,(dRad * sin(dAA)));
	}

	//Draw circle max & mid
	lSerie = 2;
	dRad = (dSmax - dSmid) / 2.0;
	dMid = (dSmax + dSmid) / 2.0;
	dat.SetNumPoints(lSerie,lNrOfPoints);
	da = PI / (lNrOfPoints - 1);
	for(int i = 0; i<lNrOfPoints ; i++)
	{
		dAA = i * da;
		dat.SetX(1,i+1,(dMid + dRad * cos(dAA)));
		dat.SetY(lSerie,i+1,(dRad * sin(dAA)));
	}

	//Draw circle max & min
	lSerie = 3;
	dRad = (dSmax - dSmin) / 2.0;
	dMid = (dSmax + dSmin) / 2.0;
	dat.SetNumPoints(lSerie,lNrOfPoints);
	da = PI / (lNrOfPoints - 1);
	for(int i = 0; i < lNrOfPoints ; i++)
	{
		dAA = i * da;
		dat.SetX(1,i + 1,(dMid + dRad * cos(dAA)));
		dat.SetY(lSerie,i + 1,(dRad * sin(dAA)));
	}

	CChartLabel lb;
	//Add Chrtlabels
	m_pChart->GetChartLabels().RemoveAll();

	AddLabel(m_pChart,strmin,1,lNrOfPoints,5, oc2dAnchorAuto,oc2dRotateNone);	
	AddLabel(m_pChart,strmid,2,lNrOfPoints,5, oc2dAnchorAuto,oc2dRotateNone);	
	AddLabel(m_pChart,strmax,3,1,5, oc2dAnchorAuto,oc2dRotateNone);

	if(bAllDataDefined)
	{
		//Draw Friction line	
		lSerie = 4;
		dat.SetNumPoints(lSerie,2);
		//dat.SetX(lSerie,1,dMinX);
		//dat.SetY(lSerie,1,dCoh + dMinX * tan(PI * dFriction / 180.0));
		dat.SetX(1,1,0.0);
		dat.SetY(lSerie,1,dCoh);
		dat.SetX(1,2,dMaxX);
		dat.SetY(lSerie,2,dCoh + dMaxX * tan(PI * dFriction / 180.0));

		//Draw perpendicular line
		lSerie = 5;
		dRad = (dSmax - dSmin) / 2.0;
		dMid = (dSmax + dSmin) / 2.0;
		dat.SetNumPoints(lSerie,2);
		double l2;
		double a2;
		double tm;

		a2 = PI / 2.0 - atan(dMid / dCoh);
		l2 = sqrt(pow(dCoh,2.0) + pow(dMid,2.0));
		tm = l2 * sin(PI * dFriction / 180.0 + a2);
		dat.SetX(1,1,dMid);
		dat.SetY(lSerie,1,0.0);
		dat.SetX(1,2,dMid - tm * sin(PI * dFriction /180.0));
		dat.SetY(lSerie,2,tm * cos(PI * dFriction / 180.0));

		AddLabel(m_pChart,"Tau Max",5,2,20, oc2dAnchorAuto,oc2dRotateNone);

		lb = AddLabel(m_pChart,"Coh.Strength",4,1,25,oc2dAnchorEast,oc2dRotate90Degrees);
		lb.SetIsConnected(TRUE);
	}

	m_pChart->SetIsBatched(FALSE);
	m_pChart->Refresh();
	

}

void DrawAnalysisCoordSystem(CAxisX *pAxis, double dTMD)
{
	geo::CMatrix mNegRot;
	GetDoc()->DSealectModel()->CalculateNEgMatrix(mNegRot, dTMD);

	
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
			double val=mNegRot.Value(j,i);
			SafeArrayPutElement(psa[i],&j,&val);
		}
		Var[i].vt= VT_ARRAY | VT_R8;
		Var[i].parray=psa[i];
	}

	pAxis->SetAxisExt(Var[0],Var[1],Var[2]);
	
	for(i=0;i<3;i++)
	{
		SafeArrayDestroy(psa[i]);
	}

	pAxis->ShowWindow(SW_SHOW);
}

void DrawBitmap(CDC* pDC, CBitmap& bitmap, CPalette *pPal, double dX1, double dY1)
{
	// Create a compatible memory DC
	CDC memDC;
	memDC.CreateCompatibleDC( pDC );
	memDC.SelectObject( &bitmap );

	// Select and realize the palette
	if( pPal != NULL && pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE )
	{
		pDC->SelectPalette( pPal, FALSE );
		pDC->RealizePalette();
	}

	BITMAP bm;
	bitmap.GetBitmap( &bm );

	pDC->BitBlt(dX1, dY1, bm.bmWidth, bm.bmHeight, &memDC, 0, 0,SRCCOPY);
}

double CircularBoreholeLength()
{ 
	return PI * GetDoc()->DSealectModel()->DrillingScheme()->GetDrillingOperation(0)->BoreholeDiameter()->Value();
}

double CircularCasingLength()
{

	CString Name = GetDoc()->DSealectModel()->DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->CasingMaterialName();
	CCasingMaterial* pCM= GetDoc()->DSealectModel()->CasingMatList()->GetMaterial(Name);
	
	return PI * pCM->CasingOuterDiameter()->Value();
}