#include "FailureTypeParameterFormation.h"
#include "GetModelInfo.h"

namespace cora
{

const QString CFailureTypeParameterFormation::FORMATION = "formation";

CFailureTypeParameterFormation::CFailureTypeParameterFormation(
  CSummaryResultFile& summaryResultFile, const std::string& object,
  const std::string& option, const std::string& parameter,
  std::vector <double>& value, const CGetModelInfo& modelInfo)
: CFailureTypeParameterBase(summaryResultFile, object, option, parameter, value,
    modelInfo.getFormationInfo().getObjects(), FORMATION)
{
}

CFailureTypeParameterFormation::~CFailureTypeParameterFormation()
{
}

} // namespace cora
