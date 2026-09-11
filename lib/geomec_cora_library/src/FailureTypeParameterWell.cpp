#include "FailureTypeParameterWell.h"
#include "GetModelInfo.h"

namespace cora
{

namespace
{

const QString WELL = "well";

} // anonymous namespace

CFailureTypeParameterWell::CFailureTypeParameterWell(
  CSummaryResultFile& summaryResultFile, const std::string& object,
  const std::string& option, const std::string& parameter,
  std::vector <double>& value, const CGetModelInfo& modelInfo)
: CFailureTypeParameterBase(summaryResultFile, object, option, parameter, value,
  modelInfo.getWellPathInfo().getObjects(), WELL)
{
}

CFailureTypeParameterWell::~CFailureTypeParameterWell()
{
}

} // namespace cora
