#if !defined(_DSMODELDRAW__INCLUDED_)
#define _DSMODELDRAW__INCLUDED_

#include "axisx.h"
#include "Chart2D.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define SCRN_TOP_PLOT  20.0
#define SCRN_BTTM_PLOT  400.0
#define SCRN_LEFT_PLOT  20.0
#define SCRN_WIDTH_PLOT  100.0

void DrawCrossSectGeom(CDC* pDC);

void DrawFormations(CDC* pDC, double dX1, double dY1, double dY2, double dWidth,bool bColor);
void TVD2Screen(double dTVD, double &dScrnY, double dY1, double dY2);
void Screen2TVD(double dScrnY, double &dTVD, double dY1, double dY2);
void DrawPorePress(CDC* pDC, double dScrnXBegin , double dPlotFactor, double dY1, double dY2);
void DrawStresses(CDC* pDC, double dX1FormPlot , double dWidthFormPlot, double dY1, double dY2);
void DrawWellPathLeadTail(CDC* pDC, double dX1, double dY1, double dY2, double dWidth);
void DrawWellPathPlasticElastic(CDC* pDC, double dX1, double dY1, double dY2, double dWidth);
void DrawAnalysisPoints(CDC* pDC, double dX1, double dY1, double dY2, double dWidth);
void DrawMesh(CDC* pDC);
void DrawModelRangeLine(CDC* pDC, double dX1, double dY1, double dY2, double dWidthFormations);
void DrawMohrChart(CChart2D* m_pChart, double dDepth);
CString GetDepthAsString(double dScrnY);
void DrawAnalysisCoordSystem(CAxisX *pAxis, double dTMD);
void DrawBitmap(CDC* pDC, CBitmap& bitmap, CPalette *pPal , double dX1, double dY1);
double CircularBoreholeLength();
double CircularCasingLength();



#endif // !defined(_DSMODELDRAW__INCLUDED_)