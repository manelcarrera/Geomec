#include "RunModelSingleRunEngine.h"
#include "RunModelData.h"
#include "DianaExecuter.h"
#include "SaveModelConsole.h"
#include "RetrieveDianaFileNamesConsole.h"

namespace cora
{

CRunModelSingleRunEngine::CRunModelSingleRunEngine(
  CAnalysisType::TAnalysisType analysisType)
: CRunModelEngineBase(analysisType)
{
}

namespace
{

void applyParameterFile(CRunModelData& runModelData)
{
  std::vector <TFailureTypeParameter> parameters =
    runModelData.parameterFile().getParameters();

  for (size_t s = 0; s < parameters.size(); ++s)
  {
    (*parameters[s]).modify(runModelData.modelData()());
  }
}

void validateResultComponents(CRunModelData& runModelData)
{
  TFailureModes failureModes =
    runModelData.getModelInfo().getFailureModeInfo().getFailureModes();

  for (size_t r = 0; r < failureModes.size(); ++r)
  {
    assert(failureModes[r]->getResultComponent()->RegisterIndex() == 0);
  }
}

void retrieveResponseParameterFile(CRunModelData& runModelData)
{
  std::vector <TLimitStateFunction> functions =
    runModelData.selectedLSFs().getLimitStateFunctions();

  for (size_t s = 0; s < functions.size(); ++s)
  {
    (*functions[s]).calculate(runModelData.responseParameterFile());
  }
}

} // anonymous namespace

bool CRunModelSingleRunEngine::run(CRunModelData& runModelData,
  CDianaExecuter& dianaExecuter, const QString& modelName,
  CSaveModelConsole& saveModelConsole,
  CRetrieveDianaFileNamesConsole& retrieveDianaFileNamesConsole) const
{
  if ((m_analysisType != CAnalysisType::AT_NONLIN) &&
    (runModelData.modelData()()->HasBranches() ||
      runModelData.modelData()()->HasPhases()))
  {
    QString additionalInformation =
      QString(getStringTableEntry(IDS_CHECK_FOR_ANALYSIS_WARNING)).
        arg(CAnalysisType(m_analysisType).Label()).
        arg(runModelData.modelData()()->InitialDepletionStage().Name());

    runModelData.summaryResultFile().addAdditionalInformation(
      additionalInformation);
  }

  bool ok = true;

  applyParameterFile(runModelData);

  ok = ok && runModelData.modelData()()->WriteFilosModel(
    runModelData.modelData().fileName(), &dianaExecuter,
    modelName.toStdString(), m_analysisType, false, false,
    saveModelConsole, retrieveDianaFileNamesConsole);

  if (ok)
  {
    validateResultComponents(runModelData);
    retrieveResponseParameterFile(runModelData);
  }

  return ok;
}

} // namespace cora
