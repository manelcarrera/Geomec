/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "ILine.h"
#include "Point.h"
#include "Vector.h"
#include "dimple.h"
#include <cmath>

namespace geo {

/* static */
ftn_double_t IObject::m_compareTolerance = EPS;

/*!
  Returns the tolerance used for floating point comparison in operators like
  ==, < and >.
  Default value is EPS.
  \sa setCompareTolerance().
*/
ftn_double_t IObject::compareTolerance() { return m_compareTolerance; }

/*!
  Changes the tolerance used for floating point comparison in operators like
  ==, < and >.
  Default value is EPS.
  Returns the previous value to reset the tolerance afterwards.
  \sa compareTolerance().
*/
ftn_double_t IObject::setCompareTolerance(ftn_double_t tolerance) {
  ftn_double_t prevTol = m_compareTolerance;
  m_compareTolerance = tolerance;
  return prevTol;
}

// checks whether the bounding boxes intersect
// will return bIncludeEdges if the boxes only "touch"
bool IObject::BoundingBoxIntersect(const geo::IObject &rhs, bool bIncludeEdges) const {
  if (Empty() || rhs.Empty())
    return false;

  CPoint ptMinThis = Min();
  CPoint ptMaxThis = Max();

  CPoint ptMinRhs = rhs.Min();
  CPoint ptMaxRhs = rhs.Max();

  bool bXEdgeTouch = false;
  bool bYEdgeTouch = false;
  bool bZEdgeTouch = false;

  if (bIncludeEdges) {
    bXEdgeTouch = fabs(ptMinThis.X() - ptMaxRhs.X()) < compareTolerance() ||
                  fabs(ptMaxThis.X() - ptMinRhs.X()) < compareTolerance();
    bYEdgeTouch = fabs(ptMinThis.Y() - ptMaxRhs.Y()) < compareTolerance() ||
                  fabs(ptMaxThis.Y() - ptMinRhs.Y()) < compareTolerance();
    bZEdgeTouch = fabs(ptMinThis.Z() - ptMaxRhs.Z()) < compareTolerance() ||
                  fabs(ptMaxThis.Z() - ptMinRhs.Z()) < compareTolerance();
  }

  bool bXOverlap = (ptMinThis.X() + compareTolerance() > ptMinRhs.X() && ptMinThis.X() < ptMaxRhs.X()) ||
                   (ptMaxThis.X() > ptMinRhs.X() && ptMaxThis.X() < ptMaxRhs.X() + compareTolerance());
  bool bYOverlap = (ptMinThis.Y() + compareTolerance() > ptMinRhs.Y() && ptMinThis.Y() < ptMaxRhs.Y()) ||
                   (ptMaxThis.Y() > ptMinRhs.Y() && ptMaxThis.Y() < ptMaxRhs.Y() + compareTolerance());
  bool bZOverlap = (ptMinThis.Z() + compareTolerance() > ptMinRhs.Z() && ptMinThis.Z() < ptMaxRhs.Z()) ||
                   (ptMaxThis.Z() > ptMinRhs.Z() && ptMaxThis.Z() < ptMaxRhs.Z() + compareTolerance());

  return ((bXOverlap && bYOverlap && bZOverlap) || bXEdgeTouch || bYEdgeTouch || bZEdgeTouch);
}

bool IObject::PointOutsideBoundingBox(const geo::IPoint &point) const {
  assert(!Empty());
  assert(!point.Empty());

  CPoint ptMinThis = Min();
  CPoint ptMaxThis = Max();

  if (point.X() + compareTolerance() < ptMinThis.X())
    return true;

  if (point.X() > ptMaxThis.X() + compareTolerance())
    return true;

  if (point.Y() + compareTolerance() < ptMinThis.Y())
    return true;

  if (point.Y() > ptMaxThis.Y() + compareTolerance())
    return true;

  if (point.Z() + compareTolerance() < ptMinThis.Z())
    return true;

  if (point.Z() > ptMaxThis.Z() + compareTolerance())
    return true;

  return false;
}

bool IObject::PointInsideBoundingBox(const geo::IPoint &point) const {

  if (PointOutsideBoundingBox(point))
    return false;

  // point is in ore on this box
  assert(!Empty());
  assert(!point.Empty());

  CPoint ptMinThis = Min();
  CPoint ptMaxThis = Max();

  if (point.X() > ptMinThis.X() + compareTolerance() && point.X() + compareTolerance() < ptMaxThis.X() &&
      point.Y() > ptMinThis.Y() + compareTolerance() && point.Y() + compareTolerance() < ptMaxThis.Y() &&
      point.Z() > ptMinThis.Z() + compareTolerance() && point.Z() + compareTolerance() < ptMaxThis.Z())
    return true;

  return false;
}

bool IObject::PointOnBoundaryOfBoundingBox(const geo::IPoint &point) const {
  if (PointOutsideBoundingBox(point))
    return false;

  if (PointInsideBoundingBox(point))
    return false;

  return true;
}

/*virtual*/ void IObject::Mirror(const geo::IPlane & /*plane*/) {
  assert(false); // not implemented
}

bool IObject::BoundingBoxOutside(const geo::IObject &rhs) const {

  assert(!Empty());
  assert(!rhs.Empty());

  CPoint ptMinThis = Min();
  CPoint ptMaxThis = Max();

  CPoint ptMinRhs = rhs.Min();
  CPoint ptMaxRhs = rhs.Max();

  if (ptMinRhs.X() > ptMaxThis.X() + compareTolerance())
    return true;

  if (ptMinRhs.Y() > ptMaxThis.Y() + compareTolerance())
    return true;

  if (ptMinRhs.Z() > ptMaxThis.Z() + compareTolerance())
    return true;

  if (ptMaxRhs.X() + compareTolerance() < ptMinThis.X())
    return true;

  if (ptMaxRhs.Y() + compareTolerance() < ptMinThis.Y())
    return true;

  if (ptMaxRhs.Z() + compareTolerance() < ptMinThis.Z())
    return true;

  return false;
}

bool IObject::BBRhsInside(const geo::IObject &rhs) const {

  if (BoundingBoxOutside(rhs))
    return false;

  CPoint ptMinThis = Min();
  CPoint ptMaxThis = Max();

  CPoint ptMinRhs = rhs.Min();
  CPoint ptMaxRhs = rhs.Max();

  if (!(ptMinRhs.X() > ptMinThis.X() + compareTolerance() && ptMaxRhs.X() + compareTolerance() < ptMaxThis.X()))
    return false;

  if (!(ptMinRhs.Y() > ptMinThis.Y() + compareTolerance() && ptMaxRhs.Y() + compareTolerance() < ptMaxThis.Y()))
    return false;

  if (!(ptMinRhs.Z() > ptMinThis.Z() + compareTolerance() && ptMaxRhs.Z() + compareTolerance() < ptMaxThis.Z()))
    return false;

  return true;
}

/*virtual*/ void IObject::RotateLine(const ILine &line, const double &dAngleDeg) {
  Move(CVector(line.First()));
  Rotate(line.Dir(), dAngleDeg);
  Move(CVector(-1 * line.First()));
}

bool IObject::IsElement() const { return false; }

bool IObject::IsInterfaceElement() const { return false; }

bool IObject::IsBody() const { return false; }

} // namespace geo
