#ifndef _LASPOINTSET_H_
#define _LASPOINTSET_H_

#include "PointSet.h"

class CLasPointSet : public CPointSet {
public:
  CLasPointSet(const CNewWellPath &well, const QString &sName, int nValueSetSize, DIMENSION dim,
               bool bCreateValueSetForCoordinates = true);

  CLasPointSet(CFemAppModel &model);

  const CNewWellPath *NewWellPath() const;
  void NewWellPath(const CNewWellPath *p);

  virtual void OnNewNeighbour(const CGraphNode &node);
  virtual void OnNeighbourDeleted(const CGraphNode &item);

  ACCEPT_GEOMECMODELVISITORS(VisitLasPointSet);

private:
  const CNewWellPath *m_pNewWellPath;
};

#endif // _LASPOINTSET_H_
