#pragma once

#include "AnalysisType.h"
#include "IDCDianaRunner.h" // struct dia::IDianaRunner::RunParams
#include "IListener.h"
#include "Wait.h"

namespace std{
  class thread;
}

class CModelBase;
class CAnalysisType;
class CDepletionStage;
class CGeomecDianaRunnerBase;
class IDianaXWrapper;
class ISaveModel;
class IRetrieveDianaFileNames;

namespace std { class thread; }

namespace dia
{
class IAnalysisStatusContainer;
}

class CDianaRunController;
class CControllerHelper
{
  CDianaRunController* controller;
  public:
    CControllerHelper(CDianaRunController* controller_) : controller(controller_){};
};

namespace controller{
  struct Params
  {
    CModelBase& model;
    const CAnalysisType& antype;
    bool bWriteInputFiles;
    bool bWriteQuadDat;
    bool bWriteOutputStreamFile;
    QString strTempPath;
  };
}

class CDianaRunController : public IListener
{
public:
  enum eRunRes{ Success, RunError_, EndDepletionError, Cancelled, StartCheckError, GuardCheckError, RunFirstError, RunNextError, UndefinedError, EndStageError, StartRunnerError };

private:

  std::thread* m_th;

  CControllerHelper m_helper;
  dia::IDianaRunner::RunParams m_params;

  enum eDepletion{ Start, End, Guard };

  //
  // FIXME: I think no needed, verify it
  //
  std::string m_title;

  CWait m_wait;

public:
  enum eRunStep { All, First, Next };

private:
  bool check(eDepletion val);

  eRunRes run_stage(std::string& FFDIR, std::string& FF);

  bool run_step(eRunStep step=First);

  eRunRes run_all();

  void task(Cmd cmd);

protected:
  void handle(Cmd cmd);

public:

  dia::IDianaRunner::RunParams& params() { return m_params; };
  void params(const dia::IDianaRunner::RunParams& p);

  CDianaRunController(CModelBase& model,
            const CAnalysisType& antype, 
            bool bWriteInputFiles, 
            bool bWriteQuadDat,
            bool bWriteOutputStreamFile,
            const QString& strTempPath);

  CDianaRunController( const controller::Params& p );

  ~CDianaRunController();

  void CleanUpAfterRun(	const QString& getPathName, 
            IDianaXWrapper* dianaXWrapper,
            const std::string &title, 
            ISaveModel& saveModel,
            IRetrieveDianaFileNames& retrieveDianaFileNames);
  
  bool run(eRunStep step=All);

  void SetLicenseRetry(int nLicenseRetry);
  QString LicenseError() const;
  bool HaveResults() const;
  bool GetCalculationResult() const;

  CModelBase& Model();
  const CModelBase& Model() const;
  const CAnalysisType& AnalysisType() const;
  bool WriteInputFiles() const;
  bool WriteQuadDat() const;
  bool WriteOutputStreamFile() const;
  bool CleanupOldResults(int iStartStage) const;
  const CDepletionStage& StartStage() const;
  const CDepletionStage& EndStage() const;

  bool OnWriteBranchFiles(const QString& getPathName, bool bCalcResult,
    ISaveModel& saveModel) const;
  bool WriteCommands() const;
  bool OnModifyInputFileNames(std::string& sComfileName, std::string& sDatfileName) const;
  bool OnQueryInputFileNames(const std::string& title,
  std::string& sComfileName, std::string& sDatfileName,
  IRetrieveDianaFileNames& retrieveDianaFileNames) const;
  void RemoveRedundantLoads() const;

private:
  bool CreateBranchFiles() const;
  QString CreateBranchFileBaseName(const QString& sBaseName, int nStageIndex) const;

private:
  CModelBase& m_model;
  CAnalysisType m_antype;
  bool m_bWriteInputFiles;
  bool m_bWriteQuadDat;
  bool m_bWriteOutputStreamFile;
  const CDepletionStage* m_pStartStage;
  const CDepletionStage* m_pEndStage;
  QString m_strTempPath;
  CGeomecDianaRunnerBase* m_pRunner;
  int m_nLicenseRetry;
  QString m_sLicenseError;
  bool m_bHaveResults;
  bool m_bCalculationResult;
  mutable std::string m_sComFile;
  mutable std::string m_sDatFile;
  eRunStep m_step;

 public:
   CGeomecDianaRunnerBase* runner(){ return m_pRunner; }
   bool end();
   void clear( bool clean = true );
   QString& tmp(){ return m_strTempPath; }

   eRunStep step(){ return m_step; };
   void step(eRunStep val){ m_step=val; };
};
