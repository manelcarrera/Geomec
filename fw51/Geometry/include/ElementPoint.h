/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef _ELEMENTPOINT_H_
#define _ELEMENTPOINT_H_

#include "Point.h"
#include <set>

#include "GeometryExports.h"

namespace geo {

class IElement;

class GEOMETRY_EXPORT CElementPoint : public CPoint {
public:
  CElementPoint();
  CElementPoint(const IElement &element);
  CElementPoint(const std::set<const IElement *> &stElements);
  CElementPoint(const IElement &element, const double &dX, const double &dY, const double &dZ = 0.0);
  CElementPoint(const std::set<const IElement *> &stElements, const double &dX, const double &dY,
                const double &dZ = 0.0);
  CElementPoint(const IPoint &pt);
  CElementPoint(const IElement &element, const IPoint &pt);
  CElementPoint(const std::set<const IElement *> &stElements, const IPoint &pt);
  CElementPoint(const CElementPoint &pt);

  CElementPoint &operator=(const CElementPoint &rhs);

  const std::set<const IElement *> &Elements() const;
  bool AddElement(const IElement &element);
  bool RemoveElement(const IElement &element);

private:
  std::set<const IElement *> m_stElements;
};

} // namespace geo

#endif // _ELEMENTPOINT_H_
