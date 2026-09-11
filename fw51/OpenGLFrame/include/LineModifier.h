/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// RectangleModifier.h: interface for the CRectangleModifier class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LINEMODIFIER_H__5101C4A6_5DE5_4D66_B2A2_731C9BA6E1A9__INCLUDED_)
#define AFX_LINEMODIFIER_H__5101C4A6_5DE5_4D66_B2A2_731C9BA6E1A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DrawDef.h"
#include "MouseListener.h"
#include "Plane.h"

#include "OpenGLFrameExports.h"

class OPENGLFRAME_EXPORT CLineModifier : public CMouseListener {
private:
  class OPENGLFRAME_EXPORT CModifierLine : public geo::ILine {
    geo::CPoint m_pt1;
    geo::CPoint m_pt2;

  public:
    CModifierLine(const geo::IPoint &pt1, const geo::IPoint &pt2);
    virtual ~CModifierLine();

    virtual bool Empty() const;

    virtual size_t Order() const { return 1; }

    virtual const geo::IPoint &Point(int nIndex) const;
    virtual void Point(int nIndex, const geo::IPoint &point);
  };

  CModifierLine m_OriginalLine;
  CModifierLine *m_pModifiedLine;

  // the original point (from m_OriginalLine) that is to be updated
  geo::IPoint *m_pOriginalPoint;

  // the point that is being dragged (temporary)
  geo::IPoint *m_pDragPoint;

  // drawdefs to push_back in the current scene
  CDrawDef m_ddPoints, m_ddLine, m_ddSelectedPoints;

  // remark:
  // the plane to create the rectangle on.
  // the constraining polygons should also be in this plane.
  geo::CPlane m_Plane;

  // Helper functions
  bool ValidateThisLine() const;
  bool Dragging() const;

public:
  CLineModifier(TFrame &view, const geo::IPoint &linepoint1, const geo::IPoint &linepoint2,
                const geo::IPlane & = geo::CPlane::PlaneXY // see remark by m_Plane
  );
  virtual ~CLineModifier();

  // CMouseListener overides called by the view
  virtual bool OnOK();
  virtual bool MousePress(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);
  virtual bool MouseRelease(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);
  virtual bool MouseDblClk(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);
  virtual bool MouseWheel(TKeyboardModifiers state, int nDelta, const TScreenPoint &point);
  virtual bool MouseMove(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);
  virtual void DrawScene();
  const geo::ILine &GetLine() const;
};

#endif // !defined(AFX_LINEMODIFIER_H__5101C4A6_5DE5_4D66_B2A2_731C9BA6E1A9__INCLUDED_)
