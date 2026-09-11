// Copyright 2011, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.

#ifndef RG_ELEMENT_H
#define RG_ELEMENT_H

#include "RGInterfaceExport.h"
#include "RGNode.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
/// IDs are 0-based and contiguously numbered
///////////////////////////////////////////////////////////////////////////////
typedef size_t RGElementId;

namespace RGElementType {
///////////////////////////////////////////////////////////////////////////////
/// Enumeration for element types; use DIANA names
///////////////////////////////////////////////////////////////////////////////
enum Type {
  // Tetrahedron
  typeTE12L, // Linear
  typeCTE30, // Quadratic

  // Hexahedron
  typeHX24L, // Linear
  typeCHX60, // Quadratic

  // Interface
  typeT18IF, // 3-noded-tri-interface element
  typeCT36I, // 6-noded-tri-interface element
  typeQ24IF, // 4-noded-quad-interface element
  typeCQ48I, // 8-noded-quad-interface element

  typeElementLast
};

INTERFACE_RGINTERFACE std::string toString(Type);
} // namespace RGElementType

///////////////////////////////////////////////////////////////////////////////
/// @class RGElement
/// @brief Base class for elements
///////////////////////////////////////////////////////////////////////////////
class INTERFACE_RGINTERFACE RGElement {
public:
  /// @brief Copy constructor
  /// @parentGeometry tcp object to be copied
  RGElement(const RGElement &tcp);

  /// @brief Constructor
  /// @param type element type
  /// @param nodes array of nodes IDs. It is supposed that order of nodes is defined by calling program
  RGElement(const RGElementType::Type &type, const std::vector<RGNodeId> &nodes);

  /// @brief Destructor
  ~RGElement();

  /// @brief Get element type
  /// @return type of element
  RGElementType::Type getType() const;

  /// @brief Get nodes number in this element
  /// @return number of nodes
  size_t getNumNodes() const;

  /// @brief Get node id for given node number
  /// @return node id
  RGNodeId getNodeN(const size_t &n) const;

  static size_t getNumFaces(RGElementType::Type &elType);

  bool operator!=(const RGElement &tcm) const;

private:
  RGElementType::Type m_type; /// element type
  RGNodeId *m_nodes;          /// array of nodes IDs. STL containers are avoid due to DLL export
                              /// Faces are defined by element type and order of the nodes
};

#endif
