 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// PolygonCreator.cpp: implementation of the CPolygonCreator class.
//
//////////////////////////////////////////////////////////////////////

#include "dimple.h"
#include "PolygonCreator.h"
#include "IOpenGLFrame.h"
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



CPolygonCreator::CPolygonCreator(TFrame& frame,	const geo::IPlane& plane)
:CPolygonModifierBase(frame, qRgb(255,0,0), qRgb(0,0,255), qRgb(0,255,0), false)
{
  //TODO check if constraining polygons are in same plane as the givven plane (htg)
  //TODO check inside and outer-polygons (htg)

  //see remark in header
  m_Plane=plane;
  
  //we are adding the m_Points and m_List to the CurrentScene() to enable
  //selection of these objects (specialy the begin point for closing the poly)
  m_ddPoints.PointSize(10);
  m_ddPoints.IsSelectable(true);
  //init state flags
  m_bCanceled=FALSE;
  m_bDisplayListRemoved=FALSE;
}

//remove the dynamic allocated points
//and reset the fixed bounding box flag of the scene
CPolygonCreator::~CPolygonCreator()
{
}

bool CPolygonCreator::MousePress(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point)
{
  if( (state & Qt::ShiftModifier) ||(state & Qt::ControlModifier ) )
    return false;

  if(button == Qt::LeftButton)
  {

    //get the corresponding world point
    geo::CLine l(Frame().UnProject(point,0),Frame().UnProject(point,1));
    geo::CPoint p = m_Plane.Intersection(l);
    
    if(!ValidatePoint(p))
      return TRUE;

    //see if selected screen point is the first point 
    //that means were finished
    if(m_pPolygon->PointHitTest(point) == 0)
    {
      // Terminate mouse listener
      OK();
    }
    else
    {
      //this is not the last one
      //add point to the polygon
      m_DragPoint = p;
      m_pPolygon->PushBack(m_DragPoint);
      
      // Just update the view
      Frame().UpdateFrame();
    }
  }
  if(button == Qt::RightButton)
    Cancel();
  
  return true;
}

bool CPolygonCreator::KeyPress( TKeyboardModifiers state,
                int                nAscii,
                TKey               key,
                int                nRepCount )
{
  if ( key == Qt::Key_Backspace ) {
  if ( m_pPolygon->PointSize() > 0 ) {
      // Removing point ...
      m_pPolygon->DeletePoint( m_pPolygon->PointSize() - 1 );
      // and update view ....
      Frame().UpdateFrame();
  }
  return true;
  }
  return CMouseListener::KeyPress( state, nAscii, key, nRepCount );
}

bool CPolygonCreator::MouseDblClk(TKeyboardModifiers /*state*/, TMouseButton /*button*/, const TScreenPoint& /*point*/)
{

  return true;
}

bool CPolygonCreator::MouseWheel(TKeyboardModifiers /*state*/, int /*nDelta*/, const TScreenPoint& /*point*/)
{
  return true;
}


bool CPolygonCreator::MouseMove(TKeyboardModifiers state, TMouseButton /*button*/, const TScreenPoint& point)
{
  if( (state & Qt::ShiftModifier) ||(state & Qt::ControlModifier ) )
    return false;

  //convert to world coor and calc intersection with the plane
  geo::CLine l(Frame().UnProject(point,0),Frame().UnProject(point,1));
  geo::CPoint p = m_Plane.Intersection(l);

//	prev = point;
  m_DragPoint=p;

  if(ValidatePoint(p))
  {
    // When the first point is on the current location and pointsize is larger than 2 we can finish
    if((m_pPolygon->PointHitTest(point) == 0) && m_pPolygon->PointSize() > 2)
    {
      //show finish cursor
      Frame().SetCursor(Qt::ArrowCursor);
    }
    else
    {
      //show valid mouse cursor
      Frame().SetCursor(Qt::CrossCursor);
    }
  }
  else
    Frame().SetCursor(Qt::ForbiddenCursor);
    
  Frame().UpdateFrame();

  return TRUE;
}



//return a polygon, called by the view after we have said
//were finished: Frame().OnFinishedMouseListener();
geo::CPolygon CPolygonCreator::GetCreatedPolygon()
{
  DIA_ASSERT(!m_bCanceled);
  return CPolygonModifierBase::Polygon();
}



//called by the view
//do not call Frame().DrawScene() !!!! (recursion)
void CPolygonCreator::DrawScene()
{
  // 2 drawdefs for different colors
  CDrawDef dd3(qRgb(255,0,0));
  CDrawDef ddGrey(qRgb(150,150,150));
  ddGrey.LineStipple(TRUE);
  ddGrey.LineWidth(2);

  //draw candidate line
  if(m_pPolygon->PointSize())
  {
    if(!(m_DragPoint == m_pPolygon->LastPoint()))
    {
      geo::CLine l(m_DragPoint, m_pPolygon->LastPoint());
      if(ValidatePoint(m_DragPoint))		
        Frame().DrawObject(l,dd3);
      else
        Frame().DrawObject(l,ddGrey);
    }
  }
  
  CPolygonModifierBase::DrawScene();
}

bool CPolygonCreator::CanFinish()
{
  return ValidateThisPolygon();
}

bool CPolygonCreator::ValidateThisPolygon()
{
  return ValidatePolygon(GetCreatedPolygon());

}

