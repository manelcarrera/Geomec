// Copyright 2011, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.

#include <RGProperty.h>

#include <cassert>

static const char *c_propType2String[RGPropertyType::propLast] = {
    "FluidPressure" // propElementFluidPressure
    ,
    "Temperature" // propElementTemperature
    ,
    "VolumetricStrain" // propElementVolumetricStrain
    ,
    "DeltaFluidPressure" // propElementDeltaFluidPressure
    ,
    "DeltaTemperature" // propElementDeltaTemperature
    ,
    "EffStress_NN" // propElementEffStress_NN
    ,
    "EffStress_EE" // propElementEffStress_EE
    ,
    "EffStress_DD" // propElementEffStress_DD
    ,
    "EffStress_NE" // propElementEffStress_NE
    ,
    "EffStress_ND" // propElementEffStress_ND
    ,
    "EffStress_ED" // propElementEffStress_ED
    ,
    "Formation" // propElementFormation
    ,
    "Young" // propElementYoung
    ,
    "Poisson" // propElementPoisson
    ,
    "Porosity" // propElementPorosity
    ,
    "SaturatedRockDensity" // propSaturatedRockDensity
    ,
    "FluidPressureFracture" // propElementFluidPressureFrac
    ,
    "TemperatureFracture" // propElementTemperatureFrac
    ,
    "VolumetricStrainFracture" // propElementVolumetricStrainFrac
    ,
    "DeltaFluidPressureFracture" // propElementDeltaFluidPressureFrac
    ,
    "DeltaTemperatureFracture" // propElementDeltaTemperatureFrac
    ,
    "FractureDensity_1_N" // propFracDens_1_N
    ,
    "FractureDensity_1_E" // propFracDens_1_E
    ,
    "FractureDensity_1_D" // propFracDens_1_D
    ,
    "FractureDensity_2_N" // propFracDens_2_N
    ,
    "FractureDensity_2_E" // propFracDens_2_E
    ,
    "FractureDensity_2_D" // propFracDens_2_D
    ,
    "FractureDensity_3_N" // propFracDens_3_N
    ,
    "FractureDensity_3_E" // propFracDens_3_E
    ,
    "FractureDensity_3_D" // propFracDens_3_D
    ,
    "DeltaDesorptionStrain" // propDeltaDesorptionStrain
    ,
    "permMultFrac_1" // propPermMultFrac_1
    ,
    "permMultFrac_2" // propPermMultFrac_2
    ,
    "permMultFrac_3" // propPermMultFrac_3
    ,
    "BodyIndex" // propBodyIndex
    ,
    "ShearModulus" // propElementShearModulus
    ,
    "Orientation_Dip" // propElementOrientation_Dip
    ,
    "Orientation_Azimuth" // propElementOrientation_Azimuth
    ,
    "YoungNormal" // propElementYoungNormal
    ,
    "YoungPlane" // propElementYoungPlane
    ,
    "PoissonNormal" // propElementPoissonNormal
    ,
    "PoissonPlane" // propElementPoissonPlane
    ,
    "ShearNormal" // propElementShearNormal
    ,
    "MaterialType" // propElementMaterialType
    ,
    "GrainBulkModulus" // propElementGrainBulkModulus
    ,
    "StrainLoadingNormal" // propElementStrainLoadingNormal
    ,
    "StrainLoadingPlane" // propElementStrainLoadingPlane
    ,
    "TotStress_NN" // propElementTotStress_NN
    ,
    "TotStress_EE" // propElementTotStress_EE
    ,
    "TotStress_DD" // propElementTotStress_DD
    ,
    "TotStress_NE" // propElementTotStress_NE
    ,
    "TotStress_ND" // propElementTotStress_ND
    ,
    "TotStress_ED" // propElementTotStress_ED
    ,
    "TotStrain_NN" // propElementTotStrain_NN
    ,
    "TotStrain_EE" // propElementTotStrain_EE
    ,
    "TotStrain_DD" // propElementTotStrain_DD
    ,
    "TotStrain_NE" // propElementTotStrain_NE
    ,
    "TotStrain_ND" // propElementTotStrain_ND
    ,
    "TotStrain_ED" // propElementTotStrain_ED
    ,
    "ResidualStrainLoading_NN" // propElementResidualStrainLoading_NN
    ,
    "ResidualStrainLoading_EE" // propElementResidualStrainLoading_EE
    ,
    "ResidualStrainLoading_DD" // propElementResidualStrainLoading_DD
    ,
    "ResidualStrainLoading_NE" // propElementResidualStrainLoading_NE
    ,
    "ResidualStrainLoading_ND" // propElementResidualStrainLoading_ND
    ,
    "ResidualStrainLoading_ED" // propElementResidualStrainLoading_ED
    ,
    "YoungModulusUndrained" // propElementYoungUndrained
    ,
    "PoissonRatioUndrained" // propElementPoissonUndrained
    ,
    "IsReservoir" // propElementIsReservoir
    ,
    "CentroidX" // propElementCentroidX
    ,
    "CentroidY" // propElementCentroidY
    ,
    "CentroidZ" // propElementCentroidZ
};

///////////////////////////////////////////////////////////////////////////////
// Documented in .h
///////////////////////////////////////////////////////////////////////////////
std::string RGProperty::toString(const RGPropertyType::Type &propType) {
  assert(propType >= RGPropertyType::propFirst);
  assert(propType < RGPropertyType::propLast);
  return c_propType2String[propType];
}

std::string RGProperty::toString() const { return toString(m_Type); }

///////////////////////////////////////////////////////////////////////////////
// Documented in .h
///////////////////////////////////////////////////////////////////////////////
RGPropertyType::Type RGProperty::fromString(const std::string &str) {
  size_t len = str.length();
  RGPropertyType::Type propType = RGPropertyType::propFirst;
  for (; propType != RGPropertyType::propLast; ++propType) {
    const char *propString = c_propType2String[propType];
    if (str.compare(0, len, propString, 0, len) == 0) {
      return propType;
    }
  }
  return propType;
}

///////////////////////////////////////////////////////////////////////////////
/// @param Element type to test
/// @return false if this property is not defined in that kind of element
///////////////////////////////////////////////////////////////////////////////
bool RGProperty::isDefined(const RGElementType::Type &elementType) const {
  bool result = true;
  if (elementType >= RGElementType::typeT18IF && elementType <= RGElementType::typeCQ48I) // interface
  {
    switch (m_Type) {
    case RGPropertyType::propElementFluidPressure:
    case RGPropertyType::propElementTemperature:
    case RGPropertyType::propElementDeltaFluidPressure:
    case RGPropertyType::propElementDeltaTemperature:
    case RGPropertyType::propElementFluidPressureFrac:
    case RGPropertyType::propElementTemperatureFrac:
    case RGPropertyType::propElementDeltaFluidPressureFrac:
    case RGPropertyType::propElementDeltaTemperatureFrac:
      result = true;
      break;
    default:
      result = false;
    }
  }

  return result;
}
