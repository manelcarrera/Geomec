/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "dimple.h"

#include "BodyGroup.h"
#include "DrawVisitor.h"
#include "IInterfaceElement.h"
#include "IPointElement.h"
#include "IPolygon.h"
#include "IQuadrilateral.h"
#include "ISymbol.h"
#include "LabelPoint.h"
#include "OpenGLArrow.h"
#include "OpenGLDraw.h"
#include "Polygon.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDrawVisitor::CDrawVisitor(COpenGLDraw &draw, const IDrawDef &drawdef, bool bDrawSteps)
    : m_draw(draw), m_drawdef(drawdef), m_bDrawSteps(bDrawSteps) {}

CDrawVisitor::~CDrawVisitor() {}

COpenGLDraw &CDrawVisitor::Draw() { return m_draw; }

const IDrawDef &CDrawVisitor::DrawDef() { return m_drawdef; }

bool CDrawVisitor::HandlePoint(const geo::IPoint &Point) {
  Draw().DrawPoint(Point, DrawDef(), m_bDrawSteps);
  return true;
}

bool CDrawVisitor::HandleLabelPoint(const CLabelPoint &LabelPoint) {
  Draw().DrawLabel(LabelPoint, DrawDef(), m_bDrawSteps);
  return true;
}

bool CDrawVisitor::HandleLine(const geo::ILine &Line) {
  Draw().DrawLine(Line, DrawDef(), m_bDrawSteps);
  return true;
}

bool CDrawVisitor::HandlePolygon(const geo::IPolygon &Polygon) {
  Draw().DrawPolygon(Polygon, DrawDef(), m_bDrawSteps);
  return true;
}

bool CDrawVisitor::HandleTriangle(const geo::ITriangle &Triangle) {
  Draw().DrawTriangle(Triangle, DrawDef(), m_bDrawSteps);
  return true;
}

// Handle a interface between two IElement
bool CDrawVisitor::HandleInterface(const geo::IInterfaceElement &interfaceElement) {
  Draw().DrawInterface(interfaceElement, DrawDef(), m_bDrawSteps);
  return true;
}

bool CDrawVisitor::HandleFace(const geo::IFace &Face) {
  Draw().DrawFace(Face, DrawDef(), m_bDrawSteps);
  return true;
}

bool CDrawVisitor::HandleQuadrilateral(const geo::IQuadrilateral &Quad) {
  Draw().DrawQuadrilateral(Quad, DrawDef(), m_bDrawSteps);
  return true;
}

bool CDrawVisitor::HandleCircle(const geo::ICircle &Circle) {
  Draw().DrawCircle(Circle, DrawDef(), m_bDrawSteps);
  return true;
}

bool CDrawVisitor::HandleConus(const geo::IConus &Conus) {
  Draw().DrawConus(Conus, DrawDef(), m_bDrawSteps);
  return true;
}

bool CDrawVisitor::HandleSphere(const geo::ISphere &Sphere) {
  Draw().DrawSphere(Sphere, DrawDef(), m_bDrawSteps);
  return true;
}

bool CDrawVisitor::HandleBox(const geo::IBox &Box) {
  Draw().DrawBox(Box, DrawDef(), m_bDrawSteps);
  return true;
}

bool CDrawVisitor::HandleBody(const geo::IBody &Body) {
  Draw().DrawBody(Body, DrawDef(), m_bDrawSteps);
  return true;
}

bool CDrawVisitor::HandleSurface(const geo::ISurface &Surface) {
  Draw().DrawSurface(Surface, DrawDef(), m_bDrawSteps);
  return true;
}

bool CDrawVisitor::HandleArray(const geo::IArray &Array) {
  Draw().DrawArray(Array, DrawDef(), m_bDrawSteps);
  return true;
}

bool CDrawVisitor::HandlePolygonPtrArray(const geo::CPolygonPtrArray &PolygonPtrArray) {
  Draw().DrawPolygonArr(PolygonPtrArray, DrawDef(), m_bDrawSteps);
  return true;
}

bool CDrawVisitor::HandleBodyGroup(const geo::CBodyGroup &BodyGroup) {
  Draw().DrawBodyGroup(BodyGroup, DrawDef(), m_bDrawSteps);
  return true;
}

bool CDrawVisitor::HandleElementSet(const geo::IElementSet &ElementSet) {
  Draw().DrawElementSet(ElementSet, DrawDef(), m_bDrawSteps);
  return true;
}

bool CDrawVisitor::HandleSymbol(const ISymbol &Symbol) {
  Draw().DrawSymbol(Symbol, DrawDef(), m_bDrawSteps);
  return true;
}

bool CDrawVisitor::HandlePointElement(const geo::IPointElement &PointElement) {
  Draw().DrawPoint(PointElement, DrawDef(), m_bDrawSteps);
  return true;
}
