
#include "DCPrescribedPressurePotential.h"
#include "INode.h"
#include "lbcx.h"
#include "lbfl.h"

namespace dia {

CPrescribedPressurePotential::CPrescribedPressurePotential(const geo::INode &node, CLoadCase &lcase, double size)
    : IStructuralLoad(lcase, size), m_node(node) {}

bool CPrescribedPressurePotential::WriteFilos() const {
  assert(ValueSize() == 1);

  ftn_int_t idx = Inquire("DEFORM", "DIM");
  if (idx < 0)
    idx = 0;
  ++idx;

  assert(!XistIndexed("DEFORM/", &idx));

  PushDir();

  ChangeIndexedDir("DEFORM/", &idx);

  ftn_int_t nodenr = ftn_int_t(m_node.Index() + 1);
  PutItemLength("NODES", &nodenr, 1);

  PutCharItem("TYPE", "PR");

  ftn_double_t dValue = ftn_double_t(Values()[0]);
  PutItemLength("VALUES", &dValue, 1);

  PopDir();

  return true;
}

} // namespace dia