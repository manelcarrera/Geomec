#include "RunModelStagedRunEngine.h"
#include "DianaExecuter.h"
#include "DianaRunController.h"
#include "GeomecUtils.h"
#include "Global.h"
#include "ISettings.h"
#include "RetrieveDianaFileNamesConsole.h"
#include "RunAnalysis_CLI.h"
#include "RunModelData.h"
#include "SaveModelConsole.h"

namespace cora {

CRunModelStagedRunEngine::CRunModelStagedRunEngine(CAnalysisType::TAnalysisType analysisType)
    : CRunModelEngineBase(analysisType) {}

namespace {

void applyParameterFile(CRunModelData &runModelData, int depletionStage) {
  std::vector<TFailureTypeParameter> parameters = runModelData.parameterFile().getParameters();

  for (size_t s = 0; s < parameters.size(); ++s) {
    if ((*parameters[s]).getActualParameter()->depletionStage() == depletionStage) {
      (*parameters[s]).modify(runModelData.modelData()());
    }
  }
}

void validateResultComponents(CRunModelData &runModelData) {
  TFailureModes failureModes = runModelData.getModelInfo().getFailureModeInfo().getFailureModes();

  for (size_t r = 0; r < failureModes.size(); ++r) {
    assert(failureModes[r]->getResultComponent()->RegisterIndex() == 0);
  }
}

void retrieveResponseParameterFile(CRunModelData &runModelData, int depletionStage) {
  std::vector<TLimitStateFunction> functions = runModelData.selectedLSFs().getLimitStateFunctions();

  for (size_t s = 0; s < functions.size(); ++s) {
    if ((*functions[s]).getDepletionStage() == depletionStage) {
      (*functions[s]).calculate(runModelData.responseParameterFile());
    }
  }
}

typedef std::vector<CDepletionStage::eOutputType> TOutputTypes;

TOutputTypes backupOutputTypes(CModelBase *modelBase) {
  TOutputTypes outputTypes;
  CDepletionStage *depletionStage = &(modelBase->DepletionStageEntry().LastStage());

  while (depletionStage && !depletionStage->Initial()) {
    outputTypes.push_back(depletionStage->OutputType());
    depletionStage = &(depletionStage->Previous());
  }

  return outputTypes;
}

void setOutputTypes(CModelBase *modelBase, CDepletionStage::eOutputType outputType) {
  CDepletionStage *depletionStage = &(modelBase->DepletionStageEntry().LastStage());

  while (depletionStage && !depletionStage->Initial()) {
    if ((depletionStage->OutputType() != CDepletionStage::BRANCH) &&
        (depletionStage->OutputType() != CDepletionStage::PHASE)) {
      depletionStage->setOutputType(outputType);
    }

    depletionStage = &(depletionStage->Previous());
  }
}

void restoreOutputTypes(CModelBase *modelBase, const TOutputTypes &outputTypes) {
  CDepletionStage *depletionStage = &(modelBase->DepletionStageEntry().LastStage());
  std::size_t d = 0;

  while (depletionStage && !depletionStage->Initial()) {
    depletionStage->setOutputType(outputTypes[d]);
    depletionStage = &(depletionStage->Previous());
  }
}

const int INITIAL_DEPLETION_STAGE = 0;

} // anonymous namespace

bool CRunModelStagedRunEngine::run(CRunModelData &runModelData, CDianaExecuter &dianaExecuter, const QString &modelName,
                                   CSaveModelConsole &saveModelConsole,
                                   CRetrieveDianaFileNamesConsole &retrieveDianaFileNamesConsole) const {
  const QString geomecTempPath = GetGeomecTempPathExt(CTempPath::TEMP_CALCULATION);
  CDianaRunController dianaRunController(*(runModelData.modelData()()), m_analysisType, false, false, false,
                                         geomecTempPath);
  bool ok = true;

  TOutputTypes outputTypes = backupOutputTypes(runModelData.modelData()());

  setOutputTypes(runModelData.modelData()(), CDepletionStage::PHASE);

  applyParameterFile(runModelData, INITIAL_DEPLETION_STAGE);

  dia::IDianaRunner::RunParams p = {runModelData.modelData().fileName(),
                                    modelName.toStdString(),
                                    geomecTempPath.toStdString(),
                                    &dianaExecuter,
                                    &saveModelConsole,
                                    &retrieveDianaFileNamesConsole,
                                    nullptr,
                                    false,
                                    _g->dsa()};
  dianaRunController.params(p);

  bool res = false;

  if (ok) {

    if (p.dsa) {
      CRunAnalysis_CLI ra(&dianaRunController, CDianaRunController::First);
      ra.wait();
      res = ra.res();
    } else {
      res = dianaRunController.run(CDianaRunController::First);
    }
  }

  ok = ok && res;

  if (ok) {
    validateResultComponents(runModelData);
    retrieveResponseParameterFile(runModelData, dianaRunController.EndStage().Index());
  }

  while (!dianaRunController.EndStage().Last()) {
    applyParameterFile(runModelData, dianaRunController.EndStage().Index());

    if (p.dsa) {
      CRunAnalysis_CLI ra(&dianaRunController, CDianaRunController::Next);
      ra.wait();
      res = ra.res();
    } else {
      res = dianaRunController.run(CDianaRunController::Next);
    }
    ok = ok && res;

    if (ok) {
      validateResultComponents(runModelData);
      retrieveResponseParameterFile(runModelData, dianaRunController.EndStage().Index());
    }
  }
  if (p.dsa)
    dianaRunController.clear();

  restoreOutputTypes(runModelData.modelData()(), outputTypes);

  return ok;
}

} // namespace cora
