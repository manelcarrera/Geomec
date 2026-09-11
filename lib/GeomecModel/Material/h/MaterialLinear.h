#ifndef _MATERIALLINEAR_H_
#define _MATERIALLINEAR_H_

#include "IMaterialRock.h"
#include "MaterialModelLinear.h"
#include "Materials.h"

class CMaterialLinear : public IMaterialRock
{
public:
  CMaterialLinear(CMaterialEntry &entry, CLibraryMaterial& libmat);
  CMaterialLinear(const CMaterialLinear& material, CMaterialEntry& entry);

  virtual bool Write(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const;
  virtual int MaterialModel() const { return MM_LINEAR; }
  virtual long MaterialModelFilter() const { return MLFO_LINEAR; }

  // the creator for this material model
  static ml::CMaterial::CCreator* GetLibraryMaterialCreator() { return new CMaterialLinearCreator; }

  // Interface for dia::IElementProperty
  virtual int WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const;
  virtual bool WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, int i, char *name) const;
  virtual void WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, double *values, int stride = 1) const;

  ACCEPT_GEOMECMODELVISITORS(VisitMaterialLinear);
};

#endif /* _MATERIALLINEAR_H_ */