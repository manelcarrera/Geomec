#include "MaterialDualCapLinEla.h"
#include "ValueTypes.h"

#include "lbfl.h"

#include "Material.h"
#include "IObject.h"

CMaterialDualCapLinEla::CMaterialDualCapLinEla(CMaterialEntry &entry, CLibraryMaterial& libmat)
: IMaterialRock(entry, libmat)
{
}

bool CMaterialDualCapLinEla::Write(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const
{
  ftn_double_t ddum;

  ddum = (ftn_double_t) ffmat.ParameterValue(IDT_VALUETYPE_POROSITY);
  PutItem("POROSI", &ddum);

  PutCharItem("YIELD", "CLAY");

  double dIniFriction = ffmat.ParameterValue(IDT_VALUETYPE_INITFRICTION) * PI / 180;
  double dHardening   = ffmat.ParameterValue(IDT_VALUETYPE_HARDENING);
  double dCohesion    = ffmat.ParameterValue(IDT_VALUETYPE_COHESION) * 1e6;
  double dGamma       = ffmat.ParameterValue(IDT_VALUETYPE_TENSILE_STRETCH);

  ftn_double_t lambda = (ftn_double_t) (dHardening);
  PutItem("LAMBDA", &lambda);

  ftn_double_t pshift = (ftn_double_t) (dCohesion / tan(dIniFriction));
  PutItem("PSHIFT", &pshift);

  ftn_double_t ccshfa = (ftn_double_t) dGamma;
  PutItem("CCSHFA", &ccshfa);

  ddum = (ftn_double_t) (ffmat.ParameterValue(IDT_VALUETYPE_PRECONSOLIDATION) * 1e6);
  PutItemLength("PRECON", &ddum, 1);

  ddum = (ftn_double_t) (ffmat.ParameterValue(IDT_VALUETYPE_CAPSHAPE));
  PutItemLength("CAP", &ddum, 1);

  ddum = (ftn_double_t) (dIniFriction);
  PutItemLength("PHI", &ddum, 1);

  ddum = (ftn_double_t) (dCohesion);
  PutItemLength("COHESI", &ddum, 1);

  double dSecondaryPreconsolidation = ffmat.ParameterValue(IDT_VALUETYPE_SEC_PRECON) * 1e6;
  double dSecondaryHardening = ffmat.ParameterValue(IDT_VALUETYPE_SEC_HARDENING) + dHardening;

  ftn_double_t douhar[2];
  douhar[0] = (ftn_double_t)dSecondaryPreconsolidation;
  douhar[1] = (ftn_double_t)dSecondaryHardening;

  PutItemLength("DOUHAR", douhar, 2);

  return IMaterial::Write(ffmat, diarunner);
}


// Interface for dia::IElementProperty
int CMaterialDualCapLinEla::WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const
{
  int size = 10; // POROSI/LAMBDA/PSHIFT/CCSHFA/PRECON/CAP/PHI/COHESI/DOUHAR(2)

  size += IMaterialRock::WriteFilosParamSize(ffmat, diarunner);
  return size;
}

bool CMaterialDualCapLinEla::WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, int i, char *name) const
{
  if (i < 10)
  {
  switch (i)
  {
  case 0:
      strncpy(name, "POROSI", 10);
      break;
  case 1:
      strncpy(name, "LAMBDA", 10);
      break;
  case 2:
      strncpy(name, "PSHIFT", 10);
      break;
  case 3:
      strncpy(name, "CCSHFA", 10);
      break;
  case 4:
      strncpy(name, "PRECON", 10);
      break;
  case 5:
      strncpy(name, "CAP", 10);
      break;
  case 6:
      strncpy(name, "PHI", 10);
      break;
  case 7:
      strncpy(name, "COHESI", 10);
      break;
  case 8:
      strncpy(name, "DOUHAR(1)", 10);
      break;
  case 9:
      strncpy(name, "DOUHAR(2)", 10);
      break;
  }
  }
  i -= 10;

  return IMaterialRock::WriteFilosParamName(ffmat, diarunner, i, name);
}

void CMaterialDualCapLinEla::WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, double *values, int stride) const
{
  *values = ffmat.ParameterValue(IDT_VALUETYPE_POROSITY); // POROSI
  values += stride;

  double dIniFriction = ffmat.ParameterValue(IDT_VALUETYPE_INITFRICTION) * PI / 180;
  double dHardening = ffmat.ParameterValue(IDT_VALUETYPE_HARDENING);
  double dCohesion = ffmat.ParameterValue(IDT_VALUETYPE_COHESION) * 1e6;
  double dGamma = ffmat.ParameterValue(IDT_VALUETYPE_TENSILE_STRETCH);

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

  *values = ffmat.ParameterValue(IDT_VALUETYPE_SEC_PRECON) * 1e6; // DOUHAR(1)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_SEC_HARDENING) + dHardening; // DOUHAR(2)
  values += stride;

  IMaterialRock::WriteFilosParamValues(ffmat, diarunner, values, stride);
}

bool CMaterialDualCapLinEla::WriteDefaultPorosity() const
{
  return false;
}