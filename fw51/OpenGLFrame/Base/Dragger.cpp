 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "dimple.h"
//#include "DrawDef.h"
#include "IOpenGLFrame.h"
#include "MouseListener.h"
#include "Dragger.h"
#include "Line.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDragger::CDragger(TFrame& frame, const geo::IPlane& plane)
: CMouseListener(frame),
  m_plane(plane)
{
}

CDragger::~CDragger()
{
}

const geo::IPoint& CDragger::BeginPoint() const
{
	DIA_ASSERT(IsDragging());
	return m_ptBeginPoint;
}

const geo::IPoint& CDragger::CurrentPoint() const
{
	DIA_ASSERT(IsDragging());
	return m_ptCurrentPoint;
}

const CDragger::TScreenPoint& CDragger::BeginScreenPoint() const
{
	DIA_ASSERT(IsDragging());
	return m_ptBeginScreenPoint;
}

const CDragger::TScreenPoint& CDragger::CurrentScreenPoint() const
{
	DIA_ASSERT(IsDragging());
	return m_ptCurrentScreenPoint;
}

const geo::IPlane& CDragger::Plane() const
{
	return m_plane;
}

bool CDragger::ValidatePoint(const geo::IPoint& /*point*/) const
{
	return true;
}

bool CDragger::IsDragging() const
{
	return !m_ptBeginPoint.Empty();
}

bool CDragger::MousePress(TKeyboardModifiers /*state*/, TMouseButton button, const TScreenPoint& point)
{
	if(button == Qt::LeftButton)
	{
		// Left button down
		DIA_ASSERT(!IsDragging());

		// Store screen points
		m_ptBeginScreenPoint = point;
		m_ptCurrentScreenPoint = point;

		// Store world points
		geo::CPoint w_point;
		if(!m_plane.Empty())
		{
			geo::CLine l(Frame().UnProject(point,0), Frame().UnProject(point,1));
			w_point = m_plane.Intersection(l);
		}
		else
			w_point = Frame().UnProject(point, 0.5);		// Use screen ...
		
		if(ValidatePoint(w_point))
		{
			m_ptBeginPoint= w_point;
			m_ptCurrentPoint= m_ptBeginPoint;

			DIA_ASSERT(IsDragging());
		}
	}

	return true;
}

bool CDragger::MouseMove(TKeyboardModifiers /*state*/, TMouseButton /*button*/, const TScreenPoint& point)
{
	// Calculate current world point
	geo::CPoint w_point;
	if(!m_plane.Empty())
	{
		geo::CLine l(Frame().UnProject(point,0),Frame().UnProject(point,1));
		w_point = m_plane.Intersection(l);
	}
	else
		w_point = Frame().UnProject(point, 0.5);		// Use screen ...

	if(IsDragging())
	{
		// Store points
		m_ptCurrentPoint = w_point;
		m_ptCurrentScreenPoint = point;
		
		//Is it valid?
		if(ValidatePoint(m_ptCurrentPoint))
			Frame().SetCursor(Qt::SizeAllCursor);
		else
			Frame().SetCursor(Qt::ForbiddenCursor);
		Frame().UpdateFrame();
	}
	else
	{
		// Do we have no drop or draw?
		if(ValidatePoint(w_point))
			Frame().SetCursor(Qt::CrossCursor);
		else
			Frame().SetCursor(Qt::ForbiddenCursor);
	}

	return true;	// Handled
}

bool CDragger::MouseRelease(TKeyboardModifiers /*state*/, TMouseButton button, const TScreenPoint &point)
{
	if(button == Qt::LeftButton)
	{
		// When dragging store point and validate
		if(IsDragging())
		{
			if(!m_plane.Empty())
			{
				geo::CLine l(Frame().UnProject(point,0), Frame().UnProject(point,1));
				m_ptCurrentPoint = m_plane.Intersection(l);
			}
			else
				m_ptCurrentPoint = Frame().UnProject(point, 0.5);		// Use screen ...

			m_ptCurrentScreenPoint = point;

			// Set back standard cursor
			Frame().SetCursor(Qt::ArrowCursor);

			if(ValidatePoint(CurrentPoint()) && ValidatePoint(BeginPoint()))
				OK();
			else
				Cancel();
		}
	}
	
	return true;	// Handled
}

bool CDragger::MouseDblClk(TKeyboardModifiers /*state*/, TMouseButton /*button*/, const TScreenPoint& /*point*/)
{
	return true;
}

bool CDragger::MouseWheel(TKeyboardModifiers /*state*/, int /*nDelta*/, const TScreenPoint& /*point*/)
{
	return true;
}


bool CDragger::OnCancel()
{
	// Switch off dragging and update view
	m_ptBeginPoint = geo::CPoint();
	m_ptCurrentPoint = geo::CPoint();
	Frame().UpdateFrame();
	return true;
}

bool CDragger::OnOK()
{
	// Switch off dragging and update view
	m_ptBeginPoint = geo::CPoint();
	m_ptCurrentPoint = geo::CPoint();
	Frame().UpdateFrame();
	return true;
}
