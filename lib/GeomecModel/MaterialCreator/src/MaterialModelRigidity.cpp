
#include "MaterialModelRigidity.h"
#include "LibraryMaterial.h"
#include "LibraryMaterialParameter.h"
#include "MaterialHelperFactory.h"
#include "MaterialModelLinear.h"
#include "MaterialUnitTypes.h"
#include "Materials.h"
#include "ValueTypes.h"
#include "mlMatParamDomain.h"

#include <cassert>

void CMaterialRigidityCreatorV2::OnCreateLibrary(CLibraryMaterial &m) { m.BehavesLinearly(true); }

void CMaterialRigidityCreatorV2::OnCreateLibraryParameters(CLibraryMaterial &m) {
  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();
  assert(f);

  AddNumber(m, 37);

  AddLibParam(MLD_RHOB, DEF_RHOB, m, f->getMPUnitType(CMaterialHelperFactory::UT_DENSITY), IDT_VALUETYPE_RHOB,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0), 0, QString(), false);
  AddLibParam(MLD_POROSITY, DEF_PORO, m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE), IDT_VALUETYPE_POROSITY,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LT_1), 0, QString(), false);
  AddLibParam(MLD_GRAINSTIFFNESS, DEF_GRAINSTIFFNESS, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_GRAINSTIFFNESS, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_10_LE_1E6), 0,
              QString(), false);
  AddLibParam(MLD_PERMEA, DEF_PERMEA, m, f->getMPUnitType(CMaterialHelperFactory::UT_PERMEA), IDT_VALUETYPE_PERMEA,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0), 0, QString(), false);
  AddLibParam(MLD_VISCOSITY, DEF_VISCOSITY, m, f->getMPUnitType(CMaterialHelperFactory::UT_VISCOSITY),
              IDT_VALUETYPE_VISCOSITY, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0), 0, QString(), false);
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
  AddLibParam(MLD_RIGIDITY_PARAM1, DEF_RIGID1, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_RIGI_PARAM1, 0, 0, QString(), false);
  AddLibParam(MLD_RIGIDITY_PARAM2, DEF_RIGID2, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_RIGI_PARAM2, 0, 0, QString(), false);
  AddLibParam(MLD_RIGIDITY_PARAM3, DEF_RIGID2, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_RIGI_PARAM3, 0, 0, QString(), false);
  AddLibParam(MLD_RIGIDITY_PARAM4, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS), IDT_VALUETYPE_RIGI_PARAM4,
              0, 0, QString(), false);
  AddLibParam(MLD_RIGIDITY_PARAM5, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS), IDT_VALUETYPE_RIGI_PARAM5,
              0, 0, QString(), false);
  AddLibParam(MLD_RIGIDITY_PARAM6, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS), IDT_VALUETYPE_RIGI_PARAM6,
              0, 0, QString(), false);
  AddLibParam(MLD_RIGIDITY_PARAM7, DEF_RIGID1, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_RIGI_PARAM7, 0, 0, QString(), false);
  AddLibParam(MLD_RIGIDITY_PARAM8, DEF_RIGID2, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_RIGI_PARAM8, 0, 0, QString(), false);
  AddLibParam(MLD_RIGIDITY_PARAM9, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS), IDT_VALUETYPE_RIGI_PARAM9,
              0, 0, QString(), false);

  AddLibParam(MLD_RIGIDITY_PARAM10, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_RIGI_PARAM10, 0, 0, QString(), false);
  AddLibParam(MLD_RIGIDITY_PARAM11, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_RIGI_PARAM11, 0, 0, QString(), false);
  AddLibParam(MLD_RIGIDITY_PARAM12, DEF_RIGID1, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_RIGI_PARAM12, 0, 0, QString(), false);
  AddLibParam(MLD_RIGIDITY_PARAM13, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_RIGI_PARAM13, 0, 0, QString(), false);
  AddLibParam(MLD_RIGIDITY_PARAM14, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_RIGI_PARAM14, 0, 0, QString(), false);
  AddLibParam(MLD_RIGIDITY_PARAM15, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_RIGI_PARAM15, 0, 0, QString(), false);
  AddLibParam(MLD_RIGIDITY_PARAM16, DEF_RIGID3, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_RIGI_PARAM16, 0, 0, QString(), false);
  AddLibParam(MLD_RIGIDITY_PARAM17, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_RIGI_PARAM17, 0, 0, QString(), false);
  AddLibParam(MLD_RIGIDITY_PARAM18, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_RIGI_PARAM18, 0, 0, QString(), false);
  AddLibParam(MLD_RIGIDITY_PARAM19, DEF_RIGID3, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_RIGI_PARAM19, 0, 0, QString(), false);

  AddLibParam(MLD_RIGIDITY_PARAM20, 0, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_RIGI_PARAM20, 0, 0, QString(), false);
  AddLibParam(MLD_RIGIDITY_PARAM21, DEF_RIGID3, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_RIGI_PARAM21, 0, 0, QString(), false);
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

int CMaterialRigidityCreatorV2::MaterialModel() { return MM_RIGIDITY; }

QString CMaterialRigidityCreatorV2::MaterialModelName() { return MLD_RIGIDITY; }

QString CMaterialRigidityCreatorV2::CalibrationPath() const { return QString(); }

void CMaterialRigidityCreatorDEPRECATED::OnCreateLibrary(CLibraryMaterial & /*m*/) {}

void CMaterialRigidityCreatorDEPRECATED::OnCreateLibraryParameters(CLibraryMaterial &m) {
  Add(new CLibraryMaterialParameter(MLD_RHOB, DEF_RHOB, m, *new CUnitTypeDensity, IDT_VALUETYPE_RHOB, new MP_GT(0), 0));
  Add(new CLibraryMaterialParameter(MLD_POROSITY, DEF_PORO, m, *new CUnitTypeNone, IDT_VALUETYPE_POROSITY,
                                    new MP_GE_LT(0, 1), 0));
  Add(new CLibraryMaterialParameter(MLD_GRAINSTIFFNESS, DEF_GRAINSTIFFNESS, m, *new CUnitTypeStress,
                                    IDT_VALUETYPE_GRAINSTIFFNESS, new MP_GE_LE(10, 1000000), 0));
  Add(new CLibraryMaterialParameter(MLD_PERMEA, DEF_PERMEA, m, *new CUnitTypePermea, IDT_VALUETYPE_PERMEA, new MP_GE(0),
                                    0));
  Add(new CLibraryMaterialParameter(MLD_VISCOSITY, DEF_VISCOSITY, m, *new CUnitTypeViscosity, IDT_VALUETYPE_VISCOSITY,
                                    new MP_GT(0), 0));
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
  Add(new CLibraryMaterialParameter(MLD_RIGIDITY_PARAM1, DEF_RIGID1, m, *new CUnitTypeStress, IDT_VALUETYPE_RIGI_PARAM1,
                                    0, 0));
  Add(new CLibraryMaterialParameter(MLD_RIGIDITY_PARAM2, DEF_RIGID2, m, *new CUnitTypeStress, IDT_VALUETYPE_RIGI_PARAM2,
                                    0, 0));
  Add(new CLibraryMaterialParameter(MLD_RIGIDITY_PARAM3, DEF_RIGID2, m, *new CUnitTypeStress, IDT_VALUETYPE_RIGI_PARAM3,
                                    0, 0));
  Add(new CLibraryMaterialParameter(MLD_RIGIDITY_PARAM4, 0, m, *new CUnitTypeStress, IDT_VALUETYPE_RIGI_PARAM4, 0, 0));
  Add(new CLibraryMaterialParameter(MLD_RIGIDITY_PARAM5, 0, m, *new CUnitTypeStress, IDT_VALUETYPE_RIGI_PARAM5, 0, 0));
  Add(new CLibraryMaterialParameter(MLD_RIGIDITY_PARAM6, 0, m, *new CUnitTypeStress, IDT_VALUETYPE_RIGI_PARAM6, 0, 0));
  Add(new CLibraryMaterialParameter(MLD_RIGIDITY_PARAM7, DEF_RIGID1, m, *new CUnitTypeStress, IDT_VALUETYPE_RIGI_PARAM7,
                                    0, 0));
  Add(new CLibraryMaterialParameter(MLD_RIGIDITY_PARAM8, DEF_RIGID2, m, *new CUnitTypeStress, IDT_VALUETYPE_RIGI_PARAM8,
                                    0, 0));
  Add(new CLibraryMaterialParameter(MLD_RIGIDITY_PARAM9, 0, m, *new CUnitTypeStress, IDT_VALUETYPE_RIGI_PARAM9, 0, 0));
  Add(new CLibraryMaterialParameter(MLD_RIGIDITY_PARAM10, 0, m, *new CUnitTypeStress, IDT_VALUETYPE_RIGI_PARAM10, 0,
                                    0));
  Add(new CLibraryMaterialParameter(MLD_RIGIDITY_PARAM11, 0, m, *new CUnitTypeStress, IDT_VALUETYPE_RIGI_PARAM11, 0,
                                    0));
  Add(new CLibraryMaterialParameter(MLD_RIGIDITY_PARAM12, DEF_RIGID1, m, *new CUnitTypeStress,
                                    IDT_VALUETYPE_RIGI_PARAM12, 0, 0));
  Add(new CLibraryMaterialParameter(MLD_RIGIDITY_PARAM13, 0, m, *new CUnitTypeStress, IDT_VALUETYPE_RIGI_PARAM13, 0,
                                    0));
  Add(new CLibraryMaterialParameter(MLD_RIGIDITY_PARAM14, 0, m, *new CUnitTypeStress, IDT_VALUETYPE_RIGI_PARAM14, 0,
                                    0));
  Add(new CLibraryMaterialParameter(MLD_RIGIDITY_PARAM15, 0, m, *new CUnitTypeStress, IDT_VALUETYPE_RIGI_PARAM15, 0,
                                    0));
  Add(new CLibraryMaterialParameter(MLD_RIGIDITY_PARAM16, DEF_RIGID3, m, *new CUnitTypeStress,
                                    IDT_VALUETYPE_RIGI_PARAM16, 0, 0));
  Add(new CLibraryMaterialParameter(MLD_RIGIDITY_PARAM17, 0, m, *new CUnitTypeStress, IDT_VALUETYPE_RIGI_PARAM17, 0,
                                    0));
  Add(new CLibraryMaterialParameter(MLD_RIGIDITY_PARAM18, 0, m, *new CUnitTypeStress, IDT_VALUETYPE_RIGI_PARAM18, 0,
                                    0));
  Add(new CLibraryMaterialParameter(MLD_RIGIDITY_PARAM19, DEF_RIGID3, m, *new CUnitTypeStress,
                                    IDT_VALUETYPE_RIGI_PARAM19, 0, 0));
  Add(new CLibraryMaterialParameter(MLD_RIGIDITY_PARAM20, 0, m, *new CUnitTypeStress, IDT_VALUETYPE_RIGI_PARAM20, 0,
                                    0));
  Add(new CLibraryMaterialParameter(MLD_RIGIDITY_PARAM21, DEF_RIGID3, m, *new CUnitTypeStress,
                                    IDT_VALUETYPE_RIGI_PARAM21, 0, 0));
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

int CMaterialRigidityCreatorDEPRECATED::MaterialModel() { return MM_RIGIDITY; }

QString CMaterialRigidityCreatorDEPRECATED::MaterialModelName() { return MLD_RIGIDITY; }

QString CMaterialRigidityCreatorDEPRECATED::CalibrationPath() const { return QString(); }
