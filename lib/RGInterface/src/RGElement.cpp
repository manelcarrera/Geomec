// Copyright 2014, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.

#include "RGElement.h"

static const size_t s_elementSize[RGElementType::typeElementLast] = {
    // Tetrahedron
    4,  // typeTE12L Linear
    10, // typeCTE30 Quadratic

    // Hexahedron
    8,  // typeHX24L Linear
    20, // typeCHX60 Quadratic

    // Interface
    6,  // typeT18IF 3-noded-tri-interface element
    12, // typeCT36I 6-noded-tri-interface element
    8,  // typeQ24IF 4-noded-quad-interface element
    16  // typeCQ48I 8-noded-quad-interface element
};

static const std::string s_elementName[RGElementType::typeElementLast] = {
    "Tetrahedron linear",    "Tetrahedron Quadratic", "Hexahedron linear",      "Hexahedron quadratic",
    "3-noded-tri-interface", "6-noded-tri-interface", "4-noded-quad-interface", "8-noded-quad-interface"};

std::string RGElementType::toString(RGElementType::Type e) { return s_elementName[e]; }

/// @brief Copy constructor
/// @parentGeometry tcp object to be copied
RGElement::RGElement(const RGElement &tcp) : m_type(tcp.m_type) {
  m_nodes = new RGNodeId[getNumNodes()];
  std::copy(tcp.m_nodes, tcp.m_nodes + getNumNodes(), m_nodes);
}

/// @brief Constructor
/// @param type element type
/// @param nodes array of nodes IDs. It is supposed that order of nodes is defined by calling program
RGElement::RGElement(const RGElementType::Type &type, const std::vector<RGNodeId> &nodes) : m_type(type) {
  assert(m_type != RGElementType::typeElementLast);
  assert(getNumNodes() == nodes.size());

  m_nodes = new RGNodeId[getNumNodes()];
  std::copy(nodes.begin(), nodes.end(), m_nodes);
}

/// @brief destructor
RGElement::~RGElement() { delete[] m_nodes; }

/// @brief Get element type
/// @return type of element
RGElementType::Type RGElement::getType() const { return m_type; }

/// @brief Get nodes number in this element
/// @return number of nodes
size_t RGElement::getNumNodes() const { return s_elementSize[m_type]; }

/// @brief Get node id for given node number
/// @return node id
RGNodeId RGElement::getNodeN(const size_t &n) const {
  assert(n < getNumNodes());
  return m_nodes[n];
}

size_t RGElement::getNumFaces(RGElementType::Type &elType) {
  return elType < RGElementType::typeHX24L ? 4 : (elType < RGElementType::typeT18IF ? 6 : 1);
}

bool RGElement::operator!=(const RGElement &tcm) const {
  return m_type != tcm.getType() || !std::equal(m_nodes, m_nodes + getNumNodes(), tcm.m_nodes);
}
