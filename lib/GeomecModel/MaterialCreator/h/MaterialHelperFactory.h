#ifndef _MATERIAL_HELPER_FACTORY_H_
#define _MATERIAL_HELPER_FACTORY_H_


#include "mlMatParam.h"
#include "mlMaterial.h"

#include <vector>

class IPQPlotter;


class CMaterialHelperFactory
{
public:

  enum MaterialCheckStrategy
  {
  MAT_NO_CHECK_STRATEGY = 0,

  MAT_Anisotropy,

  _MAT_CHECK_STRATEGY_SIZE
  };

  enum MaterialPQPlotter
  {
  MAT_NO_PQ_PLOTTER = 0,

  MAT_PQ_CamClay,
  MAT_PQ_CamClayCreep,
  MAT_PQ_DualCap,
  MAT_PQ_ModifiedMohrCoulomb,
  MAT_PQ_MohrCoulomb,

  _MAT_PQ_PLOTTER_SIZE
  };

  enum MatParamCheckStrategy
  {
  MP_NO_CHECK_STRATEGY = 0,
  MP_GT_0,
  MP_GE_0,
  MP_GE_1,
  MP_GE_1Emin6,
  MP_GT_0_LE_1,
  MP_GT_0_LE_10000,
  MP_GE_0_LT_5Emin1,
  MP_GE_0_LT_1,
  MP_GE_0_LT_70,
  MP_GE_0_LE_90,
  MP_GE_0_LE_360,
  MP_GE_min360_LE_360,
  MP_GE_0_LE_1Emin3,
  MP_GE_0_LE_1Emin1,
  MP_GE_0_LE_2Emin1,
  MP_GE_0_LE_1,
  MP_GE_1Emin3_LE_1E3,
  MP_GE_1Emin1_LE_5,
  MP_GE_1Emin1_LE_2,
  MP_GE_0_LE_10,
  MP_GE_1_LE_10,
  MP_GE_0_LE_100,
  MP_GE_1_LE_100,
  MP_GE_0_LE_1E3,
  MP_GE_0_LE_1E6,
  MP_GE_10_LE_1E6,
  MP_AnisotropyThomsenDelta,
  MP_AnisotropyThomsenEpsilon,
  MP_AnisotropyThomsenGamma,
  MP_AnisotropyVp,
  MP_AnisotropyVs,
  MP_CamClayCMP,
  MP_CamClayCreepCMP,
  MP_DualCapPreconsolidation,
  MP_DualCapSecondaryPrecon,
  MP_DualCapHardening,
  MP_DualCapSecHardening,
  MP_LinearBulkStiff,
  MP_LinearDynShear,
  MP_LinearDynUni,
  MP_LinearLatRatioMax,
  MP_LinearLatRatioMin,
  MP_LinearShearStiff,
  MP_LinearVelocityP,
  MP_LinearVelocityS,
  MP_MCHardEquivalentPlasticStrain1,
  MP_MCHardEquivalentPlasticStrain2,
  MP_MCHardEquivalentPlasticStrain3,
  MP_MohrCoulombDilatation,
  MP_MohrCoulombFriction,

  _MP_CHECK_STRATEGY_SIZE
  };

  enum MatParamSetStrategy
  {
  MP_NO_SET_STRATEGY = 0,
  MP_SET_AnisotropyPoissonNorm,
  MP_SET_AnisotropyPoissonTrans,
  MP_SET_AnisotropyShearStiff,
  MP_SET_AnisotropyThomsenDelta,
  MP_SET_AnisotropyThomsenEpsilon,
  MP_SET_AnisotropyThomsenGamma,
  MP_SET_AnisotropyVp,
  MP_SET_AnisotropyVs,
  MP_SET_AnisotropyYoungNorm,
  MP_SET_AnisotropyYoungTrans,
  MP_SET_CamClayBulkStiff,
  MP_SET_CamClayCapShap,
  MP_SET_CamClayCME,
  MP_SET_CamClayCMP,
  MP_SET_CamClayCreepBulkStiff,
  MP_SET_CamClayCreepCapShap,
  MP_SET_CamClayCreepCME,
  MP_SET_CamClayCreepCMP,
  MP_SET_CamClayCreepDynShear,
  MP_SET_CamClayCreepDynUni,
  MP_SET_CamClayCreepFriction,
  MP_SET_CamClayCreepHardening,
  MP_SET_CamClayCreepPoisson,
  MP_SET_CamClayCreepPorosity,
  MP_SET_CamClayCreepPreconsolidation,
  MP_SET_CamClayCreepShearStiff,
  MP_SET_CamClayCreepVelocityP,
  MP_SET_CamClayCreepVelocityS,
  MP_SET_CamClayCreepYoung,
  MP_SET_CamClayDynShear,
  MP_SET_CamClayDynUni,
  MP_SET_CamClayFriction,
  MP_SET_CamClayHardening,
  MP_SET_CamClayPoisson,
  MP_SET_CamClayPorosity,
  MP_SET_CamClayPreconsolidation,
  MP_SET_CamClayShearStiff,
  MP_SET_CamClayVelocityP,
  MP_SET_CamClayVelocityS,
  MP_SET_CamClayYoung,
  MP_SET_DualCapElasticHardening,
  MP_SET_DualCapPoisson,
  MP_SET_DualCapPorosity,
  MP_SET_DualCapPreconsolidation,
  MP_SET_DualCapYoung,
  MP_SET_LinearBulkStiff,
  MP_SET_LinearCME,
  MP_SET_LinearDynShear,
  MP_SET_LinearDynUni,
  MP_SET_LinearPoisson,
  MP_SET_LinearRhoB,
  MP_SET_LinearShearStiff,
  MP_SET_LinearVelocityP,
  MP_SET_LinearVelocityS,
  MP_SET_LinearYoung,
  MP_SET_UndrainedFluidBulkModulus,
  MP_SET_UndrainedFluidThermalExpansionCoefficient,
  MP_SET_UndrainedGrainStiffness,
  MP_SET_UndrainedPoisson,
  MP_SET_UndrainedPorosity,
  MP_SET_UndrainedUndrainedPoisson,
  MP_SET_UndrainedUndrainedThermalExpansionCoefficient,
  MP_SET_UndrainedUndrainedYoung,
  MP_SET_UndrainedVolumetricThermalExpansionCoefficient,
  MP_SET_UndrainedYoung,

  _MP_SET_STRATEGY_SIZE
  };

  enum MatParamUnitType
  {
  UT_NONE = 0,
  UT_STRESS,
  UT_STRESS_GRADIENT,
  UT_DENSITY,
  UT_DEPTH,
  UT_FORCE,
  UT_VELOCITY,
  UT_ANGLE,
  UT_PER_STRESS,
  UT_PER_TIME,
  UT_TEMPER,
  UT_KELVIN_TEMPER,
  UT_PER_TEMPER,
  UT_PER_DISTANCE,
  UT_THERM_CONDUCT,
  UT_THERM_CAPACITY,
  UT_PERMEA,
  UT_VISCOSITY,

  _MP_UNIT_TYPE_SIZE
  };


  static const CMaterialHelperFactory *Instance();

  ml::CMaterial::CCreator* getMatCreator(int material) const;
  const ml::CMaterial::CCheckStrategy* getMatCheckStrategy(enum MaterialCheckStrategy strategy) const;
  IPQPlotter* getMatPQPlotter(enum MaterialPQPlotter plotter) const;

  const ml::CMatParam::CCheckStrategy* getMPCheckStrategy(enum MatParamCheckStrategy strategy) const;
  const ml::CMatParam::CSetStrategy* getMPSetStrategy(enum MatParamSetStrategy strategy) const;
  const ml::CMatParam::CUnitType& getMPUnitType(enum MatParamUnitType unitType) const;

private:
  CMaterialHelperFactory();
  CMaterialHelperFactory(const CMaterialHelperFactory&);
  CMaterialHelperFactory& operator=(const CMaterialHelperFactory&);

  void createMatCreators();
  void createMatCheckStrategies();
  void createMatPQPlotters();

  void createMPCheckStrategies();
  void createMPSetStrategies();
  void createMPUnitTypes();

  std::vector<ml::CMaterial::CCreator *> m_MatCreator;
  std::vector<const ml::CMaterial::CCheckStrategy *> m_MatCheckStrategy;
  std::vector<IPQPlotter *> m_MatPQPlotter;
  std::vector<const ml::CMatParam::CCheckStrategy *> m_MPCheckStrategy;
  std::vector<const ml::CMatParam::CSetStrategy *> m_MPSetStrategy;
  std::vector<const ml::CMatParam::CUnitType *> m_MPUnitType;

};




#endif