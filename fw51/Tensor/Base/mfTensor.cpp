 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// Tensor.cpp: implementation of the CTensor class.
//
//////////////////////////////////////////////////////////////////////

#include <cassert>
#include <cmath>
#include "dimple.h"
#include "Matrix.h"
#include "Vector.h"
#include "mfTensor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//##ModelId=3BC6995B02C5
CTensor::CTensor()
: m_bEmpty(true)
{
}

CTensor::CTensor(const double& dInitVal)
: m_bEmpty(false)
{
  for(int i = 0; i < 6; i++)
    m_pData[i] = dInitVal;
}

//##ModelId=3BC6995B02C6
CTensor::CTensor(const geo::IMatrix &InitMat)
: m_bEmpty(false)
{
  assert((InitMat.ColumnSize() == 3) && (InitMat.RowSize() == 3));

  //XY == YX
  assert(fabs(InitMat.Value(0, 1)-InitMat.Value(1, 0)) < EPS );

  //XZ == ZX
  assert(fabs(InitMat.Value(0, 2)-InitMat.Value(2, 0)) < EPS );

  //YZ == ZY
  assert(fabs(InitMat.Value(1, 2)-InitMat.Value(2, 1)) < EPS );

  m_pData[0] = InitMat.Value(0, 0);
  m_pData[1] = InitMat.Value(1, 1);
  m_pData[2] = InitMat.Value(2, 2);
  m_pData[3] = InitMat.Value(1, 0);
  m_pData[4] = InitMat.Value(2, 1);
  m_pData[5] = InitMat.Value(2, 0);
}

CTensor::CTensor(const double& xx,
         const double& yy,
         const double& zz,
         const double& xy,
         const double& yz,
         const double& xz)
: m_bEmpty(false)
{
  m_pData[0] = xx;
  m_pData[1] = yy;
  m_pData[2] = zz;
  m_pData[3] = xy;
  m_pData[4] = yz;
  m_pData[5] = xz;
}


CTensor::CTensor(const double& dPrincVal1, const double& dPrincVal2, const double& dPrincVal3, const geo::IVector &xaxis, const geo::IVector &yaxis, const geo::IVector &zaxis)
: m_bEmpty(true)
{
  geo::CMatrix temp(3, 3);
  temp.Value(0, 0,  dPrincVal1);
  temp.Value(1, 1,  dPrincVal2);
  temp.Value(2, 2,  dPrincVal3);

  geo::CVector vcX = xaxis.UnitVector();
  geo::CVector vcY = yaxis.UnitVector();
  geo::CVector vcZ = zaxis.UnitVector();

  geo::CMatrix rotmat(3, 3);
  geo::CMatrix Trotmat(3, 3);
  rotmat.Value(0, 0, vcX.X());
  rotmat.Value(1, 0, vcX.Y());
  rotmat.Value(2, 0, vcX.Z());
  rotmat.Value(0, 1, vcY.X());
  rotmat.Value(1, 1, vcY.Y());
  rotmat.Value(2, 1, vcY.Z());
  rotmat.Value(0, 2, vcZ.X());
  rotmat.Value(1, 2, vcZ.Y());
  rotmat.Value(2, 2, vcZ.Z());
  Trotmat = rotmat.GetTransposed();

  geo::CMatrix rotated = rotmat * (temp * Trotmat);

  assert((fabs(rotated.Value(1, 0) - rotated.Value(0, 1)))<EPS);
  assert((fabs(rotated.Value(1, 2) - rotated.Value(2, 1)))<EPS);
  assert((fabs(rotated.Value(0, 2) - rotated.Value(2, 0)))<EPS);

  Set(rotated.Value(0, 0),
    rotated.Value(1, 1),
    rotated.Value(2, 2),
    rotated.Value(1, 0),
    rotated.Value(1, 2),
    rotated.Value(0, 2));
}
//##ModelId=3BC6995B02D3
CTensor::~CTensor()
{
}

CTensor::CTensor(const CTensor& rhs)
: ITensor(rhs), m_bEmpty(rhs.Empty())
{
  if(!rhs.Empty()) {
    for(int i = 0; i < 6; i++)
      m_pData[i] = rhs.m_pData[i];
  }
}

CTensor::CTensor(const ITensor& rhs)
: m_bEmpty(rhs.Empty())
{
  if(!rhs.Empty())
  {
    m_pData[0] = rhs.XX();
    m_pData[1] = rhs.YY();
    m_pData[2] = rhs.ZZ();
    m_pData[3] = rhs.XY();
    m_pData[4] = rhs.YZ();
    m_pData[5] = rhs.XZ();
  }
}

CTensor& CTensor::operator=(const CTensor& rhs)
{
  m_bEmpty = rhs.Empty();
  if(!rhs.Empty()) {
    for(int i = 0; i < 6; i++)
      m_pData[i] = rhs.m_pData[i];
  }

  return *this;
}

bool CTensor::Empty() const
{
  return m_bEmpty;
}

void CTensor::Invalidate()
{
  m_bEmpty = true;
}

// The and only set function
void CTensor::Set(const double& xx, const double& yy, const double& zz, const double& xy, const double& yz, const double& xz)
{
  m_bEmpty   = false;
  m_pData[0] = xx;
  m_pData[1] = yy;
  m_pData[2] = zz;
  m_pData[3] = xy;
  m_pData[4] = yz;
  m_pData[5] = xz;
}

void CTensor::XX(const double& val)
{
  // Create matrix if not existing
  m_bEmpty = false;
  m_pData[0] = val;
}

void CTensor::YY(const double& val)
{
  // Create matrix if not existing
  m_bEmpty = false;
  m_pData[1] = val;
}

void CTensor::ZZ(const double& val)
{
  // Create matrix if not existing
  m_bEmpty = false;
  m_pData[2] = val;
}

void CTensor::XY(const double& val)
{
  // Create matrix if not existing
  m_bEmpty = false;
  m_pData[3] = val;
}

void CTensor::YZ(const double& val)
{
  // Create matrix if not existing
  m_bEmpty = false;
  m_pData[4] = val;
}

void CTensor::XZ(const double& val)
{
  // Create matrix if not existing
  m_bEmpty = false;
  m_pData[5] = val;
}

const double& CTensor::XX() const
{
  assert(!Empty());
  return m_pData[0];
}

const double& CTensor::YY() const
{
  assert(!Empty());
  return m_pData[1];
}

const double& CTensor::ZZ() const
{
  assert(!Empty());
  return m_pData[2];
}

const double& CTensor::XY() const
{
  assert(!Empty());
  return m_pData[3];
}

const double& CTensor::XZ() const
{
  assert(!Empty());
  return m_pData[5];
}

const double& CTensor::YZ() const
{
  assert(!Empty());
  return m_pData[4];
}

  
