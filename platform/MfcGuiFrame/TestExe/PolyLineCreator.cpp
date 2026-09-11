// PolyLineCreator.cpp: implementation of the CPolyLineCreator class.
//
//////////////////////////////////////////////////////////////////////

#include "PolyLineCreator.h"
#include "2DDocument.h"
#include "2DSegment.h"
#include "TestExe.h"
#include "drawdef.h"
#include "iopenglframe.h"
#include "stdafx.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPolyLineCreator::CPolyLineCreator(IOpenGLFrame &frame, C2DDocument &document)
    : CMouseListener(frame), m_document(document) {}

CPolyLineCreator::~CPolyLineCreator() { Frame().SetCursor(Qt::ArrowCursor); }

void CPolyLineCreator::DrawScene() {
  if (m_points.size() > 1) {
    for (int i = 0; i < m_points.size() - 1; i++) {
      if (m_points[i] != m_points[i + 1]) {
        CDrawDef p_drawdef(qRgb(255, 0, 0));
        p_drawdef.PointSize(5.0);
        CDrawDef l_drawdef(qRgb(200, 200, 200));
        Frame().DrawObject(geo::CLine(m_points[i], m_points[i + 1]), l_drawdef);
        Frame().DrawObject(m_points[i], p_drawdef);
      }
    }
  }
}

bool CPolyLineCreator::MouseRelease(TButtonState state, TButtonState button, const TScreenPoint &point) {
  if (button == Qt::LeftButton) {
    geo::CPoint w_point = Frame().UnProject(point);
    w_point.Z(0);
    m_points.push_back(w_point);
    if (m_points.size() == 1)
      m_points.push_back(w_point);
  }

  if (button == Qt::RightButton) {
    if (m_points.size() > 2) {
      m_points.resize(m_points.size() - 1);
      OK();
    } else
      Cancel();
    return true;
  }

  return true;
}

bool CPolyLineCreator::MouseMove(TButtonState state, TButtonState button, const TScreenPoint &point) {
  Frame().SetCursor(Qt::CrossCursor);
  if (m_points.size() > 1) {
    geo::CPoint w_point = Frame().UnProject(point);
    w_point.Z(0);
    m_points[m_points.size() - 1] = w_point;
    Frame().UpdateFrame();
  }

  return true;
}

bool CPolyLineCreator::OnOK() {
  COperation operation(m_document);
  C2DPolyLine *pPolyLine = new C2DPolyLine("PolyLine", m_document.PolyLines());
  for (int i = 0; i < m_points.size() - 1; i++) {
    // Create the first vertex
    assert(m_points[i] != m_points[i + 1]);
    C2DVertex *pFirst = m_document.Vertices().NearTo(m_points[i]);
    if (!pFirst)
      pFirst = new C2DVertex(m_points[i].X(), m_points[i].Y(), m_document.Vertices());

    // Look for intersections
    std::vector<C2DSegmentContainer::TIntersection> section =
        m_document.Segments().Intersection(geo::CLine(pFirst->Point(), m_points[i + 1]));
    for (int i = 0; i < section.size(); i++) {
      C2DSegmentContainer::TIntersection intersection = section[0];
      C2DVertex *pLast = m_document.Vertices().NearTo(intersection.second);
      if (!pLast)
        const_cast<C2DSegment *>(intersection.first)->Split(intersection.second);
      pLast = m_document.Vertices().NearTo(intersection.second);
      assert(pLast);
      if (pFirst != pLast) {
        C2DSegment *pSegment = new C2DSegment(m_document.Segments(), *pFirst, *pLast);
        pPolyLine->PolyLine().PushBack(*pSegment);
        pFirst = pLast;
      }
    }

    C2DVertex *pLast = m_document.Vertices().NearTo(m_points[i + 1]);
    if (pLast != pFirst) {
      if (!pLast)
        pLast = new C2DVertex(m_points[i + 1].X(), m_points[i + 1].Y(), m_document.Vertices());

      C2DSegment *pSegment = new C2DSegment(m_document.Segments(), *pFirst, *pLast);
      pPolyLine->PolyLine().PushBack(*pSegment);
    }
  }

  return true;
}