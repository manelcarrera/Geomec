#include "XMLResponseValueType.h"

namespace cora {

namespace {

const QString RESPONSE_VALUE_TYPE_SIMPLE = "FailsWhen=\"TooHigh\" />";
const QString RESPONSE_VALUE_TYPE_FAILURE_AREA = "FailsWhen=\"TooHigh\" "
                                                 "InternalResistance=\"0\" InternalFailsWhen=\"TooHigh\"/>";

} // anonymous namespace

CXMLResponseValueType::CXMLResponseValueType(const TResponseValueTypeSimple &, const QString &responseValueTypeName)
    : m_responseValueTypeName(responseValueTypeName), m_responseValueTypePostFix(RESPONSE_VALUE_TYPE_SIMPLE) {}

CXMLResponseValueType::CXMLResponseValueType(const TResponseValueTypeFailureArea &,
                                             const QString &responseValueTypeName)
    : m_responseValueTypeName(responseValueTypeName), m_responseValueTypePostFix(RESPONSE_VALUE_TYPE_FAILURE_AREA) {}

namespace {

const QString RESPONSE_VALUE_TYPE = "<ResponseValueType Name=\"%1\" %2";

} // anonymous namespace

std::ostream &CXMLResponseValueType::operator()(std::ostream &stream) const {
  stream << RESPONSE_VALUE_TYPE.arg(m_responseValueTypeName).arg(m_responseValueTypePostFix).toStdString() << std::endl;

  return stream;
}

// static

CXMLResponseValueType::TResponseValueTypeSimple CXMLResponseValueType::responseValueTypeSimple;
CXMLResponseValueType::TResponseValueTypeFailureArea CXMLResponseValueType::responseValueTypeFailureArea;

} // namespace cora

// global

std::ostream &operator<<(std::ostream &stream, const cora::CXMLResponseValueType &rhs) { return rhs(stream); }

std::ostream &operator<<(std::ostream &stream, const cora::TXMLResponseValueType &rhs) {
  stream << *rhs;

  return stream;
}

std::ostream &operator<<(std::ostream &stream, const cora::TXMLResponseValueTypes &rhs) {
  for (cora::TXMLResponseValueTypes::const_iterator responseValueType = rhs.begin(); responseValueType != rhs.end();
       ++responseValueType) {
    stream << *responseValueType;
  }

  return stream;
}
