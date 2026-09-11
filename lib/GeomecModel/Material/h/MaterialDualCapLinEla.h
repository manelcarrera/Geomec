#ifndef _MATERIALDUALCAPLINELA_H_
#define _MATERIALDUALCAPLINELA_H_

#include "IMaterialRock.h"
#include "MaterialModelDualCapLinEla.h"
#include "Materials.h"

class CMaterialDualCapLinEla : public IMaterialRock
{
public:
	CMaterialDualCapLinEla(CMaterialEntry &entry, CLibraryMaterial& libmat);

	virtual bool Write(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const;
	virtual int MaterialModel() const { return MM_DUALCAP_LINELA; }
  virtual long MaterialModelFilter() const { return MLFO_DUALCAP_LINELA; }

  // the creator for this material model
  static ml::CMaterial::CCreator* GetLibraryMaterialCreator() { return new CMaterialDualCapLinElaCreator; }

  // Interface for dia::IElementProperty
  virtual int WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const;
  virtual bool WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, int i, char *name) const;
  virtual void WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, double *values, int stride = 1) const;

  ACCEPT_GEOMECMODELVISITORS(VisitMaterialDualCapLinEla);

protected:
  virtual bool WriteDefaultPorosity() const;
};

#endif // _MATERIALDUALCAP_H_
