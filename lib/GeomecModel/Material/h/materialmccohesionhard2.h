#ifndef _MATERIALMCCOHESIONHARD2_H_
#define _MATERIALMCCOHESIONHARD2_H_

#include "MaterialMohrCo.h"
#include "MaterialModelMCCohesionHard2.h"

class CMaterialMCCohesionHard2 : public CMaterialMohrCo
{
public:
  CMaterialMCCohesionHard2(CMaterialEntry &entry, CLibraryMaterial& libmat);
  CMaterialMCCohesionHard2(const CMaterialMCCohesionHard2& material, CMaterialEntry &entry);

  virtual bool Write(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const;
  virtual int MaterialModel() const { return MM_MC_COHESION_HARD2; }
  virtual long MaterialModelFilter() const { return MLFO_MC_COHESION_HARD2; }

  // the creator for this material model
  static ml::CMaterial::CCreator* GetLibraryMaterialCreator() { return new CMaterialModelMCCohesionHard2Creator; }

  // Interface for dia::IElementProperty
  virtual int WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const;
  virtual bool WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, int i, char *name) const;
  virtual void WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, double *values, int stride = 1) const;

  ACCEPT_GEOMECMODELVISITORS(VisitMaterialMCCohesionHard2);
};

#endif /* _MATERIALMCCOHESIONHARD2_H_ */