/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// IMatrix.h: interface for the IMatrix class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMATRIX_H__20BD69E7_0CF0_4A06_BE28_DD3F6A84525A__INCLUDED_)
#define AFX_IMATRIX_H__20BD69E7_0CF0_4A06_BE28_DD3F6A84525A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "dimple.h"

#include "GeometryExports.h"

namespace geo {
class CVector;
class CMatrix;
class CMatrixPair;
class IVector;
class GEOMETRY_EXPORT IMatrix {
public:
  IMatrix();
  virtual ~IMatrix();

  // Data access
  virtual int ColumnSize() const = 0;
  virtual int RowSize() const = 0;

  // Changes the size (nr. of rows and columns) of the
  // matrix. If extra rows and or columns are added, the new
  // elements are zero.
  virtual void ReSize(int rows, int columns) = 0;

  // Set and get the values
  virtual const double &Value(int nRow, int nColumn) const = 0;
  virtual void Value(int nRow, int nColumn, const double &dValue) const = 0;
  bool Empty() const;

  // Returns a new matrix that is the transposed of the object matrix.
  CMatrix GetTransposed() const;

  // Only usable for nonsingular square matrices. In order
  // to check if the matrix passed is nonsingular, first the
  // determinant is calculated, if the determinant is zero,
  // there is no inverse. Returns a new matrix that is the
  // inverse of the object matrix.
  CMatrix GetInverse() const;

  // Only for square matrices. Returns the determinant of
  // the matrix.
  double GetDeterminant() const;

  CMatrix StrainRotation() const;
  CMatrix StressRotation() const;

  // makes the current matrix into a matrix filled with only zeros! (and I don't mean the WWII Japanese fighter planes)
  void NullMatrix();

  // Creates an Identity matrix of the order passed.
  void CreateIdentityMatrix(int nSize);

  // Turns the object matrix into a 3 by 3 rotation matrix
  // around the passed vector by the passed angle in
  // radians. Returns a 6 by 6 stress rotation matrix
  // created with the rotation matrix.
  void CreateRotationMatrixRad(const IVector &rot_vector, const double &dAngle);
  void CreateRotationMatrixDeg(const IVector &rot_vector, const double &dAngle);

  // Takes any 6 by 6 matrix. Make sure that the matrix
  // passed is a materaial stiffness matrix. Returns the
  // rotated material stiffness matrix, around the passed
  // vector by the passed angle in degrees.
  CMatrix RotateMatStiffnessDeg(const IVector &rot_vector, const double &dAngle);
  CMatrix RotateMatStiffnessRad(const IVector &rot_vector, const double &dAngle);

  // Turns the object matrix into a 3 by 3 rotation matrix
  // around the passed vector by the passed angle in
  // degrees. Returns a 6 by 6 stress rotation matrix
  // created with the rotation matrix.
  //	CMatrix GetStressRotMatdeg(const IVector &rotvector, double dangledeg);

  // ##ModelId=3BC6995D0094
  //	CMatrix GetStressRotMatrad(const IVector &rotvector, double danglerad);
  // Turns the object matrix into a 3 by 3 rotation matrix
  // around the passed vector by the passed angle in
  // degrees. Returns a 6 by 6 strain rotation matrix
  // created with the rotation matrix.
  // ##ModelId=3BC6995D0097
  //	CMatrix GetStrainRotMatdeg(const IVector &rotvector, double dangledeg);
  // Turns the object matrix into a 3 by 3 rotation matrix
  // around the passed vector by the passed angle in
  // radians. Returns a 6 by 6 strain rotation matrix
  // created with the rotation matrix.
  // ##ModelId=3BC6995D00A1
  //	CMatrix GetStrainRotMatrad(const IVector &rotvector, double danglerad);

  // ##ModelId=3BC6995D00A4

  // Takes any 6 by 6 matrix. Make sure that the matrix
  // passed is a materaial stiffness matrix. Returns the
  // rotated material stiffness matrix, around the passed
  // vector by the passed angle in radians.
  // ##ModelId=3BC6995D00A7

  // For a real symmetric matrix a pair of matrices is
  // returned. The first matrix is a matrix containing the
  // eigenvalues on the main diagonal, the second matrix is
  // a matrix containing the accompanying eigenvectors (as
  // columns). The eigenvectors are unit vectors. The
  // eigenvalues are sorted (smallest left, largest right)
  // and the accompanying eigenvectora are also put in the
  // same order. If the matrices in the pair are 3 by 3 a
  // check is carried out if the eigenvectors still span a
  // right hand side coordinate system. If not the third
  // vector has its sign changed. Uses Jacobi iteration.
  CMatrixPair GetEigenPairs(bool bCreateRightHandSystem = true) const;

  // Returns the matrix with eigenvalues that is creatd in
  //  GetEigenPairs().
  CMatrix EigenValues() const;

  // Return the matrix with eigenvectors created in
  // GetEigenPairs().
  CMatrix EigenVectors(bool bCreateRightHandSystem = false) const;

  // Returns an Upper matrix.
  CMatrix UpperMatrix() const;

  CMatrix operator*(const IMatrix &rhs) const;
  CMatrix operator*(const double &rhs) const;
  CVector operator*(const IVector &rhs) const;
  CMatrix operator+(const IMatrix &rhs) const;
  CMatrix operator-(const IMatrix &rhs) const;

  bool operator==(const IMatrix &rhs) const;
  bool operator!=(const IMatrix &rhs) const;

  IMatrix &operator=(const IMatrix &rhs);
};

} // namespace geo

#endif // !defined(AFX_IMATRIX_H__20BD69E7_0CF0_4A06_BE28_DD3F6A84525A__INCLUDED_)
