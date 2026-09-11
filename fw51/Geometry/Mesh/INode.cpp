 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "dimple.h"
#include "INode.h"
#include "IMesh.h"
#include "IElementSet.h"

namespace geo {

INode::INode()
{
}

INode::~INode()
{
}

const IElementSet* INode::IndexingElementSet() const
{
  return &Mesh();
}

} // namespace geo
