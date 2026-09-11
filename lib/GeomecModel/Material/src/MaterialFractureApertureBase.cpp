
#include "MaterialFractureApertureBase.h"

#include "ModelBase.h"
#include "ValueTypes.h"

#include "lbfl.h"

#include "GeomecDianaRunner.h"
#include "Material.h"

CMaterialFractureApertureBase::CMaterialFractureApertureBase(CMaterialEntry &entry, CLibraryMaterial &libmat)
    : IMaterialRock(entry, libmat) {}

bool CMaterialFractureApertureBase::Write(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner) const {
  double dStiffNormal;
  double dStiffShear;

  if (ffmat.IsParameter(IDT_VALUETYPE_FRACT_STIFF_NORMAL)) {
    dStiffNormal = ffmat.ParameterValue(IDT_VALUETYPE_FRACT_STIFF_NORMAL);
  } else {
    assert(ffmat.IsParameter(IDT_VALUETYPE_REFERENCE_APERTURE));
    double a = ffmat.ParameterValue(IDT_VALUETYPE_APERTUREPARAM);
    double wref = ffmat.ParameterValue(IDT_VALUETYPE_REFERENCE_APERTURE);

    if (a == 0.0) {
      a = 1e-7;
    }

    if (wref < 1e-6) {
      wref = 1e-6;
    }

    dStiffNormal = 1 / (a * wref);
  }

  if (ffmat.IsParameter(IDT_VALUETYPE_FRACT_STIFF_SHEAR)) {
    dStiffShear = ffmat.ParameterValue(IDT_VALUETYPE_FRACT_STIFF_SHEAR);
  } else {
    dStiffShear = 0.5 * dStiffNormal;
  }

  ftn_double_t Elaval[12];
  Elaval[0] = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGMODULUS_NORM) * 1e6;
  Elaval[1] = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGMODULUS_TRANS) * 1e6;
  Elaval[2] = ffmat.ParameterValue(IDT_VALUETYPE_POISSONRATIO_NORM);
  Elaval[3] = ffmat.ParameterValue(IDT_VALUETYPE_POISSONRATIO_TRANS);
  Elaval[4] = ffmat.ParameterValue(IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS) * 1e6;
  Elaval[5] = dStiffNormal * 1e6;
  Elaval[6] = dStiffShear * 1e6;
  Elaval[7] = ffmat.ParameterValue(IDT_VALUETYPE_REFERENCESTRESS) * 1e6; // compressive is positive
  Elaval[8] = ffmat.ParameterValue(IDT_VALUETYPE_APERTUREPARAM) / 1e6;
  Elaval[9] = ffmat.ParameterValue(IDT_VALUETYPE_HIGH_FRACT_DENS);
  Elaval[10] = ffmat.ParameterValue(IDT_VALUETYPE_LOW_FRACT_DENS);
  Elaval[11] = ffmat.ParameterValue(IDT_VALUETYPE_INTER_FRACT_DENS);

  geo::CVector vecHigh = HighDensityDirection(ffmat);
  geo::CVector vecLow = LowDensityDirection(ffmat);

  // make sure vectors are orthogonal
  geo::CVector vecPerpendicular = vecHigh.CrossProduct(vecLow);
  vecLow = vecPerpendicular.CrossProduct(vecHigh);

  ftn_double_t Hi[3];
  Hi[0] = vecHigh.X();
  Hi[1] = vecHigh.Y();
  Hi[2] = vecHigh.Z();

  ftn_double_t Lo[3];
  Lo[0] = vecLow.X();
  Lo[1] = vecLow.Y();
  Lo[2] = vecLow.Z();

  PutCharItem("ELAST", "FRACAP");
  PutItemLength("ELAVAL", Elaval, 12);
  PutItemLength("FRAXHI", Hi, 3);
  PutItemLength("FRAXLO", Lo, 3);

  assert(dynamic_cast<CGeomecDianaRunnerBase *>(&diarunner));
  CGeomecDianaRunnerBase &runner = static_cast<CGeomecDianaRunnerBase &>(diarunner);
  if (runner.Model().UseDecompactionParameters()) {
    ftn_double_t deco5[10];
    deco5[0] = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGMODULUS_TRANS) * 1e6;
    deco5[1] = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGMODULUS_NORM) * 1e6;
    deco5[2] = ffmat.ParameterValue(IDT_VALUETYPE_POISSONRATIO_TRANS);
    deco5[3] = ffmat.ParameterValue(IDT_VALUETYPE_POISSONRATIO_NORM);
    deco5[4] = ffmat.ParameterValue(IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS) * 1e6;
    deco5[5] = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP) * 1e6;
    deco5[6] = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGMODULUS_NORM_DECOMP) * 1e6;
    deco5[7] = ffmat.ParameterValue(IDT_VALUETYPE_POISSONRATIO_TRANS_DECOMP);
    deco5[8] = ffmat.ParameterValue(IDT_VALUETYPE_POISSONRATIO_NORM_DECOMP);
    deco5[9] = ffmat.ParameterValue(IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS_DECOMP) * 1e6;
    PutItemLength("DECO5", deco5, 10);
  }

  return IMaterialRock::Write(ffmat, diarunner);
}

geo::CVector CMaterialFractureApertureBase::HighDensityDirection(const CFFMaterial &ffmat) const {
  return VectorFromAngles(ffmat.ParameterValue(IDT_VALUETYPE_HIGH_FRACT_DENS_INCL),
                          ffmat.ParameterValue(IDT_VALUETYPE_HIGH_FRACT_DENS_AZI));
}

geo::CVector CMaterialFractureApertureBase::IntermediateDensityDirection(const CFFMaterial &ffmat) const {
  return LowDensityDirection(ffmat).CrossProduct(HighDensityDirection(ffmat));
}

geo::CVector CMaterialFractureApertureBase::LowDensityDirection(const CFFMaterial &ffmat) const {
  return VectorFromAngles(ffmat.ParameterValue(IDT_VALUETYPE_LOW_FRACT_DENS_INCL),
                          ffmat.ParameterValue(IDT_VALUETYPE_LOW_FRACT_DENS_AZI));
}

geo::CVector CMaterialFractureApertureBase::VectorFromAngles(double dInclination, double dAzimuth) const {
  // angles in degrees, convert to radians
  double azi = dAzimuth * PI / 180;
  double inc = dInclination * PI / 180;

  geo::CVector vec(sin(inc) * cos(azi), sin(inc) * sin(azi), cos(inc));

  vec = vec.UnitVector();

  return vec;
}

// Interface for dia::IElementProperty
int CMaterialFractureApertureBase::WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner) const {
  int size = 18; // ELAVAL(12)/FRAXHI(3)/FRAXLO(3)

  assert(dynamic_cast<CGeomecDianaRunnerBase *>(&diarunner));
  CGeomecDianaRunnerBase &runner = static_cast<CGeomecDianaRunnerBase &>(diarunner);
  if (runner.Model().UseDecompactionParameters()) {
    size += 10; // DECO5(10)
  }

  size += IMaterialRock::WriteFilosParamSize(ffmat, diarunner);
  return size;
}

bool CMaterialFractureApertureBase::WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner, int i,
                                                        char *name) const {
  if (i < 12) {
    QString elaval = QString("ELAVAL(%1)").arg(i + 1);
    strncpy(name, elaval.toStdString().c_str(), 10);
    return true;
  }
  i -= 12;

  if (i < 3) {
    QString fraxhi = QString("FRAXHI(%1)").arg(i + 1);
    strncpy(name, fraxhi.toStdString().c_str(), 10);
    return true;
  }
  i -= 3;

  if (i < 3) {
    QString fraxlo = QString("FRAXLO(%1)").arg(i + 1);
    strncpy(name, fraxlo.toStdString().c_str(), 10);
    return true;
  }
  i -= 3;

  assert(dynamic_cast<CGeomecDianaRunnerBase *>(&diarunner));
  CGeomecDianaRunnerBase &runner = static_cast<CGeomecDianaRunnerBase &>(diarunner);
  if (runner.Model().UseDecompactionParameters()) {
    if (i < 10) {
      QString deco5 = QString("DECO5(%1)").arg(i + 1);
      strncpy(name, deco5.toStdString().c_str(), 10);
      return true;
    }
    i -= 10;
  }

  return IMaterialRock::WriteFilosParamName(ffmat, diarunner, i, name);
}

void CMaterialFractureApertureBase::WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner,
                                                          double *values, int stride) const {
  double dStiffNormal;
  double dStiffShear;

  if (ffmat.IsParameter(IDT_VALUETYPE_FRACT_STIFF_NORMAL)) {
    dStiffNormal = ffmat.ParameterValue(IDT_VALUETYPE_FRACT_STIFF_NORMAL);
  } else {
    assert(ffmat.IsParameter(IDT_VALUETYPE_REFERENCE_APERTURE));
    double a = ffmat.ParameterValue(IDT_VALUETYPE_APERTUREPARAM);
    double wref = ffmat.ParameterValue(IDT_VALUETYPE_REFERENCE_APERTURE);

    if (a == 0.0) {
      a = 1e-7;
    }

    if (wref < 1e-6) {
      wref = 1e-6;
    }

    dStiffNormal = 1 / (a * wref);
  }

  if (ffmat.IsParameter(IDT_VALUETYPE_FRACT_STIFF_SHEAR)) {
    dStiffShear = ffmat.ParameterValue(IDT_VALUETYPE_FRACT_STIFF_SHEAR);
  } else {
    dStiffShear = 0.5 * dStiffNormal;
  }

  *values = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGMODULUS_NORM) * 1e6; // ELAVAL(1)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGMODULUS_TRANS) * 1e6; // ELAVAL(2)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_POISSONRATIO_NORM); // ELAVAL(3)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_POISSONRATIO_TRANS); // ELAVAL(4)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS) * 1e6; // ELAVAL(5)
  values += stride;

  *values = dStiffNormal * 1e6; // ELAVAL(6)
  values += stride;

  *values = dStiffShear * 1e6; // ELAVAL(7)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_REFERENCESTRESS) * 1e6; // compressive is positive  // ELAVAL(8)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_APERTUREPARAM) / 1e6; // ELAVAL(9)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_HIGH_FRACT_DENS); // ELAVAL(10)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_LOW_FRACT_DENS); // ELAVAL(11)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_INTER_FRACT_DENS); // ELAVAL(12)
  values += stride;

  geo::CVector vecHigh = HighDensityDirection(ffmat);
  geo::CVector vecLow = LowDensityDirection(ffmat);

  // make sure vectors are orthogonal
  geo::CVector vecPerpendicular = vecHigh.CrossProduct(vecLow);
  vecLow = vecPerpendicular.CrossProduct(vecHigh);

  *values = vecHigh.X(); // FRAXHI(1)
  values += stride;

  *values = vecHigh.Y(); // FRAXHI(2)
  values += stride;

  *values = vecHigh.Z(); // FRAXHI(3)
  values += stride;

  *values = vecLow.X(); // FRAXLO(1)
  values += stride;

  *values = vecLow.Y(); // FRAXLO(2)
  values += stride;

  *values = vecLow.Z(); // FRAXLO(3)
  values += stride;

  assert(dynamic_cast<CGeomecDianaRunnerBase *>(&diarunner));
  CGeomecDianaRunnerBase &runner = static_cast<CGeomecDianaRunnerBase &>(diarunner);
  if (runner.Model().UseDecompactionParameters()) {
    *values = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGMODULUS_TRANS) * 1e6; // DECO5(1)
    values += stride;

    *values = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGMODULUS_NORM) * 1e6; // DECO5(2)
    values += stride;

    *values = ffmat.ParameterValue(IDT_VALUETYPE_POISSONRATIO_TRANS); // DECO5(3)
    values += stride;

    *values = ffmat.ParameterValue(IDT_VALUETYPE_POISSONRATIO_NORM); // DECO5(4)
    values += stride;

    *values = ffmat.ParameterValue(IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS) * 1e6; // DECO5(5)
    values += stride;

    *values = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP) * 1e6; // DECO5(6)
    values += stride;

    *values = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGMODULUS_NORM_DECOMP) * 1e6; // DECO5(7)
    values += stride;

    *values = ffmat.ParameterValue(IDT_VALUETYPE_POISSONRATIO_TRANS_DECOMP); // DECO5(8)
    values += stride;

    *values = ffmat.ParameterValue(IDT_VALUETYPE_POISSONRATIO_NORM_DECOMP); // DECO5(9)
    values += stride;

    *values = ffmat.ParameterValue(IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS_DECOMP) * 1e6; // DECO5(10)
    values += stride;
  }

  IMaterialRock::WriteFilosParamValues(ffmat, diarunner, values, stride);
}
