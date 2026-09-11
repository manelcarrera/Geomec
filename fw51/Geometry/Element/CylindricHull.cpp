#include "CylindricHull.h"

namespace geo {
const double EPSILON = 0.1;

CCylindricHull::Vector::Vector() : x(0), y(0), z(0) {}

CCylindricHull::Vector::Vector(const geo::IPoint &point) : x(point.X()), y(point.Y()), z(point.Z()) {}

CCylindricHull::Vector::Vector(const geo::IPoint &point, const Vector &origin)
    : x(point.X() - origin.x), y(point.Y() - origin.y), z(point.Z() - origin.z) {}

static inline double length2(double x, double y, double z) { return x * x + y * y + z * z; }

// CCylindricHull

CCylindricHull::CCylindricHull(const IPointSet &points, double radius)
    : m_radius(radius), m_radiusSquared(radius * radius) {
  assert(radius >= 0);

  m_origin.reserve(points.size());
  if (points.size() >= 1) {
    m_vector.reserve(points.size() - 1);
    m_length2.reserve(points.size() - 1);

    for (size_t i = 0; i < points.size() - 1; ++i) {
      m_origin.push_back(Vector(*points[i]));
      m_vector.push_back(Vector(*points[i + 1], m_origin[i]));
      m_length2.push_back(length2(m_vector[i].x, m_vector[i].y, m_vector[i].z));
    }
    m_origin.push_back(Vector(*points.back()));
  }

  CalcBB();
}

CCylindricHull::~CCylindricHull() {}

void CCylindricHull::CalcBB() {
  m_min.x = m_min.y = m_min.z = DBL_MAX;
  m_max.x = m_max.y = m_max.z = -DBL_MAX;

  for (size_t i = 0; i < m_origin.size(); ++i) {
    if (m_origin[i].x < m_min.x)
      m_min.x = m_origin[i].x;
    if (m_origin[i].y < m_min.y)
      m_min.y = m_origin[i].y;
    if (m_origin[i].z < m_min.z)
      m_min.z = m_origin[i].z;

    if (m_origin[i].x > m_max.x)
      m_max.x = m_origin[i].x;
    if (m_origin[i].y > m_max.y)
      m_max.y = m_origin[i].y;
    if (m_origin[i].z > m_max.z)
      m_max.z = m_origin[i].z;
  }

  m_min.x -= m_radius + EPSILON;
  m_min.y -= m_radius + EPSILON;
  m_min.z -= m_radius + EPSILON;

  m_max.x += m_radius + EPSILON;
  m_max.y += m_radius + EPSILON;
  m_max.z += m_radius + EPSILON;
}

double CCylindricHull::Radius() const { return m_radius; }

void CCylindricHull::Radius(double radius) {
  assert(radius >= 0);
  m_radius = radius;
  m_radiusSquared = radius * radius;

  CalcBB();
}

#define checkDistance(l)                                                                                               \
  if (l <= m_radiusSquared) {                                                                                          \
    if (includeEdge || l < m_radiusSquared)                                                                            \
      return true;                                                                                                     \
  }

bool CCylindricHull::Contains(const IPoint &point, bool includeEdge) const {
  double px = point.X(), py = point.Y(), pz = point.Z(), x, y, z, dot, len2, dis2;

  if (px < m_min.x || px > m_max.x)
    return false;
  if (py < m_min.y || py > m_max.y)
    return false;
  if (pz < m_min.z || pz > m_max.z)
    return false;

  for (size_t i = 0; i < m_vector.size(); ++i) {
    x = px - m_origin[i].x;
    y = py - m_origin[i].y;
    z = pz - m_origin[i].z;

    dot = x * m_vector[i].x + y * m_vector[i].y + z * m_vector[i].z;
    len2 = length2(x, y, z);

    // circle
    checkDistance(len2)

        // cylinder
        if (dot >= 0 && dot <= m_length2[i]) {
      dis2 = len2 - dot * dot / m_length2[i]; // Pythagoras

      checkDistance(dis2)
    }
  }

  if (m_origin.size() > 0) // check last circle
  {
    x = px - m_origin.back().x;
    y = py - m_origin.back().y;
    z = pz - m_origin.back().z;

    len2 = length2(x, y, z);

    checkDistance(len2)
  }

  return false;
}

} // namespace geo
