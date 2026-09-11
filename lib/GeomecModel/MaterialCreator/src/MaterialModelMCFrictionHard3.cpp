#include "MaterialModelMCFrictionHard3.h"
#include "MaterialHelperFactory.h"
#include "MaterialModelMCHardeningCheckStrategies.h"
#include "MaterialUnitTypes.h"
#include "Materials.h"
#include "ValueTypes.h"
#include "mlMatParamDomain.h"

#include <cassert>

void CMaterialModelMCFrictionHard3CreatorV2::OnCreateLibraryParameters(CLibraryMaterial &m) {
  AddNumber(m, 2);

  CMaterialModelMCFrictionHard2CreatorV2::OnCreateLibraryParameters(m);

  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();
  assert(f);

  AddLibParam(MLD_HARD_FRICTION3, DEF_FRICTION, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_HARD_FRICTION3, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0), 0, QString(),
              false);
  AddLibParam(MLD_EQUIV_PLAST_STRAIN3, DEF_EQUIV_PLAST_STRAIN3, m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),
              IDT_VALUETYPE_EQUIV_PLAST_STRAIN3,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_MCHardEquivalentPlasticStrain3), 0, QString(), false);
}

int CMaterialModelMCFrictionHard3CreatorV2::MaterialModel() { return MM_MC_FRICTION_HARD3; }

QString CMaterialModelMCFrictionHard3CreatorV2::MaterialModelName() { return MLD_MC_FRICTION_HARD3; }

void CMaterialModelMCFrictionHard3CreatorDEPRECATED::OnCreateLibraryParameters(CLibraryMaterial &m) {
  CMaterialModelMCFrictionHard2CreatorDEPRECATED::OnCreateLibraryParameters(m);

  Add(new CLibraryMaterialParameter(MLD_HARD_FRICTION3, DEF_FRICTION, m, *new CUnitTypeStress,
                                    IDT_VALUETYPE_HARD_FRICTION3, new MP_GE(0)));
  Add(new CLibraryMaterialParameter(MLD_EQUIV_PLAST_STRAIN3, DEF_EQUIV_PLAST_STRAIN3, m, *new CUnitTypeNone,
                                    IDT_VALUETYPE_EQUIV_PLAST_STRAIN3,
                                    new CMaterialMCHardEquivalentPlasticStrain3CheckStrategy));
}

int CMaterialModelMCFrictionHard3CreatorDEPRECATED::MaterialModel() { return MM_MC_FRICTION_HARD3; }

QString CMaterialModelMCFrictionHard3CreatorDEPRECATED::MaterialModelName() { return MLD_MC_FRICTION_HARD3; }
