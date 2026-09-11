#include "tstMaterials.h"
#include "TestLib.h"
#include <gtest/gtest.h>

#define TEST_MATERIALS

#include "MaterialModelAnisotropicCamClay.h"
#include "MaterialModelAnisotropy.h"
#include "MaterialModelCamClay.h"
#include "MaterialModelCamClayCreep.h"
#include "MaterialModelCreep.h"
#include "MaterialModelDualCap.h"
#include "MaterialModelDualCapLinEla.h"
#include "MaterialModelFractureAnisotropy.h"
#include "MaterialModelFractureAperture.h"
#include "MaterialModelFractureAperture2.h"
#include "MaterialModelLinear.h"
#include "MaterialModelMCCohesionHard1.h"
#include "MaterialModelMCCohesionHard2.h"
#include "MaterialModelMCCohesionHard3.h"
#include "MaterialModelMCFrictionHard1.h"
#include "MaterialModelMCFrictionHard2.h"
#include "MaterialModelMCFrictionHard3.h"
#include "MaterialModelMCHardeningCheckStrategies.h"
#include "MaterialModelModifiedMohrCoulomb.h"
#include "MaterialModelMohrCoulomb.h"
#include "MaterialModelRigidity.h"
#include "MaterialModelUndrained.h"
#include "MaterialModelUpscaledAnisotropy.h"
#include "mlMatParamDomain.h"

#include "MaterialUnitTypes.h"

#include "LibraryMaterial.h"
#include "LibraryMaterialParameter.h"
#include "Materials.h"
#include "ValueTypes.h"

#include "MaterialHelperFactory.h"

namespace {

const QString GROUP_MANUAL = "Manual";
const QString GROUP_BULK = "Bulk stiffness";
const QString GROUP_DYNAMIC = "Dynamic stiffness";
const QString GROUP_VELOCITIES = "Velocities";

} // namespace

namespace test_materials {

int LibraryLoad_Materials() { return 0; }

template <class MaterialModelCreator> class CMaterialTest : public MaterialModelCreator {
  MaterialModelCreator m_Creator;
  CLibraryMaterial *m_LibMat;

public:
  CMaterialTest() : m_LibMat(0) { m_LibMat = static_cast<CLibraryMaterial *>(m_Creator.OnCreateMaterial()); }
  ~CMaterialTest() {
    const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();
    ml::CMaterial::CCreator *creator = f->getMatCreator(0); // any will do
    creator->Destroy(m_LibMat);
  }
  ml::CMaterial &getMaterial() { return *m_LibMat; }
  CLibraryMaterial &getLibrary() { return *m_LibMat; }
  size_t ParameterSize() { return m_LibMat->ParameterSize(); }
  CLibraryMaterialParameter &Parameter(size_t i) { return m_LibMat->Parameter(i); }
};

// not ideal, but it looks like we expose more by using friend methods in MaterialParamTest due to namespace syntactic
// sugar
class AccessParam {
  const ml::CMatParam::CCheckStrategy *m_pCheckStrategy;
  const ml::CMatParam::CSetStrategy *m_pSetStrategy;

public:
  AccessParam(const ml::CMatParam &rhs) : m_pCheckStrategy(rhs.m_pCheckStrategy), m_pSetStrategy(rhs.m_pSetStrategy) {}
  const ml::CMatParam::CCheckStrategy *getCheck() { return m_pCheckStrategy; }
  const ml::CMatParam::CSetStrategy *getSet() { return m_pSetStrategy; }
};

// similar for Material
class AccessMaterial {
  const ml::CMaterial::CCheckStrategy *m_pCheckStrategy;

public:
  AccessMaterial(const ml::CMaterial &rhs) : m_pCheckStrategy(rhs.m_pCheckStrategy) {}
  const ml::CMaterial::CCheckStrategy *getCheck() { return m_pCheckStrategy; }
};

class AccessLibMaterial {
  const IPQPlotter *m_pPlotter;

public:
  AccessLibMaterial(const CLibraryMaterial &rhs) : m_pPlotter(rhs.m_pPQPlotter) {}
  const IPQPlotter *getPlotter() { return m_pPlotter; }
};

template <class Base, class Child> bool CompareClass(const Base &o, const Base &n) {
  return dynamic_cast<const Child *>(&o) && dynamic_cast<const Child *>(&n);
}

class MaterialParamTest : public ::testing::Test {

  bool TestUnitType(const ml::CMatParam::CUnitType &oldUnitType, const ml::CMatParam::CUnitType &newUnitType) {
    bool ret = false;

    ret = ret || CompareClass<ml::CMatParam::CUnitType, CUnitTypeNone>(oldUnitType, newUnitType);
    ret = ret || CompareClass<ml::CMatParam::CUnitType, CUnitTypeStress>(oldUnitType, newUnitType);
    ret = ret || CompareClass<ml::CMatParam::CUnitType, CUnitTypeStressGradient>(oldUnitType, newUnitType);
    ret = ret || CompareClass<ml::CMatParam::CUnitType, CUnitTypeDensity>(oldUnitType, newUnitType);
    ret = ret || CompareClass<ml::CMatParam::CUnitType, CUnitTypeDepth>(oldUnitType, newUnitType);
    ret = ret || CompareClass<ml::CMatParam::CUnitType, CUnitTypeForce>(oldUnitType, newUnitType);
    ret = ret || CompareClass<ml::CMatParam::CUnitType, CUnitTypeVelocity>(oldUnitType, newUnitType);
    ret = ret || CompareClass<ml::CMatParam::CUnitType, CUnitTypeAngle>(oldUnitType, newUnitType);
    ret = ret || CompareClass<ml::CMatParam::CUnitType, CUnitTypePerStress>(oldUnitType, newUnitType);
    ret = ret || CompareClass<ml::CMatParam::CUnitType, CUnitTypePerTime>(oldUnitType, newUnitType);
    ret = ret || CompareClass<ml::CMatParam::CUnitType, CUnitTypeTemper>(oldUnitType, newUnitType);
    ret = ret || CompareClass<ml::CMatParam::CUnitType, CUnitTypeKelvinTemper>(oldUnitType, newUnitType);
    ret = ret || CompareClass<ml::CMatParam::CUnitType, CUnitTypePerTemper>(oldUnitType, newUnitType);
    ret = ret || CompareClass<ml::CMatParam::CUnitType, CUnitTypePerDistance>(oldUnitType, newUnitType);
    ret = ret || CompareClass<ml::CMatParam::CUnitType, CUnitTypeThermConduct>(oldUnitType, newUnitType);
    ret = ret || CompareClass<ml::CMatParam::CUnitType, CUnitTypeThermCapacity>(oldUnitType, newUnitType);
    ret = ret || CompareClass<ml::CMatParam::CUnitType, CUnitTypePermea>(oldUnitType, newUnitType);
    ret = ret || CompareClass<ml::CMatParam::CUnitType, CUnitTypeViscosity>(oldUnitType, newUnitType);

    return ret;
  }

  bool TestCheckStrategy(const ml::CMatParam::CCheckStrategy *oldCheckStrategy,
                         const ml::CMatParam::CCheckStrategy *newCheckStrategy) {
    bool ret = oldCheckStrategy == 0 && newCheckStrategy == 0;

    ret = ret || CompareClass<ml::CMatParam::CCheckStrategy, CMaterialAnisotropyCheckStrategy>(*oldCheckStrategy,
                                                                                               *newCheckStrategy);
    ret = ret || CompareClass<ml::CMatParam::CCheckStrategy, CMaterialAnisotropyThomsenDeltaCheckStrategy>(
                     *oldCheckStrategy, *newCheckStrategy);
    ret = ret || CompareClass<ml::CMatParam::CCheckStrategy, CMaterialAnisotropyThomsenEpsilonCheckStrategy>(
                     *oldCheckStrategy, *newCheckStrategy);
    ret = ret || CompareClass<ml::CMatParam::CCheckStrategy, CMaterialAnisotropyThomsenGammaCheckStrategy>(
                     *oldCheckStrategy, *newCheckStrategy);
    ret = ret || CompareClass<ml::CMatParam::CCheckStrategy, CMaterialAnisotropyVpCheckStrategy>(*oldCheckStrategy,
                                                                                                 *newCheckStrategy);
    ret = ret || CompareClass<ml::CMatParam::CCheckStrategy, CMaterialAnisotropyVsCheckStrategy>(*oldCheckStrategy,
                                                                                                 *newCheckStrategy);
    ret = ret || CompareClass<ml::CMatParam::CCheckStrategy, CMaterialCamClayCMPCheckStrategy>(*oldCheckStrategy,
                                                                                               *newCheckStrategy);
    ret = ret || CompareClass<ml::CMatParam::CCheckStrategy, CMaterialCamClayCreepCMPCheckStrategy>(*oldCheckStrategy,
                                                                                                    *newCheckStrategy);
    ret = ret || CompareClass<ml::CMatParam::CCheckStrategy, CMaterialDualCapPreconsolidationCheckStrategy>(
                     *oldCheckStrategy, *newCheckStrategy);
    ret = ret || CompareClass<ml::CMatParam::CCheckStrategy, CMaterialDualCapSecondaryPreconCheckStrategy>(
                     *oldCheckStrategy, *newCheckStrategy);
    ret = ret || CompareClass<ml::CMatParam::CCheckStrategy, CMaterialLinearBulkStiffCheckStrategy>(*oldCheckStrategy,
                                                                                                    *newCheckStrategy);
    ret = ret || CompareClass<ml::CMatParam::CCheckStrategy, CMaterialLinearDynShearCheckStrategy>(*oldCheckStrategy,
                                                                                                   *newCheckStrategy);
    ret = ret || CompareClass<ml::CMatParam::CCheckStrategy, CMaterialLinearDynUniCheckStrategy>(*oldCheckStrategy,
                                                                                                 *newCheckStrategy);
    ret = ret || CompareClass<ml::CMatParam::CCheckStrategy, CMaterialLinearLatRatioMaxCheckStrategy>(
                     *oldCheckStrategy, *newCheckStrategy);
    ret = ret || CompareClass<ml::CMatParam::CCheckStrategy, CMaterialLinearLatRatioMinCheckStrategy>(
                     *oldCheckStrategy, *newCheckStrategy);
    ret = ret || CompareClass<ml::CMatParam::CCheckStrategy, CMaterialLinearShearStiffCheckStrategy>(*oldCheckStrategy,
                                                                                                     *newCheckStrategy);
    ret = ret || CompareClass<ml::CMatParam::CCheckStrategy, CMaterialLinearVelocityPCheckStrategy>(*oldCheckStrategy,
                                                                                                    *newCheckStrategy);
    ret = ret || CompareClass<ml::CMatParam::CCheckStrategy, CMaterialLinearVelocitySCheckStrategy>(*oldCheckStrategy,
                                                                                                    *newCheckStrategy);
    ret = ret || CompareClass<ml::CMatParam::CCheckStrategy, CMaterialMCHardEquivalentPlasticStrain1CheckStrategy>(
                     *oldCheckStrategy, *newCheckStrategy);
    ret = ret || CompareClass<ml::CMatParam::CCheckStrategy, CMaterialMCHardEquivalentPlasticStrain2CheckStrategy>(
                     *oldCheckStrategy, *newCheckStrategy);
    ret = ret || CompareClass<ml::CMatParam::CCheckStrategy, CMaterialMCHardEquivalentPlasticStrain3CheckStrategy>(
                     *oldCheckStrategy, *newCheckStrategy);
    ret = ret || CompareClass<ml::CMatParam::CCheckStrategy, CMaterialMohrCoulombDilatationCheckStrategy>(
                     *oldCheckStrategy, *newCheckStrategy);
    ret = ret || CompareClass<ml::CMatParam::CCheckStrategy, CMaterialMohrCoulombFrictionCheckStrategy>(
                     *oldCheckStrategy, *newCheckStrategy);

    if (!ret &&
        CompareClass<ml::CMatParam::CCheckStrategy, ml::CMatParamDomain>(*oldCheckStrategy, *newCheckStrategy)) {
      ret = static_cast<const ml::CMatParamDomain &>(*oldCheckStrategy) ==
            static_cast<const ml::CMatParamDomain &>(*newCheckStrategy);
    }

    return ret;
  }

  bool TestSetStrategy(const ml::CMatParam::CSetStrategy *oldSetStrategy,
                       const ml::CMatParam::CSetStrategy *newSetStrategy) {
    bool ret = oldSetStrategy == 0 && newSetStrategy == 0;

    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialAnisotropyPoissonNormSetStrategy>(*oldSetStrategy,
                                                                                                      *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialAnisotropyPoissonTransSetStrategy>(*oldSetStrategy,
                                                                                                       *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialAnisotropyShearStiffSetStrategy>(*oldSetStrategy,
                                                                                                     *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialAnisotropyThomsenDeltaSetStrategy>(*oldSetStrategy,
                                                                                                       *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialAnisotropyThomsenEpsilonSetStrategy>(
                     *oldSetStrategy, *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialAnisotropyThomsenGammaSetStrategy>(*oldSetStrategy,
                                                                                                       *newSetStrategy);
    ret = ret ||
          CompareClass<ml::CMatParam::CSetStrategy, CMaterialAnisotropyVpSetStrategy>(*oldSetStrategy, *newSetStrategy);
    ret = ret ||
          CompareClass<ml::CMatParam::CSetStrategy, CMaterialAnisotropyVsSetStrategy>(*oldSetStrategy, *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialAnisotropyYoungNormSetStrategy>(*oldSetStrategy,
                                                                                                    *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialAnisotropyYoungTransSetStrategy>(*oldSetStrategy,
                                                                                                     *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialCamClayBulkStiffSetStrategy>(*oldSetStrategy,
                                                                                                 *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialCamClayCapShapSetStrategy>(*oldSetStrategy,
                                                                                               *newSetStrategy);
    ret = ret ||
          CompareClass<ml::CMatParam::CSetStrategy, CMaterialCamClayCMESetStrategy>(*oldSetStrategy, *newSetStrategy);
    ret = ret ||
          CompareClass<ml::CMatParam::CSetStrategy, CMaterialCamClayCMPSetStrategy>(*oldSetStrategy, *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialCamClayCreepBulkStiffSetStrategy>(*oldSetStrategy,
                                                                                                      *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialCamClayCreepCapShapSetStrategy>(*oldSetStrategy,
                                                                                                    *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialCamClayCreepCMESetStrategy>(*oldSetStrategy,
                                                                                                *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialCamClayCreepCMPSetStrategy>(*oldSetStrategy,
                                                                                                *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialCamClayCreepDynShearSetStrategy>(*oldSetStrategy,
                                                                                                     *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialCamClayCreepDynUniSetStrategy>(*oldSetStrategy,
                                                                                                   *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialCamClayCreepFrictionSetStrategy>(*oldSetStrategy,
                                                                                                     *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialCamClayCreepHardeningSetStrategy>(*oldSetStrategy,
                                                                                                      *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialCamClayCreepPoissonSetStrategy>(*oldSetStrategy,
                                                                                                    *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialCamClayCreepPorositySetStrategy>(*oldSetStrategy,
                                                                                                     *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialCamClayCreepPreconsolidationSetStrategy>(
                     *oldSetStrategy, *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialCamClayCreepShearStiffSetStrategy>(*oldSetStrategy,
                                                                                                       *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialCamClayCreepVelocityPSetStrategy>(*oldSetStrategy,
                                                                                                      *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialCamClayCreepVelocitySSetStrategy>(*oldSetStrategy,
                                                                                                      *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialCamClayCreepYoungSetStrategy>(*oldSetStrategy,
                                                                                                  *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialCamClayDynShearSetStrategy>(*oldSetStrategy,
                                                                                                *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialCamClayDynUniSetStrategy>(*oldSetStrategy,
                                                                                              *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialCamClayFrictionSetStrategy>(*oldSetStrategy,
                                                                                                *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialCamClayHardeningSetStrategy>(*oldSetStrategy,
                                                                                                 *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialCamClayPoissonSetStrategy>(*oldSetStrategy,
                                                                                               *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialCamClayPorositySetStrategy>(*oldSetStrategy,
                                                                                                *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialCamClayPreconsolidationSetStrategy>(
                     *oldSetStrategy, *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialCamClayShearStiffSetStrategy>(*oldSetStrategy,
                                                                                                  *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialCamClayVelocityPSetStrategy>(*oldSetStrategy,
                                                                                                 *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialCamClayVelocitySSetStrategy>(*oldSetStrategy,
                                                                                                 *newSetStrategy);
    ret = ret ||
          CompareClass<ml::CMatParam::CSetStrategy, CMaterialCamClayYoungSetStrategy>(*oldSetStrategy, *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialDualCapElasticHardeningSetStrategy>(
                     *oldSetStrategy, *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialDualCapPoissonSetStrategy>(*oldSetStrategy,
                                                                                               *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialDualCapPorositySetStrategy>(*oldSetStrategy,
                                                                                                *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialDualCapPreconsolidationSetStrategy>(
                     *oldSetStrategy, *newSetStrategy);
    ret = ret ||
          CompareClass<ml::CMatParam::CSetStrategy, CMaterialDualCapYoungSetStrategy>(*oldSetStrategy, *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialLinearBulkStiffSetStrategy>(*oldSetStrategy,
                                                                                                *newSetStrategy);
    ret = ret ||
          CompareClass<ml::CMatParam::CSetStrategy, CMaterialLinearCMESetStrategy>(*oldSetStrategy, *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialLinearDynShearSetStrategy>(*oldSetStrategy,
                                                                                               *newSetStrategy);
    ret = ret ||
          CompareClass<ml::CMatParam::CSetStrategy, CMaterialLinearDynUniSetStrategy>(*oldSetStrategy, *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialLinearPoissonSetStrategy>(*oldSetStrategy,
                                                                                              *newSetStrategy);
    ret = ret ||
          CompareClass<ml::CMatParam::CSetStrategy, CMaterialLinearRhoBSetStrategy>(*oldSetStrategy, *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialLinearShearStiffSetStrategy>(*oldSetStrategy,
                                                                                                 *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialLinearVelocityPSetStrategy>(*oldSetStrategy,
                                                                                                *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialLinearVelocitySSetStrategy>(*oldSetStrategy,
                                                                                                *newSetStrategy);
    ret = ret ||
          CompareClass<ml::CMatParam::CSetStrategy, CMaterialLinearYoungSetStrategy>(*oldSetStrategy, *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialUndrainedFluidBulkModulusSetStrategy>(
                     *oldSetStrategy, *newSetStrategy);
    ret =
        ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialUndrainedFluidThermalExpansionCoefficientSetStrategy>(
                   *oldSetStrategy, *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialUndrainedGrainStiffnessSetStrategy>(
                     *oldSetStrategy, *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialUndrainedPoissonSetStrategy>(*oldSetStrategy,
                                                                                                 *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialUndrainedPorositySetStrategy>(*oldSetStrategy,
                                                                                                  *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialUndrainedUndrainedPoissonSetStrategy>(
                     *oldSetStrategy, *newSetStrategy);
    ret = ret ||
          CompareClass<ml::CMatParam::CSetStrategy, CMaterialUndrainedUndrainedThermalExpansionCoefficientSetStrategy>(
              *oldSetStrategy, *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialUndrainedUndrainedYoungSetStrategy>(
                     *oldSetStrategy, *newSetStrategy);
    ret = ret ||
          CompareClass<ml::CMatParam::CSetStrategy, CMaterialUndrainedVolumetricThermalExpansionCoefficientSetStrategy>(
              *oldSetStrategy, *newSetStrategy);
    ret = ret || CompareClass<ml::CMatParam::CSetStrategy, CMaterialUndrainedYoungSetStrategy>(*oldSetStrategy,
                                                                                               *newSetStrategy);

    return ret;
  }

  bool TestParam(CLibraryMaterialParameter &oldParam, CLibraryMaterialParameter &newParam) {
    return TestUnitType(oldParam.UnitConversion(), newParam.UnitConversion()) &&
           TestCheckStrategy(AccessParam(oldParam).getCheck(), AccessParam(newParam).getCheck()) &&
           TestSetStrategy(AccessParam(oldParam).getSet(), AccessParam(newParam).getSet());
  }

public:
  template <class OldCreator, class NewCreator> bool TestCreators() {
    CMaterialTest<OldCreator> oldCreator;
    CMaterialTest<NewCreator> newCreator;

    AccessMaterial oldMat(oldCreator.getMaterial());
    AccessMaterial newMat(newCreator.getMaterial());

    if (oldMat.getCheck() != 0 || newMat.getCheck() != 0) {
      if (!CompareClass<ml::CMaterial::CCheckStrategy, CMaterialAnisotropyCheckStrategy>(*oldMat.getCheck(),
                                                                                         *newMat.getCheck()))
        return false;
    }

    AccessLibMaterial oldLib(oldCreator.getLibrary());
    AccessLibMaterial newLib(newCreator.getLibrary());

    if (oldLib.getPlotter() != 0 || newLib.getPlotter() != 0) {
      bool ret = false;
      if (dynamic_cast<const CMaterialCamClayCreatorDEPRECATED::CPQPlotter *>(oldLib.getPlotter()))
        ret = dynamic_cast<const CMaterialCamClayCreatorV2::CPQPlotter *>(newLib.getPlotter());

      if (dynamic_cast<const CMaterialCamClayCreepCreatorDEPRECATED::CPQPlotter *>(oldLib.getPlotter()))
        ret = dynamic_cast<const CMaterialCamClayCreepCreatorV2::CPQPlotter *>(newLib.getPlotter());

      if (dynamic_cast<const CMaterialDualCapCreator::CPQPlotter *>(oldLib.getPlotter()))
        ret = dynamic_cast<const CMaterialDualCapCreator::CPQPlotter *>(newLib.getPlotter());

      if (dynamic_cast<const CMaterialModifiedMohrCoulombCreatorDEPRECATED::CPQPlotter *>(oldLib.getPlotter()))
        ret = dynamic_cast<const CMaterialModifiedMohrCoulombCreatorV2::CPQPlotter *>(newLib.getPlotter());

      if (dynamic_cast<const CMaterialMohrCoulombCreatorDEPRECATED::CPQPlotter *>(oldLib.getPlotter()))
        ret = dynamic_cast<const CMaterialMohrCoulombCreatorV2::CPQPlotter *>(newLib.getPlotter());

      if (!ret)
        return false;
    }

    if (oldCreator.ParameterSize() != newCreator.ParameterSize())
      return false;

    for (size_t i = 0; i < oldCreator.ParameterSize(); ++i) {
      if (!TestParam(oldCreator.Parameter(i), newCreator.Parameter(i)))
        return false;
    }
    return true;
  }
};

TEST_F(MaterialParamTest, ModelLinear) {
  EXPECT_TRUE((TestCreators<CMaterialLinearCreatorDEPRECATED, CMaterialLinearCreatorV2>()));
}

TEST_F(MaterialParamTest, ModelCamClay) {
  EXPECT_TRUE((TestCreators<CMaterialCamClayCreatorDEPRECATED, CMaterialCamClayCreatorV2>()));
}

TEST_F(MaterialParamTest, ModelMohrCoulomb) {
  EXPECT_TRUE((TestCreators<CMaterialMohrCoulombCreatorDEPRECATED, CMaterialMohrCoulombCreatorV2>()));
}

TEST_F(MaterialParamTest, ModelModifiedMohrCoulomb) {
  EXPECT_TRUE((TestCreators<CMaterialModifiedMohrCoulombCreatorDEPRECATED, CMaterialModifiedMohrCoulombCreatorV2>()));
}

TEST_F(MaterialParamTest, ModelCreep) {
  EXPECT_TRUE((TestCreators<CMaterialCreepCreatorDEPRECATED, CMaterialCreepCreatorV2>()));
}

TEST_F(MaterialParamTest, ModelUndrained) {
  EXPECT_TRUE((TestCreators<CMaterialUndrainedCreatorDEPRECATED, CMaterialUndrainedCreatorV2>()));
}

TEST_F(MaterialParamTest, ModelMCCohesionHard1) {
  EXPECT_TRUE((TestCreators<CMaterialModelMCCohesionHard1CreatorDEPRECATED, CMaterialModelMCCohesionHard1CreatorV2>()));
}

TEST_F(MaterialParamTest, ModelMCCohesionHard2) {
  EXPECT_TRUE((TestCreators<CMaterialModelMCCohesionHard2CreatorDEPRECATED, CMaterialModelMCCohesionHard2CreatorV2>()));
}

TEST_F(MaterialParamTest, ModelMCCohesionHard3) {
  EXPECT_TRUE((TestCreators<CMaterialModelMCCohesionHard3CreatorDEPRECATED, CMaterialModelMCCohesionHard3CreatorV2>()));
}

TEST_F(MaterialParamTest, ModelMCFrictionHard1) {
  EXPECT_TRUE((TestCreators<CMaterialModelMCFrictionHard1CreatorDEPRECATED, CMaterialModelMCFrictionHard1CreatorV2>()));
}

TEST_F(MaterialParamTest, ModelMCFrictionHard2) {
  EXPECT_TRUE((TestCreators<CMaterialModelMCFrictionHard2CreatorDEPRECATED, CMaterialModelMCFrictionHard2CreatorV2>()));
}

TEST_F(MaterialParamTest, ModelMCFrictionHard3) {
  EXPECT_TRUE((TestCreators<CMaterialModelMCFrictionHard3CreatorDEPRECATED, CMaterialModelMCFrictionHard3CreatorV2>()));
}

TEST_F(MaterialParamTest, ModelRigidity) {
  EXPECT_TRUE((TestCreators<CMaterialRigidityCreatorDEPRECATED, CMaterialRigidityCreatorV2>()));
}

/*
TEST_F(MaterialParamTest, ModelDualCap) {
  EXPECT_TRUE((TestCreators<CMaterialDualCapCreator, CMaterialDualCapCreator>()));
}
*/

TEST_F(MaterialParamTest, ModelDualCapLinEla) {
  EXPECT_TRUE((TestCreators<CMaterialDualCapLinElaCreatorDEPRECATED, CMaterialDualCapLinElaCreatorV2>()));
}

TEST_F(MaterialParamTest, ModelFractureAnisotropy) {
  EXPECT_TRUE((TestCreators<CMaterialFractureAnisotropyCreatorDEPRECATED, CMaterialFractureAnisotropyCreatorV2>()));
}

TEST_F(MaterialParamTest, ModelUpscaledAnisotropy) {
  EXPECT_TRUE((TestCreators<CMaterialUpscaledAnisotropyCreatorDEPRECATED, CMaterialUpscaledAnisotropyCreatorV2>()));
}

TEST_F(MaterialParamTest, ModelFractureAperture) {
  EXPECT_TRUE((TestCreators<CMaterialFractureApertureCreatorDEPRECATED, CMaterialFractureApertureCreatorV2>()));
}

TEST_F(MaterialParamTest, ModelAnisotropicCamClay) {
  EXPECT_TRUE((TestCreators<CMaterialAnisotropicCamClayCreatorDEPRECATED, CMaterialAnisotropicCamClayCreatorV2>()));
}

TEST_F(MaterialParamTest, ModelFractureAperture2) {
  EXPECT_TRUE((TestCreators<CMaterialFractureAperture2CreatorDEPRECATED, CMaterialFractureAperture2CreatorV2>()));
}

TEST_F(MaterialParamTest, ModelCamClayCreep) {
  EXPECT_TRUE((TestCreators<CMaterialCamClayCreepCreatorDEPRECATED, CMaterialCamClayCreepCreatorV2>()));
}

} // namespace test_materials