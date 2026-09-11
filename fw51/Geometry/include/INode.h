/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef INODE_H
#define INODE_H

#include "IPoint.h"

#include "GeometryExports.h"

namespace geo {

class IMesh;
class IElement;
class IElementSet;

class GEOMETRY_EXPORT INode : public IPoint {
public:
  INode();
  ~INode();

  virtual const IMesh &Mesh() const = 0;
  virtual int Index() const = 0;

  // Looping over elements
  virtual int AttachedElementSize() const = 0;
  virtual const IElement &AttachedElement(int nIndex) const = 0;

  // Register element
  virtual int RegisterElement(IElement &element) = 0;
  virtual bool UnregisterElement(IElement &element) = 0;

  // Indexing elementset
  virtual const IElementSet *IndexingElementSet() const;
};

} // namespace geo

#endif
