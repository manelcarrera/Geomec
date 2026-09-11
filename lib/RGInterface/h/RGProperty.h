// Copyright 2011, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.

#ifndef RG_PROPERTY
#define RG_PROPERTY

#include "RGElement.h"
#include "RGInterfaceExport.h"

#include <cfloat>
#include <string>

namespace RGPropertyType {
///////////////////////////////////////////////////////////////////////////////
/// @brief Enumeration for gridded properties. All properties are specified in SI units
///////////////////////////////////////////////////////////////////////////////
enum Type {
  propFirst = 0,
  propElementFluidPressure = 0,  /// fluid pressure (non fractured)
  propElementTemperature,        /// temperature
  propElementVolumetricStrain,   /// volumetric strain
  propElementDeltaFluidPressure, /// changes in fluid pressure since previous depletion stage
  propElementDeltaTemperature,   /// changes in temperature since previous depletion stage

  // effective stress tensor in North-East-Depth coordinate system
  propElementEffStress_NN, /// diagonal North-North value
  propElementEffStress_EE, /// diagonal East-East value
  propElementEffStress_DD, /// diagonal Depth-Depth value
  propElementEffStress_NE, /// off diagonal North-East value
  propElementEffStress_ND, /// off diagonal North-Depth value
  propElementEffStress_ED, /// off diagonal East-Depth value

  propElementFormation, /// integer property marking the formation

  // Isotropic linear elastic behavior
  propElementYoung,   /// Young's modulus; E > 0
  propElementPoisson, /// Poisson's ratio: 0 <= nu < 0.5

  propElementPorosity,      /// Porosity 0 <= phi <= 1
  propSaturatedRockDensity, /// Density

  propElementFluidPressureFrac,      /// fluid pressure (in fracture)
  propElementTemperatureFrac,        /// temperature  in Fracture
  propElementVolumetricStrainFrac,   /// volumetric strain in fracture
  propElementDeltaFluidPressureFrac, /// changes in fracture fluid pressure since previous depletion stage
  propElementDeltaTemperatureFrac,   /// changes in fracture temperature since previous depletion stage

  propFracDens_1_N, /// Fracture density 1 (highest), Northing component of direction vector
  propFracDens_1_E, /// Fracture density 1 (highest), Easting component of direction vector
  propFracDens_1_D, /// Fracture density 1 (highest), Depth component of direction vector

  propFracDens_2_N, /// Fracture density 2 (middle), Northing component of direction vector
  propFracDens_2_E, /// Fracture density 2 (middle), Easting component of direction vector
  propFracDens_2_D, /// Fracture density 2 (middle), Depth component of direction vector

  propFracDens_3_N, /// Fracture density 3 (lowest), Northing component of direction vector
  propFracDens_3_E, /// Fracture density 3 (lowest), Easting component of direction vector
  propFracDens_3_D, /// Fracture density 3 (lowest), Depth component of direction vector

  propDeltaDesorptionStrain, /// CMB: change in volumetric strain caused by gas release

  propPermMultFrac_1, /// Permeability multiplier in fracture direction 1 (highest)
  propPermMultFrac_2, /// Permeability multiplier in fracture direction 2 (middle)
  propPermMultFrac_3, /// Permeability multiplier in fracture direction 1 (lowest)

  propBodyIndex, /// Per cell its body Id

  propElementShearModulus, /// Shear modulus

  // Transverse isotropic geometric properties
  propElementOrientation_Dip,     /// Dip angle (from xy-plan) in rad
  propElementOrientation_Azimuth, /// Azimuth angle (from North) in rad

  // Transverse isotropic mechanical properties
  propElementYoungNormal,   /// Young Normal
  propElementYoungPlane,    /// Young Plane
  propElementPoissonNormal, /// => Geomec: Poisson Normal => Diana: Poisson_Normal_Transverse
  propElementPoissonPlane,  /// Poisson Plane
  propElementShearNormal,   /// => Geomec: Anisotropic Shear Modulus => Diana: Shear_Transverse_Normal

  // Material type
  propElementMaterialType, /// cf RGMaterialType

  // PoroElastic properties
  propElementGrainBulkModulus, /// Kg

  // Strain loading
  propElementStrainLoadingNormal, /// Normal component of the strain loading
  propElementStrainLoadingPlane,  /// Transverse component of the strain loading

  // Total stress tensor in North-East-Depth coordinate system
  propElementTotalStress_NN, /// diagonal North-North value
  propElementTotalStress_EE, /// diagonal East-East value
  propElementTotalStress_DD, /// diagonal Depth-Depth value
  propElementTotalStress_NE, /// off diagonal North-East value
  propElementTotalStress_ND, /// off diagonal North-Depth value
  propElementTotalStress_ED, /// off diagonal East-Depth value

  // Total strain tensor in North-East-Depth coordinate system
  propElementTotalStrain_NN, /// diagonal North-North value
  propElementTotalStrain_EE, /// diagonal East-East value
  propElementTotalStrain_DD, /// diagonal Depth-Depth value
  propElementTotalStrain_NE, /// off diagonal North-East value
  propElementTotalStrain_ND, /// off diagonal North-Depth value
  propElementTotalStrain_ED, /// off diagonal East-Depth value

  // Residual strain loading tensor in North-East-Depth coordinate system
  propElementResidualStrainLoading_NN, /// diagonal North-North value
  propElementResidualStrainLoading_EE, /// diagonal East-East value
  propElementResidualStrainLoading_DD, /// diagonal Depth-Depth value
  propElementResidualStrainLoading_NE, /// off diagonal North-East value
  propElementResidualStrainLoading_ND, /// off diagonal North-Depth value
  propElementResidualStrainLoading_ED, /// off diagonal East-Depth value

  propElementYoungUndrained,   /// Young modulus shale
  propElementPoissonUndrained, /// Poisson ratio shale
  propElementIsReservoir,      /// is the element part of a reservoir formation
  propElementCentroidX,        /// x-coordinate of centroid of RE cell
  propElementCentroidY,        /// y-coordinate of centroid of RE cell
  propElementCentroidZ,        /// z-coordinate of centroid of RE cell
  propLast
};

inline RGPropertyType::Type &operator++(RGPropertyType::Type &p) { return p = RGPropertyType::Type(p + 1); }
} // namespace RGPropertyType

///////////////////////////////////////////////////////////////////////////////
/// @class Property
///////////////////////////////////////////////////////////////////////////////
class INTERFACE_RGINTERFACE RGProperty {
public:
  RGProperty(const RGPropertyType::Type &type) : m_Type(type) { ; }
  RGProperty() : m_Type(RGPropertyType::propLast) { ; }

  RGPropertyType::Type getType() const { return m_Type; }
  bool isValid() const { return m_Type != RGPropertyType::propLast; } // Empty constructor!

  /// @brief Convert property type to string
  /// @return string representation of current property
  static std::string toString(const RGPropertyType::Type &);
  std::string toString() const;
  /// @brief Convert string to RGPropertyType
  /// @param str property type name
  /// @return propLast if string can not be converted to RGPropertyType or RGPropertyType value otherwise
  static RGPropertyType::Type fromString(const std::string &str);

  /// @brief Comparison for unequality of two RGProperty objects
  /// @param tcm other object for comparison
  /// @return true if any field in tcm object is different from the same field in current object, false otherwise
  bool operator!=(const RGProperty &tcm) const { return m_Type != tcm.m_Type; }

  /// @brief Comparison for equality of two RGProperty objects
  /// @param tcm other object for comparison
  /// @return false if any field in tcm object is different from the same field in current object, true otherwise
  bool operator==(const RGProperty &tcm) const { return !(*this != tcm); }

  /// @brief
  /// @param
  /// @return false if this property is not defined in that kind of element
  bool isDefined(const RGElementType::Type &) const;

private:
  RGPropertyType::Type m_Type;
};

#endif
