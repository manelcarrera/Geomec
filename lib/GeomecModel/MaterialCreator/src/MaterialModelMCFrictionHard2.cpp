#include "MaterialModelMCFrictionHard2.h"
#include "MaterialHelperFactory.h"
#include "MaterialModelMCHardeningCheckStrategies.h"
#include "MaterialUnitTypes.h"
#include "Materials.h"
#include "ValueTypes.h"
#include "mlMatParamDomain.h"

#include <cassert>

void CMaterialModelMCFrictionHard2CreatorV2::OnCreateLibraryParameters(CLibraryMaterial &m) {
  AddNumber(m, 2);

  CMaterialModelMCFrictionHard1CreatorV2::OnCreateLibraryParameters(m);

  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();
  assert(f);

  AddLibParam(MLD_HARD_FRICTION2, DEF_FRICTION, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),
              IDT_VALUETYPE_HARD_FRICTION2, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0), 0, QString(),
              false);
  AddLibParam(MLD_EQUIV_PLAST_STRAIN2, DEF_EQUIV_PLAST_STRAIN2, m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),
              IDT_VALUETYPE_EQUIV_PLAST_STRAIN2,
              f->getMPCheckStrategy(CMaterialHelperFactory::MP_MCHardEquivalentPlasticStrain2), 0, QString(), false);
}

int CMaterialModelMCFrictionHard2CreatorV2::MaterialModel() { return MM_MC_FRICTION_HARD2; }

QString CMaterialModelMCFrictionHard2CreatorV2::MaterialModelName() { return MLD_MC_FRICTION_HARD2; }

void CMaterialModelMCFrictionHard2CreatorDEPRECATED::OnCreateLibraryParameters(CLibraryMaterial &m) {
  CMaterialModelMCFrictionHard1CreatorDEPRECATED::OnCreateLibraryParameters(m);

  Add(new CLibraryMaterialParameter(MLD_HARD_FRICTION2, DEF_FRICTION, m, *new CUnitTypeStress,
                                    IDT_VALUETYPE_HARD_FRICTION2, new MP_GE(0)));
  Add(new CLibraryMaterialParameter(MLD_EQUIV_PLAST_STRAIN2, DEF_EQUIV_PLAST_STRAIN2, m, *new CUnitTypeNone,
                                    IDT_VALUETYPE_EQUIV_PLAST_STRAIN2,
                                    new CMaterialMCHardEquivalentPlasticStrain2CheckStrategy));
}

int CMaterialModelMCFrictionHard2CreatorDEPRECATED::MaterialModel() { return MM_MC_FRICTION_HARD2; }

QString CMaterialModelMCFrictionHard2CreatorDEPRECATED::MaterialModelName() { return MLD_MC_FRICTION_HARD2; }
