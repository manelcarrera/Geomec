#ifndef _MATERIALFRACTUREAPERTURE2_H_
#define _MATERIALFRACTUREAPERTURE2_H_

#include "MaterialFractureApertureBase.h"
#include "MaterialModelFractureAperture2.h"
#include "Materials.h"

class CMaterialFractureAperture2 : public CMaterialFractureApertureBase {
public:
  CMaterialFractureAperture2(CMaterialEntry &entry, CLibraryMaterial &libmat);

  virtual int MaterialModel() const { return MM_FRACTURE_APERTURE2; }
  virtual long MaterialModelFilter() const { return MLFO_FRACTURE_APERTURE2; }

  // the creator for this material model
  static ml::CMaterial::CCreator *GetLibraryMaterialCreator() { return new CMaterialFractureAperture2Creator; }

  ACCEPT_GEOMECMODELVISITORS(VisitMaterialFractureAperture2);
};

#endif // _MATERIALFRACTUREAPERTURE2_H_
