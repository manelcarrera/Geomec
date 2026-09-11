 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "dimple.h"
#include <cmath>
#include "2DGraphParentBase.h"
#include "2DGraphBase.h"
#include "2DGraphLineBase.h"
#include "Line.h"
#include "Rectangle.h"

C2DGraphLineBase::C2DGraphLineBase(C2DGraphBase& parent, 
								   unsigned long color,
								   bool bLineStipple)
: m_parent(parent), 
  m_Color(color),
  m_LineWidth(1),
  m_bLineStipple(bLineStipple)
{
	parent.m_vcGraphLine.append(this);
}

C2DGraphLineBase::C2DGraphLineBase(C2DGraphBase& parent, 
								   const QString& description,
								   unsigned long color,
								   bool bLineStipple)
: m_parent(parent), 
  m_Color(color),
  m_LineWidth(1),
  m_Description(description),
  m_bLineStipple(bLineStipple)
{
	parent.m_vcGraphLine.append(this);
}



C2DGraphLineBase::~C2DGraphLineBase()
{
}

double C2DGraphLineBase::InterpolateY(int nFirst, int nSecond, const double& dX) const
{
	assert(Y(nFirst).Valid() && Y(nSecond).Valid());

	double DeltaY = Y(nSecond).Value() - Y(nFirst).Value();
	double DeltaX = X(nSecond) - X(nFirst);

	if(fabs(DeltaX) < EPS)
		return Y(nSecond).Value();
		
	return (DeltaY / DeltaX) * (dX - X(nFirst)) + Y(nFirst).Value();	
}

double C2DGraphLineBase::InterpolateX(int nFirst, int nSecond, const double& dY) const
{
	assert(Y(nFirst).Valid() && Y(nSecond).Valid());

	double DeltaY = Y(nSecond).Value() - Y(nFirst).Value();
	double DeltaX = X(nSecond) - X(nFirst);

	if(fabs(DeltaX) < EPS)
		return X(nSecond);
		
	return (DeltaX / DeltaY) * (dY - Y(nFirst).Value()) + X(nFirst);
}

geo::CValue C2DGraphLineBase::InterpolateY(const double& dX) const
{
	for(int i = 0; i < (PointSize() - 1); i++)
	{
		double dMinX = std::min(X(i), X(i + 1));
		double dMaxX = std::max(X(i), X(i + 1));

		if((dMinX <= dX) && (dX <= dMaxX))
		{
			if(Y(i).Valid() && Y(i + 1).Valid())
				return geo::CValue(InterpolateY(i, i + 1, dX));
		}
	}

	return geo::CValue();
}

void C2DGraphLineBase::Update()
{
	C2DGraphBase::CDisplayPair *pLinePair = 0;
	C2DGraphBase::CDisplayPair *pSectionPair = 0;

	// Loop over the points and create lines
	TSerieVec vcSerie = GenerateValues();

	for(size_t nSerie = 0; nSerie < vcSerie.size(); nSerie++)
	{
		for(size_t nPoint = 0; nPoint < vcSerie[nSerie].size(); nPoint++)
		{
			// Create a line
			if(nPoint < (vcSerie[nSerie].size() - 1))
			{
				const TGraphPoint& first = vcSerie[nSerie][nPoint];
				const TGraphPoint& second = vcSerie[nSerie][nPoint + 1];

				if(first.first != second.first)
				{
					geo::CLine* pLine = new geo::CLine(first.first, second.first);
					if (!pLinePair)
						pLinePair = Parent().CreateNewPair();
					pLinePair->AddObject(*pLine);
				}
			}

			// Create a marker
			if((vcSerie[nSerie][nPoint].second != -1) && m_parent.MarkersEnabled())
			{
				CMarker *pMarker = CreateMarker(vcSerie[nSerie][nPoint].second);
				if(pMarker)
				{
					assert(pMarker->Position(0) == vcSerie[nSerie][nPoint].first);
					// Create a marker
					C2DGraphBase::CDisplayPair* pMarkerPair = Parent().CreateNewPair();
					pMarkerPair->DrawDef().RequireCompile(FALSE);
					pMarkerPair->DrawDef().LineStipple(LineStipple());
					pMarkerPair->DrawDef().LineWidth(LineWidth());
					pMarkerPair->DrawDef().SetFixedColor(pMarker->Color());
					pMarkerPair->DrawDef().PolyFillBack(pMarker->Fill());
					pMarkerPair->DrawDef().PolyFillFront(pMarker->Fill());
					pMarkerPair->DrawDef().SetDepthLayer2D(GRAPH_LAYER_LINE_TOP);
					pMarkerPair->AddObject(*pMarker);
					Parent().Update(pMarkerPair);
				}
			}
		}
	}

	if (pLinePair)
	{
		pLinePair->DrawDef().Visible(TRUE);
		pLinePair->DrawDef().SetFixedColor(Color());
		pLinePair->DrawDef().LineWidth(LineWidth());
		pLinePair->DrawDef().LineStipple(LineStipple());
		pLinePair->DrawDef().SetDepthLayer2D(GRAPH_LAYER_LINE_TOP);
		Parent().Update(pLinePair);
	}

	if(m_parent.InvalidateSections())
	{
		if(vcSerie.size() > 0)
		{
			// 'no formation' rectangle before top formation
			if(MinX() < vcSerie[0][0].first.X())
			{
				if ( geo::CRectangle::ValidRectangleData(
					geo::CPoint(MinX(), m_parent.MinY().Value()), 
					geo::CPoint(vcSerie[0][0].first.X(), m_parent.MaxY().Value())))
				{
					if(!pSectionPair)
						pSectionPair = m_parent.CreateNewPair();
					pSectionPair->AddObject(
						*new geo::CRectangle(
							geo::CPoint(MinX(), m_parent.MinY().Value()), 
							geo::CPoint(vcSerie[0][0].first.X(), m_parent.MaxY().Value())));
				}
			}

			for(size_t i = 0; i < (vcSerie.size() - 1); i++)
			{
				// 'no formation' rectangle for void space between formations
				double dXStart = vcSerie[i][vcSerie[i].size() - 1].first.X();
				double dXEnd = vcSerie[i + 1][0].first.X();
				if(fabs(dXStart - dXEnd) > EPS)
				{
					if ( geo::CRectangle::ValidRectangleData(
						geo::CPoint(dXStart, m_parent.MinY().Value()), 
						geo::CPoint(dXEnd, m_parent.MaxY().Value())))
					{
						if(!pSectionPair)
							pSectionPair = m_parent.CreateNewPair();
						pSectionPair->AddObject(*new geo::CRectangle(
							geo::CPoint(dXStart, m_parent.MinY().Value()), 
							geo::CPoint(dXEnd, m_parent.MaxY().Value())));
					}
				}
			}
			double dLastX = vcSerie[vcSerie.size() - 1][vcSerie[vcSerie.size() - 1].size() - 1].first.X();
			if(dLastX < MaxX())
			{
				// 'no formation' rectangle below last formation
				if ( geo::CRectangle::ValidRectangleData(
					geo::CPoint(dLastX, m_parent.MinY().Value()), 
					geo::CPoint(MaxX(), m_parent.MaxY().Value())))
				{
					if(!pSectionPair)
						pSectionPair = m_parent.CreateNewPair();
					pSectionPair->AddObject(*new geo::CRectangle(
						geo::CPoint(dLastX, m_parent.MinY().Value()), 
						geo::CPoint(MaxX(), m_parent.MaxY().Value())));
				}
			}
		}
	}

	if(pSectionPair)
	{
		pSectionPair->DrawDef().Visible(TRUE);
		pSectionPair->DrawDef().SetFixedColor(m_parent.InvalidSectionColor());
		pSectionPair->DrawDef().PolyDrawBack(TRUE);
		pSectionPair->DrawDef().PolyDrawFront(TRUE);
//		pSectionPair->DrawDef().DepthFunc(GL_NEVER);
		pSectionPair->DrawDef().SetDepthLayer2D(GRAPH_LAYER_BACKGROUND);
		Parent().Update(pSectionPair);
	}
}

double C2DGraphLineBase::MaxX() const
{
	return Parent().MaxX();
}

double C2DGraphLineBase::MinX() const
{
	return Parent().MinX();
}

C2DGraphLineBase::TSerieVec C2DGraphLineBase::GenerateValues(bool dDelimitY) const
{
	// Walk over points	
	TSerieVec ret;
	TSerie current_serie;
	ePointStatus prev_status = INVALID;	

	// We generate a serie delimited under MinX and MaxX
	for(int nPoint = 0; nPoint < PointSize(); nPoint++)
	{
		// Determine point modi ...
		ePointStatus status = INVALID;
		if(Y(nPoint).Valid())
                {
			status = Status(MinX(), MaxX(), geo::CValue(X(nPoint)));
                }
                else if ( !current_serie.empty() ) // wjrx mantis 3756
                {
			ret.push_back(current_serie);
			current_serie.clear();
                }

		// Min -> Inside or Max - Add border point 
		if((prev_status == OUTSIDE_MIN) && ((status == INSIDE) || (status == OUTSIDE_MAX)))
			current_serie.push_back(TGraphPoint(geo::CPoint(MinX(), InterpolateY(nPoint - 1, nPoint, MinX())), -1));
		// Max -> Inside or Min - Add border point 
		if((prev_status == OUTSIDE_MAX) && ((status == INSIDE) || (status == OUTSIDE_MIN)))
			current_serie.push_back(TGraphPoint(geo::CPoint(MaxX(), InterpolateY(nPoint - 1, nPoint, MaxX())), -1));
		// Inside the Min Max region
		if(status == INSIDE)
			current_serie.push_back(TGraphPoint(geo::CPoint(X(nPoint), Y(nPoint).Value()), nPoint));
		// Inside or Min -> Max - Add border point and terminate
		if(((prev_status == INSIDE) || (prev_status == OUTSIDE_MIN)) && (status == OUTSIDE_MAX))
		{
			current_serie.push_back(TGraphPoint(geo::CPoint(MaxX(), InterpolateY(nPoint - 1, nPoint, MaxX())), -1));
			ret.push_back(current_serie);
			current_serie.clear();
		}
		// Inside or Min -> Max - Add border point and terminate
		if(((prev_status == INSIDE) || (prev_status == OUTSIDE_MAX)) && (status == OUTSIDE_MIN))
		{
			current_serie.push_back(TGraphPoint(geo::CPoint(MinX(), InterpolateY(nPoint - 1, nPoint, MinX())), -1));
			ret.push_back(current_serie);
			current_serie.clear();
		}

		prev_status = status;
	}

	// Terminate any open things
	if(current_serie.size() > 0)
		ret.push_back(current_serie);

	if((!Parent().MinY().Valid() || !Parent().MaxY().Valid()) && !dDelimitY)
		return ret;

	return ret;
}

enum C2DGraphLineBase::ePointStatus C2DGraphLineBase::Status(const double& dMin, const double& dMax, const geo::CValue& value) const
{
	// Determine current status
	ePointStatus status = INVALID;
	if(value.Valid())
	{
		if(value.Value() < dMin)
			status = OUTSIDE_MIN;
		else if(dMax < value.Value())
			status = OUTSIDE_MAX;
		else
			status = INSIDE;
	}
	return status;
}

unsigned long C2DGraphLineBase::Color() const
{
	return m_Color;
}

//////////////////////////////////////////////////////////////////////

void C2DGraphLineBase::Color(unsigned long color)
{
	m_Color = color;
}
//////////////////////////////////////////////////////////////////////

unsigned long C2DGraphLineBase::LineWidth() const
{
	return m_LineWidth;
}
//////////////////////////////////////////////////////////////////////

void C2DGraphLineBase::LineWidth(unsigned long linewidth)
{
	m_LineWidth = linewidth;
}

//////////////////////////////////////////////////////////////////////

bool C2DGraphLineBase::LineStipple() const
{
	return m_bLineStipple;
}

void C2DGraphLineBase::LineStipple(bool bLinestipple)
{
	m_bLineStipple = bLinestipple;
}

CMarker* C2DGraphLineBase::CreateMarker(int nIndex) const
{
	assert(Parent().Parent());
	const C2DGraphParentBase *pParent = dynamic_cast<const C2DGraphParentBase*>(Parent().Parent());
	assert(pParent);
	assert(Y(nIndex).Valid());
	QString sDescription(m_Description);
	if(m_Description.isEmpty())
		sDescription = pParent->YLabel();
	//wjrx mantis 2372
	//Decrease the label width
	QString sLabel = QString("%1 %2 [%3] %4 [%5]\n%6")
						.arg(pParent->XLabel())
						.arg(X(nIndex))
						.arg(pParent->XUnit())
						.arg(Y(nIndex).Value())
						.arg(pParent->YUnit())
						.arg(sDescription);

	return new CMarker(geo::CPoint(X(nIndex), Y(nIndex).Value()), Color(), CMarker::MT_POINT, sLabel);
}


C2DGraphBase& C2DGraphLineBase::Parent()
{
	return m_parent;
}

const C2DGraphBase& C2DGraphLineBase::Parent() const
{
	return m_parent;
}

const QString& C2DGraphLineBase::Description() const
{
	return m_Description;
}

void C2DGraphLineBase::Description(const QString& description)
{
	m_Description = description;
}

	
