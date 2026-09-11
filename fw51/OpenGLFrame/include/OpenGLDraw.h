 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// OpenGLDraw.h: interface for the COpenGLDraw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENGLDRAW_H__A048AE74_8579_46E2_975C_A59BAA0E2325__INCLUDED_)
#define AFX_OPENGLDRAW_H__A048AE74_8579_46E2_975C_A59BAA0E2325__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IOpenGLDraw.h"
#include <qcolor.h>
#include <qgl.h>
#include <map>

#include "OpenGLFrameExports.h"

namespace geo {
  class CBodyGroup;
  class IBox;
  class IBody;
  class ITriangle;
  class IRectangle;
  class ISurface;
  class IFace;
  class IQuadrilateral;
  class IPolyLine;
  class IPolygon;
  class ICircle;
  class IConus;
  class IArray;
  class IElementSet;
  class ISphere;
  class CVector;
  class IPolygon;
  class IPointElement;
  class IInterfaceElement;
  class CPolygonPtrArray;
}



class IOpenGLFrame;
class ISymbol;
class COpenGLArrow;
class CLabelPoint;
class CHandleSelectionStack;

class OPENGLFRAME_EXPORT COpenGLDraw : public IOpenGLDraw
{
//see implementation in iopengldraw.cpp
friend class CHandleSelectionStack;

  typedef enum _DrawState
  {
    CLOSED		= -1,
    POINTS		= GL_POINTS,
    LINES		= GL_LINE_STRIP,
    TRIANGLES	= GL_TRIANGLES,
    POLYGON		= GL_POLYGON,
    LINELOOP	= GL_LINE_LOOP 
  } TDrawState;


  typedef QRgb TColor;
  typedef std::map<const geo::IPoint*, TColor> TPointToColorMap;


  mutable TDrawState m_DrawState;
  mutable bool m_bStepping;

  void InternalDraw(const geo::IObject &object, const IDrawDef &drawdef, bool bDrawSteps);
  void OpenDrawState(TDrawState state) const;
  void CloseDrawState() const;


  TColor ContrastColor(TColor color) const;
 /*
  //print formated text with a given font if *font=NULL uses default font
  void glPrint(QFont font, const char *fmt, ...);
  void glPrintRotate(double x, double y, double z, double rotate, QFont font, const char *fmt, ...);
   */
public:
  COpenGLDraw(IOpenGLFrame &frame);
  virtual ~COpenGLDraw();


  void SetOpenGLAtrib(const IDrawDef &drawdef) const;
  void RestoreOpenGLAtrib() const;

  // the number of steps this object represents
  virtual int Count(const geo::IObject &object, const IDrawDef &drawdef) const;
  // draw the object
  virtual void Draw(const geo::IObject &object, const IDrawDef &drawdef, bool bDrawSteps);

  void DrawBodyGroup(const geo::CBodyGroup &BodyGroup, const IDrawDef &drawdef, bool bDrawSteps) const;
  void DrawPoint(const geo::IPoint &pt, const IDrawDef &drawdef, bool bDrawSteps) const;
  void DrawPoint(const geo::IPointElement &element, const IDrawDef &drawdef, bool bDrawSteps) const;
  void DrawLine(const geo::ILine &line, const IDrawDef &drawdef, bool bDrawSteps) const;
  void DrawBox(const geo::IBox &box,const IDrawDef &drawdef, bool bDrawSteps) const;
  void DrawBody(const geo::IBody &body, const IDrawDef &drawdef, bool bDrawSteps) const;
  void DrawTriangle(const geo::ITriangle &triangle, const IDrawDef &drawdef, bool bDrawSteps) const;
  void DrawTriangle(const geo::ITriangle &triangle, int *pColor) const;
  void DrawRectangle(const geo::IRectangle &rectangle, const IDrawDef &drawdef, bool bDrawSteps) const;
  void DrawSurface(const geo::ISurface &surface,const IDrawDef &drawdef, bool bDrawSteps) const;
  void DrawLabel(const CLabelPoint &lbp, const IDrawDef &drawdef, bool bDrawSteps);
  void DrawFace(const geo::IFace &face, const IDrawDef &drawdef, bool bDrawSteps) const;
  void DrawQuadrilateral(const geo::IQuadrilateral &quad, const IDrawDef &drawdef, bool bDrawSteps) const;
  void DrawPolyLine(const geo::IPolyLine &line, const IDrawDef &drawdef, bool bDrawSteps) const;
  void DrawPolygon(const geo::IPolygon &poly, const IDrawDef &drawdef, bool bDrawSteps) const;
  void DrawSymbol(const ISymbol& symbol, const IDrawDef& drawdef, bool bDrawSteps);
  void DrawCircle(const geo::ICircle &circle, const IDrawDef &drawdef, bool bDrawSteps) const;
  void DrawConus(const geo::IConus &conus, const IDrawDef &drawdef, bool bDrawSteps) const;
  void DrawSphere(const geo::ISphere &sphere, const IDrawDef &drawdef, bool bDrawSteps) const;
  void DrawArray(const geo::IArray &array, const IDrawDef &drawdef, bool bDrawSteps);
  void DrawElementSet(const geo::IElementSet &elementset, const IDrawDef &drawdef, bool bDrawSteps);
  void DrawPolygonArr(const geo::CPolygonPtrArray &poly_arr,const IDrawDef &drawdef, bool bDrawSteps) const;

  //Draw a interface between two IElement
  void DrawInterface(const geo::IInterfaceElement &interfaceElement, const IDrawDef &drawdef, bool bDrawSteps);


  const IOpenGLFrame &Frame() const { return m_frame; }
  IOpenGLFrame &Frame() { return m_frame; }

  //static helper funcs
  static void SetColor(TColor color);
  static void DrawVertex( const geo::IPoint &Point );
  static void SetNormal( const geo::IVector &normal );

private:



  IOpenGLFrame& m_frame;
  bool m_NamePushed;

  typedef std::map<const geo::IPoint*, geo::CVector, geo::ICoordinate::CCoordinateLess> TPointNormalMap;
  void DrawSurfaceFace(const geo::IFace &face,
    const IDrawDef &drawdef,
    TPointNormalMap &mpPointNormal,
    bool bNeedNormals,
    bool bPlanar,
    geo::IVector &Normal) const;

};

#endif // !defined(AFX_OPENGLDRAW_H__A048AE74_8579_46E2_975C_A59BAA0E2325__INCLUDED_)
