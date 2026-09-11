 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "dimple.h"

#include "IPolygon.h"
#include "ISurface.h"
#include "BodyGroup.h"
#include "IPolyLine.h"
#include "IElement.h"
#include "Polygon.h"
#include "ISymbol.h"
#include "DrawCountVisitor.h"
#include "OpenGLDraw.h"
#include "IDrawDef.h"
#include "IPointElement.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDrawCountVisitor::CDrawCountVisitor(const COpenGLDraw &draw, const IDrawDef &drawdef) :
m_draw(draw), m_drawdef(drawdef), m_iCount(0)
{
}

CDrawCountVisitor::~CDrawCountVisitor()
{
}

const COpenGLDraw &CDrawCountVisitor::Draw()
{
	return m_draw;
}

const IDrawDef &CDrawCountVisitor::DrawDef()
{
	return m_drawdef;
}

int CDrawCountVisitor::Count()
{
	// to early or not handled?
	// TNMA: I commented this assert out because WED told me that it had no real meaning and 
	// it was driving me nuts!!
//	assert(m_iCount != 0);
	return m_iCount;
}

bool CDrawCountVisitor::HandleSurface(const geo::ISurface &Surface)
{
	m_iCount = Surface.FaceSize();

	return true;
}

bool CDrawCountVisitor::HandleArray(const geo::IArray &Array)
{
	for(size_t i = 0; i < Array.Size(); i++)
	{
		m_iCount += Draw().Count(Array.ObjectBase(i), DrawDef());
	}

	return true;
}

bool CDrawCountVisitor::HandleVolume(const geo::IVolume &Volume)
{
	m_iCount = Volume.ElementSize();

	return true;
}

bool CDrawCountVisitor::HandleBodyGroup(const geo::CBodyGroup &BodyGroup)
{
	if(DrawDef().PolyFillFront() && DrawDef().PolyFillBack())
	{
		assert(m_iCount == 0);
		for(int i = 0; i < BodyGroup.SideSurfaceSize(); i++)
			m_iCount += BodyGroup.SideSurface(i).FaceSize();

		return true;
	}

	return HandleVolume(BodyGroup);
}


bool CDrawCountVisitor::HandleElementSet(const geo::IElementSet &ElementSet)
{
	m_iCount = ElementSet.ElementSize();

	return true;
}

bool CDrawCountVisitor::HandlePolygon(const geo::IPolygon &Polygon)
{
	m_iCount = Polygon.NrOfPoints();

	return true;
}

bool CDrawCountVisitor::HandlePolyLine(const geo::IPolyLine &PolyLine)
{
	m_iCount = PolyLine.LineSize();

	return true;
}

bool CDrawCountVisitor::HandleSymbol(const ISymbol &Symbol)
{
	m_iCount += Draw().Count(Symbol.Geometry(), DrawDef());
	return true;
}

bool CDrawCountVisitor::HandleObject(const geo::IObject &/*Object*/)
{
	m_iCount = 1;
	return true;
}

bool CDrawCountVisitor::HandlePointElement(const geo::IPointElement &/*PointElement*/)
{
	m_iCount = 1;
	return true;
}
