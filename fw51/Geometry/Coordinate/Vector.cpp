 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// Vector.cpp: implementation of the CVector class.
//
//////////////////////////////////////////////////////////////////////
#include "dimple.h"
#include "Point.h"
#include "Vector.h"
#include "Line.h"
#include "math.h"

namespace geo {


const CVector CVector::NullVector(0.0, 0.0, 0.0);
const CVector CVector::Xaxis(1.0, 0.0, 0.0);
const CVector CVector::Yaxis(0.0, 1.0, 0.0);
const CVector CVector::Zaxis(0.0, 0.0, 1.0);

//returns the determinant for 3X3 matrix
double Determinant(const CVector &v1, const CVector &v2, const CVector &v3)
{
  double dX;
  double dY;
  double dZ;

  dX = v2.Y() * v3.Z() - v3.Y() * v2.Z();
  dY = v2.X() * v3.Z() - v3.X() * v2.Z();
  dZ = v2.X() * v3.Y() - v3.X() * v2.Y();

  return v1.X() * dX - v1.Y() * dY + v1.Z() * dZ;
}

//gives the solution of 3 liniar equeations with 3 unknowns
//v4=p*v1+q*v2+r*v3
//return CVector(p,q,r)
CVector GetSolution(const CVector &v1, const CVector &v2, const CVector &v3, const CVector &v4)
{
  double dD;
  double dP;
  double dQ;
  double dR;

  dD = Determinant(v1, v2, v3);

  if(dD == 0.0)	{
    //No solution
    return CVector();
  }

  dP = Determinant(v4, v2, v3);
  dQ = Determinant(v1, v4, v3);
  dR = Determinant(v1, v2, v4);

  return CVector(dP/dD, dQ/dD, dR/dD);
}


CVector::CVector()
: m_empty( true )
{
}


CVector::CVector(const double &dX, const double &dY, const double &dZ)
: m_empty( false )
{
  m_coor[0] = dX;
  m_coor[1] = dY;
  m_coor[2] = dZ;
  assert(!Empty());
}

CVector CVector::CreateFromAngles(const double& azimuth_deg, const double& inclination_deg)
{
  double sin_i = sin(DEG2RAD(inclination_deg));
  double cos_i = cos(DEG2RAD(inclination_deg));
  double cos_a = cos(DEG2RAD(azimuth_deg));
  double sin_a = sin(DEG2RAD(azimuth_deg));

  return CVector(cos_a*sin_i,sin_a*sin_i,-cos_i);
}

CVector::CVector(const IVector &rhs)
: m_empty( rhs.Empty() )
{
  if(!rhs.Empty()) {
    m_coor[0] = rhs.X();
    m_coor[1] = rhs.Y();
    m_coor[2] = rhs.Z();
  }
}

CVector::CVector(const CVector &rhs)
: m_empty( rhs.Empty() )
{
  if(!rhs.Empty()) {
    m_coor[0] = rhs.X();
    m_coor[1] = rhs.Y();
    m_coor[2] = rhs.Z();
  }
}

CVector::CVector(const IPoint &pt)
: m_empty( pt.Empty() )
{
  if(!pt.Empty()) {
    pt.AssertValid();
    m_coor[0] = pt.X();
    m_coor[1] = pt.Y();
    m_coor[2] = pt.Z();
    assert(!Empty());
  }
}

CVector::CVector(const ILine &line)
: m_empty( line.Empty() )
{
  if(!line.Empty()) {
    line.AssertValid();
    m_coor[0] = line.Second().X() - line.First().X();
    m_coor[1] = line.Second().Y() - line.First().Y();
    m_coor[2] = line.Second().Z() - line.First().Z();
    assert(!Empty());
  }
}

CVector::CVector(const IPoint &first, const IPoint &second)
: m_empty( false )
{
  assert(first.Empty() == false);
  assert(second.Empty() == false);

  m_coor[0] = second.X() - first.X();
  m_coor[1] = second.Y() - first.Y();
  m_coor[2] = second.Z() - first.Z();
  assert(!Empty());
}

const double& CVector::X() const
{
  assert(!m_empty);
  return m_coor[0];
}

const double& CVector::Y() const
{
  assert(!m_empty);
  return m_coor[1];
}

const double& CVector::Z() const
{
  assert(!m_empty);
  return m_coor[2];
}

void CVector::X(const double &dX)
{
  if(Empty()) {
    m_coor[1] = 0.0;
    m_coor[2] = 0.0;
    m_empty = false;
  }

  m_coor[0] = dX;
}


void CVector::Y(const double &dY)
{
  if(Empty()) {
    m_coor[0] = 0.0;
    m_coor[2] = 0.0;
    m_empty = false;
  }
  m_coor[1] = dY;
}


void CVector::Z(const double &dZ)
{
  if(Empty()) {
    m_coor[0] = 0.0;
    m_coor[1] = 0.0;
    m_empty = false;
  }
  m_coor[2] = dZ;
}

void CVector::AssertValid() const
{
  assert(!m_empty);
}


CVector& CVector::operator=(const CVector& rhs)
{
  m_empty = rhs.Empty();

  if(!rhs.Empty()) {
    m_coor[0] = rhs.X();
    m_coor[1] = rhs.Y();
    m_coor[2] = rhs.Z();
  }

  return *this;
}

double CVector::operator[]( int i ) const
{
  assert( !m_empty );
  assert( i >= 0 && i < 3 );
  return m_coor[i];
}
CPoint CVector::Min() const
{
  return *this;
}


CPoint CVector::Max() const
{
  return *this;
}


bool CVector::Empty() const
{
  return m_empty;
}

} // namespace geo

