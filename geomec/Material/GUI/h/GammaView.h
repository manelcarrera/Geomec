#if !defined(AFX_GAMMAVIEW_H__2AC1FE39_E847_11D3_B3AD_00A0C941C60B__INCLUDED_)
#define AFX_GAMMAVIEW_H__2AC1FE39_E847_11D3_B3AD_00A0C941C60B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GammaView.h : header file
//

#include "ResultData.h"
#include "ExperimentData.h"
#include "Materials.h"

#include <qwinwidget.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_grid.h>
#include <qwt_plot.h>
#include <qwt_legend.h>
#include <qwt_symbol.h>

class CPlotGrid : public QwtPlotGrid
{
public:
  CPlotGrid():QwtPlotGrid()
  {
  setMajorPen(QPen(Qt::black, 0, Qt::DotLine));
  setMinorPen(QPen(Qt::gray, 0, Qt::DotLine));
  }
};

class CLegend : public QwtLegend
{
public:
  CLegend():QwtLegend()
  {
  setDefaultItemMode(QwtLegendData::ReadOnly);
  }
};

class CPlot : public QwtPlot
{
public:
  CPlot(QWidget *w):QwtPlot(w)
  {
  setCanvasBackground(QColor(Qt::white));
  insertLegend(m_pLegend= new CLegend, QwtPlot::BottomLegend);
  setAutoReplot(false); // instead, use replot()
  }
  CLegend *Legend() {return m_pLegend;}

private:
  CLegend *m_pLegend;
};

class CCrossPlotCurve : public QwtPlotCurve
{
public:
  CCrossPlotCurve
  (const QString &title
  , const QColor &color
 ):QwtPlotCurve(title)
  {
  setYAxis(QwtPlot::yLeft);
  setStyle(QwtPlotCurve::NoCurve);
  QwtSymbol *pSym= new QwtSymbol;
  pSym->setStyle(QwtSymbol::XCross);
  pSym->setPen(color);
  pSym->setSize(5);
  setSymbol(pSym);
  }
};

class CLinePlotCurve : public QwtPlotCurve
{
private:
public:
  CLinePlotCurve
  (const QString &title
  , const QColor &color
 ):QwtPlotCurve(title)
  {
  setYAxis(QwtPlot::yLeft);
  setPen(color);
  }
};

class CDashedPlotCurve : public QwtPlotCurve
{
private:
public:
  CDashedPlotCurve
  (const QString &title
  , const QColor &color
 ):QwtPlotCurve(title)
  {
  setYAxis(QwtPlot::yLeft);
  QPen pen(color);
  pen.setStyle(Qt::DashLine);
  setPen(pen);
  }
};

/////////////////////////////////////////////////////////////////////////////
// CGammaView view

class CGammaView : public CView
{
protected:
  CGammaView();           // protected constructor used by dynamic creation
//	DECLARE_DYNCREATE(CGammaView)

// Attributes
public:
protected:

// Operations
public:
  BOOL PlotData(const CExperimentArray &aData, const CResultData *pResData, BOOL bIncludeYieldPoint, BOOL bIsTopGraph, mlUnitDef ud,
    bool useInitialLowerLimit, const std::vector <int>& initialLowerLimit);
  BOOL PlotPQ(const CExperimentArray &aData, const CResultData *pResData, BOOL bIsTopGraph, const CLibraryMaterial *pMat, mlUnitDef ud);

protected:
  // wjrx mantis 3167
  virtual double GetStressOrStrain(CStressStrain const &StressStrain) const = 0;
  virtual double GetStrain(CStressStrain const &StressStrain) const = 0;
  virtual double GetStress(CStressStrain const &StressStrain) const = 0;
  void SendMessageToDialog(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0);
  CStressStrainArray const &GetStrainSteps(const CExperimentData &Source) const;
  CStressStrainArray const &GetStrainSteps(const CResultData *pSource, int iSerie) const;

  QWinWidget *m_pWinWidget;
  CPlot *m_pPlot;

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CGammaView)
  protected:
  virtual void OnDraw(CDC* pDC);      // overridden to draw this view
  virtual void OnInitialUpdate();     // first time after construct
  //}}AFX_VIRTUAL

// Implementation
protected:
  virtual ~CGammaView();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

  // Generated message map functions
  //{{AFX_MSG(CGammaView)
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GAMMAVIEW_H__2AC1FE39_E847_11D3_B3AD_00A0C941C60B__INCLUDED_)
