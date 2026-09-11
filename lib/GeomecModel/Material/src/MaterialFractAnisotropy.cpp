#include "MaterialFractAnisotropy.h"
#include "ModelBase.h"
#include "ValueTypes.h"

#include "lbfl.h"

#include "GeomecDianaRunner.h"
#include "IObject.h"
#include "Material.h"
#include "Vector.h"

CMaterialFractAnisotropy::CMaterialFractAnisotropy(CMaterialEntry &entry, CLibraryMaterial &libmat)
    : CMaterialFractureApertureBase(entry, libmat) {}

bool CMaterialFractAnisotropy::Write(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner) const {
  ftn_double_t ddum;

  ddum = (ftn_double_t)(ffmat.ParameterValue(IDT_VALUETYPE_COHESION) * 1e6);
  PutItemLength("COHESI", &ddum, 1);

  ddum = (ftn_double_t)(ffmat.ParameterValue(IDT_VALUETYPE_FRICTION_ANGLE) * PI / 180);
  PutItemLength("PHI", &ddum, 1);

  double En1 = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGMODULUS_NORM);
  double Et1 = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGMODULUS_TRANS);
  double Nun1 = ffmat.ParameterValue(IDT_VALUETYPE_POISSONRATIO_NORM);
  double Nut1 = ffmat.ParameterValue(IDT_VALUETYPE_POISSONRATIO_TRANS);
  double G1 = ffmat.ParameterValue(IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS);

  geo::CMatrix stiffnessMatrix1 = CalculateStiffnessMatrix(ffmat, En1, Et1, Nun1, Nut1, G1);
  ftn_double_t rigidity[21];

  rigidity[0] = stiffnessMatrix1.Value(1, 1) * 1e6;
  rigidity[1] = stiffnessMatrix1.Value(0, 1) * 1e6;
  ;
  rigidity[2] = stiffnessMatrix1.Value(1, 2) * 1e6;
  rigidity[3] = stiffnessMatrix1.Value(1, 3) * 1e6;
  rigidity[4] = stiffnessMatrix1.Value(1, 5) * 1e6;
  rigidity[5] = stiffnessMatrix1.Value(1, 4) * 1e6;
  rigidity[6] = stiffnessMatrix1.Value(0, 0) * 1e6;
  rigidity[7] = stiffnessMatrix1.Value(0, 2) * 1e6;
  rigidity[8] = stiffnessMatrix1.Value(0, 3) * 1e6;
  rigidity[9] = stiffnessMatrix1.Value(0, 5) * 1e6;
  rigidity[10] = stiffnessMatrix1.Value(0, 4) * 1e6;
  rigidity[11] = stiffnessMatrix1.Value(2, 2) * 1e6;
  rigidity[12] = stiffnessMatrix1.Value(2, 3) * 1e6;
  rigidity[13] = stiffnessMatrix1.Value(2, 5) * 1e6;
  rigidity[14] = stiffnessMatrix1.Value(2, 4) * 1e6;
  rigidity[15] = stiffnessMatrix1.Value(3, 3) * 1e6;
  rigidity[16] = stiffnessMatrix1.Value(3, 5) * 1e6;
  rigidity[17] = stiffnessMatrix1.Value(3, 4) * 1e6;
  rigidity[18] = stiffnessMatrix1.Value(5, 5) * 1e6;
  rigidity[19] = stiffnessMatrix1.Value(4, 5) * 1e6;
  rigidity[20] = stiffnessMatrix1.Value(4, 4) * 1e6;

  PutItemLength("RIGIDI", rigidity, 21);

  assert(dynamic_cast<CGeomecDianaRunnerBase *>(&diarunner));
  CGeomecDianaRunnerBase &runner = static_cast<CGeomecDianaRunnerBase &>(diarunner);
  if (runner.Model().UseDecompactionParameters()) {
    double En2 = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGMODULUS_NORM_DECOMP);
    double Et2 = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP);
    double Nun2 = ffmat.ParameterValue(IDT_VALUETYPE_POISSONRATIO_NORM_DECOMP);
    double Nut2 = ffmat.ParameterValue(IDT_VALUETYPE_POISSONRATIO_TRANS_DECOMP);
    double G2 = ffmat.ParameterValue(IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS_DECOMP);

    geo::CMatrix stiffnessMatrix2 = CalculateStiffnessMatrix(ffmat, En2, Et2, Nun2, Nut2, G2);
    ftn_double_t deco21[42];

    memcpy(deco21, rigidity, 21 * sizeof(ftn_double_t));

    deco21[21] = stiffnessMatrix2.Value(1, 1) * 1e6;
    deco21[22] = stiffnessMatrix2.Value(0, 1) * 1e6;
    ;
    deco21[23] = stiffnessMatrix2.Value(1, 2) * 1e6;
    deco21[24] = stiffnessMatrix2.Value(1, 3) * 1e6;
    deco21[25] = stiffnessMatrix2.Value(1, 5) * 1e6;
    deco21[26] = stiffnessMatrix2.Value(1, 4) * 1e6;
    deco21[27] = stiffnessMatrix2.Value(0, 0) * 1e6;
    deco21[28] = stiffnessMatrix2.Value(0, 2) * 1e6;
    deco21[29] = stiffnessMatrix2.Value(0, 3) * 1e6;
    deco21[30] = stiffnessMatrix2.Value(0, 5) * 1e6;
    deco21[31] = stiffnessMatrix2.Value(0, 4) * 1e6;
    deco21[32] = stiffnessMatrix2.Value(2, 2) * 1e6;
    deco21[33] = stiffnessMatrix2.Value(2, 3) * 1e6;
    deco21[34] = stiffnessMatrix2.Value(2, 5) * 1e6;
    deco21[35] = stiffnessMatrix2.Value(2, 4) * 1e6;
    deco21[36] = stiffnessMatrix2.Value(3, 3) * 1e6;
    deco21[37] = stiffnessMatrix2.Value(3, 5) * 1e6;
    deco21[38] = stiffnessMatrix2.Value(3, 4) * 1e6;
    deco21[39] = stiffnessMatrix2.Value(5, 5) * 1e6;
    deco21[40] = stiffnessMatrix2.Value(4, 5) * 1e6;
    deco21[41] = stiffnessMatrix2.Value(4, 4) * 1e6;

    PutItemLength("DECO21", deco21, 42);
  }

  // skip CMaterialFractureApertureBase::Write
  return IMaterial::Write(ffmat, diarunner);
}

geo::CMatrix CMaterialFractAnisotropy::CalculateStiffnessMatrix(const CFFMaterial &ffmat, double En, double Et,
                                                                double Nun, double Nut, double G) const {
  geo::CMatrix anisoCompMatrix = CalculateAnisotropyComplianceMatrix(ffmat, En, Et, Nun, Nut, G);
  geo::CMatrix fractCompMatrix = CalculateFractureComplianceMatrix(ffmat);

  geo::CMatrix fullComplianceMatrix = fractCompMatrix + anisoCompMatrix;
  return fullComplianceMatrix.GetInverse();
}

geo::CMatrix CMaterialFractAnisotropy::CalculateAnisotropyComplianceMatrix(const CFFMaterial &ffmat, double En,
                                                                           double Et, double Nun, double Nut,
                                                                           double G) const {
  double layerAzi = ffmat.ParameterValue(IDT_VALUETYPE_LAYER_NORMAL_AZI) * PI / 180.0;
  double layerDip = ffmat.ParameterValue(IDT_VALUETYPE_LAYER_NORMAL_INCL) * PI / 180.0;

  // construct an orthonormal base
  geo::CVector n(sin(layerAzi) * sin(layerDip), cos(layerAzi) * sin(layerDip), -1.0 * cos(layerDip));
  geo::CVector y = n.GetNormal();
  y = y.UnitVector();

  // vectors n and y will be used to calculate the third vector in order to construct an orthonormal base
  geo::CVector x = n.CrossProduct(y);

  // construct base rotation matrix:
  geo::CMatrix R(3, 3);
  R.Value(0, 0, x.X());
  R.Value(0, 1, y.X());
  R.Value(0, 2, n.X());
  R.Value(1, 0, x.Y());
  R.Value(1, 1, y.Y());
  R.Value(1, 2, n.Y());
  R.Value(2, 0, x.Z());
  R.Value(2, 1, y.Z());
  R.Value(2, 2, n.Z());

  // construct rotation matrix:
  geo::CMatrix Q = RotationMatrix(R);

  // construct overall compliance matrix; values only on diagonal
  geo::CMatrix Cttn(6, 6);
  Cttn.Value(0, 0, 1.0 / Et);
  Cttn.Value(0, 1, -1.0 * Nut / Et);
  Cttn.Value(0, 2, -1.0 * Nun / En);
  Cttn.Value(1, 0, -1.0 * Nut / Et);
  Cttn.Value(1, 1, 1.0 / Et);
  Cttn.Value(1, 2, -1.0 * Nun / En);
  Cttn.Value(2, 0, -1.0 * Nun / En);
  Cttn.Value(2, 1, -1.0 * Nun / En);
  Cttn.Value(2, 2, 1.0 / En);
  Cttn.Value(3, 3, 2 * (1 + Nut) / Et);
  Cttn.Value(4, 4, 1.0 / G);
  Cttn.Value(5, 5, 1.0 / G);

  // construct anisotropy compliance matrix
  geo::CMatrix Cani = (Q * Cttn) * Q.GetTransposed();

  return Cani;
}

geo::CMatrix CMaterialFractAnisotropy::CalculateFractureComplianceMatrix(const CFFMaterial &ffmat) const {
  // all angles in radians
  double maxAzi = ffmat.ParameterValue(IDT_VALUETYPE_HIGH_FRACT_DENS_AZI) * PI / 180.0;
  double maxDip = ffmat.ParameterValue(IDT_VALUETYPE_HIGH_FRACT_DENS_INCL) * PI / 180.0;

  double minAzi = ffmat.ParameterValue(IDT_VALUETYPE_LOW_FRACT_DENS_AZI) * PI / 180.0;
  double minDip = ffmat.ParameterValue(IDT_VALUETYPE_LOW_FRACT_DENS_INCL) * PI / 180.0;

  // mantis 2936 Dip and Azi for low and high fracture densities can not be the same
  if (fabs(maxDip - minDip) < EPS &&
      fabs(maxAzi - minAzi) <
          EPS) { // just make sure that there is a small difference somewhere, else the DIANA will abort
    if (minDip >= 89.0 * PI / 180.0)
      minDip -= 0.5;
    else
      minDip += 0.5;
  }
  // construct an orthonormal base
  geo::CVector n(sin(maxAzi) * sin(maxDip), cos(maxAzi) * sin(maxDip), cos(maxDip));
  geo::CVector v(sin(minAzi) * sin(minDip), cos(minAzi) * sin(minDip), -1.0 * cos(minDip));
  double dotResult = n.DotProduct(v);
  geo::CVector y = v - (n * dotResult);
  y = y.UnitVector();

  // vectors n and y will be used to calculate the third vector in order to construct an orthonormal base
  geo::CVector x = y.CrossProduct(n);

  // construct base rotation matrix:
  geo::CMatrix R(3, 3);
  R.Value(0, 0, n.X());
  R.Value(0, 1, y.X());
  R.Value(0, 2, x.X());
  R.Value(1, 0, n.Y());
  R.Value(1, 1, y.Y());
  R.Value(1, 2, x.Y());
  R.Value(2, 0, n.Z());
  R.Value(2, 1, y.Z());
  R.Value(2, 2, x.Z());

  // construct rotation matrix:
  geo::CMatrix Q = RotationMatrix(R);

  double Dn = ffmat.ParameterValue(IDT_VALUETYPE_FRACT_STIFF_NORMAL);
  double Ds = ffmat.ParameterValue(IDT_VALUETYPE_FRACT_STIFF_SHEAR);
  double dx = ffmat.ParameterValue(IDT_VALUETYPE_HIGH_FRACT_DENS);
  double dy = ffmat.ParameterValue(IDT_VALUETYPE_LOW_FRACT_DENS);
  double dz = ffmat.ParameterValue(IDT_VALUETYPE_INTER_FRACT_DENS);

  // construct overall compliance matrix; values only on diagonal
  geo::CMatrix Cxyz(6, 6);
  Cxyz.Value(0, 0, dx / Dn);
  Cxyz.Value(1, 1, dy / Dn);
  Cxyz.Value(2, 2, dz / Dn);
  Cxyz.Value(3, 3, dx / Ds + dy / Ds);
  Cxyz.Value(4, 4, dy / Ds + dz / Ds);
  Cxyz.Value(5, 5, dz / Ds + dx / Ds);

  // construct fracture compliance matrix
  geo::CMatrix Cfr = (Q * Cxyz) * Q.GetTransposed();

  return Cfr;
}

geo::CMatrix CMaterialFractAnisotropy::RotationMatrix(const geo::CMatrix &base) const {
  assert(base.RowSize() == 3 && base.ColumnSize() == 3);

  geo::CMatrix rotation(6, 6);
  // first row
  rotation.Value(0, 0, base.Value(0, 0) * base.Value(0, 0));
  rotation.Value(0, 1, base.Value(0, 1) * base.Value(0, 1));
  rotation.Value(0, 2, base.Value(0, 2) * base.Value(0, 2));
  rotation.Value(0, 3, base.Value(0, 1) * base.Value(0, 0));
  rotation.Value(0, 4, base.Value(0, 1) * base.Value(0, 2));
  rotation.Value(0, 5, base.Value(0, 2) * base.Value(0, 0));
  // second row
  rotation.Value(1, 0, base.Value(1, 0) * base.Value(1, 0));
  rotation.Value(1, 1, base.Value(1, 1) * base.Value(1, 1));
  rotation.Value(1, 2, base.Value(1, 2) * base.Value(1, 2));
  rotation.Value(1, 3, base.Value(1, 0) * base.Value(1, 1));
  rotation.Value(1, 4, base.Value(1, 1) * base.Value(1, 2));
  rotation.Value(1, 5, base.Value(1, 0) * base.Value(1, 2));
  // third row
  rotation.Value(2, 0, base.Value(2, 0) * base.Value(2, 0));
  rotation.Value(2, 1, base.Value(2, 1) * base.Value(2, 1));
  rotation.Value(2, 2, base.Value(2, 2) * base.Value(2, 2));
  rotation.Value(2, 3, base.Value(2, 0) * base.Value(2, 1));
  rotation.Value(2, 4, base.Value(2, 1) * base.Value(2, 2));
  rotation.Value(2, 5, base.Value(2, 0) * base.Value(2, 2));
  // fourth row
  rotation.Value(3, 0, 2 * base.Value(0, 0) * base.Value(1, 0));
  rotation.Value(3, 1, 2 * base.Value(0, 1) * base.Value(1, 1));
  rotation.Value(3, 2, 2 * base.Value(0, 2) * base.Value(1, 2));
  rotation.Value(3, 3, base.Value(0, 0) * base.Value(1, 1) + base.Value(0, 1) * base.Value(1, 0));
  rotation.Value(3, 4, base.Value(0, 2) * base.Value(1, 1) + base.Value(0, 1) * base.Value(1, 2));
  rotation.Value(3, 5, base.Value(0, 2) * base.Value(1, 0) + base.Value(0, 0) * base.Value(1, 2));
  // fifth row
  rotation.Value(4, 0, 2 * base.Value(1, 0) * base.Value(2, 0));
  rotation.Value(4, 1, 2 * base.Value(1, 1) * base.Value(2, 1));
  rotation.Value(4, 2, 2 * base.Value(1, 2) * base.Value(2, 2));
  rotation.Value(4, 3, base.Value(1, 1) * base.Value(2, 0) + base.Value(1, 0) * base.Value(2, 1));
  rotation.Value(4, 4, base.Value(1, 1) * base.Value(2, 2) + base.Value(1, 2) * base.Value(2, 1));
  rotation.Value(4, 5, base.Value(1, 0) * base.Value(2, 2) + base.Value(2, 0) * base.Value(1, 2));
  // sixth row
  rotation.Value(5, 0, 2 * base.Value(2, 0) * base.Value(0, 0));
  rotation.Value(5, 1, 2 * base.Value(2, 1) * base.Value(0, 1));
  rotation.Value(5, 2, 2 * base.Value(2, 2) * base.Value(0, 2));
  rotation.Value(5, 3, base.Value(0, 1) * base.Value(2, 0) + base.Value(0, 0) * base.Value(2, 1));
  rotation.Value(5, 4, base.Value(0, 1) * base.Value(2, 2) + base.Value(0, 2) * base.Value(2, 1));
  rotation.Value(5, 5, base.Value(0, 0) * base.Value(2, 2) + base.Value(2, 0) * base.Value(0, 2));

  return rotation;
}

// Interface for dia::IElementProperty
int CMaterialFractAnisotropy::WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner) const {
  int size = 23; // COHESI/PHI/RIGIDI(21)

  assert(dynamic_cast<CGeomecDianaRunnerBase *>(&diarunner));
  CGeomecDianaRunnerBase &runner = static_cast<CGeomecDianaRunnerBase &>(diarunner);
  if (runner.Model().UseDecompactionParameters()) {
    size += 42; // DECO21(42)
  }

  size += IMaterialRock::WriteFilosParamSize(ffmat, diarunner); // skip CMaterialFractureApertureBase
  return size;
}

bool CMaterialFractAnisotropy::WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner, int i,
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

  if (i < 21) {
    QString rigidi = QString("RIGIDI(%1)").arg(i + 1);
    strncpy(name, rigidi.toStdString().c_str(), 10);
    return true;
  }
  i -= 21;

  assert(dynamic_cast<CGeomecDianaRunnerBase *>(&diarunner));
  CGeomecDianaRunnerBase &runner = static_cast<CGeomecDianaRunnerBase &>(diarunner);
  if (runner.Model().UseDecompactionParameters()) {
    if (i < 42) {
      QString deco21 = QString("DECO21(%1)").arg(i + 1);
      strncpy(name, deco21.toStdString().c_str(), 10);
      return true;
    }
    i -= 42;
  }

  return IMaterialRock::WriteFilosParamName(ffmat, diarunner, i, name); // skip CMaterialFractureApertureBase
}

void CMaterialFractAnisotropy::WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner,
                                                     double *values, int stride) const {
  *values = ffmat.ParameterValue(IDT_VALUETYPE_COHESION) * 1e6; // COHESI
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_FRICTION_ANGLE) * PI / 180; // PHI
  values += stride;

  double En1 = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGMODULUS_NORM);
  double Et1 = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGMODULUS_TRANS);
  double Nun1 = ffmat.ParameterValue(IDT_VALUETYPE_POISSONRATIO_NORM);
  double Nut1 = ffmat.ParameterValue(IDT_VALUETYPE_POISSONRATIO_TRANS);
  double G1 = ffmat.ParameterValue(IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS);

  geo::CMatrix stiffnessMatrix1 = CalculateStiffnessMatrix(ffmat, En1, Et1, Nun1, Nut1, G1);
  ftn_double_t rigidity[21];

  rigidity[0] = stiffnessMatrix1.Value(1, 1) * 1e6;
  rigidity[1] = stiffnessMatrix1.Value(0, 1) * 1e6;
  ;
  rigidity[2] = stiffnessMatrix1.Value(1, 2) * 1e6;
  rigidity[3] = stiffnessMatrix1.Value(1, 3) * 1e6;
  rigidity[4] = stiffnessMatrix1.Value(1, 5) * 1e6;
  rigidity[5] = stiffnessMatrix1.Value(1, 4) * 1e6;
  rigidity[6] = stiffnessMatrix1.Value(0, 0) * 1e6;
  rigidity[7] = stiffnessMatrix1.Value(0, 2) * 1e6;
  rigidity[8] = stiffnessMatrix1.Value(0, 3) * 1e6;
  rigidity[9] = stiffnessMatrix1.Value(0, 5) * 1e6;
  rigidity[10] = stiffnessMatrix1.Value(0, 4) * 1e6;
  rigidity[11] = stiffnessMatrix1.Value(2, 2) * 1e6;
  rigidity[12] = stiffnessMatrix1.Value(2, 3) * 1e6;
  rigidity[13] = stiffnessMatrix1.Value(2, 5) * 1e6;
  rigidity[14] = stiffnessMatrix1.Value(2, 4) * 1e6;
  rigidity[15] = stiffnessMatrix1.Value(3, 3) * 1e6;
  rigidity[16] = stiffnessMatrix1.Value(3, 5) * 1e6;
  rigidity[17] = stiffnessMatrix1.Value(3, 4) * 1e6;
  rigidity[18] = stiffnessMatrix1.Value(5, 5) * 1e6;
  rigidity[19] = stiffnessMatrix1.Value(4, 5) * 1e6;
  rigidity[20] = stiffnessMatrix1.Value(4, 4) * 1e6;

  for (int i = 0; i < 21; ++i) {
    *values = rigidity[i]; // RIGIDI(i+1)
    values += stride;
  }

  assert(dynamic_cast<CGeomecDianaRunnerBase *>(&diarunner));
  CGeomecDianaRunnerBase &runner = static_cast<CGeomecDianaRunnerBase &>(diarunner);
  if (runner.Model().UseDecompactionParameters()) {
    double En2 = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGMODULUS_NORM_DECOMP);
    double Et2 = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP);
    double Nun2 = ffmat.ParameterValue(IDT_VALUETYPE_POISSONRATIO_NORM_DECOMP);
    double Nut2 = ffmat.ParameterValue(IDT_VALUETYPE_POISSONRATIO_TRANS_DECOMP);
    double G2 = ffmat.ParameterValue(IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS_DECOMP);

    geo::CMatrix stiffnessMatrix2 = CalculateStiffnessMatrix(ffmat, En2, Et2, Nun2, Nut2, G2);
    ftn_double_t deco21[42];

    memcpy(deco21, rigidity, 21 * sizeof(ftn_double_t));

    deco21[21] = stiffnessMatrix2.Value(1, 1) * 1e6;
    deco21[22] = stiffnessMatrix2.Value(0, 1) * 1e6;
    ;
    deco21[23] = stiffnessMatrix2.Value(1, 2) * 1e6;
    deco21[24] = stiffnessMatrix2.Value(1, 3) * 1e6;
    deco21[25] = stiffnessMatrix2.Value(1, 5) * 1e6;
    deco21[26] = stiffnessMatrix2.Value(1, 4) * 1e6;
    deco21[27] = stiffnessMatrix2.Value(0, 0) * 1e6;
    deco21[28] = stiffnessMatrix2.Value(0, 2) * 1e6;
    deco21[29] = stiffnessMatrix2.Value(0, 3) * 1e6;
    deco21[30] = stiffnessMatrix2.Value(0, 5) * 1e6;
    deco21[31] = stiffnessMatrix2.Value(0, 4) * 1e6;
    deco21[32] = stiffnessMatrix2.Value(2, 2) * 1e6;
    deco21[33] = stiffnessMatrix2.Value(2, 3) * 1e6;
    deco21[34] = stiffnessMatrix2.Value(2, 5) * 1e6;
    deco21[35] = stiffnessMatrix2.Value(2, 4) * 1e6;
    deco21[36] = stiffnessMatrix2.Value(3, 3) * 1e6;
    deco21[37] = stiffnessMatrix2.Value(3, 5) * 1e6;
    deco21[38] = stiffnessMatrix2.Value(3, 4) * 1e6;
    deco21[39] = stiffnessMatrix2.Value(5, 5) * 1e6;
    deco21[40] = stiffnessMatrix2.Value(4, 5) * 1e6;
    deco21[41] = stiffnessMatrix2.Value(4, 4) * 1e6;

    for (int i = 0; i < 42; ++i) {
      *values = deco21[i]; // DECO21(i+1)
      values += stride;
    }
  }

  IMaterialRock::WriteFilosParamValues(ffmat, diarunner, values, stride); // skip CMaterialFractureApertureBase
}
