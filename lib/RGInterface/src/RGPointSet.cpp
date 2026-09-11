// Copyright 2020, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.

#include "RGInterfaceExport.h"
#include "RGPointSet.h"

#include <stdexcept>
#include <string>
#include <vector>
#include <cassert>
// Exception is thrown if
// - Name is empty or whitespace
// - Name has leading/trailing spaces
// - Length of coordinate vectors not equal
// - Point vectors empty

RGPointSet::RGPointSet(const std::string &name, const std::vector<double>& northing, const std::vector<double>& easting, const std::vector<double>& depth)
{
  int nSize = northing.size();
  int eSize = easting.size();
  int dSize = depth.size();

  if ((nSize != eSize && eSize != dSize) || name=="" || name.find(' ') != std::string::npos || nSize == 0 || eSize == 0 || dSize == 0 )
  {
    throw std::runtime_error("RGPointSet::RGPointSet() - Invalid input values");
  }	
  m_name = name;
  m_northing = northing;
  m_easting = easting;
  m_depth = depth;
}

std::string RGPointSet::GetName() const
{
  return m_name;
}

size_t RGPointSet::GetNumPoints() const
{
  return m_easting.size(); //Since all X, Y, Z are having same size so return one of the vector size
}

void RGPointSet::GetNthPoint(int n, double& northing, double& easting, double& depth) const
{
  assert(n >= 0);
  northing = m_northing.at(n);
  easting = m_easting.at(n);
  depth = m_depth.at(n);
}
