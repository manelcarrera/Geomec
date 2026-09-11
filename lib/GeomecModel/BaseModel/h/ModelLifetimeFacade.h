#ifndef _MODEL_LIFETIME_FACADE_H_
#define _MODEL_LIFETIME_FACADE_H_

class QString;
class CAnalysisLogger;
class CWellPath;

#include "ModelBase.h"

// We don't use these, but this makes it consistent with the types defined in VersionNumbers
#define WELL_ZOOMIN_MODEL 5
#define WELL_CASING_MODEL 6

// IModelLifetimeFacade works as a namespace
// (we may change this later to an actual object, if we find we need to maintain state)
//
// Intent is to have one interface dealing with New/Load/Save/Close/Delete of a model
//
// Currently, the interface is a straightforward translation of what existed
// but we want to add context to be able to differentiate between semantically different actions
// Eg, New(HEXA...) is used for a new main model / project, a new zoomin model, creating a model during load
// Or, Load is used for loading a model file as main project, and for import
//
// This makes it difficult to add decorators in a single place (for example, resetting singletons for a new(-ly loaded)
// project
//
// Update: added context Main/Child to New/Load/Save (Save is only Main)

class IModelLifetimeFacade {

  static void NewMainDecorators();

public:
  // we may add decorators to a main model
  static CModelBase *NewMain(int type, CAnalysisLogger &logger, const CVersionManager &versionManager);

  // child models should not have their own decorators
  static CModelBase *NewChild(int type, CAnalysisLogger &logger, const CVersionManager &versionManager);
  static CModelBase *NewChild(int type, CWellZoomInModel &parent, CAnalysisLogger &logger,
                              const CVersionManager &versionManager);
  static CModelBase *NewChild(int type, CModelBase &parent, CNewWellPath &wp, CAnalysisLogger &logger,
                              const CVersionManager &versionManager);
  static CModelBase *NewChild(int type, CModelBase &parent, CWellPath &wp, CAnalysisLogger &logger,
                              const CVersionManager &versionManager);

  static CModelBase *LoadMain(const CModelBase::IModelLoadSave &modelLoadSave, const QString &strFileName,
                              CAnalysisLogger &logger, bool bSilent = false);
  static CModelBase *LoadChild(const CModelBase::IModelLoadSave &modelLoadSave, const QString &strFileName,
                               CAnalysisLogger &logger);
  static bool SaveMain(const CModelBase::IModelLoadSave &modelLoadSave, CModelBase &model, const QString &strFileName,
                       bool bSwitch);

  static void Close(CModelBase *model);

  static void Delete(CModelBase *model);
};

#endif