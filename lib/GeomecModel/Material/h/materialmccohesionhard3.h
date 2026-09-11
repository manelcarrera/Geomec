#ifndef _MATERIALMCCOHESIONHARD3_H_
#define _MATERIALMCCOHESIONHARD3_H_

#include "MaterialMohrCo.h"
#include "MaterialModelMCCohesionHard3.h"

class CMaterialMCCohesionHard3 : public CMaterialMohrCo
{
public:
	CMaterialMCCohesionHard3(CMaterialEntry &entry, CLibraryMaterial& libmat);
	CMaterialMCCohesionHard3(const CMaterialMCCohesionHard3& material, CMaterialEntry &entry);

	virtual bool Write(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const;
	virtual int MaterialModel() const { return MM_MC_COHESION_HARD3; }
  virtual long MaterialModelFilter() const { return MLFO_MC_COHESION_HARD3; }

  // the creator for this material model
  static ml::CMaterial::CCreator* GetLibraryMaterialCreator() { return new CMaterialModelMCCohesionHard3Creator; }

  // Interface for dia::IElementProperty
  virtual int WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const;
  virtual bool WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, int i, char *name) const;
  virtual void WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, double *values, int stride = 1) const;

  ACCEPT_GEOMECMODELVISITORS(VisitMaterialMCCohesionHard3);
};

#endif /* _MATERIALMCCOHESIONHARD3_H_ */