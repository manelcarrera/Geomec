/* Copyright (c) 2011 TNO DIANA BV                              Confidential */

//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRIANGLE_H__9A39FDA8_F2AA_4D42_B8F2_38593DCDEB97__INCLUDED_)
#define AFX_TRIANGLE_H__9A39FDA8_F2AA_4D42_B8F2_38593DCDEB97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ITriangle.h"

#include "GeometryExports.h"

namespace geo {

class IMesh;
class ISurface;
class IElementSet;
class CPoint;

// simple 1st order triangle (3 nodes)
class GEOMETRY_EXPORT CTriangle : public ITriangle {
  CPoint *m_pPoint;
  int *m_pNode;
  int m_nIndex;
  const IElementSet *m_pElementSet;
  IMesh *m_pMesh;

public:
  // Construction ...
  CTriangle(/*IBody *pParent = 0*/); // Creates an empty triangle ...
  CTriangle(const ITriangle &rhs);   // Constructs from an unknown triagle ..
  CTriangle(const IPoint &p1,        //
            const IPoint &p2, const IPoint &p3);
  CTriangle(IMesh &mesh, int p1, int p2, int p3);
  CTriangle(IMesh &mesh, const IPoint &p1, const IPoint &p2, const IPoint &p3, bool bUnique = false);
  CTriangle(const ISurface &surface, int nIndex, int p1, int p2, int p3);
  virtual ~CTriangle();

  // Interface of IElement ...
  virtual const IPoint &Point(int nIndex) const;
  virtual void Point(int nIndex, const IPoint &pt);
  virtual size_t Order() const;

  virtual size_t NrOfNodes() const;
  virtual const INode &Node(int nIndex) const;
  virtual void Node(int nIndex, const IPoint &point);

  virtual const geo::IElementSet *IndexingElementSet() const;
  virtual int Index() const;
  virtual int PointIndex(int nIndex) const;
  virtual std::string Type() const;

  virtual int NrOfPoints() const;
};

} // namespace geo

#endif // !defined(AFX_TRIANGLE_H__9A39FDA8_F2AA_4D42_B8F2_38593DCDEB97__INCLUDED_)
