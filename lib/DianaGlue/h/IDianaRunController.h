#pragma once

#include "Classes.h"

class CModelBase;
class CAnalysisType;
class IDianaXWrapper;
class ISaveModel;
class IRetrieveDianaFileNames;

namespace dia {
class IAnalysisStatusContainer;
}

#include <QString>

class IDianaRunController {
public:
  IDianaRunController(CModelBase &model, const CAnalysisType &antype, bool bWriteInputFiles, bool bWriteQuadDat,
                      bool bWriteOutputStreamFile, const QString &strTempPath) {};

  virtual void CleanUpAfterRun(const QString &getPathName, IDianaXWrapper *dianaXWrapper, const std::string &title,
                               ISaveModel &saveModel, IRetrieveDianaFileNames &retrieveDianaFileNames) = 0;

  //
  // TODO: virtual params()
  //

  virtual bool run(eRunStep step) = 0;
};