#include "FilterOutputProperty.h"
#include "Object.h"

namespace cora {

CFilterOutputProperty::CFilterOutputProperty() {}

namespace {

const QString FAULT_SHEAR_FAILURE = "FaultShearCap";
const QString FAULT_TOTAL_SLIP = "FaultDisplacementL";
const QString FAULT_PLASTIC_SLIP = "FaultDisplacementL";

const QString FORMATION_SHEAR_FAILURE = "ShearCap";
const QString FORMATION_COMPRESSIVE_FAILURE = "EPStress1L";
const QString FORMATION_TENSILE_FAILURE = "EPStress3L";
const QString FORMATION_COMPACTION_FAILURE = "PStrain1L";
const QString FORMATION_EXTENSION_FAILURE = "PStrain3L";
const QString FORMATION_VOLUMETRIC_COMPACTION = "StrainInvVolumetric";
const QString FORMATION_COMPRESSIVE_PLASTIC_STRAIN = "PStrain1PlasticL";
const QString FORMATION_EXTENSIONAL_PLASTIC_STRAIN = "PStrain3PlasticL";

const QString WELL_SHEAR_FAILURE = "ShearCap";
const QString WELL_AXIAL_COMPRESSION_EXTENSION = "WPAxialStrain";

const QString TOP_SURFACE_SUBSIDENCE_UPHEAVE = "DisplacementV";

} // anonymous namespace

bool CFilterOutputProperty::isOutputPropertyAllowed(const CObject &object, const QString &result) const {
  if (object.getObjectType() == CObject::FAULT) {
    return (result == FAULT_SHEAR_FAILURE) || (result == FAULT_TOTAL_SLIP) || (result == FAULT_PLASTIC_SLIP);
  } else if (object.getObjectType() == CObject::FORMATION) {
    return (result == FORMATION_SHEAR_FAILURE) || (result == FORMATION_COMPRESSIVE_FAILURE) ||
           (result == FORMATION_TENSILE_FAILURE) || (result == FORMATION_COMPACTION_FAILURE) ||
           (result == FORMATION_EXTENSION_FAILURE) || (result == FORMATION_VOLUMETRIC_COMPACTION) ||
           (result == FORMATION_COMPRESSIVE_PLASTIC_STRAIN) || (result == FORMATION_EXTENSIONAL_PLASTIC_STRAIN);
  } else if (object.getObjectType() == CObject::WELL) {
    return (result == WELL_SHEAR_FAILURE) || (result == WELL_AXIAL_COMPRESSION_EXTENSION);
  } else if (object.getObjectType() == CObject::HORIZON) {
    return (result == TOP_SURFACE_SUBSIDENCE_UPHEAVE);
  }

  return false;
}

} // namespace cora
