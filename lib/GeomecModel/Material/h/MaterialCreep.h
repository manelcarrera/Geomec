#ifndef _MATERIALCREEP_H_
#define _MATERIALCREEP_H_

#include "IMaterialRock.h"
#include "MaterialModelCreep.h"
#include "Materials.h"

class CMaterialCreep : public IMaterialRock
{
public:
  CMaterialCreep(CMaterialEntry &entry, CLibraryMaterial& libmat);
  CMaterialCreep(const CMaterialCreep& material, CMaterialEntry& entry);

  virtual bool Write(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const;
  virtual int MaterialModel() const { return MM_CREEP; }
  virtual long MaterialModelFilter() const { return MLFO_CREEP; }

  // the creator for this material model
  static ml::CMaterial::CCreator* GetLibraryMaterialCreator() { return new CMaterialCreepCreator; }

  // Interface for dia::IElementProperty
  virtual int WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const;
  virtual bool WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, int i, char *name) const;
  virtual void WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, double *values, int stride = 1) const;

  ACCEPT_GEOMECMODELVISITORS(VisitMaterialCreep);
};

#endif /* _MATERIALCREEP_H_ */