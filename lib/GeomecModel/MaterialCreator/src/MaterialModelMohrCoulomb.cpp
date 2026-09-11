#include "MaterialModelMohrCoulomb.h"
#include "MaterialModelLinear.h"

#include "IObject.h"
#include "MaterialHelperFactory.h"
#include "MaterialUnitTypes.h"
#include "Materials.h"
#include "ValueTypes.h"
#include "mlMatParamDomain.h"

#include <qobject.h>

namespace {

const QString GROUP_MANUAL = "Manual";
const QString GROUP_BULK = "Bulk stiffness";
const QString GROUP_DYNAMIC = "Dynamic stiffness";
const QString GROUP_VELOCITIES = "Velocities";
const QString GROUP_COMPRESSIBILITY = "Compressibility";

} // namespace

void CMaterialMohrCoulombCreatorV2::OnCreateLibrary(CLibraryMaterial & /*m*/) {}

void CMaterialMohrCoulombCreatorV2::OnCreateLibraryParameters(CLibraryMaterial &m) {
  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();
  assert(f);

  AddNumber(m, 28);

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
  AddLibParam(MLD_POROSITY, DEF_PORO, m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE), IDT_VALUETYPE_POROSITY,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LT_1), 0, QString(), false);
  AddLibParam(MLD_GRAINSTIFFNESS, DEF_GRAINSTIFFNESS, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_GRAINSTIFFNESS, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_10_LE_1E6), 0,
              BULKS_FILOSPATH, true);
  AddLibParam(MLD_PERMEA, DEF_PERMEA, m, f->getMPUnitType(CMaterialHelperFactory::UT_PERMEA), IDT_VALUETYPE_PERMEA,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0), 0, QString(), false);
  AddLibParam(MLD_VISCOSITY, DEF_VISCOSITY, m, f->getMPUnitType(CMaterialHelperFactory::UT_VISCOSITY),
              IDT_VALUETYPE_VISCOSITY, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0), 0, QString(), false);
  AddLibParam(MLD_COHESION, DEF_COHESION, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_COHESION, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0), 0, COHESI_FILOSPATH,
              false);
  AddLibParam(MLD_FRICTIONANGLE, DEF_FRICTION, m, f->getMPUnitType(CMaterialHelperFactory::UT_ANGLE),
              IDT_VALUETYPE_FRICTION_ANGLE, f->getMPCheckStrategy(CMaterialHelperFactory::MP_MohrCoulombFriction), 0,
              FRICTI_FILOSPATH, false);
  AddLibParam(MLD_DILATATION, DEF_DILATATION, m, f->getMPUnitType(CMaterialHelperFactory::UT_ANGLE),
              IDT_VALUETYPE_DILATATION, f->getMPCheckStrategy(CMaterialHelperFactory::MP_MohrCoulombDilatation), 0,
              DILATA_FILOSPATH, false);
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

int CMaterialMohrCoulombCreatorV2::MaterialModel() { return MM_MOHRCOULOMB; }

QString CMaterialMohrCoulombCreatorV2::MaterialModelName() { return MLD_MOHRCOULOMB; }

QString CMaterialMohrCoulombCreatorV2::CalibrationPath() const { return "MOHRCO"; }

IPQPlotter *CMaterialMohrCoulombCreatorV2::PQPlotter() const {
  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();
  assert(f);

  return f->getMatPQPlotter(CMaterialHelperFactory::MAT_PQ_MohrCoulomb);
}

IPQPlotter *CMaterialMohrCoulombCreatorV2::CPQPlotter::Clone() const {
  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();
  assert(f);

  return f->getMatPQPlotter(CMaterialHelperFactory::MAT_PQ_MohrCoulomb);
}

void CMaterialMohrCoulombCreatorV2::CPQPlotter::GetPrimaryYieldCurve(const CLibraryMaterial &mat, double dPMin,
                                                                     double dPMax, CStressStrainArray &values) const {
  const ml::CMatParam *pPhi = mat.Parameter(MLD_FRICTIONANGLE);
  const ml::CMatParam *pCohesion = mat.Parameter(MLD_COHESION);

  double dPhi = pPhi->Value();
  double dPhiRad = dPhi * PI / 180; // phi in radians
  double dCohesion = pCohesion->Value();

  double k = 6 * dCohesion * cos(dPhiRad) / (3 - sin(dPhiRad));
  double m = 6 * sin(dPhiRad) / (3 - sin(dPhiRad));

  double dQMin = k + m * dPMin;
  double dQMax = k + m * dPMax;

  CStressStrain ptMin(dPMin, dQMin);
  CStressStrain ptMax(dPMax, dQMax);

  values.push_back(ptMin);
  values.push_back(ptMax);
}

void CMaterialMohrCoulombCreatorDEPRECATED::OnCreateLibrary(CLibraryMaterial & /*m*/) {}

void CMaterialMohrCoulombCreatorDEPRECATED::OnCreateLibraryParameters(CLibraryMaterial &m) {
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
  Add(new CLibraryMaterialParameter(MLD_POROSITY, DEF_PORO, m, *new CUnitTypeNone, IDT_VALUETYPE_POROSITY,
                                    new MP_GE_LT(0, 1), 0));
  Add(new CLibraryMaterialParameter(MLD_GRAINSTIFFNESS, DEF_GRAINSTIFFNESS, m, *new CUnitTypeStress,
                                    IDT_VALUETYPE_GRAINSTIFFNESS, new MP_GE_LE(10, 1000000), 0, BULKS_FILOSPATH, true));
  Add(new CLibraryMaterialParameter(MLD_PERMEA, DEF_PERMEA, m, *new CUnitTypePermea, IDT_VALUETYPE_PERMEA, new MP_GE(0),
                                    0));
  Add(new CLibraryMaterialParameter(MLD_VISCOSITY, DEF_VISCOSITY, m, *new CUnitTypeViscosity, IDT_VALUETYPE_VISCOSITY,
                                    new MP_GT(0), 0));
  Add(new CLibraryMaterialParameter(MLD_COHESION, DEF_COHESION, m, *new CUnitTypeStress, IDT_VALUETYPE_COHESION,
                                    new MP_GE(0), 0, COHESI_FILOSPATH, false));
  Add(new CLibraryMaterialParameter(MLD_FRICTIONANGLE, DEF_FRICTION, m, *new CUnitTypeAngle,
                                    IDT_VALUETYPE_FRICTION_ANGLE, new CMaterialMohrCoulombFrictionCheckStrategy, 0,
                                    FRICTI_FILOSPATH, false));
  Add(new CLibraryMaterialParameter(MLD_DILATATION, DEF_DILATATION, m, *new CUnitTypeAngle, IDT_VALUETYPE_DILATATION,
                                    new CMaterialMohrCoulombDilatationCheckStrategy, 0, DILATA_FILOSPATH, false));
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

int CMaterialMohrCoulombCreatorDEPRECATED::MaterialModel() { return MM_MOHRCOULOMB; }

QString CMaterialMohrCoulombCreatorDEPRECATED::MaterialModelName() { return MLD_MOHRCOULOMB; }

QString CMaterialMohrCoulombCreatorDEPRECATED::CalibrationPath() const { return "MOHRCO"; }

IPQPlotter *CMaterialMohrCoulombCreatorDEPRECATED::PQPlotter() const { return new CPQPlotter; }

IPQPlotter *CMaterialMohrCoulombCreatorDEPRECATED::CPQPlotter::Clone() const { return new CPQPlotter; }

void CMaterialMohrCoulombCreatorDEPRECATED::CPQPlotter::GetPrimaryYieldCurve(const CLibraryMaterial &mat, double dPMin,
                                                                             double dPMax,
                                                                             CStressStrainArray &values) const {
  const ml::CMatParam *pPhi = mat.Parameter(MLD_FRICTIONANGLE);
  const ml::CMatParam *pCohesion = mat.Parameter(MLD_COHESION);

  double dPhi = pPhi->Value();
  double dPhiRad = dPhi * PI / 180; // phi in radians
  double dCohesion = pCohesion->Value();

  double k = 6 * dCohesion * cos(dPhiRad) / (3 - sin(dPhiRad));
  double m = 6 * sin(dPhiRad) / (3 - sin(dPhiRad));

  double dQMin = k + m * dPMin;
  double dQMax = k + m * dPMax;

  CStressStrain ptMin(dPMin, dQMin);
  CStressStrain ptMax(dPMax, dQMax);

  values.push_back(ptMin);
  values.push_back(ptMax);
}

bool CMaterialMohrCoulombFrictionCheckStrategy::operator()(double dValue, const ml::CMatParam &param,
                                                           QString &strErrorMsg, int nUnitDef) const {
  // check against dilatation angle
  const ml::CMaterial &mat = param.ParentMaterial();

  const ml::CMatParam *pDilatation = mat.MatParameter(MLD_DILATATION);
  assert(pDilatation);
  double dDilatation = pDilatation->ValueToUserUnit(nUnitDef);

  if (dValue < dDilatation || dValue > 90) {
    strErrorMsg = QObject::tr("Friction angle must be at least equal to the Dilatation angle and at most 90.");
    return false;
  }

  return true;
}

bool CMaterialMohrCoulombDilatationCheckStrategy::operator()(double dValue, const ml::CMatParam &param,
                                                             QString &strErrorMsg, int nUnitDef) const {
  // check against friction angle
  const ml::CMaterial &mat = param.ParentMaterial();

  const ml::CMatParam *pFriction = mat.MatParameter(MLD_FRICTIONANGLE);
  assert(pFriction);
  double dFriction = pFriction->ValueToUserUnit(nUnitDef);

  if (dValue < 0 || dValue > dFriction) {
    strErrorMsg = QObject::tr("Dilatation angle must be at least zero and at most equal to the Friction angle.");
    return false;
  }

  return true;
}
