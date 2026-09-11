#ifndef _IMATERIALSERVERBASE_H_
#define _IMATERIALSERVERBASE_H_

class IMaterialBase;

#include "StorageNode.h"
#include "GeomecModelVisitor.h"
class CMaterialKey;
class CFFMaterial;

namespace geo
{
class IElementSet;
}

class IMaterialServerBase : public CStorageNode
{
public:
  IMaterialServerBase(const QString& strName, CFemAppModel& model);
  virtual ~IMaterialServerBase();

	virtual bool ReadOnly() const = 0;
  virtual const IMaterialBase* Material() const = 0;

  virtual void AddToCache(CMaterialKey& /*key*/, CFFMaterial * /*material*/) const {}

  virtual bool PrepareMapping(const geo::IElementSet *) const = 0;

  ACCEPT_GEOMECMODELVISITORS(VisitIMaterialServerBase);
};

#endif // _IMATERIALSERVERBASE_H_
