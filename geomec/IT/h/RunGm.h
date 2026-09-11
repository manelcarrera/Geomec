#pragma once

class CGeomecDoc;
class CModelBase;
class CDianaRunController;
class CDianaExecuter;
class CSaveModelConsole;
class CRetrieveDianaFileNamesConsole;

class CRunGm
{

  bool m_bHasRun;
  CGeomecDoc* m_doc;
  CModelBase* m_model;

  QString m_temp_path;
  QString m_app_version;

  QString m_model_full_path;
  QString m_model_path;
  QString m_model_name;

  CDianaRunController* m_pDianaRunController;
  CDianaExecuter* m_pDianaExecuter;
  CSaveModelConsole* m_pSaveModel;
  CRetrieveDianaFileNamesConsole* m_pRetrieveDianaFileNames;

private:
  CRunGm();
  ~CRunGm();

  void setup_diana_run_env();

public:

  static CRunGm *instance();

  bool step();

  void init();

};

