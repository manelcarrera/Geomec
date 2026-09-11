// Copyright 2020, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.

#ifndef RG_MONITOR_VALUES_H
#define RG_MONITOR_VALUES_H

#include "RGInterfaceExport.h"
#include "RGMonitorPointSets.h"
#include "RGDepletionStage.h"

#include<map>

///////////////////////////////////////////////////////////////////////////////
/// @class RGMonitorValues
/// @brief Acces to monitoring values
///////////////////////////////////////////////////////////////////////////////

class INTERFACE_RGINTERFACE RGMonitorValues
{
public:
	RGMonitorValues(){}
	RGMonitorValues(const RGDepletionStage& depletionStage, const RGMonitorPointSets& pointSets);
	const RGMonitorPointSets& GetPointSets() const;
	RGDepletionStage GetDepletionStage() const;
	void Set(const int& pointSetIndex, const int& pointIndex, const int& propertyIndex, double value);
	double Get(const int& pointSetIndex, const int& pointIndex, const int& propertyIndex) const;
	void clear();

private:
	RGDepletionStage m_depletionStage; 
	RGMonitorPointSets m_pointSets;

	//This class object will be created and returned to Flow for every depletion stage
	//3 DIM vector to store monitor value in the below format
	//m_monitorValues[pointSetIndex][pointIndex][propertyIndex]
	std::vector<std::vector<std::vector<double>>> m_monitorValues;
};

#endif
