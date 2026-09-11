 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "dimple.h"
#include "Vector.h"
#include "Point.h"
#include "Matrix.h"
#include <cmath>

namespace geo {


bool IVector::operator==(const IVector &rhs) const
{
  if(Empty() != rhs.Empty()) // one is empty, the other isn't
  return false;

  if(Empty()) // both empty
  return true;

  if ( fabs( rhs.X() - X() ) > compareTolerance() ) return false;
  if ( fabs( rhs.Y() - Y() ) > compareTolerance() ) return false;
  if ( fabs( rhs.Z() - Z() ) > compareTolerance() ) return false;
  return true;
}

/*!
  Special comparison against null vector.
  Use instead of operator==(CVector::NullVector).
*/
bool IVector::isNullVector() const
{
  if ( fabs( X() ) > DBL_EPSILON ) return false;
  if ( fabs( Y() ) > DBL_EPSILON ) return false;
  if ( fabs( Z() ) > DBL_EPSILON ) return false;
  return true;
}

IVector::IVector()
{
}

CVector IVector::operator+(const IVector &vector) const
{
  assert(vector.Empty() == false);
  assert(Empty() == false);

  return CVector( X() + vector.X(),
          Y() + vector.Y(),
          Z() + vector.Z());
}

CVector IVector::operator-(const IVector &vector) const
{
  assert(vector.Empty() == false);
  assert(Empty() == false);

  return CVector( X() - vector.X(),
          Y() - vector.Y(),
          Z() - vector.Z());
}

CVector IVector::operator/(const double &val) const
{
  assert(!(val == 0));
  return CVector(	X() / val,
            	Y() / val,
          Z() / val);
}


CVector IVector::operator*(const double &d) const
{
  return CVector( X() * d,
          Y() * d,
          Z() * d);
}

CVector IVector::operator-() const
{
  return Flipped();
}

/*!
  Returns the 2-norm (euclidean vector norm) of the vector.
*/
double IVector::Length() const
{
  return sqrt( X()*X() + Y()*Y() + Z()*Z() );
}

CVector IVector::CrossProduct(const IVector &vector) const
{
  return CVector(Y() * vector.Z() - Z() * vector.Y(),
           Z() * vector.X() - X() * vector.Z(),
           X() * vector.Y() - Y() * vector.X());
}

double IVector::DotProduct(const IVector &vector) const
{
  return X() * vector.X() + Y() * vector.Y()+Z() * vector.Z();
}

//the smallest absolute angle between two vectors
double IVector::AngleRad(const IVector &vector) const
{
  assert(vector.Length() != 0.0);
  assert(Length() != 0.0);

  double cos_a = DotProduct(vector) / (vector.Length() * Length());
  
  if(cos_a > 1)
  {
    return 0;
  }

  if(cos_a < -1)
  {
    return PI;
  }

  return acos(cos_a);

}


double IVector::AngleDeg(const IVector &vector) const
{
  return AngleRad(vector) * RAD_TO_DEG;
}


CVector IVector::UnitVector() const
{
  double l = Length();
  assert(l != 0.0);
  return CVector(X() / l , Y() / l , Z() / l);
}

bool IVector::IsParallel(const IVector &v) const
{
  double angle = AngleRad( v );
  if ( fabs( angle      ) < EPS ) return true;
  if ( fabs( angle - PI ) < EPS ) return true;
  return false;
}

/*!
  Returns true if the normalized DotProduct() is almost zero. The 2-norm
  Length() is taken into account in order to get a reasonable zero-check.
*/
bool IVector::IsPerpendicular(const IVector &v) const
{
  double length1  = Length();
  double length2  = v.Length();
  double dDotProd = DotProduct(v);
  return fabs(dDotProd) < (EPS * length1 * length2 );
}

bool IVector::IsOppositeDirection(const IVector &v) const
{
  if(fabs(this->AngleRad(v) - PI) < EPS)
    return true;

  return false;
}

void IVector::Flip()
{
  X(-X());
  Y(-Y());
  Z(-Z());
}

CVector IVector::Flipped() const
{
  CVector flipped(-X(), -Y(), -Z());

  return flipped;
}

// After creating a 3 by 3 rotation matrix, size is set to 4 by 4
// so that also transformation information can be stored in the matrix.
// Do not use this function if you want to create a 3 by 3 rotation matrix!!
void IVector::GetRotationMatrix(const double &AngleDeg, IMatrix& matrix ) const
{
  CMatrix m;
  matrix.ReSize(4, 4);
  m.CreateRotationMatrixDeg(*this, AngleDeg);
  for(int i = 0; i < 3; i++)
    for(int j = 0; j < 3; j++)
      matrix.Value(i, j, m.Value(i, j));

  matrix.Value(3, 3, 1.0);
}

CVector IVector::GetNormal() const
{ 

  if(IsParallel(CVector::Zaxis))
  {
    return CVector::Xaxis;
  }

  return CrossProduct(CVector::Zaxis);
}

double IVector::SquareLength()
{
  return X() * X() + Y() * Y() + Z() * Z();
}

IVector& IVector::operator+=(const IVector& rhs)
{
  // Assignment ...
  X(X() + rhs.X());
  Y(Y() + rhs.Y());
  Z(Z() + rhs.Z());

  return *this;
}

IVector& IVector::operator-=(const IVector& rhs)
{
  // Assignment ...
  X(X() - rhs.X());
  Y(Y() - rhs.Y());
  Z(Z() - rhs.Z());

  return *this;
}

// returns the in-plane component of this vector, where the plane is
// represented by the given normal. The normal must be a unit vector
CVector IVector::InPlaneComponent(const IVector &normal) const
{
  assert(fabs(normal.Length() - 1) < EPS); // must be a unit vector

  return (*this - normal * DotProduct(normal));
}

}
