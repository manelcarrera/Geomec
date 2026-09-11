#include "RotatedSystem.h"

#include "Point.h"
#include "Vector.h"
#include "IElement.h"

CRotatedSystem::CRotatedSystem(double dAzimuth, const geo::IPoint& ptRotation) // azimuth in deg
: m_dAzimuth(DEG2RAD(dAzimuth)),
  m_pptRotation(new geo::CPoint(ptRotation))
{
}

CRotatedSystem::~CRotatedSystem()
{
  delete m_pptRotation;
}

// convert from global (x, y) to local (xi, eta) system
geo::CPoint CRotatedSystem::ToLocal(const geo::IPoint& pt) const
{
  return geo::CPoint(
     (pt.X() - m_pptRotation->X()) * cos(m_dAzimuth) + (pt.Y() - m_pptRotation->Y()) * sin(m_dAzimuth),
    -(pt.X() - m_pptRotation->X()) * sin(m_dAzimuth) + (pt.Y() - m_pptRotation->Y()) * cos(m_dAzimuth),
     pt.Z()
  );
}

geo::CVector CRotatedSystem::ToLocal(const geo::IVector& v) const
{
  return geo::CVector(
     v.X() * cos(m_dAzimuth) + v.Y() * sin(m_dAzimuth),
    -v.X() * sin(m_dAzimuth) + v.Y() * cos(m_dAzimuth),
     v.Z()
  );
}

void CRotatedSystem::ToLocalSet(geo::IPoint& pt) const
{
  pt.Set((pt.X() - m_pptRotation->X()) * cos(m_dAzimuth) + (pt.Y() - m_pptRotation->Y()) * sin(m_dAzimuth),
        -(pt.X() - m_pptRotation->X()) * sin(m_dAzimuth) + (pt.Y() - m_pptRotation->Y()) * cos(m_dAzimuth),
          pt.Z());
}

void CRotatedSystem::ToLocalSet(geo::IVector& v) const
{
  v.Set(v.X() * cos(m_dAzimuth) + v.Y() * sin(m_dAzimuth),
       -v.X() * sin(m_dAzimuth) + v.Y() * cos(m_dAzimuth),
        v.Z());
}

// convert from local (xi, eta) to global (x, y) system
geo::CPoint CRotatedSystem::ToGlobal(const geo::IPoint& pt) const
{
  return geo::CPoint(
    pt.X() * cos(m_dAzimuth) - pt.Y() * sin(m_dAzimuth) + m_pptRotation->X(),
    pt.X() * sin(m_dAzimuth) + pt.Y() * cos(m_dAzimuth) + m_pptRotation->Y(),
    pt.Z()
  );
}

geo::CVector CRotatedSystem::ToGlobal(const geo::IVector& v) const
{
  return geo::CVector(
    v.X() * cos(m_dAzimuth) - v.Y() * sin(m_dAzimuth),
    v.X() * sin(m_dAzimuth) + v.Y() * cos(m_dAzimuth),
    v.Z()
  );
}

void CRotatedSystem::ToGlobalSet(geo::IPoint& pt) const
{
  pt.Set(pt.X() * cos(m_dAzimuth) - pt.Y() * sin(m_dAzimuth) + m_pptRotation->X(),
         pt.X() * sin(m_dAzimuth) + pt.Y() * cos(m_dAzimuth) + m_pptRotation->Y(),
         pt.Z());
}

void CRotatedSystem::ToGlobalSet(geo::IVector& v) const
{
  v.Set(v.X() * cos(m_dAzimuth) - v.Y() * sin(m_dAzimuth),
        v.X() * sin(m_dAzimuth) + v.Y() * cos(m_dAzimuth),
        v.Z());
}

// get the local (xi, eta) min point (in the local system)
geo::CPoint CRotatedSystem::LocalMin(const geo::IElement& element) const
{
  geo::CPoint ptRet(ToLocal(element.Point(0)));
  for(int i = 1; i < element.NrOfPoints(); ++i)
  {
    geo::CPoint pt(ToLocal(element.Point(i)));
    if(pt.X() < ptRet.X())
      ptRet.X(pt.X());
    if(pt.Y() < ptRet.Y())
      ptRet.Y(pt.Y());
    if(pt.Z() < ptRet.Z())
      ptRet.Z(pt.Z());
  }

  return ptRet;
}

// get the local (xi, eta) max point (in the local system)
geo::CPoint CRotatedSystem::LocalMax(const geo::IElement& element) const
{
  geo::CPoint ptRet(ToLocal(element.Point(0)));
  for(int i = 1; i < element.NrOfPoints(); ++i)
  {
    geo::CPoint pt(ToLocal(element.Point(i)));
    if(pt.X() > ptRet.X())
      ptRet.X(pt.X());
    if(pt.Y() > ptRet.Y())
      ptRet.Y(pt.Y());
    if(pt.Z() > ptRet.Z())
      ptRet.Z(pt.Z());
  }

  return ptRet;
}

