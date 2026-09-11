// GammaView.cpp : implementation file
//

#include "stdafx.h"
#include "GammaRadialView.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CGammaRadialView

IMPLEMENT_DYNCREATE(CGammaRadialView, CView)

CGammaRadialView::CGammaRadialView()
{
}

CGammaRadialView::~CGammaRadialView()
{
}

//wjrx mantis 3167
double CGammaRadialView::GetStressOrStrain( CStressStrain const &StressStrain ) const
{
  return GetStress(StressStrain);
}

double CGammaRadialView::GetStrain( CStressStrain const &StressStrain ) const
{
  return StressStrain.m_dRadialStrain;
}

double CGammaRadialView::GetStress( CStressStrain const &StressStrain ) const
{
  return StressStrain.m_dRadialStress;
}
