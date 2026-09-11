
#include "LasPointSet.h"
#include "NewWellPath.h"

CLasPointSet::CLasPointSet(CFemAppModel &model) : CPointSet(model), m_pNewWellPath(0) {
  pointSetType(IPointSet::WELLPATH);
}

CLasPointSet::CLasPointSet(const CNewWellPath &well, const QString &sName, int nValueSetSize, DIMENSION dim,
                           bool bCreateValueSetForCoordinates)
    : CPointSet(sName, const_cast<CFemAppModel &>(well.Model()), nValueSetSize, dim, bCreateValueSetForCoordinates,
                false) {
  pointSetType(IPointSet::WELLPATH);
  LinkTo(const_cast<CNewWellPath &>(well));
  assert(m_pNewWellPath == &well);
  CFemAppModel &model = const_cast<CFemAppModel &>(well.Model());
  reParent(model.GraphEntry(MD_BASE_POINTSET));
}

const CNewWellPath *CLasPointSet::NewWellPath() const { return m_pNewWellPath; }

// For loading old Wellpath data and then converting to NewWellPath
// wjrx mantis 3687
//
void CLasPointSet::NewWellPath(const CNewWellPath *p) {
  assert(p);
  m_pNewWellPath = p;
}

void CLasPointSet::OnNewNeighbour(const CGraphNode &node) {
  const CNewWellPath *pNewWellPath = dynamic_cast<const CNewWellPath *>(&node);
  if (pNewWellPath) {
    m_pNewWellPath = pNewWellPath;
  }
}

void CLasPointSet::OnNeighbourDeleted(const CGraphNode &item) {
  if (&item == m_pNewWellPath)
    m_pNewWellPath = 0; // release reference to the wellpath when it gets deleted

  CPointSet::OnNeighbourDeleted(item);
}
