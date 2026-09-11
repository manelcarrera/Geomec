 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// PointModifier.cpp: implementation of the CPointModifier class.
//
//////////////////////////////////////////////////////////////////////

#include "dimple.h"
#include <qgl.h>
#include "IOpenGLFrame.h"
#include "PointModifier.h"
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



CPointModifier::CPointModifier(TFrame& frame, const geo::IPoint &point, const geo::IPlane& plane)
:CMouseListener(frame), m_Plane(plane), m_ddSelection(qRgb(0, 255, 0)), m_ddDragging(qRgb(255,255,0))
{
  Point(point);

  m_ddSelection.PointSize(8);
  m_ddSelection.IsSelectable(true);
  m_ddDragging.PointSize(8);
}

//remove the dynamic allocated points
//and reset the fixed bounding box flag of the scene
CPointModifier::~CPointModifier()
{
}

void CPointModifier::Point(const geo::IPoint& point)
{
  // Renew point
  m_point = point;
  
  Frame().UpdateFrame();
}

const geo::IPoint& CPointModifier::Point() const
{
  return m_point;
}

geo::CPoint CPointModifier::CurrentPosition(const TScreenPoint& point) const
{
  geo::CPoint pt1,pt2;
  pt1 = Frame().UnProject(point,0);
  pt2 = Frame().UnProject(point,1);
  pt1.Z(0);
  pt2.Z(1);
  geo::CLine l(pt1,pt2);
  return m_Plane.Intersection(l);
}

bool CPointModifier::MouseDblClk(TKeyboardModifiers /*state*/, TMouseButton /*button*/, const TScreenPoint& /*point*/)
{
  return true;
}

bool CPointModifier::MouseWheel(TKeyboardModifiers /*state*/, int /*nDelta*/, const TScreenPoint& /*point*/)
{
  return true;
}

bool CPointModifier::MouseRelease(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point)
{
  if((state &Qt::ControlModifier) || (state &Qt::ShiftModifier))
    return false;

  if(button == Qt::LeftButton)
  {
    // Validate point if we have any
    if(Dragging())
    {
      geo::CPoint p = OnSetDragPoint(CurrentPosition(point));
      if(!ValidatePoint(p))
        return true;

      // Assign
      Point(p);
    }
  }

  return true;
}

bool CPointModifier::MousePress(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point)
{

  if((state &Qt::ControlModifier) || (state &Qt::ShiftModifier))
    return false;

  if(button == Qt::LeftButton)
  {
    // Do we have actually have a point?
    if(Point().Empty())
    {
      // Point is created here on the site
      geo::CPoint p = CurrentPosition(point);

      // Set back standard cursor
      Frame().SetCursor(Qt::ArrowCursor);

      if(!ValidatePoint(p))
        return true;
    
      // New point is created ... and validate
      Point(p);
      OK();
      return true;
    }

    // Hopefully our displaylist is hit
    std::vector<const geo::IObject*> vcSelection = Frame().ProcessSelection(point);
    for(size_t i = 0; i < vcSelection.size(); i++)
    {
      if(vcSelection[i] == &m_point)
      {
        m_drag_point = Point();
        return true;
      }
    }

    // Set back standard cursor
    Frame().SetCursor(Qt::ArrowCursor);

    OK();
  }

  return true;
}

bool CPointModifier::Dragging() const
{
  return !m_drag_point.Empty();
}

bool CPointModifier::MouseMove(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point)
{
  if((state &Qt::ControlModifier) || (state &Qt::ShiftModifier))
    return false;

  geo::CPoint ptDrag = OnSetDragPoint(CurrentPosition(point));

  if(button == Qt::LeftButton)
  {
    m_drag_point = ptDrag;
    
    OnSetCursor(m_drag_point, ValidatePoint(m_drag_point));
    Frame().UpdateFrame();
  }
  else
  {
    if(Point().Empty())
    {
      OnSetCursor(ptDrag, ValidatePoint(ptDrag));
    }
  }
  
  return true;
}

//validate a new candidate-point
bool CPointModifier::ValidatePoint(const geo::IPoint& point) const
{
  if(point.Empty())
    return false;

  return CMouseListener::ValidatePoint(point);
}

//called by the view
//do not call View().DrawScene() !!!! (recursion)
void CPointModifier::DrawScene()
{
  // If we're dragging paint the dragged item here
  if(Dragging())
  {
    //make sure the following drawing code is always visible
//		GLint iOldDeptFunc = View().SetGLDepthFunc(GL_ALWAYS);
    
    if(ValidatePoint(m_drag_point))
      Frame().DrawObject(m_drag_point, m_ddDragging);

    //reset depth function
//		View().SetGLDepthFunc(iOldDeptFunc); 
  }
  else
  {
    Frame().DrawObject(m_point, m_ddSelection);
  }
}

