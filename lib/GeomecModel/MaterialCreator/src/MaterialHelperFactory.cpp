#include "MaterialHelperFactory.h"
#include "mlMatParamDomain.h"

#include "MaterialModelAnisotropy.h"
#include "MaterialModelCamClay.h"
#include "MaterialModelCamClayCreep.h"
#include "MaterialModelDualCap.h"
#include "MaterialModelLinear.h"
#include "MaterialModelMCHardeningCheckStrategies.h"
#include "MaterialModelModifiedMohrCoulomb.h"
#include "MaterialModelMohrCoulomb.h"
#include "MaterialModelUndrained.h"

#include "MaterialAnisotropicCamClay.h"
#include "MaterialCreep.h"
#include "MaterialDualCapLinEla.h"
#include "MaterialFractAnisotropy.h"
#include "MaterialFractureAperture.h"
#include "MaterialFractureAperture2.h"
#include "MaterialRigidity.h"
#include "MaterialUpscaledAnisotropy.h"
#include "materialmccohesionhard1.h"
#include "materialmccohesionhard2.h"
#include "materialmccohesionhard3.h"
#include "materialmcfrictionhard1.h"
#include "materialmcfrictionhard2.h"
#include "materialmcfrictionhard3.h"

#include "MaterialUnitTypes.h"

#include "Materials.h"

#include <cassert>

const CMaterialHelperFactory *CMaterialHelperFactory::Instance() {
  static CMaterialHelperFactory *instance = new CMaterialHelperFactory();
  assert(instance != 0);
  return instance;
}

CMaterialHelperFactory::CMaterialHelperFactory() {
  createMatCreators();
  createMatCheckStrategies();
  createMatPQPlotters();
  createMPCheckStrategies();
  createMPSetStrategies();
  createMPUnitTypes();
}

void CMaterialHelperFactory::createMatCreators() {

  m_MatCreator.push_back(new CMaterialLinearCreator);
  assert(m_MatCreator.size() - 1 == MM_LINEAR);

  m_MatCreator.push_back(new CMaterialCamClayCreator);
  assert(m_MatCreator.size() - 1 == MM_CAMCLAY);

  m_MatCreator.push_back(new CMaterialMohrCoulombCreator);
  assert(m_MatCreator.size() - 1 == MM_MOHRCOULOMB);

  m_MatCreator.push_back(new CMaterialModifiedMohrCoulombCreator);
  assert(m_MatCreator.size() - 1 == MM_MODIFIEDMOHRCOULOMB);

  m_MatCreator.push_back(new CMaterialCreepCreator);
  assert(m_MatCreator.size() - 1 == MM_CREEP);

  m_MatCreator.push_back(new CMaterialUndrainedCreator);
  assert(m_MatCreator.size() - 1 == MM_UNDRAINED);

  m_MatCreator.push_back(new CMaterialModelMCCohesionHard1Creator);
  assert(m_MatCreator.size() - 1 == MM_MC_COHESION_HARD1);

  m_MatCreator.push_back(new CMaterialModelMCCohesionHard2Creator);
  assert(m_MatCreator.size() - 1 == MM_MC_COHESION_HARD2);

  m_MatCreator.push_back(new CMaterialModelMCCohesionHard3Creator);
  assert(m_MatCreator.size() - 1 == MM_MC_COHESION_HARD3);

  m_MatCreator.push_back(new CMaterialModelMCFrictionHard1Creator);
  assert(m_MatCreator.size() - 1 == MM_MC_FRICTION_HARD1);

  m_MatCreator.push_back(new CMaterialModelMCFrictionHard2Creator);
  assert(m_MatCreator.size() - 1 == MM_MC_FRICTION_HARD2);

  m_MatCreator.push_back(new CMaterialModelMCFrictionHard3Creator);
  assert(m_MatCreator.size() - 1 == MM_MC_FRICTION_HARD3);

  m_MatCreator.push_back(new CMaterialLinearCreator); // not used anymore, Linear as compat in case we read old models
  assert(m_MatCreator.size() - 1 == 12);

  m_MatCreator.push_back(new CMaterialRigidityCreator);
  assert(m_MatCreator.size() - 1 == MM_RIGIDITY);

  m_MatCreator.push_back(new CMaterialLinearCreator); // not used anymore
  assert(m_MatCreator.size() - 1 == MM_DUALCAP);

  m_MatCreator.push_back(new CMaterialDualCapLinElaCreator);
  assert(m_MatCreator.size() - 1 == MM_DUALCAP_LINELA);

  m_MatCreator.push_back(new CMaterialFractureAnisotropyCreator);
  assert(m_MatCreator.size() - 1 == MM_FRACTURE_ANISOTROPY);

  m_MatCreator.push_back(new CMaterialUpscaledAnisotropyCreator);
  assert(m_MatCreator.size() - 1 == MM_UPSCALED_ANISOTROPY);

  m_MatCreator.push_back(new CMaterialFractureApertureCreator);
  assert(m_MatCreator.size() - 1 == MM_FRACTURE_APERTURE);

  m_MatCreator.push_back(new CMaterialAnisotropicCamClayCreator);
  assert(m_MatCreator.size() - 1 == MM_ANISOTROPIC_CAMCLAY);

  m_MatCreator.push_back(new CMaterialFractureAperture2Creator);
  assert(m_MatCreator.size() - 1 == MM_FRACTURE_APERTURE2);

  m_MatCreator.push_back(new CMaterialCamClayCreepCreator);
  assert(m_MatCreator.size() - 1 == MM_CAMCLAY_CREEP);
}

void CMaterialHelperFactory::createMatCheckStrategies() {
  m_MatCheckStrategy.reserve(_MAT_CHECK_STRATEGY_SIZE);

  m_MatCheckStrategy.push_back(0);
  assert(m_MatCheckStrategy.size() - 1 == MAT_NO_CHECK_STRATEGY);

  m_MatCheckStrategy.push_back(new CMaterialAnisotropyCheckStrategy);
  assert(m_MatCheckStrategy.size() - 1 == MAT_Anisotropy);

  assert(m_MatCheckStrategy.size() == _MAT_CHECK_STRATEGY_SIZE);
}

void CMaterialHelperFactory::createMatPQPlotters() {
  m_MatCheckStrategy.reserve(_MAT_PQ_PLOTTER_SIZE);

  m_MatPQPlotter.push_back(0);
  assert(m_MatPQPlotter.size() - 1 == MAT_NO_PQ_PLOTTER);

  m_MatPQPlotter.push_back(new CMaterialCamClayCreator::CPQPlotter);
  assert(m_MatPQPlotter.size() - 1 == MAT_PQ_CamClay);

  m_MatPQPlotter.push_back(new CMaterialCamClayCreepCreator::CPQPlotter);
  assert(m_MatPQPlotter.size() - 1 == MAT_PQ_CamClayCreep);

  m_MatPQPlotter.push_back(new CMaterialDualCapCreator::CPQPlotter);
  assert(m_MatPQPlotter.size() - 1 == MAT_PQ_DualCap);

  m_MatPQPlotter.push_back(new CMaterialModifiedMohrCoulombCreator::CPQPlotter);
  assert(m_MatPQPlotter.size() - 1 == MAT_PQ_ModifiedMohrCoulomb);

  m_MatPQPlotter.push_back(new CMaterialMohrCoulombCreator::CPQPlotter);
  assert(m_MatPQPlotter.size() - 1 == MAT_PQ_MohrCoulomb);

  assert(m_MatPQPlotter.size() == _MAT_PQ_PLOTTER_SIZE);
}

void CMaterialHelperFactory::createMPCheckStrategies() {
  m_MPCheckStrategy.reserve(_MP_CHECK_STRATEGY_SIZE);

  m_MPCheckStrategy.push_back(0);
  assert(m_MPCheckStrategy.size() - 1 == MP_NO_CHECK_STRATEGY);

  m_MPCheckStrategy.push_back(new MP_GT(0));
  assert(m_MPCheckStrategy.size() - 1 == MP_GT_0);

  m_MPCheckStrategy.push_back(new MP_GE(0));
  assert(m_MPCheckStrategy.size() - 1 == MP_GE_0);

  m_MPCheckStrategy.push_back(new MP_GE(1));
  assert(m_MPCheckStrategy.size() - 1 == MP_GE_1);

  m_MPCheckStrategy.push_back(new MP_GE(1E-6));
  assert(m_MPCheckStrategy.size() - 1 == MP_GE_1Emin6);

  m_MPCheckStrategy.push_back(new MP_GT_LE(0, 1));
  assert(m_MPCheckStrategy.size() - 1 == MP_GT_0_LE_1);

  m_MPCheckStrategy.push_back(new MP_GT_LE(0, 10000));
  assert(m_MPCheckStrategy.size() - 1 == MP_GT_0_LE_10000);

  m_MPCheckStrategy.push_back(new MP_GE_LT(0, .5));
  assert(m_MPCheckStrategy.size() - 1 == MP_GE_0_LT_5Emin1);

  m_MPCheckStrategy.push_back(new MP_GE_LT(0, 1));
  assert(m_MPCheckStrategy.size() - 1 == MP_GE_0_LT_1);

  m_MPCheckStrategy.push_back(new MP_GE_LT(0, 70));
  assert(m_MPCheckStrategy.size() - 1 == MP_GE_0_LT_70);

  m_MPCheckStrategy.push_back(new MP_GE_LE(0, 90));
  assert(m_MPCheckStrategy.size() - 1 == MP_GE_0_LE_90);

  m_MPCheckStrategy.push_back(new MP_GE_LE(0, 360));
  assert(m_MPCheckStrategy.size() - 1 == MP_GE_0_LE_360);

  m_MPCheckStrategy.push_back(new MP_GE_LE(-360, 360));
  assert(m_MPCheckStrategy.size() - 1 == MP_GE_min360_LE_360);

  m_MPCheckStrategy.push_back(new MP_GE_LE(0, 1E-3));
  assert(m_MPCheckStrategy.size() - 1 == MP_GE_0_LE_1Emin3);

  m_MPCheckStrategy.push_back(new MP_GE_LE(0, .1));
  assert(m_MPCheckStrategy.size() - 1 == MP_GE_0_LE_1Emin1);

  m_MPCheckStrategy.push_back(new MP_GE_LE(0, .2));
  assert(m_MPCheckStrategy.size() - 1 == MP_GE_0_LE_2Emin1);

  m_MPCheckStrategy.push_back(new MP_GE_LE(0, 1));
  assert(m_MPCheckStrategy.size() - 1 == MP_GE_0_LE_1);

  m_MPCheckStrategy.push_back(new MP_GE_LE(1E-3, 1E3));
  assert(m_MPCheckStrategy.size() - 1 == MP_GE_1Emin3_LE_1E3);

  m_MPCheckStrategy.push_back(new MP_GE_LE(.1, 5));
  assert(m_MPCheckStrategy.size() - 1 == MP_GE_1Emin1_LE_5);

  m_MPCheckStrategy.push_back(new MP_GE_LE(.1, 2));
  assert(m_MPCheckStrategy.size() - 1 == MP_GE_1Emin1_LE_2);

  m_MPCheckStrategy.push_back(new MP_GE_LE(0, 10));
  assert(m_MPCheckStrategy.size() - 1 == MP_GE_0_LE_10);

  m_MPCheckStrategy.push_back(new MP_GE_LE(1, 10));
  assert(m_MPCheckStrategy.size() - 1 == MP_GE_1_LE_10);

  m_MPCheckStrategy.push_back(new MP_GE_LE(0, 100));
  assert(m_MPCheckStrategy.size() - 1 == MP_GE_0_LE_100);

  m_MPCheckStrategy.push_back(new MP_GE_LE(1, 100));
  assert(m_MPCheckStrategy.size() - 1 == MP_GE_1_LE_100);

  m_MPCheckStrategy.push_back(new MP_GE_LE(0, 1E3));
  assert(m_MPCheckStrategy.size() - 1 == MP_GE_0_LE_1E3);

  m_MPCheckStrategy.push_back(new MP_GE_LE(0, 1E6));
  assert(m_MPCheckStrategy.size() - 1 == MP_GE_0_LE_1E6);

  m_MPCheckStrategy.push_back(new MP_GE_LE(10, 1E6));
  assert(m_MPCheckStrategy.size() - 1 == MP_GE_10_LE_1E6);

  m_MPCheckStrategy.push_back(new CMaterialAnisotropyThomsenDeltaCheckStrategy());
  assert(m_MPCheckStrategy.size() - 1 == MP_AnisotropyThomsenDelta);

  m_MPCheckStrategy.push_back(new CMaterialAnisotropyThomsenEpsilonCheckStrategy());
  assert(m_MPCheckStrategy.size() - 1 == MP_AnisotropyThomsenEpsilon);

  m_MPCheckStrategy.push_back(new CMaterialAnisotropyThomsenGammaCheckStrategy());
  assert(m_MPCheckStrategy.size() - 1 == MP_AnisotropyThomsenGamma);

  m_MPCheckStrategy.push_back(new CMaterialAnisotropyVpCheckStrategy());
  assert(m_MPCheckStrategy.size() - 1 == MP_AnisotropyVp);

  m_MPCheckStrategy.push_back(new CMaterialAnisotropyVsCheckStrategy());
  assert(m_MPCheckStrategy.size() - 1 == MP_AnisotropyVs);

  m_MPCheckStrategy.push_back(new CMaterialCamClayCMPCheckStrategy());
  assert(m_MPCheckStrategy.size() - 1 == MP_CamClayCMP);

  m_MPCheckStrategy.push_back(new CMaterialCamClayCreepCMPCheckStrategy());
  assert(m_MPCheckStrategy.size() - 1 == MP_CamClayCreepCMP);

  m_MPCheckStrategy.push_back(new CMaterialDualCapPreconsolidationCheckStrategy());
  assert(m_MPCheckStrategy.size() - 1 == MP_DualCapPreconsolidation);

  m_MPCheckStrategy.push_back(new CMaterialDualCapSecondaryPreconCheckStrategy());
  assert(m_MPCheckStrategy.size() - 1 == MP_DualCapSecondaryPrecon);

  m_MPCheckStrategy.push_back(new CMaterialDualCapLinElaHardeningCheckStrategy());
  assert(m_MPCheckStrategy.size() - 1 == MP_DualCapHardening);

  m_MPCheckStrategy.push_back(new CMaterialDualCapLinElaSecondaryHardeningCheckStrategy());
  assert(m_MPCheckStrategy.size() - 1 == MP_DualCapSecHardening);

  m_MPCheckStrategy.push_back(new CMaterialLinearBulkStiffCheckStrategy());
  assert(m_MPCheckStrategy.size() - 1 == MP_LinearBulkStiff);

  m_MPCheckStrategy.push_back(new CMaterialLinearDynShearCheckStrategy());
  assert(m_MPCheckStrategy.size() - 1 == MP_LinearDynShear);

  m_MPCheckStrategy.push_back(new CMaterialLinearDynUniCheckStrategy());
  assert(m_MPCheckStrategy.size() - 1 == MP_LinearDynUni);

  m_MPCheckStrategy.push_back(new CMaterialLinearLatRatioMaxCheckStrategy());
  assert(m_MPCheckStrategy.size() - 1 == MP_LinearLatRatioMax);

  m_MPCheckStrategy.push_back(new CMaterialLinearLatRatioMinCheckStrategy());
  assert(m_MPCheckStrategy.size() - 1 == MP_LinearLatRatioMin);

  m_MPCheckStrategy.push_back(new CMaterialLinearShearStiffCheckStrategy());
  assert(m_MPCheckStrategy.size() - 1 == MP_LinearShearStiff);

  m_MPCheckStrategy.push_back(new CMaterialLinearVelocityPCheckStrategy());
  assert(m_MPCheckStrategy.size() - 1 == MP_LinearVelocityP);

  m_MPCheckStrategy.push_back(new CMaterialLinearVelocitySCheckStrategy());
  assert(m_MPCheckStrategy.size() - 1 == MP_LinearVelocityS);

  m_MPCheckStrategy.push_back(new CMaterialMCHardEquivalentPlasticStrain1CheckStrategy());
  assert(m_MPCheckStrategy.size() - 1 == MP_MCHardEquivalentPlasticStrain1);

  m_MPCheckStrategy.push_back(new CMaterialMCHardEquivalentPlasticStrain2CheckStrategy());
  assert(m_MPCheckStrategy.size() - 1 == MP_MCHardEquivalentPlasticStrain2);

  m_MPCheckStrategy.push_back(new CMaterialMCHardEquivalentPlasticStrain3CheckStrategy());
  assert(m_MPCheckStrategy.size() - 1 == MP_MCHardEquivalentPlasticStrain3);

  m_MPCheckStrategy.push_back(new CMaterialMohrCoulombDilatationCheckStrategy());
  assert(m_MPCheckStrategy.size() - 1 == MP_MohrCoulombDilatation);

  m_MPCheckStrategy.push_back(new CMaterialMohrCoulombFrictionCheckStrategy());
  assert(m_MPCheckStrategy.size() - 1 == MP_MohrCoulombFriction);

  assert(m_MPCheckStrategy.size() == _MP_CHECK_STRATEGY_SIZE);
}

void CMaterialHelperFactory::createMPSetStrategies() {
  m_MPSetStrategy.reserve(_MP_SET_STRATEGY_SIZE);

  m_MPSetStrategy.push_back(0);
  assert(m_MPSetStrategy.size() - 1 == MP_NO_SET_STRATEGY);

  m_MPSetStrategy.push_back(new CMaterialAnisotropyPoissonNormSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_AnisotropyPoissonNorm);

  m_MPSetStrategy.push_back(new CMaterialAnisotropyPoissonTransSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_AnisotropyPoissonTrans);

  m_MPSetStrategy.push_back(new CMaterialAnisotropyShearStiffSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_AnisotropyShearStiff);

  m_MPSetStrategy.push_back(new CMaterialAnisotropyThomsenDeltaSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_AnisotropyThomsenDelta);

  m_MPSetStrategy.push_back(new CMaterialAnisotropyThomsenEpsilonSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_AnisotropyThomsenEpsilon);

  m_MPSetStrategy.push_back(new CMaterialAnisotropyThomsenGammaSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_AnisotropyThomsenGamma);

  m_MPSetStrategy.push_back(new CMaterialAnisotropyVpSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_AnisotropyVp);

  m_MPSetStrategy.push_back(new CMaterialAnisotropyVsSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_AnisotropyVs);

  m_MPSetStrategy.push_back(new CMaterialAnisotropyYoungNormSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_AnisotropyYoungNorm);

  m_MPSetStrategy.push_back(new CMaterialAnisotropyYoungTransSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_AnisotropyYoungTrans);

  m_MPSetStrategy.push_back(new CMaterialCamClayBulkStiffSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_CamClayBulkStiff);

  m_MPSetStrategy.push_back(new CMaterialCamClayCapShapSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_CamClayCapShap);

  m_MPSetStrategy.push_back(new CMaterialCamClayCMESetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_CamClayCME);

  m_MPSetStrategy.push_back(new CMaterialCamClayCMPSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_CamClayCMP);

  m_MPSetStrategy.push_back(new CMaterialCamClayCreepBulkStiffSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_CamClayCreepBulkStiff);

  m_MPSetStrategy.push_back(new CMaterialCamClayCreepCapShapSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_CamClayCreepCapShap);

  m_MPSetStrategy.push_back(new CMaterialCamClayCreepCMESetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_CamClayCreepCME);

  m_MPSetStrategy.push_back(new CMaterialCamClayCreepCMPSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_CamClayCreepCMP);

  m_MPSetStrategy.push_back(new CMaterialCamClayCreepDynShearSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_CamClayCreepDynShear);

  m_MPSetStrategy.push_back(new CMaterialCamClayCreepDynUniSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_CamClayCreepDynUni);

  m_MPSetStrategy.push_back(new CMaterialCamClayCreepFrictionSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_CamClayCreepFriction);

  m_MPSetStrategy.push_back(new CMaterialCamClayCreepHardeningSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_CamClayCreepHardening);

  m_MPSetStrategy.push_back(new CMaterialCamClayCreepPoissonSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_CamClayCreepPoisson);

  m_MPSetStrategy.push_back(new CMaterialCamClayCreepPorositySetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_CamClayCreepPorosity);

  m_MPSetStrategy.push_back(new CMaterialCamClayCreepPreconsolidationSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_CamClayCreepPreconsolidation);

  m_MPSetStrategy.push_back(new CMaterialCamClayCreepShearStiffSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_CamClayCreepShearStiff);

  m_MPSetStrategy.push_back(new CMaterialCamClayCreepVelocityPSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_CamClayCreepVelocityP);

  m_MPSetStrategy.push_back(new CMaterialCamClayCreepVelocitySSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_CamClayCreepVelocityS);

  m_MPSetStrategy.push_back(new CMaterialCamClayCreepYoungSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_CamClayCreepYoung);

  m_MPSetStrategy.push_back(new CMaterialCamClayDynShearSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_CamClayDynShear);

  m_MPSetStrategy.push_back(new CMaterialCamClayDynUniSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_CamClayDynUni);

  m_MPSetStrategy.push_back(new CMaterialCamClayFrictionSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_CamClayFriction);

  m_MPSetStrategy.push_back(new CMaterialCamClayHardeningSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_CamClayHardening);

  m_MPSetStrategy.push_back(new CMaterialCamClayPoissonSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_CamClayPoisson);

  m_MPSetStrategy.push_back(new CMaterialCamClayPorositySetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_CamClayPorosity);

  m_MPSetStrategy.push_back(new CMaterialCamClayPreconsolidationSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_CamClayPreconsolidation);

  m_MPSetStrategy.push_back(new CMaterialCamClayShearStiffSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_CamClayShearStiff);

  m_MPSetStrategy.push_back(new CMaterialCamClayVelocityPSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_CamClayVelocityP);

  m_MPSetStrategy.push_back(new CMaterialCamClayVelocitySSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_CamClayVelocityS);

  m_MPSetStrategy.push_back(new CMaterialCamClayYoungSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_CamClayYoung);

  m_MPSetStrategy.push_back(new CMaterialDualCapElasticHardeningSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_DualCapElasticHardening);

  m_MPSetStrategy.push_back(new CMaterialDualCapPoissonSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_DualCapPoisson);

  m_MPSetStrategy.push_back(new CMaterialDualCapPorositySetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_DualCapPorosity);

  m_MPSetStrategy.push_back(new CMaterialDualCapPreconsolidationSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_DualCapPreconsolidation);

  m_MPSetStrategy.push_back(new CMaterialDualCapYoungSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_DualCapYoung);

  m_MPSetStrategy.push_back(new CMaterialLinearBulkStiffSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_LinearBulkStiff);

  m_MPSetStrategy.push_back(new CMaterialLinearCMESetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_LinearCME);

  m_MPSetStrategy.push_back(new CMaterialLinearDynShearSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_LinearDynShear);

  m_MPSetStrategy.push_back(new CMaterialLinearDynUniSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_LinearDynUni);

  m_MPSetStrategy.push_back(new CMaterialLinearPoissonSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_LinearPoisson);

  m_MPSetStrategy.push_back(new CMaterialLinearRhoBSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_LinearRhoB);

  m_MPSetStrategy.push_back(new CMaterialLinearShearStiffSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_LinearShearStiff);

  m_MPSetStrategy.push_back(new CMaterialLinearVelocityPSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_LinearVelocityP);

  m_MPSetStrategy.push_back(new CMaterialLinearVelocitySSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_LinearVelocityS);

  m_MPSetStrategy.push_back(new CMaterialLinearYoungSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_LinearYoung);

  m_MPSetStrategy.push_back(new CMaterialUndrainedFluidBulkModulusSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_UndrainedFluidBulkModulus);

  m_MPSetStrategy.push_back(new CMaterialUndrainedFluidThermalExpansionCoefficientSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_UndrainedFluidThermalExpansionCoefficient);

  m_MPSetStrategy.push_back(new CMaterialUndrainedGrainStiffnessSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_UndrainedGrainStiffness);

  m_MPSetStrategy.push_back(new CMaterialUndrainedPoissonSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_UndrainedPoisson);

  m_MPSetStrategy.push_back(new CMaterialUndrainedPorositySetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_UndrainedPorosity);

  m_MPSetStrategy.push_back(new CMaterialUndrainedUndrainedPoissonSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_UndrainedUndrainedPoisson);

  m_MPSetStrategy.push_back(new CMaterialUndrainedUndrainedThermalExpansionCoefficientSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_UndrainedUndrainedThermalExpansionCoefficient);

  m_MPSetStrategy.push_back(new CMaterialUndrainedUndrainedYoungSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_UndrainedUndrainedYoung);

  m_MPSetStrategy.push_back(new CMaterialUndrainedVolumetricThermalExpansionCoefficientSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_UndrainedVolumetricThermalExpansionCoefficient);

  m_MPSetStrategy.push_back(new CMaterialUndrainedYoungSetStrategy());
  assert(m_MPSetStrategy.size() - 1 == MP_SET_UndrainedYoung);

  assert(m_MPSetStrategy.size() == _MP_SET_STRATEGY_SIZE);
}

void CMaterialHelperFactory::createMPUnitTypes() {
  m_MPUnitType.reserve(_MP_UNIT_TYPE_SIZE);

  m_MPUnitType.push_back(new CUnitTypeNone());
  assert(m_MPUnitType.size() - 1 == UT_NONE);

  m_MPUnitType.push_back(new CUnitTypeStress());
  assert(m_MPUnitType.size() - 1 == UT_STRESS);

  m_MPUnitType.push_back(new CUnitTypeStressGradient());
  assert(m_MPUnitType.size() - 1 == UT_STRESS_GRADIENT);

  m_MPUnitType.push_back(new CUnitTypeDensity());
  assert(m_MPUnitType.size() - 1 == UT_DENSITY);

  m_MPUnitType.push_back(new CUnitTypeDepth());
  assert(m_MPUnitType.size() - 1 == UT_DEPTH);

  m_MPUnitType.push_back(new CUnitTypeForce());
  assert(m_MPUnitType.size() - 1 == UT_FORCE);

  m_MPUnitType.push_back(new CUnitTypeVelocity());
  assert(m_MPUnitType.size() - 1 == UT_VELOCITY);

  m_MPUnitType.push_back(new CUnitTypeAngle());
  assert(m_MPUnitType.size() - 1 == UT_ANGLE);

  m_MPUnitType.push_back(new CUnitTypePerStress());
  assert(m_MPUnitType.size() - 1 == UT_PER_STRESS);

  m_MPUnitType.push_back(new CUnitTypePerTime());
  assert(m_MPUnitType.size() - 1 == UT_PER_TIME);

  m_MPUnitType.push_back(new CUnitTypeTemper());
  assert(m_MPUnitType.size() - 1 == UT_TEMPER);

  m_MPUnitType.push_back(new CUnitTypeKelvinTemper());
  assert(m_MPUnitType.size() - 1 == UT_KELVIN_TEMPER);

  m_MPUnitType.push_back(new CUnitTypePerTemper());
  assert(m_MPUnitType.size() - 1 == UT_PER_TEMPER);

  m_MPUnitType.push_back(new CUnitTypePerDistance());
  assert(m_MPUnitType.size() - 1 == UT_PER_DISTANCE);

  m_MPUnitType.push_back(new CUnitTypeThermConduct());
  assert(m_MPUnitType.size() - 1 == UT_THERM_CONDUCT);

  m_MPUnitType.push_back(new CUnitTypeThermCapacity());
  assert(m_MPUnitType.size() - 1 == UT_THERM_CAPACITY);

  m_MPUnitType.push_back(new CUnitTypePermea());
  assert(m_MPUnitType.size() - 1 == UT_PERMEA);

  m_MPUnitType.push_back(new CUnitTypeViscosity());
  assert(m_MPUnitType.size() - 1 == UT_VISCOSITY);

  assert(m_MPUnitType.size() == _MP_UNIT_TYPE_SIZE);
}

ml::CMaterial::CCreator *CMaterialHelperFactory::getMatCreator(int material) const {
  ml::CMaterial::CCreator *pCreator = 0;

  if (material >= 0 && material < m_MatCreator.size()) {
    pCreator = m_MatCreator[material];
    pCreator->Reset();
  }

  return pCreator;
}

const ml::CMaterial::CCheckStrategy *
CMaterialHelperFactory::getMatCheckStrategy(enum MaterialCheckStrategy strategy) const {
  assert(strategy < m_MatCheckStrategy.size());
  assert(strategy == MAT_NO_CHECK_STRATEGY || m_MatCheckStrategy[strategy] != 0);
  return m_MatCheckStrategy[strategy];
}

IPQPlotter *CMaterialHelperFactory::getMatPQPlotter(enum MaterialPQPlotter plotter) const {
  assert(plotter < m_MatPQPlotter.size());
  assert(plotter == MAT_NO_PQ_PLOTTER || m_MatPQPlotter[plotter] != 0);
  return m_MatPQPlotter[plotter];
}

const ml::CMatParam::CCheckStrategy *
CMaterialHelperFactory::getMPCheckStrategy(enum MatParamCheckStrategy strategy) const {
  assert(strategy < m_MPCheckStrategy.size());
  assert(strategy == MP_NO_CHECK_STRATEGY || m_MPCheckStrategy[strategy] != 0);
  return m_MPCheckStrategy[strategy];
}

const ml::CMatParam::CSetStrategy *CMaterialHelperFactory::getMPSetStrategy(enum MatParamSetStrategy strategy) const {
  assert(strategy < m_MPSetStrategy.size());
  assert(strategy == MP_NO_SET_STRATEGY || m_MPSetStrategy[strategy] != 0);
  return m_MPSetStrategy[strategy];
}

const ml::CMatParam::CUnitType &CMaterialHelperFactory::getMPUnitType(enum MatParamUnitType unitType) const {
  assert(unitType < m_MPUnitType.size());
  assert(m_MPUnitType[unitType] != 0);
  return *m_MPUnitType[unitType];
}
