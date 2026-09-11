// Copyright 2011, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.


#include "RGInterface.h"
#include "RGSurface.h"
#include "RGSurfaceImpl.h"

#include <RescueModel.h>
#include <RescueArrayInt.h>
#include <sstream>
#include <stdexcept>


/// @brief Constructor
RGSurface::RGSurface( const std::string& name, const RGSurface::Type type, const RGSurface::Attribute attribute ) :
   m_pimpl( new RGSurface::RGSurfaceImpl( name, type, attribute ) ) {}

/// @brief Copy constructor
RGSurface::RGSurface( const RGSurface& tcp ) :
   m_pimpl( new RGSurfaceImpl( *( tcp.m_pimpl ) ) )
{
}

/// @brief Nothing specific
RGSurface::~RGSurface()
{
   delete m_pimpl;
}

/// @brief Add a polygon for this surface
/// @ param polygon is a polygon Id, thus this polygon already exists in the grid
void RGSurface::addPolygon( const RGPolygonId& polygon ) { m_pimpl->addPolygon( polygon ); }

const std::string& RGSurface::getName() const { return m_pimpl->getName(); }

const RGPolygonId& RGSurface::getPolygonN( const size_t& n) const { return m_pimpl->getPolygonN( n ); }

size_t RGSurface::getSize() const { return m_pimpl->getSize(); }

RGSurface::Type RGSurface::getType() const { return m_pimpl->getType(); }

RGSurface::Attribute RGSurface::getAttribute() const { return m_pimpl->getAttribute(); }
  
std::string RGSurface::toString() const
{
   return m_pimpl->toString();
}


const std::string c_surfaceAttribute2String[] = { "Regular", "Top", "Bottom", "Side", "UndefinedAttribute" };
std::string RGSurface::toString( const RGSurface::Attribute  att )
{
   return c_surfaceAttribute2String[ att ];
}

RGSurface::Attribute RGSurface::attributeFromString( const std::string& str )
{
   RGSurface::Attribute result = RGSurface::UndefinedAttribute;
   for ( size_t attribute = 0; attribute < RGSurface::UndefinedAttribute; ++attribute)
   {
      if ( c_surfaceAttribute2String[ attribute ].compare( str ) == 0 )
      {
         result = static_cast<RGSurface::Attribute>(attribute);
      }
   }
   return result;
}

const std::string c_surfaceType2String[] = { "Horizon", "FaultFront", "FaultBack", "UndefinedType" };
std::string RGSurface::toString( const RGSurface::Type  type )
{
   return c_surfaceType2String[ type ];
}

RGSurface::Type RGSurface::typeFromString( const std::string& str )
{
   RGSurface::Type result = RGSurface::UndefinedType;
   for ( size_t type = 0; type < RGSurface::UndefinedType; ++type)
   {
      if ( c_surfaceType2String[ type ].compare( str ) == 0 )
      {
         result = static_cast<RGSurface::Type>(type);
      }
   }
   return result;
}
