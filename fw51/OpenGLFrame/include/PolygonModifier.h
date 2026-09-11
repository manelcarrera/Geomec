/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef _POLYGONMODIFIER_H_
#define _POLYGONMODIFIER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ObjectModifier.h"
#include "Plane.h"

#include "OpenGLFrameExports.h"

class OPENGLFRAME_EXPORT CPolygonModifier : public CPolygonModifierBase {
  CDrawDef m_ddDragLine;   // Draw def to point the drag lines
  geo::CPlane m_Plane;     // The plane to create the polygon on.
                           // The constraining polygons should also be in this plane.
  geo::CPoint m_DragPoint; // Point we currently dragging if Empty we're not dragging
protected:
  // Routine validate the current polygon ...
  bool ValidateThisPolygon();

  // Dragging flag
  bool Dragging() const { return !m_DragPoint.Empty(); }

public:
  CPolygonModifier(TFrame &frame,
                   const geo::IPolygon &polygon,                // the polygon which we are modifying
                   const geo::IPlane & = geo::CPlane::PlaneXY); // see remark by m_Plane
  virtual ~CPolygonModifier();

  // CMouseListener overides called by the view
  virtual bool MousePress(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);
  virtual bool MouseRelease(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);
  virtual bool MouseDblClk(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);
  virtual bool MouseWheel(TKeyboardModifiers state, int nDelta, const TScreenPoint &point);
  virtual bool MouseMove(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);
  virtual void DrawScene();

  virtual bool KeyPress(TKeyboardModifiers state, int nAscii, TKey key, int nRepCount);
  // Function deletes the selected point
  bool CanDeleteSelectedPoint() const;
  void DeleteSelectedPoint();

  // This function is called when the user wants to insert a point. The point is already set by the variable
  // m_ptInsertPoint
  bool CanInsertPointOnCurrentLine(const TScreenPoint &screen_point) const;
  void InsertPointOnCurrentLine(const TScreenPoint &screen_point);
};

#endif
