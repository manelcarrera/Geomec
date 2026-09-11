// Copyright 2011, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.

#include "RGSurfaceImpl.h"
#include "RGInterface.h"

#include <RescueArrayInt.h>
#include <RescueModel.h>
#include <sstream>
#include <stdexcept>

/// @brief Constructor
RGSurface::RGSurfaceImpl::RGSurfaceImpl(const std::string &name, const RGSurface::Type type,
                                        const RGSurface::Attribute attribute)
    : m_name(name), m_type(type), m_attribute(attribute) {
  assert(type < RGSurface::UndefinedType);
}

/// @brief Copy constructor
RGSurface::RGSurfaceImpl::RGSurfaceImpl(const RGSurface::RGSurfaceImpl &tcp)
    : m_name(tcp.m_name), m_polygons(tcp.m_polygons), m_type(tcp.m_type), m_attribute(tcp.m_attribute) {
  // empty
}

/// @brief Add a polygon for this surface
/// @ param polygon is a polygon Id, thus this polygon already exists in the grid
void RGSurface::RGSurfaceImpl::addPolygon(const RGPolygonId &polygon) { m_polygons.push_back(polygon); }

const std::string &RGSurface::RGSurfaceImpl::getName() const { return m_name; }

const RGPolygonId &RGSurface::RGSurfaceImpl::getPolygonN(const size_t &n) const { return m_polygons.at(n); }

size_t RGSurface::RGSurfaceImpl::getSize() const { return m_polygons.size(); }

RGSurface::Type RGSurface::RGSurfaceImpl::getType() const { return m_type; }

RGSurface::Attribute RGSurface::RGSurfaceImpl::getAttribute() const { return m_attribute; }

std::string RGSurface::RGSurfaceImpl::toString() const {
  std::ostringstream oss;
  oss << "(";
  if (m_polygons.size() > 0) {
    oss << m_polygons[0];
  }
  for (size_t i = 1; i < m_polygons.size(); ++i) {
    oss << "," << m_polygons[i];
  }
  oss << ")";
  return oss.str();
}
