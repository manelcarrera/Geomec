#include "DianaRunController.h"

#include <QString>

CDianaRunController::CDianaRunController(CModelBase &model, const CAnalysisType &antype, bool bWriteInputFiles,
                                         bool bWriteQuadDat, bool bWriteOutputStreamFile, const QString &strTempPath)
    : IDianaRunController(model, antype, bWriteInputFiles, bWriteQuadDat, bWriteOutputStreamFile, strTempPath) {}

bool CDianaRunController::RunFirst(const QString &getPathName, IDianaXWrapper *dianaXWrapper, const std::string &title,
                                   ISaveModel &saveModel, IRetrieveDianaFileNames &retrieveDianaFileNames) {
  return true;
}

bool CDianaRunController::RunNext(const QString &getPathName, IDianaXWrapper *dianaXWrapper, const std::string &title,
                                  ISaveModel &saveModel, IRetrieveDianaFileNames &retrieveDianaFileNames) {
  return true;
}

void CDianaRunController::CleanUpAfterRun(const QString &getPathName, IDianaXWrapper *dianaXWrapper,
                                          const std::string &title, ISaveModel &saveModel,
                                          IRetrieveDianaFileNames &retrieveDianaFileNames) {}

bool CDianaRunController::Run(const QString &getPathName, IDianaXWrapper *dianaXWrapper, const std::string &title,
                              ISaveModel &saveModel, IRetrieveDianaFileNames &retrieveDianaFileNames,
                              dia::IAnalysisStatusContainer *pAnalysisStatusContainer, bool bSilent) {
  return true;
}
