#include "materialmcfrictionhard3.h"
#include "ValueTypes.h"
#include "lbfl.h"
#include "Material.h"
#include "IObject.h"

CMaterialMCFrictionHard3::CMaterialMCFrictionHard3(CMaterialEntry &entry, CLibraryMaterial& libmat)
: CMaterialMohrCo(entry, libmat)
{
}

bool CMaterialMCFrictionHard3::Write(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const
{
  ftn_double_t discoh[8];

  discoh[0] = 0;
  discoh[1] = sin( PI * (ffmat.ParameterValue(IDT_VALUETYPE_FRICTION_ANGLE) / 180  ));
  discoh[2] = ffmat.ParameterValue(IDT_VALUETYPE_EQUIV_PLAST_STRAIN1);
  discoh[3] = sin( PI * (ffmat.ParameterValue(IDT_VALUETYPE_HARD_FRICTION1) / 180  ));
  discoh[4] = ffmat.ParameterValue(IDT_VALUETYPE_EQUIV_PLAST_STRAIN2);
  discoh[5] = sin( PI * (ffmat.ParameterValue(IDT_VALUETYPE_HARD_FRICTION2) / 180  ));
  discoh[6] = ffmat.ParameterValue(IDT_VALUETYPE_EQUIV_PLAST_STRAIN3);
  discoh[7] = sin( PI * (ffmat.ParameterValue(IDT_VALUETYPE_HARD_FRICTION3) / 180  ));

  PutItemLength("DISCOH", discoh, 8);

  return CMaterialMohrCo::Write(ffmat, diarunner);
}

// Interface for dia::IElementProperty
int CMaterialMCFrictionHard3::WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const
{
  int size = 8; // DISCO(8)

  size += CMaterialMohrCo::WriteFilosParamSize(ffmat, diarunner);
  return size;
}

bool CMaterialMCFrictionHard3::WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, int i, char *name) const
{
  if (i < 8)
  {
  QString discoh = QString("DISCOH(%1)").arg(i + 1);
  strncpy(name, discoh.toStdString().c_str(), 10);
  return true;
  }
  i -= 8;

  return CMaterialMohrCo::WriteFilosParamName(ffmat, diarunner, i, name);
}

void CMaterialMCFrictionHard3::WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, double *values, int stride) const
{
  *values = 0; // DISCOH(1)
  values += stride;

  *values = sin(PI * (ffmat.ParameterValue(IDT_VALUETYPE_FRICTION_ANGLE) / 180)); // DISCOH(2)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_EQUIV_PLAST_STRAIN1); // DISCOH(3)
  values += stride;

  *values = sin(PI * (ffmat.ParameterValue(IDT_VALUETYPE_HARD_FRICTION1) / 180)); // DISCOH(4)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_EQUIV_PLAST_STRAIN2); // DISCOH(5)
  values += stride;

  *values = sin(PI * (ffmat.ParameterValue(IDT_VALUETYPE_HARD_FRICTION2) / 180)); // DISCOH(6)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_EQUIV_PLAST_STRAIN3); // DISCOH(7)
  values += stride;

  *values = sin(PI * (ffmat.ParameterValue(IDT_VALUETYPE_HARD_FRICTION3) / 180)); // DISCOH(8)
  values += stride;

  CMaterialMohrCo::WriteFilosParamValues(ffmat, diarunner, values, stride);
}
