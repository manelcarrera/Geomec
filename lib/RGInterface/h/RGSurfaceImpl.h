// Copyright 2013, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.


#ifndef RG_SURFACE_IMPL_H
#define RG_SURFACE_IMPL_H

#include "RGInterfaceExport.h"

#include <string>
#include <vector>
#include "RGNode.h"
#include "RGSurface.h"

class RGSurface::RGSurfaceImpl
{
public:

   RGSurfaceImpl( const std::string& name, const RGSurface::Type type, const RGSurface::Attribute = Regular );
   RGSurfaceImpl( const RGSurfaceImpl& tcp );
   void addPolygon( const RGPolygonId& polygon );

   const std::string& getName() const;
   const RGPolygonId& getPolygonN( const size_t& n) const;
   size_t getSize() const;
   Type getType() const;
   Attribute getAttribute() const;

   /// @brief Create string representation of the object
   /// @return string in form "(nodeId,x,y,z)"
   std::string toString() const;

private:
   std::string                m_name;
   std::vector< RGPolygonId > m_polygons;
   RGSurface::Type            m_type;
   RGSurface::Attribute       m_attribute;
};

#endif

