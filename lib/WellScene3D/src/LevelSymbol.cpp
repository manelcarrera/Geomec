
#include "StdAfx.h"
#include "LabelPoint.h"
#include "LevelSymbol.h"
#include <qstring.h>


#define LV_SCALE 8.0
#define LV_LINELENGTH 8*LV_SCALE
#define LV_OVERLENGTH 1*LV_SCALE
#define LV_VERTLENGTH 4*LV_SCALE
#define LV_VERTLENGTH2 2*LV_SCALE
#define LV_OVERHORLENGTH2 1*LV_SCALE
#define LV_OVERHORLENGTH3 8*LV_SCALE
#define LV_LABEL_OFFSET_X 0.7*LV_SCALE
#define LV_LABEL_OFFSET_Y 0.5*LV_SCALE



CLevelSymbol::CLevelSymbol(const geo::CPoint& level,const QString& description ,bool autoDelete)
:CWellSymbolBase(level,description,autoDelete)
{

  geo::CPoint p2 = level;
  p2.Move(geo::CVector(LV_LINELENGTH,0,0));


  m_Array.PushBack(*new geo::CLine( level, p2));


  geo::CPoint p3 = p2;
  p3.Move(geo::CVector(-LV_OVERLENGTH,0,0));

  geo::CPoint p4 = p3;
  p4.Move(geo::CVector(0,0,-LV_VERTLENGTH));
  m_Array.PushBack(*new geo::CLine( p3, p4));
  
  geo::CPoint p5 = p3;
  p5.Move(geo::CVector(-LV_OVERHORLENGTH2,0,-LV_VERTLENGTH2));
  m_Array.PushBack(*new geo::CLine( p3, p5));


  geo::CPoint p6 = p5;
  p6.Move(geo::CVector(LV_OVERHORLENGTH3,0,0));
  m_Array.PushBack(*new geo::CLine( p5, p6));

  geo::CPoint p7 = p3;
  p7.Move(geo::CVector(0,0,-LV_VERTLENGTH2));
  p7.Move(geo::CVector(LV_LABEL_OFFSET_X,0,-LV_LABEL_OFFSET_Y));

  QString tmp;
  if(m_Description.isEmpty())
  {
    tmp = "Z = %1";
    tmp.arg(m_Position.Z());
  }
  else
  {
    tmp=m_Description;
  }
  CLabelPoint* pLabel = new CLabelPoint(p7,tmp);
  
  m_Array.PushBack(*pLabel);
  // Construct
}




