/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "LineModifier.h"
#include "IOpenGLFrame.h"
#include "Line.h"
#include "dimple.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLineModifier::CLineModifier(TFrame &frame, const geo::IPoint &linepoint1, const geo::IPoint &linepoint2,
                             const geo::IPlane &plane)
    : CMouseListener(frame), m_OriginalLine(linepoint1, linepoint2), m_pModifiedLine(0), m_pOriginalPoint(0),
      m_pDragPoint(0), m_ddPoints(qRgb(0, 255, 0)), // green poly-points
      m_ddLine(qRgb(0, 255, 0)),                    // green poly-points
      m_ddSelectedPoints(qRgb(255, 0, 0)), m_Plane(plane) {
  m_ddPoints.PointSize(8);
  m_ddPoints.IsSelectable(true);
  m_ddSelectedPoints.PointSize(8);
  m_ddLine.LineStipple(TRUE);

  if (!m_OriginalLine.Empty())
    m_pModifiedLine = new CModifierLine(linepoint1, linepoint2);

  Frame().UpdateFrame();
}

CLineModifier::~CLineModifier() {
  if (m_pModifiedLine)
    delete m_pModifiedLine;
}

bool CLineModifier::Dragging() const { return (m_pDragPoint != 0); }

bool CLineModifier::OnOK() { return true; }

bool CLineModifier::MouseMove(TKeyboardModifiers /*state*/, TMouseButton button, const TScreenPoint &point) {
  // We have two cases.
  // 1	The user is not dragging.
  //		- Normal cursor when the line is edited.
  //		- Drawcursor when the line has to be created.
  // 2	The user is draggging
  //		- Update the dragpoint
  //		- Invalidate the displaylist of the points

  if (Dragging() && (button == Qt::LeftButton)) {
    // Calculate the the intersection point p.
    geo::CLine l(Frame().UnProject(point, 0), Frame().UnProject(point, 1));
    geo::CPoint ptIntersection = m_Plane.Intersection(l);
    assert(!ptIntersection.Empty());

    // update the (temporary) point we're dragging
    *m_pDragPoint = ptIntersection;

    Frame().UpdateFrame();

    // Set the cursor
    if (ValidateThisLine())
      Frame().SetCursor(Qt::ArrowCursor);
    else
      Frame().SetCursor(Qt::ForbiddenCursor);
  } else {
    // Give user feedback through mouse cursor for creation of first point
    if (!m_pModifiedLine) {
      // Calculate the the intersection point p and validate it ...
      geo::CLine l(Frame().UnProject(point, 0), Frame().UnProject(point, 1));
      if (ValidatePoint(m_Plane.Intersection(l)))
        Frame().SetCursor(Qt::CrossCursor);
      else
        Frame().SetCursor(Qt::ForbiddenCursor);
    }
  }

  return true;
}

bool CLineModifier::MouseDblClk(TKeyboardModifiers /*state*/, TMouseButton /*button*/, const TScreenPoint & /*point*/) {
  return true;
}

bool CLineModifier::MouseWheel(TKeyboardModifiers /*state*/, int /*nDelta*/, const TScreenPoint & /*point*/) {
  return true;
}
bool CLineModifier::MousePress(TKeyboardModifiers /*state*/, TMouseButton button, const TScreenPoint &point) {
  assert(!Dragging());
  if (button == Qt::LeftButton) {
    // If we're creating, we have no valid points ...
    if (m_OriginalLine.Empty()) {
      assert(m_pModifiedLine == 0);
      // Get the first point on the line we're drawing
      geo::CLine l(Frame().UnProject(point, 0), Frame().UnProject(point, 1));
      geo::CPoint ptIntersection = m_Plane.Intersection(l);
      assert(!ptIntersection.Empty());
      m_pModifiedLine = new CModifierLine(ptIntersection, ptIntersection);
      m_pDragPoint = const_cast<geo::IPoint *>(&m_pModifiedLine->Second());

      DIA_ASSERT(Dragging()); // We're dragging now!!!
      return true;
    }

    // Get the selection from the buffer
    std::vector<const geo::IObject *> vcSelection = Frame().ProcessSelection(point);
    assert(m_pModifiedLine != 0);

    // examine the selected objects
    for (size_t i = 0; i < vcSelection.size(); i++) {
      // If first point hit?
      if (&m_OriginalLine.First() == vcSelection[i]) {
        // Make first point the drag point
        m_pDragPoint = const_cast<geo::IPoint *>(&m_pModifiedLine->First());
        m_pOriginalPoint = const_cast<geo::IPoint *>(&m_OriginalLine.First());
        Frame().UpdateFrame();
        DIA_ASSERT(Dragging());
        return true;
      }

      // If second point hit?
      if (&m_OriginalLine.Second() == vcSelection[i]) {
        // Make first point the drag point
        m_pDragPoint = const_cast<geo::IPoint *>(&m_pModifiedLine->Second());
        m_pOriginalPoint = const_cast<geo::IPoint *>(&m_OriginalLine.Second());
        Frame().UpdateFrame();
        assert(Dragging());
        return true;
      }
    }

    // Nothing was hit???? Hmmm ... user is finished with this operation
    Frame().UpdateFrame();
    OK();
  }

  return true;
}

bool CLineModifier::MouseRelease(TKeyboardModifiers /*state*/, TMouseButton button, const TScreenPoint &point) {
  if (button == Qt::LeftButton) {
    if (Dragging()) {
      assert(m_pModifiedLine != 0);

      // Validate the line ...
      geo::CLine l(Frame().UnProject(point, 0), Frame().UnProject(point, 1));
      *m_pDragPoint = m_Plane.Intersection(l);

      if (ValidateThisLine()) {
        if (m_OriginalLine.Empty()) {
          // create original line
          m_OriginalLine.First(m_pModifiedLine->First());
          m_OriginalLine.Second(m_pModifiedLine->Second());

          // delete the temporary line
          delete m_pModifiedLine;
          m_pModifiedLine = 0;
        } else {
          // update the original point we are dragging
          // this updates the original line
          assert(m_pOriginalPoint);
          *m_pOriginalPoint = *m_pDragPoint;
        }

        // Reset dragging
        m_pOriginalPoint = 0;
        m_pDragPoint = 0;

        OK(); // Object destroyed
        return true;
      } else {
        // No validation ... Stop and exit when constructing and default to earlier position
        // when editing ....
        if (m_OriginalLine.Empty()) {
          // delete the temporary line
          delete m_pModifiedLine;
          m_pModifiedLine = 0;

          // Reset dragging
          m_pOriginalPoint = 0;
          m_pDragPoint = 0;

          Cancel();
          return true;
        }
      }

      DIA_ASSERT(!Dragging());

      Frame().UpdateFrame();
    }
  }

  return true;
}

bool CLineModifier::ValidateThisLine() const {
  if (Dragging()) {
    assert(m_pModifiedLine);
    return ((m_pModifiedLine->First() != m_pModifiedLine->Second()) && ValidateLine(*m_pModifiedLine));
  }

  // Validate the original line if not empty
  return (!m_OriginalLine.Empty() && ValidateLine(m_OriginalLine));
}

// called by the view
// do not call View().DrawScene() !!!! (recursion)
void CLineModifier::DrawScene() {
  if (Dragging()) {
    assert(m_pModifiedLine != 0);
    // draw the modified line
    Frame().DrawObject(*m_pDragPoint, m_ddSelectedPoints);
    Frame().DrawObject(m_pModifiedLine->First(), m_ddPoints);
    Frame().DrawObject(m_pModifiedLine->Second(), m_ddPoints);
    Frame().DrawObject(*m_pModifiedLine, m_ddLine);
  } else {
    if (!m_OriginalLine.Empty()) {
      // draw the original
      Frame().DrawObject(m_OriginalLine.First(), m_ddPoints);
      Frame().DrawObject(m_OriginalLine.Second(), m_ddPoints);
      Frame().DrawObject(m_OriginalLine, m_ddLine);
    }
  }
}

const geo::ILine &CLineModifier::GetLine() const {
  // only invoke after dragging operation
  assert(!Dragging());
  return m_OriginalLine;
}

//// CModifierLine
CLineModifier::CModifierLine::CModifierLine(const geo::IPoint &pt1, const geo::IPoint &pt2) : m_pt1(pt1), m_pt2(pt2) {}

CLineModifier::CModifierLine::~CModifierLine() {}

bool CLineModifier::CModifierLine::Empty() const { return (m_pt1.Empty() || m_pt2.Empty()); }

const geo::IPoint &CLineModifier::CModifierLine::Point(int nIndex) const {
  assert(nIndex >= 0 && nIndex < NrOfPoints());
  return ((nIndex == 0) ? m_pt1 : m_pt2);
}

void CLineModifier::CModifierLine::Point(int nIndex, const geo::IPoint &point) {
  assert(nIndex >= 0 && nIndex < NrOfPoints());
  ((nIndex == 0) ? m_pt1 : m_pt2) = point;
}
