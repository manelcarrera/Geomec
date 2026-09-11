#ifndef _MATERIALCAMCLAYCREEP_H_
#define _MATERIALCAMCLAYCREEP_H_

#include "IMaterialRock.h"
#include "MaterialModelCamClayCreep.h"
#include "Materials.h"

class CMaterialCamClayCreep : public IMaterialRock {
public:
  CMaterialCamClayCreep(CMaterialEntry &entry, CLibraryMaterial &libmat);
  CMaterialCamClayCreep(const CMaterialCamClayCreep &material, CMaterialEntry &entry);

  virtual bool Write(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner) const;
  virtual int MaterialModel() const { return MM_CAMCLAY_CREEP; }
  virtual long MaterialModelFilter() const { return MLFO_CAMCLAY_CREEP; }

  // the creator for this material model
  static ml::CMaterial::CCreator *GetLibraryMaterialCreator() { return new CMaterialCamClayCreepCreator; }

  // Interface for dia::IElementProperty
  virtual int WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner) const;
  virtual bool WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner, int i, char *name) const;
  virtual void WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner, double *values,
                                     int stride = 1) const;

  ACCEPT_GEOMECMODELVISITORS(VisitMaterialCamClayCreep);

protected:
  virtual bool WriteDefaultPorosity() const;
};

#endif /* _MATERIALCAMCLAYCREEP_H_ */
