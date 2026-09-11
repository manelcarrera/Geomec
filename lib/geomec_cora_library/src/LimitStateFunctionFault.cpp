#include "LimitStateFunctionFault.h"
#include "GetModelInfo.h"

namespace cora {

namespace {

const QString FAULT = "fault";

} // anonymous namespace

CLimitStateFunctionFault::CLimitStateFunctionFault(CSummaryResultFile &summaryResultFile,
                                                   std::vector<QString> &function, CGetModelInfo &modelInfo,
                                                   const QString &versionNumber)
    : CLimitStateFunctionBase(summaryResultFile, function, modelInfo, versionNumber,
                              modelInfo.getFaultInfo().getObjects(), FAULT) {}

CLimitStateFunctionFault::~CLimitStateFunctionFault() {}

} // namespace cora
