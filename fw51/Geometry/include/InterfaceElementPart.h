 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#if !defined(INTERFACE_ELEMENT_PART_H__INCLUDED_)
#define INTERFACE_ELEMENT_PART_H__INCLUDED_

/*!
	\class CInterfaceElementPart
	\brief Template representing a part of an interface.
  
  It could be a triangle, a point, a line or a quadrilateral.

	\author Antoine
	\date 21 april 2006
*/


#include "IElement.h"
#include "InterfaceElement.h"
#include "IQuadrilateral.h"
#include "ITriangle.h"
#include "ILine.h"
#include "IPointElement.h"

#include "GeometryExports.h"


namespace geo {


template <class BASE>
class CInterfaceElementPart : public BASE
{

	//the parent : the interface element
	const IElement& m_parent;
	//the side ; 0 for the Front, 1 for the Back
	const int m_side;

public:

	/*!
		Constructs one side of an interface element.
		\param parent the interface element
		\param side 0 for the Front, 1 for the Back
	*/
  CInterfaceElementPart(const IElement& parent, int side )
  : m_parent(parent), m_side(side)
	{
	}

  using BASE::Node;
	/*!
		Returns the n-th node of this side of the interface element.
		We take care of the 2+3, 6+3 and 8+4 elements on this method
	*/
	virtual const INode& Node( int n ) const
	{
		if ( 2*this->NrOfLines()*m_parent.Order()== m_parent.NrOfNodes()) {
			if ( m_side == 0)	// the Front
			{
				return m_parent.Node(n);
			}
			else 			// the Back
			{
				return m_parent.Node(n + (int)(m_parent.NrOfNodes()/2));
			}
		}
		// for 3+2, 6+3 8+4 elements (sur it's not nice)
		if (this->NrOfLines() <= 2 && m_parent.NrOfNodes()% 2 == 1) {
			if ( m_side == 0) 	// the Front
			{
				return m_parent.Node(n);
			}
			else 			// the Back
			{
				return m_parent.Node(n + (int)(m_parent.NrOfNodes()/2)+1);
			}
		} else if ( this->NrOfLines() > 2 &&
                2*this->NrOfLines()*m_parent.Order()!= m_parent.NrOfNodes() ) {
			if ( m_side == 0) 	// the Front
			{
				return m_parent.Node(n);
			}
			else 			// the Back
			{
				return m_parent.Node(n +(int)(this->NrOfLines()*m_parent.Order()));
			}
		} else {
      assert( this->NrOfLines() < 2 );
			if ( m_side == 0)	// the Front
			{
				return m_parent.Node(n);
			}
			else 			// the Back
			{
				return m_parent.Node(n + (int)(m_parent.NrOfNodes()/2));
			}
		}
	}


	/*!
		Returns the number of nodes of this side of the interface element.
		We take care of the 2+3, 6+3 and 8+4 elements on this method
	*/
	virtual size_t NrOfNodes() const
	{
		if ( 2*this->NrOfLines()*m_parent.Order()== m_parent.NrOfNodes())
			return m_parent.NrOfNodes()/2 ;
		// for 3+2, 6+3 8+4 elements (sur it's not nice)
		if (this->NrOfLines() <= 2 && m_parent.NrOfNodes()% 2 == 1)
		{
			if(m_side == 0) 	// the Front
			{
				return (int)(m_parent.NrOfNodes()/2)+1;
			}
			else  			//the Back
			{
				return (int)(m_parent.NrOfNodes()/2);
			}
		}
		if ((this->NrOfLines()>2) &&
        ( 2*this->NrOfLines()*m_parent.Order()!= m_parent.NrOfNodes()))
		{
			if(m_side == 0) 	// the Front
			{
				return (int)(this->NrOfLines()*m_parent.Order());
			}
			else  			//the Back
			{
				return (int)(this->NrOfLines()*(m_parent.Order()-1));
			}
		}
    assert( this->NrOfLines() < 2 );
		return m_parent.NrOfNodes()/2 ;
	}

	/*!
		Returns the order of this side of the interface element.
		We take care of the 2+3, 6+3 and 8+4 elements on this method
	*/
	virtual size_t Order() const
	{
		// for 3+2, 6+3 8+4 elements
		if ( ( this->NrOfLines() <= 2 && m_parent.NrOfNodes()% 2 == 1) ||
         ( this->NrOfLines() > 2  && 
           2*this->NrOfLines()*m_parent.Order()!= m_parent.NrOfNodes()))
		{
			//the front side has higher order than the back side
			if(m_side == 0)		//the Front
			{
				return m_parent.Order();
			}
			else			//the Back
			{
				assert(m_parent.Order() > 0);

				return m_parent.Order()-1;
			}
		}
		return m_parent.Order();
	}

	/*
    		Returns the mesh index of this element.
		The index of the part of the interface is the index of the interface element
	*/
	virtual int Index() const
	{
		return m_parent.Index();
	}

};

// Explicit template instantiation so we can export these instantiations
// (required for Windows DLL)
GEOMETRY_EXPORT_TEMPLATE template class GEOMETRY_EXPORT CInterfaceElementPart<ITriangle>;
GEOMETRY_EXPORT_TEMPLATE template class GEOMETRY_EXPORT CInterfaceElementPart<IQuadrilateral>;
GEOMETRY_EXPORT_TEMPLATE template class GEOMETRY_EXPORT CInterfaceElementPart<ILine>;
GEOMETRY_EXPORT_TEMPLATE template class GEOMETRY_EXPORT CInterfaceElementPart<IPointElement>;

typedef CInterfaceElementPart<ITriangle>      TInterfaceTriangleElement;
typedef CInterfaceElementPart<IQuadrilateral> TInterfaceQuadrilateralElement;
typedef CInterfaceElementPart<ILine>          TInterfaceLineElement;
typedef CInterfaceElementPart<IPointElement>  TInterfacePointElement;

};

#endif
