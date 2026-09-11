 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// PolygonModifier.cpp: implementation of the CPolygonCreator class.
//
//////////////////////////////////////////////////////////////////////

#include "dimple.h"
#include "IOpenGLFrame.h"
#include "PolygonModifier.h"
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

#ifdef WIN32
typedef void (__stdcall *TglAddSwapHintRectWIN)(GLint x,GLint y,GLsizei width,GLsizei height );
#else // !WIN32
typedef void (*TglAddSwapHintRectWIN)(GLint x,GLint y,GLsizei width,GLsizei height );
#endif  // WIN32

extern TglAddSwapHintRectWIN glAddSwapHintRectWIN;



CPolygonModifier::CPolygonModifier(TFrame& frame,
								   const geo::IPolygon &polygon,
								   const geo::IPlane& plane)
: CPolygonModifierBase(frame, qRgb(255,0,0), qRgb(0,0,255), qRgb(0,255,0)),
  m_Plane(plane)
{
	Frame().SetCursor(Qt::ForbiddenCursor);

	m_ddDragLine.LineStipple(true);
	m_ddDragLine.LineWidth(2);

	for (int i = 0;i < polygon.NrOfPoints(); i++)
		m_pPolygon->PushBack(polygon.Point(i));
}

//remove the dynamic allocated points
//and reset the fixed bounding box flag of the scene
CPolygonModifier::~CPolygonModifier()
{
}

bool CPolygonModifier::MouseDblClk(TKeyboardModifiers /*state*/, TMouseButton /*button*/, const TScreenPoint& /*point*/)
{
	return true;
}

bool CPolygonModifier::MouseWheel(TKeyboardModifiers /*state*/, int /*nDelta*/, const TScreenPoint& /*point*/)
{
	return true;
}

bool CPolygonModifier::MouseRelease(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point)
{
	if((state & Qt::ControlModifier)||(state & Qt::ShiftModifier))
		return false;

	if(button == Qt::LeftButton)
	{
		if(Dragging())
		{
//			SetCursor(m_previousCursor);

			// validate the polygon
			if(ValidateThisPolygon())
			{
				// Polygon is OK ...
				DIA_ASSERT(m_pPolygon->PointSelection() > -1);

				// World point
				geo::CLine l(Frame().UnProject(point,0),Frame().UnProject(point,1));
				m_DragPoint = m_Plane.Intersection(l);
				
				// Replace selected point with drag position
				m_pPolygon->ReplacePoint(m_pPolygon->PointSelection(), m_DragPoint);
			}

			// Reset drag state
			m_DragPoint = geo::CPoint();
			m_pPolygon->PointSelection(-1);

			DIA_ASSERT(!Dragging());

			Frame().UpdateFrame();
		}
	}

	return TRUE;
}

bool CPolygonModifier::MousePress(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point)
{
	if((state & Qt::ControlModifier)||(state & Qt::ShiftModifier))
		return false;

	if(button == Qt::LeftButton)
	{

		// TODO : Create a good routine for proper multiselection ...
		if (!(state & Qt::ControlModifier))
		{
			// Clear current selection
			m_pPolygon->PointSelection( -1 );
			m_DragPoint = geo::CPoint();
		}

		// The user can select one point and that is stored
		m_pPolygon->PointSelection( m_pPolygon->PointHitTest(point) );
		m_DragPoint = geo::CPoint();

		Frame().UpdateFrame();

		if(m_pPolygon->PointSelection() == -1)
			OK();	// Store if the user clicked somewhere else ...
	}


	return true;
}

bool CPolygonModifier::ValidateThisPolygon()
{
	// Replace point with selection ...
	geo::CPolygon polygon;
	for(int i = 0; i < m_pPolygon->PointSize(); i++)
	{
		// Do we have a selection?
		if(m_pPolygon->PointSelection() == i)
		{
			if(!polygon.PushBack(m_DragPoint))	// Double points
				return false;
		}
		else
		{
			if(!polygon.PushBack(m_pPolygon->Point(i)))
				return false;
		}
	}
	
	return ValidatePolygon(polygon);
}

bool CPolygonModifier::CanDeleteSelectedPoint() const
{
	// Do we actually have point selection?
	if(m_pPolygon->PointSelection() >= 0)
	{
		// Is the poly OK?
		geo::CPolygon polygon;
		for(int i = 0; i < m_pPolygon->PointSize(); i++)
		{
			// Do we have a selection?
			if(m_pPolygon->PointSelection() != i)
				polygon.PushBack(m_pPolygon->Point(i));
		}
		
		return ValidatePolygon(polygon);
	}

	return false;
}

void CPolygonModifier::DeleteSelectedPoint()
{
	// Delete the selected point
	DIA_ASSERT(CanDeleteSelectedPoint());	// Forget to call these function?

	// Do it ...
	m_pPolygon->DeletePoint(m_pPolygon->PointSelection());
	m_DragPoint = geo::CPoint();

	// Check it ...
	DIA_ASSERT(m_pPolygon->PointSelection() == -1);
}

bool CPolygonModifier::CanInsertPointOnCurrentLine(const TScreenPoint& screen_point) const
{
	// Are we currently on a line
	int nLineIndex = m_pPolygon->LineHitTest(screen_point);
	if(nLineIndex > -1)
	{
		// We're on the current line so yes if not on the corner nodes ...
		geo::CLine l(Frame().UnProject(screen_point,0),Frame().UnProject(screen_point,1));
		geo::CPoint point = m_Plane.Intersection(l);		
		return (!(m_pPolygon->Line(nLineIndex).First() == point)) && (!(m_pPolygon->Line(nLineIndex).Second() == point));
	}

	return false;
}

void CPolygonModifier::InsertPointOnCurrentLine(const TScreenPoint& screen_point)
{
	DIA_ASSERT(CanInsertPointOnCurrentLine(screen_point));

	// Are we currently on a line
	int nLineIndex = m_pPolygon->LineHitTest(screen_point);
	geo::CLine l(Frame().UnProject(screen_point,0),Frame().UnProject(screen_point,1));
	geo::CPoint point = m_Plane.Intersection(l);		
	m_pPolygon->InsertPoint(nLineIndex, point);
}

bool CPolygonModifier::KeyPress(TKeyboardModifiers state, int nAscii, TKey key, int nRepCount )
{
	if ( key == Qt::Key_Delete ) {
		if ( CanDeleteSelectedPoint() ) DeleteSelectedPoint();
		return true;
	}

	return CPolygonModifierBase::KeyPress( state, nAscii, key, nRepCount );
}

bool CPolygonModifier::MouseMove(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point)
{
	if((state & Qt::ControlModifier)||(state & Qt::ShiftModifier))
		return false;

	if(button == Qt::LeftButton)
	{
		if(m_pPolygon->PointSelection() >= 0)
		{
			// There is a point selected and we're dragging so calculate new point
			geo::CLine l(Frame().UnProject(point,0),Frame().UnProject(point,1));
			m_DragPoint = m_Plane.Intersection(l);
			Frame().UpdateFrame();
			
			// Do we have a valid polygon ....
			if (ValidateThisPolygon())
				Frame().SetCursor(Qt::ArrowCursor);
			else
				Frame().SetCursor(Qt::ForbiddenCursor);
		}
	}

	return TRUE;
}

//called by the view
//do not call Frame().DrawScene() !!!! (recursion)
void CPolygonModifier::DrawScene()
{
	//make sure the following drawing code is always visible
//	GLint iOldDeptFunc = Frame().SetGLDepthFunc(GL_ALWAYS);

	if(Dragging())
	{
		DIA_ASSERT(m_pPolygon->PointSize() > 1);
		DIA_ASSERT(m_pPolygon->PointSelection() > -1);

		//draw two lines.
		geo::CLine l1(m_pPolygon->Previous(m_pPolygon->PointSelection()), m_DragPoint);
		geo::CLine l2(m_pPolygon->Next(m_pPolygon->PointSelection()), m_DragPoint);
		Frame().DrawObject(l1,m_ddDragLine);
		Frame().DrawObject(l2,m_ddDragLine);
		Frame().DrawObject(m_DragPoint, m_pPolygon->SelectedPointDrawDef());
	}

	//reset depth function
//	Frame().SetGLDepthFunc(iOldDeptFunc);

	CPolygonModifierBase::DrawScene();
}
