#ifndef _MODEL_DIAGNOSTICS_H_
#define _MODEL_DIAGNOSTICS_H_

#include <QString>

class CModelBase;

namespace gm_diagnostics {
// Currently, diagnostics are only activated in geomec_shell's diagnostics interface
// used in CResultRegister::LoadCacheResults
const QString Hook_ResultRegister_LoadCacheResults("CResultRegister::LoadCacheResults");
// used in CModelBase::PreLoad
const QString Hook_ModelBase_LoadAnalysisLog("CModelBase::LoadAnalysisLog");
const QString Setting_LoadModelNoResults("LoadModelNoResults"); // don't store any result in the result cache
const QString Setting_LoadMainModelNoResults(
    "LoadMainModelNoResults"); // don't store the results of the main model in the result cache
const QString Setting_LoadZoominModelNoResults("LoadZoominModelNoResults");         // ditto for zoomin
const QString Setting_LoadWellZoominModelNoResults("LoadWellZoominModelNoResults"); // and wellpath zoomin
// used in CModelBase::LoadPost
const QString
    Setting_LoadMainModelOnly("LoadMainModelOnly"); // break off loading before processing the zoomin graphnodeentry
} // namespace gm_diagnostics

class IModelDiagnosticsHook {
public:
  virtual ~IModelDiagnosticsHook() {}
  virtual void start(CModelBase *) {}
  virtual void end(CModelBase *) {}
};

class IModelDiagnostics {
public:
  virtual ~IModelDiagnostics() {}
  virtual bool Setting(const QString &) const { return false; }
  virtual void Setting(const QString &, bool) {}
  virtual void Hook(const QString &, IModelDiagnosticsHook &) {}
  virtual IModelDiagnosticsHook *Hook(const QString &) const { return 0; }
};

#endif