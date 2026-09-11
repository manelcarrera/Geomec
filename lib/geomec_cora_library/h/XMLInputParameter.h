#ifndef _cora_XMLInputParameter_h_
#define _cora_XMLInputParameter_h_

#include <ostream>
#include <vector>

#include "safeQSharedPointer.h"

#include "Parameter.h"

namespace cora
{

class CXMLInputParameter
{
  public:
  CXMLInputParameter(const TParameter& inputParameter);

  std::ostream& operator () (std::ostream& stream) const;

  private:
  CXMLInputParameter(const CXMLInputParameter& rhs);
  CXMLInputParameter& operator = (CXMLInputParameter rhs);

  const TParameter m_inputParameter;
};

typedef QSharedPointer <CXMLInputParameter> TXMLInputParameter;
typedef std::vector <TXMLInputParameter> TXMLInputParameters;

} // namespace cora

// global

std::ostream& operator << (std::ostream& stream,
  const cora::CXMLInputParameter& rhs);
std::ostream& operator << (std::ostream& stream,
  const cora::TXMLInputParameter& rhs);
std::ostream& operator << (std::ostream& stream,
  const cora::TXMLInputParameters& rhs);

#endif  // _cora_XMLInputParameter_h_
