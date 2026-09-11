 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#if !defined(_EMBEDDED_TRIANGLE_H__INCLUDED_)
#define _EMBEDDED_TRIANGLE_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Triangle.h"

#include "GeometryExports.h"

namespace geo {

/*!
  \class CEmbeddedTriangle
  \brief This triangle is contained by another element.

  The class is used for crosssection, reinforcments etc. When result a projected on a crosssection the
  IValueSet is optimized for the CEmbeddedTriangle. CEmbedded triangles can also be inserted in the CSurface.
*/

class IBody;

class GEOMETRY_EXPORT  CEmbeddedTriangle  : public CTriangle
{
  const IBody& m_parent;
public:
  // Construction ...
  CEmbeddedTriangle(const IBody& parent, const ITriangle &rhs);  // Constructs from an unknown triagle ..
  CEmbeddedTriangle(const IBody& parent, const IPoint &p1, const IPoint &p2, const IPoint &p3);
  CEmbeddedTriangle(const IBody& parent, IMesh& mesh, int p1, int p2, int p3);
  CEmbeddedTriangle(const IBody& parent, IMesh& mesh, const IPoint &p1, const IPoint &p2, const IPoint &p3, bool bUnique = false);
  CEmbeddedTriangle(const IBody& parent, const ISurface& surface, int nIndex, int p1, int p2, int p3);

  const IBody* Parent() const;
};

}


#endif // !defined(_EMBEDDED_TRIANGLE_H__INCLUDED_)
