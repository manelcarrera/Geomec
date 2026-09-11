#ifndef _INTERFACE_DIAGNOSTICS_H_
#define _INTERFACE_DIAGNOSTICS_H_

#include "InterfaceBase.h"

class CModelDiagnostics;

namespace gm_shell {

class CInterfaceDiagnostics : public CInterfaceBase {
  QString m_model;
  CInterfaceModelContext *m_pModelContext;
  CModelDiagnostics *m_pModelDiagnostics;

public:
  typedef enum { DIAGNOSTICS_PARSE = IInterfaceBase::FOR_ME + 1, DIAGNOSTICS_HELP } TDiagnosticsCommand;

  virtual bool Expose() const;

  virtual const char *ShortDescription() const;

  virtual bool AcceptParameters(CArgumentParser &argParser, bool bForced = false);

  virtual void PrintUsage() const;

  virtual void Startup();

  virtual void HandleLastCommand();

private:
  void PrintHelp() const;

  int ParseAndEvaluate();
  int ParseAndEvaluateLoad(QString &line);
  int ParseAndEvaluateList(QString &line);
  int ParseAndEvaluateChangeDir(QString &line);
  int ParseAndEvaluateIgnore(QString &line);
  int ParseAndEvaluateFlushResults(QString &line);
  int ParseAndEvaluateSaveResults(QString &line);
  int ParseAndEvaluateSave(QString &line);
  int ParseAndEvaluateSetHook(QString &line);
  int ParseAndEvaluateSetTemp(QString &line);
};

} // namespace gm_shell

#endif