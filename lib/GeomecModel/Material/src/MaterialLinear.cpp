#include "MaterialLinear.h"
#include "ValueTypes.h"

#include "lbfl.h"

#include "Material.h"
#include "IObject.h"

CMaterialLinear::CMaterialLinear(CMaterialEntry &entry, CLibraryMaterial& libmat)
: IMaterialRock(entry, libmat)
{
}

bool CMaterialLinear::Write(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const
{
	ftn_double_t ddum;

	ddum = (ftn_double_t) (ffmat.ParameterValue(IDT_VALUETYPE_COHESION) * 1e6);
  PutItemLength("COHESI", &ddum, 1);

	ddum = (ftn_double_t) (ffmat.ParameterValue(IDT_VALUETYPE_FRICTION_ANGLE) * PI / 180);
	PutItemLength("PHI", &ddum, 1);

	return IMaterial::Write(ffmat, diarunner);
}

int CMaterialLinear::WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const
{
  int size = 2; // COHESI/PHI
  size += IMaterial::WriteFilosParamSize(ffmat, diarunner);
  return size;
}

bool CMaterialLinear::WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, int i, char *name) const
{
  if (i == 0)
  {
    strncpy(name, "COHESI", 10);
    return true;
  }
  --i;

  if (i == 0)
  {
    strncpy(name, "PHI", 10);
    return true;
  }
  --i;

  return IMaterial::WriteFilosParamName(ffmat, diarunner, i, name);
}

void CMaterialLinear::WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, double *values, int stride) const
{
  *values = ffmat.ParameterValue(IDT_VALUETYPE_COHESION) * 1e6; // COHESI
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_FRICTION_ANGLE) * PI / 180; // PHI
  values += stride;

  IMaterial::WriteFilosParamValues(ffmat, diarunner, values, stride);
}
