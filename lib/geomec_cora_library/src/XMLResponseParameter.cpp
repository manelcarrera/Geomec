#include "XMLResponseParameter.h"
#include "Result.h"

namespace cora
{

CXMLResponseParameter::CXMLResponseParameter(TFailureMode& failureMode)
: m_failureMode(failureMode)
, m_responseValueTypes()
{
}

namespace
{

const QString RESPONSE_PARAMETER_OPEN = "<ResponseParameter Name=\"%1\" "
  "Unit=\"%2\">";
const QString RESPONSE_PARAMETER_CLOSE = "</ResponseParameter>";

} // anonymous namespace

std::ostream& CXMLResponseParameter::operator () (std::ostream& stream) const
{
  stream << RESPONSE_PARAMETER_OPEN.arg(m_failureMode->getFailureModeLabel()).
  arg(m_failureMode->getResultComponent()->UnitName()).toStdString() <<
  std::endl;

  stream << m_responseValueTypes;

  stream << RESPONSE_PARAMETER_CLOSE.toStdString() << std::endl;

  return stream;
}

} // namespace cora

// global

std::ostream& operator << (std::ostream& stream,
  const cora::CXMLResponseParameter& rhs)
{
  return rhs(stream);
}

std::ostream& operator << (std::ostream& stream,
  const cora::TXMLResponseParameter& rhs)
{
  stream << *rhs;

  return stream;
}

std::ostream& operator << (std::ostream& stream,
  const cora::TXMLResponseParameters& rhs)
{
  for (cora::TXMLResponseParameters::const_iterator
  responseParameter = rhs.begin(); responseParameter != rhs.end();
  ++responseParameter)
  {
  stream << *responseParameter;
  }

  return stream;
}
