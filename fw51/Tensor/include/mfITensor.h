/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#if !defined(AFX_ITENSOR_H__INCLUDED_)
#define AFX_ITENSOR_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Matrix.h"

#include "TensorExports.h"

namespace geo {
class IMatrix;
class IVector;
class CVector;
}; // namespace geo
class CTensor;

class TENSOR_EXPORT ITensor {
public:
  ITensor();
  virtual ~ITensor() = 0;

  // Takes a 1 by 6 stress matrix (= vector) and creates a 3
  // by 3 stress tensor from the vector.
  void CreateStressTensorFromVector(const geo::IMatrix &stressvector);

  // Takes a 1 by 6 strain matrix (= vector) and creates a 3
  // by 3 strain tensor from the vector.
  void CreateStrainTensorFromVector(const geo::IMatrix &strainvector);

  // Returns a rotated tensor created from the object tensor.
  // Rotated around the passed vector bij the passed angle
  // in degrees.
  CTensor RotateTensorDeg(const geo::IVector &RotationAxis, const double &dangledeg) const;

  // Returns a rotated tensor created from the object tensor.
  // Rotated around the passed vector bij the passed angle
  // in radians.
  CTensor RotateTensorRad(const geo::IVector &RotationAxis, const double &danglerad) const;

  CTensor RotateTensor(const geo::IMatrix &rotation) const;

  // Rotates the tensor to a face
  geo::CVector RotateToFace(const geo::CVector &vcNormal, const geo::CVector &vcTangent) const;

  // Returns a matrix with the principal values on the main
  // diagonal.
  geo::CMatrix GetPrincipalStr() const;

  // Returns a matrix with the principal directions as
  // column vectors.
  geo::CMatrix GetPrincipalDir() const;

  // Returns a matrix pair of eigenvalues (first) and eigenvectors (second)
  geo::CMatrixPair GetEigenPairs() const;

  // Returns a 3 x 3 tensor matrix
  geo::CMatrix Matrix() const;

  // Interface to elements of the tensor...
  virtual const double &XX() const = 0;
  virtual const double &YY() const = 0;
  virtual const double &ZZ() const = 0;
  virtual const double &XY() const = 0;
  virtual const double &YZ() const = 0;
  virtual const double &XZ() const = 0;
  virtual bool Empty() const = 0;
  virtual void Invalidate() = 0;

  // The and only set function
  virtual void Set(const double &xx, const double &yy, const double &zz, const double &xy, const double &yz,
                   const double &xz) = 0;

  virtual void XX(const double &val) = 0;
  virtual void YY(const double &val) = 0;
  virtual void ZZ(const double &val) = 0;
  virtual void XY(const double &val) = 0;
  virtual void YZ(const double &val) = 0;
  virtual void XZ(const double &val) = 0;

  CTensor operator*(const ITensor &rhs) const;
  geo::CVector operator*(const geo::IVector &rhs) const;
  CTensor operator*(const double &rhs) const;
  ITensor &operator*=(const double &rhs);
  CTensor operator-(const ITensor &rhs) const;
  CTensor operator+(const ITensor &rhs) const;
  CTensor operator+=(const ITensor &rhs);
  CTensor operator/(const double &rhs) const;
  CTensor operator/=(const double &rhs);

  /// Returns Frobenius norm
  double Norm() const;

  ITensor &operator=(const ITensor &rhs);

  bool operator==(const ITensor &rhs) const;
};

#endif // !defined(AFX_ITENSOR_H__INCLUDED_)
