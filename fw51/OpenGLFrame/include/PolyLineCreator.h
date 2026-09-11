 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// PolyLineCreator.h: interface for the CPolyLineCreator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POLYLINECREATOR_H__F801027F_27C3_410C_A5F4_70960525FA05__INCLUDED_)
#define AFX_POLYLINECREATOR_H__F801027F_27C3_410C_A5F4_70960525FA05__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MouseListener.h" 
#include "Plane.h" 
#include "DrawDef.h" 

#include "OpenGLFrameExports.h"

class OPENGLFRAME_EXPORT CPolyLineCreator : public CMouseListener 
{
	CDrawDef m_ddCreatedLine;
	CDrawDef m_ddDragLine;
	CDrawDef m_ddCreatedPoint;
	geo::CPlane m_Plane;
	geo::CPoint m_DragPoint;
	geo::CPoint m_BeginPoint;

protected:
	// A line is added by the user
	virtual void AddLine(const geo::IPoint& first, const geo::IPoint& second);
	// A point is added by the user
	virtual void AddPoint(const geo::IPoint& point);

public:
	CPolyLineCreator(TFrame &frame, const geo::IPlane& plane = geo::CPlane::PlaneXY);
	virtual ~CPolyLineCreator();

	virtual const geo::IPolyLine& PolyLine() const = 0;


	virtual bool MousePress(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point);
	virtual bool MouseRelease(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point);
	virtual bool MouseMove(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point);
	virtual void DrawScene();

	virtual IDrawDef& CreatedLineDrawDef();
	virtual IDrawDef& DragLineDrawDef();
	virtual IDrawDef& CreatedPointDrawDef();
};

#endif // !defined(AFX_POLYLINECREATOR_H__F801027F_27C3_410C_A5F4_70960525FA05__INCLUDED_)
