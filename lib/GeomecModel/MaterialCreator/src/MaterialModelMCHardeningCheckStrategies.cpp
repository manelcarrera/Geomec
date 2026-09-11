#include "MaterialModelMCHardeningCheckStrategies.h"
#include "Materials.h"
#include "mlMaterial.h"

#include <qobject.h>

#include <cassert>

bool CMaterialMCHardEquivalentPlasticStrain1CheckStrategy::operator()(double dValue, const ml::CMatParam& param, QString& strErrorMsg, int nUnitDef) const
{
  const ml::CMaterial& mat = param.ParentMaterial();

  const ml::CMatParam* pEqPlStr2 = mat.MatParameter(MLD_EQUIV_PLAST_STRAIN2);
  if(pEqPlStr2)
  {
    double dEqPlStr2 = pEqPlStr2->ValueToUserUnit(nUnitDef);

    if(dValue < 0 || dValue >= dEqPlStr2)
    {
      strErrorMsg = QObject::tr("Equivalent Plastic Strain 1 must be at least equal to 0 and smaller than Equivalent Plastic Strain 2.");
      return false;
    }
  }
  else if(dValue < 0 || dValue > 1)
  {
    strErrorMsg = QObject::tr("Equivalent Plastic Strain 1 must be at least equal to 0 and at most equal to 1.");
    return false;
  }

  return true;
}

bool CMaterialMCHardEquivalentPlasticStrain2CheckStrategy::operator()(double dValue, const ml::CMatParam& param, QString& strErrorMsg, int nUnitDef) const
{
  const ml::CMaterial& mat = param.ParentMaterial();

  const ml::CMatParam* pEqPlStr1 = mat.MatParameter(MLD_EQUIV_PLAST_STRAIN1);
  assert(pEqPlStr1);
  double dEqPlStr1 = pEqPlStr1->ValueToUserUnit(nUnitDef);

  const ml::CMatParam* pEqPlStr3 = mat.MatParameter(MLD_EQUIV_PLAST_STRAIN3);

  if(pEqPlStr3)
  {
    double dEqPlStr3 = pEqPlStr3->ValueToUserUnit(nUnitDef);

    if(dValue <= dEqPlStr1 || dValue >= dEqPlStr3)
    {
      strErrorMsg = QObject::tr("Equivalent Plastic Strain 2 must be larger than Equivalent Plastic Strain 1 and smaller than Equivalent Plastic Strain 3.");
      return false;
    }
  }
  else if(dValue <= dEqPlStr1 || dValue > 1)
  {
    strErrorMsg = QObject::tr("Equivalent Plastic Strain 2 must be larger than Equivalent Plastic Strain 1 and at most equal to 1.");
    return false;
  }

  return true;
}

bool CMaterialMCHardEquivalentPlasticStrain3CheckStrategy::operator()(double dValue, const ml::CMatParam& param, QString& strErrorMsg, int nUnitDef) const
{
  const ml::CMaterial& mat = param.ParentMaterial();

  const ml::CMatParam* pEqPlStr2 = mat.MatParameter(MLD_EQUIV_PLAST_STRAIN2);
  assert(pEqPlStr2);
  double dEqPlStr2 = pEqPlStr2->ValueToUserUnit(nUnitDef);

  if(dValue <= dEqPlStr2 || dValue > 1)
  {
    strErrorMsg = QObject::tr("Equivalent Plastic Strain 3 must be larger than Equivalent Plastic Strain 2 and at most equal to 1.");
    return false;
  }

  return true;
}
