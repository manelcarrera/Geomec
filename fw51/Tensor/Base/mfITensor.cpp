 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// ITensor.cpp: implementation of the ITensor class.
//
//////////////////////////////////////////////////////////////////////

#include <cassert>
#include <cmath>
#include "dimple.h"
#include "IObject.h"
#include "ICoordinate.h"
#include "IVector.h"
#include "Vector.h"
#include "IMatrix.h"
#include "mfITensor.h"
#include "mfTensor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ITensor::ITensor()
{

}

ITensor::~ITensor()
{
}

CTensor ITensor::operator *(const ITensor& rhs) const
{
  assert(!Empty());
  assert(!rhs.Empty());

  return CTensor(Matrix() * rhs.Matrix());
}

geo::CVector ITensor::operator *(const geo::IVector &rhs) const
{
  geo::CMatrix matvec(3, 1);
  matvec.Value(0, 0, rhs.X());
  matvec.Value(1, 0, rhs.Y());
  matvec.Value(2, 0, rhs.Z());

  geo::CMatrix matres = Matrix() * matvec;
  return geo::CVector(matres.Value(0, 0), matres.Value(1, 0), matres.Value(2, 0));
}

CTensor ITensor::operator *(const double &rhs) const
{
  assert(!Empty());
  return CTensor( XX() * rhs,
          YY() * rhs,
          ZZ() * rhs,
          XY() * rhs,
          YZ() * rhs,
          XZ() * rhs);
}

ITensor& ITensor::operator *=(const double &rhs)
{
  assert(!Empty());
  Set( XX() * rhs,
     YY() * rhs,
     ZZ() * rhs,
     XY() * rhs,
     YZ() * rhs,
     XZ() * rhs);

  return *this;
}

CTensor ITensor::operator-(const ITensor& rhs) const
{
  assert(!Empty());
  assert(!rhs.Empty());

  return CTensor( XX() - rhs.XX(),
          YY() - rhs.YY(),
          ZZ() - rhs.ZZ(),
          XY() - rhs.XY(),
          YZ() - rhs.YZ(),
          XZ() - rhs.XZ());
}

CTensor ITensor::operator+(const ITensor& rhs) const
{
  assert(!Empty());
  assert(!rhs.Empty());

  return CTensor( XX() + rhs.XX(),
          YY() + rhs.YY(),
          ZZ() + rhs.ZZ(),
          XY() + rhs.XY(),
          YZ() + rhs.YZ(),
          XZ() + rhs.XZ());
}

CTensor ITensor::operator+=(const ITensor& rhs)
{
  assert(!Empty());
  assert(!rhs.Empty());

  Set( XX() + rhs.XX(),
       YY() + rhs.YY(),
       ZZ() + rhs.ZZ(),
       XY() + rhs.XY(),
       YZ() + rhs.YZ(),
       XZ() + rhs.XZ());
  return *this;
}

CTensor ITensor::operator/(const double &rhs) const
{
  assert(!Empty());
  return CTensor( XX() / rhs,
                  YY() / rhs,
                  ZZ() / rhs,
                  XY() / rhs,
                  YZ() / rhs,
                  XZ() / rhs);
}

CTensor ITensor::operator/=(const double &rhs)
{
  assert(!Empty());
  Set( XX() / rhs,
       YY() / rhs,
       ZZ() / rhs,
       XY() / rhs,
       YZ() / rhs,
       XZ() / rhs);
  return *this;
}

// Frobenius norm, but should be equal to the square root of the sum of the squared eigenvalues because of the symmetry.
double ITensor::Norm() const
{
  double square = XX() * XX() + YY() * YY() + ZZ() * ZZ() + 2 * (XY() * XY() + YZ() * YZ() + XZ() * XZ());
  double root = std::sqrt(square);

  return root;
}

ITensor& ITensor::operator=(const ITensor& rhs)
{
  Set(rhs.XX(), rhs.YY(), rhs.ZZ(), rhs.XY(), rhs.YZ(), rhs.XZ());
  return *this;
}

bool ITensor::operator==(const ITensor &rhs) const
{
  if(Empty() != rhs.Empty()) return false;
  if(Empty()) return true; // both empty

  return (fabs(XX() - rhs.XX()) < EPS &&
      fabs(YY() - rhs.YY()) < EPS &&
      fabs(ZZ() - rhs.ZZ()) < EPS &&
      fabs(XY() - rhs.XY()) < EPS &&
      fabs(YZ() - rhs.YZ()) < EPS &&
      fabs(XZ() - rhs.XZ()) < EPS);
}

CTensor ITensor::RotateTensorRad(const geo::IVector &RotationAxis, const double& dAngle) const
{
  assert(!Empty());

  geo::CMatrix rot_mat;
  rot_mat.CreateRotationMatrixRad(RotationAxis, dAngle);

  return CTensor(rot_mat.GetTransposed() * (Matrix() * rot_mat));
}

CTensor ITensor::RotateTensorDeg(const geo::IVector &RotationAxis, const double& dAngle) const
{
  assert(!Empty());

  geo::CMatrix rot_mat;
  rot_mat.CreateRotationMatrixDeg(RotationAxis, dAngle);

  return CTensor(rot_mat.GetTransposed() * (Matrix() * rot_mat));
}

CTensor ITensor::RotateTensor(const geo::IMatrix &rotation) const
{
  assert(rotation.ColumnSize() == 3 && rotation.ColumnSize() == rotation.RowSize());
  return CTensor(rotation.GetTransposed() * (Matrix() * rotation));
}


geo::CVector ITensor::RotateToFace(const geo::CVector &vcNormal, const geo::CVector &vcTangent) const
{
  geo::CVector norm = vcNormal.UnitVector();
  geo::CVector tang1 = vcTangent.UnitVector();
  geo::CVector tang2 = norm.CrossProduct(tang1);
  tang2 = tang2.UnitVector();

  geo::CMatrix rotation(3, 3);
  rotation.Value(0, 0, norm.X()); rotation.Value(0, 1, tang1.X()); rotation.Value(0, 2, tang2.X());
  rotation.Value(1, 0, norm.Y()); rotation.Value(1, 1, tang1.Y()); rotation.Value(1, 2, tang2.Y());
  rotation.Value(2, 0, norm.Z()); rotation.Value(2, 1, tang1.Z()); rotation.Value(2, 2, tang2.Z());

  return rotation.GetTransposed() * (Matrix() * norm);
}


geo::CMatrix ITensor::Matrix() const
{
  assert(!Empty());

  geo::CMatrix ret(3, 3);
  ret.Value(0, 0, XX());
  ret.Value(1, 0, XY());
  ret.Value(2, 0, XZ());
  ret.Value(0, 1, XY());
  ret.Value(1, 1, YY());
  ret.Value(2, 1, YZ());
  ret.Value(0, 2, XZ());
  ret.Value(1, 2, YZ());
  ret.Value(2, 2, ZZ());
  return ret;
}

geo::CMatrix ITensor::GetPrincipalStr() const //returns a tensor with the principal values on the main diagonal.
{
  assert(!Empty());

  return Matrix().EigenValues();
}

//##ModelId=3BC6995B02E6
geo::CMatrix ITensor::GetPrincipalDir() const//returns a tensor with the principal direction in the columns.
{
  assert(!Empty());

  // NOTA BENE: The resulting three vectors do not necessarily span a right handed system!!
  return Matrix().EigenVectors(false);
}

geo::CMatrixPair ITensor::GetEigenPairs() const
{
  return Matrix().GetEigenPairs(false);
}


  //Takes a 1 by 6 strain matrix (= vector) and creates a 3 
  //by 3 strain tensor from the vector.
void ITensor::CreateStressTensorFromVector(const geo::IMatrix &stressvector)
{

  assert(stressvector.ColumnSize() == 6 && stressvector.RowSize() == 1);

  XX(stressvector.Value(0,0));
  YY(stressvector.Value(0,1));
  ZZ(stressvector.Value(0,2));
  XY(stressvector.Value(0,3));
  YZ(stressvector.Value(0,4));
  XZ(stressvector.Value(0,5));
}


void ITensor::CreateStrainTensorFromVector(const geo::IMatrix &strainvector)
{

  assert(strainvector.ColumnSize() == 6 && strainvector.RowSize() == 1);

  XX(strainvector.Value(0,0));
  YY(strainvector.Value(0,1));
  ZZ(strainvector.Value(0,2));

  XY(0.5 * strainvector.Value(0,3));
  YZ(0.5 * strainvector.Value(0,4));
  XZ(0.5 * strainvector.Value(0,5));
}

