 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// EmbeddedTriangle.cpp: implementation of the triangle class.
//
//////////////////////////////////////////////////////////////////////

#include "dimple.h"
#include "EmbeddedTriangle.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace geo {

CEmbeddedTriangle::CEmbeddedTriangle(const IBody& parent, const ITriangle &rhs)
: CTriangle(rhs), m_parent(parent)
{
}

CEmbeddedTriangle::CEmbeddedTriangle(const IBody& parent, const IPoint &p1, const IPoint &p2, const IPoint &p3)
: CTriangle(p1, p2, p3), m_parent(parent)
{
}

CEmbeddedTriangle::CEmbeddedTriangle(const IBody& parent, IMesh& mesh, int p1, int p2, int p3)
: CTriangle(mesh, p1, p2, p3), m_parent(parent)
{
}

CEmbeddedTriangle::CEmbeddedTriangle(const IBody& parent, IMesh& mesh, const IPoint &p1, const IPoint &p2, const IPoint &p3, bool bUnique)
: CTriangle(mesh, p1, p2, p3, bUnique), m_parent(parent)
{
}

CEmbeddedTriangle::CEmbeddedTriangle(const IBody& parent, const ISurface& surface, int nIndex, int p1, int p2, int p3)
: CTriangle(surface, nIndex, p1, p2, p3), m_parent(parent)
{
}

const IBody* CEmbeddedTriangle::Parent() const
{
  return &m_parent;
}

}
