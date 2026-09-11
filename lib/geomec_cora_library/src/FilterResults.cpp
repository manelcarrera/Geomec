#include <cassert>

#include "FilterResults.h"
#include "GeomecStringTable.h"
#include "resourceIDS.h"

namespace cora
{

CFilterResults::CFilterResults()
: m_filter(fillFilter())
{
}

bool CFilterResults::isResultAllowed(const QString& result) const
{
  return isResultPresent(result) ? (*(m_filter.find(result))).second : false;
}

bool CFilterResults::isResultPresent(const QString& result) const
{
  return (m_filter.find(result) != m_filter.end());
}

const CFilterResults::TFilterMap& CFilterResults::getFilter() const
{
  return m_filter;
}

// private

namespace
{

class CFilter
{
  public:
    CFilter(unsigned int f);
    CFilter(const char* f);

    operator QString() const;

  private:
    CFilter(const CFilter& rhs);
    CFilter& operator = (CFilter rhs);

    QString m_filter;
};

CFilter::CFilter(unsigned int f)
: m_filter(getStringTableEntry(f))
{
}

CFilter::CFilter(const char* f)
: m_filter(f)
{
}

CFilter::operator QString() const
{
  return m_filter;
}

} // anonymous namespace

// static

CFilterResults::TFilterMap CFilterResults::fillFilter()
{
  TFilterMap filter;

  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_APERTURE_HI), false));  // ApertureHigh
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_APERTURE_ME), false));  // ApertureInterm
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_APERTURE_LO), false));  // ApertureLow
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_COMPACTION_CAPACITY), true)); // CompactionCap
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CONSOL), false)); // Consolidation
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_DISPLACEMENT) + CFilter(IDS_ET_VECTOR_Y), false));  // DisplacementE
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_DISPLACEMENT) + CFilter(IDS_ET_VECTOR_LENGTH), false)); // DisplacementL
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_DISPLACEMENT) + CFilter(IDS_ET_VECTOR_X), false));  // DisplacementN
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_DISPLACEMENT) + CFilter(IDS_ET_VECTOR_Z), true)); // DisplacementV
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter(IDS_ET_CONSOL), false));  // Dlt_Consolidation
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE_CHANGE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MAX) + CFilter(IDS_ET_VECTOR_LENGTH), false));  // Dlt_EPStress1L
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE_CHANGE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MED) + CFilter(IDS_ET_VECTOR_LENGTH), false));  // Dlt_EPStress2L
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE_CHANGE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MIN) + CFilter(IDS_ET_VECTOR_LENGTH), false));  // Dlt_EPStress3L
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE_CHANGE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_XX), false));  // Dlt_EStressNN
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE_CHANGE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_YY), false));  // Dlt_EStressEE
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE_CHANGE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_ZZ), false));  // Dlt_EStressVV
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE_CHANGE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_XY), false));  // Dlt_EStressNE
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE_CHANGE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_XZ), false));  // Dlt_EStressNV
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE_CHANGE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_YZ), false));  // Dlt_EStressEV
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE_CHANGE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_INV_PRESSURE), false));  // Dlt_EStressInvPressure
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE_CHANGE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_INV_2), false)); // Dlt_EStressInvI2
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE_CHANGE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_INV_3), false)); // Dlt_EStressInvI3
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE_CHANGE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_INV_VON_MISES), false)); // Dlt_EStressInvVonMises
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE_CHANGE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_INV_TRESCA), false));  // Dlt_EStressInvTresca
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE_CHANGE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_INV_MEAN), false));  // Dlt_EStressInvMean
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE_CHANGE) + CFilter("WP") + CFilter(IDS_ET_STRESS) + CFilter("Axial"), false)); // Dlt_EWPStressAxial
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE_CHANGE) + CFilter("WP") + CFilter(IDS_ET_STRESS) + CFilter("RadialMax"), false)); // Dlt_EWPStressRadialMax
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE_CHANGE) + CFilter("WP") + CFilter(IDS_ET_STRESS) + CFilter("RadialMin"), false)); // Dlt_EWPStressRadialMin
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter(IDS_ET_FAULT_EFF_NORMAL_STRESS), false)); // Dlt_FaultEStressNormal
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter(IDS_ET_FAULT_TOTAL_NORMAL_STRESS), false)); // Dlt_FaultTStressNormal
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter(IDS_ET_FAULT_PRESSURE), false));  // Dlt_FaultPressure
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter(IDS_ET_FAULT_SHEAR_STRESS) + CFilter(IDS_ET_VECTOR_LENGTH), false));  // Dlt_FaultStressShearL
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_FRACTURE_MATRIX_PRESSURE_CHANGE), false));  // Dlt_FractureMatrixPressure
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_POROSITY_RES_CHANGE), false));  // Dlt_PorosityRes
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_PORE_PRESSURE_CHANGE), false)); // Dlt_Pressure
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MAX) + CFilter(IDS_ET_VECTOR_LENGTH), false));  // Dlt_PStress1L
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MED) + CFilter(IDS_ET_VECTOR_LENGTH), false));  // Dlt_PStress2L
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MIN) + CFilter(IDS_ET_VECTOR_LENGTH), false));  // Dlt_PStress3L
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_XX), false));  // Dlt_StressNN
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_YY), false));  // Dlt_StressEE
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_ZZ), false));  // Dlt_StressVV
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_XY), false));  // Dlt_StressNE
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_XZ), false));  // Dlt_StressNV
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_YZ), false));  // Dlt_StressEV
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_INV_PRESSURE), false));  // Dlt_StressInvPressure
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_INV_2), false)); // Dlt_StressInvI2
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_INV_3), false)); // Dlt_StressInvI3
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_INV_VON_MISES), false)); // Dlt_StressInvVonMises
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_INV_TRESCA), false));  // Dlt_StressInvTresca
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_INV_MEAN), false));  // Dlt_StressInvMean
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter("Surface") + CFilter(IDS_ET_STRESS) + CFilter("Normal"), false)); // Dlt_SurfaceStressNormal
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter("Surface") + CFilter(IDS_ET_STRESS) + CFilter("ShearPlane"), false)); // Dlt_SurfaceStressShearPlane
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter("Surface") + CFilter(IDS_ET_STRESS) + CFilter("ShearPlaneDipAngle"), false)); // Dlt_SurfaceStressShearPlaneDipAngle
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_TEMPERATURE_CHANGE), false)); // Dlt_Temperature
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_THICKNESS_CHANGE), false)); // Dlt_Thickness
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_APERTURE_HI_CHANGE), false)); // Dlt_V0High
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_APERTURE_ME_CHANGE), false)); // Dlt_V0Interm
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_APERTURE_LO_CHANGE), false)); // Dlt_V0Low
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter("WP") + CFilter(IDS_ET_STRESS) + CFilter("Axial"), false)); // Dlt_WPStressAxial
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter("WP") + CFilter(IDS_ET_STRESS) + CFilter("RadialMax"), false)); // Dlt_WPStressRadialMax
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter("WP") + CFilter(IDS_ET_STRESS) + CFilter("RadialMin"), false)); // Dlt_WPStressRadialMin
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MAX) + CFilter(IDS_ET_VECTOR_Y), false));  // EPStress1E
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MAX) + CFilter(IDS_ET_VECTOR_LENGTH), true));  // EPStress1L
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MAX) + CFilter(IDS_ET_VECTOR_X), false));  // EPStress1N
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MAX) + CFilter(IDS_ET_VECTOR_Z), false));  // EPStress1V
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MED) + CFilter(IDS_ET_VECTOR_Y), false));  // EPStress2E
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MED) + CFilter(IDS_ET_VECTOR_LENGTH), false)); // EPStress2L
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MED) + CFilter(IDS_ET_VECTOR_X), false));  // EPStress2N
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MED) + CFilter(IDS_ET_VECTOR_Z), false));  // EPStress2V
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MIN) + CFilter(IDS_ET_VECTOR_Y), false));  // EPStress3E
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MIN) + CFilter(IDS_ET_VECTOR_LENGTH), true));  // EPStress3L
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MIN) + CFilter(IDS_ET_VECTOR_X), false));  // EPStress3N
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MIN) + CFilter(IDS_ET_VECTOR_Z), false));  // EPStress3V
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_XX), false)); // EStressNN
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_YY), false)); // EStressEE
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_ZZ), false)); // EStressVV
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_XY), false)); // EStressNE
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_XZ), false)); // EStressNV
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_YZ), false)); // EStressEV
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_INV_PRESSURE), false)); // EStressInvPressure
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_INV_2), false));  // EStressInvI2
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_INV_3), false));  // EStressInvI3
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_INV_VON_MISES), false));  // EStressInvVonMises
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_INV_TRESCA), false)); // EStressInvTresca
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_INV_MEAN), false)); // EStressInvMean
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE) + CFilter("WP") + CFilter(IDS_ET_STRESS) + CFilter("Axial"), false));  // EWPStressAxial
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE) + CFilter("WP") + CFilter(IDS_ET_STRESS) + CFilter("RadialMax"), false));  // EWPStressRadialMax
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE) + CFilter("WP") + CFilter(IDS_ET_STRESS) + CFilter("RadialMin"), false));  // EWPStressRadialMin
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_FAULT_DILATATION), false)); // FaultDilatation
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_FAULT_DISPLACEMENT) + CFilter(IDS_ET_VECTOR_Y), false));  // FaultDisplacementE
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_FAULT_DISPLACEMENT) + CFilter(IDS_ET_VECTOR_LENGTH), true));  // FaultDisplacementL
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_FAULT_DISPLACEMENT) + CFilter(IDS_ET_VECTOR_X), false));  // FaultDisplacementN
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_FAULT_DISPLACEMENT) + CFilter(IDS_ET_VECTOR_Z), false));  // FaultDisplacementV
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_FAULT_EFF_NORMAL_STRESS), false));  // FaultEStressNormal
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_FAULT_TOTAL_NORMAL_STRESS), false));  // FaultTStressNormal
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_FAULT_PLASTIC_SLIP) + CFilter(IDS_ET_VECTOR_Y), false));  // FaultPlasticSlipE
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_FAULT_PLASTIC_SLIP) + CFilter(IDS_ET_VECTOR_LENGTH), false)); // FaultPlasticSlipL
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_FAULT_PLASTIC_SLIP) + CFilter(IDS_ET_VECTOR_X), false));  // FaultPlasticSlipN
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_FAULT_PLASTIC_SLIP) + CFilter(IDS_ET_VECTOR_Z), false));  // FaultPlasticSlipV
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_FAULT_PRESSURE), false)); // FaultPressure
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_FAULT_SHEARCAPACITY), true)); // FaultShearCap
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_FAULT_SHEAR_STRESS) + CFilter(IDS_ET_VECTOR_Y), false));  // FaultStressShearE
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_FAULT_SHEAR_STRESS) + CFilter(IDS_ET_VECTOR_LENGTH), false)); // FaultStressShearL
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_FAULT_SHEAR_STRESS) + CFilter(IDS_ET_VECTOR_X), false));  // FaultStressShearN
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_FAULT_SHEAR_STRESS) + CFilter(IDS_ET_VECTOR_Z), false));  // FaultStressShearV
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_FRACTURE_MATRIX_PRESSURE), false)); // FractureMatrixPressure
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_GAMMA_MAX), false));  // GammaMax
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_GAMMA_MED), false));  // GammaInt
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_GAMMA_MIN), false));  // GammaMin
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_GAMMA_VERTICAL), false)); // GammaV
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_GAMMA_VOLUME), false)); // GammaVol
  filter.insert(TFilterMap::value_type(CFilter(IDS_RG_GAMMA), false));  // Gammas
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_PERMEABILITY_MULTIPLIER_HI), false)); // PermeabilityMultiplierHigh
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_PERMEABILITY_MULTIPLIER_ME), false)); // PermeabilityMultiplierInterm
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_PERMEABILITY_MULTIPLIER_LO), false)); // PermeabilityMultiplierLow
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_POROSITY_RES), false)); // PorosityRes
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_PRESSURE), false)); // Pressure
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_MAX) + CFilter(IDS_ET_VECTOR_Y), false));  // PStrain1E
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_MAX) + CFilter(IDS_ET_VECTOR_LENGTH), true));  // PStrain1L
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_MAX) + CFilter(IDS_ET_VECTOR_X), false));  // PStrain1N
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_MAX) + CFilter(IDS_ET_VECTOR_Z), false));  // PStrain1V
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_MED) + CFilter(IDS_ET_VECTOR_Y), false));  // PStrain2E
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_MED) + CFilter(IDS_ET_VECTOR_LENGTH), false)); // PStrain2L
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_MED) + CFilter(IDS_ET_VECTOR_X), false));  // PStrain2N
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_MED) + CFilter(IDS_ET_VECTOR_Z), false));  // PStrain2V
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_MIN) + CFilter(IDS_ET_VECTOR_Y), false));  // PStrain3E
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_MIN) + CFilter(IDS_ET_VECTOR_LENGTH), true));  // PStrain3L
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_MIN) + CFilter(IDS_ET_VECTOR_X), false));  // PStrain3N
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_MIN) + CFilter(IDS_ET_VECTOR_Z), false));  // PStrain3V
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_MAX) + CFilter(IDS_ET_PLASTIC) + CFilter(IDS_ET_VECTOR_Y), false));  // PStrain1PlasticE
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_MAX) + CFilter(IDS_ET_PLASTIC) + CFilter(IDS_ET_VECTOR_LENGTH), true));  // PStrain1PlasticL
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_MAX) + CFilter(IDS_ET_PLASTIC) + CFilter(IDS_ET_VECTOR_X), false));  // PStrain1PlasticN
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_MAX) + CFilter(IDS_ET_PLASTIC) + CFilter(IDS_ET_VECTOR_Z), false));  // PStrain1PlasticV
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_MED) + CFilter(IDS_ET_PLASTIC) + CFilter(IDS_ET_VECTOR_Y), false));  // PStrain2PlasticE
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_MED) + CFilter(IDS_ET_PLASTIC) + CFilter(IDS_ET_VECTOR_LENGTH), false)); // PStrain2PlasticL
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_MED) + CFilter(IDS_ET_PLASTIC) + CFilter(IDS_ET_VECTOR_X), false));  // PStrain2PlasticN
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_MED) + CFilter(IDS_ET_PLASTIC) + CFilter(IDS_ET_VECTOR_Z), false));  // PStrain2PlasticV
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_MIN) + CFilter(IDS_ET_PLASTIC) + CFilter(IDS_ET_VECTOR_Y), false));  // PStrain3PlasticE
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_MIN) + CFilter(IDS_ET_PLASTIC) + CFilter(IDS_ET_VECTOR_LENGTH), true));  // PStrain3PlasticL
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_MIN) + CFilter(IDS_ET_PLASTIC) + CFilter(IDS_ET_VECTOR_X), false));  // PStrain3PlasticN
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_MIN) + CFilter(IDS_ET_PLASTIC) + CFilter(IDS_ET_VECTOR_Z), false));  // PStrain3PlasticV
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MAX) + CFilter(IDS_ET_VECTOR_Y), false));  // PStress1E
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MAX) + CFilter(IDS_ET_VECTOR_LENGTH), false)); // PStress1L
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MAX) + CFilter(IDS_ET_VECTOR_X), false));  // PStress1N
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MAX) + CFilter(IDS_ET_VECTOR_Z), false));  // PStress1V
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MED) + CFilter(IDS_ET_VECTOR_Y), false));  // PStress2E
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MED) + CFilter(IDS_ET_VECTOR_LENGTH), false)); // PStress2L
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MED) + CFilter(IDS_ET_VECTOR_X), false));  // PStress2N
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MED) + CFilter(IDS_ET_VECTOR_Z), false));  //PStress2V
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MIN) + CFilter(IDS_ET_VECTOR_Y), false));  // PStress3E
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MIN) + CFilter(IDS_ET_VECTOR_LENGTH), false)); // PStress3L
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MIN) + CFilter(IDS_ET_VECTOR_X), false));  // PStress3N
  filter.insert(TFilterMap::value_type(CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MIN) + CFilter(IDS_ET_VECTOR_Z), false));  // PStress3V
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_SHEAR_CAPACITY), true));  // ShearCap
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_SHTOTAZIMUTH), false)); // SHtotAzimuth
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_S_HTOT_SVTOT), false)); // Shtot/Svtot
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_SHTOT_SVTOT), false));  // SHtot/Svtot
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_XX), false)); // StrainNN
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_YY), false)); // StrainEE
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_ZZ), false)); // StrainVV
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_XY), false)); // StrainNE
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_XZ), false)); // StrainNV
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_YZ), false)); // StrainEV
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_INV_2), false));  // StrainInvI2
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_INV_3), false));  // StrainInvI3
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_INV_VOLUME), true));  // StrainInvVolumetric
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_TENSOR_INV_VON_MISES), false));  // StrainInvVonMises
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_PLASTIC) + CFilter(IDS_ET_TENSOR_XX), false)); // StrainPlasticNN
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_PLASTIC) + CFilter(IDS_ET_TENSOR_YY), false)); // StrainPlasticEE
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_PLASTIC) + CFilter(IDS_ET_TENSOR_ZZ), false)); // StrainPlasticVV
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_PLASTIC) + CFilter(IDS_ET_TENSOR_XY), false)); // StrainPlasticNE
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_PLASTIC) + CFilter(IDS_ET_TENSOR_XZ), false)); // StrainPlasticNV
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_PLASTIC) + CFilter(IDS_ET_TENSOR_YZ), false)); // StrainPlasticEV
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_PLASTIC) + CFilter(IDS_ET_TENSOR_INV_2), false));  // StrainPlasticInvI2
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_PLASTIC) + CFilter(IDS_ET_TENSOR_INV_3), false));  // StrainPlasticInvI3
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_PLASTIC) + CFilter(IDS_ET_TENSOR_INV_VOLUME), false)); // StrainPlasticInvVolumetric
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_PLASTIC) + CFilter(IDS_ET_TENSOR_INV_VON_MISES), false));  // StrainPlasticInvVonMises
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_XX), false)); // StressNN
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_YY), false)); // StressEE
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_ZZ), false)); // StressVV
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_XY), false)); // StressNE
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_XZ), false)); // StressNV
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_YZ), false)); // StressEV
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_INV_PRESSURE), false)); // StressInvPressure
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_INV_2), false));  // StressInvI2
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_INV_3), false));  // StressInvI3
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_INV_VON_MISES), false));  // StressInvVonMises
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_INV_TRESCA), false)); // StressInvTresca
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_INV_MEAN), false)); // StressInvMean
  filter.insert(TFilterMap::value_type(CFilter("Surface") + CFilter(IDS_ET_STRESS) + CFilter("Normal"), false));  // SurfaceStressNormal
  filter.insert(TFilterMap::value_type(CFilter("Surface") + CFilter(IDS_ET_STRESS) + CFilter("ShearPlane"), false));  // SurfaceStressShearPlane
  filter.insert(TFilterMap::value_type(CFilter("Surface") + CFilter(IDS_ET_STRESS) + CFilter("ShearPlaneDipAngle"), false));  // SurfaceStressShearPlaneDipAngle
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_TEMPERATURE), false));  // Temperature
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_THICKNESS), false));  // Thickness
  filter.insert(TFilterMap::value_type(CFilter("WP") + CFilter("Axial") + CFilter(IDS_ET_STRAIN), true)); // WPAxialStrain
  filter.insert(TFilterMap::value_type(CFilter("WP") + CFilter("Axial") + CFilter(IDS_ET_STRAIN) + CFilter(IDS_ET_PLASTIC), false));  // WPAxialStrainPlastic
  filter.insert(TFilterMap::value_type(CFilter("WP") + CFilter(IDS_ET_STRESS) + CFilter("Axial"), false));  // WPStressAxial
  filter.insert(TFilterMap::value_type(CFilter("WP") + CFilter(IDS_ET_STRESS) + CFilter("RadialMax"), false));  // WPStressRadialMax
  filter.insert(TFilterMap::value_type(CFilter("WP") + CFilter(IDS_ET_STRESS) + CFilter("RadialMin"), false));  // WPStressRadialMin

  // 'isResultsFilterComplete' revealed that the results below were missing

  //filter.insert(TFilterMap::value_type(CFilter("RTCIStrain"), false));  // RTCIStrain
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_DSTABOR), false));  // Mud

  // added complementary to Changeset 196044

  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_PLASTIC_ENERGY), false)); // PlasticEnergy

  // added complementary to Changeset 196069

  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_FAULT_PLASTIC_ENERGY), false)); // FaultPlasticEnergy

  // added complementary to Changeset 205936

  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_INV_VON_MISES_GRADIENT), false)); // EStressInvVonMisesGradient
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_INV_MEAN_GRADIENT), false));  // EStressInvMeanGradient
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_INV_VON_MISES_GRADIENT), false)); // StressInvVonMisesGradient
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_INV_MEAN_GRADIENT), false));  // StressInvMeanGradient
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_INV_VON_MISES_GRADIENT), false));  // Dlt_StressInvVonMisesGradient
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_INV_MEAN_GRADIENT), false)); // Dlt_StressInvMeanGradient
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter(IDS_ET_EFFECTIVE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_INV_VON_MISES_GRADIENT), false));  // Dlt_EStressInvVonMisesGradient
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter(IDS_ET_EFFECTIVE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_INV_MEAN_GRADIENT), false)); // Dlt_EStressInvMeanGradient
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_PORE_PRESSURE_GRADIENT), false)); // Pore pressure gradient
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_TEMPERATURE_GRADIENT), false)); // Temperature gradient

  /*
  for some reason obsolete (since Changeset ?)

  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE_CHANGE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MAX) + CFilter(IDS_ET_VECTOR_Y), false)); // Dlt_EPStress1E
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE_CHANGE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MAX) + CFilter(IDS_ET_VECTOR_X), false)); // Dlt_EPStress1N
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE_CHANGE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MAX) + CFilter(IDS_ET_VECTOR_Z), false)); // Dlt_EPStress1V
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE_CHANGE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MED) + CFilter(IDS_ET_VECTOR_Y), false)); // Dlt_EPStress2E
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE_CHANGE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MED) + CFilter(IDS_ET_VECTOR_X), false)); // Dlt_EPStress2N
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE_CHANGE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MED) + CFilter(IDS_ET_VECTOR_Z), false)); // Dlt_EPStress2V
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE_CHANGE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MIN) + CFilter(IDS_ET_VECTOR_Y), false)); // Dlt_EPStress3E
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE_CHANGE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MIN) + CFilter(IDS_ET_VECTOR_X), false)); // Dlt_EPStress3N
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE_CHANGE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MIN) + CFilter(IDS_ET_VECTOR_Z), false)); // Dlt_EPStress3V
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter(IDS_ET_FAULT_SHEAR_STRESS) + CFilter(IDS_ET_VECTOR_Y), false)); // Dlt_FaultStressShearE
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter(IDS_ET_FAULT_SHEAR_STRESS) + CFilter(IDS_ET_VECTOR_X), false)); // Dlt_FaultStressShearN
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter(IDS_ET_FAULT_SHEAR_STRESS) + CFilter(IDS_ET_VECTOR_Z), false)); // Dlt_FaultStressShearV
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MAX) + CFilter(IDS_ET_VECTOR_Y), false)); // Dlt_PStress1E
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MAX) + CFilter(IDS_ET_VECTOR_X), false)); // Dlt_PStress1N
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MAX) + CFilter(IDS_ET_VECTOR_Z), false)); // Dlt_PStress1V
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MED) + CFilter(IDS_ET_VECTOR_Y), false)); // Dlt_PStress2E
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MED) + CFilter(IDS_ET_VECTOR_X), false)); // Dlt_PStress2N
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MED) + CFilter(IDS_ET_VECTOR_Z), false)); // Dlt_PStress2V
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MIN) + CFilter(IDS_ET_VECTOR_Y), false)); // Dlt_PStress3E
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MIN) + CFilter(IDS_ET_VECTOR_X), false)); // Dlt_PStress3N
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_CHANGE) + CFilter("P") + CFilter(IDS_ET_STRESS) + CFilter(IDS_ET_TENSOR_MIN) + CFilter(IDS_ET_VECTOR_Z), false)); // Dlt_PStress3V
  */

  // added complementary to Changeset ?

  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_STRESS) + CFilter(IDS_RG_GRADIENT), false));  // StressGradient
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_EFFECTIVE) + CFilter(IDS_ET_STRESS) + CFilter(IDS_RG_GRADIENT), false));  // EStressGradient

  // added complementary to Changeset 239280

  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_VOLUMETRICSTRAIN), false)); // VolumetricStrain
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_VOLUMETRICSTRAIN_CHANGE), false));  // VolumetricStrainChange

  // added complementary to Changeset 261892

  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_NRMSTRAIN), false));  // NormalStrain
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_LATSTRAIN), false));  // LateralStrain

  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_GVT_FACTOR_R_C), false));
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_GVT_FACTOR_R_EX), false));
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_GVT_FACTOR_S_NB), false));
  filter.insert(TFilterMap::value_type(CFilter(IDS_ET_GVT_FACTOR_S_S), false));

  return filter;
}

} // namespace cora
