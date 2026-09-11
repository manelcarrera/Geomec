 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// Sphere.cpp: implementation of the CSphere class.
//
//////////////////////////////////////////////////////////////////////

#include "dimple.h"
#include "Sphere.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace geo {

CSphere::CSphere()
: m_mid_point(), m_radius(0)
{
}

CSphere::CSphere(const IPoint& mid_point, const double radius)
: m_mid_point(mid_point), m_radius(radius)
{
}

CSphere::CSphere(const ISphere& rhs)
: m_mid_point(rhs.MidPoint()), m_radius(rhs.Radius())
{
}

const double& CSphere::Radius() const
{
  return m_radius;
}

void CSphere::Radius(const double &radius)
{
  m_radius = radius;
}

const IPoint& CSphere::MidPoint() const 
{
  return m_mid_point;
}

void CSphere::MidPoint(const IPoint& mid_point)
{
  m_mid_point = mid_point;
}



}
