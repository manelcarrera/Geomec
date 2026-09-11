// GammaView.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "GammaView.h"

//#include "MatUnit.h"
#include <QHBoxLayout>
#include <QColor>

#include "GeomecStringTable.h"
#include "MaterialUnitTypes.h"
#include "MatParamDlg.h"
#include "qwt_series_data.h"
#include "qwt_point_data.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

static QColor GetQtColor(unsigned int index)
{
  switch(index % 12)
  {
    case  0: return Qt::red; break;
    case  1: return Qt::green; break;
    case  2: return Qt::blue; break;
    case  3: return Qt::cyan; break;
    case  4: return Qt::magenta; break;
    case  5: return Qt::yellow; break;
    case  6: return Qt::darkRed; break;
    case  7: return Qt::darkGreen; break;
    case  8: return Qt::darkBlue; break;
    case  9: return Qt::darkCyan; break;
    case 10: return Qt::darkMagenta; break;
    default: return Qt::darkYellow; break;
  }
}
/////////////////////////////////////////////////////////////////////////////
// CGammaView

//IMPLEMENT_DYNCREATE(CGammaView, CScrollView)

CGammaView::CGammaView():m_pWinWidget(0),m_pPlot(0)
{
}

CGammaView::~CGammaView()
{
}

BOOL CGammaView::PlotData(const CExperimentArray &aData, const CResultData *pResData, BOOL bIncludeYieldPoint, BOOL bIsTopGraph, mlUnitDef ud,
  bool useInitialLowerLimit, const std::vector <int>& initialLowerLimit)
{
  if (m_pPlot == 0) return FALSE;
  QString Text;
  Text = QString("effective axial stress (%1)").arg(CUnitTypeStress().UnitName(ud));

  m_pPlot->setAxisTitle(QwtPlot::xBottom, Text);

  if(bIsTopGraph)
    Text = "axial strain (-)";
  else
  {
    //wjrx mantis 3167
    Text = QString("radial stress (%1)").arg(CUnitTypeStress().UnitName(ud));
  }

  m_pPlot->setAxisTitle(QwtPlot::yLeft, Text);

  m_pPlot->detachItems(); // detach and (auto)delete curves and grid

  CPlotGrid *grid= new CPlotGrid;
  grid->attach(m_pPlot);

  for (int iExperiment= 0; iExperiment< aData.size(); ++iExperiment)
  {
    CString strLabelAddExperiment;
    strLabelAddExperiment = ", Experiment data";

    CCrossPlotCurve *crv = new CCrossPlotCurve
      (LPCTSTR(aData[iExperiment].GetLabel() + strLabelAddExperiment)
      , GetQtColor(iExperiment)
     );

    // wjrx mantis 3158
    //
    // |
    // |
    // |     crvLower     crv   crvUpper
    // |
    // |
    // |-----------------------------------
    //  <---lowerSize--->|   |<-upperSize->
    //                   v   v
    //  lowerLimitFraction   upperLimitFraction
    //
    CCrossPlotCurve *crvLower = 0;
    CCrossPlotCurve *crvUpper = 0;
    double lowerLimitFraction= 0;
    double upperLimitFraction= 1;
    int lowerSize= 0;
    int upperSize= 0;

    lowerLimitFraction= aData[iExperiment].getLowerLimitFraction();
    upperLimitFraction= aData[iExperiment].getUpperLimitFraction();
    if (lowerLimitFraction > 0)
    {
      crvLower = new CCrossPlotCurve("", Qt::darkGray);
    }

    if (upperLimitFraction < 1)
    {
      crvUpper = new CCrossPlotCurve("", Qt::darkGray);
    }

    // Original comment from ChartBgChart.cpp:
    // "Exanded interface to use stresse TRUE as last
    // parameter is use stress false is not
    // depending on the override in the derived classes"
    CStressStrainArray const &aStrainSteps =
      GetStrainSteps(aData[iExperiment]);
    if (crvLower)
    {
      lowerSize= lowerLimitFraction * aStrainSteps.size();
      double *xx= new double[lowerSize];
      double *yy= new double[lowerSize];
      for (int iPoint=0, iii= 0; iPoint<lowerSize; ++iPoint, ++iii)
      {
	      xx[iii]= CUnitTypeStress().ToUserUnit(aStrainSteps[iPoint].m_dAxialStress, ud);
	      yy[iii]= GetStressOrStrain(aStrainSteps[iPoint]);
      }
      crvLower->setData( new QwtPointArrayData(xx,yy,lowerSize));
      delete [] xx;
      delete [] yy;

      crvLower->attach(m_pPlot);

      // remove this plot from the legend
      crvLower->setItemAttribute(QwtPlotItem::Legend, false);
    }

    if (crvUpper)
    {
      upperSize= (1.0-upperLimitFraction) * aStrainSteps.size();
      double *xx= new double[upperSize];
      double *yy= new double[upperSize];
      for(int iPoint = aStrainSteps.size() - upperSize, iii = 0; iPoint<aStrainSteps.size(); ++iPoint, ++iii)
      {
	      xx[iii]= CUnitTypeStress().ToUserUnit(aStrainSteps[iPoint].m_dAxialStress, ud);
	      yy[iii]= GetStressOrStrain(aStrainSteps[iPoint]);
      }
      crvUpper->setData( new QwtPointArrayData(xx,yy,upperSize));
      delete [] xx;
      delete [] yy;

      crvUpper->attach(m_pPlot);

      // remove this plot from the legend
      crvUpper->setItemAttribute(QwtPlotItem::Legend, false);
    }

    double *xx= new double[aStrainSteps.size()-lowerSize-upperSize];
    double *yy= new double[aStrainSteps.size()-lowerSize-upperSize];

    for (int iPoint= lowerSize, iii= 0; iPoint<aStrainSteps.size() - upperSize; ++iPoint, ++iii)
    {
      xx[iii]= CUnitTypeStress().ToUserUnit(aStrainSteps[iPoint].m_dAxialStress, ud);
      yy[iii]= GetStressOrStrain(aStrainSteps[iPoint]);

      if (!bIsTopGraph)
      {
        yy[iii] = CUnitTypeStress().ToUserUnit(yy[iii], ud);
      }
    }
    crv->setData(
      new QwtPointArrayData(xx,yy,aStrainSteps.size()-lowerSize-upperSize));
    delete [] xx;
    delete [] yy;

    crv->attach(m_pPlot);
  }

  if (pResData)
  for (int iResult = 0; iResult< pResData->GetNrExperiments(); ++iResult)
  {
    CString strLabelAddCalculated;
    strLabelAddCalculated = ", Calculated data";

    CLinePlotCurve *crv = new CLinePlotCurve
      (LPCTSTR(aData[iResult].GetLabel() + strLabelAddCalculated)
      , GetQtColor(iResult)
     );

    CStressStrainArray const &aStrainSteps =
      GetStrainSteps(pResData, iResult);

    double *xx= new double[aStrainSteps.size()];
    double *yy= new double[aStrainSteps.size()];

    for(size_t iPoint=0; iPoint<aStrainSteps.size(); ++iPoint)
    {
      xx[iPoint]= CUnitTypeStress().ToUserUnit(aStrainSteps[iPoint].m_dAxialStress, ud);
      if(bIsTopGraph)
      {
        yy[iPoint]= GetStressOrStrain(aStrainSteps[iPoint])
          + (useInitialLowerLimit ?
              aData[iResult].getAxialStrainOffset(initialLowerLimit[iResult] / 100.0) :
              aData[iResult].getAxialStrainOffset()); // wjrx mantis 3199
      }
      else
      {
        yy[iPoint]= GetStressOrStrain(aStrainSteps[iPoint]);
      }
    }
    crv->setData(new QwtPointArrayData(xx,yy,aStrainSteps.size()));
    delete [] xx;
    delete [] yy;

    crv->attach(m_pPlot);
  }

  m_pPlot->replot();

  return TRUE;
}

#ifdef min
#undef min  // To give std::min precedence over #define min(...)
#endif  // min

#ifdef max
#undef max  // To give std::max precedence over #define max(...)
#endif  // max

BOOL CGammaView::PlotPQ(const CExperimentArray &aData, const CResultData *pResData, BOOL bIsTopGraph, const CLibraryMaterial *pMat, mlUnitDef ud)
{
  if (m_pPlot == 0) return FALSE;
  QString Text;

  Text = QString("P (%1)").arg(CUnitTypeStress().UnitName(ud));
  m_pPlot->setAxisTitle(QwtPlot::xBottom, Text);

  Text = QString("Q (%1)").arg(CUnitTypeStress().UnitName(ud));
  m_pPlot->setAxisTitle(QwtPlot::yLeft, Text);

  m_pPlot->detachItems(); // detach and (auto)delete curves and grid

  CPlotGrid *grid= new CPlotGrid;
  grid->attach(m_pPlot);

  double dPMin;
  double dPMax;
  BOOL bInitialized = FALSE;
  for (int iExperiment= 0; iExperiment< aData.size(); ++iExperiment)
  {
    CString strLabelAddExperiment;
    strLabelAddExperiment = ", Experiment data";

    CCrossPlotCurve *crv = new CCrossPlotCurve
      (LPCTSTR(aData[iExperiment].GetLabel() + strLabelAddExperiment)
      , GetQtColor(iExperiment)
     );

    CCrossPlotCurve *crvLower = 0;
    CCrossPlotCurve *crvUpper = 0;
    double lowerLimitFraction= 0;
    double upperLimitFraction= 1;
    int lowerSize= 0;
    int upperSize= 0;

    lowerLimitFraction= aData[iExperiment].getLowerLimitFraction();
    upperLimitFraction= aData[iExperiment].getUpperLimitFraction();
    if (lowerLimitFraction > 0)
    {
      crvLower = new CCrossPlotCurve("", Qt::darkGray);
    }

    if (upperLimitFraction < 1)
    {
      crvUpper = new CCrossPlotCurve("", Qt::darkGray);
    }

    // Original comment from ChartBgChart.cpp:
    // "Exanded interface to use stresse TRUE as last
    // parameter is use stress false is not
    // depending on the override in the derived classes"
    CStressStrainArray const &aStrainSteps =
      GetStrainSteps(aData[iExperiment]);

    if (crvLower)
    {
      lowerSize= lowerLimitFraction * aStrainSteps.size();
      double *xx1= new double[lowerSize];
      double *yy1= new double[lowerSize];
      for (int iPoint=0, iii= 0; iPoint<lowerSize; ++iPoint, ++iii)
      {
        if(bInitialized)
        {
          dPMin = std::min(dPMin,aStrainSteps[iPoint].m_dP);
          dPMax = std::max(dPMax,aStrainSteps[iPoint].m_dP);
        }
        else
        {
          dPMin = aStrainSteps[iPoint].m_dP;
          dPMax = aStrainSteps[iPoint].m_dP;
          bInitialized = TRUE;
        }
        xx1[iii]= CUnitTypeStress().ToUserUnit(aStrainSteps[iPoint].m_dP, ud);
        yy1[iii]= CUnitTypeStress().ToUserUnit(aStrainSteps[iPoint].m_dQ, ud);
      }
      crvLower->setData( new QwtPointArrayData(xx1,yy1,lowerSize));
      delete [] xx1;
      delete [] yy1;

      crvLower->attach(m_pPlot);

      // remove this plot from the legend
      crvLower->setItemAttribute(QwtPlotItem::Legend, false);
    }

    if (crvUpper)
    {
      upperSize= (1.0-upperLimitFraction) * aStrainSteps.size();
      double *xx1= new double[upperSize];
      double *yy1= new double[upperSize];
      for (int iPoint=aStrainSteps.size()-upperSize, iii= 0
          ; iPoint<aStrainSteps.size()
	  ; ++iPoint, ++iii)
      {
        if(bInitialized)
        {
          dPMin = std::min(dPMin,aStrainSteps[iPoint].m_dP);
          dPMax = std::max(dPMax,aStrainSteps[iPoint].m_dP);
        }
        else
        {
          dPMin = aStrainSteps[iPoint].m_dP;
          dPMax = aStrainSteps[iPoint].m_dP;
          bInitialized = TRUE;
        }
        xx1[iii]= CUnitTypeStress().ToUserUnit(aStrainSteps[iPoint].m_dP, ud);
        yy1[iii]= CUnitTypeStress().ToUserUnit(aStrainSteps[iPoint].m_dQ, ud);
      }
      crvUpper->setData( new QwtPointArrayData(xx1,yy1,upperSize));
      delete [] xx1;
      delete [] yy1;

      crvUpper->attach(m_pPlot);

      // remove this plot from the legend
      crvUpper->setItemAttribute(QwtPlotItem::Legend, false);
    }

    double *xx1= new double[aStrainSteps.size()-lowerSize-upperSize];
    double *yy1= new double[aStrainSteps.size()-lowerSize-upperSize];

    for (int iPoint=lowerSize, iii= 0
        ; iPoint<aStrainSteps.size() - upperSize
        ; ++iPoint, ++iii)
    {
      if(bInitialized)
      {
        dPMin = std::min(dPMin,aStrainSteps[iPoint].m_dP);
        dPMax = std::max(dPMax,aStrainSteps[iPoint].m_dP);
      }
      else
      {
        dPMin = aStrainSteps[iPoint].m_dP;
        dPMax = aStrainSteps[iPoint].m_dP;
        bInitialized = TRUE;
      }
      xx1[iii]= CUnitTypeStress().ToUserUnit(aStrainSteps[iPoint].m_dP, ud);
      yy1[iii]= CUnitTypeStress().ToUserUnit(aStrainSteps[iPoint].m_dQ, ud);
    }
    crv->setData(
      new QwtPointArrayData(xx1,yy1,aStrainSteps.size()-lowerSize-upperSize));
    delete [] xx1;
    delete [] yy1;

    crv->attach(m_pPlot);
  }

  if (pResData)
  for (int iResult = 0; iResult< pResData->GetNrExperiments(); ++iResult)
  {
    CString strLabelAddCalculated;
    strLabelAddCalculated = ", Calculated data";

    CLinePlotCurve *crv = new CLinePlotCurve
      (LPCTSTR(aData[iResult].GetLabel() + strLabelAddCalculated)
      , GetQtColor(iResult)
     );

    CStressStrainArray const &aStrainSteps =
      GetStrainSteps(pResData, iResult);

    double *xx1= new double[aStrainSteps.size()];
    double *yy1= new double[aStrainSteps.size()];

    for (size_t iPoint=0; iPoint<aStrainSteps.size(); ++iPoint)
    {
      xx1[iPoint]= CUnitTypeStress().ToUserUnit(aStrainSteps[iPoint].m_dP, ud);
      yy1[iPoint]= CUnitTypeStress().ToUserUnit(aStrainSteps[iPoint].m_dQ, ud);
    }
    crv->setData( new QwtPointArrayData(xx1,yy1,aStrainSteps.size()));
    delete [] xx1;
    delete [] yy1;

    crv->attach(m_pPlot);
  }

  if(pMat && bInitialized)
  {
    // Ok Min and Max determined
    double dProcentDiffP = (fabs (dPMax) - fabs(dPMin)) * 0.10; // 10 percent

    dPMin += -dProcentDiffP;
    dPMax -= -dProcentDiffP;

    CStressStrainArray YieldCurveArray;
    if(pMat->GetPrimaryYieldCurve(dPMin,dPMax,YieldCurveArray))
    {
      if(!YieldCurveArray.empty())
      {
        double *xx1= new double[YieldCurveArray.size()];
        double *yy1= new double[YieldCurveArray.size()];

        for (size_t iCount = 0; iCount < YieldCurveArray.size(); iCount ++)
        {
          xx1[iCount]= CUnitTypeStress().ToUserUnit(YieldCurveArray[iCount].m_dP, ud);
          yy1[iCount]= CUnitTypeStress().ToUserUnit(YieldCurveArray[iCount].m_dQ, ud);
        }

        CString sYieldCurve1;
        sYieldCurve1 = "Yield Curve";

        CLinePlotCurve *crv1 = new CLinePlotCurve
          (LPCTSTR(sYieldCurve1)
          , Qt::black
         );

        crv1->setData( new QwtPointArrayData(xx1,yy1,YieldCurveArray.size()));
        delete [] xx1;
        delete [] yy1;

        crv1->attach(m_pPlot);

        YieldCurveArray.clear();
        if(pMat->GetSecondaryYieldCurve(dPMin, dPMax, YieldCurveArray))
        {
          if(!YieldCurveArray.empty())
          {
            double *xx2= new double[YieldCurveArray.size()];
            double *yy2= new double[YieldCurveArray.size()];

            for (size_t iCount = 0; iCount < YieldCurveArray.size(); iCount ++)
            {
              xx2[iCount]= CUnitTypeStress().ToUserUnit(YieldCurveArray[iCount].m_dP, ud);
              yy2[iCount]= CUnitTypeStress().ToUserUnit(YieldCurveArray[iCount].m_dQ, ud);
            }

            CString sYieldCurve2;
            sYieldCurve2 = "Secondary Yield Curve";

            CDashedPlotCurve *crv2 = new CDashedPlotCurve
              (LPCTSTR(sYieldCurve2)
              , Qt::black
             );

            crv2->setData( new QwtPointArrayData(xx2,yy2,YieldCurveArray.size()));
            delete [] xx2;
            delete [] yy2;

            crv2->attach(m_pPlot);
          }
        }
      }
    }
  }

  m_pPlot->replot();

  return TRUE;
}

CStressStrainArray const &CGammaView::GetStrainSteps(const CExperimentData &Source) const
{
    // Default Parameter is not needed here as these are always stresses.
	return Source.GetStressStrainSteps();
}

CStressStrainArray const &CGammaView::GetStrainSteps(const CResultData *pSource, int iSerie) const
{
    // Default Parameter is not needed here as these are always stresses.
	return *(&(pSource->GetStressStrainSteps()[iSerie]));
}

BEGIN_MESSAGE_MAP(CGammaView, CView)
	//{{AFX_MSG_MAP(CGammaView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGammaView drawing

void CGammaView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
}

void CGammaView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CGammaView diagnostics

#ifdef _DEBUG
void CGammaView::AssertValid() const
{
	CView::AssertValid();
}

void CGammaView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGammaView message handlers

int CGammaView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (CView::OnCreate(lpCreateStruct) == -1)
    return -1;

  m_pWinWidget = new QWinWidget(GetSafeHwnd(), 0, 0);
  m_pPlot = new CPlot(m_pWinWidget);

  QHBoxLayout *hbox= new QHBoxLayout(m_pWinWidget);
  hbox->setSpacing(0);
  hbox->setMargin(0);
  hbox->addWidget(m_pPlot);
  m_pWinWidget->move(0,0);
  m_pWinWidget->show();

  return 0;
}

void CGammaView::OnDestroy() 
{
	CView::OnDestroy();

  	if (m_pWinWidget != 0)
	{
		delete m_pWinWidget;
		m_pWinWidget= 0;
	}
}

void CGammaView::OnSize(UINT nType, int cx, int cy) 
{
  CView::OnSize(nType, cx, cy);

  if (m_pWinWidget) m_pWinWidget->resize (cx , cy);

	CWnd *pParent = GetParent();
  CMatParamDlg* pDlg = 0;
	while(pParent && !(pDlg = dynamic_cast<CMatParamDlg*>(pParent)))
    pParent = pParent->GetParent();

  if(pDlg)
    pDlg->OnChildViewSized();
}

void CGammaView::SendMessageToDialog(UINT uMsg, WPARAM wParam /* = 0 */, LPARAM lParam /* = 0 */)
{
	CWnd *pParent = GetParent();
	while(pParent && !pParent->SendMessage(uMsg, wParam, lParam)) pParent = pParent->GetParent();
}
