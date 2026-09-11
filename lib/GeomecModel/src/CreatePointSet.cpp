#include "CreatePointSet.h"
#include "LengthQuantity.h"
#include "ModelBase.h"
#include "PointSet.h"
#include "PointSetCoordinates.h"
#include "unitnode.h"

CCreatePointSet::CCreatePointSet(CPointSetCoordinates &pointSetCoordinates)
    : m_pointSetCoordinates(pointSetCoordinates) {}

CPointSet *CCreatePointSet::create(CModelBase &modelBase, const CUnitNode &unitNode, const QString &name) const {
  CPointSet *pointSet = 0;

  switch (m_pointSetCoordinates().size()) {
  case 1:
    pointSet = new CPointSet(name, modelBase, 0, CPointSet::DIM_1D, true);
    break;
  case 2:
    pointSet = new CPointSet(name, modelBase, 0, CPointSet::DIM_2D, true);
    break;
  case 3:
    pointSet = new CPointSet(name, modelBase, 0, CPointSet::DIM_3D, true);
    break;
  default:
    assert(false);
    break;
  }

  // Convert back from field to si

  CLengthQuantity lengthQuantity;

  for (size_t i = 0; i < m_pointSetCoordinates().size(); i++) {
    m_pointSetCoordinates()[i].m_min =
        lengthQuantity.Convert(m_pointSetCoordinates()[i].m_min, CQuantity::SI_UNIT, unitNode.Unit());
    m_pointSetCoordinates()[i].m_max =
        lengthQuantity.Convert(m_pointSetCoordinates()[i].m_max, CQuantity::SI_UNIT, unitNode.Unit());
  }

  geo::CPoint point(0, 0, 0);

  createPointSetRecursive(0, point, pointSet);

  return pointSet;
}

// private

void CCreatePointSet::createPointSetRecursive(size_t index, geo::CPoint &point, CPointSet *pointSet) const {
  assert(index < m_pointSetCoordinates().size());

  double minimum = m_pointSetCoordinates()[index].m_min;
  double maximum = m_pointSetCoordinates()[index].m_max;
  int density = m_pointSetCoordinates()[index].m_density;
  double delta = (maximum - minimum) / density;

  // wjrx mantis 0002917

  if (fabs(delta) < EPS) {
    density = 0; // a single point
  }

  double value;
  int i;

  for (value = minimum, i = 0; i <= density; value += delta, i++) {
    switch (m_pointSetCoordinates()[index].m_index) {
    case 0:
      point.X(value);
      break;
    case 1:
      point.Y(value);
      break;
    case 2:
      point.Z(value);
      break;
    default:
      assert(false);
      break;
    }

    if ((index + 1) == m_pointSetCoordinates().size()) {
      pointSet->PushBack(point, std::vector<double>());
    } else {
      createPointSetRecursive(index + 1, point, pointSet);
    }
  }
}
