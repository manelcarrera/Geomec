#include "RunModel.h"
#include "GeomecUtils.h"
#include "ModelLifetimeFacade.h"
#include "VersionNumber.h"
#include "RunModelData.h"
#include "RunModelEngine.h"

namespace cora
{

CRunModel::CRunModel(CRunModelData& runModelData,
  const QString& applicationVersion)
: m_runModelData(runModelData)
, m_applicationVersion(applicationVersion)
, m_analysisTypes(selectAnalysisTypes(m_runModelData.summaryResultFile(),
  m_runModelData.selectedLSFs()))
{
  processModelData();
  validateVersionNumber();
}

namespace
{

const QString MODEL_PATH_AND_NAME = "the model file path and name are: %1";

QString addFullPath(const QString& fileName)
{
#if !defined(WIN32) && !defined(WIN64)
  char fullPath[PATH_MAX];

  realpath(fileName.toStdString().c_str(), fullPath);
#else
  TCHAR fullPath[MAX_PATH];

  GetFullPathName(fileName.toStdString().c_str(), MAX_PATH, fullPath, NULL);
#endif  // !defined(WIN32) && !defined(WIN64)

  QString additionalInformation = MODEL_PATH_AND_NAME.arg(fullPath);

  return additionalInformation;
}

const QString DIANA_FAILURE = "the execution of Diana failed, "
  "please consult the capture of the output";

void reportDianaFailure(CSummaryResultFile& summaryResultFile)
{
  summaryResultFile.addAdditionalInformation(DIANA_FAILURE);
}

} // anonymous namespace

bool CRunModel::operator () ()
{
  bool ok = true;

  if ((m_runModelData.modelData())() != 0)
  {
  QString modelPath;
  QString modelFileName;

  SplitPathAndFileName(m_runModelData.modelData().fileName(), modelPath,
      modelFileName);

  QString modelName = RemoveExtension(modelFileName);
  CRunModelEngine runModelEngine(*(m_analysisTypes.begin()));

  m_runModelData.summaryResultFile().addAdditionalInformation(
      addFullPath(m_runModelData.modelData().fileName()));

  ok = ok && runModelEngine.run(m_runModelData, modelName,
      m_applicationVersion);

  if (ok)
  {
      if (!m_runModelData.minimumOutput())
      {
    ok = IModelLifetimeFacade::SaveMain(CModelBase::CModelLoadSaveDefault(m_applicationVersion), *m_runModelData.modelData()(), modelFileName, true);
      }
  }
  else
  {
      reportDianaFailure(m_runModelData.summaryResultFile());
  }
  }

  return (ok && (m_runModelData.summaryResultFile().getResultValue() ==
  CSummaryResultFile::RESULT_VALUE_OK));
}

// private

namespace
{

const QString INVALID_VERSION = "invalid (command-line argument) "
  "version number '%1' (version '%2' is supported)";

} // anonymous namespace

void CRunModel::validateVersionNumber()
{
  if (VERSION_NUMBER < m_runModelData.versionNumber())
  {
  QString additionalInformation = QString(INVALID_VERSION).
      arg(m_runModelData.versionNumber()).arg(VERSION_NUMBER);

  m_runModelData.summaryResultFile().setResultValue(
      CSummaryResultFile::RESULT_VALUE_INCONSISTENT);
  m_runModelData.summaryResultFile().addAdditionalInformation(
      additionalInformation);
  }
}

namespace
{

const QString MODEL_DOES_NOT_EXIST = "model '%1' does not exist";

} // anonymous namespace

void CRunModel::processModelData()
{
  if (m_runModelData.modelData()() == 0)
  {
  QString additionalInformation =
      QString(MODEL_DOES_NOT_EXIST).arg(m_runModelData.modelData().fileName());

  m_runModelData.summaryResultFile().setResultValue(
      CSummaryResultFile::RESULT_VALUE_INCONSISTENT);
  m_runModelData.summaryResultFile().addAdditionalInformation(
      additionalInformation);
  }
}

// static

namespace
{

const size_t ONE_ANALYSIS_TYPE = 1;

const QString FIRST_ANALYSIS_TYPE_IS_PROCESSED = "multiple analysis types are "
  "not supported (yet), only the %1 analysis is processed";

} // anonymous namespace

CRunModel::TAnalysisTypes CRunModel::selectAnalysisTypes(
  CSummaryResultFile& summaryResultFile, const CSelectedLSFs& selectedLSFs)
{
  TAnalysisTypes analysisTypes;
  std::vector <TLimitStateFunction> functions =
  selectedLSFs.getLimitStateFunctions();

  for (size_t s = 0; s < functions.size(); ++s)
  {
  analysisTypes.insert((*functions[s]).getAnalysisType());
  }

  if (analysisTypes.size() != ONE_ANALYSIS_TYPE)
  {
  CAnalysisType analysisType(*(analysisTypes.begin()));

  summaryResultFile.addAdditionalInformation(
      QString(FIRST_ANALYSIS_TYPE_IS_PROCESSED).
    arg(analysisType.Label().toLower()));
  }

  return analysisTypes;
}

} // namespace cora
