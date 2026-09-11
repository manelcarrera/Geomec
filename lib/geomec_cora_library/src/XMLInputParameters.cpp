#include "XMLInputParameters.h"
#include "ModelData.h"

namespace cora
{

CXMLInputParameters::CXMLInputParameters(CModelData& modelData,
  const TObject& object)
: m_inputParameters(createInputParameters(modelData, object))
{
}

namespace
{

const QString INPUT_PARAMETERS_OPEN = "<InputParameters>";
const QString INPUT_PARAMETERS_CLOSE = "</InputParameters>";
const QString INPUT_PARAMETERS_EMPTY = "<InputParameters/>";

} // anonymous namespace

std::ostream& CXMLInputParameters::operator () (std::ostream& stream) const
{
  if (m_inputParameters.empty())
  {
    stream << INPUT_PARAMETERS_EMPTY.toStdString() << std::endl;
  }
  else
  {
    stream << INPUT_PARAMETERS_OPEN.toStdString() << std::endl;

    stream << m_inputParameters;

    stream << INPUT_PARAMETERS_CLOSE.toStdString() << std::endl;
  }

  return stream;
}

// private

// static

TXMLInputParameters CXMLInputParameters::createInputParameters(
  CModelData& /*modelData*/, TObject object)
{
  TXMLInputParameters inputParameters;
  const TParameters parameters = object->getParameters();

  for (TParameters::const_iterator parameter = parameters.begin();
    parameter != parameters.end(); ++parameter)
  {
    inputParameters.push_back(TXMLInputParameter(
      new CXMLInputParameter(*parameter)));
  }

  return inputParameters;
}

} // namespace cora

// global

std::ostream& operator << (std::ostream& stream,
  const cora::CXMLInputParameters& rhs)
{
  return rhs(stream);
}
