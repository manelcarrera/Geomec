 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef I_INTERFACE_ELEMENT_H__INCLUDED
#define I_INTERFACE_ELEMENT_H__INCLUDED

/*!
	\class IInterfaceElement
	\brief this class only used by InterfaceElement.h. It has been created for handle a InterfaceElement in drawing classes
	\author : Antoine
	\date : 21 april 2006
*/

#include "IElement.h"
#include "INode.h"
#include "DispatchVisitorBase.h"

#include "GeometryExports.h"

namespace geo {


class GEOMETRY_EXPORT  IInterfaceElement : public IElement
{


public:

	/*!
		\fn virtual const IElement& Front() const
		return the Front part of the interface
	*/
	virtual const IElement& Front() const = 0 ;


	/*!
		\fn virtual const IElement& Back() const
		return the Back part of the interface
	*/
	virtual const IElement& Back() const = 0 ;

	/*!
		Returns the number of points.
	*/
	virtual int NrOfPoints() const
	{
		return (int)NrOfNodes();
	}

	/*!
    Returns the i-th point.
	*/
	virtual const IPoint &Point( int nIndex ) const
	{
		return Node( nIndex );
	}

	/*!
		Sets the i-th point to pt.
	*/
	virtual void Point(int nIndex, const IPoint &pt)
	{
		Node(nIndex, pt);
	}

	/*!
    Returns the total number of lines of the interface element.
	*/
	virtual int NrOfLines() const
	{
		return Front().NrOfLines() + Back().NrOfLines();
	}

	/*!
    Returns the i-th line of the interface element.
	*/
	virtual const ILine& Line(int nIndex) const
	{
    if ( nIndex >= Front().NrOfLines() )
      return Back().Line( nIndex - Front().NrOfLines() );
    return Front().Line( nIndex );
	}

	/*!
    Forced to be overridden. It's not good, but the function doesn't seem to be
    used.
	*/
	virtual double RepresentativeLength() const
	{
		return 0;
	}

  virtual double Size() const
  {
    return Front().Size();
  }

	virtual bool Visit(IVisitor &visitor) { return visitor.HandleInterface(*this); }

	virtual std::vector<TDoubleVec> IsoCoordinates() const
  {
    return Front().IsoCoordinates();
  }

	virtual TDoubleVec IsoShapeFunction(const IPoint &point) const
  {
    TDoubleVec vc = Front().IsoShapeFunction(point);
    assert(vc.size() == NrOfNodes() / 2);
    int sz = (int)vc.size();
    vc.resize(NrOfNodes());
    for(int i = 0; i < sz; ++i)
      vc[i+sz] = vc[i];

    return vc;
  }

  virtual TDoubleVec ShapeFunction(const TDoubleVec& isocoords) const
  {
    TDoubleVec vc = Front().ShapeFunction(isocoords);
    assert(vc.size() == NrOfNodes() / 2);
    int sz = (int)vc.size();
    vc.resize(NrOfNodes());
    for(int i = 0; i < sz; ++i)
      vc[i+sz] = vc[i];

    return vc;
  }

	virtual CPoint IsoToWorld(const TDoubleVec& isocoords) const
  {
    return Front().IsoToWorld(isocoords);
  }

  virtual TDoubleVec WorldToIso(const geo::IPoint& point) const
  {
    TDoubleVec vc = Front().WorldToIso(point);
    assert(vc.size() == NrOfNodes() / 2);
    int sz = (int)vc.size();
    vc.resize(NrOfNodes());
    for(int i = 0; i < sz; ++i)
      vc[i+sz] = vc[i];

    return vc;
  }

  virtual bool IsInterfaceElement() const
  {
    return true;
  }
};

}

#endif /* I_INTERFACE_ELEMENT_H__INCLUDED */
