#ifndef _MATERIALMCFRICTIONHARD1_H_
#define _MATERIALMCFRICTIONHARD1_H_

#include "MaterialMohrCo.h"
#include "MaterialModelMCFrictionHard1.h"

class CMaterialMCFrictionHard1 : public CMaterialMohrCo
{
public:
	CMaterialMCFrictionHard1(CMaterialEntry &entry, CLibraryMaterial& libmat);
	CMaterialMCFrictionHard1(const CMaterialMCFrictionHard1& material, CMaterialEntry &entry);

	virtual bool Write(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const;
	virtual int MaterialModel() const { return MM_MC_FRICTION_HARD1; }
  virtual long MaterialModelFilter() const { return MLFO_MC_FRICTION_HARD1; }

  // the creator for this material model
  static ml::CMaterial::CCreator* GetLibraryMaterialCreator() { return new CMaterialModelMCFrictionHard1Creator; }

  // Interface for dia::IElementProperty
  virtual int WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const;
  virtual bool WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, int i, char *name) const;
  virtual void WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, double *values, int stride = 1) const;

  ACCEPT_GEOMECMODELVISITORS(VisitMaterialMCFrictionHard1);
};

#endif /* _MATERIALMCFRICTIONHARD1_H_ */