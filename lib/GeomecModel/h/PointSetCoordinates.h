#ifndef _PointSetCoordinates_h_
#define _PointSetCoordinates_h_

#include <vector>

#include <QString>

class CModelBase;
class CUnitNode;

class CPointSetCoordinates {
public:
  struct CCoordinate {
    int m_index;
    int m_density;
    double m_min;
    double m_max;
    QString m_name;
  };

  CPointSetCoordinates(const CModelBase &modelBase, const CUnitNode &unitNode, bool is2D);

  void initialize(bool is2D);

  std::vector<CCoordinate> &operator()();

private:
  CPointSetCoordinates(const CPointSetCoordinates &rhs);
  CPointSetCoordinates &operator=(CPointSetCoordinates rhs);

  static CCoordinate createCoordinate(unsigned int stringTableEntry, int index, double minimumValue,
                                      double maximumValue, const CUnitNode &unitNode);
  static std::vector<CCoordinate> createCoordinates(const CModelBase &modelBase, const CUnitNode &unitNode, bool is2D);

  const CModelBase &m_modelBase;
  const CUnitNode &m_unitNode;
  std::vector<CCoordinate> m_coordinates;
};

#endif // _PointSetCoordinates_h_
