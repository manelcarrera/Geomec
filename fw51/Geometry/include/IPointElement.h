 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef _IPOINTELEMENT_H_
#define _IPOINTELEMENT_H_

#include "IElement.h"
#include "DispatchVisitorBase.h"

#include "GeometryExports.h"

namespace geo {

class GEOMETRY_EXPORT  IPointElement : public IElement
{
public:
	IPointElement();
	virtual ~IPointElement();

	virtual void AssertValid() const;

	virtual int NrOfPoints() const;
	virtual const IPoint& Point(int nIndex) const;
	virtual void Point(int nIndex, const IPoint& point);

	virtual IElement::TDoubleVec ShapeFunction(const IElement::TDoubleVec& isocoords) const;
	virtual double Size() const;

	virtual CMatrix ShapeFunctionDerived(const IElement::TDoubleVec& isocoords) const;
	virtual std::vector<IElement::TDoubleVec> IsoCoordinates() const;

	virtual size_t Order() const;
	virtual double RepresentativeLength() const;
	virtual std::vector<CLine> Lines() const;

	virtual int NrOfLines() const;
	virtual const ILine& Line(int nIndex) const;

	virtual bool Visit(IVisitor &visitor) { return visitor.HandlePointElement(*this); }
};

} // namespace geo

#endif // _IPOINTELEMENT_H_
