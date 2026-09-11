#ifndef _MATERIALCAMCLAY_H_
#define _MATERIALCAMCLAY_H_

#include "IMaterialRock.h"
#include "MaterialModelCamClay.h"
#include "Materials.h"

class CMaterialCamClay : public IMaterialRock
{
public:
  CMaterialCamClay(CMaterialEntry &entry, CLibraryMaterial& libmat);
  CMaterialCamClay(const CMaterialCamClay& material, CMaterialEntry &entry);

  virtual bool Write(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const;
  virtual int MaterialModel() const { return MM_CAMCLAY; }
  virtual long MaterialModelFilter() const { return MLFO_CAMCLAY; }

  // the creator for this material model
  static ml::CMaterial::CCreator* GetLibraryMaterialCreator() { return new CMaterialCamClayCreator; }

  // Interface for dia::IElementProperty
  virtual int WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const;
  virtual bool WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, int i, char *name) const;
  virtual void WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, double *values, int stride = 1) const;

  ACCEPT_GEOMECMODELVISITORS(VisitMaterialCamClay);

protected:
  virtual bool WriteDefaultPorosity() const;
};

#endif /* _MATERIALCAMCLAY_H_ */