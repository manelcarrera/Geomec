#include "MaterialModelMCCohesionHard1.h"
#include "Materials.h"
#include "MaterialUnitTypes.h"
#include "ValueTypes.h"
#include "mlMatParamDomain.h"
#include "MaterialModelMCHardeningCheckStrategies.h"
#include "MaterialHelperFactory.h"

#include <cassert>

void CMaterialModelMCCohesionHard1CreatorV2::OnCreateLibraryParameters(CLibraryMaterial& m)
{
  AddNumber(m, 2);

  CMaterialMohrCoulombCreatorV2::OnCreateLibraryParameters(m);

  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();
  assert(f);

  AddLibParam(MLD_HARD_COHESION1,      DEF_COHESION,            m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS), IDT_VALUETYPE_HARD_COHESION1,      f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0), 0, QString(), false);
  AddLibParam(MLD_EQUIV_PLAST_STRAIN1, DEF_EQUIV_PLAST_STRAIN1, m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),   IDT_VALUETYPE_EQUIV_PLAST_STRAIN1, f->getMPCheckStrategy(CMaterialHelperFactory::MP_MCHardEquivalentPlasticStrain1), 0, QString(), false);
}

int CMaterialModelMCCohesionHard1CreatorV2::MaterialModel()
{
  return MM_MC_COHESION_HARD1;
}

QString CMaterialModelMCCohesionHard1CreatorV2::MaterialModelName()
{
  return MLD_MC_COHESION_HARD1;
}

QString CMaterialModelMCCohesionHard1CreatorV2::CalibrationPath() const
{
  return QString();
}

void CMaterialModelMCCohesionHard1CreatorDEPRECATED::OnCreateLibraryParameters(CLibraryMaterial& m)
{
  CMaterialMohrCoulombCreatorDEPRECATED::OnCreateLibraryParameters(m);

  Add(new CLibraryMaterialParameter(MLD_HARD_COHESION1,      DEF_COHESION,            m, *new CUnitTypeStress, IDT_VALUETYPE_HARD_COHESION1,      new MP_GE(0)));
  Add(new CLibraryMaterialParameter(MLD_EQUIV_PLAST_STRAIN1, DEF_EQUIV_PLAST_STRAIN1, m, *new CUnitTypeNone,   IDT_VALUETYPE_EQUIV_PLAST_STRAIN1, new CMaterialMCHardEquivalentPlasticStrain1CheckStrategy));
}

int CMaterialModelMCCohesionHard1CreatorDEPRECATED::MaterialModel()
{
  return MM_MC_COHESION_HARD1;
}

QString CMaterialModelMCCohesionHard1CreatorDEPRECATED::MaterialModelName()
{
  return MLD_MC_COHESION_HARD1;
}

QString CMaterialModelMCCohesionHard1CreatorDEPRECATED::CalibrationPath() const
{
  return QString();
}
