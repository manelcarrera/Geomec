#ifndef _cora_XMLInputParameters_h_
#define _cora_XMLInputParameters_h_

#include <ostream>

class CModelData;

#include "Object.h"
#include "XMLInputParameter.h"

namespace cora {

class CXMLInputParameters {
public:
  CXMLInputParameters(CModelData &modelData, const TObject &object);

  std::ostream &operator()(std::ostream &stream) const;

private:
  CXMLInputParameters(const CXMLInputParameters &rhs);
  CXMLInputParameters &operator=(CXMLInputParameters rhs);

  static TXMLInputParameters createInputParameters(CModelData &modelData, TObject object);

  TXMLInputParameters m_inputParameters;
};

} // namespace cora

// global

std::ostream &operator<<(std::ostream &stream, const cora::CXMLInputParameters &rhs);

#endif // _cora_XMLInputParameters_h_
