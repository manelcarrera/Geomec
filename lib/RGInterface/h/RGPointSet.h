// Copyright 2020, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.

#ifndef RG_MONITOR_POINTSET_H
#define RG_MONITOR_POINTSET_H

#include "RGInterfaceExport.h"

#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
/// @class RGMonitorPointSet
/// @brief A single point set that will be  monitoring
///////////////////////////////////////////////////////////////////////////////
class INTERFACE_RGINTERFACE RGPointSet {
public:
  // Exception is thrown if
  // - Name is empty or whitespace
  // - Name has leading/trailing spaces
  // - Length of coordinate vectors not equal
  // - Point vectors empty
  RGPointSet() {}
  RGPointSet(const std::string &name, const std::vector<double> &northing, const std::vector<double> &easting,
             const std::vector<double> &depth);

  std::string GetName() const;
  size_t GetNumPoints() const;
  void GetNthPoint(int n, double &northing, double &easting, double &depth) const;

private:
  std::string m_name;
  std::vector<double> m_northing;
  std::vector<double> m_easting;
  std::vector<double> m_depth;
};

#endif
