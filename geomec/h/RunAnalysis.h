#pragma once

#include "DSA_Context.h"
#include "IRunAnalysis.h"

class CGeomecDoc;
class CModelBase;
class CAnalysisType;
class IExecuteDianaDialog;

//
// TODO: make it completely dsa and pass non-dsa funtionality to non-dsa implementation: CRunAnalysis_NonDsa
//
// This is to run in dsa mode
// For non-dsa there is the class CRunAnalysis_NonDsa that takes this as a base class
// Even so as non-dsa one is not yet compeleted this still implements both: dsa and non-dsa
//
class CRunAnalysis : public IRunAnalysis {
public:
  enum eRunLogs { Step_1, Step_2, Step_3, Step_4, Step_5, Step_6 };
  enum eChecks { HasResults, Model, Input, License, NoError };

  struct RunParams {
    CAnalysisType::TAnalysisType ana_type;
    bool is_write_input_files;
    bool is_write_quad_dat;
  };

  static const char *m_checks_cp[];

private:
  DSA_Context *m_dsa_context;
  CGeomecDoc *m_doc;
  CModelBase *m_model;

public:
  CRunAnalysis();
  // void start_thread();

  ~CRunAnalysis();

  void on_params(const Cmd &cmd);

  bool run_controller();

  //
  // also used from MatParamDlg
  //
  static IExecuteDianaDialog *create_dlg();
  static void delete_dlg(IExecuteDianaDialog *dlg);

  DSA_Context *context() { return m_dsa_context; };

  //
  // FIXME: as far as MFC GUI is not decoupled, these functions needs to be called from main thread (GeomecDoc)
  //
public:
  eChecks checks();
  void save();
  //
  // next two not used:
  //		end_run is step-by-step run only
  //		end_controller: already doen in  controller and runner stage by stage
  //
  void end_run();

  //
  // public as them can be called from doc
  //
  void on_run_end();

  void on_start(const Cmd &cmd);
  void on_run(const Cmd &cmd);

private:
  bool checks(eChecks type);
  void create__(DSA_Context::eType type);
  void delete_();

  void handle(Cmd cmd); //=0
};