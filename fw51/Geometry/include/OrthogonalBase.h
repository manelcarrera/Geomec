 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// OrthogonalBase.h: interface for the COrthogonalBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ORTHOGONALBASE_H__307E7292_DED0_4C6D_AD5C_45AD3E2909D8__INCLUDED_)
#define AFX_ORTHOGONALBASE_H__307E7292_DED0_4C6D_AD5C_45AD3E2909D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

#include "GeometryExports.h"

namespace geo {
class CVector;
class IVector;
class GEOMETRY_EXPORT  COrthogonalBase  
{
  mutable std::vector<CVector *> m_vcDirections;

public:
  // true creates full 3D base
  // false creates empty base
  COrthogonalBase(bool bFull3D = false);

  // creates 1D base
  COrthogonalBase(const IVector &vector);

  // creates 2D base
  COrthogonalBase(const IVector &vec1, const IVector &vec2);

  virtual ~COrthogonalBase();

  // returns whether the vector changed the base (whether it was independent)
  bool AddVector(const IVector &vector);

  // returns whether the given vector is dependent on the base
  bool Dependent(const IVector &vector) const;

  // the number of vectors in the base (0, 1, 2 or 3)
  size_t Size() const;

  // returned vector will be length 1 (unit vector)
  const IVector &Direction(size_t nIndex) const;

  // returns the projection of the given vector in the base
  // returns empty vector if base is empty
  // returns the vector if base is fully 3-dimensional
  CVector Projection(const IVector &vector) const;

  // align the base with the given vector
  // the vector should be dependent, so the function is const
  // (actually the base is not changed, it's just rotated)
  void AlignWith(const IVector &vector) const;
};

}

#endif // !defined(AFX_ORTHOGONALBASE_H__307E7292_DED0_4C6D_AD5C_45AD3E2909D8__INCLUDED_)
