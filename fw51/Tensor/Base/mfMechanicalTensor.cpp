 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// MechanicalTensor.cpp: implementation of the CMechanicalTensor class.
//
//////////////////////////////////////////////////////////////////////
#include "Matrix.h"
#include "mfMechanicalTensor.h"
#include "Vector.h"
#include <math.h>
#include "dimple.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMechanicalTensor::CMechanicalTensor()
{

}

CMechanicalTensor::~CMechanicalTensor()
{

}

CMechanicalTensor::CMechanicalTensor(const ITensor &rhs)
: CTensor(rhs)
{
}

CMechanicalTensor::CMechanicalTensor(const double &xx, const double &yy, const double &zz,
               const double &xy, const double &yz, const double &xz)
:CTensor(xx, yy, zz, xy, yz, xz)
{
}

double CMechanicalTensor::InvarI1() const
{
  return XX() + YY() + ZZ();
}

double CMechanicalTensor::InvarI2() const
{
  double xx = XX();
  double yy = YY();
  double zz = ZZ();
  double xy = XY();
  double yz = YZ();
  double xz = XZ();

  return xx * yy + yy * zz + zz * xx - xy * xy - yz * yz - xz * xz; 	
}

double CMechanicalTensor::InvarI3() const
{
  double xx = XX();
  double yy = YY();
  double zz = ZZ();
  double xy = XY();
  double yz = YZ();
  double xz = XZ();

  return xx * yy * zz + 2.0 * xy * yz * xz - xx * yz * yz - yy * xz * xz - zz * xy * xy;
}
/*
double CMechanicalTensor::InvarVonMis() const
{
  
  double I1 = XX() + YY() + ZZ();

  double I2acc = InvarI2() - ((I1 * I1) / 3.0);

  if(fabs(I2acc) < EPS)
    I2acc = 0.0;

  assert(I2acc <= 0.0);
  double VMsqrd = -3 * I2acc;
  return sqrt(VMsqrd);
}
*/
double CMechanicalTensor::PrincIntermDepth() const
{
  geo::CMatrix Values = GetPrincipalStr();
  geo::CMatrix Directions = GetPrincipalDir();

  return Values.Value(1, 1) * Directions.Value(2, 1);
}

double CMechanicalTensor::PrincIntermEast() const
{
  geo::CMatrix Values = GetPrincipalStr();
  geo::CMatrix Directions = GetPrincipalDir();

  return Values.Value(1, 1) * Directions.Value(1, 1);
}

double CMechanicalTensor::PrincIntermLength() const
{
  geo::CMatrix Values = GetPrincipalStr();
  return Values.Value(1, 1);
}

double CMechanicalTensor::PrincIntermNorth() const
{
  geo::CMatrix Values = GetPrincipalStr();
  geo::CMatrix Directions = GetPrincipalDir();

  return Values.Value(1, 1) * Directions.Value(0, 1);
}

double CMechanicalTensor::PrincMaxDepth() const
{
  geo::CMatrix Values = GetPrincipalStr();
  geo::CMatrix Directions = GetPrincipalDir();

  return Values.Value(2, 2) * Directions.Value(2, 2);
}

double CMechanicalTensor::PrincMaxEast() const
{
  geo::CMatrix Values = GetPrincipalStr();
  geo::CMatrix Directions = GetPrincipalDir();

  return Values.Value(2, 2) * Directions.Value(1, 2);
}

double CMechanicalTensor::PrincMaxLength() const
{
  geo::CMatrix Values = GetPrincipalStr();
  return Values.Value(2, 2);
}

double CMechanicalTensor::PrincMaxNorth() const
{
  geo::CMatrix Values = GetPrincipalStr();
  geo::CMatrix Directions = GetPrincipalDir();

  return Values.Value(2, 2) * Directions.Value(0, 2);
}

double CMechanicalTensor::PrincMinDepth() const
{
  geo::CMatrix Values = GetPrincipalStr();
  geo::CMatrix Directions = GetPrincipalDir();

  return Values.Value(0, 0) * Directions.Value(2, 0);
}

double CMechanicalTensor::PrincMinEast() const
{
  geo::CMatrix Values = GetPrincipalStr();
  geo::CMatrix Directions = GetPrincipalDir();

  return Values.Value(0, 0) * Directions.Value(1, 0);
}

double CMechanicalTensor::PrincMinLength() const
{
  geo::CMatrix Values = GetPrincipalStr();
  return Values.Value(0, 0);
}

double CMechanicalTensor::PrincMinNorth() const
{
  geo::CMatrix Values = GetPrincipalStr();
  geo::CMatrix Directions = GetPrincipalDir();

  return Values.Value(0, 0) * Directions.Value(0, 0);
}

double CMechanicalTensor::AzimuthMaxHorizontal() const
{
  geo::CMatrix two_d_mat(2, 2);

  two_d_mat.Value(0, 0, XX()); two_d_mat.Value(0, 1, XY());
  two_d_mat.Value(1, 0, XY()); two_d_mat.Value(1, 1, YY());

  std::pair<geo::CMatrix, geo::CMatrix> eigen_pr = two_d_mat.GetEigenPairs();
  geo::CMatrix eigen_vectors = eigen_pr.second;
  geo::CMatrix eigen_values = eigen_pr.first;

  geo::CVector north(1, 0);

  if(fabs(eigen_values.Value(0, 0) - eigen_values.Value(1, 1)) < EPS)
  {
    geo::CVector vc1(eigen_vectors.Value(0, 1), eigen_vectors.Value(1, 1));
    geo::CVector vc2(eigen_vectors.Value(0, 0), eigen_vectors.Value(1, 0));
    double ang1 = north.AngleDeg(vc1);
    double ang2 = north.AngleDeg(vc2);

    if(ang1 < ang2)
      return ang1;
    else
      return ang2;
  }

  geo::CVector vc(eigen_vectors.Value(0, 1), eigen_vectors.Value(1, 1));
  vc = vc.UnitVector();
  

  return north.AngleDeg(vc);
}

double CMechanicalTensor::MaxHorValue() const
{
  geo::CMatrix two_d_mat(2, 2);

  two_d_mat.Value(0, 0, XX()); two_d_mat.Value(0, 1, XY());
  two_d_mat.Value(1, 0, XY()); two_d_mat.Value(1, 1, YY());

  std::pair<geo::CMatrix, geo::CMatrix> eigen_pr = two_d_mat.GetEigenPairs();
  geo::CMatrix eigen_values = eigen_pr.first;

  return eigen_values.Value(1, 1);
}

double CMechanicalTensor::MinHorValue() const
{
  geo::CMatrix two_d_mat(2, 2);

  two_d_mat.Value(0, 0, XX()); two_d_mat.Value(0, 1, XY());
  two_d_mat.Value(1, 0, XY()); two_d_mat.Value(1, 1, YY());

  std::pair<geo::CMatrix, geo::CMatrix> eigen_pr = two_d_mat.GetEigenPairs();
  geo::CMatrix eigen_values = eigen_pr.first;

  return eigen_values.Value(0, 0);
}
