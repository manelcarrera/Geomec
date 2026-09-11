#ifndef _MATERIALMODMOHRCO_H_
#define _MATERIALMODMOHRCO_H_

#include "IMaterialRock.h"
#include "MaterialModelModifiedMohrCoulomb.h"
#include "Materials.h"

class CMaterialModMohrCo : public IMaterialRock
{
public:
  CMaterialModMohrCo(CMaterialEntry &entry, CLibraryMaterial& libmat);
  CMaterialModMohrCo(const CMaterialModMohrCo &material, CMaterialEntry& entry);

  virtual bool Write(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const;
  virtual int MaterialModel() const { return MM_MODIFIEDMOHRCOULOMB; }
  virtual long MaterialModelFilter() const { return MLFO_MODIFIEDMOHRCOULOMB; }

  // the creator for this material model
  static ml::CMaterial::CCreator* GetLibraryMaterialCreator() { return new CMaterialModifiedMohrCoulombCreator; }

  // Interface for dia::IElementProperty
  virtual int WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const;
  virtual bool WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, int i, char *name) const;
  virtual void WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, double *values, int stride = 1) const;

  ACCEPT_GEOMECMODELVISITORS(VisitMaterialModMohrCo);

protected:
  virtual bool WriteDefaultPorosity() const;
};

#endif /* _MATERIALMODMOHRCO_H_ */