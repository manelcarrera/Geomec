#ifndef _DCNODALTEMPERATURE_H_
#define _DCNODALTEMPERATURE_H_

#include "IDCFlowLoad.h"

namespace dia {

class CNodalTemperature : public IFlowLoad {
  const geo::INode &m_Node;

public:
  CNodalTemperature(CBoundaCase &boundacase, double size, const geo::INode &node);

  const geo::INode &Node() const;
  virtual bool WriteFilos() const;
};

} // namespace dia

#endif // _DCNODALTEMPERATURE_H_
