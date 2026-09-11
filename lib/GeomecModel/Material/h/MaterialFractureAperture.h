#ifndef _MATERIALFRACTUREAPERTURE_H_
#define _MATERIALFRACTUREAPERTURE_H_

#include "MaterialFractureApertureBase.h"
#include "MaterialModelFractureAperture.h"
#include "Materials.h"

class CMaterialFractureAperture : public CMaterialFractureApertureBase
{
public:
  CMaterialFractureAperture(CMaterialEntry& entry, CLibraryMaterial& libmat);

  virtual int MaterialModel() const { return MM_FRACTURE_APERTURE; }
  virtual long MaterialModelFilter() const { return MLFO_FRACTURE_APERTURE; }

  // the creator for this material model
  static ml::CMaterial::CCreator* GetLibraryMaterialCreator() { return new CMaterialFractureApertureCreator; }

  ACCEPT_GEOMECMODELVISITORS(VisitMaterialFractureAperture);
};

#endif // _MATERIALFRACTUREAPERTURE_H_
