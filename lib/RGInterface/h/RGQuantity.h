// Copyright 2020, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.

#ifndef RG_QUANTITY_H
#define RG_QUANTITY_H

#include "RGInterfaceExport.h"

#include <string>
#include <vector>

#ifndef _WIN32
#include <algorithm>
#endif // !_WIN32
///////////////////////////////////////////////////////////////////////////////
/// @class RGQuantity
/// @brief Predegined strings for quantities
///////////////////////////////////////////////////////////////////////////////
namespace RGQuantity {
const std::vector<std::string> m_rgQuantities = {
    "Acceleration", "Angle", "Cohesion", "Compressibility", "CrackOpening", "Curvature", "Density", "Force",
    "FractureEnergy", "GigaSinglePressure", "Pressure", "Length", "LossCoef", "None", "Percentage", "PressureGradient",
    "SinglePressure", "SmallLength", "Strain", "StressGradient", "TemperatureGradient", "Temperature",
    "ThermalConductivity", "ThermalExpansion", "TimeD", "TimeH", "VolumetricHeat", "VolumetricSpecificHeat",
    "WeightPerUnitLength", "Velocity",
    // and the backdoor.. Unknown are Quantities for User defined properties
    "Unknown"};

bool IsKnownQuantity(const std::string &quantity) {
  // If the quantity is empty or it has spaces in it then return false
  if (quantity == "" || quantity.find(" ") != std::string::npos)
    return false;

  return (std::find(m_rgQuantities.begin(), m_rgQuantities.end(), quantity) != m_rgQuantities.end() ? true : false);
}
}; // namespace RGQuantity

#endif
