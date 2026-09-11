/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// MeshTriangle.h: interface for the CMeshTriangle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHTRIANGLE_H__E854E827_F7C6_4FEE_88F0_C6C9287370E8__INCLUDED_)
#define AFX_MESHTRIANGLE_H__E854E827_F7C6_4FEE_88F0_C6C9287370E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IMesh.h"
#include "ITriangle.h"

#include "GeometryExports.h"

namespace geo {

class GEOMETRY_EXPORT CMeshTriangle : public ITriangle {
  IMesh &m_mesh;
  int m_vcNode[3];
  int m_nIndex;

public:
  CMeshTriangle(IMesh &mesh, const IPoint &p0, const IPoint &p1, const IPoint &p2);
  CMeshTriangle(IMesh &mesh, int p0, int p1, int p2);
  virtual ~CMeshTriangle();

  // Index and indexing element set
  virtual const IElementSet *IndexingElementSet() const;
  virtual int Index() const;
  virtual int PointIndex(int nIndex) const;

  // Query of points
  virtual const geo::IPoint &Point(int nIndex) const;
  virtual void Point(int nIndex, const geo::IPoint &pt);
  virtual int NrOfPoints() const;
};

} // namespace geo

#endif // !defined(AFX_MESHTRIANGLE_H__E854E827_F7C6_4FEE_88F0_C6C9287370E8__INCLUDED_)
