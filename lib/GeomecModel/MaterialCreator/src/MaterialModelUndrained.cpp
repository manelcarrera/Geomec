
#include <cmath>

#include "MaterialModelUndrained.h"

#include "LibraryMaterial.h"
#include "LibraryMaterialParameter.h"
#include "MaterialHelperFactory.h"
#include "MaterialUnitTypes.h"
#include "Materials.h"
#include "ValueTypes.h"
#include "mlMatParamDomain.h"

#include <cassert>

namespace {

const QString GROUP_DRAINED = "Drained";
const QString GROUP_UNDRAINED = "Undrained";
const QString GROUP_BULK = "Bulk stiffness";
const QString GROUP_DYNAMIC = "Dynamic stiffness";
const QString GROUP_VELOCITIES = "Velocities";
const QString GROUP_COMPRESSIBILITY = "Compressibility";

} // namespace

void CMaterialUndrainedCreatorV2::OnCreateLibrary(CLibraryMaterial &m) { m.BehavesLinearly(true); }

void CMaterialUndrainedCreatorV2::OnCreateLibraryParameters(CLibraryMaterial &m) {
  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();
  assert(f);

  AddNumber(m, 30);

  AddLibParam(MLD_YOUNGMODULUS, DEF_E, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_YOUNGS_MODULUS, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_UndrainedYoung), YOUNG_FILOSPATH, false,
              GROUP_DRAINED);
  AddLibParam(MLD_YOUNGMODULUS_DECOMP, DEF_E, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_YOUNG_DECOMP, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0), 0, QString(), false);
  AddLibParam(MLD_POISSONRATIO, DEF_NU, m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),
              IDT_VALUETYPE_POISSONS_RATIO, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LT_5Emin1),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_UndrainedPoisson), POISON_FILOSPATH, false,
              GROUP_DRAINED);
  AddLibParam(MLD_POISSONRATIO_DECOMP, DEF_NU, m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),
              IDT_VALUETYPE_POISSON_DECOMP, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LT_5Emin1), 0,
              QString(), false);
  AddLibParam(MLD_UNDRAINEDYOUNG, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_UNDRAINEDYOUNG, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_UndrainedUndrainedYoung), QString(), false,
              GROUP_UNDRAINED);
  AddLibParam(MLD_UNDRAINEDPOISSON, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),
              IDT_VALUETYPE_UNDRAINEDPOISSON, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LT_5Emin1),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_UndrainedUndrainedPoisson), QString(), false,
              GROUP_UNDRAINED);
  AddLibParam(MLD_UNDRAINEDTHERMX, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_PER_TEMPER),
              IDT_VALUETYPE_UNDRAINEDTHERMX, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LE_2Emin1),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_UndrainedUndrainedThermalExpansionCoefficient),
              QString(), false, GROUP_UNDRAINED);
  AddLibParam(MLD_BULKSTIFFNESS, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS), IDT_VALUETYPE_BULKSTIFFNESS,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearBulkStiff),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_LinearBulkStiff), QString(), false, GROUP_BULK);
  AddLibParam(MLD_SHEARMODULUS, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS), IDT_VALUETYPE_SHEARMODULUS,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearShearStiff),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_LinearShearStiff), QString(), false, GROUP_BULK);
  AddLibParam(MLD_DYNUNISTIFFNESS, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_DYNUNISTIFFNESS, f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearDynUni),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_LinearDynUni), QString(), false, GROUP_DYNAMIC);

  AddLibParam(MLD_DYNSHEARSTIFFNESS, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_DYNSHEARSTIFFNESS, f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearDynShear),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_LinearDynShear), QString(), false, GROUP_DYNAMIC);
  AddLibParam(MLD_VELOCITYP, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_VELOCITY), IDT_VALUETYPE_VELOCITYP,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearVelocityP),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_LinearVelocityP), QString(), false, GROUP_VELOCITIES);
  AddLibParam(MLD_VELOCITYS, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_VELOCITY), IDT_VALUETYPE_VELOCITYS,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearVelocityS),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_LinearVelocityS), QString(), false, GROUP_VELOCITIES);
  AddLibParam(MLD_CME, DEF_CME, m, f->getMPUnitType(CMaterialHelperFactory::UT_PER_STRESS), IDT_VALUETYPE_ELASCOMPRES,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_LinearCME), QString(), false, GROUP_COMPRESSIBILITY);
  AddLibParam(MLD_RHOB, DEF_RHOB, m, f->getMPUnitType(CMaterialHelperFactory::UT_DENSITY), IDT_VALUETYPE_RHOB,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_LinearRhoB), QString(), false);
  AddLibParam(MLD_COHESION, DEF_COHESION, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_COHESION, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0), 0, QString(), false);
  AddLibParam(MLD_FRICTIONANGLE, DEF_FRICTION, m, f->getMPUnitType(CMaterialHelperFactory::UT_ANGLE),
              IDT_VALUETYPE_FRICTION_ANGLE, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LT_70), 0, QString(),
              false);
  AddLibParam(MLD_POROSITY, DEF_PORO, m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE), IDT_VALUETYPE_POROSITY,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LT_1),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_UndrainedPorosity), QString(), false);
  AddLibParam(MLD_GRAINSTIFFNESS, DEF_GRAINSTIFFNESS, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_GRAINSTIFFNESS, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_10_LE_1E6),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_UndrainedGrainStiffness), BULKS_FILOSPATH, true);
  AddLibParam(MLD_PERMEA, DEF_SALTCREEP_UNDRAINED_PERMEA, m, f->getMPUnitType(CMaterialHelperFactory::UT_PERMEA),
              IDT_VALUETYPE_PERMEA, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0), 0, QString(), false);

  AddLibParam(MLD_VISCOSITY, DEF_VISCOSITY, m, f->getMPUnitType(CMaterialHelperFactory::UT_VISCOSITY),
              IDT_VALUETYPE_VISCOSITY, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0), 0, QString(), false);
  AddLibParam(MLD_FLUIDMODULUS, DEF_SALTCREEP_FLUIDMOD, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_FLUID_BULK_MOD, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_UndrainedFluidBulkModulus), QString(), false);
  AddLibParam(MLD_LATRATIO_MIN, DEF_LATRATIO_MIN, m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),
              IDT_VALUETYPE_LATRATIO_MIN, f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearLatRatioMin), 0,
              QString(), false);
  AddLibParam(MLD_LATRATIO_MAX, DEF_LATRATIO_MAX, m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),
              IDT_VALUETYPE_LATRATIO_MAX, f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearLatRatioMax), 0,
              QString(), false);
  AddLibParam(MLD_AZIMUTH, DEF_AZIMUTH, m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE), IDT_VALUETYPE_AZIMUTH,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LE_360), 0, QString(), false);
  AddLibParam(MLD_THERMX, DEF_THERMX, m, f->getMPUnitType(CMaterialHelperFactory::UT_PER_TEMPER),
              IDT_VALUETYPE_THERMAL_EXPANSION, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LE_1Emin3),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_UndrainedVolumetricThermalExpansionCoefficient),
              QString(), false);
  AddLibParam(MLD_THERM_CONDUCT, DEF_THERM_CONDUCT, m, f->getMPUnitType(CMaterialHelperFactory::UT_THERM_CONDUCT),
              IDT_VALUETYPE_THERM_CONDUCT, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0), 0, QString(), false);
  AddLibParam(MLD_THERM_CAPACI, DEF_THERM_CAPACI, m, f->getMPUnitType(CMaterialHelperFactory::UT_THERM_CAPACITY),
              IDT_VALUETYPE_THERM_CAPACI, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0), 0, QString(), false);
  AddLibParam(MLD_FLUIDX, DEF_FLUIDX, m, f->getMPUnitType(CMaterialHelperFactory::UT_PER_TEMPER), IDT_VALUETYPE_FLUIDX,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LE_1Emin1),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_UndrainedFluidThermalExpansionCoefficient), QString(),
              false);
  AddLibParam(MLD_FLUID_DENSITY, DEF_FLUID_DENSITY, m, f->getMPUnitType(CMaterialHelperFactory::UT_DENSITY),
              IDT_VALUETYPE_FLUID_DENSITY, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0_LE_10000), 0,
              QString(), false);
}

int CMaterialUndrainedCreatorV2::MaterialModel() { return MM_UNDRAINED; }

QString CMaterialUndrainedCreatorV2::MaterialModelName() { return MLD_UNDRAINED; }

QString CMaterialUndrainedCreatorV2::CalibrationPath() const {
  // undrained uses linear calibration
  return "LINEAR";
}

void CMaterialUndrainedCreatorDEPRECATED::OnCreateLibrary(CLibraryMaterial & /*m*/) {}

void CMaterialUndrainedCreatorDEPRECATED::OnCreateLibraryParameters(CLibraryMaterial &m) {
  Add(new CLibraryMaterialParameter(MLD_YOUNGMODULUS, DEF_E, m, *new CUnitTypeStress, IDT_VALUETYPE_YOUNGS_MODULUS,
                                    new MP_GT(0), new CMaterialUndrainedYoungSetStrategy, YOUNG_FILOSPATH, false),
      GROUP_DRAINED);
  Add(new CLibraryMaterialParameter(MLD_YOUNGMODULUS_DECOMP, DEF_E, m, *new CUnitTypeStress, IDT_VALUETYPE_YOUNG_DECOMP,
                                    new MP_GT(0), 0));
  Add(new CLibraryMaterialParameter(MLD_POISSONRATIO, DEF_NU, m, *new CUnitTypeNone, IDT_VALUETYPE_POISSONS_RATIO,
                                    new MP_GE_LT(0, 0.5), new CMaterialUndrainedPoissonSetStrategy, POISON_FILOSPATH,
                                    false),
      GROUP_DRAINED);
  Add(new CLibraryMaterialParameter(MLD_POISSONRATIO_DECOMP, DEF_NU, m, *new CUnitTypeNone,
                                    IDT_VALUETYPE_POISSON_DECOMP, new MP_GE_LT(0, 0.5), 0));
  Add(new CLibraryMaterialParameter(MLD_UNDRAINEDYOUNG, 0, m, *new CUnitTypeStress, IDT_VALUETYPE_UNDRAINEDYOUNG,
                                    new MP_GT(0), new CMaterialUndrainedUndrainedYoungSetStrategy),
      GROUP_UNDRAINED);
  Add(new CLibraryMaterialParameter(MLD_UNDRAINEDPOISSON, 0, m, *new CUnitTypeNone, IDT_VALUETYPE_UNDRAINEDPOISSON,
                                    new MP_GE_LT(0, 0.5), new CMaterialUndrainedUndrainedPoissonSetStrategy),
      GROUP_UNDRAINED);
  Add(new CLibraryMaterialParameter(MLD_UNDRAINEDTHERMX, 0, m, *new CUnitTypePerTemper, IDT_VALUETYPE_UNDRAINEDTHERMX,
                                    new MP_GE_LE(0, 0.2),
                                    new CMaterialUndrainedUndrainedThermalExpansionCoefficientSetStrategy),
      GROUP_UNDRAINED);
  Add(new CLibraryMaterialParameter(MLD_BULKSTIFFNESS, 0, m, *new CUnitTypeStress, IDT_VALUETYPE_BULKSTIFFNESS,
                                    new CMaterialLinearBulkStiffCheckStrategy, new CMaterialLinearBulkStiffSetStrategy),
      GROUP_BULK);
  Add(new CLibraryMaterialParameter(MLD_SHEARMODULUS, 0, m, *new CUnitTypeStress, IDT_VALUETYPE_SHEARMODULUS,
                                    new CMaterialLinearShearStiffCheckStrategy,
                                    new CMaterialLinearShearStiffSetStrategy),
      GROUP_BULK);
  Add(new CLibraryMaterialParameter(MLD_DYNUNISTIFFNESS, 0, m, *new CUnitTypeStress, IDT_VALUETYPE_DYNUNISTIFFNESS,
                                    new CMaterialLinearDynUniCheckStrategy, new CMaterialLinearDynUniSetStrategy),
      GROUP_DYNAMIC);
  Add(new CLibraryMaterialParameter(MLD_DYNSHEARSTIFFNESS, 0, m, *new CUnitTypeStress, IDT_VALUETYPE_DYNSHEARSTIFFNESS,
                                    new CMaterialLinearDynShearCheckStrategy, new CMaterialLinearDynShearSetStrategy),
      GROUP_DYNAMIC);
  Add(new CLibraryMaterialParameter(MLD_VELOCITYP, 0, m, *new CUnitTypeVelocity, IDT_VALUETYPE_VELOCITYP,
                                    new CMaterialLinearVelocityPCheckStrategy, new CMaterialLinearVelocityPSetStrategy),
      GROUP_VELOCITIES);
  Add(new CLibraryMaterialParameter(MLD_VELOCITYS, 0, m, *new CUnitTypeVelocity, IDT_VALUETYPE_VELOCITYS,
                                    new CMaterialLinearVelocitySCheckStrategy, new CMaterialLinearVelocitySSetStrategy),
      GROUP_VELOCITIES);
  Add(new CLibraryMaterialParameter(MLD_CME, DEF_CME, m, *new CUnitTypePerStress, IDT_VALUETYPE_ELASCOMPRES,
                                    new MP_GT(0), new CMaterialLinearCMESetStrategy));
  Add(new CLibraryMaterialParameter(MLD_RHOB, DEF_RHOB, m, *new CUnitTypeDensity, IDT_VALUETYPE_RHOB, new MP_GT(0),
                                    new CMaterialLinearRhoBSetStrategy));
  Add(new CLibraryMaterialParameter(MLD_COHESION, DEF_COHESION, m, *new CUnitTypeStress, IDT_VALUETYPE_COHESION,
                                    new MP_GE(0), 0));
  Add(new CLibraryMaterialParameter(MLD_FRICTIONANGLE, DEF_FRICTION, m, *new CUnitTypeAngle,
                                    IDT_VALUETYPE_FRICTION_ANGLE, new MP_GE_LT(0, 70), 0));
  Add(new CLibraryMaterialParameter(MLD_POROSITY, DEF_PORO, m, *new CUnitTypeNone, IDT_VALUETYPE_POROSITY,
                                    new MP_GE_LT(0, 1), new CMaterialUndrainedPorositySetStrategy));
  Add(new CLibraryMaterialParameter(MLD_GRAINSTIFFNESS, DEF_GRAINSTIFFNESS, m, *new CUnitTypeStress,
                                    IDT_VALUETYPE_GRAINSTIFFNESS, new MP_GE_LE(10, 1000000),
                                    new CMaterialUndrainedGrainStiffnessSetStrategy, BULKS_FILOSPATH, true));
  Add(new CLibraryMaterialParameter(MLD_PERMEA, DEF_SALTCREEP_UNDRAINED_PERMEA, m, *new CUnitTypePermea,
                                    IDT_VALUETYPE_PERMEA, new MP_GE(0), 0));
  Add(new CLibraryMaterialParameter(MLD_VISCOSITY, DEF_VISCOSITY, m, *new CUnitTypeViscosity, IDT_VALUETYPE_VISCOSITY,
                                    new MP_GT(0), 0));
  Add(new CLibraryMaterialParameter(MLD_FLUIDMODULUS, DEF_SALTCREEP_FLUIDMOD, m, *new CUnitTypeStress,
                                    IDT_VALUETYPE_FLUID_BULK_MOD, new MP_GT(0),
                                    new CMaterialUndrainedFluidBulkModulusSetStrategy));
  Add(new CLibraryMaterialParameter(MLD_LATRATIO_MIN, DEF_LATRATIO_MIN, m, *new CUnitTypeNone,
                                    IDT_VALUETYPE_LATRATIO_MIN, new CMaterialLinearLatRatioMinCheckStrategy, 0));
  Add(new CLibraryMaterialParameter(MLD_LATRATIO_MAX, DEF_LATRATIO_MAX, m, *new CUnitTypeNone,
                                    IDT_VALUETYPE_LATRATIO_MAX, new CMaterialLinearLatRatioMaxCheckStrategy, 0));
  Add(new CLibraryMaterialParameter(MLD_AZIMUTH, DEF_AZIMUTH, m, *new CUnitTypeNone, IDT_VALUETYPE_AZIMUTH,
                                    new MP_GE_LE(0, 360), 0));
  Add(new CLibraryMaterialParameter(MLD_THERMX, DEF_THERMX, m, *new CUnitTypePerTemper, IDT_VALUETYPE_THERMAL_EXPANSION,
                                    new MP_GE_LE(0, 1e-3),
                                    new CMaterialUndrainedVolumetricThermalExpansionCoefficientSetStrategy));
  Add(new CLibraryMaterialParameter(MLD_THERM_CONDUCT, DEF_THERM_CONDUCT, m, *new CUnitTypeThermConduct,
                                    IDT_VALUETYPE_THERM_CONDUCT, new MP_GE(0), 0));
  Add(new CLibraryMaterialParameter(MLD_THERM_CAPACI, DEF_THERM_CAPACI, m, *new CUnitTypeThermCapacity,
                                    IDT_VALUETYPE_THERM_CAPACI, new MP_GE(0), 0));
  Add(new CLibraryMaterialParameter(MLD_FLUIDX, DEF_FLUIDX, m, *new CUnitTypePerTemper, IDT_VALUETYPE_FLUIDX,
                                    new MP_GE_LE(0, 0.1),
                                    new CMaterialUndrainedFluidThermalExpansionCoefficientSetStrategy));
  Add(new CLibraryMaterialParameter(MLD_FLUID_DENSITY, DEF_FLUID_DENSITY, m, *new CUnitTypeDensity,
                                    IDT_VALUETYPE_FLUID_DENSITY, new MP_GT_LE(0, 10000), 0));
}

int CMaterialUndrainedCreatorDEPRECATED::MaterialModel() { return MM_UNDRAINED; }

QString CMaterialUndrainedCreatorDEPRECATED::MaterialModelName() { return MLD_UNDRAINED; }

QString CMaterialUndrainedCreatorDEPRECATED::CalibrationPath() const {
  // undrained uses linear calibration
  return "LINEAR";
}

static double Alpha(double Kb, double Kg) { return 1 - Kb / Kg; }

static double Theta(double phi, double alpha, double Kb, double Kg, double Kf) {
  return 1 / (3 - 3 * phi / alpha * Kb * (1 / Kg - 1 / Kf));
}

static double UndrainedYoung(double E, double alpha, double nu, double theta) {
  return E / (1 - alpha * (1 - 2 * nu) * theta);
}

static double UndrainedPoisson(double nu, double alpha, double theta, double Eu, double E) {
  return (nu + alpha * (1 - 2 * nu) * theta) * Eu / E;
}

static double DrainedYoung(double Eu, double alpha, double nu, double theta) {
  return Eu / (1 - alpha * (1 - 2 * nu) * theta);
}

static double DrainedPoisson(double alpha, double theta, double nuu) {
  return (alpha * theta * (nuu + 1) - nuu) / (2 * alpha * theta * (nuu + 1) - 1);
}

static double VolumetricThermalExpansionCoefficient(double Bu, double phi, double Bf, double Eu, double Kb, double Kg,
                                                    double Kf) {
  double alpha = Alpha(Kb, Kg);
  double Bb;

  if ((fabs(Kf) < 1E-6) || (fabs(Kg) < 1E-6)) {
    Bb = Bu;
  } else {
    double divisor = 1 + (phi * (Kb / alpha) * ((1 / Kg) - (1 / Kf)));

    Bb = (Bu - ((phi * Bf * Eu) / divisor)) / (1 - ((phi * Eu) / divisor));
  }

  return Bb;
}

static double UndrainedThermalExpansionCoefficient(double Bb, double phi, double Bf, double Eu, double Kb, double alpha,
                                                   double Kg, double Kf) {
  return Bb + ((phi * (Bf - Bb) * Eu) / (1 + (phi * (Kb / alpha) * ((1 / Kg) - (1 / Kf)))));
}

void CMaterialUndrainedYoungSetStrategy::operator()(double dValue, ml::CMatParam &param) const {
  ml::CMaterial &mat = param.ParentMaterial();

  const ml::CMatParam *pPoisson = mat.MatParameter(MLD_POISSONRATIO);
  assert(pPoisson);

  const ml::CMatParam *pBulkFluid = mat.MatParameter(MLD_FLUIDMODULUS);
  assert(pBulkFluid);

  const ml::CMatParam *pPorosity = mat.MatParameter(MLD_POROSITY);
  assert(pPorosity);

  const ml::CMatParam *pBulkModulus = mat.MatParameter(MLD_BULKSTIFFNESS);
  assert(pBulkModulus);

  const ml::CMatParam *pGrainStiffness = mat.MatParameter(MLD_GRAINSTIFFNESS);
  assert(pGrainStiffness);

  double phi = pPorosity->Value();
  double Kb = pBulkModulus->Value();
  double Kg = pGrainStiffness->Value();
  double Kf = pBulkFluid->Value();
  double nu = pPoisson->Value();
  double E = dValue;

  double alpha = Alpha(Kb, Kg);
  double theta = Theta(phi, alpha, Kb, Kg, Kf);

  double Eu = UndrainedYoung(E, alpha, nu, theta);
  double nuu = UndrainedPoisson(nu, alpha, theta, Eu, E);

  // set the parameters
  ml::CMatParam *pParam;

  if ((pParam = mat.MatParameter(MLD_UNDRAINEDYOUNG)))
    SetParamValue(*pParam, Eu);

  if ((pParam = mat.MatParameter(MLD_UNDRAINEDPOISSON)))
    SetParamValue(*pParam, nuu);

  CMaterialLinearYoungSetStrategy::operator()(dValue, param);
}

void CMaterialUndrainedPoissonSetStrategy::operator()(double dValue, ml::CMatParam &param) const {
  ml::CMaterial &mat = param.ParentMaterial();

  const ml::CMatParam *pYoung = mat.MatParameter(MLD_YOUNGMODULUS);
  assert(pYoung);

  const ml::CMatParam *pBulkFluid = mat.MatParameter(MLD_FLUIDMODULUS);
  assert(pBulkFluid);

  const ml::CMatParam *pPorosity = mat.MatParameter(MLD_POROSITY);
  assert(pPorosity);

  const ml::CMatParam *pBulkModulus = mat.MatParameter(MLD_BULKSTIFFNESS);
  assert(pBulkModulus);

  const ml::CMatParam *pGrainStiffness = mat.MatParameter(MLD_GRAINSTIFFNESS);
  assert(pGrainStiffness);

  double phi = pPorosity->Value();
  double Kb = pBulkModulus->Value();
  double Kg = pGrainStiffness->Value();
  double Kf = pBulkFluid->Value();
  double nu = dValue;
  double E = pYoung->Value();

  double alpha = Alpha(Kb, Kg);
  double theta = Theta(phi, alpha, Kb, Kg, Kf);

  double Eu = UndrainedYoung(E, alpha, nu, theta);
  double nuu = UndrainedPoisson(nu, alpha, theta, Eu, E);

  // set the parameters
  ml::CMatParam *pParam;

  if ((pParam = mat.MatParameter(MLD_UNDRAINEDYOUNG)))
    SetParamValue(*pParam, Eu);

  if ((pParam = mat.MatParameter(MLD_UNDRAINEDPOISSON)))
    SetParamValue(*pParam, nuu);

  CMaterialLinearPoissonSetStrategy::operator()(dValue, param);
}

void CMaterialUndrainedUndrainedYoungSetStrategy::operator()(double dValue, ml::CMatParam &param) const {
  ml::CMaterial &mat = param.ParentMaterial();

  const ml::CMatParam *pUndrainedPoisson = mat.MatParameter(MLD_UNDRAINEDPOISSON);
  assert(pUndrainedPoisson);

  const ml::CMatParam *pBulkFluid = mat.MatParameter(MLD_FLUIDMODULUS);
  assert(pBulkFluid);

  const ml::CMatParam *pPorosity = mat.MatParameter(MLD_POROSITY);
  assert(pPorosity);

  const ml::CMatParam *pUndrainedYoung = mat.MatParameter(MLD_UNDRAINEDYOUNG);
  assert(pUndrainedYoung);

  const ml::CMatParam *pBulkModulus = mat.MatParameter(MLD_BULKSTIFFNESS);
  assert(pBulkModulus);

  const ml::CMatParam *pGrainStiffness = mat.MatParameter(MLD_GRAINSTIFFNESS);
  assert(pGrainStiffness);

  const ml::CMatParam *pVolumetricThermalExpansionCoefficient = mat.MatParameter(MLD_THERMX);
  assert(pVolumetricThermalExpansionCoefficient);

  const ml::CMatParam *pFluidThermalExpansionCoefficient = mat.MatParameter(MLD_FLUIDX);
  assert(pFluidThermalExpansionCoefficient);

  double phi = pPorosity->Value();
  double Kb = pBulkModulus->Value();
  double Kg = pGrainStiffness->Value();
  double Kf = pBulkFluid->Value();
  double nuu = pUndrainedPoisson->Value();
  double Eu = dValue;
  double Bb = pVolumetricThermalExpansionCoefficient->Value();
  double Bf = pFluidThermalExpansionCoefficient->Value();

  double alpha = Alpha(Kb, Kg);
  double theta = Theta(phi, alpha, Kb, Kg, Kf);
  double nu = DrainedPoisson(alpha, theta, nuu);
  double E = DrainedYoung(Eu, alpha, nu, theta);
  double Bu = UndrainedThermalExpansionCoefficient(Bb, phi, Bf, Eu, Kb, alpha, Kg, Kf);

  // set the parameters
  ml::CMatParam *pParam;

  if ((pParam = mat.MatParameter(MLD_YOUNGMODULUS)))
    SetParamValue(*pParam, E);

  if ((pParam = mat.MatParameter(MLD_POISSONRATIO)))
    SetParamValue(*pParam, nu);

  if ((pParam = mat.MatParameter(MLD_UNDRAINEDTHERMX))) {
    SetParamValue(*pParam, Bu);
  }

  SetParamValue(param, Eu);
}

void CMaterialUndrainedUndrainedPoissonSetStrategy::operator()(double dValue, ml::CMatParam &param) const {
  ml::CMaterial &mat = param.ParentMaterial();

  const ml::CMatParam *pUndrainedYoung = mat.MatParameter(MLD_UNDRAINEDYOUNG);
  assert(pUndrainedYoung);

  const ml::CMatParam *pBulkFluid = mat.MatParameter(MLD_FLUIDMODULUS);
  assert(pBulkFluid);

  const ml::CMatParam *pPorosity = mat.MatParameter(MLD_POROSITY);
  assert(pPorosity);

  const ml::CMatParam *pUndrainedPoisson = mat.MatParameter(MLD_UNDRAINEDPOISSON);
  assert(pUndrainedPoisson);

  const ml::CMatParam *pBulkModulus = mat.MatParameter(MLD_BULKSTIFFNESS);
  assert(pBulkModulus);

  const ml::CMatParam *pGrainStiffness = mat.MatParameter(MLD_GRAINSTIFFNESS);
  assert(pGrainStiffness);

  double Eu = pUndrainedYoung->Value();
  double Kb = pBulkModulus->Value();
  double Kg = pGrainStiffness->Value();
  double Kf = pBulkFluid->Value();
  double phi = pPorosity->Value();
  double nuu = dValue;

  double alpha = Alpha(Kb, Kg);
  double theta = Theta(phi, alpha, Kb, Kg, Kf);
  double nu = DrainedPoisson(alpha, theta, nuu);
  double E = DrainedYoung(Eu, alpha, nu, theta);

  // set the parameters
  ml::CMatParam *pParam;

  if ((pParam = mat.MatParameter(MLD_POISSONRATIO)))
    SetParamValue(*pParam, nu);

  if ((pParam = mat.MatParameter(MLD_YOUNGMODULUS)))
    SetParamValue(*pParam, E);

  SetParamValue(param, nuu);
}

void CMaterialUndrainedUndrainedThermalExpansionCoefficientSetStrategy::operator()(double dValue,
                                                                                   ml::CMatParam &param) const {
  ml::CMaterial &mat = param.ParentMaterial();

  const ml::CMatParam *pPorosity = mat.MatParameter(MLD_POROSITY);
  assert(pPorosity);

  const ml::CMatParam *pFluidThermalExpansionCoefficient = mat.MatParameter(MLD_FLUIDX);
  assert(pFluidThermalExpansionCoefficient);

  const ml::CMatParam *pUndrainedYoungsModulus = mat.MatParameter(MLD_UNDRAINEDYOUNG);
  assert(pUndrainedYoungsModulus);

  const ml::CMatParam *pBulkStiffness = mat.MatParameter(MLD_BULKSTIFFNESS);
  assert(pBulkStiffness);

  const ml::CMatParam *pGrainStiffness = mat.MatParameter(MLD_GRAINSTIFFNESS);
  assert(pGrainStiffness);

  const ml::CMatParam *pFluidBulkModulus = mat.MatParameter(MLD_FLUIDMODULUS);
  assert(pFluidBulkModulus);

  double Bu = dValue;
  double phi = pPorosity->Value();
  double Bf = pFluidThermalExpansionCoefficient->Value();
  double Eu = pUndrainedYoungsModulus->Value();
  double Kb = pBulkStiffness->Value();
  double Kg = pGrainStiffness->Value();
  double Kf = pFluidBulkModulus->Value();
  double Bb = VolumetricThermalExpansionCoefficient(Bu, phi, Bf, Eu, Kb, Kg, Kf);

  ml::CMatParam *pParam;

  if ((pParam = mat.MatParameter(MLD_THERMX))) {
    SetParamValue(*pParam, Bb);
  }

  SetParamValue(param, Bu);
}

void CMaterialUndrainedPorositySetStrategy::operator()(double dValue, ml::CMatParam &param) const {
  ml::CMaterial &mat = param.ParentMaterial();

  const ml::CMatParam *pYoung = mat.MatParameter(MLD_YOUNGMODULUS);
  assert(pYoung);

  const ml::CMatParam *pPoisson = mat.MatParameter(MLD_POISSONRATIO);
  assert(pPoisson);

  const ml::CMatParam *pBulkFluid = mat.MatParameter(MLD_FLUIDMODULUS);
  assert(pBulkFluid);

  const ml::CMatParam *pPorosity = mat.MatParameter(MLD_POROSITY);
  assert(pPorosity);

  const ml::CMatParam *pBulkModulus = mat.MatParameter(MLD_BULKSTIFFNESS);
  assert(pBulkModulus);

  const ml::CMatParam *pGrainStiffness = mat.MatParameter(MLD_GRAINSTIFFNESS);
  assert(pGrainStiffness);

  const ml::CMatParam *pVolumetricThermalExpansionCoefficient = mat.MatParameter(MLD_THERMX);
  assert(pVolumetricThermalExpansionCoefficient);

  const ml::CMatParam *pFluidThermalExpansionCoefficient = mat.MatParameter(MLD_FLUIDX);
  assert(pFluidThermalExpansionCoefficient);

  double E = pYoung->Value();
  double phi = dValue;
  double Kb = pBulkModulus->Value();
  double Kg = pGrainStiffness->Value();
  double Kf = pBulkFluid->Value();
  double nu = pPoisson->Value();
  double Bb = pVolumetricThermalExpansionCoefficient->Value();
  double Bf = pFluidThermalExpansionCoefficient->Value();

  double alpha = Alpha(Kb, Kg);
  double theta = Theta(phi, alpha, Kb, Kg, Kf);

  double Eu = UndrainedYoung(E, alpha, nu, theta);
  double nuu = UndrainedPoisson(nu, alpha, theta, Eu, E);
  double Bu = UndrainedThermalExpansionCoefficient(Bb, phi, Bf, Eu, Kb, alpha, Kg, Kf);

  // set the parameters
  ml::CMatParam *pParam;

  if ((pParam = mat.MatParameter(MLD_UNDRAINEDYOUNG)))
    SetParamValue(*pParam, Eu);

  if ((pParam = mat.MatParameter(MLD_UNDRAINEDPOISSON)))
    SetParamValue(*pParam, nuu);

  if ((pParam = mat.MatParameter(MLD_UNDRAINEDTHERMX))) {
    SetParamValue(*pParam, Bu);
  }

  SetParamValue(param, phi);
}

void CMaterialUndrainedGrainStiffnessSetStrategy::operator()(double dValue, ml::CMatParam &param) const {
  ml::CMaterial &mat = param.ParentMaterial();

  const ml::CMatParam *pYoung = mat.MatParameter(MLD_YOUNGMODULUS);
  assert(pYoung);

  const ml::CMatParam *pPoisson = mat.MatParameter(MLD_POISSONRATIO);
  assert(pPoisson);

  const ml::CMatParam *pBulkFluid = mat.MatParameter(MLD_FLUIDMODULUS);
  assert(pBulkFluid);

  const ml::CMatParam *pPorosity = mat.MatParameter(MLD_POROSITY);
  assert(pPorosity);

  const ml::CMatParam *pBulkModulus = mat.MatParameter(MLD_BULKSTIFFNESS);
  assert(pBulkModulus);

  const ml::CMatParam *pGrainStiffness = mat.MatParameter(MLD_GRAINSTIFFNESS);
  assert(pGrainStiffness);

  const ml::CMatParam *pVolumetricThermalExpansionCoefficient = mat.MatParameter(MLD_THERMX);
  assert(pVolumetricThermalExpansionCoefficient);

  const ml::CMatParam *pFluidThermalExpansionCoefficient = mat.MatParameter(MLD_FLUIDX);
  assert(pFluidThermalExpansionCoefficient);

  double E = pYoung->Value();
  double phi = pPorosity->Value();
  double Kb = pBulkModulus->Value();
  double Kg = dValue;
  double Kf = pBulkFluid->Value();
  double nu = pPoisson->Value();
  double Bb = pVolumetricThermalExpansionCoefficient->Value();
  double Bf = pFluidThermalExpansionCoefficient->Value();

  double alpha = Alpha(Kb, Kg);
  double theta = Theta(phi, alpha, Kb, Kg, Kf);

  double Eu = UndrainedYoung(E, alpha, nu, theta);
  double nuu = UndrainedPoisson(nu, alpha, theta, Eu, E);
  double Bu = UndrainedThermalExpansionCoefficient(Bb, phi, Bf, Eu, Kb, alpha, Kg, Kf);

  // set the parameters
  ml::CMatParam *pParam;

  if ((pParam = mat.MatParameter(MLD_UNDRAINEDYOUNG)))
    SetParamValue(*pParam, Eu);

  if ((pParam = mat.MatParameter(MLD_UNDRAINEDPOISSON)))
    SetParamValue(*pParam, nuu);

  if ((pParam = mat.MatParameter(MLD_UNDRAINEDTHERMX))) {
    SetParamValue(*pParam, Bu);
  }

  SetParamValue(param, Kg);
}

void CMaterialUndrainedFluidBulkModulusSetStrategy::operator()(double dValue, ml::CMatParam &param) const {
  ml::CMaterial &mat = param.ParentMaterial();

  const ml::CMatParam *pYoung = mat.MatParameter(MLD_YOUNGMODULUS);
  assert(pYoung);

  const ml::CMatParam *pPoisson = mat.MatParameter(MLD_POISSONRATIO);
  assert(pPoisson);

  const ml::CMatParam *pBulkFluid = mat.MatParameter(MLD_FLUIDMODULUS);
  assert(pBulkFluid);

  const ml::CMatParam *pPorosity = mat.MatParameter(MLD_POROSITY);
  assert(pPorosity);

  const ml::CMatParam *pBulkModulus = mat.MatParameter(MLD_BULKSTIFFNESS);
  assert(pBulkModulus);

  const ml::CMatParam *pGrainStiffness = mat.MatParameter(MLD_GRAINSTIFFNESS);
  assert(pGrainStiffness);

  const ml::CMatParam *pVolumetricThermalExpansionCoefficient = mat.MatParameter(MLD_THERMX);
  assert(pVolumetricThermalExpansionCoefficient);

  const ml::CMatParam *pFluidThermalExpansionCoefficient = mat.MatParameter(MLD_FLUIDX);
  assert(pFluidThermalExpansionCoefficient);

  double E = pYoung->Value();
  double phi = pPorosity->Value();
  double Kb = pBulkModulus->Value();
  double Kg = pGrainStiffness->Value();
  double Kf = dValue;
  double nu = pPoisson->Value();
  double Bb = pVolumetricThermalExpansionCoefficient->Value();
  double Bf = pFluidThermalExpansionCoefficient->Value();

  double alpha = Alpha(Kb, Kg);
  double theta = Theta(phi, alpha, Kb, Kg, Kf);

  double Eu = UndrainedYoung(E, alpha, nu, theta);
  double nuu = UndrainedPoisson(nu, alpha, theta, Eu, E);
  double Bu = UndrainedThermalExpansionCoefficient(Bb, phi, Bf, Eu, Kb, alpha, Kg, Kf);

  // set the parameters
  ml::CMatParam *pParam;

  if ((pParam = mat.MatParameter(MLD_UNDRAINEDYOUNG)))
    SetParamValue(*pParam, Eu);

  if ((pParam = mat.MatParameter(MLD_UNDRAINEDPOISSON)))
    SetParamValue(*pParam, nuu);

  if ((pParam = mat.MatParameter(MLD_UNDRAINEDTHERMX))) {
    SetParamValue(*pParam, Bu);
  }

  SetParamValue(param, Kf);
}

void CMaterialUndrainedVolumetricThermalExpansionCoefficientSetStrategy::operator()(double dValue,
                                                                                    ml::CMatParam &param) const {
  ml::CMaterial &mat = param.ParentMaterial();

  const ml::CMatParam *pPorosity = mat.MatParameter(MLD_POROSITY);
  assert(pPorosity);

  const ml::CMatParam *pFluidThermalExpansionCoefficient = mat.MatParameter(MLD_FLUIDX);
  assert(pFluidThermalExpansionCoefficient);

  const ml::CMatParam *pUndrainedYoungsModulus = mat.MatParameter(MLD_UNDRAINEDYOUNG);
  assert(pUndrainedYoungsModulus);

  const ml::CMatParam *pBulkStiffness = mat.MatParameter(MLD_BULKSTIFFNESS);
  assert(pBulkStiffness);

  const ml::CMatParam *pGrainStiffness = mat.MatParameter(MLD_GRAINSTIFFNESS);
  assert(pGrainStiffness);

  const ml::CMatParam *pFluidBulkModulus = mat.MatParameter(MLD_FLUIDMODULUS);
  assert(pFluidBulkModulus);

  double Bb = dValue;
  double phi = pPorosity->Value();
  double Bf = pFluidThermalExpansionCoefficient->Value();
  double Eu = pUndrainedYoungsModulus->Value();
  double Kb = pBulkStiffness->Value();
  double Kg = pGrainStiffness->Value();
  double alpha = Alpha(Kb, Kg);
  double Kf = pFluidBulkModulus->Value();
  double Bu = UndrainedThermalExpansionCoefficient(Bb, phi, Bf, Eu, Kb, alpha, Kg, Kf);

  ml::CMatParam *pParam;

  if ((pParam = mat.MatParameter(MLD_UNDRAINEDTHERMX))) {
    SetParamValue(*pParam, Bu);
  }

  SetParamValue(param, Bb);
}

void CMaterialUndrainedFluidThermalExpansionCoefficientSetStrategy::operator()(double dValue,
                                                                               ml::CMatParam &param) const {
  ml::CMaterial &mat = param.ParentMaterial();

  const ml::CMatParam *pVolumetricThermalExpansionCoefficient = mat.MatParameter(MLD_THERMX);
  assert(pVolumetricThermalExpansionCoefficient);

  const ml::CMatParam *pPorosity = mat.MatParameter(MLD_POROSITY);
  assert(pPorosity);

  const ml::CMatParam *pUndrainedYoungsModulus = mat.MatParameter(MLD_UNDRAINEDYOUNG);
  assert(pUndrainedYoungsModulus);

  const ml::CMatParam *pBulkStiffness = mat.MatParameter(MLD_BULKSTIFFNESS);
  assert(pBulkStiffness);

  const ml::CMatParam *pGrainStiffness = mat.MatParameter(MLD_GRAINSTIFFNESS);
  assert(pGrainStiffness);

  const ml::CMatParam *pFluidBulkModulus = mat.MatParameter(MLD_FLUIDMODULUS);
  assert(pFluidBulkModulus);

  double Bb = pVolumetricThermalExpansionCoefficient->Value();
  double phi = pPorosity->Value();
  double Bf = dValue;
  double Eu = pUndrainedYoungsModulus->Value();
  double Kb = pBulkStiffness->Value();
  double Kg = pGrainStiffness->Value();
  double alpha = Alpha(Kb, Kg);
  double Kf = pFluidBulkModulus->Value();
  double Bu = UndrainedThermalExpansionCoefficient(Bb, phi, Bf, Eu, Kb, alpha, Kg, Kf);

  ml::CMatParam *pParam;

  if ((pParam = mat.MatParameter(MLD_UNDRAINEDTHERMX))) {
    SetParamValue(*pParam, Bu);
  }

  SetParamValue(param, Bf);
}
