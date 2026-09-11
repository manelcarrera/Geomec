#ifndef _MATERIALUPSCALEDANISOTROPY_H_
#define _MATERIALUPSCALEDANISOTROPY_H_

#include "IMaterialRock.h"
#include "MaterialModelUpscaledAnisotropy.h"
#include "Materials.h"
#include "Matrix.h"

class CMaterialUpscaledAnisotropy : public IMaterialRock {
public:
  CMaterialUpscaledAnisotropy(CMaterialEntry &entry, CLibraryMaterial &libmat);
  CMaterialUpscaledAnisotropy(const CMaterialUpscaledAnisotropy &material, CMaterialEntry &entry);

  virtual bool Write(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner) const;
  virtual int MaterialModel() const { return MM_UPSCALED_ANISOTROPY; }
  virtual long MaterialModelFilter() const { return MLFO_UPSCALED_ANISOTROPY; }

  // the creator for this material model
  static ml::CMaterial::CCreator *GetLibraryMaterialCreator() { return new CMaterialUpscaledAnisotropyCreator; }

  // Interface for dia::IElementProperty
  virtual int WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner) const;
  virtual bool WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner, int i, char *name) const;
  virtual void WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner, double *values,
                                     int stride = 1) const;

  ACCEPT_GEOMECMODELVISITORS(VisitMaterialUpscaledAnisotropy);
};

#endif // _MATERIALUPSCALEDANISOTROPY_H_
