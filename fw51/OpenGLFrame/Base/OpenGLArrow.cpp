/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "OpenGLArrow.h"
#include "Conus.h"
#include "Line.h"
#include "dimple.h"
#include <qgl.h>
#include <vector>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
COpenGLArrow::COpenGLArrow(const geo::IPoint &base_point, const geo::IVector &vector,
                           const TOpenGLArrowShapeDef &shape /*= TOpenGLArrowShapeDef()*/) {
  m_pLine = 0;
  m_pConus = 0;
  Change(base_point, vector, shape);
}

COpenGLArrow::COpenGLArrow(const COpenGLArrow &rhs) : geo::IArray(rhs) {
  m_pLine = new geo::CConus(*rhs.m_pLine);
  m_pConus = new geo::CConus(*rhs.m_pConus);
}

void COpenGLArrow::Change(const geo::IPoint &base_point, const geo::IVector &vector,
                          const TOpenGLArrowShapeDef &shape /*= TOpenGLArrowShapeDef()*/) {

  double line_l = shape.HeadRatio * vector.Length();
  double arrow_l = (1.0 - shape.HeadRatio) * vector.Length();
  double arrow_rad = arrow_l * shape.RadiusRatio;
  double line_width = arrow_rad * shape.WidthRatio;

  if (!m_pLine)
    m_pLine = new geo::CConus(base_point, vector, line_width, line_l, 1.0);
  else
    m_pLine->Change(base_point, vector, line_width, line_l, 1.0);

  if (!m_pConus)
    m_pConus = new geo::CConus(base_point + vector * shape.HeadRatio, vector, arrow_rad, arrow_l, 0.0);
  else
    m_pConus->Change(base_point + vector * shape.HeadRatio, vector, arrow_rad, arrow_l, 0.0);
}

COpenGLArrow::~COpenGLArrow() {
  delete m_pLine;
  delete m_pConus;
}

const geo::CVector COpenGLArrow::Dir() const { return m_pLine->Direction(); }

const geo::IObject &COpenGLArrow::ObjectBase(const size_t nIndex) const {
  assert(nIndex < 3);
  if (nIndex == 0)
    return *m_pLine;
  return *m_pConus;
}

geo::IObject &COpenGLArrow::ObjectBase(const size_t nIndex) {
  assert(nIndex < 3);
  if (nIndex == 0)
    return *m_pLine;
  return *m_pConus;
}

void COpenGLArrow::Remove(const int /*nIndex*/) { assert(false); }

size_t COpenGLArrow::Size() const { return 2; }

void COpenGLArrow::Clear() {
  assert(false); // Cannot destroy arrow
}
