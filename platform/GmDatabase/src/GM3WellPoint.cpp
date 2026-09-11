// WellPoint.cpp: implementation of the C3DWellPoint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <cmath>
#include "GM3WellPoint.h"
#include "GM3TableDef.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif


namespace gm {

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C3DWellPoint::C3DWellPoint(const geo::IPoint& point)
:CPoint(point),m_dAzimuth(0), m_dInclination(0), m_dAHD(point.Z()), m_bDirectCalc(false)
{
}

C3DWellPoint::C3DWellPoint():m_dAzimuth(0), m_dInclination(0), m_dAHD(0), m_bDirectCalc(false)
{
  m_vcDirect.resize(0);
}

C3DWellPoint::C3DWellPoint(const double &dAHD, const double &dInclination, const double &dAzimuth, const double &dX, const double &dY, const double &dZ):
geo::CPoint(dX, dY, dZ), m_dAzimuth(dAzimuth), m_dInclination(dInclination), m_dAHD(dAHD), m_bDirectCalc(false)
{
}

C3DWellPoint::~C3DWellPoint()
{
}

// copy constructor
C3DWellPoint::C3DWellPoint(const C3DWellPoint &WellPoint) : geo::CPoint(WellPoint)
{
  Azimuth(WellPoint.Azimuth());
  AlongHoleDepth(WellPoint.AlongHoleDepth());
  Inclination(WellPoint.Inclination());

  if(WellPoint.DirectVectCalculated() && !m_bDirectCalc)
  {// calculate the directional vector for the first time.
    CalcDirectionalVec(); 
  }
  else if(WellPoint.DirectVectCalculated() && m_bDirectCalc)
  {// The directional vector is no longer valid and should be recalculated.
    m_bDirectCalc = false;
    CalcDirectionalVec(); // m_bDirecCalc = true when function returns...
  }
}

void C3DWellPoint::CalcDirectionalVec()
{
  if(!m_bDirectCalc)
  {
    m_vcDirect.resize(3);

    double sinincl = sin(m_dInclination * DEG_TO_RAD);
    double sinazi = sin(m_dAzimuth * DEG_TO_RAD);
    double cosincl = cos(m_dInclination * DEG_TO_RAD);
    double cosazi = cos(m_dAzimuth * DEG_TO_RAD);

    m_vcDirect[0] = sinincl * cosazi;
    m_vcDirect[1] = sinincl * sinazi;
    m_vcDirect[2] = cosincl;

    m_bDirectCalc = true;
  }
}

const std::vector<double> C3DWellPoint::DirectionVect() 
{
  CalcDirectionalVec();
  return m_vcDirect;
}

void C3DWellPoint::Azimuth(const double &dValue)
{
  m_dAzimuth = dValue; 
  
  if(DirectVectCalculated())
  {
    m_bDirectCalc = false;
    CalcDirectionalVec();
  }
}

void C3DWellPoint::Inclination(const double &dValue)
{
  m_dInclination = dValue; 
  
  if(DirectVectCalculated())
  {
    m_bDirectCalc = false;
    CalcDirectionalVec();
  }
}

bool C3DWellPoint::operator <(const C3DWellPoint &rhs) const
{
  if(AlongHoleDepth() - rhs.AlongHoleDepth() < -EPS)
    return true;
  
  return false;
}

}