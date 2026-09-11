 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef _NODE_H_
#define _NODE_H_

#include "INode.h"

#include "GeometryExports.h"

namespace geo {

class GEOMETRY_EXPORT  CNode : public INode
{
  double *m_pCoord;
  const IMesh *m_pMesh;

public:
  // creation of a node without a mesh
  CNode();

  // creation of a node with a mesh
  CNode(const IMesh &mesh);

  // copy construction
  CNode(const CNode& rhs);

  // copy construction from another node
  CNode(const INode& rhs);

  // copy construction from another point, with or without mesh
  CNode(const IPoint& rhs, const IMesh& mesh);
  CNode(const IPoint& rhs);

  virtual ~CNode();

  const CNode& operator=(const CNode& rhs);
  const CNode& operator=(const INode& rhs);
  const CNode& operator=(const ICoordinate& rhs);

  virtual const IMesh &Mesh() const;
  virtual int Index() const;

  // Looping over elements
  virtual int AttachedElementSize() const;
  virtual const IElement& AttachedElement(int nIndex) const;

  // Register element
  virtual int RegisterElement(IElement& element);
  virtual bool UnregisterElement(IElement& element);

  virtual void AssertValid() const;
  virtual bool Empty() const;

  virtual const double& X() const;
  virtual const double& Y() const;
  virtual const double& Z() const;

  virtual void X(const double& x);
  virtual void Y(const double& y);
  virtual void Z(const double& z);
};

} // namespace geo

#endif // _NODE_H_
