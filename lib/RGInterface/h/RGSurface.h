// Copyright 2013, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.

#ifndef RG_SURFACE_H
#define RG_SURFACE_H

#include "RGInterfaceExport.h"

#include "RGNode.h"
#include <string>
#include <vector>

typedef std::vector<RGNodeId> RGPolygon;
typedef size_t RGPolygonId;

typedef size_t RGSurfaceId;

///////////////////////////////////////////////////////////////////////////////
/// Each RGSurface refers to a Horizon, Fault (front and back)
/// A RGSurface is defined by a list of Polygon ids.
///
/// Different flavours surfaces can be distinguished by there Type/Attribute:
///
///  Top Horizon        => Horizon + Top
///  Bottom Horizon     => Horizon + Bottom
///  Internal Horizon   => Horizon + Regular
///  Fault              => ( FaultFront xor FaultBack ) + Regular
///  Bounding Fault     => Horizon + Side
///
///  The rationale for handling bounding faults as horizons is that faults
///  are two-sided surfaces (= possbily different grids on either side) whereas
///  horizons are single sided surfaces.
///  Any other Type/Attribute combination should be treated as an error.
///
///////////////////////////////////////////////////////////////////////////////
class INTERFACE_RGINTERFACE RGSurface {
public:
  enum Type { Horizon = 0, FaultFront, FaultBack, UndefinedType };

  enum { NumTypes = UndefinedType };

  enum Attribute { Regular = 0, Top, Bottom, Side, UndefinedAttribute };

  RGSurface(const std::string &name, const RGSurface::Type type, const RGSurface::Attribute = Regular);
  RGSurface(const RGSurface &);
  ~RGSurface();

  void addPolygon(const RGPolygonId &polygon);

  const std::string &getName() const;
  const RGPolygonId &getPolygonN(const size_t &n) const;
  size_t getSize() const;
  Type getType() const;
  Attribute getAttribute() const;

  /// @brief Create string representation of the object
  /// @return string in form "(nodeId,x,y,z)"
  std::string toString() const;

  static std::string toString(const Attribute);
  static std::string toString(const Type);

  static Attribute attributeFromString(const std::string &);
  static Type typeFromString(const std::string &);

private:
  // Implementation is hidden
  class RGSurfaceImpl;
  RGSurfaceImpl *m_pimpl;
};

#endif
