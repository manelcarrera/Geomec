// MaterialRigidity.cpp: implementation of the CMaterialRigidity class.
//
//////////////////////////////////////////////////////////////////////

#include "MaterialRigidity.h"
#include "ValueTypes.h"

#include "lbfl.h"
#include "Material.h"
#include "IObject.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMaterialRigidity::CMaterialRigidity(CMaterialEntry &entry, CLibraryMaterial& libmat)
: IMaterialRock(entry, libmat)
{

}

CMaterialRigidity::~CMaterialRigidity()
{

}

bool CMaterialRigidity::Write(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const
{
  // ATTENTION!! Don't call the Write function in the base class, we do NOT want to write the 
  // Young's modulus and the Poisson ratio

  ftn_double_t ddum;

  ddum = (ftn_double_t) (ffmat.ParameterValue(IDT_VALUETYPE_COHESION) * 1e6);
  PutItemLength("COHESI", &ddum, 1);

  ddum = (ftn_double_t) (ffmat.ParameterValue(IDT_VALUETYPE_FRICTION_ANGLE) * PI / 180);
  PutItemLength("PHI", &ddum, 1);

  ddum = (ftn_double_t) ffmat.ParameterValue(IDT_VALUETYPE_RHOB);
  PutItem("DENSIT", &ddum);

  ftn_double_t rigidity[21];

  rigidity[0] = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM1) * 1e6;
  rigidity[1] = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM2) * 1e6;;
  rigidity[2] = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM3) * 1e6;
  rigidity[3] = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM4) * 1e6;
  rigidity[4] = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM5) * 1e6;
  rigidity[5] = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM6) * 1e6;
  rigidity[6] = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM7) * 1e6;
  rigidity[7] = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM8) * 1e6;
  rigidity[8] = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM9) * 1e6;
  rigidity[9] = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM10) * 1e6;
  rigidity[10] = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM11) * 1e6;
  rigidity[11] = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM12) * 1e6;
  rigidity[12] = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM13) * 1e6;
  rigidity[13] = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM14) * 1e6;
  rigidity[14] = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM15) * 1e6;
  rigidity[15] = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM16) * 1e6;
  rigidity[16] = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM17) * 1e6;
  rigidity[17] = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM18) * 1e6;
  rigidity[18] = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM19) * 1e6;
  rigidity[19] = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM20) * 1e6;
  rigidity[20] = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM21) * 1e6;

  PutItemLength("RIGIDI", rigidity, 21);

  ddum = (ftn_double_t) ffmat.ParameterValue(IDT_VALUETYPE_THERMAL_EXPANSION);
  ddum /= 3.0; // go from volumetric to linear thermal expansion coefficient
  PutItemLength("THERMX", &ddum, 1);

  // don't call base class
  // we should write K0 values though
  return IMaterial::WriteK0(ffmat, diarunner);
}

int CMaterialRigidity::MaterialModel() const
{
  return MM_RIGIDITY;
}


// Interface for dia::IElementProperty
int CMaterialRigidity::WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const
{
  int size = 25; // COHESI/PHI/DENSIT/RIGIDI(21)/THERMX

  size += IMaterialRock::WriteFilosK0ParamSize(ffmat, diarunner); // Only K0, not base class
  return size;
}

bool CMaterialRigidity::WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, int i, char *name) const
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

  if (i == 0)
  {
  strncpy(name, "DENSIT", 10);
  return true;
  }
  --i;

  if (i < 21)
  {
  QString rigidi = QString("RIGIDI(%1)").arg(i + 1);
  strncpy(name, rigidi.toStdString().c_str(), 10);
  return true;
  }
  i -= 21;

  if (i == 0)
  {
  strncpy(name, "THERMX", 10);
  return true;
  }
  --i;

  return IMaterialRock::WriteFilosK0ParamName(ffmat, diarunner, i, name); // Only K0, not base class
}

void CMaterialRigidity::WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, double *values, int stride) const
{
  *values = ffmat.ParameterValue(IDT_VALUETYPE_COHESION) * 1e6; // COHESI
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_FRICTION_ANGLE) * PI / 180; // PHI
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_RHOB); // DENSIT
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM1) * 1e6; // RIGIDI(1)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM2) * 1e6; // RIGIDI(2)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM3) * 1e6; // RIGIDI(3)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM4) * 1e6; // RIGIDI(4)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM5) * 1e6; // RIGIDI(5)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM6) * 1e6; // RIGIDI(6)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM7) * 1e6; // RIGIDI(7)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM8) * 1e6; // RIGIDI(8)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM9) * 1e6; // RIGIDI(9)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM10) * 1e6; // RIGIDI(10)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM11) * 1e6; // RIGIDI(11)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM12) * 1e6; // RIGIDI(12)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM13) * 1e6; // RIGIDI(13)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM14) * 1e6; // RIGIDI(14)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM15) * 1e6; // RIGIDI(15)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM16) * 1e6; // RIGIDI(16)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM17) * 1e6; // RIGIDI(17)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM18) * 1e6; // RIGIDI(18)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM19) * 1e6; // RIGIDI(19)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM20) * 1e6; // RIGIDI(20)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_RIGI_PARAM21) * 1e6; // RIGIDI(21)
  values += stride;

  // go from volumetric to linear thermal expansion coefficient
  *values = ffmat.ParameterValue(IDT_VALUETYPE_THERMAL_EXPANSION) / 3.0; // THERMX
  values += stride;

  IMaterialRock::WriteFilosK0ParamValues(ffmat, diarunner, values, stride); // Only K0, not base class
}
