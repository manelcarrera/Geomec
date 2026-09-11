#include "MaterialCamClayCreep.h"
#include "ValueTypes.h"

#include "lbfl.h"

#include "IObject.h"
#include "Material.h"

CMaterialCamClayCreep::CMaterialCamClayCreep(CMaterialEntry &entry, CLibraryMaterial &libmat)
    : IMaterialRock(entry, libmat) {}

bool CMaterialCamClayCreep::Write(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner) const {
  ftn_double_t ddum;

  ddum = (ftn_double_t)ffmat.ParameterValue(IDT_VALUETYPE_POROSITY);
  PutItem("POROSI", &ddum);

  PutCharItem("YIELD", "CLAY");

  double dIniFriction = ffmat.ParameterValue(IDT_VALUETYPE_INITFRICTION) * PI / 180;
  double dHardening = ffmat.ParameterValue(IDT_VALUETYPE_HARDENING);
  double dCohesion = ffmat.ParameterValue(IDT_VALUETYPE_COHESION) * 1e6;
  double dGamma = ffmat.ParameterValue(IDT_VALUETYPE_TENSILE_STRETCH);
  double dCreepA1 = ffmat.ParameterValue(IDT_VALUETYPE_CREEP_A1);
  double dCreepN1 = ffmat.ParameterValue(IDT_VALUETYPE_CREEP_N1);

  // waij TFS 82694 1/sec -> 1/month
  double secs_per_month = 365.25 * 24 * 60 * 60 / 12.0;
  dCreepA1 *= secs_per_month;

  ftn_double_t inerat = (ftn_double_t)(dCreepA1);
  PutItem("INERAT", &inerat);

  ftn_double_t expm = (ftn_double_t)(dCreepN1);
  PutItem("EXPM", &expm);

  ftn_double_t lambda = (ftn_double_t)(dHardening);
  PutItem("LAMBDA", &lambda);

  ftn_double_t pshift = (ftn_double_t)(dCohesion / tan(dIniFriction));
  PutItem("PSHIFT", &pshift);

  ftn_double_t ccshfa = (ftn_double_t)dGamma;
  PutItem("CCSHFA", &ccshfa);

  ddum = (ftn_double_t)(ffmat.ParameterValue(IDT_VALUETYPE_PRECONSOLIDATION) * 1e6);
  PutItemLength("PRECON", &ddum, 1);

  ddum = (ftn_double_t)(ffmat.ParameterValue(IDT_VALUETYPE_CAPSHAPE));
  PutItemLength("CAP", &ddum, 1);

  ddum = (ftn_double_t)(dIniFriction);
  PutItemLength("PHI", &ddum, 1);

  ddum = (ftn_double_t)(dCohesion);
  PutItemLength("COHESI", &ddum, 1);

  return IMaterial::Write(ffmat, diarunner);
}

// Interface for dia::IElementProperty
int CMaterialCamClayCreep::WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner) const {
  int size = 10; // POROSI/INERAT/EXPM/LAMBDA/PSHIFT/CCSHFA/PRECON/CAP/PHI/COHESI

  size += IMaterialRock::WriteFilosParamSize(ffmat, diarunner);
  return size;
}

bool CMaterialCamClayCreep::WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner, int i,
                                                char *name) const {
  if (i < 10) {
    switch (i) {
    case 0:
      strncpy(name, "POROSI", 10);
      break;
    case 1:
      strncpy(name, "INERAT", 10);
      break;
    case 2:
      strncpy(name, "EXPM", 10);
      break;
    case 3:
      strncpy(name, "LAMBDA", 10);
      break;
    case 4:
      strncpy(name, "PSHIFT", 10);
      break;
    case 5:
      strncpy(name, "CCSHFA", 10);
      break;
    case 6:
      strncpy(name, "PRECON", 10);
      break;
    case 7:
      strncpy(name, "CAP", 10);
      break;
    case 8:
      strncpy(name, "PHI", 10);
      break;
    case 9:
      strncpy(name, "COHESI", 10);
      break;
    default:
      assert(false);
    }
    return true;
  }
  i -= 10;

  return IMaterialRock::WriteFilosParamName(ffmat, diarunner, i, name);
}

void CMaterialCamClayCreep::WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner,
                                                  double *values, int stride) const {
  *values = ffmat.ParameterValue(IDT_VALUETYPE_POROSITY); // POROSI
  values += stride;

  double dIniFriction = ffmat.ParameterValue(IDT_VALUETYPE_INITFRICTION) * PI / 180;
  double dHardening = ffmat.ParameterValue(IDT_VALUETYPE_HARDENING);
  double dCohesion = ffmat.ParameterValue(IDT_VALUETYPE_COHESION) * 1e6;
  double dGamma = ffmat.ParameterValue(IDT_VALUETYPE_TENSILE_STRETCH);
  double dCreepA1 = ffmat.ParameterValue(IDT_VALUETYPE_CREEP_A1);
  double dCreepN1 = ffmat.ParameterValue(IDT_VALUETYPE_CREEP_N1);

  // waij TFS 82694 1/sec -> 1/month
  double secs_per_month = 365.25 * 24 * 60 * 60 / 12.0;
  dCreepA1 *= secs_per_month;

  *values = dCreepA1; // INERAT
  values += stride;

  *values = dCreepN1; // EXPM
  values += stride;

  *values = dHardening; // LAMBDA
  values += stride;

  *values = dCohesion / tan(dIniFriction); // PSHIFT
  values += stride;

  *values = dGamma; // CCSHFA
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_PRECONSOLIDATION) * 1e6; // PRECON
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_CAPSHAPE); // CAP
  values += stride;

  *values = dIniFriction; // PHI
  values += stride;

  *values = dCohesion; // COHESI
  values += stride;

  IMaterialRock::WriteFilosParamValues(ffmat, diarunner, values, stride);
}

bool CMaterialCamClayCreep::WriteDefaultPorosity() const { return false; }