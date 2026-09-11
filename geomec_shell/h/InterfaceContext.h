#ifndef _INTERFACE_CONTEXT_H_
#define _INTERFACE_CONTEXT_H_

#include "AnalysisLogger.h"
#include "VersionManager.h"

#include <QString>

// dsa
#include "Events.h"
#include "SafeQueue.h"
namespace std {
class thread;
}

class CModelBase;
class CDianaExecuter;
class CSaveModelConsole;
class CRetrieveDianaFileNamesConsole;
class CDianaRunController;

class IModelDiagnostics;

class CDianaRunController;

namespace gm_shell {

/////////////////////////////////////////////////
//
//				CInterfaceLogWrapper
//
/////////////////////////////////////////////////
class CInterfaceLogWrapper {
public:
  virtual ~CInterfaceLogWrapper() {}
  virtual void AddLine(const QString & /*line*/, bool /*bVerbose*/ = false, bool /*bFatal*/ = false) const {}
};

/////////////////////////////////////////////////
//
//				CInterfaceModelContext
//
/////////////////////////////////////////////////
class CInterfaceModelContext {
  CAnalysisLogger m_logger;
  CModelBase *m_pModel;
  CVersionManager m_VersionManager;

  bool m_created;

  QString m_app_version;

  QString m_model_full_path;
  QString m_model_path;
  QString m_model_name;
  QString m_model_extension;

  QString m_output_path;
  QString m_temp_path;

  QString m_type;

  CDianaExecuter *m_pDianaExecuter;
  CSaveModelConsole *m_pSaveModel;
  CRetrieveDianaFileNamesConsole *m_pRetrieveDianaFileNames;

  int m_nStage;
  bool m_bHasRun;
  bool m_bAutoExport;
  int m_nLicenseRetry;

  QString m_sLicenseError;
  bool m_bHaveResults;
  bool m_bCalculationResult;

public:
  CInterfaceModelContext(const QString &app_version);
  ~CInterfaceModelContext();

  void Load(const QString &model, IModelDiagnostics *pModelDiagnostics = 0);
  void Create(const QString &model);
  void Create(const QString &model, const QString &skua);
  bool Validate(bool forRockMech);

  void OutputPath(const QString &outputPath);
  QString OutputPath() const;

  void TempPath(const QString &tempPath);
  QString TempPath() const;

  QString ModelFullPath() const;
  QString ModelPath() const;
  QString ModelName() const;
  QString ModelExtension() const;

  CModelBase *Model();
  bool Created() const;

  void SwitchTo(CModelBase *model);
  void SwitchToParent();

  void CopyFiles();
  void MoveFiles();

  void Stage(int nStage);
  int Stage() const;

  void Type(const QString &type);
  QString Type() const;

  void AutoExport(bool flag);
  bool AutoExport() const;

  void LicenseRetry(int nLicenseRetry);
  int LicenseRetry() const;

  QString LicenseError() const;
  bool HaveResults() const;
  bool GetCalculationResult() const;

  bool Run(CInterfaceLogWrapper *log = 0);
  bool RunStep(CInterfaceLogWrapper *log = 0);
  void End();
  bool HasRun() const;
  void CleanUpAfterRun();

  void Save();
  void SaveAs(const QString &fileName);

  // deprecated
  CDianaExecuter *GetDianaExecuter();
  CSaveModelConsole *GetSaveModel();
  CRetrieveDianaFileNamesConsole *GetRetrieveDianaFileNames();
  QString ModelWorkPath();

private:
  void ProcessModelPath(const QString &model);
  void SetupDianaRunEnvironment();
  void ShutdownDianaRunEnvironment();

private:
  CDianaRunController *m_drc;
};

} // namespace gm_shell

#endif