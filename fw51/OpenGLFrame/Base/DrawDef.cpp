 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "dimple.h"
#include "IDrawDef.h"
#include "DrawDef.h"
#include "ILine.h"
#include "ICircle.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//##ModelId=3B653BCA03BB
void CDrawDef::Init()
{
	m_LineStippleFactor  = IDrawDef::LineStippleFactor();
	m_LineStipplePattern = IDrawDef::LineStipplePattern();

	m_bPolyFillFront=TRUE;
	m_bPolyFillBack=TRUE;
	m_bVisible=TRUE;
	m_bLineStipple=FALSE;
	m_fLineWidth=1;
	m_fPointSize=2;
	m_bPolyDrawFront=TRUE;
	m_bPolyDrawBack=TRUE;
	m_iDepthFunc=GL_LESS;
	m_TextColor = qRgb(255, 0, 0);
	m_bRequireCompile = TRUE;
	m_lCircleNrOfSegments=100;

	m_UseStencilBuffer=FALSE;
	m_StencilFuncFunc=GL_ALWAYS;
	m_StencilFuncRef=1;
	m_StencilFuncMask=1;
	m_StencilOpFail=GL_KEEP;
	m_StencilOpZFail=GL_KEEP;
	m_StencilOpZPass=GL_KEEP;

	m_LabelBorder=FALSE;
	m_LabelBorderOutlineColor=qRgb(0,0,255);
	m_LabelBorderInteriorColor=qRgb(255,0,0);
	
	m_IsSelectable=FALSE;

	m_curvedDraw=true;
}

CDrawDef::CDrawDef(TColor FixedColor)
: m_Color(FixedColor)
{
	Init();
	m_TextColor = FixedColor;
}


CDrawDef::CDrawDef()
: m_Color(qRgb(255, 255, 255))
{
	Init();
}

//##ModelId=3B653BCA03BC
CDrawDef::~CDrawDef()
{

}

/*COLORREF CDrawDef::Color(const geo::IPoint &pt) const
{
	return tmpColor;
	//return RGB(0, 0, 255);
}
*/

/*!
  \todo Drawing colors is pointbased instead of node based.
*/
std::vector<CDrawDef::TColor> CDrawDef::Color(const geo::IObject &object) const
{
	const geo::IElement *pElement = dynamic_cast<const geo::IElement*>(&object);
	const geo::IPoint *pPoint	  = dynamic_cast<const geo::IPoint*>(&object);
	const geo::ICircle *pCircle	  = dynamic_cast<const geo::ICircle*>(&object);

	if(pElement)
	{
//		if ( pElement->NrOfNodes() == 0 ) {
			return std::vector<TColor>( pElement->NrOfPoints(), m_Color );
//		} else {
//			return std::vector<TColor>( pElement->NrOfNodes(), m_Color );
//		}
	}
	else if(pPoint)
	{
		return std::vector<TColor>( 1, m_Color );
	}
	else if(pCircle)
	{
		return std::vector<TColor>( 1, m_Color );
	}
	else
	{
		return std::vector<TColor>( 1, m_Color );
	}
}

void CDrawDef::TextColor(const TColor color)
{
	m_TextColor = color;
}

CDrawDef::TColor CDrawDef::TextColor() const
{
	return m_TextColor;
}

void CDrawDef::SetFixedColor(const TColor color)
{
	m_Color = color;
}

CDrawDef::TColor CDrawDef::GetFixedColor() const
{
	return m_Color;
}


void CDrawDef::SetDepthLayer2D(int LayerNr)
{
	m_UseStencilBuffer= true;
	m_StencilFuncFunc=GL_GREATER;
	m_StencilFuncRef=LayerNr;
	m_StencilFuncMask=7;
	m_StencilOpFail=GL_KEEP;
	m_StencilOpZFail=GL_REPLACE;
	m_StencilOpZPass=GL_REPLACE;
}


