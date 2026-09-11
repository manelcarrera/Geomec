#ifndef _CylindricHull_h_
#define _CylindricHull_h_

#include "IPoint.h"
#include "VectorTempl.h"

namespace geo {

class GEOMETRY_EXPORT CCylindricHull {
  struct Vector {
    double x;
    double y;
    double z;

    Vector();
    Vector(const geo::IPoint &point);
    Vector(const geo::IPoint &point, const Vector &origin);
  };

  // const double EPSILON = 0.1;

public:
  typedef std::vector<const IPoint *> IPointSet;

  CCylindricHull(const IPointSet &points, double radius);
  virtual ~CCylindricHull();

  virtual bool Contains(const IPoint &point, bool includeEdge) const;

  double Radius() const;
  void Radius(double radius);

private:
  CCylindricHull(const CCylindricHull &rhs);
  CCylindricHull &operator=(CCylindricHull rhs);

  double m_radius;
  double m_radiusSquared;

  Vector m_min;
  Vector m_max;

  void CalcBB();

  std::vector<Vector> m_origin;
  std::vector<Vector> m_vector;
  std::vector<double> m_length2;
};

} // namespace geo

#endif // _CylindricHull_h_
