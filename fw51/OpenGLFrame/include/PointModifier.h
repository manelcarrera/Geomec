/* Confidential Source Code Copyright (c) 2011 TNO DIANA BV                              Confidential */
// PolygonCreator.h: interface for the CPolygonCreator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POINTMODIFIER_H__FC95697E_A9AA_4E22_81B9_57DE9C93FD73__INCLUDED_)
#define AFX_POINTMODIFIER_H__FC95697E_A9AA_4E22_81B9_57DE9C93FD73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DrawDef.h"
#include "MouseListener.h"
#include "Plane.h"

#include "OpenGLFrameExports.h"

class OPENGLFRAME_EXPORT CPointModifier : public CMouseListener {
  // The current non-validated point
  geo::CPoint m_point;
  // Dragging position during dragging. Empty when not dragging
  geo::CPoint m_drag_point;
  // remark:
  // the plane to create the point on.
  // the constraining polygons should also be in this plane.
  geo::CPlane m_Plane;
  // Drawdef for selection
  CDrawDef m_ddSelection;
  CDrawDef m_ddDragging;

protected:
  // Current position
  geo::CPoint CurrentPosition(const TScreenPoint &point) const;

  // validate a new candidate-point
  // is virtual because overrides can extend or reduce restrictions for validation
  virtual bool ValidatePoint(const geo::IPoint &point) const;
  void Point(const geo::IPoint &point);
  bool Dragging() const;

public:
  CPointModifier(TFrame &frame, const geo::IPoint &point,
                 const geo::IPlane & = geo::CPlane::PlaneXY); // see remark by m_Plane

  virtual ~CPointModifier();

  // CMouseListener overides called by the view
  virtual bool MousePress(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);
  virtual bool MouseRelease(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);
  virtual bool MouseDblClk(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);
  virtual bool MouseWheel(TKeyboardModifiers state, int nDelta, const TScreenPoint &point);
  virtual bool MouseMove(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);
  virtual void DrawScene();

  // called by the view after we have said
  // we are finished successfully: m_pView->OnFinishedMouseListener(),and ;
  const geo::IPoint &Point() const;
};

#endif // !defined(AFX_POLYGONCREATOR_H__FC95697E_A9AA_4E22_81B9_57DE9C93FD73__INCLUDED_)
