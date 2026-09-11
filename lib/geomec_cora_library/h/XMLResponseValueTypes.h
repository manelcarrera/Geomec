#ifndef _cora_XMLResponseValueTypes_h_
#define _cora_XMLResponseValueTypes_h_

#include <ostream>

#include "XMLResponseValueType.h"

namespace cora
{

class CXMLResponseValueTypes
{
  public:
  CXMLResponseValueTypes();

  std::ostream& operator () (std::ostream& stream) const;

  private:
  CXMLResponseValueTypes(const CXMLResponseValueTypes& rhs);
  CXMLResponseValueTypes& operator = (CXMLResponseValueTypes rhs);

  static TXMLResponseValueTypes createResponseValueTypes();

  TXMLResponseValueTypes m_responseValueTypes;
};

} // namespace cora

// global

std::ostream& operator << (std::ostream& stream,
  const cora::CXMLResponseValueTypes& rhs);

#endif  // _cora_XMLResponseValueTypes_h_
