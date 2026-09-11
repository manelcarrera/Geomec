/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "VectorTempl.h"
#include "dimple.h"

namespace geo {

IArray::IArray() : m_bDirty(true) {}

IArray::IArray(const IArray & /*rhs*/) : m_bDirty(true) {}

/*virtual*/ void IArray::Mirror(const geo::IPlane &plane) {
  for (size_t i = 0; i < Size(); i++) {
    ObjectBase(i).Mirror(plane);
  }

  m_bDirty = true;
}

void IArray::Rotate(const IVector &vec, const double &dAngleDeg) {
  for (size_t i = 0; i < Size(); i++) {
    ObjectBase(i).Rotate(vec, dAngleDeg);
  }

  m_bDirty = true;
}

void IArray::Transform(const IMatrix &matrix) {
  for (size_t i = 0; i < Size(); i++) {
    ObjectBase(i).Transform(matrix);
  }

  m_bDirty = true;
}

void IArray::Move(const IVector &vec) {
  for (size_t i = 0; i < Size(); i++) {
    ObjectBase(i).Move(vec);
  }

  m_bDirty = true;
}

void IArray::CalcMinMax() const {
  m_vtMin = CPoint();
  m_vtMax = CPoint();

  for (size_t i = 0; i < Size(); i++) {
    m_vtMin = m_vtMin.Min(ObjectBase(i).Min());
    m_vtMax = m_vtMax.Max(ObjectBase(i).Max());
  }

  m_bDirty = false;
}

CPoint IArray::Min() const {
  if (m_bDirty)
    CalcMinMax();
  return m_vtMin;
}

CPoint IArray::Max() const {
  if (m_bDirty)
    CalcMinMax();

  return m_vtMax;
}

bool IArray::Empty() const { return (Size() <= 0); }

void IArray::AssertValid() const { assert(!Empty()); }

} // namespace geo
