#include "MaterialMohrCo.h"
#include "ValueTypes.h"
#include "lbfl.h"
#include "Material.h"
#include "IObject.h"

CMaterialMohrCo::CMaterialMohrCo(CMaterialEntry &entry, CLibraryMaterial& libmat)
: IMaterialRock(entry, libmat)
{
}

bool CMaterialMohrCo::Write(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const
{
	ftn_double_t ddum;

	double dCohesi = ffmat.ParameterValue(IDT_VALUETYPE_COHESION) * 1e6;
	double dFricti = ffmat.ParameterValue(IDT_VALUETYPE_FRICTION_ANGLE) * PI / 180;
	double dDilatation = ffmat.ParameterValue(IDT_VALUETYPE_DILATATION) * PI / 180;

	PutCharItem("YIELD", "MOHRCO");

  ftn_double_t cohesi = (ftn_double_t)dCohesi;
  PutItemLength("COHESI", &cohesi, 1);

	ftn_double_t psi = (ftn_double_t) (dDilatation);
  PutItem("PSI", &psi);

	ddum = (ftn_double_t) (dFricti);
	PutItemLength("PHI", &ddum, 1);

	return IMaterial::Write(ffmat, diarunner);
}


// Interface for dia::IElementProperty
int CMaterialMohrCo::WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const
{
  int size = 3; // COHESI/PSI/PHI

  size += IMaterialRock::WriteFilosParamSize(ffmat, diarunner);
  return size;
}

bool CMaterialMohrCo::WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, int i, char *name) const
{
  if (i == 0)
  {
    strncpy(name, "COHESI", 10);
    return true;
  }
  --i;

  if (i == 0)
  {
    strncpy(name, "PSI", 10);
    return true;
  }
  --i;

  if (i == 0)
  {
    strncpy(name, "PHI", 10);
    return true;
  }
  --i;

  return IMaterialRock::WriteFilosParamName(ffmat, diarunner, i, name);
}

void CMaterialMohrCo::WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, double *values, int stride) const
{
  double dCohesi = ffmat.ParameterValue(IDT_VALUETYPE_COHESION) * 1e6;
  double dFricti = ffmat.ParameterValue(IDT_VALUETYPE_FRICTION_ANGLE) * PI / 180;
  double dDilatation = ffmat.ParameterValue(IDT_VALUETYPE_DILATATION) * PI / 180;

  *values = dCohesi; // COHESI
  values += stride;

  *values = dDilatation; // PSI
  values += stride;

  *values = dFricti; // PHI
  values += stride;

  IMaterialRock::WriteFilosParamValues(ffmat, diarunner, values, stride);
}
