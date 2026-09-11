#include "XMLInputParameter.h"

#ifdef _WIN32
#define MAKESTRING2(str)  #str
#define MAKESTRING(str) MAKESTRING2(str)
#pragma message("WARNING: disable C4800 (bool to int in Qt) in " __FILE__ "[" MAKESTRING(__LINE__) "]")
#pragma warning(push)
#pragma warning(disable: 4800)
#endif

#include "FormationBase.h"

#ifdef _WIN32
#pragma warning(pop)
#endif


namespace cora
{

CXMLInputParameter::CXMLInputParameter(const TParameter& inputParameter)
: m_inputParameter(inputParameter)
{
}

namespace
{

const QString INPUT_PARAMETER = "<InputParameter Name=\"%1\" "
  "MinValueActual=\"%2\" MaxValueActual=\"%3\" MeanValueActual=\"%4\" "
  "MinValueLegal=\"%5\" MaxValueLegal=\"%6\" />";

} // anonymous namespace

std::ostream& CXMLInputParameter::operator () (std::ostream& stream) const
{
  stream << INPUT_PARAMETER.arg(m_inputParameter->name()).
    arg(m_inputParameter->min()).arg(m_inputParameter->max()).
    arg(m_inputParameter->mean()).arg(m_inputParameter->lowerLimit()).
    arg(m_inputParameter->upperLimit()).toStdString() << std::endl;

  return stream;
}

} // namespace cora

// global

std::ostream& operator << (std::ostream& stream,
  const cora::CXMLInputParameter& rhs)
{
  return rhs(stream);
}

std::ostream& operator << (std::ostream& stream,
  const cora::TXMLInputParameter& rhs)
{
  stream << *rhs;

  return stream;
}

std::ostream& operator << (std::ostream& stream,
  const cora::TXMLInputParameters& rhs)
{
  for (cora::TXMLInputParameters::const_iterator inputParameter = rhs.begin();
    inputParameter != rhs.end(); ++inputParameter)
  {
    stream << *inputParameter;
  }

  return stream;
}
