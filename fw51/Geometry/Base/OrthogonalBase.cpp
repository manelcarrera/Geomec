 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// OrthogonalBase.cpp: implementation of the COrthogonalBase class.
//
//////////////////////////////////////////////////////////////////////

#include "dimple.h"
#include <cassert>
#include <cmath>
#include "Plane.h"
#include "Vector.h"
#include "OrthogonalBase.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace geo {

COrthogonalBase::COrthogonalBase(bool bFull3D)
{
  if(bFull3D)
  {
    VERIFY(AddVector(CVector::Xaxis));
    VERIFY(AddVector(CVector::Yaxis));
    VERIFY(AddVector(CVector::Zaxis));
  }
}

COrthogonalBase::COrthogonalBase(const IVector &vector)
{
  VERIFY(AddVector(vector));
}

COrthogonalBase::COrthogonalBase(const IVector &vec1, const IVector &vec2)
{
  VERIFY(AddVector(vec1));
  VERIFY(AddVector(vec2));
}

COrthogonalBase::~COrthogonalBase()
{
  for(size_t i = 0; i < m_vcDirections.size(); i++) delete m_vcDirections[i];
}

// returns whether the vector changed the base (whether it was independent)
bool COrthogonalBase::AddVector(const IVector &vector)
{
  if(Dependent(vector)) return false;

  CVector cross;

  switch(Size())
  {
  case 0:
    m_vcDirections.push_back(new CVector(vector.UnitVector()));
    return true;
  case 1:
    cross = Direction(0).CrossProduct(vector);
    assert(cross.Length() > EPS);
    m_vcDirections.push_back(new CVector(cross.CrossProduct(Direction(0)).UnitVector()));
    return true;
  case 2:
    m_vcDirections.push_back(new CVector(Direction(0).CrossProduct(Direction(1)).UnitVector()));
    return true;
  default:
    assert(false);
  }

  return false;
}

// returns whether the given vector is dependent on the base
bool COrthogonalBase::Dependent(const IVector &vector) const
{
  switch(Size())
  {
  case 0:
    return false;
  case 1:
    return (Direction(0).IsParallel(vector));
  case 2:
    {
      CVector cross = Direction(0).CrossProduct(Direction(1)).UnitVector();
      return (fabs(cross.DotProduct(vector.UnitVector())) < EPS);
    }
  case 3:
    return true;
  default:
    assert(false);
  }

  return false;
}

// the number of vectors in the base (0, 1, 2 or 3)
size_t COrthogonalBase::Size() const
{
  return m_vcDirections.size();
}

// returned vector will be length 1 (unit vector)
const IVector &COrthogonalBase::Direction(size_t nIndex) const
{
  assert(nIndex < m_vcDirections.size());
  return *m_vcDirections[nIndex];
}

// returns the projection of the given vector in the base
// returns empty vector if base is empty
// returns the vector if base is fully 3-dimensional
CVector COrthogonalBase::Projection(const IVector &vector) const
{
  switch(Size())
  {
  case 0:
    return CVector();
  case 1:
    {
      const IVector &dir = Direction(0);
      assert(fabs(dir.Length() - 1) < EPS); // should be unit vector
      return dir * vector.DotProduct(dir);
    }
  case 2:
    {
      CPlane plane(CPoint(0, 0, 0), Direction(0), Direction(1));
      return vector.InPlaneComponent(plane.Normal());
    }
  case 3:
    return CVector(vector);
  }

  assert(false);
  return CVector();
}

// align the base with the given vector
// the vector should be dependent, so the function is const
// (actually the base is not changed, it's just rotated)
void COrthogonalBase::AlignWith(const IVector &vector) const
{
  assert(vector.Length() > EPS);
  assert(Dependent(vector));
  int sz = Size();

  switch(sz)
  {
  case 1:
    *m_vcDirections[0] = vector.UnitVector();
    break;
  case 2:
    {
      CPlane plane(CPoint(0, 0, 0), Direction(0), Direction(1));
      *m_vcDirections[0] = vector.UnitVector();
      *m_vcDirections[1] = vector.CrossProduct(plane.Normal()).UnitVector();
    }
    break;
  case 3:
    {
      *m_vcDirections[0] = vector.UnitVector();
      // get a normal to the vector (the second vector in the base)
      if(Direction(0).IsParallel(geo::CVector::Xaxis))
        *m_vcDirections[1] = Direction(0).CrossProduct(CVector::Yaxis).UnitVector();
      else if(Direction(0).IsParallel(geo::CVector::Yaxis))
        *m_vcDirections[1] = Direction(0).CrossProduct(CVector::Xaxis).UnitVector();
      else
      {
        // use the one giving the largest cross product
        CVector vec1 = Direction(0).CrossProduct(CVector::Xaxis);
        CVector vec2 = Direction(0).CrossProduct(CVector::Yaxis);
        if(vec1.SquareLength() > vec2.SquareLength()) *m_vcDirections[1] = vec1.UnitVector();
        else                                          *m_vcDirections[1] = vec2.UnitVector();
      }

      // and the third is normal to both again
      *m_vcDirections[2] = Direction(0).CrossProduct(Direction(1)).UnitVector();
    }
    break;
  default:
    assert(false);
  }
}

} // namespace geo
