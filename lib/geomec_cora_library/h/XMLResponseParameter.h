#ifndef _cora_XMLResponseParameter_h_
#define _cora_XMLResponseParameter_h_

#include <ostream>
#include <vector>

#include "safeQSharedPointer.h"

#include "XMLResponseValueTypes.h"
#include "FailureMode.h"

namespace cora
{

class CXMLResponseParameter
{
  public:
  CXMLResponseParameter(TFailureMode& failureMode);

  std::ostream& operator () (std::ostream& stream) const;

  private:
  CXMLResponseParameter(const CXMLResponseParameter& rhs);
  CXMLResponseParameter& operator = (CXMLResponseParameter rhs);

  TFailureMode m_failureMode;
  CXMLResponseValueTypes m_responseValueTypes;
};

typedef QSharedPointer <CXMLResponseParameter> TXMLResponseParameter;
typedef std::vector <TXMLResponseParameter> TXMLResponseParameters;

} // namespace cora

// global

std::ostream& operator << (std::ostream& stream,
  const cora::CXMLResponseParameter& rhs);
std::ostream& operator << (std::ostream& stream,
  const cora::TXMLResponseParameter& rhs);
std::ostream& operator << (std::ostream& stream,
  const cora::TXMLResponseParameters& rhs);

#endif  // _cora_XMLResponseParameter_h_
