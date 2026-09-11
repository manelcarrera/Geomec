#include "ElementCacheObject.h"

namespace geo {

CElementCacheObject::CElementCacheObject() : m_valid(false) {}

bool CElementCacheObject::Valid() { return m_valid; }

void CElementCacheObject::Valid(bool valid) { m_valid = valid; }

TWeightingType CElementCacheObject::Type() { return m_type; }

void CElementCacheObject::Type(TWeightingType type) { m_type = type; }

void CElementCacheObject::addBody(const IBody *body) { m_bodies.push_back(body); }

const IBody *CElementCacheObject::getBody(int i) { return m_bodies[i]; }

std::size_t CElementCacheObject::NrOfBodies() { return m_bodies.size(); }

void CElementCacheObject::addIntersection(const IBody::TIntersection intersection) {
  m_intersections.push_back(intersection);
}

const IBody::TIntersection *CElementCacheObject::getIntersection(int i) {
  return const_cast<IBody::TIntersection *>(&m_intersections[i]);
}

bool CElementCacheObject::IntersectionFound() const { return m_points.size() > 0; }

void CElementCacheObject::addSize(std::size_t size) {
  m_points.reserve(m_points.size() + size);
  m_weights.reserve(m_weights.size() + size);
}

void CElementCacheObject::addPoint(const IPoint *point) { m_points.push_back(point); }

void CElementCacheObject::setPoints(std::vector<const IPoint *> points) { m_points = points; }

std::vector<const IPoint *> *CElementCacheObject::getPoints() { return &m_points; }

const std::vector<const IPoint *> *CElementCacheObject::getPoints() const {
  return const_cast<std::vector<const IPoint *> *>(&m_points);
}

void CElementCacheObject::addWeight(const double weight) { m_weights.push_back(weight); }

void CElementCacheObject::setWeights(std::vector<double> weights) { m_weights = weights; }

std::vector<double> *CElementCacheObject::getWeights() { return &m_weights; }

const std::vector<double> *CElementCacheObject::getWeights() const {
  return const_cast<std::vector<double> *>(&m_weights);
}

void CElementCacheObject::setMatrix(const CMatrix &matrix) { m_matrix = matrix; }

const CMatrix *CElementCacheObject::getMatrix() const { return const_cast<CMatrix *>(&m_matrix); }

void CElementCacheObject::Clear() {
  m_bodies.clear();
  m_intersections.clear();
  m_points.clear();
  m_weights.clear();
  m_matrix = CMatrix();
}

} // namespace geo
