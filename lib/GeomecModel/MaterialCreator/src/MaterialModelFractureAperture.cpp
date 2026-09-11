
#include "MaterialModelFractureAperture.h"
#include "MaterialModelLinear.h"

#include "LibraryMaterial.h"
#include "LibraryMaterialParameter.h"
#include "MaterialHelperFactory.h"
#include "MaterialModelAnisotropy.h"
#include "MaterialUnitTypes.h"
#include "Materials.h"
#include "ValueTypes.h"
#include "mlMatParamDomain.h"

#include <cassert>

namespace {

const QString GROUP_MANUAL = "Manual";
const QString GROUP_THOMSEN = "Thomsen Parameters";

} // namespace

void CMaterialFractureApertureCreatorV2::OnCreateLibrary(CLibraryMaterial &m) {
  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();
  assert(f);

  m.setCheckStrategy(*f->getMatCheckStrategy(CMaterialHelperFactory::MAT_Anisotropy));
}

void CMaterialFractureApertureCreatorV2::OnCreateLibraryParameters(CLibraryMaterial &m) {
  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();
  assert(f);

  AddNumber(m, 47);

  AddLibParam(MLD_YOUNGMODULUS_NORM, DEF_E, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_YOUNGMODULUS_NORM, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_AnisotropyYoungNorm), QString(), false, GROUP_MANUAL);
  AddLibParam(MLD_YOUNGMODULUS_NORM_DECOMP, DEF_E, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_YOUNGMODULUS_NORM_DECOMP, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0), 0,
              QString(), false);
  AddLibParam(MLD_YOUNGMODULUS_TRANS, DEF_E, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_YOUNGMODULUS_TRANS, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_AnisotropyYoungTrans), QString(), false, GROUP_MANUAL);
  AddLibParam(MLD_YOUNGMODULUS_TRANS_DECOMP, DEF_E, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0), 0,
              QString(), false);
  AddLibParam(MLD_POISSONRATIO_NORM, DEF_NU, m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),
              IDT_VALUETYPE_POISSONRATIO_NORM, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LT_5Emin1),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_AnisotropyPoissonNorm), QString(), false,
              GROUP_MANUAL);
  AddLibParam(MLD_POISSONRATIO_NORM_DECOMP, DEF_NU, m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),
              IDT_VALUETYPE_POISSONRATIO_NORM_DECOMP, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LT_5Emin1),
              0, QString(), false);
  AddLibParam(MLD_POISSONRATIO_TRANS, DEF_NU, m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),
              IDT_VALUETYPE_POISSONRATIO_TRANS, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LT_5Emin1),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_AnisotropyPoissonTrans), QString(), false,
              GROUP_MANUAL);
  AddLibParam(MLD_POISSONRATIO_TRANS_DECOMP, DEF_NU, m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),
              IDT_VALUETYPE_POISSONRATIO_TRANS_DECOMP, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LT_5Emin1),
              0, QString(), false);
  AddLibParam(MLD_ANISOTROPIC_SHEARMODULUS, DEF_G, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_AnisotropyShearStiff), QString(), false, GROUP_MANUAL);
  AddLibParam(MLD_ANISOTROPIC_SHEARMODULUS_DECOMP, DEF_G, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS_DECOMP, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0), 0,
              QString(), false);

  AddLibParam(MLD_VELOCITYP, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_VELOCITY), IDT_VALUETYPE_VELOCITYP,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_AnisotropyVp),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_AnisotropyVp), QString(), false, GROUP_THOMSEN);
  AddLibParam(MLD_VELOCITYS, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_VELOCITY), IDT_VALUETYPE_VELOCITYS,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_AnisotropyVs),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_AnisotropyVs), QString(), false, GROUP_THOMSEN);
  AddLibParam(MLD_THOMSEN_EPSILON, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),
              IDT_VALUETYPE_THOMSEN_EPSILON, f->getMPCheckStrategy(CMaterialHelperFactory::MP_AnisotropyThomsenEpsilon),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_AnisotropyThomsenEpsilon), QString(), false,
              GROUP_THOMSEN);
  AddLibParam(MLD_THOMSEN_GAMMA, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE), IDT_VALUETYPE_THOMSEN_GAMMA,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_AnisotropyThomsenGamma),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_AnisotropyThomsenGamma), QString(), false,
              GROUP_THOMSEN);
  AddLibParam(MLD_THOMSEN_DELTA, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE), IDT_VALUETYPE_THOMSEN_DELTA,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_AnisotropyThomsenDelta),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_AnisotropyThomsenDelta), QString(), false,
              GROUP_THOMSEN);
  AddLibParam(MLD_LAYER_NORMAL_INCL, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_ANGLE),
              IDT_VALUETYPE_LAYER_NORMAL_INCL, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LE_90), 0,
              QString(), false);
  AddLibParam(MLD_LAYER_NORMAL_AZI, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_ANGLE),
              IDT_VALUETYPE_LAYER_NORMAL_AZI, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_min360_LE_360), 0,
              QString(), false);
  AddLibParam(MLD_FRACT_STIFF_NORMAL, 1, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS_GRADIENT),
              IDT_VALUETYPE_FRACT_STIFF_NORMAL, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_1), 0, QString(),
              false);
  AddLibParam(MLD_FRACT_STIFF_SHEAR, 1, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS_GRADIENT),
              IDT_VALUETYPE_FRACT_STIFF_SHEAR, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_1), 0, QString(),
              false);
  AddLibParam(MLD_REFERENCESTRESS, DEF_REFERENCESTRESS, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_REFERENCESTRESS, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LE_1E3), 0,
              QString(), false);

  AddLibParam(MLD_APERTUREPARAM, DEF_APERTUREPARAM, m, f->getMPUnitType(CMaterialHelperFactory::UT_PER_STRESS),
              IDT_VALUETYPE_APERTUREPARAM, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LE_1E3), 0, QString(),
              false);
  AddLibParam(MLD_HIGH_FRACT_DENS, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_PER_DISTANCE),
              IDT_VALUETYPE_HIGH_FRACT_DENS, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0), 0, QString(),
              false);
  AddLibParam(MLD_HIGH_FRACT_DENS_INCL, DEF_FRACT_INCL, m, f->getMPUnitType(CMaterialHelperFactory::UT_ANGLE),
              IDT_VALUETYPE_HIGH_FRACT_DENS_INCL, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LE_90), 0,
              QString(), false);
  AddLibParam(MLD_HIGH_FRACT_DENS_AZI, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_ANGLE),
              IDT_VALUETYPE_HIGH_FRACT_DENS_AZI, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_min360_LE_360), 0,
              QString(), false);
  AddLibParam(MLD_LOW_FRACT_DENS, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_PER_DISTANCE),
              IDT_VALUETYPE_LOW_FRACT_DENS, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0), 0, QString(),
              false);
  AddLibParam(MLD_LOW_FRACT_DENS_INCL, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_ANGLE),
              IDT_VALUETYPE_LOW_FRACT_DENS_INCL, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LE_90), 0,
              QString(), false);
  AddLibParam(MLD_LOW_FRACT_DENS_AZI, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_ANGLE),
              IDT_VALUETYPE_LOW_FRACT_DENS_AZI, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_min360_LE_360), 0,
              QString(), false);
  AddLibParam(MLD_INTER_FRACT_DENS, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_PER_DISTANCE),
              IDT_VALUETYPE_INTER_FRACT_DENS, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0), 0, QString(),
              false);
  AddLibParam(MLD_V0_HI, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_DEPTH), IDT_VALUETYPE_V0_HI,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0), 0, QString(), false);
  AddLibParam(MLD_V0_ME, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_DEPTH), IDT_VALUETYPE_V0_ME,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0), 0, QString(), false);

  AddLibParam(MLD_V0_LO, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_DEPTH), IDT_VALUETYPE_V0_LO,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0), 0, QString(), false);
  AddLibParam(MLD_THERMX, DEF_THERMX, m, f->getMPUnitType(CMaterialHelperFactory::UT_PER_TEMPER),
              IDT_VALUETYPE_THERMAL_EXPANSION, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LE_1Emin3), 0,
              QString(), false);
  AddLibParam(MLD_POROSITY, DEF_PORO, m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE), IDT_VALUETYPE_POROSITY,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LT_1), 0, QString(), false);
  AddLibParam(MLD_PERMEA, DEF_PERMEA, m, f->getMPUnitType(CMaterialHelperFactory::UT_PERMEA), IDT_VALUETYPE_PERMEA,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0), 0, QString(), false);
  AddLibParam(MLD_VISCOSITY, DEF_VISCOSITY, m, f->getMPUnitType(CMaterialHelperFactory::UT_VISCOSITY),
              IDT_VALUETYPE_VISCOSITY, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0), 0, QString(), false);
  AddLibParam(MLD_RHOB, DEF_RHOB, m, f->getMPUnitType(CMaterialHelperFactory::UT_DENSITY), IDT_VALUETYPE_RHOB,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0), 0, QString(), false);
  AddLibParam(MLD_LATRATIO_MIN, DEF_LATRATIO_MIN, m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),
              IDT_VALUETYPE_LATRATIO_MIN, f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearLatRatioMin), 0,
              QString(), false);
  AddLibParam(MLD_LATRATIO_MAX, DEF_LATRATIO_MAX, m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),
              IDT_VALUETYPE_LATRATIO_MAX, f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearLatRatioMax), 0,
              QString(), false);
  AddLibParam(MLD_AZIMUTH, DEF_AZIMUTH, m, f->getMPUnitType(CMaterialHelperFactory::UT_ANGLE), IDT_VALUETYPE_AZIMUTH,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LE_360), 0, QString(), false);

  AddLibParam(MLD_COHESION, DEF_COHESION, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_COHESION, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0), 0, QString(), false);
  AddLibParam(MLD_FRICTIONANGLE, DEF_FRICTION, m, f->getMPUnitType(CMaterialHelperFactory::UT_ANGLE),
              IDT_VALUETYPE_FRICTION_ANGLE, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LT_70), 0, QString(),
              false);
  AddLibParam(MLD_THERM_CONDUCT, DEF_THERM_CONDUCT, m, f->getMPUnitType(CMaterialHelperFactory::UT_THERM_CONDUCT),
              IDT_VALUETYPE_THERM_CONDUCT, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0), 0, QString(), false);
  AddLibParam(MLD_THERM_CAPACI, DEF_THERM_CAPACI, m, f->getMPUnitType(CMaterialHelperFactory::UT_THERM_CAPACITY),
              IDT_VALUETYPE_THERM_CAPACI, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0), 0, QString(), false);
  AddLibParam(MLD_FLUIDMODULUS, DEF_FLUIDMOD, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_FLUID_BULK_MOD, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0), 0, QString(),
              false);
  AddLibParam(MLD_FLUIDX, DEF_FLUIDX, m, f->getMPUnitType(CMaterialHelperFactory::UT_PER_TEMPER), IDT_VALUETYPE_FLUIDX,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LE_1Emin1), 0, QString(), false);
  AddLibParam(MLD_FLUID_DENSITY, DEF_FLUID_DENSITY, m, f->getMPUnitType(CMaterialHelperFactory::UT_DENSITY),
              IDT_VALUETYPE_FLUID_DENSITY, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0_LE_10000), 0,
              QString(), false);
}

int CMaterialFractureApertureCreatorV2::MaterialModel() { return MM_FRACTURE_APERTURE; }

QString CMaterialFractureApertureCreatorV2::MaterialModelName() { return MLD_FRACTURE_APERTURE; }

QString CMaterialFractureApertureCreatorV2::CalibrationPath() const { return QString(); }

void CMaterialFractureApertureCreatorDEPRECATED::OnCreateLibrary(CLibraryMaterial &m) {
  m.setCheckStrategy(*(new CMaterialAnisotropyCheckStrategy));
}

void CMaterialFractureApertureCreatorDEPRECATED::OnCreateLibraryParameters(CLibraryMaterial &m) {
  Add(new CLibraryMaterialParameter(MLD_YOUNGMODULUS_NORM, DEF_E, m, *new CUnitTypeStress,
                                    IDT_VALUETYPE_YOUNGMODULUS_NORM, new MP_GT(0),
                                    new CMaterialAnisotropyYoungNormSetStrategy),
      GROUP_MANUAL);
  Add(new CLibraryMaterialParameter(MLD_YOUNGMODULUS_NORM_DECOMP, DEF_E, m, *new CUnitTypeStress,
                                    IDT_VALUETYPE_YOUNGMODULUS_NORM_DECOMP, new MP_GT(0), 0));
  Add(new CLibraryMaterialParameter(MLD_YOUNGMODULUS_TRANS, DEF_E, m, *new CUnitTypeStress,
                                    IDT_VALUETYPE_YOUNGMODULUS_TRANS, new MP_GT(0),
                                    new CMaterialAnisotropyYoungTransSetStrategy),
      GROUP_MANUAL);
  Add(new CLibraryMaterialParameter(MLD_YOUNGMODULUS_TRANS_DECOMP, DEF_E, m, *new CUnitTypeStress,
                                    IDT_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP, new MP_GT(0), 0));
  Add(new CLibraryMaterialParameter(MLD_POISSONRATIO_NORM, DEF_NU, m, *new CUnitTypeNone,
                                    IDT_VALUETYPE_POISSONRATIO_NORM, new MP_GE_LT(0, 0.5),
                                    new CMaterialAnisotropyPoissonNormSetStrategy),
      GROUP_MANUAL);
  Add(new CLibraryMaterialParameter(MLD_POISSONRATIO_NORM_DECOMP, DEF_NU, m, *new CUnitTypeNone,
                                    IDT_VALUETYPE_POISSONRATIO_NORM_DECOMP, new MP_GE_LT(0, 0.5), 0));
  Add(new CLibraryMaterialParameter(MLD_POISSONRATIO_TRANS, DEF_NU, m, *new CUnitTypeNone,
                                    IDT_VALUETYPE_POISSONRATIO_TRANS, new MP_GE_LT(0, 0.5),
                                    new CMaterialAnisotropyPoissonTransSetStrategy),
      GROUP_MANUAL);
  Add(new CLibraryMaterialParameter(MLD_POISSONRATIO_TRANS_DECOMP, DEF_NU, m, *new CUnitTypeNone,
                                    IDT_VALUETYPE_POISSONRATIO_TRANS_DECOMP, new MP_GE_LT(0, 0.5), 0));
  Add(new CLibraryMaterialParameter(MLD_ANISOTROPIC_SHEARMODULUS, DEF_G, m, *new CUnitTypeStress,
                                    IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS, new MP_GT(0),
                                    new CMaterialAnisotropyShearStiffSetStrategy),
      GROUP_MANUAL);
  Add(new CLibraryMaterialParameter(MLD_ANISOTROPIC_SHEARMODULUS_DECOMP, DEF_G, m, *new CUnitTypeStress,
                                    IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS_DECOMP, new MP_GT(0), 0));
  Add(new CLibraryMaterialParameter(MLD_VELOCITYP, 0, m, *new CUnitTypeVelocity, IDT_VALUETYPE_VELOCITYP,
                                    new CMaterialAnisotropyVpCheckStrategy, new CMaterialAnisotropyVpSetStrategy),
      GROUP_THOMSEN);
  Add(new CLibraryMaterialParameter(MLD_VELOCITYS, 0, m, *new CUnitTypeVelocity, IDT_VALUETYPE_VELOCITYS,
                                    new CMaterialAnisotropyVsCheckStrategy, new CMaterialAnisotropyVsSetStrategy),
      GROUP_THOMSEN);
  Add(new CLibraryMaterialParameter(MLD_THOMSEN_EPSILON, 0, m, *new CUnitTypeNone, IDT_VALUETYPE_THOMSEN_EPSILON,
                                    new CMaterialAnisotropyThomsenEpsilonCheckStrategy,
                                    new CMaterialAnisotropyThomsenEpsilonSetStrategy),
      GROUP_THOMSEN);
  Add(new CLibraryMaterialParameter(MLD_THOMSEN_GAMMA, 0, m, *new CUnitTypeNone, IDT_VALUETYPE_THOMSEN_GAMMA,
                                    new CMaterialAnisotropyThomsenGammaCheckStrategy,
                                    new CMaterialAnisotropyThomsenGammaSetStrategy),
      GROUP_THOMSEN);
  Add(new CLibraryMaterialParameter(MLD_THOMSEN_DELTA, 0, m, *new CUnitTypeNone, IDT_VALUETYPE_THOMSEN_DELTA,
                                    new CMaterialAnisotropyThomsenDeltaCheckStrategy,
                                    new CMaterialAnisotropyThomsenDeltaSetStrategy),
      GROUP_THOMSEN);
  Add(new CLibraryMaterialParameter(MLD_LAYER_NORMAL_INCL, 0, m, *new CUnitTypeAngle, IDT_VALUETYPE_LAYER_NORMAL_INCL,
                                    new MP_GE_LE(0, 90), 0));
  Add(new CLibraryMaterialParameter(MLD_LAYER_NORMAL_AZI, 0, m, *new CUnitTypeAngle, IDT_VALUETYPE_LAYER_NORMAL_AZI,
                                    new MP_GE_LE(-360, 360), 0));
  Add(new CLibraryMaterialParameter(MLD_FRACT_STIFF_NORMAL, 1, m, *new CUnitTypeStressGradient,
                                    IDT_VALUETYPE_FRACT_STIFF_NORMAL, new MP_GE(1), 0));
  Add(new CLibraryMaterialParameter(MLD_FRACT_STIFF_SHEAR, 1, m, *new CUnitTypeStressGradient,
                                    IDT_VALUETYPE_FRACT_STIFF_SHEAR, new MP_GE(1), 0));
  Add(new CLibraryMaterialParameter(MLD_REFERENCESTRESS, DEF_REFERENCESTRESS, m, *new CUnitTypeStress,
                                    IDT_VALUETYPE_REFERENCESTRESS, new MP_GE_LE(0, 1000), 0));
  Add(new CLibraryMaterialParameter(MLD_APERTUREPARAM, DEF_APERTUREPARAM, m, *new CUnitTypePerStress,
                                    IDT_VALUETYPE_APERTUREPARAM, new MP_GE_LE(0, 1000), 0));
  Add(new CLibraryMaterialParameter(MLD_HIGH_FRACT_DENS, 0, m, *new CUnitTypePerDistance, IDT_VALUETYPE_HIGH_FRACT_DENS,
                                    new MP_GE(0), 0));
  Add(new CLibraryMaterialParameter(MLD_HIGH_FRACT_DENS_INCL, DEF_FRACT_INCL, m, *new CUnitTypeAngle,
                                    IDT_VALUETYPE_HIGH_FRACT_DENS_INCL, new MP_GE_LE(0, 90), 0));
  Add(new CLibraryMaterialParameter(MLD_HIGH_FRACT_DENS_AZI, 0, m, *new CUnitTypeAngle,
                                    IDT_VALUETYPE_HIGH_FRACT_DENS_AZI, new MP_GE_LE(-360, 360), 0));
  Add(new CLibraryMaterialParameter(MLD_LOW_FRACT_DENS, 0, m, *new CUnitTypePerDistance, IDT_VALUETYPE_LOW_FRACT_DENS,
                                    new MP_GE(0), 0));
  Add(new CLibraryMaterialParameter(MLD_LOW_FRACT_DENS_INCL, 0, m, *new CUnitTypeAngle,
                                    IDT_VALUETYPE_LOW_FRACT_DENS_INCL, new MP_GE_LE(0, 90), 0));
  Add(new CLibraryMaterialParameter(MLD_LOW_FRACT_DENS_AZI, 0, m, *new CUnitTypeAngle, IDT_VALUETYPE_LOW_FRACT_DENS_AZI,
                                    new MP_GE_LE(-360, 360), 0));
  Add(new CLibraryMaterialParameter(MLD_INTER_FRACT_DENS, 0, m, *new CUnitTypePerDistance,
                                    IDT_VALUETYPE_INTER_FRACT_DENS, new MP_GE(0), 0));
  Add(new CLibraryMaterialParameter(MLD_V0_HI, 0, m, *new CUnitTypeDepth, IDT_VALUETYPE_V0_HI, new MP_GE(0), 0));
  Add(new CLibraryMaterialParameter(MLD_V0_ME, 0, m, *new CUnitTypeDepth, IDT_VALUETYPE_V0_ME, new MP_GE(0), 0));
  Add(new CLibraryMaterialParameter(MLD_V0_LO, 0, m, *new CUnitTypeDepth, IDT_VALUETYPE_V0_LO, new MP_GE(0), 0));
  Add(new CLibraryMaterialParameter(MLD_THERMX, DEF_THERMX, m, *new CUnitTypePerTemper, IDT_VALUETYPE_THERMAL_EXPANSION,
                                    new MP_GE_LE(0, 1e-3), 0));
  Add(new CLibraryMaterialParameter(MLD_POROSITY, DEF_PORO, m, *new CUnitTypeNone, IDT_VALUETYPE_POROSITY,
                                    new MP_GE_LT(0, 1), 0));
  Add(new CLibraryMaterialParameter(MLD_GRAINSTIFFNESS, DEF_GRAINSTIFFNESS, m, *new CUnitTypeStress,
                                    IDT_VALUETYPE_GRAINSTIFFNESS, new MP_GE_LE(10, 1000000), 0));
  Add(new CLibraryMaterialParameter(MLD_PERMEA, DEF_PERMEA, m, *new CUnitTypePermea, IDT_VALUETYPE_PERMEA, new MP_GE(0),
                                    0));
  Add(new CLibraryMaterialParameter(MLD_VISCOSITY, DEF_VISCOSITY, m, *new CUnitTypeViscosity, IDT_VALUETYPE_VISCOSITY,
                                    new MP_GT(0), 0));
  Add(new CLibraryMaterialParameter(MLD_RHOB, DEF_RHOB, m, *new CUnitTypeDensity, IDT_VALUETYPE_RHOB, new MP_GT(0), 0));
  Add(new CLibraryMaterialParameter(MLD_LATRATIO_MIN, DEF_LATRATIO_MIN, m, *new CUnitTypeNone,
                                    IDT_VALUETYPE_LATRATIO_MIN, new CMaterialLinearLatRatioMinCheckStrategy, 0));
  Add(new CLibraryMaterialParameter(MLD_LATRATIO_MAX, DEF_LATRATIO_MAX, m, *new CUnitTypeNone,
                                    IDT_VALUETYPE_LATRATIO_MAX, new CMaterialLinearLatRatioMaxCheckStrategy, 0));
  Add(new CLibraryMaterialParameter(MLD_AZIMUTH, DEF_AZIMUTH, m, *new CUnitTypeAngle, IDT_VALUETYPE_AZIMUTH,
                                    new MP_GE_LE(0, 360), 0));
  Add(new CLibraryMaterialParameter(MLD_COHESION, DEF_COHESION, m, *new CUnitTypeStress, IDT_VALUETYPE_COHESION,
                                    new MP_GE(0), 0));
  Add(new CLibraryMaterialParameter(MLD_FRICTIONANGLE, DEF_FRICTION, m, *new CUnitTypeAngle,
                                    IDT_VALUETYPE_FRICTION_ANGLE, new MP_GE_LT(0, 70), 0));
  Add(new CLibraryMaterialParameter(MLD_THERM_CONDUCT, DEF_THERM_CONDUCT, m, *new CUnitTypeThermConduct,
                                    IDT_VALUETYPE_THERM_CONDUCT, new MP_GE(0), 0));
  Add(new CLibraryMaterialParameter(MLD_THERM_CAPACI, DEF_THERM_CAPACI, m, *new CUnitTypeThermCapacity,
                                    IDT_VALUETYPE_THERM_CAPACI, new MP_GE(0), 0));
  Add(new CLibraryMaterialParameter(MLD_FLUIDMODULUS, DEF_FLUIDMOD, m, *new CUnitTypeStress,
                                    IDT_VALUETYPE_FLUID_BULK_MOD, new MP_GT(0), 0));
  Add(new CLibraryMaterialParameter(MLD_FLUIDX, DEF_FLUIDX, m, *new CUnitTypePerTemper, IDT_VALUETYPE_FLUIDX,
                                    new MP_GE_LE(0, 0.1), 0));
  Add(new CLibraryMaterialParameter(MLD_FLUID_DENSITY, DEF_FLUID_DENSITY, m, *new CUnitTypeDensity,
                                    IDT_VALUETYPE_FLUID_DENSITY, new MP_GT_LE(0, 10000), 0));
}

int CMaterialFractureApertureCreatorDEPRECATED::MaterialModel() { return MM_FRACTURE_APERTURE; }

QString CMaterialFractureApertureCreatorDEPRECATED::MaterialModelName() { return MLD_FRACTURE_APERTURE; }

QString CMaterialFractureApertureCreatorDEPRECATED::CalibrationPath() const { return QString(); }
