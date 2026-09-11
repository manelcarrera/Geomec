// Copyright 2011, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.

#ifndef RG_NODE
#define RG_NODE

#include <cassert>
#include <cmath>
#include <cstddef>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
/// IDs are 0-based and contiguously numbered
///////////////////////////////////////////////////////////////////////////////
typedef size_t RGNodeId;

///////////////////////////////////////////////////////////////////////////////
/// Node described as a point in space with XYZ coordinates
///////////////////////////////////////////////////////////////////////////////
class RGNode {
public:
  RGNode(const RGNode &tcp) {
    m_cors[0] = tcp.m_cors[0];
    m_cors[1] = tcp.m_cors[1];
    m_cors[2] = tcp.m_cors[2];
  }
  RGNode(const double &northing, const double &easting, const double &depth) {
    m_cors[0] = northing;
    m_cors[1] = easting;
    m_cors[2] = depth;
  }
  ~RGNode() {}

  double northing() const { return m_cors[0]; }
  double easting() const { return m_cors[1]; }
  double depth() const { return m_cors[2]; }

  const double &operator[](size_t n) const {
    assert(n < 3);
    return m_cors[n];
  }
  double &operator[](size_t n) {
    assert(n < 3);
    return m_cors[n];
  }

  /// @brief Compare two RGNode objects for equality with given precision
  /// @param tcm object to compare with
  /// @param eps precision
  bool isEqual(const RGNode &tcm, double eps) const {
    for (int i = 0; i < 3; ++i) {
      if (std::fabs(static_cast<float>(m_cors[i]) - static_cast<float>(tcm.m_cors[i])) > eps) {
        return false;
      }
    }
    return true;
  }

private:
  double m_cors[3];
};

#endif
