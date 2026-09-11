/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// PolyLineCreator.cpp: implementation of the CPolyLineCreator class.
//
//////////////////////////////////////////////////////////////////////
#include "PolyLineCreator.h"
#include "IOpenGLFrame.h"
#include "Line.h"
#include "PolyLine.h"
#include "dimple.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPolyLineCreator::CPolyLineCreator(TFrame &frame, const geo::IPlane &plane)
    : CMouseListener(frame), m_ddCreatedLine(qRgb(150, 150, 150)), m_ddDragLine(qRgb(255, 0, 0)),
      m_ddCreatedPoint(qRgb(255, 0, 0)), m_Plane(plane) {
  Frame().SetCursor(Qt::CrossCursor);

  m_ddDragLine.LineStipple(true);
  m_ddDragLine.LineWidth(2);
  m_ddCreatedLine.LineWidth(2);
  m_ddCreatedPoint.PointSize(8);

  assert(m_BeginPoint.Empty() && m_DragPoint.Empty());
}

CPolyLineCreator::~CPolyLineCreator() {}

bool CPolyLineCreator::MousePress(TKeyboardModifiers /*state*/, TMouseButton /*button*/,
                                  const TScreenPoint & /*point*/) {
  return true;
}

bool CPolyLineCreator::MouseRelease(TKeyboardModifiers /*state*/, TMouseButton button, const TScreenPoint &point) {
  if (button == Qt::LeftButton) {
    // Try to insert a point
    geo::CLine l(Frame().UnProject(point, 0), Frame().UnProject(point, 1));
    DIA_ASSERT(!l.Empty());
    DIA_ASSERT(!l.Second().Empty());
    DIA_ASSERT(!l.First().Empty());
    m_DragPoint = m_Plane.Intersection(l);

    m_DragPoint = OnSetDragPoint(m_DragPoint);

    if (!m_BeginPoint.Empty()) {
      if (ValidateLine(geo::CLine(m_BeginPoint, m_DragPoint))) {
        AddLine(m_BeginPoint, m_DragPoint);
        AddPoint(m_DragPoint);
        m_BeginPoint = m_DragPoint;
      }
    } else {
      // Add first point
      if (ValidatePoint(m_DragPoint)) {
        AddPoint(m_DragPoint);
        m_BeginPoint = m_DragPoint;
      }
    }
  }

  if (button == Qt::RightButton) {
    m_BeginPoint = geo::CPoint();
    Frame().UpdateFrame();
    if (PolyLine().LineSize() > 0)
      OK();
    else
      Cancel();
  }

  return true;
}

bool CPolyLineCreator::MouseMove(TKeyboardModifiers /*state*/, TMouseButton /*button*/, const TScreenPoint &point) {
  // There is a point selected and we're dragging so calculate new point
  geo::CLine l(Frame().UnProject(point, 0), Frame().UnProject(point, 1));
  m_DragPoint = m_Plane.Intersection(l);
  m_DragPoint = OnSetDragPoint(m_DragPoint);
  bool bValid = false;

  if (!m_BeginPoint.Empty()) {
    Frame().UpdateFrame();
    // don't create a line when first == second
    if (!(m_BeginPoint == m_DragPoint))
      bValid = ValidateLine(geo::CLine(m_BeginPoint, m_DragPoint));
  } else
    bValid = ValidatePoint(m_DragPoint);

  OnSetCursor(m_DragPoint, bValid);

  return true;
}

void CPolyLineCreator::DrawScene() {
  if (!m_BeginPoint.Empty()) {
    // Draw created lines and points
    for (int nLine = 0; nLine < PolyLine().LineSize(); nLine++)
      Frame().DrawObject(PolyLine().Line(nLine), CreatedLineDrawDef());
    for (int nPoint = 0; nPoint < PolyLine().PointSize(); nPoint++)
      Frame().DrawObject(PolyLine().Point(nPoint), CreatedPointDrawDef());
    if (m_BeginPoint != m_DragPoint)
      Frame().DrawObject(geo::CLine(m_BeginPoint, m_DragPoint), DragLineDrawDef());
  }
}

IDrawDef &CPolyLineCreator::CreatedLineDrawDef() { return m_ddCreatedLine; }

IDrawDef &CPolyLineCreator::DragLineDrawDef() { return m_ddDragLine; }

IDrawDef &CPolyLineCreator::CreatedPointDrawDef() { return m_ddCreatedPoint; }

void CPolyLineCreator::AddLine(const geo::IPoint & /*first*/, const geo::IPoint & /*second*/) {}

void CPolyLineCreator::AddPoint(const geo::IPoint & /*point*/) {}
