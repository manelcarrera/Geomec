#include "materialmccohesionhard1.h"
#include "ValueTypes.h"

#include "lbfl.h"

#include "Material.h"

CMaterialMCCohesionHard1::CMaterialMCCohesionHard1(CMaterialEntry &entry, CLibraryMaterial& libmat)
: CMaterialMohrCo(entry, libmat)
{
}

bool CMaterialMCCohesionHard1::Write(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const
{
	ftn_double_t kapcoh[4];

	kapcoh[0] = 0;
	kapcoh[1] = ffmat.ParameterValue(IDT_VALUETYPE_COHESION) * 1e6;
	kapcoh[2] = ffmat.ParameterValue(IDT_VALUETYPE_EQUIV_PLAST_STRAIN1);
	kapcoh[3] = ffmat.ParameterValue(IDT_VALUETYPE_HARD_COHESION1) * 1e6;

	PutItemLength("KAPCOH", kapcoh, 4);

	return CMaterialMohrCo::Write(ffmat, diarunner);
}

// Interface for dia::IElementProperty
int CMaterialMCCohesionHard1::WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const
{
  int size = 4;

  size += CMaterialMohrCo::WriteFilosParamSize(ffmat, diarunner);
  return size;
}

bool CMaterialMCCohesionHard1::WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, int i, char *name) const
{
  if (i < 4)
  {
    QString kapcoh = QString("KAPCOH(%1)").arg(i + 1);
    strncpy(name, kapcoh.toStdString().c_str(), 10);
    return true;
  }
  i -= 4;

  return CMaterialMohrCo::WriteFilosParamName(ffmat, diarunner, i, name);
}

void CMaterialMCCohesionHard1::WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, double *values, int stride) const
{
  *values = 0; // KAPCOH(1)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_COHESION) * 1e6; // KAPCOH(2)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_EQUIV_PLAST_STRAIN1); // KAPCOH(3)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_HARD_COHESION1) * 1e6; // KAPCOH(4)
  values += stride;

  CMaterialMohrCo::WriteFilosParamValues(ffmat, diarunner, values, stride);
}
