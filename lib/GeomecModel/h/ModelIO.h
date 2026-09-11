#ifndef _LOADMODEL_H_
#define _LOADMODEL_H_

class QString;
class CAnalysisLogger;

#include "ModelBase.h"


// use IModelLifetimeFacade

namespace privateModelIO
{
CModelBase* LoadModel(const CModelBase::IModelLoadSave& modelLoadSave, const QString& strFileName, CAnalysisLogger& logger, bool bMain, bool bSilent = false);
bool SaveModel(const CModelBase::IModelLoadSave& modelLoadSave, CModelBase& model, const QString& strFileName, bool bSwitch = true);
}

#endif // _LOADMODEL_H_
