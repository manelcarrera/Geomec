#ifndef _cora_XMLModelInfo_h_
#define _cora_XMLModelInfo_h_

#include <ostream>

class CModelData;

#include "XMLComponents.h"

namespace cora {

class CXMLModelInfo {
public:
  CXMLModelInfo(CModelData &modelData, const char *modelInfoFileName);

  bool operator()() const;

private:
  CXMLModelInfo(const CXMLModelInfo &rhs);
  CXMLModelInfo &operator=(CXMLModelInfo rhs);

  bool createModelInfo() const;

  CModelData &m_modelData;
  const char *m_modelInfoFileName;
  CXMLComponents m_components;
};

} // namespace cora

#endif // _cora_XMLModelInfo_h_
