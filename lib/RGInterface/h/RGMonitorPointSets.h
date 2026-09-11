// Copyright 2020, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.


#ifndef RG_MONITOR_POINTSETS_H
#define RG_MONITOR_POINTSETS_H

#include "RGInterfaceExport.h"
#include "RGGeneralProperty.h"
#include "RGPointSet.h"

#include <unordered_map>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
/// @class RGMonitorPointSets
/// @brief Collection of all point sets and properties
///////////////////////////////////////////////////////////////////////////////
class INTERFACE_RGINTERFACE RGMonitorPointSets
{
public:
  RGMonitorPointSets() { m_totalCount = 0; }
  void Add(const RGPointSet& pointSet, const std::vector<RGGeneralProperty>& props);
  const RGPointSet& GetPointSet(int n) const;
  const std::vector<RGGeneralProperty>& GetProperties(int n) const;
  size_t GetNumPointSets() const;
  size_t GetTotalCount() const;
  void clear();

private:
  std::vector<std::pair<RGPointSet, std::vector<RGGeneralProperty> > > m_monitorPointSets;
  
  //This is required to allocate Rescue Table while both dump and load operations. 
  size_t m_totalCount; 
};

#endif


