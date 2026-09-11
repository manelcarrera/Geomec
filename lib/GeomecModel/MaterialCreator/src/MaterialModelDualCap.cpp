#include "MaterialModelDualCap.h"
#include "MaterialHelperFactory.h"
#include "MaterialUnitTypes.h"
#include "Materials.h"
#include "ValueTypes.h"
#include "mlMatParamDomain.h"

#include <cassert>

#include <qobject.h>

namespace {

const QString GROUP_MANUAL = "Manual";
const QString GROUP_BULK = "Bulk stiffness";
const QString GROUP_DYNAMIC = "Dynamic stiffness";
const QString GROUP_VELOCITIES = "Velocities";

} // namespace

IPQPlotter *CMaterialDualCapCreator::CPQPlotter::Clone() const {
  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();
  assert(f);

  return f->getMatPQPlotter(CMaterialHelperFactory::MAT_PQ_DualCap);
}

void CMaterialDualCapCreator::CPQPlotter::GetSecondaryYieldCurve(const CLibraryMaterial &mat, double dPMin,
                                                                 double dPMax, CStressStrainArray &values) const {
  const ml::CMatParam *pPrecon = mat.Parameter(MLD_SEC_PRECON);
  GetYieldCurve(mat, pPrecon->Value(), dPMin, dPMax, values);
}

bool CMaterialDualCapPreconsolidationCheckStrategy::operator()(double dValue, const ml::CMatParam &param,
                                                               QString &strErrorMsg, int nUnitDef) const {
  // must be less than secondary preconsolidation stress
  const ml::CMaterial &mat = param.ParentMaterial();

  const ml::CMatParam *pSecPrecon = mat.MatParameter(MLD_SEC_PRECON);
  assert(pSecPrecon);

  double dSecPrecon = pSecPrecon->ValueToUserUnit(nUnitDef);
  if (dValue >= dSecPrecon) {
    strErrorMsg = QObject::tr("Preconsolidation stress must be less than Secondary Preconsolidation Stress");
    return false;
  }

  return true;
}

bool CMaterialDualCapSecondaryPreconCheckStrategy::operator()(double dValue, const ml::CMatParam &param,
                                                              QString &strErrorMsg, int nUnitDef) const {
  // must be greater than preconsolidation stress
  const ml::CMaterial &mat = param.ParentMaterial();

  const ml::CMatParam *pPrecon = mat.MatParameter(MLD_PRECONSOLIDATION);
  assert(pPrecon);

  double dPrecon = pPrecon->ValueToUserUnit(nUnitDef);
  if (dValue <= dPrecon) {
    strErrorMsg = QObject::tr("Secondary Preconsolidation Stress must be greater than Preconsolidation Stress");
    return false;
  }

  return true;
}

static bool ElasticValue(const double &dPoison, const double &dPrecon, const double &dPorosity, const double &dOther,
                         double &dValue) {
  dValue = 3 * (1 - 2 * dPoison) * dPrecon / ((1 - dPorosity) * dOther);
  return true;
}

void CMaterialDualCapYoungSetStrategy::operator()(double dValue, ml::CMatParam &param) const {
  ml::CMaterial &mat = param.ParentMaterial();

  const ml::CMatParam *pPoisson = mat.MatParameter(MLD_POISSONRATIO);
  assert(pPoisson);
  double nu = pPoisson->Value();

  const ml::CMatParam *pPrecon = mat.MatParameter(MLD_PRECONSOLIDATION);
  assert(pPrecon);
  double Pc0 = pPrecon->Value();

  const ml::CMatParam *pPoro = mat.MatParameter(MLD_POROSITY);
  assert(pPoro);
  double n0 = pPoro->Value();

  double dElasticHard;
  if (ElasticValue(nu, Pc0, n0, dValue, dElasticHard)) {
    ml::CMatParam *pElasticHard = mat.MatParameter(MLD_ELASTIC_HARDENING);
    assert(pElasticHard);
    if (fabs(pElasticHard->Value() - dElasticHard) > 1e-8)
      pElasticHard->Value(dElasticHard);
  }

  return CMaterialCamClayYoungSetStrategy::operator()(dValue, param);
}

void CMaterialDualCapPoissonSetStrategy::operator()(double dValue, ml::CMatParam &param) const {
  ml::CMaterial &mat = param.ParentMaterial();

  const ml::CMatParam *pPrecon = mat.MatParameter(MLD_PRECONSOLIDATION);
  assert(pPrecon);
  double Pc0 = pPrecon->Value();

  const ml::CMatParam *pPoro = mat.MatParameter(MLD_POROSITY);
  assert(pPoro);
  double n0 = pPoro->Value();

  const ml::CMatParam *pElasticHard = mat.MatParameter(MLD_ELASTIC_HARDENING);
  assert(pElasticHard);
  double dElasticHard = pElasticHard->Value();

  double dYoung;
  if (ElasticValue(dValue, Pc0, n0, dElasticHard, dYoung)) {
    ml::CMatParam *pYoung = mat.MatParameter(MLD_YOUNGMODULUS);
    assert(pYoung);
    if (fabs(pYoung->Value() - dYoung) > 1e-8)
      pYoung->Value(dYoung); // route through set strategy
  }

  CMaterialCamClayPoissonSetStrategy::operator()(dValue, param);
}

void CMaterialDualCapPorositySetStrategy::operator()(double dValue, ml::CMatParam &param) const {
  ml::CMaterial &mat = param.ParentMaterial();

  const ml::CMatParam *pPrecon = mat.MatParameter(MLD_PRECONSOLIDATION);
  assert(pPrecon);
  double Pc0 = pPrecon->Value();

  const ml::CMatParam *pPoisson = mat.MatParameter(MLD_POISSONRATIO);
  assert(pPoisson);
  double nu = pPoisson->Value();

  const ml::CMatParam *pElasticHard = mat.MatParameter(MLD_ELASTIC_HARDENING);
  assert(pElasticHard);
  double dElasticHard = pElasticHard->Value();

  double dYoung;
  if (ElasticValue(nu, Pc0, dValue, dElasticHard, dYoung)) {
    ml::CMatParam *pYoung = mat.MatParameter(MLD_YOUNGMODULUS);
    assert(pYoung);
    if (fabs(pYoung->Value() - dYoung) > 1e-8)
      pYoung->Value(dYoung); // route through set strategy
  }

  CMaterialCamClayPorositySetStrategy::operator()(dValue, param);
}

void CMaterialDualCapPreconsolidationSetStrategy::operator()(double dValue, ml::CMatParam &param) const {
  ml::CMaterial &mat = param.ParentMaterial();

  const ml::CMatParam *pPoisson = mat.MatParameter(MLD_POISSONRATIO);
  assert(pPoisson);
  double nu = pPoisson->Value();

  const ml::CMatParam *pPoro = mat.MatParameter(MLD_POROSITY);
  assert(pPoro);
  double n0 = pPoro->Value();

  const ml::CMatParam *pElasticHard = mat.MatParameter(MLD_ELASTIC_HARDENING);
  assert(pElasticHard);
  double dElasticHard = pElasticHard->Value();

  double dYoung;
  if (ElasticValue(nu, dValue, n0, dElasticHard, dYoung)) {
    ml::CMatParam *pYoung = mat.MatParameter(MLD_YOUNGMODULUS);
    assert(pYoung);
    if (fabs(pYoung->Value() - dYoung) > 1e-8)
      pYoung->Value(dYoung); // route through set strategy
  }

  CMaterialCamClayPreconsolidationSetStrategy::operator()(dValue, param);
}

void CMaterialDualCapElasticHardeningSetStrategy::operator()(double dValue, ml::CMatParam &param) const {
  ml::CMaterial &mat = param.ParentMaterial();

  const ml::CMatParam *pPoisson = mat.MatParameter(MLD_POISSONRATIO);
  assert(pPoisson);
  double nu = pPoisson->Value();

  const ml::CMatParam *pPrecon = mat.MatParameter(MLD_PRECONSOLIDATION);
  assert(pPrecon);
  double Pc0 = pPrecon->Value();

  const ml::CMatParam *pPoro = mat.MatParameter(MLD_POROSITY);
  assert(pPoro);
  double n0 = pPoro->Value();

  double dYoung;
  if (ElasticValue(nu, Pc0, n0, dValue, dYoung)) {
    ml::CMatParam *pYoung = mat.MatParameter(MLD_YOUNGMODULUS);
    assert(pYoung);
    if (fabs(pYoung->Value() - dYoung) > 1e-8)
      pYoung->Value(dYoung); // route through set strategy
  }

  SetParamValue(param, dValue);
}
