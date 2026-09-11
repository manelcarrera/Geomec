#include "ZoominModelEntry.h"
#include "BaseEntryTypes.h"
#include "resourceIDI.h"
#include "ModelBase.h"
#include "ZoominModelPlaceHolder.h"
#include "HexaModel.h"
#include "GoCadModel.h"
#include "GlobalMessage.h"

#include <QObject>

CZoominModelEntry::CZoominModelEntry(CModelBase& model)
: CStorageNodeEntry<CZoominModelPlaceHolder>(MD_BASE_ZOOMIN_MODEL, IDI_ZOOMIN_MODELS, QObject::tr("Zoom-in Models"), model)
{
}

CZoominModelEntry::~CZoominModelEntry()
{
}

namespace
{

const QString DEFAULT_ZOOM_IN_MODEL_NAME = QObject::tr("Zoomin Model %1");
const size_t DEFAULT_INITIAL_NUMBER = 1;

} // anonymous namespace

bool CZoominModelEntry::AddChildModel(CModelBase& childModel)
{
  CModelBase& model = static_cast<CModelBase&>(Model());

  if(!dynamic_cast<CHexaModel*>(&childModel) &&
     !dynamic_cast<CGoCadModel*>(&childModel))
  {
  _m()->msg(QObject::tr("Only GoCad models and hexahedron models "
                              "are supported as zoom-in models"));
     return false;
  }

  CZoominModelPlaceHolder* pPlaceHolder = new CZoominModelPlaceHolder(model, childModel);
  pPlaceHolder->reParent(this);

  model.OnNewChildModel(childModel, false);

  QString sName = createUniqueName(DEFAULT_ZOOM_IN_MODEL_NAME,
  DEFAULT_INITIAL_NUMBER);

  pPlaceHolder->Name(sName);

  return true;
}
