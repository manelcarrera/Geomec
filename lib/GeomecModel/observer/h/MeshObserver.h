#ifndef _MeshObserver_h_
#define _MeshObserver_h_

class CMeshBase;
class CValueCacheBase;

#include "GraphNode.h"
#include "GeomecModelVisitor.h"

class CMeshObserver : public CGraphNode
{
public:
  CMeshObserver(CMeshBase* mesh, CValueCacheBase& valueCache);

  CMeshObserver(const CMeshObserver& rhs);
  CMeshObserver& operator = (const CMeshObserver& rhs);

  virtual ~CMeshObserver();

  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;

  virtual void OnNewNeighbour(const CGraphNode& node);
  virtual void OnNeighbourModified(const CGraphNode& node, enum ModifiedHint hint);
  virtual void OnNeighbourDeleted(const CGraphNode& node);

  CMeshBase* getMesh() const;

  ACCEPT_GEOMECMODELVISITORS(VisitMeshObserver);

private:
  void swap(CMeshObserver& aValueCache);

  CMeshBase* m_Mesh;
  CValueCacheBase& m_ValueCache;
};

#endif  // _MeshObserver_h_
