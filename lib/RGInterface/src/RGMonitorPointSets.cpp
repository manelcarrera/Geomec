// Copyright 2020, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.

#include "RGMonitorPointSets.h"
#include "RGInterfaceExport.h"

///////////////////////////////////////////////////////////////////////////////
/// @class RGMonitorPointSets
/// @brief Collection of all point sets and properties
///////////////////////////////////////////////////////////////////////////////

void RGMonitorPointSets::Add(const RGPointSet &pointSet, const std::vector<RGGeneralProperty> &props) {
  m_monitorPointSets.emplace_back(std::make_pair(pointSet, props));
  m_totalCount += (pointSet.GetNumPoints() * props.size());
}

const RGPointSet &RGMonitorPointSets::GetPointSet(int n) const { return m_monitorPointSets.at(n).first; }

const std::vector<RGGeneralProperty> &RGMonitorPointSets::GetProperties(int n) const {
  return m_monitorPointSets.at(n).second;
}

size_t RGMonitorPointSets::GetNumPointSets() const { return m_monitorPointSets.size(); }

size_t RGMonitorPointSets::GetTotalCount() const { return m_totalCount; }

void RGMonitorPointSets::clear() { m_monitorPointSets.clear(); }
