// ArrowLabelSymbol.cpp: implementation of the CArrowLabelSymbol class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "LabelPoint.h"
#include "ArrowLabelSymbol.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CArrowLabelSymbol::CArrowLabelSymbol(const geo::CPoint& pos, const geo::IVector &dir, const QString& description ,bool autoDelete)
:CWellSymbolBase(pos,description,autoDelete)
{
	
	double ConeSize   =3;
	double Coneheight =3.5*ConeSize;
	double LineLength =5*Coneheight;

	geo::CLine *line = new geo::CLine(geo::CPoint::NullPoint, dir.UnitVector()*LineLength);
	geo::CConus *conus = new geo::CConus(line->Point(0) + dir.UnitVector()*Coneheight , -dir, ConeSize, Coneheight, 0);

	m_Array.PushBack(*line);
	m_Array.PushBack(*conus);


	QString tmp;
	if(m_Description.isEmpty())
	{
		//tmp="No description"
	}
	else
	{
		tmp=m_Description;
	}

	CLabelPoint* pLabel = new CLabelPoint(line->Point(1),tmp);//CLabelPoint::LA_CENTER_LEFT);

	m_Array.PushBack(*pLabel);
}


