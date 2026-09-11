#ifndef _MATERIALMOHRCO_H_
#define _MATERIALMOHRCO_H_

#include "IMaterialRock.h"
#include "MaterialModelMohrCoulomb.h"
#include "Materials.h"

class CMaterialMohrCo : public IMaterialRock
{
public:
	CMaterialMohrCo(CMaterialEntry &entry, CLibraryMaterial& libmat);

	virtual bool Write(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const;
	virtual int MaterialModel() const { return MM_MOHRCOULOMB; }
  virtual long MaterialModelFilter() const { return MLFO_MOHRCOULOMB; }

  // the creator for this material model
  static ml::CMaterial::CCreator* GetLibraryMaterialCreator() { return new CMaterialMohrCoulombCreator; }

  // Interface for dia::IElementProperty
  virtual int WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const;
  virtual bool WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, int i, char *name) const;
  virtual void WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, double *values, int stride = 1) const;

  ACCEPT_GEOMECMODELVISITORS(VisitMaterialMohrCo);
};

#endif /* _MATERIALMOHRCO_H_ */