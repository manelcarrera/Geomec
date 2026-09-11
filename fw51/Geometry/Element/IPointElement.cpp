 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "IPointElement.h"
#include "Line.h"

namespace geo {

IPointElement::IPointElement()
{
}

IPointElement::~IPointElement()
{
}

void IPointElement::AssertValid() const
{
	assert( NrOfPoints() == 1 );
}

int IPointElement::NrOfPoints() const
{
	return 1;
}

const IPoint& IPointElement::Point(int nIndex) const
{
	assert(nIndex == 0);
	return Node(0);
}

void IPointElement::Point(int nIndex, const IPoint& point)
{
	assert(nIndex == 0);
	Node(0, point);
}

IElement::TDoubleVec IPointElement::ShapeFunction(const IElement::TDoubleVec& /*isocoords*/) const
{
	IElement::TDoubleVec vcRet(1);
	vcRet.push_back(1.0);

	return vcRet;
}

double IPointElement::Size() const
{
	return 0;
}

size_t IPointElement::Order() const
{
	return 1;
}

double IPointElement::RepresentativeLength() const
{
	return 0;
}

std::vector<CLine> IPointElement::Lines() const
{
	return std::vector<CLine>();
}

CMatrix IPointElement::ShapeFunctionDerived(const IElement::TDoubleVec& /*isocoords*/) const
{
	assert(false);
	return CMatrix();
}

std::vector<IElement::TDoubleVec> IPointElement::IsoCoordinates() const
{
	assert(false);
	return std::vector<IElement::TDoubleVec>();
}

int IPointElement::NrOfLines() const
{
	return 0;
}

const ILine& IPointElement::Line(int /*nIndex*/) const
{
	assert(false);
	const ILine* pBogus = 0;
	return *pBogus;
}

} // namespace geo
