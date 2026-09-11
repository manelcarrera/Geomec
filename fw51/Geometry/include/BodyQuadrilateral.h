/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef BODYQUADRILATERAL_H
#define BODYQUADRILATERAL_H

#include "IQuadrilateral.h"

#include "GeometryExports.h"

namespace geo {
class IBody;

class GEOMETRY_EXPORT CBodyQuadrilateral : public IQuadrilateral {
  IBody &m_parent; // the parent body
  int m_nIndex;    // the index of this face in the parent body

public:
  CBodyQuadrilateral(IBody &parent, int nIndex);
  ~CBodyQuadrilateral();

  virtual const IBody *Parent() const;

  virtual int PointIndex(int nIndex) const;
  virtual size_t NrOfNodes() const;
  virtual const INode &Node(int nIndex) const;
  virtual void Node(int nIndex, const IPoint &point);
  virtual int Index() const;

  virtual size_t Order() const;

  int BodyPoint(int nIndex) const;
  int BodyNode(int nIndex) const;
};

} // namespace geo

#endif
