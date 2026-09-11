
#include "MaterialModelLinear.h"

#include "LibraryMaterial.h"
#include "LibraryMaterialParameter.h"
#include "MaterialHelperFactory.h"
#include "MaterialUnitTypes.h"
#include "Materials.h"
#include "ValueTypes.h"
#include "mlMatParamDomain.h"

#include <cassert>
#include <cmath>

namespace {

const QString GROUP_MANUAL = "Manual";
const QString GROUP_BULK = "Bulk stiffness";
const QString GROUP_DYNAMIC = "Dynamic stiffness";
const QString GROUP_VELOCITIES = "Velocities";
const QString GROUP_COMPRESSIBILITY = "Compressibility";

} // namespace

void CMaterialLinearCreatorV2::OnCreateLibrary(CLibraryMaterial &m) { m.BehavesLinearly(true); }

void CMaterialLinearCreatorV2::OnCreateLibraryParameters(CLibraryMaterial &m) {
  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();
  assert(f);

  AddNumber(m, 27);

  AddLibParam(MLD_YOUNGMODULUS, DEF_E, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_YOUNGS_MODULUS, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_LinearYoung), YOUNG_FILOSPATH, false, GROUP_MANUAL);
  AddLibParam(MLD_YOUNGMODULUS_DECOMP, DEF_E, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_YOUNG_DECOMP, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0), 0, QString(), false);
  AddLibParam(MLD_POISSONRATIO, DEF_NU, m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),
              IDT_VALUETYPE_POISSONS_RATIO, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LT_5Emin1),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_LinearPoisson), POISON_FILOSPATH, false, GROUP_MANUAL);
  AddLibParam(MLD_POISSONRATIO_DECOMP, DEF_NU, m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),
              IDT_VALUETYPE_POISSON_DECOMP, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LT_5Emin1), 0,
              QString(), false);
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
  AddLibParam(MLD_CME, DEF_CME, m, f->getMPUnitType(CMaterialHelperFactory::UT_PER_STRESS), IDT_VALUETYPE_ELASCOMPRES,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_LinearCME), QString(), false, GROUP_COMPRESSIBILITY);
  AddLibParam(MLD_RHOB, DEF_RHOB, m, f->getMPUnitType(CMaterialHelperFactory::UT_DENSITY), IDT_VALUETYPE_RHOB,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_LinearRhoB), QString(), false);

  AddLibParam(MLD_POROSITY, DEF_PORO, m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE), IDT_VALUETYPE_POROSITY,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LT_1), 0, QString(), false);
  AddLibParam(MLD_GRAINSTIFFNESS, DEF_GRAINSTIFFNESS, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_GRAINSTIFFNESS, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_10_LE_1E6), 0,
              BULKS_FILOSPATH, true);
  AddLibParam(MLD_PERMEA, DEF_PERMEA, m, f->getMPUnitType(CMaterialHelperFactory::UT_PERMEA), IDT_VALUETYPE_PERMEA,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0), 0, QString(), false);
  AddLibParam(MLD_VISCOSITY, DEF_VISCOSITY, m, f->getMPUnitType(CMaterialHelperFactory::UT_VISCOSITY),
              IDT_VALUETYPE_VISCOSITY, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0), 0, QString(), false);
  AddLibParam(MLD_VELOCITYP, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_VELOCITY), IDT_VALUETYPE_VELOCITYP,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearVelocityP),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_LinearVelocityP), QString(), false, GROUP_VELOCITIES);
  AddLibParam(MLD_VELOCITYS, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_VELOCITY), IDT_VALUETYPE_VELOCITYS,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearVelocityS),
              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_LinearVelocityS), QString(), false, GROUP_VELOCITIES);
  AddLibParam(MLD_COHESION, DEF_COHESION, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_COHESION, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0), 0, QString(), false);
  AddLibParam(MLD_FRICTIONANGLE, DEF_FRICTION, m, f->getMPUnitType(CMaterialHelperFactory::UT_ANGLE),
              IDT_VALUETYPE_FRICTION_ANGLE, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LT_70), 0, QString(),
              false);
  AddLibParam(MLD_LATRATIO_MIN, DEF_LATRATIO_MIN, m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),
              IDT_VALUETYPE_LATRATIO_MIN, f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearLatRatioMin), 0,
              QString(), false);
  AddLibParam(MLD_LATRATIO_MAX, DEF_LATRATIO_MAX, m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),
              IDT_VALUETYPE_LATRATIO_MAX, f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearLatRatioMax), 0,
              QString(), false);

  AddLibParam(MLD_AZIMUTH, DEF_AZIMUTH, m, f->getMPUnitType(CMaterialHelperFactory::UT_ANGLE), IDT_VALUETYPE_AZIMUTH,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LE_360), 0, QString(), false);
  AddLibParam(MLD_THERMX, DEF_THERMX, m, f->getMPUnitType(CMaterialHelperFactory::UT_PER_TEMPER),
              IDT_VALUETYPE_THERMAL_EXPANSION, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LE_1Emin3), 0,
              QString(), false);
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

int CMaterialLinearCreatorV2::MaterialModel() { return MM_LINEAR; }

QString CMaterialLinearCreatorV2::MaterialModelName() { return MLD_LINEAR; }

QString CMaterialLinearCreatorV2::CalibrationPath() const { return "LINEAR"; }

void CMaterialLinearCreatorDEPRECATED::OnCreateLibrary(CLibraryMaterial & /*m*/) {}

void CMaterialLinearCreatorDEPRECATED::OnCreateLibraryParameters(CLibraryMaterial &m) {
  Add(new CLibraryMaterialParameter(MLD_YOUNGMODULUS, DEF_E, m, *new CUnitTypeStress, IDT_VALUETYPE_YOUNGS_MODULUS,
                                    new MP_GT(0), new CMaterialLinearYoungSetStrategy, YOUNG_FILOSPATH, false),
      GROUP_MANUAL);
  Add(new CLibraryMaterialParameter(MLD_YOUNGMODULUS_DECOMP, DEF_E, m, *new CUnitTypeStress, IDT_VALUETYPE_YOUNG_DECOMP,
                                    new MP_GT(0), 0));
  Add(new CLibraryMaterialParameter(MLD_POISSONRATIO, DEF_NU, m, *new CUnitTypeNone, IDT_VALUETYPE_POISSONS_RATIO,
                                    new MP_GE_LT(0, 0.5), new CMaterialLinearPoissonSetStrategy, POISON_FILOSPATH,
                                    false),
      GROUP_MANUAL);
  Add(new CLibraryMaterialParameter(MLD_POISSONRATIO_DECOMP, DEF_NU, m, *new CUnitTypeNone,
                                    IDT_VALUETYPE_POISSON_DECOMP, new MP_GE_LT(0, 0.5), 0));
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
  Add(new CLibraryMaterialParameter(MLD_CME, DEF_CME, m, *new CUnitTypePerStress, IDT_VALUETYPE_ELASCOMPRES,
                                    new MP_GT(0), new CMaterialLinearCMESetStrategy));
  Add(new CLibraryMaterialParameter(MLD_RHOB, DEF_RHOB, m, *new CUnitTypeDensity, IDT_VALUETYPE_RHOB, new MP_GT(0),
                                    new CMaterialLinearRhoBSetStrategy));
  Add(new CLibraryMaterialParameter(MLD_POROSITY, DEF_PORO, m, *new CUnitTypeNone, IDT_VALUETYPE_POROSITY,
                                    new MP_GE_LT(0, 1), 0));
  Add(new CLibraryMaterialParameter(MLD_GRAINSTIFFNESS, DEF_GRAINSTIFFNESS, m, *new CUnitTypeStress,
                                    IDT_VALUETYPE_GRAINSTIFFNESS, new MP_GE_LE(10, 1000000), 0, BULKS_FILOSPATH, true));
  Add(new CLibraryMaterialParameter(MLD_PERMEA, DEF_PERMEA, m, *new CUnitTypePermea, IDT_VALUETYPE_PERMEA, new MP_GE(0),
                                    0));
  Add(new CLibraryMaterialParameter(MLD_VISCOSITY, DEF_VISCOSITY, m, *new CUnitTypeViscosity, IDT_VALUETYPE_VISCOSITY,
                                    new MP_GT(0), 0));
  Add(new CLibraryMaterialParameter(MLD_VELOCITYP, 0, m, *new CUnitTypeVelocity, IDT_VALUETYPE_VELOCITYP,
                                    new CMaterialLinearVelocityPCheckStrategy, new CMaterialLinearVelocityPSetStrategy),
      GROUP_VELOCITIES);
  Add(new CLibraryMaterialParameter(MLD_VELOCITYS, 0, m, *new CUnitTypeVelocity, IDT_VALUETYPE_VELOCITYS,
                                    new CMaterialLinearVelocitySCheckStrategy, new CMaterialLinearVelocitySSetStrategy),
      GROUP_VELOCITIES);
  Add(new CLibraryMaterialParameter(MLD_COHESION, DEF_COHESION, m, *new CUnitTypeStress, IDT_VALUETYPE_COHESION,
                                    new MP_GE(0), 0));
  Add(new CLibraryMaterialParameter(MLD_FRICTIONANGLE, DEF_FRICTION, m, *new CUnitTypeAngle,
                                    IDT_VALUETYPE_FRICTION_ANGLE, new MP_GE_LT(0, 70), 0));
  Add(new CLibraryMaterialParameter(MLD_LATRATIO_MIN, DEF_LATRATIO_MIN, m, *new CUnitTypeNone,
                                    IDT_VALUETYPE_LATRATIO_MIN, new CMaterialLinearLatRatioMinCheckStrategy, 0));
  Add(new CLibraryMaterialParameter(MLD_LATRATIO_MAX, DEF_LATRATIO_MAX, m, *new CUnitTypeNone,
                                    IDT_VALUETYPE_LATRATIO_MAX, new CMaterialLinearLatRatioMaxCheckStrategy, 0));
  Add(new CLibraryMaterialParameter(MLD_AZIMUTH, DEF_AZIMUTH, m, *new CUnitTypeAngle, IDT_VALUETYPE_AZIMUTH,
                                    new MP_GE_LE(0, 360), 0));
  Add(new CLibraryMaterialParameter(MLD_THERMX, DEF_THERMX, m, *new CUnitTypePerTemper, IDT_VALUETYPE_THERMAL_EXPANSION,
                                    new MP_GE_LE(0, 1e-3), 0));
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

int CMaterialLinearCreatorDEPRECATED::MaterialModel() { return MM_LINEAR; }

QString CMaterialLinearCreatorDEPRECATED::MaterialModelName() { return MLD_LINEAR; }

QString CMaterialLinearCreatorDEPRECATED::CalibrationPath() const { return "LINEAR"; }

void CMaterialLinearYoungSetStrategy::operator()(double dValue, ml::CMatParam &param) const {
  ml::CMaterial &mat = param.ParentMaterial();

  // also need Poisson's ratio and Rho b
  const ml::CMatParam *pPoisson = mat.MatParameter(MLD_POISSONRATIO);
  assert(pPoisson);

  const ml::CMatParam *pRhob = mat.MatParameter(MLD_RHOB);
  assert(pRhob);

  ml::CMatParam *pYoungDecomp = mat.MatParameter(MLD_YOUNGMODULUS_DECOMP);
  assert(pYoungDecomp);

  double dRhob = pRhob->Value();
  double dPoisson = pPoisson->Value();
  double dYoung = dValue;
  double dYoungDecomp = pYoungDecomp->Value();

  // calculate other parameters
  double dBulk = dYoung / (3.0 * (1 - 2.0 * dPoisson));
  double dShear = dYoung / (2.0 * (1.0 + dPoisson));
  double dDynUni = dYoung * (1.0 - dPoisson) / ((1.0 + dPoisson) * (1.0 - 2.0 * dPoisson));
  double dDynShear = dYoung / (2.0 * (1.0 + dPoisson));
  double dVp = sqrt(fabs(dDynUni / dRhob)) * 1000;
  double dVs = sqrt(fabs(dDynShear / dRhob)) * 1000;
  double dCme = (1 + dPoisson) * (1 - 2 * dPoisson) / (dYoung * (1 - dPoisson));

  // set the parameters
  ml::CMatParam *pParam;

  if ((pParam = mat.MatParameter(MLD_BULKSTIFFNESS)))
    SetParamValue(*pParam, dBulk);

  if ((pParam = mat.MatParameter(MLD_SHEARMODULUS)))
    SetParamValue(*pParam, dShear);

  if (!(pParam = mat.MatParameter(MLD_DYNUNISTIFFNESS)))
    SetParamValue(*pParam, dDynUni);

  if (!(pParam = mat.MatParameter(MLD_DYNSHEARSTIFFNESS)))
    SetParamValue(*pParam, dDynShear);

  if (!(pParam = mat.MatParameter(MLD_VELOCITYP)))
    SetParamValue(*pParam, dVp);

  if (!(pParam = mat.MatParameter(MLD_VELOCITYS)))
    SetParamValue(*pParam, dVs);

  if (!(pParam = mat.MatParameter(MLD_CME)))
    SetParamValue(*pParam, dCme);

  SetParamValue(param, dYoung);

  // decompaction Young's modulus should follow if value is equal to old Young's value
  if (fabs(dYoungDecomp - param.PreviousValue()) < 1e-4)
    SetParamValue(*pYoungDecomp, dValue);
}

void CMaterialLinearPoissonSetStrategy::operator()(double dValue, ml::CMatParam &param) const {
  ml::CMaterial &mat = param.ParentMaterial();

  // also need Young's modulus and Rho b
  const ml::CMatParam *pYoung = mat.MatParameter(MLD_YOUNGMODULUS);
  assert(pYoung);

  const ml::CMatParam *pRhob = mat.MatParameter(MLD_RHOB);
  assert(pRhob);

  ml::CMatParam *pPoissonDecomp = mat.MatParameter(MLD_POISSONRATIO_DECOMP);
  assert(pPoissonDecomp);

  double dRhob = pRhob->Value();
  double dYoung = pYoung->Value();
  double dPoisson = dValue;
  double dPoissonDecomp = pPoissonDecomp->Value();

  // calculate other parameters
  double dBulk = dYoung / (3.0 * (1 - 2.0 * dPoisson));
  double dShear = dYoung / (2.0 * (1.0 + dPoisson));
  double dDynUni = dYoung * (1.0 - dPoisson) / ((1.0 + dPoisson) * (1.0 - 2.0 * dPoisson));
  double dDynShear = dYoung / (2.0 * (1.0 + dPoisson));
  double dVp = sqrt(fabs(dDynUni / dRhob)) * 1000;
  double dVs = sqrt(fabs(dDynShear / dRhob)) * 1000;
  double dCme = (1 + dPoisson) * (1 - 2 * dPoisson) / (dYoung * (1 - dPoisson));

  // set the parameters
  ml::CMatParam *pParam;

  if ((pParam = mat.MatParameter(MLD_BULKSTIFFNESS)))
    SetParamValue(*pParam, dBulk);

  if ((pParam = mat.MatParameter(MLD_SHEARMODULUS)))
    SetParamValue(*pParam, dShear);

  if ((pParam = mat.MatParameter(MLD_DYNUNISTIFFNESS)))
    SetParamValue(*pParam, dDynUni);

  if ((pParam = mat.MatParameter(MLD_DYNSHEARSTIFFNESS)))
    SetParamValue(*pParam, dDynShear);

  if ((pParam = mat.MatParameter(MLD_VELOCITYP)))
    SetParamValue(*pParam, dVp);

  if ((pParam = mat.MatParameter(MLD_VELOCITYS)))
    SetParamValue(*pParam, dVs);

  if ((pParam = mat.MatParameter(MLD_CME)))
    SetParamValue(*pParam, dCme);

  SetParamValue(param, dPoisson);

  // decompaction Poisson ratio should follow if value is equal to old Poisson value
  if (fabs(dPoissonDecomp - param.PreviousValue()) < 1e-6)
    SetParamValue(*pPoissonDecomp, dValue);
}

void CMaterialLinearCMESetStrategy::operator()(double dValue, ml::CMatParam &param) const {
  ml::CMaterial &mat = param.ParentMaterial();

  // also need Young's modulus and Rho b and Poisson's ratio
  ml::CMatParam *pYoung = mat.MatParameter(MLD_YOUNGMODULUS);
  assert(pYoung);

  const ml::CMatParam *pRhob = mat.MatParameter(MLD_RHOB);
  assert(pRhob);

  const ml::CMatParam *pPoisson = mat.MatParameter(MLD_POISSONRATIO);
  assert(pPoisson);

  double dRhob = pRhob->Value();
  double dPoisson = pPoisson->Value();
  double dCme = dValue;

  // calculate Young's modulus
  double dYoung = (1 - 2 * dPoisson * dPoisson / (1 - dPoisson)) / dCme;
  SetParamValue(*pYoung, dYoung);

  // calculate other parameters
  double dBulk = dYoung / (3.0 * (1 - 2.0 * dPoisson));
  double dShear = dYoung / (2.0 * (1.0 + dPoisson));
  double dDynUni = dYoung * (1.0 - dPoisson) / ((1.0 + dPoisson) * (1.0 - 2.0 * dPoisson));
  double dDynShear = dYoung / (2.0 * (1.0 + dPoisson));
  double dVp = sqrt(fabs(dDynUni / dRhob)) * 1000;
  double dVs = sqrt(fabs(dDynShear / dRhob)) * 1000;

  // set the parameters
  ml::CMatParam *pParam;

  if ((pParam = mat.MatParameter(MLD_BULKSTIFFNESS)))
    SetParamValue(*pParam, dBulk);

  if ((pParam = mat.MatParameter(MLD_SHEARMODULUS)))
    SetParamValue(*pParam, dShear);

  if ((pParam = mat.MatParameter(MLD_DYNUNISTIFFNESS)))
    SetParamValue(*pParam, dDynUni);

  if ((pParam = mat.MatParameter(MLD_DYNSHEARSTIFFNESS)))
    SetParamValue(*pParam, dDynShear);

  if ((pParam = mat.MatParameter(MLD_VELOCITYP)))
    SetParamValue(*pParam, dVp);

  if ((pParam = mat.MatParameter(MLD_VELOCITYS)))
    SetParamValue(*pParam, dVs);

  SetParamValue(param, dCme);
}

bool CMaterialLinearBulkStiffCheckStrategy::operator()(double dValue, const ml::CMatParam &param, QString &strErrorMsg,
                                                       int nUnitDef) const {
  // check against shear stiffness
  const ml::CMaterial &mat = param.ParentMaterial();

  const ml::CMatParam *pShear = mat.MatParameter(MLD_SHEARMODULUS);
  assert(pShear);

  double dShear = pShear->ValueToUserUnit(nUnitDef);

  if (dValue < 2.0 / 3.0 * dShear) {
    strErrorMsg = "Bulk stiffness must be at least 2/3 of shear modulus.";
    return false;
  }

  return true;
}

void CMaterialLinearBulkStiffSetStrategy::operator()(double dValue, ml::CMatParam &param) const {
  ml::CMaterial &mat = param.ParentMaterial();

  const ml::CMatParam *pShear = mat.MatParameter(MLD_SHEARMODULUS);
  assert(pShear);
  double dShear = pShear->Value();

  const ml::CMatParam *pRhob = mat.MatParameter(MLD_RHOB);
  assert(pRhob);
  double dRhob = pRhob->Value();

  double dBulk = dValue;

  double dYoung = 9.0 * dBulk * dShear / (3.0 * dBulk + dShear);
  double dPoisson = (3.0 * dBulk - 2.0 * dShear) / (2.0 * (3.0 * dBulk + dShear));
  double dDynUni = dBulk + 4.0 * dShear / 3.0;
  double dDynShear = dShear;
  double dVp = sqrt(fabs(dDynUni / dRhob)) * 1000;
  double dVs = sqrt(fabs(dDynShear / dRhob)) * 1000;
  double dCme = (1 + dPoisson) * (1 - 2 * dPoisson) / (dYoung * (1 - dPoisson));

  // set the parameters
  ml::CMatParam *pParam;

  if ((pParam = mat.MatParameter(MLD_YOUNGMODULUS)))
    SetParamValue(*pParam, dYoung);

  if ((pParam = mat.MatParameter(MLD_POISSONRATIO)))
    SetParamValue(*pParam, dPoisson);

  if ((pParam = mat.MatParameter(MLD_DYNUNISTIFFNESS)))
    SetParamValue(*pParam, dDynUni);

  if ((pParam = mat.MatParameter(MLD_DYNSHEARSTIFFNESS)))
    SetParamValue(*pParam, dDynShear);

  if ((pParam = mat.MatParameter(MLD_VELOCITYP)))
    SetParamValue(*pParam, dVp);

  if ((pParam = mat.MatParameter(MLD_VELOCITYS)))
    SetParamValue(*pParam, dVs);

  if ((pParam = mat.MatParameter(MLD_CME)))
    SetParamValue(*pParam, dCme);

  SetParamValue(param, dBulk);
}

bool CMaterialLinearShearStiffCheckStrategy::operator()(double dValue, const ml::CMatParam &param, QString &strErrorMsg,
                                                        int nUnitDef) const {
  const ml::CMaterial &mat = param.ParentMaterial();

  const ml::CMatParam *pBulk = mat.MatParameter(MLD_BULKSTIFFNESS);
  assert(pBulk);

  double dBulk = pBulk->ValueToUserUnit(nUnitDef);

  if (dBulk < 2.0 / 3.0 * dValue || dValue <= 0) {
    strErrorMsg = "Shear modulus must be greater than zero and at most 1.5 times bulk stiffness.";
    return false;
  }

  return true;
}

void CMaterialLinearShearStiffSetStrategy::operator()(double dValue, ml::CMatParam &param) const {
  ml::CMaterial &mat = param.ParentMaterial();

  ml::CMatParam *pBulk = mat.MatParameter(MLD_BULKSTIFFNESS);
  assert(pBulk);
  double dBulk = pBulk->Value();

  ml::CMatParam *pRhob = mat.MatParameter(MLD_RHOB);
  assert(pRhob);
  double dRhob = pRhob->Value();

  double dShear = dValue;

  double dYoung = 9.0 * dBulk * dShear / (3.0 * dBulk + dShear);
  double dPoisson = (3.0 * dBulk - 2.0 * dShear) / (2.0 * (3.0 * dBulk + dShear));
  double dDynUni = dBulk + 4.0 * dShear / 3.0;
  double dDynShear = dShear;
  double dVp = sqrt(fabs(dDynUni / dRhob)) * 1000;
  double dVs = sqrt(fabs(dDynShear / dRhob)) * 1000;
  double dCme = (1 + dPoisson) * (1 - 2 * dPoisson) / (dYoung * (1 - dPoisson));

  // set the parameters
  ml::CMatParam *pParam;

  if ((pParam = mat.MatParameter(MLD_YOUNGMODULUS)))
    SetParamValue(*pParam, dYoung);

  if ((pParam = mat.MatParameter(MLD_POISSONRATIO)))
    SetParamValue(*pParam, dPoisson);

  if ((pParam = mat.MatParameter(MLD_DYNUNISTIFFNESS)))
    SetParamValue(*pParam, dDynUni);

  if ((pParam = mat.MatParameter(MLD_DYNSHEARSTIFFNESS)))
    SetParamValue(*pParam, dDynShear);

  if ((pParam = mat.MatParameter(MLD_VELOCITYP)))
    SetParamValue(*pParam, dVp);

  if ((pParam = mat.MatParameter(MLD_VELOCITYS)))
    SetParamValue(*pParam, dVs);

  if ((pParam = mat.MatParameter(MLD_CME)))
    SetParamValue(*pParam, dCme);

  SetParamValue(param, dShear);
}

bool CMaterialLinearDynUniCheckStrategy::operator()(double dValue, const ml::CMatParam &param, QString &strErrorMsg,
                                                    int nUnitDef) const {
  const ml::CMaterial &mat = param.ParentMaterial();

  const ml::CMatParam *pDynShear = mat.MatParameter(MLD_DYNSHEARSTIFFNESS);
  assert(pDynShear);

  double dDynShear = pDynShear->ValueToUserUnit(nUnitDef);

  if (dValue <= dDynShear || dValue < 2.0 * dDynShear) {
    strErrorMsg = "Dynamic uniaxial stiffness must be at least twice as large as dynamic shear stiffness.";
    return false;
  }

  return true;
}

void CMaterialLinearDynUniSetStrategy::operator()(double dValue, ml::CMatParam &param) const {
  ml::CMaterial &mat = param.ParentMaterial();

  const ml::CMatParam *pDynShear = mat.MatParameter(MLD_DYNSHEARSTIFFNESS);
  assert(pDynShear);
  double dDynShear = pDynShear->Value();

  const ml::CMatParam *pRhob = mat.MatParameter(MLD_RHOB);
  assert(pRhob);
  double dRhob = pRhob->Value();

  double dDynUni = dValue;

  double dYoung = dDynShear * (3.0 * dDynUni - 4.0 * dDynShear) / (dDynUni - dDynShear);
  double dPoisson = (dDynUni - 2.0 * dDynShear) / (2.0 * (dDynUni - dDynShear));
  double dBulk = dDynUni - 4.0 / 3.0 * dDynShear;
  double dShear = dDynShear;
  double dVp = sqrt(fabs(dDynUni / dRhob)) * 1000;
  double dVs = sqrt(fabs(dDynShear / dRhob)) * 1000;
  double dCme = (1 + dPoisson) * (1 - 2 * dPoisson) / (dYoung * (1 - dPoisson));

  // set the parameters
  ml::CMatParam *pParam;

  if ((pParam = mat.MatParameter(MLD_YOUNGMODULUS)))
    SetParamValue(*pParam, dYoung);

  if ((pParam = mat.MatParameter(MLD_POISSONRATIO)))
    SetParamValue(*pParam, dPoisson);

  if ((pParam = mat.MatParameter(MLD_BULKSTIFFNESS)))
    SetParamValue(*pParam, dBulk);

  if ((pParam = mat.MatParameter(MLD_SHEARMODULUS)))
    SetParamValue(*pParam, dShear);

  if ((pParam = mat.MatParameter(MLD_VELOCITYP)))
    SetParamValue(*pParam, dVp);

  if ((pParam = mat.MatParameter(MLD_VELOCITYS)))
    SetParamValue(*pParam, dVs);

  if ((pParam = mat.MatParameter(MLD_CME)))
    SetParamValue(*pParam, dCme);

  SetParamValue(param, dDynUni);
}

bool CMaterialLinearDynShearCheckStrategy::operator()(double dValue, const ml::CMatParam &param, QString &strErrorMsg,
                                                      int nUnitDef) const {
  const ml::CMaterial &mat = param.ParentMaterial();

  const ml::CMatParam *pDynUni = mat.MatParameter(MLD_DYNUNISTIFFNESS);
  assert(pDynUni);
  double dDynUni = pDynUni->ValueToUserUnit(nUnitDef);

  if (dDynUni <= dValue || dDynUni < 2.0 * dValue || dValue <= 0) {
    strErrorMsg =
        "Dynamic shear stiffness must be greater than zero and smaller than dynamic uniaxial stiffness divided by 2.";
    return false;
  }

  return true;
}

void CMaterialLinearDynShearSetStrategy::operator()(double dValue, ml::CMatParam &param) const {
  ml::CMaterial &mat = param.ParentMaterial();

  ml::CMatParam *pDynUni = mat.MatParameter(MLD_DYNUNISTIFFNESS);
  assert(pDynUni);
  double dDynUni = pDynUni->Value();

  ml::CMatParam *pRhob = mat.MatParameter(MLD_RHOB);
  assert(pRhob);
  double dRhob = pRhob->Value();

  double dDynShear = dValue;

  double dYoung = dDynShear * (3.0 * dDynUni - 4.0 * dDynShear) / (dDynUni - dDynShear);
  double dPoisson = (dDynUni - 2.0 * dDynShear) / (2.0 * (dDynUni - dDynShear));
  double dBulk = dDynUni - 4.0 / 3.0 * dDynShear;
  double dShear = dDynShear;
  double dVp = sqrt(fabs(dDynUni / dRhob)) * 1000;
  double dVs = sqrt(fabs(dDynShear / dRhob)) * 1000;
  double dCme = (1 + dPoisson) * (1 - 2 * dPoisson) / (dYoung * (1 - dPoisson));

  // set the parameters
  ml::CMatParam *pParam;

  if ((pParam = mat.MatParameter(MLD_YOUNGMODULUS)))
    SetParamValue(*pParam, dYoung);

  if ((pParam = mat.MatParameter(MLD_POISSONRATIO)))
    SetParamValue(*pParam, dPoisson);

  if ((pParam = mat.MatParameter(MLD_BULKSTIFFNESS)))
    SetParamValue(*pParam, dBulk);

  if ((pParam = mat.MatParameter(MLD_SHEARMODULUS)))
    SetParamValue(*pParam, dShear);

  if ((pParam = mat.MatParameter(MLD_VELOCITYP)))
    SetParamValue(*pParam, dVp);

  if ((pParam = mat.MatParameter(MLD_VELOCITYS)))
    SetParamValue(*pParam, dVs);

  if ((pParam = mat.MatParameter(MLD_CME)))
    SetParamValue(*pParam, dCme);

  SetParamValue(param, dDynShear);
}

void CMaterialLinearRhoBSetStrategy::operator()(double dValue, ml::CMatParam &param) const {
  ml::CMaterial &mat = param.ParentMaterial();

  const ml::CMatParam *pYoung = mat.MatParameter(MLD_YOUNGMODULUS);
  assert(pYoung);

  const ml::CMatParam *pPoisson = mat.MatParameter(MLD_POISSONRATIO);
  assert(pPoisson);

  double dRhob = dValue;
  double dYoung = pYoung->Value();
  double dPoisson = pPoisson->Value();
  double dDynUni = dYoung * (1.0 - dPoisson) / ((1.0 + dPoisson) * (1.0 - 2.0 * dPoisson));
  double dDynShear = dYoung / (2.0 * (1.0 + dPoisson));
  double dVp = sqrt(fabs(dDynUni / dRhob)) * 1000;
  double dVs = sqrt(fabs(dDynShear / dRhob)) * 1000;

  ml::CMatParam *pParam;

  if ((pParam = mat.MatParameter(MLD_VELOCITYP)))
    SetParamValue(*pParam, dVp);

  if ((pParam = mat.MatParameter(MLD_VELOCITYS)))
    SetParamValue(*pParam, dVs);

  SetParamValue(param, dRhob);
}

bool CMaterialLinearVelocityPCheckStrategy::operator()(double dValue, const ml::CMatParam &param, QString &strErrorMsg,
                                                       int /*nUnitDef*/) const {
  const ml::CMaterial &mat = param.ParentMaterial();

  const ml::CMatParam *pVs = mat.MatParameter(MLD_VELOCITYS);
  assert(pVs);

  double dVs = pVs->Value();

  if (dValue <= 0 || dValue * dValue < 2.0 * dVs * dVs) {
    strErrorMsg = "Velocity P must be greater than zero.\nThe square of Velocity P must be at least twice as large as "
                  "the square of Velocity S.";
    return false;
  }

  return true;
}

void CMaterialLinearVelocityPSetStrategy::operator()(double dValue, ml::CMatParam &param) const {
  ml::CMaterial &mat = param.ParentMaterial();

  const ml::CMatParam *pRhob = mat.MatParameter(MLD_RHOB);
  assert(pRhob);
  double dRhob = pRhob->Value();

  const ml::CMatParam *pVs = mat.MatParameter(MLD_VELOCITYS);
  assert(pVs);
  double dVs = pVs->Value();

  double dVp = dValue;

  double dDynUni = dRhob * (dVp * dVp / 1000000);
  double dDynShear = dRhob * (dVs * dVs / 1000000);
  double dYoung = dDynShear * (3.0 * dDynUni - 4.0 * dDynShear) / (dDynUni - dDynShear);
  double dPoisson = (dDynUni - 2.0 * dDynShear) / (2.0 * (dDynUni - dDynShear));
  double dBulk = dDynUni - 4.0 / 3.0 * dDynShear;
  double dShear = dDynShear;
  double dCme = (1 + dPoisson) * (1 - 2 * dPoisson) / (dYoung * (1 - dPoisson));

  ml::CMatParam *pParam;

  if ((pParam = mat.MatParameter(MLD_DYNUNISTIFFNESS)))
    SetParamValue(*pParam, dDynUni);

  if ((pParam = mat.MatParameter(MLD_DYNSHEARSTIFFNESS)))
    SetParamValue(*pParam, dDynShear);

  if ((pParam = mat.MatParameter(MLD_YOUNGMODULUS)))
    SetParamValue(*pParam, dYoung);

  if ((pParam = mat.MatParameter(MLD_POISSONRATIO)))
    SetParamValue(*pParam, dPoisson);

  if ((pParam = mat.MatParameter(MLD_BULKSTIFFNESS)))
    SetParamValue(*pParam, dBulk);

  if ((pParam = mat.MatParameter(MLD_SHEARMODULUS)))
    SetParamValue(*pParam, dShear);

  if ((pParam = mat.MatParameter(MLD_CME)))
    SetParamValue(*pParam, dCme);

  SetParamValue(param, dVp);
}

bool CMaterialLinearVelocitySCheckStrategy::operator()(double dValue, const ml::CMatParam &param, QString &strErrorMsg,
                                                       int /*nUnitDef*/) const {
  const ml::CMaterial &mat = param.ParentMaterial();

  const ml::CMatParam *pVp = mat.MatParameter(MLD_VELOCITYP);
  assert(pVp);
  double dVp = pVp->Value();

  if (dValue <= 0 || dVp * dVp < 2.0 * dValue * dValue) {
    strErrorMsg = "Velocity S must be greater than zero.\nThe square of Velocity S can be at most half of the square "
                  "of Velocity P.";
    return false;
  }

  return true;
}

void CMaterialLinearVelocitySSetStrategy::operator()(double dValue, ml::CMatParam &param) const {
  ml::CMaterial &mat = param.ParentMaterial();

  const ml::CMatParam *pRhob = mat.MatParameter(MLD_RHOB);
  assert(pRhob);
  double dRhob = pRhob->Value();

  const ml::CMatParam *pVp = mat.MatParameter(MLD_VELOCITYP);
  assert(pVp);
  double dVp = pVp->Value();

  double dVs = dValue;

  double dDynUni = dRhob * (dVp * dVp / 1000000);
  double dDynShear = dRhob * (dVs * dVs / 1000000);
  double dYoung = dDynShear * (3.0 * dDynUni - 4.0 * dDynShear) / (dDynUni - dDynShear);
  double dPoisson = (dDynUni - 2.0 * dDynShear) / (2.0 * (dDynUni - dDynShear));
  double dBulk = dDynUni - 4.0 / 3.0 * dDynShear;
  double dShear = dDynShear;
  double dCme = (1 + dPoisson) * (1 - 2 * dPoisson) / (dYoung * (1 - dPoisson));

  ml::CMatParam *pParam;

  if ((pParam = mat.MatParameter(MLD_DYNUNISTIFFNESS)))
    SetParamValue(*pParam, dDynUni);

  if ((pParam = mat.MatParameter(MLD_DYNSHEARSTIFFNESS)))
    SetParamValue(*pParam, dDynShear);

  if ((pParam = mat.MatParameter(MLD_YOUNGMODULUS)))
    SetParamValue(*pParam, dYoung);

  if ((pParam = mat.MatParameter(MLD_POISSONRATIO)))
    SetParamValue(*pParam, dPoisson);

  if ((pParam = mat.MatParameter(MLD_BULKSTIFFNESS)))
    SetParamValue(*pParam, dBulk);

  if ((pParam = mat.MatParameter(MLD_SHEARMODULUS)))
    SetParamValue(*pParam, dShear);

  if ((pParam = mat.MatParameter(MLD_CME)))
    SetParamValue(*pParam, dCme);

  SetParamValue(param, dVs);
}

bool CMaterialLinearLatRatioMinCheckStrategy::operator()(double dValue, const ml::CMatParam &param,
                                                         QString &strErrorMsg, int /*nUnitDef*/) const {
  const ml::CMaterial &mat = param.ParentMaterial();

  const ml::CMatParam *pMaxHorStressRatio = mat.MatParameter(MLD_LATRATIO_MAX);
  assert(pMaxHorStressRatio);

  if (dValue < 0 /*|| dValue > dMaxHorStressRatio*/) {
    //		strErrorMsg = "Minimum horizontal stress ratio must be greater than 0 and at most equal to the Maximum
    // horizontal stress ratio.";
    strErrorMsg = "Minimum horizontal stress ratio must be greater than 0.";
    return false;
  }

  return true;
}

bool CMaterialLinearLatRatioMaxCheckStrategy::operator()(double dValue, const ml::CMatParam &param,
                                                         QString &strErrorMsg, int /*nUnitDef*/) const {
  const ml::CMaterial &mat = param.ParentMaterial();

  const ml::CMatParam *pMinHorStressRatio = mat.MatParameter(MLD_LATRATIO_MIN);
  assert(pMinHorStressRatio);

  //	if(dValue < dMinHorStressRatio)
  if (dValue < 0) {
    //		strErrorMsg = "Maximum horizontal stress ratio must be at least equal to the Minimum horizontal stress
    // ratio.";
    strErrorMsg = "Maximum horizontal stress ratio must be greater than 0.";
    return false;
  }

  return true;
}
