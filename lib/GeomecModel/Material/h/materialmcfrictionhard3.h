#ifndef _MATERIALMCFRICTIONHARD3_H_
#define _MATERIALMCFRICTIONHARD3_H_

#include "MaterialMohrCo.h"
#include "MaterialModelMCFrictionHard3.h"

class CMaterialMCFrictionHard3 : public CMaterialMohrCo
{
public:
  CMaterialMCFrictionHard3(CMaterialEntry &entry, CLibraryMaterial& libmat);
  CMaterialMCFrictionHard3(const CMaterialMCFrictionHard3& material, CMaterialEntry &entry);

  virtual bool Write(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const;
  virtual int MaterialModel() const { return MM_MC_FRICTION_HARD3; }
  virtual long MaterialModelFilter() const { return MLFO_MC_FRICTION_HARD3; }

  // the creator for this material model
  static ml::CMaterial::CCreator* GetLibraryMaterialCreator() { return new CMaterialModelMCFrictionHard3Creator; }

  // Interface for dia::IElementProperty
  virtual int WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const;
  virtual bool WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, int i, char *name) const;
  virtual void WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, double *values, int stride = 1) const;

  ACCEPT_GEOMECMODELVISITORS(VisitMaterialMCFrictionHard3);
};

#endif /* _MATERIALMCFRICTIONHARD3_H_ */