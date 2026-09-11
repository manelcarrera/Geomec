#ifndef _MATERIALUNDRAINED_H_
#define _MATERIALUNDRAINED_H_

#include "IMaterialRock.h"
#include "MaterialModelUndrained.h"
#include "Materials.h"

class CMaterialUndrained : public IMaterialRock {
public:
  CMaterialUndrained(CMaterialEntry &entry, CLibraryMaterial &libmat);
  CMaterialUndrained(const CMaterialUndrained &material, CMaterialEntry &entry);

  virtual bool Write(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner) const;
  virtual int MaterialModel() const { return MM_UNDRAINED; }
  virtual long MaterialModelFilter() const { return MLFO_UNDRAINED; }

  // the creator for this material model
  static ml::CMaterial::CCreator *GetLibraryMaterialCreator() { return new CMaterialUndrainedCreator; }

  // Interface for dia::IElementProperty
  virtual int WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner) const;
  virtual bool WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner, int i, char *name) const;
  virtual void WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner, double *values,
                                     int stride = 1) const;

  ACCEPT_GEOMECMODELVISITORS(VisitMaterialUndrained);

protected:
  virtual bool WritePermeability() const { return false; }
  virtual bool WriteDefaultPorosity() const { return false; }
  virtual bool WriteDefaultFluidBulkModulus() const { return false; }
};

#endif /* _MATERIALUNDRAINED_H_ */