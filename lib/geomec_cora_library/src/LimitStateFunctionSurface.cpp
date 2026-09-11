#include "LimitStateFunctionSurface.h"
#include "GetModelInfo.h"

namespace cora
{

namespace
{

const QString SURFACE = "surface";

} // anonymous namespace

CLimitStateFunctionSurface::CLimitStateFunctionSurface(
  CSummaryResultFile& summaryResultFile, std::vector <QString>& function,
  CGetModelInfo& modelInfo, const QString& versionNumber)

// TODO SURFACE-HORIZON
// create a new class CLimitStateFunctionHorizon
// for now replace 'getSurfaceInfo()' with 'getHorizonInfo()'

: CLimitStateFunctionBase(summaryResultFile, function, modelInfo, versionNumber,
  modelInfo.getHorizonInfo().getObjects(), SURFACE)
{
}

CLimitStateFunctionSurface::~CLimitStateFunctionSurface()
{
}

} // namespace cora
