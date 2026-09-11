 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// Dragger.h: interface for the CDragger class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAGGER_H__C1D442B6_1EE4_4C12_8CA2_970E1B96DAAB__INCLUDED_)
#define AFX_DRAGGER_H__C1D442B6_1EE4_4C12_8CA2_970E1B96DAAB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MouseListener.h"
#include "Plane.h"

#include "OpenGLFrameExports.h"

class OPENGLFRAME_EXPORT CDragger : public CMouseListener  
{
	geo::CPoint		m_ptBeginPoint;			//world coord.
	geo::CPoint		m_ptCurrentPoint;		//world coord.
	TScreenPoint	m_ptBeginScreenPoint;	//screen coord.
	TScreenPoint	m_ptCurrentScreenPoint;	//screen coord.
	geo::CPlane m_plane;				// The plane the user is dragging on. When the plane is Empty()
										// only the screen coordinates are used.
protected:
	const geo::IPoint& BeginPoint() const;
	const geo::IPoint& CurrentPoint() const;
	const TScreenPoint& BeginScreenPoint() const;
	const TScreenPoint& CurrentScreenPoint() const;
	virtual bool ValidatePoint(const geo::IPoint& point) const;
	const geo::IPlane& Plane() const;
public:
	// Construction destruction 
	CDragger(TFrame& frame, const geo::IPlane& plane = geo::CPlane());
	virtual ~CDragger();
	
	// CMouseListener interface
	virtual bool MousePress(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point);
	virtual bool MouseRelease(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point);
	virtual bool MouseDblClk(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point);
	virtual bool MouseWheel(TKeyboardModifiers state, int nDelta, const TScreenPoint& point);
	virtual bool MouseMove(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point);
	virtual void DrawScene() = 0;

	//CDragger interface
	virtual bool OnCancel();
	virtual bool OnOK();

	bool IsDragging() const;

};

#endif // !defined(AFX_DRAGGER_H__C1D442B6_1EE4_4C12_8CA2_970E1B96DAAB__INCLUDED_)
