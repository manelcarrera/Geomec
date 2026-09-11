 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// RectangleModifier.h: interface for the CRectangleModifier class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECTANGLEMODIFIER_H__5101C4A6_5DE5_4D66_B2A2_731C9BA6E1A9__INCLUDED_)
#define AFX_RECTANGLEMODIFIER_H__5101C4A6_5DE5_4D66_B2A2_731C9BA6E1A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Dragger.h"
#include "DrawDef.h"
#include "Rectangle.h"

#include "OpenGLFrameExports.h"

// This class creates a rectangle by dragging over the screen ...
class OPENGLFRAME_EXPORT CRectangleDragger : public CDragger
{
protected:
  // During dragging one can ask the rectangle
  geo::CRectangle Rectangle() const;
public:
  CRectangleDragger(TFrame& view, const geo::IPlane& = geo::CPlane::PlaneXY);
  virtual ~CRectangleDragger();

  //CDragger interface
  virtual void DrawScene();
  virtual bool OnCancel();	
};

class OPENGLFRAME_EXPORT CRectangleModifier : public CMouseListener  
{
private:
  geo::CRectangle m_Rectangle;
  std::vector<const geo::IPoint*> m_vcPoint;

  geo::CPoint m_DragPoint;
  geo::CPoint m_BasePoint;
  int m_nSelectedCornerPoint;

  //drawdefs to push_back in the current scene
  CDrawDef m_ddPoints,m_ddRectangle;

  //remark:
  //the plane to create the rectangle on.
  //the constraining polygons should also be in this plane.
  geo::CPlane m_Plane;


  //remember if the the scene uses a fixed bounding box
  //we are setting the the FixedBoundingBox because
  //we don'd want any resizing if we add points outside the scene's bounding box 
//	BOOL m_bFixedBoundingBox; 
//	geo::CBox m_FixedBoundingBox; 

  void UpdatePoints();
  int PointHitTest(const TScreenPoint& point);
  bool Dragging() const;
public:
  CRectangleModifier(TFrame& frame, geo::CRectangle &Rectangle);

  virtual ~CRectangleModifier();

  //CMouseListener overides called by the view
  virtual bool OnOK();
  virtual void DrawScene();
  
  geo::CRectangle GetModifiedRectangle();

  virtual bool MousePress(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point);
  virtual bool MouseRelease(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point);
  virtual bool MouseDblClk(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point);
  virtual bool MouseWheel(TKeyboardModifiers state, int nDelta, const TScreenPoint& point);
  virtual bool MouseMove(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point);

protected:
  virtual void OnRectangleModified() {} // to be handled by derived
  virtual geo::CRectangle OnDragRectangle(const geo::IPoint& begin_point, const geo::IPoint& drag_point);
};

#endif // !defined(AFX_RECTANGLEMODIFIER_H__5101C4A6_5DE5_4D66_B2A2_731C9BA6E1A9__INCLUDED_)
