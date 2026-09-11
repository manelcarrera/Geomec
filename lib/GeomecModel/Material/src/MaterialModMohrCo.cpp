
#include "MaterialModMohrCo.h"
#include "ValueTypes.h"

#include "IObject.h"
#include "Material.h"
#include "lbfl.h"

CMaterialModMohrCo::CMaterialModMohrCo(CMaterialEntry &entry, CLibraryMaterial &libmat)
    : IMaterialRock(entry, libmat) {}

bool CMaterialModMohrCo::Write(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner) const {
  ftn_double_t ddum;

  double dCohesi = ffmat.ParameterValue(IDT_VALUETYPE_COHESION) * 1e6;
  double dFricti = ffmat.ParameterValue(IDT_VALUETYPE_FRICTION_ANGLE) * PI / 180;
  double dPreconsolidation = ffmat.ParameterValue(IDT_VALUETYPE_PRECONSOLIDATION) * 1e6;
  double dHardening = ffmat.ParameterValue(IDT_VALUETYPE_HARDENING);
  double dPorosity = ffmat.ParameterValue(IDT_VALUETYPE_POROSITY);
  double dCapShape = ffmat.ParameterValue(IDT_VALUETYPE_CAPSHAPEPARAM);

  PutCharItem("YIELD", "MMOHRC");

  ddum = (ftn_double_t)(dPreconsolidation);
  PutItemLength("PRECON", &ddum, 1);

  ddum = (ftn_double_t)(dCohesi);
  PutItemLength("COHESI", &dCohesi, 1);

  ddum = (ftn_double_t)(dFricti);
  PutItemLength("PHI", &ddum, 1);

  ddum = (ftn_double_t)(dCohesi / tan(dFricti));
  PutItem("PSHIFT", &ddum);

  PutCharItem("COMCRV", "EXPHAR");

  ddum = (ftn_double_t)(dHardening);
  PutItem("GAMMA", &ddum);

  ddum = (ftn_double_t)(dPorosity);
  PutItem("POROSI", &ddum);

  ddum = (ftn_double_t)(dCapShape);
  PutItemLength("CAP", &ddum, 1);

  PutCharItem("HARDEN", "COUPLE");

  double d = 0;
  PutItemLength("FLOCAP", &d, 0);

  return IMaterial::Write(ffmat, diarunner);
}

// Interface for dia::IElementProperty
int CMaterialModMohrCo::WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner) const {
  int size = 8; // PRECON/COHESI/PHI/PSHIFT/GAMMA/POROSI/CAP/FLOCAP

  size += IMaterialRock::WriteFilosParamSize(ffmat, diarunner);
  return size;
}

bool CMaterialModMohrCo::WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner, int i,
                                             char *name) const {
  if (i < 8) {
    switch (i) {
    case 0:
      strncpy(name, "PRECON", 10);
      break;
    case 1:
      strncpy(name, "COHESI", 10);
      break;
    case 2:
      strncpy(name, "PHI", 10);
      break;
    case 3:
      strncpy(name, "PSHIFT", 10);
      break;
    case 4:
      strncpy(name, "GAMMA", 10);
      break;
    case 5:
      strncpy(name, "POROSI", 10);
      break;
    case 6:
      strncpy(name, "CAP", 10);
      break;
    case 7:
      strncpy(name, "FLOCAP", 10);
      break;
    default:
      assert(false);
    }
    return true;
  }
  i -= 8;

  return IMaterialRock::WriteFilosParamName(ffmat, diarunner, i, name);
}

void CMaterialModMohrCo::WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner, double *values,
                                               int stride) const {
  double dCohesi = ffmat.ParameterValue(IDT_VALUETYPE_COHESION) * 1e6;
  double dFricti = ffmat.ParameterValue(IDT_VALUETYPE_FRICTION_ANGLE) * PI / 180;
  double dPreconsolidation = ffmat.ParameterValue(IDT_VALUETYPE_PRECONSOLIDATION) * 1e6;
  double dHardening = ffmat.ParameterValue(IDT_VALUETYPE_HARDENING);
  double dPorosity = ffmat.ParameterValue(IDT_VALUETYPE_POROSITY);
  double dCapShape = ffmat.ParameterValue(IDT_VALUETYPE_CAPSHAPEPARAM);

  *values = dPreconsolidation; // PRECON
  values += stride;

  *values = dCohesi; // COHESI
  values += stride;

  *values = dFricti; // PHI
  values += stride;

  *values = dCohesi / tan(dFricti); // PSHIFT
  values += stride;

  *values = dHardening; // GAMMA
  values += stride;

  *values = dPorosity; // POROSITY
  values += stride;

  *values = dCapShape; // CAP
  values += stride;

  *values = 0; // FLOCAP
  values += stride;

  IMaterialRock::WriteFilosParamValues(ffmat, diarunner, values, stride);
}

bool CMaterialModMohrCo::WriteDefaultPorosity() const { return false; }