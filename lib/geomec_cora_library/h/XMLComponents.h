#ifndef _cora_XMLComponents_h_
#define _cora_XMLComponents_h_

#include <ostream>

class CModelData;

#include "XMLComponent.h"

namespace cora
{

class CXMLComponents
{
  public:
  CXMLComponents(CModelData& modelData);

  std::ostream& operator () (std::ostream& stream) const;

  private:
  CXMLComponents(const CXMLComponents& rhs);
  CXMLComponents& operator = (CXMLComponents rhs);

  static TXMLComponents createComponents(CModelData& modelData);

  TXMLComponents m_components;
};

} // namespace cora

// global

std::ostream& operator << (std::ostream& stream,
  const cora::CXMLComponents& rhs);

#endif  // _cora_XMLComponents_h_
