#include "XMLResponseValueTypes.h"
#include "ResponseTypeBase.h"

namespace cora
{

CXMLResponseValueTypes::CXMLResponseValueTypes()
: m_responseValueTypes(createResponseValueTypes())
{
}

namespace
{

const QString RESPONSE_VALUE_TYPES_OPEN = "<ResponseValueTypes>";
const QString RESPONSE_VALUE_TYPES_CLOSE = "</ResponseValueTypes>";
const QString RESPONSE_VALUE_TYPES_EMPTY = "<ResponseValueTypes/>";

} // anonymous namespace

std::ostream& CXMLResponseValueTypes::operator () (std::ostream& stream) const
{
  if (m_responseValueTypes.empty())
  {
  stream << RESPONSE_VALUE_TYPES_EMPTY.toStdString() << std::endl;
  }
  else
  {
  stream << RESPONSE_VALUE_TYPES_OPEN.toStdString() << std::endl;

  stream << m_responseValueTypes;

  stream << RESPONSE_VALUE_TYPES_CLOSE.toStdString() << std::endl;
  }

  return stream;
}

// private

// static

TXMLResponseValueTypes CXMLResponseValueTypes::createResponseValueTypes()
{
  TXMLResponseValueTypes responseValueTypes;

  responseValueTypes.push_back(TXMLResponseValueType(new CXMLResponseValueType(
  CXMLResponseValueType::responseValueTypeSimple,
  CResponseTypeBase::RESPONSE_TYPE_MEAN)));
  responseValueTypes.push_back(TXMLResponseValueType(new CXMLResponseValueType(
  CXMLResponseValueType::responseValueTypeSimple,
  CResponseTypeBase::RESPONSE_TYPE_MAX)));
  responseValueTypes.push_back(TXMLResponseValueType(new CXMLResponseValueType(
  CXMLResponseValueType::responseValueTypeFailureArea,
  CResponseTypeBase::RESPONSE_TYPE_FAILURE_AREA_MAX)));
  responseValueTypes.push_back(TXMLResponseValueType(new CXMLResponseValueType(
  CXMLResponseValueType::responseValueTypeFailureArea,
  CResponseTypeBase::RESPONSE_TYPE_FAILURE_AREA_PERCENTAGE_MAX)));
  responseValueTypes.push_back(TXMLResponseValueType(new CXMLResponseValueType(
  CXMLResponseValueType::responseValueTypeSimple,
  CResponseTypeBase::RESPONSE_TYPE_MIN)));
  responseValueTypes.push_back(TXMLResponseValueType(new CXMLResponseValueType(
  CXMLResponseValueType::responseValueTypeFailureArea,
  CResponseTypeBase::RESPONSE_TYPE_FAILURE_AREA_MIN)));
  responseValueTypes.push_back(TXMLResponseValueType(new CXMLResponseValueType(
  CXMLResponseValueType::responseValueTypeFailureArea,
  CResponseTypeBase::RESPONSE_TYPE_FAILURE_AREA_PERCENTAGE_MIN)));

  return responseValueTypes;
}

} // namespace cora

// global

std::ostream& operator << (std::ostream& stream,
  const cora::CXMLResponseValueTypes& rhs)
{
  return rhs(stream);
}
