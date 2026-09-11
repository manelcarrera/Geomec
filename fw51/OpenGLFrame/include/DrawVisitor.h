 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// DrawVisitor.h: interface for the CDrawVisitor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWVISITOR_H__7B477D0E_2DBF_4E55_B46F_07E22A434442__INCLUDED_)
#define AFX_DRAWVISITOR_H__7B477D0E_2DBF_4E55_B46F_07E22A434442__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DrawVisitorBase.h"

#include "OpenGLFrameExports.h"


class COpenGLDraw;
class IDrawDef;

class OPENGLFRAME_EXPORT CDrawVisitor : public CDrawVisitorBase
{
	COpenGLDraw &m_draw;
	const IDrawDef &m_drawdef;
	bool m_bDrawSteps;
public:
	CDrawVisitor(COpenGLDraw &draw, const IDrawDef &drawdef, bool bDrawSteps);
	virtual ~CDrawVisitor();

	COpenGLDraw &Draw();
	const IDrawDef &DrawDef();

	virtual bool HandlePoint(const geo::IPoint &Point);
	virtual bool HandleLabelPoint(const CLabelPoint &LabelPoint);
	virtual bool HandleLine(const geo::ILine &Line);
	virtual bool HandlePolygon(const geo::IPolygon &Polygon);
	virtual bool HandleTriangle(const geo::ITriangle &Triangle);
	virtual bool HandleFace(const geo::IFace &Face);
	virtual bool HandleQuadrilateral(const geo::IQuadrilateral &Quad);
	virtual bool HandleCircle(const geo::ICircle &Circle);
	virtual bool HandleConus(const geo::IConus &Conus);
	virtual bool HandleSphere(const geo::ISphere &Sphere);
	virtual bool HandleBox(const geo::IBox &Box);
	virtual bool HandleBody(const geo::IBody &Body);
	virtual bool HandleSurface(const geo::ISurface &Surface);
	virtual bool HandleArray(const geo::IArray &Array);
	virtual bool HandlePolygonPtrArray(const geo::CPolygonPtrArray &PolygonPtrArray);
	virtual bool HandleBodyGroup(const geo::CBodyGroup &BodyGroup);
	virtual bool HandleElementSet(const geo::IElementSet &ElementSet);
	virtual bool HandlePointElement(const geo::IPointElement &PointElement);
	virtual bool HandleSymbol(const ISymbol &Symbol);
	//Handle a interface between two IElement
	virtual bool HandleInterface(const geo::IInterfaceElement &interfaceElement);
};

#endif // !defined(AFX_DRAWVISITOR_H__7B477D0E_2DBF_4E55_B46F_07E22A434442__INCLUDED_)
