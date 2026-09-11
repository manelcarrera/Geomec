
#include "MaterialUndrained.h"
#include "ValueTypes.h"

#include "Material.h"
#include "lbfl.h"

CMaterialUndrained::CMaterialUndrained(CMaterialEntry &entry, CLibraryMaterial &libmat)
    : IMaterialRock(entry, libmat) {}

bool CMaterialUndrained::Write(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner) const {
  ftn_double_t ddum;

  ddum = (ftn_double_t)ffmat.ParameterValue(IDT_VALUETYPE_POROSITY);
  PutItem("POROSI", &ddum);

  ddum = (ftn_double_t)(ffmat.ParameterValue(IDT_VALUETYPE_FLUID_BULK_MOD) * 1e6);
  PutItem("BULKF", &ddum);

  SetActive("SKEMPT", FTN_TRUE);

  return IMaterial::Write(ffmat, diarunner);
}

// Interface for dia::IElementProperty
int CMaterialUndrained::WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner) const {
  int size = 2; // POROSI/BULKF

  size += IMaterialRock::WriteFilosParamSize(ffmat, diarunner);
  return size;
}

bool CMaterialUndrained::WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner, int i,
                                             char *name) const {
  if (i == 0) {
    strncpy(name, "POROSI", 10);
    return true;
  }
  --i;

  if (i == 0) {
    strncpy(name, "BULKF", 10);
    return true;
  }
  --i;

  return IMaterialRock::WriteFilosParamName(ffmat, diarunner, i, name);
}

void CMaterialUndrained::WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner, double *values,
                                               int stride) const {
  *values = ffmat.ParameterValue(IDT_VALUETYPE_POROSITY); // POROSITY
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_FLUID_BULK_MOD) * 1e6; // BULKF
  values += stride;

  IMaterialRock::WriteFilosParamValues(ffmat, diarunner, values, stride);
}
