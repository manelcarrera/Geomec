#ifndef _MATERIALMCFRICTIONHARD2_H_
#define _MATERIALMCFRICTIONHARD2_H_

#include "MaterialModelMCFrictionHard2.h"
#include "MaterialMohrCo.h"

class CMaterialMCFrictionHard2 : public CMaterialMohrCo {
public:
  CMaterialMCFrictionHard2(CMaterialEntry &entry, CLibraryMaterial &libmat);
  CMaterialMCFrictionHard2(const CMaterialMCFrictionHard2 &material, CMaterialEntry &entry);

  virtual bool Write(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner) const;
  virtual int MaterialModel() const { return MM_MC_FRICTION_HARD2; }
  virtual long MaterialModelFilter() const { return MLFO_MC_FRICTION_HARD2; }

  // the creator for this material model
  static ml::CMaterial::CCreator *GetLibraryMaterialCreator() { return new CMaterialModelMCFrictionHard2Creator; }

  // Interface for dia::IElementProperty
  virtual int WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner) const;
  virtual bool WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner, int i, char *name) const;
  virtual void WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner, double *values,
                                     int stride = 1) const;

  ACCEPT_GEOMECMODELVISITORS(VisitMaterialMCFrictionHard2);
};

#endif /* _MATERIALMCFRICTIONHARD2_H_ */