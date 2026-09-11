#include "LimitStateFunctionWell.h"
#include "GetModelInfo.h"

namespace cora {

namespace {

const QString WELL = "well";

} // anonymous namespace

CLimitStateFunctionWell::CLimitStateFunctionWell(CSummaryResultFile &summaryResultFile, std::vector<QString> &function,
                                                 CGetModelInfo &modelInfo, const QString &versionNumber)
    : CLimitStateFunctionBase(summaryResultFile, function, modelInfo, versionNumber,
                              modelInfo.getWellPathInfo().getObjects(), WELL) {}

CLimitStateFunctionWell::~CLimitStateFunctionWell() {}

} // namespace cora
