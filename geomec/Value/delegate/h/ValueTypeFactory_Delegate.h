#ifndef _ValueTypeFactory_Delegate_h_
#define _ValueTypeFactory_Delegate_h_

class TPressure;

#include "ValueComponent_Delegate.h"
#include "ValueStrain.h"
#include "ValueTypeFactory.h"
#include "ValueType_Delegate.h"
#include "ValueTypes.h"
#include "resource.h"

class TLengthComponent_Delegate : public CComponentTemp_Delegate<IDS_COMPONENT_DEFAULT, 0> {
public:
  TLengthComponent_Delegate(TLengthComponent *lengthComponent);

private:
  TLengthComponent_Delegate(const TLengthComponent_Delegate &rhs);
  TLengthComponent_Delegate &operator=(const TLengthComponent_Delegate &rhs);

  TLengthComponent *m_lengthComponent;

  REGISTER_DELEGATE(TLengthComponent, TLengthComponent_Delegate);
};

class CSurfaceValueType_Delegate : public CValueTypeTemp_Delegate<IDT_VALUETYPE_SURFACE, IDI_VALUETYPE_SURFACE,
                                                                  IDS_VALUENAME_SURFACE, IDS_ET_SURFACE> {
public:
  CSurfaceValueType_Delegate(CSurfaceValueType *surfaceValueType);

  virtual bool CanDestroy() const;

private:
  CSurfaceValueType_Delegate(const CSurfaceValueType_Delegate &rhs);
  CSurfaceValueType_Delegate &operator=(const CSurfaceValueType_Delegate &rhs);

  CSurfaceValueType *m_surfaceValueType;

  REGISTER_DELEGATE(CSurfaceValueType, CSurfaceValueType_Delegate);
};

template <unsigned int uTypeId, unsigned int uIconId, unsigned int uExportTagId, unsigned int uValueNameId>
class CSingleComponentTemp_Delegate : public CValueTypeTemp_Delegate<uTypeId, uIconId, uValueNameId, uExportTagId> {
public:
  CSingleComponentTemp_Delegate(
      CSingleComponentTemp<uTypeId, uIconId, uExportTagId, uValueNameId> *singleComponentTemp);

private:
  CSingleComponentTemp_Delegate(const CSingleComponentTemp_Delegate &rhs);
  CSingleComponentTemp_Delegate &operator=(const CSingleComponentTemp_Delegate &rhs);

  CSingleComponentTemp<uTypeId, uIconId, uExportTagId, uValueNameId> *m_singleComponentTemp;

  typedef CSingleComponentTemp<uTypeId, uIconId, uExportTagId, uValueNameId> CSingleComponentTempTemplate;
  typedef CSingleComponentTemp_Delegate<uTypeId, uIconId, uExportTagId, uValueNameId>
      CSingleComponentTemp_DelegateTemplate;

  REGISTER_DELEGATE(CSingleComponentTempTemplate, CSingleComponentTemp_DelegateTemplate);
};

template <unsigned int uTypeId, unsigned int uIconId, unsigned int uExportTagId, unsigned int uValueNameId>
CSingleComponentTemp_Delegate<uTypeId, uIconId, uExportTagId, uValueNameId>::CSingleComponentTemp_Delegate(
    CSingleComponentTemp<uTypeId, uIconId, uExportTagId, uValueNameId> *singleComponentTemp)
    : CValueTypeTemp_Delegate<uTypeId, uIconId, uValueNameId, uExportTagId>(singleComponentTemp),
      m_singleComponentTemp(singleComponentTemp) {
  ACTIVATE_TEMPLATE_DELEGATE(CSingleComponentTempTemplate, CSingleComponentTemp_DelegateTemplate);
}

template <unsigned int uTypeId, unsigned int uIconId, unsigned int uExportTagId, unsigned int uValueNameId>
class CMaterialComponentTemp_Delegate
    : public CSingleComponentTemp_Delegate<uTypeId, uIconId, uExportTagId, uValueNameId> {
public:
  CMaterialComponentTemp_Delegate(
      CMaterialComponentTemp<uTypeId, uIconId, uExportTagId, uValueNameId> *materialComponentTemp);

private:
  CMaterialComponentTemp_Delegate(const CMaterialComponentTemp_Delegate &rhs);
  CMaterialComponentTemp_Delegate &operator=(const CMaterialComponentTemp_Delegate &rhs);

  CMaterialComponentTemp<uTypeId, uIconId, uExportTagId, uValueNameId> *m_materialComponentTemp;

  typedef CMaterialComponentTemp<uTypeId, uIconId, uExportTagId, uValueNameId> CMaterialComponentTempTemplate;
  typedef CMaterialComponentTemp_Delegate<uTypeId, uIconId, uExportTagId, uValueNameId>
      CMaterialComponentTemp_DelegateTemplate;

  REGISTER_DELEGATE(CMaterialComponentTempTemplate, CMaterialComponentTemp_DelegateTemplate);
};

template <unsigned int uTypeId, unsigned int uIconId, unsigned int uExportTagId, unsigned int uValueNameId>
CMaterialComponentTemp_Delegate<uTypeId, uIconId, uExportTagId, uValueNameId>::CMaterialComponentTemp_Delegate(
    CMaterialComponentTemp<uTypeId, uIconId, uExportTagId, uValueNameId> *materialComponentTemp)
    : CSingleComponentTemp_Delegate<uTypeId, uIconId, uExportTagId, uValueNameId>(materialComponentTemp),
      m_materialComponentTemp(materialComponentTemp) {
  ACTIVATE_TEMPLATE_DELEGATE(CMaterialComponentTempTemplate, CMaterialComponentTemp_DelegateTemplate);
}

class TPressure_Delegate
    : public CSingleComponentTemp_Delegate<IDT_VALUETYPE_PRESSURE, IDI_COMPONENT_PRESSURE_DISTRIBUTED, IDS_ET_PRESSURE,
                                           IDS_VALUENAME_PRESSURE> {
public:
  TPressure_Delegate(TPressure *pressure);

private:
  TPressure_Delegate(const TPressure_Delegate &rhs);
  TPressure_Delegate &operator=(const TPressure_Delegate &rhs);

  TPressure *m_pressure;

  REGISTER_DELEGATE(TPressure, TPressure_Delegate);
};

class TFractureMatrixPressure_Delegate
    : public CSingleComponentTemp_Delegate<IDT_VALUETYPE_FRACTURE_MATRIX_PRESSURE,
                                           IDI_COMPONENT_FRACTURE_MATRIX_PRESSURE_DISTRIBUTED,
                                           IDS_ET_FRACTURE_MATRIX_PRESSURE, IDS_VALUENAME_FRACTURE_MATRIX_PRESSURE> {
public:
  TFractureMatrixPressure_Delegate(TFractureMatrixPressure *fractureMatrixPressure);

private:
  TFractureMatrixPressure_Delegate(const TFractureMatrixPressure_Delegate &rhs);
  TFractureMatrixPressure_Delegate &operator=(const TFractureMatrixPressure_Delegate &rhs);

  TFractureMatrixPressure *m_fractureMatrixPressure;

  REGISTER_DELEGATE(TFractureMatrixPressure, TFractureMatrixPressure_Delegate);
};

class TTemperature_Delegate
    : public CSingleComponentTemp_Delegate<IDT_VALUETYPE_TEMPERATURE, IDI_COMPONENT_TEMPERATURE_DISTRIBUTED,
                                           IDS_ET_TEMPERATURE, IDS_VALUENAME_TEMPERATURE> {
public:
  TTemperature_Delegate(TTemperature *temperature);

private:
  TTemperature_Delegate(const TTemperature_Delegate &rhs);
  TTemperature_Delegate &operator=(const TTemperature_Delegate &rhs);

  TTemperature *m_temperature;

  REGISTER_DELEGATE(TTemperature, TTemperature_Delegate);
};

class TVolumetricStrain_Delegate
    : public CSingleComponentTemp_Delegate<IDT_VALUETYPE_VOLUMETRICSTRAIN, IDI_COMPONENT_VOLUMETRICSTRAIN_DISTRIBUTED,
                                           IDS_ET_VOLUMETRICSTRAIN, IDS_VALUENAME_VOLUMETRICSTRAIN> {
public:
  TVolumetricStrain_Delegate(TVolumetricStrain *volumetricStrain);

private:
  TVolumetricStrain_Delegate(const TVolumetricStrain_Delegate &rhs);
  TVolumetricStrain_Delegate &operator=(const TVolumetricStrain_Delegate &rhs);

  TVolumetricStrain *m_volumetricStrain;

  REGISTER_DELEGATE(TVolumetricStrain, TVolumetricStrain_Delegate);
};

class TNormalStrain_Delegate
    : public CSingleComponentTemp_Delegate<IDT_VALUETYPE_NRMSTRAIN, IDI_COMPONENT_NRMSTRAIN_DISTRIBUTED,
                                           IDS_ET_NRMSTRAIN, IDS_VALUENAME_NRMSTRAIN> {
public:
  TNormalStrain_Delegate(TNormalStrain *normalStrain);

private:
  TNormalStrain_Delegate(const TNormalStrain_Delegate &rhs);
  TNormalStrain_Delegate &operator=(const TNormalStrain_Delegate &rhs);

  TNormalStrain *m_normalStrain;

  REGISTER_DELEGATE(TNormalStrain, TNormalStrain_Delegate);
};

class TLateralStrain_Delegate
    : public CSingleComponentTemp_Delegate<IDT_VALUETYPE_LATSTRAIN, IDI_COMPONENT_LATSTRAIN_DISTRIBUTED,
                                           IDS_ET_LATSTRAIN, IDS_VALUENAME_LATSTRAIN> {
public:
  TLateralStrain_Delegate(TLateralStrain *lateralStrain);

private:
  TLateralStrain_Delegate(const TLateralStrain_Delegate &rhs);
  TLateralStrain_Delegate &operator=(const TLateralStrain_Delegate &rhs);

  TLateralStrain *m_lateralStrain;

  REGISTER_DELEGATE(TLateralStrain, TLateralStrain_Delegate);
};

class TCohesion_Delegate : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_COHESION, IDI_VALUETYPE_COHESION,
                                                                  IDS_ET_COHESION, IDS_VALUENAME_COHESION> {
public:
  TCohesion_Delegate(TCohesion *cohesion);

private:
  TCohesion_Delegate(const TCohesion_Delegate &rhs);
  TCohesion_Delegate &operator=(const TCohesion_Delegate &rhs);

  TCohesion *m_cohesion;

  REGISTER_DELEGATE(TCohesion, TCohesion_Delegate);
};

class TBulkStiffness_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_BULKSTIFFNESS, IDI_VALUETYPE_BULKSTIFFNESS,
                                             IDS_ET_BULKSTIFFNESS, IDS_VALUENAME_BULKSTIFFNESS> {
public:
  TBulkStiffness_Delegate(TBulkStiffness *bulkStiffness);

private:
  TBulkStiffness_Delegate(const TBulkStiffness_Delegate &rhs);
  TBulkStiffness_Delegate &operator=(const TBulkStiffness_Delegate &rhs);

  TBulkStiffness *m_bulkStiffness;

  REGISTER_DELEGATE(TBulkStiffness, TBulkStiffness_Delegate);
};

class TShearModulus_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_SHEARMODULUS, IDI_VALUETYPE_SHEARMODULUS,
                                             IDS_ET_SHEARMODULUS, IDS_VALUENAME_SHEARMODULUS> {
public:
  TShearModulus_Delegate(TShearModulus *shearModulus);

private:
  TShearModulus_Delegate(const TShearModulus_Delegate &rhs);
  TShearModulus_Delegate &operator=(const TShearModulus_Delegate &rhs);

  TShearModulus *m_shearModulus;

  REGISTER_DELEGATE(TShearModulus, TShearModulus_Delegate);
};

class TVelocityP_Delegate : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_VELOCITYP, IDI_VALUETYPE_VELOCITYP,
                                                                   IDS_ET_VELOCITYP, IDS_VALUENAME_VELOCITYP> {
public:
  TVelocityP_Delegate(TVelocityP *velocityP);

private:
  TVelocityP_Delegate(const TVelocityP_Delegate &rhs);
  TVelocityP_Delegate &operator=(const TVelocityP_Delegate &rhs);

  TVelocityP *m_velocityP;

  REGISTER_DELEGATE(TVelocityP, TVelocityP_Delegate);
};

class TVelocityS_Delegate : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_VELOCITYS, IDI_VALUETYPE_VELOCITYS,
                                                                   IDS_ET_VELOCITYS, IDS_VALUENAME_VELOCITYS> {
public:
  TVelocityS_Delegate(TVelocityS *velocityS);

private:
  TVelocityS_Delegate(const TVelocityS_Delegate &rhs);
  TVelocityS_Delegate &operator=(const TVelocityS_Delegate &rhs);

  TVelocityS *m_velocityS;

  REGISTER_DELEGATE(TVelocityS, TVelocityS_Delegate);
};

class TFrictionAngle_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_FRICTION_ANGLE, IDI_VALUETYPE_FRICTION_ANGLE,
                                             IDS_ET_FRICTION_ANGLE, IDS_VALUENAME_FRICTION_ANGLE> {
public:
  TFrictionAngle_Delegate(TFrictionAngle *frictionAngle);

private:
  TFrictionAngle_Delegate(const TFrictionAngle_Delegate &rhs);
  TFrictionAngle_Delegate &operator=(const TFrictionAngle_Delegate &rhs);

  TFrictionAngle *m_frictionAngle;

  REGISTER_DELEGATE(TFrictionAngle, TFrictionAngle_Delegate);
};

class TPoissonsRatio_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_POISSONS_RATIO, IDI_VALUETYPE_POISSONS_RATIO,
                                             IDS_ET_POISSON_RATIO, IDS_VALUENAME_POISSONS_RATIO> {
public:
  TPoissonsRatio_Delegate(TPoissonsRatio *poissonsRatio);

private:
  TPoissonsRatio_Delegate(const TPoissonsRatio_Delegate &rhs);
  TPoissonsRatio_Delegate &operator=(const TPoissonsRatio_Delegate &rhs);

  TPoissonsRatio *m_poissonsRatio;

  REGISTER_DELEGATE(TPoissonsRatio, TPoissonsRatio_Delegate);
};

class TYoungsModulus_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_YOUNGS_MODULUS, IDI_VALUETYPE_YOUNGS_MODULUS,
                                             IDS_ET_YOUNG_MODULUS, IDS_VALUENAME_YOUNGS_MODULUS> {
public:
  TYoungsModulus_Delegate(TYoungsModulus *youngsModulus);

private:
  TYoungsModulus_Delegate(const TYoungsModulus_Delegate &rhs);
  TYoungsModulus_Delegate &operator=(const TYoungsModulus_Delegate &rhs);

  TYoungsModulus *m_youngsModulus;

  REGISTER_DELEGATE(TYoungsModulus, TYoungsModulus_Delegate);
};

class TDensity_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_RHOB, IDI_VALUETYPE_RHOB, IDS_ET_RHOB, IDS_VALUENAME_RHOB> {
public:
  TDensity_Delegate(TDensity *density);

private:
  TDensity_Delegate(const TDensity_Delegate &rhs);
  TDensity_Delegate &operator=(const TDensity_Delegate &rhs);

  TDensity *m_density;

  REGISTER_DELEGATE(TDensity, TDensity_Delegate);
};

class TPorosity_Delegate : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_POROSITY, IDI_VALUETYPE_POROSITY,
                                                                  IDS_ET_POROSITY, IDS_VALUENAME_POROSITY> {
public:
  TPorosity_Delegate(TPorosity *porosity);

private:
  TPorosity_Delegate(const TPorosity_Delegate &rhs);
  TPorosity_Delegate &operator=(const TPorosity_Delegate &rhs);

  TPorosity *m_porosity;

  REGISTER_DELEGATE(TPorosity, TPorosity_Delegate);
};

class TInitFriction_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_INITFRICTION, IDI_VALUETYPE_INITFRICTION,
                                             IDS_ET_INITFRICTION, IDS_VALUENAME_INITFRICTION> {
public:
  TInitFriction_Delegate(TInitFriction *initFriction);

private:
  TInitFriction_Delegate(const TInitFriction_Delegate &rhs);
  TInitFriction_Delegate &operator=(const TInitFriction_Delegate &rhs);

  TInitFriction *m_initFriction;

  REGISTER_DELEGATE(TInitFriction, TInitFriction_Delegate);
};

class THardening_Delegate : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_HARDENING, IDI_VALUETYPE_HARDENING,
                                                                   IDS_ET_HARDENING, IDS_VALUENAME_HARDENING> {
public:
  THardening_Delegate(THardening *hardening);

private:
  THardening_Delegate(const THardening_Delegate &rhs);
  THardening_Delegate &operator=(const THardening_Delegate &rhs);

  THardening *m_hardening;

  REGISTER_DELEGATE(THardening, THardening_Delegate);
};

class TPreConsolidation_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_PRECONSOLIDATION, IDI_VALUETYPE_PRECONSOLIDATION,
                                             IDS_ET_PRECONSOLIDATION, IDS_VALUENAME_PRECONSOLIDATION> {
public:
  TPreConsolidation_Delegate(TPreConsolidation *preConsolidation);

private:
  TPreConsolidation_Delegate(const TPreConsolidation_Delegate &rhs);
  TPreConsolidation_Delegate &operator=(const TPreConsolidation_Delegate &rhs);

  TPreConsolidation *m_preConsolidation;

  REGISTER_DELEGATE(TPreConsolidation, TPreConsolidation_Delegate);
};

class TCapShape_Delegate : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_CAPSHAPE, IDI_VALUETYPE_CAPSHAPE,
                                                                  IDS_ET_CAPSHAPE, IDS_VALUENAME_CAPSHAPE> {
public:
  TCapShape_Delegate(TCapShape *capShape);

private:
  TCapShape_Delegate(const TCapShape_Delegate &rhs);
  TCapShape_Delegate &operator=(const TCapShape_Delegate &rhs);

  TCapShape *m_capShape;

  REGISTER_DELEGATE(TCapShape, TCapShape_Delegate);
};

class TDilatation_Delegate : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_DILATATION, IDI_VALUETYPE_DILATATION,
                                                                    IDS_ET_DILATATION, IDS_VALUENAME_DILATATION> {
public:
  TDilatation_Delegate(TDilatation *dilatation);

private:
  TDilatation_Delegate(const TDilatation_Delegate &rhs);
  TDilatation_Delegate &operator=(const TDilatation_Delegate &rhs);

  TDilatation *m_dilatation;

  REGISTER_DELEGATE(TDilatation, TDilatation_Delegate);
};

class TLatRatioMax_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_LATRATIO_MAX, IDI_VALUETYPE_LATRATIO_MAX,
                                             IDS_ET_LATRATIO_MAX, IDS_VALUENAME_LATRATIO_MAX> {
public:
  TLatRatioMax_Delegate(TLatRatioMax *latRatioMax);

private:
  TLatRatioMax_Delegate(const TLatRatioMax_Delegate &rhs);
  TLatRatioMax_Delegate &operator=(const TLatRatioMax_Delegate &rhs);

  TLatRatioMax *m_latRatioMax;

  REGISTER_DELEGATE(TLatRatioMax, TLatRatioMax_Delegate);
};

class TLatRatioMin_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_LATRATIO_MIN, IDI_VALUETYPE_LATRATIO_MIN,
                                             IDS_ET_LATRATIO_MIN, IDS_VALUENAME_LATRATIO_MIN> {
public:
  TLatRatioMin_Delegate(TLatRatioMin *latRatioMin);

private:
  TLatRatioMin_Delegate(const TLatRatioMin_Delegate &rhs);
  TLatRatioMin_Delegate &operator=(const TLatRatioMin_Delegate &rhs);

  TLatRatioMin *m_latRatioMin;

  REGISTER_DELEGATE(TLatRatioMin, TLatRatioMin_Delegate);
};

class TAzimuth_Delegate : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_AZIMUTH, IDI_VALUETYPE_AZIMUTH,
                                                                 IDS_ET_AZIMUTH, IDS_VALUENAME_AZIMUTH> {
public:
  TAzimuth_Delegate(TAzimuth *azimuth);

private:
  TAzimuth_Delegate(const TAzimuth_Delegate &rhs);
  TAzimuth_Delegate &operator=(const TAzimuth_Delegate &rhs);

  TAzimuth *m_azimuth;

  REGISTER_DELEGATE(TAzimuth, TAzimuth_Delegate);
};

class TCreepA1_Delegate : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_CREEP_A1, IDI_VALUETYPE_CREEP_A1,
                                                                 IDS_ET_CREEP_A1, IDS_VALUENAME_CREEP_A1> {
public:
  TCreepA1_Delegate(TCreepA1 *creepA1);

private:
  TCreepA1_Delegate(const TCreepA1_Delegate &rhs);
  TCreepA1_Delegate &operator=(const TCreepA1_Delegate &rhs);

  TCreepA1 *m_creepA1;

  REGISTER_DELEGATE(TCreepA1, TCreepA1_Delegate);
};

class TCreepN1_Delegate : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_CREEP_N1, IDI_VALUETYPE_CREEP_N1,
                                                                 IDS_ET_CREEP_N1, IDS_VALUENAME_CREEP_N1> {
public:
  TCreepN1_Delegate(TCreepN1 *creepN1);

private:
  TCreepN1_Delegate(const TCreepN1_Delegate &rhs);
  TCreepN1_Delegate &operator=(const TCreepN1_Delegate &rhs);

  TCreepN1 *m_creepN1;

  REGISTER_DELEGATE(TCreepN1, TCreepN1_Delegate);
};

class TCreepA2_Delegate : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_CREEP_A2, IDI_VALUETYPE_CREEP_A2,
                                                                 IDS_ET_CREEP_A2, IDS_VALUENAME_CREEP_A2> {
public:
  TCreepA2_Delegate(TCreepA2 *creepA2);

private:
  TCreepA2_Delegate(const TCreepA2_Delegate &rhs);
  TCreepA2_Delegate &operator=(const TCreepA2_Delegate &rhs);

  TCreepA2 *m_creepA2;

  REGISTER_DELEGATE(TCreepA2, TCreepA2_Delegate);
};

class TCreepN2_Delegate : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_CREEP_N2, IDI_VALUETYPE_CREEP_N2,
                                                                 IDS_ET_CREEP_N2, IDS_VALUENAME_CREEP_N2> {
public:
  TCreepN2_Delegate(TCreepN2 *creepN2);

private:
  TCreepN2_Delegate(const TCreepN2_Delegate &rhs);
  TCreepN2_Delegate &operator=(const TCreepN2_Delegate &rhs);

  TCreepN2 *m_creepN2;

  REGISTER_DELEGATE(TCreepN2, TCreepN2_Delegate);
};

class TFluidBulkModulus_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_FLUID_BULK_MOD, IDI_VALUETYPE_FLUID_BULK_MOD,
                                             IDS_ET_VALUETYPE_FLUID_SHEAR_MOD, IDS_VALUENAME_FLUID_BULK_MOD> {
public:
  TFluidBulkModulus_Delegate(TFluidBulkModulus *fluidBulkModulus);

private:
  TFluidBulkModulus_Delegate(const TFluidBulkModulus_Delegate &rhs);
  TFluidBulkModulus_Delegate &operator=(const TFluidBulkModulus_Delegate &rhs);

  TFluidBulkModulus *m_fluidBulkModulus;

  REGISTER_DELEGATE(TFluidBulkModulus, TFluidBulkModulus_Delegate);
};

class THardCohesion1_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_HARD_COHESION1, IDI_VALUETYPE_HARD_COHESION1,
                                             IDS_ET_HARD_COHESION1, IDS_VALUENAME_HARD_COHESION1> {
public:
  THardCohesion1_Delegate(THardCohesion1 *hardCohesion1);

private:
  THardCohesion1_Delegate(const THardCohesion1_Delegate &rhs);
  THardCohesion1_Delegate &operator=(const THardCohesion1_Delegate &rhs);

  THardCohesion1 *m_hardCohesion1;

  REGISTER_DELEGATE(THardCohesion1, THardCohesion1_Delegate);
};

class THardCohesion2_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_HARD_COHESION2, IDI_VALUETYPE_HARD_COHESION2,
                                             IDS_ET_HARD_COHESION2, IDS_VALUENAME_HARD_COHESION2> {
public:
  THardCohesion2_Delegate(THardCohesion2 *hardCohesion2);

private:
  THardCohesion2_Delegate(const THardCohesion2_Delegate &rhs);
  THardCohesion2_Delegate &operator=(const THardCohesion2_Delegate &rhs);

  THardCohesion2 *m_hardCohesion2;

  REGISTER_DELEGATE(THardCohesion2, THardCohesion2_Delegate);
};

class THardCohesion3_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_HARD_COHESION3, IDI_VALUETYPE_HARD_COHESION3,
                                             IDS_ET_HARD_COHESION3, IDS_VALUENAME_HARD_COHESION3> {
public:
  THardCohesion3_Delegate(THardCohesion3 *hardCohesion3);

private:
  THardCohesion3_Delegate(const THardCohesion3_Delegate &rhs);
  THardCohesion3_Delegate &operator=(const THardCohesion3_Delegate &rhs);

  THardCohesion3 *m_hardCohesion3;

  REGISTER_DELEGATE(THardCohesion3, THardCohesion3_Delegate);
};

class THardFriction1_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_HARD_FRICTION1, IDI_VALUETYPE_HARD_FRICTION1,
                                             IDS_ET_HARD_FRICTION1, IDS_VALUENAME_HARD_FRICTION1> {
public:
  THardFriction1_Delegate(THardFriction1 *hardFriction1);

private:
  THardFriction1_Delegate(const THardFriction1_Delegate &rhs);
  THardFriction1_Delegate &operator=(const THardFriction1_Delegate &rhs);

  THardFriction1 *m_hardFriction1;

  REGISTER_DELEGATE(THardFriction1, THardFriction1_Delegate);
};

class THardFriction2_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_HARD_FRICTION2, IDI_VALUETYPE_HARD_FRICTION2,
                                             IDS_ET_HARD_FRICTION2, IDS_VALUENAME_HARD_FRICTION2> {
public:
  THardFriction2_Delegate(THardFriction2 *hardFriction2);

private:
  THardFriction2_Delegate(const THardFriction2_Delegate &rhs);
  THardFriction2_Delegate &operator=(const THardFriction2_Delegate &rhs);

  THardFriction2 *m_hardFriction2;

  REGISTER_DELEGATE(THardFriction2, THardFriction2_Delegate);
};

class THardFriction3_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_HARD_FRICTION3, IDI_VALUETYPE_HARD_FRICTION3,
                                             IDS_ET_HARD_FRICTION3, IDS_VALUENAME_HARD_FRICTION3> {
public:
  THardFriction3_Delegate(THardFriction3 *hardFriction3);

private:
  THardFriction3_Delegate(const THardFriction3_Delegate &rhs);
  THardFriction3_Delegate &operator=(const THardFriction3_Delegate &rhs);

  THardFriction3 *m_hardFriction3;

  REGISTER_DELEGATE(THardFriction3, THardFriction3_Delegate);
};

class TEquivalentPlasticStrain1_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_EQUIV_PLAST_STRAIN1, IDI_VALUETYPE_EQUIV_PLAST_STRAIN1,
                                             IDS_ET_EQUIV_PLAST_STRAIN1, IDS_VALUENAME_EQUIV_PLAST_STRAIN1> {
public:
  TEquivalentPlasticStrain1_Delegate(TEquivalentPlasticStrain1 *equivalentPlasticStrain1);

private:
  TEquivalentPlasticStrain1_Delegate(const TEquivalentPlasticStrain1_Delegate &rhs);
  TEquivalentPlasticStrain1_Delegate &operator=(const TEquivalentPlasticStrain1_Delegate &rhs);

  TEquivalentPlasticStrain1 *m_equivalentPlasticStrain1;

  REGISTER_DELEGATE(TEquivalentPlasticStrain1, TEquivalentPlasticStrain1_Delegate);
};

class TEquivalentPlasticStrain2_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_EQUIV_PLAST_STRAIN2, IDI_VALUETYPE_EQUIV_PLAST_STRAIN2,
                                             IDS_ET_EQUIV_PLAST_STRAIN2, IDS_VALUENAME_EQUIV_PLAST_STRAIN2> {
public:
  TEquivalentPlasticStrain2_Delegate(TEquivalentPlasticStrain2 *equivalentPlasticStrain2);

private:
  TEquivalentPlasticStrain2_Delegate(const TEquivalentPlasticStrain2_Delegate &rhs);
  TEquivalentPlasticStrain2_Delegate &operator=(const TEquivalentPlasticStrain2_Delegate &rhs);

  TEquivalentPlasticStrain2 *m_equivalentPlasticStrain2;

  REGISTER_DELEGATE(TEquivalentPlasticStrain2, TEquivalentPlasticStrain2_Delegate);
};

class TEquivalentPlasticStrain3_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_EQUIV_PLAST_STRAIN3, IDI_VALUETYPE_EQUIV_PLAST_STRAIN3,
                                             IDS_ET_EQUIV_PLAST_STRAIN3, IDS_VALUENAME_EQUIV_PLAST_STRAIN3> {
public:
  TEquivalentPlasticStrain3_Delegate(TEquivalentPlasticStrain3 *equivalentPlasticStrain3);

private:
  TEquivalentPlasticStrain3_Delegate(const TEquivalentPlasticStrain3_Delegate &rhs);
  TEquivalentPlasticStrain3_Delegate &operator=(const TEquivalentPlasticStrain3_Delegate &rhs);

  TEquivalentPlasticStrain3 *m_equivalentPlasticStrain3;

  REGISTER_DELEGATE(TEquivalentPlasticStrain3, TEquivalentPlasticStrain3_Delegate);
};

class TCreepQR1_Delegate : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_CREEP_QR1, IDI_VALUETYPE_CREEP_QR1,
                                                                  IDS_ET_CREEP_QR1, IDS_VALUENAME_CREEP_QR1> {
public:
  TCreepQR1_Delegate(TCreepQR1 *creepQR1);

private:
  TCreepQR1_Delegate(const TCreepQR1_Delegate &rhs);
  TCreepQR1_Delegate &operator=(const TCreepQR1_Delegate &rhs);

  TCreepQR1 *m_creepQR1;

  REGISTER_DELEGATE(TCreepQR1, TCreepQR1_Delegate);
};

class TCreepQR2_Delegate : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_CREEP_QR2, IDI_VALUETYPE_CREEP_QR2,
                                                                  IDS_ET_CREEP_QR2, IDS_VALUENAME_CREEP_QR2> {
public:
  TCreepQR2_Delegate(TCreepQR2 *creepQR2);

private:
  TCreepQR2_Delegate(const TCreepQR2_Delegate &rhs);
  TCreepQR2_Delegate &operator=(const TCreepQR2_Delegate &rhs);

  TCreepQR2 *m_creepQR2;

  REGISTER_DELEGATE(TCreepQR2, TCreepQR2_Delegate);
};

class TTensileStretch_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_TENSILE_STRETCH, IDI_VALUETYPE_TENSILE_STRETCH,
                                             IDS_ET_TENSILE_STRETCH, IDS_VALUENAME_TENSILE_STRETCH> {
public:
  TTensileStretch_Delegate(TTensileStretch *tensileStretch);

private:
  TTensileStretch_Delegate(const TTensileStretch_Delegate &rhs);
  TTensileStretch_Delegate &operator=(const TTensileStretch_Delegate &rhs);

  TTensileStretch *m_tensileStretch;

  REGISTER_DELEGATE(TTensileStretch, TTensileStretch_Delegate);
};

class TThermalExpansion_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_THERMAL_EXPANSION, IDI_VALUETYPE_THERMAL_EXPANSION,
                                             IDS_ET_THERMAL_EXPANSION, IDS_VALUENAME_THERMAL_EXPANSION> {
public:
  TThermalExpansion_Delegate(TThermalExpansion *thermalExpansion);

private:
  TThermalExpansion_Delegate(const TThermalExpansion_Delegate &rhs);
  TThermalExpansion_Delegate &operator=(const TThermalExpansion_Delegate &rhs);

  TThermalExpansion *m_thermalExpansion;

  REGISTER_DELEGATE(TThermalExpansion, TThermalExpansion_Delegate);
};

class TRigidParam1_Delegate : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_RIGI_PARAM1, IDI_VALUETYPE_RIGID_1,
                                                                     IDS_RIGI_PARAM1, IDS_RIGI_PARAM1> {
public:
  TRigidParam1_Delegate(TRigidParam1 *rigidParam1);

private:
  TRigidParam1_Delegate(const TRigidParam1_Delegate &rhs);
  TRigidParam1_Delegate &operator=(const TRigidParam1_Delegate &rhs);

  TRigidParam1 *m_rigidParam1;

  REGISTER_DELEGATE(TRigidParam1, TRigidParam1_Delegate);
};

class TRigidParam2_Delegate : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_RIGI_PARAM2, IDI_VALUETYPE_RIGID_2,
                                                                     IDS_RIGI_PARAM2, IDS_RIGI_PARAM2> {
public:
  TRigidParam2_Delegate(TRigidParam2 *rigidParam2);

private:
  TRigidParam2_Delegate(const TRigidParam2_Delegate &rhs);
  TRigidParam2_Delegate &operator=(const TRigidParam2_Delegate &rhs);

  TRigidParam2 *m_rigidParam2;

  REGISTER_DELEGATE(TRigidParam2, TRigidParam2_Delegate);
};

class TRigidParam3_Delegate : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_RIGI_PARAM3, IDI_VALUETYPE_RIGID_3,
                                                                     IDS_RIGI_PARAM3, IDS_RIGI_PARAM3> {
public:
  TRigidParam3_Delegate(TRigidParam3 *rigidParam3);

private:
  TRigidParam3_Delegate(const TRigidParam3_Delegate &rhs);
  TRigidParam3_Delegate &operator=(const TRigidParam3_Delegate &rhs);

  TRigidParam3 *m_rigidParam3;

  REGISTER_DELEGATE(TRigidParam3, TRigidParam3_Delegate);
};

class TRigidParam4_Delegate : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_RIGI_PARAM4, IDI_VALUETYPE_RIGID_4,
                                                                     IDS_RIGI_PARAM4, IDS_RIGI_PARAM4> {
public:
  TRigidParam4_Delegate(TRigidParam4 *rigidParam4);

private:
  TRigidParam4_Delegate(const TRigidParam4_Delegate &rhs);
  TRigidParam4_Delegate &operator=(const TRigidParam4_Delegate &rhs);

  TRigidParam4 *m_rigidParam4;

  REGISTER_DELEGATE(TRigidParam4, TRigidParam4_Delegate);
};

class TRigidParam5_Delegate : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_RIGI_PARAM5, IDI_VALUETYPE_RIGID_5,
                                                                     IDS_RIGI_PARAM5, IDS_RIGI_PARAM5> {
public:
  TRigidParam5_Delegate(TRigidParam5 *rigidParam5);

private:
  TRigidParam5_Delegate(const TRigidParam5_Delegate &rhs);
  TRigidParam5_Delegate &operator=(const TRigidParam5_Delegate &rhs);

  TRigidParam5 *m_rigidParam5;

  REGISTER_DELEGATE(TRigidParam5, TRigidParam5_Delegate);
};

class TRigidParam6_Delegate : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_RIGI_PARAM6, IDI_VALUETYPE_RIGID_6,
                                                                     IDS_RIGI_PARAM6, IDS_RIGI_PARAM6> {
public:
  TRigidParam6_Delegate(TRigidParam6 *rigidParam6);

private:
  TRigidParam6_Delegate(const TRigidParam6_Delegate &rhs);
  TRigidParam6_Delegate &operator=(const TRigidParam6_Delegate &rhs);

  TRigidParam6 *m_rigidParam6;

  REGISTER_DELEGATE(TRigidParam6, TRigidParam6_Delegate);
};

class TRigidParam7_Delegate : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_RIGI_PARAM7, IDI_VALUETYPE_RIGID_7,
                                                                     IDS_RIGI_PARAM7, IDS_RIGI_PARAM7> {
public:
  TRigidParam7_Delegate(TRigidParam7 *rigidParam7);

private:
  TRigidParam7_Delegate(const TRigidParam7_Delegate &rhs);
  TRigidParam7_Delegate &operator=(const TRigidParam7_Delegate &rhs);

  TRigidParam7 *m_rigidParam7;

  REGISTER_DELEGATE(TRigidParam7, TRigidParam7_Delegate);
};

class TRigidParam8_Delegate : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_RIGI_PARAM8, IDI_VALUETYPE_RIGID_8,
                                                                     IDS_RIGI_PARAM8, IDS_RIGI_PARAM8> {
public:
  TRigidParam8_Delegate(TRigidParam8 *rigidParam8);

private:
  TRigidParam8_Delegate(const TRigidParam8_Delegate &rhs);
  TRigidParam8_Delegate &operator=(const TRigidParam8_Delegate &rhs);

  TRigidParam8 *m_rigidParam8;

  REGISTER_DELEGATE(TRigidParam8, TRigidParam8_Delegate);
};

class TRigidParam9_Delegate : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_RIGI_PARAM9, IDI_VALUETYPE_RIGID_9,
                                                                     IDS_RIGI_PARAM9, IDS_RIGI_PARAM9> {
public:
  TRigidParam9_Delegate(TRigidParam9 *rigidParam9);

private:
  TRigidParam9_Delegate(const TRigidParam9_Delegate &rhs);
  TRigidParam9_Delegate &operator=(const TRigidParam9_Delegate &rhs);

  TRigidParam9 *m_rigidParam9;

  REGISTER_DELEGATE(TRigidParam9, TRigidParam9_Delegate);
};

class TRigidParam10_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_RIGI_PARAM10, IDI_VALUETYPE_RIGID_10, IDS_RIGI_PARAM10,
                                             IDS_RIGI_PARAM10> {
public:
  TRigidParam10_Delegate(TRigidParam10 *rigidParam10);

private:
  TRigidParam10_Delegate(const TRigidParam10_Delegate &rhs);
  TRigidParam10_Delegate &operator=(const TRigidParam10_Delegate &rhs);

  TRigidParam10 *m_rigidParam10;

  REGISTER_DELEGATE(TRigidParam10, TRigidParam10_Delegate);
};

class TRigidParam11_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_RIGI_PARAM11, IDI_VALUETYPE_RIGID_11, IDS_RIGI_PARAM11,
                                             IDS_RIGI_PARAM11> {
public:
  TRigidParam11_Delegate(TRigidParam11 *rigidParam11);

private:
  TRigidParam11_Delegate(const TRigidParam11_Delegate &rhs);
  TRigidParam11_Delegate &operator=(const TRigidParam11_Delegate &rhs);

  TRigidParam11 *m_rigidParam11;

  REGISTER_DELEGATE(TRigidParam11, TRigidParam11_Delegate);
};

class TRigidParam12_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_RIGI_PARAM12, IDI_VALUETYPE_RIGID_12, IDS_RIGI_PARAM12,
                                             IDS_RIGI_PARAM12> {
public:
  TRigidParam12_Delegate(TRigidParam12 *rigidParam12);

private:
  TRigidParam12_Delegate(const TRigidParam12_Delegate &rhs);
  TRigidParam12_Delegate &operator=(const TRigidParam12_Delegate &rhs);

  TRigidParam12 *m_rigidParam12;

  REGISTER_DELEGATE(TRigidParam12, TRigidParam12_Delegate);
};

class TRigidParam13_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_RIGI_PARAM13, IDI_VALUETYPE_RIGID_13, IDS_RIGI_PARAM13,
                                             IDS_RIGI_PARAM13> {
public:
  TRigidParam13_Delegate(TRigidParam13 *rigidParam13);

private:
  TRigidParam13_Delegate(const TRigidParam13_Delegate &rhs);
  TRigidParam13_Delegate &operator=(const TRigidParam13_Delegate &rhs);

  TRigidParam13 *m_rigidParam13;

  REGISTER_DELEGATE(TRigidParam13, TRigidParam13_Delegate);
};

class TRigidParam14_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_RIGI_PARAM14, IDI_VALUETYPE_RIGID_14, IDS_RIGI_PARAM14,
                                             IDS_RIGI_PARAM14> {
public:
  TRigidParam14_Delegate(TRigidParam14 *rigidParam14);

private:
  TRigidParam14_Delegate(const TRigidParam14_Delegate &rhs);
  TRigidParam14_Delegate &operator=(const TRigidParam14_Delegate &rhs);

  TRigidParam14 *m_rigidParam14;

  REGISTER_DELEGATE(TRigidParam14, TRigidParam14_Delegate);
};

class TRigidParam15_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_RIGI_PARAM15, IDI_VALUETYPE_RIGID_15, IDS_RIGI_PARAM15,
                                             IDS_RIGI_PARAM15> {
public:
  TRigidParam15_Delegate(TRigidParam15 *rigidParam15);

private:
  TRigidParam15_Delegate(const TRigidParam15_Delegate &rhs);
  TRigidParam15_Delegate &operator=(const TRigidParam15_Delegate &rhs);

  TRigidParam15 *m_rigidParam15;

  REGISTER_DELEGATE(TRigidParam15, TRigidParam15_Delegate);
};

class TRigidParam16_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_RIGI_PARAM16, IDI_VALUETYPE_RIGID_16, IDS_RIGI_PARAM16,
                                             IDS_RIGI_PARAM16> {
public:
  TRigidParam16_Delegate(TRigidParam16 *rigidParam16);

private:
  TRigidParam16_Delegate(const TRigidParam16_Delegate &rhs);
  TRigidParam16_Delegate &operator=(const TRigidParam16_Delegate &rhs);

  TRigidParam16 *m_rigidParam16;

  REGISTER_DELEGATE(TRigidParam16, TRigidParam16_Delegate);
};

class TRigidParam17_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_RIGI_PARAM17, IDI_VALUETYPE_RIGID_17, IDS_RIGI_PARAM17,
                                             IDS_RIGI_PARAM17> {
public:
  TRigidParam17_Delegate(TRigidParam17 *rigidParam17);

private:
  TRigidParam17_Delegate(const TRigidParam17_Delegate &rhs);
  TRigidParam17_Delegate &operator=(const TRigidParam17_Delegate &rhs);

  TRigidParam17 *m_rigidParam17;

  REGISTER_DELEGATE(TRigidParam17, TRigidParam17_Delegate);
};

class TRigidParam18_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_RIGI_PARAM18, IDI_VALUETYPE_RIGID_18, IDS_RIGI_PARAM18,
                                             IDS_RIGI_PARAM18> {
public:
  TRigidParam18_Delegate(TRigidParam18 *rigidParam18);

private:
  TRigidParam18_Delegate(const TRigidParam18_Delegate &rhs);
  TRigidParam18_Delegate &operator=(const TRigidParam18_Delegate &rhs);

  TRigidParam18 *m_rigidParam18;

  REGISTER_DELEGATE(TRigidParam18, TRigidParam18_Delegate);
};

class TRigidParam19_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_RIGI_PARAM19, IDI_VALUETYPE_RIGID_19, IDS_RIGI_PARAM19,
                                             IDS_RIGI_PARAM19> {
public:
  TRigidParam19_Delegate(TRigidParam19 *rigidParam19);

private:
  TRigidParam19_Delegate(const TRigidParam19_Delegate &rhs);
  TRigidParam19_Delegate &operator=(const TRigidParam19_Delegate &rhs);

  TRigidParam19 *m_rigidParam19;

  REGISTER_DELEGATE(TRigidParam19, TRigidParam19_Delegate);
};

class TRigidParam20_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_RIGI_PARAM20, IDI_VALUETYPE_RIGID_20, IDS_RIGI_PARAM20,
                                             IDS_RIGI_PARAM20> {
public:
  TRigidParam20_Delegate(TRigidParam20 *rigidParam20);

private:
  TRigidParam20_Delegate(const TRigidParam20_Delegate &rhs);
  TRigidParam20_Delegate &operator=(const TRigidParam20_Delegate &rhs);

  TRigidParam20 *m_rigidParam20;

  REGISTER_DELEGATE(TRigidParam20, TRigidParam20_Delegate);
};

class TRigidParam21_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_RIGI_PARAM21, IDI_VALUETYPE_RIGID_21, IDS_RIGI_PARAM21,
                                             IDS_RIGI_PARAM21> {
public:
  TRigidParam21_Delegate(TRigidParam21 *rigidParam21);

private:
  TRigidParam21_Delegate(const TRigidParam21_Delegate &rhs);
  TRigidParam21_Delegate &operator=(const TRigidParam21_Delegate &rhs);

  TRigidParam21 *m_rigidParam21;

  REGISTER_DELEGATE(TRigidParam21, TRigidParam21_Delegate);
};

class TElasticHardening_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_ELASTIC_HARDENING, IDI_VALUETYPE_ELASTIC_HARDENING,
                                             IDS_ET_ELASTIC_HARDENING, IDS_VALUENAME_ELASTIC_HARDENING> {
public:
  TElasticHardening_Delegate(TElasticHardening *elasticHardening);

private:
  TElasticHardening_Delegate(const TElasticHardening_Delegate &rhs);
  TElasticHardening_Delegate &operator=(const TElasticHardening_Delegate &rhs);

  TElasticHardening *m_elasticHardening;

  REGISTER_DELEGATE(TElasticHardening, TElasticHardening_Delegate);
};

class TSecondaryPreconsolidation_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_SEC_PRECON, IDI_VALUETYPE_SEC_PRECON, IDS_ET_SEC_PRECON,
                                             IDS_VALUENAME_SEC_PRECON> {
public:
  TSecondaryPreconsolidation_Delegate(TSecondaryPreconsolidation *secondaryPreconsolidation);

private:
  TSecondaryPreconsolidation_Delegate(const TSecondaryPreconsolidation_Delegate &rhs);
  TSecondaryPreconsolidation_Delegate &operator=(const TSecondaryPreconsolidation_Delegate &rhs);

  TSecondaryPreconsolidation *m_secondaryPreconsolidation;

  REGISTER_DELEGATE(TSecondaryPreconsolidation, TSecondaryPreconsolidation_Delegate);
};

class TSecondaryHardening_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_SEC_HARDENING, IDI_VALUETYPE_SEC_HARDENING,
                                             IDS_ET_SEC_HARDENING, IDS_VALUENAME_SEC_HARDENING> {
public:
  TSecondaryHardening_Delegate(TSecondaryHardening *secondaryHardening);

private:
  TSecondaryHardening_Delegate(const TSecondaryHardening_Delegate &rhs);
  TSecondaryHardening_Delegate &operator=(const TSecondaryHardening_Delegate &rhs);

  TSecondaryHardening *m_secondaryHardening;

  REGISTER_DELEGATE(TSecondaryHardening, TSecondaryHardening_Delegate);
};

class TUniaxialElasticCompressibility_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_ELASCOMPRES, IDI_VALUETYPE_ELASCOMPRES, IDS_ET_ELASCOMPRES,
                                             IDS_VALUENAME_ELASCOMPRES> {
public:
  TUniaxialElasticCompressibility_Delegate(TUniaxialElasticCompressibility *uniaxialElasticCompressibility);

private:
  TUniaxialElasticCompressibility_Delegate(const TUniaxialElasticCompressibility_Delegate &rhs);
  TUniaxialElasticCompressibility_Delegate &operator=(const TUniaxialElasticCompressibility_Delegate &rhs);

  TUniaxialElasticCompressibility *m_uniaxialElasticCompressibility;

  REGISTER_DELEGATE(TUniaxialElasticCompressibility, TUniaxialElasticCompressibility_Delegate);
};

class TUniaxialPlasticCompressibility_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_PLASCOMPRES, IDI_VALUETYPE_PLASCOMPRES, IDS_ET_PLASCOMPRES,
                                             IDS_VALUENAME_PLASCOMPRES> {
public:
  TUniaxialPlasticCompressibility_Delegate(TUniaxialPlasticCompressibility *uniaxialPlasticCompressibility);

private:
  TUniaxialPlasticCompressibility_Delegate(const TUniaxialPlasticCompressibility_Delegate &rhs);
  TUniaxialPlasticCompressibility_Delegate &operator=(const TUniaxialPlasticCompressibility_Delegate &rhs);

  TUniaxialPlasticCompressibility *m_uniaxialPlasticCompressibility;

  REGISTER_DELEGATE(TUniaxialPlasticCompressibility, TUniaxialPlasticCompressibility_Delegate);
};

class TYoungModulusNormal_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_YOUNGMODULUS_NORM, IDI_VALUETYPE_YOUNGMODULUS_NORM,
                                             IDS_ET_YOUNGMODULUS_NORM, IDS_VALUENAME_YOUNGMODULUS_NORM> {
public:
  TYoungModulusNormal_Delegate(TYoungModulusNormal *youngModulusNormal);

private:
  TYoungModulusNormal_Delegate(const TYoungModulusNormal_Delegate &rhs);
  TYoungModulusNormal_Delegate &operator=(const TYoungModulusNormal_Delegate &rhs);

  TYoungModulusNormal *m_youngModulusNormal;

  REGISTER_DELEGATE(TYoungModulusNormal, TYoungModulusNormal_Delegate);
};

class TYoungModulusTransverse_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_YOUNGMODULUS_TRANS, IDI_VALUETYPE_YOUNGMODULUS_TRANS,
                                             IDS_ET_YOUNGMODULUS_TRANS, IDS_VALUENAME_YOUNGMODULUS_TRANS> {
public:
  TYoungModulusTransverse_Delegate(TYoungModulusTransverse *youngModulusTransverse);

private:
  TYoungModulusTransverse_Delegate(const TYoungModulusTransverse_Delegate &rhs);
  TYoungModulusTransverse_Delegate &operator=(const TYoungModulusTransverse_Delegate &rhs);

  TYoungModulusTransverse *m_youngModulusTransverse;

  REGISTER_DELEGATE(TYoungModulusTransverse, TYoungModulusTransverse_Delegate);
};

class TPoissonRatioNormal_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_POISSONRATIO_NORM, IDI_VALUETYPE_POISSONRATIO_NORM,
                                             IDS_ET_POISSONRATIO_NORM, IDS_VALUENAME_POISSONRATIO_NORM> {
public:
  TPoissonRatioNormal_Delegate(TPoissonRatioNormal *poissonRatioNormal);

private:
  TPoissonRatioNormal_Delegate(const TPoissonRatioNormal_Delegate &rhs);
  TPoissonRatioNormal_Delegate &operator=(const TPoissonRatioNormal_Delegate &rhs);

  TPoissonRatioNormal *m_poissonRatioNormal;

  REGISTER_DELEGATE(TPoissonRatioNormal, TPoissonRatioNormal_Delegate);
};

class TPoissonRatioTransverse_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_POISSONRATIO_TRANS, IDI_VALUETYPE_POISSONRATIO_TRANS,
                                             IDS_ET_POISSONRATIO_TRANS, IDS_VALUENAME_POISSONRATIO_TRANS> {
public:
  TPoissonRatioTransverse_Delegate(TPoissonRatioTransverse *poissonRatioTransverse);

private:
  TPoissonRatioTransverse_Delegate(const TPoissonRatioTransverse_Delegate &rhs);
  TPoissonRatioTransverse_Delegate &operator=(const TPoissonRatioTransverse_Delegate &rhs);

  TPoissonRatioTransverse *m_poissonRatioTransverse;

  REGISTER_DELEGATE(TPoissonRatioTransverse, TPoissonRatioTransverse_Delegate);
};

class TLayerNormalIncl_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_LAYER_NORMAL_INCL, IDI_VALUETYPE_LAYER_NORMAL_INCL,
                                             IDS_ET_LAYER_NORMAL_INCL, IDS_VALUENAME_LAYER_NORMAL_INCL> {
public:
  TLayerNormalIncl_Delegate(TLayerNormalIncl *layerNormalIncl);

private:
  TLayerNormalIncl_Delegate(const TLayerNormalIncl_Delegate &rhs);
  TLayerNormalIncl_Delegate &operator=(const TLayerNormalIncl_Delegate &rhs);

  TLayerNormalIncl *m_layerNormalIncl;

  REGISTER_DELEGATE(TLayerNormalIncl, TLayerNormalIncl_Delegate);
};

class TLayerNormalAzi_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_LAYER_NORMAL_AZI, IDI_VALUETYPE_LAYER_NORMAL_AZI,
                                             IDS_ET_LAYER_NORMAL_AZI, IDS_VALUENAME_LAYER_NORMAL_AZI> {
public:
  TLayerNormalAzi_Delegate(TLayerNormalAzi *layerNormalAzi);

private:
  TLayerNormalAzi_Delegate(const TLayerNormalAzi_Delegate &rhs);
  TLayerNormalAzi_Delegate &operator=(const TLayerNormalAzi_Delegate &rhs);

  TLayerNormalAzi *m_layerNormalAzi;

  REGISTER_DELEGATE(TLayerNormalAzi, TLayerNormalAzi_Delegate);
};

class TFractStiffNormal_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_FRACT_STIFF_NORMAL, IDI_VALUETYPE_FRACT_STIFF_NORMAL,
                                             IDS_ET_FRACT_STIFF_NORMAL, IDS_VALUENAME_FRACT_STIFF_NORMAL> {
public:
  TFractStiffNormal_Delegate(TFractStiffNormal *fractStiffNormal);

private:
  TFractStiffNormal_Delegate(const TFractStiffNormal_Delegate &rhs);
  TFractStiffNormal_Delegate &operator=(const TFractStiffNormal_Delegate &rhs);

  TFractStiffNormal *m_fractStiffNormal;

  REGISTER_DELEGATE(TFractStiffNormal, TFractStiffNormal_Delegate);
};

class TFractStiffShear_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_FRACT_STIFF_SHEAR, IDI_VALUETYPE_FRACT_STIFF_SHEAR,
                                             IDS_ET_FRACT_STIFF_SHEAR, IDS_VALUENAME_FRACT_STIFF_SHEAR> {
public:
  TFractStiffShear_Delegate(TFractStiffShear *fractStiffShear);

private:
  TFractStiffShear_Delegate(const TFractStiffShear_Delegate &rhs);
  TFractStiffShear_Delegate &operator=(const TFractStiffShear_Delegate &rhs);

  TFractStiffShear *m_fractStiffShear;

  REGISTER_DELEGATE(TFractStiffShear, TFractStiffShear_Delegate);
};

class TThermLinExpNormal_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_THERM_LIN_EXP_NORM, IDI_VALUETYPE_THERM_LIN_EXP_NORM,
                                             IDS_ET_THERM_LIN_EXP_NORM, IDS_VALUENAME_THERM_LIN_EXP_NORM> {
public:
  TThermLinExpNormal_Delegate(TThermLinExpNormal *thermLinExpNormal);

private:
  TThermLinExpNormal_Delegate(const TThermLinExpNormal_Delegate &rhs);
  TThermLinExpNormal_Delegate &operator=(const TThermLinExpNormal_Delegate &rhs);

  TThermLinExpNormal *m_thermLinExpNormal;

  REGISTER_DELEGATE(TThermLinExpNormal, TThermLinExpNormal_Delegate);
};

class TThermLinExpLateral_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_THERM_LIN_EXP_LAT, IDI_VALUETYPE_THERM_LIN_EXP_LAT,
                                             IDS_ET_THERM_LIN_EXP_LAT, IDS_VALUENAME_THERM_LIN_EXP_LAT> {
public:
  TThermLinExpLateral_Delegate(TThermLinExpLateral *thermLinExpLateral);

private:
  TThermLinExpLateral_Delegate(const TThermLinExpLateral_Delegate &rhs);
  TThermLinExpLateral_Delegate &operator=(const TThermLinExpLateral_Delegate &rhs);

  TThermLinExpLateral *m_thermLinExpLateral;

  REGISTER_DELEGATE(TThermLinExpLateral, TThermLinExpLateral_Delegate);
};

class TThermalConductivity_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_THERM_CONDUCT, IDI_VALUETYPE_THERM_CONDUCT,
                                             IDS_ET_THERM_CONDUCT, IDS_VALUENAME_THERM_CONDUCT> {
public:
  TThermalConductivity_Delegate(TThermalConductivity *thermalConductivity);

private:
  TThermalConductivity_Delegate(const TThermalConductivity_Delegate &rhs);
  TThermalConductivity_Delegate &operator=(const TThermalConductivity_Delegate &rhs);

  TThermalConductivity *m_thermalConductivity;

  REGISTER_DELEGATE(TThermalConductivity, TThermalConductivity_Delegate);
};

class TThermalCapacity_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_THERM_CAPACI, IDI_VALUETYPE_THERM_CAPACI,
                                             IDS_ET_THERM_CAPACI, IDS_VALUENAME_THERM_CAPACI> {
public:
  TThermalCapacity_Delegate(TThermalCapacity *thermalCapacity);

private:
  TThermalCapacity_Delegate(const TThermalCapacity_Delegate &rhs);
  TThermalCapacity_Delegate &operator=(const TThermalCapacity_Delegate &rhs);

  TThermalCapacity *m_thermalCapacity;

  REGISTER_DELEGATE(TThermalCapacity, TThermalCapacity_Delegate);
};

class THighFractDens_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_HIGH_FRACT_DENS, IDI_VALUETYPE_HIGH_FRACT_DENS,
                                             IDS_ET_HIGH_FRACT_DENS, IDS_VALUENAME_HIGH_FRACT_DENS> {
public:
  THighFractDens_Delegate(THighFractDens *highFractDens);

private:
  THighFractDens_Delegate(const THighFractDens_Delegate &rhs);
  THighFractDens_Delegate &operator=(const THighFractDens_Delegate &rhs);

  THighFractDens *m_highFractDens;

  REGISTER_DELEGATE(THighFractDens, THighFractDens_Delegate);
};

class THighFractDensIncl_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_HIGH_FRACT_DENS_INCL, IDI_VALUETYPE_HIGH_FRACT_DENS_INCL,
                                             IDS_ET_HIGH_FRACT_DENS_INCL, IDS_VALUENAME_HIGH_FRACT_DENS_INCL> {
public:
  THighFractDensIncl_Delegate(THighFractDensIncl *highFractDensIncl);

private:
  THighFractDensIncl_Delegate(const THighFractDensIncl_Delegate &rhs);
  THighFractDensIncl_Delegate &operator=(const THighFractDensIncl_Delegate &rhs);

  THighFractDensIncl *m_highFractDensIncl;

  REGISTER_DELEGATE(THighFractDensIncl, THighFractDensIncl_Delegate);
};

class THighFractDensAzi_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_HIGH_FRACT_DENS_AZI, IDI_VALUETYPE_HIGH_FRACT_DENS_AZI,
                                             IDS_ET_HIGH_FRACT_DENS_AZI, IDS_VALUENAME_HIGH_FRACT_DENS_AZI> {
public:
  THighFractDensAzi_Delegate(THighFractDensAzi *highFractDensAzi);

private:
  THighFractDensAzi_Delegate(const THighFractDensAzi_Delegate &rhs);
  THighFractDensAzi_Delegate &operator=(const THighFractDensAzi_Delegate &rhs);

  THighFractDensAzi *m_highFractDensAzi;

  REGISTER_DELEGATE(THighFractDensAzi, THighFractDensAzi_Delegate);
};

class TLowFractDens_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_LOW_FRACT_DENS, IDI_VALUETYPE_LOW_FRACT_DENS,
                                             IDS_ET_LOW_FRACT_DENS, IDS_VALUENAME_LOW_FRACT_DENS> {
public:
  TLowFractDens_Delegate(TLowFractDens *lowFractDens);

private:
  TLowFractDens_Delegate(const TLowFractDens_Delegate &rhs);
  TLowFractDens_Delegate &operator=(const TLowFractDens_Delegate &rhs);

  TLowFractDens *m_lowFractDens;

  REGISTER_DELEGATE(TLowFractDens, TLowFractDens_Delegate);
};

class TLowFractDensIncl_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_LOW_FRACT_DENS_INCL, IDI_VALUETYPE_LOW_FRACT_DENS_INCL,
                                             IDS_ET_LOW_FRACT_DENS_INCL, IDS_VALUENAME_LOW_FRACT_DENS_INCL> {
public:
  TLowFractDensIncl_Delegate(TLowFractDensIncl *lowFractDensIncl);

private:
  TLowFractDensIncl_Delegate(const TLowFractDensIncl_Delegate &rhs);
  TLowFractDensIncl_Delegate &operator=(const TLowFractDensIncl_Delegate &rhs);

  TLowFractDensIncl *m_lowFractDensIncl;

  REGISTER_DELEGATE(TLowFractDensIncl, TLowFractDensIncl_Delegate);
};

class TLowFractDensAzi_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_LOW_FRACT_DENS_AZI, IDI_VALUETYPE_LOW_FRACT_DENS_AZI,
                                             IDS_ET_LOW_FRACT_DENS_AZI, IDS_VALUENAME_LOW_FRACT_DENS_AZI> {
public:
  TLowFractDensAzi_Delegate(TLowFractDensAzi *lowFractDensAzi);

private:
  TLowFractDensAzi_Delegate(const TLowFractDensAzi_Delegate &rhs);
  TLowFractDensAzi_Delegate &operator=(const TLowFractDensAzi_Delegate &rhs);

  TLowFractDensAzi *m_lowFractDensAzi;

  REGISTER_DELEGATE(TLowFractDensAzi, TLowFractDensAzi_Delegate);
};

class TInterFractDens_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_INTER_FRACT_DENS, IDI_VALUETYPE_INT_FRACT_DENS,
                                             IDS_ET_INTER_FRACT_DENS, IDS_VALUENAME_INTER_FRACT_DENS> {
public:
  TInterFractDens_Delegate(TInterFractDens *interFractDens);

private:
  TInterFractDens_Delegate(const TInterFractDens_Delegate &rhs);
  TInterFractDens_Delegate &operator=(const TInterFractDens_Delegate &rhs);

  TInterFractDens *m_interFractDens;

  REGISTER_DELEGATE(TInterFractDens, TInterFractDens_Delegate);
};

class TPermeability_Delegate : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_PERMEA, IDI_VALUETYPE_PERMEA,
                                                                      IDS_ET_PERMEA, IDS_VALUENAME_PERMEA> {
public:
  TPermeability_Delegate(TPermeability *permeability);

private:
  TPermeability_Delegate(const TPermeability_Delegate &rhs);
  TPermeability_Delegate &operator=(const TPermeability_Delegate &rhs);

  TPermeability *m_permeability;

  REGISTER_DELEGATE(TPermeability, TPermeability_Delegate);
};

class TViscosity_Delegate : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_VISCOSITY, IDI_VALUETYPE_VISCOSITY,
                                                                   IDS_ET_VISCOSITY, IDS_VALUENAME_VISCOSITY> {
public:
  TViscosity_Delegate(TViscosity *viscosity);

private:
  TViscosity_Delegate(const TViscosity_Delegate &rhs);
  TViscosity_Delegate &operator=(const TViscosity_Delegate &rhs);

  TViscosity *m_viscosity;

  REGISTER_DELEGATE(TViscosity, TViscosity_Delegate);
};

class TYoungsModulusDecompaction_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_YOUNG_DECOMP, IDI_VALUETYPE_YOUNG_DECOMP,
                                             IDS_ET_YOUNG_DECOMP, IDS_VALUENAME_YOUNG_DECOMP> {
public:
  TYoungsModulusDecompaction_Delegate(TYoungsModulusDecompaction *youngsModulusDecompaction);

private:
  TYoungsModulusDecompaction_Delegate(const TYoungsModulusDecompaction_Delegate &rhs);
  TYoungsModulusDecompaction_Delegate &operator=(const TYoungsModulusDecompaction_Delegate &rhs);

  TYoungsModulusDecompaction *m_youngsModulusDecompaction;

  REGISTER_DELEGATE(TYoungsModulusDecompaction, TYoungsModulusDecompaction_Delegate);
};

class TPoissonsRatioDecompaction_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_POISSON_DECOMP, IDI_VALUETYPE_POISSON_DECOMP,
                                             IDS_ET_POISSON_DECOMP, IDS_VALUENAME_POISSON_DECOMP> {
public:
  TPoissonsRatioDecompaction_Delegate(TPoissonsRatioDecompaction *poissonsRatioDecompaction);

private:
  TPoissonsRatioDecompaction_Delegate(const TPoissonsRatioDecompaction_Delegate &rhs);
  TPoissonsRatioDecompaction_Delegate &operator=(const TPoissonsRatioDecompaction_Delegate &rhs);

  TPoissonsRatioDecompaction *m_poissonsRatioDecompaction;

  REGISTER_DELEGATE(TPoissonsRatioDecompaction, TPoissonsRatioDecompaction_Delegate);
};

class TFluidThermalExpansionCoefficient_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_FLUIDX, IDI_VALUETYPE_CAPSHAPE, IDS_ET_VALUETYPE_FLUIDX,
                                             IDS_VALUENAME_FLUIDX> {
public:
  TFluidThermalExpansionCoefficient_Delegate(TFluidThermalExpansionCoefficient *fluidThermalExpansionCoefficient);

private:
  TFluidThermalExpansionCoefficient_Delegate(const TFluidThermalExpansionCoefficient_Delegate &rhs);
  TFluidThermalExpansionCoefficient_Delegate &operator=(const TFluidThermalExpansionCoefficient_Delegate &rhs);

  TFluidThermalExpansionCoefficient *m_fluidThermalExpansionCoefficient;

  REGISTER_DELEGATE(TFluidThermalExpansionCoefficient, TFluidThermalExpansionCoefficient_Delegate);
};

class TFluidDensity_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_FLUID_DENSITY, IDI_VALUETYPE_FLUID_DENSITY,
                                             IDS_ET_VALUETYPE_FLUID_DENSITY, IDS_VALUENAME_FLUID_DENSITY> {
public:
  TFluidDensity_Delegate(TFluidDensity *fluidDensity);

private:
  TFluidDensity_Delegate(const TFluidDensity_Delegate &rhs);
  TFluidDensity_Delegate &operator=(const TFluidDensity_Delegate &rhs);

  TFluidDensity *m_fluidDensity;

  REGISTER_DELEGATE(TFluidDensity, TFluidDensity_Delegate);
};

class TReferenceStress_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_REFERENCESTRESS, IDI_VALUETYPE_REFERENCESTRESS,
                                             IDS_ET_VALUETYPE_REFERENCESTRESS, IDS_VALUENAME_REFERENCESTRESS> {
public:
  TReferenceStress_Delegate(TReferenceStress *referenceStress);

private:
  TReferenceStress_Delegate(const TReferenceStress_Delegate &rhs);
  TReferenceStress_Delegate &operator=(const TReferenceStress_Delegate &rhs);

  TReferenceStress *m_referenceStress;

  REGISTER_DELEGATE(TReferenceStress, TReferenceStress_Delegate);
};

class TApertureParameter_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_APERTUREPARAM, IDI_VALUETYPE_APERTUREPARAM,
                                             IDS_ET_VALUETYPE_APERTUREPARAM, IDS_VALUENAME_APERTUREPARAM> {
public:
  TApertureParameter_Delegate(TApertureParameter *apertureParameter);

private:
  TApertureParameter_Delegate(const TApertureParameter_Delegate &rhs);
  TApertureParameter_Delegate &operator=(const TApertureParameter_Delegate &rhs);

  TApertureParameter *m_apertureParameter;

  REGISTER_DELEGATE(TApertureParameter, TApertureParameter_Delegate);
};

class TGrainStiffnessParameter_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_GRAINSTIFFNESS, IDI_VALUETYPE_GRAINSTIFFNESS,
                                             IDS_ET_VALUETYPE_GRAINSTIFFNESS, IDS_VALUENAME_GRAINSTIFFNESS> {
public:
  TGrainStiffnessParameter_Delegate(TGrainStiffnessParameter *grainStiffnessParameter);

private:
  TGrainStiffnessParameter_Delegate(const TGrainStiffnessParameter_Delegate &rhs);
  TGrainStiffnessParameter_Delegate &operator=(const TGrainStiffnessParameter_Delegate &rhs);

  TGrainStiffnessParameter *m_grainStiffnessParameter;

  REGISTER_DELEGATE(TGrainStiffnessParameter, TGrainStiffnessParameter_Delegate);
};

class TYoungsModulusNormalDecomp_Delegate
    : public CMaterialComponentTemp_Delegate<
          IDT_VALUETYPE_YOUNGMODULUS_NORM_DECOMP, IDI_VALUETYPE_YOUNGMODULUS_NORM_DECOMP,
          IDS_ET_VALUETYPE_YOUNGMODULUS_NORM_DECOMP, IDS_VALUENAME_YOUNGMODULUS_NORM_DECOMP> {
public:
  TYoungsModulusNormalDecomp_Delegate(TYoungsModulusNormalDecomp *youngsModulusNormalDecomp);

private:
  TYoungsModulusNormalDecomp_Delegate(const TYoungsModulusNormalDecomp_Delegate &rhs);
  TYoungsModulusNormalDecomp_Delegate &operator=(const TYoungsModulusNormalDecomp_Delegate &rhs);

  TYoungsModulusNormalDecomp *m_youngsModulusNormalDecomp;

  REGISTER_DELEGATE(TYoungsModulusNormalDecomp, TYoungsModulusNormalDecomp_Delegate);
};

class TYoungModulusTransDecomp_Delegate
    : public CMaterialComponentTemp_Delegate<
          IDT_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP, IDI_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP,
          IDS_ET_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP, IDS_VALUENAME_YOUNGMODULUS_TRANS_DECOMP> {
public:
  TYoungModulusTransDecomp_Delegate(TYoungModulusTransDecomp *youngModulusTransDecomp);

private:
  TYoungModulusTransDecomp_Delegate(const TYoungModulusTransDecomp_Delegate &rhs);
  TYoungModulusTransDecomp_Delegate &operator=(const TYoungModulusTransDecomp_Delegate &rhs);

  TYoungModulusTransDecomp *m_youngModulusTransDecomp;

  REGISTER_DELEGATE(TYoungModulusTransDecomp, TYoungModulusTransDecomp_Delegate);
};

class TPoissonRatioNormalDecomp_Delegate
    : public CMaterialComponentTemp_Delegate<
          IDT_VALUETYPE_POISSONRATIO_NORM_DECOMP, IDI_VALUETYPE_POISSONRATIO_NORM_DECOMP,
          IDS_ET_VALUETYPE_POISSONRATIO_NORM_DECOMP, IDS_VALUENAME_POISSONRATIO_NORM_DECOMP> {
public:
  TPoissonRatioNormalDecomp_Delegate(TPoissonRatioNormalDecomp *poissonRatioNormalDecomp);

private:
  TPoissonRatioNormalDecomp_Delegate(const TPoissonRatioNormalDecomp_Delegate &rhs);
  TPoissonRatioNormalDecomp_Delegate &operator=(const TPoissonRatioNormalDecomp_Delegate &rhs);

  TPoissonRatioNormalDecomp *m_poissonRatioNormalDecomp;

  REGISTER_DELEGATE(TPoissonRatioNormalDecomp, TPoissonRatioNormalDecomp_Delegate);
};

class TPoissonRatioTransDecomp_Delegate
    : public CMaterialComponentTemp_Delegate<
          IDT_VALUETYPE_POISSONRATIO_TRANS_DECOMP, IDI_VALUETYPE_POISSONRATIO_TRANS_DECOMP,
          IDS_ET_VALUETYPE_POISSONRATIO_TRANS_DECOMP, IDS_VALUENAME_POISSONRATIO_TRANS_DECOMP> {
public:
  TPoissonRatioTransDecomp_Delegate(TPoissonRatioTransDecomp *poissonRatioTransDecomp);

private:
  TPoissonRatioTransDecomp_Delegate(const TPoissonRatioTransDecomp_Delegate &rhs);
  TPoissonRatioTransDecomp_Delegate &operator=(const TPoissonRatioTransDecomp_Delegate &rhs);

  TPoissonRatioTransDecomp *m_poissonRatioTransDecomp;

  REGISTER_DELEGATE(TPoissonRatioTransDecomp, TPoissonRatioTransDecomp_Delegate);
};

class TAnisotropicShearModulus_Delegate
    : public CMaterialComponentTemp_Delegate<
          IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS, IDI_VALUETYPE_ANISOTROPIC_SHEARMODULUS,
          IDS_ET_VALUETYPE_ANISOTROPIC_SHEARMODULUS, IDS_VALUENAME_ANISOTROPIC_SHEARMODULUS> {
public:
  TAnisotropicShearModulus_Delegate(TAnisotropicShearModulus *anisotropicShearModulus);

private:
  TAnisotropicShearModulus_Delegate(const TAnisotropicShearModulus_Delegate &rhs);
  TAnisotropicShearModulus_Delegate &operator=(const TAnisotropicShearModulus_Delegate &rhs);

  TAnisotropicShearModulus *m_anisotropicShearModulus;

  REGISTER_DELEGATE(TAnisotropicShearModulus, TAnisotropicShearModulus_Delegate);
};

class TAnisotropicShearModulusDecomp_Delegate
    : public CMaterialComponentTemp_Delegate<
          IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS_DECOMP, IDI_VALUETYPE_ANISOTROPIC_SHEARMODULUS_DECOMP,
          IDS_ET_VALUETYPE_ANISOTROPIC_SHEARMODULUS_DECOMP, IDS_VALUENAME_ANISOTROPIC_SHEARMODULUS_DECOMP> {
public:
  TAnisotropicShearModulusDecomp_Delegate(TAnisotropicShearModulusDecomp *anisotropicShearModulusDecomp);

private:
  TAnisotropicShearModulusDecomp_Delegate(const TAnisotropicShearModulusDecomp_Delegate &rhs);
  TAnisotropicShearModulusDecomp_Delegate &operator=(const TAnisotropicShearModulusDecomp_Delegate &rhs);

  TAnisotropicShearModulusDecomp *m_anisotropicShearModulusDecomp;

  REGISTER_DELEGATE(TAnisotropicShearModulusDecomp, TAnisotropicShearModulusDecomp_Delegate);
};

class TThomsenEpsilon_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_THOMSEN_EPSILON, IDI_VALUETYPE_THOMSEN_EPSILON,
                                             IDS_ET_VALUETYPE_THOMSEN_EPSILON, IDS_VALUENAME_THOMSEN_EPSILON> {
public:
  TThomsenEpsilon_Delegate(TThomsenEpsilon *thomsenEpsilon);

private:
  TThomsenEpsilon_Delegate(const TThomsenEpsilon_Delegate &);
  TThomsenEpsilon_Delegate &operator=(const TThomsenEpsilon_Delegate &);

  TThomsenEpsilon *m_thomsenEpsilon;

  REGISTER_DELEGATE(TThomsenEpsilon, TThomsenEpsilon_Delegate);
};

class TThomsenGamma_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_THOMSEN_GAMMA, IDI_VALUETYPE_THOMSEN_GAMMA,
                                             IDS_ET_VALUETYPE_THOMSEN_GAMMA, IDS_VALUENAME_THOMSEN_GAMMA> {
public:
  TThomsenGamma_Delegate(TThomsenGamma *thomsenGamma);

private:
  TThomsenGamma_Delegate(const TThomsenGamma_Delegate &);
  TThomsenGamma_Delegate &operator=(const TThomsenGamma_Delegate &);

  TThomsenGamma *m_thomsenGamma;

  REGISTER_DELEGATE(TThomsenGamma, TThomsenGamma_Delegate);
};

class TThomsenDelta_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_THOMSEN_DELTA, IDI_VALUETYPE_THOMSEN_DELTA,
                                             IDS_ET_VALUETYPE_THOMSEN_DELTA, IDS_VALUENAME_THOMSEN_DELTA> {
public:
  TThomsenDelta_Delegate(TThomsenDelta *thomsenDelta);

private:
  TThomsenDelta_Delegate(const TThomsenDelta_Delegate &);
  TThomsenDelta_Delegate &operator=(const TThomsenDelta_Delegate &);

  TThomsenDelta *m_thomsenDelta;

  REGISTER_DELEGATE(TThomsenDelta, TThomsenDelta_Delegate);
};

class TInitialFractureApertureHigh_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_V0_HI, IDI_VALUETYPE_V0_HI, IDS_ET_VALUETYPE_V0_HI,
                                             IDS_VALUENAME_V0_HI> {
public:
  TInitialFractureApertureHigh_Delegate(TInitialFractureApertureHigh *initialFractureApertureHigh);

private:
  TInitialFractureApertureHigh_Delegate(const TInitialFractureApertureHigh_Delegate &);
  TInitialFractureApertureHigh_Delegate &operator=(const TInitialFractureApertureHigh_Delegate &);

  TInitialFractureApertureHigh *m_initialFractureApertureHigh;

  REGISTER_DELEGATE(TInitialFractureApertureHigh, TInitialFractureApertureHigh_Delegate);
};

class TInitialFractureApertureInter_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_V0_ME, IDI_VALUETYPE_V0_ME, IDS_ET_VALUETYPE_V0_ME,
                                             IDS_VALUENAME_V0_ME> {
public:
  TInitialFractureApertureInter_Delegate(TInitialFractureApertureInter *initialFractureApertureInter);

private:
  TInitialFractureApertureInter_Delegate(const TInitialFractureApertureInter_Delegate &);
  TInitialFractureApertureInter_Delegate &operator=(const TInitialFractureApertureInter_Delegate &);

  TInitialFractureApertureInter *m_initialFractureApertureInter;

  REGISTER_DELEGATE(TInitialFractureApertureInter, TInitialFractureApertureInter_Delegate);
};

class TInitialFractureApertureLow_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_V0_LO, IDI_VALUETYPE_V0_LO, IDS_ET_VALUETYPE_V0_LO,
                                             IDS_VALUENAME_V0_LO> {
public:
  TInitialFractureApertureLow_Delegate(TInitialFractureApertureLow *initialFractureApertureLow);

private:
  TInitialFractureApertureLow_Delegate(const TInitialFractureApertureLow_Delegate &);
  TInitialFractureApertureLow_Delegate &operator=(const TInitialFractureApertureLow_Delegate &);

  TInitialFractureApertureLow *m_initialFractureApertureLow;

  REGISTER_DELEGATE(TInitialFractureApertureLow, TInitialFractureApertureLow_Delegate);
};

class TReferenceAperture_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_REFERENCE_APERTURE, IDI_VALUETYPE_REFERENCE_APERTURE,
                                             IDS_ET_VALUETYPE_REFERENCE_APERTURE, IDS_VALUENAME_REFERENCE_APERTURE> {
public:
  TReferenceAperture_Delegate(TReferenceAperture *referenceAperture);

private:
  TReferenceAperture_Delegate(const TReferenceAperture_Delegate &);
  TReferenceAperture_Delegate &operator=(const TReferenceAperture_Delegate &);

  TReferenceAperture *m_referenceAperture;

  REGISTER_DELEGATE(TReferenceAperture, TReferenceAperture_Delegate);
};

class TDynamicUniaxialStiffness_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_DYNUNISTIFFNESS, IDI_VALUETYPE_DYNUNISTIFFNESS,
                                             IDS_ET_DYNUNISTIFFNESS, IDS_VALUENAME_DYNUNISTIFFNESS> {
public:
  TDynamicUniaxialStiffness_Delegate(TDynamicUniaxialStiffness *dynamicUniaxialStiffness);

private:
  TDynamicUniaxialStiffness_Delegate(const TDynamicUniaxialStiffness_Delegate &);
  TDynamicUniaxialStiffness_Delegate &operator=(const TDynamicUniaxialStiffness_Delegate &);

  TDynamicUniaxialStiffness *m_dynamicUniaxialStiffness;

  REGISTER_DELEGATE(TDynamicUniaxialStiffness, TDynamicUniaxialStiffness_Delegate);
};

class TDynamicShearStiffness_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_DYNSHEARSTIFFNESS, IDI_VALUETYPE_DYNSHEARSTIFFNESS,
                                             IDS_ET_DYNSHEARSTIFFNESS, IDS_VALUENAME_DYNSHEARSTIFFNESS> {
public:
  TDynamicShearStiffness_Delegate(TDynamicShearStiffness *dynamicShearStiffness);

private:
  TDynamicShearStiffness_Delegate(const TDynamicShearStiffness_Delegate &);
  TDynamicShearStiffness_Delegate &operator=(const TDynamicShearStiffness_Delegate &);

  TDynamicShearStiffness *m_dynamicShearStiffness;

  REGISTER_DELEGATE(TDynamicShearStiffness, TDynamicShearStiffness_Delegate);
};

class TNormalStress_Delegate : public CSingleComponentTemp_Delegate<IDT_VALUETYPE_NRMSTRESS, IDI_VALUETYPE_NRMSTRESS,
                                                                    IDS_ET_NRMSTRESS, IDS_VALUENAME_NRMSTRESS> {
public:
  TNormalStress_Delegate(TNormalStress *dummy);

private:
  TNormalStress_Delegate(const TNormalStress_Delegate &rhs);
  TNormalStress_Delegate &operator=(const TNormalStress_Delegate &rhs);

  TNormalStress *m_dummy;

  REGISTER_DELEGATE(TNormalStress, TNormalStress_Delegate);
};

class TMeanStress_Delegate : public CSingleComponentTemp_Delegate<IDT_VALUETYPE_MEANSTRESS, IDI_VALUETYPE_MEANSTRESS,
                                                                  IDS_ET_MEANSTRESS, IDS_VALUENAME_MEANSTRESS> {
public:
  TMeanStress_Delegate(TMeanStress *dummy);

private:
  TMeanStress_Delegate(const TMeanStress_Delegate &rhs);
  TMeanStress_Delegate &operator=(const TMeanStress_Delegate &rhs);

  TMeanStress *m_dummy;

  REGISTER_DELEGATE(TMeanStress, TMeanStress_Delegate);
};

class TTimeStrain_Delegate : public CSingleComponentTemp_Delegate<IDT_VALUETYPE_TIMESTRAIN, IDI_VALUETYPE_TIMESTRAIN,
                                                                  IDS_ET_TIMESTRAIN, IDS_VALUENAME_TIMESTRAIN> {
public:
  TTimeStrain_Delegate(TTimeStrain *dummy);

private:
  TTimeStrain_Delegate(const TTimeStrain_Delegate &rhs);
  TTimeStrain_Delegate &operator=(const TTimeStrain_Delegate &rhs);

  TTimeStrain *m_dummy;

  REGISTER_DELEGATE(TTimeStrain, TTimeStrain_Delegate);
};

class TDeltaV_Delegate : public CSingleComponentTemp_Delegate<IDT_VALUETYPE_DELTAV, IDI_VALUETYPE_DELTAV, IDS_ET_DELTAV,
                                                              IDS_VALUENAME_DELTAV> {
public:
  TDeltaV_Delegate(TDeltaV *dummy);

private:
  TDeltaV_Delegate(const TDeltaV_Delegate &rhs);
  TDeltaV_Delegate &operator=(const TDeltaV_Delegate &rhs);

  TDeltaV *m_dummy;

  REGISTER_DELEGATE(TDeltaV, TDeltaV_Delegate);
};

class TDeltaT_Delegate : public CSingleComponentTemp_Delegate<IDT_VALUETYPE_DELTAT, IDI_VALUETYPE_DELTAT, IDS_ET_DELTAT,
                                                              IDS_VALUENAME_DELTAT> {
public:
  TDeltaT_Delegate(TDeltaT *dummy);

private:
  TDeltaT_Delegate(const TDeltaT_Delegate &rhs);
  TDeltaT_Delegate &operator=(const TDeltaT_Delegate &rhs);

  TDeltaT *m_dummy;

  REGISTER_DELEGATE(TDeltaT, TDeltaT_Delegate);
};

class TDisplacementZ_Delegate
    : public CSingleComponentTemp_Delegate<IDT_VALUETYPE_DISPLACEMENTZ, IDI_VALUETYPE_DISPLACEMENTZ,
                                           IDS_ET_DISPLACEMENTZ, IDS_VALUENAME_DISPLACEMENTZ> {
public:
  TDisplacementZ_Delegate(TDisplacementZ *dummy);

private:
  TDisplacementZ_Delegate(const TDisplacementZ_Delegate &rhs);
  TDisplacementZ_Delegate &operator=(const TDisplacementZ_Delegate &rhs);

  TDisplacementZ *m_dummy;

  REGISTER_DELEGATE(TDisplacementZ, TDisplacementZ_Delegate);
};

class TDummyMaterialParameter_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_DUMMY, IDI_VALUETYPE_DUMMY, IDS_ET_DUMMY,
                                             IDS_VALUENAME_DUMMY> {
public:
  TDummyMaterialParameter_Delegate(TDummyMaterialParameter *dummy);

private:
  TDummyMaterialParameter_Delegate(const TDummyMaterialParameter_Delegate &rhs);
  TDummyMaterialParameter_Delegate &operator=(const TDummyMaterialParameter_Delegate &rhs);

  TDummyMaterialParameter *m_dummy;

  REGISTER_DELEGATE(TDummyMaterialParameter, TDummyMaterialParameter_Delegate);
};

class TBoundaryKradMaterialParameter_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_KRAD, IDI_VALUETYPE_KRAD, IDS_ET_KRAD, IDS_VALUENAME_KRAD> {
public:
  TBoundaryKradMaterialParameter_Delegate(TBoundaryKradMaterialParameter *dummy);

private:
  TBoundaryKradMaterialParameter_Delegate(const TBoundaryKradMaterialParameter_Delegate &rhs);
  TBoundaryKradMaterialParameter_Delegate &operator=(const TBoundaryKradMaterialParameter_Delegate &rhs);

  TBoundaryKradMaterialParameter *m_dummy;

  REGISTER_DELEGATE(TBoundaryKradMaterialParameter, TBoundaryKradMaterialParameter_Delegate);
};

class TBoundaryKtanMaterialParameter_Delegate
    : public CMaterialComponentTemp_Delegate<IDT_VALUETYPE_KTAN, IDI_VALUETYPE_KTAN, IDS_ET_KTAN, IDS_VALUENAME_KTAN> {
public:
  TBoundaryKtanMaterialParameter_Delegate(TBoundaryKtanMaterialParameter *dummy);

private:
  TBoundaryKtanMaterialParameter_Delegate(const TBoundaryKtanMaterialParameter_Delegate &rhs);
  TBoundaryKtanMaterialParameter_Delegate &operator=(const TBoundaryKtanMaterialParameter_Delegate &rhs);

  TBoundaryKtanMaterialParameter *m_dummy;

  REGISTER_DELEGATE(TBoundaryKtanMaterialParameter, TBoundaryKtanMaterialParameter_Delegate);
};

#endif // _ValueTypeFactory_Delegate_h_
