#pragma once

class IDianaXWrapper;

#include "IDCDianaRunner.h" // Lib: DianaCore

class CDianaRunner : public dia::IDianaRunner  
{
public:
  CDianaRunner( const geo::IMesh &mesh ); // geo::IMesh defined in FW-Geometry
  virtual ~CDianaRunner();

protected:
  virtual bool OnRunAnalysis(IDianaXWrapper* dianaXWrapper, bool bSilent = false);
  bool OnRunAnalysis_(IDianaXWrapper* dianaXWrapper, bool bSilent = false);
  bool on_run_analysis_dsa(bool bSilent);


  // settings during run
  virtual bool ApplicationSpecificLogging() { return false; }
  virtual bool DefaultMessages() { return true; }
  virtual bool DisplayStopMessage() { return true; }
  virtual bool AutoCloseDialog() { return true; }

  virtual QString DianaExecutable(){ return ""; };

  // called just before and just after execution of kernel
  virtual void PreExecution() {}
  virtual void PostExecution(bool /*bCalcResult*/) {}

  // error starting kernel
  virtual void ErrorRunning() {}

  virtual QString LicenseError();
  virtual bool LoadPartialResults();
};
