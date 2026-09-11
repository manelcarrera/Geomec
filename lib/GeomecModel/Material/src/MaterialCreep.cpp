#include "MaterialCreep.h"
#include "ValueTypes.h"

#include "lbfl.h"

#include "GlobalConstants.h"
#include "IObject.h"
#include "Material.h"

CMaterialCreep::CMaterialCreep(CMaterialEntry &entry, CLibraryMaterial &libmat) : IMaterialRock(entry, libmat) {}

bool CMaterialCreep::Write(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner) const {
  ftn_double_t ddum;

  double dCohesi = ffmat.ParameterValue(IDT_VALUETYPE_COHESION) * 1e6;
  double dFricti = ffmat.ParameterValue(IDT_VALUETYPE_FRICTION_ANGLE) * PI / 180;
  double dA1 = ffmat.ParameterValue(IDT_VALUETYPE_CREEP_A1);
  double dA2 = ffmat.ParameterValue(IDT_VALUETYPE_CREEP_A2);
  double dn1 = ffmat.ParameterValue(IDT_VALUETYPE_CREEP_N1);
  double dn2 = ffmat.ParameterValue(IDT_VALUETYPE_CREEP_N2);
  double dQR1 = ffmat.ParameterValue(IDT_VALUETYPE_CREEP_QR1);
  double dQR2 = ffmat.ParameterValue(IDT_VALUETYPE_CREEP_QR2);
  double dAlpha = ffmat.ParameterValue(IDT_VALUETYPE_CREEP_ALPHA) * 1e6;

  // switch to 1/month unit
  dA1 *= SECONDS_PER_MONTH;
  dA2 *= SECONDS_PER_MONTH;

  ddum = (ftn_double_t)(dCohesi);
  PutItemLength("COHESI", &ddum, 1);

  ddum = (ftn_double_t)(dFricti);
  PutItemLength("PHI", &ddum, 1);

  //	ddum = (ftn_double_t) (dCohesi / tan(dFricti));
  //	PutItem("PSHIFT", &ddum);

  PutCharItem("CREEP", "COMPLI");

  ftn_double_t seccrp[7];
  seccrp[0] = (ftn_double_t)(dA1);
  seccrp[1] = (ftn_double_t)(dn1);
  seccrp[2] = (ftn_double_t)(dQR1);
  seccrp[3] = (ftn_double_t)(dA2);
  seccrp[4] = (ftn_double_t)(dn2);
  seccrp[5] = (ftn_double_t)(dQR2);
  seccrp[6] = (ftn_double_t)(dAlpha);

  PutItemLength("SECCRP", seccrp, 7);

  return IMaterial::Write(ffmat, diarunner);
}

// Interface for dia::IElementProperty
int CMaterialCreep::WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner) const {
  int size = 9; // COHESI/PHI/SECCRP(7)

  size += IMaterialRock::WriteFilosParamSize(ffmat, diarunner);
  return size;
}

bool CMaterialCreep::WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner, int i,
                                         char *name) const {
  if (i == 0) {
    strncpy(name, "COHESI", 10);
    return true;
  }
  --i;

  if (i == 0) {
    strncpy(name, "PHI", 10);
    return true;
  }
  --i;

  if (i < 7) {
    QString seccrp = QString("SECCRP(%1)").arg(i + 1);
    strncpy(name, seccrp.toStdString().c_str(), 10);
    return true;
  }
  i -= 7;

  return IMaterialRock::WriteFilosParamName(ffmat, diarunner, i, name);
}

void CMaterialCreep::WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner, double *values,
                                           int stride) const {
  double dCohesi = ffmat.ParameterValue(IDT_VALUETYPE_COHESION) * 1e6;
  double dFricti = ffmat.ParameterValue(IDT_VALUETYPE_FRICTION_ANGLE) * PI / 180;
  double dA1 = ffmat.ParameterValue(IDT_VALUETYPE_CREEP_A1);
  double dA2 = ffmat.ParameterValue(IDT_VALUETYPE_CREEP_A2);
  double dn1 = ffmat.ParameterValue(IDT_VALUETYPE_CREEP_N1);
  double dn2 = ffmat.ParameterValue(IDT_VALUETYPE_CREEP_N2);
  double dQR1 = ffmat.ParameterValue(IDT_VALUETYPE_CREEP_QR1);
  double dQR2 = ffmat.ParameterValue(IDT_VALUETYPE_CREEP_QR2);
  double dAlpha = ffmat.ParameterValue(IDT_VALUETYPE_CREEP_ALPHA) * 1e6;

  // switch to 1/month unit
  dA1 *= SECONDS_PER_MONTH;
  dA2 *= SECONDS_PER_MONTH;

  *values = dCohesi; // COHESI
  values += stride;

  *values = dFricti; // PHI
  values += stride;

  *values = dA1; // SECCRP(1)
  values += stride;

  *values = dn1; // SECCRP(2)
  values += stride;

  *values = dQR1; // SECCRP(3)
  values += stride;

  *values = dA2; // SECCRP(4)
  values += stride;

  *values = dn2; // SECCRP(5)
  values += stride;

  *values = dQR2; // SECCRP(6)
  values += stride;

  *values = dAlpha; // SECCRP(7)
  values += stride;

  IMaterialRock::WriteFilosParamValues(ffmat, diarunner, values, stride);
}
