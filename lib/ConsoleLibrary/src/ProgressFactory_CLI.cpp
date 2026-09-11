#include "ProgressFactory_CLI.h"

#include "clDualProgress.h"
#include "clDualSilentProgress.h"
#include "clGeoProgress.h"
#include "clGraphMainFrameProgress.h"
#include "clSilentProgress.h"
#include "clWaitProgress.h"

IProgressBase *CProgressFactory_CLI::create(eProgress type_, const QString &title, bool cancel, int jobs) {
  return create_imp(type_, title, cancel, jobs);
}

IProgressBase *CProgressFactory_CLI::create_imp(eProgress type_, const QString &title, bool cancel, int jobs) {
  //
  // FIXME: don't maintain 2x enums
  //
  switch (type_) {
  case Geo:
    return new CGeoProgress(0, title, cancel);
  //
  // FIXME
  //
  case Wait:
    return new CWaitProgress();
  //
  // FIXME
  //
  case MainFrame:
    return new CGraphMainFrameProgress();
  //
  case Dual:
    return new CDualProgress(jobs, title, cancel);
  case DualSilent:
    return new CDualSilentProgress;
  case Silent:
    return new CSilentProgress;
  default:
    break; // ERROR
  }

  //
  // FIXME: this shouldn't happen
  //
  return new CGeoProgress(0, title, cancel);
}
