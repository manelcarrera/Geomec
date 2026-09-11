#ifndef _cora_XMLResponseParameters_h_
#define _cora_XMLResponseParameters_h_

#include <ostream>

class CModelData;

namespace cora {

class CFilterResults;
class CFilterOutputProperty;

} // namespace cora

#include "Object.h"
#include "XMLResponseParameter.h"

namespace cora {

class CXMLResponseParameters {
public:
  CXMLResponseParameters(CModelData &modelData, TObject object);

  std::ostream &operator()(std::ostream &stream) const;

private:
  CXMLResponseParameters(const CXMLResponseParameters &rhs);
  CXMLResponseParameters &operator=(CXMLResponseParameters rhs);

  static TXMLResponseParameters createResponseParameters(CModelData &modelData, TObject object);

  TXMLResponseParameters m_responseParameters;
};

} // namespace cora

// global

std::ostream &operator<<(std::ostream &stream, const cora::CXMLResponseParameters &rhs);

#endif // _cora_XMLResponseParameters_h_
