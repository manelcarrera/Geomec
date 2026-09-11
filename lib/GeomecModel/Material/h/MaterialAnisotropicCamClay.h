#ifndef _MATERIALANISOTROPICCAMCLAY_H_
#define _MATERIALANISOTROPICCAMCLAY_H_

#include "IMaterialRock.h"
#include "MaterialModelAnisotropicCamClay.h"
#include "Materials.h"
#include "Matrix.h"

class CMaterialAnisotropicCamClay : public IMaterialRock
{
public:
	CMaterialAnisotropicCamClay(CMaterialEntry &entry, CLibraryMaterial& libmat);
	CMaterialAnisotropicCamClay(const CMaterialAnisotropicCamClay& material, CMaterialEntry &entry);

	virtual bool Write(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const;
	virtual int MaterialModel() const { return MM_ANISOTROPIC_CAMCLAY; }
  virtual long MaterialModelFilter() const { return MLFO_ANISOTROPIC_CAMCLAY; }

  // the creator for this material model
  static ml::CMaterial::CCreator* GetLibraryMaterialCreator() { return new CMaterialAnisotropicCamClayCreator; }

  // Interface for dia::IElementProperty
  virtual int WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const;
  virtual bool WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, int i, char *name) const;
  virtual void WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, double *values, int stride = 1) const;

  ACCEPT_GEOMECMODELVISITORS(VisitMaterialAnisotropicCamClay);

protected:
  virtual bool WriteDefaultPorosity() const;
};

#endif // _MATERIALANISOTROPICCAMCLAY_H_
