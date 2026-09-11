/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "RectangleModifier.h"
#include "IOpenGLFrame.h"
#include "Line.h"
#include "dimple.h"

//////////////////////////////////////////////////////////////////////
// Rectangle dragger
//////////////////////////////////////////////////////////////////////
CRectangleDragger::CRectangleDragger(TFrame &frame, const geo::IPlane &plane) : CDragger(frame, plane) {}

CRectangleDragger::~CRectangleDragger() {}

void CRectangleDragger::DrawScene() {
  if (IsDragging()) {
    if ((BeginScreenPoint().x() == CurrentScreenPoint().x()) || (BeginScreenPoint().y() == CurrentScreenPoint().y()))
      return;

    CDrawDef dd(qRgb(180, 180, 180));
    dd.PointSize(10);
    dd.LineWidth(2);
    dd.LineStipple(TRUE);
    dd.PolyFillBack(FALSE);
    dd.PolyFillFront(FALSE);

    Frame().DrawObject(Rectangle(), dd);
  }
}

geo::CRectangle CRectangleDragger::Rectangle() const {
  assert(IsDragging());

  geo::CPoint p1 = Frame().UnProject(BeginScreenPoint(), 0.5);
  geo::CPoint p2 = Frame().UnProject(TScreenPoint(BeginScreenPoint().x(), CurrentScreenPoint().y()), 0.5);
  geo::CPoint p3 = Frame().UnProject(TScreenPoint(CurrentScreenPoint().x(), BeginScreenPoint().y()), 0.5);

  if (!Plane().Empty()) {
    geo::CLine l1(Frame().UnProject(BeginScreenPoint(), 0), Frame().UnProject(BeginScreenPoint(), 1));
    p1 = Plane().Intersection(l1);
    geo::CLine l2(Frame().UnProject(TScreenPoint(BeginScreenPoint().x(), CurrentScreenPoint().y()), 0),
                  Frame().UnProject(TScreenPoint(BeginScreenPoint().x(), CurrentScreenPoint().y()), 1));
    p2 = Plane().Intersection(l2);
    geo::CLine l3(Frame().UnProject(TScreenPoint(CurrentScreenPoint().x(), BeginScreenPoint().y()), 0),
                  Frame().UnProject(TScreenPoint(CurrentScreenPoint().x(), BeginScreenPoint().y()), 1));
    p3 = Plane().Intersection(l3);
  }

  if (p1.Empty() || p2.Empty() || p3.Empty())
    return geo::CRectangle();

  return geo::CRectangle(p1, geo::CVector(p2 - p1), geo::CVector(p3 - p1));
}

bool CRectangleDragger::OnCancel() {
  bool bResult = CDragger::OnCancel();
  Frame().UpdateFrame();
  return bResult;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRectangleModifier::CRectangleModifier(TFrame &frame, geo::CRectangle &Rectangle)
    : CMouseListener(frame), m_Rectangle(Rectangle), m_nSelectedCornerPoint(-1),
      m_ddPoints(qRgb(0, 255, 0)),    // green poly-points
      m_ddRectangle(qRgb(0, 255, 0)), // green poly-points
      m_Plane(Rectangle.Plane()) {
  // Poinst size for rectangle = 8
  m_ddPoints.PointSize(8);
  m_ddPoints.IsSelectable(TRUE);
  m_vcPoint.resize(4);
  UpdatePoints();
}

bool CRectangleModifier::Dragging() const { return !m_DragPoint.Empty(); }

CRectangleModifier::~CRectangleModifier() {}

bool CRectangleModifier::OnOK() { return true; }

geo::CRectangle CRectangleModifier::GetModifiedRectangle() { return m_Rectangle; }

// Pre	: Given a screenpoint
// Post	: Return value
//			Point	0	// No point found
//			Point		// Selected point
int CRectangleModifier::PointHitTest(const TScreenPoint &point) {
  // Get selection ...
  std::vector<const geo::IObject *> vcSelection = Frame().ProcessSelection(point);

  // Walk over rectangle points
  for (int i = 0; i < m_vcPoint.size(); i++) {
    for (size_t j = 0; j < vcSelection.size(); j++) {
      if (m_vcPoint[i] == vcSelection[j])
        return i;
    }
  }

  return -1;
}

bool CRectangleModifier::MouseMove(TKeyboardModifiers /*state*/, TMouseButton /*button*/, const TScreenPoint &point) {
  if (Dragging()) {
    geo::CLine l(Frame().UnProject(point, 0), Frame().UnProject(point, 1));
    m_DragPoint = m_Plane.Intersection(l);
    if (!(m_BasePoint == m_DragPoint)) {
      m_Rectangle = OnDragRectangle(m_BasePoint, m_DragPoint);
      UpdatePoints();
    }
    Frame().UpdateFrame();
  } else {
    m_nSelectedCornerPoint = PointHitTest(point);

    Frame().UpdateFrame();
  }

  return TRUE;
}

geo::CRectangle CRectangleModifier::OnDragRectangle(const geo::IPoint &begin_point, const geo::IPoint &drag_point) {
  return geo::CRectangle(begin_point, drag_point);
}

void CRectangleModifier::UpdatePoints() {
  DIA_ASSERT(m_vcPoint.size() == m_Rectangle.NrOfPoints());

  // First invalidate
  for (int i = 0; i < m_vcPoint.size(); i++)
    m_vcPoint[i] = &m_Rectangle.Point(i);
}

bool CRectangleModifier::MouseDblClk(TKeyboardModifiers /*state*/, TMouseButton /*button*/,
                                     const TScreenPoint & /*point*/) {
  return true;
}

bool CRectangleModifier::MouseWheel(TKeyboardModifiers /*state*/, int /*nDelta*/, const TScreenPoint & /*point*/) {
  return true;
}

bool CRectangleModifier::MousePress(TKeyboardModifiers /*state*/, TMouseButton button, const TScreenPoint &point) {
  if (button == Qt::LeftButton) {
    m_nSelectedCornerPoint = PointHitTest(point);
    if (m_nSelectedCornerPoint >= 0) {
      m_DragPoint = *m_vcPoint[m_nSelectedCornerPoint];

      int index = m_Rectangle.GetOpositeIndex(m_DragPoint);
      assert(index >= 0 && index <= 3);
      m_BasePoint = m_Rectangle.Point(index);
    } else
      OK();
  }

  return true;
}

bool CRectangleModifier::MouseRelease(TKeyboardModifiers /*state*/, TMouseButton button,
                                      const TScreenPoint & /*point*/) {
  if (button == Qt::LeftButton) {
    if (Dragging()) {
      // We're draging a now we have to validate
      if (!(m_BasePoint == m_DragPoint))
        m_Rectangle = OnDragRectangle(m_BasePoint, m_DragPoint);

      // Update all points with the new rectangle
      UpdatePoints();

      // Reset the dragging mode
      m_DragPoint = geo::CPoint();
      m_BasePoint = geo::CPoint();
      m_nSelectedCornerPoint = -1;

      Frame().UpdateFrame();
      OnRectangleModified();
    }
  }

  return TRUE;
}

// called by the view
// do not call Frame().DrawScene() !!!! (recursion)
void CRectangleModifier::DrawScene() {
  // make sure the following drawing code is always visible
  //	GLint iOldDeptFunc = Frame().SetGLDepthFunc(GL_ALWAYS);
  CDrawDef ddPoint(qRgb(255, 0, 0));
  CDrawDef ddPoint2(qRgb(0, 0, 255));
  CDrawDef ddRect(qRgb(0, 0, 255));

  ddPoint.IsSelectable(TRUE);
  ddPoint2.IsSelectable(TRUE);
  ddRect.IsSelectable(TRUE);

  ddRect.PolyFillBack(FALSE);
  ddRect.PolyFillFront(FALSE);

  ddPoint.PointSize(8);
  ddPoint2.PointSize(8);

  Frame().DrawObject(m_Rectangle, ddRect);

  for (int i = 0; i < m_vcPoint.size(); i++) {
    if (m_nSelectedCornerPoint == i)
      Frame().DrawObject(*m_vcPoint[i], ddPoint);
    else
      Frame().DrawObject(*m_vcPoint[i], m_ddPoints);
  }

  // reset depth function
  //	Frame().SetGLDepthFunc(iOldDeptFunc);
}
