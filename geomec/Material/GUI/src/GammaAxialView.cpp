// GammaView.cpp : implementation file
//

#include "stdafx.h"
#include "GammaAxialView.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CGammaAxialView

IMPLEMENT_DYNCREATE(CGammaAxialView, CView)

CGammaAxialView::CGammaAxialView()
{
}

CGammaAxialView::~CGammaAxialView()
{
}

//wjrx mantis 3167
double CGammaAxialView::GetStressOrStrain( CStressStrain const &StressStrain ) const
{
  return GetStrain(StressStrain);
}

double CGammaAxialView::GetStrain( CStressStrain const &StressStrain ) const
{
  return StressStrain.m_dAxialStrain;
}

double CGammaAxialView::GetStress( CStressStrain const &StressStrain ) const
{
  return StressStrain.m_dAxialStress;
}
