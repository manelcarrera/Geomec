#include "MaterialModelDualCapLinEla.h"

#include "MaterialModelDualCap.h"
#include "MaterialModelCamClay.h"

#include "MaterialUnitTypes.h"
#include "mlMatParamDomain.h"
#include "Materials.h"
#include "LibraryMaterialParameter.h"
#include "ValueTypes.h"
#include "MaterialHelperFactory.h"

#include <cassert>

namespace {

const QString GROUP_MANUAL     = "Manual";
const QString GROUP_BULK       = "Bulk stiffness";
const QString GROUP_DYNAMIC    = "Dynamic stiffness";
const QString GROUP_VELOCITIES = "Velocities";
const QString GROUP_COMPRESSIBILITY = "Compressibility";

}

void CMaterialDualCapLinElaCreatorV2::OnCreateLibrary(CLibraryMaterial& /*m*/)
{
}

void CMaterialDualCapLinElaCreatorV2::OnCreateLibraryParameters(CLibraryMaterial& m)
{
  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();
  assert(f);

  AddNumber(m, 34);

  AddLibParam(MLD_YOUNGMODULUS,        DEF_E ,             m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),         IDT_VALUETYPE_YOUNGS_MODULUS,    f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0),                    f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_CamClayYoung),            YOUNG_FILOSPATH,  false, GROUP_MANUAL);
  AddLibParam(MLD_YOUNGMODULUS_DECOMP, DEF_E ,             m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),         IDT_VALUETYPE_YOUNG_DECOMP,      f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0),                    0, QString(), false);
  AddLibParam(MLD_POISSONRATIO,        DEF_NU,             m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),           IDT_VALUETYPE_POISSONS_RATIO,    f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LT_5Emin1),          f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_CamClayPoisson),          POISON_FILOSPATH, false, GROUP_MANUAL);
  AddLibParam(MLD_POISSONRATIO_DECOMP, DEF_NU,             m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),           IDT_VALUETYPE_POISSON_DECOMP,    f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LT_5Emin1),          0, QString(), false);
  AddLibParam(MLD_BULKSTIFFNESS,       0,                  m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),         IDT_VALUETYPE_BULKSTIFFNESS,     f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearBulkStiff),         f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_CamClayBulkStiff), QString(), false,                                 GROUP_BULK);
  AddLibParam(MLD_SHEARMODULUS,        0,                  m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),         IDT_VALUETYPE_SHEARMODULUS,      f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearShearStiff),        f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_CamClayShearStiff), QString(), false,                                GROUP_BULK);
  AddLibParam(MLD_DYNUNISTIFFNESS,     0,                  m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),         IDT_VALUETYPE_DYNUNISTIFFNESS,   f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearDynUni),            f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_CamClayDynUni), QString(), false,                                    GROUP_DYNAMIC);
  AddLibParam(MLD_DYNSHEARSTIFFNESS,   0,                  m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),         IDT_VALUETYPE_DYNSHEARSTIFFNESS, f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearDynShear),          f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_CamClayDynShear), QString(), false,                                  GROUP_DYNAMIC);
  AddLibParam(MLD_VELOCITYP,           0,                  m, f->getMPUnitType(CMaterialHelperFactory::UT_VELOCITY),       IDT_VALUETYPE_VELOCITYP,         f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearVelocityP),         f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_CamClayVelocityP), QString(), false,                                 GROUP_VELOCITIES);
  AddLibParam(MLD_VELOCITYS,           0,                  m, f->getMPUnitType(CMaterialHelperFactory::UT_VELOCITY),       IDT_VALUETYPE_VELOCITYS,         f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearVelocityS),         f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_CamClayVelocityS), QString(), false,                                 GROUP_VELOCITIES);
  
  AddLibParam(MLD_CME,                 DEF_CME,            m, f->getMPUnitType(CMaterialHelperFactory::UT_PER_STRESS),     IDT_VALUETYPE_ELASCOMPRES,       f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0),                    f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_CamClayCME), QString(), false, GROUP_COMPRESSIBILITY);
  AddLibParam(MLD_RHOB,                DEF_RHOB,           m, f->getMPUnitType(CMaterialHelperFactory::UT_DENSITY),        IDT_VALUETYPE_RHOB,              f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0),                    f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_LinearRhoB), QString(), false);
  AddLibParam(MLD_POROSITY,            DEF_PORO,           m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),           IDT_VALUETYPE_POROSITY,          f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LT_1),               f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_CamClayPorosity),         POROSI_FILOSPATH, true);
  AddLibParam(MLD_GRAINSTIFFNESS,      DEF_GRAINSTIFFNESS, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),         IDT_VALUETYPE_GRAINSTIFFNESS,    f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_10_LE_1E6),            0,                                               BULKS_FILOSPATH,  true);
  AddLibParam(MLD_PERMEA,              DEF_PERMEA,         m, f->getMPUnitType(CMaterialHelperFactory::UT_PERMEA),         IDT_VALUETYPE_PERMEA,            f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0),                    0, QString(), false);
  AddLibParam(MLD_VISCOSITY,           DEF_VISCOSITY,      m, f->getMPUnitType(CMaterialHelperFactory::UT_VISCOSITY),      IDT_VALUETYPE_VISCOSITY,         f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0),                    0, QString(), false);
  AddLibParam(MLD_INITFRICTION,        DEF_FRICTION,       m, f->getMPUnitType(CMaterialHelperFactory::UT_ANGLE),          IDT_VALUETYPE_INITFRICTION,      f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LT_70),              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_CamClayFriction),         FRICTI_FILOSPATH, false);
  AddLibParam(MLD_COHESION,            DEF_COHESION,       m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),         IDT_VALUETYPE_COHESION,          f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0),                    0,                                               COHESI_FILOSPATH, false);
  AddLibParam(MLD_HARDENING,           DEF_HARDENING,      m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),           IDT_VALUETYPE_HARDENING,         f->getMPCheckStrategy(CMaterialHelperFactory::MP_DualCapHardening),        f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_CamClayHardening),        LAMBDA_FILOSPATH, false);
  AddLibParam(MLD_PRECONSOLIDATION,    DEF_PRECONSO,       m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),         IDT_VALUETYPE_PRECONSOLIDATION,  f->getMPCheckStrategy(CMaterialHelperFactory::MP_DualCapPreconsolidation), f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_CamClayPreconsolidation), PRECON_FILOSPATH, false);
  
  AddLibParam(MLD_CAPSHAPE,            DEF_CAPSHAPE,       m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),           IDT_VALUETYPE_CAPSHAPE,          f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_1Emin1_LE_5),          f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_CamClayCapShap),          ALPHA_FILOSPATH,  false);
  AddLibParam(MLD_SEC_PRECON,          DEF_SEC_PRECON,     m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),         IDT_VALUETYPE_SEC_PRECON,        f->getMPCheckStrategy(CMaterialHelperFactory::MP_DualCapSecondaryPrecon),  0,                                               PRECO1_FILOSPATH, false);
  AddLibParam(MLD_SEC_HARDENING,       DEF_SEC_HARD,       m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),           IDT_VALUETYPE_SEC_HARDENING,     f->getMPCheckStrategy(CMaterialHelperFactory::MP_DualCapSecHardening),     0,                                               MU_FILOSPATH,     false);
  AddLibParam(MLD_GAMMA,               DEF_GAMMA,          m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),           IDT_VALUETYPE_TENSILE_STRETCH,   f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_1),                    0,                                               GAMMA_FILOSPATH,  true);
  AddLibParam(MLD_CMP,                 0,                  m, f->getMPUnitType(CMaterialHelperFactory::UT_PER_STRESS),     IDT_VALUETYPE_PLASCOMPRES,       f->getMPCheckStrategy(CMaterialHelperFactory::MP_CamClayCMP),              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_CamClayCMP), QString(), false);
  AddLibParam(MLD_LATRATIO_MIN,        DEF_LATRATIO_MIN,   m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),           IDT_VALUETYPE_LATRATIO_MIN,      f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearLatRatioMin),       0, QString(), false);
  AddLibParam(MLD_LATRATIO_MAX,        DEF_LATRATIO_MAX,   m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),           IDT_VALUETYPE_LATRATIO_MAX,      f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearLatRatioMax),       0, QString(), false);
  AddLibParam(MLD_AZIMUTH,             DEF_AZIMUTH,        m, f->getMPUnitType(CMaterialHelperFactory::UT_ANGLE),          IDT_VALUETYPE_AZIMUTH,           f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LE_360),             0, QString(), false);
  AddLibParam(MLD_THERMX,              DEF_THERMX,         m, f->getMPUnitType(CMaterialHelperFactory::UT_PER_TEMPER),     IDT_VALUETYPE_THERMAL_EXPANSION, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LE_1Emin3),          0, QString(), false);
  AddLibParam(MLD_THERM_CONDUCT,       DEF_THERM_CONDUCT,  m, f->getMPUnitType(CMaterialHelperFactory::UT_THERM_CONDUCT),  IDT_VALUETYPE_THERM_CONDUCT,     f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0),                    0, QString(), false);
  
  AddLibParam(MLD_THERM_CAPACI,        DEF_THERM_CAPACI,   m, f->getMPUnitType(CMaterialHelperFactory::UT_THERM_CAPACITY), IDT_VALUETYPE_THERM_CAPACI,      f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0),                    0, QString(), false);
  AddLibParam(MLD_FLUIDMODULUS,        DEF_FLUIDMOD,       m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),         IDT_VALUETYPE_FLUID_BULK_MOD,    f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0),                    0, QString(), false);
  AddLibParam(MLD_FLUIDX,              DEF_FLUIDX,         m, f->getMPUnitType(CMaterialHelperFactory::UT_PER_TEMPER),     IDT_VALUETYPE_FLUIDX,            f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LE_1Emin1),          0, QString(), false);
  AddLibParam(MLD_FLUID_DENSITY,       DEF_FLUID_DENSITY,  m, f->getMPUnitType(CMaterialHelperFactory::UT_DENSITY),        IDT_VALUETYPE_FLUID_DENSITY,     f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0_LE_10000),           0, QString(), false);
}

int CMaterialDualCapLinElaCreatorV2::MaterialModel()
{
  return MM_DUALCAP_LINELA;
}

QString CMaterialDualCapLinElaCreatorV2::MaterialModelName()
{
  return MLD_DUALCAP_LINELA;
}

QString CMaterialDualCapLinElaCreatorV2::CalibrationPath() const
{
  return "DOHALI";
}

IPQPlotter* CMaterialDualCapLinElaCreatorV2::PQPlotter() const
{
  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();
  assert(f);

  return f->getMatPQPlotter(CMaterialHelperFactory::MAT_PQ_DualCap);
}


void CMaterialDualCapLinElaCreatorDEPRECATED::OnCreateLibrary(CLibraryMaterial& /*m*/)
{
}

void CMaterialDualCapLinElaCreatorDEPRECATED::OnCreateLibraryParameters(CLibraryMaterial& m)
{
  Add(new CLibraryMaterialParameter(MLD_YOUNGMODULUS,        DEF_E ,             m, *new CUnitTypeStress,        IDT_VALUETYPE_YOUNGS_MODULUS,    new MP_GT(0),                                      new CMaterialCamClayYoungSetStrategy,            YOUNG_FILOSPATH,  false), GROUP_MANUAL);
  Add(new CLibraryMaterialParameter(MLD_YOUNGMODULUS_DECOMP, DEF_E ,             m, *new CUnitTypeStress,        IDT_VALUETYPE_YOUNG_DECOMP,      new MP_GT(0),                                      0));
  Add(new CLibraryMaterialParameter(MLD_POISSONRATIO,        DEF_NU,             m, *new CUnitTypeNone,          IDT_VALUETYPE_POISSONS_RATIO,    new MP_GE_LT(0, 0.5),                              new CMaterialCamClayPoissonSetStrategy,          POISON_FILOSPATH, false), GROUP_MANUAL);
  Add(new CLibraryMaterialParameter(MLD_POISSONRATIO_DECOMP, DEF_NU,             m, *new CUnitTypeNone,          IDT_VALUETYPE_POISSON_DECOMP,    new MP_GE_LT(0, 0.5),                              0));
  Add(new CLibraryMaterialParameter(MLD_BULKSTIFFNESS,       0,                  m, *new CUnitTypeStress,        IDT_VALUETYPE_BULKSTIFFNESS,     new CMaterialLinearBulkStiffCheckStrategy,         new CMaterialCamClayBulkStiffSetStrategy),                                 GROUP_BULK);
  Add(new CLibraryMaterialParameter(MLD_SHEARMODULUS,        0,                  m, *new CUnitTypeStress,        IDT_VALUETYPE_SHEARMODULUS,      new CMaterialLinearShearStiffCheckStrategy,        new CMaterialCamClayShearStiffSetStrategy),                                GROUP_BULK);
  Add(new CLibraryMaterialParameter(MLD_DYNUNISTIFFNESS,     0,                  m, *new CUnitTypeStress,        IDT_VALUETYPE_DYNUNISTIFFNESS,   new CMaterialLinearDynUniCheckStrategy,            new CMaterialCamClayDynUniSetStrategy),                                    GROUP_DYNAMIC);
  Add(new CLibraryMaterialParameter(MLD_DYNSHEARSTIFFNESS,   0,                  m, *new CUnitTypeStress,        IDT_VALUETYPE_DYNSHEARSTIFFNESS, new CMaterialLinearDynShearCheckStrategy,          new CMaterialCamClayDynShearSetStrategy),                                  GROUP_DYNAMIC);
  Add(new CLibraryMaterialParameter(MLD_VELOCITYP,           0,                  m, *new CUnitTypeVelocity,      IDT_VALUETYPE_VELOCITYP,         new CMaterialLinearVelocityPCheckStrategy,         new CMaterialCamClayVelocityPSetStrategy),                                 GROUP_VELOCITIES);
  Add(new CLibraryMaterialParameter(MLD_VELOCITYS,           0,                  m, *new CUnitTypeVelocity,      IDT_VALUETYPE_VELOCITYS,         new CMaterialLinearVelocitySCheckStrategy,         new CMaterialCamClayVelocitySSetStrategy),                                 GROUP_VELOCITIES);
  Add(new CLibraryMaterialParameter(MLD_CME,                 DEF_CME,            m, *new CUnitTypePerStress,     IDT_VALUETYPE_ELASCOMPRES,       new MP_GT(0),                                      new CMaterialCamClayCMESetStrategy));
  Add(new CLibraryMaterialParameter(MLD_RHOB,                DEF_RHOB,           m, *new CUnitTypeDensity,       IDT_VALUETYPE_RHOB,              new MP_GT(0),                                      new CMaterialLinearRhoBSetStrategy));
  Add(new CLibraryMaterialParameter(MLD_POROSITY,            DEF_PORO,           m, *new CUnitTypeNone,          IDT_VALUETYPE_POROSITY,          new MP_GE_LT(0, 1),                                new CMaterialCamClayPorositySetStrategy,         POROSI_FILOSPATH, true));
  Add(new CLibraryMaterialParameter(MLD_GRAINSTIFFNESS,      DEF_GRAINSTIFFNESS, m, *new CUnitTypeStress,        IDT_VALUETYPE_GRAINSTIFFNESS,    new MP_GE_LE(10, 1000000),                         0,                                               BULKS_FILOSPATH,  true));
  Add(new CLibraryMaterialParameter(MLD_PERMEA,              DEF_PERMEA,         m, *new CUnitTypePermea,        IDT_VALUETYPE_PERMEA,            new MP_GE(0),                                      0));
  Add(new CLibraryMaterialParameter(MLD_VISCOSITY,           DEF_VISCOSITY,      m, *new CUnitTypeViscosity,     IDT_VALUETYPE_VISCOSITY,         new MP_GT(0),                                      0));
  Add(new CLibraryMaterialParameter(MLD_INITFRICTION,        DEF_FRICTION,       m, *new CUnitTypeAngle,         IDT_VALUETYPE_INITFRICTION,      new MP_GE_LT(0, 70),                               new CMaterialCamClayFrictionSetStrategy,         FRICTI_FILOSPATH, false));
  Add(new CLibraryMaterialParameter(MLD_COHESION,            DEF_COHESION,       m, *new CUnitTypeStress,        IDT_VALUETYPE_COHESION,          new MP_GE(0),                                      0,                                               COHESI_FILOSPATH, false));
  Add(new CLibraryMaterialParameter(MLD_HARDENING,           DEF_HARDENING,      m, *new CUnitTypeNone,          IDT_VALUETYPE_HARDENING,         new MP_GT_LE(0, 1),                                new CMaterialCamClayHardeningSetStrategy,        LAMBDA_FILOSPATH, false));
  Add(new CLibraryMaterialParameter(MLD_PRECONSOLIDATION,    DEF_PRECONSO,       m, *new CUnitTypeStress,        IDT_VALUETYPE_PRECONSOLIDATION,  new CMaterialDualCapPreconsolidationCheckStrategy, new CMaterialCamClayPreconsolidationSetStrategy, PRECON_FILOSPATH, false));
  Add(new CLibraryMaterialParameter(MLD_CAPSHAPE,            DEF_CAPSHAPE,       m, *new CUnitTypeNone,          IDT_VALUETYPE_CAPSHAPE,          new MP_GE_LE(0.1, 5.0),                            new CMaterialCamClayCapShapSetStrategy,          ALPHA_FILOSPATH,  false));
  Add(new CLibraryMaterialParameter(MLD_SEC_PRECON,          DEF_SEC_PRECON,     m, *new CUnitTypeStress,        IDT_VALUETYPE_SEC_PRECON,        new CMaterialDualCapSecondaryPreconCheckStrategy,  0,                                               PRECO1_FILOSPATH, false));
  Add(new CLibraryMaterialParameter(MLD_SEC_HARDENING,       DEF_SEC_HARD,       m, *new CUnitTypeNone,          IDT_VALUETYPE_SEC_HARDENING,     new MP_GE_LE(0, 1),                                0,                                               MU_FILOSPATH,     false));
  Add(new CLibraryMaterialParameter(MLD_GAMMA,               DEF_GAMMA,          m, *new CUnitTypeNone,          IDT_VALUETYPE_TENSILE_STRETCH,   new MP_GE(1.0),                                    0,                                               GAMMA_FILOSPATH,  true));
  Add(new CLibraryMaterialParameter(MLD_CMP,                 0,                  m, *new CUnitTypePerStress,     IDT_VALUETYPE_PLASCOMPRES,       new CMaterialCamClayCMPCheckStrategy,              new CMaterialCamClayCMPSetStrategy));
  Add(new CLibraryMaterialParameter(MLD_LATRATIO_MIN,        DEF_LATRATIO_MIN,   m, *new CUnitTypeNone,          IDT_VALUETYPE_LATRATIO_MIN,      new CMaterialLinearLatRatioMinCheckStrategy,       0));
  Add(new CLibraryMaterialParameter(MLD_LATRATIO_MAX,        DEF_LATRATIO_MAX,   m, *new CUnitTypeNone,          IDT_VALUETYPE_LATRATIO_MAX,      new CMaterialLinearLatRatioMaxCheckStrategy,       0));
  Add(new CLibraryMaterialParameter(MLD_AZIMUTH,             DEF_AZIMUTH,        m, *new CUnitTypeAngle,         IDT_VALUETYPE_AZIMUTH,           new MP_GE_LE(0, 360),                              0));
  Add(new CLibraryMaterialParameter(MLD_THERMX,              DEF_THERMX,         m, *new CUnitTypePerTemper,     IDT_VALUETYPE_THERMAL_EXPANSION, new MP_GE_LE(0, 1e-3),                             0));
  Add(new CLibraryMaterialParameter(MLD_THERM_CONDUCT,       DEF_THERM_CONDUCT,  m, *new CUnitTypeThermConduct,  IDT_VALUETYPE_THERM_CONDUCT,     new MP_GE(0),                                      0));
  Add(new CLibraryMaterialParameter(MLD_THERM_CAPACI,        DEF_THERM_CAPACI,   m, *new CUnitTypeThermCapacity, IDT_VALUETYPE_THERM_CAPACI,      new MP_GE(0),                                      0));
  Add(new CLibraryMaterialParameter(MLD_FLUIDMODULUS,        DEF_FLUIDMOD,       m, *new CUnitTypeStress,        IDT_VALUETYPE_FLUID_BULK_MOD,    new MP_GT(0),                                      0));
  Add(new CLibraryMaterialParameter(MLD_FLUIDX,              DEF_FLUIDX,         m, *new CUnitTypePerTemper,     IDT_VALUETYPE_FLUIDX,            new MP_GE_LE(0, 0.1),                              0));
  Add(new CLibraryMaterialParameter(MLD_FLUID_DENSITY,       DEF_FLUID_DENSITY,  m, *new CUnitTypeDensity,       IDT_VALUETYPE_FLUID_DENSITY,     new MP_GT_LE(0, 10000),                            0));
}

int CMaterialDualCapLinElaCreatorDEPRECATED::MaterialModel()
{
  return MM_DUALCAP_LINELA;
}

QString CMaterialDualCapLinElaCreatorDEPRECATED::MaterialModelName()
{
  return MLD_DUALCAP_LINELA;
}

QString CMaterialDualCapLinElaCreatorDEPRECATED::CalibrationPath() const
{
  return "DOHALI";
}

IPQPlotter* CMaterialDualCapLinElaCreatorDEPRECATED::PQPlotter() const
{
  return new CMaterialDualCapCreator::CPQPlotter;
}


bool CMaterialDualCapLinElaHardeningCheckStrategy::operator()(double dValue, const ml::CMatParam& param, QString& strErrorMsg, int nUnitDef) const
{
  const ml::CMaterial& mat = param.ParentMaterial();

  const ml::CMatParam* pSecHardening = mat.MatParameter(MLD_SEC_HARDENING);
  assert(pSecHardening);

  double dSecHardening = pSecHardening->ValueToUserUnit(nUnitDef);
  if (dValue <= 0)
  {
  strErrorMsg = QObject::tr("Hardening must be positive.");
  return false;
  }
  if (dValue + dSecHardening <= 0)
  {
  strErrorMsg = QObject::tr("Hardening and Secondary Hardening together must be positive.");
  return false;
  }

  return true;
}

bool CMaterialDualCapLinElaSecondaryHardeningCheckStrategy::operator()(double dValue, const ml::CMatParam& param, QString& strErrorMsg, int nUnitDef) const
{
  const ml::CMaterial& mat = param.ParentMaterial();

  const ml::CMatParam* pHardening = mat.MatParameter(MLD_HARDENING);
  assert(pHardening);

  double dHardening = pHardening->ValueToUserUnit(nUnitDef);
  if(dValue + dHardening <= 0)
  {
  strErrorMsg = QObject::tr("Hardening and Secondary Hardening together must be positive.");
  return false;
  }

  return true;
}
