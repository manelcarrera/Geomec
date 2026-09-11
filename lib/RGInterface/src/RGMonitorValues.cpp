// Copyright 2020, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.


#include "RGInterfaceExport.h"
#include "RGMonitorValues.h"


///////////////////////////////////////////////////////////////////////////////
/// @class RGMonitorValues
/// @brief Acces to monitored values
///////////////////////////////////////////////////////////////////////////////RGMonitorPointSets

RGMonitorValues::RGMonitorValues(const RGDepletionStage& depletionStage, const RGMonitorPointSets& pointSets) : 
	m_depletionStage(depletionStage), m_pointSets(pointSets)
{
	//Let's reserve the memory to store the monitorable values
	int numPointSets = pointSets.GetNumPointSets();

	m_monitorValues.resize(numPointSets);
	
	for (int i = 0; i < numPointSets; i++)
	{
		int numPoints = pointSets.GetPointSet(i).GetNumPoints();
		m_monitorValues[i].resize(numPoints);

		//Number of Properties selected will be same for all the points under this pointset
		int numProps = pointSets.GetProperties(i).size(); 

		for (int j = 0; j < numPoints; j++)
		{
			m_monitorValues[i][j].resize(numProps);
		}
	}
}

const RGMonitorPointSets& RGMonitorValues::GetPointSets() const
{
	return m_pointSets;
}

RGDepletionStage RGMonitorValues::GetDepletionStage() const
{
	return m_depletionStage;
}


void RGMonitorValues::Set(const int& pointSetIndex, const int& pointIndex, const int& propertyIndex, double value)
{
	// Error if property is not in props, or point index out of bounds, or value not set
	if (pointSetIndex > m_pointSets.GetNumPointSets() || pointIndex > m_pointSets.GetPointSet(pointSetIndex).GetNumPoints() || propertyIndex > m_pointSets.GetProperties(pointSetIndex).size() )
	{
		throw std::runtime_error("Invalid Index Passed to set the value");
	}

	m_monitorValues[pointSetIndex][pointIndex][propertyIndex] = value;
}

double RGMonitorValues::Get(const int& pointSetIndex, const int& pointIndex, const int& propertyIndex) const
{
	// Error if property is not in props, or point index out of bounds, or value not set
	if (pointSetIndex > m_pointSets.GetNumPointSets() || pointIndex > m_pointSets.GetPointSet(pointSetIndex).GetNumPoints() || propertyIndex > m_pointSets.GetProperties(pointSetIndex).size())
	{
		throw std::runtime_error("Invalid Index Passed to get the value");
	}

	return m_monitorValues[pointSetIndex][pointIndex][propertyIndex];
}

void RGMonitorValues::clear()
{
	m_monitorValues.clear();
}
