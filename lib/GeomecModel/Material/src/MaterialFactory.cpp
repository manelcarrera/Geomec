
#include <cassert>

#include "LibraryMaterial.h"
#include "MaterialFactory.h"

#include "Materials.h"

#include "MaterialAnisotropicCamClay.h"
#include "MaterialCamClay.h"
#include "MaterialCamClayCreep.h"
#include "MaterialCreep.h"
#include "MaterialDualCapLinEla.h"
#include "MaterialFractAnisotropy.h"
#include "MaterialFractureAperture.h"
#include "MaterialFractureAperture2.h"
#include "MaterialLinear.h"
#include "MaterialModMohrCo.h"
#include "MaterialMohrCo.h"
#include "MaterialRigidity.h"
#include "MaterialUndrained.h"
#include "MaterialUpscaledAnisotropy.h"
#include "materialmccohesionhard1.h"
#include "materialmccohesionhard2.h"
#include "materialmccohesionhard3.h"
#include "materialmcfrictionhard1.h"
#include "materialmcfrictionhard2.h"
#include "materialmcfrictionhard3.h"

///// CMaterialFactory
CMaterialFactory *CMaterialFactory::getInstance() {
  static CMaterialFactory f;
  return &f;
}

std::string CMaterialFactory::Register(int iMaterialModel, TFactory createMethod, TGetCreator getCreatorMethod) {
  bool b = m_mpFactory.insert(TFactoryMap::value_type(iMaterialModel, std::make_pair(createMethod, getCreatorMethod)))
               .second;
  assert(b); // material model already registered
  return "registered";
}

IMaterial *CMaterialFactory::Create(CMaterialEntry &entry, CLibraryMaterial &libmat) {
  TFactoryMap::iterator it = m_mpFactory.find(libmat.MaterialModel());
  if (it == m_mpFactory.end()) {
    assert(false); // not registered
    return 0;
  }

  // call creation method
  return it->second.first(entry, libmat);
}

ml::CMaterial::CCreator *CMaterialFactory::GetLibraryMaterialCreator(int iMaterialModel) {
  TFactoryMap::const_iterator it = findMaterialModel(iMaterialModel);

  if (it == m_mpFactory.end()) {
    assert(false); // not registered
    return 0;
  }

  return it->second.second();
}

bool CMaterialFactory::isMaterialRegistered(int materialModel) const {
  return (findMaterialModel(materialModel) != m_mpFactory.end());
}

CMaterialFactory::CMaterialFactory() {
  RegisterMaterial<MM_LINEAR, CMaterialLinear>();
  RegisterMaterial<MM_ANISOTROPIC_CAMCLAY, CMaterialAnisotropicCamClay>();
  RegisterMaterial<MM_CAMCLAY, CMaterialCamClay>();
  RegisterMaterial<MM_CREEP, CMaterialCreep>();
  RegisterMaterial<MM_DUALCAP_LINELA, CMaterialDualCapLinEla>();
  RegisterMaterial<MM_FRACTURE_ANISOTROPY, CMaterialFractAnisotropy>();
  RegisterMaterial<MM_FRACTURE_APERTURE, CMaterialFractureAperture>();
  RegisterMaterial<MM_MC_COHESION_HARD1, CMaterialMCCohesionHard1>();
  RegisterMaterial<MM_MC_COHESION_HARD2, CMaterialMCCohesionHard2>();
  RegisterMaterial<MM_MC_COHESION_HARD3, CMaterialMCCohesionHard3>();
  RegisterMaterial<MM_MC_FRICTION_HARD1, CMaterialMCFrictionHard1>();
  RegisterMaterial<MM_MC_FRICTION_HARD2, CMaterialMCFrictionHard2>();
  RegisterMaterial<MM_MC_FRICTION_HARD3, CMaterialMCFrictionHard3>();
  RegisterMaterial<MM_MODIFIEDMOHRCOULOMB, CMaterialModMohrCo>();
  RegisterMaterial<MM_MOHRCOULOMB, CMaterialMohrCo>();
  RegisterMaterial<MM_RIGIDITY, CMaterialRigidity>();
  RegisterMaterial<MM_UNDRAINED, CMaterialUndrained>();
  RegisterMaterial<MM_UPSCALED_ANISOTROPY, CMaterialUpscaledAnisotropy>();
  RegisterMaterial<MM_FRACTURE_APERTURE2, CMaterialFractureAperture2>();
  RegisterMaterial<MM_CAMCLAY_CREEP, CMaterialCamClayCreep>();

  // obsolete casing steel material, for backward compatibility with old files
  RegisterMaterial<12, CMaterialLinear>();
}

template <int MATERIAL_MODEL, class MATERIAL_TYPE> void CMaterialFactory::RegisterMaterial() {
  Register(MATERIAL_MODEL, &IMaterial::Create<MATERIAL_TYPE>, &MATERIAL_TYPE::GetLibraryMaterialCreator);
}

CMaterialFactory::TFactoryMap::const_iterator CMaterialFactory::findMaterialModel(int materialModel) const {
  materialModel = (materialModel == MM_DUALCAP ? MM_LINEAR : materialModel);

  return m_mpFactory.find(materialModel);
}
