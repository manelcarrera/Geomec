 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#if !defined(INTERFACE_ELEMENT_TEMPL_H__INCLUDED_)
#define INTERFACE_ELEMENT_TEMPL_H__INCLUDED_

#include "IInterfaceElement.h"
#include "InterfaceElementPart.h"

#include "GeometryExports.h"

/*!

	\class CInterfaceElementTempl
	\brief Template to create an interface element between two interface element parts.
  
  It is among others, used by the DiaElementBase to instantiate an interface
  element with two interface element parts which are two IElement
  specializations.

  \sa CInterfaceElementPart

	\author Antoine
	\date 21 april 2006
*/

namespace geo {

template <class BASE>
class CInterfaceElementTempl : public IInterfaceElement
{
  //the two parts of the interface
  const IElement* m_Front;
 	const IElement* m_Back;

public:

	/*!
		The default constructor. Front and back side of the element are constructed.
	*/
	CInterfaceElementTempl() : m_Front( 0 ), m_Back( 0 )
	{
    m_Front = new BASE(*this,0);
    m_Back  = new BASE(*this, 1);
	}

	/*!
		Return the Front part of the interface
	*/
	virtual const IElement& Front() const
	{
		return *m_Front;
	}

	/*!
		Return the Back part of the interface
	*/
	virtual const IElement& Back() const
	{
		return *m_Back;
	}


};

// Explicit template instantiation so we can export these instantiations
// (required for Windows DLL)
GEOMETRY_EXPORT_TEMPLATE template class GEOMETRY_EXPORT CInterfaceElementTempl<TInterfaceTriangleElement>;
GEOMETRY_EXPORT_TEMPLATE template class GEOMETRY_EXPORT CInterfaceElementTempl<TInterfaceQuadrilateralElement>;
GEOMETRY_EXPORT_TEMPLATE template class GEOMETRY_EXPORT CInterfaceElementTempl<TInterfaceLineElement>;
GEOMETRY_EXPORT_TEMPLATE template class GEOMETRY_EXPORT CInterfaceElementTempl<TInterfacePointElement>; 

typedef CInterfaceElementTempl<TInterfaceTriangleElement> TInterfaceTriangle;
typedef CInterfaceElementTempl<TInterfaceQuadrilateralElement> TInterfaceQuadrilateral;
typedef CInterfaceElementTempl<TInterfaceLineElement>  TInterfaceLine;
typedef CInterfaceElementTempl<TInterfacePointElement> TInterfacePoint;

};
#endif // INTERFACE_ELEMENT_TEMPL_H__INCLUDED_
