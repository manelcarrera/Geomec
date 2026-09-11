#ifndef _CreatePointSet_h_
#define _CreatePointSet_h_

#include <QString>

class CPointSet;
class CPointSetCoordinates;
class CModelBase;
class CUnitNode;

namespace geo {

class CPoint;

} // namespace geo

class CCreatePointSet {
public:
  CCreatePointSet(CPointSetCoordinates &pointSetCoordinates);

  CPointSet *create(CModelBase &modelBase, const CUnitNode &unitNode, const QString &name) const;

private:
  CCreatePointSet(const CCreatePointSet &rhs);
  CCreatePointSet &operator=(CCreatePointSet rhs);

  void createPointSetRecursive(size_t index, geo::CPoint &point, CPointSet *pointSet) const;

  CPointSetCoordinates &m_pointSetCoordinates;
};

#endif // _CreatePointSet_h_
