/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "INode.h"
#include "IElementSet.h"
#include "IMesh.h"
#include "dimple.h"

namespace geo {

INode::INode() {}

INode::~INode() {}

const IElementSet *INode::IndexingElementSet() const { return &Mesh(); }

} // namespace geo
