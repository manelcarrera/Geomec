
#include <cassert>

#include "Materials.h"

bool FilterContainsMaterialModel(mlMatModel matmodel, int filter)
{
  mlFilterOption mmfilter = MaterialFilter(matmodel);
  return (filter & mmfilter);
}

mlFilterOption MaterialFilter(mlMatModel matmodel)
{
  switch(matmodel)
  {
  case MM_LINEAR:
  return MLFO_LINEAR;
  case MM_CAMCLAY:
  return MLFO_CAMCLAY;
  case MM_MOHRCOULOMB:
  return MLFO_MOHRCOULOMB;
  case MM_MODIFIEDMOHRCOULOMB:
  return MLFO_MODIFIEDMOHRCOULOMB;
  case MM_CREEP:
  return MLFO_CREEP;
  case MM_UNDRAINED:
  return MLFO_UNDRAINED;
  case MM_MC_COHESION_HARD1:
  return MLFO_MC_COHESION_HARD1;
  case MM_MC_COHESION_HARD2:
  return MLFO_MC_COHESION_HARD2;
  case MM_MC_COHESION_HARD3:
  return MLFO_MC_COHESION_HARD3;
  case MM_MC_FRICTION_HARD1:
  return MLFO_MC_FRICTION_HARD1;
  case MM_MC_FRICTION_HARD2:
  return MLFO_MC_FRICTION_HARD2;
  case MM_MC_FRICTION_HARD3:
  return MLFO_MC_FRICTION_HARD3;
  case MM_RIGIDITY:
  return MLFO_RIGIDITY;
  case MM_DUALCAP_LINELA:
  return MLFO_DUALCAP_LINELA;
  case MM_FRACTURE_ANISOTROPY:
  return MLFO_FRACTURE_ANISOTROPY;
  case MM_UPSCALED_ANISOTROPY:
  return MLFO_UPSCALED_ANISOTROPY;
  case MM_FRACTURE_APERTURE:
  return MLFO_FRACTURE_APERTURE;
  case MM_ANISOTROPIC_CAMCLAY:
  return MLFO_ANISOTROPIC_CAMCLAY;
  case MM_FRACTURE_APERTURE2:
  return MLFO_FRACTURE_APERTURE2;
  case MM_CAMCLAY_CREEP:
  return MLFO_CAMCLAY_CREEP;
  case MM_DUALCAP:
  break;
  }

  assert(false);
  return (mlFilterOption)-1;
}

mlMatModel MaterialModel(mlFilterOption filter)
{
  switch(filter)
  {
  case MLFO_LINEAR:
  return MM_LINEAR;
  case MLFO_CAMCLAY:
  return MM_CAMCLAY;
  case MLFO_MOHRCOULOMB:
  return MM_MOHRCOULOMB;
  case MLFO_MODIFIEDMOHRCOULOMB:
  return MM_MODIFIEDMOHRCOULOMB;
  case MLFO_CREEP:
  return MM_CREEP;
  case MLFO_UNDRAINED:
  return MM_UNDRAINED;
  case MLFO_MC_COHESION_HARD1:
  return MM_MC_COHESION_HARD1;
  case MLFO_MC_COHESION_HARD2:
  return MM_MC_COHESION_HARD2;
  case MLFO_MC_COHESION_HARD3:
  return MM_MC_COHESION_HARD3;
  case MLFO_MC_FRICTION_HARD1:
  return MM_MC_FRICTION_HARD1;
  case MLFO_MC_FRICTION_HARD2:
  return MM_MC_FRICTION_HARD2;
  case MLFO_MC_FRICTION_HARD3:
  return MM_MC_FRICTION_HARD3;
  case MLFO_RIGIDITY:
  return MM_RIGIDITY;
  case MLFO_DUALCAP_LINELA:
  return MM_DUALCAP_LINELA;
  case MLFO_FRACTURE_ANISOTROPY:
  return MM_FRACTURE_ANISOTROPY;
  case MLFO_UPSCALED_ANISOTROPY:
  return MM_UPSCALED_ANISOTROPY;
  case MLFO_FRACTURE_APERTURE:
  return MM_FRACTURE_APERTURE;
  case MLFO_ANISOTROPIC_CAMCLAY:
  return MM_ANISOTROPIC_CAMCLAY;
  case MLFO_FRACTURE_APERTURE2:
  return MM_FRACTURE_APERTURE2;
  case MLFO_CAMCLAY_CREEP:
  return MM_CAMCLAY_CREEP;
  case MLFO_ALL:
  break;
  }

  assert(false);
  return (mlMatModel)-1;
}

const QString& MaterialModelName(mlMatModel matmodel)
{
  switch(matmodel)
  {
  case MM_LINEAR:
  return MLD_LINEAR;
  case MM_CAMCLAY:
  return MLD_CAMCLAY;
  case MM_MOHRCOULOMB:
  return MLD_MOHRCOULOMB;
  case MM_MODIFIEDMOHRCOULOMB:
  return MLD_MODIFIEDMOHRCOULOMB;
  case MM_CREEP:
  return MLD_CREEP;
  case MM_UNDRAINED:
  return MLD_UNDRAINED;
  case MM_MC_COHESION_HARD1:
  return MLD_MC_COHESION_HARD1;
  case MM_MC_COHESION_HARD2:
  return MLD_MC_COHESION_HARD2;
  case MM_MC_COHESION_HARD3:
  return MLD_MC_COHESION_HARD3;
  case MM_MC_FRICTION_HARD1:
  return MLD_MC_FRICTION_HARD1;
  case MM_MC_FRICTION_HARD2:
  return MLD_MC_FRICTION_HARD2;
  case MM_MC_FRICTION_HARD3:
  return MLD_MC_FRICTION_HARD3;
  case MM_RIGIDITY:
  return MLD_RIGIDITY;
  case MM_DUALCAP_LINELA:
  return MLD_DUALCAP_LINELA;
  case MM_FRACTURE_ANISOTROPY:
  return MLD_FRACTURE_ANISOTROPY;
  case MM_UPSCALED_ANISOTROPY:
  return MLD_UPSCALED_ANISOTROPY;
  case MM_FRACTURE_APERTURE:
  return MLD_FRACTURE_APERTURE;
  case MM_ANISOTROPIC_CAMCLAY:
  return MLD_ANISOTROPIC_CAMCLAY;
  case MM_FRACTURE_APERTURE2:
  return MLD_FRACTURE_APERTURE2;
  case MM_CAMCLAY_CREEP:
  return MLD_CAMCLAY_CREEP;
  case MM_DUALCAP:
  break;
  }

  assert(false);
  QString* pBogus = 0;
  return *pBogus;
}
