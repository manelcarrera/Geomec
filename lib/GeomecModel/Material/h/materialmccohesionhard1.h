#ifndef _MATERIALMCCOHESIONHARD1_H_
#define _MATERIALMCCOHESIONHARD1_H_

#include "MaterialModelMCCohesionHard1.h"
#include "MaterialMohrCo.h"

class CMaterialMCCohesionHard1 : public CMaterialMohrCo {
public:
  CMaterialMCCohesionHard1(CMaterialEntry &entry, CLibraryMaterial &libmat);
  CMaterialMCCohesionHard1(const CMaterialMCCohesionHard1 &material, CMaterialEntry &entry);

  virtual bool Write(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner) const;
  virtual int MaterialModel() const { return MM_MC_COHESION_HARD1; }
  virtual long MaterialModelFilter() const { return MLFO_MC_COHESION_HARD1; }

  // the creator for this material model
  static ml::CMaterial::CCreator *GetLibraryMaterialCreator() { return new CMaterialModelMCCohesionHard1Creator; }

  // Interface for dia::IElementProperty
  virtual int WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner) const;
  virtual bool WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner, int i, char *name) const;
  virtual void WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner, double *values,
                                     int stride = 1) const;

  ACCEPT_GEOMECMODELVISITORS(VisitMaterialMCCohesionHard1);
};

#endif /* _MATERIALMCCOHESIONHARD1_H_ */