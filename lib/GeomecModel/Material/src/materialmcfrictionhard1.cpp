#include "materialmcfrictionhard1.h"
#include "ValueTypes.h"

#include "lbfl.h"

#include "IObject.h"
#include "Material.h"

CMaterialMCFrictionHard1::CMaterialMCFrictionHard1(CMaterialEntry &entry, CLibraryMaterial &libmat)
    : CMaterialMohrCo(entry, libmat) {}

bool CMaterialMCFrictionHard1::Write(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner) const {
  ftn_double_t discoh[4];

  discoh[0] = 0;
  discoh[1] = sin(PI * (ffmat.ParameterValue(IDT_VALUETYPE_FRICTION_ANGLE) / 180));
  discoh[2] = ffmat.ParameterValue(IDT_VALUETYPE_EQUIV_PLAST_STRAIN1);
  discoh[3] = sin(PI * (ffmat.ParameterValue(IDT_VALUETYPE_HARD_FRICTION1) / 180));

  PutItemLength("DISCOH", discoh, 4);

  return CMaterialMohrCo::Write(ffmat, diarunner);
}

// Interface for dia::IElementProperty
int CMaterialMCFrictionHard1::WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner) const {
  int size = 4; // DISCOH(4)

  size += CMaterialMohrCo::WriteFilosParamSize(ffmat, diarunner);
  return size;
}

bool CMaterialMCFrictionHard1::WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner, int i,
                                                   char *name) const {
  if (i < 4) {
    QString discoh = QString("DISCOH(%1)").arg(i + 1);
    strncpy(name, discoh.toStdString().c_str(), 10);
    return true;
  }
  i -= 4;

  return CMaterialMohrCo::WriteFilosParamName(ffmat, diarunner, i, name);
}

void CMaterialMCFrictionHard1::WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner,
                                                     double *values, int stride) const {
  *values = 0; // DISCOH(1)
  values += stride;

  *values = sin(PI * (ffmat.ParameterValue(IDT_VALUETYPE_FRICTION_ANGLE) / 180)); // DISCOH(2)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_EQUIV_PLAST_STRAIN1); // DISCOH(3)
  values += stride;

  *values = sin(PI * (ffmat.ParameterValue(IDT_VALUETYPE_HARD_FRICTION1) / 180)); // DISCOH(4)
  values += stride;

  CMaterialMohrCo::WriteFilosParamValues(ffmat, diarunner, values, stride);
}
