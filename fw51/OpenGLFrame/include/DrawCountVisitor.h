 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// DrawCountVisitor.h: interface for the CDrawCountVisitor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWCOUNTVISITOR_H__36C8A917_2008_4A51_B54C_0E049A2403BE__INCLUDED_)
#define AFX_DRAWCOUNTVISITOR_H__36C8A917_2008_4A51_B54C_0E049A2403BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class COpenGLDraw;
class IDrawDef;

#include "DrawVisitorBase.h"

#include "OpenGLFrameExports.h"

class OPENGLFRAME_EXPORT CDrawCountVisitor : public CDrawVisitorBase
{
  const COpenGLDraw &m_draw;
  const IDrawDef &m_drawdef;
  int m_iCount;

public:
  CDrawCountVisitor(const COpenGLDraw &draw, const IDrawDef &drawdef);
  virtual ~CDrawCountVisitor();

  const COpenGLDraw &Draw();
  const IDrawDef &DrawDef();
  int Count();

  virtual bool HandleSurface(const geo::ISurface &Surface);
  virtual bool HandleArray(const geo::IArray &Array);
  virtual bool HandleVolume(const geo::IVolume &Volume);
  virtual bool HandleBodyGroup(const geo::CBodyGroup &BodyGroup);
  virtual bool HandleElementSet(const geo::IElementSet &ElementSet);
  virtual bool HandlePolyLine(const geo::IPolyLine &PolyLine);
  virtual bool HandlePolygon(const geo::IPolygon &Polygon);
  virtual bool HandlePointElement(const geo::IPointElement &PointElement);
  virtual bool HandleSymbol(const ISymbol &Symbol);
  virtual bool HandleObject(const geo::IObject &Object);
};

#endif // !defined(AFX_DRAWCOUNTVISITOR_H__36C8A917_2008_4A51_B54C_0E049A2403BE__INCLUDED_)
