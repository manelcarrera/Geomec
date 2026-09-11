/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "ObjectModifier.h"
#include "IOpenGLFrame.h"
#include "Line.h"
#include "dimple.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
// CPolygonModifierBase::CPolygon
///////////////////////////////////////////////////////////////////////////////////////////////////////
CPolygonModifierBase::CPolygon::CPolygon(CPolygonModifierBase &mouse_listener, TColor point_color, TColor line_color,
                                         TColor point_selected_color, bool bAutoClose)
    : m_ddPoint(point_color), m_ddLine(line_color), m_ddSelectedPoint(point_selected_color),
      m_mouse_listener(mouse_listener), m_nSelectedPoint(-1), m_bAutoClose(bAutoClose) {
  m_ddPoint.IsSelectable(TRUE);
  m_ddLine.IsSelectable(TRUE);
  m_ddSelectedPoint.IsSelectable(TRUE);
  m_ddPoint.PointSize(8);
  m_ddSelectedPoint.PointSize(8);
}

void CPolygonModifierBase::CPolygon::DrawScene() {
  // Draw the lines first
  int i;
  for (i = 0; i < LineSize(); i++)
    m_mouse_listener.Frame().DrawObject(Line(i), m_ddLine);

  // Draw Points
  for (i = 0; i < PointSize(); i++) {
    if (i == m_nSelectedPoint)
      m_mouse_listener.Frame().DrawObject(Point(i), m_ddSelectedPoint);
    else
      m_mouse_listener.Frame().DrawObject(Point(i), m_ddPoint);
  }
}

CPolygonModifierBase::CPolygon::~CPolygon() {}

bool CPolygonModifierBase::CPolygon::PushBack(const geo::IPoint &point) {
  // Then add the point and insert it in opengl and update the lines of
  // scene
  if (PointSize() > 0) {
    if ((LastPoint() == point) || (FirstPoint() == point))
      return false;
  }

  m_vcPoint.push_back(geo::CPoint(point));
  m_mouse_listener.Frame().UpdateFrame();
  return true;
}

const geo::IPoint &CPolygonModifierBase::CPolygon::FirstPoint() const {
  DIA_ASSERT(PointSize() > 0);
  return m_vcPoint[0];
}

const geo::IPoint &CPolygonModifierBase::CPolygon::LastPoint() const {
  DIA_ASSERT(PointSize() > 0);
  return m_vcPoint[m_vcPoint.size() - 1];
}

const geo::IPoint &CPolygonModifierBase::CPolygon::Previous(int nIndex) const {
  DIA_ASSERT(nIndex >= 0);
  DIA_ASSERT(nIndex < PointSize());
  DIA_ASSERT(PointSize() > 1);
  if (nIndex == 0)
    return LastPoint();

  return m_vcPoint[nIndex - 1];
}

const geo::IPoint &CPolygonModifierBase::CPolygon::Next(int nIndex) const {
  DIA_ASSERT(nIndex >= 0);
  DIA_ASSERT(nIndex < PointSize());
  DIA_ASSERT(PointSize() > 1);
  if (nIndex == (PointSize() - 1))
    return FirstPoint();

  return m_vcPoint[nIndex + 1];
}

int CPolygonModifierBase::CPolygon::PointSize() const { return (int)m_vcPoint.size(); }

const geo::IPoint &CPolygonModifierBase::CPolygon::Point(int nIndex) const {
  DIA_ASSERT(nIndex >= 0);
  DIA_ASSERT(nIndex < PointSize());
  return m_vcPoint[nIndex];
}

int CPolygonModifierBase::CPolygon::PointHitTest(const TScreenPoint &screen_point) const {
  std::vector<const geo::IObject *> vcSelection =
      ((IOpenGLFrame &)m_mouse_listener.Frame()).ProcessSelection(screen_point);
  for (int nPoint = 0; nPoint < PointSize(); nPoint++) {
    for (size_t i = 0; i < vcSelection.size(); i++) {
      if (&Point(nPoint) == vcSelection[i])
        return nPoint;
    }
  }

  return -1;
}

int CPolygonModifierBase::CPolygon::LineHitTest(const TScreenPoint &screen_point) const {
  std::vector<const geo::IObject *> vcSelection =
      ((IOpenGLFrame &)m_mouse_listener.Frame()).ProcessSelection(screen_point);
  for (int nLine = 0; nLine < LineSize(); nLine++) {
    for (size_t i = 0; i < vcSelection.size(); i++) {
      if (&Point(nLine) == vcSelection[i])
        return nLine;
    }
  }

  return -1;
}

void CPolygonModifierBase::CPolygon::PointSelection(int nIndex) {
  // Select new selection
  m_nSelectedPoint = nIndex;
  m_mouse_listener.Frame().UpdateFrame();
}

int CPolygonModifierBase::CPolygon::PointSelection() const { return m_nSelectedPoint; }

void CPolygonModifierBase::CPolygon::ReplacePoint(int nIndex, const geo::IPoint &point) {
  // Invalidate
  m_vcPoint[nIndex] = point;

  // Update the lines
  m_mouse_listener.Frame().UpdateFrame();
}

void CPolygonModifierBase::CPolygon::DeletePoint(int nIndex) {
  // Delete the point from vector
  m_vcPoint.erase(m_vcPoint.begin() + nIndex);

  // Update the lines
  m_mouse_listener.Frame().UpdateFrame();
}

int CPolygonModifierBase::CPolygon::LineSize() const {
  if (PointSize() > 1) {
    if (m_bAutoClose)
      return PointSize();
    return PointSize() - 1;
  }

  return 0;
}

geo::CLine CPolygonModifierBase::CPolygon::Line(int nIndex) const {
  if (nIndex == (PointSize() - 1))
    return geo::CLine(LastPoint(), FirstPoint());
  return geo::CLine(Point(nIndex), Point(nIndex + 1));
}

geo::CPolygon CPolygonModifierBase::CPolygon::Polygon() const {
  geo::CPolygon vcPolygon;

  for (int i = 0; i < PointSize(); i++)
    vcPolygon.PushBack(Point(i));

  return vcPolygon;
}

void CPolygonModifierBase::CPolygon::InsertPoint(int nLineIndex, const geo::IPoint &point) {
  // We insert after the line index ...

  // Create point and add to scene
  m_vcPoint.insert(m_vcPoint.begin() + nLineIndex, geo::CPoint(point));
  m_mouse_listener.Frame().UpdateFrame();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Polygon modifier base implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////
CPolygonModifierBase::CPolygonModifierBase(TFrame &frame, TColor point_color, TColor line_color,
                                           TColor point_selected_color, bool bAutoClose)
    : CMouseListener(frame) {
  // Create the polygon ...
  m_pPolygon = new CPolygon(*this, point_color, point_selected_color, line_color, bAutoClose);
}

CPolygonModifierBase::~CPolygonModifierBase() {
  delete m_pPolygon;
  Frame().UpdateFrame();
}

bool CPolygonModifierBase::ValidatePolygon(const geo::CPolygon &polygon) const {
  if (polygon.NrOfPoints() < 3)
    return false;

  if (polygon.IsSelfIntersecting())
    return false;

  // check against the constraints
  for (int i = 0; i < ConstraintSize(); i++) {
    int count;
    for (count = 0; count < polygon.NrOfPoints(); count++) {
      if (!Constraint(i).ValidatePoint(polygon.Point(count)))
        return false;
    }
    for (count = 0; count < polygon.NrOfLines(); count++) {
      if (!Constraint(i).ValidateLine(polygon.Line(count)))
        return false;
    }
  }

  return true;
}

geo::CPolygon CPolygonModifierBase::Polygon() const { return m_pPolygon->Polygon(); }

void CPolygonModifierBase::DrawScene() { m_pPolygon->DrawScene(); }

// validate a new candidate-point
bool CPolygonModifierBase::ValidatePoint(const geo::IPoint &point) const {
  if (point.Empty())
    return false;

  geo::CLine newline;

  // exclude all conflicts with previous created points
  if (m_pPolygon->PointSize()) {
    // no previous point allowed
    if (point == m_pPolygon->LastPoint())
      return false;

    // create a temporary new line from last point to the new point
    newline = geo::CLine(m_pPolygon->LastPoint(), point);

    if (m_pPolygon->PointSize() > 2) {
      // the first point is selected
      // to close the polygon
      if (point == m_pPolygon->FirstPoint()) {
        // there are no other constraints
        if (ConstraintSize() == 0)
          return true;
      }

      // is the new line crossing one of the previous created lines
      for (int i = 0; i < m_pPolygon->PointSize() - 2; i++) {
        geo::CLine l(m_pPolygon->Point(i), m_pPolygon->Point(i + 1));
        if (!newline.IsParallel(l)) {
          geo::CPoint p1 = newline.Intersection(l);
          if (!p1.Empty() && newline.Contains(p1) && l.Contains(p1))
            return false;
        }
      }

    } else {
      // 1 or 2 points
      if (m_pPolygon->PointSize() == 2) {
        if (point == m_pPolygon->FirstPoint())
          return false;
      }
    }
  }

  // exclude all conflicts with constraining polygons
  if (!CMouseListener::ValidatePoint(point))
    return false;

  if (!newline.Empty()) {
    for (int i = 0; i < ConstraintSize(); i++) {
      if (!Constraint(i).ValidateLine(newline))
        return false;
    }
  }

  // no conficts with previous points and
  // no conflicts with constraining polygons
  return true;
}
