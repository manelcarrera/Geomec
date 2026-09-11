#include "MaterialAnisotropicCamClay.h"
#include "ValueTypes.h"
#include "ModelBase.h"

#include "lbfl.h"

#include "Material.h"
#include "IObject.h"
#include "Matrix.h"
#include "GeomecDianaRunner.h"

CMaterialAnisotropicCamClay::CMaterialAnisotropicCamClay(CMaterialEntry &entry, CLibraryMaterial& libmat)
: IMaterialRock(entry, libmat)
{
}

bool CMaterialAnisotropicCamClay::Write(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const
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

  if(!WriteTransverseIsotropicValues(ffmat, diarunner))
  return false;

  double betal = ffmat.ParameterValue(IDT_VALUETYPE_THERM_LIN_EXP_LAT);
  double betat = ffmat.ParameterValue(IDT_VALUETYPE_THERM_LIN_EXP_NORM);
  ftn_double_t thermx[3];
  thermx[0] = thermx[1] = betal;
  thermx[2] = betat;
  PutItemLength("THERMX", thermx, 3);

  return IMaterialRock::Write(ffmat, diarunner);
}

// Interface for dia::IElementProperty
int CMaterialAnisotropicCamClay::WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const
{
  int size = 8; // POROSI/LAMBDA/PSHIFT/CCSHFA/PRECON/CAP/PHI/COHESI

  size += WriteFilosTransverseIsotropicParamSize(ffmat, diarunner);

  size += 3; // THERMX

  size += IMaterialRock::WriteFilosParamSize(ffmat, diarunner);
  return size;
}

bool CMaterialAnisotropicCamClay::WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, int i, char *name) const
{
  if (i < 8)
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
  default:
      assert(false);
  }
  return true;
  }
  i -= 8;

  int transIsoSize = WriteFilosTransverseIsotropicParamSize(ffmat, diarunner);
  if (i < transIsoSize)
  {
  return WriteFilosTransverseIsotropicParamName(ffmat, diarunner, i, name);
  }
  i -= transIsoSize;

  if (i < 3)
  {
  QString thermx = QString("THERMX(%1)").arg(i + 1);
  strncpy(name, thermx.toStdString().c_str(), 10);
  return true;
  }
  i -= 3;

  return IMaterialRock::WriteFilosParamName(ffmat, diarunner, i, name);
}

void CMaterialAnisotropicCamClay::WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, double *values, int stride) const
{
  *values = ffmat.ParameterValue(IDT_VALUETYPE_POROSITY);
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

  WriteFilosTransverseIsotropicParamValues(ffmat, diarunner, values, stride);

  double betal = ffmat.ParameterValue(IDT_VALUETYPE_THERM_LIN_EXP_LAT);
  double betat = ffmat.ParameterValue(IDT_VALUETYPE_THERM_LIN_EXP_NORM);

  *values = betal; // THERMX(1)
  values += stride;

  *values = betal; // THERMX(2)
  values += stride;

  *values = betat; // THERMX(3)
  values += stride;

  IMaterialRock::WriteFilosParamValues(ffmat, diarunner, values, stride);
}

bool CMaterialAnisotropicCamClay::WriteDefaultPorosity() const
{
  return false;
}
