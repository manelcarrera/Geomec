#include "MaterialModelCamClayCreep.h"

#include "MaterialUnitTypes.h"
#include "mlMatParamDomain.h"
#include "Materials.h"
#include "ValueTypes.h"
#include "IObject.h"
#include "MaterialHelperFactory.h"

#include <qobject.h>

namespace {

const QString GROUP_MANUAL     = "Manual";
const QString GROUP_BULK       = "Bulk stiffness";
const QString GROUP_DYNAMIC    = "Dynamic stiffness";
const QString GROUP_VELOCITIES = "Velocities";
const QString GROUP_COMPRESSIBILITY = "Compressibility";

}

void CMaterialCamClayCreepCreatorV2::OnCreateLibrary(CLibraryMaterial& /*m*/)
{
}

void CMaterialCamClayCreepCreatorV2::OnCreateLibraryParameters(CLibraryMaterial& m)
{
  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();
  assert(f);

  AddNumber(m, 34);

  // waij TFS 81436 set all 'Fix' icons to '0'
  //
  AddLibParam(MLD_YOUNGMODULUS,        DEF_E ,             m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),         IDT_VALUETYPE_YOUNGS_MODULUS,    f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0),              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_CamClayCreepYoung), QString(), false); //YOUNG_FILOSPATH,  false), GROUP_MANUAL);
  AddLibParam(MLD_YOUNGMODULUS_DECOMP, DEF_E ,             m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),         IDT_VALUETYPE_YOUNG_DECOMP,      f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0),              0, QString(), false);
  AddLibParam(MLD_POISSONRATIO,        DEF_NU,             m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),           IDT_VALUETYPE_POISSONS_RATIO,    f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LT_5Emin1),    f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_CamClayCreepPoisson), QString(), false); //POISON_FILOSPATH, false), GROUP_MANUAL);
  AddLibParam(MLD_POISSONRATIO_DECOMP, DEF_NU,             m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),           IDT_VALUETYPE_POISSON_DECOMP,    f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LT_5Emin1),    0, QString(), false);
  AddLibParam(MLD_BULKSTIFFNESS,       0,                  m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),         IDT_VALUETYPE_BULKSTIFFNESS,     f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearBulkStiff),   f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_CamClayCreepBulkStiff), QString(), false, GROUP_BULK);
  AddLibParam(MLD_SHEARMODULUS,        0,                  m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),         IDT_VALUETYPE_SHEARMODULUS,      f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearShearStiff),  f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_CamClayCreepShearStiff), QString(), false, GROUP_BULK);
  AddLibParam(MLD_DYNUNISTIFFNESS,     0,                  m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),         IDT_VALUETYPE_DYNUNISTIFFNESS,   f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearDynUni),      f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_CamClayCreepDynUni), QString(), false, GROUP_DYNAMIC);
  AddLibParam(MLD_DYNSHEARSTIFFNESS,   0,                  m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),         IDT_VALUETYPE_DYNSHEARSTIFFNESS, f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearDynShear),    f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_CamClayCreepDynShear), QString(), false, GROUP_DYNAMIC);
  AddLibParam(MLD_VELOCITYP,           0,                  m, f->getMPUnitType(CMaterialHelperFactory::UT_VELOCITY),       IDT_VALUETYPE_VELOCITYP,         f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearVelocityP),   f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_CamClayCreepVelocityP), QString(), false, GROUP_VELOCITIES);
  AddLibParam(MLD_VELOCITYS,           0,                  m, f->getMPUnitType(CMaterialHelperFactory::UT_VELOCITY),       IDT_VALUETYPE_VELOCITYS,         f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearVelocityS),   f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_CamClayCreepVelocityS), QString(), false, GROUP_VELOCITIES);
  
  AddLibParam(MLD_CME,                 DEF_CME,            m, f->getMPUnitType(CMaterialHelperFactory::UT_PER_STRESS),     IDT_VALUETYPE_ELASCOMPRES,       f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0),              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_CamClayCreepCME), QString(), false, GROUP_COMPRESSIBILITY);
  AddLibParam(MLD_RHOB,                DEF_RHOB,           m, f->getMPUnitType(CMaterialHelperFactory::UT_DENSITY),        IDT_VALUETYPE_RHOB,              f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0),              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_LinearRhoB), QString(), false);
  AddLibParam(MLD_POROSITY,            DEF_PORO,           m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),           IDT_VALUETYPE_POROSITY,          f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LT_1),         f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_CamClayCreepPorosity), QString(), false); //POROSI_FILOSPATH, true));
  AddLibParam(MLD_GRAINSTIFFNESS,      DEF_GRAINSTIFFNESS, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),         IDT_VALUETYPE_GRAINSTIFFNESS,    f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_10_LE_1E6),      0, QString(), false); //BULKS_FILOSPATH,  true));
  AddLibParam(MLD_PERMEA,              DEF_PERMEA,         m, f->getMPUnitType(CMaterialHelperFactory::UT_PERMEA),         IDT_VALUETYPE_PERMEA,            f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0),              0, QString(), false);
  AddLibParam(MLD_VISCOSITY,           DEF_VISCOSITY,      m, f->getMPUnitType(CMaterialHelperFactory::UT_VISCOSITY),      IDT_VALUETYPE_VISCOSITY,         f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0),              0, QString(), false);
  AddLibParam(MLD_INITFRICTION,        DEF_FRICTION,       m, f->getMPUnitType(CMaterialHelperFactory::UT_ANGLE),          IDT_VALUETYPE_INITFRICTION,      f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LT_70),        f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_CamClayCreepFriction), QString(), false); //FRICTI_FILOSPATH, false));
  AddLibParam(MLD_COHESION,            DEF_COHESION,       m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),         IDT_VALUETYPE_COHESION,          f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0),              0, QString(), false); //COHESI_FILOSPATH, false));
  AddLibParam(MLD_HARDENING,           DEF_HARDENING,      m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),           IDT_VALUETYPE_HARDENING,         f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0_LE_1),         f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_CamClayCreepHardening), QString(), false); //LAMBDA_FILOSPATH, false));
  AddLibParam(MLD_PRECONSOLIDATION,    DEF_PRECONSO,       m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),         IDT_VALUETYPE_PRECONSOLIDATION,  f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0),              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_CamClayCreepPreconsolidation), QString(), false); //PRECON_FILOSPATH, false));
  
  AddLibParam(MLD_CAPSHAPE,            DEF_CAPSHAPE,       m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),           IDT_VALUETYPE_CAPSHAPE,          f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_1Emin1_LE_5),    f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_CamClayCreepCapShap), QString(), false); //ALPHA_FILOSPATH,  false));
  AddLibParam(MLD_GAMMA,               DEF_GAMMA,          m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),           IDT_VALUETYPE_TENSILE_STRETCH,   f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_1),              0, QString(), false); //GAMMA_FILOSPATH,  true));
  AddLibParam(MLD_CMP,                 0,                  m, f->getMPUnitType(CMaterialHelperFactory::UT_PER_STRESS),     IDT_VALUETYPE_PLASCOMPRES,       f->getMPCheckStrategy(CMaterialHelperFactory::MP_CamClayCreepCMP),   f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_CamClayCreepCMP), QString(), false);
  AddLibParam(MLD_LATRATIO_MIN,        DEF_LATRATIO_MIN,   m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),           IDT_VALUETYPE_LATRATIO_MIN,      f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearLatRatioMin), 0, QString(), false);
  AddLibParam(MLD_LATRATIO_MAX,        DEF_LATRATIO_MAX,   m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),           IDT_VALUETYPE_LATRATIO_MAX,      f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearLatRatioMax), 0, QString(), false);
  AddLibParam(MLD_AZIMUTH,             DEF_AZIMUTH,        m, f->getMPUnitType(CMaterialHelperFactory::UT_ANGLE),          IDT_VALUETYPE_AZIMUTH,           f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LE_360),       0, QString(), false);
  AddLibParam(MLD_THERMX,              DEF_THERMX,         m, f->getMPUnitType(CMaterialHelperFactory::UT_PER_TEMPER),     IDT_VALUETYPE_THERMAL_EXPANSION, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LE_1Emin3),    0, QString(), false);
  AddLibParam(MLD_THERM_CONDUCT,       DEF_THERM_CONDUCT,  m, f->getMPUnitType(CMaterialHelperFactory::UT_THERM_CONDUCT),  IDT_VALUETYPE_THERM_CONDUCT,     f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0),              0, QString(), false);
  AddLibParam(MLD_THERM_CAPACI,        DEF_THERM_CAPACI,   m, f->getMPUnitType(CMaterialHelperFactory::UT_THERM_CAPACITY), IDT_VALUETYPE_THERM_CAPACI,      f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0),              0, QString(), false);
  AddLibParam(MLD_FLUIDMODULUS,        DEF_FLUIDMOD,       m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),         IDT_VALUETYPE_FLUID_BULK_MOD,    f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0),              0, QString(), false);
  
  AddLibParam(MLD_FLUIDX,              DEF_FLUIDX,         m, f->getMPUnitType(CMaterialHelperFactory::UT_PER_TEMPER),     IDT_VALUETYPE_FLUIDX,            f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LE_1Emin1),    0, QString(), false);
  AddLibParam(MLD_FLUID_DENSITY,       DEF_FLUID_DENSITY,  m, f->getMPUnitType(CMaterialHelperFactory::UT_DENSITY),        IDT_VALUETYPE_FLUID_DENSITY,     f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0_LE_10000),     0, QString(), false);
  // waij TFS 81436: new parameters + range for n1 and A1: [1-100]
  AddLibParam(MLD_A1,                  DEF_A1,             m, f->getMPUnitType(CMaterialHelperFactory::UT_PER_TIME),       IDT_VALUETYPE_CREEP_A1,          f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LE_100),       0, QString(), false);
  AddLibParam(MLD_n1,                  DEF_N1,             m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),           IDT_VALUETYPE_CREEP_N1,          f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_1_LE_100),       0, QString(), false);
}

int CMaterialCamClayCreepCreatorV2::MaterialModel()
{
  return MM_CAMCLAY_CREEP;
}

QString CMaterialCamClayCreepCreatorV2::MaterialModelName()
{
  return MLD_CAMCLAY_CREEP;
}

QString CMaterialCamClayCreepCreatorV2::CalibrationPath() const
{
  return QString();
}

IPQPlotter* CMaterialCamClayCreepCreatorV2::PQPlotter() const
{
  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();
  assert(f);

  return f->getMatPQPlotter(CMaterialHelperFactory::MAT_PQ_CamClayCreep);
}

#ifdef min
#undef min  // To give std::min precedence over #define min(...)
#endif  // min

void CMaterialCamClayCreepCreatorV2::CPQPlotter::GetYieldCurve(const CLibraryMaterial &mat, double dPrecon, double dPMin, double dPMax, CStressStrainArray &values) const
{
  const ml::CMatParam *pPhiInit = mat.Parameter(MLD_INITFRICTION);
  const ml::CMatParam *pCapShape = mat.Parameter(MLD_CAPSHAPE);
  const ml::CMatParam *pCohesi = mat.Parameter(MLD_COHESION);
  const ml::CMatParam *pGamma = mat.Parameter(MLD_GAMMA);

  double dPhiInit = pPhiInit->Value();
  double dPhiInitRad = dPhiInit * PI / 180;	// phi init in radians
  double dCapShape = pCapShape->Value();
  double dCohesi = pCohesi->Value();
  double dGamma = pGamma->Value();

  double dPShift = dCohesi / tan(dPhiInitRad);

  double a = dCapShape / (1 + dCapShape) * dPrecon;
  double dSinPhiInit = sin(dPhiInitRad);
  double m2 = pow(6 * dSinPhiInit / (3 - dSinPhiInit), 2);

  dPMin = 0;
  double dPRightHand = std::min(dPMax, a);

  // part for p < a
  double beta2 = dGamma * dGamma;

  if(fabs(dPRightHand - dPMin) > 1e-8 && fabs(dPMin) < fabs(dPRightHand))
  {
    for(int i = 0; i < NUM_PQ_STEPS; ++i)
    {
      double p = (double)i / NUM_PQ_STEPS * (dPRightHand - dPMin) + dPMin;
      double q = GetCapShapeQ(m2, beta2, p, a);
      // shift:
      p -= dPShift;
      CStressStrain pt(p, q);
      values.push_back(pt);
    }
  }

  // part for p > a
  beta2 = 1 / (dCapShape * dCapShape);
  dPRightHand = std::min(dPMax, dPrecon);

  if(fabs(dPRightHand - a) > 1e-8 && fabs(dPRightHand) > fabs(a))
  {
    for(int i = 0; i < NUM_PQ_STEPS; ++i)
    {
      double p = (double)i / NUM_PQ_STEPS * (dPRightHand - a) + a;
      double q = GetCapShapeQ(m2, beta2, p, a);
      // shift:
      p -= dPShift;
      CStressStrain pt( p, q );
      values.push_back( pt );
    }
    // add dPRightHand
    double q = GetCapShapeQ(m2, beta2, dPRightHand, a);
    // shift:
    dPRightHand -= dPShift;
    CStressStrain pt(dPRightHand, q);
    values.push_back(pt);
  }
}

IPQPlotter* CMaterialCamClayCreepCreatorV2::CPQPlotter::Clone() const
{
  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();
  assert(f);

  return f->getMatPQPlotter(CMaterialHelperFactory::MAT_PQ_CamClayCreep);
}

void CMaterialCamClayCreepCreatorV2::CPQPlotter::GetPrimaryYieldCurve(const CLibraryMaterial& mat, double dPMin, double dPMax, CStressStrainArray& values) const
{
  const ml::CMatParam *pPrecon = mat.Parameter(MLD_PRECONSOLIDATION);
  GetYieldCurve(mat, pPrecon->Value(), dPMin, dPMax, values);
}

double CMaterialCamClayCreepCreatorV2::CPQPlotter::GetCapShapeQ(double m2, double beta2, double p, double a) const
{
  return sqrt(fabs((m2 / beta2) * p * (p - 2 * a) + m2 * a * a * (1 / beta2 - 1)));
}

void CMaterialCamClayCreepCreatorDEPRECATED::OnCreateLibrary(CLibraryMaterial& /*m*/)
{
}

void CMaterialCamClayCreepCreatorDEPRECATED::OnCreateLibraryParameters(CLibraryMaterial& m)
{
  // waij TFS 81436 set all 'Fix' icons to '0'
  //
  Add(new CLibraryMaterialParameter(MLD_YOUNGMODULUS,        DEF_E ,             m, *new CUnitTypeStress,        IDT_VALUETYPE_YOUNGS_MODULUS,    new MP_GT(0),                                new CMaterialCamClayCreepYoungSetStrategy)); //YOUNG_FILOSPATH,  false), GROUP_MANUAL);
  Add(new CLibraryMaterialParameter(MLD_YOUNGMODULUS_DECOMP, DEF_E ,             m, *new CUnitTypeStress,        IDT_VALUETYPE_YOUNG_DECOMP,      new MP_GT(0),                                0));
  Add(new CLibraryMaterialParameter(MLD_POISSONRATIO,        DEF_NU,             m, *new CUnitTypeNone,          IDT_VALUETYPE_POISSONS_RATIO,    new MP_GE_LT(0, 0.5),                        new CMaterialCamClayCreepPoissonSetStrategy)); //POISON_FILOSPATH, false), GROUP_MANUAL);
  Add(new CLibraryMaterialParameter(MLD_POISSONRATIO_DECOMP, DEF_NU,             m, *new CUnitTypeNone,          IDT_VALUETYPE_POISSON_DECOMP,    new MP_GE_LT(0, 0.5),                        0));
  Add(new CLibraryMaterialParameter(MLD_BULKSTIFFNESS,       0,                  m, *new CUnitTypeStress,        IDT_VALUETYPE_BULKSTIFFNESS,     new CMaterialLinearBulkStiffCheckStrategy,   new CMaterialCamClayCreepBulkStiffSetStrategy), GROUP_BULK);
  Add(new CLibraryMaterialParameter(MLD_SHEARMODULUS,        0,                  m, *new CUnitTypeStress,        IDT_VALUETYPE_SHEARMODULUS,      new CMaterialLinearShearStiffCheckStrategy,  new CMaterialCamClayCreepShearStiffSetStrategy), GROUP_BULK);
  Add(new CLibraryMaterialParameter(MLD_DYNUNISTIFFNESS,     0,                  m, *new CUnitTypeStress,        IDT_VALUETYPE_DYNUNISTIFFNESS,   new CMaterialLinearDynUniCheckStrategy,      new CMaterialCamClayCreepDynUniSetStrategy), GROUP_DYNAMIC);
  Add(new CLibraryMaterialParameter(MLD_DYNSHEARSTIFFNESS,   0,                  m, *new CUnitTypeStress,        IDT_VALUETYPE_DYNSHEARSTIFFNESS, new CMaterialLinearDynShearCheckStrategy,    new CMaterialCamClayCreepDynShearSetStrategy), GROUP_DYNAMIC);
  Add(new CLibraryMaterialParameter(MLD_VELOCITYP,           0,                  m, *new CUnitTypeVelocity,      IDT_VALUETYPE_VELOCITYP,         new CMaterialLinearVelocityPCheckStrategy,   new CMaterialCamClayCreepVelocityPSetStrategy), GROUP_VELOCITIES);
  Add(new CLibraryMaterialParameter(MLD_VELOCITYS,           0,                  m, *new CUnitTypeVelocity,      IDT_VALUETYPE_VELOCITYS,         new CMaterialLinearVelocitySCheckStrategy,   new CMaterialCamClayCreepVelocitySSetStrategy), GROUP_VELOCITIES);
  Add(new CLibraryMaterialParameter(MLD_CME,                 DEF_CME,            m, *new CUnitTypePerStress,     IDT_VALUETYPE_ELASCOMPRES,       new MP_GT(0),                                new CMaterialCamClayCreepCMESetStrategy));
  Add(new CLibraryMaterialParameter(MLD_RHOB,                DEF_RHOB,           m, *new CUnitTypeDensity,       IDT_VALUETYPE_RHOB,              new MP_GT(0),                                new CMaterialLinearRhoBSetStrategy));
  Add(new CLibraryMaterialParameter(MLD_POROSITY,            DEF_PORO,           m, *new CUnitTypeNone,          IDT_VALUETYPE_POROSITY,          new MP_GE_LT(0, 1),                          new CMaterialCamClayCreepPorositySetStrategy)); //POROSI_FILOSPATH, true));
  Add(new CLibraryMaterialParameter(MLD_GRAINSTIFFNESS,      DEF_GRAINSTIFFNESS, m, *new CUnitTypeStress,        IDT_VALUETYPE_GRAINSTIFFNESS,    new MP_GE_LE(10, 1000000),                   0)); //BULKS_FILOSPATH,  true));
  Add(new CLibraryMaterialParameter(MLD_PERMEA,              DEF_PERMEA,         m, *new CUnitTypePermea,        IDT_VALUETYPE_PERMEA,            new MP_GE(0),                                0));
  Add(new CLibraryMaterialParameter(MLD_VISCOSITY,           DEF_VISCOSITY,      m, *new CUnitTypeViscosity,     IDT_VALUETYPE_VISCOSITY,         new MP_GT(0),                                0));
  Add(new CLibraryMaterialParameter(MLD_INITFRICTION,        DEF_FRICTION,       m, *new CUnitTypeAngle,         IDT_VALUETYPE_INITFRICTION,      new MP_GE_LT(0, 70),                         new CMaterialCamClayCreepFrictionSetStrategy)); //FRICTI_FILOSPATH, false));
  Add(new CLibraryMaterialParameter(MLD_COHESION,            DEF_COHESION,       m, *new CUnitTypeStress,        IDT_VALUETYPE_COHESION,          new MP_GE(0),                                0)); //COHESI_FILOSPATH, false));
  Add(new CLibraryMaterialParameter(MLD_HARDENING,           DEF_HARDENING,      m, *new CUnitTypeNone,          IDT_VALUETYPE_HARDENING,         new MP_GT_LE(0, 1),                          new CMaterialCamClayCreepHardeningSetStrategy)); //LAMBDA_FILOSPATH, false));
  Add(new CLibraryMaterialParameter(MLD_PRECONSOLIDATION,    DEF_PRECONSO,       m, *new CUnitTypeStress,        IDT_VALUETYPE_PRECONSOLIDATION,  new MP_GT(0),                                new CMaterialCamClayCreepPreconsolidationSetStrategy)); //PRECON_FILOSPATH, false));
  Add(new CLibraryMaterialParameter(MLD_CAPSHAPE,            DEF_CAPSHAPE,       m, *new CUnitTypeNone,          IDT_VALUETYPE_CAPSHAPE,          new MP_GE_LE(0.1, 5.0),                      new CMaterialCamClayCreepCapShapSetStrategy)); //ALPHA_FILOSPATH,  false));
  Add(new CLibraryMaterialParameter(MLD_GAMMA,               DEF_GAMMA,          m, *new CUnitTypeNone,          IDT_VALUETYPE_TENSILE_STRETCH,   new MP_GE(1.0),                              0)); //GAMMA_FILOSPATH,  true));
  Add(new CLibraryMaterialParameter(MLD_CMP,                 0,                  m, *new CUnitTypePerStress,     IDT_VALUETYPE_PLASCOMPRES,       new CMaterialCamClayCreepCMPCheckStrategy,   new CMaterialCamClayCreepCMPSetStrategy));
  Add(new CLibraryMaterialParameter(MLD_LATRATIO_MIN,        DEF_LATRATIO_MIN,   m, *new CUnitTypeNone,          IDT_VALUETYPE_LATRATIO_MIN,      new CMaterialLinearLatRatioMinCheckStrategy, 0));
  Add(new CLibraryMaterialParameter(MLD_LATRATIO_MAX,        DEF_LATRATIO_MAX,   m, *new CUnitTypeNone,          IDT_VALUETYPE_LATRATIO_MAX,      new CMaterialLinearLatRatioMaxCheckStrategy, 0));
  Add(new CLibraryMaterialParameter(MLD_AZIMUTH,             DEF_AZIMUTH,        m, *new CUnitTypeAngle,         IDT_VALUETYPE_AZIMUTH,           new MP_GE_LE(0, 360),                        0));
  Add(new CLibraryMaterialParameter(MLD_THERMX,              DEF_THERMX,         m, *new CUnitTypePerTemper,     IDT_VALUETYPE_THERMAL_EXPANSION, new MP_GE_LE(0, 1e-3),                       0));
  Add(new CLibraryMaterialParameter(MLD_THERM_CONDUCT,       DEF_THERM_CONDUCT,  m, *new CUnitTypeThermConduct,  IDT_VALUETYPE_THERM_CONDUCT,     new MP_GE(0),                                0));
  Add(new CLibraryMaterialParameter(MLD_THERM_CAPACI,        DEF_THERM_CAPACI,   m, *new CUnitTypeThermCapacity, IDT_VALUETYPE_THERM_CAPACI,      new MP_GE(0),                                0));
  Add(new CLibraryMaterialParameter(MLD_FLUIDMODULUS,        DEF_FLUIDMOD,       m, *new CUnitTypeStress,        IDT_VALUETYPE_FLUID_BULK_MOD,    new MP_GT(0),                                0));
  Add(new CLibraryMaterialParameter(MLD_FLUIDX,              DEF_FLUIDX,         m, *new CUnitTypePerTemper,     IDT_VALUETYPE_FLUIDX,            new MP_GE_LE(0, 0.1),                        0));
  Add(new CLibraryMaterialParameter(MLD_FLUID_DENSITY,       DEF_FLUID_DENSITY,  m, *new CUnitTypeDensity,       IDT_VALUETYPE_FLUID_DENSITY,     new MP_GT_LE(0, 10000),                      0));
  // waij TFS 81436: new parameters + range for n1 and A1: [1-100]
  Add(new CLibraryMaterialParameter(MLD_A1,                  DEF_A1,             m, *new CUnitTypePerTime,       IDT_VALUETYPE_CREEP_A1,          new MP_GE_LE(0, 100),                        0));
  Add(new CLibraryMaterialParameter(MLD_n1,                  DEF_N1,             m, *new CUnitTypeNone,          IDT_VALUETYPE_CREEP_N1,          new MP_GE_LE(1, 100),                        0));
}

int CMaterialCamClayCreepCreatorDEPRECATED::MaterialModel()
{
  return MM_CAMCLAY_CREEP;
}

QString CMaterialCamClayCreepCreatorDEPRECATED::MaterialModelName()
{
  return MLD_CAMCLAY_CREEP;
}

QString CMaterialCamClayCreepCreatorDEPRECATED::CalibrationPath() const
{
  return QString();
}

IPQPlotter* CMaterialCamClayCreepCreatorDEPRECATED::PQPlotter() const
{
  return new CPQPlotter;
}

#ifdef min
#undef min  // To give std::min precedence over #define min(...)
#endif  // min

void CMaterialCamClayCreepCreatorDEPRECATED::CPQPlotter::GetYieldCurve(const CLibraryMaterial &mat, double dPrecon, double dPMin, double dPMax, CStressStrainArray &values) const
{
  const ml::CMatParam *pPhiInit = mat.Parameter(MLD_INITFRICTION);
  const ml::CMatParam *pCapShape = mat.Parameter(MLD_CAPSHAPE);
  const ml::CMatParam *pCohesi = mat.Parameter(MLD_COHESION);
  const ml::CMatParam *pGamma = mat.Parameter(MLD_GAMMA);

  double dPhiInit = pPhiInit->Value();
  double dPhiInitRad = dPhiInit * PI / 180;	// phi init in radians
  double dCapShape = pCapShape->Value();
  double dCohesi = pCohesi->Value();
  double dGamma = pGamma->Value();

  double dPShift = dCohesi / tan(dPhiInitRad);

  double a = dCapShape / (1 + dCapShape) * dPrecon;
  double dSinPhiInit = sin(dPhiInitRad);
  double m2 = pow(6 * dSinPhiInit / (3 - dSinPhiInit), 2);

  dPMin = 0;
  double dPRightHand = std::min(dPMax, a);

  // part for p < a
  double beta2 = dGamma * dGamma;

  if(fabs(dPRightHand - dPMin) > 1e-8 && fabs(dPMin) < fabs(dPRightHand))
  {
    for(int i = 0; i < NUM_PQ_STEPS; ++i)
    {
      double p = (double)i / NUM_PQ_STEPS * (dPRightHand - dPMin) + dPMin;
      double q = GetCapShapeQ(m2, beta2, p, a);
      // shift:
      p -= dPShift;
      CStressStrain pt(p, q);
      values.push_back(pt);
    }
  }

  // part for p > a
  beta2 = 1 / (dCapShape * dCapShape);
  dPRightHand = std::min(dPMax, dPrecon);

  if(fabs(dPRightHand - a) > 1e-8 && fabs(dPRightHand) > fabs(a))
  {
    for(int i = 0; i < NUM_PQ_STEPS; ++i)
    {
      double p = (double)i / NUM_PQ_STEPS * (dPRightHand - a) + a;
      double q = GetCapShapeQ(m2, beta2, p, a);
      // shift:
      p -= dPShift;
      CStressStrain pt( p, q );
      values.push_back( pt );
    }
    // add dPRightHand
    double q = GetCapShapeQ(m2, beta2, dPRightHand, a);
    // shift:
    dPRightHand -= dPShift;
    CStressStrain pt(dPRightHand, q);
    values.push_back(pt);
  }
}

IPQPlotter* CMaterialCamClayCreepCreatorDEPRECATED::CPQPlotter::Clone() const
{
  return new CPQPlotter;
}

void CMaterialCamClayCreepCreatorDEPRECATED::CPQPlotter::GetPrimaryYieldCurve(const CLibraryMaterial& mat, double dPMin, double dPMax, CStressStrainArray& values) const
{
  const ml::CMatParam *pPrecon = mat.Parameter(MLD_PRECONSOLIDATION);
  GetYieldCurve(mat, pPrecon->Value(), dPMin, dPMax, values);
}

double CMaterialCamClayCreepCreatorDEPRECATED::CPQPlotter::GetCapShapeQ(double m2, double beta2, double p, double a) const
{
  return sqrt(fabs((m2 / beta2) * p * (p - 2 * a) + m2 * a * a * (1 / beta2 - 1)));
}

static double CalcHardening(double pc0, double Cmp, double Cme, double n0, double alpha, double phi)
{
  double sinPhi = sin(DEG2RAD(phi));
  return (pc0 * (Cmp - Cme)) / ((1 - n0) * (1 - (0.5 * sqrt(alpha) * sinPhi * sinPhi)));
}

static double CalcCamclayUniaxialPlasticCompressibility(double n0, double labda, double alpha, double phi, double pc0, double Cme)
{
  double sinPhi = sin(DEG2RAD(phi));
  return (1-n0)*labda*(1-(0.5*sqrt(alpha)*sinPhi*sinPhi))/pc0 + Cme;
}

static void UpdateCamClayCreepCMP(ml::CMaterial& mat)
{
  const ml::CMatParam* pPorosity = mat.MatParameter(MLD_POROSITY);
  assert(pPorosity);

  const ml::CMatParam* pHardening = mat.MatParameter(MLD_HARDENING);
  assert(pHardening);

  const ml::CMatParam* pCapShape = mat.MatParameter(MLD_CAPSHAPE);
  assert(pCapShape);

  const ml::CMatParam* pInitFriction = mat.MatParameter(MLD_INITFRICTION);
  assert(pInitFriction);

  const ml::CMatParam* pPreconsolidation = mat.MatParameter(MLD_PRECONSOLIDATION);
  assert(pPreconsolidation);

  const ml::CMatParam* pElasticCompressibility = mat.MatParameter(MLD_CME);
  assert(pElasticCompressibility);

  ml::CMatParam* pPlasticCompressibility = mat.MatParameter(MLD_CMP);
  assert(pPlasticCompressibility);

  pPlasticCompressibility->Value(
  CalcCamclayUniaxialPlasticCompressibility(pPorosity->Value(),
                                              pHardening->Value(),
                                              pCapShape->Value(),
                                              pInitFriction->Value(),
                                              pPreconsolidation->Value(),
                                              pElasticCompressibility->Value()));
}

void CMaterialCamClayCreepYoungSetStrategy::operator()(double dValue, ml::CMatParam& param) const
{
  CMaterialLinearYoungSetStrategy::operator()(dValue, param);
  UpdateCamClayCreepCMP(param.ParentMaterial());
}

void CMaterialCamClayCreepPoissonSetStrategy::operator()(double dValue, ml::CMatParam& param) const
{
  CMaterialLinearPoissonSetStrategy::operator()(dValue, param);
  UpdateCamClayCreepCMP(param.ParentMaterial());
}

void CMaterialCamClayCreepBulkStiffSetStrategy::operator()(double dValue, ml::CMatParam& param) const
{
  CMaterialLinearBulkStiffSetStrategy::operator()(dValue, param);
  UpdateCamClayCreepCMP(param.ParentMaterial());
}

void CMaterialCamClayCreepShearStiffSetStrategy::operator()(double dValue, ml::CMatParam& param) const
{
  CMaterialLinearShearStiffSetStrategy::operator()(dValue, param);
  UpdateCamClayCreepCMP(param.ParentMaterial());
}

void CMaterialCamClayCreepDynUniSetStrategy::operator()(double dValue, ml::CMatParam& param) const
{
  CMaterialLinearDynUniSetStrategy::operator()(dValue, param);
  UpdateCamClayCreepCMP(param.ParentMaterial());
}

void CMaterialCamClayCreepDynShearSetStrategy::operator()(double dValue, ml::CMatParam& param) const
{
  CMaterialLinearDynShearSetStrategy::operator()(dValue, param);
  UpdateCamClayCreepCMP(param.ParentMaterial());
}

void CMaterialCamClayCreepVelocityPSetStrategy::operator()(double dValue, ml::CMatParam& param) const
{
  CMaterialLinearVelocityPSetStrategy::operator()(dValue, param);
  UpdateCamClayCreepCMP(param.ParentMaterial());
}

void CMaterialCamClayCreepVelocitySSetStrategy::operator()(double dValue, ml::CMatParam& param) const
{
  CMaterialLinearVelocitySSetStrategy::operator()(dValue, param);
  UpdateCamClayCreepCMP(param.ParentMaterial());
}

void CMaterialCamClayCreepCMESetStrategy::operator()(double dValue, ml::CMatParam& param) const
{
  CMaterialLinearCMESetStrategy::operator()(dValue, param);
  UpdateCamClayCreepCMP(param.ParentMaterial());
}

void CMaterialCamClayCreepPorositySetStrategy::operator()(double dValue, ml::CMatParam& param) const
{
  SetParamValue(param, dValue);
  UpdateCamClayCreepCMP(param.ParentMaterial());
}

void CMaterialCamClayCreepFrictionSetStrategy::operator()(double dValue, ml::CMatParam& param) const
{
  SetParamValue(param, dValue);
  UpdateCamClayCreepCMP(param.ParentMaterial());
}

void CMaterialCamClayCreepHardeningSetStrategy::operator()(double dValue, ml::CMatParam& param) const
{
  SetParamValue(param, dValue);
  UpdateCamClayCreepCMP(param.ParentMaterial());
}

void CMaterialCamClayCreepPreconsolidationSetStrategy::operator()(double dValue, ml::CMatParam& param) const
{
  SetParamValue(param, dValue);
  UpdateCamClayCreepCMP(param.ParentMaterial());
}

void CMaterialCamClayCreepCapShapSetStrategy::operator()(double dValue, ml::CMatParam& param) const
{
  SetParamValue(param, dValue);
  UpdateCamClayCreepCMP(param.ParentMaterial());
}

bool CMaterialCamClayCreepCMPCheckStrategy::operator()(double dValue, const ml::CMatParam& param, QString& strErrorMsg, int nUnitDef) const
{
  // perform the domain check on the resulting lambda

  const ml::CMaterial& mat = param.ParentMaterial();

  const ml::CMatParam* pPorosity = mat.MatParameter(MLD_POROSITY);
  assert(pPorosity);

  const ml::CMatParam* pCapShape = mat.MatParameter(MLD_CAPSHAPE);
  assert(pCapShape);

  const ml::CMatParam* pInitFriction = mat.MatParameter(MLD_INITFRICTION);
  assert(pInitFriction);

  const ml::CMatParam* pPreconsolidation = mat.MatParameter(MLD_PRECONSOLIDATION);
  assert(pPreconsolidation);

  const ml::CMatParam* pElasticCompressibility = mat.MatParameter(MLD_CME);
  assert(pElasticCompressibility);

  double dLambda = CalcHardening(
  pPreconsolidation->ValueToUserUnit(nUnitDef),
  dValue,
  pElasticCompressibility->ValueToUserUnit(nUnitDef),
  pPorosity->ValueToUserUnit(nUnitDef),
  pCapShape->ValueToUserUnit(nUnitDef),
  pInitFriction->ValueToUserUnit(nUnitDef));

  const ml::CMatParam* pHardening = mat.MatParameter(MLD_HARDENING);
  assert(pHardening);

  QString strLambdaError;
  if(!pHardening->CheckValueFromUserUnit(dLambda, nUnitDef, strLambdaError))
  {
  strErrorMsg = QObject::tr(
      "This value of the uniaxial plastic compressibility leads to a value\nfor the hardening parameter of %1.\n\n%2").
                                                                                          arg(dLambda).arg(strLambdaError);
  return false;
  }

  return true;
}

void CMaterialCamClayCreepCMPSetStrategy::operator()(double dValue, ml::CMatParam& param) const
{
  // updates the hardening coefficient

  ml::CMaterial& mat = param.ParentMaterial();

  const ml::CMatParam* pPorosity = mat.MatParameter(MLD_POROSITY);
  assert(pPorosity);

  const ml::CMatParam* pCapShape = mat.MatParameter(MLD_CAPSHAPE);
  assert(pCapShape);

  const ml::CMatParam* pInitFriction = mat.MatParameter(MLD_INITFRICTION);
  assert(pInitFriction);

  const ml::CMatParam* pPreconsolidation = mat.MatParameter(MLD_PRECONSOLIDATION);
  assert(pPreconsolidation);

  const ml::CMatParam* pElasticCompressibility = mat.MatParameter(MLD_CME);
  assert(pElasticCompressibility);

  double dLambda = CalcHardening(
  pPreconsolidation->Value(),
  dValue,
  pElasticCompressibility->Value(),
  pPorosity->Value(),
  pCapShape->Value(),
  pInitFriction->Value());

  ml::CMatParam* pHardening = mat.MatParameter(MLD_HARDENING);
  assert(pHardening);

  SetParamValue(*pHardening, dLambda);

  SetParamValue(param, dValue);
}
