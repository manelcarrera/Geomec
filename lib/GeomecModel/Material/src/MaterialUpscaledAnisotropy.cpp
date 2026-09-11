
#include "MaterialUpscaledAnisotropy.h"
#include "ValueTypes.h"

#include "GeomecDianaRunner.h"
#include "Material.h"
#include "lbfl.h"

CMaterialUpscaledAnisotropy::CMaterialUpscaledAnisotropy(CMaterialEntry &entry, CLibraryMaterial &libmat)
    : IMaterialRock(entry, libmat) {}

bool CMaterialUpscaledAnisotropy::Write(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner) const {
  if (!WriteTransverseIsotropicValues(ffmat, diarunner))
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
int CMaterialUpscaledAnisotropy::WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner) const {
  int size = WriteFilosTransverseIsotropicParamSize(ffmat, diarunner);

  size += 3; // THERMX(3)

  size += IMaterialRock::WriteFilosParamSize(ffmat, diarunner);
  return size;
}

bool CMaterialUpscaledAnisotropy::WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner, int i,
                                                      char *name) const {
  int transIsoSize = WriteFilosTransverseIsotropicParamSize(ffmat, diarunner);
  if (i < transIsoSize) {
    return WriteFilosTransverseIsotropicParamName(ffmat, diarunner, i, name);
  }
  i -= transIsoSize;

  if (i < 3) {
    QString thermx = QString("THERMX(%1)").arg(i + 1);
    strncpy(name, thermx.toStdString().c_str(), 10);
    return true;
  }
  i -= 3;

  return IMaterialRock::WriteFilosParamName(ffmat, diarunner, i, name);
}

void CMaterialUpscaledAnisotropy::WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner,
                                                        double *values, int stride) const {
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
