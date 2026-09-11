 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// TriangleMeshElement.cpp: implementation of the CTriangleElement class.
//
//////////////////////////////////////////////////////////////////////
typedef double REAL;

#include "tri_triangle.h"
#include "TriangleSurface.h"
#include "TriangleElement.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTriangleElement::CTriangleElement(const CTriangleSurface& surface, int nIndex, int *pointindex)
: m_nIndex(nIndex), m_surface(surface), m_pPointIndex(pointindex)
{
  assert(m_pPointIndex);
}

CTriangleElement::~CTriangleElement()
{
}

const geo::IPoint &CTriangleElement::Point(const int nIndex) const
{
  assert(nIndex >= 0 && nIndex < 3);
  return m_surface.Point(PointIndex(nIndex));	
}
  
void CTriangleElement::Point(const int /*nIndex*/, const geo::IPoint &/*pt*/)
{
  assert(false);
}

int CTriangleElement::NrOfPoints() const
{
  return 3;
}

const geo::IElementSet* CTriangleElement::IndexingElementSet() const
{
  return &m_surface;
}

int CTriangleElement::Index() const
{
  return m_nIndex;
}

int CTriangleElement::PointIndex(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < 3);
  return m_pPointIndex[nIndex];
}

std::string CTriangleElement::Type() const
{
  assert(false);
  return "ElementType";
}
  
void CTriangleElement::AssertValid() const
{
}

bool CTriangleElement::Empty() const
{
  return false;
}


