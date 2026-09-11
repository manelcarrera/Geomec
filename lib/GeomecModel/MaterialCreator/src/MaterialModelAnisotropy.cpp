
#include "MaterialModelAnisotropy.h"

#include "mlMaterial.h"
#include "Materials.h"

#include "Matrix.h"

#include <cassert>
#include <cmath>
#include <QObject>

#include "GlobalMessage.h"

namespace {

typedef struct
{
  double En;
  double Et;
  double nun;
  double nut;
  double G;
} TManual;

typedef struct
{
  double Rhob;
  double Vp;
  double Vs;
  double epsilon;
  double gamma;
  double delta;
} TThomsen;

typedef void (ml::CMatParam::CSetStrategy::*SetParamFunc)(ml::CMatParam& param, double dValue) const;

}

static bool CalculateManualValues(QString& strErrorMsg, const TThomsen& thomsen, TManual& manual)
{
  // convert from Pa to MPa
  double dRhoxVp2 = thomsen.Rhob * thomsen.Vp * thomsen.Vp / 1000000;
  double dRhoxVs2 = thomsen.Rhob * thomsen.Vs * thomsen.Vs / 1000000;

  double C11 = (1 + 2*thomsen.epsilon)*dRhoxVp2;
  double C33 = dRhoxVp2;
  double C44 = dRhoxVs2;
  double C66 = (1 + 2 * thomsen.gamma) * dRhoxVs2;
  double C12 = C11 - 2*C66;
  double C13 = -C44 + sqrt((C33 - C44) * (C33 - C44) + 2*thomsen.delta*C33*(C33 - C44));

  // stiffness matrix in MPa
  geo::CMatrix mat(3, 3);
  mat.Value(0, 0, C11);
  mat.Value(1, 1, C11);
  mat.Value(2, 2, C33);
  mat.Value(0, 1, C12);
  mat.Value(1, 0, C12);
  mat.Value(0, 2, C13);
  mat.Value(2, 0, C13);
  mat.Value(1, 2, C13);
  mat.Value(2, 1, C13);

  // compliance matrix
  if(mat.GetDeterminant() <= 0)
  {
    strErrorMsg = "This combination of Thomsen parameters is invalid, the stiffness matrix cannot be inverted";
    return false;
  }

  geo::CMatrix matInv = mat.GetInverse();

  QString s1 = "This combination of Thomsen parameters leads to a value for";
  QString s2 = "which is not allowed";
  manual.Et = 1./matInv.Value(0, 0);
  if(manual.Et <= 0)
  {
    strErrorMsg = QObject::tr("%1 Transverse Young's Modulus of %2, %3").arg(s1).arg(manual.Et).arg(s2);
    return false;
  }
  manual.En = 1./matInv.Value(2, 2);
  if(manual.En <= 0)
  {
    strErrorMsg = QObject::tr("%1 Normal Young's Modulus of %2, %3").arg(s1).arg(manual.En).arg(s2);
    return false;
  }
  manual.nut = -manual.Et*matInv.Value(0, 1);
  if(manual.nut < 0 || manual.nut >= 0.5)
  {
    strErrorMsg = QObject::tr("%1 Transverse Poisson's Ratio of %2, %3").arg(s1).arg(manual.nut).arg(s2);
    return false;
  }
  manual.nun = -manual.En*matInv.Value(1, 2);
  if(manual.nun < 0 || manual.nun >= 0.5)
  {
    strErrorMsg = QObject::tr("%1 Normal Poisson's Ratio of %2, %3").arg(s1).arg(manual.nun).arg(s2);
    return false;
  }
  manual.G = C44;
  if(manual.G <= 0)
  {
    strErrorMsg = QObject::tr("%1 Shear Modulus of %2, %3").arg(s1).arg(manual.G).arg(s2);
    return false;
  }

  return true;
}

static void SetThomsenValues(ml::CMaterial& mat, const ml::CMatParam::CSetStrategy& strat, SetParamFunc& SetParam)
{
  // Thomsen -> Manual

  const ml::CMatParam* pDensity = mat.MatParameter(MLD_RHOB);
  assert(pDensity);

  const ml::CMatParam* pVp = mat.MatParameter(MLD_VELOCITYP);
  assert(pVp);

  const ml::CMatParam* pVs = mat.MatParameter(MLD_VELOCITYS);
  assert(pVs);

  const ml::CMatParam* pEpsilon = mat.MatParameter(MLD_THOMSEN_EPSILON);
  assert(pEpsilon);

  const ml::CMatParam* pGamma = mat.MatParameter(MLD_THOMSEN_GAMMA);
  assert(pGamma);

  const ml::CMatParam* pDelta = mat.MatParameter(MLD_THOMSEN_DELTA);
  assert(pDelta);

  TThomsen thomsen;
  thomsen.Rhob = pDensity->Value();
  thomsen.Vp = pVp->Value();
  thomsen.Vs = pVs->Value();
  thomsen.epsilon = pEpsilon->Value();
  thomsen.gamma = pGamma->Value();
  thomsen.delta = pDelta->Value();

  TManual manual;
  QString string;
  if(CalculateManualValues(string, thomsen, manual))
  {
    ml::CMatParam* pYoungNorm = mat.MatParameter(MLD_YOUNGMODULUS_NORM);
    assert(pYoungNorm);
    ml::CMatParam* pYoungTrans = mat.MatParameter(MLD_YOUNGMODULUS_TRANS);
    assert(pYoungTrans);
    ml::CMatParam* pPoissonNorm = mat.MatParameter(MLD_POISSONRATIO_NORM);
    assert(pPoissonNorm);
    ml::CMatParam* pPoissonTrans = mat.MatParameter(MLD_POISSONRATIO_TRANS);
    assert(pPoissonTrans);
    ml::CMatParam* pShearModulus = mat.MatParameter(MLD_ANISOTROPIC_SHEARMODULUS);
    assert(pShearModulus);

    (strat.*SetParam)(*pYoungNorm, manual.En);
    (strat.*SetParam)(*pYoungTrans, manual.Et);
    (strat.*SetParam)(*pPoissonNorm, manual.nun);
    (strat.*SetParam)(*pPoissonTrans, manual.nut);
    (strat.*SetParam)(*pShearModulus, manual.G);
  }
}

static void SetManualValues(ml::CMaterial& mat, const ml::CMatParam::CSetStrategy& strat, SetParamFunc& SetParam)
{
  // Manual -> Thomsen

  const ml::CMatParam* pYoungNorm = mat.MatParameter(MLD_YOUNGMODULUS_NORM);
  assert(pYoungNorm);

	const ml::CMatParam *pYoungTrans = mat.MatParameter(MLD_YOUNGMODULUS_TRANS);
	assert(pYoungTrans);

	const ml::CMatParam *pPoissonNorm = mat.MatParameter(MLD_POISSONRATIO_NORM);
	assert(pPoissonNorm);

	const ml::CMatParam *pPoissonTrans = mat.MatParameter(MLD_POISSONRATIO_TRANS);
	assert(pPoissonTrans);

	const ml::CMatParam *pShearStiff = mat.MatParameter(MLD_ANISOTROPIC_SHEARMODULUS);
	assert(pShearStiff);

  const ml::CMatParam* pRhob = mat.MatParameter(MLD_RHOB);
  assert(pRhob);

  double En = pYoungNorm->Value();
  double Et = pYoungTrans->Value();
  double nun = pPoissonNorm->Value();
  double nut = pPoissonTrans->Value();
  double G = pShearStiff->Value();
  double rho = pRhob->Value();

  // build the compliance matrix
  geo::CMatrix C(6, 6);

  C.Value(0, 0, 1 / Et);
  C.Value(0, 1, -nut / Et);
  C.Value(0, 2, -nun / En);
  C.Value(1, 0, -nut / Et);
  C.Value(1, 1, 1 / Et);
  C.Value(1, 2, -nun / En);
  C.Value(2, 0, -nun / En);
  C.Value(2, 1, -nun / En);
  C.Value(2, 2, 1 / En);
  C.Value(3, 3, 2 * (1 + nut) / Et);
  C.Value(4, 4, 1 / G);
  C.Value(5, 5, 1 / G);

  // stiffness matrix in MPa
  geo::CMatrix matInv = C.GetInverse();

  double C11 = matInv.Value(0, 0);
  double C33 = matInv.Value(2, 2);
  double C44 = matInv.Value(5, 5);
  double C66 = matInv.Value(3, 3);
  double C13 = matInv.Value(0, 2);

  // convert from MPa
  double Vp = (C33 >= 0 ? sqrt(1000000 * C33 / rho) : 1);
  double Vs = (C44 >= 0 ? sqrt(1000000 * C44 / rho) : 1);

  double epsilon = (C11 - C33) / (2 * C33);
  double gamma = (C66 - C44) / (2 * C44);
  double delta = (pow(C13 + C44, 2) - pow(C33 - C44, 2)) / (2 * C33 * (C33 - C44));

  ml::CMatParam* pVp = mat.MatParameter(MLD_VELOCITYP);
  assert(pVp);

  ml::CMatParam* pVs = mat.MatParameter(MLD_VELOCITYS);
  assert(pVs);

  ml::CMatParam* pEpsilon = mat.MatParameter(MLD_THOMSEN_EPSILON);
  assert(pEpsilon);

  ml::CMatParam* pGamma = mat.MatParameter(MLD_THOMSEN_GAMMA);
  assert(pGamma);

  ml::CMatParam* pDelta = mat.MatParameter(MLD_THOMSEN_DELTA);
  assert(pDelta);

  (strat.*SetParam)(*pVp, Vp);
  (strat.*SetParam)(*pVs, Vs);
  (strat.*SetParam)(*pEpsilon, epsilon);
  (strat.*SetParam)(*pGamma, gamma);
  (strat.*SetParam)(*pDelta, delta);
}

void CMaterialAnisotropyYoungNormSetStrategy::operator()(double dValue, ml::CMatParam& param) const
{
  SetParamValue(param, dValue);

  ml::CMaterial& mat = param.ParentMaterial();

  const ml::CMatParam *pYoungNorm = mat.MatParameter(MLD_YOUNGMODULUS_NORM);
	assert(pYoungNorm);

  ml::CMatParam* pYoungNormDecomp = mat.MatParameter(MLD_YOUNGMODULUS_NORM_DECOMP);
  assert(pYoungNormDecomp);

  // decompaction Young's modulus should follow if value is equal to old Young's value
  if(fabs(pYoungNormDecomp->Value() - pYoungNorm->PreviousValue()) < 1e-4)
    SetParamValue(*pYoungNormDecomp, dValue);

  if(mat.MatParameter(MLD_THOMSEN_EPSILON))
  {
    // material has Thomsen parameters, calculate them
    SetParamFunc f = &CMaterialAnisotropyYoungNormSetStrategy::SetParamValue;
    SetManualValues(mat, *this, f);
  }
}

void CMaterialAnisotropyYoungTransSetStrategy::operator()(double dValue, ml::CMatParam& param) const
{
  SetParamValue(param, dValue);

  ml::CMaterial& mat = param.ParentMaterial();

  const ml::CMatParam *pYoungTrans = mat.MatParameter(MLD_YOUNGMODULUS_TRANS);
	assert(pYoungTrans);

  ml::CMatParam* pYoungTransDecomp = mat.MatParameter(MLD_YOUNGMODULUS_TRANS_DECOMP);
  assert(pYoungTransDecomp);

  // decompaction Young's modulus should follow if value is equal to old Young's value
  if(fabs(pYoungTransDecomp->Value() - pYoungTrans->PreviousValue()) < 1e-4)
    SetParamValue(*pYoungTransDecomp, dValue);

  if(mat.MatParameter(MLD_THOMSEN_EPSILON))
  {
    // material has Thomsen parameters, calculate them
    SetParamFunc f = &CMaterialAnisotropyYoungTransSetStrategy::SetParamValue;
    SetManualValues(mat, *this, f);
  }
}

void CMaterialAnisotropyPoissonNormSetStrategy::operator()(double dValue, ml::CMatParam& param) const
{
  SetParamValue(param, dValue);

  ml::CMaterial& mat = param.ParentMaterial();

  const ml::CMatParam *pPoissonNorm = mat.MatParameter(MLD_POISSONRATIO_NORM);
	assert(pPoissonNorm);

  ml::CMatParam* pPoissonNormDecomp = mat.MatParameter(MLD_POISSONRATIO_NORM_DECOMP);
  assert(pPoissonNormDecomp);

  // decompaction Poisson's ratio should follow if value is equal to old Poisson's ratio
  if(fabs(pPoissonNormDecomp->Value() - pPoissonNorm->PreviousValue()) < 1e-8)
    SetParamValue(*pPoissonNormDecomp, dValue);

  if(mat.MatParameter(MLD_THOMSEN_EPSILON))
  {
    // material has Thomsen parameters, calculate them
    SetParamFunc f = &CMaterialAnisotropyPoissonNormSetStrategy::SetParamValue;
    SetManualValues(mat, *this, f);
  }
}

void CMaterialAnisotropyPoissonTransSetStrategy::operator()(double dValue, ml::CMatParam& param) const
{
  SetParamValue(param, dValue);

  ml::CMaterial& mat = param.ParentMaterial();

  const ml::CMatParam *pPoissonTrans = mat.MatParameter(MLD_POISSONRATIO_TRANS);
	assert(pPoissonTrans);

  ml::CMatParam* pPoissonTransDecomp = mat.MatParameter(MLD_POISSONRATIO_TRANS_DECOMP);
  assert(pPoissonTransDecomp);

  // decompaction Poisson's ratio should follow if value is equal to old Poisson's ratio
  if(fabs(pPoissonTransDecomp->Value() - pPoissonTrans->PreviousValue()) < 1e-8)
    SetParamValue(*pPoissonTransDecomp, dValue);

  if(mat.MatParameter(MLD_THOMSEN_EPSILON))
  {
    // material has Thomsen parameters, calculate them
    SetParamFunc f = &CMaterialAnisotropyPoissonTransSetStrategy::SetParamValue;
    SetManualValues(mat, *this, f);
  }
}

void CMaterialAnisotropyShearStiffSetStrategy::operator()(double dValue, ml::CMatParam& param) const
{
  SetParamValue(param, dValue);

  ml::CMaterial& mat = param.ParentMaterial();

  const ml::CMatParam *pShearStiff = mat.MatParameter(MLD_ANISOTROPIC_SHEARMODULUS);
	assert(pShearStiff);

  ml::CMatParam* pShearStiffDecomp = mat.MatParameter(MLD_ANISOTROPIC_SHEARMODULUS_DECOMP);
  assert(pShearStiffDecomp);

  // decompaction shear stiffness should follow if value is equal to old shear stiffness
  if(fabs(pShearStiffDecomp->Value() - pShearStiff->PreviousValue()) < 1e-4)
    SetParamValue(*pShearStiffDecomp, dValue);

  if(mat.MatParameter(MLD_THOMSEN_EPSILON))
  {
    // material has Thomsen parameters, calculate them
    SetParamFunc f = &CMaterialAnisotropyShearStiffSetStrategy::SetParamValue;
    SetManualValues(mat, *this, f);
  }
}

bool CMaterialAnisotropyVpCheckStrategy::operator()(double dValue, const ml::CMatParam& param, QString& strErrorMsg, int /*nUnitDef*/) const
{
  const ml::CMaterial& mat = param.ParentMaterial();

  const ml::CMatParam* pDensity = mat.MatParameter(MLD_RHOB);
  if(!pDensity)
    return false;
  const ml::CMatParam* pVs = mat.MatParameter(MLD_VELOCITYS);
  if(!pVs)
    return false;
  const ml::CMatParam* pEpsilon = mat.MatParameter(MLD_THOMSEN_EPSILON);
  if(!pEpsilon)
    return false;
  const ml::CMatParam* pGamma = mat.MatParameter(MLD_THOMSEN_GAMMA);
  if(!pGamma)
    return false;
  const ml::CMatParam* pDelta = mat.MatParameter(MLD_THOMSEN_DELTA);
  if(!pDelta)
    return false;

  TThomsen thomsen;
  thomsen.Rhob = pDensity->Value();
  thomsen.Vp = dValue;
  thomsen.Vs = pVs->Value();
  thomsen.epsilon = pEpsilon->Value();
  thomsen.gamma = pGamma->Value();
  thomsen.delta = pDelta->Value();

  TManual manual;
  if(!CalculateManualValues(strErrorMsg, thomsen, manual))
    return false;

  return true;
}

void CMaterialAnisotropyVpSetStrategy::operator()(double dValue, ml::CMatParam& param) const
{
  ml::CMaterial& mat = param.ParentMaterial();

  ml::CMatParam* pVp = mat.MatParameter(MLD_VELOCITYP);
  assert(pVp);

  SetParamValue(*pVp, dValue);

  SetParamFunc f = &CMaterialAnisotropyVpSetStrategy::SetParamValue;
  SetThomsenValues(mat, *this, f);
}

bool CMaterialAnisotropyVsCheckStrategy::operator()(double dValue, const ml::CMatParam& param, QString& strErrorMsg, int /*nUnitDef*/) const
{
  const ml::CMaterial& mat = param.ParentMaterial();

  const ml::CMatParam* pDensity = mat.MatParameter(MLD_RHOB);
  if(!pDensity)
    return false;
  const ml::CMatParam* pVp = mat.MatParameter(MLD_VELOCITYP);
  if(!pVp)
    return false;
  const ml::CMatParam* pEpsilon = mat.MatParameter(MLD_THOMSEN_EPSILON);
  if(!pEpsilon)
    return false;
  const ml::CMatParam* pGamma = mat.MatParameter(MLD_THOMSEN_GAMMA);
  if(!pGamma)
    return false;
  const ml::CMatParam* pDelta = mat.MatParameter(MLD_THOMSEN_DELTA);
  if(!pDelta)
    return false;

  TThomsen thomsen;
  thomsen.Rhob = pDensity->Value();
  thomsen.Vp = pVp->Value();
  thomsen.Vs = dValue;
  thomsen.epsilon = pEpsilon->Value();
  thomsen.gamma = pGamma->Value();
  thomsen.delta = pDelta->Value();

  TManual manual;
  if(!CalculateManualValues(strErrorMsg, thomsen, manual))
    return false;

  return true;
}

void CMaterialAnisotropyVsSetStrategy::operator()(double dValue, ml::CMatParam& param) const
{
  ml::CMaterial& mat = param.ParentMaterial();

  ml::CMatParam* pVs = mat.MatParameter(MLD_VELOCITYS);
  assert(pVs);

  SetParamValue(*pVs, dValue);

  SetParamFunc f = &CMaterialAnisotropyVsSetStrategy::SetParamValue;
  SetThomsenValues(mat, *this, f);
}

bool CMaterialAnisotropyThomsenEpsilonCheckStrategy::operator()(double dValue, const ml::CMatParam& param, QString& strErrorMsg, int /*nUnitDef*/) const
{
  const ml::CMaterial& mat = param.ParentMaterial();

  const ml::CMatParam* pDensity = mat.MatParameter(MLD_RHOB);
  if(!pDensity)
    return false;
  const ml::CMatParam* pVp = mat.MatParameter(MLD_VELOCITYP);
  if(!pVp)
    return false;
  const ml::CMatParam* pVs = mat.MatParameter(MLD_VELOCITYS);
  if(!pVs)
    return false;
  const ml::CMatParam* pGamma = mat.MatParameter(MLD_THOMSEN_GAMMA);
  if(!pGamma)
    return false;
  const ml::CMatParam* pDelta = mat.MatParameter(MLD_THOMSEN_DELTA);
  if(!pDelta)
    return false;

  TThomsen thomsen;
  thomsen.Rhob = pDensity->Value();
  thomsen.Vp = pVp->Value();
  thomsen.Vs = pVs->Value();
  thomsen.epsilon = dValue;
  thomsen.gamma = pGamma->Value();
  thomsen.delta = pDelta->Value();

  TManual manual;
  if(!CalculateManualValues(strErrorMsg, thomsen, manual))
    return false;

  return true;
}

void CMaterialAnisotropyThomsenEpsilonSetStrategy::operator()(double dValue, ml::CMatParam& param) const
{
  ml::CMaterial& mat = param.ParentMaterial();

  ml::CMatParam* pEpsilon = mat.MatParameter(MLD_THOMSEN_EPSILON);
  assert(pEpsilon);

  SetParamValue(*pEpsilon, dValue);

  SetParamFunc f = &CMaterialAnisotropyThomsenEpsilonSetStrategy::SetParamValue;
  SetThomsenValues(mat, *this, f);
}

bool CMaterialAnisotropyThomsenGammaCheckStrategy::operator()(double dValue, const ml::CMatParam& param, QString& strErrorMsg, int /*nUnitDef*/) const
{
  const ml::CMaterial& mat = param.ParentMaterial();

  const ml::CMatParam* pDensity = mat.MatParameter(MLD_RHOB);
  if(!pDensity)
    return false;
  const ml::CMatParam* pVp = mat.MatParameter(MLD_VELOCITYP);
  if(!pVp)
    return false;
  const ml::CMatParam* pVs = mat.MatParameter(MLD_VELOCITYS);
  if(!pVs)
    return false;
  const ml::CMatParam* pEpsilon = mat.MatParameter(MLD_THOMSEN_EPSILON);
  if(!pEpsilon)
    return false;
  const ml::CMatParam* pDelta = mat.MatParameter(MLD_THOMSEN_DELTA);
  if(!pDelta)
    return false;

  TThomsen thomsen;
  thomsen.Rhob = pDensity->Value();
  thomsen.Vp = pVp->Value();
  thomsen.Vs = pVs->Value();
  thomsen.epsilon = pEpsilon->Value();
  thomsen.gamma = dValue;
  thomsen.delta = pDelta->Value();

  TManual manual;
  if(!CalculateManualValues(strErrorMsg, thomsen, manual))
    return false;

  return true;
}

void CMaterialAnisotropyThomsenGammaSetStrategy::operator()(double dValue, ml::CMatParam& param) const
{
  ml::CMaterial& mat = param.ParentMaterial();

  ml::CMatParam* pGamma = mat.MatParameter(MLD_THOMSEN_GAMMA);
  assert(pGamma);

  SetParamValue(*pGamma, dValue);

  SetParamFunc f = &CMaterialAnisotropyThomsenGammaSetStrategy::SetParamValue;
  SetThomsenValues(mat, *this, f);
}

bool CMaterialAnisotropyThomsenDeltaCheckStrategy::operator()(double dValue, const ml::CMatParam& param, QString& strErrorMsg, int /*nUnitDef*/) const
{
  const ml::CMaterial& mat = param.ParentMaterial();

  const ml::CMatParam* pDensity = mat.MatParameter(MLD_RHOB);
  if(!pDensity)
    return false;
  const ml::CMatParam* pVp = mat.MatParameter(MLD_VELOCITYP);
  if(!pVp)
    return false;
  const ml::CMatParam* pVs = mat.MatParameter(MLD_VELOCITYS);
  if(!pVs)
    return false;
  const ml::CMatParam* pEpsilon = mat.MatParameter(MLD_THOMSEN_EPSILON);
  if(!pEpsilon)
    return false;
  const ml::CMatParam* pGamma = mat.MatParameter(MLD_THOMSEN_GAMMA);
  if(!pGamma)
    return false;

  TThomsen thomsen;
  thomsen.Rhob = pDensity->Value();
  thomsen.Vp = pVp->Value();
  thomsen.Vs = pVs->Value();
  thomsen.epsilon = pEpsilon->Value();
  thomsen.gamma = pGamma->Value();
  thomsen.delta = dValue;

  TManual manual;
  if(!CalculateManualValues(strErrorMsg, thomsen, manual))
    return false;

  return true;
}

void CMaterialAnisotropyThomsenDeltaSetStrategy::operator()(double dValue, ml::CMatParam& param) const
{
  ml::CMaterial& mat = param.ParentMaterial();

  ml::CMatParam* pDelta = mat.MatParameter(MLD_THOMSEN_DELTA);
  assert(pDelta);

  SetParamValue(*pDelta, dValue);

  SetParamFunc f = &CMaterialAnisotropyThomsenDeltaSetStrategy::SetParamValue;
  SetThomsenValues(mat, *this, f);
}

bool CMaterialAnisotropyCheckStrategy::operator () (
  const ml::CMaterial& material) const
{
  const ml::CMatParam* highFractureDensityInclination =
    material.MatParameter(MLD_HIGH_FRACT_DENS_INCL);
  const ml::CMatParam* highFractureDensityAzimuth =
    material.MatParameter(MLD_HIGH_FRACT_DENS_AZI);
  const ml::CMatParam* lowFractureDensityInclination =
    material.MatParameter(MLD_LOW_FRACT_DENS_INCL);
  const ml::CMatParam* lowFractureDensityAzimuth =
    material.MatParameter(MLD_LOW_FRACT_DENS_AZI);

  if ((highFractureDensityInclination != 0) &&
    (highFractureDensityAzimuth != 0) && (lowFractureDensityInclination != 0) &&
    (lowFractureDensityAzimuth != 0) && (highFractureDensityInclination->Value() ==
    lowFractureDensityInclination->Value()) &&
    (highFractureDensityAzimuth->Value() == lowFractureDensityAzimuth->Value()))
  {
    _m()->error(QObject::tr("The highest and lowest fracture density "
      "directions should not be equal"));

    return false;
  }

  return true;
}
