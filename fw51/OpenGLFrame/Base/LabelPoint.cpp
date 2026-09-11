 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "dimple.h"
#include <algorithm>
#include <cassert>
#include "LabelPoint.h"


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
CLabelPoint::CLabelPoint(const double& dX,
						 const double& dY,
						 const double& dZ,
						 const QString &label,
						 const QFont& font,
						 eLabelAlign Alignment,
						 const double& dRotate)
: geo::CPoint(dX, dY, dZ), m_Label(label), m_eAlignment(Alignment), m_dRotate(dRotate), m_font( font )
{
}

CLabelPoint::CLabelPoint(const geo::IPoint& point,
						 const QString &label,
						 const QFont& font,
						 eLabelAlign Alignment,
						 const double&  dRotate)
: geo::CPoint(point), m_Label(label), m_eAlignment(Alignment), m_dRotate(dRotate), m_font( font )
{
}


CLabelPoint::CLabelPoint(const CLabelPoint &labelpoint)
: geo::CPoint(labelpoint), m_Label(labelpoint.m_Label), m_eAlignment(labelpoint.m_eAlignment), m_dRotate(labelpoint.m_dRotate), m_font( labelpoint.m_font )
{
}

CLabelPoint::~CLabelPoint()
{

}

// Alignment indicates the position of the label relative to the point. (Top-left, Top-right, Bottom-left or Bottom-right)
int CLabelPoint::GetAlignment() const
{
	return m_eAlignment;
}

void CLabelPoint::SetAlignment(eLabelAlign Alignment)
{
	m_eAlignment = Alignment;
}

// The text can also be put under an angle.
double CLabelPoint::GetRotate() const
{
	return m_dRotate;
}

void CLabelPoint::SetRotate(const double& dRotate)
{
	m_dRotate = dRotate;
}

// The actual text....
const QString& CLabelPoint::Label() const
{
	return m_Label;
}

void CLabelPoint::Label(const QString& label)
{
	m_Label = label;
}

void CLabelPoint::SetPoint(const double& dX, const double&  dY, const double& dZ)
{
	X(dX);
	Y(dY);
	Z(dZ);
}

const QFont& CLabelPoint::font() const
{
	return m_font;
}

