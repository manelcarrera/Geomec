#include "stdafx.h"

#include "ValueTypeFactory_Delegate.h"

TLengthComponent_Delegate::TLengthComponent_Delegate(
  TLengthComponent* lengthComponent)
: CComponentTemp_Delegate <IDS_COMPONENT_DEFAULT, 0> (lengthComponent)
, m_lengthComponent(lengthComponent)
{
}

CSurfaceValueType_Delegate::CSurfaceValueType_Delegate(
  CSurfaceValueType* surfaceValueType)
: CValueTypeTemp_Delegate <IDT_VALUETYPE_SURFACE, IDI_VALUETYPE_SURFACE,
    IDS_VALUENAME_SURFACE, IDS_ET_SURFACE> (surfaceValueType)
, m_surfaceValueType(surfaceValueType)
{
}

bool CSurfaceValueType_Delegate::CanDestroy() const
{
  return m_surfaceValueType->CanDestroy();
}

TPressure_Delegate::TPressure_Delegate(TPressure* pressure)
: CSingleComponentTemp_Delegate <IDT_VALUETYPE_PRESSURE,
    IDI_COMPONENT_PRESSURE_DISTRIBUTED, IDS_ET_PRESSURE,
    IDS_VALUENAME_PRESSURE> (pressure)
, m_pressure(pressure)
{
}

TFractureMatrixPressure_Delegate::TFractureMatrixPressure_Delegate(
  TFractureMatrixPressure* fractureMatrixPressure)
: CSingleComponentTemp_Delegate <IDT_VALUETYPE_FRACTURE_MATRIX_PRESSURE,
    IDI_COMPONENT_FRACTURE_MATRIX_PRESSURE_DISTRIBUTED,
    IDS_ET_FRACTURE_MATRIX_PRESSURE, IDS_VALUENAME_FRACTURE_MATRIX_PRESSURE> (
      fractureMatrixPressure)
, m_fractureMatrixPressure(fractureMatrixPressure)
{
}

TTemperature_Delegate::TTemperature_Delegate(TTemperature* temperature)
: CSingleComponentTemp_Delegate <IDT_VALUETYPE_TEMPERATURE,
    IDI_COMPONENT_TEMPERATURE_DISTRIBUTED, IDS_ET_TEMPERATURE,
    IDS_VALUENAME_TEMPERATURE> (temperature)
, m_temperature(temperature)
{
}

TVolumetricStrain_Delegate::TVolumetricStrain_Delegate(
  TVolumetricStrain* volumetricStrain)
: CSingleComponentTemp_Delegate <IDT_VALUETYPE_VOLUMETRICSTRAIN,
    IDI_COMPONENT_VOLUMETRICSTRAIN_DISTRIBUTED, IDS_ET_VOLUMETRICSTRAIN,
    IDS_VALUENAME_VOLUMETRICSTRAIN> (volumetricStrain)
, m_volumetricStrain(volumetricStrain)
{
}

TNormalStrain_Delegate::TNormalStrain_Delegate(
  TNormalStrain* normalStrain)
: CSingleComponentTemp_Delegate <IDT_VALUETYPE_NRMSTRAIN,
    IDI_COMPONENT_NRMSTRAIN_DISTRIBUTED, IDS_ET_NRMSTRAIN,
    IDS_VALUENAME_NRMSTRAIN> (normalStrain)
, m_normalStrain(normalStrain)
{
}

TLateralStrain_Delegate::TLateralStrain_Delegate(
  TLateralStrain* lateralStrain)
: CSingleComponentTemp_Delegate <IDT_VALUETYPE_LATSTRAIN,
    IDI_COMPONENT_LATSTRAIN_DISTRIBUTED, IDS_ET_LATSTRAIN,
    IDS_VALUENAME_LATSTRAIN> (lateralStrain)
, m_lateralStrain(lateralStrain)
{
}

TCohesion_Delegate::TCohesion_Delegate(TCohesion* cohesion)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_COHESION,
    IDI_VALUETYPE_COHESION, IDS_ET_COHESION, IDS_VALUENAME_COHESION> (cohesion)
, m_cohesion(cohesion)
{
}

TBulkStiffness_Delegate::TBulkStiffness_Delegate(TBulkStiffness* bulkStiffness)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_BULKSTIFFNESS,
    IDI_VALUETYPE_BULKSTIFFNESS, IDS_ET_BULKSTIFFNESS,
    IDS_VALUENAME_BULKSTIFFNESS> (bulkStiffness)
, m_bulkStiffness(bulkStiffness)
{
}

TShearModulus_Delegate::TShearModulus_Delegate(TShearModulus* shearModulus)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_SHEARMODULUS,
    IDI_VALUETYPE_SHEARMODULUS, IDS_ET_SHEARMODULUS,
    IDS_VALUENAME_SHEARMODULUS> (shearModulus)
, m_shearModulus(shearModulus)
{
}

TVelocityP_Delegate::TVelocityP_Delegate(TVelocityP* velocityP)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_VELOCITYP,
    IDI_VALUETYPE_VELOCITYP, IDS_ET_VELOCITYP, IDS_VALUENAME_VELOCITYP> (
      velocityP)
, m_velocityP(velocityP)
{
}

TVelocityS_Delegate::TVelocityS_Delegate(TVelocityS* velocityS)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_VELOCITYS,
    IDI_VALUETYPE_VELOCITYS, IDS_ET_VELOCITYS, IDS_VALUENAME_VELOCITYS> (
      velocityS)
, m_velocityS(velocityS)
{
}

TFrictionAngle_Delegate::TFrictionAngle_Delegate(TFrictionAngle* frictionAngle)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_FRICTION_ANGLE,
    IDI_VALUETYPE_FRICTION_ANGLE, IDS_ET_FRICTION_ANGLE,
    IDS_VALUENAME_FRICTION_ANGLE> (frictionAngle)
, m_frictionAngle(frictionAngle)
{
}

TPoissonsRatio_Delegate::TPoissonsRatio_Delegate(TPoissonsRatio* poissonsRatio)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_POISSONS_RATIO,
    IDI_VALUETYPE_POISSONS_RATIO, IDS_ET_POISSON_RATIO,
    IDS_VALUENAME_POISSONS_RATIO> (poissonsRatio)
, m_poissonsRatio(poissonsRatio)
{
}

TYoungsModulus_Delegate::TYoungsModulus_Delegate(TYoungsModulus* youngsModulus)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_YOUNGS_MODULUS,
    IDI_VALUETYPE_YOUNGS_MODULUS, IDS_ET_YOUNG_MODULUS,
    IDS_VALUENAME_YOUNGS_MODULUS> (youngsModulus)
, m_youngsModulus(youngsModulus)
{
}

TDensity_Delegate::TDensity_Delegate(TDensity* density)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_RHOB, IDI_VALUETYPE_RHOB,
    IDS_ET_RHOB, IDS_VALUENAME_RHOB> (density)
, m_density(density)
{
}

TPorosity_Delegate::TPorosity_Delegate(TPorosity* porosity)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_POROSITY,
    IDI_VALUETYPE_POROSITY, IDS_ET_POROSITY, IDS_VALUENAME_POROSITY> (porosity)
, m_porosity(porosity)
{
}

TInitFriction_Delegate::TInitFriction_Delegate(TInitFriction* initFriction)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_INITFRICTION,
    IDI_VALUETYPE_INITFRICTION, IDS_ET_INITFRICTION,
    IDS_VALUENAME_INITFRICTION> (initFriction)
, m_initFriction(initFriction)
{
}

THardening_Delegate::THardening_Delegate(THardening* hardening)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_HARDENING,
    IDI_VALUETYPE_HARDENING, IDS_ET_HARDENING, IDS_VALUENAME_HARDENING> (
      hardening)
, m_hardening(hardening)
{
}

TPreConsolidation_Delegate::TPreConsolidation_Delegate(
  TPreConsolidation* preConsolidation)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_PRECONSOLIDATION,
    IDI_VALUETYPE_PRECONSOLIDATION, IDS_ET_PRECONSOLIDATION,
    IDS_VALUENAME_PRECONSOLIDATION> (preConsolidation)
, m_preConsolidation(preConsolidation)
{
}

TCapShape_Delegate::TCapShape_Delegate(TCapShape* capShape)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_CAPSHAPE,
    IDI_VALUETYPE_CAPSHAPE, IDS_ET_CAPSHAPE, IDS_VALUENAME_CAPSHAPE> (capShape)
, m_capShape(capShape)
{
}

TDilatation_Delegate::TDilatation_Delegate(TDilatation* dilatation)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_DILATATION,
    IDI_VALUETYPE_DILATATION, IDS_ET_DILATATION, IDS_VALUENAME_DILATATION> (
      dilatation)
, m_dilatation(dilatation)
{
}

TLatRatioMax_Delegate::TLatRatioMax_Delegate(TLatRatioMax* latRatioMax)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_LATRATIO_MAX,
    IDI_VALUETYPE_LATRATIO_MAX, IDS_ET_LATRATIO_MAX,
    IDS_VALUENAME_LATRATIO_MAX> (latRatioMax)
, m_latRatioMax(latRatioMax)
{
}

TLatRatioMin_Delegate::TLatRatioMin_Delegate(TLatRatioMin* latRatioMin)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_LATRATIO_MIN,
    IDI_VALUETYPE_LATRATIO_MIN, IDS_ET_LATRATIO_MIN,
    IDS_VALUENAME_LATRATIO_MIN> (latRatioMin)
, m_latRatioMin(latRatioMin)
{
}

TAzimuth_Delegate::TAzimuth_Delegate(TAzimuth* azimuth)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_AZIMUTH,
    IDI_VALUETYPE_AZIMUTH, IDS_ET_AZIMUTH, IDS_VALUENAME_AZIMUTH> (azimuth)
, m_azimuth(azimuth)
{
}

TCreepA1_Delegate::TCreepA1_Delegate(TCreepA1* creepA1)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_CREEP_A1,
    IDI_VALUETYPE_CREEP_A1, IDS_ET_CREEP_A1, IDS_VALUENAME_CREEP_A1> (creepA1)
, m_creepA1(creepA1)
{
}

TCreepN1_Delegate::TCreepN1_Delegate(TCreepN1* creepN1)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_CREEP_N1,
    IDI_VALUETYPE_CREEP_N1, IDS_ET_CREEP_N1, IDS_VALUENAME_CREEP_N1> (creepN1)
, m_creepN1(creepN1)
{
}

TCreepA2_Delegate::TCreepA2_Delegate(TCreepA2* creepA2)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_CREEP_A2,
    IDI_VALUETYPE_CREEP_A2, IDS_ET_CREEP_A2, IDS_VALUENAME_CREEP_A2> (creepA2)
, m_creepA2(creepA2)
{
}

TCreepN2_Delegate::TCreepN2_Delegate(TCreepN2* creepN2)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_CREEP_N2,
    IDI_VALUETYPE_CREEP_N2, IDS_ET_CREEP_N2, IDS_VALUENAME_CREEP_N2> (creepN2)
, m_creepN2(creepN2)
{
}

TFluidBulkModulus_Delegate::TFluidBulkModulus_Delegate(
  TFluidBulkModulus* fluidBulkModulus)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_FLUID_BULK_MOD,
    IDI_VALUETYPE_FLUID_BULK_MOD, IDS_ET_VALUETYPE_FLUID_SHEAR_MOD,
    IDS_VALUENAME_FLUID_BULK_MOD> (fluidBulkModulus)
, m_fluidBulkModulus(fluidBulkModulus)
{
}

THardCohesion1_Delegate::THardCohesion1_Delegate(THardCohesion1* hardCohesion1)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_HARD_COHESION1,
    IDI_VALUETYPE_HARD_COHESION1, IDS_ET_HARD_COHESION1,
    IDS_VALUENAME_HARD_COHESION1> (hardCohesion1)
, m_hardCohesion1(hardCohesion1)
{
}

THardCohesion2_Delegate::THardCohesion2_Delegate(THardCohesion2* hardCohesion2)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_HARD_COHESION2,
    IDI_VALUETYPE_HARD_COHESION2, IDS_ET_HARD_COHESION2,
    IDS_VALUENAME_HARD_COHESION2> (hardCohesion2)
, m_hardCohesion2(hardCohesion2)
{
}

THardCohesion3_Delegate::THardCohesion3_Delegate(THardCohesion3* hardCohesion3)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_HARD_COHESION3,
    IDI_VALUETYPE_HARD_COHESION3, IDS_ET_HARD_COHESION3,
    IDS_VALUENAME_HARD_COHESION3> (hardCohesion3)
, m_hardCohesion3(hardCohesion3)
{
}

THardFriction1_Delegate::THardFriction1_Delegate(THardFriction1* hardFriction1)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_HARD_FRICTION1,
    IDI_VALUETYPE_HARD_FRICTION1, IDS_ET_HARD_FRICTION1,
    IDS_VALUENAME_HARD_FRICTION1> (hardFriction1)
, m_hardFriction1(hardFriction1)
{
}

THardFriction2_Delegate::THardFriction2_Delegate(THardFriction2* hardFriction2)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_HARD_FRICTION2,
    IDI_VALUETYPE_HARD_FRICTION2, IDS_ET_HARD_FRICTION2,
    IDS_VALUENAME_HARD_FRICTION2> (hardFriction2)
, m_hardFriction2(hardFriction2)
{
}

THardFriction3_Delegate::THardFriction3_Delegate(THardFriction3* hardFriction3)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_HARD_FRICTION3,
    IDI_VALUETYPE_HARD_FRICTION3, IDS_ET_HARD_FRICTION3,
    IDS_VALUENAME_HARD_FRICTION3> (hardFriction3)
, m_hardFriction3(hardFriction3)
{
}

TEquivalentPlasticStrain1_Delegate::TEquivalentPlasticStrain1_Delegate(
  TEquivalentPlasticStrain1* equivalentPlasticStrain1)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_EQUIV_PLAST_STRAIN1,
    IDI_VALUETYPE_EQUIV_PLAST_STRAIN1, IDS_ET_EQUIV_PLAST_STRAIN1,
    IDS_VALUENAME_EQUIV_PLAST_STRAIN1> (equivalentPlasticStrain1)
, m_equivalentPlasticStrain1(equivalentPlasticStrain1)
{
}

TEquivalentPlasticStrain2_Delegate::TEquivalentPlasticStrain2_Delegate(
  TEquivalentPlasticStrain2* equivalentPlasticStrain2)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_EQUIV_PLAST_STRAIN2,
    IDI_VALUETYPE_EQUIV_PLAST_STRAIN2, IDS_ET_EQUIV_PLAST_STRAIN2,
    IDS_VALUENAME_EQUIV_PLAST_STRAIN2> (equivalentPlasticStrain2)
, m_equivalentPlasticStrain2(equivalentPlasticStrain2)
{
}

TEquivalentPlasticStrain3_Delegate::TEquivalentPlasticStrain3_Delegate(
  TEquivalentPlasticStrain3* equivalentPlasticStrain3)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_EQUIV_PLAST_STRAIN3,
    IDI_VALUETYPE_EQUIV_PLAST_STRAIN3, IDS_ET_EQUIV_PLAST_STRAIN3,
    IDS_VALUENAME_EQUIV_PLAST_STRAIN3> (equivalentPlasticStrain3)
, m_equivalentPlasticStrain3(equivalentPlasticStrain3)
{
}

TCreepQR1_Delegate::TCreepQR1_Delegate(TCreepQR1* creepQR1)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_CREEP_QR1,
    IDI_VALUETYPE_CREEP_QR1, IDS_ET_CREEP_QR1, IDS_VALUENAME_CREEP_QR1> (
      creepQR1)
, m_creepQR1(creepQR1)
{
}

TCreepQR2_Delegate::TCreepQR2_Delegate(TCreepQR2* creepQR2)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_CREEP_QR2,
    IDI_VALUETYPE_CREEP_QR2, IDS_ET_CREEP_QR2, IDS_VALUENAME_CREEP_QR2> (
      creepQR2)
, m_creepQR2(creepQR2)
{
}

TTensileStretch_Delegate::TTensileStretch_Delegate(
  TTensileStretch* tensileStretch)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_TENSILE_STRETCH,
    IDI_VALUETYPE_TENSILE_STRETCH, IDS_ET_TENSILE_STRETCH,
    IDS_VALUENAME_TENSILE_STRETCH> (tensileStretch)
, m_tensileStretch(tensileStretch)
{
}

TThermalExpansion_Delegate::TThermalExpansion_Delegate(
  TThermalExpansion* thermalExpansion)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_THERMAL_EXPANSION,
    IDI_VALUETYPE_THERMAL_EXPANSION, IDS_ET_THERMAL_EXPANSION,
    IDS_VALUENAME_THERMAL_EXPANSION> (thermalExpansion)
, m_thermalExpansion(thermalExpansion)
{
}

TRigidParam1_Delegate::TRigidParam1_Delegate(TRigidParam1* rigidParam1)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_RIGI_PARAM1,
    IDI_VALUETYPE_RIGID_1, IDS_RIGI_PARAM1, IDS_RIGI_PARAM1> (rigidParam1)
, m_rigidParam1(rigidParam1)
{
}

TRigidParam2_Delegate::TRigidParam2_Delegate(TRigidParam2* rigidParam2)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_RIGI_PARAM2,
    IDI_VALUETYPE_RIGID_2, IDS_RIGI_PARAM2, IDS_RIGI_PARAM2> (rigidParam2)
, m_rigidParam2(rigidParam2)
{
}

TRigidParam3_Delegate::TRigidParam3_Delegate(TRigidParam3* rigidParam3)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_RIGI_PARAM3,
    IDI_VALUETYPE_RIGID_3, IDS_RIGI_PARAM3, IDS_RIGI_PARAM3> (rigidParam3)
, m_rigidParam3(rigidParam3)
{
}

TRigidParam4_Delegate::TRigidParam4_Delegate(TRigidParam4* rigidParam4)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_RIGI_PARAM4,
    IDI_VALUETYPE_RIGID_4, IDS_RIGI_PARAM4, IDS_RIGI_PARAM4> (rigidParam4)
, m_rigidParam4(rigidParam4)
{
}

TRigidParam5_Delegate::TRigidParam5_Delegate(TRigidParam5* rigidParam5)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_RIGI_PARAM5,
    IDI_VALUETYPE_RIGID_5, IDS_RIGI_PARAM5, IDS_RIGI_PARAM5> (rigidParam5)
, m_rigidParam5(rigidParam5)
{
}

TRigidParam6_Delegate::TRigidParam6_Delegate(TRigidParam6* rigidParam6)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_RIGI_PARAM6,
    IDI_VALUETYPE_RIGID_6, IDS_RIGI_PARAM6, IDS_RIGI_PARAM6> (rigidParam6)
, m_rigidParam6(rigidParam6)
{
}

TRigidParam7_Delegate::TRigidParam7_Delegate(TRigidParam7* rigidParam7)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_RIGI_PARAM7,
    IDI_VALUETYPE_RIGID_7, IDS_RIGI_PARAM7, IDS_RIGI_PARAM7> (rigidParam7)
, m_rigidParam7(rigidParam7)
{
}

TRigidParam8_Delegate::TRigidParam8_Delegate(TRigidParam8* rigidParam8)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_RIGI_PARAM8,
    IDI_VALUETYPE_RIGID_8, IDS_RIGI_PARAM8, IDS_RIGI_PARAM8> (rigidParam8)
, m_rigidParam8(rigidParam8)
{
}

TRigidParam9_Delegate::TRigidParam9_Delegate(TRigidParam9* rigidParam9)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_RIGI_PARAM9,
    IDI_VALUETYPE_RIGID_9, IDS_RIGI_PARAM9, IDS_RIGI_PARAM9> (rigidParam9)
, m_rigidParam9(rigidParam9)
{
}

TRigidParam10_Delegate::TRigidParam10_Delegate(TRigidParam10* rigidParam10)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_RIGI_PARAM10,
    IDI_VALUETYPE_RIGID_10, IDS_RIGI_PARAM10, IDS_RIGI_PARAM10> (rigidParam10)
, m_rigidParam10(rigidParam10)
{
}

TRigidParam11_Delegate::TRigidParam11_Delegate(TRigidParam11* rigidParam11)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_RIGI_PARAM11,
    IDI_VALUETYPE_RIGID_11, IDS_RIGI_PARAM11, IDS_RIGI_PARAM11> (rigidParam11)
, m_rigidParam11(rigidParam11)
{
}

TRigidParam12_Delegate::TRigidParam12_Delegate(TRigidParam12* rigidParam12)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_RIGI_PARAM12,
    IDI_VALUETYPE_RIGID_12, IDS_RIGI_PARAM12, IDS_RIGI_PARAM12> (rigidParam12)
, m_rigidParam12(rigidParam12)
{
}

TRigidParam13_Delegate::TRigidParam13_Delegate(TRigidParam13* rigidParam13)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_RIGI_PARAM13,
    IDI_VALUETYPE_RIGID_13, IDS_RIGI_PARAM13, IDS_RIGI_PARAM13> (rigidParam13)
, m_rigidParam13(rigidParam13)
{
}

TRigidParam14_Delegate::TRigidParam14_Delegate(TRigidParam14* rigidParam14)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_RIGI_PARAM14,
    IDI_VALUETYPE_RIGID_14, IDS_RIGI_PARAM14, IDS_RIGI_PARAM14> (rigidParam14)
, m_rigidParam14(rigidParam14)
{
}

TRigidParam15_Delegate::TRigidParam15_Delegate(TRigidParam15* rigidParam15)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_RIGI_PARAM15,
    IDI_VALUETYPE_RIGID_15, IDS_RIGI_PARAM15, IDS_RIGI_PARAM15> (rigidParam15)
, m_rigidParam15(rigidParam15)
{
}

TRigidParam16_Delegate::TRigidParam16_Delegate(TRigidParam16* rigidParam16)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_RIGI_PARAM16,
    IDI_VALUETYPE_RIGID_16, IDS_RIGI_PARAM16, IDS_RIGI_PARAM16> (rigidParam16)
, m_rigidParam16(rigidParam16)
{
}

TRigidParam17_Delegate::TRigidParam17_Delegate(TRigidParam17* rigidParam17)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_RIGI_PARAM17,
    IDI_VALUETYPE_RIGID_17, IDS_RIGI_PARAM17, IDS_RIGI_PARAM17> (rigidParam17)
, m_rigidParam17(rigidParam17)
{
}

TRigidParam18_Delegate::TRigidParam18_Delegate(TRigidParam18* rigidParam18)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_RIGI_PARAM18,
    IDI_VALUETYPE_RIGID_18, IDS_RIGI_PARAM18, IDS_RIGI_PARAM18> (rigidParam18)
, m_rigidParam18(rigidParam18)
{
}

TRigidParam19_Delegate::TRigidParam19_Delegate(TRigidParam19* rigidParam19)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_RIGI_PARAM19,
    IDI_VALUETYPE_RIGID_19, IDS_RIGI_PARAM19, IDS_RIGI_PARAM19> (rigidParam19)
, m_rigidParam19(rigidParam19)
{
}

TRigidParam20_Delegate::TRigidParam20_Delegate(TRigidParam20* rigidParam20)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_RIGI_PARAM20,
    IDI_VALUETYPE_RIGID_20, IDS_RIGI_PARAM20, IDS_RIGI_PARAM20> (rigidParam20)
, m_rigidParam20(rigidParam20)
{
}

TRigidParam21_Delegate::TRigidParam21_Delegate(TRigidParam21* rigidParam21)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_RIGI_PARAM21,
    IDI_VALUETYPE_RIGID_21, IDS_RIGI_PARAM21, IDS_RIGI_PARAM21> (rigidParam21)
, m_rigidParam21(rigidParam21)
{
}

TElasticHardening_Delegate::TElasticHardening_Delegate(
  TElasticHardening* elasticHardening)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_ELASTIC_HARDENING,
    IDI_VALUETYPE_ELASTIC_HARDENING, IDS_ET_ELASTIC_HARDENING,
    IDS_VALUENAME_ELASTIC_HARDENING> (elasticHardening)
, m_elasticHardening(elasticHardening)
{
}

TSecondaryPreconsolidation_Delegate::TSecondaryPreconsolidation_Delegate(
  TSecondaryPreconsolidation* secondaryPreconsolidation)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_SEC_PRECON,
    IDI_VALUETYPE_SEC_PRECON, IDS_ET_SEC_PRECON, IDS_VALUENAME_SEC_PRECON> (
      secondaryPreconsolidation)
, m_secondaryPreconsolidation(secondaryPreconsolidation)
{
}

TSecondaryHardening_Delegate::TSecondaryHardening_Delegate(
  TSecondaryHardening* secondaryHardening)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_SEC_HARDENING,
    IDI_VALUETYPE_SEC_HARDENING, IDS_ET_SEC_HARDENING,
    IDS_VALUENAME_SEC_HARDENING> (secondaryHardening)
, m_secondaryHardening(secondaryHardening)
{
}

TUniaxialElasticCompressibility_Delegate::
  TUniaxialElasticCompressibility_Delegate(TUniaxialElasticCompressibility*
    uniaxialElasticCompressibility)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_ELASCOMPRES,
    IDI_VALUETYPE_ELASCOMPRES, IDS_ET_ELASCOMPRES, IDS_VALUENAME_ELASCOMPRES> (
      uniaxialElasticCompressibility)
, m_uniaxialElasticCompressibility(uniaxialElasticCompressibility)
{
}

TUniaxialPlasticCompressibility_Delegate::
  TUniaxialPlasticCompressibility_Delegate(TUniaxialPlasticCompressibility*
    uniaxialPlasticCompressibility)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_PLASCOMPRES,
    IDI_VALUETYPE_PLASCOMPRES, IDS_ET_PLASCOMPRES, IDS_VALUENAME_PLASCOMPRES> (
      uniaxialPlasticCompressibility)
, m_uniaxialPlasticCompressibility(uniaxialPlasticCompressibility)
{
}

TYoungModulusNormal_Delegate::TYoungModulusNormal_Delegate(
  TYoungModulusNormal* youngModulusNormal)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_YOUNGMODULUS_NORM,
    IDI_VALUETYPE_YOUNGMODULUS_NORM, IDS_ET_YOUNGMODULUS_NORM,
    IDS_VALUENAME_YOUNGMODULUS_NORM> (youngModulusNormal)
, m_youngModulusNormal(youngModulusNormal)
{
}

TYoungModulusTransverse_Delegate::TYoungModulusTransverse_Delegate(
  TYoungModulusTransverse* youngModulusTransverse)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_YOUNGMODULUS_TRANS,
    IDI_VALUETYPE_YOUNGMODULUS_TRANS, IDS_ET_YOUNGMODULUS_TRANS,
    IDS_VALUENAME_YOUNGMODULUS_TRANS> (youngModulusTransverse)
, m_youngModulusTransverse(youngModulusTransverse)
{
}

TPoissonRatioNormal_Delegate::TPoissonRatioNormal_Delegate(
  TPoissonRatioNormal* poissonRatioNormal)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_POISSONRATIO_NORM,
    IDI_VALUETYPE_POISSONRATIO_NORM, IDS_ET_POISSONRATIO_NORM,
    IDS_VALUENAME_POISSONRATIO_NORM> (poissonRatioNormal)
, m_poissonRatioNormal(poissonRatioNormal)
{
}

TPoissonRatioTransverse_Delegate::TPoissonRatioTransverse_Delegate(
  TPoissonRatioTransverse* poissonRatioTransverse)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_POISSONRATIO_TRANS,
    IDI_VALUETYPE_POISSONRATIO_TRANS, IDS_ET_POISSONRATIO_TRANS,
    IDS_VALUENAME_POISSONRATIO_TRANS> (poissonRatioTransverse)
, m_poissonRatioTransverse(poissonRatioTransverse)
{
}

TLayerNormalIncl_Delegate::TLayerNormalIncl_Delegate(
  TLayerNormalIncl* layerNormalIncl)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_LAYER_NORMAL_INCL,
    IDI_VALUETYPE_LAYER_NORMAL_INCL, IDS_ET_LAYER_NORMAL_INCL,
    IDS_VALUENAME_LAYER_NORMAL_INCL> (layerNormalIncl)
, m_layerNormalIncl(layerNormalIncl)
{
}

TLayerNormalAzi_Delegate::TLayerNormalAzi_Delegate(
  TLayerNormalAzi* layerNormalAzi)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_LAYER_NORMAL_AZI,
    IDI_VALUETYPE_LAYER_NORMAL_AZI, IDS_ET_LAYER_NORMAL_AZI,
    IDS_VALUENAME_LAYER_NORMAL_AZI> (layerNormalAzi)
, m_layerNormalAzi(layerNormalAzi)
{
}

TFractStiffNormal_Delegate::TFractStiffNormal_Delegate(
  TFractStiffNormal* fractStiffNormal)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_FRACT_STIFF_NORMAL,
    IDI_VALUETYPE_FRACT_STIFF_NORMAL, IDS_ET_FRACT_STIFF_NORMAL,
    IDS_VALUENAME_FRACT_STIFF_NORMAL> (fractStiffNormal)
, m_fractStiffNormal(fractStiffNormal)
{
}

TFractStiffShear_Delegate::TFractStiffShear_Delegate(
  TFractStiffShear* fractStiffShear)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_FRACT_STIFF_SHEAR,
    IDI_VALUETYPE_FRACT_STIFF_SHEAR, IDS_ET_FRACT_STIFF_SHEAR,
    IDS_VALUENAME_FRACT_STIFF_SHEAR> (fractStiffShear)
, m_fractStiffShear(fractStiffShear)
{
}

TThermLinExpNormal_Delegate::TThermLinExpNormal_Delegate(
  TThermLinExpNormal* thermLinExpNormal)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_THERM_LIN_EXP_NORM,
    IDI_VALUETYPE_THERM_LIN_EXP_NORM, IDS_ET_THERM_LIN_EXP_NORM,
    IDS_VALUENAME_THERM_LIN_EXP_NORM> (thermLinExpNormal)
, m_thermLinExpNormal(thermLinExpNormal)
{
}

TThermLinExpLateral_Delegate::TThermLinExpLateral_Delegate(
  TThermLinExpLateral* thermLinExpLateral)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_THERM_LIN_EXP_LAT,
    IDI_VALUETYPE_THERM_LIN_EXP_LAT, IDS_ET_THERM_LIN_EXP_LAT,
    IDS_VALUENAME_THERM_LIN_EXP_LAT> (thermLinExpLateral)
, m_thermLinExpLateral(thermLinExpLateral)
{
}

TThermalConductivity_Delegate::TThermalConductivity_Delegate(
  TThermalConductivity* thermalConductivity)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_THERM_CONDUCT,
    IDI_VALUETYPE_THERM_CONDUCT, IDS_ET_THERM_CONDUCT,
    IDS_VALUENAME_THERM_CONDUCT> (thermalConductivity)
, m_thermalConductivity(thermalConductivity)
{
}

TThermalCapacity_Delegate::TThermalCapacity_Delegate(
  TThermalCapacity* thermalCapacity)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_THERM_CAPACI,
    IDI_VALUETYPE_THERM_CAPACI, IDS_ET_THERM_CAPACI,
    IDS_VALUENAME_THERM_CAPACI> (thermalCapacity)
, m_thermalCapacity(thermalCapacity)
{
}

THighFractDens_Delegate::THighFractDens_Delegate(THighFractDens* highFractDens)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_HIGH_FRACT_DENS,
    IDI_VALUETYPE_HIGH_FRACT_DENS, IDS_ET_HIGH_FRACT_DENS,
    IDS_VALUENAME_HIGH_FRACT_DENS> (highFractDens)
, m_highFractDens(highFractDens)
{
}

THighFractDensIncl_Delegate::THighFractDensIncl_Delegate(
  THighFractDensIncl* highFractDensIncl)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_HIGH_FRACT_DENS_INCL,
    IDI_VALUETYPE_HIGH_FRACT_DENS_INCL, IDS_ET_HIGH_FRACT_DENS_INCL,
    IDS_VALUENAME_HIGH_FRACT_DENS_INCL> (highFractDensIncl)
, m_highFractDensIncl(highFractDensIncl)
{
}

THighFractDensAzi_Delegate::THighFractDensAzi_Delegate(
  THighFractDensAzi* highFractDensAzi)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_HIGH_FRACT_DENS_AZI,
    IDI_VALUETYPE_HIGH_FRACT_DENS_AZI, IDS_ET_HIGH_FRACT_DENS_AZI,
    IDS_VALUENAME_HIGH_FRACT_DENS_AZI> (highFractDensAzi)
, m_highFractDensAzi(highFractDensAzi)
{
}

TLowFractDens_Delegate::TLowFractDens_Delegate(TLowFractDens* lowFractDens)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_LOW_FRACT_DENS,
    IDI_VALUETYPE_LOW_FRACT_DENS, IDS_ET_LOW_FRACT_DENS,
    IDS_VALUENAME_LOW_FRACT_DENS> (lowFractDens)
, m_lowFractDens(lowFractDens)
{
}

TLowFractDensIncl_Delegate::TLowFractDensIncl_Delegate(
  TLowFractDensIncl* lowFractDensIncl)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_LOW_FRACT_DENS_INCL,
    IDI_VALUETYPE_LOW_FRACT_DENS_INCL, IDS_ET_LOW_FRACT_DENS_INCL,
    IDS_VALUENAME_LOW_FRACT_DENS_INCL> (lowFractDensIncl)
, m_lowFractDensIncl(lowFractDensIncl)
{
}

TLowFractDensAzi_Delegate::TLowFractDensAzi_Delegate(
  TLowFractDensAzi* lowFractDensAzi)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_LOW_FRACT_DENS_AZI,
    IDI_VALUETYPE_LOW_FRACT_DENS_AZI, IDS_ET_LOW_FRACT_DENS_AZI,
    IDS_VALUENAME_LOW_FRACT_DENS_AZI> (lowFractDensAzi)
, m_lowFractDensAzi(lowFractDensAzi)
{
}

TInterFractDens_Delegate::TInterFractDens_Delegate(
  TInterFractDens* interFractDens)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_INTER_FRACT_DENS,
    IDI_VALUETYPE_INT_FRACT_DENS, IDS_ET_INTER_FRACT_DENS,
    IDS_VALUENAME_INTER_FRACT_DENS> (interFractDens)
, m_interFractDens(interFractDens)
{
}

TPermeability_Delegate::TPermeability_Delegate(TPermeability* permeability)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_PERMEA, IDI_VALUETYPE_PERMEA,
    IDS_ET_PERMEA, IDS_VALUENAME_PERMEA> (permeability)
, m_permeability(permeability)
{
}

TViscosity_Delegate::TViscosity_Delegate(TViscosity* viscosity)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_VISCOSITY,
    IDI_VALUETYPE_VISCOSITY, IDS_ET_VISCOSITY, IDS_VALUENAME_VISCOSITY> (
      viscosity)
, m_viscosity(viscosity)
{
}

TYoungsModulusDecompaction_Delegate::TYoungsModulusDecompaction_Delegate(
  TYoungsModulusDecompaction* youngsModulusDecompaction)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_YOUNG_DECOMP,
    IDI_VALUETYPE_YOUNG_DECOMP, IDS_ET_YOUNG_DECOMP,
    IDS_VALUENAME_YOUNG_DECOMP> (youngsModulusDecompaction)
, m_youngsModulusDecompaction(youngsModulusDecompaction)
{
}

TPoissonsRatioDecompaction_Delegate::TPoissonsRatioDecompaction_Delegate(
  TPoissonsRatioDecompaction* poissonsRatioDecompaction)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_POISSON_DECOMP,
    IDI_VALUETYPE_POISSON_DECOMP, IDS_ET_POISSON_DECOMP,
    IDS_VALUENAME_POISSON_DECOMP> (poissonsRatioDecompaction)
, m_poissonsRatioDecompaction(poissonsRatioDecompaction)
{
}

TFluidThermalExpansionCoefficient_Delegate::
  TFluidThermalExpansionCoefficient_Delegate(TFluidThermalExpansionCoefficient*
    fluidThermalExpansionCoefficient)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_FLUIDX,
    IDI_VALUETYPE_CAPSHAPE, IDS_ET_VALUETYPE_FLUIDX, IDS_VALUENAME_FLUIDX> (
      fluidThermalExpansionCoefficient)
, m_fluidThermalExpansionCoefficient(fluidThermalExpansionCoefficient)
{
}

TFluidDensity_Delegate::TFluidDensity_Delegate(TFluidDensity* fluidDensity)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_FLUID_DENSITY,
    IDI_VALUETYPE_FLUID_DENSITY, IDS_ET_VALUETYPE_FLUID_DENSITY,
    IDS_VALUENAME_FLUID_DENSITY> (fluidDensity)
, m_fluidDensity(fluidDensity)
{
}

TReferenceStress_Delegate::TReferenceStress_Delegate(
  TReferenceStress* referenceStress)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_REFERENCESTRESS,
    IDI_VALUETYPE_REFERENCESTRESS, IDS_ET_VALUETYPE_REFERENCESTRESS,
    IDS_VALUENAME_REFERENCESTRESS> (referenceStress)
, m_referenceStress(referenceStress)
{
}

TApertureParameter_Delegate::TApertureParameter_Delegate(
  TApertureParameter* apertureParameter)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_APERTUREPARAM,
    IDI_VALUETYPE_APERTUREPARAM, IDS_ET_VALUETYPE_APERTUREPARAM,
    IDS_VALUENAME_APERTUREPARAM> (apertureParameter)
, m_apertureParameter(apertureParameter)
{
}

TGrainStiffnessParameter_Delegate::TGrainStiffnessParameter_Delegate(
  TGrainStiffnessParameter* grainStiffnessParameter)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_GRAINSTIFFNESS,
    IDI_VALUETYPE_GRAINSTIFFNESS, IDS_ET_VALUETYPE_GRAINSTIFFNESS,
    IDS_VALUENAME_GRAINSTIFFNESS> (grainStiffnessParameter)
, m_grainStiffnessParameter(grainStiffnessParameter)
{
}

TYoungsModulusNormalDecomp_Delegate::TYoungsModulusNormalDecomp_Delegate(
  TYoungsModulusNormalDecomp* youngsModulusNormalDecomp)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_YOUNGMODULUS_NORM_DECOMP,
    IDI_VALUETYPE_YOUNGMODULUS_NORM_DECOMP,
    IDS_ET_VALUETYPE_YOUNGMODULUS_NORM_DECOMP,
    IDS_VALUENAME_YOUNGMODULUS_NORM_DECOMP> (youngsModulusNormalDecomp)
, m_youngsModulusNormalDecomp(youngsModulusNormalDecomp)
{
}

TYoungModulusTransDecomp_Delegate::TYoungModulusTransDecomp_Delegate(
  TYoungModulusTransDecomp* youngModulusTransDecomp)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP,
    IDI_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP,
    IDS_ET_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP,
    IDS_VALUENAME_YOUNGMODULUS_TRANS_DECOMP> (youngModulusTransDecomp)
, m_youngModulusTransDecomp(youngModulusTransDecomp)
{
}

TPoissonRatioNormalDecomp_Delegate::TPoissonRatioNormalDecomp_Delegate(
  TPoissonRatioNormalDecomp* poissonRatioNormalDecomp)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_POISSONRATIO_NORM_DECOMP,
    IDI_VALUETYPE_POISSONRATIO_NORM_DECOMP,
    IDS_ET_VALUETYPE_POISSONRATIO_NORM_DECOMP,
    IDS_VALUENAME_POISSONRATIO_NORM_DECOMP> (poissonRatioNormalDecomp)
, m_poissonRatioNormalDecomp(poissonRatioNormalDecomp)
{
}

TPoissonRatioTransDecomp_Delegate::TPoissonRatioTransDecomp_Delegate(
  TPoissonRatioTransDecomp* poissonRatioTransDecomp)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_POISSONRATIO_TRANS_DECOMP,
    IDI_VALUETYPE_POISSONRATIO_TRANS_DECOMP,
    IDS_ET_VALUETYPE_POISSONRATIO_TRANS_DECOMP,
    IDS_VALUENAME_POISSONRATIO_TRANS_DECOMP> (poissonRatioTransDecomp)
, m_poissonRatioTransDecomp(poissonRatioTransDecomp)
{
}

TAnisotropicShearModulus_Delegate::TAnisotropicShearModulus_Delegate(
  TAnisotropicShearModulus* anisotropicShearModulus)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS,
    IDI_VALUETYPE_ANISOTROPIC_SHEARMODULUS,
    IDS_ET_VALUETYPE_ANISOTROPIC_SHEARMODULUS,
    IDS_VALUENAME_ANISOTROPIC_SHEARMODULUS> (anisotropicShearModulus)
, m_anisotropicShearModulus(anisotropicShearModulus)
{
}

TAnisotropicShearModulusDecomp_Delegate::
  TAnisotropicShearModulusDecomp_Delegate(
    TAnisotropicShearModulusDecomp* anisotropicShearModulusDecomp)
: CMaterialComponentTemp_Delegate <
    IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS_DECOMP,
    IDI_VALUETYPE_ANISOTROPIC_SHEARMODULUS_DECOMP,
    IDS_ET_VALUETYPE_ANISOTROPIC_SHEARMODULUS_DECOMP,
    IDS_VALUENAME_ANISOTROPIC_SHEARMODULUS_DECOMP> (
      anisotropicShearModulusDecomp)
, m_anisotropicShearModulusDecomp(anisotropicShearModulusDecomp)
{
}

TThomsenEpsilon_Delegate::
  TThomsenEpsilon_Delegate(
    TThomsenEpsilon* thomsenEpsilon)
: CMaterialComponentTemp_Delegate <
    IDT_VALUETYPE_THOMSEN_EPSILON,
    IDI_VALUETYPE_THOMSEN_EPSILON,
    IDS_ET_VALUETYPE_THOMSEN_EPSILON,
    IDS_VALUENAME_THOMSEN_EPSILON> (
      thomsenEpsilon),
  m_thomsenEpsilon(thomsenEpsilon)
{
}

TThomsenGamma_Delegate::
  TThomsenGamma_Delegate(
    TThomsenGamma* thomsenGamma)
: CMaterialComponentTemp_Delegate <
    IDT_VALUETYPE_THOMSEN_GAMMA,
    IDI_VALUETYPE_THOMSEN_GAMMA,
    IDS_ET_VALUETYPE_THOMSEN_GAMMA,
    IDS_VALUENAME_THOMSEN_GAMMA> (
      thomsenGamma),
  m_thomsenGamma(thomsenGamma)
{
}

TThomsenDelta_Delegate::
  TThomsenDelta_Delegate(
    TThomsenDelta* thomsenDelta)
: CMaterialComponentTemp_Delegate <
    IDT_VALUETYPE_THOMSEN_DELTA,
    IDI_VALUETYPE_THOMSEN_DELTA,
    IDS_ET_VALUETYPE_THOMSEN_DELTA,
    IDS_VALUENAME_THOMSEN_DELTA> (
      thomsenDelta),
  m_thomsenDelta(thomsenDelta)
{
}

TInitialFractureApertureHigh_Delegate::TInitialFractureApertureHigh_Delegate(
  TInitialFractureApertureHigh* initialFractureApertureHigh)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_V0_HI, IDI_VALUETYPE_V0_HI,
    IDS_ET_VALUETYPE_V0_HI, IDS_VALUENAME_V0_HI> (initialFractureApertureHigh)
, m_initialFractureApertureHigh(initialFractureApertureHigh)
{
}

TInitialFractureApertureInter_Delegate::TInitialFractureApertureInter_Delegate(
  TInitialFractureApertureInter* initialFractureApertureInter)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_V0_ME, IDI_VALUETYPE_V0_ME,
    IDS_ET_VALUETYPE_V0_ME, IDS_VALUENAME_V0_ME> (initialFractureApertureInter)
, m_initialFractureApertureInter(initialFractureApertureInter)
{
}

TInitialFractureApertureLow_Delegate::TInitialFractureApertureLow_Delegate(
  TInitialFractureApertureLow* initialFractureApertureLow)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_V0_LO, IDI_VALUETYPE_V0_LO,
    IDS_ET_VALUETYPE_V0_LO, IDS_VALUENAME_V0_LO> (initialFractureApertureLow)
, m_initialFractureApertureLow(initialFractureApertureLow)
{
}

TReferenceAperture_Delegate::TReferenceAperture_Delegate(
  TReferenceAperture* referenceAperture)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_REFERENCE_APERTURE,
    IDI_VALUETYPE_REFERENCE_APERTURE, IDS_ET_VALUETYPE_REFERENCE_APERTURE,
    IDS_VALUENAME_REFERENCE_APERTURE> (referenceAperture)
, m_referenceAperture(referenceAperture)
{
}

TDynamicUniaxialStiffness_Delegate::TDynamicUniaxialStiffness_Delegate(
  TDynamicUniaxialStiffness* dynamicUniaxialStiffness)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_DYNUNISTIFFNESS,
    IDI_VALUETYPE_DYNUNISTIFFNESS, IDS_ET_DYNUNISTIFFNESS,
    IDS_VALUENAME_DYNUNISTIFFNESS> (dynamicUniaxialStiffness)
, m_dynamicUniaxialStiffness(dynamicUniaxialStiffness)
{
}

TDynamicShearStiffness_Delegate::TDynamicShearStiffness_Delegate(
  TDynamicShearStiffness* dynamicShearStiffness)
: CMaterialComponentTemp_Delegate <IDT_VALUETYPE_DYNSHEARSTIFFNESS,
    IDI_VALUETYPE_DYNSHEARSTIFFNESS, IDS_ET_DYNSHEARSTIFFNESS,
    IDS_VALUENAME_DYNSHEARSTIFFNESS> (dynamicShearStiffness)
, m_dynamicShearStiffness(dynamicShearStiffness)
{
}

TNormalStress_Delegate::TNormalStress_Delegate(TNormalStress* dummy)
  : CSingleComponentTemp_Delegate <IDT_VALUETYPE_NRMSTRESS, IDI_VALUETYPE_NRMSTRESS,
  IDS_ET_NRMSTRESS, IDS_VALUENAME_NRMSTRESS>(dummy)
  , m_dummy(dummy)
{
}

TMeanStress_Delegate::TMeanStress_Delegate(TMeanStress* dummy)
  : CSingleComponentTemp_Delegate <IDT_VALUETYPE_MEANSTRESS, IDI_VALUETYPE_MEANSTRESS,
  IDS_ET_MEANSTRESS, IDS_VALUENAME_MEANSTRESS>(dummy)
  , m_dummy(dummy)
{
}

TTimeStrain_Delegate::TTimeStrain_Delegate(TTimeStrain* dummy)
  : CSingleComponentTemp_Delegate <IDT_VALUETYPE_TIMESTRAIN, IDI_VALUETYPE_TIMESTRAIN,
  IDS_ET_TIMESTRAIN, IDS_VALUENAME_TIMESTRAIN>(dummy)
  , m_dummy(dummy)
{
}

TDeltaV_Delegate::TDeltaV_Delegate(TDeltaV* dummy)
  : CSingleComponentTemp_Delegate <IDT_VALUETYPE_DELTAV, IDI_VALUETYPE_DELTAV,
  IDS_ET_DELTAV, IDS_VALUENAME_DELTAV>(dummy)
  , m_dummy(dummy)
{
}

TDeltaT_Delegate::TDeltaT_Delegate(TDeltaT* dummy)
  : CSingleComponentTemp_Delegate <IDT_VALUETYPE_DELTAT, IDI_VALUETYPE_DELTAT,
  IDS_ET_DELTAT, IDS_VALUENAME_DELTAT>(dummy)
  , m_dummy(dummy)
{
}

TDisplacementZ_Delegate::TDisplacementZ_Delegate(TDisplacementZ* dummy)
  : CSingleComponentTemp_Delegate <IDT_VALUETYPE_DISPLACEMENTZ, IDI_VALUETYPE_DISPLACEMENTZ,
  IDS_ET_DISPLACEMENTZ, IDS_VALUENAME_DISPLACEMENTZ>(dummy)
  , m_dummy(dummy)
{
}

TDummyMaterialParameter_Delegate::TDummyMaterialParameter_Delegate(TDummyMaterialParameter* dummy)
  : CMaterialComponentTemp_Delegate <IDT_VALUETYPE_DUMMY, IDI_VALUETYPE_DUMMY,
  IDS_ET_DUMMY, IDS_VALUENAME_DUMMY>(dummy)
  , m_dummy(dummy)
{
}

TBoundaryKradMaterialParameter_Delegate::TBoundaryKradMaterialParameter_Delegate(TBoundaryKradMaterialParameter* dummy)
  : CMaterialComponentTemp_Delegate <IDT_VALUETYPE_KRAD, IDI_VALUETYPE_KRAD,
  IDS_ET_KRAD, IDS_VALUENAME_KRAD>(dummy)
  , m_dummy(dummy)
{
}

TBoundaryKtanMaterialParameter_Delegate::TBoundaryKtanMaterialParameter_Delegate(TBoundaryKtanMaterialParameter* dummy)
  : CMaterialComponentTemp_Delegate <IDT_VALUETYPE_KTAN, IDI_VALUETYPE_KTAN,
  IDS_ET_KTAN, IDS_VALUENAME_KTAN>(dummy)
  , m_dummy(dummy)
{
}