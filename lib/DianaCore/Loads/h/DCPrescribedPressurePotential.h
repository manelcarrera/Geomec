#ifndef _DCPRESCRIBEDPRESSUREPOTENTIAL_H_
#define _DCPRESCRIBEDPRESSUREPOTENTIAL_H_

#include "IDCStructuralLoad.h"

namespace dia {

class CPrescribedPressurePotential : public IStructuralLoad {
public:
  CPrescribedPressurePotential(const geo::INode &node, CLoadCase &lcase, double size);

  virtual bool WriteFilos() const;

private:
  const geo::INode &m_node;
};

} // namespace dia

#endif // _DCPRESCRIBEDPRESSUREPOTENTIAL_H_
