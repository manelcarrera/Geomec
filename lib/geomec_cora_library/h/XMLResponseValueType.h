#ifndef _cora_XMLResponseValueType_h_
#define _cora_XMLResponseValueType_h_

#include <ostream>
#include <vector>

#include <QString>
#include "safeQSharedPointer.h"

namespace cora
{

class CXMLResponseValueType
{
  public:
  static struct TResponseValueTypeSimple {} responseValueTypeSimple;
  static struct TResponseValueTypeFailureArea {} responseValueTypeFailureArea;

  CXMLResponseValueType(const TResponseValueTypeSimple&,
      const QString& responseValueTypeName);
  CXMLResponseValueType(const TResponseValueTypeFailureArea&,
      const QString& responseValueTypeName);

  std::ostream& operator () (std::ostream& stream) const;

  private:
  CXMLResponseValueType(const CXMLResponseValueType& rhs);
  CXMLResponseValueType& operator = (CXMLResponseValueType rhs);

  const QString m_responseValueTypeName;
  const QString m_responseValueTypePostFix;
};

typedef QSharedPointer <CXMLResponseValueType> TXMLResponseValueType;
typedef std::vector <TXMLResponseValueType> TXMLResponseValueTypes;

} // namespace cora

// global

std::ostream& operator << (std::ostream& stream,
  const cora::CXMLResponseValueType& rhs);
std::ostream& operator << (std::ostream& stream,
  const cora::TXMLResponseValueType& rhs);
std::ostream& operator << (std::ostream& stream,
  const cora::TXMLResponseValueTypes& rhs);

#endif  // _cora_XMLResponseValueType_h_
