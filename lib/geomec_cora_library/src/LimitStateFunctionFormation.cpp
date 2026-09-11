#include "LimitStateFunctionFormation.h"
#include "GetModelInfo.h"

namespace cora
{

namespace
{

const QString FORMATION = "formation";

} // anonymous namespace

CLimitStateFunctionFormation::CLimitStateFunctionFormation(
  CSummaryResultFile& summaryResultFile, std::vector <QString>& function,
  CGetModelInfo& modelInfo, const QString& versionNumber)
: CLimitStateFunctionBase(summaryResultFile, function, modelInfo, versionNumber,
  modelInfo.getFormationInfo().getObjects(), FORMATION)
{
}

CLimitStateFunctionFormation::~CLimitStateFunctionFormation()
{
}

} // namespace cora
