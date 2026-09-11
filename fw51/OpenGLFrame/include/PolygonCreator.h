// PolygonCreator.h: interface for the CPolygonCreator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POLYGONCREATOR_H__FC95697E_A9AA_4E22_81B9_57DE9C93FD73__INCLUDED_)
#define AFX_POLYGONCREATOR_H__FC95697E_A9AA_4E22_81B9_57DE9C93FD73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ObjectModifier.h"
#include "Plane.h"

#include "OpenGLFrameExports.h"

// Copyright (c) 2011 TNO DIANA BV                              Confidential
// Description : A generic class to create a polygon with the Geometry.lib and OpenGL.lib
// Status      : Finished (80%)
// Remarks     : Do not switch the CurrentScene in the view
//				 during the creation of the polygon
// Last review : None
// Authur      : HTG
class OPENGLFRAME_EXPORT CPolygonCreator : public CPolygonModifierBase {
protected:
  // The point that is currently dragged by the mouse and is candidate for joining
  // the polygon. It will be displayed as line with the lastpoint of the existing
  // polygon. When the drag point is empty the creation operation has not started.
  geo::CPoint m_DragPoint;

  // drawdefs to push_back in the current scene
  CDrawDef m_ddPoints, m_ddLines;

  // remark:
  // the plane to create the polygon on.
  // the constraining polygons should also be in this plane.
  geo::CPlane m_Plane;

  virtual bool CanFinish();

  // This routine builds a polygon using the m_Points and m_SelectedPoints arrays. It then validates them.
  bool ValidateThisPolygon();

  // state flags
  bool m_bCanceled;
  bool m_bDisplayListRemoved;

  // validate a new candidate-point
  // is virtual because overrides can extend or reduce restrictions for validation

public:
  CPolygonCreator(TFrame &view, const geo::IPlane & = geo::CPlane::PlaneXY);
  virtual ~CPolygonCreator();

  // CMouseListener interface
  virtual bool MousePress(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);
  //	virtual bool MouseRelease(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point);
  virtual bool MouseDblClk(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);
  virtual bool MouseWheel(TKeyboardModifiers state, int nDelta, const TScreenPoint &point);
  virtual bool MouseMove(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);
  virtual void DrawScene();

  // use ESC-key for cancel...
  virtual bool KeyPress(TKeyboardModifiers state, int nAscii, TKey key, int nRepCount);

  // called by the view after we have said
  // we are finished successfully: m_pView->OnFinishedMouseListener(),and ;
  geo::CPolygon GetCreatedPolygon();
};

#endif // !defined(AFX_POLYGONCREATOR_H__FC95697E_A9AA_4E22_81B9_57DE9C93FD73__INCLUDED_)
