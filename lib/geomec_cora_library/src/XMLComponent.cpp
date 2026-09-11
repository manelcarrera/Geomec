#include "XMLComponent.h"

namespace cora
{

CXMLComponent::CXMLComponent(CModelData& modelData, TObject object)
: m_object(object)
, m_responseParameters(modelData, object)
, m_inputParameters(modelData, object)
{
}

namespace
{

const QString COMPONENT_OPEN = "<Component Type=\"%1\" Name=\"%2\">";
const QString COMPONENT_CLOSE = "</Component>";

} // anonymous namespace

std::ostream& CXMLComponent::operator () (std::ostream& stream) const
{
  stream << COMPONENT_OPEN.arg(m_object->typeName()).
  arg(m_object->name()).toStdString() << std::endl;

  stream << m_responseParameters;
  stream << m_inputParameters;

  stream << COMPONENT_CLOSE.toStdString() << std::endl;

  return stream;
}

} // namespace cora

// global

std::ostream& operator << (std::ostream& stream, const cora::CXMLComponent& rhs)
{
  return rhs(stream);
}

std::ostream& operator << (std::ostream& stream, const cora::TXMLComponent& rhs)
{
  stream << *rhs;

  return stream;
}

std::ostream& operator << (std::ostream& stream,
  const cora::TXMLComponents& rhs)
{
  for (cora::TXMLComponents::const_iterator component = rhs.begin();
  component != rhs.end(); ++component)
  {
  stream << *component;
  }

  return stream;
}
