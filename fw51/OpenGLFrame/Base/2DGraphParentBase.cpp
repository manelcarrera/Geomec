 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "dimple.h"
#include <cmath>
#include "2DGraphParentBase.h"
#include "2DGraphLineBase.h"
#include "2DGraphBase.h"
#include "LabelPoint.h"
#include "Line.h"

C2DGraphParentBase::C2DGraphParentBase(TDirection x_direction, TAxis x_axis, TDirection y_direction, TAxis y_axis)
{
  SetAxis(x_direction, x_axis, y_direction, y_axis);
}

C2DGraphParentBase::~C2DGraphParentBase()
{
}

void C2DGraphParentBase::SetAxis(TDirection x_direction, TAxis x_axis, TDirection y_direction, TAxis y_axis)
{
  m_x_direction = x_direction;
  m_x_axis = x_axis;
  m_y_direction = y_direction;
  m_y_axis = y_axis;
}

void C2DGraphParentBase::Clear()
{
  Graph().Clear();
}

double C2DGraphParentBase::TextOffset() const
{
  return 20;
}

double C2DGraphParentBase::Offset() const
{
  return 10;
}

void C2DGraphParentBase::UpdateGraph()
{
  Graph().UpdateGraph();
}

void C2DGraphParentBase::UpdateScroll()
{
  Graph().RemoveAllObjects();
  Graph().UpdateGraphLayout(false);
  for(int i = 0; i < Graph().LineSize(); i++)
    Graph().Line(i).Update();

  UpdateFrame();
}

QString C2DGraphParentBase::XLabel() const
{
  return QString();
}

QString C2DGraphParentBase::XUnit() const
{
  return QString();
}

QString C2DGraphParentBase::YLabel() const
{
  return QString();
}

QString C2DGraphParentBase::YUnit() const
{
  return QString();
}

void C2DGraphParentBase::TickX(const unsigned int t)
{
  Graph().TickX(t);
}

void C2DGraphParentBase::TickY(const unsigned int t)
{
  Graph().TickY(t);
}

void C2DGraphParentBase::SetMinMaxX(const double& dMinX, const double& dMax)
{
  Graph().SetMinMaxX(dMinX, dMax);
}

/*virtual*/ QFont C2DGraphParentBase::GetTitleFontX() const
{
  return QFont(); //uses view default font 
}

/*virtual*/ QFont C2DGraphParentBase::GetTitleFontY() const
{
  return QFont(); //uses view default font 
}

void C2DGraphParentBase::DrawScene()
{
  DrawXLabels();
  DrawYLabels();
  IOpenGLFrame::DrawScene();
  DrawAxis(XAxis());
  DrawAxis(YAxis());
}

void C2DGraphParentBase::DrawXLabels()
{
  if((Graph().LineSize() > 0) &&
     (Graph().MinX() < Graph().MaxX()) &&
     (Graph().MaxY().Valid()) &&
     (Graph().MinY().Valid()))
  {
    int iFactor = ScaleFactor(Graph().MaxX(), Graph().MinX());

    // Print Yaxis description
    QString sAxis(""); 
    if (iFactor != 0)
      sAxis = QString(" : (x 10e%1)").arg(iFactor);

    sAxis = QString(XLabel() + sAxis + "[" + XUnit() + "]");
    DrawAxisLabel(sAxis, XAxis(), GetTitleFontX());
    
    // Walk over X-axis
    DIA_ASSERT(Graph().GridX() != 0);

    int iDivision = int( 0.5 + (Graph().MaxX() - Graph().MinX()) / Graph().GridX() );

    for (int i = 0; i < iDivision + 1; i++)
    {
      // Calculate value and location
      double dValue = Graph().MinX() + (Graph().GridX() * i);
      dValue *= pow( (double)10., -iFactor);
      QString sValue = QString("%1").arg(dValue,0,'f',2);
      DrawLabel(sValue, (double)i/(double)iDivision, XDirection(), XAxis());
    }	
  }
}

void C2DGraphParentBase::DrawAxis(TAxis axis)
{
  CDrawDef draw_def;
  draw_def.Visible(TRUE);
  draw_def.SetFixedColor(Graph().GetGraphBorderColor());
  draw_def.LineStipple(FALSE);
  draw_def.PointSize(2);
  draw_def.SetDepthLayer2D(GRAPH_LAYER_BORDER);

  int yTop = Graph().ViewPortY() + Graph().ViewPortHeight();
  int yBottom = Graph().ViewPortY();
  int xLeft = Graph().ViewPortX();
  int xRight = Graph().ViewPortX() + Graph().ViewPortWidth();

  switch(axis)
  {
  case AxisTop:
    DrawObject(geo::CLine(geo::CPoint(xLeft, yTop), geo::CPoint(xRight, yTop)), draw_def);
    break;
  case AxisBottom:
    DrawObject(geo::CLine(geo::CPoint(xLeft, yBottom), geo::CPoint(xRight, yBottom)), draw_def);
    break;
  case AxisLeft:
    DrawObject(geo::CLine(geo::CPoint(xLeft, yTop), geo::CPoint(xLeft, yBottom)), draw_def);
    break;
  case AxisRight:
    DrawObject(geo::CLine(geo::CPoint(xRight, yTop), geo::CPoint(xRight, yBottom)), draw_def);
    break;
  default:
    assert(false);
    break;
  };
}

void C2DGraphParentBase::DrawAxisLabel(const QString sLabel,
                     TAxis axis,
                     QFont font)
{
  // 
  double x, y;
  double dRotate = 0;
  switch(axis)
  {
  case AxisTop:
    x = Graph().ViewPortX() + Graph().ViewPortWidth() / 2;
    y = Graph().ViewPortY() + Graph().ViewPortHeight() + 1.5 * TextOffset();
    break;
  case AxisBottom:
    x = Graph().ViewPortX() + Graph().ViewPortWidth() / 2;
    // wjrx mantis 2338: slightly raise the x-axis text
    //
    // y = Graph().ViewPortY() - 1.5 * TextOffset();
    //
    y = Graph().ViewPortY() - 1.2 * TextOffset();
    break;
  case AxisLeft:
    // wjrx mantis 2338: no longer attempt to draw rotated text.
    // Stick it in the bottom left or right instead.
    //
    // dRotate = 90;
    //
    //  x = Graph().ViewPortX() - TextOffset(); 
    x = 5;
    // wjrx y = Graph().ViewPortY()+ Graph().ViewPortHeight() / 2;
    y = 5; //TextOffset(); //wjrx
    break;
  case AxisRight:
    // wjrx mantis 2338
    // dRotate = 90;
    x = Graph().ViewPortX() + Graph().ViewPortWidth() + TextOffset();
    y = 5;
    break;
  default:
    assert(false);
    break;
  };

  CDrawDef draw_def;
  draw_def.SetDepthLayer2D(GRAPH_LAYER_BORDER);
  draw_def.Visible(TRUE);
  draw_def.TextColor(qRgb(255,255,255));
  draw_def.LineStipple(FALSE);
  draw_def.LineWidth(1);
  draw_def.PointSize(2);

  // wjrx mantis 2338: new stuff for left and right axes
  if ( axis == AxisLeft )
  {
    CLabelPoint label_point(geo::CPoint(x, y), sLabel, font, CLabelPoint::LA_BOTTOM_LEFT);
    label_point.SetRotate(dRotate);
    DrawObject(label_point, draw_def);
  }
  else if ( axis == AxisRight )
  {
    CLabelPoint label_point(geo::CPoint(x, y), sLabel, font, CLabelPoint::LA_BOTTOM_RIGHT);
    label_point.SetRotate(dRotate);
    DrawObject(label_point, draw_def);
  }
  else
  {
    CLabelPoint label_point(geo::CPoint(x, y), sLabel, font, CLabelPoint::LA_CENTER);
    label_point.SetRotate(dRotate);
    DrawObject(label_point, draw_def);
  }
}

void C2DGraphParentBase::DrawLabel(const QString& sLabel,
           const double& dFraction, 
           TDirection direction,
           TAxis axis)
{
  double dRotate = 0;
  CLabelPoint::eLabelAlign align = CLabelPoint::LA_CENTER;
  int x = 0, y = 0;
  switch(axis)
  {
  case AxisTop:
    assert((direction == LeftRight) || (direction == RightLeft));
    align = CLabelPoint::LA_CENTER;
    y = (int) ( Graph().ViewPortY() +
          Graph().ViewPortHeight() +
          0.75 * TextOffset() );
    break;
  case AxisBottom:
    assert((direction == LeftRight) || (direction == RightLeft));
    align = CLabelPoint::LA_CENTER;
    y = Graph().ViewPortY() - (int)(0.75 * TextOffset());
    break;
  case AxisLeft:
    assert((direction == TopBottom) || (direction == BottomTop));
    //wjrx mantis 2338 rotate never worked
    //dRotate = 90;
    align = CLabelPoint::LA_BOTTOM_RIGHT;
    x = Graph().ViewPortX();
    break;
  case AxisRight:
    assert((direction == TopBottom) || (direction == BottomTop));
    //wjrx mantis 2338 rotate never worked
    //dRotate = 90;
    align = CLabelPoint::LA_BOTTOM_LEFT;
    x = Graph().ViewPortX() + Graph().ViewPortWidth();
    break;
  default:
    assert(false);
    break;
  };

  // calculate the starting point for drawing the label to avoid clipping
  // wjrx mantis 2864
  // This is not the ultimate fix for this problem.
  //
  int xshift= 0;
  int yshift= 0;
  int width= QFontMetrics( QFont() ).size( 0, sLabel ).width();
  int height= QFontMetrics( QFont() ).size( 0, sLabel ).height();
  switch (align)
  {
    case CLabelPoint::LA_TOP_LEFT:
      xshift= 0;
      yshift= height;
      break;
    case CLabelPoint::LA_CENTER_LEFT:
      xshift= 0;
      yshift= height/2;
      break;
    case CLabelPoint::LA_BOTTOM_LEFT:
      xshift= 0;
      yshift= 0;
      break;
    case CLabelPoint::LA_CENTER:
      xshift= width/2;
      yshift= height/2;
      break;
    case CLabelPoint::LA_TOP_RIGHT:
      xshift= width;
      yshift= height;
    case CLabelPoint::LA_CENTER_RIGHT:
      xshift= width;
      yshift= height/2;
      break;
      break;
    case CLabelPoint::LA_BOTTOM_RIGHT:
      xshift= width;
      yshift= 0;
      break;
    default:
      xshift= 0;
      yshift= 0;
      break;
  }

  switch(direction)
  {
  case TopBottom:
    y = Graph().ViewPortY() + int( (1. - dFraction) * Graph().ViewPortHeight() );
    if ( y - yshift <= 0 ) y= yshift+1;
    break;
  case BottomTop:
    y = Graph().ViewPortY() + int( dFraction * Graph().ViewPortHeight() );
    if ( y - yshift <= 0 ) y= yshift+1;
    break;
  case LeftRight:
    x = Graph().ViewPortX() + int( dFraction * Graph().ViewPortWidth() );
    if ( x - xshift <= 0 ) x= xshift+1;
    break;
  case RightLeft:
    x = Graph().ViewPortX() + int( (1. - dFraction)*Graph().ViewPortWidth() );
    if ( x - xshift <= 0 ) x= xshift+1; // wjrx mantis 2864
    break;
  default:
    assert(false);
    break;
  };

  // Set draw def
  CDrawDef draw_def;
  draw_def.SetDepthLayer2D(GRAPH_LAYER_BORDER);
  draw_def.Visible(TRUE);
  draw_def.TextColor(qRgb(255,255,255));
  draw_def.LineStipple(FALSE);
  draw_def.LineWidth(1);
  draw_def.PointSize(2);

  // Draw the label point
  CLabelPoint label_point(geo::CPoint(x,y), sLabel, QFont(), align);
  label_point.SetRotate(dRotate);
  DrawObject(label_point, draw_def);
}

void C2DGraphParentBase::DrawYLabels()
{
  if((Graph().LineSize() > 0) &&
     (Graph().MinX() < Graph().MaxX()) &&
     (Graph().MaxY().Valid()) &&
     (Graph().MinY().Valid()))
  {
    int iFactor = ScaleFactor(Graph().MaxY().Value(), Graph().MinY().Value());

    // Print Yaxis description
    QString sAxis(""); 
    if (iFactor != 0)
      sAxis = QString(" : (x 10e%1)").arg(iFactor);

    sAxis = QString(YLabel() + sAxis + "[" + YUnit() + "]");
    DrawAxisLabel(sAxis, YAxis(), GetTitleFontY());
    
    // Walk over Y-axis
    DIA_ASSERT(Graph().GridX() != 0);

    int iDivision = int( 0.5 + (Graph().MaxY().Value() - Graph().MinY().Value()) / Graph().GridY() ); //wjrx mantis 2664

    for (int i = 0; i < iDivision + 1; i++)
    {
      // Calculate value and location
      double dValue = Graph().MinY().Value() + (Graph().GridY() * i);
      dValue *= pow( (double)10., -iFactor );
      QString sValue;
      {
        double fabVal= fabs(dValue);
        int prec;
        if ( fabVal < 10 ) prec= 3; //wjrx mantis 2834
        else if ( fabVal < 100 ) prec= 2;
        else prec= 1;
        sValue = QString("%1").arg(dValue,0,'f',prec);
      }
      DrawLabel(sValue, (double)i/(double)iDivision, YDirection(), YAxis());
    }	
  }
}

int C2DGraphParentBase::ScaleFactor(const double & Maximum, const double & Minimum)
{
  // New implementation wjrx mantis 2559
  // Improved for wjrx mantis 2863
  // Improved for wjrx mantis 3719
  //
  double range= fabs(Maximum - Minimum);

  if ( range == 0.0 ) return 0;

  if ( (Maximum != 0) && (range / fabs(Maximum) < 1e-4) ) return 0;
  if ( (Minimum != 0) && (range / fabs(Minimum) < 1e-4) ) return 0;

  int iFactor = 0;
  while ( range > 10 )
  {
    range /= 10;
    ++iFactor;
  }
  while ( range < 1 )
  {
    range *= 10;
    --iFactor;
  }
  iFactor= ( iFactor/3 ) * 3; //10^3 10^6 etc.
  return iFactor;
}

void C2DGraphParentBase::OnResizeParent()
{
  IOpenGLFrame::OnResizeParent();

  double dLeftOffset = Offset();
  double dRightOffset = Offset();
  double dTopOffset = Offset();
  double dBottomOffset = Offset();
  AdaptOffset(XAxis(), dLeftOffset, dRightOffset, dTopOffset, dBottomOffset);
  AdaptOffset(YAxis(), dLeftOffset, dRightOffset, dTopOffset, dBottomOffset);
  Graph().SetViewPort( int( dLeftOffset ),
            int( dBottomOffset ),
            int( ViewPortWidth() - dRightOffset - dLeftOffset ),
            int( ViewPortHeight() - dTopOffset - dBottomOffset ) );
}

void C2DGraphParentBase::AdaptOffset(TAxis axis,
                   double& dLeftOffset, 
                   double& dRightOffset, 
                   double& dTopOffset,
                   double& dBottomOffset)
{
  switch(axis)
  {
  case AxisTop:
    dTopOffset += 2 * TextOffset();
    break;
  case AxisBottom:
    dBottomOffset += 2 * TextOffset();
    break;
  case AxisLeft:
    dLeftOffset += 2 * TextOffset();
    break;
  case AxisRight:
    dRightOffset += 2 * TextOffset();
    break;
  default:
    assert(false);
    break;
  };	

}

void C2DGraphParentBase::SetupProjectionMatrix() const
{
  glOrtho(0, ViewPortWidth(), 0, ViewPortHeight(), 0, 1);
}
