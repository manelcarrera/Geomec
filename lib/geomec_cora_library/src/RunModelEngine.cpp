#include "RunModelEngine.h"
#include "RunModelStagedRunEngine.h"
#include "RunModelSingleRunEngine.h"
#include "DianaExecuter.h"
#include "SaveModelConsole.h"
#include "RetrieveDianaFileNamesConsole.h"
#include "RunModelData.h"

namespace cora
{

CRunModelEngine::CRunModelEngine(CAnalysisType::TAnalysisType analysisType)
: m_runModelEngineBase(selectEngine(analysisType))
{
}

bool CRunModelEngine::run(CRunModelData& runModelData,
  const QString& modelName, const QString& applicationVersion) const
{
  CDianaExecuter dianaExecuter;
  CSaveModelConsole saveModelConsole(*runModelData.modelData()(),
    applicationVersion);
  CRetrieveDianaFileNamesConsole retrieveDianaFileNamesConsole;

  return m_runModelEngineBase->run(runModelData, dianaExecuter, modelName,
    saveModelConsole, retrieveDianaFileNamesConsole);
}

// private

// static

TRunModelEngineBase CRunModelEngine::selectEngine(
  CAnalysisType::TAnalysisType analysisType)
{
  TRunModelEngineBase runModelEngineBase;

  if (analysisType == CAnalysisType::AT_NONLIN)
  {
    runModelEngineBase =
      TRunModelEngineBase(new CRunModelStagedRunEngine(analysisType));
  }
  else
  {
    runModelEngineBase =
      TRunModelEngineBase(new CRunModelSingleRunEngine(analysisType));
  }

  return runModelEngineBase;
}

} // namespace cora
