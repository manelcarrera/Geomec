#include "FailureTypeParameterSurface.h"
#include "GetModelInfo.h"

namespace cora
{

namespace
{

const QString SURFACE = "surface";

} // anonymous namespace

CFailureTypeParameterSurface::CFailureTypeParameterSurface(
  CSummaryResultFile& summaryResultFile, const std::string& object,
  const std::string& option, const std::string& parameter,
  std::vector <double>& value, const CGetModelInfo& modelInfo)

// TODO SURFACE-HORIZON
// create a new class CFailureTypeParameterHorizon
// for now replace 'getSurfaceInfo()' with 'getHorizonInfo()'

: CFailureTypeParameterBase(summaryResultFile, object, option, parameter, value,
    modelInfo.getHorizonInfo().getObjects(), SURFACE)
{
}

CFailureTypeParameterSurface::~CFailureTypeParameterSurface()
{
}

} // namespace cora
