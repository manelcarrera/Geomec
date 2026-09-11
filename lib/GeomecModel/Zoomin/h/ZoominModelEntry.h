#ifndef _ZOOMINMODELENTRY_H_
#define _ZOOMINMODELENTRY_H_

#include "GeomecModelVisitor.h"
#include "StorageNode.h"

class CZoominModelPlaceHolder;
class CModelBase;

class CZoominModelEntry : public CStorageNodeEntry<CZoominModelPlaceHolder> {
public:
  CZoominModelEntry(CModelBase &model);
  virtual ~CZoominModelEntry();

  bool AddChildModel(CModelBase &childModel);

  ACCEPT_GEOMECMODELVISITORS(VisitZoominModelEntry);
};

#endif // _ZOOMINMODELENTRY_H_
