
#include "DCNodalTemperature.h"

#include "INode.h"

#include "lbcx.h"
#include "lbfl.h"

namespace dia {

CNodalTemperature::CNodalTemperature(CBoundaCase &boundacase, double size, const geo::INode &node)
    : IFlowLoad(boundacase, size), m_Node(node) {}

const geo::INode &CNodalTemperature::Node() const { return m_Node; }

bool CNodalTemperature::WriteFilos() const {
  ftn_int_t idx = Inquire("NODAL", "DIM");
  if (idx < 0)
    idx = 0;
  ++idx;

  assert(!XistIndexed("NODAL/", &idx));

  PushDir();

  ChangeIndexedDir("NODAL/", &idx);

  ftn_int_t iNode = ftn_int_t(m_Node.Index() + 1);
  PutItemLength("NODES", &iNode, 1);

  PutCharItem("TYPE", "P");

  PutItemLength("VALUES", Values(), ValueSize());

  PopDir();

  return true;
}

} // namespace dia
