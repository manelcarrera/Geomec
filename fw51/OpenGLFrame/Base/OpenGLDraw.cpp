 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */


#include "dimple.h"
#include <cmath>

#include <qgl.h>
#include <GL/glu.h>

#include "IDrawDef.h"
#include "IOpenGLFrame.h"
#include "OpenGLArrow.h"
#include "IOpenGLDraw.h"
#include "LabelPoint.h"
#include "OpenGLDraw.h"
#include "DrawVisitor.h"
#include "DrawCountVisitor.h"
#include "ISymbol.h"

#include "IQuadrilateral.h"
#include "ObjectDispatcher.h"
#include "BodyGroup.h"
#include "IPolyLine.h"
#include "ITriangle.h"
#include "ICircle.h"
#include "IConus.h"
#include "ISphere.h"
#include "IBox.h"
#include "IPointElement.h"
#include "Polygon.h"
#include "IInterfaceElement.h"
#include "InterfaceDrawDef.h"


#include "qstring.h"
#include "qcolor.h"

#include "CurveFunction.h"

#define GL_MAXSTRINGLEN 256

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


COpenGLDraw::COpenGLDraw(IOpenGLFrame& frame)
: m_DrawState(CLOSED), m_frame(frame)
{
}

COpenGLDraw::~COpenGLDraw()
{
}

int COpenGLDraw::Count(const geo::IObject &object, const IDrawDef &drawdef) const
{
	CDrawCountVisitor CountHandler(*this, drawdef);

	const_cast<geo::IObject &>(object).Visit(CountHandler);

	return CountHandler.Count();
}

void COpenGLDraw::Draw(const geo::IObject &object, const IDrawDef &drawdef, bool bDrawSteps)
{
	InternalDraw(object, drawdef, bDrawSteps);
    CloseDrawState();
}

void COpenGLDraw::InternalDraw(const geo::IObject &object, const IDrawDef &drawdef, bool bDrawSteps)
{
	if(!drawdef.Visible()) return;

	// If object is empty, we should not try to display it....
	if(object.Empty()) return;


	CDrawVisitor DrawHandler(*this, drawdef, bDrawSteps);

	const_cast<geo::IObject &>(object).Visit(DrawHandler);
}

void COpenGLDraw::OpenDrawState(TDrawState state) const
{
	assert(state != CLOSED); // use CloseDrawState instead

	if(m_DrawState != state)
	{
		if(m_DrawState != CLOSED) CloseDrawState();
		glBegin(state);
		m_DrawState = state;
	}
}

void COpenGLDraw::CloseDrawState() const
{
	if(m_DrawState != CLOSED)
	{
		glEnd();
		m_DrawState = CLOSED;
	}
}

void COpenGLDraw::SetColor(TColor color)
{
	glColor4f(qRed(color) / 256.0,
			  qGreen(color) / 256.0,
			  qBlue(color) / 256.0,
			  qAlpha(color) / 256.0);
}

void COpenGLDraw::DrawVertex(const geo::IPoint &point) 
{
	glVertex3f(point.X(),
			   point.Y(),
			   point.Z());
}

void COpenGLDraw::SetNormal(const geo::IVector &normal) 
{
	assert(fabs(normal.Length() - 1) < EPS); // must be a unit vector
	glNormal3f(normal.X(),
			   normal.Y(),
			   normal.Z());
}

class CHandleSelectionStack
{
public:
	CHandleSelectionStack(const geo::IObject &object, const IDrawDef &drawdef,const COpenGLDraw& draw)
		:m_Draw(draw),m_object(object),m_drawdef(drawdef)
	{
		//see if object is selectable
		if(drawdef.IsSelectable(object))
		{
			if(m_Draw.m_DrawState != COpenGLDraw::CLOSED)
				m_Draw.CloseDrawState();

			m_Pushed = const_cast<IOpenGLFrame&>(m_Draw.Frame()).RegisterSelectableObject(object);

		}
	}

	~CHandleSelectionStack()
	{
		if(m_drawdef.IsSelectable(m_object) && m_Pushed)
		{
			if(m_Draw.m_DrawState != COpenGLDraw::CLOSED)
				m_Draw.CloseDrawState();

			glPopName();
		}
	}

private:
bool  m_Pushed;
const COpenGLDraw& m_Draw;
const geo::IObject & m_object;
const IDrawDef &     m_drawdef;
};



void COpenGLDraw::DrawBodyGroup(const geo::CBodyGroup &BodyGroup, const IDrawDef &drawdef, bool bDrawSteps) const
{
	CHandleSelectionStack hs(BodyGroup,drawdef,*this);

	bool bDrawSurfaces = (drawdef.PolyFillBack() || drawdef.PolyFillFront());

	if(bDrawSurfaces)
	{
		// we only draw the outside surfaces shades
		for(int i = 0; i < BodyGroup.SideSurfaceSize(); i++) DrawSurface(BodyGroup.SideSurface(i), drawdef, bDrawSteps);
	}
	else
	{
		// we need to draw all the bodies
		for(int i = 0; i < BodyGroup.BodySize(); i++) DrawBody(BodyGroup.Body(i), drawdef, bDrawSteps);
	}


}

void COpenGLDraw::DrawPoint(const geo::IPoint &pt, const IDrawDef &drawdef, bool bDrawSteps)  const
{
	CHandleSelectionStack hs(pt,drawdef,*this);
	
	OpenDrawState(POINTS);

	std::vector<TColor> vcColor = drawdef.Color(pt);

	DIA_ASSERT(vcColor.size() == 1);

	SetColor(vcColor[0]);
	DrawVertex(pt);
	if(bDrawSteps) Frame().DrawStep();

    CloseDrawState();
}

/*!
	Draws a single point element. Different from
	DrawPoint( geo::IPoint, ... ) to
	provide CDrawDef::Color() the right type information
	( geo::IPointElement ).
*/
void COpenGLDraw::DrawPoint(const geo::IPointElement &element, const IDrawDef &drawdef, bool bDrawSteps)  const
{
	CHandleSelectionStack hs(element,drawdef,*this);
	
    // Remember current setting of GL_POINT_SMOOTH
    bool bPointSmooth = glIsEnabled(GL_POINT_SMOOTH);

    // Make pointsize larger, to make single points better visible
    // OpenGL rule: glPointSize has to be called before glBegin() is called (inside OpenDrawState())
    // The argument is an approximate diameter of a circle containing all the pixels used to draw the
    // point.
    glPointSize(6.0);
    if (!bPointSmooth) glEnable(GL_POINT_SMOOTH);          // Draw small filled circles instead of squares

	OpenDrawState(POINTS);

	std::vector<TColor> vcColor = drawdef.Color(element);

	DIA_ASSERT(vcColor.size() == 1);

	SetColor(vcColor[0]);

    DrawVertex( element.Point( 0 ) );
    
#if 0
	if ( element.NrOfNodes() == 0 ) {
		DrawVertex( element.Point( 0 ) );
	} else {
		DrawVertex( element.Node( 0 ) );
	}
#endif
	if(bDrawSteps) Frame().DrawStep();

    // Must call CloseDrawState here (it calls glEnd()) else the DIA_ASSERT with GL_INVALID_OPERATION in
    // IOpenGLFrame::DeleteList will fail
    CloseDrawState();

    // Give pointsize it's original value back
	glPointSize(drawdef.PointSize());

    // Restore original setting of GL_POINT_SMOOTH if it was off
    if (! bPointSmooth) glDisable(GL_POINT_SMOOTH);
}


void COpenGLDraw::DrawLine(const geo::ILine &line, const IDrawDef &drawdef, bool bDrawSteps)  const
{
  CHandleSelectionStack hs(line,drawdef,*this);

  int nNodeSize = line.NrOfPoints();

  std::vector<TColor> vcColor = drawdef.Color(line);
  assert( vcColor.size() == nNodeSize );

	//rigth drawing

  if ( !drawdef.CurvedDraw())
	{
		OpenDrawState(LINES);
		for ( int i = 0 ; i < nNodeSize ; i++) {
			SetColor( vcColor[i] );
			DrawVertex( line.Point( i ) );
		}
    		CloseDrawState();
	}
  else
  {
  switch ( line.Order() ) {
  case 1:
    {
    // LINE DEGREE 1
    OpenDrawState(LINES);
    for ( int i = 0; i < nNodeSize; i++ ) {
      SetColor( vcColor[i] );
      DrawVertex( line.Point( i ) );
    }
    CloseDrawState();
    break;
    }
  case 2:
    {
    // LINE DEGREE 2
    double ctrlpoints[3][3];
    setCoordinatesVector1d(&ctrlpoints[0][0], 3, line, &coef_LineD2[0][0], 3);
    glMap1d(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 3, &ctrlpoints[0][0]);

    double color_ctrlpoints[3][4];
    setColorVector1d(&color_ctrlpoints[0][0], 3, vcColor, &coef_LineD2[0][0], 3);
    glMap1d(GL_MAP1_COLOR_4, 0.0, 1.0, 4, 3, &color_ctrlpoints[0][0]);
    glEnable(GL_MAP1_VERTEX_3);
    glEnable(GL_MAP1_COLOR_4);
    OpenDrawState(LINES);
    for (int i=0; i<=CURVE_SEGMENTATION; i++) {
      glEvalCoord1d((double)i/(double)CURVE_SEGMENTATION);
    }
    CloseDrawState();
    glDisable(GL_MAP1_VERTEX_3);
    glDisable(GL_MAP1_COLOR_4);
    break;
    }
  case 3:
    {
    // LINE DEGREE 3
    double ctrlpoints[4][3];
    setCoordinatesVector1d(&ctrlpoints[0][0], 4, line, &coef_LineD3[0][0], 4);
    glMap1d(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints[0][0]);

    double color_ctrlpoints[4][4];
    setColorVector1d(&color_ctrlpoints[0][0], 4, vcColor, &coef_LineD3[0][0], 4);
    glMap1d(GL_MAP1_COLOR_4, 0.0, 1.0, 4, 4, &color_ctrlpoints[0][0]);
    glEnable(GL_MAP1_VERTEX_3);
    glEnable(GL_MAP1_COLOR_4);
    OpenDrawState(LINES);
    for (int i=0; i<=CURVE_SEGMENTATION; i++) {
      glEvalCoord1d((double)i/(double)CURVE_SEGMENTATION);
    }
    CloseDrawState();
    glDisable(GL_MAP1_VERTEX_3);
    glDisable(GL_MAP1_COLOR_4);
    break;
    }
  default:
    ;//! \todo support lines with 5 nodes?
  }


  if( bDrawSteps ) Frame().DrawStep();
  }
}

void COpenGLDraw::DrawPolyLine(const geo::IPolyLine &line, const IDrawDef &drawdef, bool bDrawSteps) const
{
	CHandleSelectionStack hs(line,drawdef,*this);

	OpenDrawState(LINES);
	
	for ( int i = 0; i < line.LineSize(); i++ ) {
		int nNodeSize = line.Line( i ).NrOfPoints();


		std::vector<TColor> vcColor = drawdef.Color( line.Line( i ) );
	
		assert( vcColor.size() == nNodeSize );

		for ( int j = 0; j < nNodeSize; j++ ) {
			SetColor( vcColor[j] );
			DrawVertex( line.Line( i ).Point( j ) );
		}
		if( bDrawSteps ) Frame().DrawStep();
	}
	CloseDrawState();
}

void COpenGLDraw::DrawTriangle(const geo::ITriangle &triangle, const IDrawDef &drawdef, bool bDrawSteps) const
{
  CHandleSelectionStack hs(triangle,drawdef,*this);

  std::vector<TColor> vcColor = drawdef.Color(triangle);

  //int nNodeSize = triangle.NrOfPoints();

  if ( !drawdef.CurvedDraw())
	{
		OpenDrawState( POLYGON );

		SetNormal( triangle.Normal() );//wjrx mantis 2386

		for (int i = 0 ; i < 3*triangle.Order() ; i++) {
			SetColor( vcColor[i] );
			DrawVertex( triangle.Point( i ) );
		}
    		CloseDrawState();
	}
  else
  {
  switch ( triangle.Order() ) {
  case 1:
    {

      // TRIANGLE DEGREE 1
      OpenDrawState( TRIANGLES );
      SetNormal( triangle.Normal() );//wjrx mantis 2386
      DIA_ASSERT( vcColor.size() == 3 );

      for (int i = 0 ; i < 3 ; i++ ) {
        SetColor( vcColor[i] );
	      DrawVertex( triangle.Point( i ) );
      }
      CloseDrawState();
      break;
    }
  case 2:
    {
      // TRIANGLE DEGREE 2
      double ctrlpoints[3][3][3];
      setCoordinatesVector2d(&ctrlpoints[0][0][0], 3, 3, triangle, &coef_TriangleD2[0][0], 6);
      glMap2d(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 3, 0.0, 1.0, 9, 3, &ctrlpoints[0][0][0]);

      double color_ctrlpoints[3][3][4];
      setColorVector2d(&color_ctrlpoints[0][0][0], 3, 3, vcColor, &coef_TriangleD2[0][0], 6);
      glMap2d(GL_MAP2_COLOR_4, 0.0, 1.0, 4, 3, 0.0, 1.0, 12, 3, &color_ctrlpoints[0][0][0]);
      glEnable(GL_MAP2_VERTEX_3);
      glEnable(GL_MAP2_COLOR_4);

      if ( drawdef.PolyFillBack() || drawdef.PolyFillFront() ) {
        // FILL DEFINITION
        // FILL THE TRIANGLE
        for (int i=0; i<CURVE_SEGMENTATION; i++) {
          glBegin(GL_QUAD_STRIP);
          for (int j=0; i+j+1<=CURVE_SEGMENTATION; j++) {
            glEvalCoord2d((double)i/(double)CURVE_SEGMENTATION, (double)j/(double)CURVE_SEGMENTATION);
            glEvalCoord2d((double)(i+1)/(double)CURVE_SEGMENTATION, (double)(j)/(double)CURVE_SEGMENTATION);
          }
          glEnd();
        }
        for (int i=1; i<=CURVE_SEGMENTATION; i++) {
          OpenDrawState(TRIANGLES);
          glEvalCoord2d((double)i/(double)CURVE_SEGMENTATION, (double)(CURVE_SEGMENTATION-i)/(double)CURVE_SEGMENTATION);
          glEvalCoord2d((double)(i-1)/(double)CURVE_SEGMENTATION, (double)(CURVE_SEGMENTATION-i)/(double)CURVE_SEGMENTATION);
          glEvalCoord2d((double)(i-1)/(double)CURVE_SEGMENTATION, (double)(CURVE_SEGMENTATION-i+1)/(double)CURVE_SEGMENTATION);
          CloseDrawState();
        }

        // NOW WE DRAW THE BOUNDARY IN ANOTHER COLOR
      } else {
        // BOUNDARY DEFINITION
        OpenDrawState(LINES);
        for (int i=0; i<=CURVE_SEGMENTATION; i++) {
          glEvalCoord2d((double)i/(double)CURVE_SEGMENTATION, 0);
        }
        CloseDrawState();

        OpenDrawState(LINES);
        for (int i=0; i<=CURVE_SEGMENTATION; i++) {
          glEvalCoord2d(0, (double)i/(double)CURVE_SEGMENTATION);
        }
        CloseDrawState();

        OpenDrawState(LINES);
        for (int i=0; i<=CURVE_SEGMENTATION; i++) {
          glEvalCoord2d((double)(CURVE_SEGMENTATION-i)/(double)CURVE_SEGMENTATION, (double)i/(double)CURVE_SEGMENTATION);
        }
        CloseDrawState();
      }
      glDisable(GL_MAP2_COLOR_4);
      glDisable(GL_MAP2_VERTEX_3);
      break;
    }
  case 3:
    {
      // TRIANGLE DEGREE 3
      double ctrlpoints[4][4][3];
      setCoordinatesVector2d(&ctrlpoints[0][0][0], 4, 4, triangle, &coef_TriangleD3[0][0], 9);
      glMap2d(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 4, 0.0, 1.0, 12, 4, &ctrlpoints[0][0][0]);

      double color_ctrlpoints[4][4][4];
      setColorVector2d(&color_ctrlpoints[0][0][0], 4, 4, vcColor, &coef_TriangleD3[0][0], 9);
      glMap2d(GL_MAP2_COLOR_4, 0.0, 1.0, 4, 4, 0.0, 1.0, 16, 4, &color_ctrlpoints[0][0][0]);
      glEnable(GL_MAP2_VERTEX_3);
      glEnable(GL_MAP2_COLOR_4);

      if ( drawdef.PolyFillBack() || drawdef.PolyFillFront() ) {
        // FILL DEFINITION

        for (int i=0; i<CURVE_SEGMENTATION; i++) {
          glBegin(GL_QUAD_STRIP);
          for (int j=0; i+j+1<=CURVE_SEGMENTATION; j++) {
            glEvalCoord2d((double)i/(double)CURVE_SEGMENTATION, (double)j/(double)CURVE_SEGMENTATION);
            glEvalCoord2d((double)(i+1)/(double)CURVE_SEGMENTATION, (double)(j)/(double)CURVE_SEGMENTATION);
          }
          glEnd();
        }
        for (int i=1; i<=CURVE_SEGMENTATION; i++) {
          OpenDrawState(TRIANGLES);
          glEvalCoord2d((double)i/(double)CURVE_SEGMENTATION, (double)(CURVE_SEGMENTATION-i)/(double)CURVE_SEGMENTATION);
          glEvalCoord2d((double)(i-1)/(double)CURVE_SEGMENTATION, (double)(CURVE_SEGMENTATION-i)/(double)CURVE_SEGMENTATION);
          glEvalCoord2d((double)(i-1)/(double)CURVE_SEGMENTATION, (double)(CURVE_SEGMENTATION-i+1)/(double)CURVE_SEGMENTATION);
          CloseDrawState();
        }

        // NOW WE DRAW THE BOUNDARY IN ANOTHER COLOR
      } else {
        // BOUNDARY DEFINITION

        OpenDrawState(LINES);
        for (int i=0; i<=CURVE_SEGMENTATION; i++) {
          glEvalCoord2d((double)i/(double)CURVE_SEGMENTATION, 0);
        }
        CloseDrawState();

        OpenDrawState(LINES);
        for (int i=0; i<=CURVE_SEGMENTATION; i++) {
          glEvalCoord2d(0, (double)i/(double)CURVE_SEGMENTATION);
        }
        CloseDrawState();

        OpenDrawState(LINES);
        for (int i=0; i<=CURVE_SEGMENTATION; i++) {
          glEvalCoord2d((double)(CURVE_SEGMENTATION-i)/(double)CURVE_SEGMENTATION, (double)i/(double)CURVE_SEGMENTATION);
        }
        CloseDrawState();
      }
      glDisable(GL_MAP2_COLOR_4);
      glDisable(GL_MAP2_VERTEX_3);
      break;
    }
    case 4:
    {
      // TRIANGLE DEGREE 4
      double ctrlpoints[5][5][3];
      setCoordinatesVector2d(&ctrlpoints[0][0][0], 5, 5, triangle, &coef_TriangleD4[0][0], 15);
      glMap2d(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 5, 0.0, 1.0, 15, 5, &ctrlpoints[0][0][0]);

      double color_ctrlpoints[5][5][4];
      setColorVector2d(&color_ctrlpoints[0][0][0], 5, 5, vcColor, &coef_TriangleD4[0][0], 15);
      glMap2d(GL_MAP2_COLOR_4, 0.0, 1.0, 4, 5, 0.0, 1.0, 20, 5, &color_ctrlpoints[0][0][0]);
      glEnable(GL_MAP2_VERTEX_3);
      glEnable(GL_MAP2_COLOR_4);

      if ( drawdef.PolyFillBack() || drawdef.PolyFillFront() ) {
        // FILL DEFINITION

        for (int i=0; i<CURVE_SEGMENTATION; i++) {
          glBegin(GL_QUAD_STRIP);
          for (int j=0; i+j+1<=CURVE_SEGMENTATION; j++) {
            glEvalCoord2d((double)i/(double)CURVE_SEGMENTATION, (double)j/(double)CURVE_SEGMENTATION);
            glEvalCoord2d((double)(i+1)/(double)CURVE_SEGMENTATION, (double)(j)/(double)CURVE_SEGMENTATION);
          }
          glEnd();
        }
        for (int i=1; i<=CURVE_SEGMENTATION; i++) {
          OpenDrawState(TRIANGLES);
          glEvalCoord2d((double)i/(double)CURVE_SEGMENTATION, (double)(CURVE_SEGMENTATION-i)/(double)CURVE_SEGMENTATION);
          glEvalCoord2d((double)(i-1)/(double)CURVE_SEGMENTATION, (double)(CURVE_SEGMENTATION-i)/(double)CURVE_SEGMENTATION);
          glEvalCoord2d((double)(i-1)/(double)CURVE_SEGMENTATION, (double)(CURVE_SEGMENTATION-i+1)/(double)CURVE_SEGMENTATION);
          CloseDrawState();
        }

        // NOW WE DRAW THE BOUNDARY IN ANOTHER COLOR
      } else {
        // BOUNDARY DEFINITION

        OpenDrawState(LINES);
        for (int i=0; i<=CURVE_SEGMENTATION; i++) {
          glEvalCoord2d((double)i/(double)CURVE_SEGMENTATION, 0);
        }
        CloseDrawState();

        OpenDrawState(LINES);
        for (int i=0; i<=CURVE_SEGMENTATION; i++) {
          glEvalCoord2d(0, (double)i/(double)CURVE_SEGMENTATION);
        }
        CloseDrawState();

        OpenDrawState(LINES);
        for (int i=0; i<=CURVE_SEGMENTATION; i++) {
          glEvalCoord2d((double)(CURVE_SEGMENTATION-i)/(double)CURVE_SEGMENTATION, (double)i/(double)CURVE_SEGMENTATION);
        }
        CloseDrawState();
      }
      glDisable(GL_MAP2_COLOR_4);
      glDisable(GL_MAP2_VERTEX_3);
      break;
    }
  default:
    assert( false ); 
  }
 }
  if ( bDrawSteps ) Frame().DrawStep();
}

void COpenGLDraw::DrawBody(const geo::IBody &body, const IDrawDef &drawdef, bool bDrawSteps)  const
{
	CHandleSelectionStack hs(body,drawdef,*this);

	// Retrieve colors first
	int nNodeSize = body.NrOfPoints();
		
	std::vector<TColor> vcColor = drawdef.Color(body);
	assert(vcColor.size() == nNodeSize);

	for( int i = 0; i < body.NrOfFaces(); i++)
	{
		const std::vector<int>& vcPoint = body.FaceNodeIndices( i );

		OpenDrawState(POLYGON);

    if ( body.Face(i).Order() == 1 ) SetNormal( body.Face(i).Normal() ); //wjrx mantis 2405
		for(size_t n = 0; n < vcPoint.size(); n++)
		{
			SetColor(vcColor[ vcPoint[n] ]);
			DrawVertex(body.Point( vcPoint[n] ));
		}

		CloseDrawState();
	}

	 if(bDrawSteps) Frame().DrawStep();
}

void COpenGLDraw::DrawBox(const geo::IBox &box, const IDrawDef &drawdef, bool bDrawSteps)  const
{
	CHandleSelectionStack hs(box,drawdef,*this);

	// Retrieve colors first 
	TPointToColorMap map;
	std::vector<TColor> vcColor = drawdef.Color(box);
	int i;
	for(i = 0; i < box.NrOfPoints(); i++)
		VERIFY(map.insert(TPointToColorMap::value_type(&box.Point(i), vcColor[i])).second);

	// wjrx mantis 2405
	// bool bUseNormal =
	// ((drawdef.PolyFillBack() || drawdef.PolyFillFront()) && m_frame.LightingEnabled());
	bool bUseNormal= 1;

	geo::CVector n;
	for(i = 0; i < box.NrOfFaces(); i++)
	{
		OpenDrawState(POLYGON);

		const geo::IFace* pFace = &box.Face(i);
		if(bUseNormal)
		{
			n = pFace->Normal().UnitVector();
			SetNormal(n);
		}

		for(int j=0;j<pFace->NrOfPoints();j++)
		{
			TPointToColorMap::const_iterator it = map.find(&pFace->Point(j));
			DIA_ASSERT(it != map.end());

			SetColor(it->second);
			DrawVertex(pFace->Point(j));
		}

		CloseDrawState();
	}
	 if(bDrawSteps) Frame().DrawStep();

}

COpenGLDraw::TColor COpenGLDraw::ContrastColor(TColor color) const
{
	int iRed   = 255 - qRed(color);
	int iGreen = 255 - qGreen(color);
	int iBlue  = 255 - qBlue(color);

	return qRgb(iRed, iGreen, iBlue);
}


void COpenGLDraw::DrawLabel(const CLabelPoint &lbp, const IDrawDef &drawdef, bool bDrawSteps)
{
//	assert(drawdef.RequireCompile()==FALSE);  
	if ( lbp.Label().isEmpty() ) return;

	CHandleSelectionStack hs(lbp,drawdef,*this);
	CloseDrawState();

	if(Frame().LightingEnabled())
		glDisable(GL_LIGHTING);

	int NrOfLines = lbp.Label().count( '\n' ) + 1;

	QSize sz = QFontMetrics( lbp.font() ).size( 0, lbp.Label() );
	int textWidth = sz.width();
	int textHeight = sz.height();

	// Instal the fout
//	int textWidth = lbp.LabelPixelWidth()-4;
//	int textHeight = lbp.LabelPixelHeight()-4; //some problems with correct textheight (GDI??)

	//project world to screen (remember the depth = t.second)
	IOpenGLFrame::TScreenPointDepth t = Frame().Project(lbp);

	QPoint ScreenCoor = t.first;

	//reset screen coor acording allignment
	switch(lbp.GetAlignment())
	{
	case CLabelPoint::LA_TOP_LEFT:
		ScreenCoor.setY(ScreenCoor.y()-textHeight);
		break;

	case CLabelPoint::LA_TOP_RIGHT:
		ScreenCoor.setX(ScreenCoor.x()-textWidth);
		ScreenCoor.setY(ScreenCoor.y()-textHeight);
		break;

	case CLabelPoint::LA_BOTTOM_LEFT:
		//default
		break;

	case CLabelPoint::LA_BOTTOM_RIGHT:
		ScreenCoor.setX(ScreenCoor.x()-textWidth);
		break;

	case CLabelPoint::LA_CENTER:
		ScreenCoor.setX(ScreenCoor.x()-int(.5*textWidth));
		ScreenCoor.setY(ScreenCoor.y()-int(.5*textHeight));
		break;

	case CLabelPoint::LA_CENTER_RIGHT:
		ScreenCoor.setX(ScreenCoor.x()-textWidth);
		ScreenCoor.setY(ScreenCoor.y()-int(.5*textHeight));
		break;

	case CLabelPoint::LA_CENTER_LEFT:
		ScreenCoor.setY(ScreenCoor.y()-int(.5*textHeight));
		break;

	}

	//draws a border on the bounding box
	if(drawdef.LabelBorder())
	{
		int count=0;

		while(count < 2)
		{
			geo::CPoint boxCoor(ScreenCoor.x(), ScreenCoor.y(), t.second);
			int bx=3;
			int by=3;
		
			if (count==1)
			{
				//border
				bx=4;
				by=4;
				SetColor(drawdef.LabelBorderOutlineColor());
				OpenDrawState(LINELOOP);
			}
			else
			{
				//polygon
				SetColor(drawdef.LabelBorderInteriorColor());
				OpenDrawState(POLYGON);
			}

			boxCoor.Move(geo::CVector(-bx,-by,0));
			int boxWidth=textWidth + 2*bx;
			int boxHeight=textHeight + 2*by;


			DrawVertex(Frame().UnProject(QPoint(int(boxCoor.X()),int(boxCoor.Y())),t.second));
			DrawVertex(Frame().UnProject(QPoint(int(boxCoor.X())+boxWidth,int(boxCoor.Y())),t.second));
			DrawVertex(Frame().UnProject(QPoint(int(boxCoor.X())+boxWidth,int(boxCoor.Y())+boxHeight),t.second));
			DrawVertex(Frame().UnProject(QPoint(int(boxCoor.X()),int(boxCoor.Y())+boxHeight),t.second));

			CloseDrawState();

			count++;
		}
	}

	glPushAttrib(GL_DEPTH_BUFFER_BIT || GL_STENCIL_BUFFER_BIT );
	
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);
		//glDepthFunc(GL_ALWAYS);
	
	TColor Color;

	//make sure color is not background color
	if(drawdef.TextColor() == Frame().BackgroundColor())
		Color = ContrastColor(drawdef.TextColor());
	else
		Color  = drawdef.TextColor();

	SetColor(Color);
	
	double textRorHeight = textHeight/double(NrOfLines);

	//set y to first row position
	ScreenCoor.setY(ScreenCoor.y() + int( textRorHeight * (NrOfLines-1) ));

	int nPos = 0;
	int nPosStart = 0;
	QString str = lbp.Label();

	//loop over lines
	while(nPos > -1)
	{
		nPos = str.indexOf('\n', nPos + 1);
		QString tmp2;

		//get next string newline
		if(nPos > -1)
			tmp2 = str.mid(nPosStart , nPos-nPosStart);
		else
		{
			if(nPosStart)
				tmp2 = str.mid(nPosStart , str.length()-nPosStart);
			else
				tmp2=str;
		}

		//print next string
		if(tmp2.length() > 0)
		{
			geo::CPoint NewWorldCoor = Frame().UnProject(ScreenCoor,t.second);
			Frame().RenderText(NewWorldCoor.X(), NewWorldCoor.Y(), NewWorldCoor.Z(), tmp2, lbp.font());
			
			//set next row position
			ScreenCoor.setY(ScreenCoor.y()-int( textRorHeight ));
			nPosStart=nPos+1;
		}
	}

	glPopAttrib();

	 if(bDrawSteps) Frame().DrawStep();


	if(Frame().LightingEnabled())
		glEnable(GL_LIGHTING);

}


//void COpenGLDraw::DrawLabel(const CLabelPoint &lbp, const IDrawDef &drawdef) const
//{
//	//alignments are not really what you would expect
//	//direction
//	CloseDrawState();
//
//	double offsetX = (m_pView->m_vPixelRatio.X()) * double(lbp.LabelPixelWidth() + 5);
//	double offsetY = (m_pView->m_vPixelRatio.Y()) * double(lbp.LabelPixelHeight() + 10) / 2.0;
//	
//	if(m_pView->CurrentScene()->Is2DScene())
//	{
//		double diffX = m_pView->CurrentScene()->ScaleLeftTop2D().X() - m_pView->CurrentScene()->ScaleRightBottom2D().X();
//		double diffY = m_pView->CurrentScene()->ScaleLeftTop2D().Y() - m_pView->CurrentScene()->ScaleRightBottom2D().Y();
//		offsetX = offsetX * _copysign(1.0, -diffX);
//		offsetY = offsetY * _copysign(1.0, diffY);
//	}
//
//	TColor Color;
//
//	if(drawdef.TextColor() == m_pView->CurrentScene()->BackgroundColor())
//		Color = ContrastColor(drawdef.TextColor());
//	else
//		Color  = drawdef.TextColor();
//
//	SetColor(Color);
//	
//	switch(lbp.GetAlignment())
//	{
//	case CLabelPoint::LA_TOP_LEFT://LA_TOP_LEFT
//		if (lbp.GetRotate() == 0)
//			glRasterPos3d(lbp.X() - offsetX, lbp.Y(), lbp.Z());
//		else
//			m_pView->glPrintRotate(lbp.X() - offsetX, lbp.Y(), lbp.Z(), lbp.GetRotate(), drawdef.Font(), lbp.Label());
//		break;
//	case CLabelPoint::LA_TOP_RIGHT://LA_TOP_RIGHT
//		if (lbp.GetRotate() == 0)
//			glRasterPos3d(lbp.X(), lbp.Y(), lbp.Z());
//		else
//			m_pView->glPrintRotate(lbp.X(), lbp.Y(), lbp.Z(), lbp.GetRotate(), drawdef.Font(), lbp.Label());
//		break;
//	case CLabelPoint::LA_BOTTOM_LEFT://LA_BOTTOM_LEFT
//		if (lbp.GetRotate() == 0)
//			glRasterPos3d(lbp.X() - offsetX, lbp.Y() - offsetY, lbp.Z());
//		else
//			m_pView->glPrintRotate(lbp.X() - offsetX, lbp.Y() - offsetY, lbp.Z(), lbp.GetRotate(), drawdef.Font(), lbp.Label());
//		break;
//	case CLabelPoint::LA_BOTTOM_RIGHT://LA_BOTTOM_RIGHT
//		if (lbp.GetRotate() == 0)
//			glRasterPos3d(lbp.X(), lbp.Y() - offsetY, lbp.Z());
//		else
//			m_pView->glPrintRotate(lbp.X(), lbp.Y() - offsetY, lbp.Z(), lbp.GetRotate(), drawdef.Font(), lbp.Label());
//		break;
//
//	case CLabelPoint::LA_CENTER://LA_CENTER  
//		if (lbp.GetRotate() == 0)
//			glRasterPos3d(lbp.X() - .5*offsetX, lbp.Y() - 0.5*offsetY, lbp.Z());
//		else
//			m_pView->glPrintRotate(lbp.X() - .5*offsetX, lbp.Y() - 0.5*offsetY, lbp.Z(), lbp.GetRotate(), drawdef.Font(), lbp.Label());
//		break;
//
//	}
//
//	if (lbp.GetRotate() == 0)
//		m_pView->glPrint(drawdef.Font(), lbp.Label());
//
//	 if(bDrawSteps) Frame().DrawStep();
//}

void COpenGLDraw::SetOpenGLAtrib(const IDrawDef &drawdef) const
{
	GLbitfield mask = 
				GL_DEPTH_BUFFER_BIT | 
				GL_ENABLE_BIT |
				GL_LINE_BIT |
				GL_POLYGON_BIT  |
				GL_TRANSFORM_BIT |
				GL_STENCIL_BUFFER_BIT |
				GL_VIEWPORT_BIT;


	glPushAttrib(mask); //store the current atribute state

	if(Frame().Dimension() == IOpenGLFrame::DIM_2D)
		glDisable(GL_DEPTH_TEST);
	else
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(drawdef.DepthFunc());
	}
	
	if(drawdef.UseStencilBuffer())
	{
//		DIA_ASSERT(drawdef.StencilFuncRef() < (pow(2,Frame().StencilBufferDepth())));
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(drawdef.StencilFuncFunc(),drawdef.StencilFuncRef(),drawdef.StencilFuncMask());
		glStencilOp(drawdef.StencilOpFail(),drawdef.StencilOpZFail(),drawdef.StencilOpZPass());
	}
	else
		glDisable(GL_STENCIL_TEST);


	glLineWidth(drawdef.LineWidth());
	glPointSize(drawdef.PointSize());

	if(drawdef.LineStipple())
	{
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(drawdef.LineStippleFactor(),drawdef.LineStipplePattern());
	}
	else
	{
		glDisable(GL_LINE_STIPPLE);
	}
	
	
	if(drawdef.PolyFillFront() && drawdef.PolyFillBack())
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	else{
		if(drawdef.PolyFillFront())
		{
			glPolygonMode(GL_FRONT,GL_FILL);
		}
		else if(drawdef.PolyFillBack())
		{
			glPolygonMode(GL_BACK,GL_FILL);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		}
	}

	if(drawdef.PolyDrawFront() && drawdef.PolyDrawBack() )
	{	
		glDisable(GL_CULL_FACE);
		glCullFace(GL_FRONT_AND_BACK);
	}
	else
	{
		if(drawdef.PolyDrawFront())
		{
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
		}
		else if(drawdef.PolyDrawBack())
		{
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}
		else
		{
			glDisable(GL_CULL_FACE);
		}
	}
}

void COpenGLDraw::RestoreOpenGLAtrib() const
{
	glPopAttrib(); //restore the previous atribute state
}

void COpenGLDraw::DrawSurfaceFace(const geo::IFace &face,
  const IDrawDef &drawdef,
  TPointNormalMap &mpPointNormal,
  bool bNeedNormals,
  bool bPlanar,
  geo::IVector &Normal) const
{
	CHandleSelectionStack hs(face,drawdef,*this);

	std::vector<TColor> vcColor = drawdef.Color(face);
	DIA_ASSERT(vcColor.size() == face.NrOfPoints() || vcColor.size() == 1);

	for(long i = 0; i < face.NrOfPoints(); i++)
	{
		const geo::IPoint &p = face.Point(i);

		TColor col;
		if(vcColor.size() == 1) col = vcColor[0];
		else col = vcColor[i];

		if(bNeedNormals)
		{
			if(!bPlanar)
			{
				TPointNormalMap::iterator it = mpPointNormal.find(&p);
				assert(it != mpPointNormal.end());
				geo::IVector &vec = it->second;
				SetNormal(vec);
			}
			else SetNormal(Normal);
		}

		SetColor(col);
		DrawVertex(p);
	}

}
 
// In this function the right colors are selected to draw the surface with and the mean normal in each point is calculated 
// for shading effects.
void COpenGLDraw::DrawSurface(const geo::ISurface &surface, const IDrawDef &drawdef, bool bDrawSteps) const
{
	CHandleSelectionStack hs(surface,drawdef,*this);

	int iFaceSize = surface.FaceSize();

	// Create a map with the mean normal in each point
	TPointNormalMap mpPointNormal;
	int iPointSize = surface.PointSize();

	// wjrx mantis 2385
	//bool bNeedNormals = drawdef.PolyFillBack() || drawdef.PolyFillFront();
	//wjrx mantis 2045
	bool bNeedNormals = 1;

	bool bPlanar = surface.Planar();
	geo::CVector Normal;

	if(bNeedNormals)
	{
		if(!bPlanar)
		{
			for(int i = 0; i < iPointSize; i++)
			{
				Normal = surface.AverageNormalInPoint(surface.Point(i)).UnitVector();
				// Add to map
				VERIFY(mpPointNormal.insert(std::make_pair(&surface.Point(i), Normal)).second);
			}
		}
		else
		{
			Normal = surface.Face(0).Normal().UnitVector();
		}
	}

	for(int n = 0; n < iFaceSize; n++)
	{
		OpenDrawState(POLYGON);
	
		const geo::IFace &face = surface.Face(n);
		DrawSurfaceFace(face, drawdef, mpPointNormal, bNeedNormals, bPlanar, Normal);

		 if(bDrawSteps) Frame().DrawStep();

		CloseDrawState();
	}

}

void COpenGLDraw::DrawCircle(const geo::ICircle &circle, const IDrawDef &drawdef, bool bDrawSteps) const
{
	CHandleSelectionStack hs(circle,drawdef,*this);

	DIA_ASSERT(circle.Radius() > 0);
	std::vector<TColor> vcColor = drawdef.Color(circle);
	DIA_ASSERT(vcColor.size() == 1);

	SetColor(vcColor[0]);

	geo::CPtrArray<geo::IPoint> points;
	circle.GetPointsOnCircumference(points, drawdef.CircleNrOfSegments());

	OpenDrawState(LINES);
	for(size_t i = 0; i < points.Size(); i++)
		DrawVertex(points.Object(i));

	CloseDrawState();
	points.ClearAndDelete();

	 if(bDrawSteps) Frame().DrawStep();

}

void COpenGLDraw::DrawConus(const geo::IConus &conus, const IDrawDef &drawdef, bool /*bDrawSteps*/) const
{
	CHandleSelectionStack hs(conus,drawdef,*this);

	conus.AssertValid();
	std::vector<TColor> vcColor = drawdef.Color(conus);
	DIA_ASSERT(vcColor.size() >= 1); 

	SetColor(vcColor[0]);


	CloseDrawState();
	glPushMatrix();
		const geo::IPoint& p = conus.BasePoint(); // + conus.Direction().UnitVector()*conus.Height();
		const geo::IVector& dir = conus.Direction();
		double angle = dir.AngleDeg(geo::CVector::Zaxis);
		geo::CVector rot_vec = conus.Direction().CrossProduct(geo::CVector::Zaxis);
		glTranslated(p.X(),p.Y(),p.Z());		
		if(fabs(angle) > EPS)
		{
			glRotated(-angle,rot_vec.X(),rot_vec.Y(),rot_vec.Z());
		}
		
		GLUquadricObj *quadObj = gluNewQuadric (); 
		gluQuadricDrawStyle (quadObj, GLU_FILL); 
		gluCylinder (quadObj, conus.Radius(),conus.Radius()*conus.Ratio(),conus.Height(),20,20);
		gluDeleteQuadric(quadObj);
	glPopMatrix();

}


//void COpenGLDraw::DrawConus(const geo::IConus &conus, const IDrawDef &drawdef) const
//{
//	conus.AssertValid();
//	std::vector<TColor> vcColor = drawdef.Color(conus);
//	DIA_ASSERT(vcColor.size() == 1);
//
//	SetColor(vcColor[0]);
//
//	geo::CCircle c1(conus.BasePoint(),conus.Radius(),conus.Direction());
//	DrawCircle(c1,drawdef);
//
//	long NrOfSegments=drawdef.CircleNrOfSegments();
//	if(NrOfSegments % 2 !=0)
//		NrOfSegments++;
//
//	geo::CPtrArray<geo::IPoint> points1;
//	c1.GetPointsOnCircumference(points1, NrOfSegments);
//
//	geo::CPtrArray<geo::IPoint> points2;
//	
//
//	if(conus.Ratio() != 0)
//	{
//		geo::CCircle c2(conus.BasePoint(),conus.Radius()*conus.Ratio(),conus.Direction());
//		c2.Move(conus.Direction().UnitVector()*conus.Height());
//		DrawCircle(c2,drawdef);
//		c2.GetPointsOnCircumference(points2, NrOfSegments);
//		for(int i=0;i<NrOfSegments-1;i++)	
//		{
//			OpenDrawState(POLYGON);
//				DrawVertex(points1.Object(i));
//				DrawVertex(points1.Object(i+1));
//				DrawVertex(points2.Object(i+1));
//				DrawVertex(points2.Object(i));
//			CloseDrawState();
//		}
//	}
//	else
//	{
//		geo::CPoint top(conus.BasePoint() + conus.Direction().UnitVector()*conus.Height());
//
//		for(int i=0;i<NrOfSegments-1;i++)	
//		{
//			OpenDrawState(POLYGON);
//				DrawVertex(points1.Object(i));
//				DrawVertex(points1.Object(i+1));
//				DrawVertex(top);
//			CloseDrawState();
//		}
//
//	}
//
//	points1.ClearAndDelete();
//	points2.ClearAndDelete();
//}

void COpenGLDraw::DrawSphere(const geo::ISphere &sphere, const IDrawDef &drawdef, bool /*bDrawSteps*/) const
{
	CHandleSelectionStack hs(sphere,drawdef,*this);

	sphere.AssertValid();
	std::vector<TColor> vcColor = drawdef.Color(sphere);
	DIA_ASSERT(vcColor.size() == 1);
	SetColor(vcColor[0]);

	glPushMatrix();
	const geo::IPoint& p = sphere.MidPoint();
	glTranslated(p.X(),p.Y(),p.Z());
	GLUquadricObj *quadObj = gluNewQuadric (); 
	gluQuadricDrawStyle (quadObj, GLU_FILL);
	gluSphere (quadObj, sphere.Radius(), 8, 8);
	gluDeleteQuadric(quadObj);
	glPopMatrix();

}

//Draw a quadrilateral
void COpenGLDraw::DrawQuadrilateral(const geo::IQuadrilateral &quad, const IDrawDef &drawdef, bool bDrawSteps) const
{
  DrawFace( quad, drawdef, bDrawSteps );
}

//Draw a face
void COpenGLDraw::DrawFace( const geo::IFace& face, const IDrawDef& drawdef, bool bDrawSteps ) const
{
  int i;
  CHandleSelectionStack hs(face,drawdef,*this);

  std::vector<TColor> vcColor = drawdef.Color(face);

  int nNodeSize = face.NrOfPoints();
  assert( vcColor.size() == nNodeSize );

  if ( !drawdef.CurvedDraw())
  {
    OpenDrawState( POLYGON );
    if ( face.Order() == 1 ) SetNormal( face.Normal() );// wjrx mantis 2405
    for( i = 0 ; i < 4*face.Order() ; i++ ) {
      SetColor( vcColor[i] ); 
      DrawVertex( face.Point( i ) );
    }
    SetColor( vcColor[0] );
    DrawVertex( face.Point( 0 ) );
    CloseDrawState();
  }
  else
  {
  switch ( face.Order() ) {
  case 1:
    {
      // ALL DEGREE 1 FACES AND ALSO QUAD DEGREE 1
      OpenDrawState( POLYGON );
      SetNormal( face.Normal() );// wjrx mantis 2405
      for( i = 0 ; i < nNodeSize ; i++ ) {
        SetColor( vcColor[i] );
        DrawVertex( face.Point( i ) );
      }
      SetColor( vcColor[0] );
      DrawVertex( face.Point( 0 ) );
      CloseDrawState();
      break;
    }
  case 2:
    {
      // QUAD DEGREE 2
      double ctrlpoints[3][3][3];
      setCoordinatesVector2d(&ctrlpoints[0][0][0], 3, 3, face, &coef_QuadD2[0][0], 8 );
      glMap2d(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 3, 0.0, 1.0, 9, 3, &ctrlpoints[0][0][0]);

      double color_ctrlpoints[3][3][4];
      setColorVector2d(&color_ctrlpoints[0][0][0], 3, 3, vcColor, &coef_QuadD2[0][0], 8);
      glMap2d(GL_MAP2_COLOR_4, 0.0, 1.0, 4, 3, 0.0, 1.0, 12, 3, &color_ctrlpoints[0][0][0]);

      glEnable(GL_MAP2_VERTEX_3);
      glEnable(GL_MAP2_COLOR_4);

      if ( drawdef.PolyFillBack() || drawdef.PolyFillFront() ) {
        // FILL DEFINITION

        glMapGrid2f(CURVE_SEGMENTATION, 0.0, 1.0, CURVE_SEGMENTATION, 0.0, 1.0);
        glEvalMesh2(GL_FILL, 0, CURVE_SEGMENTATION, 0, CURVE_SEGMENTATION);

      } else {
        // BOUNDARIES DEFINITION

        OpenDrawState(LINES);
        for ( i=0; i<=CURVE_SEGMENTATION; i++) {
          glEvalCoord2d((double)i/(double)CURVE_SEGMENTATION, 0);
        }
        CloseDrawState();

        OpenDrawState(LINES);
        for ( i=0; i<=CURVE_SEGMENTATION; i++) {
          glEvalCoord2d(0, (double)i/(double)CURVE_SEGMENTATION);
        }
        CloseDrawState();

        OpenDrawState(LINES);
        for ( i=0; i<=CURVE_SEGMENTATION; i++) {
          glEvalCoord2d((double)i/(double)CURVE_SEGMENTATION, 1);
        }
        CloseDrawState();

        OpenDrawState(LINES);
        for ( i=0; i<=CURVE_SEGMENTATION; i++) {
          glEvalCoord2d(1, (double)i/(double)CURVE_SEGMENTATION);
        }
        CloseDrawState();
      }                
      glDisable(GL_MAP2_COLOR_4);
      glDisable(GL_MAP2_VERTEX_3);
      break;
    }
  case 3:
    {
      // QUAD DEGREE 3
      double ctrlpoints[4][4][3];
      setCoordinatesVector2d(&ctrlpoints[0][0][0], 4, 4, face, &coef_QuadD3[0][0], 12);
      glMap2d(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 4, 0.0, 1.0, 12, 4, &ctrlpoints[0][0][0]);

      double color_ctrlpoints[4][4][4];
      setColorVector2d(&color_ctrlpoints[0][0][0], 4, 4, vcColor, &coef_QuadD3[0][0], 12);
      glMap2d(GL_MAP2_COLOR_4, 0.0, 1.0, 4, 4, 0.0, 1.0, 16, 4, &color_ctrlpoints[0][0][0]);

      glEnable(GL_MAP2_VERTEX_3);
      glEnable(GL_MAP2_COLOR_4);

      if ( drawdef.PolyFillBack() || drawdef.PolyFillFront() ) {
        // FILL DEFINITION

        glMapGrid2f(CURVE_SEGMENTATION, 0.0, 1.0, CURVE_SEGMENTATION, 0.0, 1.0);
        glEvalMesh2(GL_FILL, 0, CURVE_SEGMENTATION, 0, CURVE_SEGMENTATION);

      } else {
        // BOUNDARIES DEFINITION

        OpenDrawState(LINES);
        for ( i=0; i<=CURVE_SEGMENTATION; i++) {
          glEvalCoord2d((double)i/(double)CURVE_SEGMENTATION, 0);
        }
        CloseDrawState();

        OpenDrawState(LINES);
        for ( i=0; i<=CURVE_SEGMENTATION; i++) {
          glEvalCoord2d(0, (double)i/(double)CURVE_SEGMENTATION);
        }
        CloseDrawState();

        OpenDrawState(LINES);
        for ( i=0; i<=CURVE_SEGMENTATION; i++) {
          glEvalCoord2d((double)i/(double)CURVE_SEGMENTATION, 1);
        }
        CloseDrawState();

        OpenDrawState(LINES);
        for ( i=0; i<=CURVE_SEGMENTATION; i++) {
          glEvalCoord2d(1, (double)i/(double)CURVE_SEGMENTATION);
        }
        CloseDrawState();
      }
      glDisable(GL_MAP2_COLOR_4);
      glDisable(GL_MAP2_VERTEX_3);
      break;
    }
  default:
    assert( false ); //! \todo no higher order?
  }
  }
  OpenDrawState(POINTS);

  for ( int j = nNodeSize; j < face.NrOfPoints(); j++) {
    SetColor( vcColor[j] );
    DrawVertex( face.Point( j ) );
  }

  CloseDrawState();

  if ( bDrawSteps ) Frame().DrawStep();
}


extern "C"
{
#ifdef WIN32
	void CALLBACK VertexCallBackFunc(void* pObject, void* drawdef)
#else // !WIN32
	void VertexCallBackFunc(void* pObject, void* drawdef)
#endif  // WIN32
	{
		GLdouble *pt = (GLdouble*)pObject;
		if(pt != 0)
		{
			geo::CPoint p(pt[0], pt[1], pt[2]);
			IDrawDef *pDrawDef = (IDrawDef*) drawdef;
			std::vector<IDrawDef::TColor> vcColor = pDrawDef->Color(p);
			COpenGLDraw::SetColor(vcColor[0]);
			glVertex3dv(pt);
		}
		else
		{
			assert(false);
			return;
		}
	}
}

#ifdef WIN32
typedef void (__stdcall * TessFunction)();
#else // !WIN32
typedef void (* TessFunction)();
#endif  // WIN32


/*!
  Draws an interface element by drawing the two IElement parts.
*/
void COpenGLDraw::DrawInterface(const geo::IInterfaceElement &interfaceElement, const IDrawDef &drawdef, bool bDrawSteps)
{
	CInterfaceDrawDef inf_draw_def( interfaceElement, drawdef );
	//first draw
	Draw( interfaceElement.Front(), inf_draw_def, bDrawSteps);
	//second draw
	Draw( interfaceElement.Back(), inf_draw_def, bDrawSteps);
}


//draws polygons with holes
void COpenGLDraw::DrawPolygonArr(const geo::CPolygonPtrArray &poly_arr, const IDrawDef &drawdef, bool bDrawSteps) const
{
	CHandleSelectionStack hs(poly_arr,drawdef,*this);

	long i;

	if(drawdef.PolyFillBack() || drawdef.PolyFillFront())
	{

		GLUtesselator *tess;

		std::vector<GLdouble*> m_Points;

		tess = gluNewTess();
		gluTessCallback(tess, GLU_TESS_BEGIN, (TessFunction) glBegin);
		gluTessCallback(tess, GLU_TESS_VERTEX_DATA, (TessFunction) VertexCallBackFunc);
		gluTessCallback(tess, GLU_TESS_END, glEnd);

		gluTessBeginPolygon(tess, (void*)&drawdef);
    for(size_t j = 0; j <poly_arr.Size(); j++)
		{

			const geo::IPolygon* poly = &(poly_arr.Object(j));
			SetNormal(poly->Normal());
   
			gluTessBeginContour(tess);
			
			for( i = 0; i < poly->NrOfPoints(); i++)
			{
				const geo::IPoint* p;
				if(poly->IsHole())
					 p = &poly->Point(i);
				else
					 p = &poly->Point(poly->NrOfPoints() - i-1);	//clock-wise

				GLdouble* pt = new GLdouble[3];
				pt[0] = p->X();
				pt[1] = p->Y();
				pt[2] = p->Z();

				m_Points.push_back(pt);

				gluTessVertex(tess, pt, pt);
				
			}
			gluTessEndContour(tess);
		}

		 if(bDrawSteps) Frame().DrawStep();

		gluTessEndPolygon(tess); 

		gluDeleteTess(tess);

		while(m_Points.size())
		{
			delete m_Points[0];
			m_Points.erase(m_Points.begin());
		}
	}
	else
	{

		OpenDrawState(LINELOOP);
		
		for(size_t j=0 ;j <poly_arr.Size();j++)
		{
			const geo::IPolygon* poly = &(poly_arr.Object(j));
			std::vector<TColor> vcColor = drawdef.Color(*poly);
			DIA_ASSERT(vcColor.size() == poly->NrOfPoints());

			for(i = 0 ;i < poly->NrOfPoints() ; i++)
			{
				SetColor(vcColor[i]);
				DrawVertex(poly->Point(i));
			}
		}
		if(bDrawSteps) Frame().DrawStep();
		CloseDrawState();
	}

}


void COpenGLDraw::DrawPolygon(const geo::IPolygon &poly, const IDrawDef &drawdef, bool bDrawSteps) const
{
	CHandleSelectionStack hs(poly,drawdef,*this);

	if(drawdef.PolyFillBack() || drawdef.PolyFillFront())
	{
		std::vector<TColor> vcColor = drawdef.Color(poly);
		DIA_ASSERT(vcColor.size() == poly.NrOfPoints());

		SetNormal(poly.Normal());
		GLUtesselator *tess;

		std::vector<GLdouble*> m_Points;

		tess = gluNewTess();
		gluTessCallback(tess, GLU_TESS_BEGIN, (TessFunction) glBegin);
		gluTessCallback(tess, GLU_TESS_VERTEX_DATA, (TessFunction) VertexCallBackFunc);
		gluTessCallback(tess, GLU_TESS_END, glEnd);


		gluTessBeginPolygon(tess, (void*)&drawdef); 
			gluTessBeginContour(tess); 
			for(long i = 0; i < poly.NrOfPoints(); i++)
			{
				const geo::IPoint &p = poly.Point(i);

				GLdouble* pt = new GLdouble[3];
				pt[0] = p.X();
				pt[1] = p.Y();
				pt[2] = p.Z();

				m_Points.push_back(pt);

				gluTessVertex(tess, m_Points[i], m_Points[i]);

				 if(bDrawSteps) Frame().DrawStep();
			}
			gluTessEndContour(tess); 
		gluTessEndPolygon(tess); 

		gluDeleteTess(tess);

		while(m_Points.size())
		{
			delete m_Points[0];
			m_Points.erase(m_Points.begin());
		}
	}
	else
	{
		std::vector<TColor> vcColor = drawdef.Color(poly);
		DIA_ASSERT(vcColor.size() == poly.NrOfPoints());

		OpenDrawState(LINELOOP);
		long i;
		for(i = 0; i < poly.NrOfPoints(); i++)
		{
			SetColor(vcColor[i]);
			DrawVertex(poly.Point(i));
			if(bDrawSteps) Frame().DrawStep();
   		}
		CloseDrawState();
	}

}

void COpenGLDraw::DrawSymbol(const ISymbol &symbol, const IDrawDef &drawdef, bool bDrawSteps)
{
	
	CloseDrawState();

	

	GLbitfield mask = 
				GL_DEPTH_BUFFER_BIT | 
				GL_ENABLE_BIT |
				GL_LINE_BIT |
				GL_POLYGON_BIT  |
				GL_TRANSFORM_BIT |
				GL_VIEWPORT_BIT;

	glPushAttrib(mask); //store the current atribute state
	
	//get the current screen coords of the pos. of the symbol
	//we use this later
	std::vector<IOpenGLFrame::TScreenPointDepth> vcPoint;
	int i;
	for(i = 0; i < symbol.NrOfPositions(); i++)
		vcPoint.push_back(Frame().Project(symbol.Position(i)));

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	//make a new projection matrix based on the screensize
	double vpWidth2 =  double(Frame().ViewPortWidth())/2;
	double vpHeight2 = double(Frame().ViewPortHeight())/2;
  double rangeMax = symbol.Geometry().Min().Distance( symbol.Geometry().Max() );
  glOrtho( -vpWidth2, vpWidth2, -vpHeight2, vpHeight2, 0.0, rangeMax );

	//make a new modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix(); //store current the ModelMatrix

	//convert the screen coordinates of posistion to new "world" coor
	double ModelMatrix[16];
	double ProjectionMatrix[16];
	int    ViewPort[4];

	glGetDoublev(GL_MODELVIEW_MATRIX, ModelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, ProjectionMatrix);
	glGetIntegerv(GL_VIEWPORT, ViewPort);
	
	double dX, dY, dZ;
	for(i = 0; i < symbol.NrOfPositions(); i++)
	{
		IOpenGLFrame::TScreenPoint root_point = vcPoint[i].first;
		root_point = Frame().ChildToRoot(root_point); 

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			

			gluUnProject(root_point.x(),
						 root_point.y(),
						 vcPoint[i].second,
						 ModelMatrix,
						 ProjectionMatrix,
						 ViewPort,
						 &dX,
						 &dY,
						 &dZ);

			dX = dX - symbol.PivotPoint().X();
			dY = dY - symbol.PivotPoint().Y();
			dZ = dZ - symbol.PivotPoint().Z();
			
			glTranslated(dX, dY, dZ);
			//evently draw the symbol
			CHandleSelectionStack hs(symbol.Geometry(),drawdef,*this);
			
			Draw(symbol.Geometry(), drawdef, bDrawSteps);


		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		
	}
	
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix(); //restore previous MODELVIEW_MATRIX
	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix(); //restore previous PROJECTION_MATRIX

	//restore the previous matrix_mode
	glPopAttrib();

	
	
//	 if(bDrawSteps) Frame().DrawStep();
}

void COpenGLDraw::DrawArray(const geo::IArray &array, const IDrawDef &drawdef, bool bDrawSteps)
{
	CHandleSelectionStack hs(array,drawdef,*this);

	for(size_t i = 0; i < array.Size(); i++)
		InternalDraw(array.ObjectBase(i), drawdef, bDrawSteps);

}

void COpenGLDraw::DrawElementSet(const geo::IElementSet &elementset, const IDrawDef &drawdef, bool bDrawSteps)
{
	CHandleSelectionStack hs(elementset,drawdef,*this);

	for(int i = 0; i < elementset.ElementSize(); i++)
		InternalDraw(elementset.Element(i), drawdef, bDrawSteps);

}
