#if !defined(ELEMENT_CACHE_OBJECT_H_)
#define ELEMENT_CACHE_OBJECT_H_

#include <vector>
#include "IElement.h"
#include "IBody.h"
#include "IPoint.h"
#include "Matrix.h"

namespace geo {

class IBody;
class IPoint;
class CMatrix;


class GEOMETRY_EXPORT CElementCacheObject
{
  bool m_valid;
  TWeightingType m_type;
  std::vector<const IBody *> m_bodies;
  std::vector<IBody::TIntersection> m_intersections;
  std::vector<const IPoint *> m_points;
  std::vector<double> m_weights;
  CMatrix m_matrix;

public:
  CElementCacheObject();
  bool Valid();
  void Valid(bool valid);

  TWeightingType Type();
  void Type(TWeightingType type);

  void addBody(const IBody *body);
  const IBody *getBody(int i);
  std::size_t NrOfBodies();

  void addIntersection(const IBody::TIntersection intersection);
  const IBody::TIntersection *getIntersection(int i);
  bool IntersectionFound() const;

  void addSize(std::size_t size);

  void addPoint(const IPoint *point);
  void setPoints(std::vector<const IPoint *> points);
  std::vector<const IPoint *> *getPoints();
  const std::vector<const IPoint *> *getPoints() const;

  void addWeight(const double weight);
  void setWeights(std::vector<double> weights);
  std::vector<double> *getWeights();
  const std::vector<double> *getWeights() const;

  void setMatrix(const CMatrix &matrix);
  const CMatrix *getMatrix() const;

  void Clear();
};

}



#endif
