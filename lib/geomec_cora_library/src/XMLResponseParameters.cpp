#include "XMLResponseParameters.h"
#include "FemAppEntryTypes.h"
#include "FilterOutputProperty.h"
#include "FilterResults.h"
#include "GetFailureModeInfo.h"
#include "MaterialResultTree.h"
#include "ModelBase.h"
#include "ModelData.h"

namespace cora {

CXMLResponseParameters::CXMLResponseParameters(CModelData &modelData, TObject object)
    : m_responseParameters(createResponseParameters(modelData, object)) {}

namespace {

const QString RESPONSE_PARAMETERS_OPEN = "<ResponseParameters>";
const QString RESPONSE_PARAMETERS_CLOSE = "</ResponseParameters>";
const QString RESPONSE_PARAMETERS_EMPTY = "<ResponseParameters/>";

} // anonymous namespace

std::ostream &CXMLResponseParameters::operator()(std::ostream &stream) const {
  if (m_responseParameters.empty()) {
    stream << RESPONSE_PARAMETERS_EMPTY.toStdString() << std::endl;
  } else {
    stream << RESPONSE_PARAMETERS_OPEN.toStdString() << std::endl;

    stream << m_responseParameters;

    stream << RESPONSE_PARAMETERS_CLOSE.toStdString() << std::endl;
  }

  return stream;
}

// private

// static

TXMLResponseParameters CXMLResponseParameters::createResponseParameters(CModelData &modelData, TObject object) {
  TXMLResponseParameters responseParameters;
  CGetFailureModeInfo getFailureModeInfo(modelData());
  TFailureModes failureModes = getFailureModeInfo.getFailureModes();
  std::set<QString> failureModeLabels;

  for (TFailureModes::iterator failureMode = failureModes.begin(); failureMode != failureModes.end(); ++failureMode) {
    if ((*failureMode)->getObject() == object) {
      if (failureModeLabels.find((*failureMode)->getFailureModeLabel()) == failureModeLabels.end()) {
        failureModeLabels.insert((*failureMode)->getFailureModeLabel());
        responseParameters.push_back(TXMLResponseParameter(new CXMLResponseParameter(*failureMode)));
      }
    }
  }

  return responseParameters;
}

} // namespace cora

// global

std::ostream &operator<<(std::ostream &stream, const cora::CXMLResponseParameters &rhs) { return rhs(stream); }
